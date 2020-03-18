/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_peap.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define CY_SUPPLICANT_PEAP_DEBUG( x )  //printf x

/******************************************************
 *               Function Definations
 ******************************************************/
supplicant_packet_t supplicant_create_peap_response_packet( supplicant_packet_t* packet, eap_type_t eap_type, uint16_t data_length, uint8_t length_field_overhead, supplicant_workspace_t* workspace )
{
    eap_tls_packet_t*   header = NULL;
    cy_rslt_t           result;
    tls_record_t*       record = NULL;
    peap_header_t*      peap_header = NULL;
    uint16_t            header_space = 0;
    uint16_t            footer_pad_space = 0;

    supplicant_tls_calculate_overhead( workspace, data_length, &header_space, &footer_pad_space );

    result = supplicant_host_create_packet(  workspace->interface->whd_driver,packet, sizeof(eap_tls_packet_t) + sizeof(peap_header_t) + length_field_overhead + data_length + header_space + footer_pad_space + 10 );
    if ( result != CY_RSLT_SUCCESS )
    {
        return NULL;
    }

    header                = ( eap_tls_packet_t* ) supplicant_host_get_data( workspace->interface->whd_driver,*packet );

    header->eap.code      = EAP_CODE_RESPONSE;
    header->eap.id        = workspace->last_received_id;
    header->eap.type      = CY_SUPPLICANT_EAP_TYPE_PEAP;
    if ( length_field_overhead > 0 )
    {
        header->eap_tls.flags = EAP_TLS_FLAG_LENGTH_INCLUDED; /* Always include length until such time that we need to support fragmentation on transmit of PEAP packets */
    }
    else
    {
        header->eap_tls.flags = 0;
    }

    record                = (tls_record_t*) ( header->data + length_field_overhead );

    record->type          = 23;
    record->major_version = (uint8_t)workspace->tls_context->context.major_ver;
    record->minor_version = (uint8_t)workspace->tls_context->context.minor_ver;
    record->length        = htobe16( sizeof( peap_header_t ) + data_length );
    peap_header           = ( peap_header_t* ) record->message;
    peap_header->type     = eap_type;

    /* set data point to inner eap packet after all headers */

    supplicant_inner_packet_set_data(  workspace->interface->whd_driver,packet, sizeof(eap_tls_packet_t) + sizeof(peap_header_t) + sizeof(tls_record_header_t) + length_field_overhead - 1 );

    return packet;
}

void supplicant_send_peap_response_packet( supplicant_packet_t* packet, supplicant_workspace_t* workspace )
{
    eap_tls_packet_t*   header;
    tls_record_t*       record;
    uint32_t            data_length;
    uint8_t             length_field_overhead = 0;
    uint16_t            aligned_length;

    /* return to eap_tls_packet_t header */
    supplicant_inner_packet_set_data( workspace->interface->whd_driver,packet, -1 * (sizeof(eap_tls_packet_t) + sizeof(peap_header_t) + sizeof(tls_record_header_t) + length_field_overhead - 1));

    header                = ( eap_tls_packet_t* ) supplicant_host_get_data(workspace->interface->whd_driver, *packet );


    if ( header->eap_tls.flags &  EAP_TLS_FLAG_LENGTH_INCLUDED )
    {
        length_field_overhead = 4;
    }
    record                = (tls_record_t*) ( header->data + length_field_overhead );

    data_length           = htobe16(record->length);

    if( mbedtls_ssl_write( &workspace->tls_context->context , record->message, data_length ) < 0 )
    {
        return;
    }

    data_length = workspace->tls_context->context.out_msglen + sizeof(tls_record_header_t);

    memcpy(&record->type, workspace->tls_context->context.out_hdr, data_length);

    if ( length_field_overhead )
    {
        SUPPLICANT_WRITE_32_BE( &header->data, data_length );
    }

    data_length  += sizeof( eap_header_t ) + sizeof(eap_tls_header_t) + length_field_overhead;
    SUPPLICANT_WRITE_16_BE( &aligned_length, data_length );
    header->eap.length = aligned_length;

    supplicant_send_eapol_packet( *packet, workspace, EAP_PACKET, data_length );
}

