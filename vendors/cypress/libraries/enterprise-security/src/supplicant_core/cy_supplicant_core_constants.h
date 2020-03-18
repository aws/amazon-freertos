/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "whd.h"
#include "whd_int.h"
#include "whd_wifi_api.h"
#include "whd_buffer_api.h"
#include "whd_wlioctl.h"
#include "whd_types.h"
#include "whd_types_int.h"
#include "whd_wlioctl.h"

/******************************************************
 *                      Macros & Inlines
 ******************************************************/

#define ALWAYS_INLINE_PRE      //_Pragma( "inline=forced" )
#define ALWAYS_INLINE


#ifndef htobe16   /* This is defined in POSIX platforms */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE uint16_t htobe16(uint16_t v)
{
    return (uint16_t)(((v&0x00FF) << 8) | ((v&0xFF00)>>8));
}
#endif /* ifndef htobe16 */

#ifndef htobe32   /* This is defined in POSIX platforms */
ALWAYS_INLINE_PRE static inline ALWAYS_INLINE uint32_t htobe32(uint32_t v)
{
    return (uint32_t)(((v&0x000000FF) << 24) | ((v&0x0000FF00) << 8) | ((v&0x00FF0000) >> 8) | ((v&0xFF000000) >> 24));
}
#endif /* ifndef htobe32 */

/******************************************************
 *                    Constants
 ******************************************************/

#define SIZEOF_RANDOM                (64)
#define SIZEOF_SESSION_MASTER        (48)
#define SIZEOF_MPPE_KEYS             (128)
#define PMK_LEN                      (32)

#define EAP_TLS_FLAG_LENGTH_INCLUDED (0x80)
#define EAP_TLS_FLAG_MORE_FRAGMENTS  (0x40)

#define LEAP_VERSION                 (1)
#define LEAP_CHALLENGE_LEN           (8)
#define LEAP_RESPONSE_LEN            (24)
#define LEAP_KEY_LEN                 (16)

#define AVP_CODE_EAP_MESSAGE         (79)
#define AVP_FLAG_MANDATORY_MASK      (0x40)
#define AVP_FLAG_VENDOR_MASK         (0x80)
#define AVP_LENGTH_SIZE              (3)

#define EAP_MTU_SIZE                 (1020)

#define ETHERNET_ADDRESS_LENGTH      (6)

/**
 * Semaphore wait time constants
 */
#define SUPPLICANT_NEVER_TIMEOUT      (0xFFFFFFFF)
#define SUPPLICANT_WAIT_FOREVER       (0xFFFFFFFF)
#define SUPPLICANT_TIMEOUT            (5000)
#define SUPPLICANT_NO_WAIT            (0)

/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * Enumeration of Wi-Fi security modes
 */
typedef whd_security_t cy_supplicant_core_security_t;

typedef enum
{
    TLS_AGENT_EVENT_EAPOL_PACKET,
    TLS_AGENT_EVENT_ABORT_REQUESTED,
} tls_agent_event_t;

/* High level states
 * INITIALISING ( scan, join )
 * EAP_HANDSHAKE ( go through EAP state machine )
 * WPS_HANDSHAKE ( go through WPS state machine )
 */
typedef enum
{
    SUPPLICANT_INITIALISING,
    SUPPLICANT_INITIALISED,
    SUPPLICANT_IN_EAP_METHOD_HANDSHAKE,
    SUPPLICANT_CLOSING_EAP,
} supplicant_main_stage_t;

typedef enum
{
    SUPPLICANT_EAP_START         = 0,    /* (EAP start ) */
    SUPPLICANT_EAP_IDENTITY      = 1,    /* (EAP identity request, EAP identity response) */
    SUPPLICANT_EAP_NAK           = 2,
    SUPPLICANT_EAP_METHOD        = 3,
} supplicant_state_machine_stage_t;

typedef enum
{
    SUPPLICANT_LEAP_IDENTITY              = SUPPLICANT_EAP_IDENTITY,
    SUPPLICANT_LEAP_RESPOND_CHALLENGE     = 2,
    SUPPLICANT_LEAP_REQUEST_CHALLENGE     = 3,
    SUPPLICANT_LEAP_DONE                  = 4,
} supplicant_leap_state_machine_t;

typedef enum
{
    EAP_CODE_REQUEST  = 1,
    EAP_CODE_RESPONSE = 2,
    EAP_CODE_SUCCESS  = 3,
    EAP_CODE_FAILURE  = 4
} eap_code_t;

/**
 * @addtogroup enterprise_security_enums
 *  @{
 */

/** Supplicant Tunnel Eap Types
 *
 * @brief
 * Various supplicant Tunnel EAP types
 */
