/*
 * device.c - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
    
/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/source/protocol.h>
#include <ti/drivers/net/wifi/source/driver.h>
#include <ti/drivers/net/wifi/source/flowcont.h>

/*****************************************************************************/
/* Internal functions                                                        */
/*****************************************************************************/

static _i16 _SlDeviceGetStartResponseConvert(_i32 Status);
void _SlDeviceHandleResetRequestInternally(void);
void _SlDeviceResetRequestInitCompletedCB(_u32 Status, SlDeviceInitInfo_t *DeviceInitInfo);

#define RESET_REQUEST_STOP_TIMEOUT (300)

#ifndef SL_IF_OPEN_FLAGS
#define SL_IF_OPEN_FLAGS (0x0)
#endif

#ifndef SL_IF_UART_REOPEN_FLAGS
#define SL_IF_UART_REOPEN_FLAGS (0x1)
#endif

typedef struct
{
    const void    *pIfHdl;   /* Holds the last opened interface handle */
    _i8         *pDevName; /* Holds the last opened interface parameters */
    _u32         ResetRequestSessionNumber; /* Special session number to be verified upon every reset request during provisioning */
} _SlDeviceCb_t;

_SlDeviceCb_t DeviceCB; /* the device control block */

static const _i16 StartResponseLUT[16] = 
{
    ROLE_RESERVED,
    ROLE_STA,
    SL_ERROR_ROLE_STA_ERR,
    ROLE_AP,
    SL_ERROR_ROLE_AP_ERR,
    ROLE_P2P,
    SL_ERROR_ROLE_P2P_ERR,
    SL_ERROR_CALIB_FAIL,
    SL_ERROR_FS_CORRUPTED_ERR,
    SL_ERROR_FS_ALERT_ERR,
    SL_ERROR_RESTORE_IMAGE_COMPLETE, 
    SL_ERROR_INCOMPLETE_PROGRAMMING,
    ROLE_TAG,
    SL_ERROR_ROLE_TAG_ERR,
    SL_ERROR_FIPS_ERR,
    SL_ERROR_GENERAL_ERR
};

static _i16 _SlDeviceGetStartResponseConvert(_i32 Status)
{
    return StartResponseLUT[Status & 0xF];
}

/*****************************************************************************/
/* API Functions                                                             */
/*****************************************************************************/

/*****************************************************************************/
/* sl_Task                                                                   */
/*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_Task)
void* sl_Task(void* pEntry)
{
#ifdef _SlTaskEntry
    return (void*)_SlTaskEntry();
#else
    return (void*)0;
#endif
}
#endif

/*****************************************************************************/
/* sl_Start                                                                  */
/*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_Start)
_i16 sl_Start(const void* pIfHdl, _i8*  pDevName, const P_INIT_CALLBACK pInitCallBack)
{
    _i16 ObjIdx = MAX_CONCURRENT_ACTIONS;
    InitComplete_t  AsyncRsp;
    int ret = 0;    // added for releasePoolObj

    _SlDrvMemZero(&AsyncRsp, sizeof(InitComplete_t));

    /* verify no error handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();
    if (SL_IS_DEVICE_STARTED)
    {
        return SL_RET_CODE_DEV_ALREADY_STARTED;
    }
    /* Perform any preprocessing before enable networking services */
#ifdef sl_DeviceEnablePreamble
    sl_DeviceEnablePreamble();
