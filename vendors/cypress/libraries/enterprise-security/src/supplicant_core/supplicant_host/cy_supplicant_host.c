/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_supplicant_structures.h"
#include "emac_eapol.h"
#include "whd.h"
#include "cy_supplicant_host.h"
#include "cy_rtos_abstraction.h"


/******************************************************
 *                      Macros
 ******************************************************/
#define CY_SUPPLICANT_HOST_INFO( x )   //printf x

/******************************************************
 *               Function Definations
 ******************************************************/

uint32_t supplicant_host_hton32(uint32_t intlong)
{
    return htobe32(intlong);
}

uint16_t supplicant_host_hton16(uint16_t intshort)
{
    return htobe16(intshort);
}

void supplicant_host_hex_bytes_to_chars( char* cptr, const uint8_t* bptr, uint32_t blen )
{
    uint32_t i,j;
    uint8_t temp;

    i = 0;
    j = 0;
    while( i < blen )
    {
        // Convert first nibble of byte to a hex character
        temp = bptr[i] / 16;
        if ( temp < 10 )
        {
            cptr[j] = temp + '0';
        }
        else
        {
            cptr[j] = (temp - 10) + 'A';
        }
        // Convert second nibble of byte to a hex character
        temp = bptr[i] % 16;
        if ( temp < 10 )
        {
            cptr[j+1] = temp + '0';
        }
        else
        {
            cptr[j+1] = (temp - 10) + 'A';
        }
        i++;
        j+=2;
    }
}

/******************************************************
 *               Function Definitions
 ******************************************************/

void* supplicant_host_malloc( const char* name, uint32_t size )
{
    CY_SUPPLICANT_HOST_INFO(("supplicant_host_malloc: %s %u\n", name, (unsigned int)size));
    return cy_rtos_malloc( size );
}

void* supplicant_host_calloc( const char* name, uint32_t num, uint32_t size )
{
    CY_SUPPLICANT_HOST_INFO(("supplicant_host_calloc: %s %u %u\n", name, (unsigned int)num, (unsigned int)size));
    return cy_rtos_calloc( num, size );
}

void supplicant_host_free( void* p )
{
    cy_rtos_free( p );
}

cy_rslt_t supplicant_host_create_packet( whd_driver_t whd_driver, supplicant_packet_t* packet, uint16_t size )
{
    whd_result_t result;
    result = whd_host_buffer_get( whd_driver,(whd_buffer_t*) packet, WHD_NETWORK_TX, size + WHD_LINK_HEADER, WHD_TRUE );
    if ( result != WHD_SUCCESS )
    {
        *packet = 0;
        return  CY_RSLT_WIFI_SUPPLICANT_ERROR;
    }
    whd_buffer_add_remove_at_front(whd_driver, (whd_buffer_t*) packet, WHD_LINK_HEADER );

    return   CY_RSLT_SUCCESS;
}

void supplicant_host_consume_bytes( whd_driver_t whd_driver,supplicant_packet_t* packet, int32_t number_of_bytes )
{
    whd_buffer_add_remove_at_front( whd_driver,(whd_buffer_t*) packet, number_of_bytes );
}

uint8_t* supplicant_host_get_data(  whd_driver_t whd_driver,supplicant_packet_t packet )
{
    return whd_buffer_get_current_piece_data_pointer( whd_driver,packet );
}

cy_rslt_t supplicant_host_set_packet_size(  whd_driver_t whd_driver,  supplicant_packet_t packet, uint16_t packet_length )
{
    return (cy_rslt_t) whd_buffer_set_size( whd_driver,(whd_buffer_t) packet, packet_length );
}

uint16_t supplicant_host_get_packet_size(   whd_driver_t whd_driver,supplicant_packet_t packet )
{
    return whd_buffer_get_current_piece_size( whd_driver,packet );
}