cy_rslt_t supplicant_process_peap_event(supplicant_workspace_t* workspace, supplicant_packet_t packet)
{
    supplicant_phase2_state_t* peap = NULL;
    peap_packet_t* peap_packet = NULL;

    CY_SUPPLICANT_PEAP_DEBUG(("\r\n IN PROCESS PEAP EVENT \r\n"));

    peap = (supplicant_phase2_state_t*) workspace->ptr_phase2;

    peap_packet = (peap_packet_t*) supplicant_host_get_data(workspace->interface->whd_driver, packet );


    if ( peap_packet->type == CY_SUPPLICANT_EAP_TYPE_IDENTITY )
    {
        CY_SUPPLICANT_PEAP_DEBUG(("\r\n  %s %d \r\n",__FUNCTION__,__LINE__));

        /* this is probably an extension packet and this is not the type but rather the code */
        eap_header_t* eap_packet = (eap_header_t*) peap_packet;
        if ( eap_packet->type == 33 )
        {
            supplicant_packet_t response_packet;
            uint8_t* data;
            peap_extention_request_t* request = (peap_extention_request_t*) peap_packet;
            peap_extention_response_t* response;
            uint16_t aligned_length;
            CY_SUPPLICANT_PEAP_DEBUG(("\r\n  %s %d \r\n",__FUNCTION__,__LINE__));

            supplicant_create_peap_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_IDENTITY, sizeof(peap_extention_response_t) - sizeof(peap_header_t), workspace->tls_length_overhead, workspace );
            data = supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
            CY_SUPPLICANT_PEAP_DEBUG( ( "Supplicant peap received extension success\n" ) );

            response = (peap_extention_response_t*) ( data - sizeof(peap_header_t) );
            response->header.code = 2;
            response->header.id = request->header.id;
            response->header.type = request->header.type;
            SUPPLICANT_WRITE_16_BE( &aligned_length, sizeof(peap_extention_response_t) );
            response->header.length = aligned_length;
            memcpy( &response->avp[ 0 ], &request->avp[ 0 ], sizeof( response->avp[ 0 ] ) );
            peap->result = CY_RSLT_WIFI_SUPPLICANT_ABORTED;
            supplicant_send_peap_response_packet( &response_packet, workspace );
        }
        else
        {
            /* Peap packets doens't include Code, ID, or length, they are all obtained from EAP packet */
            supplicant_packet_t response_packet;
            uint8_t * data;
            CY_SUPPLICANT_PEAP_DEBUG(("\r\n  %s %d \r\n",__FUNCTION__,__LINE__));

            CY_SUPPLICANT_PEAP_DEBUG( ( "Supplicant peap received EAP ID Request\n" ) );

            peap->sub_stage = SUPPLICANT_EAP_IDENTITY;
            supplicant_create_peap_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_IDENTITY, peap->identity_length, workspace->tls_length_overhead, workspace );
            data = supplicant_host_get_data(workspace->interface->whd_driver, response_packet );
            memcpy( data, peap->identity, peap->identity_length );
            supplicant_send_peap_response_packet( &response_packet, workspace );
        }
    }
    else if ( ( peap_packet->type != peap->eap_type ) && ( peap->sub_stage == SUPPLICANT_EAP_IDENTITY ) )
    {
        CY_SUPPLICANT_PEAP_DEBUG( ( "Supplicant received PEAP packet Type %u\n", peap_packet->type ) );

        peap->sub_stage = SUPPLICANT_EAP_NAK;
    }
    else if ( ( peap_packet->type == peap->eap_type ) && ( ( peap->sub_stage == SUPPLICANT_EAP_IDENTITY ) || ( peap->sub_stage == SUPPLICANT_EAP_NAK ) || ( peap->sub_stage == SUPPLICANT_EAP_METHOD ) ) )
    {
        CY_SUPPLICANT_PEAP_DEBUG( ( "Supplicant received required EAP Type %u\n", peap->eap_type ) );

        if ( peap->eap_type == CY_SUPPLICANT_EAP_TYPE_MSCHAPV2 )
        {
            CY_SUPPLICANT_PEAP_DEBUG( ( "Process MSCHAP packet\n" ) );
            mschap_process_packet( (mschapv2_packet_t*) peap_packet->data, workspace );
            peap->sub_stage = SUPPLICANT_EAP_METHOD;
        }
    }
    return CY_RSLT_SUCCESS;
}