#endif

    /* ControlBlock init */
    (void)_SlDrvDriverCBInit();

    /* open the interface: usually SPI or UART */
    if (NULL == pIfHdl)
    {
        g_pCB->FD = sl_IfOpen((void *)pDevName, SL_IF_OPEN_FLAGS);
    }
    else
    {
        g_pCB->FD = (_SlFd_t)pIfHdl;
    }

    ObjIdx = _SlDrvProtectAsyncRespSetting((_u8 *)&AsyncRsp, START_STOP_ID, SL_MAX_SOCKETS);

    if (ObjIdx < 0)
    {
        return ObjIdx;
    }

    if( g_pCB->FD >= (_SlFd_t)0)
    {
        /* store the interface parameters for the internal call of the 
           sl_start to be called upon reset request handling */
        DeviceCB.pIfHdl = pIfHdl;
        DeviceCB.pDevName = pDevName;

        /* Mark that device is in progress! */
        SL_SET_DEVICE_START_IN_PROGRESS;

        sl_DeviceDisable();
    
        sl_IfRegIntHdlr((SL_P_EVENT_HANDLER)_SlDrvRxIrqHandler, NULL);
    
        g_pCB->pInitCallback = pInitCallBack;
        sl_DeviceEnable();
        
        if (NULL == pInitCallBack)
        {
            ret = _SlDrvWaitForInternalAsyncEvent(ObjIdx, INIT_COMPLETE_TIMEOUT, SL_OPCODE_DEVICE_INITCOMPLETE);

            SL_UNSET_DEVICE_START_IN_PROGRESS;

            SL_SET_DEVICE_STARTED;

            /* release Pool Object */
            _SlDrvReleasePoolObj(g_pCB->FunctionParams.AsyncExt.ActionIndex);
            if(ret < 0)
            {
                return ret;
            }
            else
            {
                return _SlDeviceGetStartResponseConvert(AsyncRsp.Status);
            }
        }
        else
        {
            return SL_RET_CODE_OK;
        }
    }
    return SL_BAD_INTERFACE;
}
#endif

/***************************************************************************
_SlDeviceHandleAsync_InitComplete - handles init complete signalling to 
a waiting object
****************************************************************************/
_SlReturnVal_t _SlDeviceHandleAsync_InitComplete(void *pVoidBuf)
{
    InitComplete_t     *pMsgArgs   = (InitComplete_t *)_SL_RESP_ARGS_START(pVoidBuf);
    SlDeviceInitInfo_t DeviceInitInfo;

    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();

    if(g_pCB->pInitCallback)
    {
        DeviceInitInfo.ChipId = pMsgArgs->ChipId;
        DeviceInitInfo.MoreData = pMsgArgs->MoreData;
        g_pCB->pInitCallback(_SlDeviceGetStartResponseConvert(pMsgArgs->Status), &DeviceInitInfo);
    }
    else
    {
        sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(InitComplete_t));
        SL_DRV_SYNC_OBJ_SIGNAL(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    }
        
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        if(g_pCB->pInitCallback)
        {
            SL_SET_DEVICE_STARTED;
            SL_UNSET_DEVICE_START_IN_PROGRESS;
            _SlDrvReleasePoolObj(g_pCB->FunctionParams.AsyncExt.ActionIndex);
        }
     
        return SL_OS_RET_CODE_OK;
    }
    

/***************************************************************************
_SlDeviceHandleAsync_Stop - handles stop signalling to 
a waiting object
****************************************************************************/
void _SlDeviceHandleAsync_Stop(void *pVoidBuf)
{
    _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    VERIFY_SOCKET_CB(NULL != g_pCB->StopCB.pAsyncRsp);

    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();

    if (g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs != NULL)
    {
        sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_BasicResponse_t));
        SL_DRV_SYNC_OBJ_SIGNAL(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    }

    SL_DRV_PROTECTION_OBJ_UNLOCK();

    return;
}


/*****************************************************************************
sl_stop
******************************************************************************/
typedef union
{
    SlDeviceStopCommand_t  Cmd;
    _BasicResponse_t   Rsp;
}_SlStopMsg_u;

