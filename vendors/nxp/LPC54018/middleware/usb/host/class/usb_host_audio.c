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
#if ((defined USB_HOST_CONFIG_AUDIO) && (USB_HOST_CONFIG_AUDIO))
#include "usb_host.h"
#include "usb_host_audio.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* usb audio feature uint command request declaration */
static usb_audio_request_t s_usbAudioFuRequests[NUMBER_OF_FEATURE_COMMANDS] = {
    /* Mute command request  */
    {AUDIO_FU_MUTE_MASK, ITF_REQUEST, CUR_REQUEST, AUDIO_FU_MUTE, 1U},
    /* Volume command request  */
    {AUDIO_FU_VOLUME_MASK, ITF_REQUEST, CUR_REQUEST, AUDIO_FU_VOLUME, 2U},
    {AUDIO_FU_VOLUME_MASK, ITF_REQUEST, MIN_REQUEST, AUDIO_FU_VOLUME, 2U},
    {AUDIO_FU_VOLUME_MASK, ITF_REQUEST, MAX_REQUEST, AUDIO_FU_VOLUME, 2U},
    {AUDIO_FU_VOLUME_MASK, ITF_REQUEST, RES_REQUEST, AUDIO_FU_VOLUME, 2U},
};
/* USB audio endpoint command declaration */
static usb_audio_request_t s_usbAudioEpRequests[NUMBER_OF_ENDPOINT_COMMANDS] = {
    /* USB audio Pitch command request  */
    {AUDIO_PITCH_MASK, EP_REQUEST, CUR_REQUEST, AUDIO_PITCH_CONTROL, 1U},

    /* USB audio Sampling frequency command request  */
    {AUDIO_SAMPLING_FREQ_MASK, EP_REQUEST, CUR_REQUEST, AUDIO_SAMPLING_FREQ_CONTROL, 3U},
    {AUDIO_SAMPLING_FREQ_MASK, EP_REQUEST, MIN_REQUEST, AUDIO_SAMPLING_FREQ_CONTROL, 3U},
    {AUDIO_SAMPLING_FREQ_MASK, EP_REQUEST, MAX_REQUEST, AUDIO_SAMPLING_FREQ_CONTROL, 3U},
    {AUDIO_SAMPLING_FREQ_MASK, EP_REQUEST, RES_REQUEST, AUDIO_SAMPLING_FREQ_CONTROL, 3U},
};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief initialize the audio instance.
 *
 * This function allocate the resource for audio instance.
 *
 * @param deviceHandle       The device handle.
 * @param classHandlePtr return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostAudioInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandlePtr)
{
    audio_instance_t *audioPtr = (audio_instance_t *)USB_OsaMemoryAllocate(sizeof(audio_instance_t));
    uint32_t info_value;

    if (audioPtr == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    audioPtr->deviceHandle = deviceHandle;
    audioPtr->controlIntfHandle = NULL;
    audioPtr->streamIntfHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &info_value);
    audioPtr->hostHandle = (usb_host_handle)info_value;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &info_value);
    audioPtr->controlPipe = (usb_host_pipe_handle)info_value;

    *classHandlePtr = audioPtr;
    return kStatus_USB_Success;
}

/*!
 * @brief de-initialize the audio instance.
 *
 * This function release the resource for audio instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle the class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
usb_status_t USB_HostAudioDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;

    if (deviceHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (classHandle != NULL)
    {
        if (audioPtr->isoInPipe != NULL)
        {
            status = USB_HostCancelTransfer(audioPtr->hostHandle, audioPtr->isoInPipe, NULL);
            status = USB_HostClosePipe(audioPtr->hostHandle, audioPtr->isoInPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            audioPtr->isoInPipe = NULL;
        }
        if (audioPtr->isoOutPipe != NULL)
        {
            status = USB_HostCancelTransfer(audioPtr->hostHandle, audioPtr->isoOutPipe, NULL);
            status = USB_HostClosePipe(audioPtr->hostHandle, audioPtr->isoOutPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
            audioPtr->isoOutPipe = NULL;
        }
        USB_HostCloseDeviceInterface(deviceHandle, audioPtr->streamIntfHandle);

        if ((audioPtr->controlPipe != NULL) && (audioPtr->controlTransfer != NULL))
        {
            status = USB_HostCancelTransfer(audioPtr->hostHandle, audioPtr->controlPipe, audioPtr->controlTransfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle, audioPtr->controlIntfHandle);
        USB_OsaMemoryFree(audioPtr);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

/*!
 * @brief audiostream iso in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer     callback transfer.
 * @param status        transfer status.
 */
