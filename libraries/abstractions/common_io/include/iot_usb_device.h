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
 * @file iot_usb_device.h
 * @brief File for the APIs of USB device called by application or class layer.
 */
#ifndef _IOT_USB_DEVICE_H_
#define _IOT_USB_DEVICE_H_

/**
 * @brief Return values used by this driver
 */
#define IOT_USB_DEVICE_SUCCESS                   ( 0 )     /*!< USB operation completed successfully. */
#define IOT_USB_DEVICE_ERROR                     ( 1 )     /*!< USB device error. */
#define IOT_USB_DEVICE_BUSY                      ( 2 )     /*!< USB device busy. */
#define IOT_USB_DEVICE_INVALID_VALUE             ( 3 )     /*!< At least one parameter is invalid. */
#define IOT_USB_DEVICE_WRITE_FAILED              ( 4 )     /*!< USB device write operation failed. */
#define IOT_USB_DEVICE_READ_FAILED               ( 5 )     /*!< USB device read operation failed. */
#define IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED    ( 6 )     /*!< USB device function not supported. */
#define IOT_USB_DEVICE_NOTHING_TO_CANCEL         ( 7 )     /*!< No operation in progress to cancel. */

/**
 * @brief Control endpoint index.
 */
#define IOT_USB_CONTROL_ENDPOINT                 ( 0U )

/**
 * @brief The setup packet size of USB control transfer.
 */
#define IOT_USB_SETUP_PACKET_SIZE                ( 8U )

/**
 * @brief  USB endpoint mask.
 */
#define IOT_USB_ENDPOINT_NUMBER_MASK             ( 0x0FU )

/**
 * @brief Default invalid value or the endpoint callback length of cancelled transfer.
 */
#define IOT_USB_UNINITIALIZED_VAL_32             ( 0xFFFFFFFFU )

/**
 * @defgroup iot_usb_device usb device Abstraction APIs.
 * @{
 */

/**
 * @brief common notify event types in device callback.
 */
