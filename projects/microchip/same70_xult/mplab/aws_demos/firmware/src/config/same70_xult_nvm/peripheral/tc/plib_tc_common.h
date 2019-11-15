/*******************************************************************************
  TC Peripheral Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    plib_tc_common.h

  Summary
    TC peripheral library interface.

  Description
    This file defines the interface to the TC peripheral library.  This
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
#include <stddef.h>
#include <stdbool.h>
#include "device.h"

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

typedef enum
{
    TC_TIMER_NONE = 0U,
    TC_TIMER_COMPARE_MATCH = TC_SR_CPAS_Msk,
    TC_TIMER_PERIOD_MATCH = TC_SR_CPCS_Msk,
    TC_TIMER_STATUS_MSK = TC_SR_CPAS_Msk | TC_SR_CPCS_Msk,
    /* Force the compiler to reserve 32-bit memory for enum */
    TC_TIMER_STATUS_INVALID = 0xFFFFFFFF
}TC_TIMER_STATUS;

typedef enum
{
    TC_COMPARE_NONE = 0U,
    TC_COMPARE_A = TC_SR_CPAS_Msk,
    TC_COMPARE_B = TC_SR_CPBS_Msk,
    TC_COMPARE_C = TC_SR_CPCS_Msk,
    TC_COMPARE_STATUS_MSK = TC_SR_CPAS_Msk | TC_SR_CPBS_Msk | TC_SR_CPCS_Msk,
    /* Force the compiler to reserve 32-bit memory for enum */
    TC_COMPARE_STATUS_INVALID = 0xFFFFFFFF
}TC_COMPARE_STATUS;

typedef enum
{
    TC_CAPTURE_NONE = 0U,
    TC_CAPTURE_COUNTER_OVERFLOW = TC_SR_COVFS_Msk,
    TC_CAPTURE_LOAD_OVERRUN = TC_SR_LOVRS_Msk,
    TC_CAPTURE_A_LOAD = TC_SR_LDRAS_Msk,
    TC_CAPTURE_B_LOAD = TC_SR_LDRBS_Msk,
    TC_CAPTURE_STATUS_MSK = TC_SR_COVFS_Msk | TC_SR_LOVRS_Msk | TC_SR_LDRAS_Msk | TC_SR_LDRBS_Msk,
    /* Force the compiler to reserve 32-bit memory for enum */
    TC_CAPTURE_STATUS_INVALID = 0xFFFFFFFF
}TC_CAPTURE_STATUS;

typedef enum
{
    TC_QUADRATURE_NONE = 0U,
    TC_QUADRATURE_INDEX = TC_QISR_IDX_Msk,
    TC_QUADRATURE_DIR_CHANGE = TC_QISR_DIRCHG_Msk,
    TC_QUADRATURE_ERROR = TC_QISR_QERR_Msk,
    TC_QUADRATURE_STATUS_MSK = TC_QISR_IDX_Msk | TC_QISR_DIRCHG_Msk | TC_QISR_QERR_Msk,
    /* Force the compiler to reserve 32-bit memory for enum */
    TC_QUADRATURE_STATUS_INVALID = 0xFFFFFFFF
}TC_QUADRATURE_STATUS;

// *****************************************************************************

typedef void (*TC_TIMER_CALLBACK) (TC_TIMER_STATUS status, uintptr_t context);

typedef void (*TC_COMPARE_CALLBACK) (TC_COMPARE_STATUS status, uintptr_t context);

typedef void (*TC_CAPTURE_CALLBACK) (TC_CAPTURE_STATUS status, uintptr_t context);

typedef void (*TC_QUADRATURE_CALLBACK) (TC_QUADRATURE_STATUS status, uintptr_t context);

// *****************************************************************************

typedef struct
{
    TC_TIMER_CALLBACK callback_fn;
    uintptr_t context;
}TC_TIMER_CALLBACK_OBJECT;

typedef struct
{
    TC_COMPARE_CALLBACK callback_fn;
    uintptr_t context;
}TC_COMPARE_CALLBACK_OBJECT;

typedef struct
{
    TC_CAPTURE_CALLBACK callback_fn;
    uintptr_t context;
}TC_CAPTURE_CALLBACK_OBJECT;

typedef struct
{
    TC_QUADRATURE_CALLBACK callback_fn;
    uintptr_t context;
}TC_QUADRATURE_CALLBACK_OBJECT;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif
// DOM-IGNORE-END

#endif //_PLIB_TC_COMMON_H

/**
 End of File
*/
