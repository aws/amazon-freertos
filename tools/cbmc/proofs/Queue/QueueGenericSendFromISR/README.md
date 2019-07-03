The harness in this folder proves the memory safety of QueueGenericSendFromISR
with and without QueueSets. It is abstracting away the task pool and concurrency
related functions. Further, it uses a stub for prvCopyDataToQueue.
