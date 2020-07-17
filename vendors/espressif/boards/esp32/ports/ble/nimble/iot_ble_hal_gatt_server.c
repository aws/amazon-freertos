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
 * @file aws_ble_hal_gatt_server.c
 * @brief Hardware Abstraction Layer for GATT server ble stack.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "iot_ble_config.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

#define APP_ID          0
#define MAX_SERVICES    20

static struct ble_gatt_svc_def espServices[ MAX_SERVICES + 1 ];
static BTService_t * afrServices[ MAX_SERVICES ];
static uint16_t serviceCnt = 0;
static SemaphoreHandle_t xSem;
static bool semInited;
bool xSemLock = 0;
uint16_t gattOffset = 0;

void prvGattGetSemaphore()
{
    xSemaphoreTake( xSem, portMAX_DELAY );
}

void prvGattGiveSemaphore()
{
    xSemaphoreGive( xSem );
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
static uint16_t prvCountCharacteristics( BTService_t * pxService );
static uint16_t prvCountDescriptor( BTService_t * pxService,
                                    uint16_t startHandle );
static void prvCleanupService( struct ble_gatt_svc_def * pSvc );
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

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
                                     uint16_t usMtu );


void vESPBTGATTServerCleanup( void );

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
        flags |= BLE_GATT_CHR_F_RELIABLE_WRITE; /*fixme: Check if this is required - BLE_GATT_CHR_F_AUX_WRITE */
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

    /*fixme: eBTPermWriteSigned eBTPermWriteSignedMitm to be handled */
    return flags;
}

