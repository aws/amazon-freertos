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

#include <string.h>
#include "platform_config.h" /* Needed for EXTERNAL_DCT */
#include "wiced_framework.h"
#include "wiced_dct_common.h"
#include "wiced_utilities.h"

#include "wiced_rtos.h"
#include "wwd_assert.h"

/** @file
 *
 */

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
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

#if !defined(BOOTLOADER)
wiced_mutex_t   wiced_dct_mutex;
wiced_bool_t    wiced_dct_mutex_inited;
wiced_result_t wiced_dct_init( void )
{
    wiced_result_t result = WICED_SUCCESS;

    if (wiced_dct_mutex_inited == WICED_FALSE)
    {
        result = wiced_rtos_init_mutex( &wiced_dct_mutex );
        wiced_assert( "Init DCT Mutex failed", (result == WICED_SUCCESS) );
        if (result == WICED_SUCCESS)
        {
            wiced_dct_mutex_inited = WICED_TRUE;
        }
    }
    return result;
}
#endif

/*
 * NOTE: The read of the DCT is locked so that the data does not change during the read itself.
 *       The DCT is *not* left in a locked state.
 */
wiced_result_t wiced_dct_read_lock( void** info_ptr, wiced_bool_t ptr_is_writable, dct_section_t section, uint32_t offset, uint32_t size )
{
#ifdef EXTERNAL_DCT

    UNUSED_PARAMETER( ptr_is_writable );

    *info_ptr = malloc_named( "DCT", size );
    return wiced_dct_read_with_copy( *info_ptr, section, offset, size);
#else /* ifdef EXTERNAL_DCT */
    if ( ptr_is_writable == WICED_TRUE )
    {
        *info_ptr = (void*)malloc_named( "DCT", size );
        if ( *info_ptr == NULL )
        {
            return WICED_ERROR;
        }
        wiced_dct_read_with_copy( *info_ptr, section, offset, size );
    }
    else
    {
        *info_ptr = (char*)wiced_dct_get_current_address( section ) + offset;
    }
    return WICED_SUCCESS;
#endif /* ifdef EXTERNAL_DCT */
}

wiced_result_t wiced_dct_read_unlock( void* info_ptr, wiced_bool_t ptr_is_writable )
{
#ifdef EXTERNAL_DCT

    UNUSED_PARAMETER( ptr_is_writable );

    free( info_ptr );
#else
    if ( ptr_is_writable == WICED_TRUE )
    {
        free( info_ptr );
    }
#endif /* ifdef EXTERNAL_DCT */
    return WICED_SUCCESS;
}
