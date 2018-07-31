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
#if ((defined USB_HOST_CONFIG_MSD) && (USB_HOST_CONFIG_MSD))
#include "usb_host.h"
#include "usb_host_msd.h"
#include "usb_host_hci.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief clear stall transfer callback.
 *
 * @param param    callback parameter.
 * @param transfer transfer.
 * @param status   transfer result status.
 */
static void USB_HostMsdClearHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief send clear stall transfer.
 *
 * @param msdInstance     msd instance pointer.
 * @param callbackFn  callback function.
 * @param endpoint    endpoint address.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostMsdClearHalt(usb_host_msd_instance_t *msdInstance,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint);

/*!
 * @brief mass storage reset three step processes are done.
 *
 * @param msdInstance     msd instance pointer.
 * @param status      result status.
 */
static void USB_HostMsdResetDone(usb_host_msd_instance_t *msdInstance, usb_status_t status);

/*!
 * @brief mass storage reset process step 3 callback.
 *
 * @param msdInstance msd instance pointer.
 * @param transfer     transfer
 * @param status       result status.
 */
static void USB_HostMsdMassResetClearOutCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief mass storage reset process step 2 callback.
 *
 * @param msdInstance msd instance pointer.
 * @transfer           transfer
 * @param status       result status.
 */
static void USB_HostMsdMassResetClearInCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief mass storage reset process step 1 callback.
 *
 * @param msdInstance   msd instance pointer.
 * @param transfer       transfer
 * @param status         result status.
 */
static void USB_HostMsdMassResetCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief mass storage control transfer callback function.
 *
 * @param msdInstance   msd instance pointer.
 * @param transfer       transfer
 * @param status         result status.
 */
static void USB_HostMsdControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief this function is called when ufi command is done.
 *
 * @param msdInstance     msd instance pointer.
 */
static void USB_HostMsdCommandDone(usb_host_msd_instance_t *msdInstance, usb_status_t status);

/*!
 * @brief csw transfer callback.
 *
 * @param msdInstance   msd instance pointer.
 * @param transfer       transfer
 * @param status         result status.
 */
static void USB_HostMsdCswCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief cbw transfer callback.
 *
 * @param msdInstance   msd instance pointer.
 * @param transfer       transfer
 * @param status         result status.
 */
static void USB_HostMsdCbwCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief data transfer callback.
 *
 * @param msdInstance   sd instance pointer.
 * @param transfer       transfer
 * @param status         result status.
 */
static void USB_HostMsdDataCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief msd open interface.
 *
 * @param msdInstance     msd instance pointer.
 *
 * @return kStatus_USB_Success or error codes.
 */
static usb_status_t USB_HostMsdOpenInterface(usb_host_msd_instance_t *msdInstance);

/*!
 * @brief msd set interface callback, open pipes.
 *
 * @param param       callback parameter.
 * @param transfer    callback transfer.
 * @param status      transfer status.
 */
static void USB_HostMsdSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status);

/*!
 * @brief msd control transfer common code.
 *
 * This function allocate the resource for msd instance.
 *
 * @param msdInstance          the msd class instance.
 * @param pipeCallbackFn       inner callback function.
 * @param callbackFn           callback function.
 * @param callbackParam        callback parameter.
 * @param buffer               buffer pointer.
 * @param bufferLength         buffer length.
 * @param requestType          request type.
 * @param requestValue         request value.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostMsdControl(usb_host_msd_instance_t *msdInstance,
                                       host_inner_transfer_callback_t pipeCallbackFn,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam,
                                       uint8_t *buffer,
                                       uint16_t bufferLength,
                                       uint8_t requestType,
                                       uint8_t requestValue);

/*!
 * @brief command process function, this function is called many time for one command's different state.
 *
 * @param msdInstance          the msd class instance.
 *
 * @return An error code or kStatus_USB_Success.
 */
static usb_status_t USB_HostMsdProcessCommand(usb_host_msd_instance_t *msdInstance);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void USB_HostMsdClearHaltCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    if (status != kStatus_USB_Success)
    {
        USB_HostMsdCommandDone(msdInstance, kStatus_USB_TransferCancel);
    }

    if (msdInstance->commandStatus == kMSD_CommandErrorDone)
    {
        USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error); /* command fail */
    }
    else
    {
        USB_HostMsdProcessCommand(msdInstance); /* continue to process ufi command */
    }
}

