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
 * @file iot_ble_hal_manager_adapter_ble.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */
#include "iot_config.h"
#include "iot_ble_config.h"

/* C standard library includes */
#include <stddef.h>
#include <string.h>

/* FreeRTOS includes */
#include <FreeRTOS.h>
#include <queue.h>

/* BLE HAL related files */
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "iot_ble_hal_internals.h"

/* BLE-Stack API definitions: Must be the last file in list of stack includes */
#include <icall_ble_api.h>

/*-----------------------------------------------------------*/

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL                         IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL                         IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME                              ( "BLE_HAL_GAP" )
#include "iot_logging_setup.h"

/* The following sizes below are in bytes */
#define BLE_HAL_MANAGER_ADVERTISING_BUFFER_SIZE             ( 31 )
#define BLE_HAL_MANAGER_ADV_COMPANY_ID_SIZE                 ( 2 )
#define BLE_HAL_MANAGER_ADV_OPCODE_SIZE                     ( 1 )
#define BLE_HAL_MANAGER_ADV_HEADER_LEN                      ( BLE_HAL_MANAGER_ADV_OPCODE_SIZE + 1 )
#define BLE_HAL_MANAGER_ADV_APPEARANCE_LEN                  ( 2 )
#define BLE_HAL_MANAGER_ADV_FLAGS_LEN                       ( 1 )
#define BLE_HAL_MANAGER_ADV_PERI_PREFERRED_CONN_PARAMS_LEN  ( 4 )
#define BLE_HAL_MANAGER_ADV_MAX_QUEUE_DEPTH                 ( 5 )

/* Default advertising params */
#define GAPADV_PARAMS_LEGACY_SCANN_CONN                                    \
{                                                                          \
  .eventProps = GAP_ADV_PROP_CONNECTABLE |                                 \
                GAP_ADV_PROP_SCANNABLE |                                   \
                GAP_ADV_PROP_LEGACY,                                       \
  .primIntMin = 160,                                                       \
  .primIntMax = 160,                                                       \
  .primChanMap = GAP_ADV_CHAN_ALL,                                         \
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,                       \
  .peerAddr = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },                      \
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,                               \
  .txPower = GAP_ADV_TX_POWER_NO_PREFERENCE,                               \
  .primPhy = GAP_ADV_PRIM_PHY_1_MBPS,                                      \
  .secPhy = GAP_ADV_SEC_PHY_1_MBPS,                                        \
  .sid = 0,                                                                \
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks );
static BTStatus_t _prvBTRegisterBleApp( BTUuid_t * pxAppUuid );
static BTStatus_t _prvBTUnregisterBleApp( uint8_t ucAdapterIf );
static BTStatus_t _prvBTGetBleAdapterProperty( BTBlePropertyType_t xType );
static BTStatus_t _prvBTSetBleAdapterProperty( const BTBleProperty_t * pxProperty );
static BTStatus_t _prvBTGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr );
static BTStatus_t _prvBTGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                   BTBleProperty_t xType );
static BTStatus_t _prvBTSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                   const BTBleProperty_t * pxProperty );
static BTStatus_t _prvBTScan( bool bStart );
static BTStatus_t _prvBTConnect( uint8_t ucAdapterIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 bool bIsDirect,
                                 BTTransport_t ulTransport );
static BTStatus_t _prvBTDisconnect( uint8_t ucAdapterIf,
                                   const BTBdaddr_t * pxBdAddr,
                                   uint16_t usConnId );
static BTStatus_t _prvBTStartAdv( uint8_t ucAdapterIf );
static BTStatus_t _prvBTStopAdv( uint8_t ucAdapterIf );
static BTStatus_t _prvBTReadRemoteRssi( uint8_t ucAdapterIf,
                                       const BTBdaddr_t * pxBdAddr );
static BTStatus_t _prvBTScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam );
static BTStatus_t _prvBTScanFilterAddRemove( uint8_t ucAdapterIf,
                                             uint32_t ulAction,
                                             uint32_t ulFiltType,
                                             uint32_t ulFiltIndex,
                                             uint32_t ulCompanyId,
                                             uint32_t ulCompanyIdMask,
                                             const BTUuid_t * pxUuid,
                                             const BTUuid_t * pxUuidMask,
                                             const BTBdaddr_t * pxBdAddr,
                                             char cAddrType,
                                             size_t xDataLen,
                                             char * pcData,
                                             size_t xMaskLen,
                                             char * pcMask );
static BTStatus_t _prvBTScanFilterEnable( uint8_t ucAdapterIf,
                                         bool bEnable );
static BTTransport_t _prvBTGetDeviceType( const BTBdaddr_t * pxBdAddr );
static BTStatus_t _prvBTSetAdvData( uint8_t ucAdapterIf,
                                    BTGattAdvertismentParams_t * pxParams,
                                    uint16_t usManufacturerLen,
                                    char * pcManufacturerData,
                                    uint16_t usServiceDataLen,
                                    char * pcServiceData,
                                    BTUuid_t * pxServiceUuid,
                                    size_t xNbServices );
static BTStatus_t _prvBTSetAdvRawData( uint8_t ucAdapterIf,
                                       uint8_t * pucData,
                                       uint8_t ucLen );
