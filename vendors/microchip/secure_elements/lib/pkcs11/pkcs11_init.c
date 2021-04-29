/**
 * \file
 * \brief PKCS11 Library Init/Deinit
 *
 * Copyright (c) 2017 Microchip Technology Inc. All rights reserved.
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
#include "pkcs11_os.h"
#include "pkcs11_slot.h"
#include "pkcs11_object.h"
#include "pkcs11_session.h"
#include "cryptoauthlib.h"

#ifdef CreateMutex
#undef CreateMutex /* CreateMutex is defined to CreateMutexW in synchapi.h in Windows. */
#endif

/**
 * \defgroup pkcs11 Initialization (pkcs11_)
   @{ */

/** Library intialization defaults if none were provided */
static const CK_C_INITIALIZE_ARGS pkcs11_init_defaults = {
    NULL_PTR,   /**< Callback to create a mutex */
    NULL_PTR,   /**< Callback to destroy a mutex */
    NULL_PTR,   /**< Callback to lock a mutex */
    NULL_PTR,   /**< Callback to unlock a mutex */
    0,          /**< Initialization Flags  */
    NULL_PTR,   /**< Reserved - Must be NULL */
};

/**
 * \brief Library Context - currently static but could be allocated
 */
static pkcs11_lib_ctx pkcs11_context;

/**
 * \brief Retrieve the current library context
 */
pkcs11_lib_ctx_ptr pkcs11_get_context(void)
{
    return &pkcs11_context;
}

CK_RV pkcs11_lock_context(pkcs11_lib_ctx_ptr pContext)
{
    CK_RV rv = CKR_ARGUMENTS_BAD;

//    PKCS11_DEBUG("%p\r\n", pkcs11_context.lock_mutex);

    if (pContext)
    {
        if (pContext->lock_mutex)
        {
            rv = pContext->lock_mutex(pContext->mutex);
        }
        else
        {
            rv = CKR_CANT_LOCK;
        }
    }
    return rv;
}

CK_RV pkcs11_unlock_context(pkcs11_lib_ctx_ptr pContext)
{
    CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;

//    PKCS11_DEBUG("%p\r\n", pkcs11_context.unlock_mutex);

    if (!pContext)
    {
        pContext = pkcs11_get_context();
    }

    if (pContext)
    {
        if (pContext->unlock_mutex)
        {
            rv = pContext->unlock_mutex(pContext->mutex);
        }
        else
        {
            rv = CKR_CANT_LOCK;
        }
    }

    return rv;
}

/**
 * \brief Check if the library is initialized properly
 */
CK_RV pkcs11_init_check(pkcs11_lib_ctx_ptr * ppContext, CK_BBOOL lock)
{
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    CK_RV rv = CKR_CRYPTOKI_NOT_INITIALIZED;

    if (ppContext)
    {
        *ppContext = NULL_PTR;
    }

    if (lib_ctx)
    {
        if (lib_ctx->initialized)
        {
            if (ppContext)
            {
                *ppContext = lib_ctx;
            }

            if (lock)
            {
                rv = pkcs11_lock_context(lib_ctx);
            }
            else
            {
                rv = CKR_OK;
            }
        }
    }
    return rv;
}

/**
 * \brief Initializes the PKCS11 API Library for Cryptoauthlib
 */