static usb_status_t USB_HostMsdClearHalt(usb_host_msd_instance_t *msdInstance,
                                         host_inner_transfer_callback_t callbackFn,
                                         uint8_t endpoint)
{
    usb_status_t status;
    usb_host_transfer_t *transfer;

    /* malloc one transfer */
    status = USB_HostMallocTransfer(msdInstance->hostHandle, &transfer);
    if (status != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return status;
    }
    /* save the application callback function */
    msdInstance->controlCallbackFn = NULL;
    msdInstance->controlCallbackParam = NULL;
    /* initialize transfer */
    transfer->callbackFn = callbackFn;
    transfer->callbackParam = msdInstance;
    transfer->transferBuffer = NULL;
    transfer->transferLength = 0;
    transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_CLEAR_FEATURE;
    transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_ENDPOINT;
    transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(USB_REQUEST_STANDARD_FEATURE_SELECTOR_ENDPOINT_HALT);
    transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(endpoint);
    transfer->setupPacket->wLength = 0;
    status = USB_HostSendSetup(msdInstance->hostHandle, msdInstance->controlPipe, transfer);

    if (status != kStatus_USB_Success)
    {
        USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
    }
    msdInstance->controlTransfer = transfer;

    return status;
}

static void USB_HostMsdResetDone(usb_host_msd_instance_t *msdInstance, usb_status_t status)
{
    if (msdInstance->internalResetRecovery == 1) /* internal mass reset recovery */
    {
        msdInstance->internalResetRecovery = 0;

        if ((status != kStatus_USB_Success) || (msdInstance->commandStatus == kMSD_CommandErrorDone))
        {
            USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error); /* command fail */
        }
        else
        {
            USB_HostMsdProcessCommand(msdInstance); /* continue to process ufi command */
        }
    }
    else /* user call mass storage reset recovery */
    {
        if (msdInstance->controlCallbackFn != NULL)
        {
            /* callback to application, callback function is initialized in the USB_HostMsdControl,
            or USB_HostMsdSetInterface, but is the same function */
            msdInstance->controlCallbackFn(msdInstance->controlCallbackParam, NULL, 0,
                                           status); /* callback to application */
        }
    }
}

static void USB_HostMsdMassResetClearOutCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    msdInstance->controlTransfer = NULL;
    USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
    USB_HostMsdResetDone(msdInstance, status); /* mass storage reset done */
}

static void USB_HostMsdMassResetClearInCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    msdInstance->controlTransfer = NULL;
    USB_HostFreeTransfer(msdInstance->hostHandle, transfer);

    if (status == kStatus_USB_Success)
    {
        if (msdInstance->outPipe != NULL)
        {
            /* continue to process mass storage reset */
            USB_HostMsdClearHalt(
                msdInstance, USB_HostMsdMassResetClearOutCallback,
                (USB_REQUEST_TYPE_DIR_OUT | ((usb_host_pipe_t *)msdInstance->outPipe)->endpointAddress));
        }
    }
    else
    {
        USB_HostMsdResetDone(msdInstance, status); /* mass storage reset done */
    }
}

static void USB_HostMsdMassResetCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    msdInstance->controlTransfer = NULL;
    USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
    if (status == kStatus_USB_Success)
    {
        if (msdInstance->inPipe != NULL)
        {
            /* continue to process mass storage reset */
            USB_HostMsdClearHalt(msdInstance, USB_HostMsdMassResetClearInCallback,
                                 (USB_REQUEST_TYPE_DIR_IN | ((usb_host_pipe_t *)msdInstance->inPipe)->endpointAddress));
        }
    }
    else
    {
        USB_HostMsdResetDone(msdInstance, status); /* mass storage reset done */
    }
}

static void USB_HostMsdControlCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    msdInstance->controlTransfer = NULL;
    if (msdInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostMsdControl,
        or USB_HostMsdSetInterface, but is the same function */
        msdInstance->controlCallbackFn(msdInstance->controlCallbackParam, transfer->transferBuffer,
                                       transfer->transferSofar, status); /* callback to application */
    }
    USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
}

