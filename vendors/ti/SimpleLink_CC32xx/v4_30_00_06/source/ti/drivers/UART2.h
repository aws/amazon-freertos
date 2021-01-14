/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 *  @file       UART2.h
 *  @brief      <b>PRELIMINARY</b> UART driver interface
 *
 *  <b>WARNING</b> These APIs are <b>PRELIMINARY</b>, and subject to
 *  change in the next few months.
 *
 *  The UART2 driver is an updated version of the UART driver.  The name
 *  UART2 was given due to changes in the API, to support backwards
 *  compatibility with applications using the existing UART driver.
 *  Key differences between the UART and UART2 drivers:
 *      - UART2 has both RX and TX ring buffers for receiving/sending data.
 *      - UART2 uses DMA to transfer data between the UART FIFOs and the
 *        RX and TX ring buffers.
 *      - The UART2 APIs for reading and writing data have been made more
 *        posix-like.
 *      - UART2 provides for event notification, allowing the application
 *        to receive TX start and completion events, and RX error events.
 *
 *  To use the UART2 driver, ensure that the correct driver library for your
 *  device is linked in and include this header file as follows:
 *  @code
 *  #include <ti/drivers/UART2.h>
 *  @endcode
 *
 *  This module serves as the main interface for applications.  Its purpose
 *  is to implement common code between the device specific implementations
 *  of UART2. Any device specific code that differs from the common code is
 *  called through functions prefaced with the "UART2_" naming convention.
 *  These functions are implemented in each device specific implementation.
 *
 *  @anchor ti_drivers_UART2_Overview
 *  # Overview
 *  A UART is used to translate data between the chip and a serial port.
 *  The UART2 driver simplifies reading and writing to any of the UART
 *  peripherals on the board, with multiple modes of operation and performance.
 *  These include blocking and non-blocking modes.
 *
 *  The UART2 driver interface provides device independent APIs, data types,
 *  and macros. The APIs in this driver serve as an interface to a typical RTOS
 *  application. The specific peripheral implementations are responsible for
 *  creating all the RTOS specific primitives to allow for thread-safe
 *  operation.
 *
 *  <hr>
 *  @anchor ti_drivers_UART2_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_UART2_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_UART2_Examples "examples"
 *  in the form of commented code fragments.  Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_UART2_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_UART2_Synopsis_Code
 *  @code
 *  // Import the UART2 driver definitions
 *  #include <ti/drivers/UART2.h>
 *
 *  // Initialize UART2 parameters
 *  UART2_Params params;
 *  UART2_Params_init(&params);
 *  params.baudRate = 9600;
 *  params.readMode = UART2_Mode_BLOCKING;
 *  params.writeMode = UART2_Mode_BLOCKING;
 *
 *  // Open the UART
 *  UART2_Handle uart;
 *  uart = UART2_open(CONFIG_UART0, &params);
 *
 *  // Enable receiver, inhibit low power mode
 *  UART2_rxEnable(uart);
 *
 *  // Read from the UART.
 *  size_t  bytesRead;
 *  uint8_t buffer[BUFSIZE];
 *  int32_t status;
 *  status = UART2_read(uart, buffer, BUFSIZE, &bytesRead);
 *
 *  // Write to the UART
 *  size_t  bytesWritten;
 *  status = UART2_write(uart, buffer, BUFSIZE, &bytesWritten);
 *
 *  // Close the UART
 *  UART2_close(uart);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_UART2_Examples
 *  # Examples
 *  The following code example opens a UART instance, reads
 *  a byte from the UART, and then writes the byte back to the UART.
 *
 *  @code
 *    char        input;
 *    UART2_Handle uart;
 *    UART2_Params uartParams;
 *
 *    // Open an instance of the UART2 driver
 *    UART2_Params_init(&uartParams);
 *    uartParams.baudRate = 115200;
 *    uart = UART2_open(CONFIG_UART0, &uartParams);
 *
 *    if (uart == NULL) {
 *        // UART2_open() failed
 *        while (1);
 *    }
 *
 *    // Enable receiver, inhibit low power mode
 *    UART2_rxEnable(uart);
 *
 *    // Loop forever echoing
 *    while (1) {
 *        status = UART2_read(uart, &input, 1, &bytesRead);
 *        status = UART2_write(uart, &input, 1, &bytesWritten);
 *    }
 *  @endcode
 *
 *  Details for the example code above are described in the following
 *  subsections.
 *
 *  ### Opening the UART2 Driver #
 *
 *  Opening a UART requires four steps:
 *  1.  Create and initialize a UART2_Params structure.
 *  2.  Fill in the desired parameters.
 *  3.  Call UART2_open(), passing the index of the UART in the UART2_config
 *      structure, and the address of the UART2_Params structure.  The
 *      UART2 instance is specified by the index in the UART2_config structure.
 *  4.  Check that the UART2 handle returned by UART2_open() is non-NULL,
 *      and save it.  The handle will be used to read and write to the
 *      UART you just opened.
 *
 *  Only one UART index can be used at a time; calling UART2_open() a second
 *  time with the same index previosly passed to UART2_open() will result in
 *  an error.  You can, though, re-use the index if the instance is closed
 *  via UART2_close().
 *  In the previous example code, CONFIG_UART0 is passed to UART2_open().
 *  This macro is defined in the example's ti_drivers_config.h file.
 *
 *
 *  ### Modes of Operation #
 *
 *  The UART driver can operate in blocking, non-blocking, or callback mode, by
 *  setting the writeMode and readMode parameters passed to UART2_open().
 *  If these parameters are not set, as in the example code, the UART2
 *  driver defaults to blocking mode.  Options for the writeMode and
 *  readMode parameters are #UART2_Mode_BLOCKING, #UART2_Mode_NONBLOCKING, and
 *  #UART2_Mode_CALLBACK:
 *
 *  - #UART2_Mode_BLOCKING uses a semaphore to block while data is being sent,
 *    or while waiting for some data to be received. The context of calling
 *    UART2_read() and UART2_write() in blocking mode must always be a Task.
 *    The UART2_write() call will block until all data has been copied to
 *    the TX circular buffer.  The UART2_read() calls can be configured to
 *    have two different behaviors, using the #UART2_ReadReturnMode of the
 *    #UART2_Params.  In #UART2_ReadReturnMode_FULL (the default),
 *    UART2_read() will block until the requested number of bytes has been
 *    received.  In #UART2_ReadReturnMode_PARTIAL, UART2_read() will block
 *    until either the requested number of bytes has been received,
 *    or a UART hardware read timeout has occurred.  Using
 *    UART2_ReadReturnMode_PARTIAL is a good choice if the number of
 *    incoming data bytes is unknown.  In UART2_Mode_BLOCKING,
 *    UART2_read() will always return at least some data.
 *    UART2_readTimeout() can be used to specify a timeout in system
 *    clock ticks, to wait for data.
 *    UART2_readTimeout() will return when all data is received, or
 *    the specified timeout expires, or, if using the mode
 *    UART2_ReadReturnMode_PARTIAL, a hardware read timeout occurs,
 *    whichever happens first.
 *
 *  - #UART2_Mode_NONBLOCKING does not block waiting for data to be sent
 *    or received.  UART2_write() and UART2_read() will return immediately
 *    having transferred as much data as the driver can immediately accept
 *    or has available, respectively.  If no data can be accepted or
 *    received, UART2_write() and UART2_read() return UART2_STATUS_EAGAIN.
 *
 *  - #UART2_Mode_CALLBACK is non-blocking and UART2_read() and UART2_write()
 *    will return while data is being sent in the context of a hardware
 *    interrupt.  When the read or write finishes, the UART2 driver will call
 *    the user's callback function.  In some cases, the UART data transfer
 *    may have been cancelled, so the number of bytes sent/received are
 *    passed to the callback function.  Your implementation of the callback
 *    function can use this information as needed.
 *    Since the user's callback may be called in the context of a hardware
 *    interrupt, the callback function must not make any RTOS blocking calls.
 *    The buffer passed to UART2_write() in UART2_Mode_CALLBACK must remain
 *    coherent until all the characters have been sent
 *    (ie until the tx callback has been called with a byte count equal to
 *    that passed to UART2_write()).
 *
 *  ### Enabling the Receiver #
 *
 *  The example code enables the collection of data into the RX ring buffer
 *  \a before the first call to UART2_read():
 *
 *  @code
 *  UART2_rxEnable(uart);
 *  @endcode
 *
 *  Note that this call is not necessary if the first UART2_read() is called
 *  in time to prevent the RX FIFO from overrun, or if flow control is used.
 *
 *  ### Reading and Writing Data #
 *
 *  The example code reads one byte frome the UART instance, and then writes
 *  one byte back to the same instance:
 *
 *  @code
 *  status = UART2_read(uart, &input, 1, &bytesRead);
 *  status = UART2_write(uart, &input, 1, &bytesWritten);
 *  @endcode
 *
 *  The UART2 driver allows full duplex data transfers. Therefore, it is
 *  possible to call UART2_read() and UART2_write() at the same time.
 *  It is not possible, however,
 *  to issue multiple concurrent operations in the same direction.
 *  For example, if one thread calls UART2_read(uart0, buffer0...),
 *  any other thread attempting UART2_read(uart0, buffer1...) will result in
 *  an error of UART2_STATUS_EINUSE, until all the data from the first
 *  UART2_read() has been transferred to buffer0. This applies to blocking,
 *  callback, and non-blocking modes. So applications must either synchronize
 *  UART2_read() (or UART2_write()) calls that use the same UART handle, or
 *  check for the UART2_STATUS_EINUSE return code indicating that a transfer is
 *  still ongoing.
 *
 *  <hr>
 *  @anchor ti_drivers_UART2_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 *
 *  ============================================================================
 */

