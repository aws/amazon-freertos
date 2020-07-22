/*
 * FreeRTOS BLE V2.1.0
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
 * @file aws_ble_wifi_provisioning.c
 * @brief BLE Gatt service for WiFi provisioning
 */

#include <string.h>

#include "iot_config.h"
#include "iot_ble_config.h"
#include "iot_serializer.h"
#include "iot_taskpool.h"
#include "iot_ble_wifi_provisioning.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME         ( "BLE_WIFI_PROV" )
#include "iot_logging_setup.h"

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 */
#define IOT_BLE_WIFI_PROV_INVALID_NETWORK_RSSI     ( -100 )
#define IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX    ( -1 )

#define ATTR_HANDLE( svc, ch_idx )    ( ( svc )->pusHandlesBuffer[ ch_idx ] )
#define ATTR_UUID( svc, ch_idx )      ( ( svc )->pxBLEAttributes[ ch_idx ].xCharacteristic.xUuid )

#define IS_VALID_SERIALIZER_RET( ret, pxSerializerBuf ) \
    ( ( ret == IOT_SERIALIZER_SUCCESS ) ||              \
      ( ( !pxSerializerBuf ) && ( ret == IOT_SERIALIZER_BUFFER_TOO_SMALL ) ) )

#define STORAGE_INDEX( priority )    ( wifiProvisioning.numNetworks - priority - 1 )
#define NETWORK_INFO_DEFAULT_PARAMS    { .status = eWiFiSuccess, .RSSI = IOT_BLE_WIFI_PROV_INVALID_NETWORK_RSSI, .connected = false, .savedIdx = IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX }
/** @endcond */

/**
 * @brief Parameters used in WIFI provisioning message serialization.
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
 * @defgroup
 * Wifi provisioning message types.
 */
/** @{ */
#define IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_REQ       ( 1 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP      ( 2 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_SAVE_NETWORK_REQ       ( 3 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_SAVE_NETWORK_RESP      ( 4 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_REQ       ( 5 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_RESP      ( 6 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_REQ     ( 7 )
#define IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_RESP    ( 8 )
/** @} */

#define IOT_BLE_WIFI_PROV_NUM_NETWORK_INFO_MESG_PARAMS    ( 9 )
#define IOT_BLE_WIFI_PROV_NUM_STATUS_MESG_PARAMS          ( 2 )
#define IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT          ( true )


/*---------------------------------------------------------------------------------------------------------*/

static IotBleWifiProvService_t wifiProvisioning = { 0 };

/*---------------------------------------------------------------------------------------------------------*/


static WIFIScanResult_t scanNetworks[ IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS ] = { 0 };

/*
 * @brief Callback registered for BLE write and read events received for each characteristic.
 */
static void _callback( IotBleDataTransferChannelEvent_t event,
                       IotBleDataTransferChannel_t * pChannel,
                       void * pContext );

static bool _deserializeListNetworkRequest( const uint8_t * pData,
                                            size_t length,
                                            IotBleListNetworkRequest_t * pListNetworkRequest );

/*
 * @brief Parses List Network request params and creates task to list networks.
 */
static bool _handleListNetworkRequest( const uint8_t * pData,
                                       size_t length );


static bool _deserializeAddNetworkRequest( const uint8_t * pData,
                                           size_t length,
                                           IotBleAddNetworkRequest_t * pAddNetworkRequest );

/*
 * @brief Parses Save Network request params and creates task to save the new network.
 */

static bool _handleSaveNetworkRequest( const uint8_t * pData,
                                       size_t length );


static bool _deserializeEditNetworkRequest( const uint8_t * pData,
                                            size_t length,
                                            IotBleEditNetworkRequest_t * pEditNetworkRequest );

/*
 * @brief Parses Edit Network request params and creates task to edit network priority.
 */
static bool _handleEditNetworkRequest( const uint8_t * pData,
                                       size_t length );

static bool _deserializeDeleteNetworkRequest( const uint8_t * pData,
                                              size_t length,
                                              IotBleDeleteNetworkRequest_t * pDeleteNetworkRequest );

/*
 * @brief Parses Delete Network request params and creates task to delete a WiFi networ.
 */
