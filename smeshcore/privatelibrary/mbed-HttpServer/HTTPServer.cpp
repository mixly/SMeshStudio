#include "mbed.h"
#include "HTTPServer.h"
//#define DEBUG
#include "hl_debug.h"



/* Constructor */
/* initialize all members and set the standard error handler. */
HTTPServer::HTTPServer()
    : m_pEthernet(NULL)
{
    m_pErrorHandler = StdErrorHandler;
}

HTTPServer::~HTTPServer()
{
    if (m_pEthernet == NULL) {
        INFO("Deleting EthernetInterface Object.\n");
        delete m_pEthernet;
    }
}

static const char* szStdErrorPage = "<HTML><HEAD><META content=\"text/html\" http-equiv=Content-Type></HEAD><BODY><h1>Error 404</h1><P>This resource is not available<P></BODY></HTML>\r\n\r\n";

void HTTPServer::StdErrorHandler(HTTPConnection::HTTPMessage& msg, TCPSocketConnection& tcp)
{
    char echoHeader[256];
    tcp.set_blocking(true, 1500);
    sprintf(echoHeader,"HTTP/1.0 404 Fail\r\nConnection: close\r\nContent-Length: %d\r\nContent-Type: text/html\r\nServer: mbed embedded\r\n\n\r",strlen(szStdErrorPage));
    tcp.send(echoHeader, strlen(echoHeader));
    tcp.send((char*)szStdErrorPage, strlen(szStdErrorPage));
}


bool HTTPServer::start(int port, EthernetInterface* pEthernet)
{
    //  If no ethernet interface is provided, instantiate own on the heap. This has to be deleted later in the destructor.
    //  If a valid pointer to an thernet interface is proveded, we can simply use it. 
    if (pEthernet == NULL) {
        INFO("Creating EthernetInterface object\n");
        m_pEthernet = new EthernetInterface();
        
        if (m_pEthernet == NULL) {
            ERR("Out of memory, unable to instantiate an EthernetInterface object.");
            return false;
        }

        //  Initiaize the network
        INFO("Initializing network\n");
        if (m_pEthernet->init() != 0) {
            ERR("Failed to initialize the ethernet interface !");
            delete m_pEthernet;
            m_pEthernet = NULL;
            return false;
        }
        
        //  Connect to the network using DHCP
        INFO("Connecting to the network using DHCP...");
        if (m_pEthernet->connect() != 0) {
            ERR("Failed to connect to the ethernet !");
            delete m_pEthernet;
            m_pEthernet = NULL;
            return false;
        }
        
        INFO("Connected IP %s", m_pEthernet->getIPAddress());
        
    } else {
        //  In the case that the ethernet interface is provided, it is assumed that a connection has already been created.
        INFO("Using connection IP %s", pEthernet->getIPAddress());
    }
    
    INFO("Binding to port %d...", port);
    if (m_Svr.bind(port) < 0) {
        ERR("Failed to bind to port !\n");
        error("Binding");
        return false;
    }

    INFO("Listening ...");
    if (m_Svr.listen(1) < 0) {
        ERR("Failed to listen !\n");
        error("Listening");
        return false;
    }

    INFO("Connected !");
    //  set into non blocking operation
    m_Svr.set_blocking(false, 100);

    return true;
}


int HTTPServer::poll(bool blocking)
{
    //  This thread basically checks if there is a new incoming connection.
    //  If so , a new HTTPConnection is created and the connection thread is started.
    TCPSocketConnection Clnt;
    if (m_Svr.accept(Clnt) < 0) {
        return -1;
    }

    //   a new connection was received
    INFO("Client (IP=%s) is connected !\n", Clnt.get_address());
    HTTPConnection con(Clnt);
    int c = con.poll();
    if (c == 0) {
        //  Handle the request
        INFO("Handling request !");
        HandleRequest(con.m_Msg, Clnt);
    }
    if (c == -1) {
//        break;
    }

    INFO("Leaving polling thread");
    return 0;
}

void HTTPServer::HandleRequest(HTTPConnection::HTTPMessage& msg, TCPSocketConnection& tcp)
{
    std::string localPath;
    std::map<std::string, HTTPRequestHandler*(*)(const char*, const char*, HTTPConnection::HTTPMessage&, TCPSocketConnection&), handlersComp>::const_iterator it;

    //  Iterate through registered handlers and check if the handler's path is a subset of the requested uri.
    for (it = m_lpHandlers.begin() ; it != m_lpHandlers.end() ; it++) {
        //  check if this entries' path is fully contained at the beginning of the requested path
        std::string curpth = it->first;

        if (msg.uri.find(curpth) == 0) {
            // firts matching handler found, we just take it and we'll be happy
            localPath = msg.uri.substr(curpth.length());
            break;
        }
    }

    if (it == m_lpHandlers.end()) {
        //  There is no such handler, so return invalid

        m_pErrorHandler(msg, tcp);
        INFO("Webrequest left unhandled.");
    } else {
        //  Valid handler was found
        INFO("Routing webrequest !");
        //  Instantiate the handler object (handling will be done from withing the object's constructor
        HTTPRequestHandler *phdl = (*it->second)(it->first.c_str(), localPath.c_str(), msg, tcp);
        //  now we can delete the object, because handling is completed.
        if (phdl != NULL)
            delete phdl;
    }
}