#ifndef ti_drivers_UART2__include
#define ti_drivers_UART2__include

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/utils/RingBuf.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief No hardware flow control
 */
#define UART2_FLOWCTRL_NONE 0

/*!
 * @brief Hardware flow control
 */
#define UART2_FLOWCTRL_HARDWARE 1

/** @addtogroup UART2_STATUS
 *  @{
 */
/*!
 * @brief   Successful status code returned by UART2 APIs.
 */
#define UART2_STATUS_SUCCESS         (0)

/*!
 * @brief  A  read timeout occurred (not an error).
 */
#define UART2_STATUS_SREADTIMEOUT    (1)

/*!
 * @brief  A framing error occurred.
 */
#define UART2_STATUS_EFRAMING        (-1)

/*!
 * @brief  A parity error occurred.
 */
#define UART2_STATUS_EPARITY         (-2)

/*!
 * @brief  A break error occurred.
 */
#define UART2_STATUS_EBREAK          (-4)

/*!
 * @brief  A FIFO overrun occurred.
 */
#define UART2_STATUS_EOVERRUN        (-8)

/*!
 * @brief  The UART is currently in use.
 */
#define UART2_STATUS_EINUSE          (-9)

/*!
 * @brief  An invalid argument or UART2_Params field was passed to UART2 API.
 */
#define UART2_STATUS_EINVALID        (-10)

/*!
 * @brief  General failure status returned by UART2 API.
 */
#define UART2_STATUS_EFAIL           (-11)

