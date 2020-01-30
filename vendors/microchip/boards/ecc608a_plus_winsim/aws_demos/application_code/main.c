/*
 * Amazon FreeRTOS V1.4.7
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


/*
 * Setup information:
 *
 * See the following URL for information on using FreeRTOS and FreeRTOS+TCP in
 * a Windows environment.  **NOTE**  The project described on the link is not
 * the project that is implemented in this file, but the setup is the same.  It
 * may also be necessary to have WinPCap installed (https://www.winpcap.org/):
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/examples_FreeRTOS_simulator.html.
 *
 * It is necessary to use a wired Ethernet port.  Wireless ports are unlikely
 * to work.
 */

/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include <conio.h>

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"

/* Demo application includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DHCP.h"
#include "aws_demo_logging.h"
#include "iot_system_init.h"
#include "aws_demo.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"
#include "iot_network_manager_private.h"
#include "aws_iot_network_config.h"

/* Define a name that will be used for LLMNR and NBNS searches. Once running,
 * you can "ping RTOSDemo" instead of pinging the IP address, which is useful when
 * using DHCP. */
#define mainHOST_NAME           "RTOSDemo"
#define mainDEVICE_NICK_NAME    "windows_demo"

/*-----------------------------------------------------------*/

/*
 * Miscellaneous initialization including preparing the logging and seeding the
 * random number generator.
 */
static void prvMiscInitialisation( void );

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile( void );

/* Set the following constant to pdTRUE to log using the method indicated by the
 * name of the constant, or pdFALSE to not log using the method indicated by the
 * name of the constant.  Options include to standard out (xLogToStdout), to a disk
 * file (xLogToFile), and to a UDP port (xLogToUDP).  If xLogToUDP is set to pdTRUE
 * then UDP messages are sent to the IP address configured as the echo server
 * address (see the configECHO_SERVER_ADDR0 definitions in FreeRTOSConfig.h) and
 * the port number set by configPRINT_PORT in FreeRTOSConfig.h. */
const BaseType_t xLogToStdout = pdTRUE, xLogToFile = pdFALSE, xLogToUDP = pdFALSE;


/* Use by the pseudo random number generator. */
static UBaseType_t ulNextRand;

/* Notes if the trace is running or not. */
static BaseType_t xTraceRunning = pdTRUE;

/*-----------------------------------------------------------*/
extern void vApplicationIPInit( void );
int main( void )
{
    const uint32_t ulLongTime_ms = pdMS_TO_TICKS( 1000UL );

    /*
     * Instructions for setting up the network to use this project are the same
     * as those provided on the following link for the full demo:
     * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/examples_FreeRTOS_simulator.html.
     */

    /* Miscellaneous initialization including preparing the logging and seeding
     * the random number generator. */
    prvMiscInitialisation();

    /* Initialize the network interface.
     *
     ***NOTE*** Tasks that use the network are created in the network event hook
     * when the network is connected and ready for use (see the definition of
     * vApplicationIPNetworkEventHook() below).  The address values passed in here
     * are used if ipconfigUSE_DHCP is set to 0, or if ipconfigUSE_DHCP is set to 1
     * but a DHCP server cannot be contacted. */
	FreeRTOS_printf(("FreeRTOS_IPInit\n"));
	vApplicationIPInit();

    /* Start the RTOS scheduler. */
    FreeRTOS_printf( ( "vTaskStartScheduler\n" ) );
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
     * line will never be reached.  If the following line does execute, then
     * there was insufficient FreeRTOS heap memory available for the idle and/or
     * timer tasks to be created.  See the memory management section on the
     * FreeRTOS web site for more details (this is standard text that is not
     * really applicable to the Win32 simulator port). */
    for( ; ;)
    {
        Sleep( ulLongTime_ms );
    }
}
/*-----------------------------------------------------------*/