typedef enum
{
    eUSBDeviceBusResetEvent,        /*!< USB bus reset signal detected. Signal is initiated from USB Host. */
    eUSBDeviceLPMState1Event,       /*!< USB LPM (link power management) state 1 event: for usb 2.0, correspond to L1 sleep, host initialize
                                     *  LPM extended transaction; for usb 3.0, correspond to U1 standby, both device and host can initialize. */
    eUSBDeviceLPMState2Event,       /*!< USB LPM state 2 event: for usb 2.0, correspond to L2 suspend, host nees to support to trigger it after
                                     *   3ms of inactivity; for usb 3.0, correspond to U2 standby, both device and host can initialize. */
    eUSBDeviceLPMState3Event,       /*!< USB LPM state 3 event: for usb 2.0, correspond to L3 off, disconnect or power off will trigger it;
                                     *   for usb 3.0, correspond to U3 suspend, it can only be initialized by host. */
    eUSBDeviceLPMState1ResumeEvent, /*!< USB resume event from LPM state 1: for usb 2.0 and 3.0, both device and host can initialize
                                     *   resume signaling. */
    eUSBDeviceLPMState2ResumeEvent, /*!< USB resume event from LPM state 2: for usb 2.0 and 3.0, both device and host can initialize
                                     *   resume signaling. */
    eUSBDeviceLPMState3ResumeEvent, /*!< USB resume event from LPM state 3: for usb 2.0, L3 is off state, port needs reset or power on; for usb 3.0,
                                     *   both device and host can initialize resume signaling.*/
    eUSBDeviceErrorEvent,           /*!< An error is happened in the bus. */
    eUSBDeviceDetachEvent,          /*!< USB device is disconnected from a host. */
    eUSBDeviceAttachEvent,          /*!< USB device is connected to a host. */
} IotUsbDeviceEvent_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eUSBDeviceGetSpeed,          /*!< Get device speed. Return uint8_t integer. */
    eUSBDeviceGetAddress,        /*!< Get device address. Return an uint8_t integer range from 1~127. */
    eUSBDeviceSetAddress,        /*!< Set device address. Take an uint8_t integer to set it as address. */
    eUSBDeviceGetEndpointStatus, /*!< Get endpoint status. Return IotUsbDeviceEndpointStatus_t type. */
    eUSBDeviceGetState,          /*!< Get device state. Return IotUsbDeviceState_t. */
    eUSBDeviceGetStatus,         /*!< Get device status. Return uint16_t. */
    eUSBDeviceGetSyncFrame,      /*!< Get Sync frame. Returns the frame number of synchronization frame as uint16_t. */
    eUSBDeviceSetTestMode,       /*!< Set device in test mode.  For compliance testing defined in usb protocol.
                                  *   @warning Device needs to power cycle to exit test mode.*/
    eUSBDeviceSetState,          /*!< Set device state. Take IotUsbDeviceState_t and set it*/
    eUSBDeviceLPMState1Suspend,  /*!< Suspend usb to LPM state 1: for usb 2.0, it is L1 state; for usb 3.0, it is U1 state. */
    eUSBDeviceLPMState1Resume,   /*!< Resume usb from LPM state 1: for usb 2.0, it is exit L1 state; for usb 3.0, it is exit U1 state. */
    eUSBDeviceLPMState2Suspend,  /*!< Suspend usb to LPM state 2: for usb 2.0, it is L2 state; for usb 3.0, it is U2 state. */
    eUSBDeviceLPMState2Resume,   /*!< Resume usb from LPM state 2: for usb 2.0, it is exit L2 state; for usb 3.0, it is exit U2 state. */
    eUSBDeviceLPMState3Suspend,  /*!< Suspend usb to LPM state 3: for usb 2.0, it is L3 state; for usb 3.0, it is U3 state. */
    eUSBDeviceLPMState3Resume,   /*!< Resume usb from LPM state 3: for usb 2.0, it is exit L3 state; for usb 3.0, it is exit U3 state. */
    eUSBDeviceRemoteWakeup,      /*!< Remote wakeup host. */
} IotUsbDeviceIoctlRequest_t;

/**
 * @brief USB device state.
 */
typedef enum
{
    eUsbDeviceStateConfigured, /*!< Device state, usb device is configured and ready for use by host*/
    eUsbDeviceStateAddress,    /*!< Device state, usb device Address assigned*/
    eUsbDeviceStateDefault,    /*!< Device state, usb device is reset, but address not assigned yet*/
    eUsbDeviceStateAddressing, /*!< Device state, usb device is undergoing assigning address*/
    eUsbDeviceStateTestMode,   /*!< Device state, usb device is in test mode*/
} IotUsbDeviceState_t;

/**
 * @brief Defines endpoint state.
 */
typedef enum
{
    eUsbDeviceEndpointStateIdle,    /*!< Endpoint state, idle*/
    eUsbDeviceEndpointStateStalled, /*!< Endpoint state, stalled*/
    eUsbDeviceEndpointStateBusy,    /*!< Endpoint state, busy*/
    eUsbDeviceEndpointStateClosed,  /*!< Endpoint state, Closed, not configured*/
} IotUsbDeviceEndpointStatus_t;

/**
 * @brief Define device controller ID.
 */
typedef enum
{
    eUsbDeviceControllerOHCI, /*!< Open Device Controller Interface */
    eUsbDeviceControllerUHCI, /*!< Universal Device Controller Interface */
    eUsbDeviceControllerEHCI, /*!< Enhanced Device Controller Interface */
    eUsbDeviceControllerXHCI, /*!< Extensible Device Controller Interface */
} IotUsbHostController_t;

/**
 * @brief USB device driver operation status.
 */