/*!
 * @brief  A memory allocation failure occurred.
 */
#define UART2_STATUS_EMEMORY         (-12)

/*!
 * @brief  A timeout occurred for a blocking UART2_read or UART2_write call.
 */
#define UART2_STATUS_ETIMEOUT        (-13)

/*!
 * @brief  A UART2_write() or UART2_read() operation was cancelled.
 */
#define UART2_STATUS_ECANCELLED      (-14)

/*!
 * @brief  A UART2_write() or UART2_read() called on a device not opened.
 */
#define UART2_STATUS_ENOTOPEN        (-15)

/*!
 * @brief  A UART2_write() or UART2_read() in UART2_Mode_NONBLOCKING would
 *         have blocked.
 */
#define UART2_STATUS_EAGAIN         (-16)

/** @}*/

/** @addtogroup UART2_EVENT
 *  @{
 */

/*!
 * @brief   A receive overrun has occurred.
 */
#define UART2_EVENT_OVERRUN (0x08)

/*!
 * @brief   A break has occurred.
 */
#define UART2_EVENT_BREAK   (0x04)

/*!
 * @brief   A parity error has occurred.
 */
#define UART2_EVENT_PARITY  (0x02)

/*!
 * @brief   A framing error has occurred.
 */
#define UART2_EVENT_FRAMING (0x01)

/*!
 * @brief   The UART will start transmitting data.
 *
 * This event can be useful in RS-485 half-duplex systems to toggle TX enable high.
 */
#define UART2_EVENT_TX_BEGIN (0x10)

/*!
 * @brief   The UART stopped transmitting data.
 *
 * This event can be useful in RS-485 half-duplex systems to toggle TX enable low.
 */
#define UART2_EVENT_TX_FINISHED (0x20)

/** @}*/

/*!
 *  @brief    Wait forever define
 */
#define UART2_WAIT_FOREVER           (~(0U))

/*!
 *  @brief      A handle that is returned from a UART2_open() call.
 */
typedef struct UART2_Config_   *UART2_Handle;

/*!
 *  @brief      The definition of a callback function used by the UART2 driver
 *              when used in #UART2_Mode_CALLBACK
 *              The callback can occur in task or interrupt context.
 *
 *  @param[in]  UART2_Handle            UART2_Handle
 *
 *  @param[in]  buf                     Pointer to read/write buffer
 *
 *  @param[in]  count                   Number of elements read/written
 *
 *  @param[in]  userArg                 A user supplied argument specified
 *                                      in UART2_Params.
 *
 *  @param[in]  status                  A UART2_STATUS code indicating
 *                                      success or failure of the transfer.
 */
typedef void (*UART2_Callback) (UART2_Handle handle, void *buf, size_t count,
            void *userArg, int_fast16_t status);

/*!
 *  @brief      The definition of a callback function used by the UART driver.
 *              The callback can occur in task or interrupt context.
 *
 *  @param[in]  UART2_Handle            UART2_Handle
 *
 *  @param[in]  event                   UART2_EVENT that has occurred.
 *
 *  @param[in]  data                    - UART2_EVENT_OVERRUN: accumulated count
 *                                      - UART2_EVENT_BREAK: unused
 *                                      - UART2_EVENT_PARITY: unused
 *                                      - UART2_EVENT_FRAMING: unused
 *                                      - UART2_EVENT_TX_BEGIN: unused
 *                                      - UART2_EVENT_TX_FINISHED: unused
 *
 *  @param[in]  userArg                 A user supplied argument specified
 *                                      in UART2_Params.
 *
 *  @param[in]  status                  A UART2_STATUS code indicating
 *                                      success or failure of the transfer.
 */
typedef void (*UART2_EventCallback) (UART2_Handle handle, uint32_t event,
        uint32_t data, void *userArg);

/*!
 *  @brief      UART2 mode settings
 *
 *  This enum defines the read and write modes for the configured UART.
 */
typedef enum {
    /*!
      *  UART2_write() will block the calling task until all of the data can be
      *  accepted by the device driver. UART2_read() will block until some data
      *  becomes available.
      */
    UART2_Mode_BLOCKING,

    /*!
      *  Non-blocking, UART2_write() or UART2_read() will return immediately.
      *  When the transfer has finished, the callback function is called
      *  from either the caller's context or from an interrupt context.
      */
    UART2_Mode_CALLBACK,

    /*!
      *  Non-blocking, UART2_write() or UART2_read() will return immediately.
      *  UART2_write() will copy as much data into the transmit buffer as space
      *  allows. UART2_read() will copy as much data from the receive buffer
      *  as is immediately available.
      */
    UART2_Mode_NONBLOCKING,
    /*! @cond NODOC */
    /* Added for backwards compatibility. */
    UART2_Mode_POLLING = UART2_Mode_NONBLOCKING
    /*! @endcond */
} UART2_Mode;

/*!
 *  @brief      UART2 return mode settings
 *
 *  This enumeration defines the return modes for UART2_read().
 *
 *  #UART2_ReadReturnMode_FULL unblocks or performs a callback when the read
 *  buffer has been filled with the number of bytes passed to #UART2_read().
 *  #UART2_ReadReturnMode_PARTIAL unblocks or performs a callback whenever a
 *  read timeout error occurs on the UART peripheral.  This timeout
 *  error is not the same as the blocking read timeout in the UART2_Params;
 *  the read timeout occurs if the read FIFO is non-empty and no new
 *  data has been received for a device/baudrate dependent number of
 *  clock cycles.  This mode can be used when the exact number of bytes to
 *  be read is not known.
 */
