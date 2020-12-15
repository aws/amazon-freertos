/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
 * @file iot_usb_host.h
 * @brief File for the APIs of USB host called by application or class layer.
 */

#ifndef _IOT_USB_HOST_H_
#define _IOT_USB_HOST_H_

/**
 * @defgroup iot_usb_host usb host Abstraction APIs.
 * @{
 */

/**
 * @brief Return values returned by this driver
 */
#define IOT_USB_HOST_SUCCESS                   ( 0 ) /*!< USB host operation completed successfully. */
#define IOT_USB_HOST_ERROR                     ( 1 ) /*!< USB host error. */
#define IOT_USB_HOST_BUSY                      ( 2 ) /*!< USB host busy. */
#define IOT_USB_HOST_INVALID_VALUE             ( 3 ) /*!< At least one paramter is invalid. */
#define IOT_USB_HOST_WRITE_FAIL                ( 4 ) /*!< USB host write operation failed. */
#define IOT_USB_HOST_READ_FAIL                 ( 5 ) /*!< USB host read operation failed. */
#define IOT_USB_HOST_CANCEL_FAILED             ( 6 ) /*!< USB host cancel operation failed. */
#define IOT_USB_HOST_FUNCTION_NOT_SUPPORTED    ( 7 ) /*!< USB host function not supported. */
#define IOT_USB_HOST_NOTHING_TO_CANCEL         ( 8 ) /*!< No operation in progress to cancel. */

#define IOT_USB_HOST_MAX_ENDPOINTS             ( 4 ) /*!< Maximum endpoint number for each interface. */

/**
 * @brief common host notify event types in host callback.
 */
typedef enum
{
    eUSBHostAttachEvent,          /*!< USB device is connected to host. */
    eUSBHostDetachEvent,          /*!< USB device is detached to host. */
    eUSBHostEnumerationDoneEvent, /*!< USB device enumeration is done. */
    eUSBHostResetEvent,           /*!< USB reset is done. */
    eUSBHostSuspendEvent,         /*!< USB suspend happened. */
    eUSBHostResumeEvent,          /*!< USB resume happened. */
    eUSBHostRemoteWakeEvent,      /*!< USB remote wakeup is activated. */
} IotUsbHostEvent_t;

/**
 * @brief Ioctl standard device request for USB Host HAL.
 */
typedef enum
{
    eUSBHostGetHosthandle,          /*!< Get usb host handle. */
    eUSBHostGetDeviceControlPipe,   /*!< Get usb device control pipe. */
    eUSBHostGetDeviceAddress,       /*!< Get usb device address. */
    eUSBHostOpenDeviceInterface,    /*!< Open usb device interface. */
    eUSBHostCloseDeviceInterface,   /*!< Close usb device interface. */
    eUSBHostGetDeviceInfo,          /*!< Get usb device information. */
    eUSBHostGetInterfaceInfo,       /*!< Get usb interface information. */
    eUSBHostGetInterfaceDescriptor, /*!< Get usb device interface descriptor. */
    eUSBHostGetConfigureHandle,     /*!< Get usb configure handle. */
    eUSBHostGetInterfaceHandle,     /*!< Get usb interface handle. */
} IotUsbHostIoctlRequest_t;

/**
 * @brief USB host type defined in the source file.
 */
struct  IotUsbHost;

/**
 * @brief The USB pipe, interface, configuration and device type defined in the source file.
 */
struct IotUsbDevice;
struct IotUsbConfiguration;
struct IotUsbInterface;
struct IotUsbPipe;

/**
 * @brief IotUsbHostHandle_t is the handle type returned by calling iot_usb_host_open().
 *        This is initialized in open and returned to caller. The caller must pass
 *        this pointer to the rest of APIs.
 */
typedef struct IotUsbHost            * IotUsbHostHandle_t;

/**
 * @brief IotUsbDeviceHandle_t is the handle type returned when device attach and enumerated
 *        This device handle is needed for device operation.
 */
typedef struct IotUsbDevice          * IotUsbDeviceHandle_t;

/**
 * @brief IotUsbConfigHandle_t is the configuration handle for device
 *        Only one configuration at run time.
 */
typedef struct IotUsbConfiguration   * IotUsbConfigHandle_t;

/**
 * @brief IotUsbInterfaceHandle_t is the interface handle, which can be extracted from config
 *        One device may have more than one interface to be used.
 */
typedef struct IotUsbInterface       * IotUsbInterfaceHandle_t;

/**
 * @brief IotPipeHandle_t is the pipe handle, which will be used for data and control transfer.
 */
typedef struct IotUsbPipe            * IotPipeHandle_t;

/**
 * @brief   USB Host attach/detach notify callback type. This callback is passed
 *          to the driver by calling iot_usb_host_set_host_callback.
 *
 * @param[in] pxUsbDevice     USB device handle, returned when new device attached and enumerated.
 * @param[in] xEvent          USB host event.
 * @param[in] pvUserContext   User Context passed when setting the callback.
 *                            This is not used by the driver, but just passed back to the user
 *                            in the callback.
 */
