/*
 * FreeRTOS V202012.00
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
 * @file aws_iot_network_manager.c
 * @brief Network manager is used to handled different types of network connections and their connection/disconnection events at the application layer.
 */
#include <string.h>

#include "FreeRTOS.h"

#include "iot_demo_logging.h"
#include "iot_network_manager_private.h"

#include "iot_linear_containers.h"
#include "iot_taskpool.h"

/** Platform level includes **/
#include "platform/iot_threads.h"
#include "platform/iot_clock.h"

#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"

#if BLE_ENABLED
    #include "iot_ble_config.h"
    #include "iot_ble.h"
    #include "iot_ble_numericComparison.h"
    #include "platform/iot_network_ble.h"
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
 * @brief Structure holds information for each network and the runtime state of it.
 */
typedef struct IotNMNetwork
{
    IotLink_t link;
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
    IotLink_t link;
    uint32_t networkTypes;
    AwsIotNetworkStateChangeCb_t callback;
    void * pContext;
} IotNMSubscription_t;

/**
 * @brief Network manager object, holds the list of supported networks and the list of network state change subscriptions.
 */
typedef struct IotNetworkManager
{
    IotListDouble_t subscriptions;
    IotListDouble_t networks;
    IotListDouble_t pendingInvocations;
    IotMutex_t nmLock;
    IotMutex_t subscriptionsLock;
    bool isInvocationActive;
} IotNetworkManager_t;

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

/**
 *  @brief Invoked on state changes for each of the network.
 */
static void _onNetworkStateChangeCallback( uint32_t networkType,
                                           AwsIotNetworkState_t newState );

/**
 * @brief Taskpool routine to schedule user subscriptions for network state changes.
 */
static void _dispatchNetworkStateChangeCB( IotTaskPool_t taskPool,
                                           IotTaskPoolJob_t job,
                                           void * pUserContext );

/**
 * @brief Initialize TCP/IP credentials for WIFI and ethernet networks.
 */
static void _initializeTCPIPCredentials( void );

#if BLE_ENABLED

/**
 * @brief Configuration and state for a BLE Network.
 */
    static IotNMNetwork_t bleNetwork =
    {
        .type              = AWSIOT_NETWORK_TYPE_BLE,
        .link              = IOT_LINK_INITIALIZER,
        .state             = eNetworkStateUnknown,
        .pNetworkInterface = IOT_NETWORK_INTERFACE_BLE,
        .pCredentials      = NULL,
        .pConnectionParams = NULL
    };
#endif /* if BLE_ENABLED */

/**
 * @brief The credentials placeholder for a TCP/IP network.
 *
 */
static IotNetworkCredentials_t tcpIPCredentials = { 0 };

/*
 * @brief Connection parameters placeholder for a TCP/IP network.
 */
static IotNetworkServerInfo_t tcpIPConnectionParams = { 0 };

#if WIFI_ENABLED
    static IotNMNetwork_t wifiNetwork =
    {
        .type              = AWSIOT_NETWORK_TYPE_WIFI,
        .link              = IOT_LINK_INITIALIZER,
        .state             = eNetworkStateUnknown,
        .pNetworkInterface = IOT_NETWORK_INTERFACE_AFR,
        .pCredentials      = &tcpIPCredentials,
        .pConnectionParams = &tcpIPConnectionParams
    };
#endif

#if ETH_ENABLED
    static IotNMNetwork_t ethNetwork =
    {
        .type              = AWSIOT_NETWORK_TYPE_ETH,
        .link              = IOT_LINK_INITIALIZER,
        .state             = eNetworkStateUnknown,
        .pNetworkInterface = IOT_NETWORK_INTERFACE_AFR,
        .pCredentials      = &tcpIPCredentials,
        .pConnectionParams = &tcpIPConnectionParams
    };
#endif


