/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : freertos_usr_func.c
* Version      : 1.0
* Description  : Contains FreeRTOS user-defined functions.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 30.06.2016 1.00     First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#if defined(AMAZON_FREERTOS_ENABLE_UNIT_TESTS)
#include "unity_internals.h"
#elif defined(ENABLE_UNIT_TESTS)
#include "unity.h"
#endif


#if (BSP_CFG_RTOS_USED == 1)
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/************* semaphore initialization *****************/


/************* mutex initialization *********************/


/************** queues initialization *******************/


/************** event groups initialization *************/


/************** mailbox initialization ******************/


/************** memory pool initialization **************/


/************** task initialization *********************/


/* FreeRTOS's system timer. */
void vApplicationSetupTimerInterrupt(void);

/* Hook functions used by FreeRTOS. */
void vAssertCalled(void);
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/* FreeRTOS's processing before start the kernel. */
void Processing_Before_Start_Kernel(void);

/* Main task. */
extern void main(void *pvParameters);

/* Memory functions used by FreeRTOS. */
void vApplicationGetIdleTaskMemory(StaticTask_t ** ppxIdleTaskTCBBuffer,
                                   StackType_t ** ppxIdleTaskStackBuffer,
                                   uint32_t * pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory(StaticTask_t ** ppxTimerTaskTCBBuffer,
                                    StackType_t ** ppxTimerTaskStackBuffer,
                                    uint32_t * pulTimerTaskStackSize );


/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: vApplicationSetupTimerInterrupt
* Description  : Initialize system timer for FreeRTOS with tick interrupt 1ms.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationSetupTimerInterrupt(void)
{
    /* CMT channel 0 is configured as RTOS's system timer. */
#if (BSP_CFG_RTOS_SYSTEM_TIMER == 0)
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Enable compare match timer 0. */
    MSTP( CMT0 ) = 0;

    /* Stop counter. */
    CMT.CMSTR0.BIT.STR0 = 0;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

    /* Enable interrupt on compare match.
     * Divide the PCLK by 8. */
    CMT0.CMCR.WORD = 0x00C0; // CKS=00b,CMIE=1; PCLK/8,Compare match interrupt (CMIn) enabled @60MHz

    /* Set the compare match value. */
    CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ )) / 8 - 1);

    /* Clear counter. */
    CMT0.CMCNT = 0;

    /* Clear any previously pending interrupts. */
    IR(CMT0, CMI0)  = 0;

    /* Enable the interrupt. */
    IEN(CMT0, CMI0) = 1;

    /* Set its priority to the application defined kernel priority. */
    IPR(CMT0, CMI0) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer 0. */
    CMT.CMSTR0.BIT.STR0 = 1;
#endif /* (BSP_CFG_RTOS_SYSTEM_TIMER == 0) */

    /* CMT channel 1 is configured as RTOS's system timer. */
#if (BSP_CFG_RTOS_SYSTEM_TIMER == 1)
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Enable compare match timer 1. */
    MSTP( CMT1 ) = 0;

    /* Stop counter. */
    CMT.CMSTR0.BIT.STR1 = 0;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

    /* Enable interrupt on compare match.
     * Divide the PCLK by 8. */
    CMT1.CMCR.WORD = 0x00C0; // CKS=00b,CMIE=1; PCLK/8,Compare match interrupt (CMIn) enabled @60MHz

    /* Set the compare match value. */
    CMT1.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ )) / 8 - 1);

    /* Clear counter. */
    CMT1.CMCNT = 0;

    /* Clear any previously pending interrupts. */
    IR(CMT1, CMI1)  = 0;

    /* Enable the interrupt. */
    IEN(CMT1, CMI1) = 1;

    /* Set its priority to the application defined kernel priority. */
    IPR(CMT1, CMI1) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer 1. */
    CMT.CMSTR0.BIT.STR1 = 1;
#endif /* (BSP_CFG_RTOS_SYSTEM_TIMER == 1) */

    /* CMT channel 2 is configured as RTOS's system timer. */
#if (BSP_CFG_RTOS_SYSTEM_TIMER == 2)
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Enable compare match timer 2. */
    MSTP( CMT2 ) = 0;

    /* Stop counter. */
    CMT.CMSTR1.BIT.STR2 = 0;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

    /* Enable interrupt on compare match.
     * Divide the PCLK by 8. */
    CMT2.CMCR.WORD = 0x00C0; // CKS=00b,CMIE=1; PCLK/8,Compare match interrupt (CMIn) enabled @60MHz

    /* Set the compare match value. */
    CMT2.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ )) / 8 - 1);

    /* Clear counter. */
    CMT2.CMCNT = 0;

    /* Clear any previously pending interrupts. */
    IR(CMT2, CMI2)  = 0;

    /* Enable the interrupt. */
    IEN(CMT2, CMI2) = 1;

    /* Set its priority to the application defined kernel priority. */
    IPR(CMT2, CMI2) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer 2. */
    CMT.CMSTR1.BIT.STR2 = 1;
#endif /* (BSP_CFG_RTOS_SYSTEM_TIMER == 2) */

    /* CMT channel 3 is configured as RTOS's system timer. */
#if (BSP_CFG_RTOS_SYSTEM_TIMER == 3)
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Enable compare match timer 3. */
    MSTP( CMT3 ) = 0;

    /* Stop counter. */
    CMT.CMSTR1.BIT.STR3 = 0;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

    /* Enable interrupt on compare match.
     * Divide the PCLK by 8. */
    CMT3.CMCR.WORD = 0x00C0; // CKS=00b,CMIE=1; PCLK/8,Compare match interrupt (CMIn) enabled @60MHz

    /* Set the compare match value. */
    CMT3.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ )) / 8 - 1);

    /* Clear counter. */
    CMT3.CMCNT = 0;

    /* Clear any previously pending interrupts. */
    IR(CMT3, CMI3)  = 0;

    /* Enable the interrupt. */
    IEN(CMT3, CMI3) = 1;

    /* Set its priority to the application defined kernel priority. */
    IPR(CMT3, CMI3) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer 3. */
    CMT.CMSTR1.BIT.STR3 = 1;
