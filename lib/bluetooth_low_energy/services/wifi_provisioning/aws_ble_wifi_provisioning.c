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
 * @file aws_ble_wifi_provisioning.c
 * @brief BLE Gatt service for WiFi provisioning
 */

#include <aws_ble.h>
#include <string.h>

#include "aws_ble_config.h"
#include "aws_ble_wifi_provisioning.h"
#include "semphr.h"
#include "aws_json_utils.h"


#define ATTR_DATA( svc, attrType )      ( ( svc )->pxGattService->pxCharacteristics[ attrType ].xAttributeData )
#define ATTR_HANDLE( svc, attrType )    ( ( ATTR_DATA( svc, attrType ) ).xHandle )
#define ATTR_UUID( svc, attrType )      ( ( ATTR_DATA( svc, attrType ) ).xUuid )

/*---------------------------------------------------------------------------------------------------------*/

static WifiProvService_t xWifiProvService = { 0 };

#define STORAGE_INDEX( priority )    ( xWifiProvService.usNumNetworks - priority - 1 )
#define INVALID_INDEX    ( -1 )
/*---------------------------------------------------------------------------------------------------------*/

/*
 * @brief Callback registered for BLE write and read events received for each characteristic.
 */
static void vCharacteristicCallback( BLEAttribute_t * pxAttribute,
                                     BLEAttributeEvent_t * pxEventParam );

/*
 * @brief Callback registered for client characteristic configuration descriptors.
 */
static void vClientCharCfgDescrCallback( BLEAttribute_t * pxAttribute,
                                         BLEAttributeEvent_t * pxEventParam );

/*
 * @brief Callback registered for receiving notification when the BLE service is started.
 */
static void vServiceStartedCb( BTStatus_t xStatus,
                               BLEService_t * pxService );

/*
 * @brief Callback registered for receiving notification when the GATT service is stopped.
 */
static void vServiceStoppedCb( BTStatus_t xStatus,
                               BLEService_t * pxService );

/*
 * @brief Callback registered for receiving notification when the GATT service is deleted.
 */
static void vServiceDeletedCb( BTStatus_t xStatus,
                               uint16_t usAttributeDataHandle );

/*
 * @brief Function used to create and initialize BLE service.
 */
static BaseType_t prxInitGATTService( void );

/*
 * @brief Helper function to set an WiFi provisioning event.
 */
static void prvSetEvent( WifiProvEvent_t xEvent );

/*
 * @brief Helper function to wait for a WiFi provisioning event.
 */
static BaseType_t prvWaitForEvent( WifiProvEvent_t xEvent,
                                          TickType_t xTimeout );

/*
 * @brief Parses List Network request params and creates task to list networks.
 */
static BaseType_t prxHandleListNetworkRequest( uint8_t * pucData,
                                               size_t xLength );

/*
 * @brief Parses Save Network request params and creates task to save the new network.
 */
static BaseType_t prxHandleSaveNetworkRequest( uint8_t * pucData,
                                               size_t xLength );

/*
 * @brief Parses Edit Network request params and creates task to edit network priority.
 */
static BaseType_t prxHandleEditNetworkRequest( uint8_t * pucData,
                                               size_t xLength );

/*
 * @brief Parses Delete Network request params and creates task to delete a WiFi networ.
 */
static BaseType_t prxHandleDeleteNetworkRequest( uint8_t * pucData,
                                                 size_t xLength );

/*
 * @brief Gets the GATT characteristic for a given attribute handle.
 */
static WifiProvCharacteristic_t prxGetCharFromHandle( uint16_t xHandle );


