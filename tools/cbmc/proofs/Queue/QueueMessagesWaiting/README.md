Assuming the parameter passed to QueueMessagesWaiting is a pointer to a Queue_t
struct, this harness proves the memory safety of QueueMessagesWaiting.
The concurrency related functions vPortEnterCrititcal and vPortExitCritical
are abstracted away.
