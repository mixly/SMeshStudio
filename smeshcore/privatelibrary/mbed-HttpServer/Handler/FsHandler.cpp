/* FsHandler.cpp */
#include "mbed.h"
#include "FsHandler.h"
//#define DEBUG
#include "hl_debug.h"



static int matchstrings(const char* one, const char* two)
{
    int m = 0;
    
    for (m = 0; m < min(strlen(one), strlen(two)) ; m++) {
        if (one[m] != two[m])
            return m;
    }
    return m;
}

std::map<const char*, const char*> HTTPFsRequestHandler::m_fsMap;
 
HTTPFsRequestHandler::HTTPFsRequestHandler(const char* rootPath, const char* localPath, HTTPConnection::HTTPMessage& Msg, TCPSocketConnection& Tcp)
    : HTTPRequestHandler(Msg, Tcp)
{
    m_rootPath = rootPath;
    m_localPath = localPath;
    
    string myPath = m_rootPath + m_localPath;
    
    //  Now replace the virtual root path with a mounted device path
    std::map<const char*, const char*>::iterator it;
    const char *bestMatch = NULL;
    const char *bestMatchExchange = NULL;
    int match_ind = -1;
    for (it = m_fsMap.begin() ; it != m_fsMap.end() ; it++) {
        //  find best match (if the given logical path is containted in the root
        int s = matchstrings(myPath.c_str(), it->second);
        INFO("Matching Root %s with handler %s results in %d identical characters\n", myPath.c_str(), it->second, s);
        if ((s == strlen(it->second)) && (s > match_ind)) {
            match_ind = s;
            bestMatch = it->first;
            bestMatchExchange = it->second;
        }
    }

    if (bestMatch != NULL) {
        m_rootPath = bestMatch;
        m_localPath = string(myPath).substr(strlen(bestMatchExchange));
    }
            
    handleRequest();
}

HTTPFsRequestHandler::~HTTPFsRequestHandler()
{
}

int HTTPFsRequestHandler::handleGetRequest()
{
    HTTPHeaders headers;
    
    if (m_localPath.length() > 4) 
        getStandardHeaders(headers, m_localPath.substr(m_localPath.length()-4).c_str());
    else
        getStandardHeaders(headers);
    
    INFO("Handling Get Request (root = %s, local = %s).", m_rootPath.c_str(), m_localPath.c_str());
    int retval = 0;   //success
    std::string reqPath;

    //  Check if we received a directory with the local bath
    if ((m_localPath.length() == 0) || (m_localPath.substr( m_localPath.length()-1, 1) == "/")) {
        //  yes, we shall append the default page name
        m_localPath += "index.html";
    }
    
    reqPath = m_rootPath + m_localPath;
    
    INFO("Mapping \"%s\" to \"%s\"", msg.uri.c_str(), reqPath.c_str());
        
    FILE *fp = fopen(reqPath.c_str(), "r");
    if (fp != NULL) {
        char * pBuffer = NULL;
        int sz = 8192;
        while( pBuffer == NULL) {
            sz /= 2;
            pBuffer = (char*)malloc(sz);
            if (sz < 128)
                error ("OutOfMemory");
        }
        
        //  File was found and can be returned
    
        //  first determine the size
        fseek(fp, 0, SEEK_END);
        long size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    
        startResponse(200, size);
        while(!feof(fp) && !ferror(fp)) {
            int cnt = fread(pBuffer, 1, sz , fp);
            if (cnt < 0)
                cnt = 0;
            processResponse(cnt, pBuffer);
        }
        delete pBuffer;
        endResponse();
        fclose(fp);
    }
    else {
        retval = 404;
        ERR("Requested file was not found !");
    }
    
    return retval;
}

int HTTPFsRequestHandler::handlePostRequest()
{
    return 404;
}

int HTTPFsRequestHandler::handlePutRequest()
{
    return 404;
}
