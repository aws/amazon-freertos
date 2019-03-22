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
 *  Provides prototypes / declarations for common APSTA functionality
 */
#ifndef _WWD_INTERNAL_AP_COMMON_H_
#define _WWD_INTERNAL_AP_COMMON_H_
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#define AMPDU_RX_FACTOR_8K          0     /* max receive AMPDU length is 8kb */
#define AMPDU_RX_FACTOR_16K         1     /* max receive AMPDU length is 16kb */
#define AMPDU_RX_FACTOR_32K         2     /* max receive AMPDU length is 32kb */
#define AMPDU_RX_FACTOR_64K         3     /* max receive AMPDU length is 64kb */
#define AMPDU_RX_FACTOR_INVALID  0xff     /* invalid rx factor; ignore */
#define AMPDU_MPDU_AUTO             (-1)  /* Auto number of mpdu in ampdu */


#define BANDWIDTH_20MHZ            (20)  /* 802.11n, 802.11ac 20Mhz Bandwidth */
#define BANDWIDTH_40MHZ            (40)  /* 802.11n, 802.11ac 40Mhz Bandwidth */
#define BANDWIDTH_80MHZ            (80)  /* 802.11ac 80Mhz Bandwidth          */

#define htod32(i) ((uint32_t)(i))
#define htod16(i) ((uint16_t)(i))
#define dtoh32(i) ((uint32_t)(i))
#define dtoh16(i) ((uint16_t)(i))

#define CHECK_IOCTL_BUFFER( buff )  if ( buff == NULL ) { wiced_assert("Buffer alloc failed\n", 0); return WWD_BUFFER_ALLOC_FAIL; }
#define CHECK_IOCTL_BUFFER_WITH_SEMAPHORE( buff, sema )  if ( buff == NULL ) { wiced_assert("Buffer alloc failed\n", 0 == 1 ); (void) host_rtos_deinit_semaphore( sema ); return WWD_BUFFER_ALLOC_FAIL; }
#define CHECK_RETURN( expr )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", 0 == 1 ); return check_res; } }
#define CHECK_RETURN_WITH_SEMAPHORE( expr, sema )  { wwd_result_t check_res = (expr); if ( check_res != WWD_SUCCESS ) { wiced_assert("Command failed\n", 0 == 1 ); (void) host_rtos_deinit_semaphore( sema ); return check_res; } }

/******************************************************
 *             Function prototypes
 ******************************************************/
extern wwd_result_t wwd_wifi_set_block_ack_window_size_common( wwd_interface_t interface, uint16_t ap_win_size, uint16_t sta_win_size );
extern wwd_result_t wwd_wifi_set_ampdu_parameters_common( wwd_interface_t interface, uint8_t ba_window_size, int8_t ampdu_mpdu, uint8_t rx_factor );
extern void wwd_wifi_set_ap_is_up( wiced_bool_t new_state );
extern wiced_bool_t wwd_wifi_get_ap_is_up( void );
extern wwd_result_t wwd_wifi_set_chanspec ( wwd_interface_t interface, uint8_t channel, host_semaphore_type_t *wwd_wifi_sleep_flag );

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* ifndef _WWD_INTERNAL_AP_COMMON_H_ */
