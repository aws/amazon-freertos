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
#include "iot_network_manager_private.h"
#include "platform/iot_threads.h"
#include "aws_demo.h"
#include "iot_init.h"

/* Remove dependency to MQTT */
#define MQTT_DEMO_TYPE_ENABLED (defined(CONFIG_MQTT_DEMO_ENABLED)||defined(CONFIG_SHADOW_DEMO_ENABLED)||defined(CONFIG_DEFENDER_DEMO_ENABLED)||defined(CONFIG_OTA_UPDATE_DEMO_ENABLED))

#if MQTT_DEMO_TYPE_ENABLED
#include "iot_mqtt.h"
#endif

static IotNetworkManagerSubscription_t subscription = IOT_NETWORK_MANAGER_SUBSCRIPTION_INITIALIZER;

/* Semaphore used to wait for a network to be available. */
static IotSemaphore_t demoNetworkSemaphore;

/* Variable used to indicate the connected network. */
static uint32_t demoConnectedNetwork = AWSIOT_NETWORK_TYPE_NONE;

#if MQTT_DEMO_TYPE_ENABLED
#if BLE_ENABLED 
extern const IotMqttSerializer_t IotBleMqttSerializer;
#endif


/*-----------------------------------------------------------*/

const IotMqttSerializer_t * demoGetMqttSerializer( void )
{
    const IotMqttSerializer_t * ret = NULL;

#if BLE_ENABLED
    if( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_BLE )
    {
        ret = &IotBleMqttSerializer;
    }
#endif

    return ret;
}
#endif
/*-----------------------------------------------------------*/