static void _USB_HostAudioStreamIsoInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    audio_instance_t *audioPtr = (audio_instance_t *)param;

    if (audioPtr->inCallbackFn != NULL)
    {
        /* callback function is initialized in USB_HostAudioStreamRecv */
        audioPtr->inCallbackFn(audioPtr->inCallbackParam, transfer->transferBuffer, transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
}

/*!
 * @brief audiostream iso out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer     callback transfer.
 * @param status        transfer status.
 */
static void _USB_HostAudioStreamIsoOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    audio_instance_t *audioPtr = (audio_instance_t *)param;

    if (audioPtr->outCallbackFn != NULL)
    {
        /* callback function is initialized in USB_HostAudioStreamSend */
        audioPtr->outCallbackFn(audioPtr->outCallbackParam, transfer->transferBuffer, transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
}

/*!
 * @brief audiocontrol pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer     callback transfer.
 * @param status        transfer status.
 */
static void _USB_HostAudioControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    audio_instance_t *audioPtr = (audio_instance_t *)param;

    audioPtr->controlTransfer = NULL;
    if (audioPtr->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the _USB_HostAudioControl,
        USB_HostAudioStreamSetInterface
        or USB_HostAudioControlSetInterface, but is the same function */
        audioPtr->controlCallbackFn(audioPtr->controlCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                    status);
    }
    USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
    audioPtr->isSetup = 0U;
}

/*!
 * @brief hid send control transfer common code.
 *
 * @param classHandle   the class handle.
 * @param typeRequest   setup packet request type.
 * @param request           setup packet request value.
 * @param wvalue            setup packet wvalue value.
 * @param windex            setup packet index value.
 * @param wlength           setup packet wlength value.
 * @param data                data buffer pointer will be transfer.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t _USB_HostAudioControl(usb_host_class_handle classHandle,
                                          uint8_t typeRequest,
                                          uint8_t request,
                                          uint16_t wvalue,
                                          uint16_t windex,
                                          uint16_t wlength,
                                          uint8_t *data,
                                          transfer_callback_t callbackFn,
                                          void *callbackParam)
{
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (USB_HostMallocTransfer(audioPtr->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    audioPtr->controlCallbackFn = callbackFn;
    audioPtr->controlCallbackParam = callbackParam;

    transfer->transferBuffer = data;
    transfer->transferLength = wlength;
    transfer->callbackFn = _USB_HostAudioControlCallback;
    transfer->callbackParam = audioPtr;
    transfer->setupPacket->bmRequestType = typeRequest;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(wvalue);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(windex);

    transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(wlength);
    audioPtr->isSetup = 1;

    if (USB_HostSendSetup(audioPtr->hostHandle, audioPtr->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    audioPtr->controlTransfer = transfer;

    return kStatus_USB_Success;
}

/*!
 * @brief audio open interface.
 *
 * @param audioPtr     audio instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t _USB_HostAudioOpenInterface(audio_instance_t *audioPtr)
{
    usb_status_t status;
    uint8_t ep_index = 0U;
    usb_host_pipe_init_t pipe_init;
    usb_descriptor_endpoint_t *ep_desc = NULL;
    usb_host_interface_t *interface_ptr;

    if (audioPtr->isoInPipe != NULL)
    {
        status = USB_HostClosePipe(audioPtr->hostHandle, audioPtr->isoInPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        audioPtr->isoInPipe = NULL;
    }
    if (audioPtr->isoOutPipe != NULL)
    {
        status = USB_HostClosePipe(audioPtr->hostHandle, audioPtr->isoOutPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        audioPtr->isoOutPipe = NULL;
    }

    /* open interface pipes */
    interface_ptr = (usb_host_interface_t *)audioPtr->streamIntfHandle;
    for (ep_index = 0U; ep_index < interface_ptr->epCount; ++ep_index)
    {
        ep_desc = interface_ptr->epList[ep_index].epDesc;
        if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_ISOCHRONOUS))
        {
            pipe_init.devInstance = audioPtr->deviceHandle;
            pipe_init.pipeType = USB_ENDPOINT_ISOCHRONOUS;
            pipe_init.direction = USB_IN;
            pipe_init.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipe_init.interval = ep_desc->bInterval;
            pipe_init.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipe_init.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                         USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipe_init.nakCount = USB_HOST_CONFIG_MAX_NAK;

            audioPtr->inPacketSize = pipe_init.maxPacketSize;
            audioPtr->isoEpNum = pipe_init.endpointAddress;
            status = USB_HostOpenPipe(audioPtr->hostHandle, &audioPtr->isoInPipe, &pipe_init);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_audio_stream_set_interface fail to open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
        }
        else if (((ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((ep_desc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_ISOCHRONOUS))
        {
            pipe_init.devInstance = audioPtr->deviceHandle;
            pipe_init.pipeType = USB_ENDPOINT_ISOCHRONOUS;
            pipe_init.direction = USB_OUT;
            pipe_init.endpointAddress = (ep_desc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipe_init.interval = ep_desc->bInterval;
            pipe_init.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                                 USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipe_init.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(ep_desc->wMaxPacketSize) &
                                         USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipe_init.nakCount = USB_HOST_CONFIG_MAX_NAK;

            audioPtr->outPacketSize = pipe_init.maxPacketSize;
            audioPtr->isoEpNum = pipe_init.endpointAddress;
            status = USB_HostOpenPipe(audioPtr->hostHandle, &audioPtr->isoOutPipe, &pipe_init);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_audio_stream_set_interface fail to open pipe\r\n");
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
 * @brief audio set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void _USB_HostAudioSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    audio_instance_t *audioPtr = (audio_instance_t *)param;

    audioPtr->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = _USB_HostAudioOpenInterface(audioPtr);
    }

    if (audioPtr->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the _USB_HostAudioControl,
        USB_HostAudioStreamSetInterface
        or USB_HostAudioControlSetInterface, but is the same function */
        audioPtr->controlCallbackFn(audioPtr->controlCallbackParam, NULL, 0U, status);
    }
    USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
}

/*!
 * @brief set audioclass stream interface.
 *
 * This function bind the interface with the audio instance.
 *
 * @param classHandle        The class handle.
 * @param interfaceHandle          The interface handle.
 * @param alternateSetting  The alternate setting value.
 * @param callbackFn          This callback is called after this function completes.
 * @param callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success         The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostAudioStreamSetInterface(usb_host_class_handle classHandle,
                                             usb_host_interface_handle interfaceHandle,
                                             uint8_t alternateSetting,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam)
{
    usb_status_t status;
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    usb_host_interface_t *interface_ptr;
    usb_host_transfer_t *transfer;
    audio_descriptor_union_t ptr1;
    uint32_t length = 0U, ep = 0U;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }

    audioPtr->streamIntfHandle = interfaceHandle;

    status = USB_HostOpenDeviceInterface(audioPtr->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    if (audioPtr->isoInPipe != NULL)
    {
        status = USB_HostCancelTransfer(audioPtr->hostHandle, audioPtr->isoInPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }

    if (audioPtr->isoOutPipe != NULL)
    {
        status = USB_HostCancelTransfer(audioPtr->hostHandle, audioPtr->isoOutPipe, NULL);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }
    /* open interface pipes */
    interface_ptr = (usb_host_interface_t *)interfaceHandle;
    ptr1.bufr = interface_ptr->interfaceExtension;

    length = 0U;
    while (length < interface_ptr->interfaceExtensionLength)
    {
        if ((ptr1.common->bDescriptorType == 0x04U) && (ptr1.interface->bAlternateSetting == alternateSetting))
        {
            interface_ptr->epCount = ptr1.interface->bNumEndpoints;
            break;
        }
        ptr1.bufr += ptr1.common->bLength;
        length += ptr1.common->bLength;
    }
    while (ep < interface_ptr->epCount)
    {
        if (ptr1.common->bDescriptorType == 0x24U)
        {
            if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AS_CS_GENERAL)
            {
                audioPtr->asIntfDesc = (usb_audio_stream_spepific_as_intf_desc_t *)ptr1.bufr;
            }
            else if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE)
            {
                audioPtr->formatTypeDesc = (usb_audio_stream_format_type_desc_t *)ptr1.bufr;
            }
            else
            {
            }
        }
        if (ptr1.common->bDescriptorType == 0x05U)
        {
            interface_ptr->epList[ep].epDesc = (usb_descriptor_endpoint_t *)ptr1.bufr;
            audioPtr->isoEndpDesc = (usb_audio_stream_specific_iso_endp_desc_t *)ptr1.bufr;
            ep++;
            ptr1.bufr += ptr1.common->bLength;
            interface_ptr->epList[ep].epExtension = ptr1.bufr;
            interface_ptr->epList[ep].epExtensionLength = ptr1.common->bLength;
            break;
        }
        ptr1.bufr += ptr1.common->bLength;
    }

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            status = _USB_HostAudioOpenInterface(audioPtr);
            callbackFn(callbackParam, NULL, 0U, kStatus_USB_Success);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(audioPtr->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        audioPtr->controlCallbackFn = callbackFn;
        audioPtr->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = _USB_HostAudioSetInterfaceCallback;
        transfer->callbackParam = audioPtr;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)audioPtr->streamIntfHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(audioPtr->hostHandle, audioPtr->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            audioPtr->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief set audioclass control interface.
 *
 * This function bind the interface with the audio instance.
 *
 * @param classHandle        The class handle.
 * @param interfaceHandle          The interface handle.
 * @param alternateSetting  The alternate setting value.
 * @param callbackFn          This callback is called after this function completes.
 * @param callbackParam    The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success         The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostAudioControlSetInterface(usb_host_class_handle classHandle,
                                              usb_host_interface_handle interfaceHandle,
                                              uint8_t alternateSetting,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam)
{
    usb_status_t status;
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    usb_host_interface_t *interface_ptr;
    usb_host_transfer_t *transfer;
    audio_descriptor_union_t ptr1;
    uint32_t length = 0U;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidParameter;
    }
    audioPtr->controlIntfHandle = interfaceHandle;
    interface_ptr = (usb_host_interface_t *)interfaceHandle;

    status = USB_HostOpenDeviceInterface(audioPtr->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    ptr1.bufr = interface_ptr->interfaceExtension;

    length = 0U;

    while (length < interface_ptr->interfaceExtensionLength)
    {
        if (((interface_ptr->interfaceDesc->bDescriptorType == 0x04U) &&
             (interface_ptr->interfaceDesc->bAlternateSetting == alternateSetting)) ||
            ((ptr1.common->bDescriptorType == 0x04U) && (ptr1.interface->bAlternateSetting == alternateSetting)))
        {
            break;
        }

        ptr1.bufr += ptr1.common->bLength;
        length += ptr1.common->bLength;
    }
    while (length < interface_ptr->interfaceExtensionLength)
    {
        if (ptr1.common->bDescriptorType == 0x24U)
        {
            if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AUDIO_CS_HEADER)
            {
                audioPtr->headerDesc = (usb_audio_ctrl_header_desc_t *)ptr1.bufr;
            }
            else if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AUDIO_CS_IT)
            {
                audioPtr->itDesc = (usb_audio_ctrl_it_desc_t *)ptr1.bufr;
            }
            else if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AUDIO_CS_OT)
            {
                audioPtr->otDesc = (usb_audio_ctrl_ot_desc_t *)ptr1.bufr;
            }
            else if (ptr1.common->bData[0] == USB_DESC_SUBTYPE_AUDIO_CS_FU)
            {
                audioPtr->fuDesc = (usb_audio_ctrl_fu_desc_t *)ptr1.bufr;
            }
            else
            {
            }
        }
        ptr1.bufr += ptr1.common->bLength;
        length += ptr1.common->bLength;
    }

    if (alternateSetting == 0U)
    {
        if (callbackFn != NULL)
        {
            callbackFn(callbackParam, NULL, 0U, kStatus_USB_Success);
        }
    }
    else
    {
        if (USB_HostMallocTransfer(audioPtr->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        audioPtr->controlCallbackFn = callbackFn;
        audioPtr->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = _USB_HostAudioControlCallback;
        transfer->callbackParam = audioPtr;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)audioPtr->controlIntfHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(audioPtr->hostHandle, audioPtr->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            audioPtr->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
        }
    }

    return status;
}

