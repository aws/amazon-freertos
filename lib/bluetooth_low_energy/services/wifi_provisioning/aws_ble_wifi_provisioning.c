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

#include "iot_ble_config.h"
#include "aws_ble_wifi_provisioning.h"
#include "semphr.h"
#include "aws_json_utils.h"


#define ATTR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ch_idx] )
#define ATTR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ch_idx].xCharacteristic.xUuid )

/*---------------------------------------------------------------------------------------------------------*/

static WifiProvService_t xWifiProvService = { 0 };

#define STORAGE_INDEX( priority )    ( xWifiProvService.usNumNetworks - priority - 1 )
#define INVALID_INDEX    ( -1 )
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define wifiProvLIST_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = wifiProvLIST_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define wifiProvSAVE_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = wifiProvSAVE_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define wifiProvEDIT_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = wifiProvEDIT_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define wifiProvDELETE_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = wifiProvDELETE_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define wifiProvCLIENT_CHAR_CFG_UUID_TYPE \
{ \
    .uu.uu16 = wifiProvCLIENT_CHAR_CFG_UUID,\
    .ucType   = eBTuuidType16\
}


#define wifiProvSVC_UUID_TYPE \
{ \
    .uu.uu128 = wifiProvSVC_UUID,\
    .ucType   = eBTuuidType128\
}

static uint16_t usHandlesBuffer[eNbAttributes];

static const BTAttribute_t pxAttributeTable[] = {
     {
         .xServiceUUID =  wifiProvSVC_UUID_TYPE
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic =
         {
              .xUuid = wifiProvLIST_NETWORK_CHAR_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
              .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = wifiProvSAVE_NETWORK_CHAR_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
          }
     },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = wifiProvSAVE_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = wifiProvSAVE_NETWORK_CHAR_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = wifiProvEDIT_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = wifiProvSAVE_NETWORK_CHAR_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = wifiProvDELETE_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = wifiProvSAVE_NETWORK_CHAR_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 }
};

static const BTService_t xWIFIProvisionningService =
{
  .xNumberOfAttributes = eNbAttributes,
  .ucInstId = 0,
  .xType = eBTServiceTypePrimary,
  .pusHandlesBuffer = usHandlesBuffer,
  .pxBLEAttributes = (BTAttribute_t *)pxAttributeTable
};
/*
 * @brief Callback registered for BLE write and read events received for each characteristic.
 */
static void vCharacteristicCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback registered for client characteristic configuration descriptors.
 */
static void vClientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Function used to create and initialize BLE service.
 */
static BaseType_t prxInitGATTService( void );

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
static WifiProvAttributes_t prxGetCharFromHandle( uint16_t xHandle );


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
void prvSendStatusResponse( WifiProvAttributes_t xChar, WIFIReturnCode_t xStatus );


void prvSendResponse( WifiProvAttributes_t xCharacteristic, uint8_t* pucData, size_t xLen );


/*-----------------------------------------------------------*/


static const IotBleAttributeEventCallback_t pxCallBackArray[eNbAttributes] =
{
  NULL,
  vCharacteristicCallback,
  vClientCharCfgDescrCallback,
  vCharacteristicCallback,
  vClientCharCfgDescrCallback,
  vCharacteristicCallback,
  vClientCharCfgDescrCallback,
  vCharacteristicCallback,
  vClientCharCfgDescrCallback
};



BaseType_t prxInitGATTService( void )
{
    BTStatus_t xStatus;
    BaseType_t xResult = pdFAIL;

    /* Select the handle buffer. */
    xStatus = IotBleCreateService( (BTService_t *)&xWIFIProvisionningService, (IotBleAttributeEventCallback_t *)pxCallBackArray );
	if( xStatus == eBTStatusSuccess )
	{
		xResult = pdPASS;
	}

    return xResult;
}

/*-----------------------------------------------------------*/

static WifiProvAttributes_t prxGetCharFromHandle( uint16_t usHandle )
{
    uint8_t ucCharId;

    for( ucCharId = 0; ucCharId < eNbAttributes; ucCharId++ )
    {
        if( usHandlesBuffer[ucCharId] == usHandle )
        {
            break;
        }
    }

    return ( WifiProvAttributes_t ) ucCharId;
}

/*-----------------------------------------------------------*/

BaseType_t WIFI_PROVISION_Start( void )
{
    BaseType_t xRet = pdFALSE;

    if( xWifiProvService.xInit == pdTRUE )
    {
    	xWifiProvService.usNumNetworks = prvGetNumSavedNetworks();
    	xRet = pdTRUE;
    }

    return xRet;
}

void vCharacteristicCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleReadEventParams_t * pxReadParam;
    BaseType_t xResult = pdFAIL;

    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp =
    {
        .eventStatus    = eBTStatusFail,
        .rspErrorStatus = eBTRspErrorNone
    };
    WifiProvAttributes_t xChar;

    xResp.pAttrData = &xAttrData;

    if( pEventParam->xEventType == eBLEWrite )
    {
        pxWriteParam = pEventParam->pParamWrite;
        xWifiProvService.usBLEConnId = pxWriteParam->connId;

        xChar = prxGetCharFromHandle( pxWriteParam->attrHandle );

        switch( xChar )
        {
            case eListNetworkChar:
                xResult = prxHandleListNetworkRequest( pxWriteParam->pValue, pxWriteParam->length );
                break;

            case eSaveNetworkChar:
                xResult = prxHandleSaveNetworkRequest( pxWriteParam->pValue, pxWriteParam->length );
                break;

            case eEditNetworkChar:
                xResult = prxHandleEditNetworkRequest( pxWriteParam->pValue, pxWriteParam->length );
                break;

            case eDeleteNetworkChar:
                xResult = prxHandleDeleteNetworkRequest( pxWriteParam->pValue, pxWriteParam->length );
                break;

            case eNbAttributes:
            default:
                xResult = pdFAIL;
                break;
        }

        if( xResult == pdPASS )
        {
            xResp.eventStatus = eBTStatusSuccess;
        }

        xResp.pAttrData->handle = pxWriteParam->attrHandle;
        xResp.pAttrData->pData = pxWriteParam->pValue;
        xResp.pAttrData->size = pxWriteParam->length;
        xResp.attrDataOffset = pxWriteParam->offset;
        IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pxReadParam = pEventParam->pParamRead;
        xResp.pAttrData->handle = pxReadParam->attrHandle;
        xResp.pAttrData->pData = NULL;
        xResp.pAttrData->size = 0;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        IotBle_SendResponse( &xResp, pxReadParam->connId, pxReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

static void vClientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pxWriteParam;
    IotBleReadEventParams_t * pxReadParam;

    IotBleAttributeData_t xAttrData = { 0 };
    IotBleEventResponse_t xResp =
    {
        .eventStatus    = eBTStatusSuccess,
        .rspErrorStatus = eBTRspErrorNone
    };

    xResp.pAttrData = &xAttrData;

    if( pEventParam->xEventType == eBLEWrite )
    {
        pxWriteParam = pEventParam->pParamWrite;

        if( pxWriteParam->length == 2 )
        {
            xWifiProvService.usNotifyClientEnabled = ( pxWriteParam->pValue[ 1 ] << 8 ) | pxWriteParam->pValue[ 0 ];
            xWifiProvService.usBLEConnId = pxWriteParam->connId;
            xResp.pAttrData->handle = pxWriteParam->attrHandle;
            xResp.pAttrData->pData = pxWriteParam->pValue;
            xResp.pAttrData->size = pxWriteParam->length;
            xResp.attrDataOffset = pxWriteParam->offset;
        }

        IotBle_SendResponse( &xResp, pxWriteParam->connId, pxWriteParam->transId );
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pxReadParam = pEventParam->pParamRead;
        xResp.pAttrData->handle = pxReadParam->attrHandle;
        xResp.pAttrData->pData = ( uint8_t * ) &xWifiProvService.usNotifyClientEnabled;
        xResp.pAttrData->size = 2;
        xResp.attrDataOffset = 0;
        xResp.eventStatus = eBTStatusSuccess;
        IotBle_SendResponse( &xResp, pxReadParam->connId, pxReadParam->transId );
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
                if(pxParams->sMaxNetworks > IOT_BLE_MAX_NETWORK )
                {
                    configPRINTF(("Too many network request: %d, expected at most %d\n", pxParams->sMaxNetworks, IOT_BLE_MAX_NETWORK));
                    pxParams->sMaxNetworks = IOT_BLE_MAX_NETWORK;
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

void prvSendStatusResponse( WifiProvAttributes_t xCharacteristic,
                            WIFIReturnCode_t xStatus )
{

    char cPayload[ wifiProvSTATUS_RSP_FORMAT_LEN + 1 ] = { 0 };
    size_t xPayloadLen = snprintf( cPayload, wifiProvSTATUS_RSP_FORMAT_LEN,
                            wifiProvSTATUS_RSP_FORMAT, xStatus );
    prvSendResponse( xCharacteristic, (uint8_t*) cPayload, xPayloadLen );
}

void prvSendResponse( WifiProvAttributes_t xCharacteristic, uint8_t* pucData, size_t xLen )
{
	IotBleAttributeData_t xAttrData = { 0 };
	IotBleEventResponse_t xResp = { 0 };

	xAttrData.handle = ATTR_HANDLE( xWifiProvService.pxGattService, xCharacteristic );
	xAttrData.xUuid = ATTR_UUID( xWifiProvService.pxGattService, xCharacteristic );
	xResp.attrDataOffset = 0;
	xResp.pAttrData = &xAttrData;
	xResp.rspErrorStatus = eBTRspErrorNone;

	xAttrData.pucData = pucData;
	xAttrData.xSize = xLen;

	( void ) IotBle_SendIndication( &xResp, xWifiProvService.usBLEConnId, false );
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

BaseType_t WIFI_PROVISION_Init( void )
{
    BaseType_t xStatus = pdTRUE;

    if( xWifiProvService.xInit == pdFALSE )
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

        if( xStatus == pdTRUE )
        {
        	xWifiProvService.pxGattService = ( BTService_t *)&xWIFIProvisionningService;
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

BaseType_t WIFI_PROVISION_Delete( void )
{
	BaseType_t xRet = pdFALSE;

    if( BLE_DeleteService( xWifiProvService.pxGattService ) == eBTStatusSuccess )
    {
    	xRet = pdTRUE;
    }
    if( xRet == pdTRUE )
    {

    	if( xWifiProvService.xLock != NULL )
    	{
    		vSemaphoreDelete( xWifiProvService.xLock );
    	}

    	memset( &xWifiProvService, 0x00, sizeof( WifiProvService_t ) );
    }

    return xRet;

}

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ble_wifi_prov_test_access_define.h"
#endif
