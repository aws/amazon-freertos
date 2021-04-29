/**
 * \file
 * \brief PKCS11 Library Mechanism Handling
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

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_init.h"
#include "pkcs11_mech.h"
#include "pkcs11_slot.h"
#include "cryptoauthlib.h"

/**
 * \defgroup pkcs11 Mechanisms (pkcs11_mech_)
   @{ */

typedef struct _pcks11_mech_table_e
{
    CK_MECHANISM_TYPE type;
    CK_MECHANISM_INFO info;
} pcks11_mech_table_e, *pcks11_mech_table_ptr;

#define PCKS11_MECH_ECC508_EC_CAPABILITY   (CKF_EC_F_P | CKF_EC_NAMEDCURVE | CKF_EC_UNCOMPRESS)

/* CK_MECHANISM_TYPE, MinKeySize, MaxKeySize, Flags */
static pcks11_mech_table_e pkcs11_mech_list_ecc508[] = {
    //CKM_DH_PKCS_KEY_PAIR_GEN,
    //CKM_DH_PKCS_DERIVE,
    { CKM_SHA256,                                                                                                       { 256, 256, CKF_HW | CKF_DIGEST                                                                            } },
    { CKM_SHA256_HMAC,                                                                                                  { 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY                                                                 } },
    { CKM_SHA256_HMAC_GENERAL,                                                                                          { 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY                                                                 } },
    { CKM_GENERIC_SECRET_KEY_GEN,                                                                                       { 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR                                                  } },
    { CKM_CONCATENATE_BASE_AND_KEY,                                                                                     { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_CONCATENATE_BASE_AND_DATA,                                                                                    { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_CONCATENATE_DATA_AND_BASE,                                                                                    { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_XOR_BASE_AND_DATA,                                                                                            { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_EXTRACT_KEY_FROM_KEY,                                                                                         { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_SSL3_PRE_MASTER_KEY_GEN,                                                                                      { 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR                                                  } },
    { CKM_SSL3_MASTER_KEY_DERIVE,                                                                                       { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_SSL3_KEY_AND_MAC_DERIVE,                                                                                      { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_SSL3_MASTER_KEY_DERIVE_DH,                                                                                    { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_TLS_PRE_MASTER_KEY_GEN,                                                                                       { 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR                                                  } },
    { CKM_TLS_MASTER_KEY_DERIVE,                                                                                        { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_TLS_KEY_AND_MAC_DERIVE,                                                                                       { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_TLS_MASTER_KEY_DERIVE_DH,                                                                                     { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_TLS_PRF,                                                                                                      { 0,   0,   CKF_HW | CKF_DERIVE                                                                            } },
    { CKM_SHA256_KEY_DERIVATION,                                                                                        { 256, 256, CKF_HW | CKF_DERIVE                                                                            } },
    //CKM_WTLS_PRE_MASTER_KEY_GEN,
    //CKM_WTLS_MASTER_KEY_DERIVE,
    //CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC,
    //CKM_WTLS_PRF,
    //CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE,
    //CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE,
    //CKM_TLS10_MAC_SERVER,
    //CKM_TLS10_MAC_CLIENT,
    //CKM_TLS12_MAC,
    //CKM_TLS12_KDF,
    //CKM_TLS12_MASTER_KEY_DERIVE,
    //CKM_TLS12_KEY_AND_MAC_DERIVE,
    //CKM_TLS12_MASTER_KEY_DERIVE_DH,
    //CKM_TLS12_KEY_SAFE_DERIVE,
    //CKM_TLS_MAC,
    //CKM_TLS_KDF,
    //CKM_KEY_WRAP_LYNKS,
    //CKM_KEY_WRAP_SET_OAEP,
    //CKM_CMS_SIG,
    //CKM_KIP_DERIVE,
    //CKM_KIP_WRAP,
    //CKM_KIP_MAC,
    //CKM_SEED_KEY_GEN,
    //CKM_SEED_ECB,
    //CKM_SEED_CBC,
    //CKM_SEED_MAC,
    //CKM_SEED_MAC_GENERAL,
    //CKM_SEED_CBC_PAD,
    //CKM_SEED_ECB_ENCRYPT_DATA,
    //CKM_SEED_CBC_ENCRYPT_DATA,
    { CKM_EC_KEY_PAIR_GEN,                                                                                              { 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR | PCKS11_MECH_ECC508_EC_CAPABILITY               } },
    { CKM_ECDSA,                                                                                                        { 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY | PCKS11_MECH_ECC508_EC_CAPABILITY                              } },
    { CKM_ECDSA_SHA256,                                                                                                 { 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY | PCKS11_MECH_ECC508_EC_CAPABILITY                              } },
    { CKM_ECDH1_DERIVE,                                                                                                 { 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY                                         } },
    { CKM_ECDH1_COFACTOR_DERIVE,                                                                                        { 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY                                         } },
    { CKM_ECMQV_DERIVE,                                                                                                 { 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY                                         } },
    { CKM_ECDH_AES_KEY_WRAP,                                                                                            { 0,   0,   CKF_HW | CKF_WRAP | CKF_UNWRAP | PCKS11_MECH_ECC508_EC_CAPABILITY                              } },
    //CKM_FASTHASH,
    //CKM_AES_KEY_GEN,
    //CKM_AES_ECB,
    //CKM_AES_CBC,
    //CKM_AES_MAC,
    //CKM_AES_MAC_GENERAL,
    //CKM_AES_CBC_PAD,
    //CKM_AES_CTR,
    //CKM_AES_GCM,
    //CKM_AES_CCM,
    //CKM_AES_CTS,
    //CKM_AES_CMAC,
    //CKM_AES_CMAC_GENERAL,
    //CKM_AES_XCBC_MAC,
    //CKM_AES_XCBC_MAC_96,
    //CKM_AES_GMAC,
    //CKM_AES_ECB_ENCRYPT_DATA,
    //CKM_AES_CBC_ENCRYPT_DATA,
    { CKM_DH_PKCS_PARAMETER_GEN,                                                                                        { 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR                                                  } }
    //CKM_AES_OFB,
    //CKM_AES_CFB64,
    //CKM_AES_CFB8,
    //CKM_AES_CFB128,
    //CKM_AES_CFB1,
    //CKM_AES_KEY_WRAP,
    //CKM_AES_KEY_WRAP_PAD,
};

/* CK_MECHANISM_TYPE, MinKeySize, MaxKeySize, Flags */
static pcks11_mech_table_e pkcs11_mech_list_ecc608[] = {
    //CKM_DH_PKCS_KEY_PAIR_GEN,
    //CKM_DH_PKCS_DERIVE,
    { CKM_SHA256,          { 256, 256, CKF_HW | CKF_DIGEST                                                              } },
    //{ CKM_SHA256_HMAC,{ 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY } },
    //{ CKM_SHA256_HMAC_GENERAL,{ 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY } },
    //{ CKM_GENERIC_SECRET_KEY_GEN,{ 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR } },
    //{ CKM_CONCATENATE_BASE_AND_KEY,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_CONCATENATE_BASE_AND_DATA,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_CONCATENATE_DATA_AND_BASE,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_XOR_BASE_AND_DATA,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_EXTRACT_KEY_FROM_KEY,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_SSL3_PRE_MASTER_KEY_GEN,{ 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR } },
    //{ CKM_SSL3_MASTER_KEY_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_SSL3_KEY_AND_MAC_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_SSL3_MASTER_KEY_DERIVE_DH,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_TLS_PRE_MASTER_KEY_GEN,{ 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR } },
    //{ CKM_TLS_MASTER_KEY_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_TLS_KEY_AND_MAC_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_TLS_MASTER_KEY_DERIVE_DH,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_TLS_PRF,{ 0,   0,   CKF_HW | CKF_DERIVE } },
    //{ CKM_SHA256_KEY_DERIVATION,{ 256, 256, CKF_HW | CKF_DERIVE } },
    //CKM_WTLS_PRE_MASTER_KEY_GEN,
    //CKM_WTLS_MASTER_KEY_DERIVE,
    //CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC,
    //CKM_WTLS_PRF,
    //CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE,
    //CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE,
    //CKM_TLS10_MAC_SERVER,
    //CKM_TLS10_MAC_CLIENT,
    //CKM_TLS12_MAC,
    //CKM_TLS12_KDF,
    //CKM_TLS12_MASTER_KEY_DERIVE,
    //CKM_TLS12_KEY_AND_MAC_DERIVE,
    //CKM_TLS12_MASTER_KEY_DERIVE_DH,
    //CKM_TLS12_KEY_SAFE_DERIVE,
    //CKM_TLS_MAC,
    //CKM_TLS_KDF,
    //CKM_KEY_WRAP_LYNKS,
    //CKM_KEY_WRAP_SET_OAEP,
    //CKM_CMS_SIG,
    //CKM_KIP_DERIVE,
    //CKM_KIP_WRAP,
    //CKM_KIP_MAC,
    //CKM_SEED_KEY_GEN,
    //CKM_SEED_ECB,
    //CKM_SEED_CBC,
    //CKM_SEED_MAC,
    //CKM_SEED_MAC_GENERAL,
    //CKM_SEED_CBC_PAD,
    //CKM_SEED_ECB_ENCRYPT_DATA,
    //CKM_SEED_CBC_ENCRYPT_DATA,
    { CKM_EC_KEY_PAIR_GEN, { 256, 256, CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    { CKM_ECDSA,           { 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY | PCKS11_MECH_ECC508_EC_CAPABILITY                } },
    //{ CKM_ECDSA_SHA256,{ 256, 256, CKF_HW | CKF_SIGN | CKF_VERIFY | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    //{ CKM_ECDH1_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    //{ CKM_ECDH1_COFACTOR_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    //{ CKM_ECMQV_DERIVE,{ 0,   0,   CKF_HW | CKF_DERIVE | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    //{ CKM_ECDH_AES_KEY_WRAP,{ 0,   0,   CKF_HW | CKF_WRAP | CKF_UNWRAP | PCKS11_MECH_ECC508_EC_CAPABILITY } },
    //CKM_FASTHASH,
    //CKM_AES_KEY_GEN,
    //CKM_AES_ECB,
    //CKM_AES_CBC,
    //CKM_AES_MAC,
    //CKM_AES_MAC_GENERAL,
    //CKM_AES_CBC_PAD,
    //CKM_AES_CTR,
    //CKM_AES_GCM,
    //CKM_AES_CCM,
    //CKM_AES_CTS,
    //CKM_AES_CMAC,
    //CKM_AES_CMAC_GENERAL,
    //CKM_AES_XCBC_MAC,
    //CKM_AES_XCBC_MAC_96,
    //CKM_AES_GMAC,
    //CKM_AES_ECB_ENCRYPT_DATA,
    //CKM_AES_CBC_ENCRYPT_DATA,
    //{ CKM_DH_PKCS_PARAMETER_GEN,{ 0,   0,   CKF_HW | CKF_GENERATE | CKF_GENERATE_KEY_PAIR } }
    //CKM_AES_OFB,
    //CKM_AES_CFB64,
    //CKM_AES_CFB8,
    //CKM_AES_CFB128,
    //CKM_AES_CFB1,
    //CKM_AES_KEY_WRAP,
    //CKM_AES_KEY_WRAP_PAD,
};

#define TABLE_SIZE(x)   sizeof(x) / sizeof(x[0])

static CK_MECHANISM_INFO_PTR pkcs11_mech_find_info(pcks11_mech_table_ptr pMechanismInfoList, CK_ULONG count, CK_MECHANISM_TYPE type)
{
    CK_MECHANISM_INFO_PTR rv = NULL_PTR;

    if (pMechanismInfoList)
    {
        CK_ULONG i;
        for (i = 0; i < count; i++)
        {
            if (type == pMechanismInfoList[i].type)
            {
                rv = &pMechanismInfoList[i].info;
                break;
            }
        }
    }
    return rv;
}

static void pkcs11_mech_fill_list(CK_MECHANISM_TYPE_PTR pMechanismList, pcks11_mech_table_ptr pMechanismInfoList, CK_ULONG count)
{
    if (pMechanismList && pMechanismInfoList)
    {
        CK_ULONG i;
        for (i = 0; i < count; i++)
        {
            pMechanismList[i] = pMechanismInfoList[i].type;
        }
    }
}

CK_RV pkcs11_mech_get_list(CK_SLOT_ID slotID, CK_MECHANISM_TYPE_PTR pMechanismList, CK_ULONG_PTR pulCount)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    pcks11_mech_table_ptr mech_list_ptr = NULL_PTR;
    CK_ULONG mech_cnt = 0;

    if (!lib_ctx)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    /* Ref PKCS11 Sec 5.5 - C_GetMechanismList */
    if (!pulCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Retreive the slot context - i.e. the attached device (ECC508A, SHA256, etc) */
    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    switch (slot_ctx->interface_config.devtype)
    {
    case ATECC508A:
        mech_cnt = TABLE_SIZE(pkcs11_mech_list_ecc508);
        mech_list_ptr = pkcs11_mech_list_ecc508;
        break;
    case ATECC608:
        mech_cnt = TABLE_SIZE(pkcs11_mech_list_ecc608);
        mech_list_ptr = pkcs11_mech_list_ecc608;
        break;
    default:
        break;
    }

    /* Ref PKCS11 Sec 5.5 - C_GetMechanismList Requirement #2 */
    if (pMechanismList)
    {
        if (mech_cnt > *pulCount)
        {
            *pulCount = mech_cnt;
            return CKR_BUFFER_TOO_SMALL;
        }
        else
        {
            pkcs11_mech_fill_list(pMechanismList, mech_list_ptr, mech_cnt);
        }
    }

    *pulCount = mech_cnt;

    return CKR_OK;
}

CK_RV pkcs_mech_get_info(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    CK_MECHANISM_INFO_PTR mech_info_ptr = NULL_PTR;


    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pInfo)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Retreive the slot context - i.e. the attached device (ECC508A, SHA256, etc) */
    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    switch (slot_ctx->interface_config.devtype)
    {
    case ATECC508A:
        mech_info_ptr = pkcs11_mech_find_info(pkcs11_mech_list_ecc508, TABLE_SIZE(pkcs11_mech_list_ecc508), type);
        break;
    case ATECC608:
        mech_info_ptr = pkcs11_mech_find_info(pkcs11_mech_list_ecc608, TABLE_SIZE(pkcs11_mech_list_ecc608), type);
        break;
    default:
        break;
    }

    if (!mech_info_ptr)
    {
        return CKR_MECHANISM_INVALID;
    }

    memcpy(pInfo, mech_info_ptr, sizeof(CK_MECHANISM_INFO));

    return CKR_OK;
}

/** @} */

