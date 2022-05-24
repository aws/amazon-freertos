/*
 * FreeRTOS BLE V2.2.0
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
 * @file iot_ble_wifi_provisioning_serializer.c
 * Implement APIs used for serializing messages between device and the WiFi provisioning application
 * over BLE network.
 */
#include <string.h>

#include "iot_ble_wifi_provisioning.h"
#include "cbor.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_WIFI_PROV" )
#include "iot_logging_setup.h"

/**
 * @defgroup
 * @brief Serialized values for the message types exchanged between wifi
 */
/** @{ */
#define IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_REQ       ( 1 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP      ( 2 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_REQ        ( 3 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_RESP       ( 4 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_REQ       ( 5 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_RESP      ( 6 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_REQ     ( 7 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_RESP    ( 8 )
/** @} */

/**
 * @brief Strings representing the keys for the key-value pairs serialized
 * as part of request and response messages.
 */
#define IOT_BLE_WIFI_PROV_MSG_TYPE_KEY         "w"
#define IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY     "h"
#define IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY     "t"
#define IOT_BLE_WIFI_PROV_KEY_MGMT_KEY         "q"
#define IOT_BLE_WIFI_PROV_SSID_KEY             "r"
#define IOT_BLE_WIFI_PROV_BSSID_KEY            "b"
#define IOT_BLE_WIFI_PROV_RSSI_KEY             "p"
#define IOT_BLE_WIFI_PROV_PSK_KEY              "m"
#define IOT_BLE_WIFI_PROV_STATUS_KEY           "s"
#define IOT_BLE_WIFI_PROV_HIDDEN_KEY           "f"
#define IOT_BLE_WIFI_PROV_CONNECTED_KEY        "e"
#define IOT_BLE_WIFI_PROV_INDEX_KEY            "g"
#define IOT_BLE_WIFI_PROV_NEWINDEX_KEY         "j"
#define IOT_BLE_WIFI_PROV_CONNECT_KEY          "y"
#define IOT_BLE_WIFI_PROV_END_MARKER_KEY       "l"

/**
 * @brief Defines the serialized values for WiFi security types.
 */
#define IOT_BLE_WIFI_SECURITY_TYPE_OPEN        ( 0UL )
#define IOT_BLE_WIFI_SECURITY_TYPE_WEP         ( 1UL )
#define IOT_BLE_WIFI_SECURITY_TYPE_WPA         ( 2UL )
#define IOT_BLE_WIFI_SECURITY_TYPE_WPA2        ( 3UL )
#define IOT_BLE_WIFI_SECURITY_TYPE_WPA2_ENT    ( 4UL )
#define IOT_BLE_WIFI_SECURITY_TYPE_WPA3        ( 5UL )


/**
 * @cond DOXYGEN_IGNORE
 * @brief The network index which app use to signify as to not to use.
 */
#define IOT_BLE_WIFI_PROV_NETWORK_INDEX_INVALID               ( -1 )

/**
 * @brief Number of parameters in a list network response.
 * This defines the  size of the CBOR map used to serialize the list network response messages.
 */
#define IOT_BLE_WIFI_PROV_NETWORK_INFO_RESPONSE_NUM_PARAMS    ( 10 )

/**
 * @brief Number of parameters in status response message.
 * This defines the size of the CBOR map used to serialize the list network response messages.
 */
#define IOT_BLE_WIFI_PROV_STATUS_MESG_NUM_PARAMS              ( 2 )

/**
 * @brief Macro defines the default behavior whether to connect to an access point along with
 * provisioning or not. Default value can be overriden by the provisioning app by specifying connect flag
 * in add access point request message.
 */
#define IOT_BLE_WIFI_PROV_DEFAULT_SHOULD_CONNECT              ( true )


/**
 * @cond DOXYGEN_IGNORE
 * @brief The network rssi value which is used to indicate the app as not to use.
 */
#define IOT_BLE_WIFI_PROV_RSSI_DONT_USE             ( -100 )

/**
 * @cond DOXYGEN_IGNORE
 * @brief Priority index value which is used to indicate the app as not to use.
 */
#define IOT_BLE_WIFI_PROV_NETWORK_INDEX_DONT_USE    ( -1 )


#define SERIALIZER_STATUS( status, pBuffer ) \
    ( ( status == CborNoError ) ||           \
      ( ( !pBuffer ) && ( status == CborErrorOutOfMemory ) ) )

static IotBleWiFiProvRequest_t prvToRequestType( int64_t value )

