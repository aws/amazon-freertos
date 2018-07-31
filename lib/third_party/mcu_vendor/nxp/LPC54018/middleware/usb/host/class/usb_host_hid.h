/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_HOST_HID_H_
#define _USB_HOST_HID_H_

/*******************************************************************************
 * HID class public structure, enumerations, macros, functions
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_hid_drv
 * @{
 */

/*! @brief HID class-specific request (get report) */
#define USB_HOST_HID_GET_REPORT (0x01U)
/*! @brief HID class-specific request (get idle) */
#define USB_HOST_HID_GET_IDLE (0x02U)
/*! @brief HID class-specific request (get protocol) */
#define USB_HOST_HID_GET_PROTOCOL (0x03U)
/*! @brief HID class-specific request (set report) */
#define USB_HOST_HID_SET_REPORT (0x09U)
/*! @brief HID class-specific request (set idle) */
#define USB_HOST_HID_SET_IDLE (0x0AU)
/*! @brief HID class-specific request (set protocol) */
#define USB_HOST_HID_SET_PROTOCOL (0x0BU)

/*! @brief HID class code */
#define USB_HOST_HID_CLASS_CODE (3U)
/*! @brief HID sub-class code */
#define USB_HOST_HID_SUBCLASS_CODE_NONE (0U)
/*! @brief HID sub-class code */
#define USB_HOST_HID_SUBCLASS_CODE_BOOT (1U)
/*! @brief HID class protocol code */
#define USB_HOST_HID_PROTOCOL_KEYBOARD (1U)
/*! @brief HID class protocol code */
#define USB_HOST_HID_PROTOCOL_MOUSE (2U)
/*! @brief HID class protocol code */
#define USB_HOST_HID_PROTOCOL_NONE (0U)

/*! @brief HID get/set protocol request data code */
#define USB_HOST_HID_REQUEST_PROTOCOL_BOOT (0U)
/*! @brief HID get/set protocol request data code */
#define USB_HOST_HID_REQUEST_PROTOCOL_REPORT (1U)

/*! @brief HID instance structure and HID usb_host_class_handle pointer to this structure */
typedef struct _usb_host_hid_instance
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle*/
    usb_host_interface_handle interfaceHandle; /*!< This instance's related interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< This instance's related device control pipe*/
    usb_host_pipe_handle inPipe;               /*!< HID interrupt in pipe*/
    usb_host_pipe_handle outPipe;              /*!< HID interrupt out pipe*/
    transfer_callback_t inCallbackFn;          /*!< HID interrupt in transfer callback function pointer*/
    void *inCallbackParam;                     /*!< HID interrupt in transfer callback parameter*/
    transfer_callback_t outCallbackFn;         /*!< HID interrupt out transfer callback function pointer*/
    void *outCallbackParam;                    /*!< HID interrupt out transfer callback parameter*/
    transfer_callback_t controlCallbackFn;     /*!< HID control transfer callback function pointer*/
    void *controlCallbackParam;                /*!< HID control transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;      /*!< Ongoing control transfer*/
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    uint8_t *stallDataBuffer; /*!< keep the data buffer for stall transfer's data*/
    uint32_t stallDataLength; /*!< keep the data length for stall transfer's data*/
#endif

    uint16_t inPacketSize;  /*!< HID interrupt in maximum packet size*/
    uint16_t outPacketSize; /*!< HID interrupt out maximum packet size*/
} usb_host_hid_instance_t;

/*! @brief HID descriptor structure according to the 6.2.1 in HID specification */
typedef struct _usb_host_hid_descriptor
{
    uint8_t bLength;              /*!< Total size of the HID descriptor*/
    uint8_t bDescriptorType;      /*!< Constant name specifying type of HID descriptor*/
    uint8_t bcdHID[2];            /*!< Numeric expression identifying the HID Class Specification release*/
    uint8_t bCountryCode;         /*!< Numeric expression identifying country code of the localized hardware*/
    uint8_t bNumDescriptors;      /*!< Numeric expression specifying the number of class descriptors*/
    uint8_t bHidDescriptorType;   /*!< Constant name identifying type of class descriptor*/
    uint8_t wDescriptorLength[2]; /*!< Numeric expression that is the total size of the Report descriptor*/
} usb_host_hid_descriptor_t;

