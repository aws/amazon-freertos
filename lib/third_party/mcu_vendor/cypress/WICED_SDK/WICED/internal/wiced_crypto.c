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
 *  Cryptographic functions
 *
 *  Provides cryptographic functions for use in applications
 */

#include "wiced_crypto.h"

#include "platform/wwd_platform_interface.h"

#include "platform_config.h"

#ifdef WICED_SECURE_PRNG_FORTUNA_ENABLE
#include "fortuna.h"
#endif

#include <string.h>

#include "wwd_assert.h"

#include "wwd_rtos_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef WICED_CRYPTO_ADD_CYCLECNT_ENTROPY_EACH_N_BYTE
#define WICED_CRYPTO_ADD_CYCLECNT_ENTROPY_EACH_N_BYTE    1024
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)                                     (sizeof(a) / sizeof(a[0]))
#endif

#ifdef WICED_CRYPTO_THREAD_NOT_SAFE
    #define WICED_CRYPTO_THREAD_SAFE_LOCK_STATE(name)
    #define WICED_CRYPTO_THREAD_SAFE_LOCK(name)
    #define WICED_CRYPTO_THREAD_SAFE_UNLOCK(name)
#elif defined( WICED_SAVE_INTERRUPTS )
    #define WICED_CRYPTO_THREAD_SAFE_LOCK_STATE(name)    uint32_t name
    #define WICED_CRYPTO_THREAD_SAFE_LOCK(name)          WICED_SAVE_INTERRUPTS( name )
    #define WICED_CRYPTO_THREAD_SAFE_UNLOCK(name)        WICED_RESTORE_INTERRUPTS( name )
#else
    #define WICED_CRYPTO_THREAD_SAFE_LOCK_STATE(name)
    #define WICED_CRYPTO_THREAD_SAFE_LOCK(name)          WICED_DISABLE_INTERRUPTS()
    #define WICED_CRYPTO_THREAD_SAFE_UNLOCK(name)        WICED_ENABLE_INTERRUPTS()
#endif


/******************************************************
 *                    Constants
 ******************************************************/
#define WICED_CRYPTO_SEED_FEEDBACK_MAX_LOOPS 1000

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
 *               Static Function Declarations
 ******************************************************/

static uint32_t prng_well512_get_random ( void );
static void     prng_well512_add_entropy( const void* buffer, uint16_t buffer_length );

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint32_t prng_well512_state[ 16 ];
static uint32_t prng_well512_index = 0;

static uint32_t prng_add_cyclecnt_entropy_bytes = WICED_CRYPTO_ADD_CYCLECNT_ENTROPY_EACH_N_BYTE;

static wiced_crypto_prng_t prng_well512 =
{
    .get_random  = &prng_well512_get_random,
    .add_entropy = &prng_well512_add_entropy
};

/* Fortuna takes at least 5K more in memory than Well512, so leaving default as well512
*   Advantage of fortuna is that the algorithm generates somewhat better entropy and more secure output.
*   Fortuna is also considered a cryptographically secure algorithm, unlike Well512.
*   To make fortuna the current PRNG call wiced_crypto_use_secure_prng.
*   Making fortuna the default PRNG is not doable without a rewrite of the current code, and not generally recommended due to resource intensiveness.
*   Best to allow/force the app control over when and where fortuna is used.
*/
#define WICED_CRYPTO_DEFAULT_PRNG                        (&prng_well512)

static wiced_crypto_prng_t *curr_prng =  WICED_CRYPTO_DEFAULT_PRNG;

/******************************************************
 *               Function Definitions
 ******************************************************/

static uint32_t crc32_calc( const uint8_t* buffer, uint16_t buffer_length, uint32_t prev_crc32 )
{
    uint32_t crc32 = ~prev_crc32;
    int i;

    for ( i = 0; i < buffer_length; i++ )
    {
        int j;

        crc32 ^= buffer[ i ];

        for ( j = 0; j < 8; j++ )
        {
            if ( crc32 & 0x1 )
            {
                crc32 = ( crc32 >> 1 ) ^ 0xEDB88320;
            }
            else
            {
                crc32 = ( crc32 >> 1 );
            }
        }
    }

    return ~crc32;
}

