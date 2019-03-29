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


#include "iot_demo_logging.h"
#include "iot_network_manager_private.h"

#include "iot_linear_containers.h"

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

#include "iot_taskpool.h"

/**
 * @brief Maximum number of concurrent network events which can be queued for a network type.
 */
#define _MAX_CONCURRENT_JOBS    ( 4 )

/**
 * @brief Initializer for WiFi Network Info structure.
 */
#define _NETWORK_INFO_WIFI            { .ulNetworkType = AWSIOT_NETWORK_TYPE_WIFI, .xNetworkList = IOT_LINK_INITIALIZER, .xNetworkState = eNetworkStateUnknown }

/**
 * @brief Initializer for BLE Network Info structure.
 */
#define _NETWORK_INFO_BLE             { .ulNetworkType = AWSIOT_NETWORK_TYPE_BLE, .xNetworkList = IOT_LINK_INITIALIZER, .xNetworkState = eNetworkStateUnknown }

/**
 * @brief User callback execution context used by task pool job.
 */
typedef struct NMCallbackExecutionContext
{
    IotTaskPoolJob_t xJob;
    void *pvUserContext;
    AwsIotNetworkState_t xNewtorkEvent;
    uint32_t ulNetworkType;
    struct NMCallbackExecutionContext * pxNextTask;

} NMCallbackExecutionContext_t;

/**
 *  @brief Structure used to store the User Subscription information.
 */
typedef struct NMSubscription
{
    IotLink_t xSubscriptionList;
    uint32_t ulNetworkTypes;
    AwsIotNetworkStateChangeCb_t xUserCallback;
    void *pvUserContext;
} NMSubscription_t;


/**
 * @brief Structure wraps the type and state of each networks enabled.
 */
typedef struct NMNetworkInfo
{
    IotLink_t xNetworkList;
    uint32_t ulNetworkType;
    AwsIotNetworkState_t xNetworkState;
} NMNetworkInfo_t;

/**
 * @brief Global structure contains the list of all enabled network and user subscriptions, task pool and
 * callback execution contexts.
 */
typedef struct NetworkManagerInfo
{
    SemaphoreHandle_t xSubscriptionLock;
    IotListDouble_t xSubscriptionListHead;
    IotListDouble_t xNetworkListHead;
    IotTaskPool_t* pxTaskPool;
    NMCallbackExecutionContext_t xJobs[ _MAX_CONCURRENT_JOBS ];
    bool xIsInit;
} NetworkManagerInfo_t;

#if BLE_ENABLED

NMNetworkInfo_t xBLENetworkInfo = _NETWORK_INFO_BLE;

/**
 * @brief Function used to enable a BLE network.
 *
 * @return true if BLE is enabled successfully.
 */
static BaseType_t prxBLEEnable( void );


/**
 * @brief Function used to disable a BLE network.
 *
 * @return true if BLE is disable successfully, false if already disabled
 */
static BaseType_t prxBLEDisable( void );


/**
 * @brief Callback invoked when a BLE network connects or disconnects.
 * @param xStatus[in] Whether the operation was successful or not.
 * @param usConnId[in] Connection id of the connection
 * @param xConnected[in] true if its connected, false if its disconnected.
 * @param pxBda[in] Address of the remote device connected/disconnected.
 */
static void prvBLEConnectionCallback( BTStatus_t xStatus,
                             uint16_t usConnId,
                             bool xConnected,
                             BTBdaddr_t * pxBda );


/**
 * @brief Callback invoked if advertisement started
 * @param xStatus[in] Status of the start advertisement operation.
 */

static void prvStartAdvCallback( BTStatus_t xStatus );


#endif


#if WIFI_ENABLED

NMNetworkInfo_t xWiFiNetworkInfo = _NETWORK_INFO_WIFI;

/**
 * @brief Function used to enable a WIFI network.
 *
 * @return true if WIFI is enabled successfully.
 */
static BaseType_t prxWIFIEnable( void );

/**
 * @brief Function used to disable a WIFI network.
 *
 * @return true if WIFI is disable successfully, false if already disabled
 */
static BaseType_t prxWIFIDisable( void );

/**
 * @Brief Callback registered by network manager with WIFI layer for  connection state changes.
 * @param ulNetworkType Network type passed from WIFI
 * @param xState State of the network
 * @param pvContext User context passed as it is to the callback
 */
