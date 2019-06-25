The harness and configurations in this folder show memory safety of
QueueGenericCreate, given the assumption made in the harness.

The principal assumption is that (uxItemSize * uxQueueLength) + sizeof(Queue_t)
does not overflow.
