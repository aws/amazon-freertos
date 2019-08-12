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
#if ((defined USB_HOST_CONFIG_PHDC) && (USB_HOST_CONFIG_PHDC))
#include "usb_host.h"
#include "usb_host_phdc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief phdc control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcControlPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief phdc set and clear feature endpoint halt callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcSetClearFeatureEndpointHaltCallback(void *param,
                                                            usb_host_transfer_t *transfer,
                                                            usb_status_t status);

/*!
 * @brief phdc interrupt pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcInterruptPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief phdc bulk in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcBulkInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief phdc bulk out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcBulkOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief phdc set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief meta-data message preamble signature string */
static char const metaDataMsgPreambleSignature[] = "PhdcQoSSignature";

/*******************************************************************************
 * Code
 ******************************************************************************/

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)

static void USB_HostPhdcClearInHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

    phdcInstance->controlTransfer = NULL;
    /* Reduces the number of bulk transfer to 0 to expect new message preamble transfer */
    phdcInstance->numberTransferBulkIn = 0U;
    if (phdcInstance->inCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostPhdcRecv */
        phdcInstance->inCallbackFn(phdcInstance->inCallbackParam, phdcInstance->stallDataBuffer,
                                   phdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

static void USB_HostPhdcClearOutHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

    phdcInstance->controlTransfer = NULL;
    if (phdcInstance->outCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in USB_HostPhdcSend */
        phdcInstance->outCallbackFn(phdcInstance->outCallbackParam, phdcInstance->stallDataBuffer,
                                    phdcInstance->stallDataLength, kStatus_USB_TransferStall);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

static usb_status_t USB_HostPhdcClearHalt(usb_host_phdc_instance_t *phdcInstance,
                                          usb_host_transfer_t *stallTransfer,
                                          host_inner_transfer_callback_t callbackFn,
                                          uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    phdcInstance->stallDataBuffer = stallTransfer->transferBuffer;
    phdcInstance->stallDataLength = stallTransfer->transferSofar;
    /* save the application callback function */
    phdcInstance->controlCallbackFn = NULL;
    phdcInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = phdcInstance;
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status = USB_HostSendSetup(phdcInstance->hostHandle, phdcInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
    }
    phdcInstance->controlTransfer = transfer;

    return status;
}

#endif

/*!
 * @brief phdc control pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcControlPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;
    phdcInstance->controlTransfer = NULL;
    if (kStatus_USB_Success == status)
    {
        if (USB_HOST_PHDC_SET_FEATURE_REQUEST == transfer->setupPacket->bRequest)
        {
            /* Meta-data message preamble feature is enabled */
            phdcInstance->isMessagePreambleEnabled = 1U;
        }
        else if (USB_HOST_PHDC_CLEAR_FEATURE_REQUEST == transfer->setupPacket->bRequest)
        {
            /* Meta-data message preamble feature is disable */
            phdcInstance->isMessagePreambleEnabled = 0U;
        }
        else
        {
        }
    }
    if (NULL != phdcInstance->controlCallbackFn)
    {
        /* callback function is initialized in USB_HostPhdcSetInterface */
        phdcInstance->controlCallbackFn(phdcInstance->controlCallbackParam, transfer->transferBuffer,
                                        transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief phdc set and clear feature endpoint halt callback for meta-data message preamble error.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcSetClearFeatureEndpointHaltCallback(void *param,
                                                            usb_host_transfer_t *transfer,
                                                            usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;
    phdcInstance->controlTransfer = NULL;
    if (kStatus_USB_Success == status)
    {
        if ((transfer->setupPacket->bRequest == USB_REQUEST_STANDARD_SET_FEATURE) &&
            (transfer->setupPacket->bmRequestType == USB_REQUEST_TYPE_RECIPIENT_ENDPOINT) &&
            (transfer->setupPacket->wValue ==
             USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT)) &&
            (transfer->setupPacket->wIndex ==
             USB_SHORT_TO_LITTLE_ENDIAN(phdcInstance->bulkInEndpointInformation.epDesc->bEndpointAddress)))
        {
            /* The host shall issue CLEAR_FEATURE ENDPOINT_HALT request to the device */
            usb_host_process_feature_param_t featureParam;
            featureParam.requestType = kRequestEndpoint;
            featureParam.featureSelector = USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT;
            featureParam.interfaceOrEndpoint = phdcInstance->bulkInEndpointInformation.epDesc->bEndpointAddress;
            if (kStatus_USB_Success != USB_HostPhdcSetClearFeatureEndpointHalt(phdcInstance->hostHandle,
                                                                               USB_REQUEST_STANDARD_CLEAR_FEATURE,
                                                                               &featureParam, NULL, NULL))
            {
#ifdef HOST_ECHO
                usb_echo("Error for USB_HostPhdcSetClearFeatureEndpointHalt\r\n");
#endif
            }
        }
        if ((transfer->setupPacket->bRequest == USB_REQUEST_STANDARD_CLEAR_FEATURE) &&
            (transfer->setupPacket->bmRequestType == USB_REQUEST_TYPE_RECIPIENT_ENDPOINT) &&
            (transfer->setupPacket->wValue ==
             USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT)) &&
            (transfer->setupPacket->wIndex ==
             USB_SHORT_TO_LITTLE_ENDIAN(phdcInstance->bulkInEndpointInformation.epDesc->bEndpointAddress)))
        {
            /* Reduces the number of bulk transfer to 0 to expect new message preamble transfer */
            phdcInstance->numberTransferBulkIn = 0U;
        }
    }
    if (NULL != phdcInstance->controlCallbackFn)
    {
        /* Notify to application the status of request, callback function is initialized in USB_HostPhdcSetInterface */
        phdcInstance->controlCallbackFn(phdcInstance->controlCallbackParam, transfer->transferBuffer,
                                        transfer->transferSofar, status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief phdc interrupt pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcInterruptPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostPhdcClearHalt(
                phdcInstance, transfer, USB_HostPhdcClearInHaltCallback,
                (USB_REQUEST_TYPE_DIR_IN | ((usb_host_pipe_t *)phdcInstance->interruptPipe)->endpointAddress)) ==
            kStatus_USB_Success)
        {
            USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (NULL != phdcInstance->inCallbackFn)
    {
        /* callback to application, the callback function is initialized in USB_HostPhdcRecv */
        phdcInstance->inCallbackFn(phdcInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief phdc bulk in pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcBulkInPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostPhdcClearHalt(
                phdcInstance, transfer, USB_HostPhdcClearInHaltCallback,
                (USB_REQUEST_TYPE_DIR_IN | ((usb_host_pipe_t *)phdcInstance->bulkInPipe)->endpointAddress)) ==
            kStatus_USB_Success)
        {
            USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (status == kStatus_USB_Success)
    {
        /* The meta-data message preamble is implemented and enabled */
        if (phdcInstance->isMessagePreambleEnabled == 1U)
        {
            /* The meta-data message preamble feature is enabled, then all data transfers or sets
            of data transfers shall be preceded by a meta-data message preamble transfer. The
            numberTransferBulkIn is initialized as zero for receiving this message preamble data,
            then it is updated to the value of bNumTransfers field of message preamble data */
            if (phdcInstance->numberTransferBulkIn)
            {
                /* When numberTransferBulkIn reduces to 0, a new meta-data message preamble shall
                be transferred */
                phdcInstance->numberTransferBulkIn--;
            }
            else
            {
                uint8_t preambleSignatureChecking = 1U;
                /* The received packet is meta-data message preamble */
                usb_host_phdc_metadata_preamble_t *metaDataMsgPreamble =
                    (usb_host_phdc_metadata_preamble_t *)transfer->transferBuffer;
                /* Meta-data message preamble signature checking */
                for (uint8_t i = 0U; i < USB_HOST_PHDC_MESSAGE_PREAMBLE_SIGNATURE_SIZE; i++)
                {
                    if (*(transfer->transferBuffer + i) != metaDataMsgPreambleSignature[i])
                    {
                        preambleSignatureChecking = 0U;
                        break;
                    }
                }
                if (preambleSignatureChecking)
                {
                    /* Checks if the meta-data message preamble contains an invalid bmLatencyReliability value
                    or bNumTransfers value */
                    if ((!(metaDataMsgPreamble->bNumberTransfers)) || /* bNumTransfers shall never equal zero */
                        (metaDataMsgPreamble->bQosEncodingVersion != 0x01U) || /* Encoding version should be 0x01 */
                        ((metaDataMsgPreamble->bmLatencyReliability !=
                          0x02U) && /* Medium.Good latency, reliability bin */
                         (metaDataMsgPreamble->bmLatencyReliability !=
                          0x04U) && /* Medium.Better latency, reliability bin */
                         (metaDataMsgPreamble->bmLatencyReliability !=
                          0x08U) && /* Medium.Best latency, reliability bin */
                         (metaDataMsgPreamble->bmLatencyReliability !=
                          0x10U) && /* High.Best latency, reliability bin */
                         (metaDataMsgPreamble->bmLatencyReliability !=
                          0x20U) /* VeryHigh.Best latency, reliability bin */))
                    {
                        /* The host shall issue SET_FEATURE ENDPOINT_HALT request to the device */
                        usb_host_process_feature_param_t featureParam;
                        featureParam.requestType = kRequestEndpoint;
                        featureParam.featureSelector = USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT;
                        featureParam.interfaceOrEndpoint =
                            phdcInstance->bulkInEndpointInformation.epDesc->bEndpointAddress;
                        if (kStatus_USB_Success !=
                            USB_HostPhdcSetClearFeatureEndpointHalt(
                                phdcInstance->hostHandle, USB_REQUEST_STANDARD_SET_FEATURE, &featureParam, NULL, NULL))
                        {
#ifdef HOST_ECHO
                            usb_echo(
                                "USB_HostPhdcBulkInPipeCallback: Error for "
                                "USB_HostPhdcSetClearFeatureEndpointHalt\r\n");
#endif
                        }
                    }
                    else
                    {
                        /* The meta-data message preamble data is correct, update the phdc status and
                         * numberTransferBulkIn value */
                        phdcInstance->numberTransferBulkIn = metaDataMsgPreamble->bNumberTransfers;
                    }
                }
            }
        }
    }
    if (NULL != phdcInstance->inCallbackFn)
    {
        /* callback to application, the callback function is initialized in USB_HostPhdcRecv */
        phdcInstance->inCallbackFn(phdcInstance->inCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                   status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief phdc bulk out pipe transfer callback.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcBulkOutPipeCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

#if ((defined USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL) && USB_HOST_CONFIG_CLASS_AUTO_CLEAR_STALL)
    if (status == kStatus_USB_TransferStall)
    {
        if (USB_HostPhdcClearHalt(
                phdcInstance, transfer, USB_HostPhdcClearOutHaltCallback,
                (USB_REQUEST_TYPE_DIR_OUT | ((usb_host_pipe_t *)phdcInstance->bulkOutPipe)->endpointAddress)) ==
            kStatus_USB_Success)
        {
            USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
            return;
        }
    }
#endif
    if (NULL != phdcInstance->outCallbackFn)
    {
        /* callback to application, callback function is initialized in USB_HostPhdcSend */
        phdcInstance->outCallbackFn(phdcInstance->outCallbackParam, transfer->transferBuffer, transfer->transferSofar,
                                    status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief phdc open interface.
 *
 * @param phdcInstance     phdc instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostPhdcOpenInterface(usb_host_phdc_instance_t *phdcInstance)
{
    usb_status_t status;
    uint8_t epIndex = 0U;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *epDesc = NULL;
    usb_host_interface_t *interface;
    if (NULL != phdcInstance->interruptPipe)
    {
        /* Close the PHDC interrupt pipe if it is opening */
        status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->interruptPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcOpenInterface: Error when close pipe\r\n");
#endif
        }
        phdcInstance->interruptPipe = NULL;
    }

    if (NULL != phdcInstance->bulkInPipe)
    {
        /* Close the PHDC bulk in pipe if it is opening */
        status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->bulkInPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcOpenInterface: Error when close pipe\r\n");
#endif
        }
        phdcInstance->bulkInPipe = NULL;
    }

    if (NULL != phdcInstance->bulkOutPipe)
    {
        /* Close the PHDC bulk out pipe if it is opening */
        status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->bulkOutPipe);

        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcOpenInterface: Error when close pipe\r\n");
#endif
        }
        phdcInstance->bulkOutPipe = NULL;
    }

    /* open interface pipes */
    interface = (usb_host_interface_t *)phdcInstance->interfaceHandle;
    for (epIndex = 0U; epIndex < interface->epCount; ++epIndex)
    {
        epDesc = interface->epList[epIndex].epDesc;
        if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_INTERRUPT))
        {
            /* Initialize the interrupt pipe */
            pipeInit.devInstance = phdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_INTERRUPT;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;
            status = USB_HostOpenPipe(phdcInstance->hostHandle, &phdcInstance->interruptPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcOpenInterface: Error when open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
            /* save interrupt in endpoint information */
            phdcInstance->interruptInEndpointInformation = interface->epList[epIndex];
        }
        else if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
                 ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            /* Initialize bulk in pipe */
            pipeInit.devInstance = phdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;
            status = USB_HostOpenPipe(phdcInstance->hostHandle, &phdcInstance->bulkInPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcOpenInterface: Error when open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
            /* save bulk in endpoint information */
            phdcInstance->bulkInEndpointInformation = interface->epList[epIndex];
        }
        else if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            /* Initialize bulk out pipe */
            pipeInit.devInstance = phdcInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_OUT;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;
            status = USB_HostOpenPipe(phdcInstance->hostHandle, &phdcInstance->bulkOutPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcOpenInterface: Error when open pipe\r\n");
#endif
                return kStatus_USB_Error;
            }
            /* save bulk out endpoint information */
            phdcInstance->bulkOutEndpointInformation = interface->epList[epIndex];
        }
        else
        {
        }
    }
    return kStatus_USB_Success;
}

/*!
 * @brief phdc set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostPhdcSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)param;

    phdcInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        /* set interface is done, open the interface */
        status = USB_HostPhdcOpenInterface(phdcInstance);
    }

    if (NULL != phdcInstance->controlCallbackFn)
    {
        /* Notify to application the status of set interface request, callback function is initialized in
         * USB_HostPhdcSetInterface */
        phdcInstance->controlCallbackFn(phdcInstance->controlCallbackParam, NULL, 0U, status);
    }
    USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
}

/*!
 * @brief set interface.
 *
 * This function binds the interface with the phdc instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the interface handle.
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
usb_status_t USB_HostPhdcSetInterface(usb_host_class_handle classHandle,
                                      usb_host_interface_handle interfaceHandle,
                                      uint8_t alternateSetting,
                                      transfer_callback_t callbackFn,
                                      void *callbackParam)
{
    usb_status_t status;
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidParameter;
    }

    phdcInstance->interfaceHandle = interfaceHandle;

    status = USB_HostOpenDeviceInterface(phdcInstance->deviceHandle, interfaceHandle);
    if (status != kStatus_USB_Success)
    {
        return status;
    }

    /* Cancel interrupt transfers */
    if (NULL != phdcInstance->interruptPipe)
    {
        status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->interruptPipe, NULL);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcSetInterface: Error when cancel pipe\r\n");
#endif
        }
    }
    /* Cancel bulk in transfers */
    if (NULL != phdcInstance->bulkInPipe)
    {
        status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->bulkInPipe, NULL);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcSetInterface: Error when cancel pipe\r\n");