typedef enum
{
    eUsbDeviceCompleted = IOT_USB_DEVICE_SUCCESS,        /*!< Device operation completed successfully. */
    eUsbDeviceReadFailed = IOT_USB_DEVICE_READ_FAILED,   /*!< Device read operation failed. */
    eUsbDeviceWriteFailed = IOT_USB_DEVICE_WRITE_FAILED, /*!< Device write operation failed. */
} IotUsbDeviceOperationStatus_t;

/**
 * @brief The USB device type defined in the source file.
 */
struct IotUsbDevice;

/**
 * @brief IotUsbDeviceHandle_t is the handle type returned by calling iot_usb_device_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotUsbDevice * IotUsbDeviceHandle_t;

/**
 * @brief The callback function for USB device event operation. This callback is passed to
 *        driver by using iot_usb_device_set_callback API. It's called when one of the events
 *        (IotUsbDeviceEvent_t) happen. This callback is used by upper layer to set up upper layer
 *        callback by calling iot_usb_device_set_device_callback. It is used when the bus state changes
 *        and an event is generated, e.g. an bus reset event happened, usb class layer needs to know such
 *        event and react accordingly.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 * @param[in] ulDeviceEvent USB device asynchronous event.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef int32_t (* IotUsbDeviceCallback_t) ( IotUsbDeviceHandle_t const pxUsbDevice,
                                             uint32_t ulDeviceEvent,
                                             void * pvUserContext );

/**
 * @brief The callback typedef for USB device endpoint. For each endpoint, it's passed when user
 *        set endpoint callback by iot_usb_device_set_endpoint_callback API.
 *        This callback is used to notify the upper layer about the endpoint tranafer result.
 *
 * @param[out] xStatus    Usb device asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef int32_t (* IotUsbDeviceEndpointCallbackFn_t) ( IotUsbDeviceOperationStatus_t xStatus,
                                                       void * pvUserContext );

/**
 * @brief Endpoint configuration structure.
 */
typedef struct
{
    uint16_t usMaxPacketSize;  /*!< Endpoint maximum packet size.  Depends on the USB controller
                                *   Control Transfer:
                                *       Full Speed: 8, 16, 32, 64
                                *       High Speed: 64
                                *       Low Speed:  8
                                *   Isochronous Transfer:
                                *       Full Speed: 1023
                                *       High Speed: 1024
                                *       Not supported for low speed device
                                *   Interrupt Transfer:
                                *       Full Speed: 64
                                *       High Speed: 1024
                                *       Low Speed:  8
                                *   Bulk Transfer:
                                *       Full Speed: 8, 16, 32, 64
                                *       High Speed: 512
                                *       Not supported for low speed device */
    uint8_t ucEndpointAddress; /*!< Endpoint address*/
    uint8_t ucTransferType;    /*!< Endpoint transfer type defined as: control EP is 0;
                                *    Isochronous EP is 1;  Bulk EP is 2; Interrupt EP is 3. */
    uint8_t ucZlt;             /*!< ZLT flag*/
    uint8_t ucInterval;        /*!< Endpoint polling interval of transfer*/
} IotUsbDeviceEndpointConfig_t;

/**
 * @brief Endpoint status structure.
 */
typedef struct
{
    uint8_t ucEndpointAddress; /*!< Endpoint address */
    uint16_t usEndpointStatus; /*!< Endpoint status: IotUsbDeviceEndpointStatus_t*/
} IotUsbDeviceEndpointInfo_t;

/**
 * @brief Initiates the usb device controller interface.
 *
 * @lUsbDeviceControllerInstance The instance of USB device controller to initialize.
 *
 * @return
 *   - The handle to the USB device if SUCCESS
 *   - NULL if
 *      - invalid lUsbDeviceControllerInstance
 *      - already open
 */
IotUsbDeviceHandle_t iot_usb_device_open( int32_t lUsbDeviceControllerInstance );

/**
 * @brief Connect the USB device.
 *
 * The function enables the device functionality, so that the device can be recognized
 * by the host when the device detects that it has been connected to a host.
 *
 * @warning this function must be called in order for USB to be recognized
 * but not necessarily enumerated which requires the class to exchange control packets.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if pxUsbDevice is NULL.
 */
