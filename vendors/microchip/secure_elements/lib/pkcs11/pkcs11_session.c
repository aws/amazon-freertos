/**
 * \file
 * \brief PKCS11 Library Session Handling
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

#include "cryptoauthlib.h"
#include "host/atca_host.h"

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_session.h"
#include "pkcs11_init.h"
#include "pkcs11_slot.h"
#include "pkcs11_object.h"

/**
 * \defgroup pkcs11 Session Management (pkcs11_)
   @{ */

#ifndef memset_s
int memset_s(void *dest, size_t destsz, int ch, size_t count)
{
    if (dest == NULL)
    {
        return -1;
    }
    if (destsz > SIZE_MAX)
    {
        return -1;
    }
    if (count > destsz)
    {
        return -1;
    }

    volatile unsigned char *p = dest;
    while (destsz-- && count--)
    {
        *p++ = ch;
    }

    return 0;
}
#endif


#if PKCS11_USE_STATIC_MEMORY
static pkcs11_session_ctx pkcs11_session_cache[PKCS11_MAX_SESSIONS_ALLOWED];
#endif

static pkcs11_session_ctx_ptr pkcs11_allocate_session_context(void)
{
    pkcs11_session_ctx_ptr rv = NULL_PTR;

#if PKCS11_USE_STATIC_MEMORY
    CK_ULONG i;
    for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
    {
        if (!pkcs11_session_cache[i].initialized)
        {
            rv = &pkcs11_session_cache[i];
            break;
        }
    }
#else
    /* Use dynamic memory assignement from OS abstraction layer */
#endif
    return rv;
}

pkcs11_session_ctx_ptr pkcs11_get_session_context(CK_SESSION_HANDLE hSession)
{
#if defined(__x86_64__) || defined(_WIN64)
    pkcs11_session_ctx_ptr rv = NULL_PTR;
    CK_ULONG i;
    for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
    {
        if (hSession == pkcs11_session_cache[i].handle)
        {
            rv = &pkcs11_session_cache[i];
            break;
        }
    }
    return rv;
#else
    return (pkcs11_session_ctx_ptr)hSession;
#endif
}

/**
 * \brief Check if the session is initialized properly
 */
CK_RV pkcs11_session_check(pkcs11_session_ctx_ptr * pSession, CK_SESSION_HANDLE hSession)
{
    pkcs11_session_ctx_ptr ctx = pkcs11_get_session_context(hSession);
    CK_RV rv = CKR_SESSION_HANDLE_INVALID;

    if (ctx)
    {
        if (ctx->initialized)
        {
            rv = CKR_OK;
        }
        else
        {
            rv = CKR_SESSION_CLOSED;
        }
    }

    if (pSession)
    {
        if (rv)
        {
            *pSession = NULL_PTR;
        }
        else
        {
            *pSession = ctx;
        }
    }
    return rv;
}

CK_RV pkcs11_session_open(
    CK_SLOT_ID            slotID,
    CK_FLAGS              flags,
    CK_VOID_PTR           pApplication,
    CK_NOTIFY             notify,
    CK_SESSION_HANDLE_PTR phSession
    )
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;
    pkcs11_session_ctx_ptr session_ctx;

    ((void)notify);
    ((void)pApplication);

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!phSession)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* See PKCS11 Sec 5.6 - Legacy reaons */
    if (!(flags & CKF_SERIAL_SESSION))
    {
        return CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }

    /* Retrieve the slot context - i.e. the attached device (ECC508A, SHA256, etc) */
    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    if (!slot_ctx->initialized)
    {
        return CKR_TOKEN_NOT_RECOGNIZED;
    }

    /* See PKCS11 Sec 5.6 - if token_info returned write protected r/w is disallowed */
    //if (flags & CKF_RW_SESSION)
    //{
    //    return CKR_TOKEN_WRITE_PROTECTED;
    //}

    /* Get a new session context */
    session_ctx = pkcs11_allocate_session_context();

    /* Check that a session was created */
    if (!session_ctx)
    {
        return CKR_HOST_MEMORY;
    }

    /* Initialize the session */
    session_ctx->slot = slot_ctx;
    session_ctx->initialized = TRUE;

    /* Assign the session handle */
    session_ctx->handle = (CK_SESSION_HANDLE)session_ctx;

    *phSession = session_ctx->handle;

    return CKR_OK;
}

