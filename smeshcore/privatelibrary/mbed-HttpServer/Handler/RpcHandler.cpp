/* FsHandler.cpp */
#include "mbed.h"
#include "RpcHandler.h"
#include "mbed_rpc.h"


#define DEBUG 0
#include "hl_debug.h"

RPC rpc("rpc");


HTTPRpcRequestHandler::HTTPRpcRequestHandler(const char* rootPath, const char* localPath, HTTPConnection::HTTPMessage& Msg, TCPSocketConnection& Tcp)
    : HTTPRequestHandler(Msg, Tcp)
{
    m_rootPath = rootPath;
    m_localPath = localPath;
    
    handleRequest();
}


HTTPRpcRequestHandler::~HTTPRpcRequestHandler()
{
}


int HTTPRpcRequestHandler::handleGetRequest()
{
    char outBuf[256] = {};
    bool retval = false;
    int err = 404;
    string rpc_args("");
    
    INFO("Handling RPC Get Requesst.");
    // This version of the RPC handler does only accept native RPC commands in the format
    //  /<class>/<method> <argument1> [<argument2> [<argument3> ...]]
    // So we can simply pass our local pathg to our rpc

    //  Append missing slash if needed
    if (m_localPath.c_str()[0] != '/') {
        rpc_args = "/";
    }
    // replace the HTTP strings with ascii strings
    for (int i = 0 ; i < m_localPath.length() ; i++) {
        if (m_localPath.substr(i,3) == "%20") {
            rpc_args += " ";
            i+=2;
        }
        else {
            rpc_args += m_localPath.substr(i,1);
        }
    }
    INFO("RPC to %s", rpc_args.c_str());
    retval = rpc.call(rpc_args.c_str(),outBuf);
    INFO("RPC Request returned %d with args : %s", retval==true ? 1 : 0, outBuf);
    if (retval) {
        //  No error
        startResponse(retval, strlen(outBuf));
        processResponse(strlen(outBuf), outBuf);
        endResponse();
        err = 0;
    }
    
    return err;
}

int HTTPRpcRequestHandler::handlePutRequest()
{
    return 404;
}

int HTTPRpcRequestHandler::handlePostRequest()
{
    return 404;
}
