/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#ifndef __USB_HOST_PHDC_H__
#define __USB_HOST_PHDC_H__

/*******************************************************************************
 * PHDC class public structure, enumeration, macro, function
 ******************************************************************************/
/*!
 * @addtogroup usb_host_phdc_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief PHDC class code */
#define USB_HOST_PHDC_CLASS_CODE (0x0FU)
/*! @brief PHDC sub class code */
#define USB_HOST_PHDC_SUBCLASS_CODE (0x00U)
/*! @brief PHDC protocol */
#define USB_HOST_PHDC_PROTOCOL (0x00U)
/*! @brief PHDC get status request */
#define USB_HOST_PHDC_GET_STATUS_REQUEST (0x00U)
/*! @brief PHDC set feature request */
#define USB_HOST_PHDC_SET_FEATURE_REQUEST (0x03U)
/*! @brief PHDC clear feature request */
#define USB_HOST_PHDC_CLEAR_FEATURE_REQUEST (0x01U)
/*! @brief PHDC meta-data feature */
#define USB_HOST_PHDC_FEATURE_METADATA (0x01U)
/*! @brief PHDC QoS information encoding feature */
#define USB_HOST_PHDC_QOS_ENCODING_VERSION (0x01U)

/*! @brief meta-data message preamble signature size */
#define USB_HOST_PHDC_MESSAGE_PREAMBLE_SIGNATURE_SIZE (0x10U)

/*! @brief PHDC class function descriptor type */
#define USB_HOST_PHDC_CLASSFUNCTION_DESCRIPTOR (0x20U)
/*! @brief PHDC QoS descriptor type */
#define USB_HOST_PHDC_QOS_DESCRIPTOR (0x21U)
/*! @brief PHDC function extension descriptor type */
#define USB_HOST_PHDC_11073PHD_FUNCTION_DESCRIPTOR (0x30U)
/*! @brief PHDC meta-data descriptor type */
#define USB_HOST_PHDC_METADATA_DESCRIPTOR (0x22U)

/*! @brief PHDC class function descriptor structure as defined by the PHDC class specification */
typedef struct _usb_host_phdc_class_function_desciptor
{
    uint8_t bLength;         /*!< Class function descriptor length */
    uint8_t bDescriptortype; /*!< PHDC_CLASSFUNCTION_DESCRIPTOR type */
    uint8_t bPhdcDataCode;   /*!< Data/Messaging format code */
    uint8_t bmCapability;    /*!< If bit 0 is 1 the meta-data message preamble is implemented and 0 if it is not */
} usb_host_phdc_class_function_desciptor_t;

/*! @brief Function extension descriptor (device specialization) structure as defined by the PHDC class specification.
 */
typedef struct _usb_host_phdc_function_extension_descriptor
{
    uint8_t bLength;               /*!< Function extension descriptor length */
    uint8_t bDescriptortype;       /*!< PHDC_CLASSFUNCTION_DESCRIPTOR type */
    uint8_t bReserved;             /*!< Reserved for future use */
    uint8_t bNumDevSpecs;          /*!< Number of wDevSpecializations */
    uint16_t *wDevSpecializations; /*!< Variable length list that defines the device specialization */
} usb_host_phdc_function_extension_descriptor_t;

/*! @brief QoS descriptor structure as defined by the PHDC class specification. */
typedef struct _usb_host_phdc_qos_descriptor
{
    uint8_t bLength;              /*!< QoS descriptor length */
    uint8_t bDescriptortype;      /*!< PHDC_QOS_DESCRIPTOR type */
    uint8_t bQosEncodingVersion;  /*!< Version of QoS information encoding */
    uint8_t bmLatencyReliability; /*!< Latency/reliability bin for the QoS data */
} usb_host_phdc_qos_descriptor_t;

/*! @brief Metadata descriptor structure as defined by the PHDC class specification. */
typedef struct _usb_host_phdc_metadata_descriptor
{
    uint8_t bLength;         /*!< Metadata descriptor length */
    uint8_t bDescriptortype; /*!< Descriptor type */
    uint8_t *bOpaqueData;    /*!< Opaque metadata */
} usb_host_phdc_metadata_descriptor_t;

/*! @brief Metadata message preamble structure as defined by the PHDC class specification. */
typedef struct _usb_host_phdc_metadata_preamble
{
    uint8_t
        aSignature[USB_HOST_PHDC_MESSAGE_PREAMBLE_SIGNATURE_SIZE]; /*!< Constant used to give preamble verifiability */
    uint8_t bNumberTransfers;     /*!< Count of following transfer to which the QoS setting applies */
    uint8_t bQosEncodingVersion;  /*!< Version of QoS information encoding */
    uint8_t bmLatencyReliability; /*!< See latency/reliability bin for the QoS data */
    uint8_t bOpaqueDataSize;      /*!< Opaque QoS data or meta-data size */
    uint8_t *bOpaqueData;         /*!< Opaque metadata */
} usb_host_phdc_metadata_preamble_t;

