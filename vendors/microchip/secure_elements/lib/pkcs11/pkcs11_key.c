/**
 * \file
 * \brief PKCS11 Library Key Object Handling
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
#include "crypto/atca_crypto_sw_sha1.h"

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_token.h"
#include "pkcs11_attrib.h"
#include "pkcs11_key.h"
#include "pkcs11_session.h"
#include "pkcs11_slot.h"
#include "pkcs11_util.h"
#include "pkcs11_os.h"

/**
 * \defgroup pkcs11 Key (pkcs11_key_)
   @{ */

static CK_RV pkcs11_key_get_derivekey_flag(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

        if (!pConfig)
        {
            return CKR_GENERAL_ERROR;
        }

        if (ATCA_SLOT_CONFIG_ECDH_MASK & pConfig->SlotConfig[obj_ptr->slot])
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

static CK_RV pkcs11_key_get_local_flag(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

        if (!pConfig)
        {
            return CKR_GENERAL_ERROR;
        }

        if (ATCA_SLOT_CONFIG_WRITE_CONFIG(2) == (ATCA_SLOT_CONFIG_WRITE_CONFIG_MASK & pConfig->SlotConfig[obj_ptr->slot]))
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

#if 0
static CK_RV pkcs11_key_get_gen_mechanism(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    ((void)pObject);
    ((void)pAttribute);
    return CKR_FUNCTION_NOT_SUPPORTED;
}
#endif

static const CK_MECHANISM_TYPE pkcs11_key_508_public_mech[] = {
    CKM_ECDSA,
    CKM_ECDSA_SHA256
};

static const CK_MECHANISM_TYPE pkcs11_key_508_private_mech[] = {
    CKM_EC_KEY_PAIR_GEN,
    CKM_ECDSA,
    CKM_ECDSA_SHA256
};

static CK_RV pkcs11_key_get_allowed_mechanisms(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

        if (!pConfig)
        {
            return CKR_GENERAL_ERROR;
        }

        if (ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[obj_ptr->slot])
        {
            return pkcs11_attrib_fill(pAttribute, (CK_VOID_PTR)pkcs11_key_508_private_mech,
                                      sizeof(pkcs11_key_508_private_mech));
        }
        else
        {
            return pkcs11_attrib_fill(pAttribute, (CK_VOID_PTR)pkcs11_key_508_public_mech,
                                      sizeof(pkcs11_key_508_public_mech));
        }
    }

    return CKR_ARGUMENTS_BAD;
}

/** ASN.1 Header for SECP256R1 public keys */
static const uint8_t ec_pubkey_asn1_header[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86,
    0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04
};

/** X.962 ASN.1 Header for EC public keys */
static const uint8_t ec_x962_asn1_header[] = {
    0x04, 0x41, 0x04
};

/**
 * \brief Extract a public key and convert it to the asn.1 format
 */
static CK_RV pkcs11_key_get_public_key(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    atecc508a_config_t * pConfig;
    CK_UTF8CHAR ec_asn1_key[sizeof(ec_pubkey_asn1_header) + ATCA_PUB_KEY_SIZE];
    ATCA_STATUS status;

    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    pConfig = (atecc508a_config_t*)obj_ptr->config;

    if (!pConfig)
    {
        return CKR_GENERAL_ERROR;
    }

    memcpy(ec_asn1_key, ec_pubkey_asn1_header, sizeof(ec_pubkey_asn1_header));

    if (ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[obj_ptr->slot])
    {
        status = atcab_get_pubkey(obj_ptr->slot, &ec_asn1_key[sizeof(ec_pubkey_asn1_header)]);
    }
    else
    {
        status = atcab_read_pubkey(obj_ptr->slot, &ec_asn1_key[sizeof(ec_pubkey_asn1_header)]);
    }

    if (ATCA_SUCCESS == status)
    {
        return pkcs11_attrib_fill(pAttribute, ec_asn1_key, sizeof(ec_asn1_key));
    }
    else
    {
        return CKR_FUNCTION_FAILED;
    }
}

static const uint8_t pkcs11_key_ec_params[] = { 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 };

static CK_RV pkcs11_key_get_ec_params(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    return pkcs11_attrib_fill(pAttribute, (CK_VOID_PTR)pkcs11_key_ec_params, sizeof(pkcs11_key_ec_params));
}

static CK_RV pkcs11_key_get_ec_point(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    CK_UTF8CHAR ec_asn1_key[3 + ATCA_PUB_KEY_SIZE] = { 0x04, 0x41, 0x04 };
    ATCA_STATUS status = ATCA_SUCCESS;


    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (pAttribute->pValue)
    {
        bool is_private = TRUE;

        if (obj_ptr->flags & PKCS11_OBJECT_FLAG_TA_TYPE)
        {
#if ATCA_TA_SUPPORT
            is_private = (TA_CLASS_PRIVATE_KEY == obj_ptr->handle_info.element_CKA & 0xF);
#endif
        }
        else
        {
            atecc508a_config_t * pConfig = (atecc508a_config_t*)obj_ptr->config;

            if (!pConfig)
            {
                return CKR_GENERAL_ERROR;
            }

            is_private = (ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[obj_ptr->slot]);
        }

        /* Only read from the device if there is someplace to store the result */
        if (is_private)
        {
            status = atcab_get_pubkey(obj_ptr->slot, &ec_asn1_key[3]);
            PKCS11_DEBUG("atcab_get_pubkey: %x\r\n", status);
        }
        else
        {
            status = atcab_read_pubkey(obj_ptr->slot, &ec_asn1_key[3]);
            PKCS11_DEBUG("atcab_read_pubkey: %x\r\n", status);
        }
    }

    if (ATCA_SUCCESS == status)
    {
        return pkcs11_attrib_fill(pAttribute, ec_asn1_key, sizeof(ec_asn1_key));
    }
    else
    {
        return CKR_FUNCTION_FAILED;
    }
}

static CK_RV pkcs11_key_get_secret(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)obj_ptr->data, obj_ptr->size);
    }
    else
    {
        return CKR_ARGUMENTS_BAD;
    }
}