typedef void (* IotUsbHostCallback_t) ( IotUsbDeviceHandle_t const pxUsbDevice,
                                        IotUsbHostEvent_t xEvent,
                                        void * pvUserContext );

/**
 * @brief   USB Host pipe transfer callback type. This callback is passed
 *          to the driver by calling iot_usb_host_set_pipe_callback.
 *
 * @param[in] ucStatus        Usb host transfer status.
 * @param[in] pvUserContext   User Context passed when setting the callback.
 *                            This is not used by the driver, but just passed back to the user
 *                            in the callback.
 */
typedef void (* IotUsbHostTransferCallback_t) ( void * pvUserContext,
                                                uint8_t ucStatus );

/**
 * @brief Attached device endpoint configuration structure.
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
 * @brief Attached device interface information.
 */
typedef struct IotUsbInterfaceInfo
{
    IotUsbDeviceEndpointConfig_t endpoint[ IOT_USB_HOST_MAX_ENDPOINTS ];
    uint8_t ucInterfaceIndex;
    uint8_t ucEpCount;
    uint8_t ucAlternateSettingNumber;
} IotUsbInterfaceInfo_t;

/**
 * @brief Attached device configuration information.
 */
typedef struct IotUsbConfigurationInfo
{
    IotUsbInterfaceInfo_t * pxInterface;
    uint8_t ucInterfaceCount;
} IotUsbConfigurationInfo_t;

/**
 *  @brief Attached device information.
 */
typedef struct IotUsbDeviceInfo
{
    uint8_t ucSpeed;      /*!< Device speed */
    uint8_t ucDevAddress; /*!< Device address */
    uint16_t usPid;       /*!< Usb product id */
    uint16_t usVid;       /*!< Usb vendor id */
} IotUsbDeviceInfo_t;

/**
 * @brief   iot_usb_host_open is used to initialize the usb host driver.
 *          This must be called before using any other usb host APIs.
 *
 * @param[in] lUsbHostControllerInstance The instance of usb host controller to initialize.
 *
 * @return
 *    - The handle IotUsbHostHandle_t on success
 *    - NULL if
 *       - invalid lUsbHostControllerInstance
 *       - open failed
 *       - already opened
 */
IotUsbHostHandle_t iot_usb_host_open( int32_t lUsbHostControllerInstance );

/**
 * @brief Set up usb host callback to be called on host event.
 *
 * @warning If input handle or if callback function is NULL, this function silently takes no action.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in the iot_usb_host_open() function.
 * @param[in] xCallback The callback function to be called on host event.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_usb_host_set_host_callback( IotUsbHostHandle_t const pxUsbHost,
                                     IotUsbHostCallback_t xCallback,
                                     void * pvUserContext );

/**
 * @brief set up usb pipe callback to be called on pipe transfer.
 *
 * @warning If input handle or if callback function is NULL, or invalid pipe handle,
 *          this function silently takes no action.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The usb pipe to be operated.
 * @param[in] xCallback The callback function to be called on pipe transfer.
 * @param[in] pvUserContext The user context to be passed back when callback is called.
 */
void iot_usb_host_set_pipe_callback( IotUsbHostHandle_t const pxUsbHost,
                                     IotPipeHandle_t pxPipe,
                                     IotUsbHostTransferCallback_t xCallback,
                                     void * pvUserContext );

/**
 * @brief This function is used to open a pipe per given device and endpoint descriptor.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] ucDevAddress The attached device address.
 * @param[in] pxEpConfig Given endpoint configuration.
 *
 * @return
 *   - Pipe handle on success
 *   - NULL if
 *      - invalid pxUsbHost
 *      - invalid ucDevAddress
 *      - invalid pxEpConfig
 */
IotPipeHandle_t iot_usb_host_pipe_open( IotUsbHostHandle_t const pxUsbHost,
                                        uint8_t ucDevAddress,
                                        IotUsbDeviceEndpointConfig_t * pxEpConfig );

/**
 * @brief This function is used to close given pipe.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The pipe handle to be closed.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 */
int32_t iot_usb_host_pipe_close( IotUsbHostHandle_t const pxUsbHost,
                                 IotPipeHandle_t pxPipe );

/**
 * @brief Starts the usb host read operation in non-blocking mode.
 *
 * @note iot_usb_host_set_pipe_callback() must be called first in order for
 *       the callback function to be called upon read is complete.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The pipe handle.
 * @param[out] pvBuffer The receive buffer to read the data into.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *      - pvBuffer is NULL.
 *      - xBytes == 0.
 *   - IOT_USB_HOST_BUSY if another operation is in progress on the same pipe.
 *   - IOT_USB_HOST_READ_FAIL if unable to complete the read operation.
 */
int32_t iot_usb_host_read_async( IotUsbHostHandle_t const pxUsbHost,
                                 IotPipeHandle_t pxPipe,
                                 void * const pvBuffer,
                                 size_t xBytes );

