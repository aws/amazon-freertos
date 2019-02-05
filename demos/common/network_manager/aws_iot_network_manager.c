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
#include "aws_iot_demo.h"
#include "aws_iot_network_manager.h"

#if BLE_ENABLED
#include "aws_ble_config.h"
#include "aws_ble.h"
#include "aws_ble_numericComparison.h"
#include "aws_ble_services_init.h"
#endif
#if WIFI_ENABLED
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#endif

#if ( bleconfigENABLE_WIFI_PROVISIONING == 1 )
#include "aws_ble_wifi_provisioning.h"
#include "aws_wifi_connect_task.h"
#endif

#include "aws_iot_taskpool.h"


/**
 *  @brief Structure used to store one node representing each subscription.
 */
typedef struct NMSubscriptionInfo
{
    Link_t xSubscriptionList;
    uint32_t ulNetworkTypes;
    AwsIotNetworkStateChangeCb_t xCallback;
    void *pvContext;
} NMSubscriptionInfo_t;

typedef struct NMNetworkInfo
{
    Link_t xNetworkList;
    uint32_t ulNetworkType;
    AwsIotNetworkState_t xNetworkState;
} NMNetworkInfo_t;

/**
 * @brief Network Manager Info structure containing list of networks and list of subscriptions.
 */
typedef struct NetworkManagerInfo
{
    SemaphoreHandle_t xSubscriptionLock;
    Link_t xSubscriptionListHead;
    Link_t xNetworkListHead;
    AwsIotTaskPool_t* pxTaskPool;
    bool xIsInit;
} NetworkManagerInfo_t;


#if BLE_ENABLED

NMNetworkInfo_t xBLENetworkInfo = {
    .ulNetworkType = AWSIOT_NETWORK_TYPE_BLE
};

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


static BTStatus_t prvBLEInit( void );


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

/**
 * @brief Callback invoked if advertisement data is set.
 * @param xStatus[in] Status of the set advertisement data operation.
 */
static void prvSetAdvCallback( BTStatus_t xStatus );

/**
 * @brief Callback invoked after the scan response is set.
 * @param xStatus[in] Status of scan response set operation.
 */
static void prvSetScanRespCallback( BTStatus_t xStatus );

#endif

#if WIFI_ENABLED

NMNetworkInfo_t xWiFiNetworkInfo = {
    .ulNetworkType = AWSIOT_NETWORK_TYPE_WIFI
};

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
static void prvWiFiConnectionCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState, void *pvContext );

#if ( bleconfigENABLE_WIFI_PROVISIONING == 0 )
/**
 * Connects to the WIFI using credentials configured statically
 * @return true if connected successfully.
 */
static BaseType_t prxWifiConnect( void );

#endif
#endif

/**
 * @brief Function goes through the list of subscriptions and invoke the network state change subscription callbacks.
 * @param ulNetworkType Type of the network for which the state changed
 * @param xState The new state of the network.
 */
static void prvInvokeNetworkStateChangeCallbacks( uint32_t ulNetworkType, AwsIotNetworkState_t xState );


static NetworkManagerInfo_t xNetworkManagerInfo = { 0 };



#if BLE_ENABLED

#if (bleconfigADVERTISING_UUID_SIZE == 2)
#define BT_ADV_UUID_TYPE	eBTuuidType16
#else
#define BT_ADV_UUID_TYPE	eBTuuidType128
#endif

static BTUuid_t xAdvUUID =
{
	.uu.uu128 = bleconfigADVERTISING_UUID,
	.ucType   =  BT_ADV_UUID_TYPE

};

static BLEAdvertismentParams_t xAdvParams =
{
    .bIncludeTxPower    = true,
    .bIncludeName       = true,
    .bSetScanRsp        = false,
    .ulAppearance       = bleconfigADVERTISING_APPEARANCE,
    .ulMinInterval      = bleconfigADVERTISING_INTERVAL_MIN,
    .ulMaxInterval      = bleconfigADVERTISING_INTERVAL_MAX,
    .usServiceDataLen   = 0,
    .pcServiceData      = NULL,
    .usManufacturerLen  = 0,
    .pcManufacturerData = NULL,
    .pxUUID1           = &xAdvUUID,
    .pxUUID2            = NULL
};

