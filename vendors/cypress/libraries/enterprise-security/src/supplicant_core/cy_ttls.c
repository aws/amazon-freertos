/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_ttls.h"

/******************************************************
 *                   Macros
 ******************************************************/
#define CY_SUPPLICANT_TTLS_DEBUG( x )   //printf

/******************************************************
 *              Function Prototypes
 ******************************************************/
extern int mbedtls_ssl_write_record( mbedtls_ssl_context *ssl, uint8_t force_flush );

/******************************************************
 *              Function definations
 ******************************************************/
/*
 ******************************************************************************
 * Finds AVP in Data that is encapsulated within TLS record layer
 */
avp_header_t* find_avp(uint8_t* data, uint8_t data_length, uint32_t avp_code)
{
    uint8_t left = data_length;
    uint8_t pad=0;
    avp_header_t* avp_header = (avp_header_t*)data;
    uint32_t code;
    uint32_t length = 0;

    while( left > 0 )
    {
        code = htobe32(avp_header->avp_code);
        if(code == avp_code)
        {
            if(avp_header->flags & AVP_FLAG_VENDOR_MASK)
            {
                /* not supporting any vendor attributes as of now */
                return NULL;
            }
            return avp_header;
        }

        length = ( ( ((uint32_t)avp_header->avp_length[0]) << 16 ) | ( ((uint32_t)avp_header->avp_length[1]) << 8 ) | avp_header->avp_length[2] );

        /* as per EAP-TTLSv0 RFC[rfc5281], Each AVP must begin on a
           four-octet boundary relative to the first AVP in the sequence */
        pad = (4-(length & 0x3)) & 0x3;

        avp_header = (avp_header_t*)((uint8_t *)avp_header + length + pad);
        left -= length + pad;
    }
    return NULL;
}

cy_rslt_t leap_process_packet( leap_header *packet, supplicant_workspace_t *workspace )
{
    supplicant_phase2_state_t*   phase2 = (supplicant_phase2_state_t*)workspace->ptr_phase2;
    supplicant_packet_t          response_packet;
    leap_header*                 response;
    uint8_t                      nt_reponse[LEAP_RESPONSE_LEN];
    uint16_t                     packet_size;
    uint8_t                      *challenge;
    uint8_t                      password_hash[64];
    uint8_t                      *leap_data = NULL;
    uint8_t                      password_hash_hash[64];
    uint8_t                      *ap_response;

    CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
    switch(phase2->sub_stage)
    {
        case SUPPLICANT_EAP_IDENTITY:
            challenge =    (uint8_t *)(packet + 1);
            packet_size =  sizeof(leap_header) + phase2->identity_length + LEAP_RESPONSE_LEN -1;
            CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
            supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_LEAP, packet_size, workspace->tls_length_overhead, workspace);

            response = (leap_header *)supplicant_host_get_data(workspace->interface->whd_driver, response_packet );
            response->Version     = LEAP_VERSION;
            response->reserved    = 0;
            response->count = LEAP_RESPONSE_LEN;

            mschap_nt_password_hash( (char *)phase2->password, phase2->password_length, password_hash );
            mschap_challenge_response( challenge, password_hash, nt_reponse);

            memcpy((uint8_t *) (response + 1), nt_reponse, LEAP_RESPONSE_LEN);

            leap_data = (uint8_t *) (response + 1) + LEAP_RESPONSE_LEN;
            memcpy( leap_data, phase2->identity, phase2->identity_length);
            supplicant_send_ttls_response_packet( &response_packet, workspace );

            phase2->sub_stage = SUPPLICANT_LEAP_RESPOND_CHALLENGE;
            break;

        case SUPPLICANT_LEAP_RESPOND_CHALLENGE:

            CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
            packet_size =  sizeof(leap_header) + phase2->identity_length + LEAP_CHALLENGE_LEN -1;

            supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_LEAP, packet_size, workspace->tls_length_overhead, workspace);

            response = (leap_header *)supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
            response->Version     = LEAP_VERSION;
            response->reserved    = 0;
            response->count = LEAP_CHALLENGE_LEN;

            cy_crypto_get_random( workspace->tls_context, phase2->leap_ap_challenge, LEAP_CHALLENGE_LEN );

            leap_data = (uint8_t *) (response + 1);

            memcpy( leap_data, phase2->leap_ap_challenge, LEAP_CHALLENGE_LEN);

            leap_data += LEAP_CHALLENGE_LEN;

            memcpy( leap_data, phase2->identity, phase2->identity_length);

            supplicant_send_ttls_response_packet( &response_packet, workspace );

            phase2->sub_stage = SUPPLICANT_LEAP_REQUEST_CHALLENGE;
            break;

        case SUPPLICANT_LEAP_REQUEST_CHALLENGE:
            CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
            ap_response = (uint8_t *)(packet + 1);

            mschap_nt_password_hash( (char *)phase2->password, phase2->password_length, password_hash );
            mschap_nt_password_hash( (char *)password_hash, phase2->password_length, password_hash_hash );

            mschap_challenge_response( phase2->leap_ap_challenge, password_hash_hash, nt_reponse);

            if(memcmp(ap_response, nt_reponse, LEAP_RESPONSE_LEN) == 0)
            {
                phase2->result = CY_RSLT_SUCCESS;
            }
            else
            {
                phase2->result = CY_RSLT_WIFI_SUPPLICANT_ERROR;
            }

            phase2->sub_stage = SUPPLICANT_LEAP_DONE;
            break;

        default:
            CY_SUPPLICANT_TTLS_DEBUG( ("Invalid packet\n" ) );
    }

    return CY_RSLT_SUCCESS;
}

