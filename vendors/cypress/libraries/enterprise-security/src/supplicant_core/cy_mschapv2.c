/*
 * $ Copyright Cypress Semiconductor $
 */

#include "cy_mschapv2.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define CY_SUPPLICANT_MSCHAPV2_DEBUG( x )  //printf x

/******************************************************
 *                      Constants
 ******************************************************/
#define SHA1_LENGTH (20)

/******************************************************
 *                Functions Definations
 ******************************************************/
cy_rslt_t mschap_challenge_hash(uint8_t* peer_challenge, uint8_t* authenticator_challenge, char* user_name, uint8_t* challenge)
{
    mbedtls_sha1_context sha1_ctx;
    uint8_t hash_value[SHA1_LENGTH];
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    mbedtls_sha1_init( &sha1_ctx );
    mbedtls_sha1_starts_ret( &sha1_ctx );
    mbedtls_sha1_update_ret( &sha1_ctx, (unsigned char *)peer_challenge, 16 );
    mbedtls_sha1_update_ret( &sha1_ctx, (unsigned char *)authenticator_challenge, 16 );
    mbedtls_sha1_update_ret( &sha1_ctx, (unsigned char *)user_name, strlen((const char*)user_name) );
    mbedtls_sha1_finish_ret( &sha1_ctx, (unsigned char *)hash_value );
    mbedtls_sha1_free( &sha1_ctx );

    memcpy( challenge, hash_value, 8);

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_nt_password_hash( char* password, uint16_t length, uint8_t* password_hash )
{
    mbedtls_md4_context md4_ctx;
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    mbedtls_md4_starts_ret( &md4_ctx );
    mbedtls_md4_update_ret( &md4_ctx, (unsigned char *)password, length );
    mbedtls_md4_finish_ret( &md4_ctx, (unsigned char *)password_hash );

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_permute_key(uint8_t* key56, uint8_t* key64)
{
    int i, shift;
    key64[0] = key56[0] & 0xFE;
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    for (i = 1, shift = 1; i < 8; i++, shift++)
    {
        key64[i] = 0;
        key64[i] = ( key56[i-1] << (8-shift) ) | ( key56[i] >> shift);
    }

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_des_encrypt( uint8_t* clear, uint8_t* key, uint8_t* cypher)
{
    mbedtls_des_context des_ctx;
    uint8_t key64[ 8 ];
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    mschap_permute_key( key, key64 );
    mbedtls_des_init( &des_ctx );
    mbedtls_des_setkey_enc( &des_ctx, key64 );
    mbedtls_des_crypt_ecb(&des_ctx, clear, cypher);
    mbedtls_des_free( &des_ctx );

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_challenge_response( uint8_t* challenge, uint8_t* nt_password_hash, uint8_t* nt_response )
{
    int pwd_index =0, rsp_index = 0;
    uint8_t  z_password_hash[21 + 1]={0};
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    memcpy(z_password_hash, nt_password_hash, 16);

    mschap_des_encrypt( challenge, &z_password_hash[pwd_index], &nt_response[rsp_index] );
    pwd_index += 7;
    rsp_index += 8;
    mschap_des_encrypt( challenge, &z_password_hash[pwd_index], &nt_response[rsp_index] );
    pwd_index += 7;
    rsp_index += 8;
    mschap_des_encrypt( challenge, &z_password_hash[pwd_index], &nt_response[rsp_index] );

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_generate_nt_response(uint8_t* authenticator_challenge, uint8_t* peer_challenge, char* user_name, char* password, uint16_t password_length, uint8_t* nt_response)
{
    uint8_t challenge[8];
    uint8_t password_hash[16];
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %s %d \r\n",__FILE__,__FUNCTION__,__LINE__));

    mschap_challenge_hash( peer_challenge, authenticator_challenge, user_name, challenge);
    mschap_nt_password_hash( password, password_length, password_hash );
    mschap_challenge_response( challenge, password_hash, nt_response );

    return CY_RSLT_SUCCESS;
}

cy_rslt_t mschap_process_packet( mschapv2_packet_t *packet, supplicant_workspace_t *workspace )
{

    supplicant_phase2_state_t* phase2 = (supplicant_phase2_state_t*)workspace->ptr_phase2;

    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

    if ( packet->opcode == MSCHAPV2_OPCODE_CHALLENGE )
    {
        supplicant_packet_t                response_packet = NULL;
        mschapv2_response_packet_t*  response;
        mschapv2_challenge_packet_t* challenge = (mschapv2_challenge_packet_t*) packet;
        uint16_t                     packet_size =  sizeof( mschapv2_response_packet_t ) + phase2->identity_length -1;
        uint8_t                      peer_challenge[16]= {0};
        uint16_t                     aligned_length;
        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

        cy_crypto_get_random( workspace->tls_context, peer_challenge, sizeof( peer_challenge ) );

        if (workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP)
        {
            supplicant_create_peap_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_MSCHAPV2, packet_size, workspace->tls_length_overhead, workspace);
        }
        else if(workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS)
        {
            supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_MSCHAPV2, packet_size, workspace->tls_length_overhead, workspace);
        }
        if ( response_packet == NULL )
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

            return CY_RSLT_WIFI_SUPPLICANT_UNPROCESSED;
        }
        response = (mschapv2_response_packet_t *)supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
        response->opcode     = 2;
        response->id         = challenge->id;
        response->value_size = 0x31;
        memcpy( response->peer_challenge, peer_challenge, sizeof(peer_challenge));
        memset( response->reserved, 0, sizeof(response->reserved ));
        memcpy( response->name, phase2->identity, phase2->identity_length);
        SUPPLICANT_WRITE_16_BE( &aligned_length, packet_size );
        response->length = aligned_length;
        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\nphase2->identity %s len %d \r\n",phase2->identity,phase2->identity_length));
        /* coverity[overrun-buffer-val]
            FALSE-POSITIVE: peap->password_length never exceeds 32. So parameter 'ilen' passed to md4_update() won't be more than 32.
            Hence, 'ilen' >= 64 condition will never be true. And 'input' will not be indexed at byte position 63 as reported by coverity */
        mschap_generate_nt_response( challenge->challenge, peer_challenge, (char*)phase2->identity, (char*)phase2->password, phase2->password_length, response->nt_reponse);

        if (workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP)
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));
            supplicant_send_peap_response_packet( &response_packet, workspace );
        }
        else if(workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS)
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));
            supplicant_send_ttls_response_packet( &response_packet, workspace );
        }
    }
    else if ( packet->opcode == MSCHAPV2_OPCODE_SUCCESS )
    {
        mschapv2_success_response_packet_t* response;
        supplicant_packet_t                       response_packet = NULL;
        uint16_t                            packet_size   =  sizeof( mschapv2_success_response_packet_t );

        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

        if (workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP)
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

            supplicant_create_peap_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_MSCHAPV2, packet_size, workspace->tls_length_overhead, workspace);
        }
        else if(workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS)
        {
            supplicant_create_ttls_response_packet( &response_packet, CY_SUPPLICANT_EAP_TYPE_MSCHAPV2, packet_size, workspace->tls_length_overhead, workspace);
        }

        if ( response_packet == NULL )
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

            return CY_RSLT_WIFI_SUPPLICANT_UNPROCESSED;
        }

        response = (mschapv2_success_response_packet_t *)supplicant_host_get_data( workspace->interface->whd_driver,response_packet );
        response->opcode = 3;

        if (workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP)
        {
            CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

            supplicant_send_peap_response_packet( &response_packet, workspace );
        }
        else if(workspace->eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS)
        {
            supplicant_send_ttls_response_packet( &response_packet, workspace );
            phase2->result = CY_RSLT_SUCCESS;
        }
    }
    else if ( packet->opcode == MSCHAPV2_OPCODE_FAILURE )
    {
        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));
    }
    else
    {
        CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

        return CY_RSLT_WIFI_SUPPLICANT_UNPROCESSED;
    }
    CY_SUPPLICANT_MSCHAPV2_DEBUG(("\r\n %s %d \r\n",__FUNCTION__,__LINE__));

    return CY_RSLT_SUCCESS;
}
