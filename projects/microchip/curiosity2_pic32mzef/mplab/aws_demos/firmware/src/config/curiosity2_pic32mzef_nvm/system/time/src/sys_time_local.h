/*******************************************************************************
  Time System Service Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    sys_time_local.h

  Summary:
    Time System Service local declarations and definitions.

  Description:
    This file contains the Time System Service local declarations and definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END


#ifndef SYS_TIME_LOCAL_H
#define SYS_TIME_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "system/time/sys_time.h"
#include "osal/osal.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Timer Handle Macros

  Summary:
    Timer Handle Macros

  Description:
    Timer handle related utility macros. SYS TIME timer handle is a
    combination of timer token and the timer object index. The token
    is a 16 bit number that is incremented for every new timer request
    and is used along with the timer object index to generate a new timer
    handle for every request.

  Remarks:
    None
*/

#define _SYS_TIME_HANDLE_TOKEN_MAX              (0xFFFF)
#define _SYS_TIME_INDEX_MASK                    (0x0000FFFFUL)

// *****************************************************************************
/* SYS TIME OBJECT INSTANCE structure

  Summary:
    System Time object instance structure.

  Description:
    This data type defines the System Time object instance.

  Remarks:
    None.
*/

typedef struct _SYS_TIME_TIMER_OBJ{
      bool                          inUse;    /* TRUE if in use */
      bool                          active;    /* TRUE if soft timer enabled */
      SYS_TIME_CALLBACK_TYPE        type;    /* periodic or not */
      uint32_t                      requestedTime;    /* time requested */
      volatile uint32_t             relativeTimePending;    /* time to wait, relative incase of timers in the list */
      SYS_TIME_CALLBACK             callback;    /* set to TRUE at timeout */
      uintptr_t                     context; /* context */
      volatile bool                 tmrElapsedFlag;   /* Set on every timer expiry. Cleared after user reads the status. */
      volatile bool                 tmrElapsed;    /* Set on every timer expiry. Cleared after timer is added back to the list */
      struct _SYS_TIME_TIMER_OBJ*   tmrNext; /* Next timer */
      SYS_TIME_HANDLE               tmrHandle; /* Unique handle for object */
} SYS_TIME_TIMER_OBJ;


typedef struct{
    SYS_STATUS status;
    const SYS_TIME_PLIB_INTERFACE*  timePlib;
    INT_SOURCE                      hwTimerIntNum;
    uint32_t                        hwTimerFrequency;
    volatile uint32_t               hwTimerPreviousValue;
    volatile uint32_t               hwTimerCurrentValue;
    uint32_t                        hwTimerPeriodValue;
    volatile uint32_t               hwTimerCompareValue;
    uint32_t                        hwTimerCompareMargin;
    volatile uint32_t               swCounter64Low;           /* Software counter */
    volatile uint32_t               swCounter64High;          /* Software 64-bit counter */
    uint8_t                         interruptNestingCount;
    SYS_TIME_TIMER_OBJ*             tmrActive;
    /* Mutex to protect access to the shared resources */
    OSAL_MUTEX_DECLARE(timerMutex);

} SYS_TIME_COUNTER_OBJ;   /* set of timers */


#endif //#ifndef SYS_TIME_LOCAL_H

/*******************************************************************************
 End of File
*/

