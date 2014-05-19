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

#include "HTTPD.h"


int HTTPD::httpdFile (int id, char *dir) {
    FILE *fp;
    int i, len;
    char buf[HTTPD_BUF_SIZE];
    char file[HTTPD_CMD_SIZE];

    INFO("httpdFile %d %s", id, dir);

    strcpy(file, dir);
    strcat(file, _state[id].filename);
    if (file[strlen(file) - 1] == '/') {
        strcat(file, "index.html");
    }
    DBG("file: %s\r\n", file);

    fp = fopen(file, "r");
    if (fp) {
        strcpy(buf, "HTTP/1.1 200 OK\r\n");
        _state[id].client->send_all(buf, strlen(buf));
        {
            // file size
            i = ftell(fp);
            fseek(fp, 0, SEEK_END);
            len = ftell(fp);
            fseek(fp, i, SEEK_SET);
        }

        strcpy(buf, "Server: GSwifi httpd\r\n");
        _state[id].client->send_all(buf, strlen(buf));
        if (_state[id].keepalive) {
            strcpy(buf, "Connection: Keep-Alive\r\n");
        } else {
            strcpy(buf, "Connection: close\r\n");
        }
        _state[id].client->send_all(buf, strlen(buf));
        sprintf(buf, "Content-Type: %s\r\n", mimetype(file));
        _state[id].client->send_all(buf, strlen(buf));
        sprintf(buf, "Content-Length: %d\r\n\r\n", len);
        _state[id].client->send_all(buf, strlen(buf));

        for (;;) {
            i = fread(buf, sizeof(char), sizeof(buf), fp);
            if (i <= 0) break;
            _state[id].client->send_all(buf, i);
#if defined(TARGET_LPC1768) || defined(TARGET_LPC2368)
            if (feof(fp)) break;
#endif
        }
        fclose(fp);
        return 0;
    }

    httpdError(id, 404);
    return -1;
}

void HTTPD::httpdError (int id, int err) {
    char buf[HTTPD_CMD_SIZE], msg[30];
    
    switch (err) {
    case 400:
        strcpy(msg, "Bad Request");
        break;
    case 403:
        strcpy(msg, "Forbidden");
        break;
    case 404:
        strcpy(msg, "Not Found");
        break;
    case 500:
    default:
        strcpy(msg, "Internal Server Error");
        break;
    }
    DBG("httpd error: %d %d %s\r\n", id, err, msg);
    
    sprintf(buf, "HTTP/1.1 %d %s\r\n", err, msg);
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, "Content-Type: text/html\r\n\r\n");
    _state[id].client->send_all(buf, strlen(buf));

    sprintf(buf, "<html><head><title>%d %s</title></head>\r\n", err, msg);
    _state[id].client->send_all(buf, strlen(buf));
    sprintf(buf, "<body><h1>%s</h1></body></html>\r\n", msg);
    _state[id].client->send_all(buf, strlen(buf));
    wait_ms(100);
    _state[id].client->close();
//    WARN("%d.%d.%d.%d ", _httpd[cid].host.getIp()[0], _httpd[cid].host.getIp()[1], _httpd[cid].host.getIp()[2], _httpd[cid].host.getIp()[3]);
//    WARN("%s %s %d %d -\r\n", _httpd[cid].type == GSPROT_HTTPGET ? "GET" : "POST", _httpd[cid].uri, _httpd[cid].length, err);
}


void HTTPD::recvData (int id, char c) {

    switch (_state[id].mode) {
    case MODE_REQUEST:
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            _state[id].buf->flush();
            _state[id].buf->queue(c);
            _state[id].mode = MODE_REQSTR;
        } else {
            _state[id].buf->flush();
        }
        break;
    case MODE_REQSTR:
        switch (c) {
        case 0:
            break;
        case 0x0a: // LF
        case 0x0d: // CR
            if (parseRequest(id)) {
                _state[id].mode = MODE_REQSTR;
            } else {
                _state[id].mode = MODE_HEADER;
            }
            _state[id].enter = 0;
            break;
        default:
            _state[id].buf->queue(c);
            break;
        }
        break;
    case MODE_HEADER:
        switch (c) {
        case 0:
            break;
        case 0x0a: // LF
        case 0x0d: // CR
            if (_state[id].buf->available() == 0) {
                if ((_state[id].enter == 0x0d && c == 0x0a) || (_state[id].enter == 0x0a && c == 0x0a)) {
                    _state[id].buf->flush();
                    if (_state[id].websocket) {
                        INFO("MODE_WEBSOCKET");
                        acceptWebsocket(id);
                        _state[id].mode = MODE_WEBSOCKET;
                    } else
                    if (_state[id].length) {
                        INFO("MODE_BODY");
                        _state[id].mode = MODE_BODY;
                    } else {
                        INFO("MODE_ENTER");
                        _state[id].mode = MODE_ENTER;
                    }
                }
                _state[id].enter = c;
                _state[id].buf->flush();
                break;
            }

            parseHeader(id);
            _state[id].enter = 0;
            break;
        default:
            _state[id].buf->queue(c);
            _state[id].enter = 0;
            break;
        }
        break;
    case MODE_BODY:
        _state[id].buf->queue(c);
        if (_state[id].buf->available() >= _state[id].length) {
            _state[id].mode = MODE_ENTER;
        }
        break;
    case MODE_WEBSOCKET:
    case MODE_WEBSOCKET_MASK:
    case MODE_WEBSOCKET_BODY:
        recvWS(id, c);
        break;
    }

    if (_state[id].mode == MODE_ENTER) {
        int i = getHandler(_state[id].uri);
        if (i >= 0) { 
            if (_handler[i].dir) {
                // file
                httpdFile(id, _handler[i].dir);
            } else
            if (_handler[i].funcCgi) {
                // cgi
                _handler[i].funcCgi(id);
//                _state[id].keepalive = 0;
            } else {
                httpdError(id, 403);
            }
        } else {
            httpdError(id, 404);
        }

        if (_state[id].keepalive) {
            DBG("keepalive %d", _state[id].keepalive);
            _state[id].keepalive --;
        } else {
            _state[id].client->close();
        }
        _state[id].mode = MODE_REQUEST;
    } else
    if (_state[id].mode == MODE_WEBSOCKET_ENTER) {
        parseWebsocket(id);
        _state[id].mode = MODE_WEBSOCKET;
    }
}

