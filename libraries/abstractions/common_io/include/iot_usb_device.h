/*
 * Amazon FreeRTOS Common IO V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#define IOT_USB_DEVICE_SUCCESS                   ( 0 )
#define IOT_USB_DEVICE_ERROR                     ( 1 )
#define IOT_USB_DEVICE_BUSY                      ( 2 )
#define IOT_USB_DEVICE_INVALID_HANDLE            ( 3 )
#define IOT_USB_DEVICE_INVALID_VALUE             ( 4 )
#define IOT_USB_DEVICE_INVALID_REQUEST           ( 5 )
#define IOT_USB_DEVICE_WRITE_FAILED              ( 6 )
#define IOT_USB_DEVICE_READ_FAILED               ( 7 )
#define IOT_USB_DEVICE_ERASE_FAILED              ( 8 )
#define IOT_USB_DEVICE_CTRL_OP_FAILED            ( 9 )
#define IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED    ( 10 )

/**
 * @brief Control endpoint index.
 */
#define IOT_USB_CONTROL_ENDPOINT (0U)

/**
 * @brief The setup packet size of USB control transfer.
 */
#define IOT_USB_SETUP_PACKET_SIZE (8U)

/**
 * @brief  USB endpoint mask.
 */
#define IOT_USB_ENDPOINT_NUMBER_MASK (0x0FU)

/**
 * @brief Default invalid value or the endpoint callback length of cancelled transfer.
 */
#define IOT_USB_UNINITIALIZED_VAL_32 (0xFFFFFFFFU)

/**
 * @defgroup iot_usb_device usb device Abstraction APIs.
 * @{
 */

/**
 * @brief common notify event types in device callback.
 */
typedef enum
{
    eUSBDeviceBusResetEvent,      /*!< USB bus reset signal detected */
    eUSBDeviceSuspendEvent,       /*!< USB bus suspend signal detected */
    eUSBDeviceResumeEvent,        /*!< USB bus resume signal detected. The resume signal is driven by itself or a host */
    eUSBDeviceLPMSleepEvent,      /*!< USB bus LPM sleep signal detected */
    eUSBDeviceLPMResumeEvent,     /*!< USB bus LPM resume signal detected. The resume signal is driven by itself or a host*/
    eUSBDeviceErrorEvent,         /*!< An error is happened in the bus. */
    eUSBDeviceDetachEvent,        /*!< USB device is disconnected from a host. */
    eUSBDeviceAttachEvent,        /*!< USB device is connected to a host. */
} IotUsbDeviceEvent_t;

/**
 * @brief Ioctl request types.
 */
typedef enum
{
    eUSBDeviceGetSpeed,          /*!< Get device speed. Return integer*/
    eUSBDeviceGetAddress,        /*!< Get device address. Return an integer range from 1~127*/
    eUSBDeviceSetAddress,        /*!< Set device address. Take an integer to set it as address*/
    eUSBDeviceGetEndpointStatus, /*!< Get endpoint status. Return IotUsbDeviceEndpointStatus_t type*/
    eUSBDeviceGetState,          /*!< Get device state. Return IotUsbDeviceState_t*/
    eUSBDeviceGetStatus,         /*!< Get device status. Return */
    eUSBDeviceGetSyncFrame,      /*!< Get Sync frame. Return */
    eUSBDeviceSetTestMode,       /*!< Set device in test mode. */
    eUSBDeviceSetState,          /*!< Set device state. Take IotUsbDeviceState_t and set it*/
    eUSBDeviceSleepBus,          /*!< Sleep usb bus */
    eUSBDeviceSuspendBus,        /*!< Suspend bus */
    eUSBDeviceResumeBus,         /*!< Resumne bus */
    eUSBDeviceRemoteWakeup,      /*!< Remote wakeup host */
} IotUsbDeviceIoctlRequest_t;

/**
 * @brief USB device state.
 */
typedef enum
{
    eUsbDeviceStateConfigured,        /*!< Device state, usb device is configured and ready for use by host*/
    eUsbDeviceStateAddress,           /*!< Device state, usb device Address assigned*/
    eUsbDeviceStateDefault,           /*!< Device state, usb device is reset, but address not assigned yet*/
    eUsbDeviceStateAddressing,        /*!< Device state, usb device is undergoing assigning address*/
    eUsbDeviceStateTestMode,          /*!< Device state, usb device ins on test mode*/
} IotUsbDeviceState_t;

/**
 * @brief Defines endpoint state.
 */
typedef enum
{
    eUsbDeviceEndpointStateIdle,      /*!< Endpoint state, idle*/
    eUsbDeviceEndpointStateStalled,   /*!< Endpoint state, stalled*/
    eUsbDeviceEndpointStateBusy,      /*!< Endpoint state, busy*/
    eUsbDeviceEndpointStateClosed,    /*!< Endpoint state, Closed, not configured*/
} IotUsbDeviceEndpointStatus_t;

/**
 * @brief Define device controller ID.
 */
typedef enum
{
    eUsbDeviceControllerOHCI,      /* Open Device Controller Interface */
    eUsbDeviceControllerUHCI,      /* Universal Device Controller Interface */
    eUsbDeviceControllerEHCI,      /* Enhanced Device Controller Interface */
    eUsbDeviceControllerXHCI,      /* Extensible Device Controller Interface */
} IotUsbHostController_t;

