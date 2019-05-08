/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/**
 * @brief Amazon FreeRTOS PKCS#11 Interface.
 * The following definitions are required by the PKCS#11 standard public
 * headers.
 */

#define CK_PTR          *

#ifndef NULL_PTR
    #define NULL_PTR    0
#endif

#define CK_DEFINE_FUNCTION( returnType, name )             returnType name
#define CK_DECLARE_FUNCTION( returnType, name )            returnType name
#define CK_DECLARE_FUNCTION_POINTER( returnType, name )    returnType( CK_PTR name )
#define CK_CALLBACK_FUNCTION( returnType, name )           returnType( CK_PTR name )

/**
 *   @brief Length of a SHA256 digest, in bytes.
 */
#define pcks11SHA256_DIGEST_LENGTH           32

/**
 * @brief Length of a curve P-256 ECDSA signature, in bytes.
 */
#define pkcs11ECDSA_P256_SIGNATURE_LENGTH    64

/**
 * @brief Length of PKCS #11 signature for RSA 2048 key, in bytes.
 */
#define pkcs11RSA_2048_SIGNATURE_LENGTH      ( 2048 / 8 )

#define pkcs11RSA_SIGNATURE_INPUT_LENGTH     51


/**
 * @brief Elliptic-curve object identifiers.
 * From https://tools.ietf.org/html/rfc6637#section-11.
 */
#define pkcs11ELLIPTIC_CURVE_NISTP256    "1.2.840.10045.3.1.7"

/* Bring in the public header. */

/* Undefine the macro for Keil Compiler to avoid conflict */
#if defined( __PASTE ) && defined( __CC_ARM )
    /* ARM RCVT stdint.h has a duplicate definition with PKCS #11. */
    #undef __PASTE
#endif

#include "pkcs11.h"

/* Key Template */
/* The object class must be the first attribute in the array. */
typedef struct PKCS11_KeyTemplate
{
    CK_ATTRIBUTE xObjectClass;
    CK_ATTRIBUTE xKeyType;
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xValue;
} PKCS11_KeyTemplate_t, * PKCS11_KeyTemplatePtr_t;

/* Elliptic Curve Private Key Template
 * The object class must be the first attribute in the array.
 * https://www.cryptsoft.com/pkcs11doc/v220/group__SEC__12__3__4__ELLIPTIC__CURVE__PRIVATE__KEY__OBJECTS.html */
typedef struct PKCS11_PrivateEcKeyTemplate
{
    CK_ATTRIBUTE xObjectClass; /* CKA_CLASS, set to CKO_PRIVATE_KEY. */
    CK_ATTRIBUTE xKeyType;     /* CKA_KEY_TYPE, set to CKK_EC. */
    CK_ATTRIBUTE xLabel;       /* CKA_LABEL. */
    CK_ATTRIBUTE xEcParams;    /* CKA_EC_PARAMS, the DER encoded OID for the curve. */
    CK_ATTRIBUTE xValue;       /* CKA_VALUE, the value d, the private value. */
    CK_ATTRIBUTE xTokenObject; /* CKA_TOKEN.  If true, the value should be persistent. */
} PKCS11_PrivateEcKeyTemplate_t, * PKCS11_PrivateEcKeyTemplatePtr_t;

/* RSA Private Key Template
 * The object class must be the first attribute in the array.
 * https://www.cryptsoft.com/pkcs11doc/v220/group__SEC__12__1__3__RSA__PRIVATE__KEY__OBJECTS.html */
typedef struct PKCS11_PrivateRsaKeyTemplate
{
    CK_ATTRIBUTE xObjectClass;     /* CKA_CLASS, set to CKO_PRIVATE_KEY. */
    CK_ATTRIBUTE xKeyType;         /* CKA_KEY_TYPE, set to CKK_RSA. */
    CK_ATTRIBUTE xLabel;           /* CKA_LABEL. */
    CK_ATTRIBUTE xCanSign;         /* CKA_SIGN. */
    CK_ATTRIBUTE xModulus;         /* CKA_MODULUS, the modulus N */
    CK_ATTRIBUTE xPrivateExponent; /* CKA_PRIVATE_EXPONENT, the private exponent D */
    CK_ATTRIBUTE xPublicExponent;  /* CKA_PUBLIC_EXPONENT, the public exponent E */
    CK_ATTRIBUTE xPrime1;          /* CKA_PRIME_1, P */
    CK_ATTRIBUTE xPrime2;          /* CKA_PRIME_2, Q */
    CK_ATTRIBUTE xTokenObject;     /* CKA_TOKEN, if true, the value should be persistent. */
} PKCS11_PrivateRsaKeyTemplate_t, * PKCS11_PrivateRsaKeyTemplatePtr_t;

/* Certificate Template */
/* The object class must be the first attribute in the array. */
typedef struct PKCS11_CertificateTemplate
{
    CK_ATTRIBUTE xObjectClass;     /* required certificate attribute. */
    CK_ATTRIBUTE xSubject;         /* required certificate attribute. */
    CK_ATTRIBUTE xCertificateType; /* required certificate attribute. */
    CK_ATTRIBUTE xValue;           /* required certificate attribute. */
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xTokenObject;
} PKCS11_CertificateTemplate_t, * PKCS11_CertificateTemplatePtr_t;


typedef struct PKCS11_GenerateKeyPublicTemplate
{
    CK_ATTRIBUTE xKeyType;
    CK_ATTRIBUTE xEcParams;
    CK_ATTRIBUTE xLabel;
    CK_ATTRIBUTE xValue;
} PKCS11_GenerateKeyPublicTemplate_t, * PKCS11_GenerateKeyPublicTemplatePtr_t;

typedef struct PKCS11_GenerateKeyPrivateTemplate
{
    CK_ATTRIBUTE xLabel;
} PKCS11_GenerateKeyPrivateTemplate_t, * PKCS11_GenerateKeyPrivateTemplatePtr_t;


#define pkcs11INVALID_OBJECT_HANDLE        0
#define pkcs11INVALID_MECHANISM            0xFFFFFFFF

#define pkcs11MAX_LABEL_LENGTH             32 /* 31 characters + 1 null terminator. */

#define pkcs11DER_ENCODED_OID_P256         { 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 }

/*
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

CK_RV xInitializePKCS11( void );

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
                                    const uint8_t * pcLabelName,
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
 */
void vAppendSHA256AlgorithmIdentifierSequence( uint8_t * x32ByteHashedMessage,
                                               uint8_t * x51ByteHashOidBuffer );


#endif /* ifndef _AWS_PKCS11_H_ */
