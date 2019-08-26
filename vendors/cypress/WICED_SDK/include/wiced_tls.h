/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#pragma once

#ifndef WICED_DISABLE_TLS
#include "tls_types.h"
#endif
#include "wiced_result.h"

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
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/**
 * @addtogroup tls       TLS Security
 *  @ingroup ipcoms
 *
 * Security initialisation functions for TLS enabled connections (Transport Layer Security - successor to SSL Secure Sockets Layer )
 *
 * These APIs are common to both TLS client and server entities. By default, the library supports TLS v1.2 and v1.1.
 *
 * The following snippet/pseudo-code demonstrates a sample API call flow for a secure TLS client:
 *
 * 1. wiced_tcp_create_socket ( )  (Create TCP socket)
 *
 * 2. wiced_init_root_ca_certificates ( ) (Optional API. Needed only if RootCA certificates are to be loaded and verified against the peer server certificate)
 *
 * 3. wiced_tls_init_identity ( ) (Optional API. Needed only if client application desires to load its own certificate & private key)
 *
 * 4. wiced_tls_init_context ( ) (This API initializes a TLS context. Context has all the bookkeeping information to create and manage a secure connection)
 *
 * 5. wiced_tls_set_extensions ( ) (Optional API. Needed only if client desires to use TLS extension such as Maximum Fragment Length, Server Name Indication etc.)
 *
 * 6. wiced_tcp_enable_tls ( ) (Enable TLS over the normal TCP socket that was created in the first step)
 *
 * 7. wiced_tcp_connect ( ) (Initiate a TCP connection to the remote TCP/TLS server)
 *
 * The following snippet/pseudo-code demonstrates a sample API call flow for a secure TLS server:
 *
 * 1. wiced_tcp_create_socket ( )  (Create TCP socket)
 *
 * 2. wiced_init_root_ca_certificates ( ) (Optional API. Needed only if RootCA certificates are to be loaded and verified against the peer client certificate)
 *
 * 3. wiced_tls_init_identity ( ) (Optional API. Needed only if server application desires to load its own certificate & private key)
 *
 * 4. wiced_tls_init_context ( ) (This API initializes a TLS context. Context has all the bookkeeping information to create and manage a secure connection)
 *
 * 5. wiced_tcp_enable_tls ( ) (Enable TLS over the normal TCP socket that was created in the first step)
 *
 * 6. wiced_tcp_server_accept ( ) (Accept incoming TCP connection from remote TCP/TLS client)
 *
 * The client and server applications may optionally store the certificate and private keys via the WICED DCT
 * using the Makefile MACROs CERTIFICATE and PRIVATE_KEY.
 *
 * To store the certificate, the application MUST place the certificate in the 'resources' folder of the SDK,
 * define the following MACRO 'CERTIFICATE' in the application makefile and assign the path to the certificate.
 * For instance, CERTIFICATE := $(SOURCE_ROOT)resources/certificates/wiced_demo_server_cert.cer
 *
 * Similarly, to store the private key, the application MUST place the key in the 'resources' folder of the SDK,
 * define the following MACRO 'PRIVATE_KEY' in the application makefile and assign the path to the key.
 * For instance, PRIVATE_KEY := $(SOURCE_ROOT)resources/certificates/wiced_demo_server_cert_key.key
 *
 *  @{
 */