static IotNetworkManager_t networkManager =
{
    .networks           = IOT_LIST_DOUBLE_INITIALIZER,
    .subscriptions      = IOT_LIST_DOUBLE_INITIALIZER,
    .pendingInvocations = IOT_LIST_DOUBLE_INITIALIZER,
    .isInvocationActive = false
};

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
            eventCallback.pGAPPairingStateChangedCb = &BLEGAPPairingStateChangedCb;

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
                eventCallback.pNumericComparisonCb = &BLENumericComparisonCb;

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
        bool ret = true;
        static bool bleInited = false;
        BTStatus_t status;

        bleNetwork.state = eNetworkStateDisabled;

        if( bleInited == false )
        {
            if( IotBle_Init() == eBTStatusSuccess )
            {
                bleInited = true;
            }
            else
            {
                IotLogError( "Failed to initialize BLE." );
                ret = false;
            }
        }

        if( ret == true )
        {
            status = IotBle_On();

            if( status != eBTStatusSuccess )
            {
                IotLogError( "Failed to toggle BLE on." );
                ret = false;
            }
        }

        if( ret == true )
        {
            /* Register BLE Connection callback */
            ret = _bleRegisterUnregisterCb( false );
        }

        if( ret == false )
        {
            bleNetwork.state = eNetworkStateUnknown;
        }

        return ret;
    }

