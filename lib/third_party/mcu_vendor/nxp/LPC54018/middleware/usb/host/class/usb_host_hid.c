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
#if ((defined USB_HOST_CONFIG_HID) && (USB_HOST_CONFIG_HID))
#include "usb_host.h"
#include "usb_host_hid.h"

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief hid in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHidInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hid out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHidOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hid control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHidControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hid open interface. It is called when set interface request success or open alternate setting 0 interface.
 *
 * @param hidInstance     hid instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostHidOpenInterface(usb_host_hid_instance_t *hidInstance);

/*!
 * @brief hid set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostHidSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief hid send control transfer common code.
 *
 * @param classHandle    the class handle.
 * @param requestType    setup packet request type.
 * @param request        setup packet request value.
 * @param wvalueL        setup packet wvalue low byte.
 * @param wvalueH        setup packet wvalue high byte.
 * @param wlength        setup packet wlength value.
 * @param data           data buffer pointer
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostHidControl(usb_host_class_handle classHandle,
                                       uint8_t requestType,
                                       uint8_t request,
                                       uint8_t wvalueL,
                                       uint8_t wvalueH,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam);

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)

static void USB_HostHidClearInHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

    hidInstance->controlTransfer = NULL;
    if (hidInstance->inCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostHidRecv */
        hidInstance->inCallbackFn(hidInstance->inCallbackParam, hidInstance->stallDataBuffer,
                                  hidInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

static void USB_HostHidClearOutHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

    hidInstance->controlTransfer = NULL;
    if (hidInstance->outCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in USB_HostHidSend */
        hidInstance->outCallbackFn(hidInstance->outCallbackParam, hidInstance->stallDataBuffer,
                                   hidInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

static usb_status_t USB_HostHidClearHalt(usb_host_hid_instance_t *hidInstance,
                                         usb_host_transfer_t *stallTransfer,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(hidInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    hidInstance->stallDataBuffer = stallTransfer->transferBuffer;
    hidInstance->stallDataLength = stallTransfer->transferSofar;
    /* save the application callback function */
    hidInstance->controlCallbackFn = NULL;
    hidInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = hidInstance;
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status = USB_HostSendSetup(hidInstance->hostHandle, hidInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
    }
    hidInstance->controlTransfer = transfer;

    return status;
}
#endif

static void USB_HostHidInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostHidClearHalt(hidInstance, transfer, USB_HostHidClearInHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_IN |
                                  ((usb_host_pipe_t *)hidInstance->inPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (hidInstance->inCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostHidRecv */
        hidInstance->inCallbackFn(hidInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                  status);
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

static void USB_HostHidOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostHidClearHalt(hidInstance, transfer, USB_HostHidClearOutHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_OUT |
                                  ((usb_host_pipe_t *)hidInstance->outPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (hidInstance->outCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in USB_HostHidSend */
        hidInstance->outCallbackFn(hidInstance->outCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status); /* callback to application */
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

static void USB_HostHidControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

    hidInstance->controlTransfer = NULL;
    if (hidInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostHidSetInterface
        or USB_HostHidControl, but is the same function */
        hidInstance->controlCallbackFn(hidInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

static usb_status_t USB_HostHidOpenInterface(usb_host_hid_instance_t *hidInstance)
{
    usb_status_t status;
    uint8_t epIndex = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *epDesc = NULL;
    usb_host_interface_t *interfacePointer;

    if (hidInstance->inPipe != NULL) /* close interrupt in pipe if it is open */
    {
        status = USB_HostClosePipe(hidInstance->hostHandle, hidInstance->inPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        hidInstance->inPipe = NULL;
    }
    if (hidInstance->outPipe != NULL) /* close interrupt out pipe if it is open */
    {
        status = USB_HostClosePipe(hidInstance->hostHandle, hidInstance->outPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        hidInstance->outPipe = NULL;
    }

    /* open interface pipes */
    interfacePointer = (usb_host_interface_t *)hidInstance->interfaceHandle;
    for (epIndex = 0; epIndex < interfacePointer->epCount; ++epIndex)
    {
        epDesc = interfacePointer->epList[epIndex].epDesc;
        if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            pipeInit.devInstance = hidInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            hidInstance->inPacketSize = pipeInit.maxPacketSize;

            status = USB_HostOpenPipe(hidInstance->hostHandle, &hidInstance->inPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostHidSetInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            pipeInit.devInstance = hidInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction = USB_OUT;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            hidInstance->outPacketSize = pipeInit.maxPacketSize;

            status = USB_HostOpenPipe(hidInstance->hostHandle, &hidInstance->outPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostHidSetInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else
        {
        }
    }

    return kStatus_USB_Success;
}

static void USB_HostHidSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)param;

    hidInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostHidOpenInterface(hidInstance); /* hid open interface */
    }

    if (hidInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostHidSetInterface
        or USB_HostHidControl, but is the same function */
        hidInstance->controlCallbackFn(hidInstance->controlCallbackParam, NULL, 0, status);
    }
    USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
}

usb_status_t USB_HostHidInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    uint32_t infoValue;
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)USB_OsaMemoryAllocate(
        sizeof(usb_host_hid_instance_t)); /* malloc hid class instance */

    if (hidInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    /* initialize hid instance */
    hidInstance->deviceHandle = deviceHandle;
    hidInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    hidInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    hidInstance->controlPipe = (usb_host_pipe_handle)infoValue;

    *classHandle = hidInstance;
    return kStatus_USB_Success;
}

usb_status_t USB_HostHidSetInterface(usb_host_class_handle classHandle,
                                     usb_host_interface_handle interfaceHandle,
                                     uint8_t alternateSetting,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    usb_status_t status;
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    hidInstance->interfaceHandle = interfaceHandle;
    status = USB_HostOpenDeviceInterface(hidInstance->deviceHandle,
                                         interfaceHandle); /* notify host driver the interface is open */
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    /* cancel transfers */
    if (hidInstance->inPipe != NULL)
    {
        status = USB_HostCancelTransfer(hidInstance->hostHandle, hidInstance->inPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }
    if (hidInstance->outPipe != NULL)
    {
        status = USB_HostCancelTransfer(hidInstance->hostHandle, hidInstance->outPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (alternateSetting == 0) /* open interface directly */
    {
        if (callbackFn != NULL)
        {
            status = USB_HostHidOpenInterface(hidInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else /* send setup transfer */
    {
        /* malloc one transfer */
        if (USB_HostMallocTransfer(hidInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }

        /* save the application callback function */
        hidInstance->controlCallbackFn = callbackFn;
        hidInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostHidSetInterfaceCallback;
        transfer->callbackParam = hidInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)hidInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(hidInstance->hostHandle, hidInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            hidInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
        }
    }

    return status;
}

usb_status_t USB_HostHidDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL) /* class instance has initialized */
    {
        if (hidInstance->inPipe != NULL)
        {
            status = USB_HostCancelTransfer(hidInstance->hostHandle, hidInstance->inPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(hidInstance->hostHandle, hidInstance->inPipe);            /* close pipe */

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            hidInstance->inPipe = NULL;
        }
        if (hidInstance->outPipe != NULL)
        {
            status = USB_HostCancelTransfer(hidInstance->hostHandle, hidInstance->outPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(hidInstance->hostHandle, hidInstance->outPipe);            /* close pipe */

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            hidInstance->outPipe = NULL;
        }
        if ((hidInstance->controlPipe != NULL) &&
            (hidInstance->controlTransfer != NULL)) /* cancel control transfer if there is on-going control transfer */
        {
            status =
                USB_HostCancelTransfer(hidInstance->hostHandle, hidInstance->controlPipe, hidInstance->controlTransfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle,
                                     hidInstance->interfaceHandle); /* notify host driver the interface is closed */
        USB_OsaMemoryFree(hidInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

uint16_t USB_HostHidGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;
    if (classHandle == NULL)
    {
        return 0;
    }

    if (pipeType == USB_ENDPOINT_INTERRUPT)
    {
        if (direction == USB_IN)
        {
            return hidInstance->inPacketSize;
        }
        else
        {
            return hidInstance->outPacketSize;
        }
    }

    return 0;
}

usb_status_t USB_HostHidRecv(usb_host_class_handle classHandle,
                             uint8_t *buffer,
                             uint32_t bufferLength,
                             transfer_callback_t callbackFn,
                             void *callbackParam)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (hidInstance->inPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(hidInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    hidInstance->inCallbackFn = callbackFn;
    hidInstance->inCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostHidInPipeCallback;
    transfer->callbackParam = hidInstance;

    if (USB_HostRecv(hidInstance->hostHandle, hidInstance->inPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

usb_status_t USB_HostHidSend(usb_host_class_handle classHandle,
                             uint8_t *buffer,
                             uint32_t bufferLength,
                             transfer_callback_t callbackFn,
                             void *callbackParam)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (hidInstance->outPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(hidInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* save the application callback function */
    hidInstance->outCallbackFn = callbackFn;
    hidInstance->outCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostHidOutPipeCallback;
    transfer->callbackParam = hidInstance;

    if (USB_HostSend(hidInstance->hostHandle, hidInstance->outPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostSend\r\n");
#endif
        USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

static usb_status_t USB_HostHidControl(usb_host_class_handle classHandle,
                                       uint8_t requestType,
                                       uint8_t request,
                                       uint8_t wvalueL,
                                       uint8_t wvalueH,
                                       uint16_t wlength,
                                       uint8_t *data,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam)
{
    usb_host_hid_instance_t *hidInstance = (usb_host_hid_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(hidInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    hidInstance->controlCallbackFn = callbackFn;
    hidInstance->controlCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = data;
    transfer->transferLength = wlength;
    transfer->callbackFn = USB_HostHidControlCallback;
    transfer->callbackParam = hidInstance;
    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalueL | (uint16_t)((uint16_t)wvalueH << 8));
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)hidInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    if (USB_HostSendSetup(hidInstance->hostHandle, hidInstance->controlPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
#ifdef HOST_ECHO
        usb_echo("failt for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(hidInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    hidInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

usb_status_t USB_HostHidGetReportDescriptor(usb_host_class_handle classHandle,
                                            uint8_t *buffer,
                                            uint16_t buffer_len,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_STANDARD | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_REQUEST_STANDARD_GET_DESCRIPTOR, 0, USB_DESCRIPTOR_TYPE_HID_REPORT, buffer_len, buffer, callbackFn,
        callbackParam);
}

usb_status_t USB_HostHidGetIdle(usb_host_class_handle classHandle,
                                uint8_t reportId,
                                uint8_t *idleRate,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_GET_IDLE, reportId, 0, 1, idleRate, callbackFn, callbackParam);
}

usb_status_t USB_HostHidSetIdle(usb_host_class_handle classHandle,
                                uint8_t reportId,
                                uint8_t idleRate,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_SET_IDLE, reportId, idleRate, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostHidGetProtocol(usb_host_class_handle classHandle,
                                    uint8_t *protocol,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_GET_PROTOCOL, 0, 0, 1, protocol, callbackFn, callbackParam);
}

usb_status_t USB_HostHidSetProtocol(usb_host_class_handle classHandle,
                                    uint8_t protocol,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_SET_PROTOCOL, protocol, 0, 0, NULL, callbackFn, callbackParam);
}

usb_status_t USB_HostHidGetReport(usb_host_class_handle classHandle,
                                  uint8_t reportId,
                                  uint8_t reportType,
                                  uint8_t *buffer,
                                  uint32_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_GET_REPORT, reportId, reportType, bufferLength, buffer, callbackFn, callbackParam);
}

usb_status_t USB_HostHidSetReport(usb_host_class_handle classHandle,
                                  uint8_t reportId,
                                  uint8_t reportType,
                                  uint8_t *buffer,
                                  uint32_t bufferLength,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    return USB_HostHidControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_HID_SET_REPORT, reportId, reportType, bufferLength, buffer, callbackFn, callbackParam);
}

#endif /* USB_HOST_CONFIG_HID */
