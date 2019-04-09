/*
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
 */

/**
 * @file iot_demo_afr.c
 * @brief Generic demo runner for C SDK libraries on Amazon FreeRTOS.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif


#include "aws_clientcredential.h"
#include "iot_demo_logging.h"
#include "platform/iot_network_afr.h"
#include "iot_network_manager_private.h"
#include "platform/iot_threads.h"
#include "aws_demo.h"
#include "iot_common.h"
#include "iot_mqtt.h"

static IotNetworkManagerSubscription_t subscription = IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER;
/*-----------------------------------------------------------*/
/* C SDK demo functions. */

int RunMqttDemo( bool awsIotMqttMode,
                 const char * pIdentifier,
                 void * pNetworkServerInfo,
                 void * pNetworkCredentialInfo,
                 const IotNetworkInterface_t * pNetworkInterface );

int RunShadowDemo( bool awsIotMqttMode,
                   const char * pIdentifier,
                   void * pNetworkServerInfo,
                   void * pNetworkCredentialInfo,
                   const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

static uint32_t _getAvailableNetwork( demoContext_t *pContext )
{
    uint32_t network = AwsIotNetworkManager_GetConnectedNetworks();
    
    network = ( network & pContext->networkTypes );

    if( ( network & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        network =  AWSIOT_NETWORK_TYPE_WIFI;
    }
    else if( ( network & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        network = AWSIOT_NETWORK_TYPE_BLE;
    }
    else
    {
        network = AWSIOT_NETWORK_TYPE_NONE;
    }

    return network;
}

static void _onNetworkStateChangeCallback( uint32_t network,
                                           AwsIotNetworkState_t state,
                                           void * pContext )
{
    IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;
    const IotNetworkInterface_t * pNetworkInterface;
    bool awsIotMqttMode = false;
    uint32_t networkAvailable;

    demoContext_t * pDemoContext = ( demoContext_t * ) pContext;

    if( ( state == eNetworkStateEnabled ) && ( pDemoContext->connectedNetwork == AWSIOT_NETWORK_TYPE_NONE ) )
    {

        pDemoContext->connectedNetwork = network;
        IotSemaphore_Post(&pDemoContext->networkSemaphore);

        if (pDemoContext->networkConnectedCallback != NULL)
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface(network);
            awsIotMqttMode = (network != AWSIOT_NETWORK_TYPE_BLE);
            /* ALPN only works over port 443. Disable it otherwise. */
            if (serverInfo.port != 443)
            {
                credentials.pAlpnProtos = NULL;
            }

            IotLogInfo("OnConnected: %d", network);
            pDemoContext->networkConnectedCallback(awsIotMqttMode,
                                                   clientcredentialIOT_THING_NAME,
                                                   &serverInfo,
                                                   &credentials,
                                                   pNetworkInterface);
        }
    }
    else if( ( state == eNetworkStateDisabled ) && ( pDemoContext->connectedNetwork == network ) )
    {

        pDemoContext->connectedNetwork = AWSIOT_NETWORK_TYPE_NONE;
        if( pDemoContext->networkDisconnectedCallback != NULL )
        {
            IotLogInfo("OnDisconnected: %d", network);
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface(network);
            pDemoContext->networkDisconnectedCallback(pNetworkInterface);
        }

        networkAvailable = _getAvailableNetwork(pDemoContext);
        if( ( networkAvailable != AWSIOT_NETWORK_TYPE_NONE ) && ( pDemoContext->networkConnectedCallback != NULL ) )
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface(networkAvailable);
            awsIotMqttMode = (networkAvailable != AWSIOT_NETWORK_TYPE_BLE);

            /* ALPN only works over port 443. Disable it otherwise. */
            if (serverInfo.port != 443)
            {
                credentials.pAlpnProtos = NULL;
            }

            IotLogInfo("OnConnected: %d", networkAvailable);

            pDemoContext->networkConnectedCallback( awsIotMqttMode,
                                                   clientcredentialIOT_THING_NAME,
                                                   &serverInfo,
                                                   &credentials,
                                                   pNetworkInterface);
        }
    }
}

static uint32_t _blockForAvailableNetwork( demoContext_t * pContext )
{
    IotSemaphore_Wait( &pContext->networkSemaphore );
    return _getAvailableNetwork( pContext );
}



/**
 * @brief Initialize the common libraries, Mqtt library and network manager.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initializeDemo( demoContext_t* pContext )
{
    int status = EXIT_SUCCESS;
    bool commonLibrariesInitailized = false;
    bool mqttInitialized            = false;
    bool semaphoreCreated           = false;



    /* Initialize common libraries required by network manager and demo. */
    if( IotCommon_Init() == true )
    {
        commonLibrariesInitailized = true;
    }
    else
    {
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS)
    {
        if (AwsIotNetworkManager_Init() != pdTRUE)
        {
            status = EXIT_FAILURE;
        }
    }

    /* Enable all the transport type networks used by the demo application. */
    if(status == EXIT_SUCCESS)
    {
        if( AwsIotNetworkManager_EnableNetwork( pContext->networkTypes ) != pContext->networkTypes )
        {
            IotLogError("Failed to enable all the networks required by the demo application.");
            status = EXIT_FAILURE;
        }
    }
    
    if( status == EXIT_SUCCESS )
    {
        /* Initialize the MQTT library used by demo application to send messages to Iot broker. */
        if( IotMqtt_Init() == IOT_MQTT_SUCCESS )
        {
            mqttInitialized = true;
        }
        else
        {
            status = EXIT_FAILURE;
        }
        
    }

    if( status == EXIT_SUCCESS )
    {
        /* Create semaphore to signal that a network is available */
        if(  IotSemaphore_Create(&pContext->networkSemaphore, 0, 1) != true )
        {
            status = EXIT_FAILURE;
        }
        else
        {
            semaphoreCreated = true;
        }
        
    }

    if( status == EXIT_SUCCESS )
    {
        /* Subscribe for network state change from Network Manager */
        if( AwsIotNetworkManager_SubscribeForStateChange(
                pContext->networkTypes,
                _onNetworkStateChangeCallback,
                pContext,
                &subscription ) != pdTRUE )
        {
            IotLogError( "Failed to subscribe with network manager for network state change." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_FAILURE )
    {
        if( mqttInitialized == true )
        {
            IotMqtt_Cleanup();
        }

        if( commonLibrariesInitailized == true )
        {
            IotCommon_Cleanup();
        }

        if( semaphoreCreated == true )
        {
            IotSemaphore_Destroy( & pContext->networkSemaphore );
        }
    }
        
    return status;
}

/**
 * @brief Clean up the common libraries and the MQTT library.
 */
static void _cleanupDemo( demoContext_t *pContext )
{
     /* Remove network manager subscription */
     AwsIotNetworkManager_RemoveSubscription( subscription );
     IotSemaphore_Destroy(&pContext->networkSemaphore);
     IotMqtt_Cleanup();
     IotCommon_Cleanup();
}

void runDemoTask( void * pArgument )
{
    /* On Amazon FreeRTOS, credentials and server info are defined in a header
     * and set by the initializers. */
    
    demoContext_t * pContext = ( demoContext_t * ) pArgument;

    uint32_t network = AWSIOT_NETWORK_TYPE_NONE;
    const IotNetworkInterface_t * pNetworkInterface = NULL;
    IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;
 
    int status = EXIT_SUCCESS;
    bool awsIotMqttMode = false;
    bool demoInitialized = false;
 
    status = _initializeDemo( pContext );

    if( status == EXIT_SUCCESS )
    {
        demoInitialized = true;
        
        /* Get any connected network. */
        network = _getAvailableNetwork( pContext );
        
        if( network == AWSIOT_NETWORK_TYPE_NONE )
        {
            /* No connected Networks. Block for a network to be connected. */
            network = _blockForAvailableNetwork( pContext );
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Received a connected network */
        pContext->connectedNetwork = network;
        pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( network );


        if( network == AWSIOT_NETWORK_TYPE_WIFI )
        {            
            /* ALPN only works over port 443. Disable it otherwise. */
            if( serverInfo.port != 443 )
            {
                credentials.pAlpnProtos = NULL;
            }

            /* Set IOT Mqtt Mode to true to not disable keep alive for a TCP/IP network */
            awsIotMqttMode = true;
        }
        else if( network == AWSIOT_NETWORK_TYPE_BLE )
        {
            /* Set AWS Iot Mqtt mode to false to disable keep alive for Non TCP network like Bluetooth */
            awsIotMqttMode = false;
        }
        else
        {
            /* Other network types are not supported */
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Run the demo. */
        pContext->demoFunction( awsIotMqttMode,
                                    clientcredentialIOT_THING_NAME,
                                    &serverInfo,
                                    &credentials,
                                    pNetworkInterface );

        /* Report heap usage. */
        IotLogInfo( "Demo minimum ever free heap: %lu bytes.",
                    ( unsigned long ) xPortGetMinimumEverFreeHeapSize() );
    }

    if( demoInitialized  == true )
    {
        _cleanupDemo(pContext);
    }
   
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )

BaseType_t xApplicationDNSQueryHook( const char * pcName )
{
    BaseType_t xReturn;

    /* Determine if a name lookup is for this node.  Two names are given
     * to this node: that returned by pcApplicationHostnameHook() and that set
     * by mainDEVICE_NICK_NAME. */
    if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
    {
        xReturn = pdPASS;
    }
    else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
    {
        xReturn = pdPASS;
    }
    else
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}

#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */
/*-----------------------------------------------------------*/

/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
    configPRINTF( ( "ERROR: Malloc failed to allocate memory\r\n" ) );
    taskDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    configPRINTF( ( "ERROR: stack overflow with task %s\r\n", pcTaskName ) );
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}