static bool _handleDeleteNetworkRequest( const uint8_t * pData,
                                         size_t length );


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

WIFIReturnCode_t _addNewNetwork( WIFINetworkProfile_t * pProfile,
                                 bool connect );

static IotSerializerError_t _serializeNetwork( int32_t responseType,
                                               IotBleWifiNetworkInfo_t * pNetworkInfo,
                                               uint8_t * pBuffer,
                                               size_t * plength );


static IotSerializerError_t _serializeStatusResponse( int32_t responseType,
                                                      WIFIReturnCode_t status,
                                                      uint8_t * pBuffer,
                                                      size_t * plength );

static void _sendScanNetwork( int32_t responseType,
                              WIFIScanResult_t * pScanNetwork );

/*
 * @brief  The task lists the saved network configurations in flash and also scans nearby networks.
 * It sends the profile information for each saved and scanned networks one at a time to the GATT client.
 * Maximum number of networks to scan is set in the List network request.
 */
static void _listNetworkTask( IotTaskPool_t taskPool,
                              IotTaskPoolJob_t job,
                              void * pUserContext );

/*
 * @brief  The task is used to save a new WiFi configuration.
 * It first connects to the network and if successful,saves the network onto flash with the highest priority.
 */
static void _addNetworkTask( IotTaskPool_t taskPool,
                             IotTaskPoolJob_t job,
                             void * pUserContext );

/*
 * @brief  The task is used to reorder priorities of network profiles stored in flash.
 *  If the priority of existing connected network changes then it initiates a reconnection.
 */
static void _editNetworkTask( IotTaskPool_t taskPool,
                              IotTaskPoolJob_t job,
                              void * pUserContext );

/*
 * @brief  The task is used to delete a network configuration from the flash.
 * If the network is connected, it disconnects from the network and initiates a reconnection.
 */
static void _deleteNetworkTask( IotTaskPool_t taskPool,
                                IotTaskPoolJob_t job,
                                void * pUserContext );

/*
 * @brief Gets the number of saved networks from flash.
 */
static uint32_t _getNumSavedNetworks( void );

/*
 * @brief Sends a status response for the request.
 */
static void _sendStatusResponse( int32_t responseType,
                                 WIFIReturnCode_t status );

/*-----------------------------------------------------------*/

static bool _getRequestType( const uint8_t * pRequest,
                             size_t requestLength,
                             int32_t * pType )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    bool result = true;

    ret = IOT_BLE_MESG_DECODER.init( &decoderObj, ( uint8_t * ) pRequest, requestLength );

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
            *pType = ( int32_t ) ( value.u.value.u.signedInt );
        }
        else
        {
            IotLogError( "Failed to find message type, error = %d, value type = %d", ret, value.type );
            result = false;
        }
    }

    return result;
}

static void _callback( IotBleDataTransferChannelEvent_t event,
                       IotBleDataTransferChannel_t * pChannel,
                       void * pContext )
{
    int32_t requestType;
    const uint8_t * pBuffer;
    size_t length;

    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        IotBleDataTransfer_PeekReceiveBuffer( pChannel, &pBuffer, &length );

        if( _getRequestType( pBuffer, length, &requestType ) == true )
        {
            IotLogDebug( "Received message type: %d, length %d", requestType, length );

            switch( requestType )
            {
                case IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_REQ:
                    _handleListNetworkRequest( pBuffer, length );
                    break;

                case IOT_BLE_WIFI_PROV_MSG_TYPE_SAVE_NETWORK_REQ:
                    _handleSaveNetworkRequest( pBuffer, length );
                    break;

                case IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_REQ:
                    _handleEditNetworkRequest( pBuffer, length );
                    break;

                case IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_REQ:
                    _handleDeleteNetworkRequest( pBuffer, length );
                    break;

                default:
                    IotLogWarn( "Invalid request type ( %d ) received, discarding the message", requestType );
                    break;
            }
        }

        /* Do an empty read to flush the buffer. */
        IotBleDataTransfer_Receive( pChannel, NULL, length );
    }
}