int32_t iot_usb_device_connect( IotUsbDeviceHandle_t const pxUsbDevice );

/**
 * @brief  Disconnect the USB device.
 *
 * The function disables the device functionality, after this function called, even the
 * device is attached to the host, and the device can't work.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if pxUsbDevice is NULL.
 */
int32_t iot_usb_device_disconnect( IotUsbDeviceHandle_t const pxUsbDevice );

/**
 * @brief Sets the application callback to be called on device event.
 *
 * @note Single callback is used, per instance.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 * @param[in] xCallback The callback function to be called on device event.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_usb_device_set_device_callback( IotUsbDeviceHandle_t const pxUsbDevice,
                                         IotUsbDeviceCallback_t xCallback,
                                         void * pvUserContext );

/**
 * @brief Initiates the usb device specific endpoint.
 *
 * @warning: Its the class codes responsibility to make sure the endpoint address is only opened once.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] pxEpConfig Endpoint configuration.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - pxEpConfig is NULL.
 *      - endpoint address is already open
 *      - endpoint config parameter is not supported
 *      - endpoint address is invalid
 *   - IOT_USB_DEVICE_ERROR if an error occurred with the device
 */
int32_t iot_usb_device_endpoint_open( IotUsbDeviceHandle_t const pxUsbDevice,
                                      IotUsbDeviceEndpointConfig_t * pxEpConfig );

/**
 * @brief Sets the endpoint callback to be called on endpoint transfer.
 *
 * @note Single callback is used, per instance.
 * @note Newly set callback overrides the one previously set
 *
 * @warning If input handle or if callback function is NULL, or invalid EndpointAddress issued,
 *          this function silently takes no action.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 * @param[in] xCallback The callback function to be called on endpoint transfer.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_usb_device_set_endpoint_callback( IotUsbDeviceHandle_t const pxUsbDevice,
                                           uint8_t ucEndpointAddress,
                                           IotUsbDeviceEndpointCallbackFn_t xCallback,
                                           void * pvUserContext );

/**
 * @brief De-initializes the usb device specific endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 */
int32_t iot_usb_device_endpoint_close( IotUsbDeviceHandle_t const pxUsbDevice,
                                       uint8_t ucEndpointAddress );

/**
 * @brief Stall the usb device specific endpoint.
 * This function is to set stall conditions for the specific endpoint.
 * The STALL packet indicates that the endpoint has halted, or a control pipe
 * does not support a certain request.
 * A function uses the STALL handshake packet to indicate that it is unable to
 * transmit or receive data. Besides the default control pipe, all of a function's
 * endpoints are in an undefined state after the device issues a STALL handshake
 * packet. The host must never issue a STALL handshake packet.
 * Typically, the STALL handshake indicates a functional stall. A functional
 * stall occurs when the halt feature of an endpoint is set. In this circumstance,
 * host intervention is required via the default control pipe to clear the halt
 * feature of the halted endpoint. Less often, the function returns a STALL
 * handshake during a SETUP or DATA stage of a control transfer. This is called a
 * protocol stall and is resolved when the host issues the next SETUP transaction
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 */
int32_t iot_usb_device_endpoint_stall( IotUsbDeviceHandle_t const pxUsbDevice,
                                       uint8_t ucEndpointAddress );

/**
 * @brief Unstall the usb device specific endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 */
int32_t iot_usb_device_endpoint_unstall( IotUsbDeviceHandle_t const pxUsbDevice,
                                         uint8_t ucEndpointAddress );

/**
 * @brief Starts the USB device read operation in blocking mode.
 *
 * @note This function will block until all xBytes have been read.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 *      - pvBuffer is NULL
 *      - xBytes == 0
 *   - IOT_USB_DEVICE_BUSY if another operation is in progress on the same endpoint.
 *   - IOT_USB_DEVICE_READ_FAILED if unable to complete the read operation
 */
