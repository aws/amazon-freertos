/**
 * \file
 * \brief PKCS11 Library Certificate Handling
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"
#include "atcacert/atcacert_def.h"
#include "atcacert/atcacert_client.h"
#if defined(ATCA_TNGTLS_SUPPORT) || defined(ATCA_TNGLORA_SUPPORT) || defined(ATCA_TFLEX_SUPPORT)
#include "tng_atca.h"
#endif

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_token.h"
#include "pkcs11_cert.h"
#include "pkcs11_os.h"
#include "pkcs11_util.h"

/**
 * \defgroup pkcs11 Key (pkcs11_key_)
   @{ */

static void pkcs11_cert_check_trust_data(pkcs11_object_ptr pObject)
{
#if defined(ATCA_TNGTLS_SUPPORT) || defined(ATCA_TNGLORA_SUPPORT) || defined(ATCA_TFLEX_SUPPORT)
    if (PKCS11_OBJECT_FLAG_TRUST_TYPE & pObject->flags && !pObject->data)
    {
        const atcacert_def_t * cert_def = NULL;
        (void)tng_get_device_cert_def(&cert_def);

        if (cert_def)
        {
            if (CK_CERTIFICATE_CATEGORY_AUTHORITY == pObject->class_type)
            {
                pObject->data = (void*)cert_def->ca_cert_def;
            }
            else
            {
                pObject->data = (void*)cert_def;
            }
        }
    }
#endif
}

static CK_RV pkcs11_cert_load(pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    ATCA_STATUS status = ATCA_SUCCESS;

    if (pObject->data)
    {
        atcacert_def_t * cert_cfg = (atcacert_def_t*)pObject->data;

        /* Load Certificate */
        if (pAttribute->pValue && pAttribute->ulValueLen)
        {
            uint8_t ca_key[64];
            status  = ATCA_SUCCESS;

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
            status = atcacert_read_cert(pObject->data, cert_cfg->ca_cert_def ? ca_key : NULL, pAttribute->pValue, &temp);
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
        if (atcab_is_ca_device(atcab_get_device_type()))
        {
            return pkcs11_attrib_empty(NULL, pAttribute);
        }
        else
        {
#if ATCA_TA_SUPPORT
            uint8_t handle_info[TA_HANDLE_INFO_SIZE];
            status = talib_info_get_handle_info(atcab_get_device(), pObject->slot, handle_info);

            if (ATCA_SUCCESS == status)
            {
                uint16_t cert_size = ((ta_element_attributes_t*)handle_info)->property;

                if (pAttribute->pValue && (pAttribute->ulValueLen >= cert_size))
                {
                    status = talib_read_element(atcab_get_device(), pObject->slot, &cert_size, pAttribute->pValue);
                    pAttribute->ulValueLen = cert_size;
                }
                else
                {
                    pAttribute->ulValueLen = (CK_ULONG)cert_size;
                }
            }
            else
            {
                return CKR_GENERAL_ERROR;
            }
            return CKR_OK;
#endif
        }
    }
    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_cert_get_encoded(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        pkcs11_cert_check_trust_data(obj_ptr);
        return pkcs11_cert_load(obj_ptr, pAttribute);
    }

    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_cert_get_type(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (atcab_is_ca_device(atcab_get_device_type()))
        {
            pkcs11_cert_check_trust_data(obj_ptr);

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
        else
        {
            return pkcs11_attrib_value(pAttribute, CKC_X_509, sizeof(CK_CERTIFICATE_TYPE));
        }
    }

    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_cert_get_subject(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
#ifndef ATCA_NO_HEAP
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    CK_RV rv = CKR_ARGUMENTS_BAD;

    if (obj_ptr)
    {
        pkcs11_cert_check_trust_data(obj_ptr);

        if (obj_ptr->data)
        {
            atcacert_def_t* cert_def = (atcacert_def_t*)obj_ptr->data;
            const atcacert_cert_element_t * subj_element = NULL;

            if (cert_def->cert_elements)
            {
                int i;
                for (i = 0; i < cert_def->cert_elements_count; i++)
                {
                    if (!strcmp(cert_def->cert_elements[i].id, "subject"))
                    {
                        subj_element = &cert_def->cert_elements[i];
                        break;
                    }
                }
            }

            if (subj_element)
            {
                CK_ATTRIBUTE cert_attr = { 0, NULL, 0 };

                /* Get the buffer size required first */
                rv = pkcs11_cert_load(obj_ptr, &cert_attr);

                if (CKR_OK == rv)
                {
                    cert_attr.pValue = pkcs11_os_malloc(cert_attr.ulValueLen);
                    rv = pkcs11_cert_load(obj_ptr, &cert_attr);
                }

                if (CKR_OK == rv)
                {
                    rv = pkcs11_attrib_fill(pAttribute, &((uint8_t*)cert_attr.pValue)[subj_element->cert_loc.offset],
                                            subj_element->cert_loc.count);
                }

                if (cert_attr.pValue)
                {
                    pkcs11_os_free(cert_attr.pValue);
                }
            }
        }
    }

    return rv;
#else
    return pkcs11_attrib_empty(pObject, pAttribute);
#endif
}

CK_RV pkcs11_cert_get_subject_key_id(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        pkcs11_cert_check_trust_data(obj_ptr);

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

CK_RV pkcs11_cert_get_authority_key_id(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        pkcs11_cert_check_trust_data(obj_ptr);

        if (obj_ptr->data)
        {
        }
    }
    return CKR_ARGUMENTS_BAD;
}

CK_RV pkcs11_cert_get_trusted_flag(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        if (CK_CERTIFICATE_CATEGORY_AUTHORITY == obj_ptr->class_type)
        {
            return pkcs11_attrib_true(NULL, pAttribute);
        }
        else
        {
            return pkcs11_attrib_false(NULL, pAttribute);
        }
    }
    return CKR_ARGUMENTS_BAD;
}

/**
 * CKO_CERTIFICATE (Type: CKC_X_509) - X509 Public Key Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_x509public_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                      pkcs11_object_get_class                                                                                                   },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                      pkcs11_attrib_true                                                                                                        },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,                    pkcs11_token_get_access_type                                                                                              },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,                 pkcs11_token_get_writable                                                                                                 },
    /** Description of the object(default empty). */
    { CKA_LABEL,                      pkcs11_object_get_name                                                                                                    },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,                   pkcs11_attrib_false                                                                                                       },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,                pkcs11_object_get_destroyable                                                                                             },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,           pkcs11_cert_get_type                                                                                                      },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,                    pkcs11_cert_get_trusted_flag                                                                                              },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY,       pkcs11_object_get_type                                                                                                    },
    /** Checksum */
    { CKA_CHECK_VALUE,                NULL_PTR                                                                                                                  },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,                 pkcs11_attrib_empty                                                                                                       },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,                   pkcs11_attrib_empty                                                                                                       },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,            pkcs11_attrib_empty                                                                                                       },
    /** DER-encoded Certificate subject name */
    { CKA_SUBJECT,                    pkcs11_cert_get_subject                                                                                                   },
    /** Key identifier for public/private key pair (default empty) */
    { CKA_ID,                         pkcs11_attrib_empty                                                                                                       },
    /** DER-encoded Certificate issuer name (default empty)*/
    { CKA_ISSUER,                     pkcs11_attrib_empty                                                                                                       },
    /** DER-encoding of the certificate serial number (default empty) */
    { CKA_SERIAL_NUMBER,              pkcs11_attrib_empty                                                                                                       },
    /** BER-encoded Complete Certificate */
    { CKA_VALUE,                      pkcs11_cert_get_encoded                                                                                                   },
    /** If not empty this attribute gives the URL where the complete
       certificate can be obtained (default empty) */
    { CKA_URL,                        pkcs11_attrib_empty                                                                                                       },
    /** Hash of the subject public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_SUBJECT_PUBLIC_KEY, pkcs11_cert_get_subject_key_id                                                                                            },
    /** Hash of the issuer public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_ISSUER_PUBLIC_KEY,  pkcs11_cert_get_authority_key_id                                                                                          },
    /** Java MIDP security domain. (default CK_SECURITY_DOMAIN_UNSPECIFIED) */
    { CKA_JAVA_MIDP_SECURITY_DOMAIN,  NULL_PTR                                                                                                                  },
    /** Defines the mechanism used to calculate CKA_HASH_OF_SUBJECT_PUBLIC_KEY
       and CKA_HASH_OF_ISSUER_PUBLIC_KEY. If the attribute is not present then
       the type defaults to SHA-1. */
    { CKA_NAME_HASH_ALGORITHM,        pkcs11_attrib_empty                                                                                                       },
};

