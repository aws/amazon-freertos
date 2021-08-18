/*
 * FreeRTOS V202107.00
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_network_manager.c
 * @brief Network manager is used to handled different types of network connections and their connection/disconnection events at the application layer.
 */
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "iot_demo_logging.h"
#include "iot_network_manager_private.h"

#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"

#if BLE_ENABLED
    #include "iot_ble_config.h"
    #include "iot_ble.h"
    #include "iot_ble_numericComparison.h"
#endif

#if WIFI_ENABLED
    #include "iot_wifi.h"
#endif

#if TCPIP_NETWORK_ENABLED
    #include "platform/iot_network_freertos.h"
#endif


#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    #include "iot_ble_wifi_provisioning.h"
    #include "aws_wifi_connect_task.h"
#endif
#if ( IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 1 )
    #include "iot_softap_wifi_provisioning.h"
    #include "aws_wifi_connect_task.h"
#endif

#define _NM_PARAMS( networkType, networkState )    ( ( ( uint32_t ) networkType ) << 16 | ( ( uint16_t ) networkState ) )

#define _NM_GET_NETWORK_TYPE( params )             ( ( uint32_t ) ( ( params ) >> 16 ) & 0x0000FFFFUL )

#define _NM_GET_NETWORK_STATE( params )            ( ( AwsIotNetworkState_t ) ( ( params ) & 0x0000FFFFUL ) )

#define _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS    ( 1000 )

#define _NM_WIFI_CONNECTION_RETRIES              ( 5 )

/**
 * @brief Maximum number of application subscriptions allowed with network manager.
 * Applications can subscribe callbacks to be notified of network state change events from network manager.
 */
#define NETWORK_MANAGER_MAX_SUBSCRIPTIONS        ( 5 )

/**
 * @brief Maximum number of events from underlying layers pending in network manager queue at any time.
 * Events could be generated from underlying layer at a faster rate than they are processed with in network manager
 * subscription callbacks. Queue holds pending events to be procesed by network manager any time.
 */
#define NETWORK_MANAGER_EVENT_QUEUE_SIZE         ( 10 )

/**
 * @brief Priority of the network manager task.
 */
#define NETWORK_MANAGER_TASK_PRIORITY            ( tskIDLE_PRIORITY )

/**
 * @brief Stack size for network manager task.
 * The task executes other application callbacks in its task context, so tune the stack size according
 * to application subscription callback requirements.
 */
#define NETWORK_MANAGER_TASK_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 4 )

/**
 * @brief Structure holds information for each network and the runtime state of it.
 */
typedef struct IotNMNetwork
{
    uint32_t type;
    AwsIotNetworkState_t state;
    const IotNetworkInterface_t * pNetworkInterface;
    void * pConnectionParams;
    void * pCredentials;
} IotNMNetwork_t;

/**
 * @brief Structure holds information for each of the network state change subscription.
 */
typedef struct IotNMSubscription
{
    uint32_t networkTypes;
    AwsIotNetworkStateChangeCb_t callback;
    void * pContext;
    BaseType_t isActive;
} IotNMSubscription_t;

/**
 * @brief Enum defines the types of events received by the network manager from underlying network driver.
 */
typedef enum IotNetworkEventType
{
    IOT_NETWORK_EVENT_CONNECTED = 0,
    IOT_NETWORK_EVENT_DISCONNECTED = 1
} IotNetworkEventType_t;

/**
 * @brief Structure holds the event information associated with an event received from the underlying network driver.
 * Structure is used queue network events for the network manager task to process.
 */
typedef struct IotNetworkEvent
{
    uint32_t networkType;
    IotNetworkEventType_t eventType;
} IotNetworkEvent_t;

/**
 * @brief Network manager object, holds the list of supported networks and the list of network state change subscriptions.
 */