/*-----------------------------------------------------------*/
/* TODO make same function to register/unregister or risk of memory leak. */
    static bool _bleDisable( void )
    {
        bool ret = true;

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

        return ret;
    }

    static void _bleConnectionCallback( BTStatus_t status,
                                        uint16_t connectionID,
                                        bool isConnected,
                                        BTBdaddr_t * pRemoteAddress )
    {
        AwsIotNetworkState_t newState;

        if( isConnected == true )
        {
            IotLogInfo( "BLE Connected to remote device, connId = %d\n", connectionID );
            IotBle_StopAdv( NULL );
            newState = eNetworkStateEnabled;
        }
        else
        {
            IotLogInfo( "BLE disconnected with remote device, connId = %d \n", connectionID );
            ( void ) IotBle_StartAdv( NULL );
            newState = eNetworkStateDisabled;
        }

        _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_BLE, newState );
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

            if( xSecurity == eWiFiSecurityWPA2 )
            {
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
            }

            if( status == true )
            {
                /* Try to connect to wifi access point with retry and exponential delay */
                do
                {
                    if( WIFI_ConnectAP( &( xConnectParams ) ) == eWiFiSuccess )
                    {
                        wifiNetwork.state = eNetworkStateEnabled;
                        break;
                    }
                    else
                    {
                        if( numRetries > 0 )
                        {
                            IotClock_SleepMs( delayMilliseconds );
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

        if( WIFI_On() != eWiFiSuccess )
        {
            ret = false;
        }

        if( ret == true )
        {
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
            }
        #else
            if( ret == true )
            {
                #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
                    if( IotBleWifiProv_Init() != pdTRUE )
                    {
                        ret = false;
                    }
                #endif
                #if ( IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 1 )
                    if( IotWifiSoftAPProv_Init() != pdTRUE )
                    {
                        ret = false;
                    }
                #endif
            }

            if( ret == true )
            {
                if( xWiFiConnectTaskInitialize() != pdTRUE )
                {
                    ret = false;
                }
            }
        #endif /* if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 && IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 0 ) */

        return ret;
    }

    static bool _wifiDisable( void )
    {
        bool ret = true;

        #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
            vWiFiConnectTaskDestroy();
            IotBleWifiProv_Deinit();
        #endif
        #if ( IOT_WIFI_ENABLE_SOFTAP_PROVISIONING == 1 )
            vWiFiConnectTaskDestroy();
            IotWifiSoftAPProv_Deinit();
        #endif

        if( WIFI_IsConnected( NULL ) == pdTRUE )
        {
            if( WIFI_Disconnect() != eWiFiSuccess )
            {
                ret = false;
            }
        }

        if( ret == true )
        {
            if( WIFI_Off() != eWiFiSuccess )
            {
                ret = false;
            }
        }

        return ret;
    }

    static void _wifiEventHandler( WIFIEvent_t * pxEvent )
    {
        uint8_t * pucIpAddr;

        if( pxEvent->xEventType == eWiFiEventIPReady )
        {
            pucIpAddr = ( uint8_t * ) ( &pxEvent->xInfo.xIPReady.xIPAddress.ulAddress[ 0 ] );
            IotLogInfo( "Connected to WiFi access point, ip address: %d.%d.%d.%d.", pucIpAddr[ 0 ], pucIpAddr[ 1 ], pucIpAddr[ 2 ], pucIpAddr[ 3 ] );
            _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_WIFI, eNetworkStateEnabled );
        }
        else if( pxEvent->xEventType == eWiFiEventDisconnected )
        {
            IotLogInfo( "Disconnected from WiFi access point, reason code: %d.", pxEvent->xInfo.xDisconnected.xReason );
            _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_WIFI, eNetworkStateDisabled );
        }
    }

#endif /* if WIFI_ENABLED */

/*-----------------------------------------------------------*/


static void _dispatchNetworkStateChangeCB( IotTaskPool_t taskPool,
                                           IotTaskPoolJob_t job,
                                           void * pUserContext )
{
    IotLink_t * pLink;
    IotTaskPoolJob_t pendingJob;
    IotTaskPoolError_t error;
    IotNMSubscription_t * pSubscription;
    uint32_t networkType = _NM_GET_NETWORK_TYPE( ( uint32_t ) pUserContext );
    AwsIotNetworkState_t networkState = _NM_GET_NETWORK_STATE( ( uint32_t ) pUserContext );

    IotMutex_Lock( &networkManager.subscriptionsLock );
    IotContainers_ForEach( &networkManager.subscriptions, pLink )
    {
        pSubscription = IotLink_Container( IotNMSubscription_t, pLink, link );

        if( ( pSubscription->networkTypes & networkType ) == networkType )
        {
            pSubscription->callback( networkType, networkState, pSubscription->pContext );
        }
    }
    IotMutex_Unlock( &networkManager.subscriptionsLock );


    IotMutex_Lock( &networkManager.nmLock );
    pLink = IotListDouble_RemoveHead( &networkManager.pendingInvocations );

    if( pLink != NULL )
    {
        /* We should not cast a IotTaskPoolJobStorage_t type to a IotTaskPoolJob_t the relationship
         * between storage and handle is implementaton dependent */
        pendingJob = ( IotTaskPoolJob_t ) IotLink_Container( IotTaskPoolJobStorage_t, pLink, link );

        error = IotTaskPool_Schedule( taskPool, pendingJob, 0 );

        if( error != IOT_TASKPOOL_SUCCESS )
        {
            IotLogError( "Failed to schedule a taskpool job, discarding all pending items in queue " );

            ( void ) IotTaskPool_RecycleJob( taskPool, pendingJob );
            IotContainers_ForEach( &networkManager.pendingInvocations, pLink )
            {
                /* We should not cast a IotTaskPoolJobStorage_t type to a IotTaskPoolJob_t the relationship
                 * between storage and handle is implementaton dependent */
                pendingJob = ( IotTaskPoolJob_t ) IotLink_Container( IotTaskPoolJobStorage_t, pLink, link );
                ( void ) IotTaskPool_RecycleJob( taskPool, pendingJob );
                networkManager.isInvocationActive = false;
            }
        }
    }
    else
    {
        networkManager.isInvocationActive = false;
    }

    IotMutex_Unlock( &networkManager.nmLock );

    /* Recycle current job */
    IotTaskPool_RecycleJob( taskPool, job );
}

static void _onNetworkStateChangeCallback( uint32_t networkType,
                                           AwsIotNetworkState_t newState )
{
    IotTaskPoolJob_t job;
    IotTaskPoolJobStorage_t * jobStorage;
    IotNMNetwork_t * pNetwork = NULL;
    IotLink_t * pLink;
    IotTaskPoolError_t error;

    /*  'networks' is an immutable list created/modified only during network manager initialization */
    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->type == networkType )
        {
            break;
        }
    }

    IotMutex_Lock( &networkManager.nmLock );

    if( pNetwork->state != newState )
    {
        pNetwork->state = newState;
        error = IotTaskPool_CreateRecyclableJob( IOT_SYSTEM_TASKPOOL,
                                                 _dispatchNetworkStateChangeCB,
                                                 ( void * ) _NM_PARAMS( networkType, newState ),
                                                 &job );

        if( error == IOT_TASKPOOL_SUCCESS )
        {
            if( networkManager.isInvocationActive )
            {
                jobStorage = IotTaskPool_GetJobStorageFromHandle( job );

                IotListDouble_InsertTail( &networkManager.pendingInvocations, &jobStorage->link );
            }
            else
            {
                error = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, job, 0 );

                if( error == IOT_TASKPOOL_SUCCESS )
                {
                    networkManager.isInvocationActive = true;
                }
                else
                {
                    IotLogError( "Failed to invoke subscription task for network: %d, state: %d, error: %d",
                                 pNetwork->type,
                                 newState,
                                 error );

                    /* Recycle the job if schedule failed */
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, job );
                }
            }
        }
        else
        {
            IotLogError( "Failed to create subscription task for network: %d, state: %d, error: %d",
                         pNetwork->type,
                         newState,
                         error );
        }
    }

    IotMutex_Unlock( &networkManager.nmLock );
}

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
    BaseType_t error = pdTRUE;
    static bool isInit = false;

    if( !isInit )
    {
        isInit = true;

        if( !IotMutex_Create( &networkManager.nmLock, false ) )
        {
            error = pdFALSE;
        }

        if( error == pdTRUE )
        {
            if( !IotMutex_Create( &networkManager.subscriptionsLock, false ) )
            {
                error = pdFALSE;
            }
        }

        if( error == pdTRUE )
        {
            IotListDouble_Create( &networkManager.subscriptions );
            IotListDouble_Create( &networkManager.networks );
            IotListDouble_Create( &networkManager.pendingInvocations );

            #if WIFI_ENABLED
                IotListDouble_InsertTail( &networkManager.networks, &wifiNetwork.link );
            #endif /* if WIFI_ENABLED */

            #if ETH_ENABLED
                IotListDouble_InsertTail( &networkManager.networks, &ethNetwork.link );
            #endif

            #if BLE_ENABLED
                IotListDouble_InsertTail( &networkManager.networks, &bleNetwork.link );
                /* Registration of network event callback for BLE is handled within _bleEnable() */
            #endif

            _initializeTCPIPCredentials();
        }
    }

    return error;
}

BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t networkTypes,
                                                         AwsIotNetworkStateChangeCb_t callback,
                                                         void * pContext,
                                                         IotNetworkManagerSubscription_t * pHandle )
{
    BaseType_t ret = pdFALSE;
    IotNMSubscription_t * pSubscription = pvPortMalloc( sizeof( IotNMSubscription_t ) );

    if( pSubscription != NULL )
    {
        pSubscription->networkTypes = networkTypes;
        pSubscription->callback = callback;
        pSubscription->pContext = pContext;

        IotMutex_Lock( &networkManager.subscriptionsLock );
        IotListDouble_InsertTail( &networkManager.subscriptions, &pSubscription->link );
        IotMutex_Unlock( &networkManager.subscriptionsLock );

        *pHandle = ( IotNetworkManagerSubscription_t ) pSubscription;

        ret = pdTRUE;
    }
    else
    {
        IotLogError( " Not enough memory to add new subscription" );
    }

    return ret;
}

BaseType_t AwsIotNetworkManager_RemoveSubscription( IotNetworkManagerSubscription_t handle )
{
    BaseType_t ret = pdFALSE;
    IotNMSubscription_t * pSubscription = ( IotNMSubscription_t * ) handle;
    IotNMSubscription_t * pItem;
    IotLink_t * pLink;

    if( pSubscription != NULL )
    {
        IotMutex_Lock( &networkManager.subscriptionsLock );
        IotContainers_ForEach( &networkManager.subscriptions, pLink )
        {
            pItem = IotLink_Container( IotNMSubscription_t, pLink, link );

            if( pItem == pSubscription )
            {
                IotListDouble_Remove( pLink );
                vPortFree( pItem );
                ret = pdTRUE;
                break;
            }
        }
        IotMutex_Unlock( &networkManager.subscriptionsLock );
    }

    return ret;
}

