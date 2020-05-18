/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "cy_supplicant_result.h"
#include "cy_tls_abstraction.h"
#include "cy_type_defs.h"
#include "cy_supplicant_structures.h"
#include "cy_peap.h"
#include "cy_ttls.h"
#include "mbedtls/md4.h"
#include "mbedtls/des.h"

/******************************************************
 *                  Typedef structures
 ******************************************************/
#pragma pack(1)
typedef struct
{
    uint8_t     opcode;
    uint8_t     id;
    uint16_t    length;
}mschapv2_header_t;

typedef struct
{
    uint8_t     opcode;
    uint8_t     id;
    uint16_t    length;
    uint8_t     data[1];
}mschapv2_packet_t;

typedef struct
{
    uint8_t     opcode;
    uint8_t     id;
    uint16_t    length;
    uint8_t     value_size;
    uint8_t     challenge[16];
    uint8_t     name[1];
}mschapv2_challenge_packet_t;

typedef struct
{
    uint8_t     opcode;
    uint8_t     id;
    uint16_t    length;
    uint8_t     value_size;
    uint8_t     peer_challenge[16];
    uint8_t     reserved[8];
    uint8_t     nt_reponse[24];
    uint8_t     flags;
    uint8_t     name[1];
}mschapv2_response_packet_t;

typedef struct
{
    uint8_t     opcode;
    uint8_t     id;
    uint16_t    length;
    uint8_t     message[1];
}mschapv2_success_request_packet_t;

typedef struct
{
    uint8_t     opcode;
}mschapv2_success_response_packet_t;

typedef mschapv2_success_request_packet_t mschapv2_failure_request_packet_t;

typedef mschapv2_success_response_packet_t mschapv2_failure_response_packet_t;

#pragma pack()

/******************************************************
 *              Function Prototypes
 ******************************************************/
cy_rslt_t mschap_challenge_hash           ( uint8_t* peer_challenge, uint8_t* authenticator_challenge, char* user_name, uint8_t* challenge);
cy_rslt_t mschap_nt_password_hash         ( char* password, uint16_t length, uint8_t* password_hash );
cy_rslt_t mschap_permute_key              ( uint8_t* key56, uint8_t* key64);
cy_rslt_t mschap_des_encrypt              ( uint8_t* clear, uint8_t* key, uint8_t* cypher);
cy_rslt_t mschap_challenge_response       ( uint8_t* challenge, uint8_t* nt_password_hash, uint8_t* nt_response );
cy_rslt_t mschap_generate_nt_response     ( uint8_t* authenticator_challenge, uint8_t* peer_challenge,char* user_name, char* password, uint16_t password_length, uint8_t* nt_response);
cy_rslt_t mschap_process_packet           ( mschapv2_packet_t *packet, supplicant_workspace_t *workspace );

#ifdef __cplusplus
} /*extern "C" */
#endif
