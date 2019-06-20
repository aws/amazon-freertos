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

#ifndef INCLUDED_WWD_BUS_PROTOCOL_INTERFACE_H_
#define INCLUDED_WWD_BUS_PROTOCOL_INTERFACE_H_

#include "wwd_constants.h"
#include "wwd_buffer.h"
#include "wwd_bus_protocol.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *             Constants
 ******************************************************/

typedef enum
{
    BUS_FUNCTION       = 0,
    BACKPLANE_FUNCTION = 1,
    WLAN_FUNCTION      = 2
} wwd_bus_function_t;

#define BUS_FUNCTION_MASK (0x3)  /* Update this if adding functions */

/******************************************************
 *             Macros
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

#pragma pack(1)

typedef struct
{
#ifdef WWD_BUS_HAS_HEADER
                 wwd_bus_header_t  bus_header;
#endif /* ifdef WWD_BUS_HAS_HEADER */
                 uint32_t data[1];
} wwd_transfer_bytes_packet_t;

#pragma pack()

/******************************************************
 *             Function declarations
 ******************************************************/

/* Initialisation functions */
extern wwd_result_t wwd_bus_init                       ( void );
extern wwd_result_t wwd_bus_resume_after_deep_sleep    ( void );
extern wwd_result_t wwd_bus_deinit                     ( void );

/* Device register access functions */
extern wwd_result_t wwd_bus_write_backplane_value      ( uint32_t address, uint8_t register_length, uint32_t value );
extern wwd_result_t wwd_bus_read_backplane_value       ( uint32_t address, uint8_t register_length, /*@out@*/ uint8_t* value );
extern wwd_result_t wwd_bus_write_register_value       ( wwd_bus_function_t function, uint32_t address, uint8_t value_length, uint32_t value );
extern wwd_result_t wwd_bus_read_register_value        ( wwd_bus_function_t function, uint32_t address, uint8_t value_length, /*@out@*/ uint8_t* value );

/* Device data transfer functions */
extern wwd_result_t wwd_bus_send_buffer                ( wiced_buffer_t buffer );
extern wwd_result_t wwd_bus_transfer_bytes             ( wwd_bus_transfer_direction_t direction, wwd_bus_function_t function, uint32_t address, uint16_t size, /*@in@*/ /*@out@*/ wwd_transfer_bytes_packet_t* data );
extern wwd_result_t wwd_bus_transfer_backplane_bytes   ( wwd_bus_transfer_direction_t direction, uint32_t address, uint32_t size, /*@in@*/ /*@out@*/ uint8_t* data );

/* Frame transfer function */
extern wwd_result_t wwd_bus_read_frame( /*@out@*/  wiced_buffer_t* buffer );

extern wwd_result_t wwd_bus_poke_wlan                  ( void );
extern wwd_result_t wwd_bus_set_flow_control           ( uint8_t value );
extern wiced_bool_t wwd_bus_is_flow_controlled         ( void );
extern uint32_t     wwd_bus_packet_available_to_read   ( void );
extern wwd_result_t wwd_bus_ack_interrupt              ( uint32_t intstatus );
extern wwd_result_t wwd_bus_write_wifi_firmware_image  ( void );
extern wwd_result_t wwd_bus_write_wifi_nvram_image     ( void );
extern void         wwd_bus_set_resource_download_halt ( wiced_bool_t halt );
extern void         wwd_bus_init_backplane_window      ( void );
extern wwd_result_t wwd_bus_set_backplane_window       ( uint32_t addr );

/* Configure delayed sleep of bus. A high number will mean a longer delay before sleep after the last operation (higher performance)
  * A shorter delay will mean quicker sleep after last operation (lower power use)
*/
extern void         wwd_bus_wlan_set_delayed_bus_powersave_milliseconds( uint32_t time_ms );

/* handle delayed sleep of bus */
extern uint32_t     wwd_bus_handle_delayed_release( void );

/* return WICED_TRUE if bus is currently up; WICED_FALSE otherwise */
extern wiced_bool_t wwd_bus_is_up( void );

/* return WICED_TRUE if mcu in the platform supports deep sleep pwr save; WICED_FALSE otherwise */
extern wiced_bool_t wwd_bus_platform_mcu_power_save_deep_sleep_enabled( void );

#ifndef PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS
#define PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS 10
#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */

#if PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS
extern void wwd_delayed_bus_release_schedule_update( wiced_bool_t is_scheduled );
#define DELAYED_BUS_RELEASE_SCHEDULE(schedule) do {  wwd_delayed_bus_release_schedule_update( ( schedule ) ); } while(0)
#else
#define DELAYED_BUS_RELEASE_SCHEDULE(schedule) do {} while(0)
#endif /* PLATFORM_WLAN_ALLOW_BUS_TO_SLEEP_DELAY_MS */

extern wiced_bool_t wwd_bus_wake_interrupt_present( void );

extern wwd_result_t wwd_bus_specific_wakeup( void );
extern wwd_result_t wwd_bus_specific_sleep( void );

extern void         wwd_bus_init_stats( void );
extern wwd_result_t wwd_bus_print_stats( wiced_bool_t reset_after_print );
extern wwd_result_t wwd_bus_reinit( wiced_bool_t wake_from_firmware );

#ifdef WWD_TEST_NVRAM_OVERRIDE
extern wwd_result_t wwd_bus_get_wifi_nvram_image       ( char** nvram, uint32_t* size);
#endif

#ifdef MFG_TEST_ALTERNATE_WLAN_DOWNLOAD
extern wwd_result_t external_write_wifi_firmware_and_nvram_image  ( void );
#endif /* ifdef MFG_TEST_ALTERNATE_WLAN_DOWNLOAD */

/******************************************************
 *             Global variables
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_BUS_PROTOCOL_INTERFACE_H_ */
