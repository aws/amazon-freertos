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

#include <string.h>


#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

#include "iot_ble_config.h"
#include "iot_ble_wifi_provisioning.h"
#include "aws_iot_serializer.h"

#define ATTR_HANDLE( svc, ch_idx )        ( ( svc )->pusHandlesBuffer[ch_idx] )
#define ATTR_UUID( svc, ch_idx )          ( ( svc )->pxBLEAttributes[ch_idx].xCharacteristic.xUuid )

#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf )                                \
    (  ( ret == AWS_IOT_SERIALIZER_SUCCESS ) ||                                        \
          (  ( !pxSerializerBuf ) && ( ret == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )
/*---------------------------------------------------------------------------------------------------------*/

static IotBleWifiProvService_t WifiProvService = { 0 };

#define STORAGE_INDEX( priority )    ( WifiProvService.numNetworks - priority - 1 )
#define NETWORK_INFO_DEFAULT_PARAMS        { .status = eWiFiSuccess, .RSSI = IOT_BLE_WIFI_PROV_INVALID_NETWORK_RSSI, .connected = false, .savedIdx = IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX }
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief UUID for Device Information Service.
 *
 * This UUID is used in advertisement for the companion apps to discover and connect to the device.
 */
#define IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_UUID,\
    .ucType   = eBTuuidType128\
}

#define IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID_TYPE \
{ \
    .uu.uu16 = IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID,\
    .ucType   = eBTuuidType16\
}


#define IOT_BLE_WIFI_PROV_SVC_UUID_TYPE \
{ \
    .uu.uu128 = IOT_BLE_WIFI_PROV_SVC_UUID,\
    .ucType   = eBTuuidType128\
}

static uint16_t handlesBuffer[IOT_BLE_WIFI_PROV_NB_ATTRIBUTES];

static const BTAttribute_t pAttributeTable[] = {
     {
         .xServiceUUID =  IOT_BLE_WIFI_PROV_SVC_UUID_TYPE
     },
    {
         .xAttributeType = eBTDbCharacteristic,
         .xCharacteristic =
         {
              .xUuid = IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR_UUID_TYPE,
              .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
              .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
          }
     },
     {
         .xAttributeType = eBTDbDescriptor,
         .xCharacteristicDescr =
         {
             .xUuid = IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID_TYPE,
             .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
          }
     },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 },
	{
		 .xAttributeType = eBTDbCharacteristic,
		 .xCharacteristic =
		 {
			  .xUuid = IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR_UUID_TYPE,
			  .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  ),
			  .xProperties = ( eBTPropRead | eBTPropWrite | eBTPropNotify )
		  }
	 },
	 {
		 .xAttributeType = eBTDbDescriptor,
		 .xCharacteristicDescr =
		 {
			 .xUuid = IOT_BLE_WIFI_PROV_CLIENT_CHAR_CFG_UUID_TYPE,
			 .xPermissions = ( IOT_BLE_CHAR_READ_PERM | IOT_BLE_CHAR_WRITE_PERM  )
		  }
	 }
};

static const BTService_t WIFIProvisionningService =
{
  .xNumberOfAttributes = IOT_BLE_WIFI_PROV_NB_ATTRIBUTES,
  .ucInstId = 0,
  .xType = eBTServiceTypePrimary,
  .pusHandlesBuffer = handlesBuffer,
  .pxBLEAttributes = (BTAttribute_t *)pAttributeTable
};
/*
 * @brief Callback registered for BLE write and read events received for each characteristic.
 */
static void _characteristicCallback( IotBleAttributeEvent_t * pEventParam );

/*
 * @brief Callback registered for client characteristic configuration descriptors.
 */
static void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam );

static BaseType_t _deserializeListNetworkRequest( uint8_t * pData, size_t length, IotBleListNetworkRequest_t* pListNetworkRequest );

/*
 * @brief Parses List Network request params and creates task to list networks.
 */
static BaseType_t _handleListNetworkRequest( uint8_t * pData,
                                               size_t length );


static BaseType_t _deserializeAddNetworkRequest( uint8_t * pData, size_t length, IotBleAddNetworkRequest_t* pAddNetworkRequest );
/*
 * @brief Parses Save Network request params and creates task to save the new network.
 */
static BaseType_t _handleSaveNetworkRequest( uint8_t * pData,
                                               size_t length );


static BaseType_t _deserializeEditNetworkRequest( uint8_t * pData, size_t length, IotBleEditNetworkRequest_t* pEditNetworkRequest );

/*
 * @brief Parses Edit Network request params and creates task to edit network priority.
 */
static BaseType_t _handleEditNetworkRequest( uint8_t * pData,
                                               size_t length );

static BaseType_t _deserializeDeleteNetworkRequest( uint8_t * pData, size_t length, IotBleDeleteNetworkRequest_t* pDeleteNetworkRequest );

/*
 * @brief Parses Delete Network request params and creates task to delete a WiFi networ.
 */
static BaseType_t _handleDeleteNetworkRequest( uint8_t * pData,
                                                 size_t length );

/*
 * @brief Gets the GATT characteristic for a given attribute handle.
 */
static IotBleWifiProvAttributes_t _getCharFromHandle( uint16_t handle );


WIFIReturnCode_t _appendNetwork( WIFINetworkProfile_t * pProfile );