static void USB_HostMsdCommandDone(usb_host_msd_instance_t *msdInstance, usb_status_t status)
{
    if (msdInstance->commandCallbackFn != NULL)
    {
        /* callback to application, the callback function is initialized in USB_HostMsdCommand */
        msdInstance->commandCallbackFn(msdInstance->commandCallbackParam, msdInstance->msdCommand.dataBuffer,
                                       msdInstance->msdCommand.dataSofar, status);
    }
    msdInstance->commandStatus = kMSD_CommandIdle;
}

static void USB_HostMsdCswCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    if (status == kStatus_USB_Success)
    {
        /* kStatus_USB_Success */
        if ((transfer->transferSofar == USB_HOST_UFI_CSW_LENGTH) &&
            (msdInstance->msdCommand.cswBlock.CSWSignature == USB_LONG_TO_LITTLE_ENDIAN(USB_HOST_MSD_CSW_SIGNATURE)))
        {
            switch (msdInstance->msdCommand.cswBlock.CSWStatus)
            {
                case 0:
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Success);
                    break;

                case 1:
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_MSDStatusFail);
                    break;

                case 2:
                    msdInstance->internalResetRecovery = 1;
                    msdInstance->commandStatus = kMSD_CommandErrorDone;
                    if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                    {
                        USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                    }
                    break;

                default:
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_MSDStatusFail);
                    break;
            }
        }
        else
        {
            /* mass reset recovery to end ufi command */
            msdInstance->internalResetRecovery = 1;
            msdInstance->commandStatus = kMSD_CommandErrorDone;
            if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
            {
                USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
            }
        }
    }
    else
    {
        if (status == kStatus_USB_TransferStall) /* case 1: stall */
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--; /* retry reduce when error */
            }
            if (msdInstance->msdCommand.retryTime > 0)
            {
                /* clear stall to continue the ufi command */
                if (USB_HostMsdClearHalt(
                        msdInstance, USB_HostMsdClearHaltCallback,
                        (USB_REQUEST_TYPE_DIR_IN | ((usb_host_pipe_t *)msdInstance->inPipe)->endpointAddress)) !=
                    kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
            else
            {
                /* mass reset recovery to continue ufi command */
                msdInstance->internalResetRecovery = 1;
                msdInstance->commandStatus = kMSD_CommandErrorDone;
                if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
        }
        else if (status == kStatus_USB_TransferCancel) /* case 2: cancel */
        {
            USB_HostMsdCommandDone(msdInstance, status); /* command cancel */
        }
        else /* case 3: error */
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--; /* retry reduce when error */
            }
            if (msdInstance->msdCommand.retryTime > 0)
            {
                USB_HostMsdProcessCommand(msdInstance); /* retry the last step transaction */
            }
            else
            {
                /* mass reset recovery to continue ufi command */
                msdInstance->internalResetRecovery = 1;
                msdInstance->commandStatus = kMSD_CommandErrorDone;
                if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
        }
    }
}

static void USB_HostMsdCbwCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    if (status == kStatus_USB_Success)
    {
        /* kStatus_USB_Success */
        if (transfer->transferSofar == USB_HOST_UFI_CBW_LENGTH)
        {
            msdInstance->commandStatus = kMSD_CommandTransferData;
            USB_HostMsdProcessCommand(msdInstance); /* continue to process ufi command */
        }
        else
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--;
            }
            if (msdInstance->msdCommand.retryTime > 0)
            {
                USB_HostMsdProcessCommand(msdInstance); /* retry the last step transaction */
            }
            else
            {
                /* mass reset recovery to continue ufi command */
                msdInstance->internalResetRecovery = 1;
                msdInstance->commandStatus = kMSD_CommandErrorDone;
                if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
        }
    }
    else
    {
        if (status == kStatus_USB_TransferStall) /* case 1: stall */
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--; /* retry reduce when error */
            }
            if (msdInstance->msdCommand.retryTime > 0)
            {
                /* clear stall to continue the ufi command */
                if (USB_HostMsdClearHalt(
                        msdInstance, USB_HostMsdClearHaltCallback,
                        (USB_REQUEST_TYPE_DIR_OUT | ((usb_host_pipe_t *)msdInstance->inPipe)->endpointAddress)) !=
                    kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
            else
            {
                /* mass reset recovery to continue ufi command */
                msdInstance->internalResetRecovery = 1;
                msdInstance->commandStatus = kMSD_CommandErrorDone;
                if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
        }
        else if (status == kStatus_USB_TransferCancel) /* case 2: cancel */
        {
            USB_HostMsdCommandDone(msdInstance, status); /* command cancel */
        }
        else /* case 3: error */
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--;
            }
            if (msdInstance->msdCommand.retryTime > 0)
            {
                USB_HostMsdProcessCommand(msdInstance); /* retry the last step transaction */
            }
            else
            {
                /* mass reset recovery to continue ufi command */
                msdInstance->internalResetRecovery = 1;
                msdInstance->commandStatus = kMSD_CommandErrorDone;
                if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
                {
                    USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
                }
            }
        }
        return;
    }
}

