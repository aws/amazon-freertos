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

#include "iot_ble_wifi_provisioning.h"
#include "iot_serializer.h"

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
#define IOT_BLE_WIFI_PROV_MSG_TYPE_KEY        "w"
#define IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY    "h"
#define IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY    "t"
#define IOT_BLE_WIFI_PROV_KEY_MGMT_KEY        "q"
#define IOT_BLE_WIFI_PROV_SSID_KEY            "r"
#define IOT_BLE_WIFI_PROV_BSSID_KEY           "b"
#define IOT_BLE_WIFI_PROV_RSSI_KEY            "p"
#define IOT_BLE_WIFI_PROV_PSK_KEY             "m"
#define IOT_BLE_WIFI_PROV_STATUS_KEY          "s"
#define IOT_BLE_WIFI_PROV_HIDDEN_KEY          "f"
#define IOT_BLE_WIFI_PROV_CONNECTED_KEY       "e"
#define IOT_BLE_WIFI_PROV_INDEX_KEY           "g"
#define IOT_BLE_WIFI_PROV_NEWINDEX_KEY        "j"
#define IOT_BLE_WIFI_PROV_CONNECT_KEY         "y"

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
#define IOT_BLE_WIFI_PROV_NETWORK_INDEX_INVALID    ( -1 )

/**
 * @brief Number of parameters in a list network response.
 * This defines the  size of the CBOR map used to serialize the list network response messages.
 */
#define IOT_BLE_WIFI_PROV_LIST_RESPONSE_NUM_PARAMS        ( 9 )

/**
 * @brief Number of parameters in status response message.
 * This defines the size of the CBOR map used to serialize the list network response messages.
 */
#define IOT_BLE_WIFI_PROV_STATUS_MESG_NUM_PARAMS          ( 2 )

/**
 * @brief Macro defines the default behavior whether to connect to an access point along with 
 * provisioning or not. Default value can be overriden by the provisioning app by specifying connect flag
 * in add access point request message.
 */
#define IOT_BLE_WIFI_PROV_DEFAULT_SHOULD_CONNECT          ( true )


#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf ) \
    ( ( ret == IOT_SERIALIZER_SUCCESS ) ||              \
      ( ( !pxSerializerBuf ) && ( ret == IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )


bool IotBleWiFiProv_GetRequestTypeFromMessage( const uint8_t * pMessage,
                                              size_t length,
                                              IotBleWiFiProvRequest_t * pRequeustType )
{
    IotSerializerDecoderObject_t decoderObj = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t value = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pMessage, length );

    if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        IotLogError( "Failed to initialize the decoder, error = %d, object type = %d", ret, decoderObj.type );
        result = false;
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_MSG_TYPE_KEY, &value );

        if( ( ret == IOT_SERIALIZER_SUCCESS ) &&
            ( value.type == IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            switch( value.u.value.u.signedInt )
            {
                case IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_REQ:
                    ( * pRequeustType ) =  IotBleWiFiProvRequestListNetwork;
                    break;
                case IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_REQ:
                    ( * pRequeustType ) =  IotBleWiFiProvRequestAddNetwork;
                    break;
                case IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_REQ:
                    ( * pRequeustType ) =  IotBleWiFiProvRequestEditNetwork;
                    break;
                case IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_REQ:
                    ( * pRequeustType ) =  IotBleWiFiProvRequestDeleteNetwork;
                    break;
                default:
                    result = false;
                    break;
            }
        }
        else
        {
            IotLogError( "Failed to find message type, error = %d, value type = %d", ret, value.type );
            result = false;
        }
    }

    return result;
}

