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
 *  Provides Wiced with function prototypes for IOCTL commands,
 *  and for communicating with the SDPCM module
 *
 */

#ifndef INCLUDED_WWD_SDPCM_H
#define INCLUDED_WWD_SDPCM_H

#include "wwd_buffer.h"
#include "wwd_constants.h"
#include "wwd_bus_protocol.h"
#include "chip_constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *             Constants
 ******************************************************/

/* CDC flag definition taken from bcmcdc.h */
#ifndef CDCF_IOC_SET
#define CDCF_IOC_SET                (0x02)      /** 0=get, 1=set cmd */
#endif /* ifndef CDCF_IOC_SET */

typedef enum sdpcm_command_type_enum
{
    SDPCM_GET = 0x00,
    SDPCM_SET = CDCF_IOC_SET
} sdpcm_command_type_t;


/* IOCTL swapping mode for Big Endian host with Little Endian wlan.  Default to off */
#ifdef IL_BIGENDIAN
wiced_bool_t swap = WICED_FALSE;
#define htod32(i) (swap?bcmswap32(i):i)
#define htod16(i) (swap?bcmswap16(i):i)
#define dtoh32(i) (swap?bcmswap32(i):i)
#define dtoh16(i) (swap?bcmswap16(i):i)
#else /* IL_BIGENDIAN */
#define htod32(i) ((uint32_t)(i))
#define htod16(i) ((uint16_t)(i))
#define dtoh32(i) ((uint32_t)(i))
#define dtoh16(i) ((uint16_t)(i))
#endif /* IL_BIGENDIAN */

/******************************************************
 *             Structures
 ******************************************************/

#define IOCTL_OFFSET ( sizeof(wwd_buffer_header_t) + 12 + 16 )

/******************************************************
 *             Function declarations
 ******************************************************/

extern /*@null@*/ /*@exposed@*/ void* wwd_sdpcm_get_iovar_buffer                    ( /*@special@*/ /*@out@*/ wiced_buffer_t* buffer, uint16_t data_length, const char* name )  /*@allocates *buffer@*/ /*@defines **buffer@*/;
extern /*@null@*/ /*@exposed@*/ void* wwd_sdpcm_get_ioctl_buffer                    ( /*@special@*/ /*@out@*/ wiced_buffer_t* buffer, uint16_t data_length ) /*@allocates *buffer@*/  /*@defines **buffer@*/;
extern wwd_result_t                   wwd_sdpcm_send_ioctl                          ( sdpcm_command_type_t type, uint32_t command, wiced_buffer_t send_buffer_hnd, /*@null@*/ /*@out@*/ wiced_buffer_t* response_buffer_hnd, wwd_interface_t interface ) /*@releases send_buffer_hnd@*/ ;
extern wwd_result_t                   wwd_sdpcm_send_iovar                          ( sdpcm_command_type_t type, /*@only@*/ wiced_buffer_t send_buffer_hnd, /*@special@*/ /*@out@*/ /*@null@*/ wiced_buffer_t* response_buffer_hnd, wwd_interface_t interface )  /*@allocates *response_buffer_hnd@*/  /*@defines **response_buffer_hnd@*/;
extern void                           wwd_sdpcm_process_rx_packet                   ( /*@only@*/ wiced_buffer_t buffer );
extern wwd_result_t                   wwd_sdpcm_init                                ( void );
extern void                           wwd_sdpcm_quit                                ( void ) /*@modifies internalState@*/;
extern void                           wwd_sdpcm_bus_vars_init( void );
extern void                           wwd_sdpcm_quit ( void ) /*@modifies internalState@*/;
extern wiced_bool_t                   wwd_sdpcm_has_tx_packet( void );

extern wwd_result_t                   wwd_sdpcm_get_packet_to_send                  ( /*@out@*/ wiced_buffer_t* buffer );
extern void                           wwd_sdpcm_update_credit                       ( uint8_t* data );
extern uint8_t                        wwd_sdpcm_get_available_credits               ( void );
extern void                           wwd_update_host_interface_to_bss_index_mapping( wwd_interface_t interface, uint32_t bssid_index );
extern wwd_result_t wwd_sdpcm_register_fw_cmd_exit_hook( void (*func)( sdpcm_command_type_t type, uint32_t command, const char* name, wwd_interface_t interface ) );


/******************************************************
 *             Global variables
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_SDPCM_H */