static void USB_HostMsdDataCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;
    uint8_t direction;

    if (status == kStatus_USB_Success)
    {
        /* kStatus_USB_Success */
        msdInstance->msdCommand.dataSofar += transfer->transferSofar;
        USB_HostMsdProcessCommand(msdInstance); /* continue to process ufi command */
    }
    else
    {
        if (status == kStatus_USB_TransferStall) /* case 1: stall */
        {
            if (msdInstance->msdCommand.retryTime > 0)
            {
                msdInstance->msdCommand.retryTime--; /* retry reduce when error */
            }
            if (transfer->direction == USB_IN)
            {
                direction = USB_REQUEST_TYPE_DIR_IN;
            }
            else
            {
                direction = USB_REQUEST_TYPE_DIR_OUT;
            }

            if (msdInstance->msdCommand.retryTime == 0)
            {
                msdInstance->commandStatus = kMSD_CommandTransferCSW; /* next step */
            }
            /* clear stall to continue the ufi command */
            if (USB_HostMsdClearHalt(msdInstance, USB_HostMsdClearHaltCallback,
                                     (direction | ((usb_host_pipe_t *)msdInstance->inPipe)->endpointAddress)) !=
                kStatus_USB_Success)
            {
                USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
            }
        }
        else if (status == kStatus_USB_TransferCancel) /* case 2: cancel */
        {
            USB_HostMsdCommandDone(msdInstance, status); /* command cancel */
        }
        else /* case 3: error */
        {
            /* mass reset recovery to finish ufi command */
            msdInstance->internalResetRecovery = 1;
            msdInstance->commandStatus = kMSD_CommandErrorDone;
            if (USB_HostMsdMassStorageReset(msdInstance, NULL, NULL) != kStatus_USB_Success)
            {
                USB_HostMsdCommandDone(msdInstance, kStatus_USB_Error);
            }
        }
    }
}

