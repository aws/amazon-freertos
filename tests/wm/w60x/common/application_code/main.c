#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Test includes */
#include "aws_test_runner.h"
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "aws_dev_mode_key_provisioning.h"


#define mainLOGGING_TASK_PRIORITY       (tskIDLE_PRIORITY + 5)
#define mainLOGGING_TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE * 8)
#define mainLOGGING_QUEUE_LENGTH        (15)

#define mainTEST_RUNNER_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 24 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

static void prvWifiConnect( void )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWifiStatus;
    uint8_t ucTempIp[4];

    /* Setup WiFi parameters to connect to access point. */
    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = strlen( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = strlen( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
    xNetworkParams.cChannel = 0;

    configPRINTF( ( "Starting Wi-Fi...\r\n" ) );

    xWifiStatus = WIFI_On();
    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi module initialized. Connecting to AP...\r\n" ) );
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

    xWifiStatus = WIFI_ConnectAP( &xNetworkParams );
    if( xWifiStatus == eWiFiSuccess )
    {
        configPRINTF( ( "Wi-Fi connected to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xWifiStatus = WIFI_GetIP( ucTempIp );
        if ( eWiFiSuccess == xWifiStatus )
        {
            configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                            ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
        }
    }
    else
    {
        /* Connection failed, configure SoftAP. */
        configPRINTF( ( "Wi-Fi failed to connect to AP %s.\r\n", xNetworkParams.pcSSID ) );

        xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
        xNetworkParams.pcPassword = wificonfigACCESS_POINT_PASSKEY;
        xNetworkParams.xSecurity = wificonfigACCESS_POINT_SECURITY;
        xNetworkParams.cChannel = wificonfigACCESS_POINT_CHANNEL;

        configPRINTF( ( "Connect to SoftAP %s using password %s. \r\n",
                        xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );

        while( WIFI_ConfigureAP( &xNetworkParams ) != eWiFiSuccess )
        {
            configPRINTF( ( "Connect to SoftAP %s using password %s and configure Wi-Fi. \r\n",
                            xNetworkParams.pcSSID, xNetworkParams.pcPassword ) );
        }

        configPRINTF( ( "Wi-Fi configuration successful. \r\n" ) );
    }
}

void UserMain(void)
{
    printf("=====================================\r\n");
    printf("Start TEST...[%s-%s]\r\n",__DATE__,__TIME__);
    printf("=====================================\r\n");
    
    xLoggingTaskInitialize(mainLOGGING_TASK_STACK_SIZE,
                           mainLOGGING_TASK_PRIORITY,
                           mainLOGGING_QUEUE_LENGTH);

    /* A simple example to demonstrate key and certificate provisioning in
         * microcontroller flash using PKCS#11 interface. This should be replaced
         * by production ready key provisioning mechanism. */
    vDevModeKeyProvisioning();

    if (SYSTEM_Init() == pdPASS)
    {
        /* Connect to the wifi before running the demos */
        prvWifiConnect();

        /* Create the task to run tests. */
        xTaskCreate(TEST_RUNNER_RunTests_task,
                    "TestRunner",
                    mainTEST_RUNNER_TASK_STACK_SIZE,
                    NULL,
                    tskIDLE_PRIORITY + 15,
                    NULL);
    }
}

void vApplicationDaemonTaskStartupHook( void )
{
////     WIFIReturnCode_t xWifiStatus;

//    /* Turn on the WiFi before key provisioning. This is needed because
//     * if we want to use OFFLOAD SSL, device certificate and key is stored
//     * on the WiFi module during key provisioning which requires the WiFi
//     * module to be initialized. */
////    xWifiStatus = WIFI_On();

//    if( xWifiStatus == eWiFiSuccess )
//    {
//        configPRINTF( ( "WiFi module initialized.\r\n" ) );

//        /* A simple example to demonstrate key and certificate provisioning in
//         * microcontroller flash using PKCS#11 interface. This should be replaced
//         * by production ready key provisioning mechanism. */
//        vDevModeKeyProvisioning();

//        if( SYSTEM_Init() == pdPASS )
//        {
//            /* Connect to the wifi before running the demos */
//            prvWifiConnect();

//            /* Create the task to run tests. */
//            xTaskCreate( TEST_RUNNER_RunTests_task,
//                         "TestRunner",
//                         mainTEST_RUNNER_TASK_STACK_SIZE,
//                         NULL,
//                         tskIDLE_PRIORITY,
//                         NULL );
//        }
//    }
//    else
//    {
//        configPRINTF( ( "WiFi module failed to initialize.\r\n" ) );

//        /* Stop here if we fail to initialize WiFi. */
//        configASSERT( xWifiStatus == eWiFiSuccess );
//    }
}

void vApplicationMallocFailedHook()
{
    printf( "ERROR: Malloc failed to allocate memory\r\n" );

    for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    printf( "ERROR: stack overflow with task %s\r\n", pcTaskName );
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

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

void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
void hardfaulthandlerc(unsigned int * hardfaultargs) 

{ 

	unsigned int stacked_r0; 

	unsigned int stacked_r1; 

	unsigned int stacked_r2; 

	unsigned int stacked_r3; 

	unsigned int stacked_r12; 

	unsigned int stacked_lr; 

	unsigned int stacked_pc; 

	unsigned int stacked_psr;  



    while(1)
    {
        stacked_r0 = ((unsigned long) hardfaultargs[0]);  
        stacked_r1 = ((unsigned long) hardfaultargs[1]);  
        stacked_r2 = ((unsigned long) hardfaultargs[2]);  
        stacked_r3 = ((unsigned long) hardfaultargs[3]);  
        stacked_r12 = ((unsigned long) hardfaultargs[4]);  
        stacked_lr = ((unsigned long) hardfaultargs[5]);  
        stacked_pc = ((unsigned long) hardfaultargs[6]);  
        stacked_psr = ((unsigned long) hardfaultargs[7]);  
        printf ("[Hard fault handler]\n");  
        printf ("R0 = %x\n", stacked_r0);  
        printf ("R1 = %x\n", stacked_r1);  
        printf ("R2 = %x\n", stacked_r2);  
        printf ("R3 = %x\n", stacked_r3);  
        printf ("R12 = %x\n", stacked_r12);  
        printf ("LR = %x\n", stacked_lr);  
        printf ("PC = %x\n", stacked_pc);  
        printf ("PSR = %x\n", stacked_psr);  
        printf ("BFAR = %x\n", (*((volatile unsigned long *)(0xE000ED38))));  
        printf ("CFSR = %x\n", (*((volatile unsigned long *)(0xE000ED28))));  
        printf ("HFSR = %x\n", (*((volatile unsigned long *)(0xE000ED2C))));  
        printf ("DFSR = %x\n", (*((volatile unsigned long *)(0xE000ED30))));  
        printf ("AFSR = %x\n", (*((volatile unsigned long *)(0xE000ED3C))));  
//        printf ("SCB_SHCSR = %x\n", SCB->SHCSR);
//        getkeychar();
        while(1);
    }
}  
/*-----------------------------------------------------------*/

/* The fault handler implementation calls a function called
 * prvGetRegistersFromStack(). */
//__asm void HardFault_Handler( void )
//{                                               
//    TST LR, #4
//    ITE EQ
//    MRSEQ R0, MSP
//    MRSNE R0, PSP
//    B hardfaulthandlerc
//}