static BTStatus_t _prvBTConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                                    uint32_t ulMinInterval,
                                                    uint32_t ulMaxInterval,
                                                    uint32_t ulLatency,
                                                    uint32_t ulTimeout );
static BTStatus_t _prvBTSetScanParameters( uint8_t ucAdapterIf,
                                           uint32_t ulScanInterval,
                                           uint32_t ulScanWindow );
static BTStatus_t _prvBTMultiAdvEnable( uint8_t ucAdapterIf,
                                        BTGattAdvertismentParams_t * xAdvParams );
static BTStatus_t _prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                        BTGattAdvertismentParams_t * advParams );
static BTStatus_t _prvBTMultiAdvSetInstData( uint8_t ucAdapterIf,
                                             bool bSetScanRsp,
                                             bool bIncludeName,
                                             bool bInclTxpower,
                                             uint32_t ulAppearance,
                                             size_t xManufacturerLen,
                                             char * pcManufacturerData,
                                             size_t xServiceDataLen,
                                             char * pcServiceData,
                                             BTUuid_t * pxServiceUuid,
                                             size_t xNbServices );
static BTStatus_t _prvBTMultiAdvDisable( uint8_t ucAdapterIf );
static BTStatus_t _prvBTBatchscanCfgStorage( uint8_t ucAdapterIf,
                                             uint32_t ulBatchScanFullMax,
                                             uint32_t ulBatchScanTruncMax,
                                             uint32_t ulBatchScanNotifyThreshold );
static BTStatus_t _prvBTBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                               uint32_t ulScanMode,
                                               uint32_t ulScanInterval,
                                               uint32_t ulScanWindow,
                                               uint32_t ulAddrType,
                                               uint32_t ulDiscardRule );
static BTStatus_t _prvBTBatchscanDisBatchScan( uint8_t ucAdapterIf );
static BTStatus_t _prvBTBatchscanReadReports( uint8_t ucAdapterIf,
                                              uint32_t ulScanMode );
static BTStatus_t _prvBTSetPreferredPhy( uint16_t usConnId,
                                         uint8_t ucTxPhy,
                                         uint8_t ucRxPhy,
                                         uint16_t usPhyOptions );
static BTStatus_t _prvBTReadPhy( uint16_t usConnId,
                                 BTReadClientPhyCallback_t xCb );
static const void * _prvBTGetGattClientInterface( void );
extern const void * _prvBTGetGattServerInterface( void );
static BTStatus_t _prvBTFillAdvBuffer( uint8_t *pucBufferToFill,
                                       BTGattAdvertismentParams_t * pxParams,
                                       uint16_t usManufacturerLen,
                                       char * pcManufacturerData,
                                       uint16_t usServiceDataLen,
                                       char * pcServiceData,
                                       BTUuid_t * pxServiceUuid,
                                       size_t xNbServices );
static void _prvAdvCallback( uint32_t event, void *pBuf, uintptr_t arg );

/*-----------------------------------------------------------*/

static GapAdv_params_t xAdvParams = GAPADV_PARAMS_LEGACY_SCANN_CONN;
static uint8_t *pucAdvData = NULL;
static uint16_t usAdvDataLen = 0;
static uint8_t *pucScanRspData = NULL;
static uint16_t usScanRspDataLen = 0;
static uint16 usAdvTimeout = 0;
static uint8_t ucAdvHandle;
static bool bGapInitialized = false;

/* This queue is used to signal the user task of events from _prvAdvCallback  */
static QueueHandle_t xAdvStateQueue;

static BTBleAdapter_t xBTLeAdapter =
{
    .pxBleAdapterInit                  = _prvBTBleAdapterInit,
    .pxRegisterBleApp                  = _prvBTRegisterBleApp,
    .pxUnregisterBleApp                = _prvBTUnregisterBleApp,
    .pxGetBleAdapterProperty           = _prvBTGetBleAdapterProperty,
    .pxSetBleAdapterProperty           = _prvBTSetBleAdapterProperty,
    .pxGetallBleRemoteDeviceProperties = _prvBTGetallBleRemoteDeviceProperties,
    .pxGetBleRemoteDeviceProperty      = _prvBTGetBleRemoteDeviceProperty,
    .pxSetBleRemoteDeviceProperty      = _prvBTSetBleRemoteDeviceProperty,
    .pxScan                            = _prvBTScan,
    .pxConnect                         = _prvBTConnect,
    .pxDisconnect                      = _prvBTDisconnect,
    .pxStartAdv                        = _prvBTStartAdv,
    .pxStopAdv                         = _prvBTStopAdv,
    .pxReadRemoteRssi                  = _prvBTReadRemoteRssi,
    .pxScanFilterParamSetup            = _prvBTScanFilterParamSetup,
    .pxScanFilterAddRemove             = _prvBTScanFilterAddRemove,
    .pxScanFilterEnable                = _prvBTScanFilterEnable,
    .pxGetDeviceType                   = _prvBTGetDeviceType,
    .pxSetAdvData                      = _prvBTSetAdvData,
    .pxSetAdvRawData                   = _prvBTSetAdvRawData,
    .pxConnParameterUpdateRequest      = _prvBTConnParameterUpdateRequest,
    .pxSetScanParameters               = _prvBTSetScanParameters,
    .pxMultiAdvEnable                  = _prvBTMultiAdvEnable,
    .pxMultiAdvUpdate                  = _prvBTMultiAdvUpdate,
    .pxMultiAdvSetInstData             = _prvBTMultiAdvSetInstData,
    .pxMultiAdvDisable                 = _prvBTMultiAdvDisable,
    .pxBatchscanCfgStorage             = _prvBTBatchscanCfgStorage,
    .pxBatchscanEndBatchScan           = _prvBTBatchscanEndBatchScan,
    .pxBatchscanDisBatchScan           = _prvBTBatchscanDisBatchScan,
    .pxBatchscanReadReports            = _prvBTBatchscanReadReports,
    .pxSetPreferredPhy                 = _prvBTSetPreferredPhy,
    .pxReadPhy                         = _prvBTReadPhy,
    .ppvGetGattClientInterface         = _prvBTGetGattClientInterface,
    .ppvGetGattServerInterface         = _prvBTGetGattServerInterface,
};