typedef struct IotNetworkManager
{
    IotNMNetwork_t * pNetworks;
    size_t numNetworks;
    IotNMSubscription_t subscriptions[ NETWORK_MANAGER_MAX_SUBSCRIPTIONS ];
    QueueHandle_t eventQueue;
    SemaphoreHandle_t globalMutex;
    SemaphoreHandle_t subscriptionsMutex;
} IotNetworkManagerInfo_t;

#if BLE_ENABLED

/**
 * @brief Register/unregister callbacks related to BLE middleware.
 *
 * @param[in] unRegister Set to true to unRegister
 * @return true if success false otherwise.
 *
 */
    static bool _bleRegisterUnregisterCb( bool unRegister );

/**
 * @brief Function used to enable a BLE network.
 *
 * @return true if BLE is enabled successfully.
 */
    static bool _bleEnable( void );


/**
 * @brief Function used to disable a BLE network.
 *
 * @return true if BLE is disable successfully, false if already disabled
 */
    static bool _bleDisable( void );


/**
 * @brief Callback invokedon a new BLE connection or disconnection.
 */
    static void _bleConnectionCallback( BTStatus_t status,
                                        uint16_t connectionID,
                                        bool isConnected,
                                        BTBdaddr_t * pRemoteAddress );

#endif /* if BLE_ENABLED */


#if WIFI_ENABLED

    #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 )

/**
 * Connects to the WIFI using credentials configured statically
 * @return true if connected successfully.
 */
        static bool _wifiConnectAccessPoint( void );

    #endif

/**
 * @brief Function used to enable a WIFI network.
 *
 * @return true if WIFI is enabled successfully.
 */
    static bool _wifiEnable( void );

/**
 * @brief Function used to disable a WIFI network.
 *
 * @return true if WIFI is disable successfully, false if already disabled
 */
    static bool _wifiDisable( void );

/**
 * @brief The callback invoked for WiFi events from the driver. The callback
 * is used to change the WiFi state in network manager.
 */
    static void _wifiEventHandler( WIFIEvent_t * xEvent );

#endif /* if WIFI_ENABLED */

#if ETH_ENABLED

/**
 * @brief Function used to enable a WIFI network.
 *
 * @return true if WIFI is enabled successfully.
 */
    static bool _ethEnable( void );

/**
 * @brief Function used to disable a WIFI network.
 *
 * @return true if WIFI is disable successfully, false if already disabled
 */
    static bool _ethDisable( void );

#endif /* if ETH_ENABLED */


/**
 * @brief Get the network instance associated with the network type.
 *
 * @param[in] networkType Type of the network.
 * @return Pointer to the network instance. NULL if network is not configured.
 *
 */
static IotNMNetwork_t * prvGetNetworkInstance( uint32_t networkType );

/**
 * @brief Dispatch a network event to the application subscription callbacks.
 * @param[in] pEvent Pointer to the event struct for the event.
 */
static void prvDispatch( uint32_t networkType,
                         AwsIotNetworkState_t state );

/**
 * @brief Initialize TCP/IP credentials for WIFI and ethernet networks.
 */
static void _initializeTCPIPCredentials( void );

/**
 * @brief The credentials placeholder for a TCP/IP network.
 *
 */
static IotNetworkCredentials_t tcpIPCredentials = { 0 };

/*
 * @brief Connection parameters placeholder for a TCP/IP network.
 */
static IotNetworkServerInfo_t tcpIPConnectionParams = { 0 };

static IotNMNetwork_t networks[] =
{
    #if BLE_ENABLED

        /**
         * @brief Configuration and state for a BLE Network.
         */
        {
            .type = AWSIOT_NETWORK_TYPE_BLE,
            .state = eNetworkStateDisabled,
            .pNetworkInterface = NULL,
            .pCredentials = NULL,
            .pConnectionParams = NULL
        },
    #endif /* if BLE_ENABLED */

    #if WIFI_ENABLED
        {
            .type = AWSIOT_NETWORK_TYPE_WIFI,
            .state = eNetworkStateDisabled,
            .pNetworkInterface = IOT_NETWORK_INTERFACE_AFR,
            .pCredentials = &tcpIPCredentials,
            .pConnectionParams = &tcpIPConnectionParams
        },
    #endif

    #if ETH_ENABLED
        {
            .type = AWSIOT_NETWORK_TYPE_ETH,
            .state = eNetworkStateDisabled,
            .pNetworkInterface = IOT_NETWORK_INTERFACE_AFR,
            .pCredentials = &tcpIPCredentials,
            .pConnectionParams = &tcpIPConnectionParams
        },
    #endif
};