typedef enum {
    /*! Unblock/callback when buffer is full. */
    UART2_ReadReturnMode_FULL,

    /*! Unblock/callback when no new data comes in. */
    UART2_ReadReturnMode_PARTIAL
} UART2_ReadReturnMode;

/*!
 *  @brief    UART2 data length settings
 *
 *  This enumeration defines the UART data lengths.
 */
typedef enum {
    UART2_DataLen_5 = 0,  /*!< Data length is 5 bits */
    UART2_DataLen_6 = 1,  /*!< Data length is 6 bits */
    UART2_DataLen_7 = 2,  /*!< Data length is 7 bits */
    UART2_DataLen_8 = 3   /*!< Data length is 8 bits */
} UART2_DataLen;

/*!
 *  @brief    UART2 stop bit settings
 *
 *  This enumeration defines the UART2 stop bits.
 */
typedef enum {
    UART2_StopBits_1 = 0,  /*!< One stop bit */
    UART2_StopBits_2 = 1   /*!< Two stop bits */
} UART2_StopBits;

/*!
 *  @brief    UART2 parity type settings
 *
 *  This enumeration defines the UART2 parity types.
 */
typedef enum {
    UART2_Parity_NONE = 0,  /*!< No parity */
    UART2_Parity_EVEN = 1,  /*!< Parity bit is even */
    UART2_Parity_ODD  = 2,  /*!< Parity bit is odd */
    UART2_Parity_ZERO = 3,  /*!< Parity bit is always zero */
    UART2_Parity_ONE  = 4   /*!< Parity bit is always one */
} UART2_Parity;

/*!
 *  @brief    UART2 Parameters
 *
 *  UART2 parameters are used with the UART2_open() call. Default values for
 *  these parameters are set using UART2_Params_init().
 *
 *  @sa       UART2_Params_init()
 */
typedef struct {
    UART2_Mode      readMode;        /*!< Mode for all read calls */
    UART2_Mode      writeMode;       /*!< Mode for all write calls */
    UART2_Callback  readCallback;    /*!< Pointer to read callback function for callback mode. */
    UART2_Callback  writeCallback;   /*!< Pointer to write callback function for callback mode. */
    UART2_EventCallback  eventCallback; /*!< Pointer to event callback function. */
    uint32_t        eventMask;       /*!< mask of events that the application is interested in */
    UART2_ReadReturnMode readReturnMode;  /*!< Receive return mode */
    uint32_t        baudRate;        /*!< Baud rate for UART */
    UART2_DataLen   dataLength;      /*!< Data length for UART */
    UART2_StopBits  stopBits;        /*!< Stop bits for UART */
    UART2_Parity    parityType;      /*!< Parity bit type for UART */
    void           *userArg;         /*!< User supplied argument for callback functions */
} UART2_Params;

/*! @cond NODOC */
#define UART2_BASE_OBJECT \
    /* UART2 state variable */ \
    struct { \
        uint32_t         overrunCount;     /* total count of overruns */ \
        bool             opened:1;         /* Has the obj been opened */ \
        UART2_Mode       readMode;         /* Mode for read calls */ \
        UART2_Mode       writeMode;        /* Mode for write calls */ \
        UART2_ReadReturnMode readReturnMode:1; /* RX return mode (partial/full) */ \
        bool             txEnabled:1;      /* Flag set if ongoing transmit */ \
        bool             rxEnabled:1;      /* Flag set if ongoing receive */ \
        bool             rxCancelled:1;    /* Has the TX been canceled */ \
        bool             txCancelled:1;    /* Has the TX been canceled */ \
        bool             readTimedOut:1;   /* Has read timed out */ \
        bool             writeTimedOut:1;  /* Has write timed out */ \
        bool             overrunActive:1;  /* Is a RX overrun active */ \
        bool             inReadCallback:1; /* To avoid stack overflow */ \
        bool             readCallbackPending:1; /* To avoid stack overflow */ \
        bool             inWriteCallback:1; /* To avoid stack overflow */ \
        bool             writeCallbackPending:1; /* To avoid stack overflow */ \
    } state; \
    \
    HwiP_Struct          hwi;              /* Hwi object for interrupts */ \
    uint32_t             baudRate;         /* Baud rate for UART */ \
    UART2_DataLen        dataLength;       /* Data length for UART */ \
    UART2_StopBits       stopBits;         /* Stop bits for UART */ \
    UART2_Parity         parityType;       /* Parity bit type for UART */ \
    int32_t              rxStatus;         /* RX status */ \
    int32_t              txStatus;         /* TX status */ \
    UART2_EventCallback  eventCallback;    /* User supplied event callback */ \
    uint32_t             eventMask;        /* User supplied event mask */ \
    void                *userArg;          /* User supplied arg for callbacks */ \
    \
    /* UART read variables */ \
    RingBuf_Object       rxBuffer;         /* Receive ring buffer */ \
    bool                 readInUse;        /* Is a read() active */ \
    unsigned char       *readBuf;          /* Buffer data pointer */ \
    size_t               readSize;         /* Number of bytes to read */ \
    uint32_t             nReadTransfers;   /* Number of DMA transfers needed */ \
    size_t               readCount;        /* Number of bytes left to read */ \
    size_t               rxSize;           /* # of bytes to read in DMA xfer */ \
    size_t               bytesRead;        /* Number of bytes read */ \
    SemaphoreP_Struct    readSem;          /* UART read semaphore */ \
    ClockP_Struct        readTimeoutClk;   /* Clock object to for timeouts */ \
    UART2_Callback       readCallback;     /* Pointer to read callback */ \
    \
    /* UART write variables */ \
    RingBuf_Object       txBuffer;         /* Transmit ring buffer */ \
    volatile bool        writeInUse;       /* Flag to show ongoing write */ \
    const unsigned char *writeBuf;         /* Buffer data pointer */ \
    size_t               writeSize;        /* Number of bytes to write*/ \
    uint32_t             nWriteTransfers;  /* Number of DMA transfers needed */\
    size_t               writeCount;       /* Number of bytes left to write */ \
    size_t               txSize;           /* # of bytes to write with DMA */ \
    size_t               bytesWritten;     /* Number of bytes written */ \
    SemaphoreP_Struct    writeSem;         /* UART write semaphore*/ \
    ClockP_Struct        writeTimeoutClk;  /* Clock object to for timeouts */ \
    UART2_Callback       writeCallback;    /* Pointer to write callback */ \
    \
    /* For Power management */ \
    unsigned int         powerMgrId;       /* Determined from base address */ \