/*!
 * @brief get pipe max packet size.
 *
 * @param classHandle the class handle.
 * @param pipeType    It's value is USB_ENDPOINT_CONTROL, USB_ENDPOINT_ISOCHRONOUS, USB_ENDPOINT_BULK or
 * USB_ENDPOINT_INTERRUPT.
 *                     Please reference to usb_spec.h
 * @param direction    pipe direction.
 *
 * @retval 0        The classHandle is NULL.
 * @retval max packet size.
 */
uint16_t USB_HostAudioPacketSize(usb_host_class_handle classHandle, uint8_t pipeType, uint8_t direction)
{
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (pipeType == USB_ENDPOINT_ISOCHRONOUS)
    {
        if (direction == USB_IN)
        {
            return audioPtr->inPacketSize;
        }
        else
        {
            return audioPtr->outPacketSize;
        }
    }
    return 0U;
}

/*!
 * @brief audio stream receive data.
 *
 * This function implements audioreceiving data.
 *
 * @param classHandle      The class handle.
 * @param buffer                The buffer pointer.
 * @param bufferLen          The buffer length.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error              pipe is not initialized.
 *                                                       Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostAudioStreamRecv(usb_host_class_handle classHandle,
                                     uint8_t *buffer,
                                     uint32_t bufferLen,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (audioPtr->isoInPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(audioPtr->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    audioPtr->inCallbackFn = callbackFn;
    audioPtr->inCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLen;
    transfer->callbackFn = _USB_HostAudioStreamIsoInPipeCallback;
    transfer->callbackParam = audioPtr;

    if (USB_HostRecv(audioPtr->hostHandle, audioPtr->isoInPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief audio stream send data.
 *
 * This function implements audio sending data.
 *
 * @param classHandle      The class handle.
 * @param buffer                The buffer pointer.
 * @param bufferLen          The buffer length.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error              pipe is not initialized.
 *                                                       Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostAudioStreamSend(usb_host_class_handle classHandle,
                                     uint8_t *buffer,
                                     uint32_t bufferLen,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (audioPtr->isoOutPipe == NULL)
    {
        return kStatus_USB_Error;
    }

    if (USB_HostMallocTransfer(audioPtr->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    audioPtr->outCallbackFn = callbackFn;
    audioPtr->outCallbackParam = callbackParam;
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLen;
    transfer->callbackFn = _USB_HostAudioStreamIsoOutPipeCallback;
    transfer->callbackParam = audioPtr;

    if (USB_HostSend(audioPtr->hostHandle, audioPtr->isoOutPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("failt to USB_HostSend\r\n");
#endif
        USB_HostFreeTransfer(audioPtr->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief get audio stream current altsetting descriptor.
 *
 * This function implements get audio stream current altsetting descriptor.
 *
 * @param classHandle               The class handle.
 * @param asIntfDesc                 The pointer of class specific AS interface descriptor.
 * @param formatTypeDesc       The pointer of format type descriptor.
 * @param isoEndpDesc    The pointer of specific iso endp descriptor.
 *
 * @retval kStatus_USB_Success          Get audio stream current altsetting descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 *
 */