static IotNetworkManagerInfo_t networkManager;


#if BLE_ENABLED

    static bool _bleRegisterUnregisterCb( bool unRegister )
    {
        IotBleEventsCallbacks_t eventCallback;
        BTStatus_t status = eBTStatusSuccess;
        bool ret = false;

        eventCallback.pConnectionCb = _bleConnectionCallback;

        if( unRegister == false )
        {
            status = IotBle_RegisterEventCb( eBLEConnection, eventCallback );
        }
        else
        {
            status = IotBle_UnRegisterEventCb( eBLEConnection, eventCallback );
        }

        if( status == eBTStatusSuccess )
        {
            eventCallback.pGAPPairingStateChangedCb = &vDemoBLEGAPPairingStateChangedCb;

            if( unRegister == false )
            {
                status = IotBle_RegisterEventCb( eBLEPairingStateChanged, eventCallback );
            }
            else
            {
                status = IotBle_UnRegisterEventCb( eBLEPairingStateChanged, eventCallback );
            }
        }

        #if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
            if( status == eBTStatusSuccess )
            {
                eventCallback.pNumericComparisonCb = &vDemoBLENumericComparisonCb;

                if( unRegister == false )
                {
                    status = IotBle_RegisterEventCb( eBLENumericComparisonCallback, eventCallback );
                }
                else
                {
                    status = IotBle_UnRegisterEventCb( eBLENumericComparisonCallback, eventCallback );
                }
            }
        #endif /* if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 ) */

        if( status == eBTStatusSuccess )
        {
            ret = true;
        }

        return ret;
    }

    static bool _bleEnable( void )
    {
        bool status = false;
        static bool bleInited = false;
        IotNMNetwork_t * pNetwork = NULL;


        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_BLE );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state == eNetworkStateDisabled )
        {
            if( bleInited == false )
            {
                /* Perform BLE one time initialization. */
                if( IotBle_Init() != eBTStatusSuccess )
                {
                    IotLogError( "Failed to initialize BLE stack." );
                }
                else
                {
                    bleInited = true;
                    status = true;
                }

                #if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
                    if( status == true )
                    {
                        vDemoBLENumericComparisonInit();
                    }
                #endif
            }
            else
            {
                /* BLE one time initialization done. */
                status = true;
            }

            if( status == true )
            {
                status = _bleRegisterUnregisterCb( false );
            }

            if( status == true )
            {
                if( IotBle_On() != eBTStatusSuccess )
                {
                    IotLogError( "Failed to turn on BLE." );
                    status = false;
                }
            }

            if( status == true )
            {
                pNetwork->state = eNetworkStateEnabled;
            }
        }

        return status;
    }