const CK_ULONG pkcs11_cert_x509public_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_cert_x509public_attributes);

/**
 * CKO_CERTIFICATE (Type: CKC_WTLS) - WTLS Public Key Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_wtlspublic_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                      pkcs11_object_get_class                                                                                                },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                      pkcs11_attrib_true                                                                                                     },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,                    pkcs11_token_get_access_type                                                                                           },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,                 NULL_PTR                                                                                                               },
    /** Description of the object(default empty). */
    { CKA_LABEL,                      pkcs11_object_get_name                                                                                                 },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,                   pkcs11_attrib_false                                                                                                    },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,                pkcs11_object_get_destroyable                                                                                          },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,           pkcs11_cert_get_type                                                                                                   },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,                    NULL_PTR                                                                                                               },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY,       pkcs11_object_get_type                                                                                                 },
    /** Checksum */
    { CKA_CHECK_VALUE,                NULL_PTR                                                                                                               },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,                 pkcs11_attrib_empty                                                                                                    },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,                   pkcs11_attrib_empty                                                                                                    },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,            pkcs11_attrib_empty                                                                                                    },
    /** WTLS-encoded Certificate subject name */
    { CKA_SUBJECT,                    pkcs11_attrib_empty                                                                                                    },
    /** WTLS-encoded Certificate issuer name (default empty)*/
    { CKA_ISSUER,                     pkcs11_attrib_empty                                                                                                    },
    /** WTLS-encoded Complete Certificate */
    { CKA_VALUE,                      pkcs11_cert_get_encoded                                                                                                },
    /** If not empty this attribute gives the URL where the complete
       certificate can be obtained (default empty) */
    { CKA_URL,                        pkcs11_attrib_empty                                                                                                    },
    /** Hash of the subject public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_SUBJECT_PUBLIC_KEY, pkcs11_cert_get_subject_key_id                                                                                         },
    /** Hash of the issuer public key (default empty). Hash algorithm is
       defined by CKA_NAME_HASH_ALGORITHM */
    { CKA_HASH_OF_ISSUER_PUBLIC_KEY,  pkcs11_attrib_empty                                                                                                    },
    /** Defines the mechanism used to calculate CKA_HASH_OF_SUBJECT_PUBLIC_KEY
       and CKA_HASH_OF_ISSUER_PUBLIC_KEY. If the attribute is not present then
       the type defaults to SHA-1. */
    { CKA_NAME_HASH_ALGORITHM,        pkcs11_attrib_empty                                                                                                    },
};

