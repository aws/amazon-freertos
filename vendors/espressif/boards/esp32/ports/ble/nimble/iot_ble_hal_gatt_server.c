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
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

#define APP_ID               0
#define GATT_MAX_ENTITIES    30

typedef struct
{
    BTGattAttributeType_t type;
    void * attr;
    ble_uuid_t * uuid;
    uint16_t flags;
    uint16_t parent_handle;
    uint16_t handle;
    uint16_t device_handle;
    uint8_t min_key_size;
    uint8_t total_attr;
} BTGattEntity_t;

static struct ble_gatt_access_ctxt g_ctxt;

static BTGattEntity_t xGattTable[ GATT_MAX_ENTITIES ];

static uint8_t xGattTableSize, xAttrNum, xCurrentAttrNum;

static uint16_t xLastSvcHandle;

static SemaphoreHandle_t xSem;

static void xGattTableCleanup()
{
    int i;

    for( i = 0; i < xGattTableSize; i++ )
    {
        if( xGattTable[ i ].attr )
        {
            vPortFree( xGattTable[ i ].attr );
        }

        if( xGattTable[ i ].uuid )
        {
            vPortFree( xGattTable[ i ].uuid );
        }

        memset( &xGattTable[ i ], 0, sizeof( xGattTable ) );
    }
}


void prvGattGetSemaphore()
{
    xSemaphoreTake( xSem, portMAX_DELAY );
}

/**
 * @brief Returns the given inner GATT handle for a device handle
 * @param handle device handle
 * @return inner GATT handle
 */
uint16_t prvGattFromDevHandle( uint16_t handle )
{
    for( uint16_t i = 0; i < xGattTableSize; ++i )
    {
        if( xGattTable[ i ].device_handle == handle )
        {
            return xGattTable[ i ].handle;
        }
    }

    return 0;
}

/**
 * @brief Returns the device handle for a GATT handle
 * @param handle GATT handle
 * @return device handle
 */
uint16_t prvDevFromGattHandle( uint16_t handle )
{
    for( uint16_t i = 0; i < xGattTableSize; ++i )
    {
        if( xGattTable[ i ].handle == handle )
        {
            return xGattTable[ i ].device_handle;
        }
    }

    return 0;
}

static void prvSvcGattToDevHandle( struct ble_gatt_register_ctxt * ctxt )
{
    const ble_uuid_t * uuid = ctxt->svc.svc_def->uuid;
    uint16_t handle = ctxt->svc.handle;

    xCurrentAttrNum++;

    for( uint16_t i = 0; i < xGattTableSize; i++ )
    {
        if( ble_uuid_cmp( uuid, xGattTable[ i ].uuid ) == 0 )
        {
            xGattTable[ i ].device_handle = handle;
            break;
        }
    }
}

static void prvChrGattToDevHandle( struct ble_gatt_register_ctxt * ctxt )
{
    uint16_t i = 0;
    uint16_t * handle = NULL, chr_handle = 0;

    xCurrentAttrNum++;

    if( ctxt->chr.chr_def->arg )
    {
        handle = ( uint16_t * ) ctxt->chr.chr_def->arg;
        *handle = ctxt->chr.val_handle;
    }
    else
    {
        return;
    }

    if( handle && ( ( ctxt->chr.chr_def->flags & BLE_GATT_CHR_F_NOTIFY ) ||
                    ( ctxt->chr.chr_def->flags & BLE_GATT_CHR_F_INDICATE ) ) )
    {
        for( i = 0; i < xGattTableSize; i++ )
        {
            if( xGattTable[ i ].device_handle == *handle )
            {
                chr_handle = xGattTable[ i ].handle;

                while( i++ < xGattTableSize )
                {
                    if( ( xGattTable[ i ].type != GATT_DESCR ) || ( xGattTable[ i ].parent_handle != chr_handle ) )
                    {
                        continue;
                    }

                    if( ble_uuid_cmp( BLE_UUID16_DECLARE( BLE_GATT_DSC_CLT_CFG_UUID16 ), xGattTable[ i ].uuid ) == 0 )
                    {
                        xGattTable[ i ].device_handle = *handle + 1;
                        break;
                    }
                }
            }
        }
    }
}

void * pvPortCalloc( size_t xNum,
                     size_t xSize )
{
    void * pvReturn;

    pvReturn = pvPortMalloc( xNum * xSize );

    if( pvReturn != NULL )
    {
        memset( pvReturn, 0x00, xNum * xSize );
    }

    return pvReturn;
}


