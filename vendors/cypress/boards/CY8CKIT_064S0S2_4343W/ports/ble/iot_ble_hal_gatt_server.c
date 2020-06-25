/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_ble_hal_gatt_server.c
 * @brief Hardware Abstraction Layer for GATT server ble stack.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_internal.h"
// #include "aws_ble_event_manager.h"

#include "wiced_bt_gatt.h"
#include "wiced_bt_ble.h"

#include "bt_hal_internal.h"
#include "wiced_bt_dynamic_gattdb.h"

#define APP_ID    0

uint32_t ulGattServerIFhandle;
bool bInterfaceCreated = false;
uint8_t ucGattRegistry = 0;

static uint16_t usHandle = 1;
static BTGattServerCallbacks_t xGattServerCb;
static BTBdaddr_t xRemoteAddr;
static wiced_bool_t bServerRegistered = WICED_FALSE;

extern void wiced_bt_gatt_deregister (void);

static BTStatus_t prvBTRegisterServer( BTUuid_t * pxUuid );
static BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf );
static BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks );
static BTStatus_t prvBTConnect( uint8_t ucServerIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t xTransport );
static BTStatus_t prvBTDisconnect( uint8_t ucServerIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );
static BTStatus_t prvBTAddServiceBlob( uint8_t ucServerIf,
                                       BTService_t * pxService );
static BTStatus_t prvBTAddService( uint8_t ucServerIf,
                                   BTGattSrvcId_t * pxSrvcId,
                                   uint16_t usNumHandles );
static BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                           uint16_t usServiceHandle,
                                           uint16_t usIncludedHandle );
static BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                          uint16_t usServiceHandle,
                                          BTUuid_t * pxUuid,
                                          BTCharProperties_t xProperties,
                                          BTCharPermissions_t xPermissions );
static BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue );
static BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                                      uint16_t usServiceHandle,
                                      BTUuid_t * pxUuid,
                                      BTCharPermissions_t xPermissions );
static BTStatus_t prvBTStartService( uint8_t ucServerIf,
                                     uint16_t usServiceHandle,
                                     BTTransport_t xTransport );
static BTStatus_t prvBTStopService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle );
static BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                                      uint16_t usServiceHandle );
static BTStatus_t prvBTSendIndication( uint8_t ucServerIf,
                                       uint16_t usAttributeHandle,
                                       uint16_t usConnId,
                                       size_t xLen,
                                       uint8_t * pucValue,
                                       bool bConfirm );
static BTStatus_t prvBTSendResponse( uint16_t usConnId,
                                     uint32_t ulTransId,
                                     BTStatus_t xStatus,
                                     BTGattResponse_t * pxResponse );
// static BTStatus_t prvBTReconnect( uint8_t ucServerIf,
//                                const BTBdaddr_t * pxBdAddr );
// static BTStatus_t prvBTAddDevicesToWhiteList( uint8_t ucServerIf,
//                                            const BTBdaddr_t * pxBdAddr,
//                                            uint32_t ulNumberOfDevices );
// static BTStatus_t prvBTRemoveDevicesFromWhiteList( uint8_t ucServerIf,
//                                                 const BTBdaddr_t * pxBdAddr,
//                                                 uint32_t ulNumberOfDevices );
// static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
//                                   uint16_t usMtu );
BTStatus_t prvUnRegisterGattInterface(wiced_bool_t* peRegistrationStatus);
BTStatus_t prvRegisterGattInterface(wiced_bool_t* peRegistrationStatus);





static BTGattServerInterface_t xGATTserverInterface =
{
    .pxRegisterServer     = prvBTRegisterServer,
    .pxUnregisterServer   = prvBTUnregisterServer,
    .pxGattServerInit     = prvBTGattServerInit,
    .pxConnect            = prvBTConnect,
    .pxDisconnect         = prvBTDisconnect,
    .pxAddServiceBlob     = prvBTAddServiceBlob,
    .pxAddService         = prvBTAddService,
    .pxAddIncludedService = prvBTAddIncludedService,
    .pxAddCharacteristic  = prvBTAddCharacteristic,
    .pxSetVal             = prvBTSetVal,
    .pxAddDescriptor      = prvBTAddDescriptor,
    .pxStartService       = prvBTStartService,
    .pxStopService        = prvBTStopService,
    .pxDeleteService      = prvBTDeleteService,
    .pxSendIndication     = prvBTSendIndication,
    .pxSendResponse       = prvBTSendResponse,
    // .pxReconnect          = prvBTReconnect,
    // .pxAddDevicesToWhiteList      = prvBTAddDevicesToWhiteList,
    // .pxRemoveDevicesFromWhiteList = prvBTRemoveDevicesFromWhiteList,
    // .pxConfigureMtu               = prvBTConfigureMtu
};

