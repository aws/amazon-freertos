/**
 * \file
 * \brief PKCS11 Library Certificate Handling
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_token.h"
#include "pkcs11_cert.h"
#include "pkcs11_util.h"

#include "cryptoauthlib.h"
#include "atcacert/atcacert_def.h"
#include "atcacert/atcacert_client.h"

/**
 * \defgroup pkcs11 Key (pkcs11_key_)
   @{ */

CK_RV pkcs11_cert_get_encoded(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (obj_ptr->data)
        {
            atcacert_def_t * cert_cfg = (atcacert_def_t*)obj_ptr->data;

            /* Load Certificate */
            if (pAttribute->pValue && pAttribute->ulValueLen)
            {
                uint8_t ca_key[64];
                ATCA_STATUS status  = ATCA_SUCCESS;

                if (cert_cfg->ca_cert_def)
                {
                    if (cert_cfg->ca_cert_def->public_key_dev_loc.is_genkey)
                    {
                        status = atcab_get_pubkey(cert_cfg->ca_cert_def->public_key_dev_loc.slot, ca_key);
                    }
                    else
                    {
                        status = atcab_read_pubkey(cert_cfg->ca_cert_def->public_key_dev_loc.slot, ca_key);
                    }
                }


                if (status)
                {
                    return CKR_DEVICE_ERROR;
                }

                size_t temp = pAttribute->ulValueLen;
                status = atcacert_read_cert(obj_ptr->data, cert_cfg->ca_cert_def ? ca_key : NULL, pAttribute->pValue, &temp);
                pAttribute->ulValueLen = (uint32_t)temp;

                if (ATCACERT_E_DECODING_ERROR == status)
                {
                    return CKR_DATA_INVALID;
                }
                else if (status)
                {
                    return CKR_DEVICE_ERROR;
                }
            }
            else
            {
                size_t cert_size;

                if (atcacert_read_cert_size(cert_cfg, &cert_size))
                {
                    return CKR_DEVICE_ERROR;
                }

                pAttribute->ulValueLen = (CK_ULONG)cert_size;
            }
            return CKR_OK;
        }
        else
        {
            return pkcs11_attrib_empty(NULL, pAttribute);
        }
    }

    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_cert_get_type(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (obj_ptr->data)
        {
            atcacert_def_t * cert_cfg = (atcacert_def_t*)obj_ptr->data;

            if (CERTTYPE_X509 == cert_cfg->type)
            {
                return pkcs11_attrib_value(pAttribute, CKC_X_509, sizeof(CK_CERTIFICATE_TYPE));
            }
            else
            {
                return pkcs11_attrib_value(pAttribute, CKC_VENDOR_DEFINED, sizeof(CK_CERTIFICATE_TYPE));
            }
        }
        else
        {
            return pkcs11_attrib_empty(NULL, pAttribute);
        }
    }

    return CKR_ARGUMENTS_BAD;
}

//CK_RV pkcs11_cert_get_subject(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
//{
//    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
//
//    if (obj_ptr)
//    {
//        if (obj_ptr->data)
//        {
//            atcacert_def_t * cert_cfg = (atcacert_def_t *)obj_ptr->data;
//
//            return pkcs11_attrib_fill(pAttribute, &cert_cfg->cert_template[100], 30);
//        }
//        else
//        {
//            return pkcs11_attrib_empty(NULL, pAttribute);
//        }
//    }
//
//    return CKR_ARGUMENTS_BAD;
//}

CK_RV pkcs11_cert_get_subject_key_id(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (obj_ptr->data)
        {
            if (pAttribute->pValue && pAttribute->ulValueLen)
            {
                atcacert_def_t * cert_cfg = (atcacert_def_t*)obj_ptr->data;
                uint8_t subj_key_id[20];
                ATCA_STATUS status;

                status = atcacert_read_subj_key_id(cert_cfg, subj_key_id);

                if (status)
                {
                    return CKR_DEVICE_ERROR;
                }

                return pkcs11_attrib_fill(pAttribute, subj_key_id, sizeof(subj_key_id));
            }
            else
            {
                pAttribute->ulValueLen = 20;
            }
        }
        else
        {
            return pkcs11_attrib_empty(NULL, pAttribute);
        }
    }

    return CKR_ARGUMENTS_BAD;
}

