/*
 * FreeRTOS V1.4.7
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

/* Library includes */
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#define CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED

/* AWS library includes. */
#include "iot_system_init.h"
#include "iot_logging_task.h"
#include "iot_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"
#include "iot_secure_sockets.h"
#include "wiced_rtos.h"

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* Unit test defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME                "cypress_demo"

/* DNS addresses used seed in PRNG seed generation
 * 7 total addresses to for 7 DNS queries
 * TODO Define these addresses as macros (configXYZ) */
static const char * pcDnsNames[] =
{
    "www.amazon.com",
    "aws.amazon.com",
    "www.imdb.com",
    "www.zappos.com",
    "www.amazon.co.uk",
    "www.amazon.co.in",
    "www.a9.com"
};

/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );


/**
 * @brief Generate Random Seed.
 * The random number seed generation solution presented in this function is
 * for demonstration purposes only. It is not recommended to go into production with
 * the logic presented here. The current solution takes entropy from the network latency.
 * For production development, it is recommended to use a source which will be
 * truly random in nature.
 *
 */
static void prvGenerateRandomSeed( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

uint32_t ulSeedValid = 0;
uint32_t ulSeed = 0;
static TaskHandle_t system_monitor_thread_handle;
void system_monitor_thread_main( wiced_thread_arg_t arg );
/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialization( void )
{
    /* Perform any hardware initializations, that don't require the RTOS to be
     * running, here.
     */
    configPRINT_STRING( ( "Test Message" ) );
    #ifndef WICED_DISABLE_WATCHDOG
        xTaskCreate( ( TaskFunction_t ) system_monitor_thread_main, "system monitor", 512 / sizeof( portSTACK_TYPE ), NULL, RTOS_HIGHEST_PRIORITY, &system_monitor_thread_handle );
    #endif
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    /* Perform any hardware initialization, that require the RTOS to be
     * running, here. */
    wiced_init();

    /* Initialize the AWS Libraries system. */
    if( SYSTEM_Init() == pdPASS )
    {
        /* Generate seed for PRNG */
        prvGenerateRandomSeed();

        /* Provision the device with AWS certificate and private key. */
        vDevModeKeyProvisioning();

        /* Start the demo tasks. */
        DEMO_RUNNER_RunDemos();
    }
}

/*-----------------------------------------------------------*/
static void prvGenerateRandomSeed()
{
    uint8_t ucAdddressCount = sizeof( pcDnsNames ) / sizeof( char * );
    TickType_t xTickCount = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );

    /* Populate first 4 bits based on the current clock after wifi init */
    ulSeed |= xTickCount & 0x0F;

    /* Populate remaining bits after 7 DNS queries */
    for( uint8_t ucCount = 0; ucCount < ucAdddressCount; ucCount++ )
    {
        SOCKETS_GetHostByName( pcDnsNames[ ucCount ] );
        xTickCount = platform_tick_get_time( PLATFORM_TICK_GET_SLOW_TIME_STAMP );
        ulSeed <<= 4UL;
        ulSeed |= xTickCount & 0x0F;
    }

    ulSeedValid = 1;
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined Idle task function.
 *
 * @note Do not make any blocking operations in this function.
 */
void vApplicationIdleHook( void )
{
    /* FIX ME. If necessary, update to application idle periodic actions. */

    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined application hook to process names returned by the DNS server.
 */
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        /* FIX ME. If necessary, update to applicable DNS name lookup actions. */

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
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled( const char * pcFile,
                    uint32_t ulLine )
{
    /* FIX ME. If necessary, update to applicable assertion routine actions. */

    const uint32_t ulLongSleep = 1000UL;
    volatile uint32_t ulBlockVariable = 0UL;
    volatile char * pcFileName = ( volatile char * ) pcFile;
    volatile uint32_t ulLineNumber = ulLine;

    ( void ) pcFileName;
    ( void ) ulLineNumber;

    printf( "vAssertCalled %s, %ld\n", pcFile, ( long ) ulLine );
    fflush( stdout );

    /* Setting ulBlockVariable to a non-zero value in the debugger will allow
     * this function to be exited. */
    taskDISABLE_INTERRUPTS();
    {
        while( ulBlockVariable == 0UL )
        {
            vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
        }
    }
    taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined application hook need by the FreeRTOS-Plus-TCP library.
 */
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    const char * pcApplicationHostnameHook( void )
    {
        /* FIX ME: If necessary, update to applicable registration name. */

        /* This function will be called during the DHCP: the machine will be registered
         * with an IP address plus this name. */
        return clientcredentialIOT_THING_NAME;
    }

#endif
