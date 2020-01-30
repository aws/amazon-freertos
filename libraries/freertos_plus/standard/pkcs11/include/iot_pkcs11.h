/*
 * Amazon FreeRTOS PKCS #11 V1.0.2
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef _AWS_PKCS11_H_
#define _AWS_PKCS11_H_

#include <stdint.h>

#ifdef _WIN32
    #pragma pack(push, cryptoki, 1)
#endif

/**
 * @brief Amazon FreeRTOS PKCS#11 Interface.
 * The following definitions are required by the PKCS#11 standard public
 * headers.
 */

#define CK_PTR          *

#ifndef NULL_PTR
    #define NULL_PTR    0
#endif

/* CK_DEFINE_FUNCTION is deprecated.  Implementations should use CK_DECLARE_FUNCTION
 * instead when possible. */
#define CK_DEFINE_FUNCTION( returnType, name )             returnType name
#define CK_DECLARE_FUNCTION( returnType, name )            returnType name
#define CK_DECLARE_FUNCTION_POINTER( returnType, name )    returnType( CK_PTR name )
#define CK_CALLBACK_FUNCTION( returnType, name )           returnType( CK_PTR name )

/**
 *   @brief Length of a SHA256 digest, in bytes.
 */
#define pkcs11SHA256_DIGEST_LENGTH           32

/**
 * @brief Length of a curve P-256 ECDSA signature, in bytes.
 * PKCS #11 EC signatures are represented as a 32-bit R followed
 * by a 32-bit S value, and not ASN.1 encoded.
 */
#define pkcs11ECDSA_P256_SIGNATURE_LENGTH    64

/**
 * @brief Key strength for elliptic-curve P-256.
 */
#define pkcs11ECDSA_P256_KEY_BITS            256

/**
 * @brief Public exponent for RSA.
 */
#define pkcs11RSA_PUBLIC_EXPONENT            { 0x01, 0x00, 0x01 }

/**
 * @brief The number of bits in the RSA-2048 modulus.
 *
 */
#define pkcs11RSA_2048_MODULUS_BITS          2048

/**
 * @brief Length of PKCS #11 signature for RSA 2048 key, in bytes.
 */
#define pkcs11RSA_2048_SIGNATURE_LENGTH      ( pkcs11RSA_2048_MODULUS_BITS / 8 )

/**
 * @brief Length of RSA signature data before padding.
 *
 * This is calculated by adding the SHA-256 hash len (32) to the 19 bytes in
 * pkcs11STUFF_APPENDED_TO_RSA_SIG = 51 bytes total.
 */
#define pkcs11RSA_SIGNATURE_INPUT_LENGTH     51

/**
 * @brief Elliptic-curve object identifiers.
 * From https://tools.ietf.org/html/rfc6637#section-11.
 */
#define pkcs11ELLIPTIC_CURVE_NISTP256        "1.2.840.10045.3.1.7"

/**
 * @brief Maximum length of storage for PKCS #11 label, in bytes.
 */
#define pkcs11MAX_LABEL_LENGTH               32 /* 31 characters + 1 null terminator. */

/**
 * @brief OID for curve P-256.
 */
#define pkcs11DER_ENCODED_OID_P256           { 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 }


/**
 * @brief Set to 1 if importing private keys is supported.
 *
 * If private key import is not supported, this value should be defined 0 in aws_pkcs11_config.h
 */
#ifndef pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED
    #define pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED    1
#endif

/**
 * @brief RSA signature padding for interoperability between providing hashed messages
 * and providing hashed messages encoded with the digest information.
 *
 * The TLS connection for mbedTLS expects a hashed, but unpadded input,
 * and it appended message digest algorithm encoding.  However, the PKCS #11 sign
 * function either wants unhashed data which it will both hash and pad OR
 * as done in this workaround, we provide hashed data with padding appended.
 *
 * DigestInfo :: = SEQUENCE{
 *      digestAlgorithm DigestAlgorithmIdentifier,
 *      digest Digest }
 *
 * DigestAlgorithmIdentifier :: = AlgorithmIdentifier
 * Digest :: = OCTET STRING
 *
 * This is the DigestInfo sequence, digest algorithm, and the octet string/length
 * for the digest, without the actual digest itself.
 */
#define pkcs11STUFF_APPENDED_TO_RSA_SIG    { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 }

/* Bring in the public header. */

/* Undefine the macro for Keil Compiler to avoid conflict */
#if defined( __PASTE ) && defined( __CC_ARM )
    /* ARM RCVT stdint.h has a duplicate definition with PKCS #11. */
    #undef __PASTE
#endif

#include "pkcs11.h"

