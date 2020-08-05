### Cypress RTOS Abstraction Library
The RTOS Abstraction APIs allow middleware to be written to be RTOS aware, but not need to care about what the actual RTOS is.

### What's Included?
The this release of the RTOS Abstraction API includes support for the following:
* APIs for interacting with common RTOS Features including:
    * Threads
    * Mutexes
    * Semaphores
    * Timers
    * Queues
    * Events
* Implementations are provided for
    * FreeRTOS
    * RTX (CMSIS RTOS)
    * ThreadX

### What Changed?
#### v1.3.0
* TBD
#### v1.2.0
* Added utility library for for creating background worker threads: cy_worker_thread.h
* Added support for getting a semaphore's count: cy_rtos_get_count_semaphore()
* Added support for non-recursive mutexes: cy_rtos_init_mutex2()
* Added default implementations for FreeRTOS vApplicationGetIdleTaskMemory, vApplicationGetTimerTaskMemory, and vApplicationSleep
* Added support for ThreadX
NOTE: The vApplicationSleep implementation for FreeRTOS depends on the psoc6hal 1.2.0 or later.
#### v1.1.0
* Fixed an issue with the FreeRTOS implementation where it would always allocate memory for the thread stack, even if a pre-allocated buffer was provided.
* Removed usage of assert() and replaced with CY_ASSERT()
#### v1.0.1
* Added a new function to get the ID of the currently running thread.
* A few minor updates to avoid possible warnings depending on compiler.
* Minor documentation updates
#### v1.0.0
* Initial release supporting FreeRTOS & RTX

### Supported Software and Tools
This version of the RTOS Abstraction API was validated for compatibility with the following Software and Tools:

| Software and Tools                        | Version |
| :---                                      | :----:  |
| ModusToolbox Software Environment         | 2.0     |
| GCC Compiler                              | 7.4     |
| IAR Compiler                              | 8.32    |
| ARM Compiler                              | 6.11    |


### More information
Use the following links for more information, as needed:
* [API Reference Guide](https://cypresssemiconductorco.github.io/abstraction-rtos/html/modules.html)
* [Cypress Semiconductor](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
© Cypress Semiconductor Corporation, 2019-2020.