usb_status_t USB_HostAudioStreamGetCurrentAltsettingDescriptors(

    usb_host_class_handle classHandle,
    usb_audio_stream_spepific_as_intf_desc_t **asIntfDesc,
    usb_audio_stream_format_type_desc_t **formatTypeDesc,
    usb_audio_stream_specific_iso_endp_desc_t **isoEndpDesc)
{
    audio_instance_t *audioPtr = (audio_instance_t *)classHandle;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }
    *asIntfDesc = audioPtr->asIntfDesc;
    *formatTypeDesc = audioPtr->formatTypeDesc;
    *isoEndpDesc = audioPtr->isoEndpDesc;

    return kStatus_USB_Success;
}

/*!
 * @brief usb audio feature unit request.
 *
 * This function implements usb audio feature unit request.
 *
 * @param classHandle      The class handle.
 * @param channelNo        The channel number of audio feature unit.
 * @param buf                    The feature unit request buffer pointer.
 * @param cmdCode          The feature unit command code, for example USB_AUDIO_GET_CUR_MUTE etc.
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success            Feature unit request successfully.
 * @retval kStatus_USB_InvalidHandle    The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy                There is no idle transfer.
 * @retval kStatus_USB_Error                Send transfer fail, please reference to USB_HostSendSetup.
 *
 */
