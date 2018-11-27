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

/* Standard Includes. */
#include <string.h>

/* FreeRTOS Includes. */
#include "FreeRTOS.h"

/* AWS BLE Lib Includes. */
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_event_manager.h"
#include "aws_ble_hal_internals.h"

/* Nordic/Segger Includes. */
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "SEGGER_RTT.h"

/* Nordic port Includes. */
#include "aws_ble_hal_dis.h"


uint32_t ulGattServerIFhandle;
uint16_t g_cccd_handle;
bool bInterfaceCreated = false;

static uint32_t ulGATTEvtMngHandle;
static BTGattServerCallbacks_t xGattServerCb;

extern ble_cus_t m_cus;

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

static BTGattServerInterface_t xGATTserverInterface =
{
    .pxRegisterServer     = prvBTRegisterServer,
    .pxUnregisterServer   = prvBTUnregisterServer,
    .pxGattServerInit     = prvBTGattServerInit,
    .pxConnect            = prvBTConnect,
    .pxDisconnect         = prvBTDisconnect,
    .pxAddService         = prvBTAddService,
    .pxAddIncludedService = prvBTAddIncludedService,
    .pxAddCharacteristic  = prvBTAddCharacteristic,
    .pxSetVal             = prvBTSetVal,
    .pxAddDescriptor      = prvBTAddDescriptor,
    .pxStartService       = prvBTStartService,
    .pxStopService        = prvBTStopService,
    .pxDeleteService      = prvBTDeleteService,
    .pxSendIndication     = prvBTSendIndication,
    .pxSendResponse       = prvBTSendResponse
};


/*-----------------------------------------------------------*/

