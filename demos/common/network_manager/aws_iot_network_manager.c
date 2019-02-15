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
#include "iot_ble_config.h"
#include "iot_ble.h"
#include "aws_ble_numericComparison.h"
#endif
#if WIFI_ENABLED
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#endif

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
#include "iot_ble_wifi_provisioning.h"
#include "aws_wifi_connect_task.h"
#endif

/**
 *  @brief Structure used to store one node representing each subscription.
 */
typedef struct StateChangeSubscription
{
    Link_t xSubscriptionList;
    uint32_t ulNetworkTypes;
    AwsIotNetworkStateChangeCb_t xCallback;
    void *pvContext;
} StateChangeSubscription_t;

/**
 * @brief Subscription List structure containing list head and lock for the list.
 */
typedef struct StateChangeSubscriptionList
{
    SemaphoreHandle_t xLock;
    Link_t xListHead;
    bool xIsInit;
} StateChangeSubscriptionList_t;

/**
 * @brief Function used to enable a BLE network.
 *
 * @return true if BLE is enabled successfully.
 */
static bool prvBLEEnable( void );


/**
 * @brief Function used to disable a BLE network.
 *
 * @return true if BLE is disable successfully, false if already disabled
 */
static bool prvBLEDisable( void );

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
static bool prvWIFIDisable( void );

/**
 * @brief Function goes through the list of subscriptions and invoke the network state change subscription callbacks.
 * @param ulNetworkType Type of the network for which the state changed
 * @param xState The new state of the network.
 */
static void prvInvokeNetworkStateChangeCallbacks( uint32_t ulNetworkType, AwsIotNetworkState_t xState );


#if BLE_ENABLED
/**
 * @brief Callback invoked when a BLE network connects or disconnects.
 * @param xStatus[in] Whether the operation was successful or not.
 * @param connId[in] Connection id of the connection
 * @param xConnected[in] true if its connected, false if its disconnected.
 * @param pxBda[in] Address of the remote device connected/disconnected.
 */
static void prvBLEConnectionCallback( BTStatus_t xStatus,
                             uint16_t connId,
                             bool xConnected,
                             BTBdaddr_t * pxBda );
#endif

#if WIFI_ENABLED
/**
 * @Brief Callback registered by network manager with WIFI layer for  connection state changes.
 * @param ulNetworkType Network type passed from WIFI
 * @param xState State of the network
 * @param pvContext User context passed as it is to the callback
 */
static void prvWiFiConnectionCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState, void *pvContext );
#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
/**
 * Connects to the WIFI using credentials configured statically
 * @return true if connected successfully.
 */
static BaseType_t prxWifiConnect( void );
#endif

#endif


static StateChangeSubscriptionList_t xSubscriptionList =
{
        .xListHead = { 0 },
        .xLock = NULL,
        .xIsInit = false
};
static uint32_t ulEnabledNetworks = AWSIOT_NETWORK_TYPE_NONE;
static uint32_t ulConnectedNetworks = AWSIOT_NETWORK_TYPE_NONE;
static SemaphoreHandle_t xConnectionSemaphore = NULL;

#if BLE_ENABLED


static bool prvBLEEnable( void )
{
	IotBleEventsCallbacks_t xEventCb;
	BaseType_t xRet = pdTRUE;
	static bool bInitBLE = false;
	BTStatus_t xStatus;

	if( !( ulEnabledNetworks & AWSIOT_NETWORK_TYPE_BLE  ) )
	{
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
		}
		/* Register BLE Connection callback */
		if( xRet == pdTRUE )
		{
			xEventCb.pConnectionCb = prvBLEConnectionCallback;
			if( IotBle_RegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
			{
				xRet = pdFALSE;
			}
		}

	    if( xStatus == eBTStatusSuccess )
	    {
	    	 xEventCb.pGAPPairingStateChangedCb = &BLEGAPPairingStateChangedCb;
	    	 xStatus = IotBle_RegisterEventCb( eBLEPairingStateChanged, xEventCb );
	    }

	#if ( IOT_BLE_ENABLE_NUMERIC_COMPARISON == 1 )
	    if( xStatus == eBTStatusSuccess )
	    {
	    	xEventCb.pNumericComparisonCb = &BLENumericComparisonCb;
	    	xStatus = IotBle_RegisterEventCb( eBLENumericComparisonCallback, xEventCb );
	    }
	#endif

		if( xRet == pdTRUE )
		{
		    ulEnabledNetworks |= AWSIOT_NETWORK_TYPE_BLE;
		}
	}

	return xRet;
}

