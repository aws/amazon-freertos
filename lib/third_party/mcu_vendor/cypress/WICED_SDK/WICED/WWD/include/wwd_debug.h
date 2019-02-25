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

#ifndef INCLUDED_WWD_DEBUG_H
#define INCLUDED_WWD_DEBUG_H

#include <stdio.h>
#include "wiced_defaults.h"
#include "wwd_constants.h"

#if ( defined(NETWORK_LwIP) )
#include "lwip/memp.h"
#endif

#ifdef PLATFORM_TRACE
#include "platform_trace.h"
#endif
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef DEBUG
#include "platform_assert.h"
    #define WICED_BREAK_IF_DEBUG( ) WICED_ASSERTION_FAIL_ACTION()
#else
    #define WICED_BREAK_IF_DEBUG( )
#endif

#ifdef WPRINT_PLATFORM_PERMISSION
int platform_wprint_permission(void);
#define WPRINT_PLATFORM_PERMISSION_FUNC() platform_wprint_permission()
#else
#define WPRINT_PLATFORM_PERMISSION_FUNC() 1
#endif

/******************************************************
 *             Print declarations
 ******************************************************/
#ifdef ENABLE_JLINK_TRACE
#define WPRINT_MACRO(args) do {if (WPRINT_PLATFORM_PERMISSION_FUNC()) RTT_printf args } while(0==1)
#else
#define WPRINT_MACRO(args) do {if (WPRINT_PLATFORM_PERMISSION_FUNC()) printf args;} while(0==1)
#endif

/* WICED printing macros for general SDK/Library functions*/
#ifdef WPRINT_ENABLE_LIB_INFO
    #define WPRINT_LIB_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_LIB_INFO(args)
#endif
#ifdef WPRINT_ENABLE_LIB_DEBUG
    #define WPRINT_LIB_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_LIB_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_LIB_ERROR
    #define WPRINT_LIB_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_LIB_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for the Webserver*/
#ifdef WPRINT_ENABLE_WEBSERVER_INFO
    #define WPRINT_WEBSERVER_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WEBSERVER_INFO(args)
#endif
#ifdef WPRINT_ENABLE_WEBSERVER_DEBUG
    #define WPRINT_WEBSERVER_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WEBSERVER_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_WEBSERVER_ERROR
    #define WPRINT_WEBSERVER_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_WEBSERVER_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for Applications*/
#ifdef WPRINT_ENABLE_APP_INFO
    #define WPRINT_APP_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_APP_INFO(args)
#endif
#ifdef WPRINT_ENABLE_APP_DEBUG
    #define WPRINT_APP_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_APP_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_APP_ERROR
    #define WPRINT_APP_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_APP_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for Network Stacks */
#ifdef WPRINT_ENABLE_NETWORK_INFO
    #define WPRINT_NETWORK_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_NETWORK_INFO(args)
#endif
#ifdef WPRINT_ENABLE_NETWORK_DEBUG
    #define WPRINT_NETWORK_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_NETWORK_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_NETWORK_ERROR
    #define WPRINT_NETWORK_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_NETWORK_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for the RTOS*/
#ifdef WPRINT_ENABLE_RTOS_INFO
    #define WPRINT_RTOS_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_RTOS_INFO(args)
#endif
#ifdef WPRINT_ENABLE_RTOS_DEBUG
    #define WPRINT_RTOS_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_RTOS_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_RTOS_ERROR
    #define WPRINT_RTOS_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_RTOS_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for the Security stack*/
#ifdef WPRINT_ENABLE_SECURITY_INFO
    #define WPRINT_SECURITY_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_SECURITY_INFO(args)
#endif
#ifdef WPRINT_ENABLE_SECURITY_DEBUG
    #define WPRINT_SECURITY_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_SECURITY_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_SECURITY_ERROR
    #define WPRINT_SECURITY_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_SECURITY_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for the WPS stack*/
#ifdef WPRINT_ENABLE_WPS_INFO
    #define WPS_INFO(args) WPRINT_MACRO(args)
#else
    #define WPS_INFO(args)
#endif
#ifdef WPRINT_ENABLE_WPS_DEBUG
    #define WPS_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPS_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_WPS_ERROR
    #define WPS_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPS_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for the Supplicant stack*/
#ifdef WPRINT_ENABLE_SUPPLICANT_INFO
        #define SUPPLICANT_INFO(args) WPRINT_MACRO(args)
    #else
        #define SUPPLICANT_INFO(args)
    #endif
    #ifdef WPRINT_ENABLE_SUPPLICANT_DEBUG
        #define SUPPLICANT_DEBUG(args) WPRINT_MACRO(args)
    #else
        #define SUPPLICANT_DEBUG(args)
    #endif
    #ifdef WPRINT_ENABLE_SUPPLICANT_ERROR
        #define SUPPLICANT_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
    #else
        #define SUPPLICANT_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
    #endif


