/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_ARP.h"


ARPPacket_t xARPPacket;
NetworkBufferDescriptor_t xNetworkBuffer;

/* STUB!
 * We assume that the pxGetNetworkBufferWithDescriptor function is
 * implemented correctly and returns a valid data structure.
 * This is the mock to mimic the expected behavior.
 * If this allocation fails, this might invalidate the proof.
 * FreeRTOS_OutputARPRequest calls pxGetNetworkBufferWithDescriptor
 * to get a NetworkBufferDescriptor. Then it calls vARPGenerateRequestPacket
 * passing this buffer along in the function call. vARPGenerateRequestPacket
 * casts the pointer xNetworkBuffer.pucEthernetBuffer into an ARPPacket_t pointer
 * and writes a complete ARPPacket to it. Therefore the buffer has to be at least of the size
 * of an ARPPacket to gurantee memory safety.
 */
NetworkBufferDescriptor_t *pxGetNetworkBufferWithDescriptor( size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks ){
	#ifdef CBMC_PROOF_ASSUMPTION_HOLDS
		#ifdef ipconfigETHERNET_MINIMUM_PACKET_BYTES
			xNetworkBuffer.pucEthernetBuffer = malloc(ipconfigETHERNET_MINIMUM_PACKET_BYTES);
		#else
			xNetworkBuffer.pucEthernetBuffer = malloc(xRequestedSizeBytes);
		#endif
	#else
		uint32_t malloc_size;
		__CPROVER_assert(!__CPROVER_overflow_mult(2, xRequestedSizeBytes));
		__CPROVER_assume(malloc_size > 0 && malloc_size < 2 * xRequestedSizeBytes);
		xNetworkBuffer.pucEthernetBuffer = malloc(malloc_size);
	#endif
	xNetworkBuffer.xDataLength = xRequestedSizeBytes;
	return &xNetworkBuffer;
}


void harness()
{
	uint32_t ulIPAddress;
	FreeRTOS_OutputARPRequest( ulIPAddress );
}
