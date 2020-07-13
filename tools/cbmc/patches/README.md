This directory includes patches to FreeRTOS required to run the CBMC proofs.

* One patch may no longer be required:
    * 0002-Change-FreeRTOS_IP_Private.h-union-to-struct.patch

* One patch is needed to avoid false positives:
    * 0005-Remove-volatile-qualifier-from-tasks-variables.patch

* One patch is needed to expose an encapsulated data structure:
    * remove-static-in-freertos-tcp-win.patch
