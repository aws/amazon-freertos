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
	if (ensure_memory_is_valid(pxNetworkBuffer)) {
		pxNetworkBuffer->pucEthernetBuffer = malloc(sizeof(TCPPacket_t));
	}
	return pxNetworkBuffer;
}

void harness() {
	FreeRTOS_Socket_t *pxSocket = ensure_FreeRTOS_Socket_t_is_allocated();
	if (ensure_memory_is_valid(pxSocket)) {
		pxSocket->u.xTCP.rxStream = ensure_FreeRTOS_StreamBuffer_t_is_allocated();
	}
	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	if (ensure_memory_is_valid(pxNetworkBuffer)) {
		pxNetworkBuffer->pucEthernetBuffer = malloc(sizeof(TCPPacket_t));
	}
	uint32_t ulLen;
	BaseType_t xReleaseAfterSend;
	__CPROVER_assume(pxSocket != NULL || pxNetworkBuffer != NULL); /* Both can not be NULL at the same time. */

	publicTCPReturnPacket(pxSocket, pxNetworkBuffer, ulLen, xReleaseAfterSend);

}