/*!
 ******************************************************************************
 * Process the inner EAP handshake message
 */
cy_rslt_t supplicant_process_ttls_phase2_event(supplicant_workspace_t* workspace, supplicant_packet_t packet)
{
    supplicant_phase2_state_t* phase2_state = (supplicant_phase2_state_t*)workspace->ptr_phase2;
    uint8_t* data =  supplicant_host_get_data(workspace->interface->whd_driver,packet);
    uint8_t data_length = supplicant_host_get_packet_size(workspace->interface->whd_driver,packet);
    avp_header_t*    avp_header = NULL;
    eap_header_t*    eap_header = NULL;
    mschapv2_packet_t *mschap_packet = NULL;
    leap_header *leapheader = NULL;

    CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
    if(workspace->tunnel_auth_type != CY_SUPPLICANT_TUNNEL_TYPE_EAP)
    {
        CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
        return CY_RSLT_WIFI_SUPPLICANT_ERROR;
    }

    avp_header = find_avp(data, data_length, AVP_CODE_EAP_MESSAGE);
    if(avp_header == NULL)
    {
        CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
        return CY_RSLT_WIFI_SUPPLICANT_ERROR;
    }

    eap_header = (eap_header_t*)(avp_header + 1);
    phase2_state->request_id = eap_header->id;
    if(phase2_state->sub_stage == SUPPLICANT_LEAP_IDENTITY && phase2_state->eap_type != eap_header->type)
    {
        supplicant_packet_t response_packet;
        uint8_t *data = NULL;
        CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
        /* send NAK with desired authentication Type*/
        supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_NAK, 1, workspace->tls_length_overhead, workspace);
        data = supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
        memcpy(data, &phase2_state->eap_type, 1);
        supplicant_send_ttls_response_packet( &response_packet, workspace );
        return CY_RSLT_SUCCESS;
    }

    switch (phase2_state->eap_type)
    {
        case CY_SUPPLICANT_EAP_TYPE_LEAP:
            CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
            leapheader = (leap_header *)(eap_header + 1);
            leap_process_packet(leapheader, workspace);
            break;

        case CY_SUPPLICANT_EAP_TYPE_MSCHAPV2:
            CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
            mschap_packet = (mschapv2_packet_t *)(eap_header + 1);
            mschap_process_packet(mschap_packet, workspace );
            break;

        default:
            CY_SUPPLICANT_TTLS_DEBUG( ("Unsupported inner eap type\n" ) );
    }

    return CY_RSLT_SUCCESS;
}

/*
 ******************************************************************************
 * Sends TTLS response packet
 * contains inner EAP packet.
 */