usb_status_t USB_HostAudioFeatureUnitRequest(usb_host_class_handle classHandle,
                                             uint8_t channelNo,
                                             void *buf,
                                             uint32_t cmdCode,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam)
{ /* Body */
    uint16_t windex;
    uint16_t request_value;
    audio_instance_t *if_ptr;
    usb_audio_request_t *p_feature_request;
    uint8_t *bmacontrols = NULL;
    uint8_t atribute_index;
    usb_status_t status = kStatus_USB_Error;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if_ptr = (audio_instance_t *)classHandle;

    /* pointer to command */
    p_feature_request = &(s_usbAudioFuRequests[cmdCode & 0xfU]);
    /* get request value */
    request_value = (uint16_t)((uint16_t)((uint16_t)p_feature_request->requestValue << 8U) | channelNo);

    /* Check whether this attribute valid or not */
    if (if_ptr->fuDesc == NULL)
    {
        return kStatus_USB_Error;
    }
    windex = (uint16_t)((uint16_t)((uint16_t)(if_ptr->fuDesc->bunitid) << 8U) | (if_ptr->streamIfnum));
    atribute_index = if_ptr->fuDesc->bcontrolsize * channelNo;

    if (atribute_index < (if_ptr->fuDesc->blength - 7))
    {
        bmacontrols = &(if_ptr->fuDesc->bcontrolsize);
    }

    if (bmacontrols == NULL)
    {
        return kStatus_USB_Error;
    }

    bmacontrols++;
    if (bmacontrols[atribute_index] & p_feature_request->controlMask)
    {
        status = kStatus_USB_Success;
    }

    if (kStatus_USB_Success == status)
    {
        status = _USB_HostAudioControl(classHandle, (p_feature_request->typeRequest | (cmdCode & 0x80U)),
                                       (p_feature_request->codeRequest | (cmdCode & 0x80U)), request_value, windex,
                                       p_feature_request->length, (uint8_t *)buf, callbackFn, callbackParam);
    }
    return status;
}