/*-----------------------------------------------------------*/
/* TODO make same function to register/unregister or risk of memory leak. */
    static bool _bleDisable( void )
    {
        bool ret = false;
        IotNMNetwork_t * pNetwork = NULL;

        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_BLE );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state != eNetworkStateDisabled )
        {
            /* Unregister the callbacks */
            ret = _bleRegisterUnregisterCb( true );

            if( ret == true )
            {
                /* Turn off BLE */
                if( IotBle_Off() != eBTStatusSuccess )
                {
                    ret = false;
                }
            }

            if( ret == true )
            {
                pNetwork->state = eNetworkStateDisabled;
            }
        }

        return ret;
    }

    static void _bleConnectionCallback( BTStatus_t status,
                                        uint16_t connectionID,
                                        bool isConnected,
                                        BTBdaddr_t * pRemoteAddress )
    {
        IotNetworkEvent_t event =
        {
            .networkType = AWSIOT_NETWORK_TYPE_BLE
        };
        BaseType_t sendStatus;

        if( isConnected == true )
        {
            IotLogInfo( "BLE Connected to remote device, connId = %d\n", connectionID );
            IotBle_StopAdv( NULL );
            event.eventType = IOT_NETWORK_EVENT_CONNECTED;
            sendStatus = xQueueSend( networkManager.eventQueue, &event, ( TickType_t ) 1U );
            configASSERT( sendStatus == pdTRUE );
        }
        else
        {
            IotLogInfo( "BLE disconnected with remote device, connId = %d \n", connectionID );
            ( void ) IotBle_StartAdv( NULL );
            event.eventType = IOT_NETWORK_EVENT_DISCONNECTED;
            sendStatus = xQueueSend( networkManager.eventQueue, &event, ( TickType_t ) 1U );
            configASSERT( sendStatus == pdTRUE );
        }
    }


