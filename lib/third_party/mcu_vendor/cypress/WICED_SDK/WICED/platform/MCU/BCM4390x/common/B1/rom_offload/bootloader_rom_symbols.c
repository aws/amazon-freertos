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
 * This file supplements the GCC_rom_bootloader_symbols.ld
 * all the functions in this file should have the same names as in GCC_rom_bootloader_symbols.ld*
 * * (Except for built-in functions, which are not included in this file because
 *    - they will be always present in the SDK , so linker knows that the symbol is a function
 *    - and adding them to this file results in compilation error: conflicting type of built-in function)
 *
 * GCC_rom_bootloader_symbols.ld defines a list of symbols which should be linked from ROM,
 * this file tells the linker that the symbols defined in GCC_rom_bootloader_symbols.ld are functions.
 * To ensure the arm thumb interworking logic is correct in the generated output file.
 */

#include "platform_toolchain.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

void platform_nonexistant_function (void );

/* Define the functions to be linked from ROM to have weak attribute
 * To ensure that linker treats the symbols as functions.
 */

WEAK void  hwcrypto_compute_sha256hmac_inner_outer_hashcontext( void );
WEAK void  hwcrypto_core_enable ( void );
WEAK void  hwcrypto_dcache_clean_dma_input( void );
WEAK void  hwcrypto_split_dma_data ( void );
WEAK void  hwcrypto_unprotected_blocking_dma_transfer ( void );
WEAK void  platform_hwcrypto_aes128cbc_decrypt ( void );
WEAK void  platform_hwcrypto_aescbc_decrypt_sha256_hmac ( void );
WEAK void  platform_hwcrypto_execute ( void );
WEAK void  platform_hwcrypto_init ( void );
WEAK void  platform_hwcrypto_sha256_hash ( void );
WEAK void  platform_hwcrypto_sha256_hmac ( void );
WEAK void  platform_hwcrypto_sha256_hmac_final ( void );
WEAK void  platform_hwcrypto_sha256_hmac_init ( void );
WEAK void  platform_hwcrypto_sha256_hmac_update ( void );
WEAK void  platform_hwcrypto_sha256_incremental ( void );

WEAK void  memcpy ( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/* All the functions below call platform_nonexistant_function(), which is not defined intentionally
 * To ensure that this definition of the function is not linked ever.
 */
void  hwcrypto_compute_sha256hmac_inner_outer_hashcontext( void ) { platform_nonexistant_function(); }
void  hwcrypto_core_enable ( void ) { platform_nonexistant_function(); }
void  hwcrypto_dcache_clean_dma_input( void ) { platform_nonexistant_function(); }
void  hwcrypto_split_dma_data ( void ) { platform_nonexistant_function(); }
void  hwcrypto_unprotected_blocking_dma_transfer ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_aes128cbc_decrypt ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_aescbc_decrypt_sha256_hmac ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_execute ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_init ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_hash ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_hmac ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_hmac_final ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_hmac_init ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_hmac_update ( void ) { platform_nonexistant_function(); }
void  platform_hwcrypto_sha256_incremental ( void ) { platform_nonexistant_function(); }

void  memcpy ( void ) { platform_nonexistant_function(); }