CK_RV pkcs11_session_close(CK_SESSION_HANDLE hSession)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_session_ctx_ptr session_ctx = pkcs11_get_session_context(hSession);
    pkcs11_slot_ctx_ptr slot_ctx;

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!session_ctx)
    {
        return CKR_SESSION_HANDLE_INVALID;
    }

    if (!session_ctx->initialized)
    {
        return CKR_SESSION_CLOSED;
    }

    /* Get the slot */
    slot_ctx = (pkcs11_slot_ctx_ptr)session_ctx->slot;

    if (slot_ctx)
    {
        /* Do whatever we need to shutdown the session with the device */
    }
    else
    {
        /* We should go looking for the right slot since something got messed up
           that would be a pkcs11_slot_* function to find a slot given a session */
    }

    /* Free the session */
    memset_s(session_ctx, sizeof(pkcs11_session_ctx), 0, sizeof(pkcs11_session_ctx));

    return CKR_OK;
}

/**
 * \brief Close all sessions for a given slot - not actually all open sessions
 */
CK_RV pkcs11_session_closeall(CK_SLOT_ID slotID)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_slot_ctx_ptr slot_ctx;

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    slot_ctx = pkcs11_slot_get_context(lib_ctx, slotID);

    if (!slot_ctx)
    {
        return CKR_SLOT_ID_INVALID;
    }

    /* If there were the ability to have multiple sessions open for a slot then
       we'd loop over the sessions and close them in order*/
#if PKCS11_USE_STATIC_MEMORY
    {
        int i;
        for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
        {
            pkcs11_session_close(pkcs11_session_cache[i].handle);
        }
    }
#endif

    return CKR_OK;
}

/**
 * \brief Obtains information about a particular session
 */
CK_RV pkcs11_session_get_info(CK_SESSION_HANDLE hSession, CK_SESSION_INFO_PTR pInfo)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_session_ctx_ptr session_ctx = pkcs11_get_session_context(hSession);
    pkcs11_slot_ctx_ptr slot_ctx;

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pInfo)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!session_ctx)
    {
        return CKR_SESSION_HANDLE_INVALID;
    }

    if (!session_ctx->initialized)
    {
        return CKR_SESSION_CLOSED;
    }

    pInfo->state = session_ctx->state;
    pInfo->ulDeviceError = session_ctx->error;

    slot_ctx = session_ctx->slot;
    if (slot_ctx)
    {
        pInfo->slotID = slot_ctx->slot_id;
    }

    pInfo->flags = CKF_RW_SESSION | CKF_SERIAL_SESSION;

    return CKR_OK;
}

CK_RV pkcs11_session_login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_session_ctx_ptr session_ctx = pkcs11_get_session_context(hSession);
    size_t outlen;

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pPin || ulPinLen != 64)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!session_ctx)
    {
        return CKR_SESSION_HANDLE_INVALID;
    }

    if (!session_ctx->initialized)
    {
        return CKR_SESSION_CLOSED;
    }

    /* Decode the hex string pin into a binary io protection key */
    outlen = sizeof(session_ctx->read_key);
    (void)atcab_hex2bin((const char*)pPin, ulPinLen, session_ctx->read_key, &outlen);

    return CKR_OK;
}