{
    IotBleWiFiProvRequest_t ret = IotBleWiFiProvRequestInvalid;

    switch( value )
    {
        case IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_REQ:
            ret = IotBleWiFiProvRequestListNetwork;
            break;

        case IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_REQ:
            ret = IotBleWiFiProvRequestAddNetwork;
            break;

        case IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_REQ:
            ret = IotBleWiFiProvRequestEditNetwork;
            break;

        case IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_REQ:
            ret = IotBleWiFiProvRequestDeleteNetwork;
            break;

        default:
            break;
    }

    return ret;
}

static bool prvToWiFiSecurityType( int64_t value,
                                   WIFISecurity_t * pResult )
{
    bool status = true;

    switch( value )
    {
        case IOT_BLE_WIFI_SECURITY_TYPE_OPEN:
            ( *pResult ) = eWiFiSecurityOpen;
            break;

        case IOT_BLE_WIFI_SECURITY_TYPE_WEP:
            ( *pResult ) = eWiFiSecurityWEP;
            break;

        case IOT_BLE_WIFI_SECURITY_TYPE_WPA:
            ( *pResult ) = eWiFiSecurityWPA;
            break;

        case IOT_BLE_WIFI_SECURITY_TYPE_WPA2:
            ( *pResult ) = eWiFiSecurityWPA2;
            break;

        case IOT_BLE_WIFI_SECURITY_TYPE_WPA2_ENT:
            ( *pResult ) = eWiFiSecurityWPA2;
            break;

        case IOT_BLE_WIFI_SECURITY_TYPE_WPA3:
            ( *pResult ) = eWiFiSecurityWPA2;
            break;

        default:
            status = false;
            break;
    }

    return status;
}

static CborError prvGetOrDefaultBooleanValueFromMap( CborValue * pMap,
                                                     const char * key,
                                                     bool * pResult,
                                                     bool defaultValue )
{
    CborError status;
    CborValue value;

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_boolean( &value ) )
        {
            status = cbor_value_get_boolean( &value, pResult );
        }
        else if( cbor_value_get_type( &value ) == CborInvalidType )
        {
            /* Element not found. Return default value. */
            ( *pResult ) = defaultValue;
        }
        else
        {
            status = CborErrorImproperValue;
        }
    }

    return status;
}

static CborError prvGetIntegerValueFromMap( CborValue * pMap,
                                            const char * key,
                                            int64_t * pValue )
{
    CborError status;
    CborValue value;

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_integer( &value ) )
        {
            status = cbor_value_get_int64( &value, pValue );
        }
        else
        {
            status = CborErrorImproperValue;
        }
    }

    return status;
}

static CborError prvGetTextStringValueFromMap( CborValue * pMap,
                                               const char * key,
                                               char * pBuffer,
                                               size_t * bufLength )
{
    CborError status;
    size_t length;
    CborValue value;

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_text_string( &value ) )
        {
            if( cbor_value_is_length_known( &value ) )
            {
                status = cbor_value_get_string_length( &value, &length );
            }
            else
            {
                status = cbor_value_calculate_string_length( &value, &length );
            }
        }
        else
        {
            status = CborErrorImproperValue;
        }

        if( status == CborNoError )
        {
            if( length <= *bufLength )
            {
                status = cbor_value_copy_text_string( &value, pBuffer, bufLength, NULL );
            }
            else
            {
                status = CborErrorImproperValue;
            }
        }
    }

    return status;
}

static CborError prvGetByteStringValueFromMap( CborValue * pMap,
                                               const char * key,
                                               uint8_t * pBuffer,
                                               size_t * bufLength )
{
    CborError status;
    size_t length;
    CborValue value;

    status = cbor_value_map_find_value( pMap, key, &value );

    if( status == CborNoError )
    {
        if( cbor_value_is_byte_string( &value ) )
        {
            if( cbor_value_is_length_known( &value ) )
            {
                status = cbor_value_get_string_length( &value, &length );
            }
            else
            {
                status = cbor_value_calculate_string_length( &value, &length );
            }
        }
        else
        {
            status = CborErrorImproperValue;
        }

        if( status == CborNoError )
        {
            if( length <= ( *bufLength ) )
            {
                status = cbor_value_copy_byte_string( &value, pBuffer, bufLength, NULL );
            }
            else
            {
                status = CborErrorImproperValue;
            }
        }
    }

    return status;
}