WIFIReturnCode_t prvAppendNetwork( WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t prvInsertNetwork( uint16_t usIndex,
                                   WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t prvPopNetwork( uint16_t usIndex,
                                WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t prvMoveNetwork( uint16_t usCurrentIndex,
                                 uint16_t usNewIndex );

WIFIReturnCode_t prvGetSavedNetwork( uint16_t usIndex,
                                     WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t prvConnectNetwork( WIFINetworkProfile_t * pxProfile );

WIFIReturnCode_t prvConnectSavedNetwork( uint16_t usIndex );

WIFIReturnCode_t prvAddNewNetwork( WIFINetworkProfile_t * pxProfile );

void prvSerializeNetwork( WifiNetworkInfo_t *xProfile, uint8_t *pucBuffer, uint32_t ulLen, uint32_t *pulLen );
/*
 * @brief  The task lists the saved network configurations in flash and also scans nearby networks.
 * It sends the profile information for each saved and scanned networks one at a time to the GATT client.
 * Maximum number of networks to scan is set in the List network request.
 */
static void prvListNetworkTask( void * pvParams );

/*
 * @brief  The task is used to save a new WiFi configuration.
 * It first connects to the network and if successful,saves the network onto flash with the highest priority.
 */
static void prvAddNetworkTask( void * pvParams );

/*
 * @brief  The task is used to reorder priorities of network profiles stored in flash.
 *  If the priority of existing connected network changes then it initiates a reconnection.
 */
void prvEditNetworkTask( void * pvParams );

/*
 * @brief  The task is used to delete a network configuration from the flash.
 * If the network is connected, it disconnects from the network and initiates a reconnection.
 */
void prvDeleteNetworkTask( void * pvParams );

/*
 * @brief Gets the number of saved networks from flash.
 */
static uint16_t prvGetNumSavedNetworks( void );

/*
 * @brief Sends a status response for the request.
 */
void prvSendStatusResponse( WifiProvCharacteristic_t xChar, WIFIReturnCode_t xStatus );


void prvSendResponse( WifiProvCharacteristic_t xCharacteristic, uint8_t* pucData, size_t xLen );
/* -------------------------------------------------------*/

static void vServiceStartedCb( BTStatus_t xStatus,
                               BLEService_t * pxService )
{
    if( xStatus == eBTStatusSuccess )
    {
        prvSetEvent( eWIFIPROVStarted );
    }
    else
    {
        prvSetEvent( eWIFIPROVFailed );
    }
}

/*-----------------------------------------------------------*/

static void vServiceStoppedCb( BTStatus_t xStatus,
                               BLEService_t * pxService )
{
    if( xStatus == eBTStatusSuccess )
    {
        prvSetEvent( eWIFIPROVStopped );
    }
    else
    {
        prvSetEvent( eWIFIPROVFailed );
    }
}

/*-----------------------------------------------------------*/

static void vServiceDeletedCb( BTStatus_t xStatus,
                               uint16_t usAttributeDataHandle )
{
    if( xStatus == eBTStatusSuccess )
    {
        prvSetEvent( eWIFIPROVDeleted );
    }
    else
    {
        prvSetEvent( eWIFIPROVFailed );
    }
}

/*-----------------------------------------------------------*/

BaseType_t prxInitGATTService( void )
{
    BTStatus_t xStatus;
    BTUuid_t xUUID =
    {
        .uu.uu128 = wifiProvSVC_UUID_BASE,
        .ucType   = eBTuuidType128
    };

    BTUuid_t xClientCharCfgUUID =
    {
        .uu.uu16 = wifiProvCLIENT_CHAR_CFG_UUID,
        .ucType  = eBTuuidType16
    };

    struct BLEService * pxGattService = NULL;
    size_t xNumDescrsPerChar[ wifiProvNUM_CHARS ] = { 1, 1, 1, 1 };

    xStatus = BLE_CreateService( &xWifiProvService.pxGattService, wifiProvNUM_CHARS, wifiProvNUM_DESCRS, xNumDescrsPerChar, wifiProvNum_INCL_SERVICES );

    if( xStatus == eBTStatusSuccess )
    {
        pxGattService = xWifiProvService.pxGattService;
        configASSERT( pxGattService->xNbCharacteristics == wifiProvNUM_CHARS );
        configASSERT( pxGattService->xNbDescriptors == wifiProvNUM_DESCRS );
        configASSERT( pxGattService->xNbIncludedServices == wifiProvNum_INCL_SERVICES );

        xUUID.uu.uu16 = wifiProvSVC_UUID;
        pxGattService->xAttributeData.xUuid = xUUID;

        pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ].xAttributeData.xUuid = xClientCharCfgUUID;
        pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ].xAttributeData.pucData = NULL;
        pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ].xAttributeData.xSize = 0;
        pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ].pxAttributeEventCallback = vClientCharCfgDescrCallback;

        xUUID.uu.uu16 = wifiProvLIST_NETWORK_CHAR_UUID;
        pxGattService->pxCharacteristics[ eListNetworkChar ].xAttributeData.xUuid = xUUID;
        pxGattService->pxCharacteristics[ eListNetworkChar ].xAttributeData.pucData = NULL;
        pxGattService->pxCharacteristics[ eListNetworkChar ].xAttributeData.xSize = 0;
        pxGattService->pxCharacteristics[ eListNetworkChar ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxCharacteristics[ eListNetworkChar ].xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify );
        pxGattService->pxCharacteristics[ eListNetworkChar ].pxAttributeEventCallback = vCharacteristicCallback;
        pxGattService->pxCharacteristics[ eListNetworkChar ].xNbDescriptors = 1;
        pxGattService->pxCharacteristics[ eListNetworkChar ].pxDescriptors[ 0 ] = &pxGattService->pxDescriptors[ eListNetworkCharCCFGDescr ];

        pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ].xAttributeData.xUuid = xClientCharCfgUUID;
        pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ].xAttributeData.pucData = NULL;
        pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ].xAttributeData.xSize = 0;
        pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ].pxAttributeEventCallback = vClientCharCfgDescrCallback;

        xUUID.uu.uu16 = wifiProvSAVE_NETWORK_CHAR_UUID;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xAttributeData.xUuid = xUUID;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xAttributeData.pucData = NULL;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xAttributeData.xSize = 0;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify );
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].pxAttributeEventCallback = vCharacteristicCallback;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].xNbDescriptors = 1;
        pxGattService->pxCharacteristics[ eSaveNetworkChar ].pxDescriptors[ 0 ] = &pxGattService->pxDescriptors[ eSaveNetworkCharCCFGDescr ];

        pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ].xAttributeData.xUuid = xClientCharCfgUUID;
        pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ].xAttributeData.pucData = NULL;
        pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ].xAttributeData.xSize = 0;
        pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ].pxAttributeEventCallback = vClientCharCfgDescrCallback;

        xUUID.uu.uu16 = wifiProvEDIT_NETWORK_CHAR_UUID;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xAttributeData.xUuid = xUUID;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xAttributeData.pucData = NULL;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xAttributeData.xSize = 0;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify );
        pxGattService->pxCharacteristics[ eEditNetworkChar ].pxAttributeEventCallback = vCharacteristicCallback;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].xNbDescriptors = 1;
        pxGattService->pxCharacteristics[ eEditNetworkChar ].pxDescriptors[ 0 ] = &pxGattService->pxDescriptors[ eEditNetworkCharCCFGDescr ];

        pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ].xAttributeData.xUuid = xClientCharCfgUUID;
        pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ].xAttributeData.pucData = NULL;
        pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ].xAttributeData.xSize = 0;
        pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ].pxAttributeEventCallback = vClientCharCfgDescrCallback;

        xUUID.uu.uu16 = wifiProvDELETE_NETWORK_CHAR_UUID;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xAttributeData.xUuid = xUUID;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xAttributeData.pucData = NULL;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xAttributeData.xSize = 0;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xPermissions = ( bleconfigCHAR_READ_PERM | bleconfigCHAR_WRITE_PERM );
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify );
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].pxAttributeEventCallback = vCharacteristicCallback;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].xNbDescriptors = 1;
        pxGattService->pxCharacteristics[ eDeleteNetworkChar ].pxDescriptors[ 0 ] = &pxGattService->pxDescriptors[ eDeleteNetworkCharCCFGDescr ];

        pxGattService->xServiceType = eBTServiceTypePrimary;

        xStatus = BLE_AddService( pxGattService );
    }

    return wifiProvIS_SUCCESS( xStatus );
}

