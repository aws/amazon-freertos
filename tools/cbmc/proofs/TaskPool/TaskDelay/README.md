This proof demonstrates the memory safety of the TaskDelay function.  We assume
that `pxCurrentTCB` is initialized and inserted in one of the ready tasks lists
(with and without another task in the same list). We abstract function
`xTaskResumeAll` by assuming that `xPendingReadyList` is empty and
`uxPendedTicks` is `0`. Finally, we assume nondeterministic values for global
variables `xTickCount` and `xNextTaskUnblockTime`, and `pdFALSE` for
`uxSchedulerSuspended` (to avoid assertion failure).

Configurations available:
 * `default`: The default configuration.  `useTickHook1`: The default
 * configuration with `INCLUDE_vTaskSuspend=0`
