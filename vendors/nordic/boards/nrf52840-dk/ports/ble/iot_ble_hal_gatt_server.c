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
/* Standard Includes. */
#include <string.h>
/* AWS BLE Lib Includes. */
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

/* Nordic/Segger Includes. */
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_log.h"
#include "nrf_ble_gatt.h"

/* Nordic port Includes. */
#include "iot_ble_hal_dis.h"

#define GATT_MAX_ENTITIES    30

uint32_t ulGattServerIFhandle;
uint16_t g_cccd_handle;
static ble_gatts_char_handles_t xLastCharAddedHanlde;
bool bInterfaceCreated = false;

static uint32_t ulGATTEvtMngHandle;
BTGattServerCallbacks_t xGattServerCb;
uint16_t usGattConnHandle = 0;

BLE_CUS_DEF( m_cus );
NRF_BLE_GATT_DEF( xGattHandler );

bool bGattInitialized = false;
const uint8_t iot_ble_hal_gatt_serverCCCD_UUID[] = { 0, 0, 0x29, 0x02, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A };
const uint16_t iot_ble_hal_gatt_serverCCCD_UUID_2BYTES = 0x2902;

/* This table is needed to overcome a restriction imposed by the softdevice API that all additions of services, characteristics and descriptors must
 * be sequential, i.e. to add a characteristic to a service you must add it immediately after you added a service. So we fill this table firstly and
 * we actually add these entities only on prvBTStartService call.
 *
 * The culprit is that we need to return the handler for an entity immediately, so in the deferred case we need to maintain our own hadlers and translate them
 * to softdevice's and back. For this purpose the lookup table is introduced with according functions */

typedef struct
{
    ble_uuid_t uuid;
    union
    {
        ble_add_char_params_t char_properties;
        ble_add_descr_params_t descr_properties;
    } properties;
    uint16_t parent_handle;
    uint16_t handle;
    BTGattAttributeType_t type;
} BTGattEntity_t;

typedef struct
{
    uint16_t handle;
    uint16_t softdevice_handle;
} BTGattMapping_t;

BTGattEntity_t xGattTable[ GATT_MAX_ENTITIES ];
BTGattMapping_t xGattToSDMapping[ GATT_MAX_ENTITIES ];

uint8_t xGattTableSize = 0;

/* TODO: Maybe implement more efficient functions for mapping  */
uint8_t xGattMappingTablesSize = 0;

/* Needed to access static gatt handler */
nrf_ble_gatt_t * prvGetGattHandle()
{
    return &xGattHandler;
}

/**
 * @brief Returns the softdevice handle for a given inner GATT handle
 * @param handle inner GATT handle
 * @return the softdevice handle
 */
uint16_t prvGattToSDHandle( uint16_t handle )
{
    #if _IOT_BLE_TOGGLE_BLOB_CREATE == 1
        return handle;
    #else
        for( uint16_t i = 0; i < xGattMappingTablesSize; ++i )
        {
            if( xGattToSDMapping[ i ].handle == handle )
            {
                return xGattToSDMapping[ i ].softdevice_handle;
            }
        }
        return UINT16_MAX;
    #endif
}

/**
 * @brief Returns the given inner GATT handle for a softdevice handle
 * @param handle softdevice handle
 * @return inner GATT handle
 */
uint16_t prvGattFromSDHandle( uint16_t handle )
{
    #if _IOT_BLE_TOGGLE_BLOB_CREATE == 1
        return handle;
    #else
        for( uint16_t i = 0; i < xGattMappingTablesSize; ++i )
        {
            if( xGattToSDMapping[ i ].softdevice_handle == handle )
            {
                return xGattToSDMapping[ i ].handle;
            }
        }
        return UINT16_MAX;
    #endif
}

/**
 * @brief Helper function to create Nordic type characteristic.
 */
static ret_code_t prvBTbuildCharacteristic( BTUuid_t * pxUuid,
                                            BTCharPermissions_t xPermissions,
                                            BTCharPermissions_t xProperties,
                                            ble_add_char_params_t * pxCharParams,
                                            ble_uuid_t * pBle_uuid );

/**
 * @brief Helper function to create Nordic type descriptor.
 */
static ret_code_t prvBTbuildDescriptor( BTUuid_t * pxUuid,
                                        BTCharPermissions_t xPermissions,
                                        ble_add_descr_params_t * pxDescrParams,
                                        ble_uuid_t * pBle_uuid );