static usb_status_t USB_HostMsdProcessCommand(usb_host_msd_instance_t *msdInstance)
{
    usb_status_t status = kStatus_USB_Success;
    usb_host_transfer_t *transfer;

    if (msdInstance->msdCommand.transfer == NULL)
    {
        /* malloc one transfer */
        status = USB_HostMallocTransfer(msdInstance->hostHandle, &(msdInstance->msdCommand.transfer));
        if (status != kStatus_USB_Success)
        {
            msdInstance->msdCommand.transfer = NULL;
#ifdef HOST_ECHO
            usb_echo("allocate transfer error\r\n");
#endif
            return kStatus_USB_Busy;
        }
    }
    transfer = msdInstance->msdCommand.transfer;
    switch (msdInstance->commandStatus)
    {
        case kMSD_CommandTransferCBW: /* ufi CBW phase */
            transfer->direction = USB_OUT;
            transfer->transferBuffer = (uint8_t *)(&(msdInstance->msdCommand.cbwBlock));
            transfer->transferLength = USB_HOST_UFI_CBW_LENGTH;
            transfer->callbackFn = USB_HostMsdCbwCallback;
            transfer->callbackParam = msdInstance;
            status = USB_HostSend(msdInstance->hostHandle, msdInstance->outPipe, transfer);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("host send error\r\n");
#endif
            }
            break;

        case kMSD_CommandTransferData: /* ufi DATA phase */
            if (msdInstance->msdCommand.dataBuffer != NULL)
            {
                transfer->direction = msdInstance->msdCommand.dataDirection;
                transfer->transferBuffer = (msdInstance->msdCommand.dataBuffer + msdInstance->msdCommand.dataSofar);
                transfer->transferLength = (msdInstance->msdCommand.dataLength - msdInstance->msdCommand.dataSofar);
                transfer->callbackParam = msdInstance;
                if (msdInstance->msdCommand.dataSofar != msdInstance->msdCommand.dataLength)
                {
                    if (transfer->direction == USB_OUT)
                    {
                        transfer->callbackFn = USB_HostMsdDataCallback;
                        status = USB_HostSend(msdInstance->hostHandle, msdInstance->outPipe, transfer);
                        if (status != kStatus_USB_Success)
                        {
#ifdef HOST_ECHO
                            usb_echo("host send error\r\n");
#endif
                        }
                    }
                    else
                    {
                        transfer->callbackFn = USB_HostMsdDataCallback;
                        status = USB_HostRecv(msdInstance->hostHandle, msdInstance->inPipe, transfer);
                        if (status != kStatus_USB_Success)
                        {
#ifdef HOST_ECHO
                            usb_echo("host recv error\r\n");
#endif
                        }
                    }
                    break;
                }
                else
                {
                    /* don't break */
                }
            }
            else
            {
                /* don't break */
            }
        case kMSD_CommandTransferCSW: /* ufi CSW phase */
            transfer->direction = USB_IN;
            transfer->transferBuffer = (uint8_t *)&msdInstance->msdCommand.cswBlock;
            transfer->transferLength = sizeof(usb_host_csw_t);
            transfer->callbackFn = USB_HostMsdCswCallback;
            transfer->callbackParam = msdInstance;
            status = USB_HostRecv(msdInstance->hostHandle, msdInstance->inPipe, transfer);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("host recv error\r\n");
#endif
            }
            break;

        case kMSD_CommandDone:
            USB_HostMsdCommandDone(msdInstance, kStatus_USB_Success);
            break;

        default:
            break;
    }
    return status;
}

/*!
 * @brief all ufi function calls this api.
 *
 * This function implements the common ufi commands.
 *
 * @param classHandle   the class msd handle.
 * @param buffer         buffer pointer.
 * @param bufferLength     buffer length.
 * @param callbackFn    callback function.
 * @param callbackParam callback parameter.
 * @param direction      command direction.
 * @param byteValues    ufi command fields value.
 *
 * @return An error code or kStatus_USB_Success.
 */
usb_status_t USB_HostMsdCommand(usb_host_class_handle classHandle,
                                uint8_t *buffer,
                                uint32_t bufferLength,
                                transfer_callback_t callbackFn,
                                void *callbackParam,
                                uint8_t direction,
                                uint8_t byteValues[10])
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)classHandle;
    usb_host_cbw_t *cbwPointer = &(msdInstance->msdCommand.cbwBlock);
    uint8_t index = 0;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    if (msdInstance->commandStatus != kMSD_CommandIdle)
    {
        return kStatus_USB_Busy;
    }

    /* save the application callback function */
    msdInstance->commandCallbackFn = callbackFn;
    msdInstance->commandCallbackParam = callbackParam;

    /* initialize CBWCB fields */
    for (index = 0; index < USB_HOST_UFI_BLOCK_DATA_VALID_LENGTH; ++index)
    {
        cbwPointer->CBWCB[index] = byteValues[index];
    }

    /* initialize CBW fields */
    cbwPointer->CBWDataTransferLength = USB_LONG_TO_LITTLE_ENDIAN(bufferLength);
    cbwPointer->CBWFlags = direction;
    cbwPointer->CBWLun = (byteValues[1] >> USB_HOST_UFI_LOGICAL_UNIT_POSITION);
    cbwPointer->CBWCBLength = USB_HOST_UFI_BLOCK_DATA_VALID_LENGTH;

    msdInstance->commandStatus = kMSD_CommandTransferCBW;
    if (direction == USB_HOST_MSD_CBW_FLAGS_DIRECTION_IN)
    {
        msdInstance->msdCommand.dataDirection = USB_IN;
    }
    else
    {
        msdInstance->msdCommand.dataDirection = USB_OUT;
    }
    msdInstance->msdCommand.dataBuffer = buffer;

    msdInstance->msdCommand.dataLength = bufferLength;
    msdInstance->msdCommand.dataSofar = 0;
    msdInstance->msdCommand.retryTime = USB_HOST_MSD_RETRY_MAX_TIME;

    return USB_HostMsdProcessCommand(msdInstance); /* start to process ufi command */
}

