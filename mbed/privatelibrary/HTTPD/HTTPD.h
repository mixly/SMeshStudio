/* Copyright (C) 2013 Hiroshi Suga, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef HTTPD_H
#define HTTPD_H

#include "mbed.h"
#include "rtos.h"
#include "EthernetInterface.h"
#include "CBuffer.h"

//#define DEBUG

#define HTTPD_PORT 80
#define HTTPD_MAX_CLIENTS 2
#define HTTPD_KEEPALIVE 10
#define HTTPD_TIMEOUT 15000
#define HTTPD_MAX_HANDLES 10
 
#define HTTPD_CMD_SIZE 100
#define HTTPD_BUF_SIZE 256
#define HTTPD_STACK_SIZE (1024 * 6)
//#define HTTPD_ENABLE_CLOSER

//Debug is disabled by default
#if defined(DEBUG) and (!defined(TARGET_LPC11U24))
#define DBG(x, ...) std::printf("[DBG]" x "\r\n", ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[WARN]" x "\r\n", ##__VA_ARGS__);
#define ERR(x, ...) std::printf("[ERR]" x "\r\n", ##__VA_ARGS__);
#define INFO(x, ...) std::printf("[INFO]" x "\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#define INFO(x, ...)
#endif


class HTTPD {
public:

    enum Request {
        REQ_HTTPGET,
        REQ_HTTPPOST,
        REQ_PUT,
    };

    enum Mode {
        MODE_REQUEST,
        MODE_REQSTR,
        MODE_HEADER,
        MODE_BODY,
        MODE_ENTER,
        MODE_ERROR,
        MODE_WEBSOCKET,
        MODE_WEBSOCKET_MASK,
        MODE_WEBSOCKET_BODY,
        MODE_WEBSOCKET_ENTER,
    };

    struct STATE {
        Thread *thread;
        TCPSocketConnection *client;
        volatile Request req;
        volatile Mode mode;
        CircBuffer <char>*buf;
        char uri[HTTPD_CMD_SIZE];
        char *filename;
        char *querystring;
        int enter;
        int length;
        int n;
        int keepalive;
        int websocket;
        char *websocket_key;
        int websocket_opcode;
        int websocket_flg;
        char websocket_mask[4];
        int websocket_payload;
        int (*sendws)(int id, const char *buf, int len, const char *mask);
    };

    HTTPD ();
    
    int start (int port = HTTPD_PORT);

    // --- HTTPD_req.cpp ---
    void httpdError (int id, int err);

    // --- HTTPD_ws.cpp ---
    static int sendWebsocket (int id, const char *buf, int len, const char *mask = NULL);

    // --- HTTPD_util.cpp ---
    char *getUri (int id);
    char *getFilename (int id);
    char *getQueryString (int id);
    int send (int id, const char *body, int len, const char *header = NULL);
    int receive (int id, char *buf, int len);
    int attach (const char *uri, const char *dir);
    int attach (const char *uri, void (*funcCgi)(int id));
    int base64encode (const char *input, int length, char *output, int len);
    int urlencode (const char *str, char *buf, int len);
    int urldecode (const char *str, char *buf, int len);

    static HTTPD * getInstance() {
        return _inst;
    };

private :
    static HTTPD *_inst;
    Thread *_daemon;
    TCPSocketServer _server;

#ifdef HTTPD_ENABLE_CLOSER
    struct STATE _state[HTTPD_MAX_CLIENTS + 1];
#else
    struct STATE _state[HTTPD_MAX_CLIENTS];
#endif

    struct HANDLER {
        char *uri;
        char *dir;
        void (*funcCgi)(int id);
    } _handler[HTTPD_MAX_HANDLES];

    int _handler_count;

    static void daemon (void const *arg);
    static void child (void const *arg);
    static void closer (void const *arg);

    // --- HTTPD_req.cpp ---
    int httpdFile (int id, char *dir);
    void recvData (int id, char c);
    int parseRequest (int id);
    int parseHeader (int id);
    void reqContentLength (int id, const char *buf);
    void reqConnection (int id, const char *buf);
    void reqUpgrade (int id, const char *buf);
    void reqWebSocketVersion (int id, const char *buf);
    void reqWebSocketKey (int id, const char *buf);

    // --- HTTPD_ws.cpp ---
    void recvWS (int id, char c);
    int parseWebsocket (int id);
    int acceptWebsocket (int id);

    // --- HTTPD_util.cpp ---
    int getHandler (const char *uri);
    char *mimetype (char *file);
    int strnicmp (const char *p1, const char *p2, int n);
    int from_hex (int ch);
    int to_hex (int code);
};

#endif