/*! @brief PHDC instance structure */
typedef struct _usb_host_phdc_instance
{
    usb_host_handle hostHandle;                /*!< The host handle */
    usb_device_handle deviceHandle;            /*!< The device handle */
    usb_host_interface_handle interfaceHandle; /*!< The interface handle */
    usb_host_pipe_handle controlPipe;          /*!< The control pipe */
    usb_host_pipe_handle interruptPipe;        /*!< The interrupt pipe */
    usb_host_pipe_handle bulkInPipe;           /*!< The bulk in pipe */
    usb_host_pipe_handle bulkOutPipe;          /*!< The bulk out pipe */
    transfer_callback_t inCallbackFn;          /*!< The callback function is called when the PHDC receives complete */
    void *inCallbackParam;                     /*!< The first parameter of the in callback function */
    transfer_callback_t outCallbackFn;         /*!< The callback function is called when the PHDC sends complete */
    void *outCallbackParam;                    /*!< The first parameter of the out callback function */
    transfer_callback_t controlCallbackFn;     /*!< The control callback function */
    void *controlCallbackParam;                /*!< The first parameter of the control callback function */
    usb_host_transfer_t *controlTransfer;      /*!< The control transfer pointer */

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    uint8_t *stallDataBuffer; /*!< keep the data buffer for stall transfer's data*/
    uint32_t stallDataLength; /*!< keep the data length for stall transfer's data*/
#endif

    usb_host_ep_t interruptInEndpointInformation; /*!< The interrupt in information */
    usb_host_ep_t bulkInEndpointInformation;      /*!< The bulk in information */
    usb_host_ep_t bulkOutEndpointInformation;     /*!< The bulk out information */
    uint8_t isMessagePreambleEnabled; /*!< The flag is used to check the message preamble feature is enabled or not */
    uint8_t numberTransferBulkOut;    /*!< The number of transfer that follow Meta-data Message Preamble */
    uint8_t numberTransferBulkIn;     /*!< The number of transfer that follow Meta-data Message Preamble */
} usb_host_phdc_instance_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host PHDC class APIs
 * @{
 */

/*!
 * @brief Initializes the PHDC instance.
 *
 * This function allocates the resource for PHDC instance.
 *
 * @param deviceHandle       The device handle.
 * @param classHandle        Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostPhdcInit(usb_host_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets an interface.
 *
 * This function binds the interface with the PHDC instance.
 *
 * @param classHandle      The class handle.
 * @param interfaceHandle  The interface handle.
 * @param alternateSetting The alternate setting value.
 * @param callbackFn       This callback is called after this function completes.
 * @param callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 * @retval kStatus_USB_Busy           Callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  Callback return status, the transfer is stalled by the device.
 * @retval kStatus_USB_Error          Callback return status, open pipe fail. See the USB_HostOpenPipe.
 */
extern usb_status_t USB_HostPhdcSetInterface(usb_host_class_handle classHandle,
                                             usb_host_interface_handle interfaceHandle,
                                             uint8_t alternateSetting,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief Deinitializes the PHDC instance.
 *
 * This function frees the resource for the PHDC instance.
 *
 * @param deviceHandle   The device handle.
 * @param classHandle The class handle.
 *
 * @retval kStatus_USB_Success        The device is deinitialized successfully.
 */
extern usb_status_t USB_HostPhdcDeinit(usb_host_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief Receives data.
 *
 * This function implements the PHDC receiving data.
 *
 * @param classHandle   The class handle.
 * @param qos           QoS of the data being received.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostPhdcRecv(usb_host_class_handle classHandle,
                                     uint8_t qos,
                                     uint8_t *buffer,
                                     uint32_t bufferLength,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam);

/*!
 * @brief Sends data.
 *
 * This function implements the PHDC sending data.
 *
 * @param classHandle   The class handle.
 * @param buffer        The buffer pointer.
 * @param bufferLength  The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostPhdcSend(usb_host_class_handle classHandle,
                                     uint8_t *buffer,
                                     uint32_t bufferLength,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam);

/*!
 * @brief PHDC sends the control request.
 *
 * @param classHandle   The class handle.
 * @param request       Setup packet request.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostPhdcSendControlRequest(usb_host_class_handle classHandle,
                                                   uint8_t request,
                                                   transfer_callback_t callbackFn,
                                                   void *callbackParam);

/*!
 * @brief PHDC set and clear feature endpoint halt request.
 *
 * @param classHandle   The class handle.
 * @param request       Setup packet request.
 * @param param         Request parameter
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostPhdcSetClearFeatureEndpointHalt(usb_host_class_handle classHandle,
                                                            uint8_t request,
                                                            void *param,
                                                            transfer_callback_t callbackFn,
                                                            void *callbackParam);

/*!
 * @brief USB_HostPhdcGetEndpointInformation.
 * This function returns the PHDC endpoint information structure, which contains an endpoint
 * descriptor and an endpoint extended descriptor.
 *
 * @param classHandle   The class handle.
 * @param pipeType      Pipe type.
 * @param direction     Pipe direction.
 *
 * @retval endpointReturn   All input parameters are valid.
 * @retval NULL             One or more input parameters are invalid.
 */
usb_host_ep_t *USB_HostPhdcGetEndpointInformation(usb_host_class_handle classHandle,
                                                  uint8_t pipeType,
                                                  uint8_t direction);

/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _USB_HOST_PHDC_H_ */