static usb_status_t USB_HostMsdOpenInterface(usb_host_msd_instance_t *msdInstance)
{
    usb_status_t status;
    uint8_t epIndex = 0;
    usb_host_pipe_init_t pipeInit;
    usb_descriptor_endpoint_t *epDesc = NULL;
    usb_host_interface_t *interfacePointer;

    if (msdInstance->inPipe != NULL) /* close bulk in pipe if the pipe is open */
    {
        status = USB_HostClosePipe(msdInstance->hostHandle, msdInstance->inPipe);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        msdInstance->inPipe = NULL;
    }
    if (msdInstance->outPipe != NULL) /* close bulk out pipe if the pipe is open */
    {
        status = USB_HostClosePipe(msdInstance->hostHandle, msdInstance->outPipe);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when close pipe\r\n");
#endif
        }
        msdInstance->outPipe = NULL;
    }

    /* open interface pipes */
    interfacePointer = (usb_host_interface_t *)msdInstance->interfaceHandle;
    for (epIndex = 0; epIndex < interfacePointer->epCount; ++epIndex)
    {
        epDesc = interfacePointer->epList[epIndex].epDesc;
        if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
             USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_IN) &&
            ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = msdInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_IN;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            status = USB_HostOpenPipe(msdInstance->hostHandle, &msdInstance->inPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_hid_set_interface fail to open pipe\r\n");
#endif
                return status;
            }
        }
        else if (((epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) ==
                  USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_OUT) &&
                 ((epDesc->bmAttributes & USB_DESCRIPTOR_ENDPOINT_ATTRIBUTE_TYPE_MASK) == USB_ENDPOINT_BULK))
        {
            pipeInit.devInstance = msdInstance->deviceHandle;
            pipeInit.pipeType = USB_ENDPOINT_BULK;
            pipeInit.direction = USB_OUT;
            pipeInit.endpointAddress = (epDesc->bEndpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            pipeInit.interval = epDesc->bInterval;
            pipeInit.maxPacketSize = (uint16_t)(USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                                USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_SIZE_MASK);
            pipeInit.numberPerUframe = (USB_SHORT_FROM_LITTLE_ENDIAN_ADDRESS(epDesc->wMaxPacketSize) &
                                        USB_DESCRIPTOR_ENDPOINT_MAXPACKETSIZE_MULT_TRANSACTIONS_MASK);
            pipeInit.nakCount = USB_HOST_CONFIG_MAX_NAK;

            status = USB_HostOpenPipe(msdInstance->hostHandle, &msdInstance->outPipe, &pipeInit);
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("usb_host_hid_set_interface fail to open pipe\r\n");
#endif
                return status;
            }
        }
        else
        {
        }
    }

    return kStatus_USB_Success;
}

static void USB_HostMsdSetInterfaceCallback(void *param, usb_host_transfer_t *transfer, usb_status_t status)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)param;

    msdInstance->controlTransfer = NULL;
    if (status == kStatus_USB_Success)
    {
        status = USB_HostMsdOpenInterface(msdInstance); /* msd open interface */
    }

    if (msdInstance->controlCallbackFn != NULL)
    {
        /* callback to application, callback function is initialized in the USB_HostMsdControl,
        or USB_HostMsdSetInterface, but is the same function */
        msdInstance->controlCallbackFn(msdInstance->controlCallbackParam, NULL, 0,
                                       status); /* callback to application */
    }
    USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
}

