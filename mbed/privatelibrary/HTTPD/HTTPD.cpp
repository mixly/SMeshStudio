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

HTTPD * HTTPD::_inst;

HTTPD::HTTPD () {
    _inst = this;
    memset(_state, 0, sizeof(_state));
    _handler_count = 0;
}

int HTTPD::start (int port) {
    int i;

    for (i = 0; i < HTTPD_MAX_CLIENTS; i ++) {
        _state[i].buf = new CircBuffer<char>(HTTPD_BUF_SIZE);
        _state[i].thread = new Thread(child, (void*)i, osPriorityNormal, HTTPD_STACK_SIZE);
        _state[i].client = new TCPSocketConnection;
    }
#ifdef HTTPD_ENABLE_CLOSER
    _state[HTTPD_MAX_CLIENTS].thread = new Thread(closer, (void*)HTTPD_MAX_CLIENTS, osPriorityNormal, 128);
    _state[HTTPD_MAX_CLIENTS].client = new TCPSocketConnection;
#endif

    _server.bind(port);
    _server.listen();
    _daemon = new Thread(daemon, NULL, osPriorityNormal, 256);
    return 0;
}

void HTTPD::daemon (void const *args) {
    HTTPD *httpd = HTTPD::getInstance();
    int i, t = 0;

    INFO("Wait for new connection...\r\n");
    for (;;) {
        if (t >= 0) {
            if (httpd->_server.accept(*httpd->_state[t].client) == 0) {
                INFO("accept %d\r\n", t);
                httpd->_state[t].thread->signal_set(1);
            }
        } else {
#ifdef HTTPD_ENABLE_CLOSER
            if (httpd->_server.accept(*httpd->_state[HTTPD_MAX_CLIENTS].client) == 0) {
                INFO("accept x\r\n");
                httpd->_state[HTTPD_MAX_CLIENTS].thread->signal_set(1);
            }
#endif
        }

        t = -1;
        for (i = 0; i < HTTPD_MAX_CLIENTS; i ++) {
            if (httpd->_state[i].thread->get_state() == Thread::WaitingAnd) {
                if (t < 0) t = i; // next empty thread
            }
        }
    }
}

void HTTPD::child (void const *arg) {
    HTTPD *httpd = HTTPD::getInstance();
    int id = (int)arg;
    int i, n;
    char buf[HTTPD_BUF_SIZE];

    for (;;) {
        Thread::signal_wait(1);

        httpd->_state[id].mode = MODE_REQUEST;
        httpd->_state[id].buf->flush();
        httpd->_state[id].keepalive = 0;
        INFO("Connection from %s\r\n", httpd->_state[id].client->get_address());
        httpd->_state[id].client->set_blocking(false, HTTPD_TIMEOUT);
        for (;;) {
            if (! httpd->_state[id].client->is_connected()) break;

            n = httpd->_state[id].client->receive(buf, sizeof(buf));
            if (n < 0) break;
            buf[n] = 0;
//            DBG("Recv %d '%s'", n, buf);

            for (i = 0; i < n; i ++) {
                httpd->recvData(id, buf[i]);
            }
        }

        httpd->_state[id].client->close();
        INFO("Close %s\r\n", httpd->_state[id].client->get_address());
    }
}

void HTTPD::closer (void const *arg) {
    HTTPD *httpd = HTTPD::getInstance();
    int id = (int)arg;

    for (;;) {
        Thread::signal_wait(1);

        httpd->_state[id].client->close();
        INFO("Close %s\r\n", httpd->_state[id].client->get_address());
    }
}