ble_uuid_t * prvCopytoESPUUID( BTUuid_t * pxUuid,
                               ble_uuid_t * pUuid )
{
    ble_uuid_t * pxESPuuid;
    ble_uuid16_t * uuid16 = NULL;
    ble_uuid32_t * uuid32 = NULL;
    ble_uuid128_t * uuid128 = NULL;

    switch( pxUuid->ucType )
    {
        case eBTuuidType16:

            if( pUuid == NULL )
            {
                uuid16 = pvPortCalloc( 1, sizeof( ble_uuid16_t ) );

                if( !uuid16 )
                {
                    return NULL;
                }
            }
            else
            {
                uuid16 = ( ble_uuid16_t * ) pUuid;
            }

            uuid16->u.type = BLE_UUID_TYPE_16;
            uuid16->value = pxUuid->uu.uu16;
            pxESPuuid = ( ble_uuid_t * ) uuid16;
            break;

        case eBTuuidType32:

            if( pUuid == NULL )
            {
                uuid32 = pvPortCalloc( 1, sizeof( ble_uuid32_t ) );

                if( !uuid32 )
                {
                    return NULL;
                }
            }
            else
            {
                uuid32 = ( ble_uuid32_t * ) pUuid;
            }

            uuid32->u.type = BLE_UUID_TYPE_32;
            uuid32->value = pxUuid->uu.uu32;
            pxESPuuid = ( ble_uuid_t * ) uuid32;
            break;

        case eBTuuidType128:

            if( pUuid == NULL )
            {
                uuid128 = pvPortCalloc( 1, sizeof( ble_uuid128_t ) );

                if( !uuid128 )
                {
                    return NULL;
                }
            }
            else
            {
                uuid128 = ( ble_uuid128_t * ) pUuid;
            }

            uuid128->u.type = BLE_UUID_TYPE_128;
            memcpy( uuid128->value, pxUuid->uu.uu128, sizeof( pxUuid->uu.uu128 ) );
            pxESPuuid = ( ble_uuid_t * ) uuid128;
            break;

        default:
            pxESPuuid = NULL;
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
    uint32_t trans_id;
    int rc = 0;
    bool need_rsp = 1;
    uint16_t out_len = 0;
    uint8_t dst_buf[ 512 ] = { 0 }; /* fixme: Check allocation on stack */

    ble_gap_conn_find( conn_handle, &desc );

    switch( ctxt->op )
    {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        case BLE_GATT_ACCESS_OP_READ_DSC:
            ESP_LOGD( TAG, "In read for handle %d", attr_handle );

            if( xGattServerCb.pxRequestReadCb != NULL )
            {
                xSemLock = 1;
                xGattServerCb.pxRequestReadCb( conn_handle, ( uint32_t ) ctxt, ( BTBdaddr_t * ) desc.peer_id_addr.val, attr_handle - gattOffset, 0 );
                prvGattGetSemaphore();
                xSemLock = 0;
            }

            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
        case BLE_GATT_ACCESS_OP_WRITE_DSC:
            rc = ble_hs_mbuf_to_flat( ctxt->om, dst_buf, sizeof( dst_buf ), &out_len );

            if( rc != 0 )
            {
                return BLE_ATT_ERR_UNLIKELY;
            }

            ESP_LOGD( TAG, "In write for handle %d and len %d", attr_handle, out_len );
            trans_id = ( uint32_t ) ctxt;

            if( xGattServerCb.pxRequestWriteCb != NULL )
            {
                if( ( ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR ) && ( ctxt->chr->flags & BLE_GATT_CHR_F_WRITE_NO_RSP ) )
                {
                    need_rsp = 0;
                    trans_id = 0;
                }
                else
                {
                    xSemLock = 1;
                }

                xGattServerCb.pxRequestWriteCb( conn_handle, trans_id, ( BTBdaddr_t * ) desc.peer_id_addr.val, attr_handle - gattOffset, 0, out_len, need_rsp, 0, dst_buf );

                if( need_rsp )
                {
                    prvGattGetSemaphore();
                    xSemLock = 0;
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
    ble_uuid128_t uuid128;
    BTService_t * pxService;

    if( ctxt->op == BLE_GATT_REGISTER_OP_SVC )
    {
        pxService = afrServices[ 0 ];
        ( void * ) prvCopytoESPUUID( &pxService->pxBLEAttributes[ 0 ].xServiceUUID, ( ble_uuid_t * ) &uuid128 );

        if( ble_uuid_cmp( ctxt->svc.svc_def->uuid, ( ble_uuid_t * ) &uuid128 ) == 0 )
        {
            gattOffset = ctxt->svc.handle;
        }
    }
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTRegisterServer( BTUuid_t * pxUuid )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( xGattServerCb.pxRegisterServerCb != NULL )
    {
        xGattServerCb.pxRegisterServerCb( eBTStatusSuccess, ulGattServerIFhandle, pxUuid );
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTUnregisterServer( uint8_t ucServerIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( xGattServerCb.pxUnregisterServerCb != NULL )
    {
        xGattServerCb.pxUnregisterServerCb( eBTStatusSuccess, ulGattServerIFhandle );
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGattServerInit( const BTGattServerCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    ble_hs_cfg.gatts_register_cb = prvGATTRegisterCb;
    serviceCnt = 0;

    memset( espServices, 0, sizeof( struct ble_gatt_svc_def ) * ( MAX_SERVICES + 1 ) );

    if( pxCallbacks != NULL )
    {
        xGattServerCb = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusParamInvalid;
    }

    xSem = xSemaphoreCreateBinary();

    if( xSem != NULL )
    {
        semInited = true;
    }
    else
    {
        xStatus = eBTStatusNoMem;
    }

    return xStatus;
}
/*-------------------------------------------------------------------------------------*/
void vESPBTGATTServerCleanup( void )
{
    size_t index;

    if( serviceCnt > 0 )
    {
        ble_gatts_stop();

        for( index = 0; index < serviceCnt; index++ )
        {
            prvCleanupService( &espServices[ index ] );
        }

        serviceCnt = 0;

        memset( espServices, 0, sizeof( struct ble_gatt_svc_def ) * ( MAX_SERVICES + 1 ) );
        memset( afrServices, 0, sizeof( struct BTService_t * ) * ( MAX_SERVICES + 1 ) );
    }

    if( semInited )
    {
        vSemaphoreDelete( xSem );
        semInited = false;
    }
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
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    uint16_t usIncludedHandle )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * pxUuid,
                                   BTCharProperties_t xProperties,
                                   BTCharPermissions_t xPermissions )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue )
{
    return eBTStatusUnsupported;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                               uint16_t usServiceHandle,
                               BTUuid_t * pxUuid,
                               BTCharPermissions_t xPermissions )
{
    return eBTStatusUnsupported;
}


BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTStopService( uint8_t ucServerIf,
                             uint16_t usServiceHandle )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* It is not supported to stop a GATT service, so we just return success.
     */
    if( xGattServerCb.pxServiceStoppedCb != NULL )
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

    /* @TODO: need to be implemented. However, stop is not supported.
     */
    if( xGattServerCb.pxServiceDeletedCb != NULL )
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
    BTStatus_t xStatus = eBTStatusSuccess;
    struct os_mbuf * om;

    om = ble_hs_mbuf_from_flat( pucValue, xLen );

    if( !om )
    {
        return eBTStatusFail;
    }

    if( bConfirm )
    {
        ESP_LOGD( TAG, "Send Indications" );
        xESPstatus = ble_gattc_indicate_custom( usConnId, usAttributeHandle + gattOffset, om );
    }
    else
    {
        ESP_LOGD( TAG, "Send Notifications" );
        xESPstatus = ble_gattc_notify_custom( usConnId, usAttributeHandle + gattOffset, om );
    }

    if( xESPstatus != 0 )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static bool prvValidGattRequest()
{
    if( xSemLock )
    {
        return true;
    }

    return false;
}

BTStatus_t prvBTSendResponse( uint16_t usConnId,
                              uint32_t ulTransId,
                              BTStatus_t xStatus,
                              BTGattResponse_t * pxResponse )
{
    struct ble_gatt_access_ctxt * ctxt = ( struct ble_gatt_access_ctxt * ) ulTransId;

    BTStatus_t xReturnStatus = eBTStatusSuccess;

    if( prvValidGattRequest() )
    {
        if( ctxt && ( ( ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR ) || ( ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC ) ) )
        {
            /* Huge array allocate in the stack */
            int rc = os_mbuf_append( ctxt->om, pxResponse->xAttrValue.pucValue, pxResponse->xAttrValue.xLen );

            if( rc != 0 )
            {
                xReturnStatus = eBTStatusFail;
            }
        }

        prvGattGiveSemaphore();
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    if( xGattServerCb.pxResponseConfirmationCb != NULL )
    {
        xGattServerCb.pxResponseConfirmationCb( xStatus, pxResponse->usHandle );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/
static uint16_t prvCountCharacteristics( BTService_t * pxService )
{
    uint16_t nbCharacteristics = 0;
    uint16_t index;

    for( index = 0; index < pxService->xNumberOfAttributes; index++ )
    {
        if( pxService->pxBLEAttributes[ index ].xAttributeType == eBTDbCharacteristic )
        {
            nbCharacteristics++;
        }
    }

    return nbCharacteristics;
}

static uint16_t prvCountDescriptor( BTService_t * pxService,
                                    uint16_t startHandle )
{
    uint16_t nbDescriptor = 0;
    uint16_t index;

    for( index = startHandle; index < pxService->xNumberOfAttributes; index++ )
    {
        if( pxService->pxBLEAttributes[ index ].xAttributeType == eBTDbDescriptor )
        {
            nbDescriptor++;
        }
        else if( pxService->pxBLEAttributes[ index ].xAttributeType == eBTDbCharacteristic )
        {
            break;
        }
    }

    return nbDescriptor;
}

/* @brief cleanup memory allocated for a service.
 *
 * Since all memory is initialized to 0, that function can be called to clean even half created service.
 */
static void prvCleanupService( struct ble_gatt_svc_def * pSvc )
{
    const struct ble_gatt_dsc_def * pDescriptor;
    const struct ble_gatt_chr_def * pCharacteristic;
    size_t cIndex, dIndex;

    if( pSvc->uuid != NULL )
    {
        vPortFree( ( void * ) pSvc->uuid );
    }

    if( pSvc->characteristics != NULL )
    {
        for( cIndex = 0; pSvc->characteristics[ cIndex ].uuid != NULL; cIndex++ )
        {
            pCharacteristic = &pSvc->characteristics[ cIndex ];
            vPortFree( ( void * ) pCharacteristic->uuid );

            if( pCharacteristic->descriptors != NULL )
            {
                for( dIndex = 0; pCharacteristic->descriptors[ dIndex ].uuid != NULL; dIndex++ )
                {
                    pDescriptor = &pCharacteristic->descriptors[ dIndex ];
                    vPortFree( ( void * ) pDescriptor->uuid );
                }

                vPortFree( ( void * ) pCharacteristic->descriptors );
            }
        }

        vPortFree( ( void * ) pSvc->characteristics );
    }

    memset( pSvc, 0x00, sizeof( struct ble_gatt_svc_def ) );
}

/* @brief Simple function that creates a full service in one go.
 * The function is big because of error checking but in reality it only does the following:
 * 1. Populate service fields
 * 2. Populate each attributes by looping into pxService.
 * An array for service is available in static memory, however, UUIDs and attributes have to be dynamically allocated.
 * This structure is a compromise between full dynamic allocation and static allocation.
 */
BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                              BTService_t * pxService )
{
    uint16_t index;
    uint16_t charCount;
    uint16_t dscrCount;
    struct ble_gatt_chr_def * pCharacteristics = NULL;
    struct ble_gatt_dsc_def * pDescriptors = NULL;
    BTStatus_t xStatus = eBTStatusSuccess;
    ble_uuid_t * uuid;
    struct ble_gatt_svc_def * pSvc = NULL;
    uint16_t handle = 0;
    uint16_t attributeCount = 0;

    if( ( pxService == 0 ) || ( pxService->xNumberOfAttributes == 0 ) )
    {
        configPRINTF( ( "Invalid parameters\n" ) );
        xStatus = eBTStatusFail;
    }

    if( xStatus == eBTStatusSuccess )
    {
        if( serviceCnt < MAX_SERVICES )
        {
            pSvc = &espServices[ serviceCnt ];
            afrServices[ serviceCnt ] = pxService;
        }
        else
        {
            xStatus = eBTStatusNoMem;
        }
    }

    /* Fill in service field. After that start creating characteristics and descriptors individually. */
    if( xStatus == eBTStatusSuccess )
    {
        espServices[ serviceCnt + 1 ].type = BLE_GATT_SVC_TYPE_END;

        /* Initialize starting handle. */
        if( serviceCnt != 0 )
        {
            uint16_t tmpNbAttribute = afrServices[ serviceCnt - 1 ]->xNumberOfAttributes;
            handle = afrServices[ serviceCnt - 1 ]->pusHandlesBuffer[ tmpNbAttribute - 1 ] + 1;
        }
        else
        {
            handle = 0;
        }

        /* Fill in field for ESP service. */
        pxService->pusHandlesBuffer[ attributeCount++ ] = handle;

        if( pxService->pxBLEAttributes[ 0 ].xAttributeType == eBTDbPrimaryService )
        {
            pSvc->type = BLE_GATT_SVC_TYPE_PRIMARY;
        }
        else if( pxService->pxBLEAttributes[ 0 ].xAttributeType == eBTDbSecondaryService )
        {
            pSvc->type = BLE_GATT_SVC_TYPE_SECONDARY;
        }
        else
        {
            configPRINTF( ( "Invalid service type \n" ) );
            xStatus = eBTStatusFail;
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        pSvc->uuid = prvCopytoESPUUID( &pxService->pxBLEAttributes[ 0 ].xServiceUUID, NULL );

        if( pSvc->uuid == NULL )
        {
            configPRINTF( ( "Could not allocate memory for service UUID \n" ) );
            prvCleanupService( pSvc );
            xStatus = eBTStatusNoMem;
        }
        else
        {
            pCharacteristics = pvPortCalloc( prvCountCharacteristics( pxService ) + 1, sizeof( struct ble_gatt_chr_def ) );

            if( pCharacteristics == NULL )
            {
                configPRINTF( ( "Could not allocate memory for  characteristic array \n" ) );
                prvCleanupService( pSvc );
                xStatus = eBTStatusNoMem;
            }

            pSvc->characteristics = pCharacteristics;
            pSvc->includes = NULL;
            charCount = 0;
            dscrCount = 0;
        }
    }

    /* Create characteristics and descriptor individually .*/
    if( xStatus == eBTStatusSuccess )
    {
        for( index = 0; index < pxService->xNumberOfAttributes; index++ )
        {
            switch( pxService->pxBLEAttributes[ index ].xAttributeType )
            {
                case eBTDbCharacteristic:
                    /* Allocate memory for UUID and copies it. */
                    pCharacteristics[ charCount ].uuid = prvCopytoESPUUID( &pxService->pxBLEAttributes[ index ].xCharacteristic.xUuid, NULL );
                    pCharacteristics[ charCount ].arg = ( void * ) pxService;
                    pCharacteristics[ charCount ].access_cb = prvGATTCharAccessCb;
                    pCharacteristics[ charCount ].flags = prvAFRToESPCharPerm( pxService->pxBLEAttributes[ index ].xCharacteristic.xProperties, pxService->pxBLEAttributes[ index ].xCharacteristic.xPermissions );
                    pCharacteristics[ charCount ].min_key_size = IOT_BLE_ENCRYPT_KEY_SIZE_MIN;

                    /* Allocate memory for descriptors. */
                    pCharacteristics[ charCount ].descriptors = pvPortCalloc( prvCountDescriptor( pxService, index + 1 ) + 1, sizeof( struct ble_gatt_dsc_def ) );

                    if( ( pCharacteristics[ charCount ].uuid == NULL ) || ( pCharacteristics[ charCount ].descriptors == NULL ) )
                    {
                        xStatus = eBTStatusNoMem;
                    }

                    /* Update handle for AFR. */
                    handle += 2;
                    pxService->pusHandlesBuffer[ attributeCount++ ] = handle;

                    charCount++;
                    dscrCount = 0;
                    break;

                case eBTDbIncludedService:
                    handle += 1;
                    pxService->pusHandlesBuffer[ attributeCount++ ] = handle;
                    break;

                case eBTDbDescriptor:
                    uuid = prvCopytoESPUUID( &pxService->pxBLEAttributes[ index ].xCharacteristicDescr.xUuid, NULL );
                    handle += 1;
                    pxService->pusHandlesBuffer[ attributeCount++ ] = handle;

                    if( uuid == NULL )
                    {
                        xStatus = eBTStatusNoMem;
                        break;
                    }

                    /* Characteristic descriptors are automatically added, so no need to add them here, otherwise they will be declared twice. */
                    if( ble_uuid_cmp( uuid, BLE_UUID16_DECLARE( BLE_GATT_DSC_CLT_CFG_UUID16 ) ) == 0 )
                    {
                        continue;
                    }

                    /* Allocate memory for UUID and copies it. */
                    pDescriptors = &pSvc->characteristics[ charCount - 1 ].descriptors[ dscrCount ];
                    /* Allocate memory for UUID and copies it. */
                    pDescriptors->uuid = uuid;
                    pDescriptors->arg = ( void * ) pxService;
                    pDescriptors->access_cb = prvGATTCharAccessCb;
                    pDescriptors->min_key_size = IOT_BLE_ENCRYPT_KEY_SIZE_MIN;
                    pDescriptors->att_flags = prvAFRToESPDescPerm( pxService->pxBLEAttributes[ index ].xCharacteristicDescr.xPermissions );

                    dscrCount++;

                    break;

                default:
                    break;
            }

            /* This loop contains many dynamic allocation. If one fails, clean up and exit. */
            if( xStatus != eBTStatusSuccess )
            {
                configPRINTF( ( "Failed to allocate memory during Attribute creation\n" ) );
                prvCleanupService( pSvc );
                break;
            }
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        ble_hs_cfg.gatts_register_arg = ( void * ) pxService;
        ble_gatts_reset();
        ble_svc_gap_init();
        ble_svc_gatt_init();

        if( ble_gatts_count_cfg( espServices ) != 0 )
        {
            prvCleanupService( pSvc );
            configPRINTF( ( "Failed to adjust host configuration\n" ) );
            xStatus = eBTStatusFail;
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        if( ble_gatts_add_svcs( espServices ) != 0 )
        {
            prvCleanupService( pSvc );
            configPRINTF( ( "Failed to add service\n" ) );
            xStatus = eBTStatusFail;
        }
    }

    if( xStatus == eBTStatusSuccess )
    {
        if( ble_gatts_start() != 0 )
        {
            prvCleanupService( pSvc );
            configPRINTF( ( "Failed to start service\n" ) );
            xStatus = eBTStatusFail;
        }
        else
        {
            serviceCnt++;
        }
    }

    return xStatus;
}

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
                                     uint16_t usMtu )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    esp_err_t xESPStatus;

    xESPStatus = ble_att_set_preferred_mtu( usMtu );

    if( xESPStatus != ESP_OK )
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}
