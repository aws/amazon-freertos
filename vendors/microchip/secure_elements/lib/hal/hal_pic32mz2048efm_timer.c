/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MZ2048
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#include "stddef.h"
#include "hal/atca_hal.h"
#include "system_definitions.h"


#define GetSystemClock() SYS_CLK_FREQ
#define us_SCALE   (GetSystemClock() / 2000000) // Note: Core Timer running @ (System Clock/2) 

/* ASF already have delay_us and delay_ms - see delay.h */

/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

static uint32_t ReadCoreTimer()
{
    volatile uint32_t timer;

    // get the count reg
    asm volatile ("mfc0   %0, $9" : "=r" (timer));

    return timer;
}

void delay_us(uint32_t delay)
{
    uint32_t startCnt = ReadCoreTimer();
    uint32_t waitCnt = delay * us_SCALE;

    while ((ReadCoreTimer() - startCnt) < waitCnt)
    {
        ;
    }
}

/****************************************/

/**
 * \brief This function delays for a number of microseconds.
 *
 * \param[in] delay number of 0.001 milliseconds to delay
 */
void atca_delay_us(uint32_t delay)
{
    delay_us(delay);
}

/**
 * \brief This function delays for a number of tens of microseconds.
 *
 * \param[in] delay number of 0.01 milliseconds to delay
 */
void atca_delay_10us(uint32_t delay)
{
    atca_delay_us(delay * 10);
}

/**
 * \brief This function delays for a number of milliseconds.
 *
 *        You can override this function if you like to do
 *        something else in your system while delaying.
 *
 * \param[in] delay number of milliseconds to delay
 */
void atca_delay_ms(uint32_t delay)
{

    atca_delay_us(delay * 1000);

}

/** @} */