/**
 * @brief Translate the given AFR BLE Character properties to a Nordic SDK SoftDevice Character properties
 * @param Permisions structure @ref BTCharProperties_t
 * @param Permisions structure @ref ble_gatt_char_props_t
 */
static void prvAFRToNordicCharProp( const BTCharProperties_t * pxAFRCharProps,
                                    ble_gatt_char_props_t * pxNordicCharProps );

/**
 * @brief Translate the given AFR BLE permisions for read access to a Nordic SDK SoftDevice permitions
 * @param Permisions structure @ref BTCharPermissions_t
 * @param Permisions structure @ref security_req_t
 */
static void prvAFRToNordicReadPerms( const BTCharPermissions_t * pxAFRPermitions,
                                     security_req_t * pxNordicPermitions );

/**
 * @brief Translate the given AFR BLE permisions for write access to a Nordic SDK SoftDevice permitions
 * @param Permisions structure @ref BTCharPermissions_t
 * @param Permisions structure @ref security_req_t
 */
static void prvAFRToNordicWritePerms( const BTCharPermissions_t * pxAFRPermitions,
                                      security_req_t * pxNordicPermitions );
static ret_code_t prvStartServiceAddDescriptors( uint16_t usCurrentCharNumber,
                                                 uint16_t usCCCDhandle );
static ret_code_t prvStartServiceAddCharacteristics( uint16_t usCurrentServiceNumber );
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
static BTStatus_t prvBTInvokeAttributeUuid( ble_uuid_t * pxServiceUuid,
                                            BTUuid_t * pxBaseUuid,
                                            uint8_t * pucUuidType );
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
static BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                                     BTService_t * pxService );

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
    ret_code_t xErrCode = nrf_ble_gatt_init( &xGattHandler, NULL );

    if( xErrCode == NRF_SUCCESS )
    {
        bGattInitialized = true;
    }

    xStatus = BTNRFError( xErrCode );

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
    ret_code_t xErrCode = NRF_SUCCESS;

    ble_uuid_t ble_uuid;

    /* Try to use GATT server without observers notification */
    /* ble_cus_t * p_cus = &m_cus; */

    /* Initialize service structure */
    /* p_cus->evt_handler = NULL; */
    /* p_cus->conn_handle = BLE_CONN_HANDLE_INVALID; */

    xErrCode = prvAFRUUIDtoNordic( &pxSrvcId->xId.xUuid, &ble_uuid );

    if( xErrCode == NRF_SUCCESS )
    {
        if( xGattTableSize == GATT_MAX_ENTITIES - 1 )
        {
            xErrCode = NRF_ERROR_NO_MEM;
        }
        else
        {
            xGattTable[ xGattTableSize ].type = GATT_SERVICE;
            xGattTable[ xGattTableSize ].parent_handle = 0;
            xGattTable[ xGattTableSize ].uuid = ble_uuid;
            xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 1;
            xGattTableSize += 1;
        }
    }

    BT_NRF_PRINT_ERROR( xGattTableAdd, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        if( xGattServerCb.pxServiceAddedCb )
        {
            xGattServerCb.pxServiceAddedCb( xStatus, ulGattServerIFhandle, pxSrvcId, xGattTable[ xGattTableSize - 1 ].handle );
        }

        NRF_LOG_INFO( "Service Added to HAL table with handle %d %x \n\r ", xGattTable[ xGattTableSize - 1 ].handle );
    }

    xStatus = BTNRFError( xErrCode );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTAddIncludedService( uint8_t ucServerIf,
                                    uint16_t usServiceHandle,
                                    uint16_t usIncludedHandle )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return xStatus;
}

/*-----------------------------------------------------------*/
bool prvIsCCD( BTService_t * pxService,
               uint16_t usIndex )
{
    bool xStatus = false;

    if( ( pxService->pxBLEAttributes[ usIndex ].xAttributeType == eBTDbDescriptor ) &&
        ( pxService->pxBLEAttributes[ usIndex ].xCharacteristicDescr.xUuid.ucType == eBTuuidType16 ) &&
        ( pxService->pxBLEAttributes[ usIndex ].xCharacteristicDescr.xUuid.uu.uu16 == iot_ble_hal_gatt_serverCCCD_UUID_2BYTES ) )
    {
        xStatus = true;
    }

    return xStatus;
}

