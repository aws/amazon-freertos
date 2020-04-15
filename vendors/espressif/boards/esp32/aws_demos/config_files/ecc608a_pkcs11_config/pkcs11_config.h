/**
 * \file
 * \brief PKCS11 Library Configuration
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

#ifndef PKCS11_CONFIG_H_
#define PKCS11_CONFIG_H_


/* Cryptoauthlib at the time of this module development is not versioned */
#ifndef ATCA_LIB_VER_MAJOR
#define ATCA_LIB_VER_MAJOR  3
#endif

#ifndef ATCA_LIB_VER_MINOR
#define ATCA_LIB_VER_MINOR  2
#endif

/** If an Auth-key or IoProtection Secret is to be used this is the
 * slot number of it */
#ifndef PKCS11_PIN_SLOT
#define PKCS11_PIN_SLOT                 6
#endif

/** Define to lock the PIN slot after writing */
#ifndef PKCS11_LOCK_PIN_SLOT
#define PKCS11_LOCK_PIN_SLOT            0
#endif

/** Enable PKCS#11 Debugging Messages */
#ifndef PKCS11_DEBUG_ENABLE
#define PKCS11_DEBUG_ENABLE             0
#endif

/** Use Static or Dynamic Allocation */
#ifndef PKCS11_USE_STATIC_MEMORY
#define PKCS11_USE_STATIC_MEMORY        1
#endif

/** Use a compiled configuration rather than loading from a filestore */
#ifndef PKCS11_USE_STATIC_CONFIG
#define PKCS11_USE_STATIC_CONFIG        1
#endif

/** Maximum number of slots allowed in the system - if static memory this will
   always be the number of slots */
#ifndef PKCS11_MAX_SLOTS_ALLOWED
#define PKCS11_MAX_SLOTS_ALLOWED        1
#endif

/** Maximum number of total sessions allowed in the system - if using static
   memory then this many session contexts will be allocated */
#ifndef PKCS11_MAX_SESSIONS_ALLOWED
#define PKCS11_MAX_SESSIONS_ALLOWED     10
#endif

/** Maximum number of cryptographic objects allowed to be cached */
#ifndef PKCS11_MAX_OBJECTS_ALLOWED
#define PKCS11_MAX_OBJECTS_ALLOWED      16
#endif

/** Maximum label size in characters */
#ifndef PKCS11_MAX_LABEL_SIZE
#define PKCS11_MAX_LABEL_SIZE           30
#endif

/****************************************************************************/
/* The following configuration options are for fine tuning of the library   */
/****************************************************************************/

/** Defines if the library will produce a static function list or use an
   externally defined one. This is an optimization that allows for a statically
   linked library to include only the PKCS#11 functions that the application
   intends to use. Otherwise compilers will not be able to optimize out the unusued
   functions */
#ifndef PKCS11_EXTERNAL_FUNCTION_LIST
#define PKCS11_EXTERNAL_FUNCTION_LIST    1
#endif

/** Static Search Attribute Cache in bytes (variable number of attributes based
   on size and memory requirements) */
#ifndef PKCS11_SEARCH_CACHE_SIZE
#define PKCS11_SEARCH_CACHE_SIZE        128
#endif

/** Device Support for ATECC508A */
#ifndef PKCS11_508_SUPPORT
#define PKCS11_508_SUPPORT              0
#endif

/** Device Support for ATECC608A */
#ifndef PKCS11_608_SUPPORT
#define PKCS11_608_SUPPORT              1
#endif

/** Support for configuring a "blank" or new device */
#ifndef PKCS11_TOKEN_INIT_SUPPORT
#define PKCS11_TOKEN_INIT_SUPPORT       1
#endif

/** Include the monotonic hardware feature as an object */
#ifndef PKCS11_MONOTONIC_ENABLE
#define PKCS11_MONOTONIC_ENABLE         0
#endif


#include "pkcs11/cryptoki.h"
#include <stddef.h>
typedef struct _pkcs11_slot_ctx *pkcs11_slot_ctx_ptr;
typedef struct _pkcs11_lib_ctx  *pkcs11_lib_ctx_ptr;
typedef struct _pkcs11_object   *pkcs11_object_ptr;

CK_RV pkcs11_config_load_objects(pkcs11_slot_ctx_ptr pSlot);
CK_RV pkcs11_config_load(pkcs11_slot_ctx_ptr slot_ctx);
CK_RV pkcs11_config_cert(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pcLabel);
CK_RV pkcs11_config_key(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pcLabel);
CK_RV pkcs11_config_remove_object(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject);

void pkcs11_config_init_private(pkcs11_object_ptr pObject, char * label, size_t len);
void pkcs11_config_init_public(pkcs11_object_ptr pObject, char * label, size_t len);
void pkcs11_config_init_cert(pkcs11_object_ptr pObject, char * label, size_t len);

#endif /* PKCS11_CONFIG_H_ */