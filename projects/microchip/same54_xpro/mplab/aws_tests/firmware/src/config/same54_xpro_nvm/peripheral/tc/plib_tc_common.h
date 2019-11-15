/*******************************************************************************
  Timer/Counter(TC) Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_tc_common.h

  Summary
    TC peripheral library interface.

  Description
    This file defines the interface to the TC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef PLIB_TC_COMMON_H    // Guards against multiple inclusion
#define PLIB_TC_COMMON_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/

// *****************************************************************************

typedef enum
{
    TC_CAPTURE_STATUS_NONE = 0,
    
    /* Capture status overflow */
    TC_CAPTURE_STATUS_OVERFLOW = TC_INTFLAG_OVF_Msk,

    /* Capture status error */
    TC_CAPTURE_STATUS_ERROR = TC_INTFLAG_ERR_Msk,

    /* Capture status ready for channel 0 */
    TC_CAPTURE_STAUTS_CAPTURE0_READY = TC_INTFLAG_MC0_Msk,

    /* Capture status ready for channel 1 */
    TC_CAPTURE_STATUS_CAPTURE1_READY = TC_INTFLAG_MC1_Msk,
    
    TC_CAPTURE_STATUS_MSK = TC_CAPTURE_STATUS_OVERFLOW | TC_CAPTURE_STATUS_ERROR | TC_CAPTURE_STAUTS_CAPTURE0_READY | TC_CAPTURE_STATUS_CAPTURE1_READY, 
    
    /* Force the compiler to reserve 32-bit memory for enum */
    TC_CAPTURE_STATUS_INVALID = 0xFFFFFFFF

} TC_CAPTURE_STATUS;

typedef enum
{
    TC_COMPARE_STATUS_NONE = 0,
    /*  overflow */
    TC_COMPARE_STATUS_OVERFLOW = TC_INTFLAG_OVF_Msk,
    /* match compare 0 */
    TC_COMPARE_STATUS_MATCH0 = TC_INTFLAG_MC0_Msk,
    /* match compare 1 */
    TC_COMPARE_STATUS_MATCH1 = TC_INTFLAG_MC1_Msk,
    TC_COMPARE_STATUS_MSK = TC_COMPARE_STATUS_OVERFLOW | TC_COMPARE_STATUS_MATCH0 | TC_COMPARE_STATUS_MATCH1,

    /* Force the compiler to reserve 32-bit memory for enum */
    TC_COMPARE_STATUS_INVALID = 0xFFFFFFFF
}TC_COMPARE_STATUS;

typedef enum
{
    TC_TIMER_STATUS_NONE = 0, 
    /*  overflow */
    TC_TIMER_STATUS_OVERFLOW = TC_INTFLAG_OVF_Msk,

    /* match compare 1 */
    TC_TIMER_STATUS_MATCH1 = TC_INTFLAG_MC1_Msk,
    
    TC_TIMER_STATUS_MSK = TC_TIMER_STATUS_OVERFLOW | TC_TIMER_STATUS_MATCH1, 

    /* Force the compiler to reserve 32-bit memory for enum */
    TC_TIMER_STATUS_INVALID = 0xFFFFFFFF
} TC_TIMER_STATUS;

// *****************************************************************************

typedef void (*TC_TIMER_CALLBACK) (TC_TIMER_STATUS status, uintptr_t context);

typedef void (*TC_COMPARE_CALLBACK) (TC_COMPARE_STATUS status, uintptr_t context);

typedef void (*TC_CAPTURE_CALLBACK) (TC_CAPTURE_STATUS status, uintptr_t context);

// *****************************************************************************
typedef struct
{
    TC_TIMER_CALLBACK callback;

    uintptr_t context;

} TC_TIMER_CALLBACK_OBJ;

typedef struct
{
    TC_COMPARE_CALLBACK callback;
    uintptr_t context;
}TC_COMPARE_CALLBACK_OBJ;

typedef struct
{
    TC_CAPTURE_CALLBACK callback;
    uintptr_t context;
}TC_CAPTURE_CALLBACK_OBJ;


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif /* PLIB_TC_COMMON_H */
