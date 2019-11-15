/*******************************************************************************
  Cache System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_cache.h

  Summary
    Cache System Service Library interface.

  Description
    This file defines the interface to the Cache System Service Library.

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

#ifndef SYS_CACHE_H    // Guards against multiple inclusion
#define SYS_CACHE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*  This section lists the other files that are included in this file.
*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
/* The following functions make up the methods (set of possible operations) of
   this interface.
*/
// *****************************************************************************
/* Function:
    void SYS_CACHE_EnableCaches (void)

  Summary:
    Enables both Instruction and Data Caches.

  Description:
    Turns on both Instruction and Data Caches.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_EnableCaches();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_EnableCaches (void);

/* Function:
    void SYS_CACHE_DisableCaches (void)

  Summary:
    Disables both Instruction and Data Caches.

  Description:
    Turns off both Instruction and Data Caches.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_DisableCaches();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_DisableCaches (void);

/* Function:
    void SYS_CACHE_EnableICache (void)

  Summary:
    Enable Instruction Cache.

  Description:
    Turns on Instruction Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_EnableICache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_EnableICache (void);

/* Function:
    void SYS_CACHE_DisableICache (void)

  Summary:
    Disable Instruction Cache.

  Description:
    Turns off Instruction Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_DisableICache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_DisableICache (void);

/* Function:
    void SYS_CACHE_InvalidateICache (void)

  Summary:
    Invalidate Instruction Cache.

  Description:
    Invalidates Instruction Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_InvalidateICache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_InvalidateICache (void);

/* Function:
    void SYS_CACHE_EnableDCache (void)

  Summary:
    Enable Instruction Cache.

  Description:
    Turns on Data Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_EnableDCache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_EnableDCache (void);

/* Function:
    void SYS_CACHE_DisableDCache (void)

  Summary:
    Disable Data Cache.

  Description:
    Turns off Data Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_DisableDCache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_DisableDCache (void);

/* Function:
    void SYS_CACHE_InvalidateDCache (void)

  Summary:
    Invalidate Data Cache.

  Description:
    Invalidates Data Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_InvalidateDCache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_InvalidateDCache (void);

/* Function:
    void SYS_CACHE_CleanDCache (void)

  Summary:
    Clean Data Cache.

  Description:
    Cleans Data Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_CleanDCache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_CleanDCache (void);

/* Function:
    void SYS_CACHE_CleanInvalidateDCache (void)

  Summary:
    Clean and Invalidate Data Cache.

  Description:
    Cleans and Invalidates Data Cache.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_CleanInvalidateDCache();

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_CleanInvalidateDCache (void);

/* Function:
    SYS_CACHE_InvalidateDCache_by_Addr (uint32_t *addr, int32_t size)

  Summary:
    Data Cache Invalidate by address.

  Description:
    Invalidates Data Cache for the given address.

  Precondition:
    None.

  Parameters:
    addr - address (aligned to 32-byte boundary)
    size - size of memory block (in number of bytes)

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_InvalidateDCache_by_Addr(addr, size);

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_InvalidateDCache_by_Addr (uint32_t *addr, int32_t size);

/* Function:
    SYS_CACHE_CleanDCache_by_Addr (uint32_t *addr, int32_t size)

  Summary:
    Data Cache Clean by address.

  Description:
    Cleans Data Cache for the given address.

  Precondition:
    None.

  Parameters:
    addr - address (aligned to 32-byte boundary)
    size - size of memory block (in number of bytes)

  Returns:
    None.

  Example:
    <code>

    SYS_CACHE_CleanDCache_by_Addr(addr, size);

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_CleanDCache_by_Addr (uint32_t *addr, int32_t size);

/* Function:
    SYS_CACHE_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t size)

  Summary:
    Data Cache Clean and Invalidate by address.

  Description:
    Cleans and invalidates Data Cache for the given address.

  Precondition:
    None.

  Parameters:
    addr - address (aligned to 32-byte boundary)
    size - size of memory block (in number of bytes)

    Returns:
    None.

  Example:
    <code>

    SYS_CACHE_CleanInvalidateDCache_by_Addr(addr, size);

    </code>

  Remarks:
    None.
*/
void SYS_CACHE_CleanInvalidateDCache_by_Addr (uint32_t *addr, int32_t size);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_CACHE_H
