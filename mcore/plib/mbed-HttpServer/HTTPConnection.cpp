/* HTTPConnection.cpp */

#include "mbed.h"
#include "HTTPConnection.h"
//#define DEBUG
#include "hl_debug.h"

#include <vector>
using std::vector;

using std::string;



const struct HTTPRequestConfig {
    const char* request_string;
    HTTPRequestType request_type;
} g_requestConfig[] = {
    { "GET",    HTTP_RT_GET },
    { "POST",   HTTP_RT_POST},
    { "PUT",    HTTP_RT_PUT},
    { "OPTIONS",HTTP_RT_OPTIONS},
    { "HEAD",   HTTP_RT_HEAD},
    { "DELETE", HTTP_RT_DELETE},
    { "TRACE",  HTTP_RT_TRACE},
    { "CONNECT",HTTP_RT_CONNECT}
};


HTTPConnection::HTTPConnection(TCPSocketConnection& clnt) : m_Tcp(clnt)
{
}


HTTPConnection::~HTTPConnection()
{
    close();
}

void HTTPConnection::close()
{
    m_Msg.headers.clear();
}

int HTTPConnection::poll()
{
    static char buffer[256] = {};

    
    int rcvd= 0;
    INFO("Waiting for new data in connection");
    //  Try receiving request line
    rcvd = receiveLine(buffer, 255, 3000); 
    if (rcvd == -1) {
        //  there was an error, probably the connection was closed, so close this connection as well
        INFO("No more data available. Will close this connection now.");
        close();
        return -1;
    }

    //  The Request has not yet been received so try it
    rcvd = parse(buffer);
    if (rcvd == -1) {
        //  Invalid content received, so close the connection
        INFO("Invalid message received, so sending negative response and closing connection !");
        sprintf(buffer,"HTTP/1.1 400 BadRequest\n\rContent-Length: %d\n\rContent-Type: text\n\r\n\r\n\r",0);
        m_Tcp.set_blocking(true, 1500);
        m_Tcp.send(buffer,strlen(buffer));
        close();
        rcvd = -1;
        return -1;
    }
    //  The request has been received, try receive the body
    while(rcvd > 0) {
        rcvd = receiveLine((char*)buffer, 255, 3000); 
        //  First check if we received an empty line. This would indicate the end of the message or message body.
        if (rcvd < 0) {
            //  there was an empty line, so we can start with performing the request
            INFO("Request Header was received completely. Performing request.");
            rcvd = 0;
            break;
        }
        else {
            //  add message body
            if (parseHeader(buffer) == 0) {
            }
            else {
                WARN("Invalid message header received !");
            }
        }
    }             
    INFO("Leaving poll function!");
    return rcvd;
}

int HTTPConnection::receiveLine(char* szLine, int nMaxLen, int nTimeout, char cLineTerm)
{
    if ((szLine == NULL) || (nMaxLen == 0))
        return -1;
    
    szLine[0] = 0;
    m_Tcp.set_blocking(false);
    
    if (!m_Tcp.is_connected()) {
        error("NOT COnnected anymore");
        return -1;
    }
    Timer tm;
    int i;
    
    //  Try to receive up to the max number of characters
    for (i = 0 ; i < nMaxLen-1 ; i++) {
        int c;
        c = m_Tcp.receive( szLine + i, 1 );
        //  Check that - if no character was currently received - the timeout period is reached.
        if ((c == 0) || (c==-1)) {
            //  no character was read, so check if operation timed out
            if (tm.read_ms() > nTimeout) {
                //  Operation timed out
                INFO("Timeout occured in function 'receiveLine'.");
                return -1;
            }
        }
        
        //  Check if line terminating character was received
        if (szLine[i] == cLineTerm)
        {   
            break;
        }
    }
    //  Terminate with \0
    szLine[i] = 0;

    //  Trim for '\r' linefeed at the end
    if( (i >0) && (szLine[i-1] == '\r')) {
        i--;
        szLine[i] = 0;
    }
    
    //  return number of characters received in the line or return -2 if an empty line was received
    if ((i == 0) || ((i==1) &&(szLine[0] == '\r')))
    {
        //  empty line received, so return -2
        return -2;
    }
    return i;    
}