static bool _deserializeListNetworkRequest( const uint8_t * pData,
                                            size_t length,
                                            IotBleListNetworkRequest_t * pListNetworkRequest )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
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
                pListNetworkRequest->maxNetworks = IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS;
            }
            else
            {
                pListNetworkRequest->maxNetworks = value.u.value.u.signedInt;
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
            pListNetworkRequest->timeoutMs = value.u.value.u.signedInt;
        }
    }

    IOT_BLE_MESG_DECODER.destroy( &decoderObj );

    return result;
}


/*-----------------------------------------------------------*/

static bool _handleListNetworkRequest( const uint8_t * pData,
                                       size_t length )
{
    bool status = false;
    IotTaskPoolError_t taskStatus = IOT_TASKPOOL_SUCCESS;
    IotTaskPoolJob_t job = IOT_TASKPOOL_JOB_INITIALIZER;

    if( IotSemaphore_TryWait( &wifiProvisioning.lock ) == true )
    {
        memset( &wifiProvisioning.listNetworkRequest, 0x00, sizeof( IotBleListNetworkRequest_t ) );

        status = _deserializeListNetworkRequest( pData, length, &wifiProvisioning.listNetworkRequest );

        if( status == true )
        {
            IotLogDebug( "List network request parameters: max networks = %d, timeout = %d",
                         wifiProvisioning.listNetworkRequest.maxNetworks,
                         wifiProvisioning.listNetworkRequest.timeoutMs );

            taskStatus = IotTaskPool_CreateRecyclableJob( IOT_SYSTEM_TASKPOOL,
                                                          _listNetworkTask,
                                                          NULL,
                                                          &job );

            if( taskStatus == IOT_TASKPOOL_SUCCESS )
            {
                taskStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, job, 0 );

                if( taskStatus != IOT_TASKPOOL_SUCCESS )
                {
                    IotLogError( "Failed to schedule taskpool job for list network request" );
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, job );
                    status = false;
                }
            }
            else
            {
                IotLogError( "Failed to allocate taskpool job for list network request" );
                status = false;
            }
        }

        if( status == false )
        {
            IotSemaphore_Post( &wifiProvisioning.lock );
        }
    }
    else
    {
        IotLogError( "Failed to process the list network request, another list network request in progress." );
        status = false;
    }

    return status;
}

static bool _deserializeAddNetworkRequest( const uint8_t * pData,
                                           size_t length,
                                           IotBleAddNetworkRequest_t * pAddNetworkRequest )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
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
                strncpy( pAddNetworkRequest->network.cSSID, ( const char * ) value.u.value.u.string.pString, value.u.value.u.string.length );
                pAddNetworkRequest->network.cSSID[ value.u.value.u.string.length ] = '\0';
                pAddNetworkRequest->network.ucSSIDLength = ( value.u.value.u.string.length + 1 );
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
                memcpy( pAddNetworkRequest->network.ucBSSID, value.u.value.u.string.pString, wificonfigMAX_BSSID_LEN );
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
            pAddNetworkRequest->network.xSecurity = value.u.value.u.signedInt;
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
                strncpy( pAddNetworkRequest->network.cPassword, ( const char * ) value.u.value.u.string.pString, value.u.value.u.string.length );
                pAddNetworkRequest->network.cPassword[ value.u.value.u.string.length ] = '\0';
                pAddNetworkRequest->network.ucPasswordLength = ( uint16_t ) ( value.u.value.u.string.length + 1 );
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
            if( ( value.u.value.u.signedInt >= IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX ) &&
                ( value.u.value.u.signedInt < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS ) )
            {
                pAddNetworkRequest->savedIdx = value.u.value.u.signedInt;
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
            pAddNetworkRequest->connect = value.u.value.u.booleanValue;
        }
        else if( ret == IOT_SERIALIZER_NOT_FOUND )
        {
            IotLogInfo( "Connect flag not set in request, using default value always connect = %d", IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT );
            pAddNetworkRequest->connect = IOT_BLE_WIFI_PROV_DEFAULT_ALWAYS_CONNECT;
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

static bool _handleSaveNetworkRequest( const uint8_t * pData,
                                       size_t length )
{
    bool status = false;
    IotTaskPoolError_t taskStatus = IOT_TASKPOOL_SUCCESS;
    IotTaskPoolJob_t job = IOT_TASKPOOL_JOB_INITIALIZER;

    if( IotSemaphore_TryWait( &wifiProvisioning.lock ) == true )
    {
        memset( &wifiProvisioning.addNetworkRequest, 0x00, sizeof( IotBleAddNetworkRequest_t ) );
        status = _deserializeAddNetworkRequest( pData, length, &wifiProvisioning.addNetworkRequest );

        if( status == true )
        {
            IotLogDebug( "Add network request parameters, SSID: %.*s, index: %d, connect: %d",
                         wifiProvisioning.addNetworkRequest.network.ucSSIDLength,
                         wifiProvisioning.addNetworkRequest.network.cSSID,
                         wifiProvisioning.addNetworkRequest.savedIdx,
                         wifiProvisioning.addNetworkRequest.connect );

            taskStatus = IotTaskPool_CreateRecyclableJob( IOT_SYSTEM_TASKPOOL,
                                                          _addNetworkTask,
                                                          NULL,
                                                          &job );

            if( taskStatus == IOT_TASKPOOL_SUCCESS )
            {
                taskStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, job, 0 );

                if( taskStatus != IOT_TASKPOOL_SUCCESS )
                {
                    IotLogError( "Failed to schedule taskpool job for add network request" );
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, job );
                    status = false;
                }
            }
            else
            {
                IotLogError( "Failed to allocate taskpool job for add network request" );
                status = false;
            }
        }

        if( status == false )
        {
            IotSemaphore_Post( &wifiProvisioning.lock );
        }
    }
    else
    {
        IotLogError( "Failed to process the add network request. Already a request is being processed" );
        status = false;
    }

    return status;
}