void prvGATTeventHandler( ble_evt_t const * p_ble_evt )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BTUuid_t xUuid;

    switch( p_ble_evt->header.evt_id )
    {
        case BLE_GAP_EVT_CONNECTED:

            /*SEGGER_RTT_printf( 0, "BLE_GAP_EVT_CONNECTED - connection ID -  %d 0x%x \n\r",
                               p_ble_evt->evt.gap_evt.conn_handle,
                               p_ble_evt->evt.gap_evt.conn_handle );*/

            xGattServerCb.pxConnectionCb( p_ble_evt->evt.gap_evt.conn_handle, ulGattServerIFhandle, true, NULL );

            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:

            if( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ )
            {
                /*SEGGER_RTT_printf( 0, "BLE_GATTS_AUTHORIZE_TYPE_READ - handle %d 0x%x\n\r",
                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle,
                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle );*/

                xGattServerCb.pxRequestReadCb( p_ble_evt->evt.gap_evt.conn_handle,
                                               BLE_GATTS_AUTHORIZE_TYPE_READ,
                                               NULL,
                                               p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle,
                                               p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.offset );
            }
            else if( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE )
            {
                /*SEGGER_RTT_printf( 0, "BLE_GATTS_AUTHORIZE_TYPE_WRITE - handle %d 0x%x length %d \n\r",
                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle,
                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle,
                                   p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.len );*/

                xGattServerCb.pxRequestWriteCb( p_ble_evt->evt.gap_evt.conn_handle,
                                                BLE_GATTS_AUTHORIZE_TYPE_WRITE,
                                                NULL,
                                                p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle,
                                                p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.offset,
                                                p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.len,
                                                true,
                                                false,
                                                p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.data );
            }
            else
            {
                SEGGER_RTT_printf( 0, "Error : BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST Invalid \n\r" );
            }

            break;

        case BLE_GATTS_EVT_WRITE:

            /*SEGGER_RTT_printf( 0, "on handle %d hex %x \n\r",
                               p_ble_evt->evt.gatts_evt.params.write.handle,
                               p_ble_evt->evt.gatts_evt.params.write.handle );*/

            xGattServerCb.pxRequestWriteCb( p_ble_evt->evt.gap_evt.conn_handle,
                                            0,
                                            NULL,
                                            p_ble_evt->evt.gatts_evt.params.write.handle,
                                            p_ble_evt->evt.gatts_evt.params.write.offset,
                                            p_ble_evt->evt.gatts_evt.params.write.len,
                                            true,
                                            false,
                                            p_ble_evt->evt.gatts_evt.params.write.data );

            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:

            xGattServerCb.pxIndicationSentCb( p_ble_evt->evt.gap_evt.conn_handle, xStatus );

            break;

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST :

            if( xGattServerCb.pxMtuChangedCb != NULL )
            {
                xGattServerCb.pxMtuChangedCb( p_ble_evt->evt.gap_evt.conn_handle, p_ble_evt->evt.gatts_evt.params.exchange_mtu_request.client_rx_mtu );
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

    xGattServerCb.pxRegisterServerCb( eBTStatusSuccess, ulGattServerIFhandle, pxUuid );
    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( xStatus == eBTStatusSuccess )
    {
        if( pxCallbacks != NULL )
        {
            xGattServerCb = *pxCallbacks;
        }
        else
        {
            xStatus = eBTStatusParamInvalid;
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
    BTStatus_t xStatus = eBTStatusSuccess;

    uint32_t err_code;
    ble_uuid128_t base_uuid;
    ble_add_char_params_t add_char_params;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_char_handles_t p_char_handle;
    ble_add_descr_params_t p_descr;
    uint16_t p_descr_handle;
    ble_uuid_t ble_uuid;


    ble_cus_t * p_cus = &m_cus;


    /* Initialize service structure */
    p_cus->evt_handler = NULL;
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;

    /* Add Service UUID */
    memcpy( &base_uuid.uuid128, &pxSrvcId->xId.xUuid.uu.uu128, 16 );
    err_code = sd_ble_uuid_vs_add( &base_uuid, &p_cus->uuid_type );


    ble_uuid.type = p_cus->uuid_type;
    memcpy( &ble_uuid.uuid, ( ( uint8_t * ) &base_uuid.uuid128 ) + 12, 2 );

    /* Add the Service */
    err_code = sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle );

    if( err_code != NRF_SUCCESS )
    {
        SEGGER_RTT_printf( 0, "Error %d in adding service \n\r", err_code );
        return err_code;
    }

    SEGGER_RTT_printf( 0, "Service Added with handle %d %x \n\r ", p_cus->service_handle );

    xGattServerCb.pxServiceAddedCb( xStatus, ulGattServerIFhandle, pxSrvcId, p_cus->service_handle );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    uint16_t usIncludedHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * pxUuid,
                                   BTCharProperties_t xProperties,
                                   BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ble_add_char_params_t add_char_params;
    ble_uuid128_t base_uuid;
    uint32_t err_code;
    ble_gatts_char_handles_t char_handle;


    memset( &add_char_params, 0, sizeof( add_char_params ) );

    if( xProperties & eBTPropRead )
    {
        add_char_params.char_props.read = 1;
    }

    if( xProperties & eBTPropWriteNoResponse )
    {
        add_char_params.char_props.write_wo_resp = 1;
    }

    if( xProperties & eBTPropWrite )
    {
        add_char_params.char_props.write = 1;
    }

    if( xProperties & eBTPropNotify )
    {
        add_char_params.char_props.notify = 1;
    }

    if( xProperties & eBTPropIndicate )
    {
        add_char_params.char_props.indicate = 1;
    }

    ble_cus_t * p_cus = &m_cus;

    /* Add  UUID */
    memcpy( &base_uuid.uuid128, &pxUuid->uu.uu128, 16 );
    err_code = sd_ble_uuid_vs_add( &base_uuid, &p_cus->uuid_type );

    memcpy( &add_char_params.uuid, ( ( uint8_t * ) &base_uuid.uuid128 ) + 12, 2 );

    if( err_code != 0 )
    {
        SEGGER_RTT_printf( 0, "Error %d in adding uuid to soft \n\r", err_code );
    }

    add_char_params.uuid_type = p_cus->uuid_type;
    add_char_params.max_len = 180;
    add_char_params.init_len = sizeof( uint8_t );
    add_char_params.is_var_len = true;


    add_char_params.is_defered_read = true;
    add_char_params.is_defered_write = true;

    add_char_params.read_access = SEC_MITM;
    add_char_params.write_access = SEC_MITM;
    add_char_params.cccd_write_access = SEC_MITM;

    err_code = characteristic_add( p_cus->service_handle, &add_char_params, &char_handle );

    if( err_code != 0 )
    {
        SEGGER_RTT_printf( 0, "Error %d in adding characteristic \n\r", err_code );
    }

    xGattServerCb.pxCharacteristicAddedCb( xStatus, ulGattServerIFhandle, pxUuid, p_cus->service_handle, char_handle.value_handle );

    SEGGER_RTT_printf( 0, "Charactersitics Added with handle %d %x \n\r", char_handle.value_handle );

    if( char_handle.cccd_handle != 0 )
    {
        g_cccd_handle = char_handle.cccd_handle;
        SEGGER_RTT_printf( 0, "CCCD Added with handle %d %x \n\r", char_handle.cccd_handle );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                               uint16_t usServiceHandle,
                               BTUuid_t * pxUuid,
                               BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xGattServerCb.pxDescriptorAddedCb( xStatus, ulGattServerIFhandle, pxUuid, usServiceHandle, g_cccd_handle );

    g_cccd_handle = 0;


    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    xGattServerCb.pxServiceStartedCb( xStatus, ucServerIf, usServiceHandle );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStopService( uint8_t ucServerIf,
                             uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                               uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

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
    BTStatus_t xStatus = eBTStatusSuccess;
    uint32_t err;
    uint16_t len;

    ble_gatts_hvx_params_t hvx_params;

    memset( &hvx_params, 0, sizeof( hvx_params ) );
    hvx_params.handle = usAttributeHandle;
    hvx_params.p_data = pucValue;
    len = xLen;
    hvx_params.p_len = &len;

    if( bConfirm )
    {
        hvx_params.type = BLE_GATT_HVX_INDICATION;
    }
    else
    {
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    }

    err = sd_ble_gatts_hvx( usConnId, &hvx_params );

    if( err != 0 )
    {
        SEGGER_RTT_printf( 0, "Error %d HVX \n\r", err );
    }
    else
    {
        /*SEGGER_RTT_printf( 0, "prvBTSendIndication - BLE_GATT_HVX_NOTIFICATION length %d \n\r", *(hvx_params.p_len) );*/
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSendResponse( uint16_t usConnId,
                              uint32_t ulTransId,
                              BTStatus_t xStatus,
                              BTGattResponse_t * pxResponse )
{
    BTStatus_t xReturnStatus = eBTStatusSuccess;
    uint32_t err;
    ble_gatts_rw_authorize_reply_params_t authreply;

    ble_gatts_value_t writereply;

    switch( ulTransId )
    {
        case BLE_GATTS_AUTHORIZE_TYPE_READ:
            authreply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;

            authreply.params.read.len = pxResponse->xAttrValue.xLen;
            authreply.params.read.offset = pxResponse->xAttrValue.usOffset;
            authreply.params.read.p_data = pxResponse->xAttrValue.pucValue;

            authreply.params.read.update = 1;
            authreply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;

            err = sd_ble_gatts_rw_authorize_reply( usConnId, &authreply );

            if( err != 0 )
            {
                SEGGER_RTT_printf( 0, "Error %d prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_READ \n\r", err );
            }
            else
            {
                /*SEGGER_RTT_printf( 0, "prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_READ length %d \n\r", authreply.params.read.len );*/
            }

            break;

        case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
            authreply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;

            authreply.params.write.len = pxResponse->xAttrValue.xLen;
            authreply.params.write.offset = pxResponse->xAttrValue.usOffset;
            authreply.params.write.p_data = pxResponse->xAttrValue.pucValue;

            authreply.params.write.update = 1;
            authreply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;

            err = sd_ble_gatts_rw_authorize_reply( usConnId, &authreply );

            if( err != 0 )
            {
                SEGGER_RTT_printf( 0, "Error %d prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_WRITE \n\r", err );
            }
            else
            {
                /*SEGGER_RTT_printf( 0, "prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_WRITE length %d \n\r", authreply.params.write.len );*/
            }

            break;

        case 0:

            /*SEGGER_RTT_printf( 0, "prvBTSendResponse - BLE_GATTS_EVT_WRITE \n\r" );*/

            break;

        default:
            ;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}
