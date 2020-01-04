/**
 * \file
 * \brief PKCS11 Library Object Attribute Handling
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

#ifndef PKCS11_ATTRIB_H_
#define PKCS11_ATTRIB_H_

#include "cryptoki.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Populate an attribute based on the "object" */
typedef CK_RV (*attrib_f)(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);

typedef struct _pkcs11_attrib_model
{
    const CK_ATTRIBUTE_TYPE type;
    const attrib_f          func;
} pkcs11_attrib_model, *pkcs11_attrib_model_ptr;

#ifdef __cplusplus
}
#endif

CK_RV pkcs11_attrib_fill(CK_ATTRIBUTE_PTR pAttribute, const CK_VOID_PTR pData, const CK_ULONG ulSize);
CK_RV pkcs11_attrib_value(CK_ATTRIBUTE_PTR pAttribute, const CK_ULONG ulValue, const CK_ULONG ulSize);

CK_RV pkcs11_attrib_false(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_attrib_true(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);
CK_RV pkcs11_attrib_empty(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute);


#endif /* PKCS11_ATTRIB_H_ */