int32_t iot_usb_device_read_sync( IotUsbDeviceHandle_t const pxUsbDevice,
                                  uint8_t ucEndpointAddress,
                                  uint8_t * const pvBuffer,
                                  size_t xBytes );

/**
 * @brief Starts the USB device write operation in blocking mode.
 *
 * note: this function will block until all xBytes have been written.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 *      - pvBuffer is NULL
 *      - xBytes == 0
 *   - IOT_USB_DEVICE_BUSY if another operation is in progress on the same endpoint.
 *   - IOT_USB_DEVICE_WRITE_FAILED if unable to complete the write operation
 */
int32_t iot_usb_device_write_sync( IotUsbDeviceHandle_t const pxUsbDevice,
                                   uint8_t ucEndpointAddress,
                                   uint8_t * const pvBuffer,
                                   size_t xBytes );

/**
 * @brief Starts the USB device read operation in non-blocking mode.
 *
 * @note iot_usb_device_set_device_callback() must be called first in
 * order for the callback function to be called to signal when the
 * read is complete.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 *      - pvBuffer is NULL
 *      - xBytes == 0
 *   - IOT_USB_DEVICE_BUSY if another operation is in progress on the same endpoint.
 *   - IOT_USB_DEVICE_READ_FAILED if unable to complete the read operation
 */
int32_t iot_usb_device_read_async( IotUsbDeviceHandle_t const pxUsbDevice,
                                   uint8_t ucEndpointAddress,
                                   uint8_t * const pvBuffer,
                                   size_t xBytes );

/**
 * @brief Starts the USB device write operation in non-blocking mode.
 *
 * @note iot_usb_device_set_device_callback() must be called first in
 * order for the callback function to be called to signal when the
 * write is complete.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 *      - pvBuffer is NULL
 *      - xBytes == 0
 *   - IOT_USB_DEVICE_BUSY if another operation is in progress on the same endpoint.
 *   - IOT_USB_DEVICE_WRITE_FAILED if unable to complete the write operation
 */
int32_t iot_usb_device_write_async( IotUsbDeviceHandle_t const pxUsbDevice,
                                    uint8_t ucEndpointAddress,
                                    uint8_t * const pvBuffer,
                                    size_t xBytes );

/**
 * @brief Used for various USB device control function.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] xUsbDeviceRequest Should be one of IotUsbDeviceIoctlRequest_t.
 * @param[in,out] pvBuffer The configuration values for the IOCTL request.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid xUsbDeviceRequest
 *   - IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED if ioctl request not suported.
 *      -  Only valid for
 *      -    eUSBDeviceSleepBus
 *      -    eUSBDeviceSuspendBus
 *      -    eUSBDeviceResumeBus
 *      -    eUSBDeviceRemoteWakeup
 *      -    eUSBDeviceGetSyncFrame
 *   - or other USB Error.
 */
int32_t iot_usb_device_ioctl( IotUsbDeviceHandle_t const pxUsbDevice,
                              IotUsbDeviceIoctlRequest_t xUsbDeviceRequest,
                              void * const pvBuffer );

/**
 * @brief This function is used to cancel the pending transfer for given endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - invalid ucEndpointAddress
 *   - IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED if cancel transfer opertion not supported
 *   - IOT_USB_DEVICE_NOTHING_TO_CANCEL if nothing to cancel
 */
int32_t iot_usb_device_endpoint_cancel_transfer( IotUsbDeviceHandle_t const pxUsbDevice,
                                                 uint8_t ucEndpointAddress );

/**
 * @brief Close the USB device peripheral.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 *
 * @return
 *   - IOT_USB_DEVICE_SUCCESS on success
 *   - IOT_USB_DEVICE_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - pxUsbDevice not open (already closed).
 */
int32_t iot_usb_device_close( IotUsbDeviceHandle_t const pxUsbDevice );

/**
 * @}
 */
/* end of group iot_usb_device */

#endif /* ifndef _IOT_USB_DEVICE_H_ */