WIFIReturnCode_t _insertNetwork( uint16_t index,
                                   WIFINetworkProfile_t * pProfile );

WIFIReturnCode_t _popNetwork( uint16_t index,
                                WIFINetworkProfile_t * pProfile );

WIFIReturnCode_t _moveNetwork( uint16_t currentIndex,
                                 uint16_t newIndex );

WIFIReturnCode_t _getSavedNetwork( uint16_t index,
                                     WIFINetworkProfile_t * pProfile );

WIFIReturnCode_t _connectNetwork( WIFINetworkProfile_t * pProfile );

WIFIReturnCode_t _connectSavedNetwork( uint16_t index );

WIFIReturnCode_t _addNewNetwork( WIFINetworkProfile_t * pProfile, bool connect );


static AwsIotSerializerError_t _serializeNetwork( IotBleWifiNetworkInfo_t *pNetworkInfo, uint8_t *pBuffer, size_t *plength );

static void _sendSavedNetwork( WIFINetworkProfile_t *pSavedNetwork, uint16_t idx );

static void _sendScanNetwork( WIFIScanResult_t *pScanNetwork );

static AwsIotSerializerError_t _serializeStatusResponse( WIFIReturnCode_t status, uint8_t* pBuffer, size_t* plength );

/*
 * @brief  The task lists the saved network configurations in flash and also scans nearby networks.
 * It sends the profile information for each saved and scanned networks one at a time to the GATT client.
 * Maximum number of networks to scan is set in the List network request.
 */
static void _listNetworkTask( void * pParams );

/*
 * @brief  The task is used to save a new WiFi configuration.
 * It first connects to the network and if successful,saves the network onto flash with the highest priority.
 */
static void _addNetworkTask( void * pParams );

/*
 * @brief  The task is used to reorder priorities of network profiles stored in flash.
 *  If the priority of existing connected network changes then it initiates a reconnection.
 */
void _editNetworkTask( void * pParams );

/*
 * @brief  The task is used to delete a network configuration from the flash.
 * If the network is connected, it disconnects from the network and initiates a reconnection.
 */
void _deleteNetworkTask( void * pParams );

/*
 * @brief Gets the number of saved networks from flash.
 */
static uint16_t _getNumSavedNetworks( void );

/*
 * @brief Sends a status response for the request.
 */
void _sendStatusResponse( IotBleWifiProvAttributes_t characteristic, WIFIReturnCode_t status );


void _sendResponse( IotBleWifiProvAttributes_t characteristic, uint8_t* pData, size_t len );


/*-----------------------------------------------------------*/


static const IotBleAttributeEventCallback_t pCallBackArray[IOT_BLE_WIFI_PROV_NB_ATTRIBUTES] =
{
  NULL,
  _characteristicCallback,
  _clientCharCfgDescrCallback,
  _characteristicCallback,
  _clientCharCfgDescrCallback,
  _characteristicCallback,
  _clientCharCfgDescrCallback,
  _characteristicCallback,
  _clientCharCfgDescrCallback
};

/*-----------------------------------------------------------*/

static IotBleWifiProvAttributes_t _getCharFromHandle( uint16_t handle )
{
    uint8_t charId;

    for( charId = 0; charId < IOT_BLE_WIFI_PROV_NB_ATTRIBUTES; charId++ )
    {
        if( handlesBuffer[charId] == handle )
        {
            break;
        }
    }

    return ( IotBleWifiProvAttributes_t ) charId;
}

/*-----------------------------------------------------------*/

BaseType_t IotBleWifiProv_Start( void )
{
    BaseType_t ret = pdFALSE;

    if( WifiProvService.init == pdTRUE )
    {
    	WifiProvService.numNetworks = _getNumSavedNetworks();
        ret = pdTRUE;
    }

    return ret;
}