/*-----------------------------------------------------------*/

static WifiProvCharacteristic_t prxGetCharFromHandle( uint16_t usHandle )
{
    uint8_t ucCharId;
    BLEService_t * pxService = xWifiProvService.pxGattService;

    for( ucCharId = 0; ucCharId < eMaxChars; ucCharId++ )
    {
        if( pxService->pxCharacteristics[ ucCharId ].xAttributeData.xHandle == usHandle )
        {
            break;
        }
    }

    return ( WifiProvCharacteristic_t ) ucCharId;
}

/*-----------------------------------------------------------*/

void vCharacteristicCallback( BLEAttribute_t * pxAttribute,
                              BLEAttributeEvent_t * pxEventParam )
{
    BLEWriteEventParams_t * pxWriteParam;
    BLEReadEventParams_t * pxReadParam;
    BaseType_t xResult = pdFAIL;

    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp =
    {
        .xEventStatus    = eBTStatusFail,
        .xRspErrorStatus = eBTRspErrorNone
    };
    WifiProvCharacteristic_t xChar;

    xResp.pxAttrData = &xAttrData;

    if( pxEventParam->xEventType == eBLEWrite )
    {
        pxWriteParam = pxEventParam->pxParamWrite;
        xWifiProvService.usBLEConnId = pxWriteParam->usConnId;

        xChar = prxGetCharFromHandle( pxWriteParam->pxAttribute->pxCharacteristic->xAttributeData.xHandle );

        switch( xChar )
        {
            case eListNetworkChar:
                xResult = prxHandleListNetworkRequest( pxWriteParam->pucValue, pxWriteParam->xLength );
                break;

            case eSaveNetworkChar:
                xResult = prxHandleSaveNetworkRequest( pxWriteParam->pucValue, pxWriteParam->xLength );
                break;

            case eEditNetworkChar:
                xResult = prxHandleEditNetworkRequest( pxWriteParam->pucValue, pxWriteParam->xLength );
                break;

            case eDeleteNetworkChar:
                xResult = prxHandleDeleteNetworkRequest( pxWriteParam->pucValue, pxWriteParam->xLength );
                break;

            case eMaxChars:
            default:
                xResult = pdFAIL;
                break;
        }

        if( xResult == pdPASS )
        {
            xResp.xEventStatus = eBTStatusSuccess;
        }

        xResp.pxAttrData->xHandle = pxWriteParam->pxAttribute->pxCharacteristic->xAttributeData.xHandle;
        xResp.pxAttrData->pucData = pxWriteParam->pucValue;
        xResp.pxAttrData->xSize = pxWriteParam->xLength;
        xResp.xAttrDataOffset = pxWriteParam->usOffset;
        BLE_SendResponse( &xResp, pxWriteParam->usConnId, pxWriteParam->ulTransId );
    }
    else if( pxEventParam->xEventType == eBLERead )
    {
        pxReadParam = pxEventParam->pxParamRead;
        xResp.pxAttrData->xHandle = pxReadParam->pxAttribute->pxCharacteristic->xAttributeData.xHandle;
        xResp.pxAttrData->pucData = NULL;
        xResp.pxAttrData->xSize = 0;
        xResp.xAttrDataOffset = 0;
        xResp.xEventStatus = eBTStatusSuccess;
        BLE_SendResponse( &xResp, pxReadParam->usConnId, pxReadParam->ulTransId );
    }
}

/*-----------------------------------------------------------*/

static void vClientCharCfgDescrCallback( BLEAttribute_t * pxAttribute,
                                         BLEAttributeEvent_t * pxEventParam )
{
    BLEWriteEventParams_t * pxWriteParam;
    BLEReadEventParams_t * pxReadParam;

    BLEAttributeData_t xAttrData = { 0 };
    BLEEventResponse_t xResp =
    {
        .xEventStatus    = eBTStatusSuccess,
        .xRspErrorStatus = eBTRspErrorNone
    };

    xResp.pxAttrData = &xAttrData;

    if( pxEventParam->xEventType == eBLEWrite )
    {
        pxWriteParam = pxEventParam->pxParamWrite;

        if( pxWriteParam->xLength == 2 )
        {
            xWifiProvService.usNotifyClientEnabled = ( pxWriteParam->pucValue[ 1 ] << 8 ) | pxWriteParam->pucValue[ 0 ];
            xWifiProvService.usBLEConnId = pxWriteParam->usConnId;
            xResp.pxAttrData->xHandle = pxWriteParam->pxAttribute->pxCharacteristic->xAttributeData.xHandle;
            xResp.pxAttrData->pucData = pxWriteParam->pucValue;
            xResp.pxAttrData->xSize = pxWriteParam->xLength;
            xResp.xAttrDataOffset = pxWriteParam->usOffset;
        }

        BLE_SendResponse( &xResp, pxWriteParam->usConnId, pxWriteParam->ulTransId );
    }
    else if( pxEventParam->xEventType == eBLERead )
    {
        pxReadParam = pxEventParam->pxParamRead;
        xResp.pxAttrData->xHandle = pxReadParam->pxAttribute->pxCharacteristic->xAttributeData.xHandle;
        xResp.pxAttrData->pucData = ( uint8_t * ) &xWifiProvService.usNotifyClientEnabled;
        xResp.pxAttrData->xSize = 2;
        xResp.xAttrDataOffset = 0;
        xResp.xEventStatus = eBTStatusSuccess;
        BLE_SendResponse( &xResp, pxReadParam->usConnId, pxReadParam->ulTransId );
    }
}

