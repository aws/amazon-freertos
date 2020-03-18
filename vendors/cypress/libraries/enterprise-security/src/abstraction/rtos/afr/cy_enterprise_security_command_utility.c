/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *  Implements commands actions for command console application
 *
 *  This file provides command console application to control actions
 *  for enterprise security network
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command_console.h"
#include "cy_enterprise_security_command_utility.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_process_et.h"
#include "cy_enterprise_security.h"
#include "certificate.h"
#include "cy_enterprise_security_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

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
int test_console(int argc, char *argv[], tlv_buffer_t** data);
/******************************************************
 *               Variables Definitions
 ******************************************************/

#define WIFI_ENTERPRISE_SECURITY_COMMANDS \
        { (char*) "join_ent", join_ent, 0, NULL, NULL, (char*) "<ssid> <eap_tls|peap|eap_ttls> [username] [password] [eap] [mschapv2] [client-cert] <wpa2|wpa2_tkip|wpa|wpa_tkip|wpa2_fbt>", (char*) "Join an AP using an enterprise EAP method. DHCP assumed."}, \
        { (char*) "leave_ent", leave_ent, 0, NULL, NULL, (char*) "", (char*) "Leaves an enterprise AP and stops processing enterprise security events."}, \
        { (char*) "mallinfo", mallinfo, 0, NULL, NULL, (char*) "", (char*) "prints current heap size."}, \

const cy_command_console_cmd_t ent_command_table[] =
{
        WIFI_ENTERPRISE_SECURITY_COMMANDS
        CMD_TABLE_END
};

#define ENT_APP_INFO( x ) printf x

#pragma pack(1)
struct cmd_result
{
    char status;
    char flag;
    int  count;
};

static enterprise_security_parameters_t ent_parameters;
/******************************************************
 *               Function Definitions
 ******************************************************/
void ent_utility_init( void )
{
    cy_command_console_add_table( ent_command_table );
}

/*!
 ******************************************************************************
 * Convert a type string to an supplicant_tunnel_auth_type_t type.
 *
 * @param[in] arg  The string containing the value.
 *
 * @return    The value represented by the string.
 */
static cy_supplicant_tunnel_auth_type_t str_to_tunnel_authentication_type( char* arg )
{
    if( strcmp( arg, "eap" ) == 0 )
    {
        return CY_SUPPLICANT_TUNNEL_TYPE_EAP;
    }
    else
    {
        ENT_APP_INFO( ( "Unsupported Tunnel Authentication Type: '%s'\r\n", arg ) );
        return CY_SUPPLICANT_TUNNEL_TYPE_NONE;
    }
}

static cy_supplicant_eap_type_t str_to_enterprise_security_type( char* arg )
{
    if( strcmp( arg, "eap_tls" ) == 0 )
    {
        return CY_SUPPLICANT_EAP_TYPE_TLS;
    }
    else if( strcmp( arg, "peap" ) == 0 )
    {
        return CY_SUPPLICANT_EAP_TYPE_PEAP;
    }
    else if( strcmp( arg, "eap_ttls" ) == 0 )
    {
        return CY_SUPPLICANT_EAP_TYPE_TTLS;
    }
    else if( strcmp( arg, "mschapv2" ) == 0 )
    {
        return CY_SUPPLICANT_EAP_TYPE_MSCHAPV2;
    }
    else if( strcmp( arg, "cisco_leap" ) == 0 )
    {
        return CY_SUPPLICANT_EAP_TYPE_LEAP;
    }
    else
    {
        ENT_APP_INFO( ( "Bad EAP type: '%s'\r\n", arg ) );
        return CY_SUPPLICANT_EAP_TYPE_NONE;
    }
}

cy_supplicant_core_security_t str_to_enterprise_authtype( char* arg )
{
    if( strcmp( arg, "wpa2_tkip" ) == 0 )
    {
        return WHD_SECURITY_WPA2_TKIP_ENT;
    }
    else if( strcmp( arg, "wpa2" ) == 0 )
    {
        return WHD_SECURITY_WPA2_MIXED_ENT;
    }
    else if( strcmp( arg, "wpa" ) == 0 )
    {
        return WHD_SECURITY_WPA_MIXED_ENT;
    }
    else if( strcmp( arg, "wpa_tkip" ) == 0 )
    {
        return WHD_SECURITY_WPA_TKIP_ENT;
    }
    else if( strcmp( arg, "wpa2_fbt" ) == 0 )
    {
        return WHD_SECURITY_WPA2_FBT_ENT;
    }
    else
    {
        ENT_APP_INFO( ( "Bad auth type: '%s'\r\n", arg ) );
        return WHD_SECURITY_UNKNOWN;
    }
}