/**
 * @brief USB device driver operation status.
 */
typedef enum
{
    eUsbDeviceCompleted = IOT_USB_DEVICE_SUCCESS,
    eUsbDeviceReadFailed = IOT_USB_DEVICE_READ_FAILED,
    eUsbDeviceWriteFailed = IOT_USB_DEVICE_WRITE_FAILED,
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
 *        (IotUsbDeviceEvent_t) happen.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 * @param[in] xDeviceEvent    USB device asynchronous event.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef int32_t (*IotUsbDeviceCallback_t) ( IotUsbDeviceHandle_t const pxUsbDevice, uint32_t xDeviceEvent, void * pvUserContext );

/**
 * @brief The callback typedef for USB device endpoint. For each endpoint, it's passed when user
 *        set endpoint callback by iot_usb_device_set_endpoint_callback API.
 *
 * @param[out] xStatus    Usb device asynchronous operation status.
 * @param[in] pvUserContext User Context passed when setting the callback.
 *                          This is not used or modified by the driver. The context
 *                          is provided by the caller when setting the callback, and is
 *                          passed back to the caller in the callback.
 *
 */
typedef int32_t (* IotUsbDeviceEndpointCallbackFn_t) ( IotUsbDeviceOperationStatus_t xStatus, void * pvUserContext );

/**
 * @brief Endpoint configuration structure.
 */
typedef struct
{
    uint16_t usMaxPacketSize;  /*!< Endpoint maximum packet size */
    uint8_t ucEndpointAddress; /*!< Endpoint address*/
    uint8_t ucTransferType;    /*!< Endpoint transfer type*/
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
 * @return The handle to the USB device if SUCCESS else NULL.
 */
IotUsbDeviceHandle_t iot_usb_device_open( int32_t lUsbDeviceControllerInstance );

/**
 * @brief Connect the USB device.
 *
 * The function enables the device functionality, so that the device can be recognized
 * by the host when the device detects that it has been connected to a host.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE.
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
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE.
 */
int32_t iot_usb_device_disconnect( IotUsbDeviceHandle_t const pxUsbDevice );

/**
 * @brief Sets the application callback to be called on device event.
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
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] pxEpConfig Endpoint configuration.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_endpoint_open( IotUsbDeviceHandle_t const pxUsbDevice,
                                      IotUsbDeviceEndpointConfig_t * pxEpConfig);

/**
 * @brief Sets the endpoint callback to be called on endpoint transfer.
 *
 * @param[in] pxUsbDevice The usb device peripheral handle returned in the open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 * @param[in] xCallback The callback function to be called on endpoint transfer.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_usb_device_set_endpoint_callback( IotUsbDeviceHandle_t const pxUsbDevice,
                           uint8_t ucEndpointAddress, IotUsbDeviceEndpointCallbackFn_t xCallback,
                           void * pvUserContext );

/**
 * @brief De-initiates the usb device specific endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_endpoint_close( IotUsbDeviceHandle_t const pxUsbDevice,
                                          uint8_t ucEndpointAddress );

/**
 * @brief Stall the usb device specific endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_endpoint_stall( IotUsbDeviceHandle_t const pxUsbDevice,
                                          uint8_t ucEndpointAddress );

/**
 * @brief Unstall the usb device specific endpoint.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint Address.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_endpoint_unstall( IotUsbDeviceHandle_t const pxUsbDevice,
                                          uint8_t ucEndpointAddress );

/**
 * @brief Starts the USB device read operation in blocking mode.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_read_sync( IotUsbDeviceHandle_t const pxUsbDevice,
                            uint8_t ucEndpointAddress,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the USB device write operation in blocking mode.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_write_sync( IotUsbDeviceHandle_t const pxUsbDevice,
                            uint8_t ucEndpointAddress,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the USB device read operation in non-blocking mode.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
 */
int32_t iot_usb_device_read_async( IotUsbDeviceHandle_t const pxUsbDevice,
                            uint8_t ucEndpointAddress,
                            uint8_t * const pvBuffer,
                            size_t xBytes );

/**
 * @brief Starts the USB device write operation in non-blocking mode.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 * @param[in] ucEndpointAddress Endpoint index
 * @param[out] pvBuffer The receive buffer to read the data into
 * @param[in] xBytes The number of bytes to read.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE
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
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED, or other USB Error.
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
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *          FAILED = IOT_USB_DEVICE_FUNCTION_INVALID_VALUE
 *          or IOT_USB_DEVICE_FUNCTION_NOT_SUPPORTED, IOT_USB_DEVICE_NOTHING_TO_CANCEL
 */
int32_t iot_usb_device_endpoint_cancel_transfer( IotUsbDeviceHandle_t const pxUsbDevice, uint8_t ucEndpointAddress );

/**
 * @brief Close the USB device peripheral.
 *
 * @param[in] pxUsbDevice The USB device handle returned in open() call.
 *
 * @return SUCCESS = IOT_USB_DEVICE_SUCCESS,
 *         FAILED = IOT_USB_DEVICE_INVALID_VALUE.
 */
int32_t iot_usb_device_close( IotUsbDeviceHandle_t const pxUsbDevice );

/**
 * @}
 */
/* end of group iot_usb_device */

#endif /* ifndef _IOT_USB_DEVICE_H_ */