/*-----------------------------------------------------------*/
BTStatus_t prvConvertWicedGattErrorToAfr (wiced_bt_gatt_status_t xWicedErrorCode)
{
    BTStatus_t xAfrStatus;

    switch(xWicedErrorCode)
    {
        case WICED_BT_GATT_SUCCESS:
            xAfrStatus = eBTStatusSuccess;
        break;

        case WICED_BT_GATT_INVALID_HANDLE :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BT_GATT_INVALID_PDU :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BT_GATT_INSUF_AUTHENTICATION :
            xAfrStatus = eBTStatusAuthFailure;
        break;

        case WICED_BT_GATT_INVALID_OFFSET :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BT_GATT_INSUF_AUTHORIZATION :
            xAfrStatus = eBTStatusAuthFailure;
        break;

        case WICED_BT_GATT_INVALID_ATTR_LEN :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BT_GATT_INSUF_ENCRYPTION :
            xAfrStatus = eBTStatusAuthFailure;
        break;

        case WICED_BT_GATT_INSUF_RESOURCE :
            xAfrStatus = eBTStatusNoMem;
        break;

        case WICED_BT_GATT_ILLEGAL_PARAMETER :
            xAfrStatus = eBTStatusParamInvalid;
        break;

        case WICED_BT_GATT_NO_RESOURCES :
            xAfrStatus = eBTStatusNoMem;
        break;

        case WICED_BT_GATT_BUSY :
            xAfrStatus = eBTStatusBusy;
        break;

        case WICED_BT_GATT_PENDING :
            xAfrStatus = eBTStatusBusy;
        break;

        case WICED_BT_GATT_AUTH_FAIL :
            xAfrStatus = eBTStatusAuthFailure;
        break;

        case WICED_BT_GATT_READ_NOT_PERMIT :
        case WICED_BT_GATT_WRITE_NOT_PERMIT :
        case WICED_BT_GATT_REQ_NOT_SUPPORTED :
        case WICED_BT_GATT_PREPARE_Q_FULL :
        case WICED_BT_GATT_NOT_FOUND :
        case WICED_BT_GATT_NOT_LONG :
        case WICED_BT_GATT_INSUF_KEY_SIZE :
        case WICED_BT_GATT_ERR_UNLIKELY :
        case WICED_BT_GATT_UNSUPPORT_GRP_TYPE :
        case WICED_BT_GATT_INTERNAL_ERROR :
        case WICED_BT_GATT_WRONG_STATE :
        case WICED_BT_GATT_DB_FULL :
        case WICED_BT_GATT_ERROR :
        case WICED_BT_GATT_CMD_STARTED :
        case WICED_BT_GATT_MORE :
        case WICED_BT_GATT_INVALID_CFG :
        case WICED_BT_GATT_SERVICE_STARTED :
        case WICED_BT_GATT_ENCRYPED_NO_MITM :
        case WICED_BT_GATT_NOT_ENCRYPTED :
        case WICED_BT_GATT_CONGESTED :
        case WICED_BT_GATT_CCC_CFG_ERR :
        case WICED_BT_GATT_PRC_IN_PROGRESS :
        case WICED_BT_GATT_OUT_OF_RANGE :
        default:
            xAfrStatus = eBTStatusFail;
        break;
    }

    return xAfrStatus;
}

static void prvConvertUuidToWICED( BTUuid_t *xUuidSrc, wiced_bt_uuid_t *xUuidDst )
{
    if ( eBTuuidType16 == xUuidSrc->ucType )
    {
        xUuidDst->len = 2;
        xUuidDst->uu.uuid16 = xUuidSrc->uu.uu16;
    }
    else if ( eBTuuidType32 == xUuidSrc->ucType )
    {
        xUuidDst->len = 4;
        xUuidDst->uu.uuid32 = xUuidSrc->uu.uu32;
    }
    else
    {
        xUuidDst->len = 16;
        memcpy( xUuidDst->uu.uuid128, xUuidSrc->uu.uu128, 16 );
    }
}

