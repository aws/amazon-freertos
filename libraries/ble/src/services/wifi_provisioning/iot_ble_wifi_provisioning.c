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
 * @file aws_ble_wifi_provisioning.c
 * @brief BLE Gatt service for WiFi provisioning
 */

#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "iot_config.h"
#include "iot_ble_config.h"
#include "iot_ble_data_transfer.h"
#include "iot_ble_wifi_provisioning.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL                           IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL                           IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME                                ( "BLE_WIFI_PROV" )
#include "iot_logging_setup.h"

#define IOT_BLE_WIFI_PROVISIONING_COMMAND_TIMEOUT_MS    ( 5000 )

/**
 * @brief Macro to check if provided network index for an operation is within range.
 */
#define IS_INDEX_WITHIN_RANGE( index )    ( ( index >= 0 ) && ( index < wifiProvisioning.numNetworks ) )

#define STORAGE_INDEX( priority )         ( wifiProvisioning.numNetworks - priority - 1 )

#define INVALID_INDEX    ( -1 )

/*---------------------------------------------------------------------------------------------------------*/

/**
 * @ingroup ble_datatypes_structs
 * @brief Structure used for internal bookkeeping of WiFi provisioning service.
 */
typedef struct IotBleWifiProvService
{
    TaskHandle_t taskHandle;                  /**< Handle for the task used for WiFi provisioing. */
    IotBleDataTransferChannel_t * pChannel;   /**< A pointer to the ble connection object. */
    uint16_t numNetworks;                     /**< Keeps track of total number of networks stored. */
    int16_t connectedIdx;                     /**< Keeps track of the flash index of the network that is connected. */
    QueueHandle_t messageQueue;               /**< Message queue use by the provisioning task to process incoming requests. */
    SemaphoreHandle_t mutex;                  /**< Mutex used to synchronize between application task the provsioning task .*/
    IotBleWifiProvSerializer_t * pSerializer; /**< Serializer interface used to pack/unpack messages over BLE. */
} IotBleWifiProvService_t;

/**
 * @brief Structure is used to hold each element in the message queue of WiFi provisioing service.
 */
typedef struct
{
    IotBleWiFiProvRequest_t type;
    union
    {
        IotBleWifiProvListNetworksRequest_t listNetworkRequest;
        IotBleWifiProvAddNetworkRequest_t addNetworkRequest;
        IotBleWifiProvEditNetworkRequest_t editNetworkRequest;
        IotBleWifiProvDeleteNetworkRequest_t deleteNetworkRequest;
    } m;
} IotBleWifiProvMessage_t;

static IotBleWifiProvService_t wifiProvisioning = { 0 };

/*---------------------------------------------------------------------------------------------------------*/


static WIFIScanResult_t scanNetworks[ IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS ] = { 0 };

/*
 * @brief Callback registered for BLE write and read events received for each characteristic.
 */
static void prvTransportReceiveCallback( IotBleDataTransferChannelEvent_t event,
                                         IotBleDataTransferChannel_t * pChannel,
                                         void * pContext );

/*
 * @brief Parses List Network request params and creates task to list networks.
 */
static BaseType_t prvHandleListNetworkRequest( const uint8_t * pData,
                                               size_t length );

/*
 * @brief Parses Save Network request params and creates task to save the new network.
 */

static BaseType_t prvHandleAddNetworkRequest( const uint8_t * pData,
                                              size_t length );

/*
 * @brief Parses Edit Network request params and creates task to edit network priority.
 */
static BaseType_t prvHandleEditNetworkRequest( const uint8_t * pData,
                                               size_t length );

/*
 * @brief Parses Delete Network request params and creates task to delete a WiFi networ.
 */
static BaseType_t prvHandleDeleteNetworkRequest( const uint8_t * pData,
                                                 size_t length );

/*
 * @brief Sends a status response for the request.
 */
static void prvSendStatusResponse( IotBleWiFiProvRequest_t responseType,
                                   WIFIReturnCode_t status );

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

/*
 * @brief Gets the number of saved networks from flash.
 */
