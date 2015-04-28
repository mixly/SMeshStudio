/* HTTPServer.cpp */
/*
Copyright (c) 2013 Henry Leinen (henry[dot]leinen [at] online [dot] de)
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include "mbed.h"
#include "EthernetInterface.h"
#include "HTTPConnection.h"
#include "HTTPRequestHandler.h"

#include <map>
using std::map;
#include <string>
using std::string;

#include <TCPSocketConnection.h>
#include <TCPSocketServer.h>

/** Typedefinition for a handler function
*/
typedef void (*HTTPRequestHandlerFunction)(HTTPConnection::HTTPMessage&, TCPSocketConnection&);


/** Class HTTPServer for use with <a href="http://mbed.org/users/samux/code/WiflyInterface/">WiflyInterface</a>.
* This class is a simple implementation of an HTTP Server for use with the WiFly interface.
* The class will listen for incoming connections on the (configurable) HTTP port. For each 
* incoming connection, one request will be processed. 
* After instantiating this class, add all required handlers using the \c addHandler function,
* replace the default error handler using \c addErrorHandler if needed and call the \c start
* method to initialize the class.
* You need to continuously poll for any new incoming connections after one request has been
* served using the \c poll member function.
*
* \b Example:
* @code
* #include "mbed.h"
* #include "HTTPServer.h"
* #include "LocalFileSystem.h"
*
* LocalFileSystem local("local"); 
*
* void main(void)
* {
*     HTTPServer svr;
*     svr.mount("/local/", "/");
*     svr.addHandler<HTTPFsRequestHandler>( "/" );
*     svr.start();
*     while(1)
*     {
*         if (svr.poll() < 0)
*             exit(0);
*     }
* }
* @endcode
*
* An alternate approach e.g. if you need to perform additional tasks using the EthernetInterface
* there is the possibility to provide the EthernetInterface object in an initialized and connected
* state. __NOTE: You should choose this scenario for compatibility reasons.___
*
* \b Example2:
* @code
* #include "mbed.h"
* #include "HTTPServer.h"
* #include "EthernetInterface.h"
* #include "LocalFileSystem.h"
*
* LocalFileSystem local("local");
* EthernetInterface eth;
*
* void main(void)
* {
*     HTTPServer svr;
*     //    Initialize the ethernet interface
*     if (eth.init() != 0) {
*         printf("Initialization of EthernetInterface failed !");
*         exit(0);
*     }
*     //    Connect using DHCP
*     if (eth.connect() !=0) {
*         printf("Failed to connect using DHCP !");
*         exit(0);
*     }
*     
*     // Moint the local file system and provide a handler for 'root'.
*     svr.mount("/local/", "/");
*     svr.addHandler<HTTPFsRequestHandler>( "/" );
*     // Start the server on port 80, providing our own ethernet interface object.
*     svr.start(80, &eth);
*     while(1)
*     {
*         if (svr.poll() < 0)
*             exit(0);
*     }
* }
* @endcode
*     
*/
class HTTPServer
{
        TCPSocketServer         m_Svr;
        bool                    m_bServerListening;
        EthernetInterface*      m_pEthernet;
       
    public:
        /** Constructor for HTTPServer objects.
        */
        HTTPServer();
        
        /** Destructor for HTTPServer objects.
        */
        ~HTTPServer();

        /**
        * Structure which will allow to order the stored handlers according to their associated path.
        */
        struct handlersComp //Used to order handlers in the right way
        {
            bool operator() (const string& handler1, const string& handler2) const
            {
                //The first handler is longer than the second one
                if (handler1.length() > handler2.length())
                    return true; //Returns true if handler1 is to appear before handler2
                else if (handler1.length() < handler2.length())
                    return false;
                else //To avoid the == case, sort now by address
                    return ((&handler1)>(&handler2));
            }
        };

        /**
        * Adds a request handler to the handlers list. You will have to use one of the existing implementations.
        * With each handler a \c uri or \c path is associated. Whenever a request is received the server will
        * walk through all registered handlers and check which \c path is matching.
        * @param T : class which will be instanciated to serve these requests for the associated \b path.
        * @param path : request uri starting with this \c path will be served using this handler.
        */
        template<typename T>
        void addHandler(const char* path)
        { m_lpHandlers[path] = &T::create; }
    
        /**
        * Replaces the standard error Handler. The error Handler will be called everytime a request is not
        * matching any of the registered \c paths or \c uris.
        * @param hdlFunc: User specified handler function which will be used in error conditions.
        */
        void addErrorHandler(HTTPRequestHandlerFunction hdlFunc)
        { m_pErrorHandler = hdlFunc!=NULL ?hdlFunc : StdErrorHandler; }    

        /** Binds server to a specific port and starts listening. This member prepares the internal variables and the server socket
        * and terminates after successfull initialization
        * @param port : port on which to listen for incoming connections
        * @param pEthernet : a pointer to an existing EthernetInterface object or NULL if the HTTPServer shall allocate the object. _Please note that for compatibility reasons
        * your should consider to create the EthernetInterface as a static variable. Otherwise the the object will be created on the heap._
        * @returns : false if an unrecoverable error occured or if the ethernet interface was not set or not initialized correctly, or true if everything was ok.
        */
        bool start(int port = 80, EthernetInterface* pEthernet = NULL);
        
        /** Performs the regular polling of the server component. Needs to be called cyclically.
        * The function will internally check whether new connections are requested by a client and will also poll all existing client connections.
        * @param blocking : if true, 
        * @returns -1 if there was a problem. If 0 is returned, the latest request was served successfully and the server is
        * ready for processing the next request. Simply call \c poll as long as you want to serve new incoming requests.
        */
        int poll(bool blocking = true);
        
    private:
        
        /** The standard error handler function.
        * @param msg : Request message data.
        * @param tcp : Socket to be used for responding.
        */
        static void StdErrorHandler(HTTPConnection::HTTPMessage& msg, TCPSocketConnection& tcp);
        
        /** Internal function which processes a request and which will try to find the matching handler function
        * for the given request. Please note that the function will search through the list of handlers, iterating
        * from longest to shortest \c paths. If the registered \c path is a subset of the request the associated
        * handler is considered as being a match.
        * @param msg : Request message data. Contains the requested logical \c uri. 
        * @param tcp : Socket to be used for communication with the client.
        */
        void HandleRequest(HTTPConnection::HTTPMessage& msg, TCPSocketConnection& tcp);
    
        /** Map of handler objects. Can be any object derived from \ref HTTPRequestHeader. Use the \ref addHandler function
        * to register new handler objects.
        */
        map<string, HTTPRequestHandler* (*)(const char*, const char*, HTTPConnection::HTTPMessage&, TCPSocketConnection&), handlersComp>   m_lpHandlers;
        HTTPRequestHandlerFunction m_pErrorHandler;
        
 };
 
 #endif //__HTTPSERVER_H__