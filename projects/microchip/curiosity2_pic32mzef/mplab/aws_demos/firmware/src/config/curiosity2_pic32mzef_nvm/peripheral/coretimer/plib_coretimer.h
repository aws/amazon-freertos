/*******************************************************************************
  Interface definition of Core Timer PLIB.

  Company:
    Microchip Technology Inc.

  File Name:
    plib_coretimer.h

  Summary:
    Interface definition of the Core Timer Plib .

  Description:
    This file defines the interface for the Core Timer Plib.
*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_CORETIMER_H    // Guards against multiple inclusion
#define PLIB_CORETIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus // Provide C++ Compatibility
	extern "C" {
#endif

#define CORE_TIMER_FREQUENCY    100000000


typedef void (*CORETIMER_CALLBACK)(uint32_t status, uintptr_t context);

typedef struct
{
    CORETIMER_CALLBACK  callback;
    uintptr_t           context;
} CORETIMER_OBJECT ;

void CORETIMER_Initialize( void );
void CORETIMER_CallbackSet ( CORETIMER_CALLBACK callback, uintptr_t context );
uint32_t CORETIMER_FrequencyGet ( void );
void CORETIMER_Start();
void CORETIMER_Stop();
uint32_t CORETIMER_CounterGet();
void CORETIMER_CompareSet(uint32_t compare);

void CORETIMER_DelayMs ( uint32_t delay_ms);
void CORETIMER_DelayUs ( uint32_t delay_us);


#ifdef __cplusplus // Provide C++ Compatibility
 }
#endif

#endif
