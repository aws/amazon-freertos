/**
 * \file
 * \brief PKCS11 Library Object Handling
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

#ifndef PKCS11_OBJECT_H_
#define PKCS11_OBJECT_H_

#include "cryptoki.h"
#include "pkcs11_config.h"
#include "pkcs11_attrib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _pkcs11_object
{

    /** The Class Identifier */
    CK_OBJECT_CLASS class_id;
    /** The Class Type */
    CK_ULONG class_type;
    /** List of attribute models this object possesses */
    pkcs11_attrib_model const * attributes;
    /** Count of attribute models */
    CK_ULONG     count;
    CK_ULONG     size;
    CK_VOID_PTR  config;
    CK_BYTE      slot;
    CK_FLAGS     flags;
    CK_UTF8CHAR  name[PKCS11_MAX_LABEL_SIZE + 1];
    void const * data;
} pkcs11_object, *pkcs11_object_ptr;

typedef struct _pkcs11_object_cache_t
{
    /** Arbitrary (but unique) non-null identifier for an object */
    CK_OBJECT_HANDLE handle;
    /** The actual object  */
    pkcs11_object_ptr object;
} pkcs11_object_cache_t;

extern pkcs11_object_cache_t pkcs11_object_cache[];

extern const pkcs11_attrib_model pkcs11_object_monotonic_attributes[];
extern const CK_ULONG pkcs11_object_monotonic_attributes_count;

#define PKCS11_OBJECT_FLAG_DESTROYABLE      0x01
#define PKCS11_OBJECT_FLAG_MODIFIABLE       0x02
#define PKCS11_OBJECT_FLAG_DYNAMIC          0x04

CK_RV pkcs11_object_alloc(pkcs11_object_ptr * ppObject);
CK_RV pkcs11_object_free(pkcs11_object_ptr pObject);
CK_RV pkcs11_object_check(pkcs11_object_ptr * ppObject, CK_OBJECT_HANDLE handle);
CK_RV pkcs11_object_find(pkcs11_object_ptr * ppObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);

CK_RV pkcs11_object_get_class(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_object_get_name(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_object_get_type(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_object_get_destroyable(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_object_get_size(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ULONG_PTR pulSize);
CK_RV pkcs11_object_get_handle(pkcs11_object_ptr pObject, CK_OBJECT_HANDLE_PTR phObject);

CK_RV pkcs11_object_create(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject);
CK_RV pkcs11_object_destroy(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject);

CK_RV pkcs11_object_deinit(pkcs11_lib_ctx_ptr pContext);

#ifdef __cplusplus
}
#endif

#endif /* PKCS11_OBJECT_H_ */
