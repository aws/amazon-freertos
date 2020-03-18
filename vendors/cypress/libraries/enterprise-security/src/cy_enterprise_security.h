/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include  "cy_supplicant_result.h"
#include  "cy_type_defs.h"
#include  "cy_supplicant_core_constants.h"
#include  "whd.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/**
 * @addtogroup enterprise_security_defines
 *  @{
 */
#define CY_SUPPLICANT_MAX_IDENTITY_LENGTH ( 32 )        /**< Maximum identity length supported */
#define CY_SUPPLICANT_MAX_PASSWORD_LENGTH ( 64 )        /**< Maximum password length to use */
/**
 * @}
 */
/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * @addtogroup enterprise_security_enums
 *  @{
 */

/** Supplicant status
 *
 * @brief
 * Various supplicant status codes.
 */
typedef enum
{
    CY_SUPPLICANT_STATUS_JOIN_SUCCESS = 0,              /**< Join successful */
    CY_SUPPLICANT_STATUS_JOIN_FAILURE,                  /**< Join failure */
    CY_SUPPLICANT_STATUS_LEAVE_SUCCESS,                 /**< Leave successful */
    CY_SUPPLICANT_STATUS_LEAVE_FAILURE,                 /**< Leave failure */
    CY_SUPPLICANT_STATUS_LEFT_ALREADY,                  /**< Left already */
    CY_SUPPLICANT_STATUS_SUPPLICANT_INIT_FAILURE,       /**< Supplicant init failure */
    CY_SUPPLICANT_STATUS_PRE_PHASE1_FAILURE,            /**< Pre-phase1 failure */
    CY_SUPPLICANT_STATUS_TLS_FAILURE,                   /**< TLS failure */
    CY_SUPPLICANT_STATUS_PHASE2_FAILURE,                /**< Phase2 failure */
    CY_SUPPLICANT_STATUS_EAP_FAILURE,                   /**< EAP failure */
    CY_SUPPLICANT_STATUS_TIMEOUT,                       /**< Timeout */
    CY_SUPPLICANT_STATUS_FAIL,                          /**< Status fail */
    CY_SUPPLICANT_STATUS_PASS,                          /**< Status pass */
    CY_SUPPLICANT_STATUS_UP,                            /**< Status up */
    CY_SUPPLICANT_STATUS_DOWN,                          /**< status down */
    CY_SUPPLICANT_STATUS_INVALID                        /**< Invalid value */
}cy_supplicant_status_t;

/**
 * @}
 */

/**
 * @addtogroup enterprise_security_struct
 *  @{
 */

/** Tunnel authentication type :  Represents inner authentication protocol used
 *
 * @brief
 *
 * Lets say for example
 *
 * <b> MSCHAPv2 : <em> CY_SUPPLICANT_TUNNEL_TYPE_MSCHAPV2 </em> </b>
 *
 * <b> EAP      : <em> CY_SUPPLICANT_TUNNEL_TYPE_EAP </em> </b>
 * 
 */
typedef supplicant_tunnel_auth_type_t cy_supplicant_tunnel_auth_type_t;

/** EAP type : Represents Eap authentication type to use
 *
 * @brief
 * Lets say for example
 *
 * <b> EAP-TLS  : <em> CY_SUPPLICANT_EAP_TYPE_TLS </em> </b>
 *
 * <b> PEAP     : <em> CY_SUPPLICANT_EAP_TYPE_PEAP </em> </b>
 *
 * <b> EAP-TTLS : <em> CY_SUPPLICANT_EAP_TYPE_TTLS </em> </b>
 *
 */
typedef eap_type_t cy_supplicant_eap_type_t;

/**
 * @}
 */

/** TLS security parameters
 *
 * @brief supplicant TLS security parameters
 * Root certificate and its length
 * Client certificate and its length
 * Client private key and its length
 */
typedef struct cy_supplicant_security_s
{
    char*      ca_cert;     /**<  CA certificate */
    uint32_t   ca_cert_len; /**<  CA certificate length */
    char*      cert;        /**<  Client certificate in PEM format */
    uint32_t   cert_len;    /**<  Client certificate length */
    char*      key;         /**<  Client private key */
    uint32_t   key_len;     /**<  Client private key length */
} cy_supplicant_security_t;

/** Inner identity credentials
 *
 * @brief supplicant inner identity credentials to used
 *
 * username and its length
 * password and its length
 */
typedef struct cy_supplicant_inner_identity_s
{
    char                    identity[ CY_SUPPLICANT_MAX_IDENTITY_LENGTH ];  /**<  Username */
    uint8_t                 identity_length;                                /**<  Username length */
    /* in Windows password is UNICODE */
    char                    password[ CY_SUPPLICANT_MAX_PASSWORD_LENGTH ];    /**<  Password */
    uint8_t                 password_length;                                /**<  Password length */
}cy_supplicant_inner_identity_t;

/** Phase2 PEAP configuration structure
 *
 * @brief supplicant phase2 PEAP configuration structure
 *
 */
typedef struct cy_supplicant_peap_s
{
    cy_supplicant_inner_identity_t      inner_identity;  /**<  Inner identity credentials like username and password */

} cy_supplicant_peap_t;

/** Phase2 EAP-TTLS configuration structure
 *
 * @brief supplicant phase2 EAP TTLS configuration structure
 *
 */
typedef struct cy_supplicant_eap_ttls_s
{
    cy_supplicant_eap_type_t            inner_eap_type;           /**<  Inner EAP type */
    cy_supplicant_inner_identity_t      inner_identity;           /**<  Inner identity credentials like username and password */
    uint8_t                             is_client_cert_required;  /**<  Used to mandate client authentication */
} cy_supplicant_eap_ttls_t;


/** Phase2 configuration structure
 *
 * @brief supplicant phase2 configuration structure
 *
 */
typedef struct cy_supplicant_phase2_config_s
{
    cy_supplicant_tunnel_auth_type_t tunnel_auth_type; /**<  Inner authentication protocol used (phase2) */
    union
    {
        cy_supplicant_peap_t peap;          /**< Inner identity information for PEAP */
        cy_supplicant_eap_ttls_t eap_ttls;  /**< Inner identity information for EAP_TTLS */
    } tunnel_protocol;                      /**< Tunnel protocol */
} cy_supplicant_phase2_config_t;

/** Supplicant workspace information.
 *
 * @brief supplicant internal core context
 * supplicant workspace information.
 * @note for internal use
 */
typedef struct cy_supplicant_core_s
{
    supplicant_workspace_t*           supplicant_workspace;  /**<  Supplicant workspace context */
}cy_supplicant_core_t;

/** Supplicant instance
 *
 * @brief supplicant instance
 * @note For internal use
 */
typedef struct cy_supplicant_instance_s
{
    char                                ssid[ 64 ];                                             /**<  WIFI SSID */
    cy_supplicant_eap_type_t            eap_type;                                               /**<  Authentication mechanism want to use */
    cy_supplicant_security_t            tls_security;                                           /**<  TLS security parameters like CA certificate, client certificate and client private key to be used */
    cy_supplicant_core_security_t       auth_type;                                              /**<  Security auth type used */
    char                                outer_eap_identity[CY_SUPPLICANT_MAX_IDENTITY_LENGTH];  /**<  Outer EAP identity */
    uint8_t                             outer_eap_identity_length;                              /**<  Outer EAP identity length  */
    cy_supplicant_phase2_config_t       phase2_config;                                          /**<  Phase2 configuration parmaters to be filled. Used only for EAP_TTLS & PEAP */

    /* For internal management */
    cy_tls_context_t*                   tls_context;                                            /**<  TLS context */
    cy_supplicant_core_t                supplicant_core;                                        /**<  Supplicant core (for book keeping) */
    cy_tls_identity_t*                  tls_identity;                                           /**<  Identity for the secure connection */
    whd_interface_t                     interface;                                              /**<  WHD interface */
    cy_tls_session_t                    saved_session;                                          /**<  Used during session resumption case */
} cy_supplicant_instance_t;

/******************************************************
 *               Function Declarations
 ******************************************************/

/** Perform enterprise security init
 *
 * @param[in] supplicant_instance  : pointer to Supplicant instance should be passed by user.
 *
 * @return  cy_supplicant_status_t : init status @ref cy_supplicant_status_t
 */
cy_supplicant_status_t cy_join_ent_init( cy_supplicant_instance_t *supplicant_instance );

/** Perform enterprise security deinit
 *
 * @param[in] supplicant_instance  : pointer to Supplicant instance should be passed by user.
 *
 * @return  cy_supplicant_status_t : deinit status @ref cy_supplicant_status_t
 */
cy_supplicant_status_t cy_join_ent_deinit( cy_supplicant_instance_t *supplicant_instance );

/** Perform WiFi join to an enterprise network
 *
 * @param[in] supplicant_instance  : pointer to Supplicant instance should be passed by user.
 *
 * @return  cy_supplicant_status_t : join status @ref cy_supplicant_status_t
 */
cy_supplicant_status_t cy_join_ent( cy_supplicant_instance_t *supplicant_instance );

/** Perform WiFi leave from an enterprise network
 *
 * @param[in] supplicant_instance  : pointer to an Supplicant instance should be passed by user.
 *
 * @return  cy_supplicant_status_t : join status @ref cy_supplicant_status_t
 */
cy_supplicant_status_t cy_leave_ent( cy_supplicant_instance_t *supplicant_instance );

#ifdef __cplusplus
} /*extern "C" */
#endif
