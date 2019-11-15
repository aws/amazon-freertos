/*******************************************************************************
  Cortex-M L1 Cache Header

  File Name:
    device_cache.h

  Summary:
    Preprocessor definitions to provide L1 Cache control.

  Description:
    An MPLAB PLIB or Project can include this header to perform cache cleans,
    invalidates etc. For the DCache and ICache.

  Remarks:
    This header should not define any prototypes or data definitions, or 
    include any files that do.  The file only provides macro definitions for 
    build-time.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef DEVICE_CACHE_H
#define DEVICE_CACHE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: L1 Cache Configuration
// *****************************************************************************
// *****************************************************************************
#define ICACHE_ENABLE()
#define ICACHE_DISABLE()
#define ICACHE_INVALIDATE()
#define INSTRUCTION_CACHE_ENABLED                      false

#define DCACHE_ENABLE()
#define DCACHE_DISABLE()
#define DCACHE_INVALIDATE()
#define DCACHE_CLEAN()
#define DCACHE_CLEAN_INVALIDATE()
#define DCACHE_CLEAN_BY_ADDR(addr,sz)
#define DCACHE_INVALIDATE_BY_ADDR(addr,sz)
#define DCACHE_CLEAN_INVALIDATE_BY_ADDR(addr,sz)
#define DATA_CACHE_ENABLED                             false

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DEVICE_CACHE_H