wiced_bt_gatt_status_t prvGattOperationCompleteCb( wiced_bt_gatt_operation_complete_t *p_data )
{
    switch ( p_data->op )
    {
        case GATTC_OPTYPE_WRITE:
            printf("HANDLE ME!!!!\n");
            break;

        case GATTC_OPTYPE_CONFIG:
            printf("HANDLE ME!!!!\n");
            break;

        case GATTC_OPTYPE_NOTIFICATION:
            printf("HANDLE ME!!!!\n");
            break;

        case GATTC_OPTYPE_READ:
        case GATTC_OPTYPE_INDICATION:
            printf("HANDLE ME!!!!\n");
            break;
    }
    return WICED_BT_GATT_SUCCESS;
}

 wiced_result_t prvGattsReadReqHandler(void* arg )
 {
   wiced_bt_gatt_attribute_request_t *p_data =
       (wiced_bt_gatt_attribute_request_t *)arg;

   //@TODO: check if 0 is ok for the transaction ID
   if (xGattServerCb.pxRequestReadCb)
     xGattServerCb.pxRequestReadCb(p_data->conn_id, 0, &xRemoteAddr,
                                   p_data->data.read_req.handle,
                                   p_data->data.read_req.offset);

    vPortFree(arg);

   return WICED_SUCCESS;
 }

 wiced_result_t prvGattsWriteReqHandler(void* arg )
 {
    wiced_bt_gatt_attribute_request_t *p_data =
       (wiced_bt_gatt_attribute_request_t *)arg;
    uint8_t *write_data_ptr = (uint8_t *)(p_data+1);

    //@TODO: check if 0 is ok for the transaction ID
    if ( xGattServerCb.pxRequestWriteCb )
        xGattServerCb.pxRequestWriteCb( p_data->conn_id, 0, &xRemoteAddr, p_data->data.write_req.handle, p_data->data.write_req.offset,
                p_data->data.write_req.val_len, 1, p_data->data.write_req.is_prep, write_data_ptr );

    vPortFree(arg);

   return WICED_SUCCESS;
 }

 wiced_result_t prvGattsMtuReqHandler(void* arg )
 {
   wiced_bt_gatt_attribute_request_t *p_data =
       (wiced_bt_gatt_attribute_request_t *)arg;

    if ( xGattServerCb.pxMtuChangedCb )
        xGattServerCb.pxMtuChangedCb( p_data->conn_id, p_data->data.mtu );

    vPortFree(arg);

   return WICED_SUCCESS;
 }