static bool IotBleWiFiProv_DeserializeListNetworksRequest( const uint8_t * pData,
                                                          size_t length,
                                                          IotBleWifiProvListNetworksRequest_t * pListNetworksRequest )
{
    IotSerializerDecoderObject_t decoderObj = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t value = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        IotLogError( "Failed to initialize the decoder, error = %d, object type = %d", ret, decoderObj.type );
        result = false;
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_MAX_NETWORKS_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get max Networks parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( ( value.u.value.u.signedInt <= 0 ) || ( value.u.value.u.signedInt > IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS ) )
            {
                IotLogWarn( "Networks %d exceeds configured value, truncating to  %d max network\n",
                            ( uint16_t ) value.u.value.u.signedInt,
                            IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS );
                pListNetworksRequest->scanSize = IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS;
            }
            else
            {
                pListNetworksRequest->scanSize = value.u.value.u.signedInt;
            }
        }
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_SCAN_TIMEOUT_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get timeout parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            pListNetworksRequest->scanTimeoutMS = value.u.value.u.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

/*-----------------------------------------------------------*/

bool IotBleWiFiProv_DeserializeAddNetworksRequest( const uint8_t * pData,
                                                         size_t length,
                                                         IotBleWifiProvAddNetworkRequest_t * pAddNetworkReq )
{
    IotSerializerDecoderObject_t decoderObj = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t value = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;


    memset( pAddNetworkReq, 0x00, sizeof(IotBleWifiProvAddNetworkRequest_t) );

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        IotLogError( "Failed to initialize the decoder, error = %d, object type = %d", ret, decoderObj.type );
        result = false;
    }

    if( result == true )
    {
        value.u.value.u.string.pString = NULL;
        value.u.value.u.string.length = 0;
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_SSID_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            IotLogError( "Failed to get SSID parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( value.u.value.u.string.length >= wificonfigMAX_SSID_LEN )
            {
                IotLogError( "SSID, %.*s, exceeds maximum length %d",
                             value.u.value.u.string.length,
                             ( const char * ) value.u.value.u.string.pString,
                             wificonfigMAX_SSID_LEN );
                result = false;
            }
            else
            {
                memcpy( pAddNetworkReq->accessPointInfo.ucSSID, value.u.value.u.string.pString, value.u.value.u.string.length );
                pAddNetworkReq->accessPointInfo.ucSSIDLength = ( value.u.value.u.string.length );
            }
        }
    }

    if( result == true )
    {
        value.type = IOT_SERIALIZER_SCALAR_BYTE_STRING;
        value.u.value.u.string.pString = NULL;
        value.u.value.u.string.length = 0;
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_BSSID_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_BYTE_STRING ) )
        {
            IotLogError( "Failed to get BSSID parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( value.u.value.u.string.length != wificonfigMAX_BSSID_LEN )
            {
                IotLogError( "Parameter BSSID length (%d) does not match BSSID length %d",
                             value.u.value.u.string.length,
                             wificonfigMAX_BSSID_LEN );
                result = false;
            }
            else
            {
                memcpy( pAddNetworkReq->accessPointInfo.ucBSSID, value.u.value.u.string.pString, wificonfigMAX_BSSID_LEN );
            }
        }
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get WIFI xSecurity parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            switch( value.u.value.u.signedInt )
            {
                case IOT_BLE_WIFI_SECURITY_TYPE_OPEN:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityOpen;
                    break;

                case IOT_BLE_WIFI_SECURITY_TYPE_WEP:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityWEP;
                    break;

                case IOT_BLE_WIFI_SECURITY_TYPE_WPA:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityWPA;
                    break;

                case IOT_BLE_WIFI_SECURITY_TYPE_WPA2:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityWPA2;
                    break;

                case IOT_BLE_WIFI_SECURITY_TYPE_WPA2_ENT:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityWPA2;
                    break;

                case IOT_BLE_WIFI_SECURITY_TYPE_WPA3:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityWPA2;
                    break;

                default:
                    pAddNetworkReq->accessPointInfo.xSecurity = eWiFiSecurityNotSupported;
                    break;
            }
        }
    }

    if( result == true )
    {
        value.u.value.u.string.pString = NULL;
        value.u.value.u.string.length = 0;
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_PSK_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_TEXT_STRING ) )
        {
            IotLogError( "Failed to get password parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( value.u.value.u.string.length >= wificonfigMAX_PASSPHRASE_LEN )
            {
                IotLogError( "SSID, %.*s, exceeds maximum length %d",
                             value.u.value.u.string.length,
                             ( const char * ) value.u.value.u.string.pString,
                             wificonfigMAX_SSID_LEN );
                result = false;
            }
            else
            {
                memcpy( pAddNetworkReq->accessPointInfo.cPassword, value.u.value.u.string.pString, value.u.value.u.string.length );
                pAddNetworkReq->accessPointInfo.ucPasswordLength = ( uint8_t ) ( value.u.value.u.string.length );
            }
        }
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get network index parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            
            if( ( value.u.value.u.signedInt >= 0 ) &&
                ( value.u.value.u.signedInt < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS ) )
            {
                pAddNetworkReq->isProvisioned = true;
                pAddNetworkReq->prirorityIndex = value.u.value.u.signedInt;
            }
            else
            {
                IotLogError( "Network index parameter ( %lld ) is out of range", value.u.value.u.signedInt );
                result = false;
            }
        }
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_CONNECT_KEY, &value );

        if( ( ret == IOT_SERIALIZER_SUCCESS ) &&
            ( value.type == IOT_SERIALIZER_SCALAR_BOOL ) )
        {
            pAddNetworkReq->shouldConnect = value.u.value.u.booleanValue;
        }
        else if( ret == IOT_SERIALIZER_NOT_FOUND )
        {
            IotLogInfo( "Connect flag not set in request, using default value, should connect = %d", IOT_BLE_WIFI_PROV_DEFAULT_SHOULD_CONNECT );
            pAddNetworkReq->shouldConnect = IOT_BLE_WIFI_PROV_DEFAULT_SHOULD_CONNECT;
        }
        else
        {
            IotLogError( "Error in getting connect flag, error = %d, value type = %d", ret, value.type );
            result = false;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

/*-----------------------------------------------------------*/

bool IotBleWiFiProv_DeserializeEditNetworkRequest( const uint8_t * pData,
                                                   size_t length,
                                                   IotBleWifiProvEditNetworkRequest_t * pEditNetworkReq )
{
    IotSerializerDecoderObject_t decoderObj = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t value = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        IotLogError( "Failed to initialize decoder, error = %d, object type = %d", ret, decoderObj.type );
        result = false;
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get network index parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( ( value.u.value.u.signedInt >= 0 ) &&
                ( value.u.value.u.signedInt < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS ) )
            {
                pEditNetworkReq->currentPriorityIndex = value.u.value.u.signedInt;
            }
            else
            {
                IotLogError( "Network index parameter ( %lld ) is out of range", value.u.value.u.signedInt );
                result = false;
            }
        }
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_NEWINDEX_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get new network index parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( ( value.u.value.u.signedInt >= 0 ) &&
                ( value.u.value.u.signedInt < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS ) )
            {
                pEditNetworkReq->newPriorityIndex = value.u.value.u.signedInt;
            }
            else
            {
                IotLogError( "New Network index parameter ( %lld ) is out of range", value.u.value.u.signedInt );
                result = false;
            }
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}