static bool _deserializeEditNetworkRequest( const uint8_t * pData,
                                            size_t length,
                                            IotBleEditNetworkRequest_t * pEditNetworkRequest )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
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
                pEditNetworkRequest->curIdx = value.u.value.u.signedInt;
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
                pEditNetworkRequest->newIdx = value.u.value.u.signedInt;
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

/*-----------------------------------------------------------*/

static bool _handleEditNetworkRequest( const uint8_t * pData,
                                       size_t length )
{
    bool status = false;
    IotTaskPoolError_t taskStatus = IOT_TASKPOOL_SUCCESS;
    IotTaskPoolJob_t job = IOT_TASKPOOL_JOB_INITIALIZER;

    if( IotSemaphore_TryWait( &wifiProvisioning.lock ) == true )
    {
        memset( &wifiProvisioning.editNetworkRequest, 0x00, sizeof( IotBleEditNetworkRequest_t ) );
        status = _deserializeEditNetworkRequest( pData, length, &wifiProvisioning.editNetworkRequest );

        if( status == true )
        {
            IotLogDebug( "Edit network request parameters, Current index: %d, new index: %d",
                         wifiProvisioning.editNetworkRequest.curIdx,
                         wifiProvisioning.editNetworkRequest.newIdx );

            taskStatus = IotTaskPool_CreateRecyclableJob( IOT_SYSTEM_TASKPOOL,
                                                          _editNetworkTask,
                                                          NULL,
                                                          &job );

            if( taskStatus == IOT_TASKPOOL_SUCCESS )
            {
                taskStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, job, 0 );

                if( taskStatus != IOT_TASKPOOL_SUCCESS )
                {
                    IotLogError( "Failed to schedule taskpool job for edit network request" );
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, job );
                    status = false;
                }
            }
        }
        else
        {
            IotLogError( "Failed to allocate taskpool job for edit network request" );
            status = false;
        }

        if( status == false )
        {
            IotSemaphore_Post( &wifiProvisioning.lock );
        }
    }
    else
    {
        IotLogError( "Failed to process the edit network request. Already a request is being processed" );
        status = false;
    }

    return status;
}