/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
 * events are only received if implemented in the MAC driver. */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    char cBuffer[ 16 ];
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the IP stack if they have not already been
         * created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /* A simple example to demonstrate key and certificate provisioning in
             * microcontroller flash using PKCS#11 interface. This should be replaced
             * by production ready key provisioning mechanism. */
            vDevModeKeyProvisioning( );

            /* Initialize AWS system libraries */
            SYSTEM_Init();

            /* Start the demo tasks. */
            DEMO_RUNNER_RunDemos();

            xTasksAlreadyCreated = pdTRUE;
        }

        /* Print out the network configuration, which may have come from a DHCP
         * server. */
        FreeRTOS_GetAddressConfiguration(
            &ulIPAddress,
            &ulNetMask,
            &ulGatewayAddress,
            &ulDNSServerAddress );
        FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        FreeRTOS_printf( ( "\r\n\r\nIP Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        FreeRTOS_printf( ( "Subnet Mask: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        FreeRTOS_printf( ( "Gateway Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        FreeRTOS_printf( ( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer ) );
    }
}
/*-----------------------------------------------------------*/

static void prvMiscInitialisation( void )
{
    uint32_t ulLoggingIPAddress;

    /* Initialise the trace recorder and create the label used to post user
     * events to the trace recording on each tick interrupt. */
    vTraceEnable( TRC_START );

    /* Initialise the logging library. */
    ulLoggingIPAddress = FreeRTOS_inet_addr_quick(
        configECHO_SERVER_ADDR0,
        configECHO_SERVER_ADDR1,
        configECHO_SERVER_ADDR2,
        configECHO_SERVER_ADDR3 );
    vLoggingInit(
        xLogToStdout,
        xLogToFile,
        xLogToUDP,
        ulLoggingIPAddress,
        configPRINT_PORT );
}


void vApplicationIdleHook( void )
{
    const uint32_t ulMSToSleep = 1;
    const TickType_t xKitHitCheckPeriod = pdMS_TO_TICKS( 1000UL );
    static TickType_t xTimeNow, xLastTimeCheck = 0;

    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If application tasks make use of the
     * vTaskDelete() API function to delete themselves then it is also important
     * that vApplicationIdleHook() is permitted to return to its calling function,
     * because it is the responsibility of the idle task to clean up memory
     * allocated by the kernel to any task that has since deleted itself. */

    /* _kbhit() is a Windows system function, and system functions can cause
     * crashes if they somehow block the FreeRTOS thread.  The call to _kbhit()
     * can be removed if it causes problems.  Limiting the frequency of calls to
     * _kbhit() should minimize the potential for issues. */
    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastTimeCheck ) > xKitHitCheckPeriod )
    {
        if( _kbhit() != pdFALSE )
        {
            if( xTraceRunning == pdTRUE )
            {
                xTraceRunning = pdFALSE;
                vTraceStop();
                prvSaveTraceFile();
            }
        }

        xLastTimeCheck = xTimeNow;
    }

    /* This is just a trivial example of an idle hook.  It is called on each
     * cycle of the idle task if configUSE_IDLE_HOOK is set to 1 in
     * FreeRTOSConfig.h.  It must *NOT* attempt to block.  In this case the
     * idle task just sleeps to lower the CPU usage. */
    Sleep( ulMSToSleep );
}

/*-----------------------------------------------------------*/

void vAssertCalled( const char * pcFile,
                    uint32_t ulLine )
{
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
            Sleep( ulLongSleep );
        }
    }
    taskENABLE_INTERRUPTS();
}

/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
    FILE * pxOutputFile;

    fopen_s( &pxOutputFile, "Trace.dump", "wb" );

    if( pxOutputFile != NULL )
    {
        fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
        fclose( pxOutputFile );
        printf( "\r\nTrace output saved to Trace.dump\r\n" );
    }
    else
    {
        printf( "\r\nFailed to create trace dump file\r\n" );
    }
}
/*-----------------------------------------------------------*/

void getUserCmd( char * pucUserCmd )
{
    char cTmp;

    scanf( "%c%c", pucUserCmd, &cTmp );
}
/*-----------------------------------------------------------*/
