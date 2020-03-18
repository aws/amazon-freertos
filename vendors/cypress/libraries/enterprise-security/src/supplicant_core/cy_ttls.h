/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef MW_ENTERPRISE_SECURITY_SRC_SUPPLICANT_CORE_CY_TTLS_H_
#define MW_ENTERPRISE_SECURITY_SRC_SUPPLICANT_CORE_CY_TTLS_H_

#include "cy_type_defs.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_structures.h"
#include "cy_supplicant_host.h"
#include "cy_eap.h"
#include "cy_tls_abstraction.h"
#include "cy_mschapv2.h"
#include "cy_supplicant_process_et.h"

/******************************************************
 *              Packed Structures
 ******************************************************/
#pragma pack(1)

typedef struct
{
    uint32_t avp_code;
    uint8_t flags;
    uint8_t avp_length[3];
} avp_header_t;

typedef struct
{
    uint8_t Version;
    uint8_t reserved;
    uint8_t count;
} leap_header;

typedef struct
{
    avp_header_t avp_header;
    uint8_t data[1];
} avp_packet_t;

#pragma pack()

/******************************************************
 *              Function Prototypes
 ******************************************************/
cy_rslt_t supplicant_process_ttls_phase2_event(supplicant_workspace_t* workspace, supplicant_packet_t packet);
void      supplicant_send_ttls_response_packet( supplicant_packet_t* packet, supplicant_workspace_t* workspace );
supplicant_packet_t supplicant_create_ttls_response_packet( supplicant_packet_t* packet, eap_type_t eap_type, uint16_t data_length, uint8_t length_field_overhead, supplicant_workspace_t* workspace );
cy_rslt_t supplicant_init_ttls_phase2_handshake(supplicant_workspace_t* workspace);

#endif /* MW_ENTERPRISE_SECURITY_SRC_SUPPLICANT_CORE_CY_TTLS_H_ */