/*-----------------------------------------------------------*/

static bool prvBLEDisable( void )
{
	bool xRet = true;
	IotBleEventsCallbacks_t xEventCb;

	if( ( ulEnabledNetworks & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
	{
		xEventCb.pConnectionCb = prvBLEConnectionCallback;
		if( IotBle_UnRegisterEventCb( eBLEConnection, xEventCb ) != eBTStatusSuccess )
		{
			xRet = false;
		}

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

		if( xRet == true )
		{
		    ulEnabledNetworks &= ~AWSIOT_NETWORK_TYPE_BLE;
		    ulConnectedNetworks &= ~AWSIOT_NETWORK_TYPE_BLE;
		}
	}

	return xRet;
}

static void prvBLEConnectionCallback( BTStatus_t xStatus,
                             uint16_t connId,
                             bool xConnected,
                             BTBdaddr_t * pxBda )
{

    if( xConnected == true )
    {
    	AwsIotLogInfo ( "BLE Connected to remote device, connId = %d\n", connId );
    	ulConnectedNetworks |= AWSIOT_NETWORK_TYPE_BLE;
    	prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_BLE, eNetworkStateEnabled );
        ( void ) xSemaphoreGive( xConnectionSemaphore );
        IotBle_StopAdv();


    }
    else
    {
        AwsIotLogInfo ( "BLE disconnected with remote device, connId = %d \n", connId );
        ulConnectedNetworks &= ~AWSIOT_NETWORK_TYPE_BLE;
        prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_BLE, eNetworkStateDisabled );
        if( ( ulEnabledNetworks &  AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
        {
            ( void ) IotBle_StartAdv( );
        }

    }
}

#else

static bool prvBLEEnable( void )
{
	return false;
}

static bool prvBLEDisable( void )
{
	return false;
}

#endif
/*-----------------------------------------------------------*/

#if WIFI_ENABLED
static void prvWiFiConnectionCallback( uint32_t ulNetworkType, AwsIotNetworkState_t xState, void *pvContext )
{
    ( void ) ulNetworkType;
    ( void ) pvContext;

    if( xState == eNetworkStateEnabled )
    {
        ulConnectedNetworks |= AWSIOT_NETWORK_TYPE_WIFI;
        ( void ) xSemaphoreGive( xConnectionSemaphore );
    }
    else
    {
        ulConnectedNetworks &= ~ AWSIOT_NETWORK_TYPE_WIFI;
    }

    prvInvokeNetworkStateChangeCallbacks( AWSIOT_NETWORK_TYPE_WIFI, xState );
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

    if( !( ulEnabledNetworks & AWSIOT_NETWORK_TYPE_WIFI  ) )
    {

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

#if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 0 )
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
        if( xRet == pdTRUE )
        {
            ulEnabledNetworks |= AWSIOT_NETWORK_TYPE_WIFI;
        }

    }
    return xRet;
}
static bool prvWIFIDisable( void )
{
    bool xRet = false;
    if( ( ulEnabledNetworks & AWSIOT_NETWORK_TYPE_WIFI  ) == AWSIOT_NETWORK_TYPE_WIFI )
    {

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

        if( xRet == true )
        {
            ulConnectedNetworks &= ~AWSIOT_NETWORK_TYPE_WIFI;
            ulEnabledNetworks &= ~AWSIOT_NETWORK_TYPE_WIFI;
        }
    }

    return xRet;
}

#else
static BaseType_t prxWIFIEnable( void )
{
	return false;
}
static bool prvWIFIDisable( void )
{
	return false;
}

#endif

/*-----------------------------------------------------------*/

static void prvInvokeNetworkStateChangeCallbacks( uint32_t ulNetworkType, AwsIotNetworkState_t xState )
{
	Link_t *pxLink;
	StateChangeSubscription_t* pxSubscription;

	( void ) xSemaphoreTake(xSubscriptionList.xLock, portMAX_DELAY );
	listFOR_EACH( pxLink, &xSubscriptionList.xListHead )
	{
		pxSubscription = listCONTAINER( pxLink, StateChangeSubscription_t, xSubscriptionList );
		if( ( pxSubscription->ulNetworkTypes & ulNetworkType ) == ulNetworkType )
		{
			pxSubscription->xCallback( ulNetworkType, xState, pxSubscription->pvContext );
		}
	}
	( void ) xSemaphoreGive(xSubscriptionList.xLock );
}

/*-----------------------------------------------------------*/


BaseType_t AwsIotNetworkManager_Init( void )
{
	BaseType_t xRet = pdFALSE;

	if( !xSubscriptionList.xIsInit )
	{
		xSubscriptionList.xLock = xSemaphoreCreateMutex();
		if( xSubscriptionList.xLock != NULL )
		{
			listINIT_HEAD( &xSubscriptionList.xListHead );
			xRet = pdTRUE;
		}

                if( xRet == pdTRUE )
                {
                    xConnectionSemaphore = xSemaphoreCreateBinary();
                    if( xConnectionSemaphore != NULL )
                    {
                        xRet = pdTRUE;
                    }
                }

                if( xRet == pdTRUE )
                {
                    xSubscriptionList.xIsInit = true;
                }
	}

	return xRet;
}

BaseType_t AwsIotNetworkManager_SubscribeForStateChange( uint32_t ulNetworkTypes, AwsIotNetworkStateChangeCb_t xCallback, void * pvContext, SubscriptionHandle_t* pxHandle  )
{
	BaseType_t xRet = pdFALSE;
	StateChangeSubscription_t* pxSubscription;


	if( xSubscriptionList.xIsInit )
	{
		pxSubscription = pvPortMalloc( sizeof( StateChangeSubscription_t ) );
		if( pxSubscription != NULL )
		{
			pxSubscription->xCallback = xCallback;
			pxSubscription->ulNetworkTypes = ulNetworkTypes;
			pxSubscription->pvContext = pvContext;

			( void ) xSemaphoreTake(xSubscriptionList.xLock, portMAX_DELAY );
			listADD( &xSubscriptionList.xListHead, &pxSubscription->xSubscriptionList );
			( void ) xSemaphoreGive( xSubscriptionList.xLock );
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
	StateChangeSubscription_t* ppxSubscription, *pxListItem;
	Link_t* pxLink;

	if( xSubscriptionList.xIsInit )
	{
                ppxSubscription = ( StateChangeSubscription_t* ) xHandle;
		( void ) xSemaphoreTake( xSubscriptionList.xLock, portMAX_DELAY );
		listFOR_EACH( pxLink, &xSubscriptionList.xListHead )
		{
			pxListItem = listCONTAINER( pxLink, StateChangeSubscription_t, xSubscriptionList );
			if( pxListItem == ppxSubscription )
			{
				listREMOVE( pxLink );
				vPortFree( ppxSubscription );
				xRet = pdTRUE;
				break;
			}

		}
		( void ) xSemaphoreGive( xSubscriptionList.xLock );
	}

	return xRet;
}

uint32_t AwsIotNetworkManager_GetConfiguredNetworks( void )
{
	return configENABLED_NETWORKS;
}

uint32_t AwsIotNetworkManager_GetEnabledNetworks( void )
{
    return ulEnabledNetworks;
}

uint32_t AwsIotNetworkManager_GetConnectedNetworks( void )
{
    return ulConnectedNetworks;
}

uint32_t AwsIotNetworkManager_WaitForNetworkConnection( void )
{
    if( ulConnectedNetworks == AWSIOT_NETWORK_TYPE_NONE )
    {
        ( void ) xSemaphoreTake( xConnectionSemaphore, portMAX_DELAY );
    }
    return ulConnectedNetworks;
}

uint32_t AwsIotNetworkManager_EnableNetwork( uint32_t ulNetworkTypes )
{
    uint32_t ulEnabled = AWSIOT_NETWORK_TYPE_NONE;

    if( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        if( prvBLEEnable() == true )
        {
            ulEnabled |= AWSIOT_NETWORK_TYPE_BLE;
        }
    }

    if( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        if( prxWIFIEnable() == pdTRUE )
        {
            ulEnabled  |= AWSIOT_NETWORK_TYPE_WIFI;
        }
    }


    return ulEnabled;
}

uint32_t AwsIotNetworkManager_DisableNetwork( uint32_t ulNetworkTypes )
{
	uint32_t ulDisabled = AWSIOT_NETWORK_TYPE_NONE;

	if( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
	{
		if( prvWIFIDisable() == true )
		{
			ulDisabled  |= AWSIOT_NETWORK_TYPE_WIFI;
		}
	}
	if( ( ulNetworkTypes & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
	{
		if( prvBLEDisable() == true )
		{
			ulDisabled |= AWSIOT_NETWORK_TYPE_BLE;
		}
	}

	return ulDisabled;
}