static CK_RV pkcs11_key_get_secret_length(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)obj_ptr->size, sizeof(CK_ULONG));
    }
    else
    {
        return CKR_ARGUMENTS_BAD;
    }
}

static CK_RV pkcs11_key_get_check_value(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (obj_ptr)
    {
        uint8_t sha1_digest[ATCA_SHA1_DIGEST_SIZE];
        if (ATCA_SUCCESS == atcac_sw_sha1(obj_ptr->data, obj_ptr->size, sha1_digest))
        {
            return pkcs11_attrib_fill(pAttribute, sha1_digest, 3);
        }
        else
        {
            return CKR_GENERAL_ERROR;
        }
    }
    else
    {
        return CKR_ARGUMENTS_BAD;
    }
}

/**
 * CKO_PUBLIC_KEY - Public Key Object Model
 */
const pkcs11_attrib_model pkcs11_key_public_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,              pkcs11_object_get_class                                                                             },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,              pkcs11_attrib_true                                                                                  },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,            pkcs11_attrib_false                                                                                 },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,         pkcs11_token_get_writable                                                                           },
    /** Description of the object(default empty). */
    { CKA_LABEL,              pkcs11_object_get_name                                                                              },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,           pkcs11_attrib_false                                                                                 },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,        pkcs11_object_get_destroyable                                                                       },
    /** Type of key */
    { CKA_KEY_TYPE,           pkcs11_object_get_type                                                                              },
    /** Key identifier for key (default empty) */
    { CKA_ID,                 pkcs11_attrib_empty                                                                                 },
    /** Start date for the key (default empty) */
    { CKA_START_DATE,         pkcs11_attrib_empty                                                                                 },
    /** End date for the key (default empty) */
    { CKA_END_DATE,           pkcs11_attrib_empty                                                                                 },
    /** CK_TRUE if key supports key derivation (i.e., if other keys can be derived from this one (default CK_FALSE) */
    { CKA_DERIVE,             pkcs11_key_get_derivekey_flag                                                                       },
    /** CK_TRUE only if key was either generated locally (i.e., on the token)
       with a C_GenerateKey or C_GenerateKeyPair call created with a C_CopyObject
       call as a copy of a key which had its CKA_LOCAL attribute set to CK_TRUE */
    { CKA_LOCAL,              pkcs11_attrib_true                                                                                  },
    /** Identifier of the mechanism used to generate the key material. */
    { CKA_KEY_GEN_MECHANISM,  NULL_PTR                                                                                            },
    /** A list of mechanisms allowed to be used with this key. The number of
       mechanisms in the array is the ulValueLen component of the attribute
       divided by the size of CK_MECHANISM_TYPE. */
    { CKA_ALLOWED_MECHANISMS, pkcs11_key_get_allowed_mechanisms                                                                   },
    /** DER-encoding of the key subject name (default empty) */
    { CKA_SUBJECT,            pkcs11_attrib_empty                                                                                 },
    /** CK_TRUE if key supports encryption */
    { CKA_ENCRYPT,            NULL_PTR                                                                                            },
    /** CK_TRUE if key supports verification where the signature is an appendix to the data */
    { CKA_VERIFY,             pkcs11_attrib_true                                                                                  },
    /** CK_TRUE if key supports verification where the data is recovered from the signature */
    { CKA_VERIFY_RECOVER,     NULL_PTR                                                                                            },
    /** CK_TRUE if key supports wrapping (i.e., can be used to wrap other keys) */
    { CKA_WRAP,               NULL_PTR                                                                                            },
    /** The key can be trusted for the application that it was created. The
        wrapping key can be used to wrap keys with CKA_WRAP_WITH_TRUSTED set
        to CK_TRUE. */
    { CKA_TRUSTED,            NULL_PTR                                                                                            },
    /** For wrapping keys. The attribute template to match against any keys
        wrapped using this wrapping key. Keys that do not match cannot be
        wrapped. The number of attributes in the array is the ulValueLen
        component of the attribute divided by the size of CK_ATTRIBUTE. */
    { CKA_WRAP_TEMPLATE,      NULL_PTR                                                                                            },
    /** DER-encoding of the SubjectPublicKeyInfo for this public key.
        (MAY be empty, DEFAULT derived from the underlying public key data)
        SubjectPublicKeyInfo ::= SEQUENCE {
            algorithm AlgorithmIdentifier,
            subjectPublicKey BIT_STRING } */
    { CKA_PUBLIC_KEY_INFO,    pkcs11_key_get_public_key                                                                           },

    /** DER - encoding of an ANSI X9.62 Parameters value
        Parameters ::= CHOICE {
            ecParameters ECParameters,
            namedCurve CURVES.&id({CurveNames}),
            implicitlyCA NULL } */
    { CKA_EC_PARAMS,          pkcs11_key_get_ec_params                                                                            },
    /** DER - encoding of ANSI X9.62 ECPoint value Q */
    { CKA_EC_POINT,           pkcs11_key_get_ec_point                                                                             },
};