void _characteristicCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleReadEventParams_t * pReadParam;
    BaseType_t result = pdFAIL;

    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .eventStatus    = eBTStatusFail,
        .rspErrorStatus = eBTRspErrorNone
    };
    IotBleWifiProvAttributes_t characteristic;

    resp.pAttrData = &attrData;

    if( pEventParam->xEventType == eBLEWrite )
    {
        pWriteParam = pEventParam->pParamWrite;
        WifiProvService.BLEConnId = pWriteParam->connId;

        characteristic = _getCharFromHandle( pWriteParam->attrHandle );

        switch( characteristic )
        {
            case IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR:
                result = _handleListNetworkRequest( pWriteParam->pValue, pWriteParam->length );
                break;

            case IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR:
                result = _handleSaveNetworkRequest( pWriteParam->pValue, pWriteParam->length );
                break;

            case  IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR:
                result = _handleEditNetworkRequest( pWriteParam->pValue, pWriteParam->length );
                break;

            case IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR:
                result = _handleDeleteNetworkRequest( pWriteParam->pValue, pWriteParam->length );
                break;

            case IOT_BLE_WIFI_PROV_NB_ATTRIBUTES:
            default:
                result = pdFAIL;
                break;
        }

        if( result == pdPASS )
        {
            resp.eventStatus = eBTStatusSuccess;
        }

        resp.pAttrData->handle = pWriteParam->attrHandle;
        resp.pAttrData->pData = pWriteParam->pValue;
        resp.pAttrData->size = pWriteParam->length;
        resp.attrDataOffset = pWriteParam->offset;
        IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pReadParam = pEventParam->pParamRead;
        resp.pAttrData->handle = pReadParam->attrHandle;
        resp.pAttrData->pData = NULL;
        resp.pAttrData->size = 0;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        IotBle_SendResponse( &resp, pReadParam->connId, pReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

static void _clientCharCfgDescrCallback( IotBleAttributeEvent_t * pEventParam )
{
    IotBleWriteEventParams_t * pWriteParam;
    IotBleReadEventParams_t * pReadParam;

    IotBleAttributeData_t attrData = { 0 };
    IotBleEventResponse_t resp =
    {
        .eventStatus    = eBTStatusSuccess,
        .rspErrorStatus = eBTRspErrorNone
    };

    resp.pAttrData = &attrData;

    if( pEventParam->xEventType == eBLEWrite )
    {
        pWriteParam = pEventParam->pParamWrite;

        if( pWriteParam->length == 2 )
        {
            WifiProvService.notifyClientEnabled = ( pWriteParam->pValue[ 1 ] << 8 ) | pWriteParam->pValue[ 0 ];
            WifiProvService.BLEConnId = pWriteParam->connId;
            resp.pAttrData->handle = pWriteParam->attrHandle;
            resp.pAttrData->pData = pWriteParam->pValue;
            resp.pAttrData->size = pWriteParam->length;
            resp.attrDataOffset = pWriteParam->offset;
        }

        IotBle_SendResponse( &resp, pWriteParam->connId, pWriteParam->transId );
    }
    else if( pEventParam->xEventType == eBLERead )
    {
        pReadParam = pEventParam->pParamRead;
        resp.pAttrData->handle = pReadParam->attrHandle;
        resp.pAttrData->pData = ( uint8_t * ) &WifiProvService.notifyClientEnabled;
        resp.pAttrData->size = 2;
        resp.attrDataOffset = 0;
        resp.eventStatus = eBTStatusSuccess;
        IotBle_SendResponse( &resp, pReadParam->connId, pReadParam->transId );
    }
}

/*-----------------------------------------------------------*/

static uint16_t _getNumSavedNetworks( void )
{
    uint16_t idx;
    WIFIReturnCode_t WifiRet;
    WIFINetworkProfile_t profile;

    for( idx = 0; idx < IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS; idx++ )
    {
        WifiRet = WIFI_NetworkGet( &profile, idx );

        if( WifiRet != eWiFiSuccess )
        {
            break;
        }
    }

    return idx;
}

static BaseType_t _deserializeListNetworkRequest( uint8_t * pData, size_t length, IotBleListNetworkRequest_t* pListNetworkRequest )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdTRUE;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize the decoder, error = %d, object type = %d\n", ret, decoderObj.type ));
        result = pdFALSE;
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get max Networks parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( ( value.value.signedInt <= 0 )
                    || ( value.value.signedInt  > IOT_BLE_MAX_NETWORK ) )
            {
                
                    configPRINTF(( "WARN: Max Networks (%d) exceeds configured Max networks (%d). Caping max networks to %d\n",
                        value.value.signedInt,
                        IOT_BLE_MAX_NETWORK,
                        IOT_BLE_MAX_NETWORK ));
                pListNetworkRequest->maxNetworks = IOT_BLE_MAX_NETWORK;
            }
            else
            {
                pListNetworkRequest->maxNetworks = value.value.signedInt;
            }
        }
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get timeout parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {

            pListNetworkRequest->timeoutMs = value.value.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );
    return result;
}


/*-----------------------------------------------------------*/

static BaseType_t _handleListNetworkRequest( uint8_t * pData,
                                               size_t length )
{
    BaseType_t status = pdFALSE;
    IotBleListNetworkRequest_t * pParams = pvPortMalloc( sizeof( IotBleListNetworkRequest_t ));
    if( pParams != NULL )
    {
        status = _deserializeListNetworkRequest( pData, length, pParams );
        if( status == pdTRUE )
        {
            status = xTaskCreate(
                    _listNetworkTask,
                    "WifiProvListNetwork",
                    configMINIMAL_STACK_SIZE * 6,
                    pParams,
					IOT_BLE_WIFI_PROV_TASK_PRIORITY,
                    NULL );
        }
        else
        {
            vPortFree( pParams );
        }
    }
    else
    {
        configPRINTF(( "Failed to allocate memory for List Network Request\n" ));
    }

    return status;
}