/*!
 * @brief usb audio endpoint request.
 *
 * This function implements usb audio endpoint request.
 *
 * @param classHandle      The class handle.
 * @param buf                    The feature unit buffer pointer.
 * @param cmdCode          The feature unit command code, for example USB_AUDIO_GET_CUR_PITCH etc .
 * @param callbackFn         This callback is called after this function completes.
 * @param callbackParam   The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success          Endpoint request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy               There is no idle transfer.
 * @retval kStatus_USB_Error               Send transfer fail, please reference to USB_HostSendSetup.
 *
 */
usb_status_t USB_HostAudioEndpointRequest(
    usb_host_class_handle classHandle, void *buf, uint32_t cmdCode, transfer_callback_t callbackFn, void *callbackParam)
{
    uint8_t endp_num;
    usb_status_t status = kStatus_USB_Error;
    uint16_t request_value;
    usb_audio_request_t *p_endpoint_request;
    audio_instance_t *audioPtr;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }
    audioPtr = (audio_instance_t *)classHandle;

    /* pointer to command */
    p_endpoint_request = &(s_usbAudioEpRequests[cmdCode & 0xfU]);
    /* get request value */
    request_value = (uint16_t)((uint16_t)((uint16_t)p_endpoint_request->requestValue << 8U));

    /* Check whether this attribute valid or not */
    if (audioPtr->isoEndpDesc == NULL)
    {
        return kStatus_USB_Error;
    }
    if ((audioPtr->isoEndpDesc->bmattributes) && (p_endpoint_request->controlMask))
    {
        status = kStatus_USB_Success;
    }
    else
    {
        status = kStatus_USB_InvalidRequest;
    }

    if (kStatus_USB_Success == status)
    {
        /* Any isochronous pipe is supported? */
        if ((NULL == audioPtr->isoInPipe) && (NULL == audioPtr->isoOutPipe))
        {
            return kStatus_USB_InvalidParameter;
        }
        else if (NULL != audioPtr->isoInPipe)
        {
            endp_num = (audioPtr->isoEpNum | 0x80U);
        }
        else
        {
            endp_num = audioPtr->isoEpNum;
        } /* Endif */

        status = _USB_HostAudioControl(classHandle, (p_endpoint_request->typeRequest | (cmdCode & 0x80U)),
                                       (p_endpoint_request->codeRequest | (cmdCode & 0x80U)), request_value, endp_num,
                                       p_endpoint_request->length, (uint8_t *)buf, callbackFn, callbackParam);
    }

    return status;
}
#endif /* USB_HOST_CONFIG_AUDIO */
