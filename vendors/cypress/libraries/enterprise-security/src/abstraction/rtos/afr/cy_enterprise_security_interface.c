/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Implements user functions for joining/leaving enterprise security network
 *
 *  This file provides end-user functions which allow joining or leaving
 *  enterprise security network.
 *
 */

#include "cy_wifi_abstraction.h"
#include "cy_enterprise_security_interface.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_process_et.h"

#define ENTERPRISE_SECUTIRY_DEBUG_INFO(x) printf x

cy_supplicant_status_t join( enterprise_security_parameters_t* ent_parameters )
{
    cy_supplicant_status_t result = CY_SUPPLICANT_STATUS_JOIN_SUCCESS;
    int res = 0;

    supplicant_instance.tls_security.ca_cert = ent_parameters->ca_cert;
    supplicant_instance.tls_security.ca_cert_len = strlen( ent_parameters->ca_cert );
    supplicant_instance.tls_security.cert = ent_parameters->client_cert;
    supplicant_instance.tls_security.cert_len = strlen( ent_parameters->client_cert );
    supplicant_instance.tls_security.key = ent_parameters->client_key;
    supplicant_instance.tls_security.key_len = strlen( ent_parameters->client_key );

    if( is_wifi_connected() == WIFI_CONNECTED )
    {
        return CY_SUPPLICANT_STATUS_UP;
    }

    result = cy_join_ent_init( &supplicant_instance );
    if ( result != CY_SUPPLICANT_STATUS_PASS )
    {
        ENTERPRISE_SECUTIRY_DEBUG_INFO( ( " Unable to perform join ent init %d\n", result ) );
        return CY_SUPPLICANT_STATUS_SUPPLICANT_INIT_FAILURE;
    }

    /* Copy SSID */
    strncpy( supplicant_instance.ssid, ent_parameters->ssid, SSID_NAME_SIZE );
    supplicant_instance.ssid[ SSID_NAME_SIZE - 1 ] = '\0';

    /* Copy EAP identity */
    strncpy( supplicant_instance.outer_eap_identity, (const char *)ent_parameters->outer_eap_identity, CY_SUPPLICANT_MAX_IDENTITY_LENGTH );
    supplicant_instance.outer_eap_identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH - 1 ] = '\0';
    supplicant_instance.outer_eap_identity_length = strlen( supplicant_instance.outer_eap_identity ) + 1;

    /* Copy EAP and Auth type */
    supplicant_instance.eap_type = ent_parameters->eap_type;
    supplicant_instance.auth_type = ent_parameters->auth_type;

    if( ent_parameters->eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP )
    {
        supplicant_instance.phase2_config.tunnel_auth_type = ent_parameters->phase2.tunnel_auth_type;
        /* Copy PEAP identiy */
        strncpy( supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.identity,
                 ent_parameters->phase2.inner_identity, CY_SUPPLICANT_MAX_IDENTITY_LENGTH );
        supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH - 1 ] = '\0';
        supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.identity_length =
            strlen( supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.identity ) + 1;

        /* Copy PEAP password */
        strncpy( supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.password,
                 ent_parameters->phase2.inner_password, CY_SUPPLICANT_MAX_PASSWORD_LENGTH );
        supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.password[ CY_SUPPLICANT_MAX_PASSWORD_LENGTH - 1 ] = '\0';
        supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.password_length =
            strlen( supplicant_instance.phase2_config.tunnel_protocol.peap.inner_identity.password ) + 1;
    }
    else if( ent_parameters->eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS )
    {
        supplicant_instance.phase2_config.tunnel_auth_type = ent_parameters->phase2.tunnel_auth_type;
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_eap_type = ent_parameters->phase2.inner_eap_type;
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.is_client_cert_required = ent_parameters->is_client_cert_required;
        /* Copy EAP-TTLS identiy */
        strncpy( supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.identity,
                 ent_parameters->phase2.inner_identity, CY_SUPPLICANT_MAX_IDENTITY_LENGTH );
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH - 1 ] = '\0';
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.identity_length =
            strlen( supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.identity ) + 1;

        /* Copy EAP-TTLS password */
        strncpy( supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.password,
                 ent_parameters->phase2.inner_password, CY_SUPPLICANT_MAX_PASSWORD_LENGTH );
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.password[ CY_SUPPLICANT_MAX_PASSWORD_LENGTH - 1 ] = '\0';
        supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.password_length =
            strlen( supplicant_instance.phase2_config.tunnel_protocol.eap_ttls.inner_identity.password ) + 1;
    }

    wifi_on_ent();

    result = cy_join_ent( &supplicant_instance );
    if( result != CY_SUPPLICANT_STATUS_PASS )
    {
        cy_join_ent_deinit( &supplicant_instance );
        ENTERPRISE_SECUTIRY_DEBUG_INFO( ( "ERROR: cy_join_ent failed with error %d\n", result ) );
        return result;
    }
    res = connect_ent( ent_parameters->ssid, strlen( ent_parameters->ssid ) + 1, NULL, 0, ent_parameters->auth_type );
    if( res != CY_SUPPLICANT_STATUS_PASS )
    {
        cy_leave_ent( &supplicant_instance );
        cy_join_ent_deinit( &supplicant_instance );
        ENTERPRISE_SECUTIRY_DEBUG_INFO( ( "ERROR: connect failed with error %d\n", res ) );
        return CY_SUPPLICANT_STATUS_JOIN_FAILURE;
    }

    return CY_SUPPLICANT_STATUS_JOIN_SUCCESS;
}

cy_supplicant_status_t leave()
{
    int res = 0;

    cy_supplicant_status_t result = CY_SUPPLICANT_STATUS_LEAVE_SUCCESS;
    if( is_wifi_connected() != WIFI_CONNECTED )
    {
        return CY_SUPPLICANT_STATUS_DOWN;
    }

    result = cy_leave_ent( &supplicant_instance );
    if( result !=  CY_SUPPLICANT_STATUS_PASS )
    {
        return CY_SUPPLICANT_STATUS_FAIL;
    }

    result = cy_join_ent_deinit( &supplicant_instance );
    if( result !=  CY_SUPPLICANT_STATUS_PASS )
    {
        return CY_SUPPLICANT_STATUS_FAIL;
    }

    res = disconnect_ent();
    if( res != CY_SUPPLICANT_STATUS_PASS )
    {
        ENTERPRISE_SECUTIRY_DEBUG_INFO( ( "ERROR: disconnect failed with error %d\n", result ) );
        return CY_SUPPLICANT_STATUS_LEAVE_FAILURE;
    }

    ENTERPRISE_SECUTIRY_DEBUG_INFO( ( "successfully left\r\n" ) );
    return CY_SUPPLICANT_STATUS_LEAVE_SUCCESS;
}