static BaseType_t _deserializeAddNetworkRequest( uint8_t * pData, size_t length, IotBleAddNetworkRequest_t* pAddNetworkRequest )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdTRUE;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize the decoder, error = %d, object type = %d\n", ret, decoderObj.type ));
        result = pdFALSE;
    }

    if( result == pdTRUE )
    {
        value.value.pString = NULL;
        value.value.stringLength = 0;
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_SSID_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            configPRINTF(( "Failed to get SSID parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( value.value.stringLength >= wificonfigMAX_SSID_LEN )
            {
                configPRINTF(( "SSID, %.*s, exceeds maximum length %d\n",
                        value.value.stringLength,
                        ( const char * )value.value.pString,
                        wificonfigMAX_SSID_LEN ));
                result = pdFALSE;
            }
            else
            {
                strncpy( pAddNetworkRequest->network.cSSID, ( const char * ) value.value.pString, value.value.stringLength );
                pAddNetworkRequest->network.cSSID[ value.value.stringLength ] = '\0';
                pAddNetworkRequest->network.ucSSIDLength = ( value.value.stringLength + 1 );
            }
        }
    }

    if( result == pdTRUE )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
        value.value.pString = NULL;
        value.value.stringLength = 0;
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_BSSID_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING ) )
        {
            configPRINTF(( "Failed to get BSSID parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( value.value.stringLength != wificonfigMAX_BSSID_LEN )
            {
                configPRINTF(( "Parameter BSSID length (%d) does not match BSSID length %d\n",
                        value.value.stringLength,
                        wificonfigMAX_BSSID_LEN ));
                result = pdFALSE;
            }
            else
            {
                memcpy( pAddNetworkRequest->network.ucBSSID, value.value.pString, wificonfigMAX_BSSID_LEN );
            }
        }
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get WIFI xSecurity parameter, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            pAddNetworkRequest->network.xSecurity = value.value.signedInt;
        }
    }

    if( result == pdTRUE )
    {
        value.value.pString = NULL;
          value.value.stringLength = 0;
          ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_PSK_KEY, &value );
          if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                  ( value.type != AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
          {
              configPRINTF(( "Failed to get password parameter, error = %d, value type = %d\n", ret, value.type ));
              result = pdFALSE;
          }
          else
          {
              if( value.value.stringLength >= wificonfigMAX_PASSPHRASE_LEN )
              {
                  configPRINTF(( "SSID, %.*s, exceeds maximum length %d\n",
                          value.value.stringLength,
                          ( const char * )value.value.pString,
                          wificonfigMAX_SSID_LEN ));
                  result = pdFALSE;
              }
              else
              {
                  strncpy( pAddNetworkRequest->network.cPassword, ( const char * ) value.value.pString, value.value.stringLength );
                  pAddNetworkRequest->network.cPassword[ value.value.stringLength ] = '\0';
                  pAddNetworkRequest->network.ucPasswordLength = ( uint16_t )( value.value.stringLength + 1 );
              }
          }
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get network index parameter, error = %d, value type = %d.\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( ( value.value.signedInt >= IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX )
                    && ( value.value.signedInt < IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS  ) )
            {
                pAddNetworkRequest->savedIdx = value.value.signedInt;
            }
            else
            {
                configPRINTF(( "Network index parameter ( %d ) is out of range.\n", value.value.signedInt ));
                result = pdFALSE;
            }
        }
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_CONNECT_KEY, &value );
        if( ( ret == AWS_IOT_SERIALIZER_SUCCESS ) &&
                ( value.type == AWS_IOT_SERIALIZER_SCALAR_BOOL ) )
        {
            pAddNetworkRequest->connect = value.value.booleanValue;
        }
        else if( ret == AWS_IOT_SERIALIZER_NOT_FOUND )
        {
            configPRINTF(( "No connect flag specified, using default value connect: %d\n", IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT ));
            pAddNetworkRequest->connect = IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT;

        }
        else
        {
            configPRINTF(( "Error in getting connect flag, error = %d, value type = %d\n", ret, value.type ));
            result = pdFALSE;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

/*-----------------------------------------------------------*/

static BaseType_t _handleSaveNetworkRequest( uint8_t * pData,
                                               size_t length )
{
    BaseType_t status = pdFALSE;
    IotBleAddNetworkRequest_t *pParams = pvPortMalloc( sizeof( IotBleAddNetworkRequest_t ));

    if( pParams != NULL )
    {
        status = _deserializeAddNetworkRequest( pData, length, pParams );

        if( status == pdTRUE )
        {
            status = xTaskCreate(
                    _addNetworkTask,
                    "WifiProvAddNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pParams,
                    IOT_BLE_WIFI_PROV_MODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
        }
        else
        {
            vPortFree( pParams );
        }
    }
    else
    {
        configPRINTF(( "Failed to allocate memory for save network request\n " ));
    }

    return status;
}

static BaseType_t _deserializeEditNetworkRequest( uint8_t * pData, size_t length, IotBleEditNetworkRequest_t* pEditNetworkRequest )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    AwsIotSerializerError_t  ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdTRUE;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize decoder, error = %d, object type = %d\n", ret, decoderObj.type ));
        result = pdFALSE;
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get network index parameter, error = %d, value type = %d.\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( ( value.value.signedInt >= 0 )
                    && ( value.value.signedInt < IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS  ) )
            {
                pEditNetworkRequest->curIdx = value.value.signedInt;
            }
            else
            {
                configPRINTF(( "Network index parameter ( %d ) is out of range.\n", value.value.signedInt ));
                result = pdFALSE;
            }
        }
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_NEWINDEX_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get new network index parameter, error = %d, value type = %d.\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( ( value.value.signedInt >= 0 )
                    && ( value.value.signedInt < IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS  ) )
            {
                pEditNetworkRequest->newIdx = value.value.signedInt;
            }
            else
            {
                configPRINTF(( "New Network index parameter ( %d ) is out of range.\n", value.value.signedInt ));
                result = pdFALSE;
            }
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

/*-----------------------------------------------------------*/

static BaseType_t _handleEditNetworkRequest( uint8_t * pData, size_t length )
{
    BaseType_t status = pdFALSE;
    IotBleEditNetworkRequest_t *pParams = pvPortMalloc( sizeof( IotBleEditNetworkRequest_t ));

    if( pParams != NULL )
    {
        status = _deserializeEditNetworkRequest( pData, length, pParams );

        if( status == pdTRUE )
        {
            status = xTaskCreate(
                    _editNetworkTask,
                    "WifiProvEditNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pParams,
                    IOT_BLE_WIFI_PROV_MODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
        }
        else
        {
            vPortFree( pParams );
        }
    }
    else
    {
        configPRINTF(( "Failed to allocate memory for edit network request\n " ));
    }

    return status;
}

static BaseType_t _deserializeDeleteNetworkRequest( uint8_t * pData, size_t length, IotBleDeleteNetworkRequest_t* pDeleteNetworkRequest )
{

    AwsIotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    BaseType_t result = pdTRUE;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
            ( decoderObj.type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        configPRINTF(( "Failed to initialize decoder, error = %d, object type = %d\n", ret, decoderObj.type ));
        result = pdFALSE;
    }

    if( result == pdTRUE )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );
        if( ( ret != AWS_IOT_SERIALIZER_SUCCESS ) ||
                ( value.type != AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            configPRINTF(( "Failed to get network index parameter, error = %d, value type = %d.\n", ret, value.type ));
            result = pdFALSE;
        }
        else
        {
            if( ( value.value.signedInt >= 0 )
                    && ( value.value.signedInt < IOT_BLE_WIFI_PROV_MAX_SAVED_NETWORKS  ) )
            {
                pDeleteNetworkRequest->idx = ( int16_t ) value.value.signedInt;
            }
            else
            {
                configPRINTF(( "Network index parameter ( %d ) is out of range.\n", value.value.signedInt ));
                result = pdFALSE;
            }
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

/*-----------------------------------------------------------*/

static BaseType_t _handleDeleteNetworkRequest( uint8_t * pData,
                                                 size_t length )
{
    BaseType_t status = pdFALSE;
    IotBleDeleteNetworkRequest_t *pParams = pvPortMalloc( sizeof( IotBleDeleteNetworkRequest_t ));

    if( pParams != NULL )
    {
        status = _deserializeDeleteNetworkRequest( pData, length, pParams );

        if( status == pdTRUE )
        {
            status = xTaskCreate(
                    _deleteNetworkTask,
                    "WifiProvDeleteNetwork",
                    configMINIMAL_STACK_SIZE * 4,
                    pParams,
                    IOT_BLE_WIFI_PROV_MODIFY_NETWORK_TASK_PRIORITY,
                    NULL );
        }
        else
        {
            vPortFree( pParams );
        }
    }
    else
    {
        configPRINTF(( "Failed to allocate memory for delete network request\n " ));
    }

    return status;
}


AwsIotSerializerError_t _serializeNetwork( IotBleWifiNetworkInfo_t *pNetworkInfo, uint8_t *pBuffer, size_t *plength )
{
    AwsIotSerializerEncoderObject_t container = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t networkMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );
    if( ret == AWS_IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &networkMap, IOT_BLE_WIFI_PROV_NUM_NETWORK_INFO_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = pNetworkInfo->status;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_STATUS_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING;
        value.value.pString = ( uint8_t * ) pNetworkInfo->pSSID;
        value.value.stringLength = pNetworkInfo->SSIDLength;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_SSID_KEY, value );
    }
    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING;
        value.value.pString = ( uint8_t * ) pNetworkInfo->pBSSID;
        value.value.stringLength = pNetworkInfo->BSSIDLength;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_BSSID_KEY, value );
    }
    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = pNetworkInfo->security;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, value );
    }
    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_BOOL;
        value.value.booleanValue = pNetworkInfo->hidden;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_HIDDEN_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = pNetworkInfo->RSSI;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_RSSI_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_BOOL;
        value.value.booleanValue = pNetworkInfo->connected;        
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_CONNECTED_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = pNetworkInfo->savedIdx;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_INDEX_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        ret = IOT_BLE_MESG_ENCODER.closeContainer( &container, &networkMap );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *plength = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &container );
        }
        else
        {
            *plength = IOT_BLE_MESG_ENCODER.getEncodedSize( &container, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &container );
        ret = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return ret;
}