static bool IotBleWiFiProv_GetRequestTypeCbor( const uint8_t * pMessage,
                                               size_t length,
                                               IotBleWiFiProvRequest_t * pRequestType )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t requestValue = 0L;
    bool result = true;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_MSG_TYPE_KEY, &requestValue );

        if( status == CborNoError )
        {
            ( *pRequestType ) = prvToRequestType( requestValue );
        }
    }

    IotLogInfo( "Decode cbor request message, status = %d", status );

    return result;
}

static bool IotBleWiFiProv_DeserializeListNetworkRequestCbor( const uint8_t * pMessage,
                                                              size_t length,
                                                              IotBleWifiProvListNetworksRequest_t * pListNetworksRequest )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;

    memset( pListNetworksRequest, 0x00, sizeof( IotBleWifiProvListNetworksRequest_t ) );
    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY, &val );

        if( status == CborNoError )
        {
            pListNetworksRequest->scanSize = ( int16_t ) val;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY, &val );

        if( status == CborNoError )
        {
            pListNetworksRequest->scanTimeoutMS = ( int16_t ) val;
        }
    }

    IotLogInfo( "Decode list network cbor message, status = %d", status );

    return( status == CborNoError );
}

/*-----------------------------------------------------------*/

static bool IotBleWiFiProv_DeserializeAddNetworkRequestCbor( const uint8_t * pMessage,
                                                             size_t length,
                                                             IotBleWifiProvAddNetworkRequest_t * pAddNetworkReq )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;
    bool boolVal;

    status = cbor_parser_init( pMessage, length, 0, &parser, &map );
    memset( pAddNetworkReq, 0x00, sizeof( IotBleWifiProvAddNetworkRequest_t ) );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_INDEX_KEY, &val );

        if( status == CborNoError )
        {
            if( val != IOT_BLE_WIFI_PROV_NETWORK_INDEX_DONT_USE )
            {
                pAddNetworkReq->info.index = ( int16_t ) val;
                pAddNetworkReq->isProvisioned = true;
            }
            else
            {
                pAddNetworkReq->isProvisioned = false;
            }
        }
    }

    if( pAddNetworkReq->isProvisioned == false )
    {
        if( status == CborNoError )
        {
            length = sizeof( pAddNetworkReq->info.network.ucSSID );
            status = prvGetTextStringValueFromMap( &map, IOT_BLE_WIFI_PROV_SSID_KEY, ( char * ) pAddNetworkReq->info.network.ucSSID, &length );

            if( status == CborNoError )
            {
                pAddNetworkReq->info.network.ucSSIDLength = length;
            }
            else
            {
                if( status == CborErrorOutOfMemory )
                {
                    IotLogError( "SSID exceeds allowable length %d",
                                 sizeof( pAddNetworkReq->info.network.ucSSID ) );
                }
            }
        }

        if( status == CborNoError )
        {
            length = sizeof( pAddNetworkReq->info.network.ucBSSID );
            status = prvGetByteStringValueFromMap( &map, IOT_BLE_WIFI_PROV_BSSID_KEY, pAddNetworkReq->info.network.ucBSSID, &length );

            if( length != sizeof( pAddNetworkReq->info.network.ucBSSID ) )
            {
                status = CborErrorImproperValue;
            }
        }

        if( status == CborNoError )
        {
            status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, &val );

            if( prvToWiFiSecurityType( val, &pAddNetworkReq->info.network.xSecurity ) == false )
            {
                status = CborErrorImproperValue;
            }
        }

        if( status == CborNoError )
        {
            length = sizeof( pAddNetworkReq->info.network.cPassword );
            status = prvGetTextStringValueFromMap( &map, IOT_BLE_WIFI_PROV_PSK_KEY, ( char * ) pAddNetworkReq->info.network.cPassword, &length );

            if( status == CborNoError )
            {
                pAddNetworkReq->info.network.ucPasswordLength = length;
            }
        }
    }

    if( status == CborNoError )
    {
        status = prvGetOrDefaultBooleanValueFromMap( &map, IOT_BLE_WIFI_PROV_CONNECT_KEY, &boolVal, IOT_BLE_WIFI_PROV_DEFAULT_SHOULD_CONNECT );

        if( status == CborNoError )
        {
            pAddNetworkReq->shouldConnect = boolVal;
        }
    }

    IotLogInfo( "Decode add network cbor message, status = %d", status );

    return( status == CborNoError );
}

/*-----------------------------------------------------------*/

