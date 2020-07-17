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
 * @file iot_test_wifi_provisioning.c
 * @brief Tests for Wifi provisioning.
 */

#include "iot_config.h"

/* C standard library includes. */
#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"
#include "iot_ble_wifi_provisioning.h"
#include "iot_ble_wifi_prov_test_access_declare.h"
#include "iot_ble_data_transfer.h"
#include "aws_clientcredential.h"
/* Test framework includes. */
#include "unity_fixture.h"
#include "unity.h"

static void prvRemoveSavedNetworks( void );
static void prvGetRealWIFINetwork( WIFINetworkProfile_t * pxNetwork );
static void prvGetTestWIFINetwork( WIFINetworkProfile_t * pxNetwork,
                                   uint16_t usId );
static bool prvIsSameNetwork( WIFINetworkProfile_t * pxNetwork1,
                              WIFINetworkProfile_t * pxNetwork2 );
static bool prvConnectRealNetwork( void );


#define testNETWORK              "testNetwork%d"
#define testWIFI_MAX_RETRY       5
#define testMAXWIFI_WAIT_TIME    pdMS_TO_TICKS( 10000 )
#define testWIFI_DELAY           pdMS_TO_TICKS( 2000 )

TEST_GROUP( Full_WiFi_Provisioning );

/*-----------------------------------------------------------*/

#include "bt_hal_manager_types.h"
static bool bleInitialized = false;
static bool iotBleInitialized = false;
static bool iotBleWifiProvisioningInitialized = false;
extern BTStatus_t bleStackInit( void );

static bool bleInit()
{
    if( !bleInitialized )
    {
        bleInitialized = ( eBTStatusSuccess == bleStackInit() );
    }

    if( bleInitialized && !iotBleInitialized )
    {
        if( IotBle_Init() == eBTStatusSuccess )
        {
            if( IotBle_On() == eBTStatusSuccess )
            {
                iotBleInitialized = true;
            }
        }
    }

    return iotBleInitialized;
}

/*-----------------------------------------------------------*/

TEST_SETUP( Full_WiFi_Provisioning )
{
    if( bleInit() )
    {
        iotBleWifiProvisioningInitialized = IotBleWifiProv_Init();
    }

    TEST_ASSERT_EQUAL( bleInitialized, true );
    TEST_ASSERT_EQUAL( iotBleInitialized, true );
    TEST_ASSERT_EQUAL( iotBleWifiProvisioningInitialized, true );
}

/*-----------------------------------------------------------*/

TEST_TEAR_DOWN( Full_WiFi_Provisioning )
{
    if( iotBleWifiProvisioningInitialized )
    {
        IotBleWifiProv_Deinit();
        prvRemoveSavedNetworks();
        iotBleWifiProvisioningInitialized = false;
    }
}


TEST_GROUP_RUNNER( Full_WiFi_Provisioning )
{
    prvRemoveSavedNetworks();

    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_AddNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_GetSavedNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeNetworkPriority );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeConnectedNetworkPriority );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteConnectedNetwork );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_GetNumNetworks );
    RUN_TEST_CASE( Full_WiFi_Provisioning, WIFI_PROVISION_ConnectSavedNetwork );

    prvRemoveSavedNetworks();

    if( TEST_PROTECT() )
    {
        TEST_ASSERT_EQUAL( pdPASS, prvConnectRealNetwork() );
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_AddNetwork )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;

    if( TEST_PROTECT() )
    {
        prvGetRealWIFINetwork( &xNetwork );
        xStatus = test_AddNewNetwork( &xNetwork, true );

        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        /* Verify that the network is connected */
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
        TEST_ASSERT_EQUAL( true, test_GetConnectedNetwork( &xNetwork ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );

        /* Verify that network is saved */
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_GetSavedNetwork )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;


    if( TEST_PROTECT() )
    {
        prvGetRealWIFINetwork( &xNetwork );
        xStatus = test_AppendNetwork( &xNetwork );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_GetSavedNetwork( 0, &xNetwork ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
    }
}


TEST( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteNetwork )
{
    WIFINetworkProfile_t xNetwork;
    WIFIReturnCode_t xStatus;

    prvGetRealWIFINetwork( &xNetwork );

    if( TEST_PROTECT() )
    {
        /* Add the new network */
        xStatus = test_AddNewNetwork( &xNetwork, true );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        /* Verify that network is added */
        TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ) );

        /* Delete the network  */
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        xStatus = test_PopNetwork( 0, &xNetwork );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_NOT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
    }
}


TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeNetworkPriority )
{
    WIFINetworkProfile_t xNetwork = { 0 }, xTestProfile = { 0 };
    WIFIReturnCode_t xStatus;
    int x;

    if( TEST_PROTECT() )
    {
        /* Add 3 dummy networks */
        for( x = 0; x < 3; x++ )
        {
            prvGetTestWIFINetwork( &xNetwork, x );
            xStatus = test_AppendNetwork( &xNetwork );
            TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        }

        /* Verify the order of the networks */
        for( x = 0; x < 3; x++ )
        {
            prvGetTestWIFINetwork( &xTestProfile, x );
            TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
            xStatus = prvIsSameNetwork( &xNetwork, &xTestProfile );
            TEST_ASSERT_EQUAL( true, xStatus );
        }

        /* Change priority of network from 0 to 2 */
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ) );

        /* Verify the Network is at storage index 0  */
        prvGetTestWIFINetwork( &xTestProfile, 2 );
        TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 0 ) );
        xStatus = prvIsSameNetwork( &xNetwork, &xTestProfile );
        TEST_ASSERT_EQUAL( true, xStatus );

        /*Verify Network1 is at storage index 2 */
        prvGetTestWIFINetwork( &xTestProfile, 1 );
        TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, 2 ) );
        TEST_ASSERT_EQUAL( true, prvIsSameNetwork( &xNetwork, &xTestProfile ) );

        /* Change priority from 0 to  2  twice */
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ) );
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ) );

        /* Verify its back to the initial order */
        for( x = 0; x < 3; x++ )
        {
            prvGetTestWIFINetwork( &xTestProfile, x );
            TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
            TEST_ASSERT_EQUAL( true, prvIsSameNetwork( &xNetwork, &xTestProfile ) );
        }

        /* Move network 0 to network 1  and network 1 to network 0 */
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 1 ) );
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 1, 0 ) );

        /* Verify the order remains the same */
        for( x = 0; x < 3; x++ )
        {
            prvGetTestWIFINetwork( &xTestProfile, x );
            TEST_ASSERT_EQUAL( eWiFiSuccess, WIFI_NetworkGet( &xNetwork, x ) );
            TEST_ASSERT_EQUAL( true, prvIsSameNetwork( &xNetwork, &xTestProfile ) );
        }
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ChangeConnectedNetworkPriority )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;
    int x;

    if( TEST_PROTECT() )
    {
        /* Add 2 dummy networks */
        for( x = 0; x < 2; x++ )
        {
            prvGetTestWIFINetwork( &xNetwork, x );
            xStatus = test_AppendNetwork( &xNetwork );
            TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        }

        /* Add real network */
        prvGetRealWIFINetwork( &xNetwork );
        xStatus = test_AddNewNetwork( &xNetwork, true );


        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );

        /*Verify the connected network */
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        TEST_ASSERT_EQUAL( true, test_GetConnectedNetwork( &xNetwork ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );

        /* Change priority of connected network from 0 to 2 */
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 2 ) );

        /*Verify WiFi is connected and connected network remains same */
        memset( &xNetwork, 0x00, sizeof( WIFINetworkProfile_t ) );
        TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
        TEST_ASSERT_EQUAL( true, test_GetConnectedNetwork( &xNetwork ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_DeleteConnectedNetwork )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;

    if( TEST_PROTECT() )
    {
        /* Add real network */
        prvGetRealWIFINetwork( &xNetwork );
        xStatus = test_AddNewNetwork( &xNetwork, true );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );

        /* Delete the WiFi Network */
        xStatus = test_PopNetwork( 0, NULL );
        /*Verify there is no connected network */
        TEST_ASSERT_EQUAL( false, test_GetConnectedNetwork( &xNetwork ) );
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_GetNumNetworks )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;

    if( TEST_PROTECT() )
    {
        /*Add 1 network */
        prvGetTestWIFINetwork( &xNetwork, 0 );
        xStatus = test_AppendNetwork( &xNetwork );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( 1, IotBleWifiProv_GetNumNetworks() );

        /*Add 2 networks */
        prvGetTestWIFINetwork( &xNetwork, 1 );
        xStatus = test_AppendNetwork( &xNetwork );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( 2, IotBleWifiProv_GetNumNetworks() );

        /*Move Networks */
        TEST_ASSERT_EQUAL( eWiFiSuccess, test_MoveNetwork( 0, 1 ) );
        TEST_ASSERT_EQUAL( 2, IotBleWifiProv_GetNumNetworks() );

        /*Delete Network 1 */
        xStatus = test_PopNetwork( 0, NULL );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( 1, IotBleWifiProv_GetNumNetworks() );

        /*Delete Network 2 */
        xStatus = test_PopNetwork( 0, NULL );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( 0, IotBleWifiProv_GetNumNetworks() );
    }
}