static AwsIotSerializerError_t _serializeStatusResponse( WIFIReturnCode_t status, uint8_t* pBuffer, size_t* plength )
{
    AwsIotSerializerEncoderObject_t container = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    AwsIotSerializerEncoderObject_t responseMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerScalarData_t value = { 0 };
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );
    if( ret == AWS_IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &responseMap, IOT_BLE_WIFI_PROV_NUM_STATUS_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.signedInt = status;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &responseMap, IOT_BLE_WIFI_PROV_STATUS_KEY, value );
    }
    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        ret = IOT_BLE_MESG_ENCODER.closeContainer( &container, &responseMap );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        if( pBuffer == NULL )
        {
            *plength = IOT_BLE_MESG_ENCODER.getExtraBufferSizeNeeded( &container );
        }
        else
        {
            *plength = IOT_BLE_MESG_ENCODER.getEncodedSize( &container, pBuffer );
        }

        IOT_BLE_MESG_ENCODER.destroy( &container );

        ret = AWS_IOT_SERIALIZER_SUCCESS;
    }

    return ret;
}


/*-----------------------------------------------------------*/

void _sendStatusResponse( IotBleWifiProvAttributes_t characteristic,
                            WIFIReturnCode_t status )
{

    uint8_t *pBuffer = NULL;
    size_t mesgLen;
    AwsIotSerializerError_t ret = AWS_IOT_SERIALIZER_SUCCESS;
    ret = _serializeStatusResponse( status, NULL, &mesgLen );
    if( ret == AWS_IOT_SERIALIZER_SUCCESS )
    {
        pBuffer = pvPortMalloc( mesgLen );
        if( pBuffer != NULL )
        {
            ret = _serializeStatusResponse( status, pBuffer, &mesgLen );
        }
        else
        {
            ret = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( ret == AWS_IOT_SERIALIZER_SUCCESS )
    {
        _sendResponse( characteristic, pBuffer, mesgLen );
    }
    else
    {
        configPRINTF(( "Failed to serialize status response, error = %d\n", ret ));
    }

    if( pBuffer != NULL )
    {
        vPortFree( pBuffer );
    }
}

void _sendResponse( IotBleWifiProvAttributes_t characteristic, uint8_t* pData, size_t len )
{
	IotBleAttributeData_t attrData = { 0 };
	IotBleEventResponse_t resp = { 0 };

	attrData.handle = ATTR_HANDLE( WifiProvService.pGattService, characteristic );
	attrData.uuid = ATTR_UUID( WifiProvService.pGattService, characteristic );
	resp.attrDataOffset = 0;
	resp.pAttrData = &attrData;
	resp.rspErrorStatus = eBTRspErrorNone;

	attrData.pData = pData;
	attrData.size = len;

	( void ) IotBle_SendIndication( &resp, WifiProvService.BLEConnId, false );
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t _appendNetwork( WIFINetworkProfile_t * pProfile )
{
    WIFIReturnCode_t ret;
    uint16_t idx;

    ret = WIFI_NetworkAdd( pProfile, &idx );

    if( ret == eWiFiSuccess )
    {
        WifiProvService.numNetworks++;
        WifiProvService.connectedIdx++;
    }

    return ret;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t _popNetwork( uint16_t index,
                                WIFINetworkProfile_t * pProfile )
{
    WIFIReturnCode_t ret = eWiFiSuccess;

    if( pProfile != NULL )
    {
        ret = WIFI_NetworkGet( pProfile, STORAGE_INDEX( index ) );
    }

    if( ret == eWiFiSuccess )
    {
        ret = WIFI_NetworkDelete( STORAGE_INDEX( index ) );
    }

    if( ret == eWiFiSuccess )
    {
        WifiProvService.numNetworks--;

        /* Shift the priority for connected network */
        if( index < WifiProvService.connectedIdx )
        {
            WifiProvService.connectedIdx--;
        }
        else if( index == WifiProvService.connectedIdx )
        {
            WifiProvService.connectedIdx = IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t _moveNetwork( uint16_t currentIndex,
                                 uint16_t newIndex )
{
    WIFIReturnCode_t ret = eWiFiSuccess;
    WIFINetworkProfile_t profile;

    if( currentIndex != newIndex )
    {
        ret = _popNetwork( currentIndex, &profile );

        if( ret == eWiFiSuccess )
        {
            ret = _insertNetwork( newIndex, &profile );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t _getSavedNetwork( uint16_t index,
                                     WIFINetworkProfile_t * pProfile )
{
	return WIFI_NetworkGet( pProfile, STORAGE_INDEX( index ) );
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t _connectNetwork( WIFINetworkProfile_t * pProfile )
{
    WIFIReturnCode_t ret = eWiFiFailure;
    WIFINetworkParams_t networkParams = { 0 };

    networkParams.pcSSID = pProfile->cSSID;
    networkParams.ucSSIDLength = pProfile->ucSSIDLength;
    networkParams.pcPassword = pProfile->cPassword;
    networkParams.ucPasswordLength = pProfile->ucPasswordLength;
    networkParams.xSecurity = pProfile->xSecurity;
    ret = WIFI_ConnectAP( &networkParams );
    return ret;
}

WIFIReturnCode_t _addNewNetwork( WIFINetworkProfile_t * pProfile, bool connect )
{
    WIFIReturnCode_t ret = eWiFiSuccess;

    if( connect == true )
    {
        ret = _connectNetwork( pProfile );
    }
    if( ret == eWiFiSuccess )
    {
    	ret = _appendNetwork( pProfile );
    	if( ( ret == eWiFiSuccess )
    	        && ( connect == true ) )
    	{
    		WifiProvService.connectedIdx = 0;
    	}
    }
    return ret;
}


WIFIReturnCode_t _connectSavedNetwork( uint16_t index )
{
    WIFIReturnCode_t ret;
    WIFINetworkProfile_t profile;

    ret = _getSavedNetwork( index, &profile );
    if( ret == eWiFiSuccess )
    {
     ret = _connectNetwork( &profile );
    	if( ret == eWiFiSuccess )
    	{
    		WifiProvService.connectedIdx = index;
    	}
    }
    return ret;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t _insertNetwork( uint16_t index,
                                   WIFINetworkProfile_t * pProfile )
{
    WIFIReturnCode_t ret;
    WIFINetworkProfile_t profile;
    uint16_t numElementsToShift, x;

    /* All higher priority elements needs to be shifted */
    numElementsToShift = index;

    ret = _appendNetwork( pProfile );

    if( ret == eWiFiSuccess )
    {
        for( x = 0; x < numElementsToShift; x++ )
        {
            ret = _popNetwork( index, &profile );
            configASSERT( ret == eWiFiSuccess );
            ret = _appendNetwork( &profile );
            configASSERT( ret == eWiFiSuccess );
        }
    }

    return ret;
}

static void _sendSavedNetwork( WIFINetworkProfile_t *pSavedNetwork, uint16_t idx )
{
    IotBleWifiNetworkInfo_t networkInfo = NETWORK_INFO_DEFAULT_PARAMS;
    uint8_t * message = NULL;
    size_t messageLen = 0;
    AwsIotSerializerError_t serializerRet;

    networkInfo.pSSID = pSavedNetwork->cSSID;
    networkInfo.SSIDLength = pSavedNetwork->ucSSIDLength - 1;
    networkInfo.pBSSID = pSavedNetwork->ucBSSID;
    networkInfo.BSSIDLength = wificonfigMAX_BSSID_LEN;
    networkInfo.connected = ( WifiProvService.connectedIdx == idx );
    networkInfo.security = pSavedNetwork->xSecurity;
    networkInfo.savedIdx = ( int32_t ) idx;

    serializerRet = _serializeNetwork( &networkInfo, NULL, &messageLen );
    if( serializerRet == AWS_IOT_SERIALIZER_SUCCESS )
    {
         message = pvPortMalloc( messageLen );
        if(  message != NULL )
        {
            serializerRet = _serializeNetwork( &networkInfo,  message, &messageLen );
        }
        else
        {
            serializerRet = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( serializerRet == AWS_IOT_SERIALIZER_SUCCESS )
    {
        _sendResponse( IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR,  message, messageLen );
    }
    else
    {
        configPRINTF(( "Failed to send network profile, SSID:%*s\n",
                pSavedNetwork->ucSSIDLength, pSavedNetwork->cSSID ));
    }

    if(  message != NULL )
    {
        vPortFree(  message );
    }
}

static void _sendScanNetwork( WIFIScanResult_t *pScanNetwork )
{
    IotBleWifiNetworkInfo_t networkInfo = NETWORK_INFO_DEFAULT_PARAMS;
    uint8_t * message = NULL;
    size_t messageLen = 0;
    AwsIotSerializerError_t serializerRet;

    networkInfo.pSSID = pScanNetwork->cSSID;
    networkInfo.SSIDLength = strlen( pScanNetwork->cSSID );
    networkInfo.pBSSID = pScanNetwork->ucBSSID;
    networkInfo.BSSIDLength = wificonfigMAX_BSSID_LEN;
    networkInfo.RSSI = pScanNetwork->cRSSI;
    networkInfo.hidden = ( bool ) pScanNetwork->ucHidden;
    networkInfo.security = pScanNetwork->xSecurity;

    serializerRet = _serializeNetwork( &networkInfo, NULL, &messageLen );
    if( serializerRet == AWS_IOT_SERIALIZER_SUCCESS )
    {
         message = pvPortMalloc( messageLen );
        if(  message != NULL )
        {
            serializerRet = _serializeNetwork( &networkInfo,  message, &messageLen );
        }
        else
        {
            serializerRet = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( serializerRet == AWS_IOT_SERIALIZER_SUCCESS )
    {
        _sendResponse( IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR,  message, messageLen );
    }
    else
    {
        configPRINTF(( "Failed to send network profile, SSID:%s\n",
                pScanNetwork->cSSID ));
    }

    if(  message != NULL )
    {
        vPortFree(  message );
    }
}
/*-----------------------------------------------------------*/

void _listNetworkTask( void * pParams )
{
    IotBleListNetworkRequest_t * pListNetworReq = ( IotBleListNetworkRequest_t * ) pParams;
    WIFIScanResult_t xScanResults[ pListNetworReq->maxNetworks ];
    WIFINetworkProfile_t profile;
    uint16_t idx;
    WIFIReturnCode_t WifiRet;

    if( xSemaphoreTake( WifiProvService.lock, portMAX_DELAY ) == pdPASS )
    {
    	for( idx = 0; idx < WifiProvService.numNetworks; idx++ )
    	{
    	    WifiRet = _getSavedNetwork( idx, &profile );
    		if( WifiRet == eWiFiSuccess )
    		{
    		    _sendSavedNetwork( &profile, idx );
    		}
    	}

    	memset( xScanResults, 0x00, sizeof( WIFIScanResult_t ) * pListNetworReq->maxNetworks );
    	WifiRet = WIFI_Scan( xScanResults, pListNetworReq->maxNetworks );
    	if( WifiRet == eWiFiSuccess )
    	{
    		for( idx = 0; idx < pListNetworReq->maxNetworks; idx++ )
    		{
    			if( strlen( xScanResults[ idx ].cSSID ) > 0 )
    			{
    			    _sendScanNetwork( &xScanResults[ idx ] );
    			}
    		}
    	}
    	else
    	{
    		_sendStatusResponse( IOT_BLE_WIFI_PROV_LIST_NETWORK_CHAR, WifiRet );
    	}

    	xSemaphoreGive( WifiProvService.lock );
    }

    vPortFree( pListNetworReq );
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

void _addNetworkTask( void * pParams )
{
    WIFIReturnCode_t ret = eWiFiFailure;
    IotBleAddNetworkRequest_t * pAddNetworkReq = ( IotBleAddNetworkRequest_t * ) pParams;

    if( xSemaphoreTake( WifiProvService.lock, portMAX_DELAY ) == pdPASS )
    {
        if( pAddNetworkReq->savedIdx != IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX )
        {
         ret = _connectSavedNetwork( pAddNetworkReq->savedIdx );
        }
        else
        {
         ret = _addNewNetwork( &pAddNetworkReq->network, pAddNetworkReq->connect );
        }
        xSemaphoreGive( WifiProvService.lock );
    }

    _sendStatusResponse( IOT_BLE_WIFI_PROV_SAVE_NETWORK_CHAR, ret );
    vPortFree( pAddNetworkReq );
    vTaskDelete( NULL );
}



/*-----------------------------------------------------------*/

void _deleteNetworkTask( void * pParams )
{
    WIFIReturnCode_t ret = eWiFiFailure;
    IotBleDeleteNetworkRequest_t * pDeleteNetworkReq = ( IotBleDeleteNetworkRequest_t * ) pParams;


    if( xSemaphoreTake( WifiProvService.lock, portMAX_DELAY ) == pdPASS )
    {
        ret = _popNetwork( pDeleteNetworkReq->idx, NULL );

        if( ret == eWiFiSuccess )
        {
            if( WifiProvService.connectedIdx == IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX )
            {
                ( void ) WIFI_Disconnect();
            }
        }
        xSemaphoreGive( WifiProvService.lock );
    }

    _sendStatusResponse( IOT_BLE_WIFI_PROV_DELETE_NETWORK_CHAR, ret );
    vPortFree( pDeleteNetworkReq );
    vTaskDelete( NULL );
}



/*-----------------------------------------------------------*/

void _editNetworkTask( void * pParams )
{
    WIFIReturnCode_t ret = eWiFiFailure;
    IotBleEditNetworkRequest_t * pEditNetworkReq = ( IotBleEditNetworkRequest_t * ) pParams;

    if( xSemaphoreTake( WifiProvService.lock, portMAX_DELAY ) == pdPASS )
    {
        ret = _moveNetwork( pEditNetworkReq->curIdx, pEditNetworkReq->newIdx );
        xSemaphoreGive( WifiProvService.lock );
    }

    _sendStatusResponse(  IOT_BLE_WIFI_PROV_EDIT_NETWORK_CHAR, ret );
    vPortFree( pEditNetworkReq );
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

BaseType_t IotBleWifiProv_Init( void )
{
	BTStatus_t status = eBTStatusSuccess;
	BTStatus_t error = pdFAIL;

    if( WifiProvService.init == pdFALSE )
    {

		WifiProvService.lock = xSemaphoreCreateMutex();

		if( WifiProvService.lock != NULL )
		{
			xSemaphoreGive( WifiProvService.lock );
		}
		else
		{
			status = eBTStatusFail;
		}

        if( status == eBTStatusSuccess )
        {
        	WifiProvService.pGattService = ( BTService_t *)&WIFIProvisionningService;
        	status = IotBle_CreateService( (BTService_t *)&WIFIProvisionningService, (IotBleAttributeEventCallback_t *)pCallBackArray );
        }
        if( status == eBTStatusSuccess )
        {
        	WifiProvService.connectedIdx = IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX;
        	WifiProvService.init = pdTRUE;
        }
    }
    else
    {
    	status = eBTStatusFail;
    }

    if(status == eBTStatusSuccess)
    {
    	error = pdPASS;
    }

    return error;
}

/*-----------------------------------------------------------*/
uint16_t IotBleWifiProv_GetNumNetworks( void )
{
	return _getNumSavedNetworks();
}

BaseType_t IotBleWifiProv_Connect( uint16_t networkIndex )
{
	WIFIReturnCode_t WifiRet;
	BaseType_t ret = pdFALSE;
	WifiRet = _connectSavedNetwork( networkIndex );
	if( WifiRet == eWiFiSuccess )
	{
	 ret = pdTRUE;
	}
	return ret;
}


BaseType_t IotBleWifiProv_EraseAllNetworks( void )
{

    BaseType_t ret = pdTRUE;
    WIFIReturnCode_t WiFiRet;

    if( xSemaphoreTake( WifiProvService.lock, portMAX_DELAY ) == pdPASS )
    {
        while( WifiProvService.numNetworks > 0 )
        {
            WiFiRet = _popNetwork( 0, NULL );
            if( WiFiRet != eWiFiSuccess )
            {
                configPRINTF(( "Failed to delete WIFI network, error = %d\n", WiFiRet ));
                ret = pdFALSE;
                break;
            }
        }
        xSemaphoreGive( WifiProvService.lock );
    }
    else
    {
        ret = pdFALSE;
    }

    return ret;
}

/*-----------------------------------------------------------*/

BaseType_t IotBleWifiProv_Delete( void )
{
	BaseType_t ret = pdFALSE;

    if( IotBle_DeleteService( WifiProvService.pGattService ) == eBTStatusSuccess )
    {
     ret = pdTRUE;
    }
    if( ret == pdTRUE )
    {

    	if( WifiProvService.lock != NULL )
    	{
    		vSemaphoreDelete( WifiProvService.lock );
    	}

    	memset( &WifiProvService, 0x00, sizeof( IotBleWifiProvService_t ) );
    }

    return ret;

}

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "iot_ble_wifi_prov_test_access_define.h"
#endif