CK_RV pkcs11_init(CK_C_INITIALIZE_ARGS_PTR pInitArgs)
{
    CK_BBOOL allset = FALSE;
    CK_BBOOL allunset = FALSE;
    pkcs11_lib_ctx_ptr lib_ctx = pkcs11_get_context();
    CK_RV rv = CKR_OK;

    if (!pInitArgs)
    {
        pInitArgs = (CK_C_INITIALIZE_ARGS_PTR)&pkcs11_init_defaults;
    }

    if (!lib_ctx)
    {
        /** \todo This is where we should allocate a new context if we're using dynamic memory */
        /** \todo If we're using dyamic memory we need to make sure to deallocate it if any of the errors after the allocations are encountered */
        return CKR_GENERAL_ERROR;
    }

    if (lib_ctx->initialized)
    {
        return CKR_CRYPTOKI_ALREADY_INITIALIZED;
    }

    allset = (pInitArgs->CreateMutex && pInitArgs->DestroyMutex && pInitArgs->LockMutex && pInitArgs->UnlockMutex);
    allunset = (!pInitArgs->CreateMutex && !pInitArgs->DestroyMutex && !pInitArgs->LockMutex && !pInitArgs->UnlockMutex);

    /* PKCS11 Sec 5.4 - All must be set or unset - No mixing  */
    if ((!allset && !allunset) || pInitArgs->pReserved)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* PKCS11 Sec 5.4 Rules 2 & 3 */
    if (allset || (CKF_OS_LOCKING_OK & pInitArgs->flags))
    {
        /* Calling system plans to make calls to the library from multiple threads */
        if (allset)
        {
            /* Use the provided callbacks to perform locking operations */
            lib_ctx->create_mutex = pInitArgs->CreateMutex;
            lib_ctx->destroy_mutex = pInitArgs->DestroyMutex;
            lib_ctx->lock_mutex = pInitArgs->LockMutex;
            lib_ctx->unlock_mutex = pInitArgs->UnlockMutex;
        }
        else
        {
            /* Means we need to use native calls */
            lib_ctx->create_mutex = pkcs11_os_create_mutex;
            lib_ctx->destroy_mutex = pkcs11_os_destroy_mutex;
            lib_ctx->lock_mutex = pkcs11_os_lock_mutex;
            lib_ctx->unlock_mutex = pkcs11_os_unlock_mutex;
        }
    }

    /* Only need to check if our library needs to create threads */
    if (CKF_LIBRARY_CANT_CREATE_OS_THREADS & pInitArgs->flags)
    {
        /* If we can't operate successfully without creating threads we'd respond: */
        // return CKR_NEED_TO_CREATE_THREADS;
    }

    /* Perform library initialization steps */
    if (lib_ctx->create_mutex)
    {
        if (lib_ctx->create_mutex(&lib_ctx->mutex))
        {
            PKCS11_DEBUG("Create Failed\r\n");
            return CKR_CANT_LOCK;
        }
    }

    /* Lock the library mutex */
    if (lib_ctx->lock_mutex)
    {
        if (lib_ctx->lock_mutex(lib_ctx->mutex))
        {
            PKCS11_DEBUG("Lock Failed\r\n");
            return CKR_CANT_LOCK;
        }
    }

    /* Initialize the Crypto device */
    lib_ctx->slots = pkcs11_slot_initslots(PKCS11_MAX_SLOTS_ALLOWED);
    if (lib_ctx->slots)
    {
        lib_ctx->slot_cnt = PKCS11_MAX_SLOTS_ALLOWED;
    }

    /* Set up a slot with a configuration */
    rv = pkcs11_slot_config(0);

    if (CKR_OK == rv)
    {
        /* Attempt to Initialize the slot */
        rv = pkcs11_slot_init(0);
    }

    if (CKR_OK == rv)
    {
        lib_ctx->initialized = TRUE;
    }

    /* UnLock the library mutex */
    if (lib_ctx->unlock_mutex)
    {
        if (lib_ctx->unlock_mutex(lib_ctx->mutex))
        {
            return CKR_CANT_LOCK;
        }
    }

    return rv;
}

/* Close the library */
CK_RV pkcs11_deinit(CK_VOID_PTR pReserved)
{
    uint32_t ulSlot = 0;

    if (pReserved)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!pkcs11_context.initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    /* Release the crypto device */
    atcab_release();

    /* Close all the sessions that might be open */
    for (; ulSlot < pkcs11_context.slot_cnt; ulSlot++)
    {
        pkcs11_slot_ctx_ptr slot_ctx_ptr = &((pkcs11_slot_ctx_ptr)(pkcs11_context.slots))[ulSlot];
        if (slot_ctx_ptr)
        {
            (void)pkcs11_session_closeall(slot_ctx_ptr->slot_id);
        }
    }

    /* Clear the object cache */
    (void)pkcs11_object_deinit(&pkcs11_context);

    /** \todo If other threads are waiting for something to happen this call should
       cause those calls to unblock and return CKR_CRYPTOKI_NOT_INITIALIZED - How
       that is done by this simplified mutex API is yet to be determined */

    pkcs11_context.initialized = FALSE;

    return CKR_OK;
}

/** @} */