int join_ent( int argc, char *argv[], tlv_buffer_t** data)
{
    int result = 0;
    char* ssid = argv[ 1 ];
    cy_supplicant_eap_type_t  eap_type;
    cy_supplicant_core_security_t auth_type;
    char *username = NULL;
    char *password = NULL;

    if( argc < 4 )
    {
        ENT_APP_INFO( ( "Error Insufficient arguments\n" ) );
        return ERR_INSUFFICENT_ARGS;
    }
    eap_type = str_to_enterprise_security_type( argv[ 2 ] );
    if( eap_type == CY_SUPPLICANT_EAP_TYPE_NONE )
    {
        ENT_APP_INFO( ( "Unknown security type\n" ) );
        return ERR_BAD_ARG;
    }

    if( ( eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP ) && ( argc < 6 ) )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    if( ( eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS ) && ( argc < 7 ) )
    {
        return ERR_INSUFFICENT_ARGS;
    }

    auth_type = str_to_enterprise_authtype( argv[ argc - 1 ] );
    if( auth_type == WHD_SECURITY_UNKNOWN )
    {
        ENT_APP_INFO( ("Unknown security type\n" ) );
        return ERR_BAD_ARG;
    }

    ent_parameters.is_client_cert_required = 0;

    if( ( eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP ) ||
        ( eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS ) || 
        ( eap_type == CY_SUPPLICANT_EAP_TYPE_TLS ) )
    {
        username = argv[ 3 ];

        /* Copy phase2 identity */
        strncpy( ent_parameters.phase2.inner_identity, username, CY_SUPPLICANT_MAX_IDENTITY_LENGTH );
        ent_parameters.phase2.inner_identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH - 1 ] = '\0';

        if( eap_type == CY_SUPPLICANT_EAP_TYPE_TLS )
        {
            ent_parameters.is_client_cert_required = 1;
        }
        else if( eap_type == CY_SUPPLICANT_EAP_TYPE_PEAP )
        {
            password = argv[ 4 ];
            ent_parameters.phase2.inner_eap_type = CY_SUPPLICANT_EAP_TYPE_MSCHAPV2;
            ent_parameters.phase2.tunnel_auth_type = CY_SUPPLICANT_TUNNEL_TYPE_MSCHAPV2;
            strncpy( ent_parameters.phase2.inner_password, password, CY_SUPPLICANT_MAX_PASSWORD_LENGTH );
            ent_parameters.phase2.inner_password[ CY_SUPPLICANT_MAX_PASSWORD_LENGTH - 1 ] = '\0';
        }
        else if( eap_type == CY_SUPPLICANT_EAP_TYPE_TTLS )
        {
            password = argv[ 4 ];
            strncpy( ent_parameters.phase2.inner_password, password, CY_SUPPLICANT_MAX_PASSWORD_LENGTH );
            ent_parameters.phase2.inner_password[ CY_SUPPLICANT_MAX_PASSWORD_LENGTH - 1 ] = '\0';

            ent_parameters.phase2.tunnel_auth_type = str_to_tunnel_authentication_type( argv[ 5 ] );
            if( ent_parameters.phase2.tunnel_auth_type == CY_SUPPLICANT_TUNNEL_TYPE_EAP )
            {
                ent_parameters.phase2.inner_eap_type = str_to_enterprise_security_type(argv[ 6 ]);
                if( ent_parameters.phase2.inner_eap_type == CY_SUPPLICANT_EAP_TYPE_NONE )
                {
                    ENT_APP_INFO( ( "Unknown inner eap type\n" ) );
                    return ERR_BAD_ARG;
                }
            }
            else
            {
                ENT_APP_INFO( ( "Unsupported Tunnel Authentication Type\n" ) );
                return ERR_BAD_ARG;
            }

            if( argc > 7 )
            {
                char *arg = NULL;
                if( ent_parameters.phase2.tunnel_auth_type == CY_SUPPLICANT_TUNNEL_TYPE_EAP )
                {
                    arg = argv[ 7 ];
                }
                if( strcmp( arg, "client-cert" ) == 0 )
                {
                    ent_parameters.is_client_cert_required = 1;
                }
                else
                {
                    ent_parameters.is_client_cert_required = 0;
                }
            }
            else
            {
                ent_parameters.is_client_cert_required = 0;
            }
        }
    }

    if( ent_parameters.is_client_cert_required == 1 )
    {
        ent_parameters.client_cert = (char*) WIFI_USER_CERTIFICATE_STRING;
        ent_parameters.client_key = (char*) WIFI_USER_PRIVATE_KEY_STRING;
    }
    else
    {
        ent_parameters.client_cert = NULL;
        ent_parameters.client_key = NULL;
    }
    ent_parameters.ca_cert = (char*) WIFI_ROOT_CERTIFICATE_STRING;

    strncpy( ent_parameters.ssid, ssid, SSID_NAME_SIZE );
    ent_parameters.ssid[ SSID_NAME_SIZE - 1 ] = '\0';

    strncpy( ent_parameters.outer_eap_identity, (char *)username, CY_SUPPLICANT_MAX_IDENTITY_LENGTH );
    ent_parameters.outer_eap_identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH - 1 ] = '\0';

    ent_parameters.eap_type = eap_type;
    ent_parameters.auth_type = auth_type;

    result = join( &ent_parameters );

    if( result != CY_SUPPLICANT_STATUS_JOIN_SUCCESS )
    {
        ENT_APP_INFO( ( "Enterprise Security Join failed with error %d\n", result ) );
        return ERR_UNKNOWN;
    }

    ENT_APP_INFO( ( "Enterprise Security Join successful\n" ) );

    return ERR_CMD_OK;
}

int leave_ent( int argc, char *argv[], tlv_buffer_t** data )
{
    int result;

    result = leave();
    if( result != CY_SUPPLICANT_STATUS_LEAVE_SUCCESS )
    {
        return ERR_UNKNOWN;
    }

    return ERR_CMD_OK;
}

int mallinfo( int argc, char *argv[], tlv_buffer_t** data )
{
    /* Used to keep track of heap memory */
    ENT_APP_INFO( ( "\r\nCurrent Heap size: %u bytes", (unsigned int) configTOTAL_HEAP_SIZE ) );
    ENT_APP_INFO( ( "\r\n Available Heap size: %u bytes\r\n", (unsigned int) xPortGetFreeHeapSize() ) );
    return ERR_CMD_OK;
}

#ifdef __cplusplus
}
#endif