/**
 * CKO_CERTIFICATE (Type: CKC_X_509) - X509 Public Key Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_x509public_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                      pkcs11_object_get_class                                                                           },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                      pkcs11_attrib_true                                                                                },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,                    pkcs11_token_get_access_type                                                                      },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,                 pkcs11_token_get_writable                                                                         },
    /** Description of the object(default empty). */
    { CKA_LABEL,                      pkcs11_object_get_name                                                                            },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,                   pkcs11_attrib_false                                                                               },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,                pkcs11_object_get_destroyable                                                                     },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,           pkcs11_cert_get_type                                                                              },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,                    NULL_PTR                                                                                          },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY,       pkcs11_object_get_type                                                                            },
    /** Checksum */
    { CKA_CHECK_VALUE,                NULL_PTR                                                                                          },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,                 pkcs11_attrib_empty                                                                               },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,                   pkcs11_attrib_empty                                                                               },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,            pkcs11_attrib_empty                                                                               },
    /** DER-encoded Certificate subject name */
    { CKA_SUBJECT,                    pkcs11_attrib_empty                                                                               },
    /** Key identifier for public/private key pair (default empty) */
    { CKA_ID,                         pkcs11_attrib_empty                                                                               },
    /** DER-encoded Certificate issuer name (default empty)*/
    { CKA_ISSUER,                     pkcs11_attrib_empty                                                                               },
    /** DER-encoding of the certificate serial number (default empty) */
    { CKA_SERIAL_NUMBER,              pkcs11_attrib_empty                                                                               },
    /** BER-encoded Complete Certificate */
    { CKA_VALUE,                      pkcs11_cert_get_encoded                                                                           },
    /** If not empty this attribute gives the URL where the complete
       certificate can be obtained (default empty) */
    { CKA_URL,                        pkcs11_attrib_empty                                                                               },
    /** Hash of the subject public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_SUBJECT_PUBLIC_KEY, pkcs11_cert_get_subject_key_id                                                                    },
    /** Hash of the issuer public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_ISSUER_PUBLIC_KEY,  pkcs11_attrib_empty                                                                               },
    /** Java MIDP security domain. (default CK_SECURITY_DOMAIN_UNSPECIFIED) */
    { CKA_JAVA_MIDP_SECURITY_DOMAIN,  NULL_PTR                                                                                          },
    /** Defines the mechanism used to calculate CKA_HASH_OF_SUBJECT_PUBLIC_KEY
       and CKA_HASH_OF_ISSUER_PUBLIC_KEY. If the attribute is not present then
       the type defaults to SHA-1. */
    { CKA_NAME_HASH_ALGORITHM,        pkcs11_attrib_empty                                                                               },
};

const CK_ULONG pkcs11_cert_x509public_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_cert_x509public_attributes);

