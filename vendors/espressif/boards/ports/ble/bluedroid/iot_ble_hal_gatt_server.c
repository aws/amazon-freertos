/*
* FreeRTOS
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_ble_hal_gatt_server.c
 * @brief Hardware Abstraction Layer for GATT server ble stack.
 */
#include "iot_config.h"
#include <string.h>
#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

#define APP_ID    0

static BTGattServerCallbacks_t xGattServerCb;
static BTUuid_t xServerUuidCb;

uint32_t ulGattServerIFhandle;
bool bInterfaceCreated = false;


static BTStatus_t prvBTRegisterServer( BTUuid_t * pxUuid );
static BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf );
static BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks );
static BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                                     BTService_t * pxService );
static BTStatus_t prvBTConnect( uint8_t ucServerIf,
                                const BTBdaddr_t * pxBdAddr,
                                bool bIsDirect,
                                BTTransport_t xTransport );
static BTStatus_t prvBTDisconnect( uint8_t ucServerIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );
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

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
                                     uint16_t usMtu );

static BTGattServerInterface_t xGATTserverInterface =
{
    .pxRegisterServer     = prvBTRegisterServer,
    .pxUnregisterServer   = prvBTUnregisterServer,
    .pxGattServerInit     = prvBTGattServerInit,
    .pxConnect            = prvBTConnect,
    .pxDisconnect         = prvBTDisconnect,
    .pxAddServiceBlob     = prvAddServiceBlob,
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
    .pxConfigureMtu       = prvBTConfigureMtu
};

/*-----------------------------------------------------------*/
uint16_t prvGetUUIDlen( BTUuid_t * pxUuid )
{
    uint16_t usLen;

    switch( pxUuid->ucType )
    {
        case eBTuuidType16:
            usLen = ESP_UUID_LEN_16;
            break;

        case eBTuuidType32:
            usLen = ESP_UUID_LEN_32;
            break;

        case eBTuuidType128:
        default:
            usLen = ESP_UUID_LEN_128;
            break;
    }

    return usLen;
}


void prvCopytoESPUUID( esp_bt_uuid_t * pxESPuuid,
                       BTUuid_t * pxUuid )
{
    switch( pxUuid->ucType )
    {
        case eBTuuidType16:
            pxESPuuid->len = ESP_UUID_LEN_16;
            pxESPuuid->uuid.uuid16 = pxUuid->uu.uu16;
            break;

        case eBTuuidType32:
            pxESPuuid->len = ESP_UUID_LEN_32;
            pxESPuuid->uuid.uuid32 = pxUuid->uu.uu32;
            break;

        case eBTuuidType128:
        default:
            pxESPuuid->len = ESP_UUID_LEN_128;
            memcpy( pxESPuuid->uuid.uuid128, pxUuid->uu.uu128, ESP_UUID_LEN_128 );
            break;
    }
}

/*-----------------------------------------------------------*/

void prvCopyFromESPUUI( BTUuid_t * pxUuid,
                        esp_bt_uuid_t * pxESPuuid )
{
    switch( pxESPuuid->len )
    {
        case ESP_UUID_LEN_16:
            pxUuid->ucType = eBTuuidType16;
            pxUuid->uu.uu16 = pxESPuuid->uuid.uuid16;
            break;

        case ESP_UUID_LEN_32:
            pxUuid->ucType = eBTuuidType32;
            pxUuid->uu.uu32 = pxESPuuid->uuid.uuid32;
            break;

        case ESP_UUID_LEN_128:
        default:
            pxUuid->ucType = eBTuuidType128;
            memcpy( pxUuid->uu.uu128, pxESPuuid->uuid.uuid128, bt128BIT_UUID_LEN );
            break;
    }
}

/*-----------------------------------------------------------*/