int HTTPD::parseRequest (int id) {
    int i, j, len;
    char buf[HTTPD_CMD_SIZE];

    for (len = 0; len < sizeof(buf); len++) {
        if (_state[id].buf->dequeue(&buf[len]) == false) break;
    }
    buf[len] = 0;

    if (strnicmp(buf, "GET ", 4) == 0) {
        _state[id].req = REQ_HTTPGET;
        j = 4;
    } else
    if (strnicmp(buf, "POST ", 5) == 0) {
        _state[id].req = REQ_HTTPPOST;
        j = 5;
    } else {
        return -1;
    }

    for (i = 0; i < len - j; i ++) {
        _state[id].uri[i] = buf[i + j];
        if (buf[i + j] == ' ' || i >= sizeof(buf) - 1) {
            _state[id].uri[i] = 0;
            INFO("URI %d '%s'", _state[id].req, _state[id].uri);
            _state[id].mode = MODE_HEADER;
            _state[id].buf->flush();
            _state[id].length = 0;
            _state[id].n = 0;
            _state[id].websocket = 0;
            _state[id].filename = NULL;
            _state[id].querystring = NULL;
            break;
        }
    }

    i = getHandler(_state[id].uri);
    if (i >= 0) { 
        _state[id].filename = &_state[id].uri[strlen(_handler[i].uri)];
        for (i = 0; i < strlen(_state[id].filename); i ++) {
            if (_state[id].filename[i] == '?') {
                _state[id].filename[i] = 0;
                _state[id].querystring = _state[id].filename + i + 1;
                break;
            }
        }
        INFO("FILE '%s' QUERY '%s'", _state[id].filename, _state[id].querystring);
    }
    return 0;
}

#define HEADER_TABLE_NUM 5
int HTTPD::parseHeader (int id) {
    int i;
    char buf[HTTPD_CMD_SIZE];
    static const struct HEADER_TABLE {
        const char header[24];
        void (HTTPD::*func)(int id, const char*);
    } header_table[HEADER_TABLE_NUM] = {
      {"Content-Length:",         &HTTPD::reqContentLength},
      {"Connection:",             &HTTPD::reqConnection},
      {"Upgrade: websocket",      &HTTPD::reqUpgrade},
      {"Sec-WebSocket-Version:",  &HTTPD::reqWebSocketVersion},
      {"Sec-WebSocket-Key:",      &HTTPD::reqWebSocketKey},
    };
    for (i = 0; i < sizeof(buf); i++) {
        if (_state[id].buf->dequeue(&buf[i]) == false) break;
    }
    buf[i] = 0;

    for (i = 0; i < HEADER_TABLE_NUM; i ++) {
        if (strnicmp(buf, header_table[i].header, strlen(header_table[i].header)) == 0) {
            DBG("parse header %d '%s'\r\n", i, buf);
            if (header_table[i].func != NULL) {
                (this->*(header_table[i].func))(id, buf);
            }
            return 0;
        }
    }

    return -1;
}

void HTTPD::reqContentLength (int id, const char *buf) {
    _state[id].length = atoi(&buf[16]);
}

void HTTPD::reqConnection (int id, const char *buf) {
    if (strnicmp(&buf[12], "Keep-Alive", 10) == 0 && _state[id].keepalive == 0) {
        _state[id].keepalive = HTTPD_KEEPALIVE;
    } else {
        _state[id].keepalive = 0;
    }
}

void HTTPD::reqUpgrade (int id, const char *buf) {
    if (! _state[id].websocket) _state[id].websocket = 1;
}

void HTTPD::reqWebSocketVersion (int id, const char *buf) {
    _state[id].websocket = atoi(&buf[23]);
}

void HTTPD::reqWebSocketKey (int id, const char *buf) {
    if (_state[id].websocket_key == NULL) {
        _state[id].websocket_key = (char*)malloc(30);
    }
    strncpy(_state[id].websocket_key, &buf[19], 30);
}
