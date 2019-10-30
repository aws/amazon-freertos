/**
 * \file
 * \brief PKCS11 Library Session Management & Context
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

#ifndef PKCS11_SESSION_H_
#define PKCS11_SESSION_H_

#include "cryptoki.h"
#include "pkcs11_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Session Context */
typedef struct _pkcs11_session_ctx
{
    CK_BBOOL            initialized;
    pkcs11_slot_ctx_ptr slot;
    CK_SESSION_HANDLE   handle;
    CK_STATE            state;
    CK_ULONG            error;
    CK_ATTRIBUTE_PTR    attrib_list;
    CK_ULONG            attrib_count;
    CK_ULONG            object_index;
    CK_ULONG            object_count;
    CK_OBJECT_HANDLE    active_object;
    CK_BYTE             read_key[32];           /**< Accepted through C_Login as the user pin */
} pkcs11_session_ctx, *pkcs11_session_ctx_ptr;

#ifdef __cplusplus
}
#endif
//pkcs11_session_ctx_ptr pkcs11_get_session_context(CK_SESSION_HANDLE hSession);
CK_RV pkcs11_session_check(pkcs11_session_ctx_ptr * pSession, CK_SESSION_HANDLE hSession);

CK_RV pkcs11_session_get_info(CK_SESSION_HANDLE hSession, CK_SESSION_INFO_PTR pInfo);
CK_RV pkcs11_session_open(CK_SLOT_ID slotID, CK_FLAGS flags, CK_VOID_PTR pApplication, CK_NOTIFY notify, CK_SESSION_HANDLE_PTR phSession);
CK_RV pkcs11_session_close(CK_SESSION_HANDLE hSession);
CK_RV pkcs11_session_closeall(CK_SLOT_ID slotID);

CK_RV pkcs11_session_login(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen);
CK_RV pkcs11_session_logout(CK_SESSION_HANDLE hSession);
CK_RV pkcs11_session_authorize(pkcs11_session_ctx_ptr pSession, CK_VOID_PTR pObject);

#endif /* PKCS11_SESSION_H_ */