uint32_t AwsIotNetworkManager_GetConfiguredNetworks( void )
{
    IotNMNetwork_t * pNetwork;
    IotLink_t * pLink;
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;

    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );
        networks |= pNetwork->type;
    }

    return networks;
}

uint32_t AwsIotNetworkManager_GetEnabledNetworks( void )
{
    IotNMNetwork_t * pNetwork;
    IotLink_t * pLink;
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;

    IotMutex_Lock( &networkManager.nmLock );
    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->state != eNetworkStateUnknown )
        {
            networks |= pNetwork->type;
        }
    }
    IotMutex_Unlock( &networkManager.nmLock );

    return networks;
}

uint32_t AwsIotNetworkManager_GetConnectedNetworks( void )
{
    IotNMNetwork_t * pNetwork;
    IotLink_t * pLink;
    uint32_t networks = AWSIOT_NETWORK_TYPE_NONE;

    IotMutex_Lock( &networkManager.nmLock );
    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->state == eNetworkStateEnabled )
        {
            networks |= pNetwork->type;
        }
    }
    IotMutex_Unlock( &networkManager.nmLock );

    return networks;
}

uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t networkTypes )
{
    uint32_t enabled = AWSIOT_NETWORK_TYPE_NONE;

    #if BLE_ENABLED
        if( ( ( networkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE ) &&
            ( bleNetwork.state == eNetworkStateUnknown ) )
        {
            if( _bleEnable() == true )
            {
                enabled |= AWSIOT_NETWORK_TYPE_BLE;
            }
        }
    #endif

    #if WIFI_ENABLED
        if( ( ( networkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI ) &&
            ( wifiNetwork.state == eNetworkStateUnknown ) )
        {
            if( _wifiEnable() == true )
            {
                enabled |= AWSIOT_NETWORK_TYPE_WIFI;
            }
        }
    #endif

    #if ETH_ENABLED
        if( ( ( networkTypes & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH ) &&
            ( ethNetwork.state == eNetworkStateUnknown ) )
        {
            enabled |= AWSIOT_NETWORK_TYPE_ETH;
            ethNetwork.state = eNetworkStateEnabled;
        }
    #endif
    return enabled;
}

uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t networkTypes )
{
    uint32_t disabled = AWSIOT_NETWORK_TYPE_NONE;

    /* Unused parameter when no networks enabled */
    ( void ) networkTypes;

    #if WIFI_ENABLED
        if( ( ( networkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI ) &&
            ( wifiNetwork.state != eNetworkStateUnknown ) )
        {
            if( _wifiDisable() == true )
            {
                disabled |= AWSIOT_NETWORK_TYPE_WIFI;
                _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_WIFI, eNetworkStateUnknown );
            }
        }
    #endif

    #if BLE_ENABLED
        if( ( ( networkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE ) &&
            ( bleNetwork.state != eNetworkStateUnknown ) )
        {
            if( _bleDisable() == true )
            {
                disabled |= AWSIOT_NETWORK_TYPE_BLE;
                _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_BLE, eNetworkStateUnknown );
            }
        }
    #endif

    return disabled;
}

const IotNetworkInterface_t * AwsIotNetworkManager_GetNetworkInterface( uint32_t networkType )
{
    IotNMNetwork_t * pNetwork;
    IotLink_t * pLink;
    const IotNetworkInterface_t * pInterface = NULL;

    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->type == networkType )
        {
            pInterface = pNetwork->pNetworkInterface;
        }
    }

    return pInterface;
}

void * AwsIotNetworkManager_GetCredentials( uint32_t networkType )
{
    void * pCredentials = NULL;
    IotLink_t * pLink;
    IotNMNetwork_t * pNetwork;

    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->type == networkType )
        {
            pCredentials = pNetwork->pCredentials;
        }
    }

    return pCredentials;
}

void * AwsIotNetworkManager_GetConnectionParams( uint32_t networkType )
{
    void * pConnectionParams = NULL;
    IotLink_t * pLink;
    IotNMNetwork_t * pNetwork;

    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );

        if( pNetwork->type == networkType )
        {
            pConnectionParams = pNetwork->pConnectionParams;
        }
    }

    return pConnectionParams;
}