static const _SlCmdCtrl_t _SlStopCmdCtrl =
{
    SL_OPCODE_DEVICE_STOP_COMMAND,
    (_SlArgSize_t)sizeof(SlDeviceStopCommand_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Stop)
_i16 sl_Stop(const _u16 Timeout)
{
    _i16 RetVal=0;
    _SlStopMsg_u      Msg;
    _BasicResponse_t  AsyncRsp;
    _i16 ObjIdx = MAX_CONCURRENT_ACTIONS;
    _u8 ReleasePoolObject = FALSE;
    _u8 IsProvInProgress = FALSE;

    /* NOTE: don't check VERIFY_API_ALLOWED(), this command is not
     * filtered in error handling and also not filtered in NWP lock state.
     * If we are in the middle of assert handling than ignore stopping
     * the device with timeout and force immediate shutdown as we would like
     * to avoid any additional commands to the NWP */
    if( (Timeout != 0) && (SL_IS_DEVICE_STARTED)
       && (!SL_IS_RESTART_REQUIRED))
    {
        /* Clear the Async response structure */
        _SlDrvMemZero(&AsyncRsp, sizeof(_BasicResponse_t));

        /* let the device make the shutdown using the defined timeout */
        Msg.Cmd.Timeout = Timeout;

        IsProvInProgress = SL_IS_PROVISIONING_IN_PROGRESS;

        /* if provisioning in progress do not take pool object as we are not going to wait for it  */
        if (!IsProvInProgress)
        {
            ObjIdx = _SlDrvProtectAsyncRespSetting((_u8 *)&AsyncRsp, START_STOP_ID, SL_MAX_SOCKETS);
            if (ObjIdx < 0)
            {
                return ObjIdx;
            }

            ReleasePoolObject = TRUE;
        }
  
        /* Set the stop-in-progress flag */
        SL_SET_DEVICE_STOP_IN_PROGRESS;

        VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlStopCmdCtrl, &Msg, NULL));

        int ret_pool = 0; // for _SlDrvReleasePoolObj
        /* Do not wait for stop async event if provisioning is in progress */
        if((SL_OS_RET_CODE_OK == (_i16)Msg.Rsp.status) && (!(IsProvInProgress)))
        {
            /* Wait for sync object to be signaled */
            ret_pool = _SlDrvWaitForInternalAsyncEvent(ObjIdx, STOP_DEVICE_TIMEOUT, SL_OPCODE_DEVICE_STOP_ASYNC_RESPONSE);

            Msg.Rsp.status = AsyncRsp.status;
            RetVal = Msg.Rsp.status;
        }

        /* Release pool object only if taken */
        if (ReleasePoolObject == TRUE)
        {
            _SlDrvReleasePoolObj(ObjIdx);
            if(ret_pool < 0)
            {
                return ret_pool;
            }
        }
    
        /* This macro wait for the NWP to raise a ready for shutdown indication.
        * This function is unique for the CC32XX family, and expected to return
        * in less than 600 mSec, which is the time takes for NWP to gracefully shutdown. */
        WAIT_NWP_SHUTDOWN_READY;
    }
    else
    {
        if ((!SL_IS_DEVICE_STARTED)
       && (!SL_IS_RESTART_REQUIRED))
        {
            sl_DeviceDisable();
            return SL_RET_CODE_DEV_NOT_STARTED;
        }
        /* Set the stop-in-progress flag */
        SL_SET_DEVICE_STOP_IN_PROGRESS;
    }
    /* Release (signal) all active and pending commands */
    _SlDrvReleaseAllActivePendingPoolObj();

#ifdef SL_PLATFORM_MULTI_THREADED
    /* Do not continue until all sync object deleted (in relevant context) */
    while (g_pCB->NumOfDeletedSyncObj < MAX_CONCURRENT_ACTIONS)
    {
        usleep(100000);
    }
#endif    

    /* Lock during stopping the interface only if reset is not required (if reset requires it 
     means GlobalLockObj is already deleted and interface operations cannot be performed) */
    if (!SL_IS_RESTART_REQUIRED)
    {
        SL_DRV_OBJ_LOCK_FOREVER(&GlobalLockObj);
    }
    sl_IfRegIntHdlr(NULL, NULL);
    sl_DeviceDisable();
    RetVal = sl_IfClose(g_pCB->FD);

    (void)_SlDrvDriverCBDeinit();

    /* clear the stop-in-progress flag */
    SL_UNSET_DEVICE_STOP_IN_PROGRESS;

    /* clear the device started flag */
    SL_UNSET_DEVICE_STARTED; 

    if (!SL_IS_RESTART_REQUIRED)
    {
        SL_DRV_OBJ_UNLOCK(&GlobalLockObj);
    }
    /* Clear the restart device flag  */
    SL_UNSET_RESTART_REQUIRED;

    return RetVal;
}
#endif