const CK_ULONG pkcs11_key_public_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_key_public_attributes);

/**
 * CKO_PUBLIC_KEY (Type: CKK_EC) - EC/ECDSA Public Key Object Model
 */
const pkcs11_attrib_model pkcs11_key_ec_public_attributes[] = {
    /** DER - encoding of an ANSI X9.62 Parameters value
        Parameters ::= CHOICE {
            ecParameters ECParameters,
            namedCurve CURVES.&id({CurveNames}),
            implicitlyCA NULL } */
    { CKA_EC_PARAMS, pkcs11_key_get_ec_params                       },
    /** DER - encoding of ANSI X9.62 ECPoint value Q */
    { CKA_EC_POINT,  pkcs11_key_get_ec_point                        },
};

/**
 * CKO_PRIVATE_KEY - Private Key Object Base Model
 */
const pkcs11_attrib_model pkcs11_key_private_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,               pkcs11_object_get_class                                                                                },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,               pkcs11_attrib_true                                                                                     },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,             pkcs11_attrib_true                                                                                     },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,          pkcs11_token_get_writable                                                                              },
    /** Description of the object(default empty). */
    { CKA_LABEL,               pkcs11_object_get_name                                                                                 },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,            pkcs11_attrib_false                                                                                    },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,         pkcs11_object_get_destroyable                                                                          },
    /** Type of key */
    { CKA_KEY_TYPE,            pkcs11_object_get_type                                                                                 },
    /** Key identifier for key (default empty) */
    { CKA_ID,                  pkcs11_attrib_empty                                                                                    },
    /** Start date for the key (default empty) */
    { CKA_START_DATE,          pkcs11_attrib_empty                                                                                    },
    /** End date for the key (default empty) */
    { CKA_END_DATE,            pkcs11_attrib_empty                                                                                    },
    /** CK_TRUE if key supports key derivation (i.e., if other keys can be derived from this one (default CK_FALSE) */
    { CKA_DERIVE,              pkcs11_key_get_derivekey_flag                                                                          },
    /** CK_TRUE only if key was either generated locally (i.e., on the token)
       with a C_GenerateKey or C_GenerateKeyPair call created with a C_CopyObject
       call as a copy of a key which had its CKA_LOCAL attribute set to CK_TRUE */
    { CKA_LOCAL,               pkcs11_key_get_local_flag                                                                              },
    /** Identifier of the mechanism used to generate the key material. */
    { CKA_KEY_GEN_MECHANISM,   NULL_PTR                                                                                               },
    /** A list of mechanisms allowed to be used with this key. The number of
       mechanisms in the array is the ulValueLen component of the attribute
       divided by the size of CK_MECHANISM_TYPE. */
    { CKA_ALLOWED_MECHANISMS,  pkcs11_key_get_allowed_mechanisms                                                                      },
    /** DER-encoding of the key subject name (default empty) */
    { CKA_SUBJECT,             pkcs11_attrib_empty                                                                                    },
    /** CK_TRUE if key is sensitive */
    { CKA_SENSITIVE,           pkcs11_token_get_access_type                                                                           },
    /** CK_TRUE if key supports decryption */
    { CKA_DECRYPT,             NULL_PTR                                                                                               },
    /** CK_TRUE if key supports signatures where the signature is an appendix to the data */
    { CKA_SIGN,                pkcs11_attrib_true                                                                                     },
    /** CK_TRUE if key supports signatures where the data can be recovered from the signature9 */
    { CKA_SIGN_RECOVER,        NULL_PTR                                                                                               },
    /** CK_TRUE if key supports unwrapping (i.e., can be used to unwrap other keys)9 */
    { CKA_UNWRAP,              NULL_PTR                                                                                               },
    /** CK_TRUE if key is extractable and can be wrapped 9 */
    { CKA_EXTRACTABLE,         NULL_PTR                                                                                               },
    /** CK_TRUE if key has always had the CKA_SENSITIVE attribute set to CK_TRUE */
    { CKA_ALWAYS_SENSITIVE,    pkcs11_token_get_access_type                                                                           },
    /** CK_TRUE if key has never had the CKA_EXTRACTABLE attribute set to CK_TRUE */
    { CKA_NEVER_EXTRACTABLE,   NULL_PTR                                                                                               },
    /** CK_TRUE if the key can only be wrapped with a wrapping key that has CKA_TRUSTED set to CK_TRUE. Default is CK_FALSE. */
    { CKA_WRAP_WITH_TRUSTED,   NULL_PTR                                                                                               },
    /** For wrapping keys. The attribute template to match against any keys
        wrapped using this wrapping key. Keys that do not match cannot be
        wrapped. The number of attributes in the array is the ulValueLen
        component of the attribute divided by the size of CK_ATTRIBUTE. */
    { CKA_UNWRAP_TEMPLATE,     NULL_PTR                                                                                               },
    /** If CK_TRUE, the user has to  supply the PIN for each use (sign or decrypt) with the key. Default is CK_FALSE. */
    { CKA_ALWAYS_AUTHENTICATE, pkcs11_attrib_false                                                                                    },
    /** DER-encoding of the SubjectPublicKeyInfo for the associated public key
        (MAY be empty; DEFAULT derived from the underlying private key data;
        MAY be manually set for specific key types; if set; MUST be consistent
        with the underlying private key data)   */
    { CKA_PUBLIC_KEY_INFO,     pkcs11_key_get_public_key                                                                              },
    /** DER - encoding of an ANSI X9.62 Parameters value
        Parameters ::= CHOICE {
            ecParameters ECParameters,
            namedCurve CURVES.&id({CurveNames}),
            implicitlyCA NULL } */
    { CKA_EC_PARAMS,           pkcs11_key_get_ec_params                                                                               },
    /** DER - encoding of ANSI X9.62 ECPoint value Q */
    { CKA_EC_POINT,            pkcs11_key_get_ec_point                                                                                },
    /** The value of the private key should remain private.  A NULL function pointer is interpreted as a sensitive attribute. */
    { CKA_VALUE,               NULL_PTR                                                                                               },
};