static bool IotBleWiFiProv_DeserializeEditNetworkRequestCbor( const uint8_t * pMessage,
                                                              size_t length,
                                                              IotBleWifiProvEditNetworkRequest_t * pEditNetworkReq )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;

    memset( pEditNetworkReq, 0x00, sizeof( IotBleWifiProvEditNetworkRequest_t ) );
    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_INDEX_KEY, &val );

        if( status == CborNoError )
        {
            pEditNetworkReq->currentPriorityIndex = ( int16_t ) val;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_NEWINDEX_KEY, &val );

        if( status == CborNoError )
        {
            pEditNetworkReq->newPriorityIndex = ( int16_t ) val;
        }
    }

    IotLogInfo( "Decode edit network cbor message, status = %d", status );

    return( status == CborNoError );
}


static bool IotBleWiFiProv_DeserializeDeleteNetworkRequestCbor( const uint8_t * pMessage,
                                                                size_t length,
                                                                IotBleWifiProvDeleteNetworkRequest_t * pDeleteNetworkRequest )
{
    CborParser parser = { 0 };
    CborValue map = { 0 };
    CborError status = CborNoError;
    int64_t val = 0;

    memset( pDeleteNetworkRequest, 0x00, sizeof( IotBleWifiProvDeleteNetworkRequest_t ) );
    status = cbor_parser_init( pMessage, length, 0, &parser, &map );

    if( status == CborNoError )
    {
        if( !cbor_value_is_map( &map ) )
        {
            status = CborErrorImproperValue;
        }
    }

    if( status == CborNoError )
    {
        status = prvGetIntegerValueFromMap( &map, IOT_BLE_WIFI_PROV_INDEX_KEY, &val );

        if( status == CborNoError )
        {
            pDeleteNetworkRequest->priorityIndex = ( int16_t ) val;
        }
    }

    IotLogInfo( "Decode delete network cbor message, status = %d", status );

    return( status == CborNoError );
}

static uint8_t prvToResponseType( IotBleWiFiProvRequest_t requestType )
{
    uint8_t responseType = 0;

    switch( requestType )
    {
        case IotBleWiFiProvRequestListNetwork:
            responseType = IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP;
            break;

        case IotBleWiFiProvRequestAddNetwork:
            responseType = IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_RESP;
            break;

        case IotBleWiFiProvRequestEditNetwork:
            responseType = IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_RESP;
            break;

        case IotBleWiFiProvRequestDeleteNetwork:
            responseType = IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_RESP;
            break;

        default:
            break;
    }

    return responseType;
}