wiced_bt_gatt_status_t prvGattsReqCb( wiced_bt_gatt_attribute_request_t *p_data )
{
    wiced_result_t result = WICED_BT_GATT_INVALID_PDU;
    wiced_bt_gatt_attribute_request_t *data_ptr = NULL;
    wiced_bt_gatt_attribute_request_t *write_data_ptr = NULL;

    switch ( p_data->request_type )
    {
        case GATTS_REQ_TYPE_READ:
          data_ptr = (wiced_bt_gatt_attribute_request_t*)pvPortMalloc(sizeof(wiced_bt_gatt_attribute_request_t));
          memcpy(data_ptr, p_data, sizeof(wiced_bt_gatt_attribute_request_t));
          wiced_rtos_send_asynchronous_event(&btStack_worker_thread, prvGattsReadReqHandler, (void*)data_ptr);
          result = WICED_BT_GATT_PENDING;
          break;

        case GATTS_REQ_TYPE_WRITE_REQ:
        case GATTS_REQ_TYPE_PREP_WRITE:
          data_ptr = (wiced_bt_gatt_attribute_request_t*)pvPortMalloc(sizeof(wiced_bt_gatt_attribute_request_t) + p_data->data.write_req.val_len);
          write_data_ptr = data_ptr + 1;
          memcpy(data_ptr, p_data, sizeof(wiced_bt_gatt_attribute_request_t));
          memcpy(write_data_ptr, p_data->data.write_req.p_val, p_data->data.write_req.val_len);

          wiced_rtos_send_asynchronous_event(&btStack_worker_thread, prvGattsWriteReqHandler, (void*)data_ptr);
            /* Indicate no auto response for Write request by returning WICED_BT_GATT_PENDING */
            result = WICED_BT_GATT_PENDING;
            break;

        case GATTS_REQ_TYPE_WRITE_CMD:
            //@TODO: check if 0 is ok for the transaction ID
            if ( xGattServerCb.pxRequestWriteCb )
                xGattServerCb.pxRequestWriteCb( p_data->conn_id, 0, &xRemoteAddr, p_data->data.write_req.handle, p_data->data.write_req.offset,
                        p_data->data.write_req.val_len, 0, p_data->data.write_req.is_prep, p_data->data.write_req.p_val );
            /* Indicate no auto response for Write request by returning WICED_BT_GATT_PENDING */
            result = WICED_BT_GATT_PENDING;
            break;

        case GATTS_REQ_TYPE_WRITE_EXEC:
            if ( xGattServerCb.pxRequestExecWriteCb )
                xGattServerCb.pxRequestExecWriteCb( p_data->conn_id, 0, &xRemoteAddr, p_data->data.exec_write );
            return WICED_BT_GATT_PENDING;
            break;

        case GATTS_REQ_TYPE_MTU:
            data_ptr = (wiced_bt_gatt_attribute_request_t*)pvPortMalloc(sizeof(wiced_bt_gatt_attribute_request_t));
            memcpy(data_ptr, p_data, sizeof(wiced_bt_gatt_attribute_request_t));
            wiced_rtos_send_asynchronous_event(&btStack_worker_thread, prvGattsMtuReqHandler, (void*)data_ptr);
            break;

        case GATTS_REQ_TYPE_CONF:
            //@TODO: Should we compare with "Handle Value Indication" handle before giving eBTStatusSuccess ???
            if ( xGattServerCb.pxIndicationSentCb )
                xGattServerCb.pxIndicationSentCb( p_data->conn_id, eBTStatusSuccess);
            break;

        default:
            break;
    }

    return result;
}

wiced_bt_gatt_status_t prvGattsCb( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data )
{
    wiced_bt_gatt_status_t result = WICED_BT_GATT_INVALID_PDU;

    switch(event)
    {
    case GATT_CONNECTION_STATUS_EVT:

        memcpy( xRemoteAddr.ucAddress, p_data->connection_status.bd_addr, BD_ADDR_LEN );

        if(xGattServerCb.pxConnectionCb)
            xGattServerCb.pxConnectionCb(p_data->connection_status.conn_id, 0, p_data->connection_status.connected,
                    &xRemoteAddr);
        break;

    case GATT_OPERATION_CPLT_EVT:
        result = prvGattOperationCompleteCb(&p_data->operation_complete);
        break;

    case GATT_DISCOVERY_RESULT_EVT:
        printf("HANDLE ME!!!!\n");
        break;

    case GATT_DISCOVERY_CPLT_EVT:
        printf("HANDLE ME!!!!\n");
        break;

    case GATT_ATTRIBUTE_REQUEST_EVT:
        result = prvGattsReqCb(&p_data->attribute_request);
        break;

    case GATT_CONGESTION_EVT:
        if(xGattServerCb.pxConnectionCb)
            xGattServerCb.pxCongestionCb(p_data->congestion.conn_id, p_data->congestion.congested);
        break;

    default:
        break;
    }

    return result;
}