static void prvWiFiEventCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState );

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
/**
 * Connects to the WIFI using credentials configured statically
 * @return true if connected successfully.
 */
static BaseType_t prxWifiConnect( void );

#endif
#endif

static void prvInvokeSubscription( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkEvent );

static void prvUserCallbackRoutine( struct IotTaskPool * pTaskPool, struct IotTaskPoolJob * pJob, void * pUserContext );


static NetworkManagerInfo_t xNetworkManagerInfo = { .xSubscriptionListHead = IOT_LIST_DOUBLE_INITIALIZER, .xNetworkListHead = IOT_LIST_DOUBLE_INITIALIZER };



#if BLE_ENABLED

BaseType_t _registerUnregisterCb(bool unRegister)
{
    IotBleEventsCallbacks_t xEventCb;
    BTStatus_t xStatus;
    BaseType_t ret = pdFALSE;

	xEventCb.pConnectionCb = prvBLEConnectionCallback;
    if(unRegister == false)
    {
		xStatus = IotBle_RegisterEventCb( eBLEConnection, xEventCb );
    }else
    {
		xStatus = IotBle_UnRegisterEventCb( eBLEConnection, xEventCb );
    }

	xEventCb.pGAPPairingStateChangedCb = &BLEGAPPairingStateChangedCb;
	if( xStatus == eBTStatusSuccess )
	{
		if(unRegister == false)
		{
			xStatus = IotBle_RegisterEventCb( eBLEPairingStateChanged, xEventCb );
		}else
		{
			xStatus = IotBle_UnRegisterEventCb( eBLEPairingStateChanged, xEventCb );
		}
	}

#if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
	xEventCb.pNumericComparisonCb = &BLENumericComparisonCb;
	if( xStatus == eBTStatusSuccess )
	{
		if(unRegister == false)
		{
			xStatus = IotBle_RegisterEventCb( eBLENumericComparisonCallback, xEventCb );
		}else
		{
			xStatus = IotBle_UnRegisterEventCb( eBLENumericComparisonCallback, xEventCb );
		}
	}
#endif

    if(xStatus == eBTStatusSuccess)
    {
    	ret = pdTRUE;
    }

    return ret;
}

static BaseType_t prxBLEEnable( void )
{
    BaseType_t xRet = pdTRUE;
    static bool bInitBLE = false;
    BTStatus_t xStatus;

    if( bInitBLE == false )
    {
        xStatus = IotBle_Init();

        if( xStatus == eBTStatusSuccess )
        {
            bInitBLE = true;
        }
    }
    else
    {
        xStatus = IotBle_On();
        IotBle_StartAdv(NULL);
    }
    /* Register BLE Connection callback */
    _registerUnregisterCb(false);

    if( xStatus != eBTStatusSuccess)
    {
    	xRet = pdFALSE;
    }

    return xRet;
}

/*-----------------------------------------------------------*/
/* TODO make same function to registere/unregister or risk of memory leak. */
static BaseType_t prxBLEDisable( void )
{
    bool xRet = true;

    xRet = _registerUnregisterCb(true);

    if( xRet == true )
    {
        if( IotBle_StopAdv() != eBTStatusSuccess )
        {
            xRet = false;
        }
    }

    if( xRet == true )
    {
        if( IotBle_Off() != eBTStatusSuccess )
        {
            xRet = false;
        }
    }

	return xRet;
}

static void prvStartAdvCallback( BTStatus_t xStatus )
{
    if( xStatus == eBTStatusSuccess )
    {
        IotLogInfo ( "Started advertisement. Listening for a BLE Connection.\n" );
    }
}

static void prvBLEConnectionCallback( BTStatus_t xStatus,
                             uint16_t connId,
                             bool xConnected,
                             BTBdaddr_t * pxBda )
{


    AwsIotNetworkState_t xPrevState = xBLENetworkInfo.xNetworkState;

    if( xConnected == true )
    {
        IotLogInfo ( "BLE Connected to remote device, connId = %d\n", connId );
        IotBle_StopAdv();
        xBLENetworkInfo.xNetworkState = eNetworkStateEnabled;
    }
    else
    {
        IotLogInfo ( "BLE disconnected with remote device, connId = %d \n", connId );

        if( xBLENetworkInfo.xNetworkState != eNetworkStateUnknown )
        {
            ( void ) IotBle_StartAdv( prvStartAdvCallback );
            xBLENetworkInfo.xNetworkState = eNetworkStateDisabled;
        }
    }

    if( xPrevState != xBLENetworkInfo.xNetworkState )
    {
        prvInvokeSubscription( xBLENetworkInfo.ulNetworkType, xBLENetworkInfo.xNetworkState  );
    }
}
#endif
/*-----------------------------------------------------------*/

