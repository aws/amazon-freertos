/*
* Amazon FreeRTOS V1.x.x
* Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* http://aws.amazon.com/freertos
* http://www.FreeRTOS.org
*/

/**
* @file aws_hal_perf_cntr.c
* @brief File of the implementation for performance counter APIs calling STM drivers.
*/

#include <stdint.h>

/* ST HAL APIs to be called in this file. */
#include "stm32l4xx_hal.h"

/* API definition to be implemented in this file. */
#include "aws_hal_perf_cntr.h"

/* Load FreeRTOS user defined performance counter configurations. */
#include "FreeRTOSConfig.h"

/**
 * @brief Default performance counter frequency.
 *
 * Default is set to 10MHz ==> (uint32_t) 0xffffffff / 10MHz = 429.4967s.
 * So roughly every 7 mins, kernel needs to serve an overflow IRQ.
 */
#define PERF_COUNTER_FREQ_DEFAULT           ( 10000000 )

/**
 * @brief Maximum performance counter frequency.
 *
 * Maximum frequency for APB1 is 80MHz. Kernel needs to serve and overflow
 * IRQ in each ~53.6870s.
 */
#define PERF_COUNTER_FREQ_MAX               ( 80000000 )


/**
 * User suggested performance counter frequency.
 *
 * @warning The value of configHAL_PERF_COUNTER_FREQ needs to be smaller than
 * TIM clock.
 *
 * @warning TIM prescaler only takes integer. configHAL_PERF_COUNTER_FREQ can
 * be any value, but prescaler is (uint32_t)(PCLK1 frequency / configHAL_PERF_COUNTER_FREQ).
 */
#ifdef configHAL_PERF_COUNTER_FREQ
    #define HAL_PERF_COUNTER_FREQ       ( configHAL_PERF_COUNTER_FREQ )
#else
    #define HAL_PERF_COUNTER_FREQ       ( PERF_COUNTER_FREQ_DEFAULT )
#endif

/**
 * @brief Timer width, counter period, and loading value.
 */
#define HW_TIMER_32_WIDTH           ( sizeof( uint32_t ) * 8 )
#define HW_TIMER_32_CONST_PERIOD    ( UINT32_MAX )
#define HW_TIMER_32_LOADING_VALUE   ( 0x0UL )


/*-----------------------------------------------------------*/

static TIM_HandleTypeDef xTimerHandle = { 0 };
static uint64_t ullTimerOverflow = 0;

/*-----------------------------------------------------------*/
/**
 * @brief Enable interrupt for the timer.
 */

void HAL_TIM_Base_MspInit( TIM_HandleTypeDef * pxTimerHandle )
{

    if ( pxTimerHandle->Instance == TIM5 )
    {
        /* Enable timer clock. */
        __HAL_RCC_TIM5_CLK_ENABLE();

        /* Set interrupt priority, and enable counter overflow interrupt. */
        HAL_NVIC_SetPriority(TIM5_IRQn, configHAL_PERF_COUNTER_INTERRUPT_PRIORITY, 0 );
        HAL_NVIC_EnableIRQ(TIM5_IRQn);
    }
}

/*-----------------------------------------------------------*/
/**
 * @brief Clean up
 */

void HAL_TIM_Base_MspDeInit( TIM_HandleTypeDef * pxTimerHandle )
{
    if ( pxTimerHandle->Instance == TIM5 )
    {
        /* Disable counter overflow interrupt. */
        HAL_NVIC_DisableIRQ(TIM5_IRQn);

        /* Stop TIM5 source clock. */
        __HAL_RCC_TIM5_CLK_DISABLE();
    }

}

/*-----------------------------------------------------------*/
/**
 * @brief Implement overflow ISR for TIM5
 */
void HAL_TIM5_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    ullTimerOverflow++;
}

/*-----------------------------------------------------------*/