/*-----------------------------------------------------------*/

BTBleAdapterCallbacks_t _xBTBleAdapterCallbacks;

BTUuid_t xAppUuid =
{
    .ucType  = eBTuuidType16,
    .uu.uu16 = ( uint16_t ) 0x00
};

/*-----------------------------------------------------------*/

/**
 * @brief      Advertising state change callback from BLE-Stack
 *             Function is of type pfnGapCB_t
 *
 * @warning    This function executes in the BLE-Stack context
 *             Which may include ISR!
 *
 * @param[in]  event  Relevant Advertising event from GapAdv_Events
 * @param      pBuf   Buffer of data associated with event
 * @param[in]  arg    App argument passed in at set creation time
 */
static void _prvAdvCallback( uint32_t event, void *pBuf, uintptr_t arg )
{
    /* Send notice to application task */
    if( xAdvStateQueue != NULL)
    {
        xQueueSendFromISR( xAdvStateQueue, ( void * ) &event, NULL );

        if( pBuf != NULL)
        {
            ICall_free(pBuf);
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief      Function to fill advertisement and scan response buffers
 *             with AD structures based on user parameters
 *
 * @param      pucBufferToFill     Pointer to the advertising or scan response
 *                                 buffer to populate
 * @param      pxParams            Advertising parameters
 * @param[in]  usManufacturerLen   Length of manufacterer data in bytes
 * @param      pcManufacturerData  Pointer to manufacturer data, will be copied
 *                                 into the buffer being filled
 * @param[in]  usServiceDataLen    Length of service data field
 * @param      pcServiceData       Pointer to service data, will be copied
 *                                 into the buffer being filled
 * @param      pxServiceUuid       Array of service UUIDs
 * @param[in]  xNbServices         Number of services in the service array
 *
 * @return     The bt status.
 */
static BTStatus_t _prvBTFillAdvBuffer( uint8_t *pucBufferToFill,
                                       BTGattAdvertismentParams_t * pxParams,
                                       uint16_t usManufacturerLen,
                                       char * pcManufacturerData,
                                       uint16_t usServiceDataLen,
                                       char * pcServiceData,
                                       BTUuid_t * pxServiceUuid,
                                       size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    bStatus_t  xStackStatus = SUCCESS;
    uint16_t  usBufferIdx = 0;

    if( pxParams == NULL || pucBufferToFill == NULL )
    {
        return ( eBTStatusParamInvalid );
    }

    /* AD types and links to their relevant bits of the specification can be
     * found here: https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile/
     *
     * The code below intends to pack an array based on the format described in the
     * BLUETOOTH CORE SPECIFICATION Version 5.2 | Vol 3, Part C, Section 11
     * Each AD type is defined within:
     * Core Specification Supplement document: CSS v9, Part A, Section 1
     */

    /* Always include AD flags in the adv data, always as the first item */
    if( !pxParams->bSetScanRsp )
    {
        pucBufferToFill[ usBufferIdx++ ] = BLE_HAL_MANAGER_ADV_FLAGS_LEN + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_FLAGS;

        /* CC13x2/26x2 device does not support Bluetooth Classic
         * Don't increment buffer index here because we need append the
         * discovery flag to the same field
         */
        pucBufferToFill[ usBufferIdx ] = GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED;

        /* If the advertising has a timeout, its limited discoverable, else
         * general discoverable
         */
        if( pxParams->ucTimeout != 0 )
        {
            pucBufferToFill[ usBufferIdx++ ] |= GAP_ADTYPE_FLAGS_LIMITED;
        }
        else
        {
            pucBufferToFill[ usBufferIdx++ ] |= GAP_ADTYPE_FLAGS_GENERAL;
        }
    }

    /* Pack TX power into the buffer */
    if( pxParams->bIncludeTxPower )
    {
        pucBufferToFill[ usBufferIdx++ ] = sizeof( int8_t ) + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_POWER_LEVEL;
        pucBufferToFill[ usBufferIdx++ ] = ( int8_t ) pxParams->ucTxPower;
    }

    /* Fill in device name */
    if( pxParams->ucName.ucShortNameLen > 0 )
    {
        uint8_t ucDeviceName[ GAP_DEVICE_NAME_LEN ];
        uint16_t  usNameLen = 0;
        xStackStatus = GGS_GetParameter( GGS_DEVICE_NAME_ATT,
                                         &ucDeviceName );
        xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );

        if( xStatus == eBTStatusSuccess)
        {
            usNameLen = strlen ( ( const char * ) ucDeviceName );

            if( pxParams->ucName.xType == BTGattAdvNameShort )
            {
                usNameLen = IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE;
                pucBufferToFill[ usBufferIdx++ ] = usNameLen + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
                pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_LOCAL_NAME_SHORT;
            }
            else if( pxParams->ucName.xType == BTGattAdvNameComplete )
            {
                pucBufferToFill[ usBufferIdx++ ] = usNameLen + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
                pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
            }

            /* Copy name into buffer */
            memcpy( &pucBufferToFill[ usBufferIdx ],  ucDeviceName, usNameLen );
            usBufferIdx += usNameLen;

        }
        else
        {
            return (eBTStatusFail);
        }
    }

    if( pxParams->ulAppearance > 0 )
    {
        uint16_t usAppearance = pxParams->ulAppearance & 0x0000FFFF;
        pucBufferToFill[ usBufferIdx++ ] = BLE_HAL_MANAGER_ADV_APPEARANCE_LEN + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_APPEARANCE;
        pucBufferToFill[ usBufferIdx++ ] = LO_UINT16( usAppearance );
        pucBufferToFill[ usBufferIdx++ ] = HI_UINT16( usAppearance );
    }

    /* Pack peripheral preferred connection parameters into the buffer */
    if( ( pxParams->ulMinInterval > 0 ) && ( pxParams->ulMaxInterval > 0 ) )
    {
        uint16_t usMinCi = pxParams->ulMinInterval & 0x0000FFFF;
        uint16_t usMaxCi = pxParams->ulMaxInterval & 0x0000FFFF;
        pucBufferToFill[ usBufferIdx++ ] = BLE_HAL_MANAGER_ADV_PERI_PREFERRED_CONN_PARAMS_LEN + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE;
        pucBufferToFill[ usBufferIdx++ ] = LO_UINT16( usMinCi );
        pucBufferToFill[ usBufferIdx++ ] = HI_UINT16( usMinCi );
        pucBufferToFill[ usBufferIdx++ ] = LO_UINT16( usMaxCi );
        pucBufferToFill[ usBufferIdx++ ] = HI_UINT16( usMaxCi );
    }

    /* Pack manufacturer data into the buffer */
    if( ( pcManufacturerData != NULL ) &&
        ( usManufacturerLen >= BLE_HAL_MANAGER_ADV_COMPANY_ID_SIZE ) )
    {
        pucBufferToFill[ usBufferIdx++ ] = usManufacturerLen + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_MANUFACTURER_SPECIFIC;
        memcpy( &pucBufferToFill[ usBufferIdx],
                pcManufacturerData,
                usManufacturerLen );
        usBufferIdx += usManufacturerLen;
    }

    /* Pack service data into the buffer */
    if( (pcServiceData != NULL ) && ( usServiceDataLen > 0 ))
    {
        pucBufferToFill[ usBufferIdx++ ] = usServiceDataLen + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
        pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_SERVICE_DATA;
        memcpy( &pucBufferToFill[ usBufferIdx ],
                pcServiceData,
                usServiceDataLen );
        usBufferIdx += usServiceDataLen;
    }

    /* Iterate over all advertised services and pack them in the buffer */
    for( uint16_t idx = 0; idx < xNbServices; idx++ )
    {
        if( pxServiceUuid[  idx ] .ucType == eBTuuidType16 )
        {
            pucBufferToFill[ usBufferIdx++ ] = ATT_BT_UUID_SIZE + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
            pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_16BIT_MORE;
            memcpy( &pucBufferToFill[ usBufferIdx ],
                    &pxServiceUuid[ idx ].uu.uu16,
                    ATT_BT_UUID_SIZE );
            usBufferIdx += ATT_BT_UUID_SIZE;
        }
        else if ( pxServiceUuid[  idx ] .ucType == eBTuuidType128 )
        {
            pucBufferToFill[ usBufferIdx++ ] = ATT_UUID_SIZE + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
            pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_128BIT_MORE;
            memcpy( &pucBufferToFill[ usBufferIdx],
                    &pxServiceUuid[ idx ].uu.uu128,
                    ATT_UUID_SIZE );
            usBufferIdx += ATT_UUID_SIZE;
        }
        else if ( pxServiceUuid[  idx ] .ucType == eBTuuidType32 )
        {
            pucBufferToFill[ usBufferIdx++ ] = ATT_32_BIT_UUID_SIZE + BLE_HAL_MANAGER_ADV_OPCODE_SIZE;
            pucBufferToFill[ usBufferIdx++ ] = GAP_ADTYPE_32BIT_MORE;
            memcpy( &pucBufferToFill[ usBufferIdx ],
                    &pxServiceUuid[ idx ].uu.uu32,
                    ATT_32_BIT_UUID_SIZE );
            usBufferIdx += ATT_32_BIT_UUID_SIZE;
        }
    }

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBleAdapterInit( const BTBleAdapterCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxCallbacks != NULL )
    {
        /* Copy local instance of callbacks */
        _xBTBleAdapterCallbacks = *pxCallbacks;

        /* Create a queue for receiving advertising events from stack to app */
        xAdvStateQueue = xQueueCreate( BLE_HAL_MANAGER_ADV_MAX_QUEUE_DEPTH,
                                       sizeof( int32_t ) );

        if( xAdvStateQueue == NULL )
        {
            xStatus = eBTStatusNoMem;
        }
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTRegisterBleApp( BTUuid_t * pxAppUuid )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    xAppUuid.ucType = pxAppUuid->ucType;
    memcpy( &xAppUuid.uu.uu128, &( pxAppUuid->uu.uu128 ), bt128BIT_UUID_LEN );

    if( _xBTBleAdapterCallbacks.pxRegisterBleAdapterCb )
    {
        _xBTBleAdapterCallbacks.pxRegisterBleAdapterCb( eBTStatusSuccess, 0, pxAppUuid );
    }
    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTUnregisterBleApp( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTGetBleAdapterProperty( BTBlePropertyType_t xType )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetBleAdapterProperty( const BTBleProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTGetallBleRemoteDeviceProperties( BTBdaddr_t * pxRremoteAddr )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTGetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                    BTBleProperty_t xType )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetBleRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                                    const BTBleProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTScan( bool bStart )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTConnect( uint8_t ucAdapterIf,
                                 const BTBdaddr_t * pxBdAddr,
                                 bool bIsDirect,
                                 BTTransport_t ulTransport )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTDisconnect( uint8_t ucAdapterIf,
                                    const BTBdaddr_t * pxBdAddr,
                                    uint16_t usConnId )
{
    BTStatus_t xStatus = eBTStatusUnsupported;
    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTStartAdv( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    bStatus_t  xStackStatus = SUCCESS;
    uint32_t   ulAdvEvent = 0;
    GapAdv_enableOptions_t xAdvEnableOpts = GAP_ADV_ENABLE_OPTIONS_USE_MAX;

    /* Load adv data if it is setup properly */
    if( pucAdvData != NULL && usAdvDataLen > 0 )
    {
        /* Load the Advertising buffer if one exists */
        xStackStatus = GapAdv_loadByHandle( ucAdvHandle,
                                            GAP_ADV_DATA_TYPE_ADV,
                                            usAdvDataLen,
                                            pucAdvData );
        xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );
        if(xStatus != eBTStatusSuccess)
        {
            return ( xStatus );
        }
    }
    else
    {
        return ( eBTStatusParamInvalid );
    }

    /* Load scan response data if it is setup properly */
    if( pucScanRspData != NULL && usScanRspDataLen > 0 )
    {
        /* Load the Scan response data buffer if one exists */
        xStackStatus = GapAdv_loadByHandle( ucAdvHandle,
                                            GAP_ADV_DATA_TYPE_SCAN_RSP,
                                            usScanRspDataLen,
                                            pucScanRspData );

        xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );
        if(xStatus != eBTStatusSuccess)
        {
            return ( xStatus );
        }
    }
    else
    {
        return ( eBTStatusParamInvalid );
    }

    /* Set event mask for advertising set */
    xStackStatus = GapAdv_setEventMask( ucAdvHandle,
                                        GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                        GAP_ADV_EVT_MASK_END_AFTER_DISABLE  |
                                        GAP_ADV_EVT_MASK_SET_TERMINATED );

    xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );
    if(xStatus != eBTStatusSuccess)
    {
        return ( xStatus );
    }

    if( usAdvTimeout != 0)
    {
        /* If the user has specified a timeout, then we will only
         * advertise for a limited time
         */
        xAdvEnableOpts = GAP_ADV_ENABLE_OPTIONS_USE_DURATION;
    }

    /* Enable advertising */
    xStackStatus = GapAdv_enable( ucAdvHandle,
                                  xAdvEnableOpts,
                                  usAdvTimeout );
    xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );

    if( xStatus == eBTStatusSuccess )
    {
        /* Wait for advertising disable event */
        if( xQueueReceive( xAdvStateQueue, &ulAdvEvent, portMAX_DELAY ) == pdPASS )
        {
            if(ulAdvEvent != GAP_EVT_ADV_START_AFTER_ENABLE)
            {
                return ( eBTStatusNotReady );
            }
            else
            {
                if( _xBTBleAdapterCallbacks.pxAdvStatusCb )
                {
                    _xBTBleAdapterCallbacks.pxAdvStatusCb( eBTStatusSuccess, 0, true );
                }
            }
        }
        else
        {
            return ( eBTStatusFail );
        }
    }

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTStopAdv( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    bStatus_t xStackStatus = SUCCESS;
    uint32_t   ulAdvEvent = 0;

    xStackStatus = GapAdv_disable( ucAdvHandle );

    if( xStackStatus == bleAlreadyInRequestedMode)
    {
        xStatus = eBTStatusSuccess;
        if( _xBTBleAdapterCallbacks.pxAdvStatusCb )
        {
            _xBTBleAdapterCallbacks.pxAdvStatusCb( eBTStatusSuccess, 0, false );
        }

        return ( xStatus );
    }
    else
    {
        xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );
    }

    /* Wait for advertising disable event */
    if( xQueueReceive( xAdvStateQueue, &ulAdvEvent, portMAX_DELAY ) == pdPASS )
    {
        if( ulAdvEvent != GAP_EVT_ADV_END_AFTER_DISABLE )
        {
            return ( eBTStatusNotReady );
        }
        else
        {
            if( _xBTBleAdapterCallbacks.pxAdvStatusCb )
            {
                _xBTBleAdapterCallbacks.pxAdvStatusCb( eBTStatusSuccess, 0, false );
            }
        }
    }
    else
    {
        return ( eBTStatusFail );
    }

    /* Reset advertising timeout */
    usAdvTimeout = 0;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTReadRemoteRssi( uint8_t ucAdapterIf,
                                        const BTBdaddr_t * pxBdAddr )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTScanFilterParamSetup( BTGattFiltParamSetup_t xFiltParam )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTScanFilterAddRemove( uint8_t ucAdapterIf,
                                             uint32_t ulAction,
                                             uint32_t ulFiltType,
                                             uint32_t ulFiltIndex,
                                             uint32_t ulCompanyId,
                                             uint32_t ulCompanyIdMask,
                                             const BTUuid_t * pxUuid,
                                             const BTUuid_t * pxUuidMask,
                                             const BTBdaddr_t * pxBdAddr,
                                             char cAddrType,
                                             size_t xDataLen,
                                             char * pcData,
                                             size_t xMaskLen,
                                             char * pcMask )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTScanFilterEnable( uint8_t ucAdapterIf,
                                          bool bEnable )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

BTTransport_t _prvBTGetDeviceType( const BTBdaddr_t * pxBdAddr )
{
    return ( BTTransportLe );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetAdvData( uint8_t ucAdapterIf,
                                    BTGattAdvertismentParams_t * pxParams,
                                    uint16_t usManufacturerLen,
                                    char * pcManufacturerData,
                                    uint16_t usServiceDataLen,
                                    char * pcServiceData,
                                    BTUuid_t * pxServiceUuid,
                                    size_t xNbServices )
{
    /* This is a pointer to the pointer that holds the adv or scan data */
    uint8_t **pucBufferToLoad = NULL;
    BTStatus_t xStatus = eBTStatusSuccess;
    uint16_t   usBufferLen = 0;
    uint32_t ulAdvEvent = 0;
    uint16_t usAdvEventProps = GAP_ADV_PROP_CONNECTABLE |                      \
                               GAP_ADV_PROP_SCANNABLE |                        \
                               GAP_ADV_PROP_LEGACY;
    int8_t ucTxPower = GAP_ADV_TX_POWER_NO_PREFERENCE;
    uint32_t ulPrimIntMin = 160;
    uint32_t ulPrimIntMax = 160;
    bool bLocalGapInitStatus = false;

    /* Check for valid inputs */
    if( pxParams == NULL )
    {
        return ( eBTStatusParamInvalid );
    }

    /* Read shared variable in critical section, there is very little chance
     * of collision here as GAP_DEVICE_INIT would have come much earlier, but
     * for safety's sake, protect the read.
     *
     * This is done to protect against this API being called before the device
     * is initailized and the advertising set can be created
     */
    vPortEnterCritical();
    bLocalGapInitStatus = bGapInitialized;
    vPortExitCritical();

    if ( !bLocalGapInitStatus )
    {
        return ( eBTStatusNotReady );
    }

    /* First stop advertising if its enabled and pend on disable event  */
    if( SUCCESS == GapAdv_disable( ucAdvHandle ) )
    {
        /* Wait for advertising disable event */
        if( xQueueReceive( xAdvStateQueue, &ulAdvEvent, portMAX_DELAY ) == pdPASS )
        {
            if( ulAdvEvent != GAP_EVT_ADV_END_AFTER_DISABLE )
            {
                return ( eBTStatusNotReady );
            }
        }
    }

    /* Add up all the length of the requested set information */
    if( pxParams->ucName.ucShortNameLen > 0 )
    {
        usBufferLen += pxParams->ucName.ucShortNameLen + BLE_HAL_MANAGER_ADV_HEADER_LEN;
    }

    if( pxParams->ulAppearance > 0 )
    {
        /* Per V5.2, Vol 3, Part C, Section 12.2 of the spec, appearance is 2 octets */
        usBufferLen += BLE_HAL_MANAGER_ADV_APPEARANCE_LEN + BLE_HAL_MANAGER_ADV_HEADER_LEN;
    }

    if( ( pxParams->ulMinInterval > 0 ) && ( pxParams->ulMaxInterval > 0 ) )
    {
        usBufferLen += BLE_HAL_MANAGER_ADV_PERI_PREFERRED_CONN_PARAMS_LEN + BLE_HAL_MANAGER_ADV_HEADER_LEN;
    }

    if( usManufacturerLen > 0 )
    {
        usBufferLen += usManufacturerLen + BLE_HAL_MANAGER_ADV_HEADER_LEN;
    }

    if( usServiceDataLen > 0 )
    {
        usBufferLen += ATT_BT_UUID_SIZE + usServiceDataLen + BLE_HAL_MANAGER_ADV_HEADER_LEN;
    }

    if( pxParams->bIncludeTxPower )
    {
        usBufferLen += BLE_HAL_MANAGER_ADV_HEADER_LEN + sizeof(pxParams->ucTxPower);
    }

    /* Copy the advertising timeout parameter into a local variable. It will
     * be used later when advertising is enabled
     */
    if( pxParams->usTimeout != 0 )
    {
        usAdvTimeout = pxParams->usTimeout;
    }

    /* Get length of all solicited services */
    for( uint16_t idx = 0; idx < xNbServices; idx++ )
    {
        usBufferLen += BLE_HAL_MANAGER_ADV_HEADER_LEN;
        if( pxServiceUuid[ idx ].ucType == eBTuuidType16 )
        {
            usBufferLen += ATT_BT_UUID_SIZE;
        }
        else if ( pxServiceUuid[ idx ].ucType == eBTuuidType128 )
        {
            usBufferLen += ATT_UUID_SIZE;
        }
        else if ( pxServiceUuid[ idx ].ucType == eBTuuidType32 )
        {
            usBufferLen += ATT_32_BIT_UUID_SIZE;
        }
    }

    if( usBufferLen > LL_MAX_ADV_DATA_LEN )
    {
        /* Remove legacy flag if the length of requested data is too large */
        usAdvEventProps &= ~( GAP_ADV_PROP_LEGACY );
    }

    /* Clear the connectable flag, it is set by default */
    if( pxParams->usAdvertisingEventProperties == BTAdvNonconnInd )
    {
        usAdvEventProps &= ~( GAP_ADV_PROP_CONNECTABLE );
    }

    /* If we're to use directed adv, set the flag */
    if( pxParams->usAdvertisingEventProperties == BTAdvDirectInd )
    {
        usAdvEventProps |= GAP_ADV_PROP_DIRECTED;
    }

    /* Set the tx power of this advertising set based on input parameter */
    if( pxParams->ucTxPower )
    {
        ucTxPower = ( int8_t ) pxParams->ucTxPower;
        GapAdv_setParam( ucAdvHandle,
                         GAP_ADV_PARAM_TX_POWER,
                         &ucTxPower );
    }

    if (pxParams->ulMinInterval != 0)
    {
        /* Set advertising interval params based on user input */
        ulPrimIntMin = ( uint32_t ) pxParams->usMinAdvInterval;
        GapAdv_setParam( ucAdvHandle,
                         GAP_ADV_PARAM_PRIMARY_INTERVAL_MIN,
                         &ulPrimIntMin );
    }

    if( pxParams->usMaxAdvInterval != 0 )
    {
        ulPrimIntMax = ( uint32_t ) pxParams->usMaxAdvInterval;
        GapAdv_setParam( ucAdvHandle,
                         GAP_ADV_PARAM_PRIMARY_INTERVAL_MAX,
                         &ulPrimIntMax );
    }

    /* The following parameters are ignored because they are reserved for a 5.0
     * interface
     *
     * primChanMap = pxParams->ucChannelMap;
     * primPhy = pxParams->ucPrimaryAdvertisingPhy;
     * secPhy = pxParams->ucSecondaryAdvertisingPhy;
     */

    /* Read which buffer is advertisement vs. scan response  */
    if( pxParams->bSetScanRsp )
    {
        pucBufferToLoad = &pucScanRspData;
        usScanRspDataLen = usBufferLen;
    }
    else
    {
        /* We always include flags in adv data, so include them in the length */
        usBufferLen += BLE_HAL_MANAGER_ADV_HEADER_LEN + BLE_HAL_MANAGER_ADV_FLAGS_LEN;
        pucBufferToLoad = &pucAdvData;
        usAdvDataLen = usBufferLen;
    }

    /* Always Set advertising event properties, must do this down here because
     * the scannable flag was just set above
     */
    GapAdv_setParam( ucAdvHandle, GAP_ADV_PARAM_PROPS, &usAdvEventProps );

    /* Allocate the buffer, free if existing */
    if( *pucBufferToLoad != NULL )
    {
        IotBle_Free( *pucBufferToLoad );
    }
    *pucBufferToLoad = IotBle_Malloc( usBufferLen );
    if( *pucBufferToLoad == NULL )
    {
        return ( eBTStatusNoMem );
    }

    /* Now, populate the buffer with data */
    xStatus = _prvBTFillAdvBuffer( *pucBufferToLoad,
                                    pxParams,
                                    usManufacturerLen,
                                    pcManufacturerData,
                                    usServiceDataLen,
                                    pcServiceData,
                                    pxServiceUuid,
                                    xNbServices );

    /* We have set the parameters and filled the buffers, issue callback */
    if( _xBTBleAdapterCallbacks.pxSetAdvDataCb != NULL )
    {
        _xBTBleAdapterCallbacks.pxSetAdvDataCb( xStatus );
    }

    return ( xStatus );
}


/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetAdvRawData( uint8_t ucAdapterIf,
                                       uint8_t * pucData,
                                       uint8_t ucLen )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetScanParameters( uint8_t ucAdapterIf,
                                           uint32_t ulScanInterval,
                                           uint32_t ulScanWindow )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTMultiAdvEnable( uint8_t ucAdapterIf,
                                        BTGattAdvertismentParams_t * xAdvParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTMultiAdvUpdate( uint8_t ucAdapterIf,
                                        BTGattAdvertismentParams_t * advParams )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTMultiAdvSetInstData( uint8_t ucAdapterIf,
                                             bool bSetScanRsp,
                                             bool bIncludeName,
                                             bool bInclTxpower,
                                             uint32_t ulAppearance,
                                             size_t xManufacturerLen,
                                             char * pcManufacturerData,
                                             size_t xServiceDataLen,
                                             char * pcServiceData,
                                             BTUuid_t * pxServiceUuid,
                                             size_t xNbServices )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTMultiAdvDisable( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBatchscanCfgStorage( uint8_t ucAdapterIf,
                                             uint32_t ulBatchScanFullMax,
                                             uint32_t ulBatchScanTruncMax,
                                             uint32_t ulBatchScanNotifyThreshold )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBatchscanEndBatchScan( uint8_t ucAdapterIf,
                                               uint32_t ulScanMode,
                                               uint32_t ulScanInterval,
                                               uint32_t ulScanWindow,
                                               uint32_t ulAddrType,
                                               uint32_t ulDiscardRule )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBatchscanDisBatchScan( uint8_t ucAdapterIf )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTBatchscanReadReports( uint8_t ucAdapterIf,
                                              uint32_t ulScanMode )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTSetPreferredPhy( uint16_t usConnId,
                                         uint8_t ucTxPhy,
                                         uint8_t ucRxPhy,
                                         uint16_t usPhyOptions )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTReadPhy( uint16_t usConnId,
                                 BTReadClientPhyCallback_t xCb )
{
    BTStatus_t xStatus = eBTStatusUnsupported;

    return ( xStatus );
}

/*-----------------------------------------------------------*/

const void * _prvBTGetGattClientInterface( void )
{
    return ( NULL );
}

/*-----------------------------------------------------------*/

const void * _prvGetLeAdapter( void )
{
    return ( &xBTLeAdapter );
}

/*-----------------------------------------------------------*/

static BTStatus_t _prvBTConnParameterUpdateRequest( const BTBdaddr_t * pxBdAddr,
                                                    uint32_t ulMinInterval,
                                                    uint32_t ulMaxInterval,
                                                    uint32_t ulLatency,
                                                    uint32_t ulTimeout )
{
    return eBTStatusFail;
}

/*-----------------------------------------------------------*/

BTStatus_t xBTCleanUp( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    bStatus_t xStackStatus = SUCCESS;

    if(xAdvStateQueue != NULL )
    {
        vQueueDelete( xAdvStateQueue );
    }

    xAdvStateQueue = NULL;

    /* Destroy advertising set, don't free adv data as this is managed by
     * the HAL layer and is freed below
     */
    xStackStatus = GapAdv_destroy( ucAdvHandle, GAP_ADV_FREE_OPTION_DONT_FREE );

    xStatus = _IotBleHalTypes_ConvertStatus( xStackStatus );

    /* Free advertising and scan response data if it exists */
    if( pucAdvData != NULL )
    {
        IotBle_Free( pucAdvData );
        pucAdvData = NULL;
    }

    if( pucScanRspData != NULL )
    {
        IotBle_Free( pucScanRspData );
        pucScanRspData = NULL;
    }

    return ( xStatus );
}

/*-----------------------------------------------------------*/

BTStatus_t xBTStop( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    /* Terminate any connections */
    GAP_TerminateLinkReq( LINKDB_CONNHANDLE_ALL,
                          LL_PEER_REQUESTED_POWER_OFF_TERM );

     /* Stop advertising, if advertising */
    GapAdv_disable( ucAdvHandle );

    return ( xStatus );
}

/*-----------------------------------------------------------*/

BTStatus_t bleStackInit( void )
{
  return ( eBTStatusSuccess );
}

/*-----------------------------------------------------------*/

void vProcessGapDeviceInit( gapDeviceInitDoneEvent_t *pxPkt )
{
    if( pxPkt->hdr.status == SUCCESS )
    {
        bStatus_t xStackStatus = SUCCESS;
        /* Create advertising set with default params */
        xStackStatus = GapAdv_create( &_prvAdvCallback,
                                      &xAdvParams,
                                      &ucAdvHandle );

        if( xStackStatus == SUCCESS )
        {
            bGapInitialized = true;
        }
        else
        {
            bGapInitialized = false;
            IotLogError( "Creation of advertising set failed" );
        }
    }
    else
    {
        IotLogError( "GapDevice initialization failed" );
        bGapInitialized = false;
    }

}

/*-----------------------------------------------------------*/
