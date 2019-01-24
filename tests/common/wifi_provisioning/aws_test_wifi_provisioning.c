/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_test_wifi_provisioning.c
 * @brief Tests for Wifi provisioning.
 */

/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"
#include "aws_ble_wifi_provisioning.h"
#include "aws_ble_wifi_prov_test_access_declare.h"
#include "aws_clientcredential.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"
#include "aws_ble.h"

#define testSERVER_UUID                 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define testNETWORK                "testNetwork%d"

static void prvBLEInitialize( void );
static void prvBLEDeInitialize( void );
static void prvRemoveSavedNetworks( void );
static void prvGetRealWIFINetwork( WIFINetworkProfile_t *pxNetwork );
static void prvGetTestWIFINetwork( WIFINetworkProfile_t *pxNetwork, uint16_t usId );
static BaseType_t prvIsSameNetwork( WIFINetworkProfile_t *pxNetwork1,  WIFINetworkProfile_t *pxNetwork2 );
static BaseType_t prvConnectRealNetwork( void );
static BTUuid_t xServerUUID =
{
	.ucType   = eBTuuidType128,
	.uu.uu128 = testSERVER_UUID
};

#define testMAXWIFI_WAIT_TIME    pdMS_TO_TICKS( 10000 )
#define testWIFI_DELAY   pdMS_TO_TICKS( 2000 )

TEST_GROUP( Full_WiFi_Provisioning );

/*-----------------------------------------------------------*/

TEST_SETUP( Full_WiFi_Provisioning )
{


}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_WiFi_Provisioning )
{
	prvRemoveSavedNetworks();
}


TEST_GROUP_RUNNER( Full_WiFi_Provisioning )
{

	prvBLEInitialize();
	prvRemoveSavedNetworks();

    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_Init );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_StartStop );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_AddNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_GetSavedNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeNetworkPriority );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeConnectedNetworkPriority );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteConnectedNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_GetNumNetworks);
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ConnectSavedNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_Deinit );

    prvBLEDeInitialize();
    prvRemoveSavedNetworks();

    TEST_ASSERT_EQUAL( pdPASS, prvConnectRealNetwork() );
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_Init )
{

	BaseType_t xStatus = WIFI_PROVISION_Init();
	TEST_ASSERT_EQUAL( pdTRUE, xStatus);
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_StartStop )
{

	BaseType_t xStatus = WIFI_PROVISION_Start();
	TEST_ASSERT_EQUAL( pdPASS, xStatus);
	xStatus = WIFI_PROVISION_Stop();
	TEST_ASSERT_EQUAL( pdPASS, xStatus );
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_AddNetwork )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;


	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		prvGetRealWIFINetwork( &xNetwork );
		xStatus = test_AddNewNetwork( &xNetwork );

		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		/* Verify that the network is connected */
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
		TEST_ASSERT_EQUAL( pdTRUE, test_GetConnectedNetwork( &xNetwork ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );

		/* Verify that network is saved */
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ));
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
	}

    WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_GetSavedNetwork )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		prvGetRealWIFINetwork( &xNetwork );
		xStatus = test_AppendNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_GetSavedNetwork(0, &xNetwork ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
	}

    WIFI_PROVISION_Stop();
}


TEST( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteNetwork )
{

	WIFINetworkProfile_t xNetwork;
	WIFIReturnCode_t xStatus;

	prvGetRealWIFINetwork( &xNetwork );

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		/* Add the new network */
		xStatus = test_AddNewNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		/* Verify that network is added */
		TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ));

		/* Delete the network  */
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		xStatus = test_PopNetwork( 0, &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_NOT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ));
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
	}
    WIFI_PROVISION_Stop();
}


TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeNetworkPriority )
{

	WIFINetworkProfile_t xNetwork = { 0 }, xTestProfile = { 0 };
	WIFIReturnCode_t xStatus;
	int x;

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		/* Add 3 dummy networks */
		for( x = 0; x < 3; x++ )
		{
			prvGetTestWIFINetwork(&xNetwork, x);
			xStatus = test_AppendNetwork( &xNetwork );
			TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		}

		/* Verify the order of the networks */
		for( x = 0; x < 3; x++ )
		{
			prvGetTestWIFINetwork(&xTestProfile, x);
			TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
			xStatus = prvIsSameNetwork( &xNetwork, &xTestProfile );
			TEST_ASSERT_EQUAL( pdTRUE,  xStatus );
		}

		/* Change priority of network from 0 to 2 */
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ));

		/* Verify the Network is at storage index 0  */
		prvGetTestWIFINetwork(&xTestProfile, 2);
		TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ) );
		xStatus = prvIsSameNetwork( &xNetwork, &xTestProfile );
		TEST_ASSERT_EQUAL( pdTRUE,  xStatus );

		/*Verify Network1 is at storage index 2 */
		prvGetTestWIFINetwork(&xTestProfile, 1);
		TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 2 ) );
		TEST_ASSERT_EQUAL( pdTRUE, prvIsSameNetwork( &xNetwork, &xTestProfile ) );

		/* Change piority from 0 to  2  twice */
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ));
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ));
		/* Verify its back to the initial order */
		for( x = 0; x < 3; x++ )
		{
			prvGetTestWIFINetwork(&xTestProfile, x);
			TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
			TEST_ASSERT_EQUAL( pdTRUE, prvIsSameNetwork( &xNetwork, &xTestProfile ) );
		}

		/* Move network 0 to network 1  and network 1 to network 0 */
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 1 ));
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 1, 0 ));
		/* Verify the order remains the same */
		for( x = 0; x < 3; x++ )
		{
			prvGetTestWIFINetwork(&xTestProfile, x);
			TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
			TEST_ASSERT_EQUAL( pdTRUE, prvIsSameNetwork( &xNetwork, &xTestProfile ) );
		}
	}

    WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeConnectedNetworkPriority )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;
	int x;

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		/* Add 2 dummy networks */
		for( x = 0; x < 2; x++ )
		{
			prvGetTestWIFINetwork(&xNetwork, x);
			xStatus = test_AppendNetwork( &xNetwork );
			TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		}

		/* Add real network */
		prvGetRealWIFINetwork( &xNetwork );
		xStatus = test_AddNewNetwork( &xNetwork );


		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );

		/*Verify the connected network */
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		TEST_ASSERT_EQUAL( pdTRUE, test_GetConnectedNetwork( &xNetwork ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );

		/* Change priority of connected network from 0 to 2 */
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ));

		/*Verify WiFi is connected and connected network remains same */
		memset(&xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
		TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
		TEST_ASSERT_EQUAL( pdTRUE, test_GetConnectedNetwork( &xNetwork ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );

	}

    WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteConnectedNetwork )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		/* Add real network */
		prvGetRealWIFINetwork( &xNetwork );
		xStatus = test_AddNewNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );

		/* Delete the WiFi Network */
		xStatus = test_PopNetwork( 0, NULL );
		/*Verify there is no connected network */
		TEST_ASSERT_EQUAL( pdFALSE, test_GetConnectedNetwork( &xNetwork ) );
	}

    WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_GetNumNetworks )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;

	WIFI_PROVISION_Start();

	if( TEST_PROTECT() )
	{
		//Add 1 network
		prvGetTestWIFINetwork(&xNetwork, 0);
		xStatus = test_AppendNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( 1, WIFI_PROVISION_GetNumNetworks() );

		//Add 2 networks
		prvGetTestWIFINetwork(&xNetwork, 1);
		xStatus = test_AppendNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( 2, WIFI_PROVISION_GetNumNetworks() );

		//Move Networks
		TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 1 ));
		TEST_ASSERT_EQUAL( 2, WIFI_PROVISION_GetNumNetworks() );

		//Delete Network 1
		xStatus = test_PopNetwork( 0, NULL );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( 1, WIFI_PROVISION_GetNumNetworks() );

		//Delete Network 2
		xStatus = test_PopNetwork( 0, NULL );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( 0, WIFI_PROVISION_GetNumNetworks() );
	}

	WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ConnectSavedNetwork )
{

	WIFINetworkProfile_t xNetwork = { 0 };
	WIFIReturnCode_t xStatus;
	BaseType_t xResult;
	int x;

	xStatus = WIFI_Disconnect();
	TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);

	WIFI_PROVISION_Start();
	if( TEST_PROTECT() )
	{

		/* Add dummy networks */
		for( x = 0; x < 2; x++ )
		{
			prvGetTestWIFINetwork(&xNetwork, x);
			xStatus = test_AppendNetwork( &xNetwork );
			TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		}

		/* Add real network */
		prvGetRealWIFINetwork( &xNetwork );
		xStatus = test_AppendNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus);
		TEST_ASSERT_EQUAL( 3, WIFI_PROVISION_GetNumNetworks() );

		/* Connect to real network */
		xResult = WIFI_PROVISION_Connect( 0 );
		TEST_ASSERT_EQUAL( pdTRUE, xResult );
		TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
		xResult = test_GetConnectedNetwork( &xNetwork );
		TEST_ASSERT_EQUAL( pdTRUE, xResult );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
		TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
		TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
		TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
	}

    WIFI_PROVISION_Stop();
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_Deinit )
{
	BaseType_t xStatus = WIFI_PROVISION_Delete();
	TEST_ASSERT_EQUAL( pdPASS, xStatus);
}