static BTStatus_t prvBLEInit( void )
{
    BTStatus_t xStatus = eBTStatusSuccess;
    BLEEventsCallbacks_t xEventCb;
    BTUuid_t xServerUUID =
    {
        .ucType   = eBTuuidType128,
        .uu.uu128 = bleconfigSERVER_UUID
    };


#if ( bleconfigENABLE_BONDING == 1 )
    const bool bIsBondable = true;
#else
    const bool bIsBondable = false;
#endif

#if ( bleconfigENABLE_SECURE_CONNECTION == 1 )
    const bool bSecureConnection = true;
#else
    const bool bSecureConnection = false;
#endif
    const uint32_t usMtu = bleconfigPREFERRED_MTU_SIZE;
    const BTIOtypes_t xIO = bleconfigINPUT_OUTPUT;
    size_t xNumProperties;

    BTProperty_t xDeviceProperties[] =
    {
        {
            .xType = eBTpropertyBdname,
            .xLen = strlen( bleconfigDEVICE_NAME ),
            .pvVal = ( void * ) bleconfigDEVICE_NAME
        },
        {
            .xType = eBTpropertyBondable,
            .xLen = 1,
            .pvVal = ( void * ) &bIsBondable
        },
		{
			.xType = eBTpropertySecureConnectionOnly,
			.xLen = 1,
			.pvVal = ( void * ) &bSecureConnection
		},
		{
			.xType = eBTpropertyIO,
			.xLen = 1,
			.pvVal = ( void * ) &xIO
		},
        {
            .xType = eBTpropertyLocalMTUSize,
            .xLen = 1,
            .pvVal = ( void * ) &usMtu
        }
    };

    xNumProperties = sizeof( xDeviceProperties ) / sizeof ( xDeviceProperties[0] );

    xStatus = BLE_Init( &xServerUUID, xDeviceProperties, xNumProperties );

    if( xStatus == eBTStatusSuccess )
    {
    	 xEventCb.pxGAPPairingStateChangedCb = &BLEGAPPairingStateChangedCb;
    	 xStatus = BLE_RegisterEventCb( eBLEPairingStateChanged, xEventCb );
    }

#if ( bleconfigENABLE_NUMERIC_COMPARISON == 1 )
    if( xStatus == eBTStatusSuccess )
    {
    	xEventCb.pxNumericComparisonCb = &BLENumericComparisonCb;
    	xStatus = BLE_RegisterEventCb( eBLENumericComparisonCallback, xEventCb );
    }
#endif

    /* Initialize BLE Services */
    if( xStatus == eBTStatusSuccess )
    {
        /*Initialize bluetooth services */
        if( BLE_SERVICES_Init() != pdPASS )
        {
            xStatus = eBTStatusFail;
        }
    }

    return xStatus;
}


