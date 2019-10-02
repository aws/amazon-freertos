This is the memory safety proof for FreeRTOS_OutputARPRequest
method combined with the BufferAllocation_1.c allocation strategy.

This proof is a work-in-progress.  Proof assumptions are described in
the harness.  The proof also assumes the following functions are
memory safe and have no side effects relevant to the memory safety of
this function:

* vPortEnterCritical
* vPortExitCritical
* vPortGenerateSimulatedInterrupt
* vAssertCalled
* xTaskGetSchedulerState
* pvTaskIncrementMutexHeldCount
* xTaskRemoveFromEventList
* xTaskPriorityDisinherit

This proof checks FreeRTOS_OutputARPRequest in multiple configuration:

* The config_minimal_configuration proof states that the
  implementation and interaction between FreeRTOS_OutputARPRequest and
  FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.c
  is memory save.  This proof depends entirely of the implementation
  correctness of vNetworkInterfaceAllocateRAMToBuffers.
* The minimal_configuration_minimal_packet_size reminds on the
  importance of the implementation correctness of the
  vNetworkInterfaceAllocateRAMToBuffers for the proof.  Further, it
  serves as a reminder that the ipconfigETHERNET_MINIMUM_PACKET_BYTES
  can only be used in alignment with the implementation of
  vNetworkInterfaceAllocateRAMToBuffers.  The current model used in
  the proof for vNetworkInterfaceAllocateRAMToBuffers does not work
  together with ipconfigETHERNET_MINIMUM_PACKET_BYTES.
* The config_minimal_configuration_linked_rx_messages proof guarantees
  that the ipconfigUSE_LINKED_RX_MESSAGES define does not interfere
  with the memory safety claim.

All harnesses include the queue.c file, but test only for the happy path.
