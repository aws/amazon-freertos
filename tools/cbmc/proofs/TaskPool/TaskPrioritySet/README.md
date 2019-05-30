This proof demonstrates the memory safety of the TaskPrioritySet function.  The
initialization for the task to be set and `pxCurrentTCB` is quite similar
(since the task handle may be NULL, and in that case `pxCurrentTCB` is used).
Task lists are initialized with these tasks and nondet. filled with a few more
items. 
