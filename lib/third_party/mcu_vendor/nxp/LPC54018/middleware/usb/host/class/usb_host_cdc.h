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

#ifndef __USB_HOST_CDC_H__
#define __USB_HOST_CDC_H__

/*!
 * @addtogroup usb_host_cdc_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Class-specific request */
/* CDC 1.2 */
/*! @brief CDC class-specific request (SEND_ENCAPSULATED_COMMAND) */
#define USB_HOST_CDC_SEND_ENCAPSULATED_COMMAND 0x00U
/*! @brief CDC class-specific request (GET_ENCAPSULATED_RESPONSE) */
#define USB_HOST_CDC_GET_ENCAPSULATED_RESPONSE 0x01U
/* PSTN */
/*! @brief CDC class-specific request (SET_LINE_CODING) */
#define USB_HOST_CDC_SET_LINE_CODING 0x20U
/*! @brief CDC class-specific request (GET_LINE_CODING) */
#define USB_HOST_CDC_GET_LINE_CODING 0x21U
/*! @brief CDC class-specific request (SET_CONTROL_LINE_STATE) */
#define USB_HOST_CDC_SET_CONTROL_LINE_STATE 0x22U

/*! @brief CDC class-specific notifications(SerialState) bitmap*/
#define USB_HOST_ACM_UART_STATE_BITMAP_BTXCARRITER 0x01U
/*! @brief CDC class-specific notifications(SerialState) bitmap*/
#define USB_HOST_ACM_UART_STATE_BITMAP_BRXCARRITER 0x02U
/*! @brief CDC class-specific notifications(SerialState) bitmap*/
#define USB_HOST_ACM_UART_STATE_BITMAP_BBREAK 0x04U
/*! @brief CDC class-specific notifications(SerialState) bitmap*/
#define USB_HOST_ACM_UART_STATE_BITMAP_BBRINGSIGNAL 0x10U

/*! @brief CDC class-specific request (SET_CONTROL_LINE_STATE) bitmap*/
#define USB_HOST_CDC_CONTROL_LINE_STATE_DTR 0x01U
/*! @brief CDC class-specific request (SET_CONTROL_LINE_STATE) bitmap*/
#define USB_HOST_CDC_CONTROL_LINE_STATE_RTS 0x02U

/*CDC SPEC*/
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_HEADER 0x00U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_CM 0x01U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_ACM 0x02U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_DLM 0x03U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_TR 0x04U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_TC_LSR 0x05U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_UNION 0x06U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_CS 0x07U
/*! @brief CDC class-specific bDescriptor SubType in functional descriptors*/
#define USB_HOST_DESC_SUBTYPE_TOM 0x08U

/*See the CDC specification page20*/
/*! @brief CDC class-specific code, Communications Interface Class Code*/
#define USB_HOST_CDC_COMMUNICATIONS_CLASS_CODE 0x02U
/*! @brief CDC class-specific code,Communications Class Subclass Codes*/
#define USB_HOST_CDC_SUBCLASS_ACM_CODE 0x02U
/*No class specific protocol required. See the CDC specification page22*/
#define USB_HOST_CDC_PROTOCOL_CODE 0x00U
/*! @brief CDC class-specific code,Data Class Interface Codes*/
#define USB_HOST_CDC_DATA_CLASS_CODE 0x0AU
/* This field is unused for Data Class interfaces and should have a value of 00h.*/
#define USB_HOST_CDC_DATA_SUBCLASS_CODE 0x00U
/*No class-specific protocol required. See the CDC specification page22*/
#define USB_HOST_CDC_DATA_PROTOCOL_CODE 0x00U

/*! @brief CDC GetLineCoding structure according to the 6.3 in PSTN specification */
typedef struct _usb_host_cdc_line_coding_struct
{
    uint32_t dwDTERate;  /*!< Data terminal rate, in bits per second*/
    uint8_t bCharFormat; /*!< Stop bits*/
    uint8_t bParityType; /*!< Parity*/
    uint8_t bDataBits;   /*!< Data bits (5, 6, 7, 8 or 16).*/
} usb_host_cdc_line_coding_struct_t;

/*! @brief CDC GetLineCoding structure according to the 6.3 in PSTN specification */
typedef struct _usb_host_cdc_control_line_state_struct
{
    uint16_t line_state; /*!< D1, This signal corresponds to V.24 signal 105 and RS-232 signal RTS*/
                         /*!< D0, This signal corresponds to V.24 signal 108/2 and RS-232 signal DTR*/
} usb_host_cdc_control_line_state_struct_t;

