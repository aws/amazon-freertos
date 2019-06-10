/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "FreeRTOS_Stream_Buffer.h"

#include "../../utility/memory_assignments.c"

/* This proof assumes that pxGetNetworkBufferWithDescriptor is implemented correctly. */
int32_t publicTCPPrepareSend( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, UBaseType_t uxOptionsLength );

/* Abstraction of pxGetNetworkBufferWithDescriptor */
NetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks ){
	NetworkBufferDescriptor_t *pxBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated ();
	if (ensure_memory_is_valid(pxBuffer)) {
		pxBuffer->pucEthernetBuffer = malloc(xRequestedSizeBytes);
		pxBuffer->xDataLength = xRequestedSizeBytes;
	}
	return pxBuffer;
}

void harness() {
	FreeRTOS_Socket_t *pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	if (ensure_memory_is_valid(pxNetworkBuffer)) {
		pxNetworkBuffer->pucEthernetBuffer = malloc(sizeof(TCPPacket_t));
		__CPROVER_assume(pxNetworkBuffer->xDataLength == sizeof(TCPPacket_t));
	}
	UBaseType_t uxOptionsLength;
	if(ensure_memory_is_valid(pxSocket)) {
		pxSocket->u.xTCP.rxStream = ensure_FreeRTOS_StreamBuffer_t_is_allocated();
		pxSocket->u.xTCP.txStream = ensure_FreeRTOS_StreamBuffer_t_is_allocated();
		pxSocket->u.xTCP.pxPeerSocket = ensure_FreeRTOS_Socket_t_is_allocated();
		publicTCPPrepareSend(pxSocket, &pxNetworkBuffer, uxOptionsLength );

	}
}
