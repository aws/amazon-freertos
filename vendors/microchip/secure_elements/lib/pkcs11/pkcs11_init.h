/**
 * \file
 * \brief PKCS11 Library Initialization & Context
 *
 * Copyright (c) 2017 Microchip Technology Inc. All rights reserved.
 *
 * \atmel_crypto_device_library_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \atmel_crypto_device_library_license_stop
 */

#ifndef PKCS11_INIT_H_
#define PKCS11_INIT_H_

#include "cryptoki.h"
#include "pkcs11_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Library Context */
typedef struct _pkcs11_lib_ctx
{
    CK_BBOOL        initialized;
    CK_CREATEMUTEX  create_mutex;
    CK_DESTROYMUTEX destroy_mutex;
    CK_LOCKMUTEX    lock_mutex;
    CK_UNLOCKMUTEX  unlock_mutex;
    CK_VOID_PTR     mutex;
    CK_VOID_PTR     slots;
    CK_ULONG        slot_cnt;
#if !PKCS11_USE_STATIC_CONFIG
    CK_CHAR config_path[200];
#endif
} pkcs11_lib_ctx, *pkcs11_lib_ctx_ptr;

#ifdef __cplusplus
}
#endif

CK_RV pkcs11_init(CK_C_INITIALIZE_ARGS_PTR pInitArgs);
CK_RV pkcs11_deinit(CK_VOID_PTR pReserved);
CK_RV pkcs11_init_check(pkcs11_lib_ctx_ptr * ppContext, CK_BBOOL lock);

pkcs11_lib_ctx_ptr pkcs11_get_context(void);
CK_RV pkcs11_lock_context(pkcs11_lib_ctx_ptr pContext);
CK_RV pkcs11_unlock_context(pkcs11_lib_ctx_ptr pContext);

#endif /* PKCS11_INIT_H_ */