usb_status_t USB_HostMsdInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle)
{
    uint32_t infoValue;
    usb_status_t status;
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)USB_OsaMemoryAllocate(
        sizeof(usb_host_msd_instance_t)); /* malloc msd class instance */

    if (msdInstance == NULL)
    {
        return kStatus_USB_AllocFail;
    }

    /* initialize msd instance */
    msdInstance->deviceHandle = deviceHandle;
    msdInstance->interfaceHandle = NULL;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetHostHandle, &infoValue);
    msdInstance->hostHandle = (usb_host_handle)infoValue;
    USB_HostHelperGetPeripheralInformation(deviceHandle, kUSB_HostGetDeviceControlPipe, &infoValue);
    msdInstance->controlPipe = (usb_host_pipe_handle)infoValue;

    msdInstance->msdCommand.cbwBlock.CBWSignature = USB_LONG_TO_LITTLE_ENDIAN(USB_HOST_MSD_CBW_SIGNATURE);
    status = USB_HostMallocTransfer(msdInstance->hostHandle, &(msdInstance->msdCommand.transfer));
    if (status != kStatus_USB_Success)
    {
        msdInstance->msdCommand.transfer = NULL;
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
    }

    *classHandle = msdInstance;
    return kStatus_USB_Success;
}

usb_status_t USB_HostMsdSetInterface(usb_host_class_handle classHandle,
                                     usb_host_interface_handle interfaceHandle,
                                     uint8_t alternateSetting,
                                     transfer_callback_t callbackFn,
                                     void *callbackParam)
{
    usb_status_t status = kStatus_USB_Error;
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)classHandle;
    usb_host_transfer_t *transfer;

    if (classHandle == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    status = USB_HostOpenDeviceInterface(msdInstance->deviceHandle,
                                         interfaceHandle); /* notify host driver the interface is open */
    if (status != kStatus_USB_Success)
    {
        return status;
    }
    msdInstance->interfaceHandle = interfaceHandle;

    /* cancel transfers */
    if (msdInstance->inPipe != NULL)
    {
        status = USB_HostCancelTransfer(msdInstance->hostHandle, msdInstance->inPipe, NULL);
        if (status != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error when cancel pipe\r\n");
#endif
        }
    }
    if (msdInstance->outPipe != NULL)
    {
        status = USB_HostCancelTransfer(msdInstance->hostHandle, msdInstance->outPipe, NULL);
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
            status = USB_HostMsdOpenInterface(msdInstance);
            callbackFn(callbackParam, NULL, 0, status);
        }
    }
    else /* send setup transfer */
    {
        /* malloc one transfer */
        if (USB_HostMallocTransfer(msdInstance->hostHandle, &transfer) != kStatus_USB_Success)
        {
#ifdef HOST_ECHO
            usb_echo("error to get transfer\r\n");
#endif
            return kStatus_USB_Busy;
        }
        /* save the application callback function */
        msdInstance->controlCallbackFn = callbackFn;
        msdInstance->controlCallbackParam = callbackParam;
        /* initialize transfer */
        transfer->callbackFn = USB_HostMsdSetInterfaceCallback;
        transfer->callbackParam = msdInstance;
        transfer->setupPacket->bRequest = USB_REQUEST_STANDARD_SET_INTERFACE;
        transfer->setupPacket->bmRequestType = USB_REQUEST_TYPE_RECIPIENT_INTERFACE;
        transfer->setupPacket->wIndex = USB_SHORT_TO_LITTLE_ENDIAN(
            ((usb_host_interface_t *)msdInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber);
        transfer->setupPacket->wValue = USB_SHORT_TO_LITTLE_ENDIAN(alternateSetting);
        transfer->setupPacket->wLength = 0;
        transfer->transferBuffer = NULL;
        transfer->transferLength = 0;
        status = USB_HostSendSetup(msdInstance->hostHandle, msdInstance->controlPipe, transfer);

        if (status == kStatus_USB_Success)
        {
            msdInstance->controlTransfer = transfer;
        }
        else
        {
            USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
        }
    }

    return status;
}