static uint32_t _getNumSavedNetworks( void );

/*-----------------------------------------------------------*/

static void prvTransportReceiveCallback( IotBleDataTransferChannelEvent_t event,
                                         IotBleDataTransferChannel_t * pChannel,
                                         void * pContext )
{
    IotBleWiFiProvRequest_t request;
    const uint8_t * pBuffer;
    size_t length;


    if( event == IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED )
    {
        IotBleDataTransfer_PeekReceiveBuffer( pChannel, &pBuffer, &length );

        if( wifiProvisioning.pSerializer->getRequestType( pBuffer, length, &request ) == true )
        {
            configPRINTF( ( "Received message type: %d, length %d.\r\n", request, length ) );

            switch( request )
            {
                case IotBleWiFiProvRequestListNetwork:
                    prvHandleListNetworkRequest( pBuffer, length );
                    break;

                case IotBleWiFiProvRequestAddNetwork:
                    prvHandleAddNetworkRequest( pBuffer, length );
                    break;

                case IotBleWiFiProvRequestEditNetwork:
                    prvHandleEditNetworkRequest( pBuffer, length );
                    break;

                case IotBleWiFiProvRequestDeleteNetwork:
                    prvHandleDeleteNetworkRequest( pBuffer, length );
                    break;

                default:
                    IotLogWarn( "Invalid request type ( %d ) received, discarding the message", request );
                    break;
            }
        }

        /* Do an empty read to flush the buffer. */
        IotBleDataTransfer_Receive( pChannel, NULL, length );
    }
}

/*-----------------------------------------------------------*/

static BaseType_t prvHandleListNetworkRequest( const uint8_t * pData,
                                               size_t length )
{
    BaseType_t status;
    IotBleWifiProvMessage_t message = { 0 };

    message.type = IotBleWiFiProvRequestListNetwork;

    status = wifiProvisioning.pSerializer->deserializeListNetworkRequest( pData, length, &message.m.listNetworkRequest );

    if( status == pdTRUE )
    {
        if( ( message.m.listNetworkRequest.scanSize <= 0 ) || ( message.m.listNetworkRequest.scanSize > IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS ) )
        {
            IotLogInfo( "Networks %d exceeds configured value, truncating to  %d max network\n",
                        ( uint16_t ) message.m.listNetworkRequest.scanSize,
                        IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS );
            message.m.listNetworkRequest.scanSize = IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS;
        }

        IotLogDebug( "List network request parameters: max networks = %d, timeout = %d",
                     message.m.listNetworkRequest.scanSize,
                     message.m.listNetworkRequest.scanTimeoutMS );

        status = xQueueSend( wifiProvisioning.messageQueue, &message, 0 );

        if( status != pdTRUE )
        {
            IotLogError( "Failed to enqueue list network request, queue is full." );
        }
    }
    else
    {
        IotLogError( "Failed to deserialize list network request." );
    }

    return status;
}

/*-----------------------------------------------------------*/


static BaseType_t prvHandleAddNetworkRequest( const uint8_t * pData,
                                              size_t length )
{
    BaseType_t status;
    IotBleWifiProvMessage_t message = { 0 };

    message.type = IotBleWiFiProvRequestAddNetwork;

    status = wifiProvisioning.pSerializer->deserializeAddNetworkRequest( pData, length, &message.m.addNetworkRequest );

    if( status == pdTRUE )
    {
        IotLogDebug( "Add network request parameters, SSID: %.*s, isProvisioned: %d, index: %d, connect: %d",
                     message.m.addNetworkRequest.info.network.ucSSIDLength,
                     ( char * ) message.m.addNetworkRequest.info.network.ucSSID,
                     message.m.addNetworkRequest.isProvisioned,
                     message.m.addNetworkRequest.info.index,
                     message.m.addNetworkRequest.shouldConnect );

        status = xQueueSend( wifiProvisioning.messageQueue, &message, 0 );

        if( status != pdTRUE )
        {
            IotLogError( "Failed to enqueue add network request, queue is full." );
        }
    }
    else
    {
        IotLogError( "Failed to deserialize add network request." );
    }

    return status;
}