#endif
        }
    }
    /* Cancel bulk out transfers */
    if (NULL != phdcInstance->bulkOutPipe)
    {
        status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->bulkOutPipe, NULL);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcSetInterface: Error when cancel pipe\r\n");
#endif
        }
        phdcInstance->bulkOutPipe = NULL;
    }
    if (0U == alternateSetting)
    {
        if (NULL != callbackFn)
        {
            status = USB_HostPhdcOpenInterface(phdcInstance);
            callbackFn(callbackParam, NULL, 0U, status);
        }
    }
    else
    {
        /* Create transfer buffer */
        if (USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("USB_HostPhdcSetInterface: Error to get transfer\r\n");
#endif
            return kStatus_USB_Error;
        }
        /* Save application callback function and parameter */
        phdcInstance->controlCallbackFn = callbackFn;
        phdcInstance->controlCallbackParam = callbackParam;
        /* Initialize transfer */
        transfer->callbackFn = USB_HostPhdcSetInterfaceCallback;
        transfer->callbackParam = phdcInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)phdcInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        /* Send set interface request to device */
        status = USB_HostSendSetup(phdcInstance->hostHandle, phdcInstance->controlPipe, transfer);
        if (status == kStatus_USB_Success)
        {
            phdcInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
        }
    }
    return status;
}

