/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_type_defs.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_structures.h"
#include "cy_supplicant_host.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define EAP_WRAPPER_DEBUG( x )  //printf x
#define EAP_WRAPPER_INFO( x )   //printf x

/******************************************************
 *                   Typedef structures
 ******************************************************/
#pragma pack(1)

typedef struct
{
    uint8_t ether_dhost[ETHERNET_ADDRESS_LENGTH];
    uint8_t ether_shost[ETHERNET_ADDRESS_LENGTH];
    uint16_t ether_type;
} ether_header_t;

typedef struct
{
    uint8_t version;
    uint8_t type;
    uint16_t length;
} eapol_header_t;

typedef struct
{
    ether_header_t ethernet;
    eapol_header_t eapol;
} eapol_packet_header_t;

typedef struct
{
    ether_header_t ethernet;
    eapol_header_t eapol;
    uint8_t data[1];
} eapol_packet_t;

typedef struct
{
    uint8_t code;
    uint8_t id;
    uint16_t length;
    uint8_t type;
} eap_header_t;

typedef struct
{
    ether_header_t ethernet;
    eapol_header_t eapol;
    eap_header_t eap;
    uint8_t data[1];
} eap_packet_t;

typedef struct
{
    uint8_t flags;
} eap_tls_header_t;

typedef struct
{
    ether_header_t ethernet;
    eapol_header_t eapol;
    eap_header_t eap;
    eap_tls_header_t eap_tls;
    uint8_t data[1]; /* Data starts with a length of TLS data field or TLS data depending on the flags field */
} eap_tls_packet_t;

typedef struct
{
    uint16_t type;
    uint16_t length;
    uint8_t value[1];
} avp_request_t;

typedef struct
{
    uint16_t type;
    uint16_t length;
    uint16_t status;
} avp_result_t;

typedef struct
{
    uint8_t type;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t length;
    uint8_t message[1];
} tls_record_t;

/* Helper structure to create TLS record */
typedef struct
{
    uint8_t type;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t length;
} tls_record_header_t;

#pragma pack()

/******************************************************
 *               Function Prototypes
 ******************************************************/
cy_rslt_t supplicant_send_eapol_start               ( supplicant_workspace_t* workspace );
void      supplicant_send_eap_response_packet       ( supplicant_workspace_t* workspace, eap_type_t eap_type, uint8_t* data, uint16_t data_length );
cy_rslt_t supplicant_send_zero_length_eap_tls_packet( supplicant_workspace_t* workspace );
cy_rslt_t supplicant_send_eap_tls_fragment          ( supplicant_workspace_t* workspace, supplicant_packet_t packet );
void      supplicant_send_eapol_packet              ( supplicant_packet_t packet, supplicant_workspace_t* workspace, eapol_packet_type_t type, uint16_t content_size );

#ifdef __cplusplus
} /*extern "C" */
#endif
