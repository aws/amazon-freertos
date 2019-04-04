/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#include "semphr.h"

#include "iot_demo_logging.h"
#include "iot_network_manager_private.h"

#include "iot_linear_containers.h"
#include "iot_taskpool.h"

/** Platform level includes **/
#include "platform/iot_threads.h"
#include "platform/iot_network_ble.h"
#include "platform/iot_network_afr.h"

#if BLE_ENABLED

#include "iot_ble_config.h"
#include "iot_ble.h"
#include <iot_ble_numericComparison.h>
#endif

#if WIFI_ENABLED
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#endif

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
#include "iot_ble_wifi_provisioning.h"
#include "aws_wifi_connect_task.h"
#endif

#define _NM_PARAMS( networkType, networkState )           ( ( ( uint32_t ) networkType ) <<  16 | ( ( uint16_t ) networkState ) )

#define _NM_GET_NETWORK_TYPE( params )                    ( ( uint32_t ) ( ( params ) >>  16 ) & 0x0000FFFFUL )

#define _NM_GET_NETWORK_STATE( params )                   ( ( AwsIotNetworkState_t ) ( ( params ) & 0x0000FFFFUL ) )

/**
 * @brief Structure wraps the type and state of each networks enabled.
 */
typedef struct IotNMNetwork
{
    IotLink_t link;
    uint32_t type;
    AwsIotNetworkState_t state;
    const IotNetworkInterface_t *networkInterface;
} IotNMNetwork_t;

typedef struct IotNMSubscription
{
    IotLink_t link;
    uint32_t networkTypes;
    AwsIotNetworkStateChangeCb_t callback;
    void *pContext;
} IotNMSubscription_t;

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

#endif


#if WIFI_ENABLED

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
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

#endif

/**
 *  @brief Invoked on state changes for each of the network.
 */
static void _onNetworkStateChangeCallback( uint32_t networkType, AwsIotNetworkState_t newState );

/**
 * @brief Taskpool routine to schedule user subscriptions for network state cnhanges. 
 */
static void _dispatchNetworkStateChangeCB( struct IotTaskPool * pTaskPool, struct IotTaskPoolJob * pJob, void * pUserContext );


#if BLE_ENABLED
static IotNMNetwork_t bleNetwork =
{
    .type            = AWSIOT_NETWORK_TYPE_BLE,
    .link            = IOT_LINK_INITIALIZER,
    .state           = eNetworkStateUnknown,
    .networkInterface = IOT_NETWORK_INTERFACE_BLE
};
#endif

#if WIFI_ENABLED
IotNMNetwork_t wifiNetwork =
{ 
    .type            = AWSIOT_NETWORK_TYPE_WIFI,
    .link            = IOT_LINK_INITIALIZER,
    .state           = eNetworkStateUnknown,
    .networkInterface = IOT_NETWORK_INTERFACE_AFR
};
#endif 

static IotNetworkManager_t networkManager =
{
    .networks              = IOT_LIST_DOUBLE_INITIALIZER,
    .subscriptions         = IOT_LIST_DOUBLE_INITIALIZER,
    .pendingInvocations    = IOT_LIST_DOUBLE_INITIALIZER,
    .isInvocationActive    = false
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
		if(unRegister == false)
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
#endif

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

    if( bleInited == false )
    {
        if( IotBle_Init() == eBTStatusSuccess )
        {
             bleInited = true;
        }
        else
        {
            ret = false;
        }
    }
    else
    {
        status = IotBle_On();    
  
        if( status == eBTStatusSuccess )
        {
            status = IotBle_StartAdv( NULL );
        }
        if( status != eBTStatusSuccess )
        {
            ret = false;
        }
    }

    if( ret == true )
    {
        /* Register BLE Connection callback */
        ret = _bleRegisterUnregisterCb(false);
    }
 
    return ret;
}

/*-----------------------------------------------------------*/
/* TODO make same function to registere/unregister or risk of memory leak. */
static bool _bleDisable( void )
{
    bool ret = true;

    /* Unregister the callbacks */
    ret =  _bleRegisterUnregisterCb( true );

    if( ret == true )
    {
        if( IotBle_StopAdv() != eBTStatusSuccess )
        {
            ret = false;
        }
    }

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

static void _bleConnectionCallback(BTStatus_t status,
                                   uint16_t connectionID,
                                   bool isConnected,
                                   BTBdaddr_t *pRemoteAddress)
{

    AwsIotNetworkState_t newState;

    if( isConnected == true )
    {
        IotLogInfo ( "BLE Connected to remote device, connId = %d\n", connectionID );
        IotBle_StopAdv();
        newState = eNetworkStateEnabled;
    }
    else
    {
        IotLogInfo ( "BLE disconnected with remote device, connId = %d \n", connectionID );
        ( void ) IotBle_StartAdv( NULL );
        newState = eNetworkStateDisabled;
    }
    _onNetworkStateChangeCallback( AWSIOT_NETWORK_TYPE_BLE, newState );
}


#endif
/*-----------------------------------------------------------*/
#if WIFI_ENABLED

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )

static bool _wifiConnectAccessPoint( void )
{

    bool ret = true;
    static const WIFINetworkParams_t network =
    {
        .pcSSID           = clientcredentialWIFI_SSID;
        .ucSSIDLength     = sizeof(clientcredentialWIFI_SSID);
        .pcPassword       = clientcredentialWIFI_PASSWORD;
        .ucPasswordLength = sizeof(clientcredentialWIFI_PASSWORD);
        .xSecurity        = clientcredentialWIFI_SECURITY;
    };

    /* Setup parameters. */
    if( WIFI_ConnectAP( &( network ) ) != eWiFiSuccess )
    {
        ret = false;
    }

    return ret;
}