/*-----------------------------------------------------------*/

static BaseType_t prvHandleEditNetworkRequest( const uint8_t * pData,
                                               size_t length )
{
    BaseType_t status;
    IotBleWifiProvMessage_t message = { 0 };

    message.type = IotBleWiFiProvRequestEditNetwork;

    status = wifiProvisioning.pSerializer->deserializeEditNetworkRequest( pData, length, &message.m.editNetworkRequest );

    if( status == pdTRUE )
    {
        IotLogDebug( "Edit network request parameters, Current index: %d, new index: %d",
                     message.m.editNetworkRequest.currentPriorityIndex,
                     message.m.editNetworkRequest.newPriorityIndex );

        status = xQueueSend( wifiProvisioning.messageQueue, &message, 0 );

        if( status != pdTRUE )
        {
            IotLogError( "Failed to enqueue edit network request, queue is full." );
        }
    }
    else
    {
        IotLogError( "Failed to deserialize edit network request." );
    }

    return status;
}

/*-----------------------------------------------------------*/

static BaseType_t prvHandleDeleteNetworkRequest( const uint8_t * pData,
                                                 size_t length )
{
    BaseType_t status;
    IotBleWifiProvMessage_t message = { 0 };

    message.type = IotBleWiFiProvRequestDeleteNetwork;

    status = wifiProvisioning.pSerializer->deserializeDeleteNetworkRequest( pData, length, &message.m.deleteNetworkRequest );

    if( status == pdTRUE )
    {
        IotLogDebug( "Delete network request parameters, index: %d",
                     message.m.deleteNetworkRequest.priorityIndex );

        status = xQueueSend( wifiProvisioning.messageQueue, &message, 0 );

        if( status != pdTRUE )
        {
            IotLogError( "Failed to enqueue delete network request, queue is full." );
        }
    }
    else
    {
        IotLogError( "Failed to deserialize delete network request." );
    }

    return status;
}

/*-----------------------------------------------------------*/

static void prvSendStatusResponse( IotBleWiFiProvRequest_t request,
                                   WIFIReturnCode_t status )
{
    uint8_t * pBuffer = NULL;
    size_t mesgLen = 0;
    bool ret;
    IotBleWifiProvResponse_t response = { 0 };

    response.requestType = request;
    response.status = status;
    response.statusOnly = true;

    ret = wifiProvisioning.pSerializer->getSerializedSizeForResponse( &response, &mesgLen );

    if( ret == true )
    {
        IotLogDebug( "Respone for request type %d, serialized message length %d.", request, mesgLen );

        pBuffer = pvPortMalloc( mesgLen );

        if( pBuffer == NULL )
        {
            IotLogError( "Failed to allocate message for serializing response for request type %d.", request );
            ret = false;
        }
    }
    else
    {
        IotLogError( "Failed to get serialized size of status response." );
    }

    if( ret == true )
    {
        ret = wifiProvisioning.pSerializer->serializeResponse( &response, pBuffer, mesgLen );

        if( ret == false )
        {
            IotLogError( "Failed to serialize status response." );
        }
    }

    if( ret == true )
    {
        if( IotBleDataTransfer_Send( wifiProvisioning.pChannel, pBuffer, mesgLen ) != mesgLen )
        {
            IotLogError( "Failed to send response for request type %d through ble connection.", request );
        }
    }

    if( pBuffer != NULL )
    {
        vPortFree( pBuffer );
    }
}

/*-----------------------------------------------------------*/

static void prvSendListNetworkResponse( IotBleWifiProvResponse_t * pResponse )
{
    uint8_t * message = NULL;
    size_t messageLen = 0;
    bool status;

    status = wifiProvisioning.pSerializer->getSerializedSizeForResponse( pResponse, &messageLen );

    if( status == true )
    {
        message = pvPortMalloc( messageLen );

        if( message != NULL )
        {
            status = wifiProvisioning.pSerializer->serializeResponse( pResponse, message, messageLen );
        }
        else
        {
            IotLogError( "Not enough memory to serialize response." );
            status = false;
        }
    }
    else
    {
        IotLogError( "Failed to serialize list network response." );
    }

    if( status == true )
    {
        if( IotBleDataTransfer_Send( wifiProvisioning.pChannel, message, messageLen ) != messageLen )
        {
            IotLogError( "Failed to send list network response over BLE." );
        }
    }

    if( message != NULL )
    {
        vPortFree( message );
    }
}