/*****************************************************************************
sl_DeviceEventMaskSet
*****************************************************************************/
typedef union
{
    SlDeviceMaskEventSetCommand_t        Cmd;
    _BasicResponse_t                Rsp;
}_SlEventMaskSetMsg_u;


#if _SL_INCLUDE_FUNC(sl_DeviceEventMaskSet)

static const _SlCmdCtrl_t _SlEventMaskSetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKSET,
    (_SlArgSize_t)sizeof(SlDeviceMaskEventSetCommand_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};


_i16 sl_DeviceEventMaskSet(const _u8 EventClass ,const _u32 Mask)
{
    _SlEventMaskSetMsg_u Msg;

    /* verify that this api is allowed. if not allowed then
    ignore the API execution and return immediately with an error */
    VERIFY_API_ALLOWED(SL_OPCODE_SILO_DEVICE);

    Msg.Cmd.Group = EventClass;
    Msg.Cmd.Mask = Mask;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlEventMaskSetCmdCtrl, &Msg, NULL));

    return (_i16)Msg.Rsp.status;
}
#endif
    
/******************************************************************************
sl_EventMaskGet
******************************************************************************/
typedef union
{
    SlDeviceMaskEventGetCommand_t        Cmd;
    SlDeviceMaskEventGetResponse_t      Rsp;
}_SlEventMaskGetMsg_u;



#if _SL_INCLUDE_FUNC(sl_DeviceEventMaskGet)

static const _SlCmdCtrl_t _SlEventMaskGetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKGET,
    (_SlArgSize_t)sizeof(SlDeviceMaskEventGetCommand_t),
    (_SlArgSize_t)sizeof(SlDeviceMaskEventGetResponse_t)
};


_i16 sl_DeviceEventMaskGet(const _u8 EventClass,_u32 *pMask)
{
    _SlEventMaskGetMsg_u Msg;

    /* verify that this api is allowed. if not allowed then
    ignore the API execution and return immediately with an error */
    VERIFY_API_ALLOWED(SL_OPCODE_SILO_DEVICE);

    Msg.Cmd.Group = EventClass;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlEventMaskGetCmdCtrl, &Msg, NULL));

    *pMask = Msg.Rsp.Mask;

    return SL_RET_CODE_OK;
}
#endif



/******************************************************************************
sl_DeviceGet
******************************************************************************/

typedef union
{
    SlDeviceSetGet_t        Cmd;
    SlDeviceSetGet_t        Rsp;
}_SlDeviceMsgGet_u;



#if _SL_INCLUDE_FUNC(sl_DeviceGet)

static const _SlCmdCtrl_t _SlDeviceGetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICEGET,
    (_SlArgSize_t)sizeof(SlDeviceSetGet_t),
    (_SlArgSize_t)sizeof(SlDeviceSetGet_t)
};

_i16 sl_DeviceGet(const _u8 DeviceGetId, _u8 *pOption,_u16 *pConfigLen, _u8 *pValues)
{
    _SlDeviceMsgGet_u         Msg;
    _SlCmdExt_t               CmdExt;

    /* verify that this api is allowed. if not allowed then
    ignore the API execution and return immediately with an error */
    VERIFY_API_ALLOWED(SL_OPCODE_SILO_DEVICE);

    if (*pConfigLen == 0)
    {
        return SL_EZEROLEN;
    }

    if( pOption )
    {

       _SlDrvResetCmdExt(&CmdExt);
        CmdExt.RxPayloadLen = (_i16)*pConfigLen;
        CmdExt.pRxPayload = (_u8 *)pValues;

        Msg.Cmd.DeviceSetId = DeviceGetId;

        Msg.Cmd.Option   = (_u16)*pOption;

        VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlDeviceGetCmdCtrl, &Msg, &CmdExt));

        if( pOption )
        {
            *pOption = (_u8)Msg.Rsp.Option;
        }

        if (CmdExt.RxPayloadLen < CmdExt.ActualRxPayloadLen) 
        {
            *pConfigLen = (_u16)CmdExt.RxPayloadLen;

            return SL_ESMALLBUF;
        }
        else
        {
            *pConfigLen = (_u16)CmdExt.ActualRxPayloadLen;
        }

        return (_i16)Msg.Rsp.Status;
    }
    else
    {
        return SL_RET_CODE_INVALID_INPUT;
    }
}
#endif