#endif


static bool _wifiEnable( void )
{
    bool ret = true;

    if( WIFI_On() != eWiFiSuccess )
    {
        ret = false;
    }

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
    if( ret == true )
    {
        ret = _wifiConnectAccessPoint();
    }
#else
    if ( ret == true )
    {
        if( IotBleWifiProv_Start() != pdTRUE )
        {
            ret = false;
        }
    }

    if( ret == true )
    {
      if(  xWiFiConnectTaskInitialize() != pdTRUE )
      {
          ret = false;
      }

    }
#endif

    return ret;
}

static bool _wifiDisable( void )
{
    bool ret = true;

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    
    vWiFiConnectTaskDestroy();

#endif
    if( WIFI_IsConnected() == pdTRUE )
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

#endif

/*-----------------------------------------------------------*/


static void _dispatchNetworkStateChangeCB( struct IotTaskPool * pTaskPool, struct IotTaskPoolJob * pJob, void * pUserContext )
{
    IotLink_t *pLink;
    IotTaskPoolJob_t* pPendingJob;
    IotTaskPoolError_t error;
    IotNMSubscription_t* pSubscription;
    uint32_t networkType              = _NM_GET_NETWORK_TYPE( ( uint32_t ) pUserContext );
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
    if (pLink != NULL)
    {
        pPendingJob = IotLink_Container(IotTaskPoolJob_t, pLink, link);
        
        error = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, pPendingJob, 0 );
        if( error != IOT_TASKPOOL_SUCCESS )
        {
            IotLogError( "Failed to schedule a taskpool job, discarding all pending items in queue " );

            ( void ) IotTaskPool_RecycleJob( pTaskPool, pPendingJob );
            IotContainers_ForEach( &networkManager.pendingInvocations, pLink )
            {
                pPendingJob = IotLink_Container( IotTaskPoolJob_t, pLink, link );
                ( void ) IotTaskPool_RecycleJob( pTaskPool, pPendingJob );
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
    IotTaskPool_RecycleJob( pTaskPool, pJob );

}

static void _onNetworkStateChangeCallback( uint32_t networkType, AwsIotNetworkState_t newState )
{
    IotTaskPoolJob_t *pJob;
    IotNMNetwork_t* pNetwork = NULL;
    IotLink_t *pLink;
    IotTaskPoolError_t error;

    /*  'networks' is an immutable list created/modified only during network manager intialization */
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
                                                  &pJob );
        if( error == IOT_TASKPOOL_SUCCESS )
        {
            if( networkManager.isInvocationActive )
            {
                IotListDouble_InsertTail( &networkManager.pendingInvocations, &pJob->link );
            }
            else
            {
                error = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, pJob, 0 );
                
                if( error == IOT_TASKPOOL_SUCCESS )
                {
                    networkManager.isInvocationActive = true;
                }
                else
                {
                    IotLogError("Failed to invoke subscription task for network: %d, state: %d, error:",
                                pNetwork->type,
                                newState,
                                error);
                    
                    /* Recycle the job if schedule failed */
                    IotTaskPool_RecycleJob( IOT_SYSTEM_TASKPOOL, pJob );
                }
            }
        }
        else
        {
            IotLogError("Failed to create subscription task for network: %d, state: %d, error:",
                        pNetwork->type,
                        newState,
                        error);
        }
        
    }

    IotMutex_Unlock( &networkManager.nmLock );
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
           
           /* One time registration of network event callback with the Wi-Fi driver */
           if( WIFI_RegisterNetworkStateChangeEventCallback( _onNetworkStateChangeCallback ) != eWiFiSuccess )
           {
                error = pdFALSE;
           } 
#endif
#if BLE_ENABLED
            IotListDouble_InsertTail( &networkManager.networks, &bleNetwork.link );
            /* Registration of network event callback for BLE is handled within _bleEnable() */
#endif
         }
    }

    return error;
}

BaseType_t AwsIotNetworkManager_SubscribeForStateChange(uint32_t networkTypes,
                                                        AwsIotNetworkStateChangeCb_t callback,
                                                        void *pContext,
                                                        IotNetworkManagerSubscription_t *pHandle)
{
    BaseType_t ret = pdFALSE;
    IotNMSubscription_t* pSubscription = pvPortMalloc( sizeof( IotNMSubscription_t ) );

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
        IotLogError(" Not enough memory to add new subscription");
    }
   
    return ret;
}

BaseType_t AwsIotNetworkManager_RemoveSubscription(  IotNetworkManagerSubscription_t handle )
{
    BaseType_t ret = pdFALSE;
    IotNMSubscription_t * pSubscription = ( IotNMSubscription_t * ) handle; 
    IotNMSubscription_t * pItem;
    IotLink_t* pLink;

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

    IotNMNetwork_t* pNetwork;
    IotLink_t* pLink;
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
    IotNMNetwork_t *pNetwork;
    IotLink_t* pLink;
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
    IotNMNetwork_t *pNetwork;
    IotLink_t* pLink;
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
    IotMutex_Unlock(&networkManager.nmLock);

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

    return enabled;
}

uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t networkTypes )
{
    uint32_t disabled = AWSIOT_NETWORK_TYPE_NONE;

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
    IotNMNetwork_t *pNetwork;
    IotLink_t* pLink;
    const IotNetworkInterface_t *pInterface = NULL;
    
    IotContainers_ForEach( &networkManager.networks, pLink )
    {
        pNetwork = IotLink_Container( IotNMNetwork_t, pLink, link );
        if( pNetwork->type == networkType )
        {
            pInterface = pNetwork->networkInterface;
        }
    }

    return pInterface;
}