/*! @endcond */

/*!
 *  @cond NODOC
 *  UART2 Object. Applications must not access any member variables of
 *  this structure!
 */
typedef struct {
    UART2_BASE_OBJECT
} UART2_Object;
/*! @endcond */

/*! @cond NODOC */
#define UART2_BASE_HWATTRS \
    /*! UART Peripheral's base address */ \
    uint32_t        baseAddr; \
    /*! UART Peripheral's interrupt vector */ \
    int             intNum; \
    /*! UART Peripheral's interrupt priority */ \
    uint8_t         intPriority; \
    /*! Pointer to an application RX buffer */ \
    unsigned char  *rxBufPtr; \
    /*! Size of rxBufPtr */ \
    size_t          rxBufSize; \
    /*! Pointer to an application TX buffer */ \
    unsigned char  *txBufPtr; \
    /*! Size of txBufPtr */ \
    size_t          txBufSize; \
    /*! Hardware flow control setting */ \
    uint32_t        flowControl; \
    /*! UART RX pin assignment */ \
    uint32_t        rxPin; \
    /*! UART TX pin assignment */ \
    uint32_t        txPin; \
    /*! UART clear to send (CTS) pin assignment */ \
    uint32_t        ctsPin; \
    /*! UART request to send (RTS) pin assignment */ \
    uint32_t        rtsPin; \
/*! @endcond */

/*!
 *  @cond NODOC
 *  UART2 HWAttrs.
 */
typedef struct {
    UART2_BASE_HWATTRS
} UART2_HWAttrs;
/*! @endcond */

/*!
 *  @brief  UART2 Global configuration
 *
 *  The UART2_Config structure contains a set of pointers used to characterize
 *  the UART2 driver implementation.
 *
 */
typedef struct UART2_Config_ {
    /*! Pointer to a driver specific data object */
    void                *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void          const *hwAttrs;
} UART2_Config;

extern const UART2_Config UART2_config[];
extern const uint_least8_t UART2_count;

/*!
 *  @brief  Function to close a UART peripheral specified by the UART2 handle
 *
 *  @pre    UART2_open() has been called.
 *  @pre    There are no ongoing read or write calls.  Any ongoing read
 *          or write calls can be cancelled with UART2_readCancel() or
 *          UART2_writeCancel().
 *
 *  @param[in]  handle      A #UART2_Handle returned from UART2_open()
 *
 *  @sa     UART2_open()
 */
extern void UART2_close(UART2_Handle handle);

/*!
 *  @brief  Function to flush data in the UART RX FIFO.
 *
 *  @pre    UART2_open() has been called.
 *
 *  This function can be called to remove all data from the RX FIFO, for
 *  example, after a UART read error has occurred.
 *  All data in the RX circular buffer will be discarded.
 *
 *  @param[in]  handle      A #UART2_Handle returned from UART2_open()
 */
extern void UART2_flushRx(UART2_Handle handle);

/*!
 *  @brief  Get the number of bytes available in the circular buffer.
 *
 *  @pre    UART2_open() has been called.
 *
 *  @param[in]  handle      A #UART2_Handle returned from UART2_open()
 *
 *  @return Returns the number of bytes available in the RX circular
 *          buffer.
 *
 *  @sa     UART2_rxEnable()
 */
extern size_t UART2_getRxCount(UART2_Handle handle);

/*!
 *  @brief  Function to initialize a given UART peripheral
 *
 *  Function to initialize a given UART peripheral specified by the
 *  particular index value.
 *
 *  @param[in]  index     Logical peripheral number for the UART indexed into
 *                        the UART2_config table
 *
 *  @param[in]  params    Pointer to a parameter block. If NULL, default
 *                        parameter values will be used. All the fields in
 *                        this structure are read-only.
 *
 *  @sa     UART2_close()
 */
extern UART2_Handle UART2_open(uint_least8_t index, UART2_Params *params);

/*!
 *  @brief  Function to initialize the UART2_Params struct to its defaults
 *
 *  @param[in]  params  A pointer to UART2_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *      readMode = UART2_Mode_BLOCKING;
 *      writeMode = UART2_Mode_BLOCKING;
 *      eventCallback = NULL;
 *      eventMask = 0;
 *      readCallback = NULL;
 *      writeCallback = NULL;
 *      readReturnMode = UART2_ReadReturnMode_FULL;
 *      baudRate = 115200;
 *      dataLength = UART2_DataLen_8;
 *      stopBits = UART2_StopBits_1;
 *      parityType = UART2_Parity_NONE;
 *      userArg = NULL;
 */
extern void UART2_Params_init(UART2_Params *params);