bool IotBleWiFiProv_DeserializeDeleteNetworkRequest( const uint8_t * pData,
                                                     size_t length,
                                                     IotBleWifiProvDeleteNetworkRequest_t * pDeleteNetworkRequest )
{
    IotSerializerDecoderObject_t decoderObj = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerDecoderObject_t value = IOT_SERIALIZER_DECODER_OBJECT_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pData, length );

    if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
        ( decoderObj.type != IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        IotLogError( "Failed to initialize decoder, error = %d, object type = %d", ret, decoderObj.type );
        result = false;
    }

    if( result == true )
    {
        ret = IOT_BLE_MESG_DECODER.find( &decoderObj, IOT_BLE_WIFI_PROV_INDEX_KEY, &value );

        if( ( ret != IOT_SERIALIZER_SUCCESS ) ||
            ( value.type != IOT_SERIALIZER_SCALAR_SIGNED_INT ) )
        {
            IotLogError( "Failed to get network index parameter, error = %d, value type = %d", ret, value.type );
            result = false;
        }
        else
        {
            if( ( value.u.value.u.signedInt >= 0 ) &&
                ( value.u.value.u.signedInt < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS ) )
            {
                pDeleteNetworkRequest->priorityIndex = ( int16_t ) value.u.value.u.signedInt;
            }
            else
            {
                IotLogError( "Network index parameter %lld is out of range", value.u.value.u.signedInt );
                result = false;
            }
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}

IotSerializerError_t IotBleWifiProv_SerializeListNetworkResponse( IotBleWiFiProvListNetworkResponse_t * pNetworkInfo,
                                                                  uint8_t * pBuffer,
                                                                  size_t * plength )
{
    IotSerializerEncoderObject_t container = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t networkMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t value = IOT_SERIALIZER_SCALAR_DATA_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &networkMap, IOT_BLE_WIFI_PROV_LIST_RESPONSE_NUM_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_MSG_TYPE_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = pNetworkInfo->status;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_STATUS_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_TEXT_STRING;
        value.value.u.string.pString = ( uint8_t * ) pNetworkInfo->pSSID;
        value.value.u.string.length = pNetworkInfo->SSIDLength;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_SSID_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_BYTE_STRING;
        value.value.u.string.pString = ( uint8_t * ) pNetworkInfo->pBSSID;
        value.value.u.string.length = pNetworkInfo->BSSIDLength;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_BSSID_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = pNetworkInfo->security;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_KEY_MGMT_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_BOOL;
        value.value.u.booleanValue = pNetworkInfo->hidden;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_HIDDEN_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = pNetworkInfo->RSSI;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_RSSI_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_BOOL;
        value.value.u.booleanValue = pNetworkInfo->connected;
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &networkMap, IOT_BLE_WIFI_PROV_CONNECTED_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = pNetworkInfo->priorityIndex;
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
        ret = IOT_SERIALIZER_SUCCESS;
    }

    return ( ret == IOT_SERIALIZER_SUCCESS );
}