void prvGATTeventHandler( esp_gatts_cb_event_t event,
                          esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t * param )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTUuid_t xUuid;
    BTGattSrvcId_t xGattSrvcId;

    switch( event )
    {
        case ESP_GATTS_REG_EVT:
            ulGattServerIFhandle = gatts_if;

            if( xGattServerCb.pxRegisterServerCb != NULL )
            {
                if( param->reg.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                    bInterfaceCreated = false;
                }
                else
                {
                    bInterfaceCreated = true;
                }

                xGattServerCb.pxRegisterServerCb( xStatus, ulGattServerIFhandle, &xServerUuidCb );
            }

            break;

        case ESP_GATTS_CREATE_EVT:

            if( xGattServerCb.pxServiceAddedCb != NULL )
            {
                if( param->create.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattSrvcId.xId.ucInstId = param->create.service_id.id.inst_id;
                prvCopyFromESPUUI( &xGattSrvcId.xId.xUuid, &param->create.service_id.id.uuid );

                if( param->create.service_id.is_primary == true )
                {
                    xGattSrvcId.xServiceType = eBTServiceTypePrimary;
                }
                else
                {
                    xGattSrvcId.xServiceType = eBTServiceTypeSecondary;
                }

                xGattServerCb.pxServiceAddedCb( xStatus, ulGattServerIFhandle, &xGattSrvcId, param->create.service_handle );
            }

            break;

        case ESP_GATTS_ADD_INCL_SRVC_EVT:

            if( xGattServerCb.pxIncludedServiceAddedCb != NULL )
            {
                if( param->add_incl_srvc.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxIncludedServiceAddedCb( xStatus, ulGattServerIFhandle, param->add_incl_srvc.service_handle, param->add_incl_srvc.attr_handle );
            }

            break;

        case ESP_GATTS_ADD_CHAR_EVT:

            if( xGattServerCb.pxCharacteristicAddedCb != NULL )
            {
                if( param->add_char.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                prvCopyFromESPUUI( &xUuid, &param->add_char.char_uuid );
                xGattServerCb.pxCharacteristicAddedCb( xStatus, ulGattServerIFhandle, &xUuid, param->add_char.service_handle, param->add_char.attr_handle );
            }

            break;

        case ESP_GATTS_ADD_CHAR_DESCR_EVT:

            if( xGattServerCb.pxDescriptorAddedCb != NULL )
            {
                if( param->add_char_descr.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                prvCopyFromESPUUI( &xUuid, &param->add_char_descr.descr_uuid );
                xGattServerCb.pxDescriptorAddedCb( xStatus, ulGattServerIFhandle, &xUuid, param->add_char_descr.service_handle, param->add_char_descr.attr_handle );
            }

            break;

        case ESP_GATTS_START_EVT:

            if( xGattServerCb.pxServiceStartedCb != NULL )
            {
                if( param->start.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxServiceStartedCb( xStatus, ulGattServerIFhandle, param->start.service_handle );
            }

            break;

        case ESP_GATTS_STOP_EVT:

            if( xGattServerCb.pxServiceStoppedCb != NULL )
            {
                if( param->stop.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxServiceStoppedCb( xStatus, ulGattServerIFhandle, param->stop.service_handle );
            }

            break;

        case ESP_GATTS_DELETE_EVT:

            if( xGattServerCb.pxServiceDeletedCb != NULL )
            {
                if( param->del.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxServiceDeletedCb( xStatus, ulGattServerIFhandle, param->del.service_handle );
            }

            break;

        case ESP_GATTS_CONF_EVT:

            if( xGattServerCb.pxIndicationSentCb != NULL )
            {
                if( param->conf.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxIndicationSentCb( param->conf.conn_id, xStatus );
            }

            break;

        case ESP_GATTS_RESPONSE_EVT:

            if( xGattServerCb.pxResponseConfirmationCb != NULL )
            {
                if( param->rsp.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxResponseConfirmationCb( xStatus, param->rsp.handle );
            }

            break;

        case ESP_GATTS_UNREG_EVT:

            if( xGattServerCb.pxUnregisterServerCb != NULL )
            {
                xGattServerCb.pxUnregisterServerCb( xStatus, ulGattServerIFhandle );
            }

            break;

        case ESP_GATTS_CONNECT_EVT:

            if( xGattServerCb.pxConnectionCb != NULL )
            {
                xGattServerCb.pxConnectionCb( param->connect.conn_id, ulGattServerIFhandle, true, ( BTBdaddr_t * ) &param->connect.remote_bda );
            }

            break;

        case ESP_GATTS_DISCONNECT_EVT:

            if( xGattServerCb.pxConnectionCb != NULL )
            {
                xGattServerCb.pxConnectionCb( param->disconnect.conn_id, ulGattServerIFhandle, false, ( BTBdaddr_t * ) &param->disconnect.remote_bda );
            }

            break;

        case ESP_GATTS_READ_EVT:

            if( xGattServerCb.pxRequestReadCb != NULL )
            {
                xGattServerCb.pxRequestReadCb( param->read.conn_id, param->read.trans_id, ( BTBdaddr_t * ) &param->read.bda, param->read.handle, param->read.offset );
            }

            break;

        case ESP_GATTS_WRITE_EVT:

            if( xGattServerCb.pxRequestWriteCb != NULL )
            {
                xGattServerCb.pxRequestWriteCb( param->write.conn_id, param->write.trans_id, ( BTBdaddr_t * ) &param->write.bda, param->write.handle, param->write.offset, param->write.len, param->write.need_rsp, param->write.is_prep, param->write.value );
            }

            break;

        case ESP_GATTS_EXEC_WRITE_EVT:

            if( xGattServerCb.pxRequestExecWriteCb != NULL )
            {
                xGattServerCb.pxRequestExecWriteCb( param->exec_write.conn_id, param->exec_write.trans_id, ( BTBdaddr_t * ) &param->exec_write.bda, param->exec_write.exec_write_flag );
            }

            break;

        case ESP_GATTS_MTU_EVT:

            if( xGattServerCb.pxMtuChangedCb != NULL )
            {
                xGattServerCb.pxMtuChangedCb( param->mtu.conn_id, param->mtu.mtu );
            }

            break;

        case ESP_GATTS_SET_ATTR_VAL_EVT:

            if( xGattServerCb.pxSetValCallbackCb != NULL )
            {
                if( param->set_attr_val.status != ESP_OK )
                {
                    xStatus = eBTStatusFail;
                }

                xGattServerCb.pxSetValCallbackCb( xStatus, param->set_attr_val.attr_handle );
            }

            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTRegisterServer( BTUuid_t * pxUuid )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTUuid_t * pxAPPuuid;

    if( bInterfaceCreated == false )
    {
        pxAPPuuid = IotBle_Malloc( sizeof( BTUuid_t ) );

        if( pxAPPuuid != NULL )
        {
            memcpy( &xServerUuidCb, pxUuid, sizeof( BTUuid_t ) );

            if( esp_ble_gatts_app_register( APP_ID ) != ESP_OK )
            {
                xStatus = eBTStatusFail;
            }
        }
        else
        {
            xStatus = eBTStatusNoMem;
        }
    }
    else
    {
        xGattServerCb.pxRegisterServerCb( eBTStatusSuccess, ulGattServerIFhandle, pxUuid );
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( bInterfaceCreated == true )
    {
        if( esp_ble_gatts_app_unregister( ulGattServerIFhandle ) != ESP_OK )
        {
            xStatus = eBTStatusFail;
        }
        else
        {
            bInterfaceCreated = false;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;


    if( pxCallbacks != NULL )
    {
        xGattServerCb = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    if( xStatus == eBTStatusSuccess )
    {
        if( esp_ble_gatts_register_callback( prvGATTeventHandler ) != ESP_OK )
        {
            xStatus = eBTStatusFail;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConnect( uint8_t ucServerIf,
                         const BTBdaddr_t * pxBdAddr,
                         bool bIsDirect,
                         BTTransport_t xTransport )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDisconnect( uint8_t ucServerIf,
                            const BTBdaddr_t * pxBdAddr,
                            uint16_t usConnId )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddService( uint8_t ucServerIf,
                            BTGattSrvcId_t * pxSrvcId,
                            uint16_t usNumHandles )
{
    esp_gatt_srvc_id_t xService;
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxSrvcId->xServiceType == eBTServiceTypePrimary )
    {
        xService.is_primary = true;
    }
    else
    {
        xService.is_primary = false;
    }

    xService.id.inst_id = pxSrvcId->xId.ucInstId;

    prvCopytoESPUUID( &xService.id.uuid, &pxSrvcId->xId.xUuid );


    xESPstatus = esp_ble_gatts_create_service( ucServerIf, &xService, usNumHandles );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    uint16_t usIncludedHandle )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_add_included_service( usServiceHandle, usIncludedHandle );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * pxUuid,
                                   BTCharProperties_t xProperties,
                                   BTCharPermissions_t xPermissions )
{
    esp_bt_uuid_t xCharUUID;
    esp_attr_value_t xCharVal;
    esp_attr_control_t xControl;
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    prvCopytoESPUUID( &xCharUUID, pxUuid );

    xControl.auto_rsp = ESP_GATT_RSP_BY_APP;

    xCharVal.attr_len = 0;
    xCharVal.attr_max_len = 0;
    xCharVal.attr_value = NULL;

    xESPstatus = esp_ble_gatts_add_char( usServiceHandle, &xCharUUID, ( esp_gatt_perm_t ) xPermissions, ( esp_gatt_char_prop_t ) xProperties, &xCharVal, &xControl );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_set_attr_value( pxValue->usHandle, pxValue->xAttrValue.xLen, pxValue->xAttrValue.pucValue );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                               uint16_t usServiceHandle,
                               BTUuid_t * pxUuid,
                               BTCharPermissions_t xPermissions )
{
    esp_bt_uuid_t xCharDescrUUID;
    esp_attr_value_t xCharDescrVal;
    esp_attr_control_t xControl;
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    prvCopytoESPUUID( &xCharDescrUUID, pxUuid );

    xCharDescrVal.attr_len = 0;
    xCharDescrVal.attr_max_len = 0;
    xCharDescrVal.attr_value = NULL;

    xControl.auto_rsp = ESP_GATT_RSP_BY_APP;

    xESPstatus = esp_ble_gatts_add_char_descr( usServiceHandle, &xCharDescrUUID, xPermissions, &xCharDescrVal, &xControl );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_start_service( usServiceHandle );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStopService( uint8_t ucServerIf,
                             uint16_t usServiceHandle )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_stop_service( usServiceHandle );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                               uint16_t usServiceHandle )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_delete_service( usServiceHandle );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSendIndication( uint8_t ucServerIf,
                                uint16_t usAttributeHandle,
                                uint16_t usConnId,
                                size_t xLen,
                                uint8_t * pucValue,
                                bool bConfirm )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xStatus = eBTStatusSuccess;

    xESPstatus = esp_ble_gatts_send_indicate( ucServerIf, usConnId, usAttributeHandle, xLen, ( uint8_t * ) pucValue, bConfirm );

    if( xESPstatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSendResponse( uint16_t usConnId,
                              uint32_t ulTransId,
                              BTStatus_t xStatus,
                              BTGattResponse_t * pxResponse )
{
    esp_err_t xESPstatus = ESP_OK;
    BTStatus_t xReturnStatus = eBTStatusSuccess;
    /* Huge array allocate in the stack */

    esp_gatt_rsp_t rsp;

    rsp.handle = pxResponse->usHandle;
    rsp.attr_value.auth_req = pxResponse->xAttrValue.xRspErrorStatus;
    rsp.attr_value.handle = pxResponse->xAttrValue.usHandle;
    rsp.attr_value.len = pxResponse->xAttrValue.xLen;
    rsp.attr_value.offset = pxResponse->xAttrValue.usOffset;

    if( pxResponse->xAttrValue.xLen <= ESP_GATT_MAX_ATTR_LEN )
    {
        memcpy( rsp.attr_value.value, pxResponse->xAttrValue.pucValue, pxResponse->xAttrValue.xLen );

        xESPstatus = esp_ble_gatts_send_response( ulGattServerIFhandle, usConnId, ulTransId, xReturnStatus, &rsp );

        if( xESPstatus != ESP_OK )
        {
            xReturnStatus = eBTStatusFail;
        }
    }
    else
    {
        xReturnStatus = eBTStatusFail;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
                                     uint16_t usMtu )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_err_t xESPStatus;

    xESPStatus = esp_ble_gatt_set_local_mtu( usMtu );

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                              BTService_t * pxService )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/
const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}