#if WIFI_ENABLED
static void prvWiFiEventCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState )
{
    if( xState != xWiFiNetworkInfo.xNetworkState )
    {
        xWiFiNetworkInfo.xNetworkState = xState;
        prvInvokeSubscription( xWiFiNetworkInfo.ulNetworkType, xWiFiNetworkInfo.xNetworkState );
    }
}

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
static BaseType_t prxWifiConnect( void )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;

    /* Setup parameters. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;

    xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

    return ( xWifiStatus == eWiFiSuccess );
}
#endif


static BaseType_t prxWIFIEnable( void )
{
    BaseType_t xRet = pdFALSE;

    if( WIFI_On() == eWiFiSuccess )
    {
        xRet = pdTRUE;
    }

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
    if( xRet == pdTRUE )
    {
        xRet = prxWifiConnect();
    }
#else
    if ( xRet == pdTRUE )
    {
        xRet = IotBleWifiProv_Start();
    }
    if( xRet == pdTRUE )
    {
        xRet = xWiFiConnectTaskInitialize();
    }
#endif

    return xRet;
}

static BaseType_t prxWIFIDisable( void )
{
    BaseType_t xRet = pdFALSE;

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
    vWiFiConnectTaskDestroy();
#endif

    if( WIFI_IsConnected() == pdTRUE )
    {
        if( WIFI_Disconnect() == eWiFiSuccess )
        {
            xRet = true;
        }
    }
    if( xRet == true )
    {
        if( WIFI_Off() != eWiFiSuccess )
        {
            xRet = false;
        }
    }

    return xRet;
}

#endif

/*-----------------------------------------------------------*/

static void prvInvokeSubscription( uint32_t ulNetworkType, AwsIotNetworkState_t xNetworkEvent )
{
    uint16_t usIdx;
    IotTaskPoolError_t xError = IOT_TASKPOOL_NO_MEMORY;
    IotTaskPoolJobStatus_t xTaskStatus;
    NMCallbackExecutionContext_t *pxTask;

    /* Search for a free job. */
    for( usIdx = 0; usIdx < _MAX_CONCURRENT_JOBS; usIdx++ )
    {
        pxTask = &( xNetworkManagerInfo.xJobs[usIdx] );
        ( void ) IotTaskPool_GetStatus( xNetworkManagerInfo.pxTaskPool, &pxTask->xJob, &xTaskStatus );
        if( xTaskStatus == IOT_TASKPOOL_STATUS_READY )
        {
        	pxTask->ulNetworkType = ulNetworkType;
        	pxTask->xNewtorkEvent = xNetworkEvent;
        	pxTask->pvUserContext = pxTask;

            xError = IotTaskPool_Schedule( xNetworkManagerInfo.pxTaskPool, &pxTask->xJob, 0 );
            break;
        }
    }

    if(xError != IOT_TASKPOOL_SUCCESS)
    {
    	if(usIdx != _MAX_CONCURRENT_JOBS)
    	{
    		IotLogError(" Could not invoke subscription, error %d returned. \n", xError);
    	}else
    	{
    		IotLogError(" Could not invoke subscription, no job were ready.\n", xError);
    	}
    }
}