static BaseType_t prxBLEEnable( void )
{
	BLEEventsCallbacks_t xEventCb;
	BaseType_t xRet = pdTRUE;
	static bool bInitBLE = false;
	BTStatus_t xStatus;

	if( bInitBLE == false )
	{
	    xStatus = prvBLEInit();
	    if( xStatus == eBTStatusSuccess )
	    {
	        bInitBLE = true;
	    }
	    else
	    {
	        xRet = pdFALSE;
	    }
	}
	else
	{
	    if( BLE_ON() != eBTStatusSuccess )
	    {
	        xRet = pdFALSE;
	    }
	}
	/* Register BLE Connection callback */
	if( xRet == pdTRUE )
	{
	    xEventCb.pxConnectionCb = prvBLEConnectionCallback;
	    if( BLE_RegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
	    {
	        xRet = pdFALSE;
	    }
	}

	if( xRet == pdTRUE )
	{
	    xAdvParams.bSetScanRsp = false;
	    if( BLE_SetAdvData( BTAdvInd, &xAdvParams, prvSetAdvCallback ) != eBTStatusSuccess )
	    {
	        xRet = pdFALSE;
	    }
	}


	return xRet;
}

/*-----------------------------------------------------------*/

static BaseType_t prxBLEDisable( void )
{
	BaseType_t xRet = pdTRUE;
	BLEEventsCallbacks_t xEventCb;

	xEventCb.pxConnectionCb = prvBLEConnectionCallback;
	if( BLE_UnRegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
	{
	    xRet = pdFALSE;
	}

	if( xRet == pdTRUE )
	{
	    if( BLE_StopAdv() != eBTStatusSuccess )
	    {
	        xRet = pdFALSE;
	    }
	}

	if( xRet == pdTRUE )
	{
	    if( BLE_OFF() != eBTStatusSuccess )
	    {
	        xRet = pdFALSE;
	    }
	}

	return xRet;
}

static void prvStartAdvCallback( BTStatus_t xStatus )
{
    if( xStatus == eBTStatusSuccess )
    {
        AwsIotLogInfo ( "Started advertisement. Listening for a BLE Connection.\n" );
    }
}

static void prvSetScanRespCallback( BTStatus_t xStatus )
{
    if( xStatus == eBTStatusSuccess )
    {
        AwsIotLogInfo( "Successfully set the scan Response data \n");
        ( void ) BLE_StartAdv( prvStartAdvCallback );
    }
}

static void prvSetAdvCallback( BTStatus_t xStatus )
{
    if( xStatus == eBTStatusSuccess )
    {
    	/* Set the scan response */
    	xAdvParams.bSetScanRsp = true;
        ( void ) BLE_SetAdvData( BTAdvInd, &xAdvParams, prvSetScanRespCallback );
    }
}

static void prvBLEConnectionCallback( BTStatus_t xStatus,
                             uint16_t usConnId,
                             bool xConnected,
                             BTBdaddr_t * pxBda )
{


    if( xConnected == true )
    {
    	AwsIotLogInfo ( "BLE Connected to remote device, connId = %d\n", usConnId );
        BLE_StopAdv();
        xBLENetworkInfo.xNetworkState = eNetworkStateEnabled;
    }
    else
    {
        AwsIotLogInfo ( "BLE disconnected with remote device, connId = %d \n", usConnId );
        prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_BLE, eNetworkStateDisabled );
        if( xBLENetworkInfo.xNetworkState == eNetworkStateEnabled )
        {
            xBLENetworkInfo.xNetworkState = eNetworkStateDisabled;
            ( void ) BLE_StartAdv( prvStartAdvCallback );
        }

    }

    prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_BLE, xBLENetworkInfo.xNetworkState );
}

#endif
/*-----------------------------------------------------------*/

#if WIFI_ENABLED
static void prvWiFiConnectionCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState, void *pvContext )
{
    ( void ) ulNetworkType;
    ( void ) pvContext;
    xWiFiNetworkInfo.xNetworkState = xState;
    prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_WIFI, xState );
}

#if ( bleconfigENABLE_WIFI_PROVISIONING == 0 )
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

    if( xRet == pdTRUE )
    {
        if( WIFI_RegisterStateChangeCallback( prvWiFiConnectionCallback, NULL ) != eWiFiSuccess )
        {
            xRet = pdFALSE;
        }
    }

#if ( bleconfigENABLE_WIFI_PROVISIONING == 0 )
    if( xRet == pdTRUE )
    {
        xRet = prxWifiConnect();
    }