#endif /* if BLE_ENABLED */
/*-----------------------------------------------------------*/
#if WIFI_ENABLED

    #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 )

        static bool _wifiConnectAccessPoint( void )
        {
            bool status = true;
            WIFINetworkParams_t xConnectParams;
            size_t xSSIDLength, xPasswordLength;
            const char * pcSSID = clientcredentialWIFI_SSID;
            const char * pcPassword = clientcredentialWIFI_PASSWORD;
            WIFISecurity_t xSecurity = clientcredentialWIFI_SECURITY;
            uint32_t numRetries = _NM_WIFI_CONNECTION_RETRIES;
            uint32_t delayMilliseconds = _NM_WIFI_CONNECTION_RETRY_INTERVAL_MS;

            if( pcSSID != NULL )
            {
                xSSIDLength = strlen( pcSSID );

                if( ( xSSIDLength > 0 ) && ( xSSIDLength < wificonfigMAX_SSID_LEN ) )
                {
                    xConnectParams.ucSSIDLength = xSSIDLength;
                    memcpy( xConnectParams.ucSSID, clientcredentialWIFI_SSID, xSSIDLength );
                }
                else
                {
                    status = false;
                }
            }
            else
            {
                status = false;
            }

            xConnectParams.xSecurity = xSecurity;

            switch( xSecurity )
            {
                case eWiFiSecurityWPA:
                case eWiFiSecurityWPA2:

                    if( pcPassword != NULL )
                    {
                        xPasswordLength = strlen( clientcredentialWIFI_PASSWORD );

                        if( ( xPasswordLength > 0 ) && ( xPasswordLength < wificonfigMAX_PASSPHRASE_LEN ) )
                        {
                            xConnectParams.xPassword.xWPA.ucLength = xPasswordLength;
                            memcpy( xConnectParams.xPassword.xWPA.cPassphrase, clientcredentialWIFI_PASSWORD, xPasswordLength );
                        }
                        else
                        {
                            status = false;
                        }
                    }
                    else
                    {
                        status = false;
                    }

                    break;

                case eWiFiSecurityOpen:
                    /* Nothing to do. */
                    break;

                case eWiFiSecurityWPA3:
                case eWiFiSecurityWPA2_ent:
                case eWiFiSecurityWEP:
                default:
                    IotLogError( "The configured WiFi security option is not supported." );
                    status = false;
                    break;
            }

            if( status == true )
            {
                /* Try to connect to wifi access point with retry and exponential delay */
                do
                {
                    if( WIFI_ConnectAP( &( xConnectParams ) ) == eWiFiSuccess )
                    {
                        break;
                    }
                    else
                    {
                        if( numRetries > 0 )
                        {
                            vTaskDelay( pdMS_TO_TICKS( delayMilliseconds ) );
                            delayMilliseconds = delayMilliseconds * 2;
                        }
                        else
                        {
                            status = false;
                        }
                    }
                } while( numRetries-- > 0 );
            }

            return status;
        }

    #endif /* if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 ) */


    static bool _wifiEnable( void )
    {
        bool ret = true;
        WIFIReturnCode_t wifiRet;
        IotNMNetwork_t * pNetwork = NULL;

        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_WIFI );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state == eNetworkStateDisabled )
        {
            if( WIFI_On() != eWiFiSuccess )
            {
                ret = false;
            }

            if( ret == true )
            {
                /* Network is enabled now. */
                pNetwork->state = eNetworkStateEnabled;

                /* Register network state change callback with the Wi-Fi driver */
                wifiRet = WIFI_RegisterEvent( eWiFiEventIPReady, _wifiEventHandler );

                if( wifiRet == eWiFiSuccess )
                {
                    wifiRet = WIFI_RegisterEvent( eWiFiEventDisconnected, _wifiEventHandler );
                }

                if( ( wifiRet != eWiFiSuccess ) && ( wifiRet != eWiFiNotSupported ) )
                {
                    ret = false;
                }
            }

            #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 )
                if( ret == true )
                {
                    ret = _wifiConnectAccessPoint();

                    if( ret == true )
                    {
                        pNetwork->state = eNetworkStateConnected;
                    }
                }
            #else
                if( ret == true )
                {
                    if( xWiFiConnectTaskInitialize() != pdTRUE )
                    {
                        ret = false;
                    }
                }
            #endif /* if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 ) */
        }

        return ret;
    }

    static bool _wifiDisable( void )
    {
        bool ret = true;

        IotNMNetwork_t * pNetwork = NULL;

        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_WIFI );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state != eNetworkStateDisabled )
        {
            #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 ) || ( IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 1 )
                vWiFiConnectTaskDestroy();
            #endif

            WIFI_RegisterEvent( eWiFiEventDisconnected, NULL );

            if( ret == true )
            {
                if( WIFI_IsConnected( NULL ) == pdTRUE )
                {
                    if( WIFI_Disconnect() != eWiFiSuccess )
                    {
                        ret = false;
                    }
                }
            }

            if( ret == true )
            {
                if( WIFI_Off() != eWiFiSuccess )
                {
                    ret = false;
                }
            }

            if( ret == true )
            {
                pNetwork->state = eNetworkStateDisabled;
            }
        }
        else
        {
            IotLogError( "WiFi network in UNKNOWN state, cannot be disabled." );
            ret = false;
        }

        return ret;
    }

    static void _wifiEventHandler( WIFIEvent_t * pxEvent )
    {
        uint8_t * pucIpAddr;
        IotNetworkEvent_t event =
        {
            .networkType = AWSIOT_NETWORK_TYPE_WIFI
        };
        BaseType_t status;

        if( pxEvent->xEventType == eWiFiEventIPReady )
        {
            pucIpAddr = ( uint8_t * ) ( &pxEvent->xInfo.xIPReady.xIPAddress.ulAddress[ 0 ] );
            IotLogInfo( "Connected to WiFi access point, ip address: %d.%d.%d.%d.", pucIpAddr[ 0 ], pucIpAddr[ 1 ], pucIpAddr[ 2 ], pucIpAddr[ 3 ] );
            event.eventType = IOT_NETWORK_EVENT_CONNECTED;
            status = xQueueSend( networkManager.eventQueue, &event, ( TickType_t ) 1U );
            configASSERT( status == pdTRUE );
        }
        else if( pxEvent->xEventType == eWiFiEventDisconnected )
        {
            IotLogInfo( "Disconnected from WiFi access point, reason code: %d.", pxEvent->xInfo.xDisconnected.xReason );
            event.eventType = IOT_NETWORK_EVENT_DISCONNECTED;
            status = xQueueSend( networkManager.eventQueue, &event, ( TickType_t ) 1U );
            configASSERT( status == pdTRUE );
        }
    }

#endif /* if WIFI_ENABLED */


