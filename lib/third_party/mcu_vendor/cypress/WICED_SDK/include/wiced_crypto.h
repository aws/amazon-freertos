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
 *  Define cryptographic functions
 */

#pragma once

#include <stdint.h>
#include "wiced_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef uint32_t (*wiced_crypto_prng_get_random_t)( void );
typedef void     (*wiced_crypto_prng_add_entropy_t)( const void* buffer, uint16_t buffer_length );

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    wiced_crypto_prng_get_random_t  get_random;
    wiced_crypto_prng_add_entropy_t add_entropy;
} wiced_crypto_prng_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * Gets a 16 bit random numbers.
 *
 * Allows user applications to retrieve 16 bit random numbers.
 * If cryptographic security for rand numbers is required, call wiced_crypto_use_secure_prng first
 * @param buffer : pointer to the buffer which will receive the
 *                 generated random data
 * @param buffer_length : size of the buffer
 *
 * @return WICED_SUCCESS or Error code
 */
extern wiced_result_t wiced_crypto_get_random( void* buffer, uint16_t buffer_length );

/**
 * Feed entropy into random number generator.
 *
 * @param buffer : pointer to the buffer which contains random data
 * @param buffer_length : size of the buffer
 *
 * @return WICED_SUCCESS or Error code
 */
extern wiced_result_t wiced_crypto_add_entropy( const void* buffer, uint16_t buffer_length );

/**
 * Set new PRNG implementation.
 *
 * @param prng : pointer to PRNG implementation, if NULL then default (Well512) would be used
 *
 * @return WICED_SUCCESS or Error code
 */
extern wiced_result_t wiced_crypto_set_prng( wiced_crypto_prng_t* prng );

/**
 * Use default PRNG (currently Well512).  This is not a cryptographically secure PRNG.
 * Advantage is, relatively small memory requirements.
 * @return WICED_SUCCESS
 */
extern wiced_result_t wiced_crypto_use_default_prng( void );

#ifdef WICED_SECURE_PRNG_FORTUNA_ENABLE
/**
 * Use cryptographically secure PRNG  (currently Fortuna).  This uses ~5k heap while enabled.
 * The default PRNG (Well512) will be used as the initial source of starting entropy (seed).
 * See apps/test/entropy for more details on usage.
 * @return WICED_SUCCESS if successfully enabled
 */
extern wiced_result_t wiced_crypto_use_secure_prng( void );
#endif /* WICED_SECURE_PRNG_FORTUNA_ENABLE */

/**
 * Use the parameter to seed the default pseudo-random number generator (PRNG) from a low variability source.
 * Examples of this type of entropy are cycles taken to complete a function.
 * Note this type of seeding is usually most effective if the secure PRNG is compiled in.
 * (See wiced_crypto_use_secure_prng for more info.)
 * @param entropy : variable holding a value with some small amount of variability that can be used as an initial seed
 *                          Example: number of cycles to complete functions accessing hardware
 * @return WICED_SUCCESS or Error code
 */
extern void wiced_crypto_prng_add_low_variability_entropy( uint32_t entropy );

#ifdef __cplusplus
} /*extern "C" */
#endif
