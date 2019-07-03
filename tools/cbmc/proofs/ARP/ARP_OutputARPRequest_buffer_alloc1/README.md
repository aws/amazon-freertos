The proofs in this directory address the memory safety of the FreeRTOS_OutputARPRequest
method combined with the BufferAllocation_1.c allocation strategy.

The proofs are in more detail:

Abstracting away vPortEnterCritical, vPortExitCritical, vAssertCalled,
xTaskGetSchedulerState, pvTaskIncrementMutexHeldCount, xTaskRemoveFromEventList,
vPortGenerateSimulatedInterrupt, xTaskPriorityDisinherit by leaving out the implementations.

Disabling the tracing library in the header.

Given that vNetworkInterfaceAllocateRAMToBuffers, pvPortMalloc, xNetworkInterfaceOutput are
memory save and stick with the assumptions postulated in the harness:

The config_minimal_configuration proof states that the implementation
and interaction between FreeRTOS_OutputARPRequest and
FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.c is memory save.
This proof depends entirely of the implementation
correctness of vNetworkInterfaceAllocateRAMToBuffers.

The minimal_configuration_minimal_packet_size reminds
on the importance of the implementation correctness of the vNetworkInterfaceAllocateRAMToBuffers for the proof.
Further, it serves as a reminder that the ipconfigETHERNET_MINIMUM_PACKET_BYTES
can only be used in alignment with the implementation of vNetworkInterfaceAllocateRAMToBuffers.
The current model used in the proof for vNetworkInterfaceAllocateRAMToBuffers
does not work together with ipconfigETHERNET_MINIMUM_PACKET_BYTES.

The config_minimal_configuration_linked_rx_messages proof guarantees
that the ipconfigUSE_LINKED_RX_MESSAGES define does not interfere with
the memory safety claim.

All harnesses include the queue.c file, but test only for the happy path.
