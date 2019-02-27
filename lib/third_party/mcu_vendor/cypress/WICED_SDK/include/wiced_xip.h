/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Header file for XIP
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *            Macros
 ******************************************************/

#ifdef APPLICATION_XIP_ENABLE
/*
 * Read-only data/instructions are placed in sflash by default with XIP enabled
 * except for data referred by sflash driver and sflash driver itself.
 * Assigning ro-data/instructions into RAM can be done by using these macros.
 *  */

/* Read-only data section
 * For example, declare a variable with the macro :
 *  const int WICED_XIP_RAM_RO_DATA(xip_data) = 100;
 * */
#define WICED_XIP_RAM_SECTION_RO_DATA( _rodata )     ".ram_rodata."#_rodata

#define WICED_XIP_RAM_RO_DATA( _rodata ) \
        SECTION( WICED_XIP_RAM_SECTION_RO_DATA( _rodata ) ) _rodata

/* Instruction section
 *
 * For example, define a function with the macro :
 * void WICED_XIP_RAM_FUNC( foo ) ( int arg1 , int arg2) {
 *                 .................
 * }
 *  */
#define WICED_XIP_RAM_SECTION_FUNC( _func )     ".ram_function."#_func

#define WICED_XIP_RAM_FUNC( _func ) \
        NEVER_INLINE SECTION( WICED_XIP_RAM_SECTION_FUNC( _func ) ) _func
#else
#define WICED_XIP_RAM_RO_DATA( _rodata )            _rodata
#define WICED_XIP_RAM_FUNC( _func )                 _func
#endif /* APPLICATION_XIP_ENABLE */

/******************************************************
 *            Enumerations
 ******************************************************/

/******************************************************
 *            Structures
 ******************************************************/

/******************************************************
 *            Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
