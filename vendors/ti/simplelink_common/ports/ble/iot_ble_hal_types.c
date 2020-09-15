/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file iot_ble_hal_types.c
 * @brief Converts AFR BLE HAL types to TI BLE-Stack types
 */

#include "iot_ble_hal_types.h"

BTStatus_t _IotBleHalTypes_ConvertStatus( bStatus_t xStatus )
{
    BTStatus_t xRet = eBTStatusSuccess;

    switch ( xStatus )
    {
        case SUCCESS:
            xRet = eBTStatusSuccess;
            break;
        case bleProcedureComplete:
        case bleAlreadyInRequestedMode:
        case bleLinkEncrypted:
            xRet = eBTStatusDone;
            break;
        case bleNotReady:
        case blePairingTimedOut:
        case bleIncorrectMode:
            xRet = eBTStatusNotReady;
            break;
        case bleNoResources:
        case bleMemAllocError:
        case bleMemFreeError:
        case MSG_BUFFER_NOT_AVAIL:
            xRet = eBTStatusNoMem;
            break;
        case bleNotConnected:
            xRet = eBTStatusRMTDevDown;
            break;
        case blePending:
            xRet = eBTStatusBusy;
            break;
        case bleInternalError:
        case bleTimeout:
        case bleGAPUserCanceled:
        case bleGAPConnNotAcceptable:
        case bleGAPBufferInUse:
        case bleGAPNotFound:
        case bleGAPFilteredOut:
        case bleInvalidPDU:
        case FAILURE:
            xRet = eBTStatusFail;
            break;
        case bleInvalidMtuSize:
        case bleInvalidRange:
        case INVALIDPARAMETER:
        case INVALID_TASK:
        case INVALID_MSG_POINTER:
        case INVALID_EVENT_ID:
        case INVALID_INTERRUPT_ID:
        case INVALID_MEM_SIZE:
            xRet = eBTStatusParamInvalid;
            break;
        case bleGAPBondRejected:
        case bleInsufficientAuthen:
        case bleInsufficientEncrypt:
        case bleInsufficientKeySize:
            xRet = eBTStatusAuthFailure;
            break;

        default:
            xRet = eBTStatusUnHandled;
            break;
    }

    return ( xRet );
}

/*-----------------------------------------------------------*/