static uint32_t prng_well512_get_random( void )
{
    /*
     * Implementation of WELL (Well equidistributed long-period linear) pseudorandom number generator.
     * Use WELL512 source code placed by inventor to public domain.
     *
     * This is NOT cryptographically secure pseudorandom number generator (CSPRNG).
     * If need CSPRNG please use THE wiced_crypto_use_secure_prng API and enable
     * the WICED_SECURE_PRNG_FORTUNA_ENABLE build flag.
     */

    uint32_t a, b, c, d;
    uint32_t result;

    WICED_CRYPTO_THREAD_SAFE_LOCK_STATE( flags );

    WICED_CRYPTO_THREAD_SAFE_LOCK( flags );

    a = prng_well512_state[ prng_well512_index ];
    c = prng_well512_state[ ( prng_well512_index + 13 ) & 15 ];
    b = a ^ c ^ ( a << 16 ) ^ ( c << 15 );
    c = prng_well512_state[ ( prng_well512_index + 9 ) & 15 ];
    c ^= ( c >> 11 );
    a = prng_well512_state[ prng_well512_index ] = b ^ c;
    d = a ^ ( ( a << 5 ) & (uint32_t)0xDA442D24UL );
    prng_well512_index = ( prng_well512_index + 15 ) & 15;
    a = prng_well512_state[ prng_well512_index ];
    prng_well512_state[ prng_well512_index ] = a ^ b ^ d ^ ( a << 2 ) ^ ( b << 18 ) ^ ( c << 28 );

    result = prng_well512_state[ prng_well512_index ];

    WICED_CRYPTO_THREAD_SAFE_UNLOCK( flags );

    return result;
}

static void prng_well512_add_entropy( const void* buffer, uint16_t buffer_length )
{
    uint32_t crc32[ ARRAYSIZE( prng_well512_state ) ];
    uint32_t curr_crc32 = 0;
    unsigned i;

    WICED_CRYPTO_THREAD_SAFE_LOCK_STATE( flags );

    for ( i = 0; i < ARRAYSIZE( crc32 ); i++ )
    {
        curr_crc32 = crc32_calc( buffer, buffer_length, curr_crc32 );
        crc32[ i ] = curr_crc32;
    }

    WICED_CRYPTO_THREAD_SAFE_LOCK( flags );

    for ( i = 0; i < ARRAYSIZE( prng_well512_state ); i++ )
    {
        prng_well512_state[ i ] ^= crc32[ i ];
    }

    WICED_CRYPTO_THREAD_SAFE_UNLOCK( flags );
}

static wiced_bool_t prng_is_add_cyclecnt_entropy( uint16_t buffer_length )
{
    wiced_bool_t add_entropy = WICED_FALSE;

    WICED_CRYPTO_THREAD_SAFE_LOCK_STATE( flags );

    WICED_CRYPTO_THREAD_SAFE_LOCK( flags );

    if ( prng_add_cyclecnt_entropy_bytes >= WICED_CRYPTO_ADD_CYCLECNT_ENTROPY_EACH_N_BYTE )
    {
        prng_add_cyclecnt_entropy_bytes %= WICED_CRYPTO_ADD_CYCLECNT_ENTROPY_EACH_N_BYTE;
        add_entropy = WICED_TRUE;
    }

    prng_add_cyclecnt_entropy_bytes += buffer_length;

    WICED_CRYPTO_THREAD_SAFE_UNLOCK( flags );

    return add_entropy;
}

static void prng_add_cyclecnt_entropy( uint16_t buffer_length )
{
    if ( prng_is_add_cyclecnt_entropy( buffer_length ) )
    {
        uint32_t cycle_count = host_platform_get_cycle_count( );
        curr_prng->add_entropy( &cycle_count, sizeof( cycle_count ) );
    }
    return;
}

wiced_result_t wiced_crypto_get_random( void* buffer, uint16_t buffer_length )
{
    uint8_t* p = buffer;

    prng_add_cyclecnt_entropy( buffer_length );

    while ( buffer_length != 0 )
    {
        uint32_t rnd_val = curr_prng->get_random( );
        int      i;

        for ( i = 0; i < 4; i++ )
        {
            *p++ = (uint8_t)( rnd_val & 0xFF );
            if ( --buffer_length == 0 )
            {
                break;
            }
            rnd_val = ( rnd_val >> 8 );
        }
    }

    return WICED_SUCCESS;
}