/*!
 *  @brief  Function that reads data from a UART.
 *
 *  %UART2_read() reads data from a UART controller. The destination is
 *  specified by \a buffer and the number of bytes to read is given by
 *  \a size.
 *
 *  In #UART2_Mode_BLOCKING, %UART2_read() blocks task execution until all
 *  the data in buffer has been read, if the read return mode is
 *  #UART2_ReadReturnMode_FULL.
 *  If the read return mode is #UART2_ReadReturnMode_PARTIAL, %UART2_read()
 *  returns before all the data has been read, if some data has been received,
 *  but reception has been inactive sufficiently long for a hardware read
 *  timeout to occur (e.g., for a 32-bit period).
 *  If a receive error occurs (e.g., framing, fifo overrun), %UART2_read()
 *  will return with the number of bytes read up to the occurance of the
 *  error.
 *
 *  In #UART2_Mode_CALLBACK, %UART2_read() does not block task execution.
 *  Instead, a callback function specified by UART2_Params::readCallback
 *  is called when the transfer is finished (#UART2_ReadReturnMode_FULL), or
 *  reception has become inactive (#UART2_ReadReturnMode_PARTIAL).
 *  The callback function can occur in the caller's context or in SWI
 *  context, depending on the device-specific implementation.
 *  An unfinished asynchronous read operation must always be cancelled using
 *  UART2_readCancel() before calling UART2_close().
 *
 *  In #UART2_Mode_NONBLOCKING, %UART2_read() will return the minimum
 *  of size bytes and the number of bytes in the RX circular buffer.
 *  In this mode, the application should check the number of bytes
 *  returned in the bytesRead parameter.  A status of success will be
 *  returned, even if not all bytes requested were read, unless no
 *  data is available or an error occured.  If no data is available,
 *  a status of UART2_STATUS_EAGAIN is returned.
 *
 *  @note It is ok to call %UART2_read() from its own callback function when in
 *  #UART2_Mode_CALLBACK.
 *
 *  @param[in]  handle  A #UART2_Handle returned by UART2_open()
 *
 *  @param[in]  buffer  A pointer to an empty buffer to which
 *                      received data should be read
 *
 *  @param[in]  size    The number of bytes to be read into buffer
 *
 *  @param[out]  bytesRead   If non-NULL, the location to store the number of
 *                      bytes actually read into the buffer.  If NULL, this
 *                      parameter will be ignored.  In callback mode, NULL
 *                      could be passed in for this parameter, since the
 *                      callback function will be passed the number of bytes
 *                      read.  In blocking mode, NULL can be passed,
 *                      however, status should be checked in case the number
 *                      of bytes requested was not received due to errors.
 *                      In non-blocking mode, it is not recommended to pass
 *                      NULL for this parameter, as it would be impossible to
 *                      determine the number of bytes actually read.
 *
 *  @return Returns a status indicating success or failure of the read.
 *
 *  @retval #UART2_STATUS_SUCCESS    The call was successful.
 *  @retval #UART2_STATUS_EINUSE     Another read from the UART is currently
 *                                   ongoing.
 *  @retval #UART2_STATUS_EAGAIN     In #UART2_Mode_NONBLOCKING, no data is
 *                                   currently available.
 *  @retval #UART2_STATUS_ECANCELLED In #UART2_Mode_BLOCKING, the read was
 *                                   canceled by a call to UART2_readCancel()
 *                                   before any data could be received.
 */
extern int_fast16_t UART2_read(UART2_Handle handle, void *buffer, size_t size,
        size_t *bytesRead);

/*! @cond NODOC */
extern int_fast16_t __attribute__((weak)) UART2_readFull(UART2_Handle handle,
        void *buffer, size_t size, size_t *bytesRead);
/*! @endcond */

/*!
 *  @brief  Function that reads data from a UART, with a specified timeout
 *  for blocking mode.
 *
 *  %UART2_readTimeout() reads data from a UART controller. The destination is
 *  specified by \a buffer and the number of bytes to read is given by
 *  \a size.
 *
 *  In #UART2_Mode_BLOCKING with #UART2_ReadReturnMode_FULL,
 *  %UART2_readTimeout() blocks task execution until all the data in buffer
 *  has been read, or the specified timeout has elapsed.
 *  In #UART2_Mode_BLOCKING with #UART2_ReadReturnMode_PARTIAL,
 *  %UART2_readTimeout() returns before all the data has been read, if some
 *  data has been received, but reception has been inactive sufficiently
 *  long for a hardware read timeout to occur (e.g., for a 32-bit period).
 *  %UART2_readTimeout() will also return if the specified timeout parameter
 *  has elapsed.  Note that the timeout parameter is different from the
 *  hardware read timeout.
 *
 *  In #UART2_Mode_CALLBACK, %UART2_readTimeout() does not block task
 *  execution. Instead, a callback function specified by
 *  UART2_Params::readCallback is called when the transfer is finished
 *  (#UART2_ReadReturnMode_FULL), or reception has become inactive
 *  (#UART2_ReadReturnMode_PARTIAL).
 *  The callback function can occur in the caller's context or in HWI
 *  context, depending on the device-specific implementation.
 *  An unfinished asynchronous read operation must always be cancelled using
 *  UART2_readCancel() before calling UART2_close().  In #UART2_Mode_CALLBACK,
 *  the timeout parameter passed to %UART2_readTimeout(), is ignored.
 *
 *  In #UART2_Mode_NONBLOCKING, %UART2_readTimeout() will return the minimum of
 *  size and the number of data in the RX circular buffer.  In this mode,
 *  the application should check the number of bytes read in the
 *  bytesRead parameter.  A status of success will be returned if
 *  one or more bytes is available, unless an error occured.
 *  In #UART2_Mode_NONBLOCKING, the timeout parameter passed to
 *  %UART2_readTimeout(), is ignored.
 *
 *  @note It is ok to call %UART2_readTimeout() from its own callback
 *  function when in #UART2_Mode_CALLBACK.
 *
 *  @param[in]  handle  A #UART2_Handle returned by UART2_open()
 *
 *  @param[in]  buffer  A pointer to an empty buffer to which
 *                      received data should be read
 *
 *  @param[in]  size    The number of bytes to be read into buffer
 *
 *  @param[out]  bytesRead   If non-NULL, the location to store the number of
 *                      bytes actually read into the buffer.  If NULL, this
 *                      parameter will be ignored.  In callback mode, NULL
 *                      could be passed in for this parameter, since the
 *                      callback function will be passed the number of bytes
 *                      read.  Similarly, in blocking mode with infinite
 *                      timeout, NULL can be passed.  However, status should
 *                      be checked in case the number of bytes requested was
 *                      not received due to errors.
 *                      In non-blocking mode, it is not recommended to pass
 *                      NULL for this parameter, as it would be impossible to
 *                      determine the number of bytes actually read.
 *
 *  @param[in]  timeout The number of system clock ticks to wait until
 *                      all data is received.  If not all requested data
 *                      was received within the timeout period, an error of
 *                      UART2_STATUS_ETIMEOUT will be returned.  This
 *                      parameter is only applicable to #UART2_Mode_BLOCKING.
 *
 *  @return Returns a status indicating success or failure of the read.
 *
 *  @retval #UART2_STATUS_SUCCESS    The call was successful.
 *  @retval #UART2_STATUS_EINUSE     Another read from the UART is currently
 *                                   ongoing.
 *  @retval #UART2_STATUS_EAGAIN     In #UART2_Mode_NONBLOCKING, no data is
 *                                   currently available.
 *  @retval #UART2_STATUS_ECANCELLED In #UART2_Mode_BLOCKING, the read was
 *                                   canceled by a call to UART2_readCancel()
 *                                   before any data could be received.
 *  @retval #UART2_STATUS_ETIMEOUT   The read operation timed out.
 */