BTStatus_t prvBTRegisterServer( BTUuid_t * pxUuid )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = prvRegisterGattInterface(&bServerRegistered);

    //@TODO: Should we support multiple GATT interfaces ??
    if ( xGattServerCb.pxRegisterServerCb )
        xGattServerCb.pxRegisterServerCb( xStatus, 0, pxUuid );

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xStatus = prvUnRegisterGattInterface(&bServerRegistered);

    if ( xGattServerCb.pxUnregisterServerCb )
        xGattServerCb.pxUnregisterServerCb( xStatus, ucServerIf );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    usHandle = 1;

    if( pxCallbacks != NULL )
    {
        xGattServerCb = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConnect( uint8_t ucServerIf,
                         const BTBdaddr_t * pxBdAddr,
                         bool bIsDirect,
                         BTTransport_t xTransport )
{
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    wiced_bt_device_address_t xBdAddr;

    if(bIsDirect)
    {
        memcpy(xBdAddr, pxBdAddr->ucAddress, btADDRESS_LEN);

        /* start directed adv to the received addr */
        xWICEDStatus = wiced_bt_start_advertisements(BTM_BLE_ADVERT_DIRECTED_HIGH, BLE_ADDR_RANDOM, (wiced_bt_device_address_ptr_t)&xBdAddr);
        return prvConvertWicedErrorToAfr(xWICEDStatus);
    }
    else
    {
        return eBTStatusUnsupported;
    }
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDisconnect( uint8_t ucServerIf,
                            const BTBdaddr_t * pxBdAddr,
                            uint16_t usConnId )
{
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;

    if(0 == usConnId)
    {
        xWICEDStatus = wiced_bt_start_advertisements(BTM_BLE_ADVERT_OFF, 0, NULL);
        return prvConvertWicedErrorToAfr(xWICEDStatus);
    }
    else
    {
        xWICEDStatus = wiced_bt_gatt_disconnect(usConnId);
        return prvConvertWicedGattErrorToAfr(xWICEDStatus);
    }
}

/*-----------------------------------------------------------*/

static BTStatus_t prvBTAddServiceBlob( uint8_t ucServerIf, BTService_t * pxService )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddService( uint8_t ucServerIf,
                            BTGattSrvcId_t * pxSrvcId,
                            uint16_t usNumHandles )
{
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_uuid_t xUuid;
    uint8_t usHandleCache;
    wiced_result_t xWicedStatus = WICED_BT_GATT_ERROR;

    prvConvertUuidToWICED( &pxSrvcId->xId.xUuid, &xUuid );

    xWicedStatus = wiced_bt_dynamic_gattdb_add_service(usNumHandles, &usHandleCache, &xUuid, (( pxSrvcId->xServiceType == eBTServiceTypePrimary ) ? 1 : 0));
    xStatus = prvConvertWicedErrorToAfr(xWicedStatus);

    if ( xGattServerCb.pxServiceAddedCb )
        xGattServerCb.pxServiceAddedCb( xStatus, ucServerIf, pxSrvcId, usHandleCache);

    return xStatus;
}

/*-----------------------------------------------------------*/

extern uint8_t legattdb_getUUID(uint16_t usHandle, uint8_t *UUIDBuf);
extern uint8_t legattdb_getGroupEndHandle(uint16_t usHandle);

uint16_t prvGetUUID( uint16_t usServiceHandle, uint8_t* aucServiceUUID )
{
    return legattdb_getUUID( usServiceHandle, aucServiceUUID );
}

uint16_t prvGetEndHandle( uint16_t usServiceHandle )
{
    return legattdb_getGroupEndHandle( usServiceHandle );
}

BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    uint16_t usIncludedHandle )
{

    BTStatus_t xStatus = eBTStatusFail;
    uint8_t usHandleCache;
    uint16_t usIncludedServiceAttributeHandle, usEndGroupHandle;
    wiced_bt_uuid_t xServiceUUID;

    usIncludedServiceAttributeHandle = usIncludedHandle;

    if(WICED_SUCCESS != wiced_bt_dynamic_gattdb_util_get_UUID(usIncludedHandle, &xServiceUUID))
    {
        return eBTStatusParamInvalid;
    }

    usEndGroupHandle = wiced_bt_dynamic_gattdb_util_get_service_end_handle(usIncludedHandle);
    if( 0 == usEndGroupHandle )
    {
        return eBTStatusParamInvalid;
    }

    if ( WICED_SUCCESS == wiced_bt_dynamic_gattdb_add_included_service( usServiceHandle, &usHandleCache, usIncludedServiceAttributeHandle, usEndGroupHandle, &xServiceUUID ) )
    {
        xStatus = eBTStatusSuccess;
    }

    if ( xGattServerCb.pxIncludedServiceAddedCb )
        xGattServerCb.pxIncludedServiceAddedCb( xStatus, ucServerIf, usServiceHandle, usIncludedHandle);

    return xStatus;
}

/*-----------------------------------------------------------*/

uint8_t prvConvertPermissionsToWICED( BTCharPermissions_t xPermissions )
{
    uint8_t ucWICEDPermissions = 0;

    ucWICEDPermissions |= ( xPermissions & eBTPermRead ) ? LEGATTDB_PERM_READABLE : 0 ;
    ucWICEDPermissions |= ( xPermissions & eBTPermReadEncrypted ) ? LEGATTDB_PERM_READABLE : 0 ; // @TODO: Not supported by WICED
    ucWICEDPermissions |= ( xPermissions & eBTPermReadEncryptedMitm ) ? (LEGATTDB_PERM_AUTH_READABLE | LEGATTDB_PERM_READABLE) : 0 ;
    ucWICEDPermissions |= ( xPermissions & eBTPermWrite ) ? (LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ | LEGATTDB_PERM_RELIABLE_WRITE) : 0 ;
    ucWICEDPermissions |= ( xPermissions & eBTPermWriteEncrypted ) ? (LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ | LEGATTDB_PERM_RELIABLE_WRITE) : 0 ; // @TODO: Not supported by WICED
    ucWICEDPermissions |= ( xPermissions & eBTPermWriteEncryptedMitm ) ? (LEGATTDB_PERM_WRITABLE | LEGATTDB_PERM_RELIABLE_WRITE) : 0 ;
    ucWICEDPermissions |= ( xPermissions & eBTPermWriteSigned ) ? (LEGATTDB_PERM_WRITE_CMD | LEGATTDB_PERM_WRITE_REQ | LEGATTDB_PERM_RELIABLE_WRITE) : 0 ; //@TODO: Is this the correct mapping?
    ucWICEDPermissions |= ( xPermissions & eBTPermWriteSignedMitm ) ? (LEGATTDB_PERM_WRITABLE | LEGATTDB_PERM_RELIABLE_WRITE) : 0 ; //@TODO: Is this the correct mapping?

    return ucWICEDPermissions;
}

BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * pxUuid,
                                   BTCharProperties_t xProperties,
                                   BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_uuid_t xUuid;
    uint8_t usHandleCache;

    prvConvertUuidToWICED( pxUuid, &xUuid );

    if ( WICED_SUCCESS == wiced_bt_dynamic_gattdb_add_characteristic( usServiceHandle, &usHandleCache, &xUuid,
            xProperties, prvConvertPermissionsToWICED ( xPermissions ) ) )
    {
        xStatus = eBTStatusSuccess;
    }

    /* Updated the global attribute handle before calling the application callback.
     * This will prevent failures when prvBTAddCharacteristic is called from application callback context */
    if ( xGattServerCb.pxCharacteristicAddedCb )
        xGattServerCb.pxCharacteristicAddedCb( xStatus, ucServerIf, pxUuid, usServiceHandle, usHandleCache);

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                               uint16_t usServiceHandle,
                               BTUuid_t * pxUuid,
                               BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_uuid_t xUuid;
    uint8_t usHandleCache;

    prvConvertUuidToWICED( pxUuid, &xUuid );
    if ( WICED_SUCCESS == wiced_bt_dynamic_gattdb_add_descriptor( usServiceHandle, &usHandleCache, &xUuid, prvConvertPermissionsToWICED( xPermissions) ))
    {
        xStatus = eBTStatusSuccess;
    }

    /* Updated the global attribute handle before calling the application callback.
     * This will prevent failures when prvBTAddDescriptor is called from application callback context */
    if ( xGattServerCb.pxDescriptorAddedCb )
        xGattServerCb.pxDescriptorAddedCb( xStatus, ucServerIf, pxUuid, usServiceHandle, usHandleCache);

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;

    xWICEDStatus = wiced_bt_dynamic_gattdb_start_service( usServiceHandle );
    xStatus = prvConvertWicedErrorToAfr(xWICEDStatus);

    if ( xGattServerCb.pxServiceStartedCb )
    {
        xGattServerCb.pxServiceStartedCb( xStatus, ucServerIf, usServiceHandle );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStopService( uint8_t ucServerIf,
                             uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    if ( xGattServerCb.pxServiceStoppedCb )
    {
        xGattServerCb.pxServiceStoppedCb(xStatus,ucServerIf,usServiceHandle);
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                               uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    if ( xGattServerCb.pxServiceDeletedCb )
    {
        xGattServerCb.pxServiceDeletedCb(xStatus,ucServerIf,usServiceHandle);
    }
    return xStatus;
}

/*-----------------------------------------------------------*/
 wiced_result_t prvGattsIndCbHandler(void* arg )
 {
   uint16_t *scratch_buff = (uint16_t *)arg;

    /* Trigger callbcak here for notification packets only */
    if (xGattServerCb.pxIndicationSentCb)
    {
        xGattServerCb.pxIndicationSentCb(scratch_buff[0], scratch_buff[1]);
    }

    vPortFree(arg);

   return WICED_SUCCESS;
 }

BTStatus_t prvBTSendIndication( uint8_t ucServerIf,
                                uint16_t usAttributeHandle,
                                uint16_t usConnId,
                                size_t xLen,
                                uint8_t * pucValue,
                                bool bConfirm )
{
    BTStatus_t xStatus = eBTStatusFail;
    wiced_bt_gatt_status_t xWicedStatus = WICED_BT_GATT_ERROR;
    uint16_t *scratch_buff = (uint16_t*)pvPortMalloc(2*sizeof(uint16_t));

    if ( bConfirm )
    {
        xWicedStatus = wiced_bt_gatt_send_indication( usConnId, usAttributeHandle, xLen, pucValue );
    }
    else
    {
        xWicedStatus = wiced_bt_gatt_send_notification( usConnId, usAttributeHandle, xLen, pucValue );
    }

    xStatus = prvConvertWicedGattErrorToAfr( xWicedStatus );

    scratch_buff[0] = usConnId;
    scratch_buff[1] = xStatus;

    wiced_rtos_send_asynchronous_event(&btStack_worker_thread, prvGattsIndCbHandler, (void*)scratch_buff);

    return xStatus;
}

/*-----------------------------------------------------------*/
 wiced_result_t prvGattsRspCbHandler(void* arg )
 {
   uint16_t *scratch_buff = (uint16_t *)arg;

    /* Trigger callbcak here for notification packets only */
    if ( xGattServerCb.pxResponseConfirmationCb )
    {
        xGattServerCb.pxResponseConfirmationCb(scratch_buff[0], scratch_buff[1]);
    }

    vPortFree(arg);

   return WICED_SUCCESS;
 }

BTStatus_t prvBTSendResponse( uint16_t usConnId,
                              uint32_t ulTransId,
                              BTStatus_t xStatus,
                              BTGattResponse_t * pxResponse )
{
    wiced_bt_gatt_status_t xWicedStatus = WICED_BT_GATT_ERROR;
    uint16_t *scratch_buff = (uint16_t*)pvPortMalloc(2*sizeof(uint16_t));

    if ( eBTStatusSuccess == xStatus )
    {
          xWicedStatus = wiced_bt_gatt_send_response(
              WICED_BT_GATT_SUCCESS, usConnId, pxResponse->usHandle,
              pxResponse->xAttrValue.xLen, pxResponse->xAttrValue.usOffset,
              pxResponse->xAttrValue.pucValue);

        xStatus = prvConvertWicedGattErrorToAfr(xWicedStatus);
    }

    scratch_buff[0] = xStatus;
    scratch_buff[1] = pxResponse->usHandle;

    wiced_rtos_send_asynchronous_event(&btStack_worker_thread, prvGattsRspCbHandler, (void*)scratch_buff);

    return xStatus;

}

/*-----------------------------------------------------------*/
#if 0
static BTStatus_t prvBTReconnect( uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr )
{
    wiced_bt_dev_status_t xWICEDStatus = WICED_SUCCESS;
    wiced_bt_device_address_t xBdAddr;

    memcpy(xBdAddr, pxBdAddr->ucAddress, btADDRESS_LEN);

    /* start directed adv to the received addr */
    xWICEDStatus = wiced_bt_start_advertisements(BTM_BLE_ADVERT_DIRECTED_HIGH, BLE_ADDR_RANDOM, (wiced_bt_device_address_ptr_t)&xBdAddr);

    return prvConvertWicedErrorToAfr(xWICEDStatus);
}

/*-----------------------------------------------------------*/

static BTStatus_t prvBTAddDevicesToWhiteList( uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, uint32_t ulNumberOfDevices )
{

    if( ( 0 != ulNumberOfDevices ) && ( NULL != pxBdAddr) )
        wiced_btm_ble_update_advertisement_filter_policy(BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ);
    else
        return eBTStatusParamInvalid;

    while( ulNumberOfDevices-- )
    {
        if(!wiced_bt_ble_update_advertising_white_list(WICED_TRUE, *(wiced_bt_device_address_t* )pxBdAddr))
        {
            return eBTStatusFail;
        }

        pxBdAddr++;
    }

    if( xGattServerCb.pxBTWhiteListChangedCb )
        xGattServerCb.pxBTWhiteListChangedCb( pxBdAddr, true );

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

static BTStatus_t prvBTRemoveDevicesFromWhiteList( uint8_t ucServerIf, const BTBdaddr_t * pxBdAddr, uint32_t ulNumberOfDevices )
{

    if( ( 0 == ulNumberOfDevices ) || ( NULL == pxBdAddr) )
        return eBTStatusParamInvalid;

    while( ulNumberOfDevices-- )
    {
        if(!wiced_bt_ble_update_advertising_white_list(WICED_FALSE, *(wiced_bt_device_address_t* )pxBdAddr))
        {
            return eBTStatusFail;
        }

        pxBdAddr++;
    }

    /* check if the white list is empty, if so update the filter policy */
    if(0 == wiced_bt_ble_get_white_list_size())
    {
        wiced_btm_ble_update_advertisement_filter_policy(BTM_BLE_ADVERT_FILTER_ALL_CONNECTION_REQ_ALL_SCAN_REQ);
    }

    if( xGattServerCb.pxBTWhiteListChangedCb )
        xGattServerCb.pxBTWhiteListChangedCb( pxBdAddr, false );

    return eBTStatusSuccess;
}

/*-----------------------------------------------------------*/

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf, uint16_t usMtu )
{
    return eBTStatusUnsupported;
}
#endif
/*-----------------------------------------------------------*/

const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}

/*-----------------------------------------------------------*/
BTStatus_t prvRegisterGattInterface(wiced_bool_t* peRegistrationStatus)
{
    BTStatus_t xStatus = eBTStatusSuccess;
    wiced_bt_gatt_status_t xWicedStatus = WICED_BT_GATT_SUCCESS;

    /* Note - Client and server register/unregister are called from the same application thread on Amazon Baton.
     * If that's not true for other designs, then we need to use a Mutex for below code block for synchronization */

    /* Prevent multiple registration */
    if(*peRegistrationStatus == WICED_FALSE)
    {
        /* Prevent WICED gatt interface registration if someone else (other GATT role for example)
         * has already registered the callback */
        if(ucGattRegistry == 0)
        {
            /* Register with stack to receive GATT callback */
            xWicedStatus = wiced_bt_gatt_register( prvGattsCb );
            xStatus = prvConvertWicedGattErrorToAfr(xWicedStatus);
        }

        if(xWicedStatus == WICED_BT_GATT_SUCCESS)
        {
            *peRegistrationStatus = WICED_TRUE;
            ucGattRegistry++;
        }
    }
    else
    {
        xStatus = eBTStatusFail;
    }
    return xStatus;
}

/*-----------------------------------------------------------*/
BTStatus_t prvUnRegisterGattInterface(wiced_bool_t* peRegistrationStatus)
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Note - Client and server register/unregister are called from the same application thread on Amazon Baton.
     * If that's not true for other designs, then we need to use a Mutex for below code block for synchronization */

    /* Allow unregister only if the caller has already registered */
    if(*peRegistrationStatus == WICED_TRUE)
    {
        /* WICED GATT Unregister on the last call */
        if(ucGattRegistry == 1)
        {
            wiced_bt_gatt_deregister();
        }

        if(ucGattRegistry != 0)
        {
            ucGattRegistry--;
        }
        *peRegistrationStatus = WICED_FALSE;
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/
BTStatus_t CleanupGattServer(void)
{
    return prvBTUnregisterServer(0);
}