/*-----------------------------------------------------------*/

static uint16_t prvGetNumSavedNetworks( void )
{
    uint16_t usIdx;
    WIFIReturnCode_t xWifiRet;
    WIFINetworkProfile_t xProfile;

    for( usIdx = 0; usIdx < wifiProvMAX_SAVED_NETWORKS; usIdx++ )
    {
        xWifiRet = WIFI_NetworkGet( &xProfile, usIdx );

        if( xWifiRet != eWiFiSuccess )
        {
            break;
        }
    }

    return usIdx;
}

/*-----------------------------------------------------------*/

static BaseType_t prxHandleListNetworkRequest( uint8_t * pucData,
                                               size_t xLength )
{
    BaseType_t xStatus = pdFALSE;
    jsmntok_t xTokens[ wifiProvMAX_TOKENS ];
    int16_t sNumTokens;
    ListNetworkRequest_t * pxParams;

    if( ( sNumTokens = JsonUtils_Parse( ( char * ) pucData, xLength, xTokens, wifiProvMAX_TOKENS ) ) > 0 )
    {
        pxParams = pvPortMalloc( sizeof( ListNetworkRequest_t ) );

        if( pxParams != NULL )
        {
            xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                               xTokens,
                                               sNumTokens,
                                               wifiProvMAX_NETWORKS_KEY,
                                               strlen( wifiProvMAX_NETWORKS_KEY ),
                                               ( int16_t * ) &pxParams->sMaxNetworks );

            if( xStatus == pdTRUE )
            {
                if(pxParams->sMaxNetworks > bleconfigMAX_NETWORK )
                {
                    configPRINTF(("Too many network request: %d, expected at most %d\n", pxParams->sMaxNetworks, bleconfigMAX_NETWORK));
                    pxParams->sMaxNetworks = bleconfigMAX_NETWORK;
                }
                else if ( pxParams->sMaxNetworks <= 0 )
                {
                    configPRINTF(( "Invalid message received, max scan networks (%d) is <= 0\n", pxParams->sMaxNetworks  ));
                    xStatus = pdFALSE;
                }
            }

            if( xStatus == pdTRUE )
            {
                xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                                   xTokens,
                                                   sNumTokens,
                                                   wifiProvSCAN_TIMEOUT_KEY,
                                                   strlen( wifiProvSCAN_TIMEOUT_KEY ),
                                                   &pxParams->sTimeoutMs );
            }

            if( xStatus == pdTRUE )
            {
                xStatus = xTaskCreate(
                    prvListNetworkTask,
                    "WifiProvListNetwork",
                    configMINIMAL_STACK_SIZE * 6,
                    pxParams,
					wifiProvLIST_NETWORK_TASK_PRIORITY,
                    NULL );
            }

            if( xStatus == pdFALSE )
            {
                vPortFree( pxParams );
            }
        }
    }

    return xStatus;
}


/*-----------------------------------------------------------*/

