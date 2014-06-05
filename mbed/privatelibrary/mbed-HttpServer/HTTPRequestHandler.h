/* HTTPRequestHandler.h */
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
#ifndef __HTTPREQUESTHANDLER_H__
#define __HTTPREQUESTHANDLER_H__

#include "mbed.h"
#include "HTTPConnection.h"

#include <map>

typedef std::map<const char*,const char*> HTTPHeaders;

typedef enum {
    HTTP_Continue                       = 100,  // 100
    HTTP_SwitchingProtocols             = 101,  // 101
    HTTP_Ok                             = 200,  // 200
    HTTP_Created                        = 201,  // 201
    HTTP_Accepted                       = 202,  // 202
    HTTP_NonAuthoritativeInformation    = 203,  // 203
    HTTP_NoContent                      = 204,  // 204
    HTTP_ResetContent                   = 205,  // 205
    HTTP_PartialContent                 = 206,  // 206
    HTTP_MultipleChoices                = 300,  // 300
    HTTP_MovedPermanently               = 301,  // 301
    HTTP_Found                          = 302,  // 302
    HTTP_SeeOther                       = 303,  // 303
    HTTP_NotModified                    = 304,  // 304
    HTTP_UseProxy                       = 305,  // 305
    HTTP_TemporaryRedirect              = 307,  // 307
    HTTP_BadRequest                     = 400,  // 400
    HTTP_Unauthorized                   = 401,  // 401
    HTTP_PaymentRequired                = 402,  // 402
    HTTP_Forbidden                      = 403,  // 403
    HTTP_NotFound                       = 404,  // 404
    HTTP_MethodNotAllowed               = 405,  // 405
    HTTP_NotAcceptable                  = 406,  // 406
    HTTP_ProxyAuthRequired              = 407,  // 407
    HTTP_RequestTimeOut                 = 408,  // 408
    HTTP_Conflict                       = 409,  // 409
    HTTP_Gone                           = 410,  // 410
    HTTP_LengthRequired                 = 411,  // 411
    HTTP_PreconditionFailed             = 412,  // 412
    HTTP_RequestEntityTooLarge          = 413,  // 413
    HTTP_RequestURITooLarge             = 414,  // 414
    HTTP_UnsupportedMediaType           = 415,  // 415
    HTTP_RequestedRangeNotSatisfiable   = 416,  // 416
    HTTP_ExpectationFailed              = 417,  // 417
    HTTP_InternalServerError            = 500,  // 500
    HTTP_NotImplemented                 = 501,  // 501
    HTTP_BadGateway                     = 502,  // 502
    HTTP_ServiceUnavailable             = 503,  // 503
    HTTP_GatewayTimeout                 = 504,  // 504
    HTTP_HTTPVersionNotSupported        = 505,  // 505
} HTTPResponseCode;

/** class HTTPRequestHandler is the base class for HTTP Handler request implementations.
*
*/
class HTTPRequestHandler
{
    protected:
        HTTPConnection::HTTPMessage& msg;
        TCPSocketConnection& tcp;
        
    public:
        /** Constructor for HTTPRequestHandler objects.
         */
        HTTPRequestHandler(HTTPConnection::HTTPMessage&, TCPSocketConnection&);
        
        /** Destructor for HTTPRequestHandler objects.
        */
        virtual ~HTTPRequestHandler();
        
        /** Handler function which will be used by the HTTPServer to serve requests.
        * The default version of this function will dispatch respective handler member
        * functions according to the request type given in the HTTPMessage object.
        * The list of possible functions dispatched is :
        * * handleGetRequest
        * * handlePostRequest
        * * handlePutRequest
        */
        virtual void handleRequest();
        
        /** Handler function which will handle errors and return errors correctly
        * @param errorCode : The error code returned by the HTTP Server to represent the error condition.
        * @param msg : Request Message information.
        * @param tcp : The socket which represents the active connection to the client.
        */
        virtual void handleError(int errorCode, HTTPHeaders* header = NULL);

        /** Function sends the response header which consists of the return code and the headers section
        * the response header also contains the length (in bytes) of the body. You need to send the body
        * right after calling this function. Please note that you do not need to consider the terminating
        * CR+LF after your last CR+LF. This will be sent automatically once \c endResponse is called. Also
        * the Length given in \c nLen does not need to consider this additional chars. It will also be
        * automatically added in \c startResponse. if you need to change the headers, please do NOT 
        * specify the \c Content-Length Header. This is done automatically be the function.
        */
        void startResponse(int returnCode, long nLen, HTTPHeaders* header = NULL);
        void processResponse(int nLen, char* body );
        void endResponse();

    protected:
        /** Handler function to serve GET requests. Download ressource from server from \c uri location.
        */
        virtual int handleGetRequest() = 0;
        
        /** Handler function to serve PUT requests. Upload ressource to server to \c uri location.
        */
        virtual int handlePutRequest() = 0;
        
        /** Handler function to serve POST requests. Send data to webserver. Can also be appended to uri.
        */
        virtual int handlePostRequest() = 0;
        
        void getStandardHeaders(HTTPHeaders& header, const char* fext = NULL);
};

#endif //   __HTTPREQUESTHANDLER_H__