BTStatus_t prvAddServiceBlob( uint8_t ucServerIf,
                              BTService_t * pxService )
{
    uint16_t usIndex = 0;
    BTStatus_t xStatus = eBTStatusSuccess;
    ble_uuid_t ble_uuid;
    ble_add_descr_params_t xDescrParams;
    ble_add_char_params_t xCharParams;
    ret_code_t xErrCode = NRF_SUCCESS;
    ble_gatts_char_handles_t xCharHandle;
    bool isCCD;

    #if _IOT_BLE_TOGGLE_BLOB_CREATE == 1
        for( usIndex = 0; usIndex < pxService->xNumberOfAttributes; usIndex++ )
        {
            switch( pxService->pxBLEAttributes[ usIndex ].xAttributeType )
            {
                case eBTDbPrimaryService:
                    xErrCode = prvAFRUUIDtoNordic( &pxService->pxBLEAttributes[ 0 ].xServiceUUID, &ble_uuid );

                    if( xErrCode == NRF_SUCCESS )
                    {
                        xErrCode = sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY,
                                                             &ble_uuid,
                                                             &pxService->pusHandlesBuffer[ 0 ] );
                    }

                    break;

                case eBTDbSecondaryService:
                    xErrCode = prvAFRUUIDtoNordic( &pxService->pxBLEAttributes[ 0 ].xServiceUUID, &ble_uuid );

                    if( xErrCode == NRF_SUCCESS )
                    {
                        xErrCode = sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_SECONDARY,
                                                             &ble_uuid,
                                                             &pxService->pusHandlesBuffer[ 0 ] );
                    }

                    break;

                case eBTDbIncludedService:
                    break;

                case eBTDbCharacteristicDecl:
                    break;

                case eBTDbCharacteristic:
                    xErrCode = prvBTbuildCharacteristic( &pxService->pxBLEAttributes[ usIndex ].xCharacteristic.xUuid,
                                                         pxService->pxBLEAttributes[ usIndex ].xCharacteristic.xPermissions,
                                                         pxService->pxBLEAttributes[ usIndex ].xCharacteristic.xProperties,
                                                         &xCharParams,
                                                         &ble_uuid );

                    isCCD = false;

                    if( usIndex + 1 < pxService->xNumberOfAttributes )
                    {
                        isCCD = prvIsCCD( pxService, usIndex + 1 );
                    }

                    if( ( xErrCode == NRF_SUCCESS ) && ( isCCD == false ) )
                    {
                        xErrCode = characteristic_add( BLE_GATT_HANDLE_INVALID,
                                                       &xCharParams,
                                                       &xCharHandle );

                        pxService->pusHandlesBuffer[ usIndex ] = xCharHandle.value_handle;
                    }

                    break;

                case eBTDbDescriptor:

                    xErrCode = prvBTbuildDescriptor( &pxService->pxBLEAttributes[ usIndex ].xCharacteristicDescr.xUuid,
                                                     pxService->pxBLEAttributes[ usIndex ].xCharacteristicDescr.xPermissions,
                                                     &xDescrParams,
                                                     &ble_uuid );

                    if( xErrCode == NRF_SUCCESS )
                    {
                        if( prvIsCCD( pxService, usIndex ) == false )
                        {
                            xErrCode = descriptor_add( BLE_GATT_HANDLE_INVALID,
                                                       &xDescrParams,
                                                       &pxService->pusHandlesBuffer[ usIndex ] );
                        }
                        else
                        {
                            if( xDescrParams.write_access == SEC_NO_ACCESS )
                            {
                                xCharParams.cccd_write_access = MAX( SEC_OPEN, xDescrParams.read_access );
                            }
                            else
                            {
                                xCharParams.cccd_write_access = xDescrParams.write_access;
                            }

                            xErrCode = characteristic_add( BLE_GATT_HANDLE_INVALID,
                                                           &xCharParams,
                                                           &xCharHandle );
                            pxService->pusHandlesBuffer[ usIndex - 1 ] = xCharHandle.value_handle;
                            pxService->pusHandlesBuffer[ usIndex ] = xCharHandle.cccd_handle;
                        }
                    }

                    break;

                default:
                    break;
            }

            if( xErrCode != NRF_SUCCESS )
            {
                break;
            }
        }

        return BTNRFError( xErrCode );
    #else /* if _IOT_BLE_TOGGLE_BLOB_CREATE == 1 */
        return eBTStatusUnsupported;
    #endif /* if _IOT_BLE_TOGGLE_BLOB_CREATE == 1 */
}