typedef enum
{
    CY_SUPPLICANT_TUNNEL_TYPE_NONE        = 0 /**<  Invalid EAP type. */,
    CY_SUPPLICANT_TUNNEL_TYPE_EAP         = 1 /**<  EAP as tunnel EAP type. */,
    CY_SUPPLICANT_TUNNEL_TYPE_CHAP        = 2 /**<  CHAP as tunnel EAP type. */,
    CY_SUPPLICANT_TUNNEL_TYPE_MSCHAP      = 3 /**<  MSCHAP as tunnel EAP type. */,
    CY_SUPPLICANT_TUNNEL_TYPE_MSCHAPV2    = 4 /**<  MSCHAPv2 as tunnel EAP type. */,
    CY_SUPPLICANT_TUNNEL_TYPE_PAP         = 5 /**<  PAP as tunnel EAP type. */
}  supplicant_tunnel_auth_type_t;

/** Supplicant Eap Types
 *
 * @brief
 * Various supplicant EAP TYPES
 */
typedef enum
{
    CY_SUPPLICANT_EAP_TYPE_NONE         = 0   /**<  Invalid EAP type. */,
    CY_SUPPLICANT_EAP_TYPE_IDENTITY     = 1   /**<  IDENTITY type refer to RFC 3748. */,
    CY_SUPPLICANT_EAP_TYPE_NOTIFICATION = 2   /**<  NOTIFICATION type refer to RFC 3748. */,
    CY_SUPPLICANT_EAP_TYPE_NAK          = 3   /**<  Response only type refer to RFC 3748. */,
    CY_SUPPLICANT_EAP_TYPE_MD5          = 4,  /**<  EAP-MD5 type refer to RFC 3748. */
    CY_SUPPLICANT_EAP_TYPE_OTP          = 5   /**<  EAP-OTP type refer to RFC 3748. */,
    CY_SUPPLICANT_EAP_TYPE_GTC          = 6,  /**<  EAP-GTC type refer to RFC 3748. */
    CY_SUPPLICANT_EAP_TYPE_TLS          = 13  /**<  EAP-TLS type refer to RFC 2716. */,
    CY_SUPPLICANT_EAP_TYPE_LEAP         = 17  /**<  EAP-LEAP type and it is Cisco proprietary. */,
    CY_SUPPLICANT_EAP_TYPE_SIM          = 18  /**<  EAP-SIM type refer to draft-haverinen-pppext-eap-sim-12.txt. */,
    CY_SUPPLICANT_EAP_TYPE_TTLS         = 21  /**<  EAP-TTLS type refer to draft-ietf-pppext-eap-ttls-02.txt. */,
    CY_SUPPLICANT_EAP_TYPE_AKA          = 23  /**<  EAP-AKA type refer to draft-arkko-pppext-eap-aka-12.txt. */,
    CY_SUPPLICANT_EAP_TYPE_PEAP         = 25  /**<  PEAP type refer to draft-josefsson-pppext-eap-tls-eap-06.txt. */,
    CY_SUPPLICANT_EAP_TYPE_MSCHAPV2     = 26  /**<  MSCHAPv2 type refer to draft-kamath-pppext-eap-mschapv2-00.txt. */,
    CY_SUPPLICANT_EAP_TYPE_TLV          = 33  /**<  TLV type refer to draft-josefsson-pppext-eap-tls-eap-07.txt. */,
    CY_SUPPLICANT_EAP_TYPE_FAST         = 43  /**<  draft-cam-winget-eap-fast-00.txt. */,
    CY_SUPPLICANT_EAP_TYPE_PAX          = 46, /**<  draft-clancy-eap-pax-04.txt. */
    CY_SUPPLICANT_EAP_TYPE_EXPANDED_NAK = 253 /**<  RFC 3748. */,
    CY_SUPPLICANT_EAP_TYPE_WPS          = 254 /**<  Wireless Simple Config. */,
    CY_SUPPLICANT_EAP_TYPE_PSK          = 255 /**<  EXPERIMENTAL - type not yet allocated draft-bersani-eap-psk-09. */
}eap_type_t;
/**
 * @}
 */

/**
 * EAPOL types
 */
typedef enum
{
    EAP_PACKET                   = 0,
    EAPOL_START                  = 1,
    EAPOL_LOGOFF                 = 2,
    EAPOL_KEY                    = 3,
    EAPOL_ENCAPSULATED_ASF_ALERT = 4
} eapol_packet_type_t;

/*
 * MSCHAPV2 codes
 */
typedef enum
{
    MSCHAPV2_OPCODE_CHALLENGE       = 1,
    MSCHAPV2_OPCODE_RESPONSE        = 2,
    MSCHAPV2_OPCODE_SUCCESS         = 3,
    MSCHAPV2_OPCODE_FAILURE         = 4,
    MSCHAPV2_OPCODE_CHANGE_PASSWORD = 7,
} mschapv2_opcode_t;

#ifdef __cplusplus
} /*extern "C" */
#endif