const CK_ULONG pkcs11_key_private_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_key_private_attributes);

/**
 * CKO_PRIVATE_KEY (Type: CKK_RSA) - RSA Private Key Object Model
 */
const pkcs11_attrib_model pkcs11_key_rsa_private_attributes[] = {
    /** Big integer Modulus n */
    { CKA_MODULUS,          NULL_PTR                                                 },
    /** Big integer Public exponent e */
    { CKA_PUBLIC_EXPONENT,  NULL_PTR                                                 },
    /** Big integer Private exponent d */
    { CKA_PRIVATE_EXPONENT, NULL_PTR                                                 },
    /** Big integer Prime p */
    { CKA_PRIME_1,          NULL_PTR                                                 },
    /** Big integer Prime q */
    { CKA_PRIME_2,          NULL_PTR                                                 },
    /** Big integer Private exponent d modulo p - 1 */
    { CKA_EXPONENT_1,       NULL_PTR                                                 },
    /** Big integer Private exponent d modulo q - 1 */
    { CKA_EXPONENT_2,       NULL_PTR                                                 },
    /** Big integer CRT coefficient q - 1 mod p */
    { CKA_COEFFICIENT,      NULL_PTR                                                 },
};

/**
 * CKO_PRIVATE_KEY (Type: CKK_EC) - EC/ECDSA Public Key Object Model
 */