/**
 * @brief TIM5 ISR
 *
 * @note a cleaner way to handle timer IRQ for ST is to implement a branch for
 * .Instance == TIM5 in HAL_TIM_PeriodElapsedCallback(). Yet, to not expose static
 * variable ullTimerOverflow, enabling USE_HAL_TIM_REGISTER_CALLBACKS and use
 * .PeriodElapsedCallback for TIM5.
 */

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler( &xTimerHandle );
}
/*-----------------------------------------------------------*/

void aws_hal_perfcounter_open(void)
{
    uint32_t ulTimClock = 0;

    /* Clear static variable which tracks total number of timer overflow. */
    ullTimerOverflow = 0;

    /* Get timer clock frequency. */
    ulTimClock = HAL_RCC_GetPCLK1Freq();

    /* Check user input frequency is valid. */
    configASSERT( HAL_PERF_COUNTER_FREQ <= ulTimClock );

    /* From STM32L475xx chip datasheet:
     * - TIM2 and TIM5 are 32-bit general purpose timers, while others are all 16-bit.
     * - Peripheral current consumption (Range 1/Range 2/Low power and sleep):
     *   TIM2: 6.8/6.5/6.3 (uA/MHZ)
     *   TIM5: 6.5/5.5/6.1 (uA/MHZ)
     * To minimize the need for handling timer overflow interrupt with relative low cost,
     * prefer to use TIM5 as the hardware timer for performance counter.
     */
    xTimerHandle.Instance = TIM5;

    /* To minimize the need for serving overflow IRQ,
     * .Period is always set to the max value the counter allows.
     * Performance counter resolution is adjusted with .Prescaler.
     */
    xTimerHandle.Init.Prescaler = ulTimClock / HAL_PERF_COUNTER_FREQ - 1;
    xTimerHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    xTimerHandle.Init.Period = HW_TIMER_32_CONST_PERIOD;
    xTimerHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    xTimerHandle.Init.RepetitionCounter = HW_TIMER_32_LOADING_VALUE;
    xTimerHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    xTimerHandle.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;

    xTimerHandle.Lock = HAL_UNLOCKED;

    xTimerHandle.State = HAL_TIM_STATE_RESET;

    HAL_TIM_Base_Init( &xTimerHandle );

    /* Only generate interrupt when overflow. */
    __HAL_TIM_URS_ENABLE( &xTimerHandle );

    /* Clear the initial SR.UIF bit before timer starts. */
    __HAL_TIM_CLEAR_FLAG( &xTimerHandle, TIM_FLAG_UPDATE);

    /* Register overflow IRQ callback. */
    HAL_TIM_RegisterCallback( &xTimerHandle, HAL_TIM_PERIOD_ELAPSED_CB_ID, HAL_TIM5_PeriodElapsedCallback );

    /* Timer and interrupt are configured. Now start timer. */
    HAL_TIM_Base_Start_IT( &xTimerHandle );
}

/*-----------------------------------------------------------*/

void aws_hal_perfcounter_close( void)
{
    HAL_TIM_Base_Stop_IT( &xTimerHandle );

    HAL_TIM_Base_DeInit( &xTimerHandle );
}

/*-----------------------------------------------------------*/

uint64_t aws_hal_perfcounter_get_value(void)
{
    return ( uint64_t ) ( ( ullTimerOverflow << HW_TIMER_32_WIDTH ) | __HAL_TIM_GET_COUNTER( &xTimerHandle ) );
}

/*-----------------------------------------------------------*/

uint32_t aws_hal_perfcounter_get_frequency_hz(void)
{
    uint32_t ulTimClock = 0;

    /* Get timer clock frequency. */
    ulTimClock = HAL_RCC_GetPCLK1Freq();

    /* Avoid dividing by 0. */
    configASSERT( ulTimClock > 0 );

    return ( ulTimClock / ( xTimerHandle.Init.Prescaler + 1 ) );
}

/*-----------------------------------------------------------*/
