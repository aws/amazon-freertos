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

#include "usb_host_config.h"
#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))
#include "usb_host.h"
#include "usb_host_hub.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief hub control transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHubControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hub interrupt in transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHubInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief USB_HostHubSendPortReset's transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHubResetCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hub control transfer common code.
 *
 * @param classHandle   the class handle.
 * @param requestType   request type.
 * @param request        setup packet request field.
 * @param wvalue         setup packet wValue field.
 * @param windex         setup packet wIndex field.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostHubClassRequestCommon(usb_host_class_handle classHandle,
                                                  uint8_t requestType,
                                                  uint8_t request,
                                                  uint16_t wvalue,
                                                  uint16_t windex,
                                                  uint8_t *buffer,
                                                  uint16_t bufferLength,
                                                  transfer_callback_t callbackFn,
                                                  void *callbackParam);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void USB_HostHubControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)param;

    hubInstance->controlTransfer = NULL;
    if (hubInstance->controlCallbackFn)
    {
        /* callback to application, callback function is initialized in the USB_HostPrinterControl,
        USB_HostPrinterSetInterface
        or USB_HostHubClassRequestCommon, but is the same function */
        hubInstance->controlCallbackFn(hubInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status); /* callback to application */
    }
    USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
}

static void USB_HostHubInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)param;

    if (hubInstance->inCallbackFn)
    {
        /* callback to application, callback function is initialized in the USB_HostHubInterruptRecv */
        hubInstance->inCallbackFn(hubInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                  status); /* callback to application */
    }
    USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
}

static void USB_HostHubResetCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)param;

    /* note: there is not callback to application, the re-enumeration will start automatically after reset. */
    USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
}

static usb_status_t USB_HostHubClassRequestCommon(usb_host_class_handle classHandle,
                                                  uint8_t requestType,
                                                  uint8_t request,
                                                  uint16_t wvalue,
                                                  uint16_t windex,
                                                  uint8_t *buffer,
                                                  uint16_t bufferLength,
                                                  transfer_callback_t callbackFn,
                                                  void *callbackParam)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (hubInstance->controlTransfer != NULL)
    {
        return kStatus_USB_Busy;
    }

    /* get transfer */
    if (USB_HostMallocTransfer(hubInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }

    /* save hub application callback */
    hubInstance->controlCallbackFn = callbackFn;
    hubInstance->controlCallbackParam = callbackParam;

    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostHubControlCallback;
    transfer->callbackParam = hubInstance;
    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalue);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(windex);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(bufferLength);

    /* send transfer */
    if (USB_HostSendSetup(hubInstance->hostHandle, hubInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("Error in hid get report descriptor\r\n");
#endif
        USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    hubInstance->controlTransfer = transfer; /* record the on-going setup transfer */
    return kStatus_USB_Success;
}

usb_status_t USB_HostHubInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    /* malloc the hub instance */
    usb_host_hub_instance_t *hubInstance =
        (usb_host_hub_instance_t *)USB_OsaMemoryAllocate(sizeof(usb_host_hub_instance_t));

    uint32_t infoValue;

    if (hubInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
    hubInstance->hubDescriptor = (uint8_t *)SDK_Malloc(7 + (USB_HOST_HUB_MAX_PORT >> 3) + 1, USB_CACHE_LINESIZE);
    hubInstance->portStatusBuffer = (uint8_t *)SDK_Malloc(4, USB_CACHE_LINESIZE);
    hubInstance->hubStatusBuffer = (uint8_t *)SDK_Malloc(4, USB_CACHE_LINESIZE);
    hubInstance->hubBitmapBuffer = (uint8_t *)SDK_Malloc((USB_HOST_HUB_MAX_PORT >> 3) + 1, USB_CACHE_LINESIZE);
#endif

    /* initialize hub instance structure */
    hubInstance->deviceHandle = deviceHandle;
    hubInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    hubInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    hubInstance->controlPipe = (usb_host_pipe_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceLevel, &infoValue);
    hubInstance->hubLevel = infoValue;

    *classHandle = hubInstance; /* return the hub class handle */
    return kStatus_USB_Success;
}

usb_status_t USB_HostHubSetInterface(usb_host_class_handle classHandle,
                                     usb_host_interface_handle interfaceHandle,
                                     uint8_t alternateSetting,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    usb_status_t status;
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)classHandle;
    usb_host_interface_t *interface = (usb_host_interface_t *)interfaceHandle;
    usb_descriptor_endpoint_t *epDesc = NULL;
    usb_host_pipe_init_t pipeInit;
    uint8_t epIndex;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    hubInstance->interfaceHandle = interfaceHandle; /* save the interface handle */

    /* notify the host driver that the interface is used by class */
    status = USB_HostOpenDeviceInterface(hubInstance->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    /* close opened hub interrupt pipe */
    if (hubInstance->interruptPipe != NULL)
    {
        status = USB_HostCancelTransfer(hubInstance->hostHandle, hubInstance->interruptPipe, NULL);
        status = USB_HostClosePipe(hubInstance->hostHandle, hubInstance->interruptPipe);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        hubInstance->interruptPipe = NULL;
    }

    /* open hub interrupt pipe */
    for (epIndex = 0; epIndex < interface->epCount; ++epIndex)
    {
        epDesc = interface->epList[epIndex].epDesc;
        if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            /* get pipe information from endpoint descriptor */
            pipeInit.devInstance = hubInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            /* open hub interrupt in pipe */
            status = USB_HostOpenPipe(hubInstance->hostHandle, &hubInstance->interruptPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_hid_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
            break;
        }
    }

    /* hub don't support alternatesetting that is not 0 */
    if (alternateSetting == 0)
    {
        if (callbackFn != NULL)
        {
            callbackFn(callbackParam, NULL, 0, kStatus_USB_Success);
        }
    }
    else
    {
#ifdef HOST_ECHO
        usb_echo("host don't supprt alternate setting\r\n");
#endif
        return kStatus_USB_Error;
    }

    return status;
}

usb_status_t USB_HostHubDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)classHandle;
    uint8_t status;
    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL)
    {
        /* close opened hub interrupt pipe */
        if (hubInstance->interruptPipe != NULL)
        {
            status = USB_HostCancelTransfer(hubInstance->hostHandle, hubInstance->interruptPipe, NULL);
            status = USB_HostClosePipe(hubInstance->hostHandle, hubInstance->interruptPipe);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("hub close interrupt pipe error\r\n");
#endif
            }
            hubInstance->interruptPipe = NULL;
        }

        /* cancel control transfer if exist */
        if ((hubInstance->controlPipe != NULL) && (hubInstance->controlTransfer != NULL))
        {
            status =
                USB_HostCancelTransfer(hubInstance->hostHandle, hubInstance->controlPipe, hubInstance->controlTransfer);
        }

        /* notify host driver that the interface will not be used */
        USB_HostCloseDeviceInterface(deviceHandle, hubInstance->interfaceHandle);