const pkcs11_attrib_model pkcs11_key_ec_private_attributes[] = {
    /** DER - encoding of an ANSI X9.62 Parameters value
        Parameters ::= CHOICE {
            ecParameters ECParameters,
            namedCurve CURVES.&id({CurveNames}),
            implicitlyCA NULL } */
    { CKA_EC_PARAMS, pkcs11_key_get_ec_params                       },
    /** DER - encoding of ANSI X9.62 ECPoint value Q */
    { CKA_EC_POINT,  pkcs11_key_get_ec_point                        },
};


/**
 * CKO_SECRET_KEY - Secret Key Object Base Model
 */
const pkcs11_attrib_model pkcs11_key_secret_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,              pkcs11_object_get_class                                                                             },
    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
    { CKA_TOKEN,              pkcs11_token_get_storage                                                                            },
    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
    { CKA_PRIVATE,            pkcs11_token_get_access_type                                                                        },
    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
    { CKA_MODIFIABLE,         pkcs11_token_get_writable                                                                           },
    /** Description of the object(default empty). */
    { CKA_LABEL,              pkcs11_object_get_name                                                                              },
    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
    { CKA_COPYABLE,           pkcs11_attrib_false                                                                                 },
    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
    { CKA_DESTROYABLE,        pkcs11_object_get_destroyable                                                                       },
    /** Type of key */
    { CKA_KEY_TYPE,           pkcs11_object_get_type                                                                              },
    /** Key identifier for key (default empty) */
    { CKA_ID,                 pkcs11_attrib_empty                                                                                 },
    /** Start date for the key (default empty) */
    { CKA_START_DATE,         pkcs11_attrib_empty                                                                                 },
    /** End date for the key (default empty) */
    { CKA_END_DATE,           pkcs11_attrib_empty                                                                                 },
    /** CK_TRUE if key supports key derivation (i.e., if other keys can be derived from this one (default CK_FALSE) */
    { CKA_DERIVE,             pkcs11_attrib_true                                                                                  },
    /** CK_TRUE only if key was either generated locally (i.e., on the token)
       with a C_GenerateKey or C_GenerateKeyPair call created with a C_CopyObject
       call as a copy of a key which had its CKA_LOCAL attribute set to CK_TRUE */
    { CKA_LOCAL,              pkcs11_key_get_local_flag                                                                           },
    /** Identifier of the mechanism used to generate the key material. */
    { CKA_KEY_GEN_MECHANISM,  NULL_PTR                                                                                            },
    /** A list of mechanisms allowed to be used with this key. The number of
       mechanisms in the array is the ulValueLen component of the attribute
       divided by the size of CK_MECHANISM_TYPE. */
    { CKA_ALLOWED_MECHANISMS, NULL_PTR                                                                                            },
    /** CK_TRUE if key is sensitive */
    { CKA_SENSITIVE,          pkcs11_token_get_access_type                                                                        },
    /** CK_TRUE if key supports encryption */
    { CKA_ENCRYPT,            NULL_PTR                                                                                            },
    /** CK_TRUE if key supports decryption */
    { CKA_DECRYPT,            NULL_PTR                                                                                            },
    /** CK_TRUE if key supports signatures (i.e., authentication codes) where
        the signature is an appendix to the data */
    { CKA_SIGN,               NULL_PTR                                                                                            },
    /** CK_TRUE if key supports verification (i.e., of authentication codes)
        where the signature is an appendix to the data */
    { CKA_VERIFY,             NULL_PTR                                                                                            },
    /** CK_TRUE if key supports wrapping (i.e., can be used to wrap other keys)  */
    { CKA_WRAP,               NULL_PTR                                                                                            },
    /** CK_TRUE if key supports unwrapping (i.e., can be used to unwrap other keys) */
    { CKA_UNWRAP,             NULL_PTR                                                                                            },
    /** CK_TRUE if key is extractable and can be wrapped */
    { CKA_EXTRACTABLE,        NULL_PTR                                                                                            },
    /** CK_TRUE if key has always had the CKA_SENSITIVE attribute set to CK_TRUE */
    { CKA_ALWAYS_SENSITIVE,   pkcs11_token_get_access_type                                                                        },
    /** CK_TRUE if key has never had the CKA_EXTRACTABLE attribute set to CK_TRUE  */
    { CKA_NEVER_EXTRACTABLE,  NULL_PTR                                                                                            },
    /** Key checksum */
    { CKA_CHECK_VALUE,        pkcs11_key_get_check_value                                                                          },
    /** CK_TRUE if the key can only be wrapped with a wrapping key that has CKA_TRUSTED set to CK_TRUE. Default is CK_FALSE. */
    { CKA_WRAP_WITH_TRUSTED,  NULL_PTR                                                                                            },
    /**  The wrapping key can be used to wrap keys with CKA_WRAP_WITH_TRUSTED set to CK_TRUE. */
    { CKA_TRUSTED,            NULL_PTR                                                                                            },
    /** For wrapping keys. The attribute template to match against any keys
        wrapped using this wrapping key. Keys that do not match cannot be
        wrapped. The number of attributes in the array is the ulValueLen
        component of the attribute divided by the size of CK_ATTRIBUTE */
    { CKA_WRAP_TEMPLATE,      NULL_PTR                                                                                            },
    /** For wrapping keys. The attribute template to apply to any keys unwrapped
        using this wrapping key. Any user supplied template is applied after
        this template as if the object has already been created. The number of
        attributes in the array is the ulValueLen component of the attribute
        divided by the size of CK_ATTRIBUTE.  */
    { CKA_UNWRAP_TEMPLATE,    NULL_PTR                                                                                            },
    /* Key value */
    { CKA_VALUE,              pkcs11_key_get_secret                                                                               },
    /* Length in bytes of the key */
    { CKA_VALUE_LEN,          pkcs11_key_get_secret_length                                                                        },
};