static bool _deserializeDeleteNetworkRequest( const uint8_t * pData,
                                              size_t length,
                                              IotBleDeleteNetworkRequest_t * pDeleteNetworkRequest )
{
    IotSerializerDecoderObject_t decoderObj = { 0 }, value = { 0 };
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
                pDeleteNetworkRequest->idx = ( int16_t ) value.u.value.u.signedInt;
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

/*-----------------------------------------------------------*/

static bool _handleDeleteNetworkRequest( const uint8_t * pData,
                                         size_t length )
{
    bool status = false;
    IotTaskPoolError_t taskStatus = IOT_TASKPOOL_SUCCESS;
    IotTaskPoolJob_t job = IOT_TASKPOOL_JOB_INITIALIZER;

    if( IotSemaphore_TryWait( &wifiProvisioning.lock ) == true )
    {
        memset( &wifiProvisioning.deleteNetworkRequest, 0x00, sizeof( IotBleDeleteNetworkRequest_t ) );
        status = _deserializeDeleteNetworkRequest( pData, length, &wifiProvisioning.deleteNetworkRequest );

        if( status == true )
        {
            IotLogDebug( "Edit network request parameters, index: %d",
                         wifiProvisioning.deleteNetworkRequest.idx );

            taskStatus = IotTaskPool_CreateRecyclableJob( IOT_SYSTEM_TASKPOOL,
                                                          _deleteNetworkTask,
                                                          NULL,
                                                          &job );

            if( taskStatus == IOT_TASKPOOL_SUCCESS )
            {
                taskStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, job, 0 );

                if( taskStatus != IOT_TASKPOOL_SUCCESS )
                {
                    IotLogError( "Failed to schedule taskpool job for delete network request" );
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, job );
                    status = false;
                }
            }
            else
            {
                IotLogError( "Failed to allocate taskpool job for delete network request" );
                status = false;
            }
        }

        if( status == false )
        {
            IotSemaphore_Post( &wifiProvisioning.lock );
        }
    }
    else
    {
        IotLogError( "Failed to process the delete network request. Already a request is being processed" );
        status = false;
    }

    return status;
}


static IotSerializerError_t _serializeNetwork( int32_t responseType,
                                               IotBleWifiNetworkInfo_t * pNetworkInfo,
                                               uint8_t * pBuffer,
                                               size_t * plength )
{
    IotSerializerEncoderObject_t container = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t networkMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t value = { 0 };
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &networkMap, IOT_BLE_WIFI_PROV_NUM_NETWORK_INFO_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = responseType;
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
        value.value.u.signedInt = pNetworkInfo->savedIdx;
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

    return ret;
}

static IotSerializerError_t _serializeStatusResponse( int32_t responseType,
                                                      WIFIReturnCode_t status,
                                                      uint8_t * pBuffer,
                                                      size_t * plength )
{
    IotSerializerEncoderObject_t container = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;
    IotSerializerEncoderObject_t responseMap = IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    IotSerializerScalarData_t value = { 0 };
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;
    size_t length = *plength;

    ret = IOT_BLE_MESG_ENCODER.init( &container, pBuffer, length );

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        ret = IOT_BLE_MESG_ENCODER.openContainer( &container, &responseMap, IOT_BLE_WIFI_PROV_NUM_STATUS_MESG_PARAMS );
    }

    if( IS_VALID_SERIALIZER_RET( ret, pBuffer ) )
    {
        value.type = IOT_SERIALIZER_SCALAR_SIGNED_INT;
        value.value.u.signedInt = responseType;
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

    return ret;
}


/*-----------------------------------------------------------*/

