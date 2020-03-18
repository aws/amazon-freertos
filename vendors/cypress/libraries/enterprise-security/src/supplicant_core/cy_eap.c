/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_eap.h"

void supplicant_send_eapol_packet(supplicant_packet_t packet, supplicant_workspace_t* workspace, eapol_packet_type_t type, uint16_t content_size )
{
    eapol_packet_t* header = (eapol_packet_t*) supplicant_host_get_data( workspace->interface->whd_driver,packet );
    memcpy( header->ethernet.ether_dhost, &workspace->authenticator_mac_address, sizeof(whd_mac_t) );
    memcpy( header->ethernet.ether_shost, &workspace->supplicant_mac_address, sizeof(whd_mac_t) );
    header->ethernet.ether_type = supplicant_host_hton16( ETHER_TYPE_802_1X );
    header->eapol.version = 1;
    header->eapol.type    = type;
    header->eapol.length  = supplicant_host_hton16( content_size );
    supplicant_host_send_packet( workspace->supplicant_host_workspace, packet, content_size + sizeof(eapol_packet_header_t));
}

cy_rslt_t supplicant_send_eapol_start( supplicant_workspace_t *workspace )
{
    supplicant_packet_t packet;
    cy_rslt_t result;

    result = supplicant_host_create_packet(workspace->interface->whd_driver, &packet, sizeof(eapol_packet_t) );
    if ( result != CY_RSLT_SUCCESS )
    {
        EAP_WRAPPER_DEBUG(( " ERR: #### Sending EAPOL start packet\r\n" ));
        return CY_RSLT_WIFI_SUPPLICANT_GENERIC_ERROR;
    }

    if (packet == 0)
    {
        return CY_RSLT_WIFI_SUPPLICANT_GENERIC_ERROR;
    }
    EAP_WRAPPER_DEBUG(("Sending EAPOL start\r\n"));
    supplicant_send_eapol_packet( packet, workspace, EAPOL_START, 0);

    return CY_RSLT_SUCCESS;
}

void supplicant_send_eap_response_packet( supplicant_workspace_t* workspace, eap_type_t eap_type, uint8_t* data, uint16_t data_length )
{
    supplicant_packet_t packet;
    eap_packet_t* header;
    cy_rslt_t result;
    uint16_t      aligned_length;

    result = supplicant_host_create_packet( workspace->interface->whd_driver,&packet, sizeof(eap_packet_t) + data_length );
    if ( result != CY_RSLT_SUCCESS )
    {
        EAP_WRAPPER_DEBUG(( " ERR: #### Sending EAP Response\r\n" ));
        return;
    }

    header             = ( eap_packet_t* ) supplicant_host_get_data(workspace->interface->whd_driver,packet );
    header->eap.code   = EAP_CODE_RESPONSE;
    header->eap.id     = workspace->last_received_id;
    SUPPLICANT_WRITE_16_BE( &aligned_length, ( sizeof( eap_header_t ) + data_length ) );
    header->eap.length = aligned_length;
    header->eap.type   = eap_type;
    memcpy( header->data, data, data_length );
    EAP_WRAPPER_DEBUG( ( "Sending EAP Response\r\n" ) );

    supplicant_send_eapol_packet( packet, workspace, EAP_PACKET, sizeof( eap_header_t ) + data_length );
    EAP_WRAPPER_DEBUG(( "EAP Response sent\r\n" ));
}

cy_rslt_t supplicant_send_zero_length_eap_tls_packet( supplicant_workspace_t* workspace )
{
    supplicant_packet_t     packet;
    eap_tls_packet_t* header;
    uint16_t          aligned_length;

    cy_rslt_t result = supplicant_host_create_packet( workspace->interface->whd_driver, &packet, sizeof(eap_tls_packet_t) );
    if ( result != CY_RSLT_SUCCESS )
    {
        return CY_RSLT_WIFI_SUPPLICANT_GENERIC_ERROR;
    }

    header                = ( eap_tls_packet_t* ) supplicant_host_get_data( workspace->interface->whd_driver,packet );
    header->eap.code      = EAP_CODE_RESPONSE;
    header->eap.id        = workspace->last_received_id;
    SUPPLICANT_WRITE_16_BE( &aligned_length, (sizeof(eap_header_t) + sizeof(eap_tls_header_t)) );
    header->eap.length = aligned_length;
    header->eap_tls.flags = 0;
    header->eap.type      = workspace->eap_type;
    EAP_WRAPPER_DEBUG( ( "Sending zero length EAP-TLS packet\r\n" ) );
    supplicant_send_eapol_packet( packet, workspace, EAP_PACKET, (sizeof(eap_header_t) + sizeof(eap_tls_header_t)) );

    return CY_RSLT_SUCCESS;
}

cy_rslt_t supplicant_send_eap_tls_fragment( supplicant_workspace_t* workspace, supplicant_packet_t packet )
{
    eap_tls_packet_t*  header = ( eap_tls_packet_t* ) supplicant_host_get_data( workspace->interface->whd_driver,packet );

    /* We set the EAP ID here because fragmentation occurs before we know what the EAP ID will be */
    header->eap.id      = workspace->last_received_id;
    uint16_t packet_length = supplicant_host_get_packet_size( workspace->interface->whd_driver,packet );
    EAP_WRAPPER_DEBUG( ("Outgoing eap tls packet_length = [%u]\n", (unsigned int)packet_length) );
    supplicant_send_eapol_packet( packet, workspace, EAP_PACKET, packet_length - sizeof(ether_header_t) - sizeof(eapol_header_t) );

    return CY_RSLT_SUCCESS;
}