CK_RV pkcs11_session_logout(CK_SESSION_HANDLE hSession)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    pkcs11_session_ctx_ptr session_ctx = pkcs11_get_session_context(hSession);

    if (!lib_ctx || !lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!session_ctx)
    {
        return CKR_SESSION_HANDLE_INVALID;
    }

    if (!session_ctx->initialized)
    {
        return CKR_SESSION_CLOSED;
    }

    /* Wipe the io protection secret */
    memset_s(session_ctx->read_key, sizeof(session_ctx->read_key), 0, sizeof(session_ctx->read_key));

    return CKR_OK;
}

/* Authorize an object for use */
CK_RV pkcs11_session_authorize(pkcs11_session_ctx_ptr pSession, CK_VOID_PTR pObject)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;
    atecc508a_config_t * pConfig;

    ATCA_STATUS status;
    uint8_t response[MAC_SIZE];
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];
    uint16_t key_id = PKCS11_PIN_SLOT;
    atca_check_mac_in_out_t checkmac_params;
    atca_temp_key_t temp_key;
    atca_nonce_in_out_t nonce_params;
    uint8_t num_in[NONCE_NUMIN_SIZE];
    uint8_t rand_out[RANDOM_NUM_SIZE];
    uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];

    if (!pSession || !obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

//    pConfig = (atecc508a_config_t *)obj_ptr->config;
//
//    if(!pConfig)
//    {
//        return CKR_GENERAL_ERROR;
//    }

//    if (ATCA_KEY_CONFIG_REQ_AUTH_MASK & pConfig->KeyConfig[obj_ptr->slot])
//    {
//        key_id = (pConfig->KeyConfig[obj_ptr->slot] &
//                ATCA_KEY_CONFIG_AUTH_KEY_MASK) >> ATCA_KEY_CONFIG_AUTH_KEY_SHIFT;
//    }
//    else
//    {
//        /* No Authorization is required */
//        return CKR_OK;
//    }

    /* Initialize the intermediate buffers */
    memset(&temp_key, 0, sizeof(temp_key));
    memset(&nonce_params, 0, sizeof(nonce_params));
    memset(num_in, 0, sizeof(num_in));
    memset(other_data, 0, sizeof(other_data));

    /* Read Device Serial Number */
    status = atcab_read_serial_number(sn);

    if (ATCA_SUCCESS == status)
    {
        /* Perform random nonce and store it in tempkey */
        nonce_params.mode = NONCE_MODE_SEED_UPDATE;
        nonce_params.zero = 0;
        nonce_params.num_in = num_in;
        nonce_params.rand_out = rand_out;
        nonce_params.temp_key = &temp_key;
        status = atcab_nonce_rand(nonce_params.num_in, rand_out);
    }

    if (ATCA_SUCCESS == status)
    {
        /* Calculate nonce value of tempkey locally */
        status = atcah_nonce(&nonce_params);
    }

    if (ATCA_SUCCESS == status)
    {
        /* Calculate the expected checkmac answer the host will provide */
        other_data[0] = ATCA_MAC;
        other_data[2] = (uint8_t)key_id;

        checkmac_params.mode = CHECKMAC_MODE_BLOCK2_TEMPKEY;
        checkmac_params.key_id = key_id;
        checkmac_params.client_chal = NULL;
        checkmac_params.client_resp = response;
        checkmac_params.other_data = other_data;
        checkmac_params.sn = sn;
        checkmac_params.otp = NULL;
        checkmac_params.slot_key = pSession->read_key;
        checkmac_params.target_key = NULL;
        checkmac_params.temp_key = &temp_key;
        status = atcah_check_mac(&checkmac_params);
    }

    if (ATCA_SUCCESS == status)
    {
        /* Perform CheckMac which will compare the host provided
         * mac against the internally computed one */
        status = atcab_checkmac(
            checkmac_params.mode,
            checkmac_params.key_id,
            checkmac_params.client_chal,
            checkmac_params.client_resp,
            checkmac_params.other_data);
    }

    if (ATCA_SUCCESS == status)
    {
        return CKR_OK;
    }
    else
    {
        return CKR_PIN_INCORRECT;
    }
}

/** @} */