/*****************************************************************************/
#ifndef WICED_DISABLE_TLS
/** Initialises a simple TLS context handle
 *
 * @param[out] context  : A pointer to a wiced_tls_context_t context object that will be initialised.
 *                                  The context object is analogous to a cookie which has all the information to process a TLS message.
 *                                  This is the entity that has all the bookkeeping information (TLS handshake state, TLS session etc.).
 * @param[in]  identity : A pointer to a wiced_tls_identity_t object initialised with @ref wiced_tls_init_identity.
 * @param[in]  peer_cn  : Expected peer CommonName (or NULL)
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_init_context( wiced_tls_context_t* context, wiced_tls_identity_t* identity, const char* peer_cn );

/** Initialise the trusted root CA certificates for the TLS context.
 *
 * This API is optional and only needed if the client desires to the collection of trusted root CA certificates
 * to verify the received certificate for this connection context.
 *
 * @param[in,out] context             : A pointer to a wiced_tls_context_t context object
 * @param[in] trusted_ca_certificates : A chain of x509 certificates in PEM or DER format.
 *                                        This chain of certificates comprise the public keys of the signing authorities.
 *                                        During the handshake, these public keys are used to verify the authenticity of the peer
 * @param[in] length                  : Certificate length
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_set_context_root_ca_certificates( wiced_tls_context_t* context, const char* trusted_ca_certificates, const uint32_t length );

/** Set TLS extension.
 *
 * This API is optional and only needed if the client desires to use one of the TLS extensions
 * such as Maximum Fragment Length, Server Name Indication, Application Layer Protocol Negotiation etc.
 *
 * @param[in,out] context   : A pointer to a wiced_tls_context_t context object
 * @param[in] extension     : A pointer to a wiced_tls_extension_t extension
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_set_extension(wiced_tls_context_t* context, wiced_tls_extension_t* extension);

/** Initializes a TLS identity using a supplied certificate and private key
 *
 * @param[out] identity           : A pointer to a wiced_tls_identity_t object that will be initialized
 *                                  The identity is a data structure that encompasses the device's own certificate/key.
 * @param[in]  private_key        : The server private key in binary format. This key is used to sign the handshake message
 * @param[in]  key_length         : Private key length
 * @param[in]  certificate_data   : The server x509 certificate in PEM or DER format
 * @param[in]  certificate_length : The length of the certificate
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_init_identity( wiced_tls_identity_t* identity, const char* private_key, const uint32_t key_length, const uint8_t* certificate_data, uint32_t certificate_length );

/** Add pair of certificate and private key to identity.
 * NOTE: This API should be only used for server to configure multiple certificate & key ex. adding certificate and key of RSA and ECDSA type. If used for client, then only first pair of certificate
 * & key pair will be used.
 *
 * @param[in] identity            : A pointer to a wiced_tls_identity_t object.
 *                                  The identity is a data structure that encompasses the device's own certificate/key.
 * @param[out] credential         : Pointer to a credential info. This must be unique for each certificate-key pair and has to be alive as long as certificate/key is valid.
 * @param[in]  private_key        : The server private key in binary format. This key is used to sign the handshake message
 * @param[in]  key_length         : Private key length
 * @param[in]  certificate_data   : The server x509 certificate in PEM or DER format
 * @param[in]  certificate_length : The length of the certificate
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_add_identity( wiced_tls_identity_t* identity, wiced_tls_credentials_info_t* credential, const char* private_key, const uint32_t key_length, const uint8_t* certificate_data, uint32_t certificate_length );

/** Remove pre-added pair of certificate and key from identity. This API should be used only for server to remove pre-loaded certificate & key pair. The API should be only used for server.
 *
 * @param[in] identity           : A pointer to a wiced_tls_identity_t object.
 *                                 The identity is a data structure that encompasses the device's own certificate/key.
 * @param[in] credential         : Pointer to the credential which was used to invoke wiced_tls_add_identity() API while adding identity.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_remove_identity( wiced_tls_identity_t* identity, wiced_tls_credentials_info_t* credential );

/**
 *
 * @deprecated: This API is deprecated and will be discontinued. Use wiced_tls_set_extension() instead.
 *
 * Adds a TLS extension to the list of extensions sent in Client Hello message.
 *
 * @param[in] context     : A pointer to a wiced_tls context initialized with wiced_tls_init_context
 * @param[in] extension   : A pointer to an extension to be added in client hello message.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_add_extension( wiced_tls_context_t* context, const ssl_extension* extension );

/** DeiInitialises a TLS identity
 *
 * @param[in] identity    : A pointer to a wiced_tls_identity_t object that will be de-initialised
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_deinit_identity( wiced_tls_identity_t* tls_identity);

/** Initialise the trusted root CA certificates
 *
 *  Initialises the collection of trusted root CA certificates used to verify received certificates
 *
 * @param[in] trusted_ca_certificates : A chain of x509 certificates in PEM or DER format.
 *                                                   This chain of certificates comprise the public keys of the signing authorities.
 *                                                   During the handshake, these public keys are used to verify the authenticity of the peer
 * @param[in] length                  : Certificate length
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_init_root_ca_certificates( const char* trusted_ca_certificates, const uint32_t length );


/** De-initialise the trusted root CA certificates
 *
 *  De-initialises the collection of trusted root CA certificates used to verify received certificates
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_deinit_root_ca_certificates( void );


/** De-initialise a previously inited simple or advanced context
 *
 * @param[in,out] context : A pointer to either a wiced_tls_context_t object
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_tls_deinit_context( wiced_tls_context_t* context );

/** @} */
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