int HTTPConnection::parse(char* buffer)
{
    //  Check if buffer is invalid or its content not long enough.
    if ((buffer == NULL) || (strlen(buffer) < 4)) {
        ERR("Buffer content is invalid or too short.");
        return -1;
    }
    
    std::vector<std::string> args;
    args.clear();
    
    int argno = 0;
    //  decompose string into a list of arguments
    char s = 0; // current starting char
    int nLen = strlen(buffer)+1;
    for (int i = 0 ; i < nLen ; i++) {
        if ((buffer[i] == ' ') || (buffer[i] == '\n') || (buffer[i] == 0)) {
            // new arg found
            buffer[i] = 0;
            if (argno++ == 1) {
                //  its the uri
                // parse the uri args
                parseUriArgs(&buffer[s], m_Msg.args);
            }
            INFO("Found argument \"%s\"", &buffer[s]);
            args.push_back(&buffer[s]);
            s = i+1;
        }
    }
    
    // store the uri and the HTTP version
    m_Msg.uri = args[1];
    m_Msg.version = args[2];    
    
    //  Find matching request type
    for (int i = 0 ; i < sizeof(g_requestConfig)/sizeof(struct HTTPRequestConfig) ; i++) {
        if (args.at(0) == g_requestConfig[i].request_string) {
            m_Msg.request = g_requestConfig[i].request_type;
        }
    }
    args.clear();
    
    return 1;
}


int HTTPConnection::parseHeader(char *buffer)
{
    //  Check if the buffer is invalid or if the content is too short to be meaningful
    if ((strlen(buffer) <3) || (buffer == NULL))
        return -1;
        
    //  decompose string into a touple of <field name> : <field value>
    int value_start = 0;
    int buflen = strlen(buffer)+1;
    for (int i = 0 ; i < buflen ; i++) {
        if (buffer[i] == ':') {
            //  touple found
            buffer[i] = 0;
            value_start = i+1;    
            m_Msg.headers[buffer] = &buffer[value_start];
            
            INFO("Header name=\"%s\" : value=\"%s\".", buffer, &buffer[value_start]);            
            return 0;
        }
    }
    
    ERR("Did not recieve a valid header : \"%s\".", buffer);
    return -1;
}

int HTTPConnection::parseUriArgs(char *buffer, map<string,string>&args)
{
    // Check if the buffer is invalid or if the content is too short to be meaningful
    if ((strlen(buffer) <3) || (buffer == NULL))
        return -1;
        
    int args_start = -1;
    int value_start = -1;
    int buflen = strlen(buffer) +1;
    const char* argname = NULL;
    const char* valuename = NULL;
    for (int i = 0; i < buflen ; i++) {
        if (args_start == -1) {  // args section not yet found
            if (buffer[i] == '?') {  // starts with a question mark, so got it
                buffer[i] = 0;
                args_start = i; //  set the start of the args section
                INFO("Argument section found !");
            }
        }
        else {                  // search arg-value touples
            if (argname == NULL) {    //  arg-name found ?
                if (buffer[i] == '=') {
                    //  yes, separate the arg-name
                    buffer[i] = 0;
                    argname = &buffer[args_start];
                    value_start = i+1;
                    INFO("Argument name %s", argname);
                }
            }
            else { // search for end of value
                if ((buffer[i] == '&') || (buffer[i] == 0) || (buffer[i] == '\r') || (buffer[i] == '\n')) {
                    buffer[i] = 0;
                    valuename = &buffer[value_start];
                    INFO("Argument value %s", valuename);
                    args[argname] = valuename;
                    //  reset all indicators
                    argname = NULL;
                    valuename = NULL;
                }
            }
        }
    }
    
    return 0;
}
