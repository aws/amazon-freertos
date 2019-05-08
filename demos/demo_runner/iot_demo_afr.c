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

/* The config header is always included first. */
#include "iot_config.h"


#include "aws_clientcredential.h"
#include "iot_demo_logging.h"
#include "platform/iot_network_afr.h"
#include "iot_network_manager_private.h"
#include "platform/iot_threads.h"
#include "aws_demo.h"
#include "iot_init.h"
#include "iot_mqtt.h"

/* Server info, End point and port #. */
static const IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;

/* Secure connection info. */
static IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

static IotNetworkManagerSubscription_t subscription = IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER;

/* Semaphore used to wait for a network to be available. */
static IotSemaphore_t demoNetworkSemaphore;

/* Variable used to indicate the connected network. */
static uint32_t demoConnectedNetwork = AWSIOT_NETWORK_TYPE_NONE;


#if BLE_ENABLED
extern const IotMqttSerializer_t IotBleMqttSerializer;
#endif

const IotMqttSerializer_t* getSerializerOverride( void )
{
    const IotMqttSerializer_t* ret = NULL;

#if BLE_ENABLED
    if( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_BLE )
    {
        ret = &IotBleMqttSerializer;
    }
#endif

    return ret;
}

/*-----------------------------------------------------------*/

static uint32_t _getConnectedNetworkForDemo( demoContext_t *pDemoContext )
{
    uint32_t ret = ( AwsIotNetworkManager_GetConnectedNetworks() & pDemoContext->networkTypes );

    if( ( ret & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        ret =  AWSIOT_NETWORK_TYPE_WIFI;
    }
    else if( ( ret & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        ret = AWSIOT_NETWORK_TYPE_BLE;
    }
    else if ( ( ret & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH )
    {
        ret = AWSIOT_NETWORK_TYPE_ETH;
    }
    else
    {
        ret = AWSIOT_NETWORK_TYPE_NONE;
    }

    return ret;
}

static uint32_t _waitForDemoNetworkConnection( demoContext_t * pDemoContext )
{
    IotSemaphore_Wait( &demoNetworkSemaphore );

    return _getConnectedNetworkForDemo( pDemoContext );
}


static void _onNetworkStateChangeCallback( uint32_t network,
                                           AwsIotNetworkState_t state,
                                           void * pContext )
{
    const IotNetworkInterface_t * pNetworkInterface;
    bool awsIotMqttMode = false;

    demoContext_t * pDemoContext = ( demoContext_t * ) pContext;

    if( ( state == eNetworkStateEnabled ) && ( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_NONE ) )
    {

        demoConnectedNetwork = network;
        IotSemaphore_Post( &demoNetworkSemaphore);

        if (pDemoContext->networkConnectedCallback != NULL)
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );

            /* ALPN only works over port 443. Disable it otherwise. */
            if( serverInfo.port != 443 )
            {
                credentials.pAlpnProtos = NULL;
            }

            awsIotMqttMode = ( demoConnectedNetwork != AWSIOT_NETWORK_TYPE_BLE );

            pDemoContext->networkConnectedCallback( awsIotMqttMode,
                                                    clientcredentialIOT_THING_NAME,
                                                    &serverInfo,
                                                    &credentials,
                                                    pNetworkInterface );
        }
    }
    else if( ( state == eNetworkStateDisabled ) && ( demoConnectedNetwork == network ) )
    {

        if( pDemoContext->networkDisconnectedCallback != NULL )
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( network );
            pDemoContext->networkDisconnectedCallback( pNetworkInterface );
        }

        demoConnectedNetwork = _getConnectedNetworkForDemo( pDemoContext );
        if( demoConnectedNetwork != AWSIOT_NETWORK_TYPE_NONE )
        {

            if( pDemoContext->networkConnectedCallback != NULL )
            {
                pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );

                /* ALPN only works over port 443. Disable it otherwise. */
                if( serverInfo.port != 443 )
                {
                    credentials.pAlpnProtos = NULL;
                }

                awsIotMqttMode = ( demoConnectedNetwork != AWSIOT_NETWORK_TYPE_BLE );

                pDemoContext->networkConnectedCallback( awsIotMqttMode,
                                                        clientcredentialIOT_THING_NAME,
                                                        &serverInfo,
                                                        &credentials,
                                                        pNetworkInterface );
            }
        }
    }
}


