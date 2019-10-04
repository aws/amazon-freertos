/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"

#include "../../utility/memory_assignments.c"

/* This proof assumes that pxDuplicateNetworkBufferWithDescriptor is implemented correctly. */

void publicTCPReturnPacket( FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulLen, BaseType_t xReleaseAfterSend );

/* Abstraction of pxDuplicateNetworkBufferWithDescriptor*/
NetworkBufferDescriptor_t *pxDuplicateNetworkBufferWithDescriptor( NetworkBufferDescriptor_t * const pxNetworkBuffer,
	BaseType_t xNewLength ) {
	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	if (ensure_memory_is_valid(pxNetworkBuffer, sizeof(*pxNetworkBuffer))) {
		pxNetworkBuffer->pucEthernetBuffer = safeMalloc(sizeof(TCPPacket_t));
		__CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer);
	}
	return pxNetworkBuffer;
}

void harness() {
	FreeRTOS_Socket_t *pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	if (ensure_memory_is_valid(pxNetworkBuffer, sizeof(*pxNetworkBuffer))) {
		pxNetworkBuffer->pucEthernetBuffer = safeMalloc(sizeof(TCPPacket_t));
		__CPROVER_assume(pxNetworkBuffer->pucEthernetBuffer);
	}
	uint32_t ulLen;
	BaseType_t xReleaseAfterSend;
	/* The code does not expect both of these to be equal to NULL at the same time. */
	__CPROVER_assume(pxSocket != NULL || pxNetworkBuffer != NULL);
	publicTCPReturnPacket(pxSocket, pxNetworkBuffer, ulLen, xReleaseAfterSend);
}