const CK_ULONG pkcs11_key_secret_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_key_secret_attributes);

CK_RV pkcs11_key_write(CK_VOID_PTR pSession, CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    pkcs11_session_ctx_ptr session_ctx = (pkcs11_session_ctx_ptr)pSession;
    atecc508a_config_t * cfg_ptr;
    ATCA_STATUS status = ATCA_SUCCESS;

    if (!obj_ptr || !pAttribute || !pAttribute->pValue)
    {
        return CKR_ARGUMENTS_BAD;
    }

    cfg_ptr = (atecc508a_config_t*)obj_ptr->config;

    if (obj_ptr->class_id == CKO_PUBLIC_KEY && pAttribute->type == CKA_EC_POINT)
    {
        if (!memcmp(ec_x962_asn1_header, pAttribute->pValue, sizeof(ec_x962_asn1_header)))
        {
            if (cfg_ptr->KeyConfig[obj_ptr->slot] & ATCA_KEY_CONFIG_PRIVATE_MASK)
            {
                /* Assume it is paired with the private key that was stored */
                return CKR_OK;
            }
            else
            {
                /* Actually write the public key into the slot */
                status = atcab_write_pubkey(obj_ptr->slot, &(((uint8_t*)pAttribute->pValue)[sizeof(ec_x962_asn1_header)]));
            }
        }
        else
        {
            return CKR_ARGUMENTS_BAD;
        }
    }
    else if (obj_ptr->class_id == CKO_PRIVATE_KEY && pAttribute->type == CKA_VALUE)
    {
        uint8_t key_buf[36] = { 0, 0, 0, 0 };
        uint16_t write_key_id = cfg_ptr->SlotConfig[obj_ptr->slot];
        write_key_id &= ATCA_SLOT_CONFIG_WRITE_KEY_MASK;
        write_key_id >>= ATCA_SLOT_CONFIG_WRITE_KEY_SHIFT;

        memcpy(&key_buf[4], pAttribute->pValue, 32);

        /* Requires the io protection secret to be configured previously and for the
            configuration to support this - should only be enabled for testing purposes.
            Production devices should never have this feature enabled. */
        status = atcab_priv_write(obj_ptr->slot, key_buf, write_key_id, session_ctx->read_key, NULL);
    }
    else
    {
        return CKR_ARGUMENTS_BAD;
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

CK_RV pkcs11_key_generate_pair
(
    CK_SESSION_HANDLE    hSession,
    CK_MECHANISM_PTR     pMechanism,
    CK_ATTRIBUTE_PTR     pPublicKeyTemplate,
    CK_ULONG             ulPublicKeyAttributeCount,
    CK_ATTRIBUTE_PTR     pPrivateKeyTemplate,
    CK_ULONG             ulPrivateKeyAttributeCount,
    CK_OBJECT_HANDLE_PTR phPublicKey,
    CK_OBJECT_HANDLE_PTR phPrivateKey
)
{
    CK_ATTRIBUTE_PTR pName = NULL;
    pkcs11_lib_ctx_ptr pLibCtx;
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pPublic = NULL;
    pkcs11_object_ptr pPrivate = NULL;
    int i;
    CK_RV rv = CKR_OK;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pMechanism || !pPublicKeyTemplate || !ulPublicKeyAttributeCount ||
        !pPrivateKeyTemplate || !ulPrivateKeyAttributeCount ||
        !phPublicKey || !phPrivateKey)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    /* @todo Perform the various mechanism and key attribute checks */

    if (CKM_EC_KEY_PAIR_GEN != pMechanism->mechanism)
    {
        return CKR_MECHANISM_INVALID;
    }


    for (i = 0; i < ulPrivateKeyAttributeCount; i++)
    {
        if (CKA_LABEL == pPrivateKeyTemplate[i].type)
        {
            pName = &pPrivateKeyTemplate[i];
            break;
        }
    }

    if (!pName || pName->ulValueLen > PKCS11_MAX_LABEL_SIZE)
    {
        return CKR_TEMPLATE_INCONSISTENT;
    }

    /* Must create two new objects - a public and private key */

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pPrivate);
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pPublic);
    }

    if (CKR_OK == rv)
    {
        if (!pPublic || !pPrivate)
        {
            rv = CKR_TEMPLATE_INCONSISTENT;
        }
    }

    if (CKR_OK == rv)
    {
        pPrivate->class_id = CKO_PRIVATE_KEY;
        rv = pkcs11_config_key(pLibCtx, pSession->slot, pPrivate, pName);
    }

    if (CKR_OK == rv)
    {
        pPublic->slot = pPrivate->slot;
        pPublic->flags = pPrivate->flags;
        memcpy(pPublic->name, pName->pValue, pName->ulValueLen);
        pPublic->class_id = CKO_PUBLIC_KEY;
        pPublic->class_type = CKK_EC;
        pPublic->attributes = pkcs11_key_public_attributes;
        pPublic->count = pkcs11_key_public_attributes_count;
        pPublic->size = 64;
        pPublic->config = &((pkcs11_slot_ctx_ptr)pSession->slot)->cfg_zone;

        if (CKR_OK == (rv = pkcs11_lock_context(pLibCtx)))
        {
            rv = pkcs11_util_convert_rv(atcab_genkey(pPrivate->slot, NULL));
            (void)pkcs11_unlock_context(pLibCtx);
        }
    }

    if (CKR_OK == rv)
    {
        pkcs11_object_get_handle(pPrivate, phPrivateKey);
        pkcs11_object_get_handle(pPublic, phPublicKey);
    }
    else
    {
        if (pPrivate)
        {
            pkcs11_object_free(pPrivate);
        }
        if (pPublic)
        {
            pkcs11_object_free(pPublic);
        }
    }

    return rv;
}

