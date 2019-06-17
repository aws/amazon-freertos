/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : clock.h
* Author             : AMS - HEA&RF BU
* Version            : V1.0.1
* Date               : 19-July-2012
* Description        : Header file for clock library, that gives a simple time
*                       reference to the BLE Stack.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <hal_types.h>

/**
 * Number of clocks in one seconds.
 * This value must be set by each platorm implementation, basing on its needs.
 */
extern const uint32_t CLOCK_SECOND;

typedef uint32_t tClockTime;

/**
 * This function initializes the clock library and should be called before
 * any other Stack functions.
 *
 */
void Clock_Init(void);

/**
 * This function returns the current system clock time. it is used by
 * the host stack and has to be implemented.
 *
 * @return The current clock time, measured in system ticks.
 */
tClockTime Clock_Time(void);

/**
 * This function waits for a given number of milliseconds.
 *
 */
void Clock_Wait(uint32_t i);

/**
 * It suspends system clock.
 *
 */
void Clock_Suspend(void);


#endif /* __CLOCK_H__ */