/*-----------------------------------------------------------*/


static void prvSendSavedNetwork( WIFINetworkProfile_t * pNetwork,
                                 uint16_t priority,
                                 bool isLast )
{
    IotBleWifiProvResponse_t response = { 0 };

    response.requestType = IotBleWiFiProvRequestListNetwork;
    response.networkInfo.isSavedNetwork = true;
    response.networkInfo.isHidden = false;
    response.networkInfo.isLast = isLast;
    response.networkInfo.isConnected = ( wifiProvisioning.connectedIdx == priority );
    response.networkInfo.info.pSavedNetwork = pNetwork;
    response.networkInfo.index = priority;

    prvSendListNetworkResponse( &response );
}

/*-----------------------------------------------------------*/

static void prvSendScanNetwork( WIFIScanResult_t * pNetwork,
                                bool isLast )
{
    IotBleWifiProvResponse_t response = { 0 };

    response.requestType = IotBleWiFiProvRequestListNetwork;
    response.networkInfo.isSavedNetwork = false;
    response.networkInfo.isHidden = false;
    response.networkInfo.isConnected = false;
    response.networkInfo.isLast = isLast;
    response.networkInfo.info.pScannedNetwork = pNetwork;

    prvSendListNetworkResponse( &response );
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

    if( ( ret == eWiFiSuccess ) && ( wifiProvisioning.numNetworks > 0 ) )
    {
        wifiProvisioning.numNetworks--;

        /* Shift the priority for connected network */
        if( index < wifiProvisioning.connectedIdx )
        {
            wifiProvisioning.connectedIdx--;
        }
        else if( index == wifiProvisioning.connectedIdx )
        {
            wifiProvisioning.connectedIdx = INVALID_INDEX;
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

    memcpy( networkParams.ucSSID, pProfile->ucSSID, pProfile->ucSSIDLength );
    networkParams.ucSSIDLength = pProfile->ucSSIDLength;
    networkParams.xSecurity = pProfile->xSecurity;

    if( ( networkParams.xSecurity == eWiFiSecurityWPA2 ) || ( networkParams.xSecurity == eWiFiSecurityWPA ) )
    {
        memcpy( networkParams.xPassword.xWPA.cPassphrase, pProfile->cPassword, pProfile->ucPasswordLength );
        networkParams.xPassword.xWPA.ucLength = pProfile->ucPasswordLength;
    }

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

/*-----------------------------------------------------------*/

static void prvProcessListNetworkRequest( IotBleWifiProvListNetworksRequest_t * pRequest )
{
    WIFINetworkProfile_t profile;
    uint16_t idx, numNetworks;
    WIFIReturnCode_t status;
    bool isLast = false;

    ( void ) pRequest->scanTimeoutMS;

    for( idx = 0; idx < wifiProvisioning.numNetworks; idx++ )
    {
        status = _getSavedNetwork( idx, &profile );

        if( status == eWiFiSuccess )
        {
            if( idx == wifiProvisioning.numNetworks - 1 )
            {
                isLast = true;
            }
            else
            {
                isLast = false;
            }

            prvSendSavedNetwork( &profile, idx, isLast );
        }
    }

    memset( scanNetworks, 0x00, sizeof( WIFIScanResult_t ) * IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS );

    status = WIFI_Scan( scanNetworks, pRequest->scanSize );

    if( status == eWiFiSuccess )
    {
        for( numNetworks = 0; numNetworks < pRequest->scanSize; numNetworks++ )
        {
            if( scanNetworks[ numNetworks ].ucSSIDLength == 0 )
            {
                break;
            }
        }

        if( numNetworks == 0 )
        {
            prvSendStatusResponse( IotBleWiFiProvRequestListNetwork, status );
        }
        else
        {
            for( idx = 0; idx < numNetworks; idx++ )
            {
                if( idx == numNetworks - 1 )
                {
                    isLast = true;
                }
                else
                {
                    isLast = false;
                }

                prvSendScanNetwork( &scanNetworks[ idx ], isLast );
            }
        }
    }
    else
    {
        prvSendStatusResponse( IotBleWiFiProvRequestListNetwork, status );
    }
}

/*-----------------------------------------------------------*/

static void prvProcessAddNetworkRequest( IotBleWifiProvAddNetworkRequest_t * pRequest )
{
    WIFIReturnCode_t status = eWiFiFailure;

    if( pRequest->isProvisioned == true )
    {
        if( IS_INDEX_WITHIN_RANGE( pRequest->info.index ) )
        {
            status = _connectSavedNetwork( pRequest->info.index );
        }
    }
    else
    {
        if( wifiProvisioning.numNetworks < IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS )
        {
            status = _addNewNetwork( &pRequest->info.network,
                                     pRequest->shouldConnect );

            if( status != eWiFiSuccess )
            {
                IotLogError( "Failed to provision new network with SSID: %.*s, password: %.*s",
                             pRequest->info.network.ucSSIDLength,
                             ( char * ) pRequest->info.network.ucSSID,
                             pRequest->info.network.ucPasswordLength,
                             pRequest->info.network.cPassword );
            }
        }
        else
        {
            IotLogError( "Failed to add a new network, max networks limit (%d) reached", IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS );
            status = eWiFiFailure;
        }
    }

    prvSendStatusResponse( IotBleWiFiProvRequestAddNetwork, status );
}

/*-----------------------------------------------------------*/

static void prvProcessDeleteNetworkRequest( IotBleWifiProvDeleteNetworkRequest_t * pRequest )
{
    WIFIReturnCode_t status = eWiFiFailure;

    if( IS_INDEX_WITHIN_RANGE( pRequest->priorityIndex ) )
    {
        status = _popNetwork( pRequest->priorityIndex, NULL );
    }

    if( status == eWiFiSuccess )
    {
        if( wifiProvisioning.connectedIdx == INVALID_INDEX )
        {
            ( void ) WIFI_Disconnect();
        }
    }

    prvSendStatusResponse( IotBleWiFiProvRequestDeleteNetwork, status );
}

/*-----------------------------------------------------------*/

static void prvProcessEditNetworkRequest( IotBleWifiProvEditNetworkRequest_t * pRequest )
{
    WIFIReturnCode_t status = eWiFiFailure;

    if( IS_INDEX_WITHIN_RANGE( pRequest->currentPriorityIndex ) &&
        IS_INDEX_WITHIN_RANGE( pRequest->newPriorityIndex ) )
    {
        status = _moveNetwork( pRequest->currentPriorityIndex, pRequest->newPriorityIndex );
    }

    prvSendStatusResponse( IotBleWiFiProvRequestEditNetwork, status );
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
    bool ret = true;

    wifiProvisioning.mutex = xSemaphoreCreateMutex();

    if( wifiProvisioning.mutex == NULL )
    {
        IotLogError( "Failed to create mutex for WiFi provisioning task." );
        ret = false;
    }

    if( ret == true )
    {
        wifiProvisioning.messageQueue = xQueueCreate( 2U, sizeof( IotBleWifiProvMessage_t ) );

        if( wifiProvisioning.messageQueue == NULL )
        {
            IotLogError( "Failed to create queue for WiFi provisioning task." );
            ret = false;
        }
    }

    if( ret == true )
    {
        wifiProvisioning.pSerializer = IotBleWifiProv_GetSerializer();

        if( wifiProvisioning.pSerializer == NULL )
        {
            ret = false;
        }
    }

    if( ret == true )
    {
        wifiProvisioning.connectedIdx = INVALID_INDEX;
        wifiProvisioning.numNetworks = _getNumSavedNetworks();
    }

    return ret;
}

bool IotBleWifiProv_RunProcessLoop( void )
{
    IotBleWifiProvMessage_t message = { 0 };
    bool processLoopStatus = false;

    configPRINTF( ( "Entering provisonining loop function.\r\n" ) );

    wifiProvisioning.pChannel = IotBleDataTransfer_Open( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING );

    if( wifiProvisioning.pChannel != NULL )
    {
        processLoopStatus = IotBleDataTransfer_SetCallback( wifiProvisioning.pChannel, prvTransportReceiveCallback, NULL );
    }
    else
    {
        IotLogError( "Failed to open BLE transport channel for WiFi provisioning." );
        processLoopStatus = false;
    }

    if( processLoopStatus == true )
    {
        configPRINTF( ( "Entering loop.\r\n" ) );

        for( ; ; )
        {
            memset( &message, 0x00, sizeof( IotBleWifiProvMessage_t ) );
            ( void ) xQueueReceive( wifiProvisioning.messageQueue, &message, portMAX_DELAY );

            if( message.type == IotBleWiFiProvRequestStop )
            {
                configPRINTF( ( "Received stop.\r\n" ) );
                xQueueReset( wifiProvisioning.messageQueue );
                break;
            }
            else
            {
                configPRINTF( ( "Received message.\r\n" ) );
                xSemaphoreTake( wifiProvisioning.mutex, portMAX_DELAY );

                switch( message.type )
                {
                    case IotBleWiFiProvRequestListNetwork:
                        prvProcessListNetworkRequest( &message.m.listNetworkRequest );
                        break;

                    case IotBleWiFiProvRequestAddNetwork:
                        prvProcessAddNetworkRequest( &message.m.addNetworkRequest );
                        break;

                    case IotBleWiFiProvRequestEditNetwork:
                        prvProcessEditNetworkRequest( &message.m.editNetworkRequest );
                        break;

                    case IotBleWiFiProvRequestDeleteNetwork:
                        prvProcessDeleteNetworkRequest( &message.m.deleteNetworkRequest );
                        break;

                    default:
                        break;
                }

                xSemaphoreGive( wifiProvisioning.mutex );
            }
        }
    }

    configPRINTF( ( "Exit loop.\r\n" ) );

    if( wifiProvisioning.pChannel != NULL )
    {
        IotBleDataTransfer_Close( wifiProvisioning.pChannel );
        IotBleDataTransfer_Reset( wifiProvisioning.pChannel );
        wifiProvisioning.pChannel = NULL;
    }

    return processLoopStatus;
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

    xSemaphoreTake( wifiProvisioning.mutex, portMAX_DELAY );

    WifiRet = _connectSavedNetwork( networkIndex );

    if( WifiRet == eWiFiSuccess )
    {
        ret = true;
    }

    xSemaphoreGive( wifiProvisioning.mutex );

    return ret;
}

bool IotBleWifiProv_EraseAllNetworks( void )
{
    bool ret = true;
    WIFIReturnCode_t WiFiRet;

    xSemaphoreTake( wifiProvisioning.mutex, portMAX_DELAY );

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

    xSemaphoreGive( wifiProvisioning.mutex );

    return ret;
}

/*-----------------------------------------------------------*/

bool IotBleWifiProv_Stop( void )
{
    BaseType_t status;
    IotBleWifiProvMessage_t message =
    {
        .type = IotBleWiFiProvRequestStop
    };

    status = xQueueSend( wifiProvisioning.messageQueue, &message, pdMS_TO_TICKS( IOT_BLE_WIFI_PROVISIONING_COMMAND_TIMEOUT_MS ) );
    return( status == pdTRUE );
}

/*-----------------------------------------------------------*/

void IotBleWifiProv_Deinit( void )
{
    /*Delete queue and semaphore. */
    vQueueDelete( wifiProvisioning.messageQueue );
    vSemaphoreDelete( wifiProvisioning.mutex );

    memset( &wifiProvisioning, 0x00, sizeof( IotBleWifiProvService_t ) );
}

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
    #include "iot_ble_wifi_prov_test_access_define.h"
#endif