/*! @brief HID descriptor structure according to the 6.2.1 in HID specification */
typedef struct _usb_host_hid_class_descriptor
{
    uint8_t bHidDescriptorType;   /*!< Constant name specifying type of optional descriptor*/
    uint8_t wDescriptorLength[2]; /*!< Numeric expression that is the total size of the optional descriptor*/
} usb_host_hid_class_descriptor_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host HID class APIs
 * @{
 */

/*!
 * @brief Initializes the HID instance.
 *
 * This function allocate the resource for the HID instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[out] classHandle   Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostHidInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets the interface.
 *
 * This function binds the interface with the HID instance.
 *
 * @param[in] classHandle      The class handle.
 * @param[in] interfaceHandle  The interface handle.
 * @param[in] alternateSetting The alternate setting value.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostHidSetInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam);

/*!
 * @brief Deinitializes the HID instance.
 *
 * This function frees the resources for the HID instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[in] classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostHidDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Gets the pipe maximum packet size.
 *
 * @param[in] classHandle The class handle.
 * @param[in] pipeType    Its value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                        See the usb_spec.h
 * @param[in] direction   Pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval          Maximum packet size.
 */
extern uint16_t USB_HostHidGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief HID get report descriptor.
 *
 * This function implements the HID report descriptor request.
 *
 * @param[in] classHandle   The class handle.
 * @param[out] buffer        The buffer pointer.
 * @param[in] buffer_len     The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidGetReportDescriptor(usb_host_class_handle classHandle,
                                                   uint8_t *buffer,
                                                   uint16_t buffer_len,
                                                   transfer_callback_t callbackFn,
                                                   void *callbackParam);

/*!
 * @brief Receives data.
 *
 * This function implements the HID receiving data.
 *
 * @param[in] classHandle   The class handle.
 * @param[out] buffer       The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostHidRecv(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLength,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief Sends data.
 *
 * This function implements the HID sending data.
 *
 * @param[in] classHandle   The class handle.
 * @param[in] buffer        The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostHidSend(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLength,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief HID get idle.
 *
 * This function implements the HID class-specific request (get idle).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[out] idleRate     Return idle rate value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidGetIdle(usb_host_class_handle classHandle,
                                       uint8_t reportId,
                                       uint8_t *idleRate,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief HID set idle.
 *
 * This function implements the HID class-specific request (set idle).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] idleRate      Idle rate value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidSetIdle(usb_host_class_handle classHandle,
                                       uint8_t reportId,
                                       uint8_t idleRate,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*!
 * @brief HID get protocol.
 *
 * This function implements the HID class-specific request (get protocol).
 *
 * @param[in] classHandle   The class handle.
 * @param[out] protocol      Return protocol value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidGetProtocol(usb_host_class_handle classHandle,
                                           uint8_t *protocol,
                                           transfer_callback_t callbackFn,
                                           void *callbackParam);

/*!
 * @brief HID set protocol.
 *
 * This function implements the HID class-specific request (set protocol).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] protocol      Protocol value.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidSetProtocol(usb_host_class_handle classHandle,
                                           uint8_t protocol,
                                           transfer_callback_t callbackFn,
                                           void *callbackParam);

/*!
 * @brief HID get report.
 *
 * This function implements the HID class-specific request (get report).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] reportType    Report type.
 * @param[out] buffer       The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidGetReport(usb_host_class_handle classHandle,
                                         uint8_t reportId,
                                         uint8_t reportType,
                                         uint8_t *buffer,
                                         uint32_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*!
 * @brief HID set report.
 *
 * This function implements the HID class-specific request (set report).
 *
 * @param[in] classHandle   The class handle.
 * @param[in] reportId      Report ID.
 * @param[in] reportType    Report type.
 * @param[in] buffer        The buffer pointer.
 * @param[in] bufferLength  The buffer length.
 * @param[in] callbackFn    This callback is called after this function completes.
 * @param[in] callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostHidSetReport(usb_host_class_handle classHandle,
                                         uint8_t reportId,
                                         uint8_t reportType,
                                         uint8_t *buffer,
                                         uint32_t bufferLength,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam);

/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _USB_HOST_HID_H_ */
