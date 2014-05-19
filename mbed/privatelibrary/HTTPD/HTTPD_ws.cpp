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
#include "sha1.h"

void HTTPD::recvWS (int id, char c) {

    switch (_state[id].mode) {
    case MODE_WEBSOCKET:
        if (_state[id].n == 0) {
            // flag
            _state[id].websocket_opcode = c & 0x0f;
            _state[id].websocket_flg = c << 8;
            _state[id].n ++;
        } else
        if (_state[id].n == 1) {
            // length 7bit
            _state[id].websocket_flg |= c;
            _state[id].length = c & 0x7f;
            _state[id].n ++;
            if (_state[id].length < 126) {
                _state[id].n = 0;
                if (_state[id].length) {
                    if (_state[id].websocket_flg & 0x0080) {
                        _state[id].mode = MODE_WEBSOCKET_MASK;
                    } else {
                        _state[id].mode = MODE_WEBSOCKET_BODY;
                    }
                } else {
                    _state[id].mode = MODE_WEBSOCKET_ENTER;
                }
                DBG("ws length %d\r\n", _state[id].length);
            }
        } else {
            // length 16bit,64bit
            if (_state[id].n == 2) {
                _state[id].length = c;
                _state[id].n ++;
            } else
            if (_state[id].n < 9 && (_state[id].websocket_flg & 0x7f) == 127) {
                // 64bit
                _state[id].length = (_state[id].length << 8) | c;
                _state[id].n ++;
            } else {
                // end
                _state[id].length = (_state[id].length << 8) | c;
                _state[id].n = 0;
                if (_state[id].websocket_flg & 0x0080) {
                    _state[id].mode = MODE_WEBSOCKET_MASK;
                } else {
                    _state[id].mode = MODE_WEBSOCKET_BODY;
                }
                DBG("ws length2 %d\r\n", _state[id].length);
            }
        }
        break;
    case MODE_WEBSOCKET_MASK:
        // masking key
        _state[id].websocket_mask[_state[id].n] = c;
        _state[id].n ++;
        if (_state[id].n >= 4) {
            _state[id].n = 0;
            _state[id].mode = MODE_WEBSOCKET_BODY;
            DBG("ws mask\r\n");
        }
        break;
    case MODE_WEBSOCKET_BODY:
        // payload
        if (_state[id].websocket_flg & 0x0080) {
            // un-mask
            _state[id].buf->queue(c ^ _state[id].websocket_mask[_state[id].n & 0x03]); 
        } else {
            _state[id].buf->queue(c); 
        }
        _state[id].n ++;
        if (_state[id].n >= _state[id].length) {
            _state[id].mode = MODE_WEBSOCKET_ENTER;
        }
        break;
    }
}

int HTTPD::parseWebsocket (int id) {
    int i;

    DBG("ws opcode %d\r\n", _state[id].websocket_opcode);
    switch (_state[id].websocket_opcode) {
    case 0x00: // continuation
        break;
    case 0x01: // text
    case 0x02: // binary
        i = getHandler(_state[id].uri);
        if (i >= 0) {
            if (_handler[i].funcCgi) {
                // cgi
                _handler[i].funcCgi(id);
            }
        }
        break;
    case 0x08: // close
        _state[id].client->close();
        break;
    case 0x09: // ping
        {
        char pong[_state[id].n + 2];
        pong[0] = 0x8a;
        pong[1] = 0x04;
        for (i = 0; i < _state[id].length; i ++) {
            if (_state[id].buf->dequeue(&pong[i + 2]) == false) break;
        }
        _state[id].client->send_all(pong, _state[id].length + 2);
        }
        break;
    case 0x0a: // pong
        break;
    default:
        break;
    }
    _state[id].n = 0;
    _state[id].length = 0;
    return 0;
}

int HTTPD::acceptWebsocket (int id) {
    char buf[HTTPD_CMD_SIZE], buf2[HTTPD_CMD_SIZE];
    
    DBG("websocket accept: %d\r\n", id);

    strcpy(buf, "HTTP/1.1 101 Switching Protocols\r\n");
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, "Upgrade: websocket\r\n");
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, "Connection: Upgrade\r\n");
    _state[id].client->send_all(buf, strlen(buf));

    strcpy(buf, "Sec-WebSocket-Accept: ");
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, _state[id].websocket_key);
    strcat(buf, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    sha1(buf, strlen(buf), buf2);
    base64encode(buf2, 20, buf, sizeof(buf));
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, "\r\n\r\n");
    _state[id].client->send_all(buf, strlen(buf));
    _state[id].client->set_blocking(true, HTTPD_TIMEOUT * 100);
    return 0;
}

int HTTPD::sendWebsocket (int id, const char *buf, int len, const char *mask) {
    HTTPD *httpd = HTTPD::getInstance();
    int i = 0, r;
    char tmp[10];

    tmp[i++] = 0x81; // single, text frame
    if (len < 126) {
        tmp[i++] = (mask == NULL ? 0 : 0x80) | len;
    } else {
        tmp[i++] = (mask == NULL ? 0 : 0x80) | 126;
        tmp[i++] = (len >> 8) & 0xff;
        tmp[i++] = len & 0xff;
    }
    if (mask) {
        memcpy(&tmp[i], mask, 4);
        i += 4;
    }
    r = httpd->_state[id].client->send_all(tmp, i);

    if (r >= 0) {
        if (mask) {
            char tmp2[len];
            for (i = 0; i < len; i ++) {
                tmp2[i] = buf[i] ^ mask[i & 0x03];
            }
            r = httpd->_state[id].client->send_all(tmp2, len);
        } else {
            r = httpd->_state[id].client->send_all((char*)buf, len);
        }
    }
    return r;
}