void generateDeviceIdentifierAndMetrics( void );

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
    void *pConnectionParams = NULL, *pCredentials = NULL;

    demoContext_t * pDemoContext = ( demoContext_t * ) pContext;

    if( ( state == eNetworkStateEnabled ) && ( demoConnectedNetwork == AWSIOT_NETWORK_TYPE_NONE ) )
    {
        demoConnectedNetwork = network;
        IotSemaphore_Post( &demoNetworkSemaphore );

        if( pDemoContext->networkConnectedCallback != NULL )
        {
            pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( network );
            pConnectionParams = AwsIotNetworkManager_GetConnectionParams( network );
            pCredentials      = AwsIotNetworkManager_GetCredentials( network ),
    
            pDemoContext->networkConnectedCallback( true,
                                                    clientcredentialIOT_THING_NAME,
                                                    pConnectionParams,
                                                    pCredentials,
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
                pConnectionParams = AwsIotNetworkManager_GetConnectionParams( demoConnectedNetwork );
                pCredentials      = AwsIotNetworkManager_GetCredentials( demoConnectedNetwork );
                
                pDemoContext->networkConnectedCallback( true,
                                                        clientcredentialIOT_THING_NAME,
                                                        pConnectionParams,
                                                        pCredentials,
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
static int _initialize( demoContext_t * pContext )
{
    int status = EXIT_SUCCESS;
    bool commonLibrariesInitailized = false;
    bool semaphoreCreated = false;

    /* Generate device identifier and device metrics. */
    generateDeviceIdentifierAndMetrics();

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
            IotLogInfo( "No networks connected for the demo. Waiting for a network connection. " );
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

/*-----------------------------------------------------------*/
void runDemoTask( void * pArgument )
{
    /* On Amazon FreeRTOS, credentials and server info are defined in a header
     * and set by the initializers. */

    demoContext_t * pContext = ( demoContext_t * ) pArgument;
    const IotNetworkInterface_t * pNetworkInterface = NULL;
    void *pConnectionParams = NULL, *pCredentials = NULL;
    int status;

    status = _initialize( pContext );

    if( status == EXIT_SUCCESS )
    {
        IotLogInfo( "Successfully initialized the demo. Network type for the demo: %d", demoConnectedNetwork );

        pNetworkInterface = AwsIotNetworkManager_GetNetworkInterface( demoConnectedNetwork );
        pConnectionParams = AwsIotNetworkManager_GetConnectionParams( demoConnectedNetwork );
        pCredentials      = AwsIotNetworkManager_GetCredentials( demoConnectedNetwork );

        /* Run the demo. */
        status = pContext->demoFunction( true,
                                clientcredentialIOT_THING_NAME,
                                pConnectionParams,
                                pCredentials,
                                pNetworkInterface );

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
    ( void ) xTask;

    /* Loop forever */
    for( ; ; )
    {
    }
}

/*
 * @brief Total length of the hash in bytes.
 */
#define _MD5_HASH_LENGTH_BYTES    ( 16 )

/*
 * @brief Length in 32-bit words of each chunk used for hash computation .
 */
#define _MD5_CHUNK_LENGTH_WORDS   ( 16 )

/*
 * Encode Length of one byte.
*/
#define _BYTE_ENCODE_LENGTH      ( 2 )

/*
 * @brief Length in bytes of each chunk used for hash computation.
 */
#define _MD5_CHUNK_LENGTH_BYTES   ( _MD5_CHUNK_LENGTH_WORDS * 4 )

#define LEFT_ROTATE( x, c ) ( ( x << c ) | ( x >> ( 32 - c ) ) )

static const char S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

static const unsigned int K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
};

/*
 * @brief MD5 hashing algorithm to generate unique identifier for a sequeunce of bytes.
 * The hash algorithm is adapted from wikipedia and does not dependent on any third party libraries.
 */
static void _generateHash( const char *pData, size_t dataLength, uint8_t *pHash, size_t hashLength )
{
    uint32_t A, B, C, D, F, G;
    uint32_t chunk[ _MD5_CHUNK_LENGTH_WORDS ] = { 0 };
    const uint32_t *pCurrent = NULL;
    uint32_t *pOutput = ( uint32_t * )pHash;
    size_t i;

    //Initialize variables
    pOutput[ 0 ] = 0x67452301;
    pOutput[ 1 ] = 0xefcdab89;
    pOutput[ 2 ] = 0x98badcfe;
    pOutput[ 3 ] = 0x10325476;

    configASSERT( hashLength >= _MD5_HASH_LENGTH_BYTES );

    while( dataLength > 0 )
    {
        A = pOutput[ 0 ];
        B = pOutput[ 1 ];
        C = pOutput[ 2 ];
        D = pOutput[ 3 ];

        F = G = 0;

        if( dataLength < _MD5_CHUNK_LENGTH_BYTES )
        {
            memcpy( chunk, pData, dataLength );
            pCurrent = chunk;
            pData += _MD5_CHUNK_LENGTH_BYTES;
            dataLength = 0;
        }
        else
        {
            pCurrent = ( uint32_t * ) pData;
            pData += _MD5_CHUNK_LENGTH_BYTES;
            dataLength -= _MD5_CHUNK_LENGTH_BYTES;
        }

        for( i = 0; i < 64; i++ )
        {
            if( i < 16 )
            {
                F = ( B & C ) | ( ( ~B ) & D );
                G = i;
            }
            else if( i < 32 )
            {
                F = ( D & B ) | ( ( ~D ) & C );
                G = ( ( 5 * i ) + 1 ) % 16;
            }
            else if( i < 48 )
            {
                F = ( B ^ C ) ^ D;
                G = ( ( 3 * i ) + 5 ) % 16;
            }
            else
            {
                F = C ^ ( B | ( ~D ) );
                G = ( 7 * i ) % 16;
            }

            F = F + A + K[ i ] + pCurrent[ G ];
            A = D;
            D = C;
            C = B;
            B = B + LEFT_ROTATE( F, S[ i ] );
        }

        pOutput[ 0 ] += A;
        pOutput[ 1 ] += B;
        pOutput[ 2 ] += C;
        pOutput[ 3 ] += D;
    }
}

/*
 * @brief Length of device identifier.
 * Device identifier is represented as hex string of MD5 hash of the device certificate.
 */
#define AWS_IOT_DEVICE_IDENTIFIER_LENGTH ( _MD5_HASH_LENGTH_BYTES * 2 )

/**
 * @brief Device metrics name format
 */
#define AWS_IOT_METRICS_NAME "?SDK=" IOT_SDK_NAME "&Version=4.0.0&Platform=" IOT_PLATFORM_NAME "&AFRDevID=%.*s"

 /**
  * @brief Length of #AWS_IOT_METRICS_NAME.
  */
#define AWS_IOT_METRICS_NAME_LENGTH    ( ( uint16_t ) ( sizeof( AWS_IOT_METRICS_NAME ) + AWS_IOT_DEVICE_IDENTIFIER_LENGTH ) )

/**
 * @brief Unique identifier for the device.
 */
static char deviceIdentifier[ AWS_IOT_DEVICE_IDENTIFIER_LENGTH + 1] = { 0 };

/*
  @brief Device metrics sent to the cloud.
 */
static char deviceMetrics[AWS_IOT_METRICS_NAME_LENGTH + 1] = { 0 };


static void _generateDeviceMetrics(void)
{
    size_t length;
    length = snprintf( deviceMetrics,
        AWS_IOT_METRICS_NAME_LENGTH,
        AWS_IOT_METRICS_NAME,
        AWS_IOT_DEVICE_IDENTIFIER_LENGTH,
        deviceIdentifier );
    configASSERT(length > 0);
}

/*
 * @brief Generates a unique identifier for the device and metrics sent by the device to cloud.
 * Function should only be called once at intialization.
 */
void generateDeviceIdentifierAndMetrics( void )
{
    const char *pCert = keyCLIENT_CERTIFICATE_PEM;
    size_t certLength = strlen( keyCLIENT_CERTIFICATE_PEM );
    uint8_t hash[ _MD5_HASH_LENGTH_BYTES ] = { 0 };
    char *pBuffer = deviceIdentifier;
    int i;
    size_t ret;

    if ( ( NULL == pCert ) || ( 0 == strcmp("", pCert ) ) )
    {
        configPRINTF(( "ERROR: Generating device identifier and metrics requires a valid certificate.\r\n" ));
        /* Duplicating the check in assert to force compiler to not optimize it. */
        configASSERT( ( NULL != pCert ) && ( 0 != strcmp( "", pCert ) ) );
    }
    else
    {
        _generateHash(pCert, certLength, hash, _MD5_HASH_LENGTH_BYTES);

        for( i = 0; i < _MD5_HASH_LENGTH_BYTES; i++ )
        {
            ret = snprintf( pBuffer, ( _BYTE_ENCODE_LENGTH + 1 ), "%02X", hash[ i ] );
            configASSERT( ret > 0 );

            pBuffer += _BYTE_ENCODE_LENGTH;
        }

        _generateDeviceMetrics();
    }
}

/*
 * @brief Retrieves the unique identifier for the device.
 */
const char *getDeviceIdentifier( void )
{
    return deviceIdentifier;
}

/*
 * @brief Retrieves the device metrics to be sent to cloud.
 */
const char *getDeviceMetrics( void )
{
    return deviceMetrics;
}