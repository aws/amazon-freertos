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

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* EDIT THIS FILE:
 * Wi-Fi SSID, password & security settings,
 * AWS endpoint, certificate, private key & thing name. */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"


/* FreeRTOS header files. */
#include "FreeRTOS.h"
#include "task.h"

/* AWS System files. */
#include "aws_wifi.h"
#include "aws_system_init.h"
#include "aws_test_runner.h"
#include "aws_logging_task.h"
#include "aws_dev_mode_key_provisioning.h"
#include "aws_pkcs11.h"

/* TI-Driver includes. */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

/* CC3220SF board file. */
#include "Board.h"

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* Test runner task defines. */
#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* Prototypes */

void vApplicationDaemonTaskStartupHook( void );
static void prvWifiConnect( void );
static CK_RV prvProvisionRootCA( void );


/**
 * @brief Performs board and logging initializations, then starts the OS.
 *
 * Functions that require the OS to be running
 * are handled in vApplicationDaemonTaskStartupHook().
 *
 * @sa vApplicationDaemonTaskStartupHook()
 *
 * @return This function should not return.
 */
int main( void )
{
    /* Call board init functions. */
    Board_initGeneral();

    /* Start logging task. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    return( 0 );
}

/*-----------------------------------------------------------*/

/**
 * @brief Completes board, Wi-Fi, and AWS Library System initializations
 * and creates the test runner task.
 *
 * This task is run when configUSE_DAEMON_TASK_STARTUP_HOOK = 1.
 */
void vApplicationDaemonTaskStartupHook( void )
{
    UART_Handle xtUartHndl;
    WIFIReturnCode_t xWifiStatus;

    /* Hardware initialization required after the RTOS is running. */
    GPIO_init();
    SPI_init();

    /* Configure the UART. */
    xtUartHndl = InitTerm();
    UART_control( xtUartHndl, UART_CMD_RXDISABLE, NULL );

    configPRINTF( ( "Starting Wi-Fi Module ...\r\n" ) );

    /* Initialize Wi-Fi module. This is called before key provisioning because
     * initializing the Wi-Fi module also initializes the CC3220SF's file system. */
    xWifiStatus = WIFI_On();

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi module initialized.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Wi-Fi module failed to initialize.\r\n" ) );

        /* Delay to allow the lower priority logging task to print the above status. */
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

        while( 1 )
        {
        }
    }

    /* A simple example to demonstrate key and certificate provisioning in
     * flash using PKCS#11 interface. This should be replaced
     * by production ready key provisioning mechanism. */
    vDevModeKeyProvisioning();
    prvProvisionRootCA();

    /* Initialize the AWS Libraries system. */
    if( SYSTEM_Init() == pdPASS )
    {
        prvWifiConnect();

        /* Create the task to run tests. */
        xTaskCreate( TEST_RUNNER_RunTests_task,
                     "TestRunner",
                     mainTEST_RUNNER_TASK_STACK_SIZE,
                     NULL,
                     tskIDLE_PRIORITY,
                     NULL );
    }
}
/* ----------------------------------------------------------*/



/**
 * @brief Imports the trusted Root CA required for a connection to
 * AWS IoT endpoint.
 */
CK_RV prvProvisionRootCA( void )
{
    uint8_t * pucRootCA = NULL;
    uint32_t ulRootCALength = 0;
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR xFunctionList;
    CK_SLOT_ID xSlotId;
    CK_SESSION_HANDLE xSessionHandle;
    CK_OBJECT_HANDLE xCertificateHandle;

    /* Use either Verisign or Starfield root CA 
     * depending on whether this is an ATS endpoint. */
    if( strstr( clientcredentialMQTT_BROKER_ENDPOINT, "-ats.iot" ) == NULL )
    {
        pucRootCA = ( uint8_t * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM;
        ulRootCALength = tlsVERISIGN_ROOT_CERTIFICATE_LENGTH;
    }
    else
    {
        pucRootCA = ( uint8_t * ) tlsSTARFIELD_ROOT_CERTIFICATE_PEM;
        ulRootCALength = tlsSTARFIELD_ROOT_CERTIFICATE_LENGTH;
    }

    xResult = xInitializePkcsSession( &xFunctionList,
                                      &xSlotId,
                                      &xSessionHandle );

    if( xResult == CKR_OK )
    {
        xResult = xProvisionCertificate( xSessionHandle,
                                         pucRootCA,
                                         ulRootCALength,
                                         pkcs11configLABEL_ROOT_CERTIFICATE,
                                         &xCertificateHandle );
    }

    return xResult;
}

/**
 * @brief Connect the Wi-Fi access point specifed in aws_clientcredential.h
 *
 */
static void prvWifiConnect( void )
{
    WIFIReturnCode_t xWifiStatus;
    WIFINetworkParams_t xNetworkParams;
    uint8_t ucTempIp[ 4 ];

    /* Initialize Network params. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.cChannel = 0;


    /* Connect to Wi-Fi. */
    xWifiStatus = WIFI_ConnectAP( &xNetworkParams );

    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi connected to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xWifiStatus = WIFI_GetIP( ucTempIp );

        if( eWiFiSuccess == xWifiStatus )
        {
            configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                            ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
        }
    }
    else
    {
        configPRINTF( ( "Wi-Fi failed to connect to AP %s.\r\n", clientcredentialWIFI_SSID ) );

        /* Delay to allow the lower priority logging task to print the above status. */
        vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

        while( 1 )
        {
        }
    }
}
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
    /* The TCP tests will test behavior when the entire heap is allocated. In
     * order to avoid interfering with those tests, this function does nothing. */
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
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}

/*-----------------------------------------------------------*/


void vApplicationIdleHook( void )
{
    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 2000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Provides memory to the FreeRTOS idle task.
 *
 * configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task.
 *
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/*-----------------------------------------------------------*/


/**
 * @brief Provides memory to the FreeRTOS timer task.
 *
 * configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*-----------------------------------------------------------*/

/**
 * @brief Socket event handler function.
 *
 * TI SimpleLink driver expects this function to be defined to notify
 * various socket events. The actual socket event handler funtion is
 * defined by the secure sockets layer in the file lib\secure_sockets\
 * portable\ti\cc3220_launchpad\aws_secure_sockets.c. If the user chooses
 * not to download secure sockets library, this weak definition ensures
 * that there are no linker errors.
 *
 * @param  pSlSockEvent Pointer to a structure containing the socket event
 * and the relevant data.
 *
 * @see SlSockEvent_t.
 */

void portWEAK_SYMBOL SimpleLinkSockEventHandler( void )
{
    configPRINTF( ( "Call of stub socket event handler.\r\n" ) );
}
