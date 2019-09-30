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
	size_t socketSize = sizeof(FreeRTOS_Socket_t);
	if (ensure_memory_is_valid(pxSocket, socketSize)) {
		/* ucOptionLength is the number of valid bytes in ulOptionsData[].
		ulOptionsData[] is initialized as uint32_t ulOptionsData[ipSIZE_TCP_OPTIONS/sizeof(uint32_t)].
		This assumption is required for a memcpy function that copies uxOptionsLength
		data from pxTCPHeader->ucOptdata to pxTCPWindow->ulOptionsData.*/
		__CPROVER_assume(pxSocket->u.xTCP.xTCPWindow.ucOptionLength == sizeof(uint32_t) * ipSIZE_TCP_OPTIONS/sizeof(uint32_t));
		/* uxRxWinSize is initialized as size_t. This assumption is required to terminate `while(uxWinSize > 0xfffful)` loop.*/
		__CPROVER_assume(pxSocket->u.xTCP.uxRxWinSize >= 0 && pxSocket->u.xTCP.uxRxWinSize <= sizeof(size_t));
		/* uxRxWinSize is initialized as uint16_t. This assumption is required to terminate `while(uxWinSize > 0xfffful)` loop.*/
		__CPROVER_assume(pxSocket->u.xTCP.usInitMSS == sizeof(uint16_t));
	}

	NetworkBufferDescriptor_t *pxNetworkBuffer = ensure_FreeRTOS_NetworkBuffer_is_allocated();
	size_t bufferSize = sizeof(NetworkBufferDescriptor_t);
	if(ensure_memory_is_valid(pxNetworkBuffer, bufferSize)) {
		pxNetworkBuffer->pucEthernetBuffer = safeMalloc(sizeof(TCPPacket_t));
	}
	if (ensure_memory_is_valid(pxSocket, socketSize) &&
		ensure_memory_is_valid(pxNetworkBuffer, bufferSize) &&
		ensure_memory_is_valid(pxNetworkBuffer->pucEthernetBuffer, sizeof(TCPPacket_t)) &&
		ensure_memory_is_valid(pxSocket->u.xTCP.pxPeerSocket, socketSize)) {

			publicTCPHandleState(pxSocket, &pxNetworkBuffer);

	}
}
