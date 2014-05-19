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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MIMETABLE_NUM 9
static const struct {
    char ext[5];
    char type[24];
} mimetable[MIMETABLE_NUM] = {
    {"txt", "text/plain"},  // default
    {"html", "text/html"},
    {"htm", "text/html"},
    {"css", "text/css"},
    {"js",  "application/javascript"},
    {"jpg", "image/jpeg"},
    {"png", "image/png"},
    {"gif", "image/gif"},
    {"ico", "image/x-icon"},
};

char *HTTPD::getUri (int id) {
    return _state[id].uri;
}

char *HTTPD::getFilename (int id) {
    return _state[id].filename;
}

char *HTTPD::getQueryString (int id) {
    return _state[id].querystring;
}

int HTTPD::receive (int id, char *buf, int len) {
    int i;

    for (i = 0; i < len; i ++) {
        if (_state[id].buf->dequeue(&buf[i]) == false) break;
    }
    return i;
}

int HTTPD::send (int id, const char *body, int len, const char *header) {
    char buf[HTTPD_CMD_SIZE];

    strcpy(buf, "HTTP/1.1 200 OK\r\n");
    _state[id].client->send_all(buf, strlen(buf));
    strcpy(buf, "Server: GSwifi httpd\r\n");
    _state[id].client->send_all(buf, strlen(buf));
    if (_state[id].keepalive) {
        strcpy(buf, "Connection: Keep-Alive\r\n");
    } else {
        strcpy(buf, "Connection: close\r\n");
    }
    _state[id].client->send_all(buf, strlen(buf));
    if (header) {
        _state[id].client->send_all((char*)header, strlen(header));
    }
    sprintf(buf, "Content-Length: %d\r\n\r\n", len);
    _state[id].client->send_all(buf, strlen(buf));

    return _state[id].client->send_all((char*)body, len);
}

int HTTPD::getHandler (const char *uri) {
    int i;

    for (i = 0; i < _handler_count; i ++) {
        if (strncmp(uri, _handler[i].uri, strlen(_handler[i].uri)) == NULL) {
            // found
            return i;
        }
    }
    return -1;
}

int HTTPD::attach (const char *uri, const char *dir) {
    if (_handler_count < HTTPD_MAX_HANDLES) {
        _handler[_handler_count].uri = (char*)malloc(strlen(uri) + 1);
        strcpy(_handler[_handler_count].uri, uri);
        _handler[_handler_count].dir = (char*)malloc(strlen(dir) + 1);
        strcpy(_handler[_handler_count].dir, dir);
        _handler[_handler_count].funcCgi = NULL;
        _handler_count ++;
        return 0;
    } else {
        return -1;
    }
}

int HTTPD::attach (const char *uri, void (*funcCgi)(int)) {
    if (_handler_count < HTTPD_MAX_HANDLES) {
        _handler[_handler_count].uri = (char*)malloc(strlen(uri) + 1);
        strcpy(_handler[_handler_count].uri, uri);
        _handler[_handler_count].dir = NULL;
        _handler[_handler_count].funcCgi = funcCgi;
        _handler_count ++;
        return 0;
    } else {
        return -1;
    }
}


char *HTTPD::mimetype (char *file) {
    int i, j;

    for (i = 0; i < MIMETABLE_NUM; i ++) {
        j = strlen(mimetable[i].ext);
        if (file[strlen(file) - j - 1] == '.' &&
          strnicmp(&file[strlen(file) - j], mimetable[i].ext, j) == NULL) {
            return (char*)mimetable[i].type;
        }
    }
    return (char*)mimetable[0].type;
}

int HTTPD::strnicmp (const char *p1, const char *p2, int n) {
    int i, r = -1;
    char c1, c2;
    
    for (i = 0; i < n; i ++) {
        c1 = (p1[i] >= 'a' && p1[i] <= 'z') ? p1[i] - ('a' - 'A'): p1[i];
        c2 = (p2[i] >= 'a' && p2[i] <= 'z') ? p2[i] - ('a' - 'A'): p2[i];
        r = c1 - c2;
        if (r) break;
    }
    return r;
}


/* base64encode code from 
 * Copyright (c) 2010 Donatien Garnier (donatiengar [at] gmail [dot] com)
 */
int HTTPD::base64encode (const char *input, int length, char *output, int len) {
    static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned int c, c1, c2, c3;
 
    if (len < ((((length-1)/3)+1)<<2)) return -1;
    for(unsigned int i = 0, j = 0; i<length; i+=3,j+=4) {
        c1 = ((((unsigned char)*((unsigned char *)&input[i]))));
        c2 = (length>i+1)?((((unsigned char)*((unsigned char *)&input[i+1])))):0;
        c3 = (length>i+2)?((((unsigned char)*((unsigned char *)&input[i+2])))):0;
 
        c = ((c1 & 0xFC) >> 2);
        output[j+0] = base64[c];
        c = ((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4);
        output[j+1] = base64[c];
        c = ((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6);
        output[j+2] = (length>i+1)?base64[c]:'=';
        c = (c3 & 0x3F);
        output[j+3] = (length>i+2)?base64[c]:'=';
    }
    output[(((length-1)/3)+1)<<2] = '\0';
    return 0;
}
 
 
/* urlencode code from 
 * Copyright (c) 2010 Donatien Garnier (donatiengar [at] gmail [dot] com)
 */
int HTTPD::urlencode (const char *str, char *buf, int len) {
//  char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
    const char *pstr = str;
    char *pbuf = buf;
 
    if (len < (strlen(str) * 3 + 1)) return -1;
    while (*pstr) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
            *pbuf++ = *pstr;
        else if (*pstr == ' ') 
            *pbuf++ = '+';
        else 
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return 0;
}
 
/* urldecode code from 
 * Copyright (c) 2010 Donatien Garnier (donatiengar [at] gmail [dot] com)
 */
int HTTPD::urldecode (const char *str, char *buf, int len) {
//  char *pstr = str, *buf = (char*)malloc(strlen(str) + 1), *pbuf = buf;
    const char *pstr = str;
    char *pbuf = buf;
 
    if (len < (strlen(str) / 3 - 1)) return -1;
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } else if (*pstr == '+') { 
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
    return 0;
}

int HTTPD::from_hex (int ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}
 
int HTTPD::to_hex (int code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}
