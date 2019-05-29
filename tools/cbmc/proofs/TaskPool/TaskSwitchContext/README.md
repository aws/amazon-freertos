This proof demonstrates the memory safety of the TaskSwitchContext function.
We assume task ready lists to be initialized and filled with one element each,
and `pxCurrentTCB` to be the highest priority task.  We also set
`uxSchedulerSuspended` to a nondeterministic value.
