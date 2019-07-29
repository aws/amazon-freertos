/**
 * \file
 * \brief PKCS11 Library Slot Handling & Context
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

#ifndef PKCS11_SLOT_H_
#define PKCS11_SLOT_H_

//#include "pkcs11_config.h"
#include "pkcs11_init.h"
#include "cryptoauthlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Slot Context */
typedef struct _pkcs11_slot_ctx
{
    CK_BBOOL           initialized;
    CK_SLOT_ID         slot_id;
    ATCAIfaceCfg*      interface_config;
    CK_SESSION_HANDLE  session;
    atecc608a_config_t cfg_zone;
    CK_FLAGS           flags;
} pkcs11_slot_ctx, *pkcs11_slot_ctx_ptr;

#ifdef __cplusplus
}
#endif

CK_RV pkcs11_slot_init(CK_SLOT_ID slotID);
CK_RV pkcs11_slot_config(CK_SLOT_ID slotID);
CK_VOID_PTR pkcs11_slot_initslots(CK_ULONG pulCount);
pkcs11_slot_ctx_ptr pkcs11_slot_get_context(pkcs11_lib_ctx_ptr lib_ctx, CK_SLOT_ID slotID);


CK_RV pkcs11_slot_get_list(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList, CK_ULONG_PTR pulCount);
CK_RV pkcs11_slot_get_info(CK_SLOT_ID slotID, CK_SLOT_INFO_PTR pInfo);

#endif /* PKCS11_SLOT_H_ */
