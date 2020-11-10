#line __LINE__ "net_Socket.c"
/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/
#include "../../coap.h"

typedef struct {
	CoAP_Socket_t SocketMemory[MAX_ACTIVE_SOCKETS];
	bool initDone;
} CoAP_SocketCtrl_t;

static CoAP_SocketCtrl_t SocketCtrl = {.initDone = false};

CoAP_Socket_t* _rom AllocSocket() {
	int i;
	if (!SocketCtrl.initDone) {
		for (i = 0; i < MAX_ACTIVE_SOCKETS; i++) {
			SocketCtrl.SocketMemory[i].Alive = false;
		}
		SocketCtrl.initDone = true;
	}

	for (i = 0; i < MAX_ACTIVE_SOCKETS; i++) {
		CoAP_Socket_t* socket = &(SocketCtrl.SocketMemory[i]);
		if (socket->Alive == false) {
			memset(socket, 0, sizeof(*socket));
			socket->Alive = true;
			return socket;
		}
	}

	return NULL; //no free memory
}

CoAP_Socket_t* _rom RetrieveSocket(Socket_t handle) {
	int i;
	for (i = 0; i < MAX_ACTIVE_SOCKETS; i++) {
		if (SocketCtrl.SocketMemory[i].Alive &&
			SocketCtrl.SocketMemory[i].Handle == handle) //corresponding socket found!
		{
			return &(SocketCtrl.SocketMemory[i]);
		}

	}
	return NULL; //not found
}
