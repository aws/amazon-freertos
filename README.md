## Amazon FreeRTOS 201908.00 Release ESP-IDF SMP Support

This special branch of Amazon FreeRTOS contains the modifications necessary to support Espressif's
fork of FreeRTOS bundles with ESP-IDF for the ESP32. That fork enables the ESP32's second CPU core
for FreeRTOS tasks by [extending the FreeRTOS
scheduler](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/freertos-smp.html). 

This branch replaces the FreeRTOS 10 kernel in AmazonFreeRTOS with the Espressif fork from ESP-IDF
3.1.1 Use this branch directly to build on Amazon FreeRTOS 201908.00, or apply these commits to
future versions of Amazon FreeRTOS.

The commit history is organized into the following types of changes: 

1. *AFR FIX UPSTREAM* - These changes are candidates for pushing into the Amazon FreeRTOS master
   repository
2. *COPY* - Copy the FreeRTOS sources from esp-idf
3. *LIBRARY PATCH* - Update the build system and flags for the new files
4. *ESP-IDF PATCH* - These changes are candidates for pushing into any future releases of ESP-IDF
   slated for integration with Amazon FreeRTOS
5. *CONFIG* - Merge AFR requirements into the ESP-IDF `FreeRTOSConfig.h`. Many of these support the
   demos. You may wish to modify these for your application.

There are changes to the scheduler and handling of critical sections from FreeRTOS. ESP-IDF requires
calls to `portENTER_CRITICAL()` to pass a mutex used to lock the other core. This port supports the
original FreeRTOS macro signature by providing a global mutex. Note though that the other core will
continue to run unless it enters a matching critical section.  Both of the
following forms are supported:

```
/* 
 * Scheduler and Interrupts are disabled on the local core. The other core will
 * run until it similarly calls portENTER_CRITICAL() 
 */
 
/* Explicit critical section lock on a shared resource */
static portMUX_TYPE xSubsystemMutex = portMUX_INITIALIZER_UNLOCKED;
portENTER_CRITICAL(&xSubsystemMutex);
 
/* Translated by the preprocessor into a lock on global_portmux */
portENTER_CRITICAL();

```

## Getting Started

For more information on Amazon FreeRTOS, refer to the [Getting Started section of Amazon FreeRTOS webpage](https://aws.amazon.com/freertos).

To directly access the **Getting Started Guide** for supported hardware platforms, click the corresponding link in the Supported Hardware section below.

For detailed documentation on Amazon FreeRTOS, refer to the [Amazon FreeRTOS User Guide](https://aws.amazon.com/documentation/freertos).

## Supported Hardware

The following MCU boards are supported for this branch of Amazon FreeRTOS:
1. **Espressif** - [ESP32-DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview), [ESP-WROVER-KIT](https://www.espressif.com/en/products/hardware/esp-wrover-kit/overview).

## amazon-freeRTOS/projects
The ```./projects``` folder contains the IDE test and demo projects for each vendor and their boards. The majority of boards can be built with both IDE and cmake (there are some exceptions!). Please refer to the Getting Started Guides above for board specific instructions.
