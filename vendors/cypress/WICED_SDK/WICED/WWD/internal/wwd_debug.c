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
#include "wwd_debug.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/
#ifdef WWD_ENABLE_STATS
wwd_stats_t wwd_stats;
#ifdef NETWORK_LwIP
pbuf_stats_t pbuf_stats;
#endif
#endif /* WWD_ENABLE_STATS */

/******************************************************
 *             Function definitions
 ******************************************************/
void wwd_init_stats ( void )
{
#ifdef WWD_ENABLE_STATS
    memset( &wwd_stats, 0, sizeof(wwd_stats) );
#ifdef NETWORK_LwIP
    memset( &pbuf_stats, 0, sizeof(pbuf_stats_t) );
#endif
#endif /* WWD_ENABLE_STATS */
}

wwd_result_t wwd_print_stats ( wiced_bool_t reset_after_print )
{
#ifdef WWD_ENABLE_STATS
    WPRINT_MACRO(( "WWD Stats.. \n"
                   "tx_total:%ld, rx_total:%ld, tx_no_mem:%ld, rx_no_mem:%ld\n"
                   "tx_fail:%ld, no_credit:%ld, flow_control:%ld\n",
                   wwd_stats.tx_total, wwd_stats.rx_total, wwd_stats.tx_no_mem, wwd_stats.rx_no_mem,
                   wwd_stats.tx_fail, wwd_stats.no_credit, wwd_stats.flow_control ));

    if ( reset_after_print == WICED_TRUE )
    {
        memset( &wwd_stats, 0, sizeof(wwd_stats) );
    }
    return wwd_bus_print_stats( reset_after_print );
#else /* WWD_ENABLE_STATS */
    UNUSED_VARIABLE(reset_after_print);
    return WWD_DOES_NOT_EXIST;
#endif /* WWD_ENABLE_STATS */
}


wwd_result_t dump_pbuf_stats( wiced_bool_t reset_after_print )
{
#if( (defined WWD_ENABLE_STATS)  && (  defined(NETWORK_LwIP) ) )
    printf("== Previous PBUF stats ==\n");
    printf("Allocation stats: host_buffer[got:%ld failed:%ld] timed-out: %ld\n",  pbuf_stats.pbuf_alloc_raw_host_buffer, pbuf_stats.pbuf_failed, wwd_stats.internal_host_buffer_fail_with_timeout );
    printf("Free stats: ref-zero:%ld ref:%ld\n", pbuf_stats.pbuf_free_ref_count_zero, pbuf_stats.pbuf_free_ref_count );
    pbuf_stats.pbuf_tx_alloc_in_use = memp_in_use( MEMP_PBUF_POOL_TX );
    pbuf_stats.pbuf_rx_alloc_in_use = memp_in_use( MEMP_PBUF_POOL_RX );
    pbuf_stats.pbuf_memp_in_use = memp_in_use( MEMP_PBUF );
    printf("Alloc pbufs: pbuf_tx_in_use:%d pbuf_rx_in_use:%d pbuf_memp_in_use=%d\n", pbuf_stats.pbuf_tx_alloc_in_use, pbuf_stats.pbuf_rx_alloc_in_use, pbuf_stats.pbuf_memp_in_use );

    if ( reset_after_print == WICED_TRUE )
    {
        memset( &pbuf_stats, 0, sizeof(pbuf_stats_t) );
    }
    return WWD_SUCCESS;
#else /* ( defined(NETWORK_LwIP)  && defined (WWD_ENABLE_STATS) ) */
    UNUSED_VARIABLE(reset_after_print);
    return WWD_DOES_NOT_EXIST;
#endif /* ( defined(NETWORK_LwIP)  && defined (WWD_ENABLE_STATS) ) */
}