static void prvUserCallbackRoutine( struct IotTaskPool * pTaskPool, struct IotTaskPoolJob * pJob, void * pUserContext )
{
    IotLink_t* pxLink = NULL;
    NMSubscription_t* pxSubscription = NULL;
    NMCallbackExecutionContext_t* pxContext = ( NMCallbackExecutionContext_t* ) pUserContext;
    IotTaskPoolError_t xError = IOT_TASKPOOL_NO_MEMORY;

    ( void ) xSemaphoreTake( xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
    IotContainers_ForEach( &xNetworkManagerInfo.xSubscriptionListHead,  pxLink )
    {
        pxSubscription = IotLink_Container( NMSubscription_t, pxLink,  xSubscriptionList );
        pxSubscription->xUserCallback(pxContext->ulNetworkType, pxContext->xNewtorkEvent, pxSubscription->pvUserContext);

    }

    /* Job completed, ready again to be used. */
    xError = IotTaskPool_CreateJob( prvUserCallbackRoutine,
    		                                pxContext,
											&pxContext->xJob );
    if(xError != IOT_TASKPOOL_SUCCESS)
    {
    	IotLogError(" Could not invoke subscription, error %d returned \n", xError);
    }

    ( void ) xSemaphoreGive( xNetworkManagerInfo.xSubscriptionLock );
}

/*-----------------------------------------------------------*/


BaseType_t AwsIotNetworkManager_Init( void )
{
    BaseType_t xRet = pdTRUE;
    NMCallbackExecutionContext_t *pxTask;
    uint16_t usIdx;
    IotTaskPoolError_t xError = IOT_TASKPOOL_NO_MEMORY;


    if( !xNetworkManagerInfo.xIsInit )
    {
        xNetworkManagerInfo.xSubscriptionLock = xSemaphoreCreateMutex();

        if( xNetworkManagerInfo.xSubscriptionLock == NULL )
        {
            xRet = pdFALSE;
        }

        if( xRet == pdTRUE )
        {
			/* Set all job to ready at initialization. */
			for( usIdx = 0; usIdx < _MAX_CONCURRENT_JOBS; usIdx++ )
			{
				pxTask = &( xNetworkManagerInfo.xJobs[usIdx] );

				xError = IotTaskPool_CreateJob( prvUserCallbackRoutine,
														pxTask,
														&pxTask->xJob );
				if(xError != IOT_TASKPOOL_SUCCESS)
				{
					xRet = pdFALSE;
					IotLogError(" Could not initialize task pool job array, error %d returned \n", xError);
					break;
				}
			}
        }

        if( xRet == pdTRUE )
        {
            xNetworkManagerInfo.pxTaskPool = IOT_SYSTEM_TASKPOOL;
            if( xNetworkManagerInfo.pxTaskPool == NULL )
            {
                xRet = pdFALSE;
            }
        }

        if( xRet == pdTRUE )
        {
            IotListDouble_Create( &xNetworkManagerInfo.xSubscriptionListHead );
            IotListDouble_Create( &xNetworkManagerInfo.xNetworkListHead );
        
#if BLE_ENABLED
            IotListDouble_InsertTail( &xNetworkManagerInfo.xNetworkListHead, &xBLENetworkInfo.xNetworkList );
#endif

#if WIFI_ENABLED

           IotListDouble_InsertTail( &xNetworkManagerInfo.xNetworkListHead, &xWiFiNetworkInfo.xNetworkList );
           if( WIFI_RegisterNetworkStateChangeEventCallback( prvWiFiEventCallback ) != eWiFiSuccess )
           {
                xRet = pdFALSE;
           } 
#endif
            xNetworkManagerInfo.xIsInit = pdTRUE;
        }
        else
        {
            if( xNetworkManagerInfo.xSubscriptionLock != NULL )
            {
                vSemaphoreDelete( xNetworkManagerInfo.xSubscriptionLock );
            }
        }

    }
    return xRet;
}

BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t ulNetworkTypes, AwsIotNetworkStateChangeCb_t xCallback, void * pvContext, SubscriptionHandle_t* pxHandle  )
{
    BaseType_t xRet = pdFALSE;
    NMSubscription_t* pxSubscription;

    if( xNetworkManagerInfo.xIsInit )
    {
        pxSubscription = pvPortMalloc( sizeof( NMSubscription_t ) );
        if( pxSubscription != NULL )
        {
            pxSubscription->ulNetworkTypes = ulNetworkTypes;
            pxSubscription->pvUserContext = pvContext;
            pxSubscription->xUserCallback = xCallback;

            ( void ) xSemaphoreTake( xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
            IotListDouble_InsertTail( &xNetworkManagerInfo.xSubscriptionListHead, &pxSubscription->xSubscriptionList );
            ( void ) xSemaphoreGive( xNetworkManagerInfo.xSubscriptionLock );
            *pxHandle = ( SubscriptionHandle_t ) pxSubscription;
            xRet = pdTRUE;
        }
        else
        {
            IotLogInfo(" Not enough memory to add new subscription \n");
        }
    }
    else
    {
        IotLogInfo( "Subscription List not initialized \n");
    }
    return xRet;
}

BaseType_t AwsIotNetworkManager_RemoveSubscription(  SubscriptionHandle_t xHandle )
{
    BaseType_t xRet = pdFALSE;
    NMSubscription_t* ppxSubscription, *pxListItem;
    IotLink_t* pxLink;

    if( xNetworkManagerInfo.xIsInit )
    {
        ppxSubscription = ( NMSubscription_t* ) xHandle;
        ( void ) xSemaphoreTake( xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
        IotContainers_ForEach( &xNetworkManagerInfo.xSubscriptionListHead, pxLink )
        {
            pxListItem = IotLink_Container( NMSubscription_t, pxLink, xSubscriptionList );
            if( pxListItem == ppxSubscription )
            {
                IotListDouble_Remove( pxLink );
                vPortFree( ppxSubscription );
                xRet = pdTRUE;
                break;
            }

        }
        ( void ) xSemaphoreGive( xNetworkManagerInfo.xSubscriptionLock );
    }

    return xRet;
}

uint32_t AwsIotNetworkManager_GetConfiguredNetworks( void )
{
    IotLink_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    IotContainers_ForEach( &xNetworkManagerInfo.xNetworkListHead, pxLink )
    {
        pxNetwork = IotLink_Container( NMNetworkInfo_t, pxLink, xNetworkList );
        ulRet |= pxNetwork->ulNetworkType;
    }
    return ulRet;
}

uint32_t AwsIotNetworkManager_GetEnabledNetworks( void )
{
    IotLink_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    IotContainers_ForEach( &xNetworkManagerInfo.xNetworkListHead, pxLink )
    {
        pxNetwork = IotLink_Container( NMNetworkInfo_t, pxLink, xNetworkList );
        if( pxNetwork->xNetworkState != eNetworkStateUnknown )
        {
            ulRet |= pxNetwork->ulNetworkType;
        }
    }

    return ulRet;
}

uint32_t AwsIotNetworkManager_GetConnectedNetworks( void )
{
    IotLink_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    IotContainers_ForEach( &xNetworkManagerInfo.xNetworkListHead, pxLink )
    {
        pxNetwork = IotLink_Container( NMNetworkInfo_t, pxLink, xNetworkList );
        if( pxNetwork->xNetworkState == eNetworkStateEnabled )
        {
            ulRet |= pxNetwork->ulNetworkType;
        }
    }

    return ulRet;
}

uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t ulNetworkTypes )
{
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;

#if BLE_ENABLED
    if( ( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE ) &&
           ( xBLENetworkInfo.xNetworkState == eNetworkStateUnknown ) )
    {
        xBLENetworkInfo.xNetworkState = eNetworkStateDisabled;
        if( prxBLEEnable() == pdTRUE )
        {
            ulRet |= AWSIOT_NETWORK_TYPE_BLE;
        }
    }
#endif

#if WIFI_ENABLED

    if( ( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI ) &&
           ( xWiFiNetworkInfo.xNetworkState == eNetworkStateUnknown ) )
    {
        xWiFiNetworkInfo.xNetworkState = eNetworkStateDisabled;
        if( prxWIFIEnable() == pdTRUE )
        {
            ulRet |= AWSIOT_NETWORK_TYPE_WIFI;
        }
    }
#endif

    return ulRet;
}

uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t ulNetworkTypes )
{
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;

#if WIFI_ENABLED
    if( ( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI ) &&
           ( xWiFiNetworkInfo.xNetworkState != eNetworkStateUnknown ) )
    {
        if( prxWIFIDisable() == pdTRUE )
        {
            xWiFiNetworkInfo.xNetworkState = eNetworkStateUnknown;
            ulRet  |= AWSIOT_NETWORK_TYPE_WIFI;
        }
    }
#endif

#if BLE_ENABLED
    if( ( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE ) &&
           ( xBLENetworkInfo.xNetworkState != eNetworkStateUnknown ) )
    {
        if( prxBLEDisable() == pdTRUE )
        {
            xBLENetworkInfo.xNetworkState = eNetworkStateUnknown;
            ulRet |= AWSIOT_NETWORK_TYPE_BLE;
        }
    }
#endif
    return ulRet;
}