static void prvBLEInitialize( void )
{
	BTStatus_t xStatus;
	TEST_ASSERT_NOT_EQUAL( NULL, BTGetBluetoothInterface() );
	BTGetBluetoothInterface()->pxEnable( 0 );
	xStatus = BLE_Init( &xServerUUID, NULL, 0 );
	TEST_ASSERT_EQUAL( eBTStatusSuccess, xStatus );
}

static void prvBLEDeInitialize( void )
{
	TEST_ASSERT_NOT_EQUAL( NULL, BTGetBluetoothInterface() );
	BTGetBluetoothInterface()->pxDisable();
}

static void prvGetRealWIFINetwork( WIFINetworkProfile_t *pxNetwork )
{
	memcpy(pxNetwork->cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) );
	pxNetwork->ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
	memcpy(pxNetwork->cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ));
	pxNetwork->ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
	memset(pxNetwork->ucBSSID, 0x00, sizeof( pxNetwork->ucBSSID ));
	pxNetwork->xSecurity = clientcredentialWIFI_SECURITY;
}

static BaseType_t prvIsSameNetwork( WIFINetworkProfile_t *pxNetwork1,  WIFINetworkProfile_t *pxNetwork2 )
{
	BaseType_t xRet = pdFALSE;
	if( pxNetwork1->ucSSIDLength == pxNetwork2->ucSSIDLength )
	{
	    xRet = ( 0 == strncmp( pxNetwork1->cSSID, pxNetwork2->cSSID, pxNetwork2->ucSSIDLength ) );
	}
	return xRet;
}

static BaseType_t prvConnectRealNetwork( void )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFINetworkProfile_t xProfile;
    BaseType_t xResult = pdPASS;

    prvGetRealWIFINetwork( &xProfile );

    xNetworkParams.pcSSID = xProfile.cSSID;
    xNetworkParams.ucSSIDLength = xProfile.ucSSIDLength;
    xNetworkParams.pcPassword = xProfile.cPassword;
    xNetworkParams.ucPasswordLength = xProfile.ucPasswordLength;
    xNetworkParams.xSecurity = xProfile.xSecurity;


    xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );

    if( eWiFiSuccess != xWiFiStatus )
    {
        configPRINTF( ( "Could not connect to the access point.\r\n" ) );
        xResult = pdFAIL;
    }

    if( eWiFiSuccess == xWiFiStatus )
    {
        vTaskDelay( testWIFI_DELAY );

        if( pdTRUE != WIFI_IsConnected() )
        {
            configPRINTF( ( "Wi-Fi is not connected.\r\n" ) );
            xResult = pdFAIL;
        }
    }

    return xResult;
}

static void prvGetTestWIFINetwork( WIFINetworkProfile_t *pxNetwork, uint16_t usId )
{
	pxNetwork->ucSSIDLength = ( snprintf( pxNetwork->cSSID, sizeof(pxNetwork->cSSID), testNETWORK, usId ) + 1 );
	memset(pxNetwork->cPassword, 0x00, sizeof( pxNetwork->cPassword ));
	pxNetwork->ucPasswordLength = 0;
	memset(pxNetwork->ucBSSID, 0x00, sizeof( pxNetwork->ucBSSID ));
	pxNetwork->xSecurity = clientcredentialWIFI_SECURITY;
}

static void prvRemoveSavedNetworks( void )
{
	uint16_t usNumNetworks = wifiProvMAX_SAVED_NETWORKS;
	WIFIReturnCode_t xRet;
	WIFINetworkProfile_t xProfile;

	while( usNumNetworks > 0 )
	{
		xRet = WIFI_NetworkGet( &xProfile, 0 );
		if( xRet == eWiFiSuccess )
		{
			WIFI_NetworkDelete( 0 );
		}
		else
		{
			break;
		}
		usNumNetworks--;
	}
}