BTGattServerCallbacks_t xGattServerCb;
uint32_t ulGattServerIFhandle = 0;

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
static BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
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
    .pxSendResponse       = prvBTSendResponse
};

/*-----------------------------------------------------------*/

static uint16_t prvAFRToESPCharPerm( BTCharProperties_t xProperties,
                                     BTCharPermissions_t xPermissions )
{
    uint16_t flags = 0;

    if( xProperties & eBTPropBroadcast )
    {
        flags |= BLE_GATT_CHR_F_BROADCAST;
    }

    if( xProperties & eBTPropRead )
    {
        flags |= BLE_GATT_CHR_F_READ;
    }

    if( xProperties & eBTPropWriteNoResponse )
    {
        flags |= BLE_GATT_CHR_F_WRITE_NO_RSP;
    }

    if( xProperties & eBTPropWrite )
    {
        flags |= BLE_GATT_CHR_F_WRITE;
    }

    if( xProperties & eBTPropNotify )
    {
        flags |= BLE_GATT_CHR_F_NOTIFY;
    }

    if( xProperties & eBTPropIndicate )
    {
        flags |= BLE_GATT_CHR_F_INDICATE;
    }

    if( xProperties & eBTPropSignedWrite )
    {
        flags |= BLE_GATT_CHR_F_AUTH_SIGN_WRITE;
    }

    if( xProperties & eBTPropExtendedProps )
    {
        flags |= BLE_GATT_CHR_F_RELIABLE_WRITE; /*HD: Check if this is required - BLE_GATT_CHR_F_AUX_WRITE */
    }

    if( xPermissions & eBTPermReadEncrypted )
    {
        flags |= ( BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC );
    }

    if( xPermissions & eBTPermReadEncryptedMitm )
    {
        flags |= ( BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_READ_AUTHEN );
    }

    if( xPermissions & eBTPermWriteEncrypted )
    {
        flags |= ( BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC );
    }

    if( xPermissions & eBTPermWriteEncryptedMitm )
    {
        flags |= ( BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_WRITE_AUTHEN );
    }

    return flags;
}

static uint8_t prvAFRToESPDescPerm( BTCharPermissions_t xPermissions )
{
    uint8_t flags = 0;

    if( xPermissions & eBTPermRead )
    {
        flags |= BLE_ATT_F_READ;
    }

    if( xPermissions & eBTPermReadEncrypted )
    {
        flags |= ( BLE_ATT_F_READ | BLE_ATT_F_READ_ENC );
    }

    if( xPermissions & eBTPermReadEncryptedMitm )
    {
        flags |= ( BLE_ATT_F_READ | BLE_ATT_F_READ_ENC | BLE_ATT_F_READ_AUTHEN );
    }

    if( xPermissions & eBTPermWrite )
    {
        flags |= BLE_ATT_F_WRITE;
    }

    if( xPermissions & eBTPermWriteEncrypted )
    {
        flags |= ( BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC );
    }

    if( xPermissions & eBTPermWriteEncryptedMitm )
    {
        flags |= ( BLE_ATT_F_WRITE | BLE_ATT_F_WRITE_ENC | BLE_ATT_F_WRITE_AUTHEN );
    }

    /*HD: eBTPermWriteSigned eBTPermWriteSignedMitm to be handled */
    return flags;
}

ble_uuid_t * prvCopytoESPUUID( BTUuid_t * pxUuid )
{
    ble_uuid_t * pxESPuuid;
    ble_uuid16_t * uuid16 = NULL;
    ble_uuid32_t * uuid32 = NULL;
    ble_uuid128_t * uuid128 = NULL;

    switch( pxUuid->ucType )
    {
        case eBTuuidType16:
            uuid16 = pvPortCalloc( 1, sizeof( ble_uuid16_t ) );

            if( !uuid16 )
            {
                return NULL;
            }

            uuid16->u.type = BLE_UUID_TYPE_16;
            uuid16->value = pxUuid->uu.uu16;
            pxESPuuid = ( ble_uuid_t * ) uuid16;
            break;

        case eBTuuidType32:
            uuid32 = pvPortCalloc( 1, sizeof( ble_uuid32_t ) );

            if( !uuid32 )
            {
                return NULL;
            }

            uuid32->u.type = BLE_UUID_TYPE_32;
            uuid32->value = pxUuid->uu.uu32;
            pxESPuuid = ( ble_uuid_t * ) uuid32;
            break;

        case eBTuuidType128:
        default:
            uuid128 = pvPortCalloc( 1, sizeof( ble_uuid128_t ) );

            if( !uuid128 )
            {
                return NULL;
            }

            uuid128->u.type = BLE_UUID_TYPE_128;
            memcpy( uuid128->value, pxUuid->uu.uu128, sizeof( pxUuid->uu.uu128 ) );
            pxESPuuid = ( ble_uuid_t * ) uuid128;
            break;
    }

    return pxESPuuid;
}