/**
 * @brief Initialize the common libraries, Mqtt library and network manager.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initialize( demoContext_t* pContext )
{
    int status = EXIT_SUCCESS;
    bool commonLibrariesInitailized = false;
    bool semaphoreCreated           = false;

    /* Initialize common libraries required by network manager and demo. */
    if( IotSdk_Init() == true )
    {
        commonLibrariesInitailized = true;
    }
    else
    {
        IotLogInfo( "Failed to initialize the common library." );
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS)
    {
        if (AwsIotNetworkManager_Init() != pdTRUE)
        {
            IotLogError( "Failed to initialize network manager library." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Create semaphore to signal that a network is available for the demo. */
        if(  IotSemaphore_Create( &demoNetworkSemaphore, 0, 1) != true )
        {
            IotLogError( "Failed to create semaphore to wait for a network connection." );
            status = EXIT_FAILURE;
        }
        else
        {
            semaphoreCreated = true;
        }

    }

    if( status == EXIT_SUCCESS )
    {
        /* Subscribe for network state change from Network Manager. */
        if( AwsIotNetworkManager_SubscribeForStateChange( pContext->networkTypes,
                                                          _onNetworkStateChangeCallback,
                                                          pContext,
                                                          &subscription ) != pdTRUE )
        {
            IotLogError( "Failed to subscribe network state change callback." );
            status = EXIT_FAILURE;
        }
    }

    /* Initialize all the  networks configured for the device. */
    if(status == EXIT_SUCCESS)
    {
        if( AwsIotNetworkManager_EnableNetwork( configENABLED_NETWORKS ) != configENABLED_NETWORKS )
        {
            IotLogError( "Failed to intialize all the networks configured for the device." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Wait for network configured for the demo to be initialized. */
        demoConnectedNetwork = _getConnectedNetworkForDemo( pContext );

        if( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_NONE )
        {
            /* Network not yet initialized. Block for a network to be intialized. */
            IotLogInfo( "No networks connected for the demo. Waiting for a network connection. ");
            demoConnectedNetwork = _waitForDemoNetworkConnection( pContext );
        }
    }

    if( status == EXIT_FAILURE )
    {

        if( semaphoreCreated == true )
        {
            IotSemaphore_Destroy( &demoNetworkSemaphore );
        }

        if( commonLibrariesInitailized == true )
        {
            IotSdk_Cleanup();
        }
    }

    return status;
}

/**
 * @brief Clean up the common libraries and the MQTT library.
 */
static void _cleanup( void )
{
     /* Remove network manager subscription */
     AwsIotNetworkManager_RemoveSubscription( subscription );
     IotSemaphore_Destroy( &demoNetworkSemaphore );
     IotSdk_Cleanup();
}

void runDemoTask( void * pArgument )
{
    /* On Amazon FreeRTOS, credentials and server info are defined in a header
     * and set by the initializers. */

    demoContext_t * pContext = ( demoContext_t * ) pArgument;
    const IotNetworkInterface_t * pNetworkInterface = NULL;
    IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

    int status = EXIT_SUCCESS;
    bool awsIotMqttMode = false;

    status = _initialize( pContext );

    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "Successfully initialized the demo. Network type for the demo: %d", demoConnectedNetwork );

        pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );

        /* ALPN only works over port 443. Disable it otherwise. */
        if( serverInfo.port != 443 )
        {
            credentials.pAlpnProtos = NULL;
        }

        /* Set AWS Iot Mqtt mode to false to disable keep alive for non TCP/IP networks like bluetooth */
        awsIotMqttMode = ( demoConnectedNetwork != AWSIOT_NETWORK_TYPE_BLE );

        /* Run the demo. */
        status = pContext->demoFunction( awsIotMqttMode,
                                         clientcredentialIOT_THING_NAME,
                                         &serverInfo,
                                         &credentials,
                                         pNetworkInterface );

        /* Report heap usage. */
        IotLogInfo( "Demo minimum ever free heap: %lu bytes.",
                    ( unsigned long ) xPortGetMinimumEverFreeHeapSize() );

        /* Log the demo status. */
        if( status == EXIT_SUCCESS )
        {
            IotLogInfo( "Demo completed successfully." );
        }
        else
        {
            IotLogError( "Error running demo." );
        }

        _cleanup();
    }
    else
    {
        IotLogError( "Failed to initialize the demo. exiting..." );
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

	/* Unused Parameters */
	( void )xTask;

    /* Loop forever */
    for( ; ; )
    {
    }
}