const CK_ULONG pkcs11_cert_wtlspublic_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_cert_wtlspublic_attributes);

/**
 * CKO_CERTIFICATE (Type: CKC_X_509_ATTR_CERT) - X509 Attribute Certificate Model
 */
const pkcs11_attrib_model pkcs11_cert_x509_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,                pkcs11_object_get_class                                                                        },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,                pkcs11_attrib_true                                                                             },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,              pkcs11_token_get_access_type                                                                   },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,           NULL_PTR                                                                                       },
    /** Description of the object(default empty). */
    { CKA_LABEL,                pkcs11_object_get_name                                                                         },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,             pkcs11_attrib_false                                                                            },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,          pkcs11_object_get_destroyable                                                                  },
    /** Type of certificate */
    { CKA_CERTIFICATE_TYPE,     pkcs11_cert_get_type                                                                           },
    /** The certificate can be trusted for the application that it was created. */
    { CKA_TRUSTED,              NULL_PTR                                                                                       },
    /** Default CK_CERTIFICATE_CATEGORY_UNSPECIFIED) */
    { CKA_CERTIFICATE_CATEGORY, pkcs11_object_get_type                                                                         },
    /** Checksum */
    { CKA_CHECK_VALUE,          NULL_PTR                                                                                       },
    /** Start date for the certificate (default empty) */
    { CKA_START_DATE,           pkcs11_attrib_empty                                                                            },
    /** End date for the certificate (default empty) */
    { CKA_END_DATE,             pkcs11_attrib_empty                                                                            },
    /** ALL: DER-encoding of the SubjectPublicKeyInfo for the public key
       contained in this certificate (default empty)
       SubjectPublicKeyInfo ::= SEQUENCE {
       algorithm AlgorithmIdentifier,
       subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,      pkcs11_attrib_empty                                                                            },
    /** X509: DER-encoding of the attribute certificate's subject field. This
       is distinct from the CKA_SUBJECT attribute contained in CKC_X_509
       certificates because the ASN.1 syntax and encoding are different. */
    { CKA_OWNER,                pkcs11_attrib_empty                                                                            },
    /** X509: DER-encoding of the attribute certificate's issuer field. This
       is distinct from the CKA_ISSUER attribute contained in CKC_X_509
       certificates because the ASN.1 syntax and encoding are different.
       (default empty) */
    { CKA_AC_ISSUER,            pkcs11_attrib_empty                                                                            },
    /** DER-encoding of the certificate serial number (default empty) */
    { CKA_SERIAL_NUMBER,        pkcs11_attrib_empty                                                                            },
    /** X509: BER-encoding of a sequence of object identifier values corresponding
       to the attribute types contained in the certificate. When present, this
       field offers an opportunity for applications to search for a particular
       attribute certificate without fetching and parsing the certificate itself.
       (default empty) */
    { CKA_ATTR_TYPES,           pkcs11_attrib_empty                                                                            },
    /** BER-encoded Complete Certificate */
    { CKA_VALUE,                pkcs11_cert_get_encoded                                                                        },
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

    if (atcab_is_ca_device(atcab_get_device_type()))
    {
        status = atcacert_write_cert(obj_ptr->data, pAttribute->pValue, pAttribute->ulValueLen);
    }
    else
    {
#if ATCA_TA_SUPPORT
        ATCADevice device = atcab_get_device();
        uint8_t handle_info[TA_HANDLE_INFO_SIZE];
        status = talib_info_get_handle_info(device, obj_ptr->slot, handle_info);

        if (TA_HANDLE_EXIST_ERROR == status)
        {
            /* Create a new handle */
            talib_handle_init_data((ta_element_attributes_t*)handle_info, (uint16_t)pAttribute->ulValueLen);
            status = talib_create_element_with_handle(device, obj_ptr->slot, (ta_element_attributes_t*)handle_info);
        }

        if (ATCA_SUCCESS == status)
        {
            status = talib_write_element(device, obj_ptr->slot, (uint16_t)pAttribute->ulValueLen, pAttribute->pValue);
        }
#else
        status = ATCA_NO_DEVICES;
#endif
    }

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

