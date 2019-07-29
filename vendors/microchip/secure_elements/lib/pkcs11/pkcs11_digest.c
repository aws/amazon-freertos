
#include "pkcs11_init.h"
#include "pkcs11_digest.h"
#include "pkcs11_object.h"
#include "pkcs11_session.h"
#include "pkcs11_util.h"
#include "cryptoauthlib.h"

#if !PKCS11_HARDWARE_SHA256
#include "crypto/atca_crypto_sw_sha2.h"
static atcac_sha2_256_ctx pkcs11_digest_ctx;
static bool pkcs11_digest_initalized;
#endif

/**
 * \brief Initializes a message-digesting operation using the specified mechanism in the specified session
 */
CK_RV pkcs11_digest_init(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism)
{
    pkcs11_session_ctx_ptr pSession;
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

    if (CKM_SHA256 != pMechanism->mechanism)
    {
        return CKR_MECHANISM_INVALID;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }
#if PKCS11_HARDWARE_SHA256
    return CKR_FUNCTION_NOT_SUPPORTED;
#else
    rv = atcac_sw_sha2_256_init(&pkcs11_digest_ctx);

    if (ATCA_SUCCESS == rv)
    {
        pkcs11_digest_initalized = TRUE;
    }

    return pkcs11_util_convert_rv(rv);
#endif
}

/**
 * \brief Digest the specified data in a one-pass operation and return the resulting digest
 */
CK_RV pkcs11_digest(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen)
{
    pkcs11_session_ctx_ptr pSession;
    CK_RV rv;

    rv = pkcs11_init_check(NULL_PTR, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pData || !ulDataLen || !pulDigestLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!pDigest)
    {
        *pulDigestLen = ATCA_SHA2_256_DIGEST_SIZE;
        return CKR_OK;
    }
    else if (ATCA_SHA2_256_DIGEST_SIZE > *pulDigestLen)
    {
        return CKR_BUFFER_TOO_SMALL;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

#if PKCS11_HARDWARE_SHA256
    ((void)hSession);
    ((void)pData);
    ((void)ulDataLen);
    ((void)pDigest);
    ((void)pulDigestLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
#else
    if (!pkcs11_digest_initalized)
    {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    rv = atcac_sw_sha2_256(pData, ulDataLen, pDigest);

    pkcs11_digest_initalized = FALSE;

    return pkcs11_util_convert_rv(rv);
#endif
}

/**
 * \brief Continues a multiple-part digesting operation
 */
CK_RV pkcs11_digest_update(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)
{
    pkcs11_session_ctx_ptr pSession;
    CK_RV rv;

    rv = pkcs11_init_check(NULL_PTR, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pPart || !ulPartLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

#if PKCS11_HARDWARE_SHA256
    ((void)hSession);
    ((void)pPart);
    ((void)ulPartLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
#else
    if (!pkcs11_digest_initalized)
    {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    rv = atcac_sw_sha2_256_update(&pkcs11_digest_ctx, pPart, ulPartLen);

    return pkcs11_util_convert_rv(rv);
#endif

}

/**
 * \brief Finishes a multiple-part digesting operation
 */
CK_RV pkcs11_digest_final(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen)
{
    pkcs11_session_ctx_ptr pSession;
    CK_RV rv;

    rv = pkcs11_init_check(NULL_PTR, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pulDigestLen)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!pDigest)
    {
        *pulDigestLen = ATCA_SHA2_256_DIGEST_SIZE;
        return CKR_OK;
    }
    else if (ATCA_SHA2_256_DIGEST_SIZE > *pulDigestLen)
    {
        return CKR_BUFFER_TOO_SMALL;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

#if PKCS11_HARDWARE_SHA256
    ((void)hSession);
    ((void)pDigest);
    ((void)pulDigestLen);

    return CKR_FUNCTION_NOT_SUPPORTED;
#else
    if (!pkcs11_digest_initalized)
    {
        return CKR_OPERATION_NOT_INITIALIZED;
    }

    rv = atcac_sw_sha2_256_finish(&pkcs11_digest_ctx, pDigest);

    pkcs11_digest_initalized = FALSE;

    return pkcs11_util_convert_rv(rv);
#endif
}