TEST( Full_WiFi_Provisioning, WIFI_PROVISION_ConnectSavedNetwork )
{
    WIFINetworkProfile_t xNetwork = { 0 };
    WIFIReturnCode_t xStatus;
    bool xResult;
    int x;

    xStatus = WIFI_Disconnect();
    TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );

    if( TEST_PROTECT() )
    {
        /* Add dummy networks */
        for( x = 0; x < 2; x++ )
        {
            prvGetTestWIFINetwork( &xNetwork, x );
            xStatus = test_AppendNetwork( &xNetwork );
            TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        }

        /* Add real network */
        prvGetRealWIFINetwork( &xNetwork );
        xStatus = test_AppendNetwork( &xNetwork );
        TEST_ASSERT_EQUAL( eWiFiSuccess, xStatus );
        TEST_ASSERT_EQUAL( 3, IotBleWifiProv_GetNumNetworks() );

        /* Connect to real network */
        xResult = IotBleWifiProv_Connect( 0 );
        TEST_ASSERT_EQUAL( true, xResult );
        TEST_ASSERT_EQUAL( pdTRUE, WIFI_IsConnected() );
        xResult = test_GetConnectedNetwork( &xNetwork );
        TEST_ASSERT_EQUAL( true, xResult );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetwork.ucSSIDLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
        TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_PASSWORD ), xNetwork.ucPasswordLength );
        TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetwork.cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) ) );
        TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetwork.xSecurity );
    }
}

static void prvGetRealWIFINetwork( WIFINetworkProfile_t * pxNetwork )
{
    memcpy( pxNetwork->cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) );
    pxNetwork->ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    memcpy( pxNetwork->cPassword, clientcredentialWIFI_PASSWORD, sizeof( clientcredentialWIFI_PASSWORD ) );
    pxNetwork->ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    memset( pxNetwork->ucBSSID, 0x00, sizeof( pxNetwork->ucBSSID ) );
    pxNetwork->xSecurity = clientcredentialWIFI_SECURITY;
}

static bool prvIsSameNetwork( WIFINetworkProfile_t * pxNetwork1,
                              WIFINetworkProfile_t * pxNetwork2 )
{
    bool xRet = false;

    if( pxNetwork1->ucSSIDLength == pxNetwork2->ucSSIDLength )
    {
        xRet = ( 0 == strncmp( pxNetwork1->cSSID, pxNetwork2->cSSID, pxNetwork2->ucSSIDLength ) );
    }

    return xRet;
}

static bool prvConnectRealNetwork( void )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams = { 0 };
    WIFINetworkProfile_t xProfile;
    bool xResult = pdPASS;
    uint32_t retry = 0;

    prvGetRealWIFINetwork( &xProfile );

    xNetworkParams.pcSSID = xProfile.cSSID;
    xNetworkParams.ucSSIDLength = xProfile.ucSSIDLength;
    xNetworkParams.pcPassword = xProfile.cPassword;
    xNetworkParams.ucPasswordLength = xProfile.ucPasswordLength;
    xNetworkParams.xSecurity = xProfile.xSecurity;

    do
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        retry++;
    } while( ( eWiFiSuccess != xWiFiStatus ) && ( retry < testWIFI_MAX_RETRY ) );

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

static void prvGetTestWIFINetwork( WIFINetworkProfile_t * pxNetwork,
                                   uint16_t usId )
{
    pxNetwork->ucSSIDLength = ( snprintf( pxNetwork->cSSID, sizeof( pxNetwork->cSSID ), testNETWORK, usId ) + 1 );
    memset( pxNetwork->cPassword, 0x00, sizeof( pxNetwork->cPassword ) );
    pxNetwork->ucPasswordLength = 0;
    memset( pxNetwork->ucBSSID, 0x00, sizeof( pxNetwork->ucBSSID ) );
    pxNetwork->xSecurity = clientcredentialWIFI_SECURITY;
}

static void prvRemoveSavedNetworks( void )
{
    uint16_t usNumNetworks = IOT_BLE_WIFI_PROVISIONING_MAX_SAVED_NETWORKS;
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