/******************************************************************************
sl_DeviceSet
******************************************************************************/
typedef union
{
    SlDeviceSetGet_t    Cmd;
    _BasicResponse_t   Rsp;
}_SlDeviceMsgSet_u;



#if _SL_INCLUDE_FUNC(sl_DeviceSet)

static const _SlCmdCtrl_t _SlDeviceSetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICESET,
    (_SlArgSize_t)sizeof(SlDeviceSetGet_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};

_i16 sl_DeviceSet(const _u8 DeviceSetId ,const _u8 Option,const _u16 ConfigLen,const _u8 *pValues)
{
    _SlDeviceMsgSet_u         Msg;
    _SlCmdExt_t               CmdExt;

    /* verify that this api is allowed. if not allowed then
    ignore the API execution and return immediately with an error */
    VERIFY_API_ALLOWED(SL_OPCODE_SILO_DEVICE);

    _SlDrvResetCmdExt(&CmdExt);

    CmdExt.TxPayload1Len = (ConfigLen+3) & (~3);
    CmdExt.pTxPayload1 = (_u8 *)pValues;

    Msg.Cmd.DeviceSetId    = DeviceSetId;
    Msg.Cmd.ConfigLen   = ConfigLen;
    Msg.Cmd.Option   = Option;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlDeviceSetCmdCtrl, &Msg, &CmdExt));

    return (_i16)Msg.Rsp.status;
}
#endif


/******************************************************************************
_SlDeviceEventHandler - handles internally device async events
******************************************************************************/
_SlReturnVal_t _SlDeviceEventHandler(void* pEventInfo)
{
    DeviceEventInfo_t*    pInfo = (DeviceEventInfo_t*)pEventInfo;
    _SlResponseHeader_t*  pHdr  = (_SlResponseHeader_t *)pInfo->pAsyncMsgBuff;
    _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pHdr);
    SlDeviceEvent_t       DeviceEvent;

    _SlDrvMemZero(&DeviceEvent, sizeof(DeviceEvent));

    switch(pHdr->GenHeader.Opcode)
    {
    case SL_OPCODE_DEVICE_INITCOMPLETE:
        _SlDeviceHandleAsync_InitComplete(pHdr);
        break;
    case SL_OPCODE_DEVICE_STOP_ASYNC_RESPONSE:
        _SlDeviceHandleAsync_Stop(pHdr);
        break;
    case SL_OPCODE_DEVICE_RESET_REQUEST_ASYNC_EVENT:
        {
            SlDeviceResetRequestData_t *pResetRequestData = (SlDeviceResetRequestData_t*)pMsgArgs;

#if defined(slcb_DeviceGeneralEvtHdlr) || defined (EXT_LIB_REGISTERED_GENERAL_EVENTS)
            if (pResetRequestData->Caller == SL_DEVICE_RESET_REQUEST_CALLER_PROVISIONING_EXTERNAL_CONFIGURATION)
            {
                /* call the registered events handlers (application/external lib) */
                DeviceEvent.Id = SL_DEVICE_EVENT_RESET_REQUEST;
                DeviceEvent.Data.ResetRequest.Status = 0;
                DeviceEvent.Data.ResetRequest.Caller = pResetRequestData->Caller;
                _SlDrvHandleGeneralEvents(&DeviceEvent);
                break;
            }
#endif

            if (!_SlDrvIsApiInProgress() && SL_IS_PROVISIONING_IN_PROGRESS)
            {
                if (pResetRequestData->SessionNumber != DeviceCB.ResetRequestSessionNumber)
                {
                    /* store the last session number */
                    DeviceCB.ResetRequestSessionNumber = pResetRequestData->SessionNumber;
                    
                    /* perform the reset request */
                    _SlDeviceHandleResetRequestInternally();
                }
            }
        }
        break;

    case SL_OPCODE_DEVICE_ABORT:
    {
        /* release global lock of cmd context */
        if (pInfo->bInCmdContext == TRUE)
        {
            SL_DRV_LOCK_GLOBAL_UNLOCK(TRUE);
        }

        _SlDrvHandleFatalError(SL_DEVICE_EVENT_FATAL_DEVICE_ABORT,
                                *((_u32*)pMsgArgs - 1),    /* Abort type */
                                *((_u32*)pMsgArgs));       /* Abort data */
        }
        break;

    case  SL_OPCODE_DEVICE_DEVICE_ASYNC_GENERAL_ERROR:
        {
#if defined(slcb_DeviceGeneralEvtHdlr) || defined (EXT_LIB_REGISTERED_GENERAL_EVENTS)
          
            DeviceEvent.Id = SL_DEVICE_EVENT_ERROR;
            DeviceEvent.Data.Error.Code = pMsgArgs->status;
            DeviceEvent.Data.Error.Source = (SlDeviceSource_e)pMsgArgs->sender;
            _SlDrvHandleGeneralEvents(&DeviceEvent);
#endif
        }
        break;

    case SL_OPCODE_DEVICE_FLOW_CTRL_ASYNC_EVENT:
        _SlFlowContSet((void *)pHdr);
        break;
    default:
        SL_ERROR_TRACE2(MSG_306, "ASSERT: _SlDeviceEventHandler : invalid opcode = 0x%x = %1", pHdr->GenHeader.Opcode, pHdr->GenHeader.Opcode);
    }

    return SL_OS_RET_CODE_OK;
}