/*! @brief CDC SerialState structure according to the 6.5.4 in PSTN specification */
typedef struct _usb_host_cdc_acm_state_struct
{
    uint8_t reserved[8];  /*!< Notify response by the device, this is used as notification header which is return by the
                             device */
    uint8_t bmstate;      /*!< UART State Bitmap Values*/
    uint8_t reserved1[1]; /*!< Fix 4B align issue*/
    uint8_t reserved2[2]; /*!< Fix 4B align issue*/
} usb_host_cdc_acm_state_struct_t;

/*! @brief CDC Header Functional Descriptor structure according to the 5.2.3 in CDC specification */
typedef struct _usb_host_cdc_head_function_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE descriptor type.*/
    uint8_t bDescriptorSubtype; /*!<Header functional descriptor subtype.*/
    uint8_t bcdCDC[2]; /*!<USB Class Definitions for Communications Devices Specification release number in binary-coded
                          decimal.*/
} usb_host_cdc_head_function_desc_struct_t;

/*! @brief CDC Call Management Functional Descriptor structure according to the 5.3.1 in PSTN specification */
typedef struct _usb_host_cdc_call_manage_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Call Management functional descriptor subtype.*/
    uint8_t bmCapabilities;     /*!<The capabilities that this configuration supports.*/
    uint8_t bDataInterface;     /*!<Interface number of Data Class interface optionally used for call management.*/
} usb_host_cdc_call_manage_desc_struct_t;

/*! @brief CDC Abstract Control Management Functional Descriptor structure according to the 5.3.2 in PSTN specification
 */
typedef struct _usb_host_cdc_abstract_control_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Abstract Control Management functional descriptor subtype.*/
    uint8_t bmCapabilities;     /*!<The capabilities that this configuration supports.*/
} usb_host_cdc_abstract_control_desc_struct_t;

/*! @brief CDC Direct Line Management Functional Descriptor structure according to the 5.3.3 in PSTN specification */
typedef struct _usb_host_cdc_direct_line_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Direct Line Management functional descriptor subtype,.*/
    uint8_t bmCapabilities;     /*!<The capabilities that this configuration supports.*/
} usb_host_cdc_direct_line_desc_struct_t;

/*! @brief CDC Telephone Ringer Functional Descriptor structure according to the 5.3.4 in PSTN specification */
typedef struct _usb_host_cdc_telephone_ringer_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Telephone Ringer functional descriptor subtype*/
    uint8_t bRingerVolSteps;    /*!<Number of discrete steps in volume supported by the ringer,.*/
    uint8_t bNumRingerPatterns; /*!<Number of ringer patterns supported.*/
} usb_host_cdc_telephone_ringer_desc_struct_t;

/*! @brief CDC Telephone Call and Line State Reporting Capabilities Descriptor structure according to the 5.3.6 in PSTN
 * specification */
typedef struct _usb_host_cdc_tcLsr_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Telephone Call State Reporting Capabilities descriptor subtype.*/
    uint8_t bmCapabilities[4];  /*!<Call and line state reporting capabilities of the device.*/
} usb_host_cdc_tcLsr_desc_struct_t;

/*! @brief CDC Header Functional Descriptor structure according to the 5.2.3 in CDC specification */
typedef struct _usb_host_cdc_union_interface_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE descriptor type.*/
    uint8_t bDescriptorSubtype; /*!<Union Functional Descriptor SubType.*/
    uint8_t bControlInterface;  /*!<USB Class Definitions for Communications Devices Specification release number in
                                   binary-coded decimal.*/
} usb_host_cdc_union_interface_desc_struct_t;

/*! @brief CDC Telephone Operational Modes Functional Descriptor structure according to the 5.3.5 in PSTN specification
 */
typedef struct _usb_host_cdc_tom_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE.*/
    uint8_t bDescriptorSubtype; /*!<Telephone Operational Modes functional descriptor subtype.*/
    uint8_t bmCapabilities;     /*!<Operational modes:.*/
} usb_host_cdc_tom_desc_struct_t;

/*! @brief CDC common Functional Descriptor structure */
typedef struct _usb_host_cdc_common_desc_struct
{
    uint8_t bFunctionLength;    /*!<Size of this descriptor in bytes.*/
    uint8_t bDescriptorType;    /*!<CS_INTERFACE descriptor type.*/
    uint8_t bDescriptorSubtype; /*!<Header functional descriptor subtype.*/
} usb_host_cdc_common_desc_struct_t;

