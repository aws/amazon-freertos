/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file    iot_sdio.h
 * @brief   This file contains all the SDIO HAL API definitions.
 *          Here we define SDIO driver as a protocol layer driver which builds on top of SDMMC
 *          host driver. SDMMC host driver operates at physical layer and provide APIs to send
 *          commands to the slave device and receive command responses, send and receive data,
 *          and handle error conditions on the bus.
 *
 *          SD/SDIO/MMC driver can be built on top of SDMMC host. This interface only define
 *          the APIs for SDIO applications, the interface can be expanded to support SD/MMC either
 *          by expanding the APIs in this file or adding separate files for SD/MMC.
 *
 * @design consideration
 *          SDIO HAL APIs performs protocol layer tasks between SDIO host and SDIO card.
 *          Some APIs perform a single task and some APIs perform a sequence of tasks.
 *          For examples, iot_sdio_io_read_direct() API performs a single task of reading
 *          one byte from card register, and iot_sdio_card_connect() API performs all tasks
 *          required by card initialization as specified in section 3.1.2 in "SD Specification
 *          Part E1 SDIO".
 *
 *          Upon calling iot_sdio_open() API, the user is a given a pxSdioHandle, a
 *          reference to the SDIO card slot that the user is interested in. Thereafter, user
 *          uses this handle to call other APIs to perform tasks associated with the target
 *          card slot. For example, iot_sdio_check_card_presence_status(pxSdioHandle, true)
 *          checks SDIO card presence, and iot_sdio_io_write_extended(pxSdioHandle)
 *          writes data to SDIO card in the slot, etc.
 *
 * @limitation
 *          SDIO HAL APIs are intended to be used in embedded systems where each SDIO host
 *          controller is connected to either a single hot swappable sido card slot or multiple
 *          eSDIO devices in a Shared Bus Configuration (Refer to Section 7.3 of "SD Specification
 *          Part E1 SDIO").
 *
 *          These APIs are not suitable for systems where a single SDIO host controller supports
 *          multiple hot swappable slots. With multiple hot swappable slots available on a single
 *          host controller, it is not possible pre-determine which card slot SDIO device will be
 *          inserted, and know which card slot handle to request. Since this type of host controller
 *          is very rare even in non-embedded world, it is not worth to add complexity to SDIO HAL
 *          APIs to support it.
 */

#ifndef _IOT_SDIO_H_
#define _IOT_SDIO_H_

#include <stdint.h>
#include <stdbool.h>


/**
 * @defgroup iot_sdio SDIO HAL APIs
 * @{
 */

/**
 * @brief The return codes for the functions in SDIO
 */
/*!< SDIO operation completed successfully. */
#define IOT_SDIO_SUCCESS                       ( 0 )

/*!< SDIO ioctl failed. */
#define IOT_SDIO_IOCTL_FAIL                    ( 1 )

/*!< SDIO host controller initialization failed. */
#define IOT_SDIO_HOST_INIT_FAIL                ( 2 )

/*!< At least one parameter is invalid. */
#define IOT_SDIO_INVALID_VALUE                 ( 3 )

/*!< Sending command to or receive response from card failed. */
#define IOT_SDIO_TRANSFER_FAIL                 ( 4 )

/*!< If the card is initialized in 3.3V signaling, and card support high speed
 *   mode (SHS ==1), host can issue CMD52 in RAW mode, setting EHS bit in CCCR
 *   to one to switch to high speed mode. This error code indicates that the
 *   sdio card supports high speed mode but failed to switch to high speed
 *   mode. The bus clock rate remains unchanged in this case. */
#define IOT_SDIO_SWITCH_HIGH_SPEED_FAIL        ( 5 )

/*!< If the card is initialized in 3.3V signaling, and card support high speed
 *   mode (SHS ==1), host can issue CMD52 in RAW mode, setting EHS bit in CCCR
 *   to one to switch to high speed mode. This error code indicates that the
 *   sdio card does not supports high speed mode. The bus clock rate will be
 *   set to the default 25MHz in this case. */
#define IOT_SDIO_LOW_SPEED_CARD                ( 6 )

/*!< If the card is initialized in 1.8V signaling, and card support UHS-I, host
 *   will try to set card's bus timing mode to the highest mode the card
 *   supports using CMD52, and set the max clock frequence for that mode. This
 *   error code indicates a failure of such operation */
#define IOT_SDIO_SELECT_BUS_TIMING_FAIL        ( 7 )

/*!< Setting block size for block transfer mode failed. */
#define IOT_SDIO_SET_CARD_BLOCK_SIZE_FAIL      ( 8 )

/*!< During card initializaton, A host that supports UHS-I use CMD5 to probe if
 *   card also supports UHS-I and ready to switch from 3.3v to 1.8v. Once
 *   voltage switch request is accepted, host sends CMD11 to initiate voltage
 *   switch sequence. This error code indicates either card failed to respond
 *   to CMD11 or card responded to CMD11 but failed to switch voltage.*/
#define IOT_SDIO_SWITCH_VOLTAGE_FAIL           ( 9 )

/*!< host controller not ready. */
#define IOT_SDIO_HOST_NOT_READY                ( 10 )

/*!< Not a valid SDIO card. */
#define IOT_SDIO_INVALID_CARD                  ( 11 )

/*!< Send Relative Address (CMD3) failed. */
#define IOT_SDIO_SEND_RELATIVE_ADDRESS_FAIL    ( 12 )

/*!< Select Card (CMD7) failed. */
#define IOT_SDIO_SELECT_CARD_FAIL              ( 13 )

/*!< Read (via CMD52) CIS (Card Information
 *   Structure failed. */
#define IOT_SDIO_READ_CIS_FAIL                 ( 14 )

/*!< Set (via CMD52) data bus width failed. */
#define IOT_SDIO_SET_DATA_BUS_WIDTH_FAIL       ( 15 )

/*!< Card does not support Asynchronous Interrupt. */
#define IOT_SDIO_ASYNC_INT_NOT_SUPPORTED       ( 16 )

/*!< Reading CCCR (function 0) or FBR (function 1-7) failed*/
#define IOT_SDIO_GET_CARD_CAPABILITY_FAIL      ( 17 )

/*!< Api function is not supported by platform */
#define IOT_SDIO_FUNCTION_NOT_SUPPORTED        ( 18 )

/**
 * @brief sdio io bus width
 */

#define IOT_SDIO_BUS_1BIT    ( 0 )     /*!< 1 bit bus mode */
/*!< 1 is reserved per SDIO specification*/
#define IOT_SDIO_BUS_4BIT    ( 2 )     /*!< 4 bit bus mode */
#define IOT_SDIO_BUS_8BIT    ( 3 )     /*!< 8 bit bus mode */

/**
 * @brief sdio io read/write direction
 */
typedef enum IotSdioDirection
{
    eIORead = 0U,  /*!< io read */
    eIOWrite = 1U, /*!< io write */
} IotSdioDirection_t;

/**
 * @brief SDIO I/O function number type
 *        Each SDIO card can support up to 7 I/O functions.
 *        Each I/O function can implement one of the following:
 *          - SDIO Standard UART
 *          - SDIO Bluetooth Type-B standard interface
 *          - SDIO GPS standard interface
 *          - SDIO Camera standard interface
 *          - SDIO PHS standard interface
 *          - SDIO WLAN interface
 *          - Embedded SDIO-ATA standard interface
 *          - SDIO Bluetooth Type-A AMP standard interface
 *          - Other SDIO standard inferface type identified by
 *            Extended SDIO Standard Function interface code
 */
typedef enum IotSdioFunction
{
    eSdioFunction0,      /*!< refer to common register area present on all I/O cards*/
    eSdioFunction1,      /*!< refer to register area of 1st I/O function if card supports it*/
    eSdioFunction2,      /*!< refer to register area of 2nd I/O function if card supports it*/
    eSdioFunction3,      /*!< refer to register area of 3rd I/O function if card supports it*/
    eSdioFunction4,      /*!< refer to register area of 4th I/O function if card supports it*/
    eSdioFunction5,      /*!< refer to register area of 5th I/O function if card supports it*/
    eSdioFunction6,      /*!< refer to register area of 6th I/O function if card supports it*/
    eSdioFunction7,      /*!< refer to register area of 7th I/O function if card supports it*/
    eSdioFunctionMemory, /*!< refer to register area of memory function of a combo card*/
} IotSdioFunction_t;

/**
 * @brief card detect type
 */
typedef enum IotSdioCardDetectType
{
    eDetectCardByGpioCD,    /*!< sdio card detect by CD pin through GPIO */
    eDetectCardByHostCD,    /*!< sdio card detect by CD pin through host */
    eDetectCardByHostDATA3, /*!< sdio card detect by DAT3 pin through host */
    eDetectCardNonStandard, /*!< sdio card detect by non standard method
                             *   certain sdio host may support */
} IotSdioCardDetectType_t;

/**
 * @brief common notify event types in sdio callback
 */
typedef enum IotSdioEventType
{
    eSdioCardInterruptEvent, /*!< Interrupt received from sdio card */
    eSdioCardInsertedEvent,  /*!< Sdio card insertion is detected */
    eSdioCardRemovedEvent,   /*!< Sdio card removal is detected */
    eSdioCardPowerOnEvent,   /*!< Powering on sdio card */
    eSdioCardPowerOffEvent,  /*!< Powering off sdio card */
} IotSdioEventType_t;

/**
 * @brief IOCTL request types.
 */
typedef enum IotSdioIoctlRequest
{
    eSDIOSetDriverStrength,   /*!< Set driver strength type
                               *   input: SDIO card driver strength.
                               *          input data type IotSdioSdDriverStrength_t */
    eSDIOSetCardInactive,     /*!< put card in inactive state */
    eSDIOSwitchToHighSpeed,   /*!< switch to high speed mode if card supports it */
    eSDIOSetDataBusWidth,     /*!< set sdio card data bus width and change host setting to support it
                               *   input: sdio bus width, input data type IotSdioBusWidth_t */
    eSDIOGetCardCapabilities, /*!< request host to learn capabilities of specified card function
                               *   input: I/O function number, of type IotSdioFunction_t*/
    eSDIOSetCardDetectParams, /*!< Set card detect parameters
                               *   input: card detect parameters.
                               *          input data type IotSdioCardDetectParam_t*/
    eSDIOSetFuncBlockSize,    /*!< Set transfer block size for a function
                               *   input: I/O function and block size.
                               *          input data type IotSdioFuncBlkSize_t*/
    eSDIOEnableIOFunctionIrq, /*!< enable/disable irq for individual io functions
                               *   input: I/O function number and enable or disable,
                               *          input data type IotSdioPerFuncEnable */
    eSDIOEnableAsyncIrqMode,  /*!< enable/disable asynchronous interrupt mode
                               *    input: boolean to indicate enable or disable */
    eSDIOEnableIo,            /*!< enable/disable an I/O function on card
                               *   input: I/O function number and boolean to indicate enable
                               *          or disable, input data type IotSdioPerFuncEnable_t */
    eSDIOSelectIo,            /*!< Select an I/O function for suspend/resume
                               *   input: I/O function number, input data type IotSdioFunction_t */
    eSDIOAbortIo,             /*!< Abort an ongoing transfer initiated by the specified I/O function
                               *   input: I/O function number. Input data type IotSdioFunction_t */
    eSDIOReadCis,             /*!< Host read list of tuples from card CIS
                               *   input: I/O function number and tuple list to read,
                               *          input data type IotSdioReadCis_t*/
    eSDIOPowerOnOffCard,      /*!< Power on/off card
                               *   input: boolean true for power on, false for power off
                               *          boolean true to use board's default power method,
                               *          false to use user defined power method via callback.
                               *          input data type IoTSdioPowerOnOffCard_t */
    eSDIOCheckCardPresence,   /*!< check card presence
                               *   output: boolean to indicate card present or not present*/
} IotSdioIoctlRequest_t;

/**
 * @brief sdio bus width, input parameter type for eSDIOSetDataBusWidth ioctl request
 */
typedef enum IotSdioBusWidth
{
    eDataBus1Bit = IOT_SDIO_BUS_1BIT, /*!< 1 bit bus mode */
    eDataBus4Bit = IOT_SDIO_BUS_4BIT, /*!< 4 bit bus mode*/
    eDataBus8Bit = IOT_SDIO_BUS_8BIT, /*!< 8 bit bus mode*/
} IotSdioBusWidth_t;

/**
 * @brief input parameter type for eSDIOSetCardDetectParams ioctl request
 */
typedef struct IoTSdioCardDetectParam
{
    IotSdioCardDetectType_t eCardDetectType; /*!< One of the Sd card detect types supported by sdio host */
    bool bCallbackOnInsert;                  /*!< If true sdio callback will be invoked on card insert. */
    bool bCallbackOnRemoval;                 /*!< If true sdio callback will be invoked on card card removal. */
} IotSdioCardDetectParam_t;

/**
 * @brief input parameter type for eSDIOPowerOnOffCard ioctl request
 */
typedef struct IoTSdioPowerOnOffCard
{
    bool bPowerOn;     /*!< True to power on card, false to power off card. */
    bool bUseCallback; /*!< indicate if user callback should be used to power on/off card. */
} IoTSdioPowerOnOffCard_t;

/**
 * @brief input parameter type for eSDIOSetFuncBlockSize ioctl request
 */
typedef struct IotSdioFuncBlkSize
{
    IotSdioFunction_t eFunc; /*!< IO function number range from eSdioFunction0 to eSdioFunction7*/
    uint32_t ulBlockSize;    /*!< Block size to be set for I/O function. Must not be greater
                              *   than the maximum block size supported by IO function.*/
} IotSdioFuncBlkSize_t;

/**
 * @brief input parameter type for eSDIOEnableIOFunctionIrq ioctl request
 *        and for eSDIOEnableIo ioctl request
 */
typedef struct IotSdioPerFuncEnable
{
    IotSdioFunction_t eFunc; /*!< IO function number range from eSdioFunction0 to eSdioFunction7 */
    bool bEnable;            /*!< true to enable, false to disable */
} IotSdioPerFuncEnable_t;

/**
 * @brief input parameter type for eSDIOReadCis ioctl request
 */
typedef struct IotSdioReadCis
{
    IotSdioFunction_t eFunc;       /*!< IO function number range from eSdioFunction0 to eSdioFunction7 */
    const uint32_t * pulTupleList; /*!< Point to a list of tuple codes to be read.
                                    *   Tuple code that is not supported by sdio card will be ignored. */
    uint32_t ulTupleNum;           /*!< Number of tuple codes in the list. */
} IotSdioReadCis_t;

/**
 * @brief SDIO card driver strength, input parameter type for eSDIOSetDriverStrength ioctl request
 */
typedef enum IotSdioSdDriverStrength
{
    eSdDriverStrengthTypeB = 0U, /*!< default driver strength (50 ohm x1)*/
    eSdDriverStrengthTypeA = 1U, /*!< driver strength TYPE A  (33 ohm x1.5)*/
    eSdDriverStrengthTypeC = 2U, /*!< driver strength TYPE C  (66 ohm x0.75)*/
    eSdDriverStrengthTypeD = 3U, /*!< driver strength TYPE D  (100 ohm x0.5)*/
} IotSdioSdDriverStrength_t;

/**
 * @brief SDIO descriptor type defined in the source file.
 */
struct IotSdioSlotDescriptor;

/**
 * @brief IotSdioSlotHandle_t type is the SDIO slot handle returned by calling iot_sdio_open()
 */
typedef struct IotSdioSlotDescriptor * IotSdioSlotHandle_t;

/**
 * @brief The callback function for sdio event operation. This callback is passed to
 *        driver by using iot_sdio_set_callback API. It's called when one of the events
 *        (IotSdioEventType_t) happens.
 *
 * @param[in] pxSdioHandle  Handle to sdio card slot instance returned in iot_sdio_open().
 * @param[in] eSdioEvent    sdio asynchronous event.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef void (* IotSdioCallback_t) ( IotSdioSlotHandle_t const pxSdioHandle,
                                     IotSdioEventType_t eSdioEvent,
                                     void * pvUserContext );

/**
 * @brief SDIO I/O function callback type.  User can call iot_sdio_set_io_function_callback() API
 *        to set one IOFunctionCallback per each standard SDIO function supported on a sdio card.
 *
 *        Since the I/O function number is parameter to IOFunctionCallback, user can implement one
 *        single IOFunctionCallback and set it for all I/O functions. Inside the IOFunctionCallback,
 *        different action can be taken according the I/O function number.
 *
 *        When there is a sdio card interrupt (eSdioCardInterruptEvent). The host should
 *        checks the interrupt pending status of each card I/O function and call that
 *        function's user defined IOFunctionCallback as needed.
 *
 *        The job of a IOFunctionCallback is to service the interrupt condition caused by the
 *        corresponding I/O function. It should read/write to the function unique area on
 *        the card to clear pending interrupt. The specific implementation for each
 *        IOFunctionCallback should be according to the specification of SDIO function it is
 *        written for.
 *
 * @param[in] pxSdioHandle   Handle to sdio card slot instance returned in iot_sdio_open().
 * @param[in] eFunc          I/O Function number that are associated with IRQ.
 * @param[in] pvUserContext  User context passed when setting the callback
 *                           This is not used by the driver, but just passed
 *                           back to the user in the callback.
 */
typedef void (* IotSdioIOFunctionCallback_t) ( IotSdioSlotHandle_t const pxSdioHandle,
                                               IotSdioFunction_t eFunc,
                                               void * pvUserContext );

/*!
 * @brief   iot_sdio_open() is used to open a handle to a sdio card slot.
 *
 *          This API must be called before using any other iot_sdio APIs.
 *          Upon successful return, the underline SDIO host controller should have
 *          been initialized properly.
 *
 * @param[in]   ucHostIdx                           SDIO host controller index.
 *                                                  Must be 0 if only single host available.
 *                                                  Range from 0 to n-1 if n hosts available.
 * @param[in]   ucSlotIdx                           Card slot index.
 *                                                  Must be 0 if only single slot is supported.
 *                                                  Range from 0 to n-1 if n card slots is supported.
 *
 * @return
 *   - returns the handle IotSdioSlotHandle_t on success
 *   - NULL if
 *       - host index number is invalid
 *       - slot index number is invalid
 *       - if a handle on this card slot instance is already open.
 */
IotSdioSlotHandle_t iot_sdio_open( uint8_t ucHostIdx,
                                   uint8_t ucSlotIdx );

/*!
 * @brief   iot_sdio_close() is used to close the sdio card slot handle.
 *          IO operation initiated through this handle before will
 *          not be aborted.  Caller should call iot_sdio_abort_io()
 *          to abort IO operation.
 *
 *          Up on a successful return of the API, the SDIO host controller should
 *          be regarded as uninitialized. All cards on this host should be regarded
 *          as disconnected.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open()
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success close
 *   - IOT_SDIO_INVALID_VALUE on NULL pxSdioHandle or already closed handle.
 */
int32_t iot_sdio_close( IotSdioSlotHandle_t const pxSdioHandle );

/*!
 * @brief   iot_sdio_card_connect() is used to connect SDIO card.
 *
 *          This API should perform card connection sequence as specified in
 *          section 3.1.2 in "SD Specification Part E1 SDIO"
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open()
 *
 * @return
 *   - IOT_SDIO_SUCCESS if card if successfully connected.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_HOST_NOT_READY if host controller is not initialized.
 *   - IOT_SDIO_SWITCH_VOLTAGE_FAIL if card failed to respond to voltage switch
 *       command (CMD11) or card responded to CMD11 but failed to switch voltage.
 *   - IOT_SDIO_INVALID_CARD if card is invalid (memory only card)
 *   - IOT_SDIO_SEND_RELATIVE_ADDRESS_FAIL if Send Relative Address (CMD3) failed.
 *   - IOT_SDIO_SELECT_CARD_FAIL if Select Card (CMD7) failed.
 *   - IOT_SDIO_GET_CARD_CAPABILITY_FAIL if failed to get card capability (CCCR).
 *   - IOT_SDIO_READ_CIS_FAIL if failed to read common CIS tuples from card.
 *   - IOT_SDIO_SET_DATA_BUS_WIDTH_FAIL if failed to set maximum data bus width.
 *   - IOT_SDIO_SELECT_BUS_TIMING_FAIL if failed to switch to card supported timing mode.
 */
int32_t iot_sdio_card_connect( IotSdioSlotHandle_t const pxSdioHandle );

/*!
 * @brief   iot_sdio_card_disconnect() is used to disconnect SDIO card.
 *
 *          This API should soft reset all I/O functions in sdio card and de-select the
 *          the card.
 *          User is not required to call this api before trying to re-connect the card.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open()
 *
 * @return
 *   - IOT_SDIO_SUCCESS if sdio card is successfully reset and de-selected.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL is failed to reset(write CCCR reg) or de-select(CMD7) card.
 */
int32_t iot_sdio_card_disconnect( IotSdioSlotHandle_t const pxSdioHandle );

/*!
 * @brief   iot_sdio_card_reset() is used to reset the sdio card in the target slot.
 *          This API should soft reset all I/O functions in sdio card.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open()
 *
 * @return
 *   - IOT_SDIO_SUCCESS if sdio card is successfully reset.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL is failed to reset(write CCCR reg) card.
 */
int32_t iot_sdio_card_reset( IotSdioSlotHandle_t const pxSdioHandle );

/*!
 * @brief   iot_sdio_io_write_direct() implements the write portion of CMD52.
 *          It is used to write 1 byte to a single register within the
 *          register space of a card I/O function.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open().
 * @param[in]   eFunc           IO function number range from eSdioFunction0
 *                              to eSdioFunction7.
 * @param[in]   ulRegAddr       The address of the byte of data inside of the
 *                              selected function to write.
 *                              Range is 0 - 0x1ffff.
 * @param[in/out]   pucData     In: data to be written to selected address
 *                              Out: If bRaw (read after write) is == false,
 *                                   it is same as the input data.
 *                                   If bRaw (read after write) is == true,
 *                                   it contains value read from the same
 *                                   address after write.
 * @param[in]   bRaw           The read after write flag.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL if failed to send command to for receive
 *                            response from card.
 */
int32_t iot_sdio_io_write_direct( IotSdioSlotHandle_t const pxSdioHandle,
                                  IotSdioFunction_t eFunc,
                                  uint32_t ulRegAddr,
                                  uint8_t * pucData,
                                  bool bRaw );

/*!
 * @brief   iot_sdio_io_read_direct() implements the read portion of CMD52.
 *          It is used to read 1 byte from a single register within the
 *          register space of a card I/O function.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open().
 * @param[in]   eFunc           IO function number range from eSdioFunction0
 *                              to eSdioFunction7.
 * @param[in]   ulRegAddr       The address of the byte of data inside of the
 *                              selected function to read.
 *                              Range is 0 - 0x1ffff.
 * @param[out]   pucData        Contains data read when command succeeded.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL if failed to send command or receive
 *                            response from card.
 */
int32_t iot_sdio_io_read_direct( IotSdioSlotHandle_t const pxSdioHandle,
                                 IotSdioFunction_t eFunc,
                                 uint32_t ulRegAddr,
                                 uint8_t * pucData );

/*!
 * @brief   iot_sdio_io_write_extended() implements the write portion of CMD53.
 *          It is used to write multiple bytes or blocks to a single address
 *          or incremental addresses within the register space in a I/O
 *          function. It provides highest transfer rate possible.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open().
 * @param[in]   eFunc           IO function number range from eSdioFunction0
 *                              to eSdioFunction7.
 * @param[in]   bBlockMode      Set true to transfer on block basis.
 *                              Set false to transfer on byte basis.
 * @param[in]   bOpcode         Set true to write multiple bytes to
 *                              incrementing addresses.
 *                              Set false to write multiple bytes to a fixed
 *                              address.
 * @param[in]   ulRegAddr       Start Address of I/O register to write.
 *                              Range is 0 - 0x1ffff.
 * @param[in]   pucBuf          Buffer that contains data to write.
 * @param[in]   ulCount         Number of bytes to write if bBlockMode is false.
 *                              Number of blocks to write if bBlockMode is true.
 *                              Range is 0 - 0x1ff
 *                              When bBlockMode is false, a value of 0 shall
 *                              cause 512 bytes to be written.
 *                              When bBlockMode is true, a value of 0 shall
 *                              cause infinite blocks to be written until
 *                              iot_sdio_abort_io is called.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL if failed to send command or receive
 *                            response from card.
 */
int32_t iot_sdio_io_write_extended( IotSdioSlotHandle_t const pxSdioHandle,
                                    IotSdioFunction_t eFunc,
                                    bool bBlockMode,
                                    bool bOpcode,
                                    uint32_t ulRegAddr,
                                    uint8_t * pucBuf,
                                    uint32_t ulCount );

/*!
 * @brief   iot_sdio_io_read_extended() implements the read portion of CMD53.
 *          It is used to read multiple bytes or blocks from a single address
 *          or incremental addresses within the register space in a I/O
 *          function. It provides highest transfer rate possible.
 *
 * @param[in]   pxSdioHandle    Handle to sdio card slot instance returned in
 *                              iot_sdio_open().
 * @param[in]   eFunc           IO function number range from eSdioFunction0
 *                              to eSdioFunction7.
 * @param[in]   bBlockMode      Set true to transfer on block basis.
 *                              Set false to transfer on byte basis.
 * @param[in]   bOpcode         Set true to read multiple bytes from
 *                              incrementing addresses.
 *                              Set false to read multiple bytes from a fixed
 *                              address.
 * @param[in]   ulRegAddr       Start Address of I/O register to read.
 *                              Range is 0 - 0x1ffff.
 * @param[out]   pucBuf         Buffer to receive read data.
 * @param[in]   ulCount         Number of bytes to read if bBlockMode is false.
 *                              Number of blocks to read if bBlockMode is true.
 *                              Range is 0 - 0x1ff
 *                              When bBlockMode is false, a value of 0 shall
 *                              cause 512 bytes to be read.
 *                              When bBlockMode is true, a value of 0 shall
 *                              cause infinite blocks to be read until
 *                              iot_sdio_abort_io is called.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 *   - IOT_SDIO_TRANSFER_FAIL if failed to send command or receive
 *                            response from card.
 */
int32_t iot_sdio_io_read_extended( IotSdioSlotHandle_t const pxSdioHandle,
                                   IotSdioFunction_t eFunc,
                                   bool bBlockMode,
                                   bool bOpcode,
                                   uint32_t ulRegAddr,
                                   uint8_t * pucBuf,
                                   uint32_t ulCount );

/*!
 * @brief   iot_sdio_set_io_function_callback() is used to set IOFunctionCallback for each I/O function the
 *          card supports.
 *
 *          IOFunctionCallback for an card I/O function is invoked when the corresponding I/O function
 *          pending interrupt bit is set in the Card's CCCR. Since the I/O function number is parameter to
 *          IOFunctionCallback, user can also implement one single IOFunctionCallback and set it for all
 *          I/O functions. Inside the IOFunctionCallback, different action can be taken according the I/O
 *          function number.
 *
 *          The job of a IOFunctionCallback is to service the interrupt condition caused by the
 *          corresponding I/O function. It should read/write to the function unique area on
 *          the card to clear pending interrupt. The specific implementation for each
 *          IOFunctionCallback should be according to the specification of SDIO function it is
 *          written for.
 *
 * @param[in]   pxSdioHandle                     Handle to sdio card slot instance returned in
 *                                               iot_sdio_open().
 * @param[in]   eFunc                            IO function number range from eSdioFunction0
 *                                               to eSdioFunction7.
 * @param[in]   xIOFunctionCallback              IOFunctionCallback to be set for I/O function eFunc.
 * @param[in]   pvIOFunctionCallbackUserContext  User context to be passed when IOFunctionCallback is
 *                                               invoked.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 */
int32_t iot_sdio_set_io_function_callback( IotSdioSlotHandle_t const pxSdioHandle,
                                           IotSdioFunction_t eFunc,
                                           IotSdioIOFunctionCallback_t xIOFunctionCallback,
                                           void * pvIOFunctionCallbackUserContext );

/*!
 * @brief   iot_sdio_set_sdio_callback() is used to set sdio card interrupt callback.
 *
 * @param[in]   pxSdioHandle               Handle to sdio card slot instance returned in
 *                                         iot_sdio_open().
 * @param[in]   xSdioCallback              Callback function to be called from driver.
 * @param[in]   pvSdioCallbackUserContext  User context to be passed when callback is called.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid.
 */
int32_t iot_sdio_set_sdio_callback( IotSdioSlotHandle_t const pxSdioHandle,
                                    IotSdioCallback_t xSdioCallback,
                                    void * pvSdioCallbackUserContext );

/**
 * @brief iot_sdio_ioctl() is Used for various sdio control function.
 *
 * @param[in] pxSdioHandle Handle to sdio card slot instance returned in iot_sdio_open().
 * @param[in] xSdioIoctlRequest Should be one of IotSdioIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @return
 *   - IOT_SDIO_SUCCESS on success.
 *   - IOT_SDIO_INVALID_VALUE if any parameter is invalid or not supported by host or card.
 *   - IOT_SDIO_TRANSFER_FAIL if failed to send command to or receive response from card.
 *   - IOT_SDIO_SET_CARD_BLOCK_SIZE_FAIL if eSDIOSetFuncBlockSize ioctl request failed.
 *   - IOT_SDIO_LOW_SPEED_CARD if switching speed failed due low speed only card.
 *     (for eSDIOSwitchToHighSpeed request only).
 *   - IOT_SDIO_SWITCH_HIGH_SPEED_FAIL if sdio card supports high speed mode but failed
 *       to switch to high speed mode. (for eSDIOSwitchToHighSpeed only).
 *   - IOT_SDIO_ASYNC_INT_NOT_SUPPORTED if card doesn't support async interrupt mode.
 *       (for eSDIOEnableAsyncIrqMode only).
 *   - IOT_SDIO_READ_CIS_FAIL if read cis command execution failed. (for eSDIOReadCis only)
 *   - IOT_SDIO_FAIL other failures that is unrelated to command execution.
 */
int32_t iot_sdio_ioctl( IotSdioSlotHandle_t const pxSdioHandle,
                        IotSdioIoctlRequest_t xSdioIoctlRequest,
                        void * const pvBuffer );

/**
 * @}
 */

#endif /* _IOT_SDIO_H_ */