#ifdef ATCA_NO_HEAP
static uint8_t pkcs11_key_cache[32];

static uint8_t pkcs11_key_used(uint8_t * key, size_t keylen)
{
    if (key)
    {
        for (int i = 0; i < keylen; i++)
        {
            if (key[i])
            {
                return 1;
            }
        }
    }
    return 0;
}
#endif

CK_RV pkcs11_key_derive
(
    CK_SESSION_HANDLE    hSession,
    CK_MECHANISM_PTR     pMechanism,
    CK_OBJECT_HANDLE     hBaseKey,
    CK_ATTRIBUTE_PTR     pTemplate,
    CK_ULONG             ulCount,
    CK_OBJECT_HANDLE_PTR phKey
)
{
    pkcs11_session_ctx_ptr pSession = NULL;
    pkcs11_lib_ctx_ptr pLibCtx;
    pkcs11_object_ptr pBaseKey = NULL;
    pkcs11_object_ptr pSecretKey = NULL;
    CK_ECDH1_DERIVE_PARAMS_PTR pEcdhParameters = NULL;
    CK_RV rv = CKR_OK;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!hSession || !pMechanism || !hBaseKey ||
        !pTemplate || !ulCount || !phKey)
    {
        return CKR_ARGUMENTS_BAD;
    }

    *phKey = CK_INVALID_HANDLE;

    if (CKM_ECDH1_DERIVE == pMechanism->mechanism || CKM_ECDH1_COFACTOR_DERIVE == pMechanism->mechanism)
    {
        if (sizeof(CK_ECDH1_DERIVE_PARAMS) != pMechanism->ulParameterLen ||
            !pMechanism->pParameter)
        {
            rv = CKR_ARGUMENTS_BAD;
        }
        else
        {
            pEcdhParameters = (CK_ECDH1_DERIVE_PARAMS_PTR)pMechanism->pParameter;
            if (!pEcdhParameters->pPublicData)
            {
                rv = CKR_ARGUMENTS_BAD;
            }
        }
    }
    else
    {
        rv = CKR_FUNCTION_NOT_SUPPORTED;
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_session_check(&pSession, hSession);
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_check(&pBaseKey, hBaseKey);
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pSecretKey);
    }

    for (int i = 0; (i < ulCount) && (CKR_OK == rv); i++)
    {
        if (CKA_LABEL == pTemplate[i].type)
        {
            if (pTemplate[i].pValue && pTemplate[i].ulValueLen > PKCS11_MAX_LABEL_SIZE)
            {
                rv = CKR_TEMPLATE_INCONSISTENT;
            }
            else
            {
                memcpy(pSecretKey->name, pTemplate[i].pValue, pTemplate[i].ulValueLen);
            }
        }
        else if (CKA_CLASS == pTemplate[i].type)
        {
            if (sizeof(pSecretKey->class_id) != pTemplate[i].ulValueLen)
            {
                rv = CKR_TEMPLATE_INCONSISTENT;
            }
            else
            {
                memcpy(&pSecretKey->class_id, pTemplate[i].pValue, sizeof(pSecretKey->class_id));
            }
        }
        else if (CKA_KEY_TYPE == pTemplate[i].type)
        {
            if (sizeof(pSecretKey->class_type) != pTemplate[i].ulValueLen)
            {
                rv = CKR_TEMPLATE_INCONSISTENT;
            }
            else
            {
                memcpy(&pSecretKey->class_type, pTemplate[i].pValue, sizeof(pSecretKey->class_type));
            }
        }
    }

    if (CKR_OK == rv)
    {
        /* Use the tempkey slot id */
        pSecretKey->slot = ATCA_TEMPKEY_KEYID;
        pSecretKey->attributes = pkcs11_key_secret_attributes;
        pSecretKey->count = pkcs11_key_secret_attributes_count;
        pSecretKey->size = 32;
        pSecretKey->config = &((pkcs11_slot_ctx_ptr)pSession->slot)->cfg_zone;
        pSecretKey->flags = PKCS11_OBJECT_FLAG_DESTROYABLE | PKCS11_OBJECT_FLAG_SENSITIVE;
#ifdef ATCA_NO_HEAP
        if (!pkcs11_key_used(pkcs11_key_cache, sizeof(pkcs11_key_cache)))
        {
            pSecretKey->data = pkcs11_key_cache;
        }
#else
        pSecretKey->data = pkcs11_os_malloc(pSecretKey->size);
#endif
        if (!pSecretKey->data)
        {
            rv = CKR_HOST_MEMORY;
        }
    }

    if (CKR_OK == rv)
    {
        if (CKR_OK == (rv = pkcs11_lock_context(pLibCtx)))
        {
            ATCA_STATUS status = ATCA_SUCCESS;

            /* Because of the number of ECDH options this function unfortunately has a complex bit of logic
               to walk through to select the proper ECDH command. Normally this would be left up to the user
               to chose */

            if (ATCA_TEMPKEY_KEYID == pBaseKey->slot)
            {
                if (pSession->logged_in)
                {
                    status = atcab_ecdh_tempkey_ioenc(&pEcdhParameters->pPublicData[1], pSecretKey->data, pSession->read_key);
                }
                else
                {
                    status = atcab_ecdh_tempkey(&pEcdhParameters->pPublicData[1], pSecretKey->data);
                }
            }
            else if (16 > pBaseKey->slot)
            {
                if (ATCA_SLOT_CONFIG_WRITE_ECDH_MASK & pSession->slot->cfg_zone.SlotConfig[pBaseKey->slot])
                {
                    uint16_t read_key_id = (ATCA_SLOT_CONFIG_READKEY_MASK & pSession->slot->cfg_zone.SlotConfig[pBaseKey->slot | 0x01])
                                           >> ATCA_SLOT_CONFIG_READKEY_SHIFT;
                    status = atcab_ecdh_enc(pBaseKey->slot, &pEcdhParameters->pPublicData[1], pSecretKey->data,
                                            pSession->read_key, read_key_id, NULL);
                }
                else if ((ATECC508A != pSession->slot->interface_config.devtype) &&
                         (ATCA_CHIP_OPT_IO_PROT_EN_MASK & pSession->slot->cfg_zone.ChipOptions) &&
                         pSession->logged_in)
                {
                    status = atcab_ecdh_ioenc(pBaseKey->slot, &pEcdhParameters->pPublicData[1], pSecretKey->data, pSession->read_key);
                }
                else
                {
                    status = atcab_ecdh(pBaseKey->slot, &pEcdhParameters->pPublicData[1], pSecretKey->data);
                }
            }
            else
            {
                status = ATCA_GEN_FAIL;
            }

            (void)pkcs11_unlock_context(pLibCtx);

            rv = pkcs11_util_convert_rv(status);
        }
    }

    if (CKR_OK == rv)
    {
        pkcs11_object_get_handle(pSecretKey, phKey);
    }
    else if (pSecretKey)
    {
        pkcs11_object_free(pSecretKey);
    }

    return rv;
}

/** @} */
