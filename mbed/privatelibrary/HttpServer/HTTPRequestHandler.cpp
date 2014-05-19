/* HTTPRequestHandler.cpp */
#include "mbed.h"
#include "HTTPRequestHandler.h"
//#define DEBUG
#include "hl_debug.h"
#include <ctype.h>


static char buffer[128];


const char hdrStandard[] = "DNT: 1\r\n"
                            "MaxAge: 0\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Content-Type: text/html\r\n"
                            "Server: mbed embedded\r\n"
                            "Accessible: 1\r\n"
                            "\r\n";


static int _stricmp(const char* a, const char* b)
{
    int la = strlen(a);
    int lb = strlen(b);
    for (int i = 0 ; i < min(la, lb) ; i++) {
        if (tolower((int)a[i]) != tolower((int)b[i]))
            return i;
    }
    return 0;
}


static const struct mapping_t {
    const char* key;
    const char* value;
} fileTypeMapping[]  = {
        {".gif", "Content-Type: image/gif\r\n"   },
    {".jpg", "Content-Type: image/jpeg\r\n"  },
    {".jpeg","Content-Type: image/jpeg\r\n"  },
    {".ico", "Content-Type: image/x-icon\r\n"},
    {".png", "Content-Type: image/png\r\n"   },
    {".zip", "Content-Type: image/zip\r\n"   },
    {".gz",  "Content-Type: image/gz\r\n"    },
    {".tar", "Content-Type: image/tar\r\n"   },
    {".txt", "Content-Type: plain/text\r\n"  },
    {".pdf", "Content-Type: application/pdf\r\n" },
    {".htm", "Content-Type: text/html\r\n"   },
    {".html","Content-Type: text/html\r\n"   },
    {".css", "Content-Type: text/css\r\n"    },
    {".js",  "Content-Type: text/javascript\r\n"}};
    
HTTPRequestHandler::HTTPRequestHandler(HTTPConnection::HTTPMessage& Msg, TCPSocketConnection& Tcp)
    : msg(Msg), tcp(Tcp)
{
    msg = Msg;
    tcp = Tcp;

}

HTTPRequestHandler::~HTTPRequestHandler()
{
}

void HTTPRequestHandler::getStandardHeaders(HTTPHeaders& header, const char* fext)
{
    header.clear();
    header["DNT"] = "1";
    header["MaxAge"] = "0";
    header["Connection"] = "Keep-Alive";
    header["Server"] = "mbed Embedded";
    if (fext == NULL)
        header["Content-Type"] = "text/html";
    else {
        for (int i = 0 ; i < sizeof(fileTypeMapping)/sizeof(struct mapping_t) ;i++) {
            if (_stricmp(fileTypeMapping[i].key, fext) == 0) {
                header["Content-Type"] = fileTypeMapping[i].value;
                break;
            }
        }
    }
}

void HTTPRequestHandler::handleRequest()
{
    int err = 0;
    
    switch (msg.request) {
        case HTTP_RT_GET:
            INFO("Dispatching GET Request.");
            err = handleGetRequest();
            break;
            
        case HTTP_RT_POST:
            INFO("Dispatching POST request.");
            err = handlePostRequest();
            break;
            
        case HTTP_RT_PUT:
            INFO("Dispatching PUT request.");
            err = handlePutRequest();
            break;
            
        default:
            INFO("Error in handleRequest, unhandled request type.");
            err = HTTP_NotImplemented;
            break;
    }
    
    //  if any of these functions returns a negative number, call the error handler
    if (err > 0) {
        handleError(err);
    }        
}

static const char* szErrorPage = "<HTML><HEAD><META content=\"text/html\" http-equiv=Content-Type></HEAD><BODY><h1>Error</h1><P>HTTPServer Error<P></BODY></HTML>\r\n\r\n";

void HTTPRequestHandler::handleError(int errorCode, HTTPHeaders* header)
{
    INFO("Handling error !");
    tcp.set_blocking(false, 1500);
    sprintf(buffer,"HTTP/1.1 %d Error\r\n", errorCode);
    tcp.send(buffer, strlen(buffer));
    sprintf(buffer, "Content-Length: %d\r\n", strlen(szErrorPage));
    tcp.send(buffer, strlen(buffer));
    if (header == NULL) {
        sprintf(buffer, "Content-Type: text/html\r\nServer: mbed embedded\r\n\n\r");
        tcp.send(buffer, strlen(buffer));
    }
    else {
        for ( map<const char*, const char*>::iterator cIter = header->begin() ; cIter != header->end() ; cIter ++) {
            tcp.send((char*)cIter->first, strlen(cIter->first));
            tcp.send(": ", 2);
            tcp.send((char*)cIter->second, strlen(cIter->second));
            tcp.send("\r\n",2);
        }
        tcp.send("\r\n",2);
    }
    tcp.send((char*)szErrorPage, strlen(szErrorPage));
}


void HTTPRequestHandler::startResponse(int returnCode, long nLen, HTTPHeaders* header)
{
    INFO("Starting response (%ld bytes in total)!", nLen);
    tcp.set_blocking(false, 1500);
    sprintf(buffer, "HTTP/1.1 %d OK\r\n", returnCode);
    tcp.send(buffer, strlen(buffer));
    sprintf(buffer, "Content-Length: %ld\r\n", nLen);    //  Add 2 chars for the terminating CR+LF
    tcp.send(buffer, strlen(buffer));
    INFO("Sending standard headers !");
    if (header == NULL) {
        tcp.send_all((char*)hdrStandard, strlen(hdrStandard));
    }
    else {
        for ( map<const char*, const char*>::iterator cIter = header->begin() ; cIter != header->end() ; cIter ++) {
            tcp.send_all((char*)cIter->first, strlen(cIter->first));
            tcp.send_all(": ", 2);
            tcp.send_all((char*)cIter->second, strlen(cIter->second));
            tcp.send_all("\r\n\r\n",2);
        }
        tcp.send_all("\r\n", 2);
    }
    INFO("Proceeding !");
    //  other content must be sent using the 'processResponse' function
}

void HTTPRequestHandler::processResponse(int nLen, char* body)
{
    INFO("Processing Response (%d bytes)!\n",nLen);
    tcp.send_all(body, nLen);
}

void HTTPRequestHandler::endResponse()
{
    INFO("Ending Response !");
}
