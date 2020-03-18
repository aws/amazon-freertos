/*
 * $ Copyright Cypress Semiconductor $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ssl.h"
#include "ssl_internal.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "cipher.h"
#include "md4.h"
#include "sha1.h"
#include "des.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Typedefs
 ******************************************************/

/* IMPLEMENTATION NOTE: Core supplicant implementation should not access any of the structure members defined in this file */
typedef struct mbedtls_ssl_context cy_tls_workspace_t;
typedef struct mbedtls_ssl_session cy_tls_session_t;
typedef struct mbedtls_x509_crt cy_x509_crt_t;
typedef struct mbedtls_pk_context cy_pk_context_t;
typedef struct mbedtls_entropy_context cy_entropy_context_t;
typedef struct mbedtls_ctr_drbg_context cy_ctr_drbg_context_t;
typedef struct mbedtls_ssl_config cy_ssl_config_t;

typedef struct
{
    cy_pk_context_t private_key;
    cy_x509_crt_t certificate;
    uint8_t is_client_auth;
} cy_tls_identity_t;

#ifdef __cplusplus
} /*extern "C" */
#endif
