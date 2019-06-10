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

/* Abstraction of pxGetNetworkBufferWithDescriptor. It creates a buffer. */
NetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks ){
	NetworkBufferDescriptor_t *pxBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated ();
	size_t bufferSize = sizeof(NetworkBufferDescriptor_t);
	if (ensure_memory_is_valid(pxBuffer, bufferSize)) {
		/* The code does not expect pucEthernetBuffer to be equal to NULL if
		pxBuffer is not NULL. */
		pxBuffer->pucEthernetBuffer = malloc(xRequestedSizeBytes);
		pxBuffer->xDataLength = xRequestedSizeBytes;
	}
	return pxBuffer;
}

void harness() {
	FreeRTOS_Socket_t *pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	size_t socketSize = sizeof(FreeRTOS_Socket_t);
	size_t bufferSize = sizeof(NetworkBufferDescriptor_t);
	if (ensure_memory_is_valid(pxNetworkBuffer, sizeof(*pxNetworkBuffer))) {
		pxNetworkBuffer->xDataLength = bufferSize;
		/* The code does not expect pucEthernetBuffer to be equal to NULL if
		pxNetworkBuffer is not NULL. */
		pxNetworkBuffer->pucEthernetBuffer = malloc(bufferSize);
	}
	UBaseType_t uxOptionsLength;
	if(pxSocket) {
		publicTCPPrepareSend(pxSocket, &pxNetworkBuffer, uxOptionsLength );
	}
}