void _SlDeviceResetRequestInitCompletedCB(_u32 Status, SlDeviceInitInfo_t *DeviceInitInfo)
{
    /* Do nothing...*/
}


void _SlDeviceHandleResetRequestInternally(void)
{    
    _u8 irqCountLast = RxIrqCnt;
#if (defined(slcb_GetTimestamp))
      _SlTimeoutParams_t      TimeoutInfo={0};

      _SlDrvStartMeasureTimeout(&TimeoutInfo, 2*RESET_REQUEST_STOP_TIMEOUT);
#endif

    /* Here we send stop command with timeout, but the API will not blocked 
       Till the stop complete event is received as we in the middle of async event handling */
    sl_Stop(RESET_REQUEST_STOP_TIMEOUT);
    
    /* wait till the stop complete cmd & async 
       event messages are received (2 Irqs) */
    do
    {
#if (defined(slcb_GetTimestamp))
         if (_SlDrvIsTimeoutExpired(&TimeoutInfo))
         {
            break;
         }
#endif
    }
    while((RxIrqCnt - irqCountLast) < 2);

    /* start the device again */
    sl_Start(DeviceCB.pIfHdl, DeviceCB.pDevName ,_SlDeviceResetRequestInitCompletedCB);
    
}



/******************************************************************************
sl_DeviceStat
******************************************************************************/

_i16 sl_DeviceStatStart(const _u32 Flags) // start collecting the statistics
{
    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();
    if(SL_IS_WLAN_RX_STAT_IN_PROGRESS)
    {
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        return SL_RET_CODE_WLAN_RX_STAT_IN_PROGRESS;
    }
    else
    {
       /* turn on flag indication for RX statistics is in progress
        * to avoid parallel "starts" between
        * Device statistics API and RX statistics API */
        SL_SET_DEVICE_STAT_IN_PROGRESS;
        SL_DRV_PROTECTION_OBJ_UNLOCK();
       /* verify that this api is allowed. if not allowed then
        ignore the API execution and return immediately with an error */
        VERIFY_API_ALLOWED(SL_OPCODE_SILO_WLAN);
        return _SlDrvBasicCmd(SL_OPCODE_WLAN_STARTRXSTATCOMMAND);
    }

}

