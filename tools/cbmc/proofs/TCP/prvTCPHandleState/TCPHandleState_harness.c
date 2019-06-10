/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"

#include "../../utility/memory_assignments.c"

/* This proof assumes that prvTCPPrepareSend and prvTCPReturnPacket are correct.
These functions are proved to be correct separately. */

BaseType_t publicTCPHandleState( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer );

void harness() {
	FreeRTOS_Socket_t *pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
	if (ensure_memory_is_valid(pxSocket)) {
		pxSocket->u.xTCP.rxStream = ensure_FreeRTOS_StreamBuffer_t_is_allocated();
		pxSocket->u.xTCP.txStream = ensure_FreeRTOS_StreamBuffer_t_is_allocated();
		pxSocket->u.xTCP.pxPeerSocket = ensure_FreeRTOS_Socket_t_is_allocated();
		__CPROVER_assume(pxSocket->u.xTCP.xTCPWindow.ucOptionLength == sizeof(uint32_t) * ipSIZE_TCP_OPTIONS/sizeof(uint32_t));
		__CPROVER_assume(pxSocket->u.xTCP.uxRxWinSize >= 0 && pxSocket->u.xTCP.uxRxWinSize <= sizeof(size_t));
		__CPROVER_assume(pxSocket->u.xTCP.usInitMSS == sizeof(uint16_t));
	}

	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	if(ensure_memory_is_valid(pxNetworkBuffer)) {
		pxNetworkBuffer->pucEthernetBuffer = safeMalloc(sizeof(TCPPacket_t));
	}
	if (ensure_memory_is_valid(pxSocket) && ensure_memory_is_valid(pxNetworkBuffer) &&
		ensure_memory_is_valid(pxNetworkBuffer->pucEthernetBuffer)) {
		publicTCPHandleState(pxSocket, &pxNetworkBuffer);

	}
}
