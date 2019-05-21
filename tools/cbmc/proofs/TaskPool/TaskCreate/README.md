This proof demonstrates the memory safety of the TaskCreate function.  We
assume task lists to be initialized, and nondet. set `pxCurrentTCB`,
`uxCurrentNumberOfTasks`, `pcName` and `pxCreateTask`. STACK_DEPTH is set to a
fixed number (10) since it is not possible to specify a range.
