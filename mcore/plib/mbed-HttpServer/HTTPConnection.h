/* HTTPConnection.h */
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
#ifndef __HTTPConnection_H__
#define __HTTPConnection_H__

#include "mbed.h"
#include "TCPSocketConnection.h"

#include <string>
#include <map>

class HTTPServer;

/** Type HTTPRequestType enumerates request types
*/
typedef enum 
{
    HTTP_RT_GET,        /*!< GET request */
    HTTP_RT_POST,       /*!< POST request */
    HTTP_RT_PUT,        /*!< PUT request */
    HTTP_RT_OPTIONS,    /*!< OPTIONS request */
    HTTP_RT_HEAD,       /*!< HEAD request */
    HTTP_RT_DELETE,     /*!< DELETE request */
    HTTP_RT_TRACE,      /*!< TRACE request */
    HTTP_RT_CONNECT     /*!< CONNECT request */
} HTTPRequestType;

/** class HTTPConnection, encapsulates one connection being made throught the HTTPServer
 *
 */
class HTTPConnection {
    public:
        
        /** HTTPMessage contains all the details of the request received by external HTTP client.
        */
        typedef struct 
        {
            /** Specifies the request type received
            */
            HTTPRequestType                         request;
            /** The uri associated with the request.
            */
            std::string                             uri;
            /** Contains the HTTP/1.1 or HTTP/1.0 version requested by client.
            */
            std::string                             version;
            /** Map of headers provided by the client in the form <HeaderName>:<HeaderValue>
            */
            std::map<std::string, std::string>      headers;
            /** Map of arguments that came with the uri string
            */
            std::map<std::string, std::string>      args;
        } HTTPMessage;
        
        /** Public constructor for HTTPConnection objects.
         *
         */
        HTTPConnection (TCPSocketConnection& clnt);
        
        /** Destructor for HTTPConnection objects.
        *
        */
        ~HTTPConnection();
        
        /** Query if this connection is already closed and can be deleted.
        @returns true, if connection is closed.
        */
        bool is_closed();
        
        /** Polling function for the connection.
        * @returns -1 if connection is not required anymore. In the current version of this library this should be the normal case. This may change in future versions.
        */
        int poll();
    
    protected:
        
        /** Function to close this connection. To be called from internally.
        */
        void close();

        friend class HTTPServer;
                        
        TCPSocketConnection m_Tcp;
        HTTPMessage m_Msg;

        /** parse a HTTP request line from the content of the buffer. 
        * @param buffer : the request received from the client in the form <RequestType> <uri> <Http Version>
        * @returns -1 if the request is invalid or 0 if the request is valid
        */
        int parse(char *buffer);
        
        /** parses the received string in \c buffer for HTTP request headers.
        * @param buffer : buffer which contains the string to parse for headers.
        * @returns -1 in case of an error, otherwise returns 0
        */
        int parseHeader(char *buffer);
        int receiveHeaders(const char* buffer, int nBuffSize);
        
        /** Function which receives a line from the current Socket
        * @param szline : will contain one line received from the socket
        * @param nMaxLen : the size of the buffer. If the line is longer than the buffer the line is cropped at the end.
        * @param nTimeout : if the timeout elapses, only the portion that has been received within this time will be returned.
        * @param szLineTerm : the \c end-of-line character to be used to detect the end of the line.
        * @returns -1 in case of an error or timeout, -2 in case of an empty line or the number of received characters.
        */
        int receiveLine(char* szLine, int nMaxLen, int nTimeout = -1, char szLineTerm = '\n');

    
        /** parse the receoved \c uri_string for arguments which will be stored in the \c args map.
        * @parse uri_string : the uri string which was received from the client.
        * @parse args : the args map which will receive the argument:value touples from the \c uri_string.
        * @returns -1 if an error occured, otherwise returns 0.
        */
        int parseUriArgs(char *uri_string, map<string,string>& args);

};

#endif // __HTTPConnection_H__