/**
 * CKO_CERTIFICATE (Type: CKC_WTLS) - WTLS Public Key Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_wtlspublic_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                      pkcs11_object_get_class                                                                         },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                      pkcs11_attrib_true                                                                              },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,                    pkcs11_token_get_access_type                                                                    },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,                 NULL_PTR                                                                                        },
    /** Description of the object(default empty). */
    { CKA_LABEL,                      pkcs11_object_get_name                                                                          },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,                   pkcs11_attrib_false                                                                             },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,                pkcs11_object_get_destroyable                                                                   },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,           pkcs11_cert_get_type                                                                            },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,                    NULL_PTR                                                                                        },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY,       pkcs11_object_get_type                                                                          },
    /** Checksum */
    { CKA_CHECK_VALUE,                NULL_PTR                                                                                        },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,                 pkcs11_attrib_empty                                                                             },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,                   pkcs11_attrib_empty                                                                             },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,            pkcs11_attrib_empty                                                                             },
    /** WTLS-encoded Certificate subject name */
    { CKA_SUBJECT,                    pkcs11_attrib_empty                                                                             },
    /** WTLS-encoded Certificate issuer name (default empty)*/
    { CKA_ISSUER,                     pkcs11_attrib_empty                                                                             },
    /** WTLS-encoded Complete Certificate */
    { CKA_VALUE,                      pkcs11_cert_get_encoded                                                                         },
    /** If not empty this attribute gives the URL where the complete
       certificate can be obtained (default empty) */
    { CKA_URL,                        pkcs11_attrib_empty                                                                             },
    /** Hash of the subject public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_SUBJECT_PUBLIC_KEY, pkcs11_cert_get_subject_key_id                                                                  },
    /** Hash of the issuer public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_ISSUER_PUBLIC_KEY,  pkcs11_attrib_empty                                                                             },
    /** Defines the mechanism used to calculate CKA_HASH_OF_SUBJECT_PUBLIC_KEY
       and CKA_HASH_OF_ISSUER_PUBLIC_KEY. If the attribute is not present then
       the type defaults to SHA-1. */
    { CKA_NAME_HASH_ALGORITHM,        pkcs11_attrib_empty                                                                             },
};

const CK_ULONG pkcs11_cert_wtlspublic_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_cert_wtlspublic_attributes);

/**
 * CKO_CERTIFICATE (Type: CKC_X_509_ATTR_CERT) - X509 Attribute Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_x509_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                pkcs11_object_get_class                                                         },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                pkcs11_attrib_true                                                              },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,              pkcs11_token_get_access_type                                                    },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,           NULL_PTR                                                                        },
    /** Description of the object(default empty). */
    { CKA_LABEL,                pkcs11_object_get_name                                                          },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,             pkcs11_attrib_false                                                             },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,          pkcs11_object_get_destroyable                                                   },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,     pkcs11_cert_get_type                                                            },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,              NULL_PTR                                                                        },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY, pkcs11_object_get_type                                                          },
    /** Checksum */
    { CKA_CHECK_VALUE,          NULL_PTR                                                                        },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,           pkcs11_attrib_empty                                                             },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,             pkcs11_attrib_empty                                                             },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,      pkcs11_attrib_empty                                                             },
    /** X509: DER-encoding of the attribute certificate's subject field. This
       is distinct from the CKA_SUBJECT attribute contained in CKC_X_509
       certificates because the ASN.1 syntax and encoding are different. */
    { CKA_OWNER,                pkcs11_attrib_empty                                                             },
    /** X509: DER-encoding of the attribute certificate's issuer field. This
       is distinct from the CKA_ISSUER attribute contained in CKC_X_509
       certificates because the ASN.1 syntax and encoding are different.
       (default empty) */
    { CKA_AC_ISSUER,            pkcs11_attrib_empty                                                             },
    /** DER-encoding of the certificate serial number (default empty) */
    { CKA_SERIAL_NUMBER,        pkcs11_attrib_empty                                                             },
    /** X509: BER-encoding of a sequence of object identifier values corresponding
       to the attribute types contained in the certificate. When present, this
       field offers an opportunity for applications to search for a particular
       attribute certificate without fetching and parsing the certificate itself.
       (default empty) */
    { CKA_ATTR_TYPES,           pkcs11_attrib_empty                                                             },
    /** BER-encoded Complete Certificate */
    { CKA_VALUE,                pkcs11_cert_get_encoded                                                         },
};

const CK_ULONG pkcs11_cert_x509_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_cert_x509_attributes);

CK_RV pkcs11_cert_x509_write(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    ATCA_STATUS status;

    if (!obj_ptr || !pAttribute || !pAttribute->pValue || pAttribute->type != CKA_VALUE)
    {
        return CKR_ARGUMENTS_BAD;
    }

    status = atcacert_write_cert(obj_ptr->data, pAttribute->pValue, pAttribute->ulValueLen);

    if (ATCA_SUCCESS == status)
    {
        return CKR_OK;
    }
    else
    {
        return CKR_GENERAL_ERROR;
    }
}



/** @} */