/*-----------------------------------------------------------*/

static int prvGATTCharAccessCb( uint16_t conn_handle,
                                uint16_t attr_handle,
                                struct ble_gatt_access_ctxt * ctxt,
                                void * arg )
{
    struct ble_gap_conn_desc desc;
    int rc = 0;
    bool need_rsp = 1;
    uint16_t handle = 0, out_len = 0;
    uint8_t dst_buf[ 512 ] = { 0 }; /* HD: Check allocation on stack */

    ble_gap_conn_find( conn_handle, &desc );

    switch( ctxt->op )
    {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        case BLE_GATT_ACCESS_OP_READ_DSC:
            ESP_LOGI( TAG, "In read for handle %d", attr_handle );
            handle = prvGattFromDevHandle( attr_handle );
            memcpy( &g_ctxt, ctxt, sizeof( g_ctxt ) );

            if( handle && ( xGattServerCb.pxRequestReadCb != NULL ) )
            {
                xGattServerCb.pxRequestReadCb( conn_handle, ( uint32_t ) ctxt, ( BTBdaddr_t * ) desc.peer_id_addr.val, handle, 0 );
            }

            prvGattGetSemaphore();
            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            rc = ble_hs_mbuf_to_flat( ctxt->om, dst_buf, sizeof( dst_buf ), &out_len );

            if( rc != 0 )
            {
                return BLE_ATT_ERR_UNLIKELY;
            }

            ESP_LOGI( TAG, "In write for handle %d and len %d", attr_handle, out_len );
            handle = prvGattFromDevHandle( attr_handle );

            if( handle && ( xGattServerCb.pxRequestWriteCb != NULL ) )
            {
                memcpy( &g_ctxt, ctxt, sizeof( g_ctxt ) );

                if( ( ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR ) && ( ctxt->chr->flags & BLE_GATT_CHR_F_WRITE_NO_RSP ) )
                {
                    need_rsp = 0;
                }

                if( xGattServerCb.pxRequestWriteCb != NULL )
                {
                    xGattServerCb.pxRequestWriteCb( conn_handle, ( uint32_t ) ctxt, ( BTBdaddr_t * ) desc.peer_id_addr.val, handle, 0, out_len, need_rsp, 0, dst_buf );
                }

                if( need_rsp )
                {
                    prvGattGetSemaphore();
                }
            }

            return 0;

            break;

        default:
            assert( 0 );
            rc = BLE_ATT_ERR_UNLIKELY;
    }

    return rc;
}