#if ETH_ENABLED

    static bool _ethEnable( void )
    {
        IotNMNetwork_t * pNetwork = NULL;
        bool ret = false;

        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_ETH );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state == eNetworkStateDisabled )
        {
            pNetwork->state = eNetworkStateConnected;
            ret = true;
        }

        return ret;
    }

    static bool _ethDisable( void )
    {
        IotNMNetwork_t * pNetwork = NULL;
        bool ret = false;

        pNetwork = prvGetNetworkInstance( AWSIOT_NETWORK_TYPE_ETH );
        configASSERT( pNetwork != NULL );

        if( pNetwork->state != eNetworkStateDisabled )
        {
            pNetwork->state = eNetworkStateDisabled;
            ret = true;
        }

        return ret;
    }

#endif /* if ETH_ENABLED */

/*-----------------------------------------------------------*/

static IotNMNetwork_t * prvGetNetworkInstance( uint32_t networkType )
{
    size_t index;
    IotNMNetwork_t * pNetwork = NULL;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].type == networkType )
        {
            pNetwork = &networkManager.pNetworks[ index ];
            break;
        }
    }

    return pNetwork;
}

/*-----------------------------------------------------------*/

static void prvDispatch( uint32_t networkType,
                         AwsIotNetworkState_t state )
{
    size_t index;

    for( index = 0; index < NETWORK_MANAGER_MAX_SUBSCRIPTIONS; index++ )
    {
        if( ( ( networkManager.subscriptions[ index ].networkTypes & networkType ) == networkType ) &&
            ( networkManager.subscriptions[ index ].isActive == true ) )
        {
            IotLogInfo( "Invoked callback for network state change event." );
            networkManager.subscriptions[ index ].callback( networkType, state, networkManager.subscriptions[ index ].pContext );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvNetworkManagerTask( void * pvParams )
{
    BaseType_t status;
    IotNetworkEvent_t event = { 0 };
    IotNMNetwork_t * pNetwork = NULL;
    bool stateChange = false;

    for( ; ; )
    {
        status = xQueueReceive( networkManager.eventQueue, &event, portMAX_DELAY );

        if( status == pdTRUE )
        {
            stateChange = false;
            pNetwork = prvGetNetworkInstance( event.networkType );
            configASSERT( pNetwork != NULL );

            xSemaphoreTake( networkManager.globalMutex, portMAX_DELAY );

            if( ( pNetwork->state == eNetworkStateEnabled ) && ( event.eventType == IOT_NETWORK_EVENT_CONNECTED ) )
            {
                pNetwork->state = eNetworkStateConnected;
                stateChange = true;
            }
            else if( ( pNetwork->state == eNetworkStateConnected ) && ( event.eventType == IOT_NETWORK_EVENT_DISCONNECTED ) )
            {
                pNetwork->state = eNetworkStateEnabled;
                stateChange = true;
            }

            xSemaphoreGive( networkManager.globalMutex );

            if( stateChange )
            {
                xSemaphoreTake( networkManager.subscriptionsMutex, portMAX_DELAY );
                prvDispatch( event.networkType, pNetwork->state );
                xSemaphoreGive( networkManager.subscriptionsMutex );
            }
        }
    }
}


/*-----------------------------------------------------------*/

static void _initializeTCPIPCredentials( void )
{
    #if TCPIP_NETWORK_ENABLED
        tcpIPConnectionParams.pHostName = clientcredentialMQTT_BROKER_ENDPOINT;
        tcpIPConnectionParams.port = clientcredentialMQTT_BROKER_PORT;

        if( tcpIPConnectionParams.port == 443 )
        {
            tcpIPCredentials.pAlpnProtos = socketsAWS_IOT_ALPN_MQTT;
        }
        else
        {
            tcpIPCredentials.pAlpnProtos = NULL;
        }

        tcpIPCredentials.maxFragmentLength = 0;
        tcpIPCredentials.disableSni = false;
        tcpIPCredentials.pRootCa = NULL;
        tcpIPCredentials.rootCaSize = 0;
        tcpIPCredentials.pClientCert = keyCLIENT_CERTIFICATE_PEM;
        tcpIPCredentials.clientCertSize = sizeof( keyCLIENT_CERTIFICATE_PEM );
        tcpIPCredentials.pPrivateKey = keyCLIENT_PRIVATE_KEY_PEM;
        tcpIPCredentials.privateKeySize = sizeof( keyCLIENT_PRIVATE_KEY_PEM );
    #endif /* if TCPIP_NETWORK_ENABLED */
}

/*-----------------------------------------------------------*/

BaseType_t AwsIotNetworkManager_Init( void )
{
    BaseType_t status = pdTRUE;
    static bool isInit = false;

    if( !isInit )
    {
        isInit = true;

        memset( &networkManager, 0x00, sizeof( IotNetworkManagerInfo_t ) );

        networkManager.globalMutex = xSemaphoreCreateMutex();

        if( networkManager.globalMutex == NULL )
        {
            IotLogError( "Failed to create global mutex for network manager." );
            status = pdFALSE;
        }

        if( status == pdTRUE )
        {
            networkManager.subscriptionsMutex = xSemaphoreCreateMutex();

            if( networkManager.subscriptionsMutex == NULL )
            {
                IotLogError( "Failed to create subscription mutex for network manager." );
                status = pdFALSE;
            }
        }

        if( status == pdTRUE )
        {
            networkManager.eventQueue = xQueueCreate( NETWORK_MANAGER_EVENT_QUEUE_SIZE, sizeof( IotNetworkEvent_t ) );

            if( networkManager.eventQueue == NULL )
            {
                IotLogError( "Failed to create event queue for network manager." );
                status = pdFALSE;
            }
        }

        if( status == pdTRUE )
        {
            if( ( status = xTaskCreate( prvNetworkManagerTask,
                                        "NetworkManager",
                                        NETWORK_MANAGER_TASK_STACK_SIZE,
                                        NULL,
                                        NETWORK_MANAGER_TASK_PRIORITY,
                                        NULL ) ) != pdPASS )
            {
                IotLogError( "Failed to create network manager task." );
            }
        }

        if( status == pdTRUE )
        {
            networkManager.pNetworks = networks;
            networkManager.numNetworks = sizeof( networks ) / sizeof( IotNMNetwork_t );
            _initializeTCPIPCredentials();
        }
    }

    return status;
}

BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t networkTypes,
                                                         AwsIotNetworkStateChangeCb_t callback,
                                                         void * pContext,
                                                         IotNetworkManagerSubscription_t * pHandle )
{
    BaseType_t ret = pdFALSE;
    size_t index;
    IotNMSubscription_t * pSubscription = NULL;

    xSemaphoreTake( networkManager.subscriptionsMutex, portMAX_DELAY );

    for( index = 0; index < NETWORK_MANAGER_MAX_SUBSCRIPTIONS; index++ )
    {
        pSubscription = &networkManager.subscriptions[ index ];

        if( pSubscription->isActive == pdFALSE )
        {
            pSubscription->isActive = pdTRUE;
            pSubscription->networkTypes = networkTypes;
            pSubscription->callback = callback;
            pSubscription->pContext = pContext;
            ( *pHandle ) = ( IotNetworkManagerSubscription_t ) pSubscription;
            ret = pdTRUE;
            break;
        }
    }

    xSemaphoreGive( networkManager.subscriptionsMutex );

    if( ret == pdFALSE )
    {
        IotLogError( "Not enough memory to store new subscription" );
    }

    return ret;
}

BaseType_t AwsIotNetworkManager_RemoveSubscription( IotNetworkManagerSubscription_t handle )
{
    BaseType_t ret = pdFALSE;
    size_t index;
    IotNMSubscription_t * pSubscription = NULL;

    xSemaphoreTake( networkManager.subscriptionsMutex, portMAX_DELAY );

    for( index = 0; index < NETWORK_MANAGER_MAX_SUBSCRIPTIONS; index++ )
    {
        pSubscription = &networkManager.subscriptions[ index ];

        if( pSubscription == ( IotNMSubscription_t * ) handle )
        {
            memset( pSubscription, 0x00, sizeof( IotNMSubscription_t ) );
            ret = pdTRUE;
            break;
        }
    }

    xSemaphoreGive( networkManager.subscriptionsMutex );

    if( ret == pdFALSE )
    {
        IotLogError( "Cannot find the subscription to be removed." );
    }

    return ret;
}

uint32_t AwsIotNetworkManager_GetConfiguredNetworks( void )
{
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        networks |= networkManager.pNetworks[ index ].type;
    }

    return networks;
}

uint32_t AwsIotNetworkManager_GetEnabledNetworks( void )
{
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].state == eNetworkStateEnabled )
        {
            networks |= networkManager.pNetworks[ index ].type;
        }
    }

    return networks;
}

