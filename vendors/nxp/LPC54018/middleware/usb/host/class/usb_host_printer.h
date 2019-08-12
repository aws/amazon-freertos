/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

#ifndef __USB_HOST_PRINTER_H__
#define __USB_HOST_PRINTER_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @addtogroup usb_host_printer_drv
 * @{
 */

/*! @brief Printer class code */
#define USB_HOST_PRINTER_CLASS_CODE (7U)
/*! @brief Printer sub-class code */
#define USB_HOST_PRINTER_SUBCLASS_CODE (1U)
/*! @brief Printer class protocol code (Unidirectional interface) */
#define USB_HOST_PRINTER_PROTOCOL_UNIDIRECTION (1U)
/*! @brief Printer class protocol code (Bi-directional interface) */
#define USB_HOST_PRINTER_PROTOCOL_BIDIRECTION (2U)
/*! @brief Printer class protocol code (IEEE® 1284.4 compatible bi-directional interface) */
#define USB_HOST_PRINTER_PROTOCOL_IEEE1284 (3U)

/*! @brief Printer class-specific request (GET_DEVICE_ID) */
#define USB_HOST_PRINTER_GET_DEVICE_ID (0)
/*! @brief Printer class-specific request (GET_PORT_STATUS) */
#define USB_HOST_PRINTER_GET_PORT_STATUS (1)
/*! @brief Printer class-specific request (SOFT_RESET) */
#define USB_HOST_PRINTER_SOFT_RESET (2)

/*! @brief Paper empty bit mask for GET_PORT_STATUS */
#define USB_HOST_PRINTER_PORT_STATUS_PAPER_EMPTRY_MASK (0x20U)
/*! @brief Select bit mask for GET_PORT_STATUS */
#define USB_HOST_PRINTER_PORT_STATUS_SELECT_MASK (0x10U)
/*! @brief Error bit mask for GET_PORT_STATUS */
#define USB_HOST_PRINTER_PORT_STATUS_NOT_ERROR_MASK (0x08U)

/*! @brief Printer instance structure and printer usb_host_class_handle pointer to this structure */
typedef struct _usb_host_printer_instance
{
    usb_host_handle hostHandle;                /*!< This instance's related host handle*/
    usb_device_handle deviceHandle;            /*!< This instance's related device handle*/
    usb_host_interface_handle interfaceHandle; /*!< This instance's related interface handle*/
    usb_host_pipe_handle controlPipe;          /*!< This instance's related device control pipe*/
    usb_host_pipe_handle inPipe;               /*!< Printer bulk in pipe*/
    usb_host_pipe_handle outPipe;              /*!< Printer bulk out pipe*/
    transfer_callback_t inCallbackFn;          /*!< Printer bulk in transfer callback function pointer*/
    void *inCallbackParam;                     /*!< Printer bulk in transfer callback parameter*/
    transfer_callback_t outCallbackFn;         /*!< Printer bulk out transfer callback function pointer*/
    void *outCallbackParam;                    /*!< Printer bulk out transfer callback parameter*/
    transfer_callback_t controlCallbackFn;     /*!< Printer control transfer callback function pointer*/
    void *controlCallbackParam;                /*!< Printer control transfer callback parameter*/
    usb_host_transfer_t *controlTransfer;      /*!< Ongoing control transfer*/
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    uint8_t *stallDataBuffer;
    uint32_t stallDataLength;
#endif

    uint16_t inPacketSize;  /*!< Printer bulk in maximum packet size*/
    uint16_t outPacketSize; /*!< Printer bulk out maximum packet size*/
} usb_host_printer_instance_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @name USB host printer class APIs
 * @{
 */

/*!
 * @brief Initializes the printer instance.
 *
 * This function allocate the resource for the printer instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[out] classHandle   Return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
extern usb_status_t USB_HostPrinterInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief Sets the interface.
 *
 * This function binds the interface with the printer instance.
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
extern usb_status_t USB_HostPrinterSetInterface(usb_host_class_handle classHandle,
                                                usb_host_interface_handle interfaceHandle,
                                                uint8_t alternateSetting,
                                                transfer_callback_t callbackFn,
                                                void *callbackParam);

/*!
 * @brief De-initializes the printer instance.
 *
 * This function frees the resources for the printer instance.
 *
 * @param[in] deviceHandle   The device handle.
 * @param[in] classHandle    The class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
extern usb_status_t USB_HostPrinterDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

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
 * @retval          Maximum Packet size.
 */
extern uint16_t USB_HostPrinterGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief Receives data.
 *
 * This function implements the printer receiving data.
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
extern usb_status_t USB_HostPrinterRecv(usb_host_class_handle classHandle,
                                        uint8_t *buffer,
                                        uint32_t bufferLength,
                                        transfer_callback_t callbackFn,
                                        void *callbackParam);

/*!
 * @brief Sends data.
 *
 * This function implements the printer sending data.
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
extern usb_status_t USB_HostPrinterSend(usb_host_class_handle classHandle,
                                        uint8_t *buffer,
                                        uint32_t bufferLength,
                                        transfer_callback_t callbackFn,
                                        void *callbackParam);

/*!
 * @brief Gets the pipe maximum packet size.
 *
 * @param[in] classHandle The class handle.
 * @param[in] pipeType    Its value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT. See the usb_spec.h
 * @param[in] direction   Pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval          Maximum Packet size.
 */
extern uint16_t USB_HostPrinterGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction);

/*!
 * @brief Printer get device ID.
 *
 * This function implements the printer class-specific request (GET_DEVICE_ID).
 *
 * @param[in] classHandle      The class handle.
 * @param[in] interfaceIndex   Interface index.
 * @param[in] alternateSetting Get the alternateSetting's device ID.
 * @param[out] buffer          The buffer pointer.
 * @param[in] length           The buffer length.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostPrinterGetDeviceId(usb_host_class_handle classHandle,
                                               uint8_t interfaceIndex,
                                               uint8_t alternateSetting,
                                               uint8_t *buffer,
                                               uint32_t length,
                                               transfer_callback_t callbackFn,
                                               void *callbackParam);

/*!
 * @brief Printer get port status.
 *
 * This function implements the printer class-specific request (GET_PORT_STATUS).
 *
 * @param[in] classHandle      The class handle.
 * @param[in] portStatus       Port status buffer.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostPrinterGetPortStatus(usb_host_class_handle classHandle,
                                                 uint8_t *portStatus,
                                                 transfer_callback_t callbackFn,
                                                 void *callbackParam);

/*!
 * @brief Printer soft reset.
 *
 * This function implements the printer class-specific request (SOFT_RESET).
 *
 * @param[in] classHandle      The class handle.
 * @param[in] portStatus       Port status buffer.
 * @param[in] callbackFn       This callback is called after this function completes.
 * @param[in] callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Request successful.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          Send transfer fail. See the USB_HostSendSetup.
 */
extern usb_status_t USB_HostPrinterSoftReset(usb_host_class_handle classHandle,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);
/*! @}*/

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* __USB_HOST_PRINTER_H__ */