void supplicant_host_free_packet(  whd_driver_t whd_driver,supplicant_packet_t packet )
{
    whd_buffer_release( whd_driver,(whd_buffer_t) packet, WHD_NETWORK_RX );
}

void supplicant_host_send_packet( void* workspace, supplicant_packet_t packet, uint16_t size )
{
    supplicant_rtos_workspace_t* host = (supplicant_rtos_workspace_t*) workspace;
    whd_buffer_set_size( host->interface->whd_driver,(whd_buffer_t) packet, size );
    whd_network_send_ethernet_data(  host->interface, packet );
}

cy_rslt_t supplicant_host_leave( whd_interface_t interface )
{
    whd_wifi_leave((whd_interface_t) interface );
    return CY_RSLT_SUCCESS;
}

void supplicant_host_start_timer( void* workspace, uint32_t timeout )
{
    supplicant_rtos_workspace_t* host = (supplicant_rtos_workspace_t*) workspace;
    cy_rtos_get_time( &host->timer_reference );
    host->timer_timeout = timeout;
}

void supplicant_host_stop_timer( void* workspace )
{
    supplicant_rtos_workspace_t* host = (supplicant_rtos_workspace_t*) workspace;
    host->timer_timeout = 0;
}

uint32_t supplicant_host_get_timer( void* workspace )
{
    supplicant_rtos_workspace_t* host = (supplicant_rtos_workspace_t*)workspace;
    return host->timer_timeout;
}

cy_rslt_t supplicant_queue_message_packet( void* workspace, supplicant_event_t type, supplicant_packet_t packet )
{
    cy_rslt_t result;
    supplicant_workspace_t* temp = (supplicant_workspace_t*)workspace;
    supplicant_rtos_workspace_t* host_workspace = (supplicant_rtos_workspace_t*)temp->supplicant_host_workspace;
    supplicant_event_message_t   message;
    message.event_type = type;
    message.data.packet = packet;
    result =  cy_rtos_put_queue( &host_workspace->event_queue, &message, SUPPLICANT_NEVER_TIMEOUT, 0 );
    if ( result != CY_RSLT_SUCCESS )
    {
        whd_buffer_release( temp->interface->whd_driver,(whd_buffer_t) packet, WHD_NETWORK_RX );
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t supplicant_queue_message_uint( void* workspace, supplicant_event_t type, uint32_t value )
{
    cy_rslt_t result;
    supplicant_rtos_workspace_t* host_workspace = (supplicant_rtos_workspace_t*)workspace;
    supplicant_event_message_t   message;
    message.event_type = type;
    message.data.value = value;
    result =  cy_rtos_put_queue( &host_workspace->event_queue, &message, SUPPLICANT_NEVER_TIMEOUT, 0 );
    if ( result != CY_RSLT_SUCCESS )
    {
        return CY_RSLT_WIFI_SUPPLICANT_ERROR;
    }
    return CY_RSLT_SUCCESS;
}

void supplicant_get_bssid( whd_interface_t interface, whd_mac_t* mac )
{
    whd_wifi_get_bssid( interface,(whd_mac_t*)mac );
}

cy_rslt_t supplicant_set_passphrase( whd_interface_t interface, const uint8_t* security_key, uint8_t key_length )
{
    whd_result_t result = whd_wifi_set_passphrase( interface,security_key, key_length );
    if(result == WHD_SUCCESS)
    {
        return CY_RSLT_SUCCESS;
    }
    return CY_RSLT_WIFI_SUPPLICANT_ERROR;
}

cy_rslt_t suppliant_emac_register_eapol_packet_handler(eapol_packet_handler_t eapol_packet_handler)
{
    whd_result_t result = cy_emac_register_eapol_packet_handler(eapol_packet_handler);
    if(result == WHD_SUCCESS || result == WHD_HANDLER_ALREADY_REGISTERED)
    {
        return CY_RSLT_SUCCESS;
    }
    return CY_RSLT_WIFI_SUPPLICANT_ERROR;
}