#else
    if ( xRet == pdTRUE )
    {
        xRet = WIFI_PROVISION_Start();
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

#if ( bleconfigENABLE_WIFI_PROVISIONING == 1 )
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

static void prvInvokeNetworkStateChangeCallbacks( uint32_t ulNetworkType, AwsIotNetworkState_t xState )
{
	Link_t *pxLink;
	NMSubscriptionInfo_t* pxSubscription;

	( void ) xSemaphoreTake( xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
	listFOR_EACH( pxLink, &xNetworkManagerInfo.xSubscriptionListHead )
	{
		pxSubscription = listCONTAINER( pxLink, NMSubscriptionInfo_t, xSubscriptionList );
		if( ( pxSubscription->ulNetworkTypes & ulNetworkType ) == ulNetworkType )
		{
			pxSubscription->xCallback( ulNetworkType, xState, pxSubscription->pvContext );
		}
	}
	( void ) xSemaphoreGive( xNetworkManagerInfo.xSubscriptionLock );
}

/*-----------------------------------------------------------*/


BaseType_t AwsIotNetworkManager_Init( void )
{
	BaseType_t xRet = pdTRUE;
	if( !xNetworkManagerInfo.xIsInit )
	{
	    xNetworkManagerInfo.xSubscriptionLock = xSemaphoreCreateMutex();
		if( xNetworkManagerInfo.xSubscriptionLock == NULL )
		{
			xRet = pdFALSE;
		}
		if( xRet == pdTRUE )
		{
		    xNetworkManagerInfo.pxTaskPool = AWS_IOT_TASKPOOL_SYSTEM_TASKPOOL;
		    if( xNetworkManagerInfo.pxTaskPool == NULL )
		    {
		        xRet = pdFALSE;
		    }
		}

		if( xRet == pdTRUE )
		{
		    listINIT_HEAD( &xNetworkManagerInfo.xSubscriptionListHead );
		    listINIT_HEAD( &xNetworkManagerInfo.xNetworkListHead );

#if BLE_ENABLED
		    listADD( &xNetworkManagerInfo.xNetworkListHead, &xBLENetworkInfo.xNetworkList );
#endif
#if WIFI_ENABLED
		    listADD( &xNetworkManagerInfo.xNetworkListHead, &xWiFiNetworkInfo.xNetworkList );
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
	NMSubscriptionInfo_t* pxSubscription;


	if( xNetworkManagerInfo.xIsInit )
	{
		pxSubscription = pvPortMalloc( sizeof( NMSubscriptionInfo_t ) );
		if( pxSubscription != NULL )
		{
			pxSubscription->xCallback = xCallback;
			pxSubscription->ulNetworkTypes = ulNetworkTypes;
			pxSubscription->pvContext = pvContext;

			( void ) xSemaphoreTake(xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
			listADD( &xNetworkManagerInfo.xSubscriptionListHead, &pxSubscription->xSubscriptionList );
			( void ) xSemaphoreGive( xNetworkManagerInfo.xSubscriptionLock );
			*pxHandle = ( SubscriptionHandle_t ) pxSubscription;
			xRet = pdTRUE;
		}
		else
		{
			AwsIotLogInfo(" Not enough memory to add new subscription \n");
		}
	}
	else
	{
		AwsIotLogInfo( "Subscription List not initialized \n");
	}
	return xRet;
}

BaseType_t AwsIotNetworkManager_RemoveSubscription(  SubscriptionHandle_t xHandle )
{
	BaseType_t xRet = pdFALSE;
	NMSubscriptionInfo_t* ppxSubscription, *pxListItem;
	Link_t* pxLink;

	if( xNetworkManagerInfo.xIsInit )
	{
                ppxSubscription = ( NMSubscriptionInfo_t* ) xHandle;
		( void ) xSemaphoreTake( xNetworkManagerInfo.xSubscriptionLock, portMAX_DELAY );
		listFOR_EACH( pxLink, &xNetworkManagerInfo.xSubscriptionListHead )
		{
			pxListItem = listCONTAINER( pxLink, NMSubscriptionInfo_t, xSubscriptionList );
			if( pxListItem == ppxSubscription )
			{
				listREMOVE( pxLink );
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
    Link_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    listFOR_EACH( pxLink, &xNetworkManagerInfo.xNetworkListHead )
    {
        pxNetwork = listCONTAINER( pxLink, NMNetworkInfo_t, xNetworkList );
        ulRet |= pxNetwork->ulNetworkType;
    }
    return ulRet;
}

uint32_t AwsIotNetworkManager_GetEnabledNetworks( void )
{
    Link_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    listFOR_EACH( pxLink, &xNetworkManagerInfo.xNetworkListHead )
    {
        pxNetwork = listCONTAINER( pxLink, NMNetworkInfo_t, xNetworkList );
        if( pxNetwork->xNetworkState != eNetworkStateUnknown )
        {
            ulRet |= pxNetwork->ulNetworkType;
        }
    }

    return ulRet;
}

uint32_t AwsIotNetworkManager_GetConnectedNetworks( void )
{
    Link_t* pxLink;
    NMNetworkInfo_t* pxNetwork;
    uint32_t ulRet = AWSIOT_NETWORK_TYPE_NONE;
    listFOR_EACH( pxLink, &xNetworkManagerInfo.xNetworkListHead )
    {
        pxNetwork = listCONTAINER( pxLink, NMNetworkInfo_t, xNetworkList );
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

#ifdef BLE_ENABLED
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

#ifdef WIFI_ENABLED

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

#ifdef WIFI_ENABLED
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

#ifdef BLE_ENABLED
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
