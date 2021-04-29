/**
 * \file
 * \brief PKCS11 Library Session Handling
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
#include "host/atca_host.h"

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_session.h"
#include "pkcs11_token.h"
#include "pkcs11_init.h"
#include "pkcs11_slot.h"
#include "pkcs11_object.h"
#include "pkcs11_os.h"
#include "pkcs11_util.h"

/**
 * \defgroup pkcs11 Session Management (pkcs11_)
   @{ */

#ifdef ATCA_NO_HEAP
static pkcs11_session_ctx pkcs11_session_cache[PKCS11_MAX_SESSIONS_ALLOWED];
#elif UINTPTR_MAX == 0xffffffffffffffff
static pkcs11_session_ctx_ptr pkcs11_session_cache[PKCS11_MAX_SESSIONS_ALLOWED];
#endif

static pkcs11_session_ctx_ptr pkcs11_allocate_session_context(void)
{
    pkcs11_session_ctx_ptr rv = NULL_PTR;

#ifdef ATCA_NO_HEAP
    CK_ULONG i;
    for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
    {

        if (!pkcs11_session_cache[i].initialized)
        {
            rv = &pkcs11_session_cache[i];
            break;
        }
    }
#elif UINTPTR_MAX == 0xffffffffffffffff
    CK_ULONG i;
    for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
    {
        if (!pkcs11_session_cache[i])
        {
            /* Use dynamic memory assignement from OS abstraction layer */
            rv = pkcs11_os_malloc(sizeof(pkcs11_session_ctx));
            pkcs11_session_cache[i] = rv;
            break;
        }
    }
#else
    rv = pkcs11_os_malloc(sizeof(pkcs11_session_ctx));
#endif

    return rv;
}

pkcs11_session_ctx_ptr pkcs11_get_session_context(CK_SESSION_HANDLE hSession)
{
#if UINTPTR_MAX == 0xffffffffffffffff
    pkcs11_session_ctx_ptr rv = NULL_PTR;
    CK_ULONG i;
    for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
    {
#ifdef ATCA_NO_HEAP
        if (hSession == pkcs11_session_cache[i].handle)
        {
            rv = &pkcs11_session_cache[i];
            break;
        }
#else
        if (pkcs11_session_cache[i])
        {
            if (hSession == pkcs11_session_cache[i]->handle)
            {
                rv = pkcs11_session_cache[i];
                break;
            }
        }
#endif
    }
    return rv;
#else
    return (pkcs11_session_ctx_ptr)hSession;
#endif
}

static CK_RV pkcs11_session_free_session_context(pkcs11_session_ctx_ptr session_ctx)
{
    CK_RV rv = CKR_ARGUMENTS_BAD;

    if (session_ctx)
    {
        (void)pkcs11_util_memset(session_ctx, sizeof(pkcs11_session_ctx), 0, sizeof(pkcs11_session_ctx));
#ifndef ATCA_NO_HEAP
#if UINTPTR_MAX == 0xffffffffffffffff
        CK_ULONG i;
        for (i = 0; i < PKCS11_MAX_SESSIONS_ALLOWED; i++)
        {
            if (session_ctx == pkcs11_session_cache[i])
            {
                pkcs11_session_cache[i] = NULL;
                break;
            }
        }
#endif
        pkcs11_os_free(session_ctx);
#endif
        rv = CKR_OK;
    }
    return rv;
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
    (void)pkcs11_session_free_session_context(session_ctx);

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
#ifdef ATCA_NO_HEAP
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
    pkcs11_lib_ctx_ptr pLibCtx = pkcs11_get_context();
    pkcs11_session_ctx_ptr session_ctx = pkcs11_get_session_context(hSession);
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];
    CK_RV rv;

    if (!pLibCtx || !pLibCtx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pPin || !ulPinLen)
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

    if (64 != ulPinLen)
    {
        if (CKR_OK == (rv = pkcs11_lock_context(pLibCtx)))
        {
            rv = pkcs11_util_convert_rv(atcab_read_serial_number(sn));
            (void)pkcs11_unlock_context(pLibCtx);
        }

        if (CKR_OK == rv)
        {
            rv = pkcs11_token_convert_pin_to_key(pPin, ulPinLen, sn, (CK_LONG)sizeof(sn),
                                                 session_ctx->read_key, (CK_LONG)sizeof(session_ctx->read_key));
        }
    }
    else
    {
        rv = pkcs11_token_convert_pin_to_key(pPin, ulPinLen, NULL, 0, session_ctx->read_key,
                                             (CK_LONG)sizeof(session_ctx->read_key));
    }

    if (CKR_OK == rv)
    {
        session_ctx->logged_in = TRUE;
    }

    return rv;
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
    (void)pkcs11_util_memset(session_ctx->read_key, sizeof(session_ctx->read_key), 0, sizeof(session_ctx->read_key));

    session_ctx->logged_in = FALSE;

    return CKR_OK;
}

#if 0
/* Authorize an object for use - Legacy function @todo rework with new access model */
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
#endif

/** @} */