extern int_fast16_t UART2_readTimeout(UART2_Handle handle, void *buffer,
        size_t size, size_t *bytesRead, uint32_t timeout);

/*!
 *  @brief  Function that cancels a UART2_read() function call.
 *
 *  This function cancels an asynchronous UART2_read() operation in
 *  in #UART2_Mode_CALLBACK, or unblocks a UART2_read() call in
 *  #UART2_Mode_BLOCKING.
 *  In #UART2_Mode_CALLBACK,  UART2_readCancel() calls the registered read
 *  callback function with the number of bytes received so far.
 *  It is the application's responsibility to check the count argument
 *  in the callback function and handle the case where only a subset of the
 *  bytes were received.  The callback function will be passed a status of
 *  #UART2_STATUS_ECANCELLED.
 *
 *  In #UART2_Mode_BLOCKING, #UART2_read() will return
 *  #UART2_STATUS_ECANCELLED, and the bytesRead parameter will be set to
 *  the number of bytes received so far.
 *
 *  This API has no affect in #UART2_Mode_NONBLOCKING.
 *
 *  @param[in]  handle      A #UART2_Handle returned by UART2_open()
 */
extern void UART2_readCancel(UART2_Handle handle);

/*!
 *  @brief  Function that writes data to a UART.
 *
 *  %UART2_write() writes data from a memory buffer to the UART interface.
 *  The source is specified by \a buffer and the number of bytes to write
 *  is given by \a size.
 *
 *  In #UART2_Mode_BLOCKING, UART2_write() blocks task execution until all
 *  the data in buffer has been written.
 *
 *  In #UART2_Mode_CALLBACK, %UART2_write() does not block task execution.
 *  Instead, a callback function specified by UART2_Params::writeCallback is
 *  called when the transfer is finished.  The buffer passed to UART2_write()
 *  in #UART2_Mode_CALLBACK is not copied. The buffer must remain coherent
 *  until all the characters have been sent (ie until the write callback has
 *  been called with a byte count equal to that passed to UART2_write()).
 *  The callback function can occur in the caller's task context or in a HWI or
 *  SWI context, depending on the device implementation.
 *  An unfinished asynchronous write operation must always be cancelled using
 *  UART2_writeCancel() before calling UART2_close().
 *
 *  In #UART2_Mode_NONBLOCKING, UART2_write() will send out as many of the
 *  bytes in the buffer as possible, until the TX circular buffer is
 *  full.  In non-blocking mode, UART2_write() can be called from
 *  any context.
 *  The bytesWritten parameter should not be NULL so the application can
 *  determine the number of bytes actually written.
 *
 *  @param[in]  handle   A #UART2_Handle returned by UART2_open()
 *
 *  @param[in]  buffer   A read-only pointer to buffer containing data to
 *                       be written to the UART
 *
 *  @param[in]  size     The number of bytes in the buffer that should be
 *                       written to the UART
 *
 *  @param[out]  bytesWritten If non-NULL, the location to store the number of
 *                       bytes actually written to the UART in
 *                       UART2_Mode_BLOCKING and UART2_Mode_NONBLOCKING.  In
 *                       UART2_Mode_CALLBACK, bytesWritten will be set to 0.
 *                       If bytesWritten is NULL, this parameter will be
 *                       ignored.
 *                       In non-blocking mode, it is not recommended to pass
 *                       NULL for bytesWritten, as the application would have
 *                       no way to determine the number of bytes actually
 *                       written.  In non-blocking mode, a status of success
 *                       will be returned even if not all the requested
 *                       bytes could be written.
 *
 *  @return Returns a status indicating success or failure of the write.
 *
 *  @retval #UART2_STATUS_SUCCESS The call was successful.
 *  @retval #UART2_STATUS_EINUSE  Another write to the UART is currently
 *                                ongoing.
 */
