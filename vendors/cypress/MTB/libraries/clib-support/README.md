# CLib FreeRTOS Support

## Overview

The CLib FreeRTOS Support Library provides the necessary hooks to make C library functions such as malloc and free thread safe. This implementation is specific to FreeRTOS and requires it to be present to build. For details on what this library provides see the toolchain specific documentation at:
* ARM: https://developer.arm.com/docs/100073/0614/the-arm-c-and-c-libraries/multithreaded-support-in-arm-c-libraries/management-of-locks-in-multithreaded-applications
* GCC: https://sourceware.org/newlib/libc.html#g_t_005f_005fmalloc_005flock
* IAR: http://supp.iar.com/filespublic/updinfo/011261/arm/doc/EWARM_DevelopmentGuide.ENU.pdf

The startup code must call cy_toolchain_init (for GCC and IAR). This must occur after static data initialization and before static constructors. This is done automatically for PSoC devices. See the PSoC startup files for an example.

To enable Thread Local Storage, configUSE_NEWLIB_REENTRANT must be enabled.

While this is specific to FreeRTOS, it can be used as a basis for supporting other RTOSes as well.

## Requirements
To use this library, the following configuration options must be enabled in FreeRTOSConfig.h:
* configUSE_MUTEXES
* configUSE_RECURSIVE_MUTEXES
* configSUPPORT_STATIC_ALLOCATION

When building with IAR, the '--threaded_lib' argument must also be provided when linking. This is done automatically with psoc6make 1.3.1 and later.

## Features
* GCC Newlib implementations for:
    * _sbrk
    * __malloc_lock
    * __malloc_unlock
    * __env_lock
    * __env_unlock
* ARM C library implementations for:
    * _platform_post_stackheap_init
    * __user_perthread_libspace
    * _mutex_initialize
    * _mutex_acquire
    * _mutex_release
    * _mutex_free
    * _sys_exit
    * $Sub$$_sys_open
    * _ttywrch
    * _sys_command_string
* IAR C library implementations for:
    * __aeabi_read_tp
    * _reclaim_reent
    * __iar_system_Mtxinit
    * __iar_system_Mtxlock
    * __iar_system_Mtxunlock
    * __iar_system_Mtxdst
    * __iar_file_Mtxinit
    * __iar_file_Mtxlock
    * __iar_file_Mtxunlock
    * __iar_file_Mtxdst
    * __iar_Initdynamiclock
    * __iar_Lockdynamiclock
    * __iar_Unlockdynamiclock
    * __iar_Dstdynamiclock
    * __close
    * __lseek
    * remove
* C++ implementations for:
    * __cxa_guard_acquire
    * __cxa_guard_abort
    * __cxa_guard_release

## More information
* [Cypress Semiconductor](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
© Cypress Semiconductor Corporation, 2020.
