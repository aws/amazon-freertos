/*******************************************************************************
 Interface definition of RTT PLIB.
 
 Company:
    Microchip Technology Inc.
    
 File Name:
    plib_rtt_common.h
    
 Summary:
    Interface definition of rtt Plib.
    
 Description:
    This file defines the interface for the rtt Plib.
    It allows user to start, stop and configure the on-chip real time timer.
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

#ifndef PLIB_RTT_COMMON_H    // Guards against multiple inclusion
#define PLIB_RTT_COMMON_H

#include <stdint.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END


typedef enum
{
    RTT_PERIODIC = 0x20000,     //Periodic interrupt
    RTT_ALARM     = 0x10000,     // One time Alarm

    /* Force the compiler to reserve 32-bit memory for enum */
    RTT_INTERRUPT_INVALID = 0xFFFFFFFF
}RTT_INTERRUPT_TYPE;

typedef void (*RTT_CALLBACK)(RTT_INTERRUPT_TYPE type, uintptr_t context);

typedef struct
{
    RTT_CALLBACK          callback;
    uintptr_t             context;
} RTT_OBJECT ;



// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
}
#endif
// DOM-IGNORE-END

#endif