/**
 * @brief Starts the usb host write operation in non-blocking mode.
 *
 * @note iot_usb_host_set_pipe_callback() must be called first in order for
 *       the callback function to be called upon read is complete.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The pipe handle.
 * @param[out] pvBuffer The transmit buffer containing the data to be written. It must be allocated by caller
 *                      and remain valid before write complete.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *      - pvBuffer is NULL.
 *      - xBytes == 0.
 *   - IOT_USB_HOST_BUSY if another operation is in progress on the same pipe.
 *   - IOT_USB_HOST_WRITE_FAIL if unable to complete the write operation.
 */
int32_t iot_usb_host_write_async( IotUsbHostHandle_t const pxUsbHost,
                                  IotPipeHandle_t pxPipe,
                                  void * const pvBuffer,
                                  size_t xBytes );

/**
 * @brief Starts the usb host read operation in blocking mode.
 *
 * @note This function will block untill all xBytes are read.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The pipe handle.
 * @param[out] pvBuffer The receive buffer to read the data into.
 * @param[in] xBytes The number of bytes to read.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *      - pvBuffer is NULL.
 *      - xBytes == 0.
 *   - IOT_USB_HOST_BUSY if another operation is in progress on the same pipe.
 *   - IOT_USB_HOST_READ_FAIL if unable to complete the read operation.
 */
int32_t iot_usb_host_read_sync( IotUsbHostHandle_t const pxUsbHost,
                                IotPipeHandle_t pxPipe,
                                void * const pvBuffer,
                                size_t xBytes );

/**
 * @brief Starts the usb host write operation in blocking mode.
 *
 * @note This function will block untill all xBytes are written.
 *
 * @param[in] pxUsbHost The usb host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The pipe handle.
 * @param[out] pvBuffer The transmit buffer containing the data to be written. It must be allocated by caller
 *                      and remain valid before write complete.
 * @param[in] xBytes The number of bytes to write.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *      - pvBuffer is NULL.
 *      - xBytes == 0.
 *   - IOT_USB_HOST_BUSY if another operation is in progress on the same pipe.
 *   - IOT_USB_HOST_WRITE_FAIL if unable to complete the write operation.
 */
int32_t iot_usb_host_write_sync( IotUsbHostHandle_t const pxUsbHost,
                                 IotPipeHandle_t pxPipe,
                                 void * const pvBuffer,
                                 size_t xBytes );

/**
 * @brief This function is used to cancel the current operation in progress.
 *
 * @param[in] pxUsbHost The USB Host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The usb pipe on which user want to cancel operation.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *   - IOT_USB_HOST_FUNCTION_NOT_SUPPORTED if cancel transfer is not supported.
 *   - IOT_USB_HOST_NOTHING_TO_CANCEL if nothing to cancel.
 */
int32_t iot_usb_host_cancel( IotUsbHostHandle_t const pxUsbHost,
                             IotPipeHandle_t pxPipe );

/**
 * @brief Used for various usb host control function.
 *
 * @param[in] pxUsbDevice The USB device peripheral handle for host to control.
 * @param[in] IotUsbHostIoctlRequest_t The request should be one of IotUsbHostIoctlRequest_t.
 * @param[in,out] pvBuffer The request values for the usb host.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbDevice is NULL.
 *      - xRequest is invalid.
 *      - pvBuffer is NULL.
 *   - IOT_USB_HOST_FUNCTION_NOT_SUPPORTED if ioctl request not supported.
 *   - IOT_USB_HOST_ERROR for other error.
 */
int32_t iot_usb_host_ioctl( IotUsbDeviceHandle_t pxUsbDevice,
                            IotUsbHostIoctlRequest_t xRequest,
                            void * const pvBuffer );

/**
 * @brief Send setup packet to the pipe.
 *
 * @param[in] pxUsbHost The USB Host peripheral handle returned in iot_usb_host_open() function.
 * @param[in] pxPipe The usb pipe on which we are operating.
 * @param[in] pvSetup Setup packet.
 * @param[in] pucData Data to be sent if needed.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxPipe is NULL.
 *      - pvSetup is NULL.
 *      - pvData is NULL.
 *   - IOT_USB_HOST_ERROR for other error.
 */
int32_t iot_usb_host_send_setup( IotUsbHostHandle_t const pxUsbHost,
                                 IotPipeHandle_t pxPipe,
                                 void * pvSetup,
                                 uint8_t * pucData );

/**
 * @brief Close the USB hsot peripheral.
 *
 * @param[in] pxUsbHost The USB host handle returned in open() call.
 *
 * @return
 *   - IOT_USB_HOST_SUCCESS on success
 *   - IOT_USB_HOST_INVALID_VALUE if
 *      - pxUsbHost is NULL.
 *      - pxUsbHost not open (already closed).
 */
int32_t iot_usb_host_close( IotUsbHostHandle_t const pxUsbHost );

/**
 * @}
 */
/* end of group iot_usb_host */

#endif /* ifndef _IOT_USB_HOST_H_ */
