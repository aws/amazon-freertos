The harness proves memory safety of
QueueGenericCreateStatic under the assumption made in the harness.

The principal assumption is that (uxItemSize * uxQueueLength) + sizeof(Queue_t)
does not overflow. Further, ucQueueStorage must only be null iff uxItemSize is null.
In addition, the passed queue storage is assumed to be allocated to the right size.

The configurations for configSUPPORT_DYNAMIC_ALLOCATION set to 0 and 1 are checked.