wiced_result_t wiced_crypto_add_entropy( const void* buffer, uint16_t buffer_length )
{
    curr_prng->add_entropy( buffer, buffer_length );
    return WICED_SUCCESS;
}

/* generate a random sequence from one PRNG; give it to the other for a seed */
static void wiced_crypto_set_current_prng( wiced_crypto_prng_t *requested_prng )
{
    wiced_result_t r;
    uint32_t rand_bytes;

    /* check not already: if so, NO-OP */
    if ( curr_prng == requested_prng )
    {
        return;
    }

    /* get some random output from old */
    r = wiced_crypto_get_random( &rand_bytes, sizeof( rand_bytes ) );

    /* update */
    curr_prng = requested_prng;

    /* add entropy to new from the old */
    if ( WICED_SUCCESS == r )
    {
        wiced_crypto_add_entropy( &rand_bytes, sizeof( rand_bytes ) );
    }
    else
    {
        /* This seems fatal; for some reason the old PRNG isn't working, so we can't seed the new one */
        WPRINT_WICED_ERROR(( "Unable to reseeed new PRNG, due to error %d\n", r ));
    }
}

wiced_result_t wiced_crypto_use_default_prng( void )
{
    wiced_crypto_set_current_prng( &prng_well512 );

#ifdef WICED_SECURE_PRNG_FORTUNA_ENABLE
    /* turn off secure prng, and free up any allocated resources for it */
    fortuna_disable( );
#endif /* WICED_SECURE_PRNG_FORTUNA_ENABLE */

    return WICED_SUCCESS;
}

#ifdef WICED_SECURE_PRNG_FORTUNA_ENABLE
wiced_result_t wiced_crypto_use_secure_prng( void )
{
    wiced_result_t result             = WICED_SUCCESS;
    wiced_crypto_prng_t *fortuna_prng = NULL;

    if ( FORTUNA_OK == fortuna_enable( &fortuna_prng, (fortuna_client_time_func_t)host_rtos_get_time ) )
    {
        wiced_crypto_set_current_prng( fortuna_prng );
    }
    else
    {
        result = WICED_OUT_OF_HEAP_SPACE;
    }

    return result;
}
#endif /* WICED_SECURE_PRNG_FORTUNA_ENABLE */

wiced_result_t wiced_crypto_set_prng( wiced_crypto_prng_t* prng )
{
    curr_prng = ( NULL == prng ) ?  WICED_CRYPTO_DEFAULT_PRNG : prng;
    return WICED_SUCCESS;
}

/* Given a low variability number: generate a seed value */
void wiced_crypto_prng_add_low_variability_entropy( uint32_t cycles )
{
    wiced_result_t result = WICED_SUCCESS;

#ifdef WICED_SECURE_PRNG_FORTUNA_ENABLE
    /* secure RNG is best for utilizing the small amount of entropy found here
        * If that doesn't work, then using default RNG is better than nothing
        */
    result = wiced_crypto_use_secure_prng( );

    if ( WICED_SUCCESS != result )
    {
        WPRINT_WICED_ERROR(("Unable to use secure prng in seeding process.  Using default prng instead.\n"));
        /* go on and use the default prng */
    }
#endif /* WICED_SECURE_PRNG_FORTUNA_ENABLE */

    wiced_crypto_add_entropy( &cycles, sizeof( cycles ) );
    wiced_crypto_get_random( &cycles, sizeof( cycles ) );

    /* feedback loop for somewhat random amount of loops (could be zero) */
    for ( int i = 0, loops = (int)( cycles%WICED_CRYPTO_SEED_FEEDBACK_MAX_LOOPS ) ; i < loops ; i++ )
    {
        wiced_crypto_add_entropy( &cycles, sizeof( cycles ) );
        wiced_crypto_get_random( &cycles, sizeof( cycles ) );
    }

    /* do get at least twice: allow debug output after this and move state further from start */
    wiced_crypto_get_random( &cycles, sizeof( cycles ) );

    /* hide output */
    memset( &cycles, 0, sizeof( cycles ) );

    /* revert to default random number generator - next random number will be passed as seed into the default RNG,
        * when/if the secure PRNG was used above
        */
    result = wiced_crypto_use_default_prng( );

    if ( WICED_SUCCESS != result )
    {
        WPRINT_WICED_ERROR(("Can't start default PRNG\n"));
    }
}
