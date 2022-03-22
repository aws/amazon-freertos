/*
 * FreeRTOS V202203.00
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
 * @file iot_demo_freertos.c
 * @brief Generic demo runner for C SDK libraries on FreeRTOS.
 */

/* The config header is always included first. */
#include "iot_config.h"

#include <string.h>
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "iot_demo_logging.h"
#include "iot_network_manager_private.h"
#include "platform/iot_threads.h"
#include "aws_demo.h"
#include "iot_init.h"

static IotNetworkManagerSubscription_t subscription = IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER;

/* Semaphore used to wait for a network to be available. */
static IotSemaphore_t demoNetworkSemaphore;

/* Variable used to indicate the connected network. */
static uint32_t demoConnectedNetwork = AWSIOT_NETWORK_TYPE_NONE;

/*-----------------------------------------------------------*/

static uint32_t _getConnectedNetworkForDemo( demoContext_t * pDemoContext )
{
    uint32_t ret = ( AwsIotNetworkManager_GetConnectedNetworks() & pDemoContext->networkTypes );

    if( ( ret & AWSIOT_NETWORK_TYPE_WIFI ) == AWSIOT_NETWORK_TYPE_WIFI )
    {
        ret = AWSIOT_NETWORK_TYPE_WIFI;
    }
    else if( ( ret & AWSIOT_NETWORK_TYPE_BLE ) == AWSIOT_NETWORK_TYPE_BLE )
    {
        ret = AWSIOT_NETWORK_TYPE_BLE;
    }
    else if( ( ret & AWSIOT_NETWORK_TYPE_ETH ) == AWSIOT_NETWORK_TYPE_ETH )
    {
        ret = AWSIOT_NETWORK_TYPE_ETH;
    }
    else
    {
        ret = AWSIOT_NETWORK_TYPE_NONE;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static uint32_t _waitForDemoNetworkConnection( demoContext_t * pDemoContext )
{
    IotSemaphore_Wait( &demoNetworkSemaphore );

    return _getConnectedNetworkForDemo( pDemoContext );
}

/*-----------------------------------------------------------*/

static void _onNetworkStateChangeCallback( uint32_t network,
                                           AwsIotNetworkState_t state,
                                           void * pContext )
{
    const IotNetworkInterface_t * pNetworkInterface = NULL;
    void * pConnectionParams = NULL, * pCredentials = NULL;
    uint32_t disconnectedNetworks = AWSIOT_NETWORK_TYPE_NONE;

    demoContext_t * pDemoContext = ( demoContext_t * ) pContext;

    if( ( state == eNetworkStateConnected ) && ( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_NONE ) )
    {
        demoConnectedNetwork = network;
        IotSemaphore_Post( &demoNetworkSemaphore );

        /* Disable the disconnected networks to save power and reclaim any unused memory. */
        disconnectedNetworks = configENABLED_NETWORKS & ( ~demoConnectedNetwork );

        if( disconnectedNetworks != AWSIOT_NETWORK_TYPE_NONE )
        {
            AwsIotNetworkManager_DisableNetwork( disconnectedNetworks );
        }

        if( pDemoContext->networkConnectedCallback != NULL )
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( network );
            pConnectionParams = AwsIotNetworkManager_GetConnectionParams( network );
            pCredentials = AwsIotNetworkManager_GetCredentials( network ),

            pDemoContext->networkConnectedCallback( true,
                                                    clientcredentialIOT_THING_NAME,
                                                    pConnectionParams,
                                                    pCredentials,
                                                    pNetworkInterface );
        }
    }
    else if( ( state == eNetworkStateEnabled ) && ( demoConnectedNetwork == network ) )
    {
        if( pDemoContext->networkDisconnectedCallback != NULL )
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( network );
            pDemoContext->networkDisconnectedCallback( pNetworkInterface );
        }

        /* Re-enable all the networks for the demo for reconnection. */
        disconnectedNetworks = configENABLED_NETWORKS & ( ~demoConnectedNetwork );

        if( disconnectedNetworks != AWSIOT_NETWORK_TYPE_NONE )
        {
            AwsIotNetworkManager_EnableNetwork( disconnectedNetworks );
        }

        demoConnectedNetwork = _getConnectedNetworkForDemo( pDemoContext );

        if( demoConnectedNetwork != AWSIOT_NETWORK_TYPE_NONE )
        {
            if( pDemoContext->networkConnectedCallback != NULL )
            {
                pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );
                pConnectionParams = AwsIotNetworkManager_GetConnectionParams( demoConnectedNetwork );
                pCredentials = AwsIotNetworkManager_GetCredentials( demoConnectedNetwork );

                pDemoContext->networkConnectedCallback( true,
                                                        clientcredentialIOT_THING_NAME,
                                                        pConnectionParams,
                                                        pCredentials,
                                                        pNetworkInterface );
            }
        }
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Initialize the common libraries, Mqtt library and network manager.
 *
 * @return `EXIT_SUCCESS` if all libraries were successfully initialized;
 * `EXIT_FAILURE` otherwise.
 */
static int _initialize( demoContext_t * pContext )
{
    int status = EXIT_SUCCESS;
    bool commonLibrariesInitialized = false;
    bool semaphoreCreated = false;

    /* Initialize the C-SDK common libraries. This function must be called
     * once (and only once) before calling any other C-SDK function. */
    if( IotSdk_Init() == true )
    {
        commonLibrariesInitialized = true;
    }
    else
    {
        IotLogInfo( "Failed to initialize the common library." );
        status = EXIT_FAILURE;
    }

    if( status == EXIT_SUCCESS )
    {
        if( AwsIotNetworkManager_Init() != pdTRUE )
        {
            IotLogError( "Failed to initialize network manager library." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Create semaphore to signal that a network is available for the demo. */
        if( IotSemaphore_Create( &demoNetworkSemaphore, 0, 1 ) != true )
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
    if( status == EXIT_SUCCESS )
    {
        if( AwsIotNetworkManager_EnableNetwork( configENABLED_NETWORKS ) != configENABLED_NETWORKS )
        {
            IotLogError( "Failed to initialize all the networks configured for the device." );
            status = EXIT_FAILURE;
        }
    }

    if( status == EXIT_SUCCESS )
    {
        /* Wait for network configured for the demo to be initialized. */
        if( pContext->networkTypes != AWSIOT_NETWORK_TYPE_NONE )
        {
            demoConnectedNetwork = _getConnectedNetworkForDemo( pContext );

            if( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_NONE )
            {
                /* Network not yet initialized. Block for a network to be initialized. */
                IotLogInfo( "No networks connected for the demo. Waiting for a network connection. " );
                demoConnectedNetwork = _waitForDemoNetworkConnection( pContext );
            }
        }
    }

    if( status == EXIT_FAILURE )
    {
        if( semaphoreCreated == true )
        {
            IotSemaphore_Destroy( &demoNetworkSemaphore );
        }

        if( commonLibrariesInitialized == true )
        {
            IotSdk_Cleanup();
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

/**
 * @brief Clean up the common libraries and the MQTT library.
 */
static void _cleanup( void )
{
    /* Remove network manager subscription */
    AwsIotNetworkManager_RemoveSubscription( subscription );
    /* Disable all the networks used by the demo.*/
    AwsIotNetworkManager_DisableNetwork( configENABLED_NETWORKS );
    IotSemaphore_Destroy( &demoNetworkSemaphore );
    IotSdk_Cleanup();
}

/*-----------------------------------------------------------*/

void runDemoTask( void * pArgument )
{
    demoContext_t * pContext = ( demoContext_t * ) pArgument;
    const IotNetworkInterface_t * pNetworkInterface = NULL;
    void * pConnectionParams = NULL, * pCredentials = NULL;
    int status;


    #ifdef democonfigMEMORY_ANALYSIS
        democonfigMEMORY_ANALYSIS_STACK_DEPTH_TYPE xBeforeDemoTaskWaterMark, xAfterDemoTaskWaterMark = 0;
        xBeforeDemoTaskWaterMark = democonfigMEMORY_ANALYSIS_STACK_WATERMARK( NULL );

        size_t xBeforeDemoHeapSize, xAfterDemoHeapSize = 0;
        xBeforeDemoHeapSize = democonfigMEMORY_ANALYSIS_MIN_EVER_HEAP_SIZE();
    #endif /* democonfigMEMORY_ANALYSIS */


    /* DO NOT EDIT - This demo start marker is used in the test framework to
     * determine the start of a demo. */
    IotLogInfo( "---------STARTING DEMO---------\n" );

    status = _initialize( pContext );

    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "Successfully initialized the demo. Network type for the demo: %d", demoConnectedNetwork );

        pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );
        pConnectionParams = AwsIotNetworkManager_GetConnectionParams( demoConnectedNetwork );
        pCredentials = AwsIotNetworkManager_GetCredentials( demoConnectedNetwork );

        /* Run the demo. */
        status = pContext->demoFunction( true,
                                         clientcredentialIOT_THING_NAME,
                                         pConnectionParams,
                                         pCredentials,
                                         pNetworkInterface );

        #ifdef democonfigMEMORY_ANALYSIS
            /* If memory analysis is enabled metrics regarding the heap and stack usage of the demo will print. */
            /* This format is used for easier parsing and creates an avenue for future metrics to be added. */
            xAfterDemoHeapSize = democonfigMEMORY_ANALYSIS_MIN_EVER_HEAP_SIZE();
            IotLogInfo( "memory_metrics::freertos_heap::before::bytes::%u", xBeforeDemoHeapSize );
            IotLogInfo( "memory_metrics::freertos_heap::after::bytes::%u", xAfterDemoHeapSize );

            xAfterDemoTaskWaterMark = democonfigMEMORY_ANALYSIS_STACK_WATERMARK( NULL );
            IotLogInfo( "memory_metrics::demo_task_stack::before::bytes::%u", xBeforeDemoTaskWaterMark );
            IotLogInfo( "memory_metrics::demo_task_stack::after::bytes::%u", xAfterDemoTaskWaterMark );
        #endif /* democonfigMEMORY_ANALYSIS */

        /* Give a chance to drain the logging queue to increase the probability
         * of the following messages used by the test framework not getting
         * dropped. */
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );

        /* Log the demo status. */
        if( status == EXIT_SUCCESS )
        {
            /* DO NOT EDIT - This message is used in the test framework to
             * determine whether or not the demo was successful. */
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

    /* DO NOT EDIT - This demo end marker is used in the test framework to
     * determine the end of a demo. */
    IotLogInfo( "-------DEMO FINISHED-------\n" );
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

/* It is recommended to implement hooks that use platform specific APIs. This allows
 * for better error messages and recovery. Should platform specific hooks be implemented,
 * add this macro to iot_config.h to avoid compiling these symbols.*/
#ifndef iotconfigUSE_PORT_SPECIFIC_HOOKS

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
        configPRINT_STRING( ( "ERROR: Malloc failed to allocate memory\r\n" ) );
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
        configPRINT_STRING( ( "ERROR: stack overflow in task " ) );
        configPRINT_STRING( ( pcTaskName ) );
        configPRINT_STRING( ( "\r\n" ) );
        portDISABLE_INTERRUPTS();

        /* Unused Parameters */
        ( void ) xTask;
        ( void ) pcTaskName;

        /* Loop forever */
        for( ; ; )
        {
        }
    }
#endif /* iotconfigUSE_PORT_SPECIFIC_HOOKS */
/*-----------------------------------------------------------*/
