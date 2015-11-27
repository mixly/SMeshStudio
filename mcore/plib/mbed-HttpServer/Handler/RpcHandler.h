/* RpcHandler.h */
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
#ifndef __RPCHANDLER_H__
#define __RPCHANDLER_H__

#include "mbed.h"
#include "HTTPRequestHandler.h"

class HTTPRpcRequestHandler : public HTTPRequestHandler
{
        std::string m_rootPath;
        std::string m_localPath;

    public:
        /** constructor for HTTPRpcRequestHandler object and stores the request related data locally. 
        * the request handling will be initiated from within the constructor.
        * @param rootPath : The path under which the handler was registered.
        * @param localPath : The path which is relative to the registered file system root.
        * @param Msg : Message request information that comes with the request.
        * @param Tcp : The socket connection for communicating with the client.
        */
        HTTPRpcRequestHandler(const char* rootPath, const char* localPath, HTTPConnection::HTTPMessage& Msg, TCPSocketConnection& Tcp);
        
        /** Destructor 
        */
        virtual ~HTTPRpcRequestHandler();
  
        /** static creation function for this object.
        */
        static inline HTTPRequestHandler* create(const char* rootPath, const char* localPath, HTTPConnection::HTTPMessage& msg, TCPSocketConnection& tcp) { return new HTTPRpcRequestHandler(rootPath, localPath, msg, tcp); }
              
        /** Handler function to serve GET requests
        */
        virtual int handleGetRequest();
        
        /** Handler function to serve PUT requests
        */
        virtual int handlePutRequest();
        
        /** Handler function to serve POST requests
        */
        virtual int handlePostRequest();

        /** Parse a uri string for uri file name and argument:value pairs
        */
//        int parseUriArgs(string uri, string map<string, string>& args);
};
#endif //   __RPCHANDLER_H__