static BaseType_t prxHandleSaveNetworkRequest( uint8_t * pucData,
                                               size_t xLength )
{
    BaseType_t xStatus = pdTRUE;
    jsmntok_t xTokens[ wifiProvMAX_TOKENS ];
    int16_t sNumTokens;
    const char * pcValue = NULL;
    uint32_t ulValLen;
    AddNetworkRequest_t * pxAddNetworkReq = NULL;
    int16_t sSecurity;

    if( ( sNumTokens = JsonUtils_Parse( ( char * ) pucData, xLength, xTokens, wifiProvMAX_TOKENS ) ) > 0 )
    {
        pxAddNetworkReq = pvPortMalloc( sizeof( AddNetworkRequest_t ) );

        if( pxAddNetworkReq != NULL )
        {
            memset( pxAddNetworkReq, 0x00, sizeof( AddNetworkRequest_t ) );
            JsonUtils_GetStrValue( ( const char * ) pucData,
                                   xTokens,
                                   sNumTokens,
                                   wifiProvSSID_KEY,
                                   strlen( wifiProvSSID_KEY ),
                                   &pcValue,
                                   &ulValLen );

            if( ( pcValue != NULL ) && ( ulValLen <= wificonfigMAX_SSID_LEN ) )
            {
                strncpy( pxAddNetworkReq->xNetwork.cSSID, pcValue, ulValLen );
                pxAddNetworkReq->xNetwork.ucSSIDLength = ( ulValLen + 1 );
            }
            else
            {
                xStatus = pdFALSE;
            }

            if( xStatus == pdTRUE )
            {
            	/* Use an intermediary variable to avoid memory corruption. */
                xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                                   xTokens,
                                                   sNumTokens,
                                                   wifiProvKEY_MGMT_KEY,
                                                   strlen( wifiProvKEY_MGMT_KEY ),
                                                   ( int16_t * ) &sSecurity );
                pxAddNetworkReq->xNetwork.xSecurity = sSecurity;
            }

            if( xStatus == pdTRUE )
            {
                JsonUtils_GetStrValue( ( const char * ) pucData,
                                       xTokens,
                                       sNumTokens,
                                       wifiProvPSK_KEY,
                                       strlen( wifiProvPSK_KEY ),
                                       &pcValue,
                                       &ulValLen );

                if( ( pcValue != NULL ) && ( ulValLen <= wificonfigMAX_PASSPHRASE_LEN ) )
                {
                    strncpy( pxAddNetworkReq->xNetwork.cPassword, pcValue, ulValLen );
                    pxAddNetworkReq->xNetwork.ucPasswordLength = ( ulValLen + 1 );
                }
                else
                {
                    xStatus = pdFALSE;
                }
            }

            if( xStatus == pdTRUE )
            {
                xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                                   xTokens,
                                                   sNumTokens,
                                                   wifiProvINDEX_KEY,
                                                   strlen( wifiProvINDEX_KEY ),
                                                   ( int16_t * ) &pxAddNetworkReq->sSavedIdx );
            }

            if( xStatus == pdTRUE )
            {
                JsonUtils_GetStrValue( ( const char * ) pucData,
                                       xTokens,
                                       sNumTokens,
                                       wifiProvBSSID_KEY,
                                       strlen( wifiProvBSSID_KEY ),
                                       &pcValue,
                                       &ulValLen );

                if( ( pcValue != NULL ) && ( ulValLen == wifiProvBSSID_MAX_LEN ) )
                {
                    sscanf( pcValue, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &pxAddNetworkReq->xNetwork.ucBSSID[ 0 ],
                            &pxAddNetworkReq->xNetwork.ucBSSID[ 1 ], &pxAddNetworkReq->xNetwork.ucBSSID[ 2 ],
                            &pxAddNetworkReq->xNetwork.ucBSSID[ 3 ], &pxAddNetworkReq->xNetwork.ucBSSID[ 4 ], &pxAddNetworkReq->xNetwork.ucBSSID[ 5 ] );
                }
                else
                {
                    xStatus = pdFALSE;
                }
            }

            if( xStatus == pdTRUE )
            {
                xStatus = xTaskCreate(
                    prvAddNetworkTask,
                    "WifiProvAddNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pxAddNetworkReq,
					wifiProvMODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
            }

            if( xStatus == pdFALSE )
            {
                vPortFree( pxAddNetworkReq );
            }
        }
    }
    else
    {
        xStatus = pdFALSE;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prxHandleEditNetworkRequest( uint8_t * pucData,
                                               size_t xLength )
{
    BaseType_t xStatus = pdTRUE;
    jsmntok_t xTokens[ wifiProvMAX_TOKENS ];
    int16_t sNumTokens;
    EditNetworkRequest_t * pxEditNetworkReq = NULL;

    if( ( sNumTokens = JsonUtils_Parse( ( char * ) pucData, xLength, xTokens, wifiProvMAX_TOKENS ) ) > 0 )
    {
        pxEditNetworkReq = pvPortMalloc( sizeof( EditNetworkRequest_t ) );

        if( pxEditNetworkReq != NULL )
        {
            xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                               xTokens,
                                               sNumTokens,
                                               wifiProvINDEX_KEY,
                                               strlen( wifiProvINDEX_KEY ),
                                               ( int16_t * ) &pxEditNetworkReq->sCurIdx );

            if( xStatus == pdTRUE )
            {
                xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                                   xTokens,
                                                   sNumTokens,
                                                   wifiProvNEWINDEX_KEY,
                                                   strlen( wifiProvNEWINDEX_KEY ),
                                                   ( int16_t * ) &pxEditNetworkReq->sNewIdx );
            }

            if( xStatus == pdTRUE )
            {
                xStatus = xTaskCreate(
                    prvEditNetworkTask,
                    "WifiProvEditNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pxEditNetworkReq,
					wifiProvMODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
            }

            if( xStatus == pdFALSE )
            {
                vPortFree( pxEditNetworkReq );
            }
        }
    }
    else
    {
        xStatus = pdFALSE;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static BaseType_t prxHandleDeleteNetworkRequest( uint8_t * pucData,
                                                 size_t xLength )
{
    BaseType_t xStatus = pdTRUE;
    jsmntok_t xTokens[ wifiProvMAX_TOKENS ];
    int16_t sNumTokens;
    DeleteNetworkRequest_t * pxDeleteNetworkReq = NULL;

    if( ( sNumTokens = JsonUtils_Parse( ( char * ) pucData, xLength, xTokens, wifiProvMAX_TOKENS ) ) > 0 )
    {
        pxDeleteNetworkReq = pvPortMalloc( sizeof( DeleteNetworkRequest_t ) );

        if( pxDeleteNetworkReq != NULL )
        {
            xStatus = JsonUtils_GetInt16Value( ( char * ) pucData,
                                               xTokens,
                                               sNumTokens,
                                               wifiProvINDEX_KEY,
                                               strlen( wifiProvINDEX_KEY ),
                                               ( int16_t * ) &pxDeleteNetworkReq->sIdx );

            if( xStatus == pdTRUE )
            {
                xStatus = xTaskCreate(
                    prvDeleteNetworkTask,
                    "WifiProvDeleteNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pxDeleteNetworkReq,
					wifiProvMODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
            }

            if( xStatus == pdFALSE )
            {
                vPortFree( pxDeleteNetworkReq );
            }
        }
    }
    else
    {
        xStatus = pdFALSE;
    }

    return xStatus;
}

void prvSerializeNetwork( WifiNetworkInfo_t *pxNetwork, uint8_t *pucBuffer, uint32_t ulLen, uint32_t *pulLen )
{
	char cBSSID[ wifiProvBSSID_MAX_LEN + 1 ];

	if( ulLen >= wifProvLIST_NETWORK_RSP_LEN )
	{
		if( pxNetwork->xIsScan == pdTRUE )
		{
			snprintf( cBSSID, sizeof( cBSSID ), "%02x:%02x:%02x:%02x:%02x:%02x",  pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 0 ],
					pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 1 ], pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 2 ],
					pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 3 ], pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 4 ], pxNetwork->info.pxScannedNetworkInfo->ucBSSID[ 5 ] );

			*pulLen = snprintf( (char *) pucBuffer,
					ulLen,
					wifiProvLISTNETWORK_RSP_FORMAT,
					eWiFiSuccess,
					strlen( pxNetwork->info.pxScannedNetworkInfo->cSSID ),
					pxNetwork->info.pxScannedNetworkInfo->cSSID,
					wifiProvBSSID_MAX_LEN,
					cBSSID,
					pxNetwork->info.pxScannedNetworkInfo->xSecurity,
					( pxNetwork->info.pxScannedNetworkInfo->ucHidden == pdTRUE ) ? wifiProvTRUE : wifiProvFALSE,
					pxNetwork->info.pxScannedNetworkInfo->cRSSI,
					wifiProvFALSE,
					INVALID_INDEX );
		}
		else
		{
			snprintf( cBSSID, sizeof( cBSSID ), "%02x:%02x:%02x:%02x:%02x:%02x",  pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 0 ],
								pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 1 ], pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 2 ],
								pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 3 ], pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 4 ], pxNetwork->info.pxSavedNetworkInfo->ucBSSID[ 5 ] );
			*pulLen = snprintf( (char *) pucBuffer,
					 ulLen,
					 wifiProvLISTNETWORK_RSP_FORMAT,
					 eWiFiSuccess,
					 pxNetwork->info.pxSavedNetworkInfo->ucSSIDLength,
					 pxNetwork->info.pxSavedNetworkInfo->cSSID,
					 wifiProvBSSID_MAX_LEN,
					 cBSSID,
					 pxNetwork->info.pxSavedNetworkInfo->xSecurity,
					 wifiProvTRUE,
					 -100,
					 ( pxNetwork->xConnected ) ? wifiProvTRUE : wifiProvFALSE,
					 pxNetwork->sSavedIdx );
		}

	}
	else
	{
		*pulLen = 0;
	}
}

