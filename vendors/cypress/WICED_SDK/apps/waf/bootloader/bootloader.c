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

#include <stdio.h>
#include "wicedfs.h"
#include "platform_dct.h"
#include "elf.h"
#include "wiced_framework.h"
#include "wiced_utilities.h"
#include "platform_config.h"
#include "platform_resource.h"
#include "waf_platform.h"
#include "wwd_rtos.h"

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

static void load_program( const load_details_t * load_details, uint32_t* new_entry_point );

/******************************************************
 *               Variable Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

int main( void )
{
    const load_details_t* load_details;
    uint32_t              entry_point;
    boot_detail_t         boot;
    wiced_result_t result = WICED_SUCCESS;

#ifdef PLATFORM_HAS_OTA
    NoOS_setup_timing( );

    if ( wiced_waf_check_factory_reset( ) != WICED_SUCCESS )
    {
        wiced_dct_restore_factory_reset( );
        wiced_waf_app_set_boot( DCT_FR_APP_INDEX, PLATFORM_DEFAULT_LOAD );
    }

    NoOS_stop_timing( );
#endif

    boot.load_details.valid = 0;
    boot.entry_point        = 0;
    result =  wiced_dct_read_with_copy( &boot, DCT_INTERNAL_SECTION, OFFSET( platform_dct_header_t, boot_detail ), sizeof(boot_detail_t) );

    if ( result != WICED_SUCCESS )
    {
        return -1;
    }
    load_details = &boot.load_details;
    entry_point  = boot.entry_point;

    if ( load_details->valid != 0 )
    {
        load_program( load_details, &entry_point );
    }

    wiced_waf_start_app( entry_point );

    while(1)
    {
    }

    /* Should never get here */
    return 0;
}

static void load_program( const load_details_t * load_details, uint32_t* new_entry_point )
{
    /* Image copy has been requested */
    if ( load_details->destination.id == EXTERNAL_FIXED_LOCATION)
    {
        /* External serial flash destination. Currently not allowed */
        return;
    }

    /* Internal Flash/RAM destination */
    if ( load_details->source.id == EXTERNAL_FIXED_LOCATION )
    {
        /* Fixed location in serial flash source - i.e. no filesystem */
        wiced_waf_app_load( &load_details->source, new_entry_point );
    }
    else if ( load_details->source.id == EXTERNAL_FILESYSTEM_FILE )
    {
        /* Filesystem location in serial flash source */
        wiced_waf_app_load( &load_details->source, new_entry_point );
    }

    if ( load_details->load_once != 0 )
    {
        boot_detail_t boot;

        boot.entry_point                 = 0;
        boot.load_details.load_once      = 1;
        boot.load_details.valid          = 0;
        boot.load_details.destination.id = INTERNAL;
        wiced_dct_write_boot_details( &boot );
    }
}