void supplicant_send_ttls_response_packet( supplicant_packet_t* packet, supplicant_workspace_t* workspace )
{
    eap_tls_packet_t*   header;
    tls_record_t*       record;
    uint32_t            data_length;
    uint8_t             length_field_overhead = 0;
    uint16_t            aligned_length;

    /* return to eap_tls_packet_t header */
    supplicant_inner_packet_set_data( workspace->interface->whd_driver,packet, -1 * (sizeof(eap_tls_packet_t) + sizeof(avp_header_t) + sizeof(eap_header_t) + sizeof(tls_record_header_t) + workspace->tls_length_overhead - 1));

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

/*!
 ******************************************************************************
 * Creates TTLS response packet, TLS record layer includes AVP and AVP data
 * contains inner EAP packet.
 */
supplicant_packet_t supplicant_create_ttls_response_packet( supplicant_packet_t* packet, eap_type_t eap_type, uint16_t data_length, uint8_t length_field_overhead, supplicant_workspace_t* workspace )
{

    eap_tls_packet_t*   header;
    cy_rslt_t       result;
    tls_record_t*       record;
    avp_header_t*       avp_header;
    uint16_t            header_space;
    uint16_t            footer_pad_space;
    uint32_t            avp_length;
    eap_header_t        *inner_eap;
    supplicant_phase2_state_t* phase2_state = (supplicant_phase2_state_t*)workspace->ptr_phase2;

    supplicant_tls_calculate_overhead( workspace, data_length, &header_space, &footer_pad_space );

    result = supplicant_host_create_packet(workspace->interface->whd_driver, packet, sizeof(eap_tls_packet_t) + sizeof(avp_header_t) + sizeof(eap_header_t) + length_field_overhead + data_length + header_space + footer_pad_space + 10 );
    if ( result != CY_RSLT_SUCCESS )
    {
        return NULL;
    }

    /* Outer EAP Header */
    header                = ( eap_tls_packet_t* ) supplicant_host_get_data(workspace->interface->whd_driver, *packet );
    header->eap.code      = EAP_CODE_RESPONSE;
    header->eap.id        = workspace->last_received_id;
    header->eap.type      = CY_SUPPLICANT_EAP_TYPE_TTLS;

    if ( length_field_overhead > 0 )
    {
        header->eap_tls.flags = EAP_TLS_FLAG_LENGTH_INCLUDED;
    }
    else
    {
        header->eap_tls.flags = 0;
    }

    /* TLS Header */
    record                = (tls_record_t*) ( header->data + length_field_overhead );
    record->type          = 23;
    record->major_version = (uint8_t)workspace->tls_context->context.major_ver;
    record->minor_version = (uint8_t)workspace->tls_context->context.minor_ver;
    record->length        = htobe16( sizeof( avp_header_t ) +  sizeof( eap_header_t ) + data_length );

    /*AVP Header */
    avp_header  = ( avp_header_t* ) record->message;
    avp_header->avp_code = htobe32(AVP_CODE_EAP_MESSAGE);
    avp_header->flags = AVP_FLAG_MANDATORY_MASK;
    /* AVP length is a 3 octet value */
    avp_length = sizeof(avp_header_t) + sizeof( eap_header_t) + data_length;
    avp_length = htobe32(avp_length);
    memcpy(avp_header->avp_length, (((uint8_t *)&avp_length) + 1), AVP_LENGTH_SIZE);

    /*Inner EAP Header */
    inner_eap = (eap_header_t *)(avp_header + 1);
    if(phase2_state->sub_stage == SUPPLICANT_LEAP_RESPOND_CHALLENGE)
    {
        inner_eap->code = EAP_CODE_REQUEST;
    }
    else
    {
        inner_eap->code = EAP_CODE_RESPONSE;
    }
    inner_eap->length = htobe16(sizeof( eap_header_t) + data_length);
    inner_eap->type = eap_type;
    inner_eap->id = phase2_state->request_id;

    /* set data point to inner eap packet after all headers */
    supplicant_inner_packet_set_data( workspace->interface->whd_driver,packet, sizeof(eap_tls_packet_t) + sizeof(avp_header_t) + sizeof(eap_header_t) + sizeof(tls_record_header_t) + length_field_overhead - 1 );

    return packet;
}

/*!
 ******************************************************************************
 * Initializes inner authentication by sending inner EAP message, EAP-Response/Identity
 */
cy_rslt_t supplicant_init_ttls_phase2_handshake(supplicant_workspace_t* workspace)
{
    supplicant_packet_t response_packet;
    uint8_t * data;
    supplicant_phase2_state_t* phase2_state;
    CY_SUPPLICANT_TTLS_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));
    supplicant_phase2_init( workspace, workspace->inner_eap_type );

    phase2_state = (supplicant_phase2_state_t*)workspace->ptr_phase2;

    supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_IDENTITY,
                                            phase2_state->identity_length, workspace->tls_length_overhead,
                                            workspace );
    data = supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
    memcpy( data, phase2_state->identity, phase2_state->identity_length );
    supplicant_send_ttls_response_packet( &response_packet, workspace );
    phase2_state->sub_stage = SUPPLICANT_EAP_IDENTITY;

    if ( supplicant_phase2_start( workspace ) != CY_RSLT_SUCCESS )
    {
        CY_SUPPLICANT_TTLS_DEBUG( ( "Supplicant Phase2 failed to start\n" ) );
    }

    return CY_RSLT_SUCCESS;
}
