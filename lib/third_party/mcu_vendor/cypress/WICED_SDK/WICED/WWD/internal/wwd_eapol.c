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
 *
 */

#include "network/wwd_buffer_interface.h"
#include "wwd_network_constants.h"
#include "wwd_eapol.h"
#include <stdio.h>

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

/******************************************************
 *               Variables Definitions
 ******************************************************/

eapol_packet_handler_t wwd_eapol_packet_handler = NULL;

/******************************************************
 *               Function Definitions
 ******************************************************/

void wwd_eapol_receive_eapol_packet( /*@only@*/ wiced_buffer_t buffer, wwd_interface_t interface )
{
    if ( buffer != NULL )
    {
        if ( wwd_eapol_packet_handler != NULL )
        {
            wwd_eapol_packet_handler( buffer, interface );
        }
        else
        {
            host_buffer_release( buffer, WWD_NETWORK_RX );
        }
    }
}

uint8_t* wwd_eapol_get_eapol_data( wwd_eapol_packet_t packet )
{
    return host_buffer_get_current_piece_data_pointer( packet );
}

uint16_t wwd_get_eapol_packet_size( wwd_eapol_packet_t packet )
{
    return host_buffer_get_current_piece_size( packet );
}

wwd_result_t wwd_eapol_register_receive_handler( eapol_packet_handler_t eapol_packet_handler )
{
    if ( wwd_eapol_packet_handler == NULL )
    {
        wwd_eapol_packet_handler = eapol_packet_handler;
        return WWD_SUCCESS;
    }
    return WWD_HANDLER_ALREADY_REGISTERED;
}

void wwd_eapol_unregister_receive_handler( void )
{
    wwd_eapol_packet_handler = NULL;
}