static void _sendStatusResponse( int32_t responseType,
                                 WIFIReturnCode_t status )
{
    uint8_t * pBuffer = NULL;
    size_t mesgLen = 0;
    IotSerializerError_t ret = IOT_SERIALIZER_SUCCESS;

    ret = _serializeStatusResponse( responseType, status, NULL, &mesgLen );

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        IotLogDebug( "Serialized message length %d", mesgLen );
        pBuffer = pvPortMalloc( mesgLen );

        if( pBuffer != NULL )
        {
            ret = _serializeStatusResponse( responseType, status, pBuffer, &mesgLen );
        }
        else
        {
            ret = IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( ret == IOT_SERIALIZER_SUCCESS )
    {
        if( IotBleDataTransfer_Send( wifiProvisioning.pChannel, pBuffer, mesgLen ) != mesgLen )
        {
            IotLogError( "Failed to send status response through ble connection" );
        }
    }
    else
    {
        IotLogError( "Failed to serialize status response, error = %d\n", ret );
    }

    if( pBuffer != NULL )
    {
        vPortFree( pBuffer );
    }
}


/*-----------------------------------------------------------*/

WIFIReturnCode_t _appendNetwork( WIFINetworkProfile_t * pProfile )
{
    WIFIReturnCode_t ret;
    uint16_t idx;

    ret = WIFI_NetworkAdd( pProfile, &idx );

    if( ret == eWiFiSuccess )
    {
        wifiProvisioning.numNetworks++;
        wifiProvisioning.connectedIdx++;
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
        wifiProvisioning.numNetworks--;

        /* Shift the priority for connected network */
        if( index < wifiProvisioning.connectedIdx )
        {
            wifiProvisioning.connectedIdx--;
        }
        else if( index == wifiProvisioning.connectedIdx )
        {
            wifiProvisioning.connectedIdx = IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX;
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

WIFIReturnCode_t _addNewNetwork( WIFINetworkProfile_t * pProfile,
                                 bool connect )
{
    WIFIReturnCode_t ret = eWiFiSuccess;

    if( connect == true )
    {
        ret = _connectNetwork( pProfile );
    }

    if( ret == eWiFiSuccess )
    {
        ret = _appendNetwork( pProfile );

        if( ( ret == eWiFiSuccess ) &&
            ( connect == true ) )
        {
            wifiProvisioning.connectedIdx = 0;
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
            wifiProvisioning.connectedIdx = index;
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

            if( ret == eWiFiSuccess )
            {
                ret = _appendNetwork( &profile );
            }

            if( ret != eWiFiSuccess )
            {
                IotLogError( "Failed to insert network at index %d", index );
                break;
            }
        }
    }

    return ret;
}

static void _sendSavedNetwork( int32_t responseType,
                               WIFINetworkProfile_t * pSavedNetwork,
                               uint16_t idx )
{
    IotBleWifiNetworkInfo_t networkInfo = NETWORK_INFO_DEFAULT_PARAMS;
    uint8_t * message = NULL;
    size_t messageLen = 0;
    IotSerializerError_t serializerRet;

    networkInfo.pSSID = pSavedNetwork->cSSID;
    networkInfo.SSIDLength = pSavedNetwork->ucSSIDLength - 1;
    networkInfo.pBSSID = pSavedNetwork->ucBSSID;
    networkInfo.BSSIDLength = wificonfigMAX_BSSID_LEN;
    networkInfo.connected = ( wifiProvisioning.connectedIdx == idx );
    networkInfo.security = pSavedNetwork->xSecurity;
    networkInfo.savedIdx = ( int32_t ) idx;

    serializerRet = _serializeNetwork( responseType, &networkInfo, NULL, &messageLen );

    if( serializerRet == IOT_SERIALIZER_SUCCESS )
    {
        message = pvPortMalloc( messageLen );

        if( message != NULL )
        {
            serializerRet = _serializeNetwork( responseType, &networkInfo, message, &messageLen );
        }
        else
        {
            serializerRet = IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( serializerRet == IOT_SERIALIZER_SUCCESS )
    {
        if( IotBleDataTransfer_Send( wifiProvisioning.pChannel, message, messageLen ) != messageLen )
        {
            IotLogError( "Failed to send saved network ( SSID:%.*s )", pSavedNetwork->ucSSIDLength, pSavedNetwork->cSSID );
        }
    }
    else
    {
        IotLogError( "Failed to serialize saved network ( SSID:%.*s )", pSavedNetwork->ucSSIDLength, pSavedNetwork->cSSID );
    }

    if( message != NULL )
    {
        vPortFree( message );
    }
}

static void _sendScanNetwork( int32_t responseType,
                              WIFIScanResult_t * pScanNetwork )
{
    IotBleWifiNetworkInfo_t networkInfo = NETWORK_INFO_DEFAULT_PARAMS;
    uint8_t * message = NULL;
    size_t messageLen = 0;
    IotSerializerError_t serializerRet;

    networkInfo.pSSID = pScanNetwork->cSSID;
    networkInfo.SSIDLength = strlen( pScanNetwork->cSSID );
    networkInfo.pBSSID = pScanNetwork->ucBSSID;
    networkInfo.BSSIDLength = wificonfigMAX_BSSID_LEN;
    networkInfo.RSSI = pScanNetwork->cRSSI;
    networkInfo.hidden = ( bool ) pScanNetwork->ucHidden;
    networkInfo.security = pScanNetwork->xSecurity;

    serializerRet = _serializeNetwork( responseType, &networkInfo, NULL, &messageLen );

    if( serializerRet == IOT_SERIALIZER_SUCCESS )
    {
        message = pvPortMalloc( messageLen );

        if( message != NULL )
        {
            serializerRet = _serializeNetwork( responseType, &networkInfo, message, &messageLen );
        }
        else
        {
            serializerRet = IOT_SERIALIZER_OUT_OF_MEMORY;
        }
    }

    if( serializerRet == IOT_SERIALIZER_SUCCESS )
    {
        if( IotBleDataTransfer_Send( wifiProvisioning.pChannel, message, messageLen ) != messageLen )
        {
            IotLogError( "Failed to send scanned network network ( SSID:%s )", pScanNetwork->cSSID );
        }
    }
    else
    {
        IotLogError( "Failed to serialize scanned network network ( SSID:%s )", pScanNetwork->cSSID );
    }

    if( message != NULL )
    {
        vPortFree( message );
    }
}
/*-----------------------------------------------------------*/

void _listNetworkTask( IotTaskPool_t taskPool,
                       IotTaskPoolJob_t job,
                       void * pUserContext )
{
    WIFINetworkProfile_t profile;
    uint16_t idx;
    WIFIReturnCode_t status;
    uint32_t networks_found = 0;

    for( idx = 0; idx < wifiProvisioning.numNetworks; idx++ )
    {
        status = _getSavedNetwork( idx, &profile );

        if( status == eWiFiSuccess )
        {
            _sendSavedNetwork( IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP, &profile, idx );
        }
    }

    memset( scanNetworks, 0x00, sizeof( WIFIScanResult_t ) * IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS );

    status = WIFI_Scan( scanNetworks, wifiProvisioning.listNetworkRequest.maxNetworks );

    if( status == eWiFiSuccess )
    {
        for( idx = 0; idx < wifiProvisioning.listNetworkRequest.maxNetworks; idx++ )
        {
            if( strlen( scanNetworks[ idx ].cSSID ) > 0 )
            {
                networks_found++;
                _sendScanNetwork( IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP, &scanNetworks[ idx ] );
            }
        }

        if( !networks_found )
        {
            _sendStatusResponse( IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP, status );
        }
    }
    else
    {
        _sendStatusResponse( IOT_BLE_WIFI_PROV_MSG_TYPE_LIST_NETWORK_RESP, status );
    }

    IotSemaphore_Post( &wifiProvisioning.lock );
    IotTaskPool_RecycleJob( taskPool, job );
}

/*-----------------------------------------------------------*/

static void _addNetworkTask( IotTaskPool_t taskPool,
                             IotTaskPoolJob_t job,
                             void * pUserContext )
{
    WIFIReturnCode_t ret = eWiFiFailure;

    if( wifiProvisioning.addNetworkRequest.savedIdx != IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX )
    {
        ret = _connectSavedNetwork( wifiProvisioning.addNetworkRequest.savedIdx );
    }
    else
    {
        if( wifiProvisioning.numNetworks < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS )
        {
            ret = _addNewNetwork( &wifiProvisioning.addNetworkRequest.network,
                                  wifiProvisioning.addNetworkRequest.connect );
        }
        else
        {
            IotLogError( "Failed to add a new network, max networks limit (%d) reached", IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS );
            ret = eWiFiFailure;
        }
    }

    _sendStatusResponse( IOT_BLE_WIFI_PROV_MSG_TYPE_SAVE_NETWORK_RESP, ret );
    IotSemaphore_Post( &wifiProvisioning.lock );
    IotTaskPool_RecycleJob( taskPool, job );
}



/*-----------------------------------------------------------*/

static void _deleteNetworkTask( IotTaskPool_t taskPool,
                                IotTaskPoolJob_t job,
                                void * pUserContext )
{
    WIFIReturnCode_t ret = eWiFiFailure;

    ret = _popNetwork( wifiProvisioning.deleteNetworkRequest.idx, NULL );

    if( ret == eWiFiSuccess )
    {
        if( wifiProvisioning.connectedIdx == IOT_BLE_WIFI_PROV_INVALID_NETWORK_INDEX )
        {
            ( void ) WIFI_Disconnect();
        }
    }

    _sendStatusResponse( IOT_BLE_WIFI_PROV_MSG_TYPE_DELETE_NETWORK_RESP, ret );

    IotSemaphore_Post( &wifiProvisioning.lock );
    IotTaskPool_RecycleJob( taskPool, job );
}



/*-----------------------------------------------------------*/

static void _editNetworkTask( IotTaskPool_t taskPool,
                              IotTaskPoolJob_t job,
                              void * pUserContext )
{
    WIFIReturnCode_t ret = eWiFiFailure;

    ret = _moveNetwork( wifiProvisioning.editNetworkRequest.curIdx, wifiProvisioning.editNetworkRequest.newIdx );
    _sendStatusResponse( IOT_BLE_WIFI_PROV_MSG_TYPE_EDIT_NETWORK_RESP, ret );
    IotSemaphore_Post( &wifiProvisioning.lock );
    IotTaskPool_RecycleJob( taskPool, job );
}

/*-----------------------------------------------------------*/

static uint32_t _getNumSavedNetworks( void )
{
    uint32_t idx;
    WIFIReturnCode_t WifiRet;
    WIFINetworkProfile_t profile;

    for( idx = 0; idx < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS; idx++ )
    {
        WifiRet = WIFI_NetworkGet( &profile, idx );

        if( WifiRet != eWiFiSuccess )
        {
            break;
        }
    }

    return idx;
}

/*-----------------------------------------------------------*/


bool IotBleWifiProv_Init( void )
{
    bool ret = false;

    ret = IotSemaphore_Create( &wifiProvisioning.lock, 1, 1 );

    if( ret == true )
    {
        wifiProvisioning.numNetworks = _getNumSavedNetworks();

        wifiProvisioning.pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING );

        if( wifiProvisioning.pChannel != NULL )
        {
            ( void ) IotBleDataTransfer_SetCallback( wifiProvisioning.pChannel, _callback, NULL );
        }
        else
        {
            ret = false;
        }
    }

    return ret;
}

/*--------------------------------------------------------------------------------*/

uint32_t IotBleWifiProv_GetNumNetworks( void )
{
    return _getNumSavedNetworks();
}

bool IotBleWifiProv_Connect( uint32_t networkIndex )
{
    WIFIReturnCode_t WifiRet;
    bool ret = false;

    WifiRet = _connectSavedNetwork( networkIndex );

    if( WifiRet == eWiFiSuccess )
    {
        ret = true;
    }

    return ret;
}


bool IotBleWifiProv_EraseAllNetworks( void )
{
    bool ret = true;
    WIFIReturnCode_t WiFiRet;

    IotSemaphore_Wait( &wifiProvisioning.lock );

    while( wifiProvisioning.numNetworks > 0 )
    {
        WiFiRet = _popNetwork( 0, NULL );

        if( WiFiRet != eWiFiSuccess )
        {
            IotLogError( "Failed to delete WIFI network, error = %d", WiFiRet );
            ret = false;
            break;
        }
    }

    IotSemaphore_Post( &wifiProvisioning.lock );

    return ret;
}

/*-----------------------------------------------------------*/

void IotBleWifiProv_Deinit( void )
{
    if( wifiProvisioning.pChannel != NULL )
    {
        IotBleDataTransfer_Close( wifiProvisioning.pChannel );
        IotBleDataTransfer_Reset( wifiProvisioning.pChannel );
    }

    /*Delete service. */
    IotSemaphore_Destroy( &wifiProvisioning.lock );

    memset( &wifiProvisioning, 0x00, sizeof( IotBleWifiProvService_t ) );
}

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "iot_ble_wifi_prov_test_access_define.h"
#endif