/*! @brief CDC union Functional Descriptor structure for analyze a class-specific descriptor */
typedef union _usb_cdc_func_desc_struct
{
    usb_host_cdc_common_desc_struct_t common;
    usb_host_cdc_head_function_desc_struct_t head;
    usb_host_cdc_union_interface_desc_struct_t unionDesc;
    usb_host_cdc_call_manage_desc_struct_t callManage;
    usb_host_cdc_abstract_control_desc_struct_t acm;
    usb_host_cdc_direct_line_desc_struct_t dl;
    usb_host_cdc_telephone_ringer_desc_struct_t tr;
    usb_host_cdc_tom_desc_struct_t tom;
    usb_host_cdc_tcLsr_desc_struct_t tcLsr;
} usb_cdc_func_desc_struct_t;

typedef struct _usb_host_cdc_instance_struct
{
    usb_host_handle hostHandle;                             /*!< The handle of the USB host. */
    usb_device_handle deviceHandle;                         /*!< The handle of the USB device structure. */
    usb_host_interface_handle controlInterfaceHandle;       /*!< The handle of the CDC device control interface. */
    usb_host_interface_handle dataInterfaceHandle;          /*!< The handle of the CDC device data interface. */
    usb_host_pipe_handle interruptPipe;                     /*!< Interrupt pipe for the CDC device data interface. */
    usb_host_pipe_handle controlPipe;                       /*!< Control pipe*/
    usb_host_pipe_handle inPipe;                            /*!< CDC bulk in pipe*/
    usb_host_pipe_handle outPipe;                           /*!< CDC bulk out pipe*/
    usb_host_cdc_head_function_desc_struct_t *headDesc;     /*!< CDC class-specific head function descriptor*/
    usb_host_cdc_call_manage_desc_struct_t *callManageDesc; /*!< cdCDCc class-specific call manage descriptor*/
    usb_host_cdc_abstract_control_desc_struct_t
        *abstractControlDesc; /*!< CDC class-specific abstract control descriptor*/
    usb_host_cdc_union_interface_desc_struct_t *unionInterfaceDesc; /*!< CDC class-specific union function descriptor*/
    void *interruptCallbackParam;                                   /*!< CDC interrupt in transfer callback parameter*/
    void *controlCallbackParam;                                     /*!< CDC control transfer callback parameter*/
    void *inCallbackParam;                                          /*!< CDC bulk in transfer callback parameter*/
    void *outCallbackParam;                                         /*!< CDC bulk out transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;                           /*!< Ongoing control transfer*/
    transfer_callback_t controlCallbackFn;   /*!< CDC control transfer callback function pointer*/
    transfer_callback_t interruptCallbackFn; /*!< CDC interrupt transfer callback function pointer*/
    transfer_callback_t outCallbackFn;       /*!< CDC bulk out transfer callback function pointer*/
    transfer_callback_t inCallbackFn;        /*!< CDC bulk in transfer callback function pointer*/
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    uint8_t *stallDataBuffer; /*!< Keep the data buffer for stall transfer's data*/
    uint32_t stallDataLength; /*!< Keep the data length for stall transfer's data*/
#endif
    uint16_t packetSize;        /*!< CDC control pipe maximum packet size*/
    uint16_t bulkOutPacketSize; /*!< CDC bulk out maximum packet size*/
    uint16_t bulkInPacketSize;  /*!< CDC bulk in maximum packet size*/
} usb_host_cdc_instance_struct_t;

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @name USB CDC host class driver
 * @{
 */

/*!
 * @brief Initializes the CDC instance.
 *
 * This function allocates the resource for the CDC instance.
 *
 * @param deviceHandle       The device handle.
 * @param classHandle Returns class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostCdcInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief CDC set data interface callback and opens pipes.
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

extern usb_status_t USB_HostCdcSetDataInterface(usb_host_class_handle classHandle,
                                                usb_host_interface_handle interfaceHandle,
                                                uint8_t alternateSetting,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief CDC set control interface callback and opens pipes.
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

extern usb_status_t USB_HostCdcSetControlInterface(usb_host_class_handle classHandle,
                                                   usb_host_interface_handle interfaceHandle,
                                                   uint8_t alternateSetting,
                                                   transfer_callback_t callbackFn,
                                                   void *callbackParam);

/*!
 * @brief Deinitializes the CDC instance.
 *
 * This function frees the resource for the CDC instance.
 *
 * @param deviceHandle   The device handle.
 * @param classHandle The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostCdcDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

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
 * @retval max      Packet size.
 */
extern uint16_t USB_HostCdcGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief Receives data.
 *
 * This function implements the CDC receiving data.
 *
 * @param classHandle   The class handle.
 * @param buffer         The buffer pointer.
 * @param bufferLength   The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostCdcDataRecv(usb_host_class_handle classHandle,
                                        uint8_t *buffer,
                                        uint32_t bufferLength,
                                        transfer_callback_t callbackFn,
                                        void *callbackParam);

/*!
 * @brief Sends data.
 *
 * This function implements the CDC sending data.
 *
 * @param classHandle   The class handle.
 * @param buffer         The buffer pointer.
 * @param bufferLength     The buffer length.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Receive request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy             There is no idle transfer.
 * @retval kStatus_USB_Error            Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostSend.
 */
extern usb_status_t USB_HostCdcDataSend(usb_host_class_handle classHandle,
                                        uint8_t *buffer,
                                        uint32_t bufferLength,
                                        transfer_callback_t callbackFn,
                                        void *callbackParam);

/*!
 * @brief Interrupts the receive data.
 *
 * This function implements the interrupt receiving data.
 *
 * @param classHandle    The class handle.
 * @param buffer          The buffer pointer.
 * @param bufferLength      The buffer length.
 * @param callbackFn     This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success         Receive request successfully.
 * @retval kStatus_USB_InvalidHandle   The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy            There is no idle transfer.
 * @retval kStatus_USB_Error           Pipe is not initialized.
 *                                    Or, send transfer fail. See the USB_HostRecv.
 */
extern usb_status_t USB_HostCdcInterruptRecv(usb_host_class_handle classHandle,
                                             uint8_t *buffer,
                                             uint32_t bufferLength,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief CDC get line coding.
 *
 * This function implements the CDC GetLineCoding request. See the PSTN specification.
 *
 * @param classHandle   The class handle.
 * @param uartLineCoding   The line coding pointer.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostCdcGetAcmLineCoding(usb_host_class_handle classHandle,
                                                usb_host_cdc_line_coding_struct_t *uartLineCoding,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief CDC setControlLineState.
 *
 * This function implements the CDC etControlLineState request. See PSTN specification.
 *
 * @param classHandle   The class handle.
 * @param dtr           The DRS value.
 * @param rts           The RTS value.
 * @param callbackFn    This callback is called after this function completes.
 * @param callbackParam The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostCdcSetAcmCtrlState(
    usb_host_class_handle classHandle, uint8_t dtr, uint8_t rts, transfer_callback_t callbackFn, void *callbackParam);

/*!
 * @brief cdc send encapsulated command.
 *
 * This function implements cdc SEND_ENCAPSULATED_COMMAND request.refer to cdc 1.2 spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcSendEncapsulatedCommand(usb_host_class_handle classHandle,
                                                uint8_t *buffer,
                                                uint16_t bufferLength,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief cdc get encapsulated response.
 *
 * This function implements cdc GET_ENCAPSULATED_RESPONSE request.refer to cdc 1.2 spec.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcGetEncapsulatedResponse(usb_host_class_handle classHandle,
                                                uint8_t *buffer,
                                                uint16_t bufferLength,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief CDC gets the ACM descriptor.
 *
 * This function is hunting for the class-specific ACM descriptor in the configuration and gets the corresponding
 * descriptor.
 *
 * @param classHandle          The class handle.
 * @param headDesc             The head function descriptor pointer.
 * @param callManageDesc       The call management functional descriptor pointer.
 * @param abstractControlDesc  The abstract control management functional pointer.
 * @param unionInterfaceDesc   The union functional descriptor pointer.
 *
 * @retval kStatus_USB_Error          Analyse descriptor error.
 */
extern usb_status_t USB_HostCdcGetAcmDescriptor(usb_host_class_handle classHandle,
                                                usb_host_cdc_head_function_desc_struct_t **headDesc,
                                                usb_host_cdc_call_manage_desc_struct_t **callManageDesc,
                                                usb_host_cdc_abstract_control_desc_struct_t **abstractControlDesc,
                                                usb_host_cdc_union_interface_desc_struct_t **unionInterfaceDesc);

/*!
 * @brief CDC send control transfer common code.
 *
 * @param classHandle    The class handle.
 * @param request_type   Set up the packet request type.
 * @param request        Set up the packet request value.
 * @param wvalue_l       Set up the packet wvalue low byte.
 * @param wvalue_h       Set up the packet wvalue high byte.
 * @param wlength        Set up the packet wlength value.
 * @param data           Data buffer pointer
 * @param callbackFn     This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
extern usb_status_t USB_HostCdcControl(usb_host_class_handle classHandle,
                                       uint8_t request_type,
                                       uint8_t request,
                                       uint8_t wvalue_l,
                                       uint8_t wvalue_h,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);
/*@}*/

#ifdef __cplusplus
}
#endif
/*@}*/

#endif /*__USB_HOST_CDC_H__*/