static ret_code_t prvBTbuildCharacteristic( BTUuid_t * pxUuid,
                                            BTCharPermissions_t xPermissions,
                                            BTCharProperties_t xProperties,
                                            ble_add_char_params_t * pxCharParams,
                                            ble_uuid_t * pBle_uuid )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    xErrCode = prvAFRUUIDtoNordic( pxUuid, pBle_uuid );

    if( xErrCode == NRF_SUCCESS )
    {
        memset( pxCharParams, 0, sizeof( ble_add_char_params_t ) );

        pxCharParams->uuid = pBle_uuid->uuid;
        pxCharParams->uuid_type = pBle_uuid->type;

        prvAFRToNordicCharProp( &xProperties, &pxCharParams->char_props );

        if( xProperties & eBTPropExtendedProps )
        {
            pxCharParams->char_ext_props.reliable_wr = 0; /* NOTE: There is no information in received args */
            pxCharParams->char_ext_props.wr_aux = 0;      /* NOTE: There is no information in received args */
        }

        pxCharParams->max_len = BLE_GATTS_VAR_ATTR_LEN_MAX; /* Was 180 */
        pxCharParams->init_len = sizeof( uint8_t );
        pxCharParams->is_var_len = true;
        pxCharParams->is_value_user = false;        /**< Indicate if the content of the characteristic is to be stored in the application (user) or in the stack.*/
        pxCharParams->p_init_value = NULL;          /**< Initial encoded value of the characteristic.*/
        pxCharParams->p_user_descr = NULL;          /**< Pointer to user descriptor if needed*/
        pxCharParams->p_presentation_format = NULL; /**< Pointer to characteristic format if needed*/

        pxCharParams->is_defered_read = true;       /* NOTE: This field is set due to client waits for a callback when char is read */
        pxCharParams->is_defered_write = false;

        prvAFRToNordicReadPerms( &xPermissions, &pxCharParams->read_access );
        prvAFRToNordicWritePerms( &xPermissions, &pxCharParams->write_access );
    }

    return xErrCode;
}

BTStatus_t prvBTAddCharacteristic( uint8_t ucServerIf,
                                   uint16_t usServiceHandle,
                                   BTUuid_t * pxUuid,
                                   BTCharProperties_t xProperties,
                                   BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xErrCode = NRF_SUCCESS;
    ble_add_char_params_t add_char_params;
    ble_uuid_t ble_uuid;

    xErrCode = prvBTbuildCharacteristic( pxUuid, xPermissions, xProperties, &add_char_params, &ble_uuid );

    if( xErrCode == NRF_SUCCESS )
    {
        if( xGattTableSize == GATT_MAX_ENTITIES - 1 )
        {
            xErrCode = NRF_ERROR_NO_MEM;
        }
        else
        {
            xGattTable[ xGattTableSize ].type = GATT_CHAR;
            xGattTable[ xGattTableSize ].parent_handle = UINT16_MAX;

            for( int i = 0; i < xGattTableSize; ++i )
            {
                if( ( xGattTable[ i ].type == GATT_SERVICE ) && ( xGattTable[ i ].handle == usServiceHandle ) )
                {
                    xGattTable[ xGattTableSize ].parent_handle = xGattTable[ i ].handle;
                    break;
                }
            }

            if( xGattTable[ xGattTableSize ].parent_handle == UINT16_MAX )
            {
                xErrCode = NRF_ERROR_INVALID_PARAM;
            }
            else
            {
                xGattTable[ xGattTableSize ].uuid = ble_uuid;
                xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 2;
                xGattTable[ xGattTableSize ].properties.char_properties = add_char_params;
                xGattTableSize += 1;
            }
        }
    }

    BT_NRF_PRINT_ERROR( characteristic_add, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        NRF_LOG_INFO( "Charachtersitics Added to HAL table with handle %d %x \n\r", xGattTable[ xGattTableSize - 1 ].handle );

        if( xGattServerCb.pxCharacteristicAddedCb )
        {
            xGattServerCb.pxCharacteristicAddedCb( xStatus,
                                                   ulGattServerIFhandle,
                                                   pxUuid,
                                                   xGattTable[ xGattTableSize - 1 ].parent_handle,
                                                   xGattTable[ xGattTableSize - 1 ].handle );
        }
    }

    return BTNRFError( xErrCode );
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTSetVal( BTGattResponse_t * pxValue )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ble_gatts_value_t xValueParams;
    ret_code_t xErrCode;

    memset( &xValueParams, 0, sizeof( xValueParams ) );
    xValueParams.len = pxValue->xAttrValue.xLen;
    xValueParams.offset = pxValue->xAttrValue.usOffset;
    xValueParams.p_value = pxValue->xAttrValue.pucValue;

    xErrCode = sd_ble_gatts_value_set( usGattConnHandle, prvGattToSDHandle( pxValue->usHandle ), &xValueParams );

    if( xErrCode != NRF_SUCCESS )
    {
        NRF_LOG_ERROR( "Error %d HVX \n\r", xErrCode );
    }

    xStatus = BTNRFError( xErrCode );
    return xStatus;
}