#endif /* (BSP_CFG_RTOS_SYSTEM_TIMER == 3) */

} /* End of function vApplicationSetupTimerInterrupt() */

/******************************************************************************
* Function name: sbrk
* Description  : This implementation prevents using CC-RX's or GNURX+NEWLIB's
*                malloc() and calloc() which are not thread safe.
* Arguments    : size - not used
* Return value : -1 (failure) but vAssertCalled() may not return
******************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
extern int8_t *sbrk(size_t size);
int8_t *sbrk(size_t size)
{
    R_INTERNAL_NOT_USED(size);
    vAssertCalled();
    return (int8_t *) - 1;
}
#endif

/******************************************************************************
* Function name: _top_of_heap
* Description  : This implementation prevents using GNURX+OPTLIB's
*                malloc() and calloc() which are not thread safe.
* Arguments    : none
* Return value : end (failure) but vAssertCalled() may not return
******************************************************************************/
#if defined(__GNUC__)
extern int8_t end;
int8_t *_heap_of_memory = (int8_t *)&end;
int8_t *_last_heap_object = (int8_t *)&end;
extern int8_t *_top_of_heap(void);
int8_t *_top_of_heap(void)
{
    vAssertCalled();
    return &end;
}
#endif

/******************************************************************************
* Function Name: vAssertCalled
* Description  : This function is used to validate the input parameters.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vAssertCalled(void)
{
#if defined(CONFIG_FREERTOS_ASSERT_FAIL_ABORT)
    /* Assert call defined for debug builds. */
    /* debugging with E1/E2/E2L emulator */
    /* if not using a emulator, you can use LED on/off or serial terminal */
    volatile unsigned long ul = 0;

    taskENTER_CRITICAL();
    {
        /* Program may stop here when you stop it by debugger. In the case,
        use the debugger to set ul to a non-zero value in order to step out
        of this function to determine why it was called. */
        while( 0 == ul )
        {
            R_NOP();
        }
    }
    taskEXIT_CRITICAL();
#elif defined(ENABLE_UNIT_TESTS) || defined(AMAZON_FREERTOS_ENABLE_UNIT_TESTS)
    /* unity testing */
    /* TEST_ABORT() of unity_internal.h (and also TEST_PASS() of unity.h)
    jumps to the place where TEST_PROTECT() was executed. */
    TEST_ABORT();
#else /* defined(CONFIG_FREERTOS_ASSERT_DISABLE) || defined(NDEBUG) or nothing */
    /* Disable Assert call for release builds. */
    /* nothing to do */
#endif
} /* End of function vAssertCalled() */

/******************************************************************************
* Function Name: vApplicationIdleHook
* Description  : This function will be called on each cycle of the idle task.
*                NOTE: vApplicationIdleHook() MUST NOT CALL A FUNCTION
*                      THAT MIGHT BLOCK UNDER ANY CIRCUMSTANCES.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationIdleHook(void)
{
    /* Implement user-code for user own purpose. */

    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT_STRING(("."));
        xLastPrint = xTimeNow;
    }

} /* End of function vApplicationIdleHook() */

/******************************************************************************
* Function Name: vApplicationTickHook
* Description  : This function will be called every tick interrupt.
*                NOTE: vApplicationTickHook() EXECUTES FROM WITHIN AN ISR,
*                      SO MUST BE VERY SHORT AND NOT USE MUCH STACK.
*                      IN ADDITION, NOT CALL ANY APIs WITHOUT "FromISR" OR
*                      "FROM_ISR" AT THE END.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationTickHook(void)
{
    /* Implement user-code for user own purpose. */

} /* End of function vApplicationTickHook() */

/******************************************************************************
* Function Name: vApplicationMallocFailedHook
* Description  : This function is to capture the failure while
*                memory allocation.
* Arguments    : None.
* Return Value : None.
******************************************************************************/
void vApplicationMallocFailedHook(void)
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    taskDISABLE_INTERRUPTS();
    for( ; ; )
    {
        /* Loop here */
    };

} /* End of function vApplicationMallocFailedHook() */

/******************************************************************************
* Function Name: vApplicationStackOverflowHook
* Description  : Hook function is to capture the failure when the stack size
*                is insufficient for processing.
* Arguments    : pxTask -
*                    Task handler
*                pcTaskName -
*                    Pointer of where to store the task's name
* Return Value : None.
******************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t pxTask, signed char *pcTaskName)
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    taskDISABLE_INTERRUPTS();
    for( ; ; )
    {
        /* Loop here */
    };

} /* End of function vApplicationStackOverflowHook() */

/******************************************************************************
* Function Name : Processing_Before_Start_Kernel
* Description   : Create a main task, FreeRTOS's objects (e.g. mailbox, task,
*                 semaphore, mutex...) if required.
* Arguments     : None.
* Return value  : None.
******************************************************************************/
void Processing_Before_Start_Kernel(void)
{
    BaseType_t ret;

    /************** task creation ****************************/
    /* Main task. */
    ret = xTaskCreate(main, "MAIN_TASK", 512, NULL, 3, NULL);
    if (pdPASS != ret)
    {
        while(1)
        {
            /* Failed! Task can not be created. */
        }
    }
} /* End of function Processing_Before_Start_Kernel() */


/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
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

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif /* (BSP_CFG_RTOS_USED == 1) */