bool IotBleWifiProv_SerializeStatusResponse( IotBleWiFiProvRequest_t request,
                                              WIFIReturnCode_t status,
                                              uint8_t * pBuffer,
                                              size_t * plength )
{
    IotSerializerEncoderObject_t container = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t responseMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t value = IOT_SERIALIZER_SCALAR_DATA_INITIALIZER;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &responseMap, IOT_BLE_WIFI_PROV_STATUS_MESG_NUM_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;

        switch( request )
        {
            case IotBleWiFiProvRequestListNetwork:
                value.value.u.signedInt = IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP;
                break;
            case IotBleWiFiProvRequestAddNetwork:
                value.value.u.signedInt = IOT_BLE_WIFI_PROV_MSG_TYPE_ADD_NETWORK_RESP;
                break;
            case IotBleWiFiProvRequestEditNetwork:
                value.value.u.signedInt = IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_RESP;
                break;
            case IotBleWiFiProvRequestDeleteNetwork:
                value.value.u.signedInt = IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_RESP;
                break;
            default:
                value.value.u.signedInt = 0;
                ret = IOT_SERIALIZER_UNDEFINED;
                break;
        }
        
        ret = IOT_BLE_MESG_ENCODER.appendKeyValue( &responseMap, IOT_BLE_WIFI_PROV_MSG_TYPE_KEY, value );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = status;
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

        ret = IOT_SERIALIZER_SUCCESS;
    }

    return ( ret == IOT_SERIALIZER_SUCCESS );
}