/*-----------------------------------------------------------*/
static ret_code_t prvBTbuildDescriptor( BTUuid_t * pxUuid,
                                        BTCharPermissions_t xPermissions,
                                        ble_add_descr_params_t * pxDescrParams,
                                        ble_uuid_t * pBle_uuid )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    memset( pxDescrParams, 0, sizeof( ble_add_descr_params_t ) );

    /* Try to use GATT server without observers notification */
    /* ble_cus_t * p_cus = &m_cus; */
    pxDescrParams->max_len = BLE_GATTS_VAR_ATTR_LEN_MAX; /* or BLE_GATTS_FIX_ATTR_LEN_MAX or any smaller value * / */

    /* Check if we try to add a CCCD */
    if( ( ( pxUuid->ucType == eBTuuidType128 ) && ( memcmp( pxUuid->uu.uu128, iot_ble_hal_gatt_serverCCCD_UUID, 16 ) == 0 ) ) ||
        ( ( pxUuid->ucType == eBTuuidType16 ) && ( pxUuid->uu.uu16 == iot_ble_hal_gatt_serverCCCD_UUID_2BYTES ) ) )
    {
        pBle_uuid->type = BLE_UUID_TYPE_BLE;
        pBle_uuid->uuid = BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG;
        pxDescrParams->max_len = 50; /* or BLE_GATTS_FIX_ATTR_LEN_MAX or any smaller value * / */
    }
    else
    {
        xErrCode = prvAFRUUIDtoNordic( pxUuid, pBle_uuid );
    }

    if( xErrCode == NRF_SUCCESS )
    {
        pxDescrParams->uuid_type = pBle_uuid->type;
        pxDescrParams->uuid = pBle_uuid->uuid;

        pxDescrParams->is_var_len = true;
        pxDescrParams->max_len = BLE_GATTS_VAR_ATTR_LEN_MAX; /* or BLE_GATTS_FIX_ATTR_LEN_MAX or any smaller value * / */
        pxDescrParams->init_len = sizeof( uint8_t );
        pxDescrParams->is_value_user = false;

        pxDescrParams->is_defered_read = true; /* NOTE: This field is set due to client waits for a callback when desc is read */
        pxDescrParams->is_defered_write = false;

        prvAFRToNordicReadPerms( &xPermissions, &pxDescrParams->read_access );
        prvAFRToNordicWritePerms( &xPermissions, &pxDescrParams->write_access );
    }

    return xErrCode;
}