_i16 sl_DeviceStatGet(const _u16 ConfigId,_u16 length,void* buffer)
{
    _i16 RetVal = 0;
    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();
    /* Device statistics and Rx statistics cannot run parallel */
    if(SL_IS_WLAN_RX_STAT_IN_PROGRESS)
    {
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        return SL_RET_CODE_WLAN_RX_STAT_IN_PROGRESS;
    }
    else
    {
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        if(SL_DEVICE_STAT_WLAN_RX == ConfigId )
        {
            /* In this case we use SL_OPCODE_WLAN_GETRXSTATCOMMAND even though we are at "Device" module -
             * duo to the fact  we want  keep this API to call the exact same deprecated API which called from Wlan  */
            _SlCmdCtrl_t CmdCtrl = {SL_OPCODE_WLAN_GETRXSTATCOMMAND, 0, (_SlArgSize_t)sizeof(SlDeviceGetStat_t)};

            /* verify that this api is allowed. if not allowed then
            ignore the API execution and return immediately with an error */
            VERIFY_API_ALLOWED(SL_OPCODE_SILO_WLAN);

            _SlDrvMemZero(buffer, (_u16)sizeof(SlDeviceGetStat_t));
            VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&CmdCtrl, buffer, NULL));

        }
        else
        {
            _u8 configOpt = ConfigId;
            RetVal = sl_DeviceGet(SL_DEVICE_GENERAL,&configOpt,&length,(_u8* )buffer);
        }

        return RetVal;
    }
}

_i16 sl_DeviceStatStop(const _u32 Flags) //stop collecting the statistics
{
    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();
    if(SL_IS_WLAN_RX_STAT_IN_PROGRESS)
    {
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        return SL_RET_CODE_WLAN_RX_STAT_IN_PROGRESS;
    }
    else
    {
        SL_UNSET_DEVICE_STAT_IN_PROGRESS;
        SL_DRV_PROTECTION_OBJ_UNLOCK();
        /* verify that this api is allowed. if not allowed then
           ignore the API execution and return immediately with an error */
        VERIFY_API_ALLOWED(SL_OPCODE_SILO_WLAN);

        return _SlDrvBasicCmd(SL_OPCODE_WLAN_STOPRXSTATCOMMAND);

    }

}
/******************************************************************************
sl_DeviceUartSetMode 
******************************************************************************/
#ifdef SL_IF_TYPE_UART
typedef union
{
    SlDeviceUartSetModeCommand_t      Cmd;
    SlDeviceUartSetModeResponse_t     Rsp;
}_SlUartSetModeMsg_u;


#if _SL_INCLUDE_FUNC(sl_DeviceUartSetMode)


const _SlCmdCtrl_t _SlUartSetModeCmdCtrl =
{
    SL_OPCODE_DEVICE_SETUARTMODECOMMAND,
    (_SlArgSize_t)sizeof(SlDeviceUartSetModeCommand_t),
    (_SlArgSize_t)sizeof(SlDeviceUartSetModeResponse_t)
};

_i16 sl_DeviceUartSetMode(const SlDeviceUartIfParams_t *pUartParams)
{
    _SlUartSetModeMsg_u Msg;
    _u32 magicCode = (_u32)0xFFFFFFFF;

    Msg.Cmd.BaudRate = pUartParams->BaudRate;
    Msg.Cmd.FlowControlEnable = pUartParams->FlowControlEnable;


    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlUartSetModeCmdCtrl, &Msg, NULL));

    /* cmd response OK, we can continue with the handshake */
    if (SL_RET_CODE_OK == Msg.Rsp.status)
    {
        sl_IfMaskIntHdlr();

        /* Close the comm port */
        sl_IfClose(g_pCB->FD);

        /* Re-open the comm port */
        sl_IfOpen((void * )pUartParams, SL_IF_UART_REOPEN_FLAGS);

        sl_IfUnMaskIntHdlr();

        /* send the magic code and wait for the response */
        sl_IfWrite(g_pCB->FD, (_u8* )&magicCode, 4);

        magicCode = UART_SET_MODE_MAGIC_CODE;
        sl_IfWrite(g_pCB->FD, (_u8* )&magicCode, 4);

        /* clear magic code */
        magicCode = 0;

        /* wait (blocking) till the magic code to be returned from device */
        sl_IfRead(g_pCB->FD, (_u8* )&magicCode, 4);

        /* check for the received magic code matching */
        if (UART_SET_MODE_MAGIC_CODE != magicCode)
        {
            _SL_ASSERT(0);
        }
    }

    return (_i16)Msg.Rsp.status;
}
#endif
#endif