/*!
 * @brief initialize the phdc instance.
 *
 * This function allocates the resource for phdc instance.
 *
 * @param deviceHandle       the device handle.
 * @param classHandle        return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostPhdcInit(usb_host_handle deviceHandle, usb_host_class_handle *classHandle)
{
    usb_host_phdc_instance_t *phdcInstance =
        (usb_host_phdc_instance_t *)USB_OsaMemoryAllocate(sizeof(usb_host_phdc_instance_t));
    uint32_t infoValue;

    if (NULL == phdcInstance)
    {
        return kStatus_USB_AllocFail;
    }
    /* Initialize PHDC instance */
    phdcInstance->deviceHandle = deviceHandle;
    phdcInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    phdcInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    phdcInstance->controlPipe = (usb_host_pipe_handle)infoValue;

    *classHandle = phdcInstance;
    return kStatus_USB_Success;
}

/*!
 * @brief de-initialize the phdc instance.
 *
 * This function release the resource for phdc instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle the class handle.
 *
 * @retval kStatus_USB_Success        The device is de-initialized successfully.
 */
usb_status_t USB_HostPhdcDeinit(usb_host_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_status_t status;
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;

    if (NULL == deviceHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (NULL != classHandle)
    {
        if (NULL != phdcInstance->interruptPipe)
        {
            /* Cancel/close interrupt transfers/pipe */
            status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->interruptPipe, NULL);
            status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->interruptPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcDeinit: Error when close pipe\r\n");
#endif
            }
            phdcInstance->interruptPipe = NULL;
        }
        if (NULL != phdcInstance->bulkInPipe)
        {
            /* Cancel/close bulk in transfers/pipe */
            status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->bulkInPipe, NULL);
            status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->bulkInPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcDeinit: Error when close pipe\r\n");
#endif
            }
            phdcInstance->bulkInPipe = NULL;
        }
        if (NULL != phdcInstance->bulkOutPipe)
        {
            /* Cancel/close bulk out transfers/pipe */
            status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->bulkOutPipe, NULL);
            status = USB_HostClosePipe(phdcInstance->hostHandle, phdcInstance->bulkOutPipe);

            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcDeinit: Error when close pipe\r\n");