uint32_t AwsIotNetworkManager_GetConnectedNetworks( void )
{
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].state == eNetworkStateConnected )
        {
            networks |= networkManager.pNetworks[ index ].type;
        }
    }

    return networks;
}

uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t networkTypes )
{
    uint32_t enabled = AWSIOT_NETWORK_TYPE_NONE;

    xSemaphoreTake( networkManager.globalMutex, portMAX_DELAY );

    #if BLE_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
        {
            if( _bleEnable() == true )
            {
                enabled |= AWSIOT_NETWORK_TYPE_BLE;
            }
        }
    #endif

    #if WIFI_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
        {
            if( _wifiEnable() == true )
            {
                enabled |= AWSIOT_NETWORK_TYPE_WIFI;
            }
        }
    #endif

    #if ETH_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH )
        {
            if( _ethEnable() == true )
            {
                enabled |= AWSIOT_NETWORK_TYPE_ETH;
            }
        }
    #endif

    xSemaphoreGive( networkManager.globalMutex );

    return enabled;
}

uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t networkTypes )
{
    uint32_t disabled = AWSIOT_NETWORK_TYPE_NONE;

    /* Unused parameter when no networks enabled */
    ( void ) networkTypes;

    xSemaphoreTake( networkManager.globalMutex, portMAX_DELAY );

    #if WIFI_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
        {
            if( _wifiDisable() == true )
            {
                disabled |= AWSIOT_NETWORK_TYPE_WIFI;
            }
        }
    #endif

    #if BLE_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
        {
            if( _bleDisable() == true )
            {
                disabled |= AWSIOT_NETWORK_TYPE_BLE;
            }
        }
    #endif

    #if ETH_ENABLED
        if( ( networkTypes & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH )
        {
            if( _ethDisable() == true )
            {
                disabled |= AWSIOT_NETWORK_TYPE_ETH;
            }
        }
    #endif

    xSemaphoreGive( networkManager.globalMutex );


    return disabled;
}

const IotNetworkInterface_t * AwsIotNetworkManager_GetNetworkInterface( uint32_t networkType )
{
    const IotNetworkInterface_t * pInterface = NULL;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].type == networkType )
        {
            pInterface = networkManager.pNetworks[ index ].pNetworkInterface;
        }
    }

    return pInterface;
}

void * AwsIotNetworkManager_GetCredentials( uint32_t networkType )
{
    void * pCredentials = NULL;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].type == networkType )
        {
            pCredentials = networkManager.pNetworks[ index ].pCredentials;
        }
    }

    return pCredentials;
}

void * AwsIotNetworkManager_GetConnectionParams( uint32_t networkType )
{
    void * pConnectionParams = NULL;
    size_t index;

    for( index = 0; index < networkManager.numNetworks; index++ )
    {
        if( networkManager.pNetworks[ index ].type == networkType )
        {
            pConnectionParams = networkManager.pNetworks[ index ].pConnectionParams;
        }
    }

    return pConnectionParams;
}
