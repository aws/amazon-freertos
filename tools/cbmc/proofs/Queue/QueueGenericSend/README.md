The harness in this folder proves the memory safety of QueueGenericSend
with and without QueueSets. It is abstracting away the task pool and concurrency
related functions and assumes the parameters to be initialized to valid data structures.
Further, prvCopyDataToQueue, prvUnlockQueue and prvNotifyQueueSetContainer are abstracted away.