#if ((defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE > 0U))
        SDK_Free(hubInstance->hubDescriptor);
        SDK_Free(hubInstance->portStatusBuffer);
        SDK_Free(hubInstance->hubStatusBuffer);
        SDK_Free(hubInstance->hubBitmapBuffer);
#endif
        USB_OsaMemoryFree(hubInstance);
    }
    else
    {
        /* notify host driver that the interface will not be used */
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostHubInterruptRecv(usb_host_class_handle classHandle,
                                      uint8_t *buffer,
                                      uint16_t bufferLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* get transfer */
    if (USB_HostMallocTransfer(hubInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }

    /* save hub application callback */
    hubInstance->inCallbackFn = callbackFn;
    hubInstance->inCallbackParam = callbackParam;

    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostHubInPipeCallback;
    transfer->callbackParam = hubInstance;

    /* call host driver API to receive data */
    if (USB_HostRecv(hubInstance->hostHandle, hubInstance->interruptPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostHubSendPortReset(usb_host_class_handle classHandle, uint8_t portNumber)
{
    usb_host_hub_instance_t *hubInstance = (usb_host_hub_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* get transfer */
    if (USB_HostMallocTransfer(hubInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }

    /* initialize transfer */
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->callbackFn = USB_HostHubResetCallback;
    transfer->callbackParam = hubInstance;
    transfer->setupPacket->bmRequestType =
        USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_FEATURE;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(PORT_RESET);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(portNumber);
    transfer->setupPacket->wLength = 0;

    /* send the transfer */
    if (USB_HostSendSetup(hubInstance->hostHandle, hubInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("Error in hid get report descriptor\r\n");
#endif
        USB_HostFreeTransfer(hubInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

/*!
 * @brief hub get descriptor.
 *
 * This function implements get hub descriptor specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubGetDescriptor(usb_host_class_handle classHandle,
                                      uint8_t *buffer,
                                      uint16_t bufferLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    return USB_HostHubClassRequestCommon(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_DEVICE,
        USB_REQUEST_STANDARD_GET_DESCRIPTOR, 0x00, 0, buffer, bufferLength, callbackFn, callbackParam);
}

/*!
 * @brief hub clear feature.
 *
 * This function implements clear hub feature specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubClearFeature(usb_host_class_handle classHandle,
                                     uint8_t feature,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    return USB_HostHubClassRequestCommon(classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS,
                                         USB_REQUEST_STANDARD_CLEAR_FEATURE, feature, 0, NULL, 0, callbackFn,
                                         callbackParam);
}

/*!
 * @brief hub get status.
 *
 * This function implements get hub status specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubGetStatus(usb_host_class_handle classHandle,
                                  uint8_t *buffer,
                                  uint16_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostHubClassRequestCommon(classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS,
                                         USB_REQUEST_STANDARD_GET_STATUS, 0, 0, buffer, bufferLength, callbackFn,
                                         callbackParam);
}

/*!
 * @brief hub set feature.
 *
 * This function implements set hub feature specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubSetPortFeature(usb_host_class_handle classHandle,
                                       uint8_t portNumber,
                                       uint8_t feature,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    return USB_HostHubClassRequestCommon(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
        USB_REQUEST_STANDARD_SET_FEATURE, feature, portNumber, NULL, 0, callbackFn, callbackParam);
}

/*!
 * @brief hub clear port feature.
 *
 * This function implements clear hub port feature specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubClearPortFeature(usb_host_class_handle classHandle,
                                         uint8_t portNumber,
                                         uint8_t feature,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    return USB_HostHubClassRequestCommon(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
        USB_REQUEST_STANDARD_CLEAR_FEATURE, feature, portNumber, NULL, 0, callbackFn, callbackParam);
}

/*!
 * @brief hub port get status.
 *
 * This function implements get hub port status specific request.
 *
 * @param classHandle   the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength     the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostHubGetPortStatus(usb_host_class_handle classHandle,
                                      uint8_t portNumber,
                                      uint8_t *buffer,
                                      uint16_t bufferLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    return USB_HostHubClassRequestCommon(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_OTHER,
        USB_REQUEST_STANDARD_GET_STATUS, 0, portNumber, buffer, bufferLength, callbackFn, callbackParam);
}

#endif /* USB_HOST_CONFIG_HUB */
