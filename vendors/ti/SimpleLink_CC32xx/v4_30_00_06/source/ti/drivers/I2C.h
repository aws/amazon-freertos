/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       I2C.h
 *  @brief      Inter-Integrated Circuit (I2C) Driver
 *
 *  @anchor ti_drivers_I2C_Overview
 *  # Overview
 *
 *  The I2C driver is designed to operate as an I2C master and will not
 *  function as an I2C slave. Multi-master arbitration is not supported;
 *  therefore, this driver assumes it is the only I2C master on the bus.
 *  This I2C driver's API set provides the ability to transmit and receive
 *  data over an I2C bus between the I2C master and I2C slave(s). The
 *  application is responsible for manipulating and interpreting the data.
 *
 *
 *  <hr>
 *  @anchor ti_drivers_I2C_Usage
 *  # Usage
 *
 *  This section provides a basic @ref ti_drivers_I2C_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_I2C_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  I2C APIs and their effect are provided in subsequent sections.
 *
 *  @anchor ti_drivers_I2C_Synopsis
 *  ## Synopsis #
 *  @anchor ti_drivers_I2C_Synopsis_Code
 *  @code
 *  // Import I2C Driver definitions
 *  #include <ti/drivers/I2C.h>
 *
 *  // Define name for an index of an I2C bus
 *  #define SENSORS 0
 *
 *  // Define the slave address of device on the SENSORS bus
 *  #define OPT_ADDR 0x47
 *
 *  // One-time init of I2C driver
 *  I2C_init();
 *
 *  // initialize optional I2C bus parameters
 *  I2C_Params params;
 *  I2C_Params_init(&params);
 *  params.bitRate = I2C_400kHz;
 *
 *  // Open I2C bus for usage
 *  I2C_Handle i2cHandle = I2C_open(SENSORS, &params);
 *
 *  // Initialize slave address of transaction
 *  I2C_Transaction transaction = {0};
 *  transaction.slaveAddress = OPT_ADDR;
 *
 *  // Read from I2C slave device
 *  transaction.readBuf = data;
 *  transaction.readCount = sizeof(data);
 *  transaction.writeCount = 0;
 *  I2C_transfer(i2cHandle, &transaction);
 *
 *  // Write to I2C slave device
 *  transaction.writeBuf = command;
 *  transaction.writeCount = sizeof(command);
 *  transaction.readCount = 0;
 *  I2C_transferTimeout(i2cHandle, &transaction, 5000);
 *
 *  // Close I2C
 *  I2C_close(i2cHandle);
 *  @endcode
 *
 *  @anchor ti_drivers_I2C_Examples
 *  ## Examples
 *
 *  @li @ref ti_drivers_I2C_Example_open "Getting an I2C bus handle"
 *  @li @ref ti_drivers_I2C_Example_write3bytes "Sending 3 bytes"
 *  @li @ref ti_drivers_I2C_Example_read5bytes "Reading 5 bytes"
 *  @li @ref ti_drivers_I2C_Example_writeread "Writing then reading in a single transaction"
 *  @li @ref ti_drivers_I2C_Example_callback "Using Callback mode"
 *
 *  @anchor ti_drivers_I2C_Example_open
 *  ## Opening the I2C Driver
 *
 *  After calling I2C_init(), the application can open an I2C instance by
 *  calling I2C_open().The following code example opens an I2C instance with
 *  default parameters by passing @p NULL for the #I2C_Params argument.
 *
 *  @code
 *  I2C_Handle i2cHandle;
 *
 *  i2cHandle = I2C_open(0, NULL);
 *
 *  if (i2cHandle == NULL) {
 *      // Error opening I2C
 *      while (1) {}
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_I2C_Example_write3bytes
 *  ## Sending three bytes of data.
 *
 *  @code
 *  I2C_Transaction i2cTransaction = {0};
 *  uint8_t writeBuffer[3];
 *
 *  writeBuffer[0] = 0xAB;
 *  writeBuffer[1] = 0xCD;
 *  writeBuffer[2] = 0xEF;
 *
 *  i2cTransaction.slaveAddress = 0x50;
 *  i2cTransaction.writeBuf = writeBuffer;
 *  i2cTransaction.writeCount = 3;
 *  i2cTransaction.readBuf = NULL;
 *  i2cTransaction.readCount = 0;
 *
 *  status = I2C_transfer(i2cHandle, &i2cTransaction);
 *
 *  if (status == false) {
 *      // Unsuccessful I2C transfer
 *      if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
 *          // I2C slave address not acknowledged
 *      }
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_I2C_Example_read5bytes
 *  ## Reading five bytes of data.
 *
 *  @code
 *  I2C_Transaction i2cTransaction = {0};
 *  uint8_t readBuffer[5];
 *
 *  i2cTransaction.slaveAddress = 0x50;
 *  i2cTransaction.writeBuf = NULL;
 *  i2cTransaction.writeCount = 0;
 *  i2cTransaction.readBuf = readBuffer;
 *  i2cTransaction.readCount = 5;
 *
 *  status = I2C_transfer(i2cHandle, &i2cTransaction);
 *
 *  if (status == false) {
 *      if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
 *          // I2C slave address not acknowledged
 *      }
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_I2C_Example_writeread
 *  ## Writing two bytes and reading four bytes in a single transaction.
 *
 *  @code
 *  I2C_Transaction i2cTransaction = {0};
 *  uint8_t readBuffer[4];
 *  uint8_t writeBuffer[2];
 *
 *  writeBuffer[0] = 0xAB;
 *  writeBuffer[1] = 0xCD;
 *
 *  i2cTransaction.slaveAddress = 0x50;
 *  i2cTransaction.writeBuf = writeBuffer;
 *  i2cTransaction.writeCount = 2;
 *  i2cTransaction.readBuf = readBuffer;
 *  i2cTransaction.readCount = 4;
 *
 *  status = I2C_transfer(i2cHandle, &i2cTransaction);
 *
 *  if (status == false) {
*       if (i2cTransaction->status == I2C_STATUS_ADDR_NACK) {
*           // slave address not acknowledged
*       }
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_I2C_Example_callback
 *  ## Using callback mode
 *  This final example shows usage of #I2C_MODE_CALLBACK, with queuing
 *  of multiple transactions. Because multiple transactions are simultaneously
 *  queued, separate #I2C_Transaction structures must be used. Each
 *  #I2C_Transaction will contain a custom application argument of a
 *  semaphore handle. The #I2C_Transaction.arg will point to the semaphore
 *  handle. When the callback function is called, the #I2C_Transaction.arg is
 *  checked for @p NULL. If this value is not @p NULL, then it can be assumed
 *  the @p arg is pointing to a valid semaphore handle. The semaphore handle
 *  is then used to call @p sem_post(). Hypothetically, this can be used to
 *  signal transaction completion to the task(s) that queued the
 *  transaction(s).
 *
 *  @code
 *  void callbackFxn(I2C_Handle handle, I2C_Transaction *msg, bool status)
 *  {
 *      // if transaction failed
 *      if (status == false) {
 *          if (msg->status == I2C_STATUS_ADDR_NACK) {
 *              // slave address not acknowledged
 *          }
 *          else if (msg->status == I2C_STATUS_CANCEL) {
 *              // transaction canceled by I2C_cancel()
 *          }
 *      }
 *
 *      // Check for a custom argument
 *      if (msg->arg != NULL) {
 *
 *          // In this example, the custom argument is a semaphore handle
 *          // Perform a semaphore post
 *          sem_post((sem_t *) (msg->arg));
 *      }
 *  }
 *  @endcode
 *
 *  Snippets of the thread code that initiates the transactions are shown below.
 *  Note the use of multiple #I2C_Transaction structures. The handle of the
 *  semaphore to be posted is specified via @p i2cTransaction2.arg.
 *  I2C_transfer() is called three times to initiate each transaction.
 *  Since callback mode is used, these functions return immediately. After
 *  the transactions have been queued, other work can be done. Eventually,
 *  @p sem_wait() is called causing the thread to block until the transaction
 *  completes. When the transaction completes, the application's callback
 *  function, @p callbackFxn will be called. Once #I2C_CallbackFxn posts the
 *  semaphore, the thread will be unblocked and can resume execution.
 *
 *  @code
 *  void thread(arg0, arg1)
 *  {
 *
 *      I2C_Transaction i2cTransaction0 = {0};
 *      I2C_Transaction i2cTransaction1 = {0};
 *      I2C_Transaction i2cTransaction2 = {0};
 *
 *      // ...
 *
 *      i2cTransaction0.arg = NULL;
 *      i2cTransaction1.arg = NULL;
 *      i2cTransaction2.arg = semaphoreHandle;
 *
 *      // ...
 *
 *      I2C_transfer(i2c, &i2cTransaction0);
 *      I2C_transfer(i2c, &i2cTransaction1);
 *      I2C_transfer(i2c, &i2cTransaction2);
 *
 *      // ...
 *
 *      sem_wait(semaphoreHandle);
 *  }
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_I2C_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_I2C__include
#define ti_drivers_I2C__include

/*! @cond */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup I2C_STATUS I2C Status codes
 *  These macros are reservations for I2C.h
 *  @{
 */

/*! @cond */
/*!
 * @private
 * Common I2C_control status code reservation offset.
 * I2C driver implementations should offset status codes with
 * #I2C_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define I2CXYZ_STATUS_ERROR0     I2C_STATUS_RESERVED - 0
 * #define I2CXYZ_STATUS_ERROR1     I2C_STATUS_RESERVED - 1
 * #define I2CXYZ_STATUS_ERROR2     I2C_STATUS_RESERVED - 2
 * @endcode
 */
#define I2C_STATUS_RESERVED        (-32)
/*! @endcond */

/*!
 * @brief  I2C transaction is queued but has not started
 */
#define I2C_STATUS_QUEUED          (1)

/*!
 * @brief  Successful status code returned by I2C API.
 */
#define I2C_STATUS_SUCCESS         (0)

/*!
 * @brief  Generic error status code returned by I2C API.
 */
#define I2C_STATUS_ERROR           (-1)

/*!
 * @brief  An error status code returned by I2C_control() for undefined
 * command codes.
 */
#define I2C_STATUS_UNDEFINEDCMD    (-2)

/*!
 * @brief  I2C operation timed-out
 */
#define I2C_STATUS_TIMEOUT         (-3)

/*!
 * @brief  I2C serial clock line timeout
 */
#define I2C_STATUS_CLOCK_TIMEOUT   (-4)

/*!
 * @brief  I2C slave address not acknowledged
 */
#define I2C_STATUS_ADDR_NACK       (-5)

/*!
 * @brief  I2C data byte not acknowledged
 */
#define I2C_STATUS_DATA_NACK       (-6)

/*!
 * @brief  I2C multi-master arbitration lost
 */
#define I2C_STATUS_ARB_LOST        (-7)

/*!
 * @brief  I2C transaction is in progress or returned without completing
 */
#define I2C_STATUS_INCOMPLETE      (-8)

/*!
 * @brief  I2C bus already in use by another controller. The I2C transaction
 * was therefore unable to start.
 */
#define I2C_STATUS_BUS_BUSY        (-9)

/*!
 * @brief  I2C transaction canceled by I2C_cancel()
 */
#define I2C_STATUS_CANCEL          (-10)

/*!
 * @brief  I2C transaction is invalid. This may occur if:
 *   1. The #I2C_Transaction.readCount and #I2C_Transaction.writeCount are
 *      both set to 0.
 *   2. A call to I2C_transfer() is made from a #I2C_CallbackFxn while queued
 *      transactions are being canceled. See also: I2C_cancel()
 */
#define I2C_STATUS_INVALID_TRANS   (-11)
/** @} */

/*!
 *  @brief  Wait forever define used to specify timeouts.
 */
#define I2C_WAIT_FOREVER           (~(0U))

/*!
 *  @brief      A handle that is returned from an I2C_open() call.
 */
typedef struct I2C_Config_ *I2C_Handle;

/*!
 *  @brief  Defines a transaction to be used with I2C_transfer() or
 *          I2C_transferTimeout()
 *
 *  After a call to I2C_transfer(), the #I2C_Transaction.status reflects
 *  the current transfer status.
 *
 *  @sa I2C_transfer(), I2C_transferTimeout()
 */
typedef struct {
    /*!
     *  Pointer to a buffer of at least #I2C_Transaction.writeCount bytes.
     *  If #I2C_Transaction.writeCount is 0, this pointer is not used.
     */
    void         *writeBuf;

    /*!
     *  Number of bytes to write to the I2C slave device. A value of 0
     *  indicates no data will be written to the slave device and only a read
     *  will occur. If this value
     *  is not 0, the driver will always perform the write transfer first.
     *  The data written to the I2C bus is preceded by the
     *  #I2C_Transaction.slaveAddress with the write bit set. If
     *  @p writeCount bytes are successfully sent and
     *  acknowledged, the transfer will complete or perform a read--depending
     *  on #I2C_Transaction.readCount.
     *
     *  @note Both #I2C_Transaction.writeCount and #I2C_Transaction.readCount
     *  can not be 0.
     */
    size_t        writeCount;

    /*!
     *  Pointer to a buffer of at least #I2C_Transaction.readCount bytes.
     *  If #I2C_Transaction.readCount is 0, this pointer is not used.
     */
    void         *readBuf;

    /*!
     *  Number of bytes to read from the I2C slave device. A value of 0
     *  indicates no data will be read and only a write will occur. If
     *  #I2C_Transaction.writeCount is not 0, this driver will perform the
     *  write first, followed by the read. The data read from the bus is
     *  preceded by the #I2C_Transaction.slaveAddress with the read bit set.
     *  After @p readCount bytes are successfully read, the transfer will
     *  complete.
     *
     *  @note Both #I2C_Transaction.writeCount and #I2C_Transaction.readCount
     *  can not be 0.
     */
    size_t        readCount;

    /*!
     * Pointer to a custom argument to be passed to the #I2C_CallbackFxn
     * function via the #I2C_Transaction structure.
     *
     * @note The #I2C_CallbackFxn function is only called when operating in
     * #I2C_MODE_CALLBACK.
     *
     * @sa  #I2C_MODE_CALLBACK
     * @sa  #I2C_CallbackFxn
     */
    void         *arg;

    /*!
     *  I2C status of the current transaction. The status may be used to
     *  determine why a transaction failed. Potential codes are:
     *      @li #I2C_STATUS_SUCCESS
     *      @li #I2C_STATUS_ERROR
     *      @li #I2C_STATUS_TIMEOUT
     *      @li #I2C_STATUS_CLOCK_TIMEOUT
     *      @li #I2C_STATUS_ADDR_NACK
     *      @li #I2C_STATUS_DATA_NACK
     *      @li #I2C_STATUS_ARB_LOST
     *      @li #I2C_STATUS_INCOMPLETE
     *      @li #I2C_STATUS_BUS_BUSY
     *      @li #I2C_STATUS_CANCEL
     *      @li #I2C_STATUS_INVALID_TRANS
     *
     *  This status may also be used to determine if a transaction is queued
     *  (#I2C_STATUS_QUEUED) or in progress (#I2C_STATUS_INCOMPLETE).
     */
    volatile int_fast16_t status;

    /*!
     *  I2C slave address used for the transaction. The slave address is
     *  the first byte transmitted during an I2C transfer. The read/write bit
     *  is automatically set based upon the #I2C_Transaction.writeCount and
     *  #I2C_Transaction.readCount.
     */
    uint_least8_t slaveAddress;

    /*!
     *  @private This is reserved for use by the driver and must never be
     *  modified by the application.
     */
    void         *nextPtr;
} I2C_Transaction;

/*!
 *  @brief    Return behavior of I2C_Transfer() specified in the #I2C_Params.
 *
 *  This enumeration defines the return behaviors for a call to I2C_transfer().
 *
 *  @sa  #I2C_Params
 */
typedef enum {
    /*!
     *  In #I2C_MODE_BLOCKING, calls to I2C_transfer() block until the
     *  #I2C_Transaction completes. Other threads calling I2C_transfer()
     *  while a transaction is in progress are also placed into a blocked
     *  state. If multiple threads are blocked, the thread with the highest
     *  priority will be unblocked first. This implies that arbitration
     *  will not be executed in chronological order.
     *
     *  @note When using #I2C_MODE_BLOCKING, I2C_transfer() must be called
     *  from a thread context.
     */
    I2C_MODE_BLOCKING,

    /*!
     *  In #I2C_MODE_CALLBACK, calls to I2C_transfer() return immediately. The
     *  application's callback function, #I2C_Params.transferCallbackFxn, is
     *  called when the transaction is complete. Sequential calls to
     *  I2C_transfer() will place #I2C_Transaction structures into an
     *  internal queue. Queued transactions are automatically started after the
     *  previous transaction has completed. This queuing occurs regardless of
     *  any error state from previous transactions. The transactions are
     *  always executed in chronological order. The
     *  #I2C_Params.transferCallbackFxn function will be called asynchronously
     *  as each transaction is completed.
     */
    I2C_MODE_CALLBACK
} I2C_TransferMode;

/*!
 *  @brief  The definition of a callback function.
 *
 *  When operating in #I2C_MODE_CALLBACK, the callback function is called
 *  when an I2C_transfer() completes. The application is responsible for
 *  declaring an #I2C_CallbackFxn function and providing a pointer
 *  in #I2C_Params.transferCallbackFxn.
 *
 *  @warning  The callback function is called from an interrupt context.
 *
 *  @param[out]  handle    #I2C_Handle used with the initial call to
 *  I2C_transfer()
 *
 *  @param[out]  transaction    Pointer to the #I2C_Transaction structure used
 *  with the initial call to I2C_transfer(). This structure contains the
 *  custom argument specified by @p transaction.arg and the transaction status.
 *
 *  @param[out]  transferStatus    Boolean indicating if the I2C transaction
 *  was successful. If @p true, the transaction was successful. If @p false,
 *  the transaction failed.
 */
typedef void (*I2C_CallbackFxn)(I2C_Handle handle, I2C_Transaction *transaction,
    bool transferStatus);

/*!
 *  @brief  Bit rate for an I2C driver instance specified in the #I2C_Params.
 *
 *  This enumeration defines the bit rates used with an I2C_transfer().
 *
 *  @note You must check that the device specific implementation supports the
 *  desired #I2C_BitRate.
 */
typedef enum {
    I2C_100kHz     = 0,    /*!< I2C Standard-mode. Up to 100 kbit/s. */
    I2C_400kHz     = 1,    /*!< I2C Fast-mode. Up to 400 kbit/s. */
    I2C_1000kHz    = 2,    /*!< I2C Fast-mode Plus. Up to 1Mbit/s. */
    I2C_3330kHz    = 3,    /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
    I2C_3400kHz    = 3,    /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
} I2C_BitRate;

/*!
 *  @brief I2C parameters used with I2C_open().
 *
 *  I2C_Params_init() must be called prior to setting fields in
 *  this structure.
 *
 *  @sa  I2C_Params_init()
 */
typedef struct {
    /*! #I2C_TransferMode for all I2C transfers. */
    I2C_TransferMode transferMode;

    /*!
     *  Pointer to a #I2C_CallbackFxn to be invoked after a
     *  I2C_transfer() completes when operating in #I2C_MODE_CALLBACK.
     */
    I2C_CallbackFxn transferCallbackFxn;

    /*!
     * A #I2C_BitRate specifying the frequency at which the I2C peripheral
     * will transmit data during a I2C_transfer().
     */
    I2C_BitRate bitRate;

    /*! Pointer to a device specific extension of the #I2C_Params */
    void *custom;
} I2C_Params;

/*! @cond NODOC */
#define I2C_BASE_OBJECT \
    /* I2C control variables */ \
    I2C_TransferMode           transferMode;        /* Blocking or Callback mode */ \
    I2C_CallbackFxn            transferCallbackFxn; /* Callback function pointer */ \
    I2C_Transaction           *currentTransaction;  /* Ptr to current I2C transaction */ \
    \
    /* I2C transaction pointers for I2C_MODE_CALLBACK */ \
    I2C_Transaction * volatile headPtr;            /* Head ptr for queued transactions */ \
    I2C_Transaction           *tailPtr;            /* Tail ptr for queued transactions */ \
    \
    /* I2C RTOS objects */ \
    HwiP_Struct                hwi;                /* Hwi object handle */ \
    SemaphoreP_Struct          mutex;              /* Grants exclusive access to I2C */ \
    SemaphoreP_Struct          transferComplete;   /* Signal I2C transfer complete */ \
    \
    /* Read and write variables */ \
    const uint8_t             *writeBuf;           /* Internal inc. writeBuf index */ \
    size_t                     writeCount;         /* Internal dec. writeCounter */ \
    uint8_t                   *readBuf;            /* Internal inc. readBuf index */ \
    size_t                     readCount;          /* Internal dec. readCounter */ \
    \
    bool                       isOpen;             /* Flag to show module is open */ \
/*! @endcond */

/*!
 *  @cond NODOC
 *  I2C Object. Applications must not access any member variables of
 *  this structure!
 */
typedef struct {
    I2C_BASE_OBJECT
} I2C_Object;
/*! @endcond */

/*! @cond NODOC */
#define I2C_BASE_HWATTRS \
    /*! I2C Peripheral's base address */ \
    uint32_t baseAddr; \
    /*! I2C Peripheral's interrupt vector */ \
    uint32_t intNum; \
    /*! I2C Peripheral's interrupt priority. \
     * \
     *  Note for CC26XX: The CC26XX uses three of the priority bits, \
     *  meaning ~0 has the same effect as (7 << 5). \
     * \
     *  (7 << 5) will apply the lowest priority. \
     *  (1 << 5) will apply the highest priority. \
     * \
     *  Setting the priority to 0 is not supported by the I2CCC26XX driver. \
     * \
     *  Hwi's with priority 0 ignore the Hwi dispatcher to support zero-latency \
     *  interrupts, thus invalidating the critical sections in this driver. \
     */ \
    uint32_t intPriority;
/*! @endcond */

/*!
 *  @cond NODOC
 *  I2C HWAttrs.
 */
typedef struct {
    I2C_BASE_HWATTRS
} I2C_HWAttrs;
/*! @endcond */

/*!
 *  @brief I2C driver's custom @ref driver_configuration "configuration"
 *  structure.
 *
 *  @sa     I2C_init()
 *  @sa     I2C_open()
 */
typedef struct I2C_Config_ {
    /*! Pointer to a driver specific @ref driver_objects "data object". */
    void               *object;

    /*! Pointer to a driver specific @ref driver_hardware_attributes
     *  "hardware attributes structure". */
    void         const *hwAttrs;
} I2C_Config;

extern const I2C_Config I2C_config[];
extern const uint_least8_t I2C_count;

/*!
 *  @brief  Cancels all I2C transfers
 *
 *  This function will cancel asynchronous I2C_transfer() operations by
 *  generating a STOP condition on the I2C bus.
 *
 *  Calls to I2C_cancel() return immediately; however, the transaction
 *  may not yet be canceled.
 *
 *  For #I2C_MODE_BLOCKING, the current transaction is canceled.
 *
 *  For #I2C_MODE_CALLBACK mode, the in progress transfer, as
 *  well as any queued transfers, will be canceled. The individual callback
 *  functions for each transfer will be called in chronological order. The
 *  callback functions are called in an interrupt context. Additional calls
 *  to I2C_transfer() invoked from the callback function of a canceled
 *  transaction will always fail. In such cases, the #I2C_Transaction.status
 *  will indicate #I2C_STATUS_INVALID_TRANS.
 *
 *  A canceled transaction may be identified when the #I2C_Transaction.status
 *  is set to #I2C_STATUS_CANCEL.
 *
 *  @note This API may not handle cases where the I2C slave holds the clock
 *  line indefinitely.
 *
 *  @pre    I2C_Transfer() has been called.
 *
 *  @param[in]  handle  An #I2C_Handle returned from I2C_open()
 *
 *  @note   Different I2C slave devices will behave differently when an
 *          in-progress transfer fails and needs to be canceled. The slave
 *          may need to be reset, or there may be other slave-specific
 *          steps that can be used to successfully resume communication.
 *
 *  @sa  I2C_transfer()
 *  @sa  #I2C_MODE_CALLBACK
 */
extern void I2C_cancel(I2C_Handle handle);

/*!
 *  @brief  Function to close an I2C driver instance
 *
 *  @pre  I2C_open() has been called.
 *
 *  @param[in]  handle  An #I2C_Handle returned from I2C_open()
 */
extern void I2C_close(I2C_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a
 *          driver instance.
 *
 *  @pre    I2C_open() has to be called first.
 *
 *  @param[in]  handle   An #I2C_Handle returned from I2C_open()
 *
 *  @param[in]  cmd     A command value defined by the device specific
 *                      implementation
 *
 *  @param[in]  controlArg    An optional R/W (read/write) argument that is
 *                            accompanied with @p cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @retval #I2C_STATUS_SUCCESS The call was successful.
 *  @retval #I2C_STATUS_UNDEFINEDCMD The @p cmd value is not supported by
 *                                   the device specific implementation.
 */
extern int_fast16_t I2C_control(I2C_Handle handle, uint_fast16_t cmd,
    void *controlArg);

/*!
 *  @brief  Function to initialize the I2C driver.
 *
 *  This function must also be called before any otherI2C driver APIs.
 */
extern void I2C_init(void);

/*!
 *  @brief  Open an I2C driver instance.
 *
 *  @pre    I2C_init() has been called.
 *
 *  @param[in]  index    Index in the @p I2C_Config[] array.
 *
 *  @param[in]  params    Pointer to an initialized #I2C_Params structure.
 *                        If NULL, the default #I2C_Params values are used.
 *
 *  @return An #I2C_Handle on success, or @p NULL on an error.
 *
 *  @sa     I2C_init()
 *  @sa     I2C_close()
 */
extern I2C_Handle I2C_open(uint_least8_t index, I2C_Params *params);

/*!
 *  @brief  Initialize an #I2C_Params structure to its default values.
 *
 *  @param[in]  params    A pointer to #I2C_Params structure for
 *                        initialization.
 *
 *  Defaults values are:
 *  @arg #I2C_Params.transferMode = #I2C_MODE_BLOCKING
 *  @arg #I2C_Params.transferCallbackFxn = @p NULL
 *  @arg #I2C_Params.bitRate = #I2C_100kHz
 *  @arg #I2C_Params.custom = @p NULL
 */
extern void I2C_Params_init(I2C_Params *params);

/*!
 *  @brief  Set the I2C SCL clock timeout.
 *
 *  An I2C slave can extend a I2C transaction by periodically pulling the
 *  clock low to create a slow bit transfer rate. The application can use this
 *  API to program a counter in the I2C module. The count is used to force a
 *  timeout if an I2C slave holds the clock line low for longer than the
 *  @p timeout duration. An #I2C_STATUS_CLOCK_TIMEOUT status indicates a
 *  timeout event occured.
 *
 *  @param[in]  handle      An #I2C_Handle returned from I2C_open()
 *
 *  @param[in]  timeout     Timeout in units of I2C clock cycles. Refer to
 *                          the device specifc reference manual to determine
 *                          how to calculate the timeout value.
 *
 *  @return  Possible return values include:
 *            @li #I2C_STATUS_SUCCESS
 *            @li #I2C_STATUS_ERROR
 *
 *  @sa I2C_transfer()
 */
extern int_fast16_t I2C_setClockTimeout(I2C_Handle handle, uint32_t timeout);

/*!
 *  @brief  Perform an I2C transaction with an I2C slave peripheral.
 *
 *  This function will perform an I2C transfer, as specified by an
 *  #I2C_Transaction structure.
 *
 *  @note When using #I2C_MODE_BLOCKING, this must be called from a thread
 *  context.
 *
 *  @param[in]  handle      An #I2C_Handle returned from I2C_open()
 *
 *  @param[in]  transaction  A pointer to an #I2C_Transaction. The application
 *  is responsible for allocating and initializing an #I2C_Transaction
 *  structure prior to passing it to I2C_Transfer(). This
 *  structure must persist in memory unmodified until the transfer is complete.
 *
 *  @note #I2C_Transaction structures cannot be re-used until the previous
 *  transaction has completed. Upon the completion of a transaction, the
 *  #I2C_Transaction.status may be used for error handling.
 *
 *  @return In #I2C_MODE_BLOCKING: @p true for a successful transfer; @p false
 *          for an error (for example, an I2C bus fault (NACK)).
 *
 *  @return In #I2C_MODE_CALLBACK: always @p true. The #I2C_CallbackFxn @p bool
 *          argument will be @p true to indicate success, and @p false to
 *          indicate an error.
 *
 *  @pre I2C_open() has been called.
 *
 *  @sa  I2C_open(), I2C_transferTimeout()
 *  @sa  I2C_Transaction
 */
extern bool I2C_transfer(I2C_Handle handle, I2C_Transaction *transaction);

/*!
 *  @brief  Perform an I2C transaction with an I2C slave peripheral.
 *
 *  This function will perform an I2C transfer, as specified by an
 *  #I2C_Transaction structure. If the timeout is exceeded, then the
 *  I2C transaction is canceled.
 *
 *  @note When using #I2C_MODE_BLOCKING, this must be called from a thread
 *  context.
 *
 *  @note  The timeout restriction is only applied when using
 *  #I2C_MODE_BLOCKING. If using #I2C_MODE_CALLBACK, the application should
 *  manage timeouts using I2C_cancel(). Additionally, this timeout may not
 *  handle cases where the I2C slave holds the clock line indefinitely.
 *
 *  @param[in]  handle    An #I2C_Handle returned from I2C_open()
 *
 *  @param[in]  transaction  A pointer to an #I2C_Transaction. The application
 *  is responsible for allocating and initializing an #I2C_Transaction
 *  structure prior to passing it to I2C_TransferTimeout(). This
 *  structure must persist in memory unmodified until the transfer is complete.
 *
 *  @param[in]  timeout    The time in system ticks to wait for the transaction
 *  to complete. Passing I2C_WAIT_FOREVER into this parameter will cause
 *  I2C_transferTimeout() to behave the same as I2C_transfer() but with a
 *  more detailed return status.
 *
 *  @note #I2C_Transaction structures cannot be re-used until the previous
 *  transaction has completed. Upon the completion of a transaction, the
 *  #I2C_Transaction.status may be used for error handling.
 *
 *  @return  In #I2C_MODE_CALLBACK: always @p I2C_STATUS_SUCCESS.
 *           The #I2C_CallbackFxn @p transferStatus argument will be @p true
 *           to indicate success, and @p false to indicate an error.
 *
 *  @return  In #I2C_MODE_BLOCKING: Possible return values include:
 *            @li #I2C_STATUS_SUCCESS
 *            @li #I2C_STATUS_ERROR
 *            @li #I2C_STATUS_TIMEOUT
 *            @li #I2C_STATUS_CLOCK_TIMEOUT
 *            @li #I2C_STATUS_ADDR_NACK
 *            @li #I2C_STATUS_DATA_NACK
 *            @li #I2C_STATUS_ARB_LOST
 *            @li #I2C_STATUS_INCOMPLETE
 *            @li #I2C_STATUS_BUS_BUSY
 *            @li #I2C_STATUS_CANCEL
 *            @li #I2C_STATUS_INVALID_TRANS
 *
 *  @pre I2C_open() has been called.
 *
 *  @sa  I2C_open(), I2C_transfer()
 *  @sa  I2C_Transaction
 */
extern int_fast16_t I2C_transferTimeout(I2C_Handle handle,
    I2C_Transaction *transaction, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_I2C__include */