/* WICED printing macros for Platforms*/
#ifdef WPRINT_ENABLE_PLATFORM_INFO
    #define WPRINT_PLATFORM_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_PLATFORM_INFO(args)
#endif
#ifdef WPRINT_ENABLE_PLATFORM_DEBUG
    #define WPRINT_PLATFORM_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_PLATFORM_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_PLATFORM_ERROR
    #define WPRINT_PLATFORM_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_PLATFORM_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

/* WICED printing macros for Wiced Internal functions*/
#ifdef WPRINT_ENABLE_WICED_INFO
    #define WPRINT_WICED_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WICED_INFO(args)
#endif
#ifdef WPRINT_ENABLE_WICED_DEBUG
    #define WPRINT_WICED_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WICED_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_WICED_ERROR
    #define WPRINT_WICED_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_WICED_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif
#ifdef WPRINT_ENABLE_WICED_TEST
    #define WPRINT_WICED_TEST(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WICED_TEST(args)
#endif


/* WICED printing macros for Wiced Wi-Fi Driver*/
#ifdef WPRINT_ENABLE_WWD_INFO
    #define WPRINT_WWD_INFO(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WWD_INFO(args)
#endif
#ifdef WPRINT_ENABLE_WWD_DEBUG
    #define WPRINT_WWD_DEBUG(args) WPRINT_MACRO(args)
#else
    #define WPRINT_WWD_DEBUG(args)
#endif
#ifdef WPRINT_ENABLE_WWD_ERROR
    #define WPRINT_WWD_ERROR(args) { WPRINT_MACRO(args); WICED_BREAK_IF_DEBUG(); }
#else
    #define WPRINT_WWD_ERROR(args) { WICED_BREAK_IF_DEBUG(); }
#endif

#ifdef WWD_ENABLE_STATS

#define WWD_STATS_INCREMENT_VARIABLE( var )                            \
    do { wwd_stats.var++; } while ( 0 )

#define WWD_STATS_CONDITIONAL_INCREMENT_VARIABLE( condition, var )     \
    do { if (condition) { wwd_stats.var++; }} while ( 0 )

typedef struct
{
    uint32_t tx_total;      /* Total number of TX packets sent from WWD */
    uint32_t rx_total;      /* Total number of RX packets received at WWD */
    uint32_t tx_no_mem;     /* Number of times WWD could not send due to no buffer */
    uint32_t rx_no_mem;     /* Number of times WWD could not receive due to no buffer */
    uint32_t tx_fail;       /* Number of times TX packet failed */
    uint32_t no_credit;     /* Number of times WWD could not send due to no credit */
    uint32_t flow_control;  /* Number of times WWD Flow control is enabled */
    uint32_t internal_host_buffer_fail_with_timeout; /* Internal host buffer get failed after timeout */
} wwd_stats_t;
extern wwd_stats_t wwd_stats;

#if ( defined(NETWORK_LwIP) )
typedef struct
{
   uint32_t pbuf_failed;                /* Total number of times pbuf alloc failed */
   uint32_t pbuf_free_ref_count;        /* pbuf free reference count i.e how many times pbuf free is called */
   uint32_t pbuf_free_ref_count_zero;   /* Total number of times pbuf_free is called  and memory is freed */
   uint32_t pbuf_alloc_raw_host_buffer; /* Total number of times pbuf_alloc is called with type PBUF_RAW */
   int pbuf_tx_alloc_in_use;            /* Total number of pbuf's in TX packet pool being used */
   int pbuf_rx_alloc_in_use;            /* Total number of pbuf's in RX packet pool being used */
   int pbuf_memp_in_use;                /* Total number of MEMP_PBUF in use */
} pbuf_stats_t;

extern pbuf_stats_t pbuf_stats;

#define PBUF_STATS_INCREMENT_VARIABLE( var )  \
        do { pbuf_stats.var++; } while ( 0 )
#endif /* NETWORK_LwIP */
#else /* WWD_ENABLE_STATS */

#define WWD_STATS_INCREMENT_VARIABLE( var )
#define WWD_STATS_CONDITIONAL_INCREMENT_VARIABLE( condition, var )
#define PBUF_STATS_INCREMENT_VARIABLE( var )
#endif /* WWD_ENABLE_STATS */

#ifdef NETWORK_LwIP
extern int memp_in_use( memp_t type );
#endif
extern void wwd_init_stats ( void );
extern wwd_result_t wwd_print_stats ( wiced_bool_t reset_after_print );
extern wwd_result_t dump_pbuf_stats( wiced_bool_t reset_after_print );
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_DEBUG_H */