static bool prvSerializeResponseCbor( const IotBleWifiProvResponse_t * pResponse,
                                      uint8_t * pBuffer,
                                      size_t * pLength )
{
    size_t length = *pLength;
    size_t numParams =
        ( pResponse->statusOnly == true ) ? IOT_BLE_WIFI_PROV_STATUS_MESG_NUM_PARAMS
        : IOT_BLE_WIFI_PROV_NETWORK_INFO_RESPONSE_NUM_PARAMS;
    CborError status;
    CborEncoder encoder = { 0 }, mapEncoder = { 0 };

    cbor_encoder_init( &encoder, pBuffer, length, 0 );

    status = cbor_encoder_create_map( &encoder, &mapEncoder, numParams );

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_MSG_TYPE_KEY, strlen( IOT_BLE_WIFI_PROV_MSG_TYPE_KEY ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, prvToResponseType( pResponse->requestType ) );
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_STATUS_KEY, strlen( IOT_BLE_WIFI_PROV_STATUS_KEY ) );

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_int( &mapEncoder, pResponse->status );
        }
    }

    if( pResponse->statusOnly == false )
    {
        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_SSID_KEY, strlen( IOT_BLE_WIFI_PROV_SSID_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                if( pResponse->networkInfo.isSavedNetwork == true )
                {
                    status = cbor_encode_text_string( &mapEncoder,
                                                      ( char * ) pResponse->networkInfo.info.pSavedNetwork->ucSSID,
                                                      pResponse->networkInfo.info.pSavedNetwork->ucSSIDLength );
                }
                else
                {
                    status = cbor_encode_text_string( &mapEncoder,
                                                      ( char * ) pResponse->networkInfo.info.pScannedNetwork->ucSSID,
                                                      pResponse->networkInfo.info.pScannedNetwork->ucSSIDLength );
                }
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_BSSID_KEY, strlen( IOT_BLE_WIFI_PROV_BSSID_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                if( pResponse->networkInfo.isSavedNetwork == true )
                {
                    status = cbor_encode_byte_string( &mapEncoder,
                                                      pResponse->networkInfo.info.pSavedNetwork->ucBSSID,
                                                      sizeof( pResponse->networkInfo.info.pSavedNetwork->ucBSSID ) );
                }
                else
                {
                    status = cbor_encode_byte_string( &mapEncoder,
                                                      pResponse->networkInfo.info.pScannedNetwork->ucBSSID,
                                                      sizeof( pResponse->networkInfo.info.pScannedNetwork->ucBSSID ) );
                }
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, strlen( IOT_BLE_WIFI_PROV_KEY_MGMT_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                if( pResponse->networkInfo.isSavedNetwork == true )
                {
                    status = cbor_encode_int( &mapEncoder, pResponse->networkInfo.info.pSavedNetwork->xSecurity );
                }
                else
                {
                    status = cbor_encode_int( &mapEncoder, pResponse->networkInfo.info.pScannedNetwork->xSecurity );
                }
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_HIDDEN_KEY, strlen( IOT_BLE_WIFI_PROV_HIDDEN_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                status = cbor_encode_boolean( &mapEncoder, pResponse->networkInfo.isHidden );
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_RSSI_KEY, strlen( IOT_BLE_WIFI_PROV_RSSI_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                if( pResponse->networkInfo.isSavedNetwork == true )
                {
                    status = cbor_encode_int( &mapEncoder, IOT_BLE_WIFI_PROV_RSSI_DONT_USE );
                }
                else
                {
                    status = cbor_encode_int( &mapEncoder, pResponse->networkInfo.info.pScannedNetwork->cRSSI );
                }
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_CONNECTED_KEY, strlen( IOT_BLE_WIFI_PROV_CONNECTED_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                status = cbor_encode_boolean( &mapEncoder, pResponse->networkInfo.isConnected );
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_INDEX_KEY, strlen( IOT_BLE_WIFI_PROV_INDEX_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                if( pResponse->networkInfo.isSavedNetwork == true )
                {
                    status = cbor_encode_int( &mapEncoder, pResponse->networkInfo.index );
                }
                else
                {
                    status = cbor_encode_int( &mapEncoder, IOT_BLE_WIFI_PROV_NETWORK_INDEX_DONT_USE );
                }
            }
        }

        if( SERIALIZER_STATUS( status, pBuffer ) == true )
        {
            status = cbor_encode_text_string( &mapEncoder, IOT_BLE_WIFI_PROV_END_MARKER_KEY, strlen( IOT_BLE_WIFI_PROV_END_MARKER_KEY ) );

            if( SERIALIZER_STATUS( status, pBuffer ) == true )
            {
                status = cbor_encode_boolean( &mapEncoder, pResponse->networkInfo.isLast );
            }
        }
    }

    if( SERIALIZER_STATUS( status, pBuffer ) == true )
    {
        status = cbor_encoder_close_container( &encoder, &mapEncoder );
    }

    if( pBuffer == NULL )
    {
        *pLength = cbor_encoder_get_extra_bytes_needed( &encoder );
    }
    else
    {
        *pLength = cbor_encoder_get_buffer_size( &encoder, pBuffer );
    }

    return SERIALIZER_STATUS( status, pBuffer );
}

static bool IotBleWifiProv_GetSerializeResponseSizeCbor( const IotBleWifiProvResponse_t * pResponse,
                                                         size_t * pLength )
{
    return prvSerializeResponseCbor( pResponse, NULL, pLength );
}

static bool IotBleWifiProv_SerializeResponseCbor( const IotBleWifiProvResponse_t * pResponse,
                                                  uint8_t * pBuffer,
                                                  size_t length )
{
    return prvSerializeResponseCbor( pResponse, pBuffer, &length );
}

IotBleWifiProvSerializer_t * IotBleWifiProv_GetSerializer( void )
{
    static IotBleWifiProvSerializer_t bleWifiProvSerializer =
    {
        .getRequestType                  = IotBleWiFiProv_GetRequestTypeCbor,
        .deserializeListNetworkRequest   = IotBleWiFiProv_DeserializeListNetworkRequestCbor,
        .deserializeAddNetworkRequest    = IotBleWiFiProv_DeserializeAddNetworkRequestCbor,
        .deserializeEditNetworkRequest   = IotBleWiFiProv_DeserializeEditNetworkRequestCbor,
        .deserializeDeleteNetworkRequest = IotBleWiFiProv_DeserializeDeleteNetworkRequestCbor,
        .getSerializedSizeForResponse    = IotBleWifiProv_GetSerializeResponseSizeCbor,
        .serializeResponse               = IotBleWifiProv_SerializeResponseCbor
    };

    return &bleWifiProvSerializer;
}