BTStatus_t prvBTAddDescriptor( uint8_t ucServerIf,
                               uint16_t usServiceHandle,
                               BTUuid_t * pxUuid,
                               BTCharPermissions_t xPermissions )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xErrCode = NRF_SUCCESS;
    ble_uuid_t ble_uuid;
    ble_add_descr_params_t xDescrParams;

    xErrCode = prvBTbuildDescriptor( pxUuid, xPermissions, &xDescrParams, &ble_uuid );

    if( xErrCode == NRF_SUCCESS )
    {
        if( xGattTableSize == GATT_MAX_ENTITIES - 1 )
        {
            xErrCode = NRF_ERROR_NO_MEM;
        }
        else
        {
            xGattTable[ xGattTableSize ].type = GATT_DESCR;
            /* We sets the parent of the descriptor to be the latest added characteristic which belongs to the given service */
            xGattTable[ xGattTableSize ].parent_handle = UINT16_MAX;
            uint16_t usParentNumber = UINT16_MAX;

            for( int16_t i = ( int16_t ) xGattTableSize - 1; i >= 0; --i )
            {
                if( ( xGattTable[ i ].type == GATT_CHAR ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) )
                {
                    usParentNumber = i;
                    xGattTable[ xGattTableSize ].parent_handle = xGattTable[ i ].handle;
                    break;
                }
            }

            /* Set the correct permissions for the responding characteristics CCCD
             * It cannot be SEC_NO_ACCESS as CCCD should always be readable and writable
             */
            if( ( ble_uuid.type == BLE_UUID_TYPE_BLE ) &&
                ( ble_uuid.uuid == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG ) &&
                ( usParentNumber != UINT16_MAX ) )
            {
                if( xDescrParams.write_access == SEC_NO_ACCESS )
                {
                    xGattTable[ usParentNumber ].properties.char_properties.cccd_write_access = MAX( SEC_OPEN, xDescrParams.read_access );
                }
                else
                {
                    xGattTable[ usParentNumber ].properties.char_properties.cccd_write_access = xDescrParams.write_access;
                }
            }

            if( xGattTable[ xGattTableSize ].parent_handle == UINT16_MAX )
            {
                xErrCode = NRF_ERROR_INVALID_PARAM;
            }
            else
            {
                xGattTable[ xGattTableSize ].uuid = ble_uuid;
                xGattTable[ xGattTableSize ].handle = xGattTableSize == 0 ? 1 : xGattTable[ xGattTableSize - 1 ].handle + 1;
                xGattTable[ xGattTableSize ].properties.descr_properties = xDescrParams;
                xGattTableSize += 1;
            }
        }
    }

    BT_NRF_PRINT_ERROR( xGattTableDescr, xErrCode );

    xStatus = BTNRFError( xErrCode );

    if( xStatus == eBTStatusSuccess )
    {
        NRF_LOG_INFO( "Descr Added to HAL table with handle %d %x \n\r", xGattTable[ xGattTableSize - 1 ].handle );

        if( xGattServerCb.pxDescriptorAddedCb )
        {
            xGattServerCb.pxDescriptorAddedCb( xStatus,
                                               ulGattServerIFhandle,
                                               pxUuid,
                                               usServiceHandle,
                                               xGattTable[ xGattTableSize - 1 ].handle );
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

ret_code_t prvStartServiceAddDescriptors( uint16_t usCurrentCharNumber,
                                          uint16_t usCCCDhandle )
{
    ret_code_t xErrCode = NRF_SUCCESS;

    for( int i = 0; i < xGattTableSize; ++i )
    {
        if( ( xGattTable[ i ].type == GATT_DESCR ) && ( xGattTable[ i ].parent_handle == xGattTable[ usCurrentCharNumber ].handle ) ) /* Add descriptors */
        {
            /* Check if it is a CCCD handler as for indications and notifications it is registered automatically and we don't need to add it */
            if( ( usCCCDhandle != BLE_GATT_HANDLE_INVALID ) &&
                ( xGattTable[ i ].uuid.type == BLE_UUID_TYPE_BLE ) &&
                ( xGattTable[ i ].uuid.uuid == BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG ) )
            {
                xGattToSDMapping[ xGattMappingTablesSize ].handle = xGattTable[ i ].handle;
                xGattToSDMapping[ xGattMappingTablesSize ].softdevice_handle = usCCCDhandle;
            }
            else
            {
                /* Non-sequential addition doesn't work in current versions of NRF5 SDK,
                 * if it is added, replace BLE_GATT_HANDLE_INVALID with (prvGattToSDHandle( xGattTable[ i ].parent_handle) */
                xErrCode = descriptor_add( BLE_GATT_HANDLE_INVALID,
                                           &xGattTable[ i ].properties.descr_properties,
                                           &xGattToSDMapping[ xGattMappingTablesSize ].softdevice_handle );
                xGattToSDMapping[ xGattMappingTablesSize ].handle = xGattTable[ i ].handle;
            }

            xGattMappingTablesSize += 1;

            if( xErrCode != NRF_SUCCESS )
            {
                break;
            }

            NRF_LOG_INFO( "Descriptor Added to SD table with handle %d %x \n\r",
                          xGattToSDMapping[ xGattMappingTablesSize - 1 ].softdevice_handle );
        }
    }

    BT_NRF_PRINT_ERROR( prvStartServiceAddDescriptors, xErrCode );

    return xErrCode;
}

ret_code_t prvStartServiceAddCharacteristics( uint16_t usCurrentServiceNumber )
{
    ret_code_t xErrCode = NRF_SUCCESS;
    uint16_t usServiceHandle = xGattTable[ usCurrentServiceNumber ].handle;

    for( int i = 0; i < xGattTableSize; ++i )
    {
        if( ( xGattTable[ i ].type == GATT_CHAR ) && ( xGattTable[ i ].parent_handle == usServiceHandle ) )
        {
            ble_gatts_char_handles_t sd_handle;
            uint16_t usSoftDeviceServiceHandle = prvGattToSDHandle( usServiceHandle );

            if( usSoftDeviceServiceHandle == UINT16_MAX )
            {
                xErrCode = NRF_ERROR_INVALID_PARAM;
            }
            else
            {
                xErrCode = characteristic_add( usSoftDeviceServiceHandle,
                                               &xGattTable[ i ].properties.char_properties,
                                               &sd_handle );
                xGattToSDMapping[ xGattMappingTablesSize ].handle = xGattTable[ i ].handle;
                xGattToSDMapping[ xGattMappingTablesSize ].softdevice_handle = sd_handle.value_handle;
                xGattMappingTablesSize += 1;
            }

            if( xErrCode != NRF_SUCCESS )
            {
                break;
            }

            NRF_LOG_INFO( "Charachtersitic Added to SD table with handle %d %x \n\r",
                          xGattToSDMapping[ xGattMappingTablesSize - 1 ].softdevice_handle );

            xErrCode = prvStartServiceAddDescriptors( i, sd_handle.cccd_handle );

            if( xErrCode != NRF_SUCCESS )
            {
                break;
            }
        }
    }

    BT_NRF_PRINT_ERROR( prvStartServiceAddCharacteristics, xErrCode );

    return xErrCode;
}

BTStatus_t prvBTStartService( uint8_t ucServerIf,
                              uint16_t usServiceHandle,
                              BTTransport_t xTransport )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xErrCode = NRF_SUCCESS;
    uint16_t usCurrentServiceNumber = UINT16_MAX;

    for( uint16_t i = 0; i < xGattTableSize; ++i )
    {
        if( ( xGattTable[ i ].type == GATT_SERVICE ) && ( xGattTable[ i ].handle == usServiceHandle ) )
        {
            usCurrentServiceNumber = i;
            break;
        }
    }

    if( usCurrentServiceNumber == UINT16_MAX )
    {
        xErrCode = NRF_ERROR_INVALID_PARAM;
    }

    if( xErrCode == NRF_SUCCESS )
    {
        xErrCode = sd_ble_gatts_service_add( BLE_GATTS_SRVC_TYPE_PRIMARY,
                                             &xGattTable[ usCurrentServiceNumber ].uuid,
                                             &xGattToSDMapping[ xGattMappingTablesSize ].softdevice_handle );
        xGattToSDMapping[ xGattMappingTablesSize ].handle = usServiceHandle;
        xGattMappingTablesSize += 1;
    }

    BT_NRF_PRINT_ERROR( prvBTStartService, xErrCode );

    if( xErrCode == NRF_SUCCESS )
    {
        NRF_LOG_INFO( "Service Added to SD table with handle %d %x \n\r ",
                      xGattToSDMapping[ xGattMappingTablesSize - 1 ].softdevice_handle );

        xErrCode = prvStartServiceAddCharacteristics( usCurrentServiceNumber );
    }

    xStatus = BTNRFError( xErrCode );

    if( xGattServerCb.pxServiceStartedCb != NULL )
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

    /* It is not supported to delete service without reenabling the SoftDevice, so we just return success.
     * https://devzone.nordicsemi.com/f/nordic-q-a/16899/removing-gatt-services
     * TODO: Wait till this functionality is added
     */
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
    BTStatus_t xStatus = eBTStatusSuccess;
    uint32_t err;
    uint16_t len;

    ble_gatts_hvx_params_t hvx_params;

    memset( &hvx_params, 0, sizeof( hvx_params ) );
    hvx_params.handle = prvGattToSDHandle( usAttributeHandle );
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
        xStatus = eBTStatusFail;
        NRF_LOG_ERROR( 0, "Error %d HVX \n\r", err );
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
    ret_code_t xErrCode = NRF_SUCCESS;
    ble_gatts_rw_authorize_reply_params_t authreply;

    ble_gatts_value_t writereply;

    switch( ulTransId )
    {
        case BLE_GATTS_AUTHORIZE_TYPE_READ:
            authreply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;

            authreply.params.read.len = pxResponse->xAttrValue.xLen;
            authreply.params.read.offset = pxResponse->xAttrValue.usOffset;
            authreply.params.read.p_data = pxResponse->xAttrValue.pucValue;

            authreply.params.read.update = 1; /* NOTE: This flag is considered only on write event */

            if( xStatus == eBTStatusSuccess )
            {
                authreply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
            }
            else
            {
                authreply.params.read.gatt_status = BLE_GATT_STATUS_ATTERR_READ_NOT_PERMITTED;
            }

            xErrCode = sd_ble_gatts_rw_authorize_reply( usConnId, &authreply );

            if( xErrCode != 0 )
            {
                NRF_LOG_ERROR( "Error %d prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_READ \n\r", xErrCode );
            }

            break;

        case BLE_GATTS_AUTHORIZE_TYPE_WRITE:
            authreply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;

            authreply.params.write.len = pxResponse->xAttrValue.xLen;
            authreply.params.write.offset = pxResponse->xAttrValue.usOffset;
            authreply.params.write.p_data = pxResponse->xAttrValue.pucValue;

            authreply.params.write.update = 1;

            if( xStatus == eBTStatusSuccess )
            {
                authreply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
            }
            else
            {
                authreply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_WRITE_NOT_PERMITTED;
            }

            xErrCode = sd_ble_gatts_rw_authorize_reply( usConnId, &authreply );

            if( xErrCode != NRF_SUCCESS )
            {
                NRF_LOG_ERROR( "Error %d prvBTSendResponse - BLE_GATTS_AUTHORIZE_TYPE_WRITE \n\r", xErrCode );
            }

            break;

        case 0:
            xStatus = eBTStatusSuccess;

            break;

        default:
            ;
    }

    xReturnStatus = BTNRFError( xErrCode );

    if( xGattServerCb.pxResponseConfirmationCb != NULL )
    {
        xGattServerCb.pxResponseConfirmationCb( xStatus, pxResponse->usHandle );
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

const void * prvBTGetGattServerInterface()
{
    return &xGATTserverInterface;
}

/*-----------------------------------------------------------*/

static void prvAFRToNordicCharProp( const BTCharProperties_t * pxAFRCharProps,
                                    ble_gatt_char_props_t * pxSoftDevCharProps )
{
    if( *pxAFRCharProps & eBTPropBroadcast )
    {
        pxSoftDevCharProps->broadcast = 1;
    }

    if( *pxAFRCharProps & eBTPropRead )
    {
        pxSoftDevCharProps->read = 1;
    }

    if( *pxAFRCharProps & eBTPropWriteNoResponse )
    {
        pxSoftDevCharProps->write_wo_resp = 1;
    }

    if( *pxAFRCharProps & eBTPropWrite )
    {
        pxSoftDevCharProps->write = 1;
    }

    if( *pxAFRCharProps & eBTPropNotify )
    {
        pxSoftDevCharProps->notify = 1;
    }

    if( *pxAFRCharProps & eBTPropIndicate )
    {
        pxSoftDevCharProps->indicate = 1;
    }

    if( *pxAFRCharProps & eBTPropSignedWrite )
    {
        pxSoftDevCharProps->auth_signed_wr = 1;
    }
}

/*-----------------------------------------------------------*/

static void prvAFRToNordicReadPerms( const BTCharPermissions_t * pxAFRPermitions,
                                     security_req_t * pxNordicPermitions )
{
    switch( *pxAFRPermitions & 0x0007 )
    {
        case eBTPermNone:
            *pxNordicPermitions = SEC_NO_ACCESS;
            break;

        case eBTPermRead:
            *pxNordicPermitions = SEC_OPEN;
            break;

        case eBTPermReadEncrypted:
            *pxNordicPermitions = SEC_JUST_WORKS;
            break;

        case eBTPermReadEncryptedMitm:
            *pxNordicPermitions = SEC_MITM;
            break;

        default:
            *pxNordicPermitions = SEC_NO_ACCESS;
    }
}

/*-----------------------------------------------------------*/

static void prvAFRToNordicWritePerms( const BTCharPermissions_t * pxAFRPermitions,
                                      security_req_t * pxNordicPermitions )
{
    switch( *pxAFRPermitions & 0x01F0 )
    {
        case eBTPermNone:
            *pxNordicPermitions = SEC_NO_ACCESS;
            break;

        case eBTPermWrite:
            *pxNordicPermitions = SEC_OPEN;
            break;

        case eBTPermWriteEncrypted:
            *pxNordicPermitions = SEC_JUST_WORKS;
            break;

        case eBTPermWriteEncryptedMitm:
            *pxNordicPermitions = SEC_MITM;
            break;

        case eBTPermWriteSigned:
            *pxNordicPermitions = SEC_SIGNED;
            break;

        case eBTPermWriteSignedMitm:
            *pxNordicPermitions = SEC_SIGNED_MITM;
            break;

        default:
            *pxNordicPermitions = SEC_NO_ACCESS;
    }
}

static BTStatus_t prvBTConfigureMtu( uint8_t ucServerIf,
                                     uint16_t usMtu )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    ret_code_t xNRFstatus = NRF_SUCCESS;

    nrf_ble_gatt_t * pxGattHandler = prvGetGattHandle();

    xNRFstatus = nrf_ble_gatt_att_mtu_periph_set( pxGattHandler, usMtu );
    xStatus = BTNRFError( xNRFstatus );
    return xStatus;
}
