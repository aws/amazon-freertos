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
#if ((defined USB_HOST_CONFIG_CDC) && (USB_HOST_CONFIG_CDC))
#include "usb_host.h"
#include "usb_host_cdc.h"
#include "usb_host_devices.h"

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)

static void USB_HostCdcClearInHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->inCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostCdcDataRecv */
        cdcInstance->inCallbackFn(cdcInstance->inCallbackParam, cdcInstance->stallDataBuffer,
                                  cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

static void USB_HostCdcClearOutHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->outCallbackFn != NULL)
    {
        /* callback to application，the callback function is initialized in USB_HostCdcDataSend */
        cdcInstance->outCallbackFn(cdcInstance->outCallbackParam, cdcInstance->stallDataBuffer,
                                   cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}
static void USB_HostCdcClearInterruptHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (cdcInstance->interruptCallbackFn != NULL)
    {
        /* callback to application */
        cdcInstance->interruptCallbackFn(cdcInstance->interruptCallbackParam, cdcInstance->stallDataBuffer,
                                         cdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

static usb_status_t USB_HostCdcClearHalt(usb_host_cdc_instance_struct_t *cdcInstance,
                                         usb_host_transfer_t *stallTransfer,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    cdcInstance->stallDataBuffer = stallTransfer->transferBuffer;
    cdcInstance->stallDataLength = stallTransfer->transferSofar;
    /* save the application callback function */
    cdcInstance->controlCallbackFn = NULL;
    cdcInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = cdcInstance;
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
    }
    cdcInstance->controlTransfer = transfer;

    return status;
}
#endif

/*!
 * @brief cdc data in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostCdcDataInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(cdcInstance, transfer, USB_HostCdcClearInHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_IN |
                                  ((usb_host_pipe_t *)cdcInstance->inPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif

    if (cdcInstance->inCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostCdcDataRecv */
        cdcInstance->inCallbackFn(cdcInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                  status);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostCdcDataOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;
#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(cdcInstance, transfer, USB_HostCdcClearOutHaltCallback,
                                 (USB_REQUEST_TYPE_DIR_OUT |
                                  ((usb_host_pipe_t *)cdcInstance->outPipe)->endpointAddress)) == kStatus_USB_Success)
        {
            USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (cdcInstance->outCallbackFn != NULL)
    {
        /* callback to application，the callback function is initialized in USB_HostCdcDataSend */
        cdcInstance->outCallbackFn(cdcInstance->outCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status);
    }

    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostCdcInterruptPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostCdcClearHalt(
                cdcInstance, transfer, USB_HostCdcClearInterruptHaltCallback,
                (USB_REQUEST_TYPE_DIR_OUT | ((usb_host_pipe_t *)cdcInstance->interruptPipe)->endpointAddress)) ==
            kStatus_USB_Success)
        {
            USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif

    if (cdcInstance->interruptCallbackFn != NULL)
    {
        cdcInstance->interruptCallbackFn(cdcInstance->interruptCallbackParam, transfer->transferBuffer,
                                         transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc data out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
void USB_HostCdcControlPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc open data interface.
 *
 * @param cdcInstance     cdc instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostCdcOpenDataInterface(usb_host_cdc_instance_struct_t *cdcInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interfaceHandle;

    if (cdcInstance->inPipe != NULL)
    {
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->inPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->inPipe = NULL;
    }

    if (cdcInstance->outPipe != NULL)
    {
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->outPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->outPipe = NULL;
    }
    status = USB_HostOpenDeviceInterface(cdcInstance->deviceHandle, cdcInstance->dataInterfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    /* open interface pipes */
    interfaceHandle = (usb_host_interface_t *)cdcInstance->dataInterfaceHandle;

    for (ep_index = 0; ep_index < interfaceHandle->epCount; ++ep_index)
    {
        ep_desc = interfaceHandle->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = cdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = ep_desc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->bulkInPacketSize = pipeInit.maxPacketSize;
            status = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->inPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_audio_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = cdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_OUT;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = ep_desc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->bulkOutPacketSize = pipeInit.maxPacketSize;
            status = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->outPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_cdc_set_dat_interface fail to open pipe\r\n");
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

/*!
 * @brief cdc set data interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcSetDataInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostCdcOpenDataInterface(cdcInstance);
    }

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, NULL, 0, status);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief cdc open control interface.
 *
 * @param cdcInstance     cdc instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostCdcOpenControlInterface(usb_host_cdc_instance_struct_t *cdcInstance)
{
    usb_status_t status;
    uint8_t ep_index = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interfaceHandle;

    if (cdcInstance->interruptPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);
        status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->interruptPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        cdcInstance->interruptPipe = NULL;
    }

    status = USB_HostOpenDeviceInterface(cdcInstance->deviceHandle, cdcInstance->controlInterfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    /* open interface pipes */
    interfaceHandle = (usb_host_interface_t *)cdcInstance->controlInterfaceHandle;

    for (ep_index = 0; ep_index < interfaceHandle->epCount; ++ep_index)
    {
        ep_desc = interfaceHandle->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            pipeInit.devInstance = cdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = ep_desc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            cdcInstance->packetSize = pipeInit.maxPacketSize;

            status = USB_HostOpenPipe(cdcInstance->hostHandle, &cdcInstance->interruptPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostCdcSetControlInterface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
    }
    return kStatus_USB_Success;
}

/*!
 * @brief cdc set control interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostCdcSetContorlInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)param;

    cdcInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostCdcOpenControlInterface(cdcInstance);
    }

    if (cdcInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostCdcControl,
        USB_HostCdcSetControlInterface
        or USB_HostCdcSetDataInterface, but is the same function */
        cdcInstance->controlCallbackFn(cdcInstance->controlCallbackParam, NULL, 0, status);
    }
    USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
}

/*!
 * @brief initialize the cdc instance.
 *
 * This function allocate the resource for cdc instance.
 *
 * @param deviceHandle       the device handle.
 * @param classHandle return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostCdcInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    usb_host_cdc_instance_struct_t *control_ptr =
        (usb_host_cdc_instance_struct_t *)USB_OsaMemoryAllocate(sizeof(usb_host_cdc_instance_struct_t));
    uint32_t info_value;

    if (control_ptr == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    control_ptr->deviceHandle = deviceHandle;
    control_ptr->controlInterfaceHandle = NULL;
    control_ptr->dataInterfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &info_value);
    control_ptr->hostHandle = (usb_host_handle)info_value;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &info_value);
    control_ptr->controlPipe = (usb_host_pipe_handle)info_value;

    *classHandle = control_ptr;
    return kStatus_USB_Success;
}

/*!
 * @brief set control interface.
 *
 * This function bind the control interface with the cdc instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the control interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostCdcSetControlInterface(usb_host_class_handle classHandle,
                                            usb_host_interface_handle interfaceHandle,
                                            uint8_t alternateSetting,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    usb_status_t status;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    status = kStatus_USB_Success;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    cdcInstance->controlInterfaceHandle = interfaceHandle;

    /* cancel transfers */
    if (cdcInstance->interruptPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (alternateSetting == 0)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostCdcOpenControlInterface(cdcInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        cdcInstance->controlCallbackFn = callbackFn;
        cdcInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostCdcSetContorlInterfaceCallback;
        transfer->callbackParam = cdcInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)cdcInstance->controlInterfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            cdcInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief set data interface.
 *
 * This function bind the control interface with the cdc instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the data interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostCdcSetDataInterface(usb_host_class_handle classHandle,
                                         usb_host_interface_handle interfaceHandle,
                                         uint8_t alternateSetting,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    usb_status_t status;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    status = kStatus_USB_Success;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    cdcInstance->dataInterfaceHandle = interfaceHandle;

    /* cancel transfers */
    if (cdcInstance->inPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->inPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (cdcInstance->outPipe != NULL)
    {
        status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->outPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (alternateSetting == 0)
    {
        if (callbackFn != NULL)
        {
            status = USB_HostCdcOpenDataInterface(cdcInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        cdcInstance->controlCallbackFn = callbackFn;
        cdcInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostCdcSetDataInterfaceCallback;
        transfer->callbackParam = cdcInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)cdcInstance->dataInterfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            cdcInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief de-initialize the cdc instance.
 *
 * This function release the resource for cdc instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    the class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
usb_status_t USB_HostCdcDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL)
    {
        if (cdcInstance->interruptPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->interruptPipe, NULL);
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->interruptPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->interruptPipe = NULL;
        }

        USB_HostCloseDeviceInterface(deviceHandle, cdcInstance->controlInterfaceHandle);

        if (cdcInstance->inPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->inPipe, NULL);
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->inPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->inPipe = NULL;
        }
        if (cdcInstance->outPipe != NULL)
        {
            status = USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->outPipe, NULL);
            status = USB_HostClosePipe(cdcInstance->hostHandle, cdcInstance->outPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            cdcInstance->outPipe = NULL;
        }
        if ((cdcInstance->controlPipe != NULL) && (cdcInstance->controlTransfer != NULL))
        {
            status =
                USB_HostCancelTransfer(cdcInstance->hostHandle, cdcInstance->controlPipe, cdcInstance->controlTransfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle, cdcInstance->dataInterfaceHandle);

        USB_OsaMemoryFree(cdcInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

/*!
 * @brief receive data.
 *
 * This function implements cdc receiving data.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostCdcDataRecv(usb_host_class_handle classHandle,
                                 uint8_t *buffer,
                                 uint32_t bufferLength,
                                 transfer_callback_t callbackFn,
                                 void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->inPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->inCallbackFn = callbackFn;
    cdcInstance->inCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostCdcDataInPipeCallback;
    transfer->callbackParam = cdcInstance;

    if (USB_HostRecv(cdcInstance->hostHandle, cdcInstance->inPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief send data.
 *
 * This function implements cdc sending data.
 *
 * @param classHandle    the class handle.
 * @param buffer         the buffer pointer.
 * @param bufferLength   the buffer length.
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          receive request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy             There is no idle transfer.
 * @retval kStatus_USB_Error            pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostCdcDataSend(usb_host_class_handle classHandle,
                                 uint8_t *buffer,
                                 uint32_t bufferLength,
                                 transfer_callback_t callbackFn,
                                 void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->outPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->outCallbackFn = callbackFn;
    cdcInstance->outCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostCdcDataOutPipeCallback;
    transfer->callbackParam = cdcInstance;

    if (USB_HostSend(cdcInstance->hostHandle, cdcInstance->outPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostSend\r\n");
#endif
        USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

/*!
 * @brief interrupt receive data.
 *
 * This function implements interrupt receiving data.
 *
 * @param classHandle     the class handle.
 * @param buffer          the buffer pointer.
 * @param bufferLength    the buffer length.
 * @param callbackFn      this callback is called after this function completes.
 * @param callbackParam   the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success         receive request successfully.
 * @retval kStatus_USB_InvalidHandle   The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy            There is no idle transfer.
 * @retval kStatus_USB_Error           pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostCdcInterruptRecv(usb_host_class_handle classHandle,
                                      uint8_t *buffer,
                                      uint32_t bufferLength,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (cdcInstance->interruptPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->interruptCallbackFn = callbackFn;
    cdcInstance->interruptCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostCdcInterruptPipeCallback;
    transfer->callbackParam = cdcInstance;

    if (USB_HostRecv(cdcInstance->hostHandle, cdcInstance->interruptPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to usb_interrupt_recv\r\n");
#endif
        USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}
/*!
 * @brief get pipe max packet size.
 *
 * @param[in] classHandle the class handle.
 * @param[in] pipeType    It's value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                        Please reference to usb_spec.h
 * @param[in] direction   pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval max packet size.
 */
uint16_t USB_HostCdcGetPacketsize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    if (classHandle == NULL)
    {
        return 0;
    }

    if (pipeType == USB_ENDPOINT_BULK)
    {
        if (direction == USB_IN)
        {
            return cdcInstance->bulkInPacketSize;
        }
        else
        {
            return cdcInstance->bulkOutPacketSize;
        }
    }

    return 0;
}

/*!
 * @brief cdc send control transfer common code.
 *
 * @param classHandle    the class handle.
 * @param request_type   setup packet request type.
 * @param request        setup packet request value.
 * @param wvalue_l       setup packet wvalue low byte.
 * @param wvalue_h       setup packet wvalue high byte.
 * @param wlength        setup packet wlength value.
 * @param data           data buffer pointer
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
usb_status_t USB_HostCdcControl(usb_host_class_handle classHandle,
                                uint8_t request_type,
                                uint8_t request,
                                uint8_t wvalue_l,
                                uint8_t wvalue_h,
                                uint16_t wlength,
                                uint8_t *data,
                                transfer_callback_t callbackFn,
                                void *callbackParam)
{
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (USB_HostMallocTransfer(cdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    cdcInstance->controlCallbackFn = callbackFn;
    cdcInstance->controlCallbackParam = callbackParam;

    transfer->transferBuffer = data;
    transfer->transferLength = wlength;
    transfer->callbackFn = USB_HostCdcControlPipeCallback;
    transfer->callbackParam = cdcInstance;
    transfer->setupPacket->bmRequestType = request_type;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalue_l | (uint16_t)((uint16_t)wvalue_h << 8));
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)cdcInstance->controlInterfaceHandle)->interfaceDesc->bInterfaceNumber);
    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);

    if (USB_HostSendSetup(cdcInstance->hostHandle, cdcInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(cdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    cdcInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

/*!
 * @brief cdc get line coding.
 *
 * This function implements cdc GetLineCoding request.refer to pstn spec.
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
usb_status_t USB_HostCdcGetAcmLineCoding(usb_host_class_handle classHandle,
                                         usb_host_cdc_line_coding_struct_t *uartLineCoding,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_GET_LINE_CODING, 0, 0, 7, (uint8_t *)uartLineCoding, callbackFn, callbackParam);
}

/*!
 * @brief cdc setControlLineState.
 *
 * This function implements cdc etControlLineState request.refer to pstn spec.
 *
 * @param classHandle   the class handle.
 * @param buffer        the buffer pointer.
 * @param bufferLength  the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 */
usb_status_t USB_HostCdcSetAcmCtrlState(
    usb_host_class_handle classHandle, uint8_t dtr, uint8_t rts, transfer_callback_t callbackFn, void *callbackParam)
{
    uint16_t lineState = 0;

    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;

    lineState = dtr ? USB_HOST_CDC_CONTROL_LINE_STATE_DTR : 0;
    lineState |= rts ? USB_HOST_CDC_CONTROL_LINE_STATE_RTS : 0;
    return USB_HostCdcControl(
        cdcInstance, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_SET_CONTROL_LINE_STATE, USB_SHORT_GET_LOW(lineState), USB_SHORT_GET_HIGH(lineState), 0, NULL,
        callbackFn, callbackParam);
}

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
                                                void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_SEND_ENCAPSULATED_COMMAND, 0, 0, bufferLength, buffer, callbackFn, callbackParam);
}

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
                                                void *callbackParam)
{
    return USB_HostCdcControl(
        classHandle, USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE,
        USB_HOST_CDC_GET_ENCAPSULATED_RESPONSE, 0, 0, bufferLength, buffer, callbackFn, callbackParam);
}

/*!
 * @brief cdc get acm descriptor.
 *
 * This function is hunting for class specific acm decriptor in the configuration ,get the corresponding
 * descriptor .
 *
 * @param classHandle          the class handle.
 * @param headDesc             the head function descriptor pointer.
 * @param callManageDesc       the call management functional descriptor pointer.
 * @param abstractControlDesc  the abstract control management functional pointer.
 * @param unionInterfaceDesc   the union functional descriptor pointer.
 *
 * @retval kStatus_USB_Error          analyse descriptor error.
 */
usb_status_t USB_HostCdcGetAcmDescriptor(usb_host_class_handle classHandle,
                                         usb_host_cdc_head_function_desc_struct_t **headDesc,
                                         usb_host_cdc_call_manage_desc_struct_t **callManageDesc,
                                         usb_host_cdc_abstract_control_desc_struct_t **abstractControlDesc,
                                         usb_host_cdc_union_interface_desc_struct_t **unionInterfaceDesc)
{
    usb_status_t status;
    usb_descriptor_union_t *ptr1;
    uint32_t end_address;
    usb_host_cdc_instance_struct_t *cdcInstance = (usb_host_cdc_instance_struct_t *)classHandle;
    usb_cdc_func_desc_struct_t *cdc_common_ptr;
    usb_host_interface_t *interface_handle;

    status = kStatus_USB_Success;
    interface_handle = (usb_host_interface_t *)cdcInstance->controlInterfaceHandle;
    ptr1 = (usb_descriptor_union_t *)interface_handle->interfaceExtension;
    end_address = (uint32_t)(interface_handle->interfaceExtension + interface_handle->interfaceExtensionLength);

    while ((uint32_t)ptr1 < end_address)
    {
        cdc_common_ptr = (usb_cdc_func_desc_struct_t *)&ptr1->common;
        switch (cdc_common_ptr->common.bDescriptorSubtype)
        {
            case USB_HOST_DESC_SUBTYPE_HEADER:
                *headDesc = &cdc_common_ptr->head;
                if ((((uint32_t)((*headDesc)->bcdCDC[1]) << 8) + (*headDesc)->bcdCDC[0]) > 0x0110)
                {
                    status = kStatus_USB_Error;
                }
                break;
            case USB_HOST_DESC_SUBTYPE_UNION:
                if (cdc_common_ptr->unionDesc.bControlInterface == interface_handle->interfaceDesc->bInterfaceNumber)
                {
                    *unionInterfaceDesc = &cdc_common_ptr->unionDesc;
                }
                else
                {
                    status = kStatus_USB_Error;
                }
                break;
            case USB_HOST_DESC_SUBTYPE_CM:
                *callManageDesc = &cdc_common_ptr->callManage;
                break;
            case USB_HOST_DESC_SUBTYPE_ACM:
                *abstractControlDesc = &cdc_common_ptr->acm;
                break;
            default:
                break;
        }

        if (kStatus_USB_Success != status)
        {
            break;
        }
        ptr1 = (usb_descriptor_union_t *)((uint8_t *)ptr1 + ptr1->common.bLength);
    }
    cdcInstance->headDesc = *headDesc;
    cdcInstance->callManageDesc = *callManageDesc;
    cdcInstance->abstractControlDesc = *abstractControlDesc;
    cdcInstance->unionInterfaceDesc = *unionInterfaceDesc;
    return status;
}

#endif