#endif
            }
            phdcInstance->bulkOutPipe = NULL;
        }
        if ((NULL != phdcInstance->controlPipe) && (NULL != phdcInstance->controlTransfer))
        {
            /* Cancel control transfers */
            status = USB_HostCancelTransfer(phdcInstance->hostHandle, phdcInstance->controlPipe,
                                            phdcInstance->controlTransfer);
        }
        /* Close device interface */
        USB_HostCloseDeviceInterface(deviceHandle, phdcInstance->interfaceHandle);
        /* Release PHDC instance */
        USB_OsaMemoryFree(phdcInstance);
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
 * This function implements phdc receiving data.
 *
 * @param classHandle   the class handle.
 * @param qos           QoS of the data being received.
 * @param buffer        the buffer pointer.
 * @param bufferLength  the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HostPhdcRecv(usb_host_class_handle classHandle,
                              uint8_t qos,
                              uint8_t *buffer,
                              uint32_t bufferLength,
                              transfer_callback_t callbackFn,
                              void *callbackParam)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_host_transfer_t *transfer;
    usb_host_pipe_handle pipe;
    usb_host_phdc_qos_descriptor_t *qosDesc = NULL;

    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if ((NULL == phdcInstance->interruptPipe) && (NULL == phdcInstance->bulkInPipe))
    {
        return kStatus_USB_Error;
    }
    /* Allocate the transfer buffer */
    if (USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* Save application callback function and parameter */
    phdcInstance->inCallbackFn = callbackFn;
    phdcInstance->inCallbackParam = callbackParam;
    /* Initialize the transfer pointer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackParam = phdcInstance;
    /* The on can receive the data on interrupt pipe or bulk in pipe depends on the QoS value */
    pipe = (qos & 0x01U) ? (phdcInstance->interruptPipe) : (phdcInstance->bulkInPipe);
    if (pipe == phdcInstance->bulkInPipe)
    {
        /* get bulk in QoS descriptor */
        qosDesc = (usb_host_phdc_qos_descriptor_t *)phdcInstance->bulkInEndpointInformation.epExtension;
        transfer->callbackFn = USB_HostPhdcBulkInPipeCallback;
    }
    else
    {
        /* get interrupt in QoS descriptor */
        qosDesc = (usb_host_phdc_qos_descriptor_t *)phdcInstance->interruptInEndpointInformation.epExtension;
        transfer->callbackFn = USB_HostPhdcInterruptPipeCallback;
    }
    /* Latency and reliability checking */
    if (!(qos & qosDesc->bmLatencyReliability))
    {
#ifdef HOST_ECHO
        usb_echo("USB_HostPhdcRecv, ERROR: invalid QoS bin");
#endif
        return kStatus_USB_Error;
    }
    /* The previous control transfer is pending */
    if (NULL != phdcInstance->controlTransfer)
    {
#ifdef HOST_ECHO
        usb_echo("USB_HostPhdcRecv, ERROR: Control transfer is in progress");
#endif
        return kStatus_USB_Busy;
    }
    if (USB_HostRecv(phdcInstance->hostHandle, pipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("fail to USB_HostRecv\r\n");
#endif
        USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }

    return kStatus_USB_Success;
}

/*!
 * @brief send data.
 *
 * This function implements phdc sending data.
 *
 * @param classHandle   the class handle.
 * @param buffer        the buffer pointer.
 * @param bufferLength  the buffer length.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostPhdcSend(usb_host_class_handle classHandle,
                              uint8_t *buffer,
                              uint32_t bufferLength,
                              transfer_callback_t callbackFn,
                              void *callbackParam)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_host_transfer_t *transfer;
    usb_status_t status;
    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (NULL == phdcInstance->bulkOutPipe)
    {
        return kStatus_USB_Error;
    }
    if (NULL != phdcInstance->controlTransfer)
    {
        status = kStatus_USB_Busy;
#ifdef HOST_ECHO
        usb_echo("USB_HostPhdcSend, Error: control transfer is in progress");
#endif
        return status;
    }
    /* The meta-data message preamble is implemented and enabled */
    if (phdcInstance->isMessagePreambleEnabled == 1U)
    {
        /* The meta-data message preamble feature is enabled, then all data transfers or sets
        of data transfers shall be preceded by a meta-data message preamble transfer. The
        numberTransferBulkOut is initialized as zero for sending this message preamble data,
        then it is updated to the value of bNumTransfers field of message preamble data */
        if (phdcInstance->numberTransferBulkOut)
        {
            /* When numberTransferBulkOut reduces to 0, a new meta-data message preamble shall
            be transferred */
            phdcInstance->numberTransferBulkOut--;
        }
        else
        {
            usb_host_phdc_qos_descriptor_t *qosDesc =
                (usb_host_phdc_qos_descriptor_t *)phdcInstance->bulkOutEndpointInformation.epExtension;
            ;
            uint8_t latencyReliability = ((usb_host_phdc_metadata_preamble_t *)buffer)->bmLatencyReliability;
            /* Latency reliability validity checking */
            if ((latencyReliability != 0x02U) && /* Medium.Good latency, reliability bin */
                (latencyReliability != 0x04U) && /* Medium.Better latency, reliability bin */
                (latencyReliability != 0x08U) && /* Medium.Best latency, reliability bin */
                (latencyReliability != 0x10U) && /* High.Best latency, reliability bin */
                (latencyReliability != 0x20U) /* VeryHigh.Best latency, reliability bin */)
            {
                status = kStatus_USB_InvalidRequest;
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcSend, Error: invalid LatencyReliability");
#endif
                return status;
            }
            /* LatencyReliablity checking */
            if (0U == (qosDesc->bmLatencyReliability & latencyReliability))
            {
                status = kStatus_USB_Error;
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcSend, Error: the latency reliability is not supported by Bulk OUT endpoint");
#endif
                return status;
            }
            if (0U == ((usb_host_phdc_metadata_preamble_t *)buffer)->bNumberTransfers)
            {
                status = kStatus_USB_Error;
#ifdef HOST_ECHO
                usb_echo("USB_HostPhdcSend, Error: the numTransfer should never zero");
#endif
                return status;
            }
            /* Update the number of bulk out transfer */
            phdcInstance->numberTransferBulkOut = ((usb_host_phdc_metadata_preamble_t *)buffer)->bNumberTransfers;
        }
    }
    /* Allocate the transfer pointer */
    if (USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* Save the application callback function and parameter */
    phdcInstance->outCallbackFn = callbackFn;
    phdcInstance->outCallbackParam = callbackParam;
    /* Initialize the transfer pointer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = USB_HostPhdcBulkOutPipeCallback;
    transfer->callbackParam = phdcInstance;
    if (USB_HostSend(phdcInstance->hostHandle, phdcInstance->bulkOutPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("fail to USB_HostSend\r\n");
#endif
        USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    return kStatus_USB_Success;
}

/*!
 * @brief phdc sends control request.
 *
 * @param classHandle   the class handle.
 * @param request_type  setup packet request.
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostPhdcSendControlRequest(usb_host_class_handle classHandle,
                                            uint8_t request,
                                            transfer_callback_t callbackFn,
                                            void *callbackParam)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_status_t status = kStatus_USB_Success;
    usb_host_transfer_t *transfer;

    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (NULL == phdcInstance->controlPipe)
    {
        return kStatus_USB_Error;
    }

    if (NULL != phdcInstance->controlTransfer)
    {
        return kStatus_USB_Busy;
    }
    /* Allocate the transfer pointer */
    if (USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* Save the callback function and parameter */
    phdcInstance->controlCallbackFn = callbackFn;
    phdcInstance->controlCallbackParam = callbackParam;
    /* Initialize the transfer pointer */
    transfer->callbackFn = USB_HostPhdcControlPipeCallback;
    transfer->callbackParam = phdcInstance;
    transfer->setupPacket->bRequest = request;
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
        ((usb_host_interface_t *)phdcInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
    switch (request)
    {
        case USB_HOST_PHDC_GET_STATUS_REQUEST:
            /* Initialize the PHDC get status request */
            transfer->setupPacket->wValue = 0U;
            transfer->setupPacket->bmRequestType =
                USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
            transfer->setupPacket->wLength = USB_SHORT_TO_LITTLE_ENDIAN(2U);
            break;
        case USB_HOST_PHDC_SET_FEATURE_REQUEST:
        case USB_HOST_PHDC_CLEAR_FEATURE_REQUEST:
            /* Initialize the PHDC set/clear feature request */
            transfer->setupPacket->bmRequestType =
                USB_REQUEST_TYPE_DIR_OUT | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
            transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(
                USB_HOST_PHDC_FEATURE_METADATA | (uint16_t)((uint16_t)USB_HOST_PHDC_QOS_ENCODING_VERSION << 8U));
            transfer->setupPacket->wLength = 0U;
            break;
        default:
            status = kStatus_USB_InvalidRequest;
            break;
    }
    if (USB_HostSendSetup(phdcInstance->hostHandle, phdcInstance->controlPipe, transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("fail for USB_HostSendSetup\r\n");
#endif
        USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    phdcInstance->controlTransfer = transfer;

    return status;
}

/*!
 * @brief phdc set and clear feature endpoint halt request for meta-data message preamble error.
 *
 * @param classHandle   the class handle.
 * @param request       setup packet request.
 * @param param         request parameter
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        send request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostSend.
 */
usb_status_t USB_HostPhdcSetClearFeatureEndpointHalt(usb_host_class_handle classHandle,
                                                     uint8_t request,
                                                     void *param,
                                                     transfer_callback_t callbackFn,
                                                     void *callbackParam)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_host_transfer_t *transfer;
    if (NULL == classHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (NULL == phdcInstance->controlPipe)
    {
        return kStatus_USB_Error;
    }

    if (NULL != phdcInstance->controlTransfer)
    {
        return kStatus_USB_Busy;
    }
    /* Allocate the transfer pointer */
    if (USB_HostMallocTransfer(phdcInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("error to get transfer\r\n");
#endif
        return kStatus_USB_Error;
    }
    /* Save application callback function and parameter */
    phdcInstance->controlCallbackFn = callbackFn;
    phdcInstance->controlCallbackParam = callbackParam;
    /* Initialize the transfer request */
    transfer->callbackFn = USB_HostPhdcSetClearFeatureEndpointHaltCallback;
    transfer->callbackParam = phdcInstance;
    if (USB_HostRequestControl(phdcInstance->deviceHandle, request, transfer, param))
    {
#ifdef HOST_ECHO
        usb_echo("fail for USB_HostRequestControl\r\n");
#endif
        USB_HostFreeTransfer(phdcInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    phdcInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

/*!
 * @brief USB_HostPhdcGetEndpointInformation.
 * This function returns the PHDC endpoint information structure contains endpoint
 * descriptor and endpoint extended descriptor.
 *
 * @param classHandle   the class handle.
 * @param pipeType      pipe type.
 * @param direction     pipe direction.
 *
 * @retval endpointReturn   All input parameters are valid.
 * @retval NULL             One or more input parameters are invalid.
 */
usb_host_ep_t *USB_HostPhdcGetEndpointInformation(usb_host_class_handle classHandle,
                                                  uint8_t pipeType,
                                                  uint8_t direction)
{
    usb_host_phdc_instance_t *phdcInstance = (usb_host_phdc_instance_t *)classHandle;
    usb_host_ep_t *endpointReturn = NULL;
    if (NULL != classHandle)
    {
        if (pipeType == USB_ENDPOINT_BULK)
        {
            if (direction == USB_IN)
            {
                /* bulk in endpoint information */
                endpointReturn = (usb_host_ep_t *)&phdcInstance->bulkInEndpointInformation;
            }
            else
            {
                /* bulk out endpoint information */
                endpointReturn = (usb_host_ep_t *)&phdcInstance->bulkOutEndpointInformation;
            }
        }
        else if (pipeType == USB_ENDPOINT_INTERRUPT)
        {
            /* interrupt in endpoint information */
            endpointReturn = (usb_host_ep_t *)&phdcInstance->interruptInEndpointInformation;
        }
        else
        {
        }
    }
    return endpointReturn;
}
#endif