usb_status_t USB_HostMsdDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)classHandle;
    usb_status_t status;

    if (classHandle != NULL)
    {
        if (msdInstance->inPipe != NULL)
        {
            status = USB_HostCancelTransfer(msdInstance->hostHandle, msdInstance->inPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(msdInstance->hostHandle, msdInstance->inPipe);            /* close pipe */
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
        }
        if (msdInstance->outPipe != NULL)
        {
            status = USB_HostCancelTransfer(msdInstance->hostHandle, msdInstance->outPipe, NULL); /* cancel pipe */
            status = USB_HostClosePipe(msdInstance->hostHandle, msdInstance->outPipe);            /* close pipe */
            if (status != kStatus_USB_Success)
            {
#ifdef HOST_ECHO
                usb_echo("error when close pipe\r\n");
#endif
            }
        }
        if ((msdInstance->controlPipe != NULL) &&
            (msdInstance->controlTransfer != NULL)) /* cancel control transfer if there is on-going control transfer */
        {
            status =
                USB_HostCancelTransfer(msdInstance->hostHandle, msdInstance->controlPipe, msdInstance->controlTransfer);
        }
        if (msdInstance->msdCommand.transfer)
        {
            USB_HostFreeTransfer(msdInstance->hostHandle, msdInstance->msdCommand.transfer);
        }
        USB_HostCloseDeviceInterface(deviceHandle,
                                     msdInstance->interfaceHandle); /* notify host driver the interface is closed */
        USB_OsaMemoryFree(msdInstance);
    }
    else
    {
        USB_HostCloseDeviceInterface(deviceHandle, NULL);
    }

    return kStatus_USB_Success;
}

static usb_status_t USB_HostMsdControl(usb_host_msd_instance_t *msdInstance,
                                       host_inner_transfer_callback_t pipeCallbackFn,
                                       transfer_callback_t callbackFn,
                                       void *callbackParam,
                                       uint8_t *buffer,
                                       uint16_t bufferLength,
                                       uint8_t requestType,
                                       uint8_t requestValue)
{
    usb_host_transfer_t *transfer;

    if (msdInstance == NULL)
    {
        return kStatus_USB_InvalidHandle;
    }

    /* malloc one transfer */
    if (USB_HostMallocTransfer(msdInstance->hostHandle, &transfer) != kStatus_USB_Success)
    {
#ifdef HOST_ECHO
        usb_echo("allocate transfer error\r\n");
#endif
        return kStatus_USB_Busy;
    }
    /* save the application callback function */
    msdInstance->controlCallbackFn = callbackFn;
    msdInstance->controlCallbackParam = callbackParam;
    /* initialize transfer */
    transfer->transferBuffer = buffer;
    transfer->transferLength = bufferLength;
    transfer->callbackFn = pipeCallbackFn;
    transfer->callbackParam = msdInstance;

    transfer->setupPacket->bmRequestType = requestType;
    transfer->setupPacket->bRequest = requestValue;
    transfer->setupPacket->wValue = 0x0000;
    transfer->setupPacket->wIndex =
        ((usb_host_interface_t *)msdInstance->interfaceHandle)->interfaceDesc->bInterfaceNumber;
    transfer->setupPacket->wLength = bufferLength;

    if (USB_HostSendSetup(msdInstance->hostHandle, msdInstance->controlPipe, transfer) !=
        kStatus_USB_Success) /* call host driver api */
    {
        USB_HostFreeTransfer(msdInstance->hostHandle, transfer);
        return kStatus_USB_Error;
    }
    msdInstance->controlTransfer = transfer;

    return kStatus_USB_Success;
}

usb_status_t USB_HostMsdMassStorageReset(usb_host_class_handle classHandle,
                                         transfer_callback_t callbackFn,
                                         void *callbackParam)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)classHandle;

    return USB_HostMsdControl(msdInstance, USB_HostMsdMassResetCallback, callbackFn, callbackParam, NULL, 0,
                              (USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE),
                              USB_HOST_HID_MASS_STORAGE_RESET);
}

usb_status_t USB_HostMsdGetMaxLun(usb_host_class_handle classHandle,
                                  uint8_t *logicalUnitNumber,
                                  transfer_callback_t callbackFn,
                                  void *callbackParam)
{
    usb_host_msd_instance_t *msdInstance = (usb_host_msd_instance_t *)classHandle;

    return USB_HostMsdControl(
        msdInstance, USB_HostMsdControlCallback, callbackFn, callbackParam, logicalUnitNumber, 1,
        (USB_REQUEST_TYPE_DIR_IN | USB_REQUEST_TYPE_TYPE_CLASS | USB_REQUEST_TYPE_RECIPIENT_INTERFACE),
        USB_HOST_HID_GET_MAX_LUN);
}

#endif /* USB_HOST_CONFIG_MSD */