void prvGATTRegisterCb( struct ble_gatt_register_ctxt * ctxt,
                        void * arg )
{
    uint16_t * handle;
    char buf[ BLE_UUID_STR_LEN ];

    switch( ctxt->op )
    {
        case BLE_GATT_REGISTER_OP_SVC:
            MODLOG_DFLT( DEBUG, "registered service %s with handle=%d\n",
                         ble_uuid_to_str( ctxt->svc.svc_def->uuid, buf ),
                         ctxt->svc.handle );
            prvSvcGattToDevHandle( ctxt );
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            MODLOG_DFLT( DEBUG, "registered characteristic %s with "
                                "def_handle=%d val_handle=%d\n",
                         ble_uuid_to_str( ctxt->chr.chr_def->uuid, buf ),
                         ctxt->chr.def_handle,
                         ctxt->chr.val_handle );
            prvChrGattToDevHandle( ctxt );
            break;

        case BLE_GATT_REGISTER_OP_DSC:
            MODLOG_DFLT( DEBUG, "registered descriptor %s with handle=%d\n",
                         ble_uuid_to_str( ctxt->dsc.dsc_def->uuid, buf ),
                         ctxt->dsc.handle );

            if( ctxt->dsc.dsc_def->arg )
            {
                handle = ( uint16_t * ) ctxt->dsc.dsc_def->arg;
                *handle = ctxt->dsc.handle;
            }

            xCurrentAttrNum++;
            break;

        default:
            assert( 0 );
            break;
    }

    if( xCurrentAttrNum == ( xAttrNum + 3 /* GAP attributes */ + 2 /* GATT attributes */ ) )
    {
        for( int i = 0; i < xGattTableSize; i++ )
        {
            ESP_LOGD( TAG, "Type: %d Handle: %d Dev Handle: %d", xGattTable[ i ].type, xGattTable[ i ].handle, xGattTable[ i ].device_handle );
        }

        if( xGattServerCb.pxServiceStartedCb != NULL )
        {
            xGattServerCb.pxServiceStartedCb( eBTStatusSuccess, ulGattServerIFhandle, xLastSvcHandle );
        }

        xCurrentAttrNum = 0;
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

    xGattServerCb.pxUnregisterServerCb( eBTStatusSuccess, ulGattServerIFhandle );
    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    ble_hs_cfg.gatts_register_cb = prvGATTRegisterCb;

    if( pxCallbacks != NULL )
    {
        xGattServerCb = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    xSem = xSemaphoreCreateBinary();
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

    if( xGattTableSize == GATT_MAX_ENTITIES )
    {
        xGattTableCleanup();
        return eBTStatusNoMem;
    }

    struct ble_gatt_svc_def * svc = pvPortCalloc( 1 + 1, sizeof( struct ble_gatt_svc_def ) );

    if( !svc )
    {
        xGattTableCleanup();
        return eBTStatusNoMem;
    }

    if( pxSrvcId->xServiceType == eBTServiceTypePrimary )
    {
        svc->type = BLE_GATT_SVC_TYPE_PRIMARY;
    }
    else if( pxSrvcId->xServiceType == eBTServiceTypeSecondary )
    {
        svc->type = BLE_GATT_SVC_TYPE_SECONDARY;
    }

    xGattTable[ xGattTableSize ].uuid = prvCopytoESPUUID( &pxSrvcId->xId.xUuid );

    if( !xGattTable[ xGattTableSize ].uuid )
    {
        xGattTableCleanup();
        return eBTStatusNoMem;
    }

    svc->uuid = xGattTable[ xGattTableSize ].uuid;

    xGattTable[ xGattTableSize ].type = GATT_SERVICE;
    xGattTable[ xGattTableSize ].attr = svc;
    xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 1;
    ESP_LOGD( TAG, "Saving service data at index %d for handle %d", xGattTableSize, xGattTable[ xGattTableSize ].handle );
    xGattTableSize += 1;

    if( xGattServerCb.pxServiceAddedCb )
    {
        xGattServerCb.pxServiceAddedCb( xStatus, ulGattServerIFhandle, pxSrvcId, xGattTable[ xGattTableSize - 1 ].handle );
    }

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

    xGattTable[ xGattTableSize ].uuid = prvCopytoESPUUID( pxUuid );

    if( !xGattTable[ xGattTableSize ].uuid )
    {
        xGattTableCleanup();
        return eBTStatusNoMem;
    }

    xGattTable[ xGattTableSize ].type = GATT_CHAR;
    xGattTable[ xGattTableSize ].flags = prvAFRToESPCharPerm( xProperties, xPermissions );
    xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 2;
    ESP_LOGD( TAG, "Saving characteristic data at index %d for handle %d", xGattTableSize, xGattTable[ xGattTableSize ].handle );
    xGattTable[ xGattTableSize ].parent_handle = UINT16_MAX;

    for( int i = 0; i < xGattTableSize; ++i )
    {
        if( ( xGattTable[ i ].type == GATT_SERVICE ) && ( xGattTable[ i ].handle == usServiceHandle ) )
        {
            xGattTable[ xGattTableSize ].parent_handle = xGattTable[ i ].handle;
            /*Increment the total attr count of the service */
            ESP_LOGD( TAG, "Parent service is at index %d and handle %d", i, xGattTable[ i ].handle );
            xGattTable[ i ].total_attr += 1;
            break;
        }
    }

    if( xGattTable[ xGattTableSize ].parent_handle == UINT16_MAX )
    {
        xGattTableCleanup();
        return eBTStatusParamInvalid;
    }

    xGattTableSize += 1;

    if( xGattServerCb.pxCharacteristicAddedCb )
    {
        xGattServerCb.pxCharacteristicAddedCb( xStatus, ulGattServerIFhandle, pxUuid, xGattTable[ xGattTableSize - 1 ].parent_handle,
                                               xGattTable[ xGattTableSize - 1 ].handle );
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

    xGattTable[ xGattTableSize ].uuid = prvCopytoESPUUID( pxUuid );

    if( !xGattTable[ xGattTableSize ].uuid )
    {
        xGattTableCleanup();
        return eBTStatusNoMem;
    }

    xGattTable[ xGattTableSize ].type = GATT_DESCR;
    xGattTable[ xGattTableSize ].flags = prvAFRToESPDescPerm( xPermissions );
    xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 1;
    ESP_LOGD( TAG, "Saving descriptor data at index %d for handle %d", xGattTableSize, xGattTable[ xGattTableSize ].handle );
    xGattTable[ xGattTableSize ].parent_handle = UINT16_MAX;

    for( int16_t i = ( int16_t ) xGattTableSize - 1; i >= 0; --i )
    {
        if( ( xGattTable[ i ].type == GATT_CHAR ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) )
        {
            xGattTable[ xGattTableSize ].parent_handle = xGattTable[ i ].handle;

            /*Increment the total attr count of the char */
            if( ble_uuid_cmp( xGattTable[ xGattTableSize ].uuid, BLE_UUID16_DECLARE( BLE_GATT_DSC_CLT_CFG_UUID16 ) ) == 0 )
            {
            }
            else
            {
                ESP_LOGD( TAG, "Parent characteristic is at index %d and handle %d", i, xGattTable[ i ].handle );
                xGattTable[ i ].total_attr += 1;
            }

            break;
        }
    }

    if( xGattTable[ xGattTableSize ].parent_handle == UINT16_MAX )
    {
        xGattTableCleanup();
        return eBTStatusParamInvalid;
    }

    xGattTableSize += 1;

    if( xGattServerCb.pxDescriptorAddedCb )
    {
        xGattServerCb.pxDescriptorAddedCb( xStatus, ulGattServerIFhandle, pxUuid, usServiceHandle, xGattTable[ xGattTableSize - 1 ].handle );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BTStatus_t prvPopulateGattDatabase( uint16_t last_svc_index )
{
    struct ble_gatt_svc_def * svc = NULL;
    struct ble_gatt_chr_def * chr = NULL;
    struct ble_gatt_dsc_def * dsc = NULL;
    int i = 0, j, k, cnt_chr = 0, cnt_dsc = 0;

    for( i = 0; i <= last_svc_index; i++ )
    {
        if( ( xGattTable[ i ].type == GATT_SERVICE ) && ( xGattTable[ i ].flags == 0 ) )
        {
            xAttrNum += 1;
            svc = xGattTable[ i ].attr;

            /* Populate the characteristics and descriptors of the service */
            cnt_chr = 0;
            svc->characteristics = pvPortCalloc( xGattTable[ i ].total_attr + 1, sizeof( struct ble_gatt_chr_def ) );

            if( !svc->characteristics )
            {
                xGattTableCleanup();
                return eBTStatusNoMem;
            }

            /* Save the pointer, might be required for freeing */
            xGattTable[ i + 1 ].attr = ( struct ble_gatt_chr_def * ) svc->characteristics;
            chr = ( struct ble_gatt_chr_def * ) svc->characteristics;

            for( j = i + 1; j < xGattTableSize; j++ )
            {
                if( ( xGattTable[ j ].type == GATT_CHAR ) && ( xGattTable[ j ].parent_handle == xGattTable[ i ].handle ) )
                {
                    chr->uuid = xGattTable[ j ].uuid;
                    chr->arg = &xGattTable[ j ].device_handle;
                    chr->access_cb = prvGATTCharAccessCb;
                    chr->flags = xGattTable[ j ].flags;

                    if( xGattTable[ j ].total_attr )
                    {
                        cnt_dsc = 0;
                        chr->descriptors = pvPortCalloc( xGattTable[ j ].total_attr + 1, sizeof( struct ble_gatt_dsc_def ) );

                        if( !chr->descriptors )
                        {
                            xGattTableCleanup();
                            return eBTStatusNoMem;
                        }

                        xGattTable[ j + 1 ].attr = chr->descriptors;
                        dsc = chr->descriptors;

                        for( k = j + 1; k < xGattTableSize; k++ )
                        {
                            if( ( xGattTable[ k ].type == GATT_DESCR ) && ( xGattTable[ k ].parent_handle == xGattTable[ j ].handle ) )
                            {
                                if( ble_uuid_cmp( xGattTable[ k ].uuid, BLE_UUID16_DECLARE( BLE_GATT_DSC_CLT_CFG_UUID16 ) ) == 0 )
                                {
                                    continue;
                                }

                                dsc->uuid = xGattTable[ k ].uuid;
                                dsc->arg = &xGattTable[ k ].device_handle;
                                dsc->access_cb = prvGATTCharAccessCb;
                                dsc->att_flags = xGattTable[ k ].flags;
                                dsc++;
                                cnt_dsc++;

                                if( cnt_dsc == xGattTable[ j ].total_attr )
                                {
                                    xAttrNum += xGattTable[ j ].total_attr;
                                    break;
                                }
                            }
                        }
                    }

                    chr++;
                    cnt_chr++;

                    if( cnt_chr == xGattTable[ i ].total_attr )
                    {
                        xAttrNum += xGattTable[ i ].total_attr;
                        break;
                    }
                }
            }

            xGattTable[ i ].flags = 1;
        }
    }

    return eBTStatusSuccess;
}


BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    int i = 0, last_svc_index = 0;
    BTStatus_t xStatus = eBTStatusSuccess;

    for( int i = xGattTableSize; i >= 0; i-- )
    {
        if( ( xGattTable[ i ].type == GATT_SERVICE ) && ( xGattTable[ i ].handle == usServiceHandle ) )
        {
            last_svc_index = i;
            xLastSvcHandle = xGattTable[ i ].handle;
            ESP_LOGD( TAG, "Starting service with handle %d and present at index %d", xGattTable[ i ].handle, i );
            break;
        }
    }

    if( ( last_svc_index == xGattTableSize ) || ( !xGattTable[ last_svc_index ].attr ) )
    {
        xGattTableCleanup();
        return eBTStatusFail;
    }

    if( xGattTable[ last_svc_index ].flags )
    {
        if( xGattServerCb.pxServiceStartedCb != NULL )
        {
            xGattServerCb.pxServiceStartedCb( eBTStatusSuccess, ulGattServerIFhandle, xGattTable[ last_svc_index ].handle );
        }

        xCurrentAttrNum = 0;
        return eBTStatusSuccess;
    }

    xStatus = prvPopulateGattDatabase( last_svc_index );

    ble_gatts_reset();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    for( i = 0; i <= last_svc_index; i++ )
    {
        if( xGattTable[ i ].type == GATT_SERVICE )
        {
            ble_gatts_count_cfg( ( struct ble_gatt_svc_def * ) xGattTable[ i ].attr );
            ble_gatts_add_svcs( ( struct ble_gatt_svc_def * ) xGattTable[ i ].attr );
        }
    }

    ble_gatts_start();

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStopService( uint8_t ucServerIf,
                             uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* It is not supported to stop a GATT service, so we just return success.
     */
    if( xGattServerCb.pxServiceStoppedCb )
    {
        xGattServerCb.pxServiceStoppedCb( eBTStatusSuccess, ucServerIf, usServiceHandle );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                               uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( xGattServerCb.pxServiceDeletedCb )
    {
        xGattServerCb.pxServiceDeletedCb( eBTStatusSuccess, ucServerIf, usServiceHandle );
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
    int xESPstatus = 0;
    uint16_t handle;
    BTStatus_t xStatus = eBTStatusSuccess;

    handle = prvDevFromGattHandle( usAttributeHandle );
    struct os_mbuf * om;
    om = ble_hs_mbuf_from_flat( pucValue, xLen );

    if( !om )
    {
        return eBTStatusFail;
    }

    if( bConfirm )
    {
        ESP_LOGI( TAG, "Send Indications" );
        xESPstatus = ble_gattc_indicate_custom( usConnId, handle, om );
    }
    else
    {
        ESP_LOGI( TAG, "Send Notifications" );
        xESPstatus = ble_gattc_notify_custom( usConnId, handle, om );
    }

    if( xESPstatus != 0 )
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
    /*struct ble_gatt_access_ctxt *ctxt = ( struct ble_gatt_access_ctxt * )ulTransId; */
    struct ble_gatt_access_ctxt * ctxt = &g_ctxt;

    BTStatus_t xReturnStatus = eBTStatusSuccess;

    if( ctxt && ( ( ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR ) || ( ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC ) ) )
    {
        /* Huge array allocate in the stack */
        int rc = os_mbuf_append( ctxt->om, pxResponse->xAttrValue.pucValue, pxResponse->xAttrValue.xLen );

        if( rc != 0 )
        {
            xReturnStatus = eBTStatusFail;
        }
    }

    xSemaphoreGive( xSem );

    if( xGattServerCb.pxResponseConfirmationCb != NULL )
    {
        xGattServerCb.pxResponseConfirmationCb( xStatus, pxResponse->usHandle );
    }

    return xReturnStatus;
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
