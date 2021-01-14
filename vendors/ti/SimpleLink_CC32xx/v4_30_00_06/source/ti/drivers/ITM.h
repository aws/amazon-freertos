/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!*****************************************************************************
 *  @file       ITM.h
 *  @brief ITM driver header
 *
 *  @anchor ti_drivers_ITM_Overview
 *  <h3> Overview </h3>
 *  This driver implements APIs to configure and control the ARM Instrumentation
 *  Trace Macrocell (ITM), Debug Watchpoint and Trace (DWT), and  Trace Port
 *  Instrumentation Unit (TPIU) IPs to realize non-intrusive software logging
 *  and runtime trace.
 *
 *  # Overview <a name="overview"></a>
 *  The ITM software module provides application level APIs for the non-invasive
 *  debug capabilities of the ARM Cortex-M family. This includes the following
 *  hardware modules:
 *
 *   - Instrumentation Trace Macrocell (ITM)
 *   - Debug Watchpoint and Trace (DWT)
 *   - Trace Port Instrumentation Unit (TPIU)
 *
 *  At a high level, the DWT provides watchpoint, data trace, and program
 *  counter sampling. The ITM provides memory mapped registers for low-intrusion
 *  software profiling. The TPIU provides an external interface for the ITM and
 *  DWT. Further details can be found in the
 *  [ARMv7-M Architecture Reference Manual](https://static.docs.arm.com/ddi0403/e/DDI0403E_d_armv7m_arm.pdf)
 *
 *  <h3> Limitations and Constraints </h3>
 *  The driver is is designed with the following constraints in mind:
 *    - ITM is designed as a singleton. This means that  a single instance is
 *      shared across all clients
 *    - The parallel TracePort mode of the TPIU is not supported, only
 *      serial protocols are considered
 *    - The ITM hardware is configured via firmware on the DUT.
 *      Log records can be read independent of any IDE or debugger configuration.
 *      This means that if the IDE debug system configures the ITM, it should
 *      be turned off.
 *    - In Code Composer Studio, it is not currently possible to disable
 *      the debugger configuration mentioned above. No ITM output will be visible
 *      while the IDE's debugger is connected. Please flash your firmware, then
 *      disconnect and reset the device to see ITM data on the serial port.
 *
 *  @anchor ti_drivers_ITM_Setup
 *  <h3> Setup </h3>
 *  The ITM is configured via hardware attributes stored in the hardware
 *  attributes structure. This structure contains a common portion that is used
 *  for all drivers. This structure may be extended for some devices such as
 *  CC26XX.
 *
 *  <h3> Opening the driver </h3>
 *  Unlike other drivers, the ITM is intended to be a singleton.
 *  This means that @ref ITM_open can be called multiple times.
 *  The ITM will only be configured the first time open is called.
 *
 *  Furthermore, helper functions for features such as PC sampling must coherent
 *  across clients. The driver offers no protection against mismatched
 *  configuration.
 *
 *  The open call will enable ITM as well as the necessary stimulus ports.
 *  It will setup the TPIU for necessary baudrate and serial format.
 *
 *  @anchor ti_drivers_ITM_PinMux
 *  <h3> Pin Muxing </h3>
 *  As the ITM driver is primarily interfacing to ARM defined IP, it is almost
 *  entirely common across all supported devices. The only specifics are pin
 *  muxing of the SWO pin. See the table below for some notes

 *  | Device Family | Debug Protocol | Muxing          | Configurable? |
 *  |---------------|----------------|-----------------|---------------|
 *  | CC32XX        | SWD            | Shared with TDO | N             |
 *  | CC13XX/CC26XX | JTAG/cJTAG     | Any pin         | Y             |
 *  | MSP432E4      | SWD            | Shared with TDO | N             |
 *
 *  Device specific pin muxing is done by the device specific ITM backend
 *  implementation.
 *
 *  @anchor ti_drivers_ITM_SwMessages
 *  <h3> Software Messages </h3>
 *  The ITM stimulus ports enable serialization of application data with low
 *  overhead. There are multiple ports available, they are selectable via
 *  software.
 *
 *  Data written to the software stimulus ports is serialized by the TPIU
 *  and wrapped in the SWIT packet format. This packet format is standardized
 *  by ARM and described in [ARMv7-M Architecture Reference Manual](https://static.docs.arm.com/ddi0403/e/DDI0403E_d_armv7m_arm.pdf)
 *
 *  There are three tiers of access to the stimulus ports. In the table below,
 *  polled access means that the API/macro will poll the port's busy flag
 *  before writing. This is done to prevent silent data loss that
 *  occurs when writing to a port that is not ready. Actual serialization of the
 *  data will occur later inside the TPIU.
 *
 *    - ITM_PortX - Macro to write or read to the port, doesn't poll before
 *    - ITM_sendXPolling - Macro that polls and writes to the port
 *    - ITM_sendXAtomic - Function that calls ITM_sendXPolling with interrupts
 *                        disabled
 *
 *  It is up the the application writer to understand the tradeoff associated
 *  with each of these and select the correct one.
 *
 *  <h3> DWT Features </h3>
 *  The Data Watchpoint and Trace (DWT) module is capable of many
 *  instrumentation features such as
 *
 *  | Feature                  | ITM API                             |
 *  |--------------------------|-------------------------------------|
 *  | Exception trace          | @ref ITM_enableExceptionTrace       |
 *  | Program Counter sampling | @ref ITM_enablePCSampling           |
 *  | Event counting           | @ref ITM_enableEventCounter         |
 *  | Synchronization packets  | @ref ITM_enableSyncPackets          |
 *
 *
 *  Data generated by the DWT is serialized via the TPIU. DWT packet formats
 *  are defined in the ARMv7-M Architecture Reference Manual referenced above
 *
 *  <h3>Flush </h3>
 *  The ITM/DWT/TPIU hardware resides in the CPU power domain. This means that
 *  whenever the CPU domain is powered down, ITM will power down.
 *  Powering down when data is inside in the TPIU can result in lost data.
 *  In order to prevent dataloss, the device's power policy will flush the
 *  ITM before powering down the CPU domain. When returning from sleep, the
 *  power policy will restore the ITM. This is achieved using the
 *  @ref ITM_flush and @ref ITM_restore.
 *
 *  These functions are weakly defined as empty functions. This reduces the
 *  overhead in the power policy when ITM is not enabled. These weak functions
 *  are overridden by syscfg when ITM is enabled.
 *
 */

#ifndef ti_drivers_ITM__include
#define ti_drivers_ITM__include

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Base address of the Instrumentation Trace Macrocell (ITM) module
 *
 */
#define ITM_BASE_ADDR           (0xE0000000)

/**
 * @brief Base address of the Debug Watchpoint and Trace (DWT) module
 *
 */
#define ITM_DWT_BASE_ADDR       (0xE0001000)

/**
 * @brief Base address of the CPU_SCS module
 *
 */
#define ITM_SCS_BASE_ADDR       (0xE000E000)

/**
 * @brief Base address of the Trace Port Instrumentation Unit (TPIU) module
 *
 */
#define ITM_TPIU_BASE_ADDR      (0xE0040000)

/**
 * @brief   Value to unlock ARM debug modules. This value should be written to
 *          the Lock Access Registers (LAR) in order to enable their
 *          configuration and use
 *
 */
#define ITM_LAR_UNLOCK          (0xC5ACCE55)

/**
 * @brief Write a 32-bit word to stimulus port n
 *
 * @warning This does not does not first poll the port for availability.
 *
 */
#define ITM_port32(n) (*((volatile unsigned int *)  (ITM_BASE_ADDR +4*n)))

/* The do {} while() loops below are to protect the macros so that they're
 * evaluated correctly regardless of the call site. They do not have any
 * function besides ensuring that the syntax is preserved correctly by the pre
 * processor.
 */
/**
 * @brief Write a 32-bit word to stimulus port n with polling
 *
 * @warning This macro does not guarantee atomic access between poll and write
 *
 */
#define ITM_send32Polling(n, x)                                                \
    do                                                                         \
    {                                                                          \
        while(0 == ITM_port32(n));                                             \
        ITM_port32(n) = x;                                                     \
    } while(0)

/**
 * @brief Write a 16-bit half word to stimulus port n
 *
 * @warning This does not does not first poll the port for availability.
 */
#define ITM_port16(n) (*((volatile unsigned short *)(ITM_BASE_ADDR +4*n)))

/**
 * @brief Write a 16-bit word to stimulus port n with polling
 *
 * @warning This macro does not guarantee atomic access between poll and write
 *
 */
#define ITM_send16Polling(n, x)                                                \
    do                                                                         \
    {                                                                          \
        while(0 == ITM_port16(n));                                             \
        ITM_port16(n) = x;                                                     \
    } while(0)

/**
 * @brief Write a byte to stimulus port n
 *
 * @warning This does not does not first poll the port for availability.
 */
#define ITM_port8(n)  (*((volatile unsigned char *) (ITM_BASE_ADDR +4*n)))

/**
 * @brief Write a 8-bit word to stimulus port n with polling
 *
 * @warning This macro does not guarantee atomic access between poll and write
 *
 */
#define ITM_send8Polling(n, x)                                                 \
    do                                                                         \
    {                                                                          \
        while(0 == ITM_port8(n));                                              \
        ITM_port8(n) = x;                                                      \
    }while(0)


typedef enum
{
    ITM_TPIU_SWO_MANCHESTER     = 0x00000001, /*!< Serial format is manchester */
    ITM_TPIU_SWO_UART           = 0x00000002, /*!< Serial format is UART */
} ITM_TPIU_PortFormat;

/*! @cond NODOC */
/* This enables a common defintion of the hwAttrs structure that can be easily
 * extended by the device specific implementations. This structure defintion
 * must be the first line of any device specific structure
 */
#define ITM_BASE_HWATTRS                                                        \
    ITM_TPIU_PortFormat format;         /* Wire interface used by TPIU */       \
    uint32_t            traceEnable;    /* Bitmask of enabled stimulus ports */ \
    uint32_t            tpiuPrescaler;  /* Baudrate to be used by the TPIU */   \
    uint32_t            fullPacketInCycles; /* Cycles in a full word */         \
/*! @endcond */

/*!
 *  @brief      ITM Hardware Attributes
 *
 *  ITM hardware attributes should be included in the board file or syscfg
 *  generated file and provides constant hardware specific configuration.
 */
typedef struct
{
    ITM_BASE_HWATTRS
} ITM_HWAttrs;

/**
 * @brief Control the action taken by the DWT on comparator match
 */
typedef enum
{
    ITM_Disabled                        = 0x0, /*!< Disabled */
    ITM_EmitPc                          = 0x1, /*!< Emit Program Counter */
    ITM_EmitDataOnReadWrite             = 0x2, /*!< Emit Data on Read or Write */
    ITM_SamplePcAndEmitDataOnReadWrite  = 0x3, /*!< Emit Program Counter on Read or Write */
    ITM_SampleDataOnRead                = 0xC, /*!< Sample Data on Read */
    ITM_SampleDataOnWrite               = 0xD, /*!< Sample Data on Write */
    ITM_SamplePcAndDataOnRead           = 0xE, /*!< Sample PC and Data on Read */
    ITM_SamplePcAndDataOnWrite          = 0xF, /*!< Sample PC and Data on Write */
} ITM_WatchpointAction;

/**
 * @brief Prescaler for ITM timestamp generation based on the trace packet
 *        reference clock
 */
typedef enum
{
    ITM_TS_DIV_NONE = 0,    /*!< No division */
    ITM_TS_DIV_4    = 1,    /*!< Divide by 4 */
    ITM_TS_DIV_16   = 2,    /*!< Divide by 16 */
    ITM_TS_DIV_64   = 3     /*!< Divide by 64 */
} ITM_TimeStampPrescaler;
/**
 * @brief Synchronous packet generation rate based on cycles of CYCCNT
 *        This controls how often sync packets will be generated.
 *        The tap controls which bit transition in the counter triggers a packet
 */
typedef enum
{
    ITM_SYNC_NONE      = 0, /*!< Disabled */
    ITM_SYNC_TAP_BIT24 = 1, /*!< Tap the CYCCNT register at bit 24 */
    ITM_SYNC_TAP_BIT26 = 2, /*!< Tap the CYCCNT register at bit 26 */
    ITM_SYNC_TAP_BIT28 = 3  /*!< Tap the CYCCNT register at bit 28 */
} ITM_SyncPacketRate;

/**
 * @brief Open and configure the ITM, DWT, and TPIU. This includes muxing
 *        pins as needed
 *
 * @warning    On CC13xx/26xx this must be called from the task context.
 *             This is because PIN_open pends forever on a semaphore.
 *
 * @return true - The ITM was successfully configured and the pins were acquired
 * @return false - Pin muxing failed and the ITM was not configured
 */
extern bool ITM_open(void);

/**
 *  Disable the ITM, when it is no longer in use, it will be shutdown.
 *  This will also turn off any additional features that were enabled such as
 *  PC sampling or interrupt tracing
 *
 *  @sa ITM_open()
 */
extern void ITM_close(void);

/**
 * @brief Write the contents of a buffer to the stimulus port, polling to ensure
 *        the port is available
 *
 * @warning This API will disable interrupts for the entire buffer duration
 *          Use it cautiously to prevent negatively impacting interrupt latency
 *
 * @param port   The stimulus port to use
 * @param msg    Data to send.
 * @param length length of buffer in bytes
 */
extern void ITM_sendBufferAtomic(const uint8_t port, const char* msg,
                                 size_t length);

/**
 * @brief Write a 32-bit word to the given stimulus port, polling to ensure the
 *        port is available
 *
 * @param port   The stimulus port to use
 * @param value  The 32-bit value to write
 */
extern void ITM_send32Atomic(uint8_t port, uint32_t value);

/**
 * @brief Write a 16-bit short to the given stimulus port, polling to ensure the
 *        port is available
 *
 * @param port  The stimulus port to use
 * @param value The 16-bit value to write
 */
extern void ITM_send16Atomic(uint8_t port, uint16_t value);

/**
 * @brief Write an 8-bit byte to the given stimulus port, polling to ensure the
 *        port is available
 *
 * @param port  The stimulus port to use
 * @param value The 8-bit value to write
 */
extern void ITM_send8Atomic(uint8_t port, uint8_t value);

/**
 * @brief Enable exception tracing
 *        This will trigger the DWT to generate packets when the device
 *        enters or leaves an exception. The ITM will forward these packets
 *        to the TPIU to be sent to the debugger.
 *
 */
extern void ITM_enableExceptionTrace(void);

/**
 * @brief Disable exception tracing.
 *
 * @sa  ITM_enableExceptionTrace()
 *
 */
extern void ITM_disableExceptionTrace(void);

/**
 *
 * @brief Enable periodic sampling of the program counter using the DWT
 *        POSTCNT timer.
 *
 * @param prescale1024  true: divide system clock by 1024 to generate POSTCNT
 *                      false: divide system clock by 64 to generate POSTCNT
 * @param postReset     4-bit downcounter that is reloaded on POSTCNT expiration
 *
 * @note  PC sampling and event counting are mutally exclusive.
 *        It is not recommended to call @ref ITM_enableEventCounter()
 *        after this API has been called
 *
 */
extern void ITM_enablePCSampling(bool prescale1024, uint8_t postReset);

/**
 * @brief Enable generation of event counter packets using the DWT
 *        POSTCNT timer.
 *
 * @param prescale1024  true: divide system clock by 1024 to generate POSTCNT
 *                      false: divide system clock by 64 to generate POSTCNT
 * @param postReset     4-bit downcounter that is reloaded on POSTCNT expiration
 *
 * @note  PC sampling and event counting are mutally exclusive.
 *        It is not recommended to call @ref ITM_enablePCSampling()
 *        after this API has been called
 */
extern void ITM_enableEventCounter(bool prescale1024, uint8_t postReset);

/**
 * @brief Disable program counter and event sampling in the DWT
 *
 * @sa ITM_enablePCSampling(), ITM_enableEventCounter
 *
 */
extern void ITM_disablePCAndEventSampling();

/**
 * @brief Enable the generation of local timestamp packets from the ITM module
 *        These are packets sent form the ITM that measure how long it has been
 *        since the previous timestamp
 *
 * @param tsPrescale Prescaler value for the timestamp clock.
 * @param asyncMode true: Synchronous mode - generate timestamps by dividing
 *                         the system clock.
                    false: Asynchronous mode - generate timestamps by dividing
 *                         the TPIU clock.
 */
extern void ITM_enableTimestamps(ITM_TimeStampPrescaler tsPrescale,
                                 bool asyncMode);

/**
 * @brief Enable the generation of synchronization packets from the ITM
 *        based on the CYCCNT counter. Synchronization packets can be used
 *        to recover bit-to-byte alignment information in a serial data stream
 *
 * @warning  When using the TPIU as a asynchronous serial trace port
 *           ARM recommends disabling these packets to reduce stream bandwith.
 *
 * @param syncPacketRate Tap for CYCCNT. Controls the frequency of sync packets.
 */
extern void ITM_enableSyncPackets(ITM_SyncPacketRate syncPacketRate);

/**
 * @brief Enable the watchpoint functionality using a DWT comparator
 *
 * @param function The DWT_FUNCTION field to be programmed.
 *                 This controls what data is emitted on comparator match
 *                 see @ref ITM_WatchpointAction for possible values
 * @param address  The address for the comparator to match on
 * @return true    The watchpoint was successfully set
 * @return false   There was no watchpoint available (all are in use)
 */
extern bool ITM_enableWatchpoint(ITM_WatchpointAction function,
                                 const uintptr_t address);

/**
 * @brief Flush the ITM in preparation for power off of CPU domain
 *
 *        This will disable PC sampling, and other DWT features, flush the fifo
 *        and (if applicable) setup the PIN as a GPIO.
 *
 * @warning This function should not be called from ISR context
 *
 * @note  This function is invoked by the DPL power policy right before
 *        entering IDLE/DEEPSLEEP. By default it is an empty stub so that the
 *        power policy doesn't incur the flush overhead when ITM isn't used.
 *        When ITM is enabled this function will be strongly defined by SYSCFG.
 *
 */
extern void __attribute__((weak)) ITM_flush(void);

/**
 * @brief Prepare the ITM hardware to return from power off of CPU domain
 *        This will reenable DWT features, re apply the ITM pin mux
 *
 * @warning This function should not be called from ISR context
 *
 * @note  This function is invoked by the DPL power policy right after
 *        leaving IDLE/DEEPSLEEP. By default it is an empty stub so that the
 *        power policy doesn't incur the flush overhead when ITM isn't used.
 *        When ITM is enabled this function will be strongly defined by SYSCFG.
 */
extern void __attribute__((weak)) ITM_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ITM__include */
