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

static struct ble_gatt_access_ctxt g_ctxt;

static SemaphoreHandle_t xSem;



void prvGattGetSemaphore()
{
    xSemaphoreTake( xSem, portMAX_DELAY );
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
    uint16_t out_len = 0;
    uint8_t dst_buf[ 512 ] = { 0 }; /* fixme: Check allocation on stack */

    ble_gap_conn_find( conn_handle, &desc );

    switch( ctxt->op )
    {
        case BLE_GATT_ACCESS_OP_READ_CHR:
        case BLE_GATT_ACCESS_OP_READ_DSC:
            ESP_LOGI( TAG, "In read for handle %d", attr_handle );
            memcpy( &g_ctxt, ctxt, sizeof( g_ctxt ) );

            if( xGattServerCb.pxRequestReadCb != NULL )
            {
                xGattServerCb.pxRequestReadCb( conn_handle, ( uint32_t ) ctxt, ( BTBdaddr_t * ) desc.peer_id_addr.val, attr_handle, 0 );
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

            if( xGattServerCb.pxRequestWriteCb != NULL )
            {
                memcpy( &g_ctxt, ctxt, sizeof( g_ctxt ) );

                if( ( ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR ) && ( ctxt->chr->flags & BLE_GATT_CHR_F_WRITE_NO_RSP ) )
                {
                    need_rsp = 0;
                }

                if( xGattServerCb.pxRequestWriteCb != NULL )
                {
                    xGattServerCb.pxRequestWriteCb( conn_handle, ( uint32_t ) ctxt, ( BTBdaddr_t * ) desc.peer_id_addr.val, attr_handle, 0, out_len, need_rsp, 0, dst_buf );
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
    BTService_t * pxService = (BTService_t *)arg;
    char buf[ BLE_UUID_STR_LEN ];
    uint16_t offset = pxService->pusHandlesBuffer[0];

    switch( ctxt->op )
    {
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGD( TAG, "registered service %s with handle=%d\n",
                         ble_uuid_to_str( ctxt->svc.svc_def->uuid, buf ),
                         ctxt->svc.handle );
            pxService->pusHandlesBuffer[0] = ctxt->svc.handle;
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            ESP_LOGD( TAG, "registered characteristic %s with "
                                "def_handle=%d val_handle=%d\n",
                         ble_uuid_to_str( ctxt->chr.chr_def->uuid, buf ),
                         ctxt->chr.def_handle,
                         ctxt->chr.val_handle );
            pxService->pusHandlesBuffer[ctxt->chr.val_handle - offset] = ctxt->chr.val_handle;

            break;

        case BLE_GATT_REGISTER_OP_DSC:
            ESP_LOGD( TAG, "registered descriptor %s with handle=%d\n",
                         ble_uuid_to_str( ctxt->dsc.dsc_def->uuid, buf ),
                         ctxt->dsc.handle );

            if( ctxt->dsc.dsc_def->arg )
            {
            	pxService->pusHandlesBuffer[ctxt->dsc.handle - offset] = ctxt->dsc.handle;
            }

            break;

        default:
            assert( 0 );
            break;
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
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDeleteService( uint8_t ucServerIf,
                               uint16_t usServiceHandle )
{
    return eBTStatusUnsupported;
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
        ESP_LOGI( TAG, "Send Indications" );
        xESPstatus = ble_gattc_indicate_custom( usConnId, usAttributeHandle, om );
    }
    else
    {
        ESP_LOGI( TAG, "Send Notifications" );
        xESPstatus = ble_gattc_notify_custom( usConnId, usAttributeHandle, om );
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
uint16_t prvCountCharacteristics( BTService_t * pxService )
{
	uint16_t nbCharacteristics = 0;
	uint16_t index;

    for( index = 0; index < pxService->xNumberOfAttributes; index++ )
    {
    	if(pxService->pxBLEAttributes[index].xAttributeType == eBTDbCharacteristic)
    	{
    		nbCharacteristics++;
    	}
    }

	return nbCharacteristics;
}

uint16_t prvCountDescriptor( BTService_t * pxService, uint16_t startHandle )
{
	uint16_t nbDescriptor = 0;
	uint16_t index;

    for( index = startHandle; index < pxService->xNumberOfAttributes; index++ )
    {
    	if(pxService->pxBLEAttributes[index].xAttributeType == eBTDbDescriptor)
    	{
    		nbDescriptor++;
    	}else if(pxService->pxBLEAttributes[index].xAttributeType == eBTDbCharacteristic)
    	{
    		break;
    	}
    }

	return nbDescriptor;
}

BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                              BTService_t * pxService )
{
	uint16_t index;
	uint16_t charCount;
	uint16_t dscrCount;
	struct ble_gatt_svc_def svc;
    struct ble_gatt_chr_def *characteristics;
    struct ble_gatt_dsc_def *descriptor;
    BTStatus_t xReturnStatus = eBTStatusSuccess;

	if(pxService->pxBLEAttributes[index].xAttributeType == eBTDbPrimaryService)
	{
		svc.type = BLE_GATT_SVC_TYPE_PRIMARY;
	}else if(pxService->pxBLEAttributes[index].xAttributeType == eBTDbPrimaryService)
	{
		svc.type = BLE_GATT_SVC_TYPE_SECONDARY;
	}else
	{
		//unsupported
	}

	svc.uuid = prvCopytoESPUUID(&pxService->pxBLEAttributes[0].xServiceUUID);


	svc.characteristics = pvPortCalloc( prvCountCharacteristics(pxService), sizeof( struct ble_gatt_chr_def ) );
	charCount = 0;

    for( index = 0; index < pxService->xNumberOfAttributes; index++ )
    {
    	switch(pxService->pxBLEAttributes[index].xAttributeType)
    	{
    	case eBTDbCharacteristic:
        	/* Allocate memory for UUID and copies it. */
        	svc.characteristics[charCount].uuid = prvCopytoESPUUID(&pxService->pxBLEAttributes[index].xCharacteristic.xUuid);
        	svc.characteristics[charCount].arg = (void *)pxService;
        	svc.characteristics[charCount].access_cb = prvGATTCharAccessCb;
        	svc.characteristics[charCount].flags = prvAFRToESPCharPerm( pxService->pxBLEAttributes[index].xCharacteristic.xProperties, pxService->pxBLEAttributes[index].xCharacteristic.xPermissions );

        	/* Allocate memory for descriptors. */
        	svc.characteristics[charCount].descriptors = pvPortCalloc( prvCountDescriptor(pxService, index+1), sizeof( struct ble_gatt_dsc_def ) );
        	charCount++;
        	dscrCount = 0;
    		break;
    	case eBTDbDescriptor:
        	/* Allocate memory for UUID and copies it. */
    		descriptor = &svc.characteristics[charCount - 1].descriptors[dscrCount];
    		/* Allocate memory for UUID and copies it. */
    		descriptor->uuid = prvCopytoESPUUID(&pxService->pxBLEAttributes[index].xCharacteristicDescr.xUuid);
    		descriptor->arg = (void *)pxService;
    		descriptor->access_cb = prvGATTCharAccessCb;
    		descriptor->att_flags = prvAFRToESPDescPerm( pxService->pxBLEAttributes[index].xCharacteristicDescr.xPermissions );

    		dscrCount++;
    		break;
    	default:
    		break;
    	}
    }
    ble_gatts_reset();
    ble_svc_gap_init();
    ble_svc_gatt_init();
	ble_gatts_count_cfg( &svc );
	ble_gatts_add_svcs( &svc );
    ble_gatts_start();

    charCount = 0;
    for( index = 0; index < pxService->xNumberOfAttributes; index++ )
    {
    	switch(pxService->pxBLEAttributes[index].xAttributeType)
    	{
    	case eBTDbCharacteristic:
        	/* Allocate memory for UUID and copies it. */
        	vPortFree( svc.characteristics[charCount].uuid );
        	vPortFree( svc.characteristics[charCount].descriptors );
        	charCount++;
        	dscrCount = 0;
    		break;
    	case eBTDbDescriptor:
        	/* Allocate memory for UUID and copies it. */
    		descriptor = &svc.characteristics[charCount - 1].descriptors[dscrCount];
    		/* Allocate memory for UUID and copies it. */
    		vPortFree( descriptor->uuid );
    		dscrCount++;
    		break;
    	default:
    		break;
    	}
    }

    vPortFree(svc.characteristics);

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}
