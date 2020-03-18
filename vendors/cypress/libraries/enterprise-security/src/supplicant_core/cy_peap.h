/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_type_defs.h"
#include "cy_tls_abstraction.h"
#include "cy_mschapv2.h"
#include "cy_supplicant_process_et.h"
#include "cy_supplicant_core_constants.h"
#include "cy_supplicant_structures.h"
#include "cy_supplicant_host.h"
#include "cy_eap.h"

/******************************************************
 *                 Typedef Structures
 ******************************************************/
typedef struct
{
    uint8_t  type;
} peap_header_t;

typedef struct
{
    uint8_t  type;
    uint8_t  data[1];
} peap_packet_t;

typedef struct
{
    eap_header_t    header;
    avp_request_t   avp[1];
}peap_extention_request_t;

typedef struct
{
    eap_header_t   header;
    avp_result_t   avp[1];
}peap_extention_response_t;


/******************************************************
 *               Function prototypes
 ******************************************************/
extern cy_rslt_t    supplicant_inner_packet_set_data        ( whd_driver_t whd_driver,supplicant_packet_t* packet, int32_t size );
supplicant_packet_t supplicant_create_peap_response_packet  ( supplicant_packet_t* packet, eap_type_t eap_type, uint16_t data_length, uint8_t length_field_overhead, supplicant_workspace_t* workspace );
void                supplicant_send_peap_response_packet    ( supplicant_packet_t* packet, supplicant_workspace_t* workspace );

#ifdef __cplusplus
} /*extern "C" */
#endif