extern int_fast16_t UART2_write(UART2_Handle handle, const void *buffer,
        size_t size, size_t *bytesWritten);

/*!
 *  @brief  Function that disables collecting of RX data into the circular
 *          buffer.
 *
 *  The driver implementation uses a circular buffer to collect RX
 *  data while a UART2_read() is not in progress.  This function will
 *  disable buffering of RX data into the circular buffer. UART2_read() will
 *  read directly from the UART driver's RX buffer.  Disabling the circular
 *  buffer will also allow the device to go into low power modes.
 *
 *  @param[in]  handle      A #UART2_Handle returned by UART2_open()
 *
 *  @sa     UART2_rxEnable()
 */
extern void UART2_rxDisable(UART2_Handle handle);

/*!
 *  @brief  Function that enables collecting of RX data into the circular
 *          buffer.
 *
 *  The driver implementation uses a circular buffer to collect RX
 *  data while a UART2_read() is not in progress.  This function will
 *  enable buffering of RX data into the circular buffer. UART2_read() will
 *  read directly from the UART drivers RX buffer. Enabling the circular
 *  buffer will also prevent the device from going into low power modes.
 *
 *  @param[in]  handle      A #UART2_Handle returned by UART2_open()
 *
 *  @sa     UART2_rxDisable()
 */
extern void UART2_rxEnable(UART2_Handle handle);

/*!
 *  @brief  Function that writes data to a UART, with a specified timeout.
 *
 *  %UART2_writeTimeout() writes data from a memory buffer to the UART
 *  interface.
 *  The source is specified by \a buffer and the number of bytes to write
 *  is given by \a size.
 *  A timeout in system clock ticks specifies the maximum time to wait
 *  until all data is written (#UART2_Mode_BLOCKING only).
 *
 *  In #UART2_Mode_BLOCKING, UART2_writeTimeout() blocks task execution until
 *  all the data in buffer has been written, or the timeout expires.
 *
 *  In #UART2_Mode_CALLBACK, %UART2_writeTimeout() does not block task
 *  execution.  Instead, a callback function specified by
 *  UART2_Params::writeCallback is called when the transfer is finished.
 *  The buffer passed to UART2_writeTimeout() in #UART2_Mode_CALLBACK is not
 *  copied. The buffer must remain coherent until all the characters have
 *  been sent (ie until the write callback has been called with a byte count
 *  equal to that passed to UART2_writeTimeout()).
 *  The callback function can occur in the caller's task context or in
 *  interrupt context, depending on the device implementation.
 *  An unfinished asynchronous write operation must always be cancelled using
 *  UART2_writeCancel() before calling UART2_close().
 *
 *  In #UART2_Mode_NONBLOCKING, UART2_writeTimeout() will send out as many of
 *  the bytes in the buffer as possible, until the TX FIFO is full.  In
 *  non-blocking mode, UART2_writeTimeout() is non-blocking and can be called
 *  from any context.  The bytesWritten parameter should not be NULL so the
 *  application can determine the number of bytes actually written.
 *
 *  @param[in]  handle   A #UART2_Handle returned by UART2_open()
 *
 *  @param[in]  buffer   A read-only pointer to buffer containing data to
 *                       be written to the UART
 *
 *  @param[in]  size     The number of bytes in the buffer that should be
 *                       written to the UART
 *
 *  @param[out]  bytesWritten If non-NULL, the location to store the number of
 *                       bytes actually written to the UART in
 *                       UART2_Mode_BLOCKING and UART2_Mode_NONBLOCKING.  In
 *                       UART2_Mode_CALLBACK, bytesWritten will be set to 0.
 *                       If bytesWritten is NULL, this parameter will be
 *                       ignored.
 *                       In non-blocking mode, it is not recommended to pass
 *                       NULL for bytesWritten, as the application would have
 *                       no way to determine the number of bytes actually
 *                       written.  In non-blocking mode, a status of success
 *                       will be returned even if not all the requested
 *                       bytes could be written.
 *
 *  @param[in]  timeout  The number of system clock ticks to wait for the
 *                       write to complete (#UART2_Mode_BLOCKING only).  If
 *                       the timeout expires before all bytes are written, a
 *                       status of UART2_STATUS_ETIMEOUT will be returned.
 *
 *  @return Returns a status indicating success or failure of the write.
 *
 *  @retval #UART2_STATUS_SUCCESS The call was successful.
 *  @retval #UART2_STATUS_EINUSE  Another write to the UART is currently
 *                                ongoing.
 *  @retval #UART2_STATUS_ETIMEOUT The write operation timed out.
 */
extern int_fast16_t UART2_writeTimeout(UART2_Handle handle, const void *buffer,
        size_t size, size_t *bytesWritten, uint32_t timeout);

/*!
 *  @brief  Function that cancels a UART2_write() function call.
 *
 *  This function cancels an asynchronous UART2_write() operation when
 *  write mode is #UART2_Mode_CALLBACK, or an ongoing UART2_write() in
 *  #UART2_Mode_NONBLOCKING.
 *  In callback mode, UART2_writeCancel() calls the registered
 *  write callback function no matter how many bytes were sent. It
 *  is the application's responsibility to check the count argument in the
 *  callback function and handle cases where only a subset of the bytes were
 *  sent. The callback function will be passed a status of
 *  #UART2_STATUS_ECANCELLED.
 *  In blocking mode, UART2_write() will return #UART2_STATUS_ECANCELLED.
 *
 *  @note The above applies to %UART2_writeTimeout() as well.
 *
 *  This API has no affect in non-blocking mode.
 *
 *  @param[in]  handle      A #UART2_Handle returned by UART2_open()
 */
extern void UART2_writeCancel(UART2_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_UART2__include */
