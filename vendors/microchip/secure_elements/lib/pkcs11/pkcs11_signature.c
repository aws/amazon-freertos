/**
 * \file
 * \brief PKCS11 Library Sign/Verify Handling
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
#include "pkcs11_signature.h"
#include "pkcs11_object.h"
#include "pkcs11_session.h"
#include "pkcs11_util.h"
#include "cryptoauthlib.h"

#include "atcacert/atcacert_der.h"

/**
 * \defgroup pkcs11 Signature (pkcs11_signature_)
   @{ */

/**
 * \brief Initialize a signing operation using the specified key and mechanism
 */
CK_RV pkcs11_signature_sign_init(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pObject;
    CK_RV rv;

    rv = pkcs11_init_check(NULL_PTR, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pMechanism)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pObject, hKey);
    if (rv)
    {
        return rv;
    }

    pSession->active_object = hKey;

    return CKR_OK;
}

/**
 * \brief Sign the data in a single pass operation
 */
CK_RV pkcs11_signature_sign(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{
    pkcs11_lib_ctx_ptr pLibCtx = NULL;
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pKey;
    CK_RV rv;
    ATCA_STATUS status;

    ((void)ulDataLen);

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pKey, pSession->active_object);
    if (rv)
    {
        return rv;
    }

    /* Check parameters */
    if (pulSignatureLen)
    {
        if (pSignature)
        {
            if (CKR_OK != (rv = pkcs11_lock_context(pLibCtx)))
            {
                return rv;
            }
            status = atcab_sign(pKey->slot, pData, pSignature);
            (void)pkcs11_unlock_context(pLibCtx);
            if (status)
            {
                PKCS11_DEBUG("atcab_sign: %02X\n", status);
                return CKR_FUNCTION_FAILED;
            }
        }
        *pulSignatureLen = ATCA_SIG_SIZE;
    }
    else
    {
        return CKR_ARGUMENTS_BAD;
    }

    return CKR_OK;
}

/**
 * \brief Continues a multiple-part signature operation
 */
CK_RV pkcs11_signature_sign_continue(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
    ((void)hSession);
    ((void)pPart);
    ((void)ulPartLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * \brief Finishes a multiple-part signature operation
 */
CK_RV pkcs11_signature_sign_finish(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)
{
    ((void)hSession);
    ((void)pSignature);
    ((void)pulSignatureLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * \brief Initializes a verification operation using the specified key and mechanism
 */
CK_RV pkcs11_signature_verify_init(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)
{
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pObject;
    CK_RV rv;

    rv = pkcs11_init_check(NULL_PTR, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pMechanism)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pObject, hKey);
    if (rv)
    {
        return rv;
    }

    pSession->active_object = hKey;

    return CKR_OK;
}

/**
 * \brief Verifies a signature on single-part data
 */
CK_RV pkcs11_signature_verify(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{
    pkcs11_lib_ctx_ptr pLibCtx = NULL;
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pKey;
    atecc508a_config_t * pConfig;
    ATCA_STATUS status;
    bool verified = FALSE;
    CK_RV rv;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pKey, pSession->active_object);
    if (rv)
    {
        return rv;
    }

    /* Check parameters */
    if (!pData || ulDataLen != ATCA_SHA_DIGEST_SIZE || !pSignature || ulSignatureLen != VERIFY_256_SIGNATURE_SIZE)
    {
        return CKR_ARGUMENTS_BAD;
    }

    pConfig = (atecc508a_config_t*)pKey->config;
    if (!pConfig)
    {
        return CKR_GENERAL_ERROR;
    }

    if (CKR_OK != (rv = pkcs11_lock_context(pLibCtx)))
    {
        return rv;
    }

    if (ATCA_KEY_CONFIG_PRIVATE_MASK & pConfig->KeyConfig[pKey->slot])
    {
        /* Device can't verify against a private key so ask the device for
           the public key first then perform an external verify */
        uint8_t pub_key[ATCA_PUB_KEY_SIZE];

        status = atcab_get_pubkey(pKey->slot, pub_key);
        if (ATCA_SUCCESS == status)
        {
            status = atcab_verify_extern(pData, pSignature, pub_key, &verified);
        }
    }
    else
    {
        /* Assume Public Key has been stored properly and verify against
           whatever is stored */
        status = atcab_verify_stored(pData, pSignature, pKey->slot, &verified);
    }

    (void)pkcs11_unlock_context(pLibCtx);

    if (ATCA_SUCCESS == status)
    {
        rv = verified ? CKR_OK : CKR_SIGNATURE_INVALID;
    }
    else
    {
        rv = CKR_DEVICE_ERROR;
    }

    return rv;
}

/**
 * \brief Continues a multiple-part verification operation
 */
CK_RV pkcs11_signature_verify_continue(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
    ((void)hSession);
    ((void)pPart);
    ((void)ulPartLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
}

/**
 * \brief Finishes a multiple-part verification operation
 */
CK_RV pkcs11_signature_verify_finish(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)
{
    ((void)hSession);
    ((void)pSignature);
    ((void)ulSignatureLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
}


/** @} */

