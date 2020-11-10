#line __LINE__ "liblobaro_coap.c"
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

#include "liblobaro_coap.h"
#include "coap.h"
#include "coap_main.h"
#include "portmacro.h"

static const TickType_t xReceiveTimeOut = pdMS_TO_TICKS(40000);
static const TickType_t xSendTimeOut = pdMS_TO_TICKS(40000);


NetTransmit_fn CoAP_Send_Wifi(Socket_t socketHandle, NetPacket_t *pckt) {
	NetTransmit_fn result = false;
	BaseType_t xTransmitted;


	xTransmitted = SOCKETS_Send(socketHandle, /* The socket being sent to. */
	(void*) pckt->pData, /* The data being sent. */
	pckt->size, /* The length of the data being sent. */
	0); /* No flags. */

	if (xTransmitted < 0) {
		/* Error? */
		configPRINTF(( "ERROR - Failed to send CoAP packet\r\n" ));
	} else {

		result = true;
		configPRINTF(( "CoAP Packet Sent\r\n" ));
	}

	return (result);
}
void _ram	CoAP_Recv_Wifi(Socket_t socketHandle, NetPacket_t* pPacket,const NetEp_t ServerEp){
	/* Clear the buffer into which the echoed string will be
			 * placed. */
			unsigned char pcReceivedString[200];
			BaseType_t xReturned;
			xReturned = SOCKETS_Recv(socketHandle, /* The socket being received from. */
			&(pcReceivedString[0]), /* The buffer into which the received data will be written. */
			200, /* The size of the buffer provided to receive the data. */
			0);
			if (xReturned < 0) {
				/* Error occurred.  Latch it so it can be detected
				 * below. */
				configPRINTF(
						( "ERROR: Receiving from CoAP server %d bytes\r\n",xReturned ));
				return;
			} else if (xReturned == 0) {
				/* Timed out. */
				configPRINTF(( "Timed out receiving from CoAP server\r\n" ));
				return;
			}
			pPacket->pData = pvPortMalloc(sizeof(uint8_t) * xReturned);
			pPacket->pData = pcReceivedString;
			pPacket->size = xReturned;
			// The remote EndPoint is either the sender for incoming packets or the receiver for outgoing packets
			pPacket->remoteEp = ServerEp;

			CoAP_HandleIncomingPacket(socketHandle, pPacket);
}

