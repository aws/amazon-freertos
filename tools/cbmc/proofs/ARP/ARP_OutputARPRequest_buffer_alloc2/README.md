The proofs in this directory address the memory safety of the FreeRTOS_OutputARPRequest
method combined with the BufferAllocation_2.c allocation strategy.

The proofs are in more detail:

Abstracting away vPortEnterCritical, vPortExitCritical, vAssertCalled,
xTaskGetSchedulerState, pvTaskIncrementMutexHeldCount, xTaskRemoveFromEventList,
vPortGenerateSimulatedInterrupt, xTaskPriorityDisinherit by leaving out the implementations.

Disabling the tracing library in the header.

Given that vNetworkInterfaceAllocateRAMToBuffers, pvPortMalloc,
pvPortFree, xNetworkInterfaceOutput are memory save and stick with
the assumptions modeled in the harness:

The proof in the directory config_minimal_configuration guarantees that the implementation
and interaction between FreeRTOS_OutputARPRequest and
FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c are memory save.
This proof depends entirely of the implementation
correctness of vNetworkInterfaceAllocateRAMToBuffers.

The proof in directory minimal_configuration_minimal_packet_size guarantees
that using FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c
along with the ipconfigETHERNET_MINIMUM_PACKET_BYTES is memory save
as long as TCP is enabled ( ipconfigUSE_TCP 1 ) and
ipconfigETHERNET_MINIMUM_PACKET_BYTES < sizeof( TCPPacket_t ).

The directory minimal_configuration_minimal_packet_size_no_tcp reminds
that ipconfigETHERNET_MINIMUM_PACKET_BYTES must not be used
if TCP is disabled ( ipconfigUSE_TCP 1 ) along with
the FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_2.c allocator.

The proof in directory config_minimal_configuration_linked_rx_messages guarantees
that the ipconfigUSE_LINKED_RX_MESSAGES define does not interfere with
the memory safety claim.

All harnesses include the queue.c file, but test only for the happy path.