/* Certificate Template */
/* The object class must be the first attribute in the array. */
typedef struct PKCS11_CertificateTemplate
{
    CK_ATTRIBUTE xObjectClass;     /* CKA_CLASS, set to CKO_CERTIFICATE. */
    CK_ATTRIBUTE xSubject;         /* CKA_SUBJECT, this parameter is required by the PKCS #11 standard. */
    CK_ATTRIBUTE xCertificateType; /* CKA_CERTIFICATE_TYPE, set to CKC_X_509. */
    CK_ATTRIBUTE xValue;           /* CKA_VALUE, the DER byte array of the certificate contents. */
    CK_ATTRIBUTE xLabel;           /* CKA_LABEL. */
    CK_ATTRIBUTE xTokenObject;     /* CKA_TOKEN. */
} PKCS11_CertificateTemplate_t, * PKCS11_CertificateTemplatePtr_t;

/**
 * @brief Initializes a PKCS #11 session.
 *
 * @return CKR_OK if successful.
 * Else, see <a href="https://tiny.amazon.com/wtscrttv">PKCS #11 specification</a>
 * for more information.
 */
CK_RV xInitializePKCS11( void );


/**
 * @brief Get a list of available PKCS #11 slots.
 *
 * \note This function allocates memory for slots.
 * Freeing this memory is the responsibility of the caller.
 *
 *	\param[out] ppxSlotId       Pointer to slot list.  This slot list is
 *                              malloc'ed by the function and must be
 *                              freed by the caller.
 *  \param[out] pxSlotCount     Pointer to the number of slots found.
 *
 *  \return CKR_OK or PKCS #11 error code. (PKCS #11 error codes are positive).
 *
 */
CK_RV xGetSlotList( CK_SLOT_ID ** ppxSlotId,
                    CK_ULONG * pxSlotCount );

/**
 * \brief Initializes the PKCS #11 module and opens a session.
 *
 * \param[out]     pxSession   Pointer to the PKCS #11 session handle
 *                             that is created by this function.
 *
 * \return  CKR_OK upon success.  PKCS #11 error code on failure.
 *          Note that PKCS #11 error codes are positive.
 */
CK_RV xInitializePkcs11Session( CK_SESSION_HANDLE * pxSession );

/**
 *  \brief Initializes a PKCS #11 module and token.
 *
 *  \return CKR_OK upon success.  PKCS #11 error code on failure.
 *          Note that PKCS #11 error codes are positive.
 */
CK_RV xInitializePkcs11Token( void );

/**
 * \brief Searches for an object with a matching label and class provided.
 *
 *   \param[in]  xSession       An open PKCS #11 session.
 *   \param[in]  pcLabelName    A pointer to the object's label (CKA_LABEL).
 *   \param[in]  xClass         The class (CKA_CLASS) of the object.
 *                              ex: CKO_PUBLIC_KEY, CKO_PRIVATE_KEY, CKO_CERTIFICATE
 *   \param[out] pxHandle       Pointer to the location where the handle of
 *                              the found object should be placed.
 *
 * \note If no matching object is found, pxHandle will point
 * to an object with handle 0 (Invalid Object Handle).
 *
 * \note This function assumes that there is only one
 * object that meets the CLASS/LABEL criteria.
 */
CK_RV xFindObjectWithLabelAndClass( CK_SESSION_HANDLE xSession,
                                    const char * pcLabelName,
                                    CK_OBJECT_CLASS xClass,
                                    CK_OBJECT_HANDLE_PTR pxHandle );

/**
 * \brief Appends digest algorithm sequence to SHA-256 hash for RSA signatures
 *
 * This function pre-appends the digest algorithm identifier to the SHA-256
 * hash of a message.
 *
 * DigestInfo :: = SEQUENCE{
 *      digestAlgorithm DigestAlgorithmIdentifier,
 *      digest Digest }
 *
 * \param[in] pc32ByteHashedMessage     A 32-byte buffer containing the SHA-256
 *                                      hash of the data to be signed.
 * \param[out] pc51ByteHashOidBuffer    A 51-byte output buffer containing the
 *                                      DigestInfo structure.  This memory
 *                                      must be allocated by the caller.
 *
 * \return CKR_OK if successful, CKR_ARGUMENTS_BAD if NULL pointer passed in.
 *
 */
CK_RV vAppendSHA256AlgorithmIdentifierSequence( uint8_t * x32ByteHashedMessage,
                                                uint8_t * x51ByteHashOidBuffer );

#ifdef _WIN32
    #pragma pack(pop, cryptoki)
#endif

#endif /* ifndef _AWS_PKCS11_H_ */