/*-----------------------------------------------------------*/

void prvSendStatusResponse( WifiProvCharacteristic_t xCharacteristic,
                            WIFIReturnCode_t xStatus )
{

    char cPayload[ wifiProvSTATUS_RSP_FORMAT_LEN + 1 ] = { 0 };
    size_t xPayloadLen = snprintf( cPayload, wifiProvSTATUS_RSP_FORMAT_LEN,
                            wifiProvSTATUS_RSP_FORMAT, xStatus );
    prvSendResponse( xCharacteristic, (uint8_t*) cPayload, xPayloadLen );
}

void prvSendResponse( WifiProvCharacteristic_t xCharacteristic, uint8_t* pucData, size_t xLen )
{
	BLEAttributeData_t xAttrData = { 0 };
	BLEEventResponse_t xResp = { 0 };

	xAttrData.xHandle = ATTR_HANDLE( &xWifiProvService, xCharacteristic );
	xAttrData.xUuid = ATTR_UUID( &xWifiProvService, xCharacteristic );
	xResp.xAttrDataOffset = 0;
	xResp.pxAttrData = &xAttrData;
	xResp.xRspErrorStatus = eBTRspErrorNone;

	xAttrData.pucData = pucData;
	xAttrData.xSize = xLen;

	( void ) BLE_SendIndication( &xResp, xWifiProvService.usBLEConnId, false );
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t prvAppendNetwork( WIFINetworkProfile_t * pxProfile )
{
    WIFIReturnCode_t xRet;
    uint16_t usIdx;

    xRet = WIFI_NetworkAdd( pxProfile, &usIdx );

    if( xRet == eWiFiSuccess )
    {
        xWifiProvService.usNumNetworks++;
        xWifiProvService.sConnectedIdx++;
    }

    return xRet;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t prvPopNetwork( uint16_t usIndex,
                                WIFINetworkProfile_t * pxProfile )
{
    WIFIReturnCode_t xRet = eWiFiSuccess;

    if( pxProfile != NULL )
    {
        xRet = WIFI_NetworkGet( pxProfile, STORAGE_INDEX( usIndex ) );
    }

    if( xRet == eWiFiSuccess )
    {
        xRet = WIFI_NetworkDelete( STORAGE_INDEX( usIndex ) );
    }

    if( xRet == eWiFiSuccess )
    {
        xWifiProvService.usNumNetworks--;

        /* Shift the priority for connected network */
        if( usIndex < xWifiProvService.sConnectedIdx )
        {
            xWifiProvService.sConnectedIdx--;
        }
        else if( usIndex == xWifiProvService.sConnectedIdx )
        {
            xWifiProvService.sConnectedIdx = INVALID_INDEX;
        }
    }

    return xRet;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t prvMoveNetwork( uint16_t usCurrentIndex,
                                 uint16_t usNewIndex )
{
    WIFIReturnCode_t xRet = eWiFiSuccess;
    WIFINetworkProfile_t xProfile;

    if( usCurrentIndex != usNewIndex )
    {
        xRet = prvPopNetwork( usCurrentIndex, &xProfile );

        if( xRet == eWiFiSuccess )
        {
            xRet = prvInsertNetwork( usNewIndex, &xProfile );
        }
    }

    return xRet;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t prvGetSavedNetwork( uint16_t usIndex,
                                     WIFINetworkProfile_t * pxProfile )
{
	return WIFI_NetworkGet( pxProfile, STORAGE_INDEX( usIndex ) );
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t prvConnectNetwork( WIFINetworkProfile_t * pxProfile )
{
    WIFIReturnCode_t xRet = eWiFiFailure;
    WIFINetworkParams_t xNetworkParams = { 0 };

    xNetworkParams.pcSSID = pxProfile->cSSID;
    xNetworkParams.ucSSIDLength = pxProfile->ucSSIDLength;
    xNetworkParams.pcPassword = pxProfile->cPassword;
    xNetworkParams.ucPasswordLength = pxProfile->ucPasswordLength;
    xNetworkParams.xSecurity = pxProfile->xSecurity;
    xRet = WIFI_ConnectAP( &xNetworkParams );
    return xRet;
}

WIFIReturnCode_t prvAddNewNetwork( WIFINetworkProfile_t * pxProfile )
{
    WIFIReturnCode_t xRet;

    xRet = prvConnectNetwork( pxProfile );
    if( xRet == eWiFiSuccess )
    {
    	xRet = prvAppendNetwork( pxProfile );
    	if( xRet == eWiFiSuccess )
    	{
    		xWifiProvService.sConnectedIdx = 0;
    	}
    }
    return xRet;
}


WIFIReturnCode_t prvConnectSavedNetwork( uint16_t usIndex )
{
    WIFIReturnCode_t xRet;
    WIFINetworkProfile_t xProfile;

    xRet = prvGetSavedNetwork( usIndex, &xProfile );
    if( xRet == eWiFiSuccess )
    {
    	xRet = prvConnectNetwork( &xProfile );
    	if( xRet == eWiFiSuccess )
    	{
    		xWifiProvService.sConnectedIdx = usIndex;
    	}
    }
    return xRet;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t prvInsertNetwork( uint16_t usIndex,
                                   WIFINetworkProfile_t * pxProfile )
{
    WIFIReturnCode_t xRet;
    WIFINetworkProfile_t xProfile;
    uint16_t usNumElementsToShift, x;

    /* All higher priority elements needs to be shifted */
    usNumElementsToShift = usIndex;

    xRet = prvAppendNetwork( pxProfile );

    if( xRet == eWiFiSuccess )
    {
        for( x = 0; x < usNumElementsToShift; x++ )
        {
            xRet = prvPopNetwork( usIndex, &xProfile );
            configASSERT( xRet == eWiFiSuccess );
            xRet = prvAppendNetwork( &xProfile );
            configASSERT( xRet == eWiFiSuccess );
        }
    }

    return xRet;
}

/*-----------------------------------------------------------*/

void prvListNetworkTask( void * pvParams )
{
    ListNetworkRequest_t * pxListNetworReq = ( ListNetworkRequest_t * ) pvParams;
    WIFIScanResult_t xScanResults[ pxListNetworReq->sMaxNetworks ];
    WIFINetworkProfile_t xProfile;
    WifiNetworkInfo_t xInfo;
    char cMesg[ wifProvLIST_NETWORK_RSP_LEN + 1 ];
    uint32_t ulMsgLen;
    WIFIReturnCode_t xRet = eWiFiSuccess;
    uint16_t usIdx;

    if( xSemaphoreTake( xWifiProvService.xLock, portMAX_DELAY ) == pdPASS )
    {
    	xInfo.xIsScan = pdFALSE;
    	for( usIdx = 0; usIdx < xWifiProvService.usNumNetworks; usIdx++ )
    	{
    		xRet = prvGetSavedNetwork( usIdx, &xProfile );
    		if( xRet == eWiFiSuccess )
    		{
    			xInfo.info.pxSavedNetworkInfo = &xProfile;
    			xInfo.xConnected = ( xWifiProvService.sConnectedIdx == usIdx );
    			xInfo.sSavedIdx = usIdx;
    			prvSerializeNetwork( &xInfo, (uint8_t*)cMesg, sizeof( cMesg ), &ulMsgLen );
    			prvSendResponse( eListNetworkChar, (uint8_t*)cMesg, ulMsgLen );
    		}
    	}

    	memset( xScanResults, 0x00, sizeof( WIFIScanResult_t ) * pxListNetworReq->sMaxNetworks );
    	xRet = WIFI_Scan( xScanResults, pxListNetworReq->sMaxNetworks );
    	if( xRet == eWiFiSuccess )
    	{
    		xInfo.xIsScan = pdTRUE;
    		xInfo.xConnected = pdFALSE;
    		xInfo.sSavedIdx = INVALID_INDEX;

    		for( usIdx = 0; usIdx < pxListNetworReq->sMaxNetworks; usIdx++ )
    		{
    			if( strlen( xScanResults[ usIdx ].cSSID ) > 0 )
    			{
    				xInfo.info.pxScannedNetworkInfo = &xScanResults[ usIdx ];
    				prvSerializeNetwork( &xInfo, (uint8_t*)cMesg, sizeof( cMesg ), &ulMsgLen );
    				prvSendResponse( eListNetworkChar, (uint8_t*)cMesg, ulMsgLen );
    			}
    		}
    	}
    	else
    	{
    		prvSendStatusResponse( eListNetworkChar, xRet );
    	}


    	xSemaphoreGive( xWifiProvService.xLock );
    }

    vPortFree( pxListNetworReq );
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

void prvAddNetworkTask( void * pvParams )
{
    WIFIReturnCode_t xRet = eWiFiFailure;
    AddNetworkRequest_t * pxAddNetworkReq = ( AddNetworkRequest_t * ) pvParams;

    if( xSemaphoreTake( xWifiProvService.xLock, portMAX_DELAY ) == pdPASS )
    {
        if( pxAddNetworkReq->sSavedIdx != INVALID_INDEX )
        {
        	xRet = prvConnectSavedNetwork( pxAddNetworkReq->sSavedIdx );
        }
        else
        {
        	xRet = prvAddNewNetwork( &pxAddNetworkReq->xNetwork );
        }
        xSemaphoreGive( xWifiProvService.xLock );
    }

    if( xRet == eWiFiSuccess )
    {
        prvSetEvent( eWIFIPROVConnected );
    }

    prvSendStatusResponse( eSaveNetworkChar, xRet );
    vPortFree( pxAddNetworkReq );
    vTaskDelete( NULL );
}



/*-----------------------------------------------------------*/

void prvDeleteNetworkTask( void * pvParams )
{
    WIFIReturnCode_t xRet = eWiFiFailure;
    DeleteNetworkRequest_t * pxDeleteNetworkReq = ( DeleteNetworkRequest_t * ) pvParams;


    if( xSemaphoreTake( xWifiProvService.xLock, portMAX_DELAY ) == pdPASS )
    {
        xRet = prvPopNetwork( pxDeleteNetworkReq->sIdx, NULL );

        if( xRet == eWiFiSuccess )
        {
            if( xWifiProvService.sConnectedIdx == INVALID_INDEX )
            {
                ( void ) WIFI_Disconnect();
            }
        }
        xSemaphoreGive( xWifiProvService.xLock );
    }

    prvSendStatusResponse( eDeleteNetworkChar, xRet );
    vPortFree( pxDeleteNetworkReq );
    vTaskDelete( NULL );
}



/*-----------------------------------------------------------*/

void prvEditNetworkTask( void * pvParams )
{
    WIFIReturnCode_t xRet = eWiFiFailure;
    EditNetworkRequest_t * pxEditNetworkReq = ( EditNetworkRequest_t * ) pvParams;

    if( xSemaphoreTake( xWifiProvService.xLock, portMAX_DELAY ) == pdPASS )
    {
        xRet = prvMoveNetwork( pxEditNetworkReq->sCurIdx, pxEditNetworkReq->sNewIdx );
        xSemaphoreGive( xWifiProvService.xLock );
    }

    prvSendStatusResponse( eEditNetworkChar, xRet );
    vPortFree( pxEditNetworkReq );
    vTaskDelete( NULL );
}


/*-----------------------------------------------------------*/

static void prvSetEvent( WifiProvEvent_t xEvent )
{
    ( void ) xEventGroupClearBits( xWifiProvService.xEventGroup, ALL_EVENTS );
    ( void ) xEventGroupSetBits( xWifiProvService.xEventGroup, xEvent );
}

/*-----------------------------------------------------------*/

static BaseType_t prvWaitForEvent( WifiProvEvent_t xEvent,
                                          TickType_t xTimeout )
{
    EventBits_t xSetBits = xEventGroupWaitBits( xWifiProvService.xEventGroup, ( xEvent | eWIFIPROVFailed ), pdFALSE, pdFALSE, xTimeout );

    return( !( xSetBits & eWIFIPROVFailed ) && ( xSetBits & xEvent ) );
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_PROVISION_Init( void )
{
    BaseType_t xStatus = pdTRUE;

    if( xWifiProvService.xInit == pdFALSE )
    {
        xWifiProvService.xEventGroup = xEventGroupCreate();

        if( xWifiProvService.xEventGroup != NULL )
        {
        	( void ) xEventGroupClearBits( xWifiProvService.xEventGroup, ALL_EVENTS );
        }
        else
        {
            xStatus = pdFALSE;
        }

        if( xStatus == pdTRUE )
        {
            xWifiProvService.xLock = xSemaphoreCreateMutex();

            if( xWifiProvService.xLock != NULL )
            {
                xSemaphoreGive( xWifiProvService.xLock );
            }
            else
            {
                xStatus = pdFALSE;
            }
        }

        if( xStatus == pdTRUE )
        {
            xStatus = prxInitGATTService();
        }
        if( xStatus == pdTRUE )
        {
        	xWifiProvService.sConnectedIdx = INVALID_INDEX;
        	xWifiProvService.xInit = pdTRUE;
        }
    }
    else
    {
        xStatus = pdFALSE;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_PROVISION_Start( void )
{
    BaseType_t xRet = pdFALSE;

    if( xWifiProvService.xInit == pdTRUE )
    {
    	xWifiProvService.usNumNetworks = prvGetNumSavedNetworks();
    	if( BLE_StartService( xWifiProvService.pxGattService, vServiceStartedCb ) == eBTStatusSuccess )
    	{
    		xRet = prvWaitForEvent( eWIFIPROVStarted, portMAX_DELAY );
    	}
    }

    return xRet;
}

/*-----------------------------------------------------------*/
uint16_t WIFI_PROVISION_GetNumNetworks( void )
{
	return prvGetNumSavedNetworks();
}

BaseType_t WIFI_PROVISION_Connect( uint16_t usNetworkIndex )
{
	WIFIReturnCode_t xWifiRet;
	BaseType_t xRet = pdFALSE;
	xWifiRet = prvConnectSavedNetwork( usNetworkIndex );
	if( xWifiRet == eWiFiSuccess )
	{
		xRet = pdTRUE;
	}
	return xRet;
}


BaseType_t WIFI_PROVISION_EraseAllNetworks( void )
{

    BaseType_t xRet = pdTRUE;
    WIFIReturnCode_t xWiFiRet;

    if( xSemaphoreTake( xWifiProvService.xLock, portMAX_DELAY ) == pdPASS )
    {
        while( xWifiProvService.usNumNetworks > 0 )
        {
            xWiFiRet = prvPopNetwork( 0, NULL );
            if( xWiFiRet != eWiFiSuccess )
            {
                configPRINTF(( "Failed to delete WIFI network, error = %d\n", xWiFiRet ));
                xRet = pdFALSE;
                break;
            }
        }
        xSemaphoreGive( xWifiProvService.xLock );
    }
    else
    {
        xRet = pdFALSE;
    }

    return xRet;
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_PROVISION_Stop( void )
{
    BLE_StopService( xWifiProvService.pxGattService, vServiceStoppedCb );
    return prvWaitForEvent( eWIFIPROVStopped, portMAX_DELAY );
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_PROVISION_Delete( void )
{
	BaseType_t xRet = pdFALSE;

    if( BLE_DeleteService( xWifiProvService.pxGattService, vServiceDeletedCb ) == eBTStatusSuccess )
    {
    	xRet = prvWaitForEvent( eWIFIPROVDeleted, portMAX_DELAY );
    }
    if( xRet == pdTRUE )
    {

    	if( xWifiProvService.xLock != NULL )
    	{
    		vSemaphoreDelete( xWifiProvService.xLock );
    	}

    	if( xWifiProvService.xEventGroup != NULL )
    	{
    		vEventGroupDelete( xWifiProvService.xEventGroup );
    	}

    	memset( &xWifiProvService, 0x00, sizeof( WifiProvService_t ) );
    }

    return xRet;

}

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ble_wifi_prov_test_access_define.h"
#endif
