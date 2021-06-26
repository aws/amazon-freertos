/**
 * \file
 * \brief PKCS11 Library Configuration
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

#ifndef PKCS11_CONFIG_H_
#define PKCS11_CONFIG_H_


/** Define to lock the PIN slot after writing */
#ifndef PKCS11_LOCK_PIN_SLOT
    #define PKCS11_LOCK_PIN_SLOT    0
#endif

/** Enable PKCS#11 Debugging Messages */
#ifndef PKCS11_DEBUG_ENABLE
    #define PKCS11_DEBUG_ENABLE    0
#endif

/** Use Static or Dynamic Allocation */
#ifndef PKCS11_USE_STATIC_MEMORY
    #define PKCS11_USE_STATIC_MEMORY    1
#endif

/** Use a compiled configuration rather than loading from a filestore */
#ifndef PKCS11_USE_STATIC_CONFIG
    #define PKCS11_USE_STATIC_CONFIG    1
#endif

/** Maximum number of slots allowed in the system - if static memory this will
 * always be the number of slots */
#ifndef PKCS11_MAX_SLOTS_ALLOWED
    #define PKCS11_MAX_SLOTS_ALLOWED    1
#endif

/** Maximum number of total sessions allowed in the system - if using static
 *    memory then this many session contexts will be allocated */
#ifndef PKCS11_MAX_SESSIONS_ALLOWED
    #define PKCS11_MAX_SESSIONS_ALLOWED    10
#endif

/** Maximum number of cryptographic objects allowed to be cached */
#ifndef PKCS11_MAX_OBJECTS_ALLOWED
    #define PKCS11_MAX_OBJECTS_ALLOWED    16
#endif

/** Maximum label size in characters */
#ifndef PKCS11_MAX_LABEL_SIZE
    #define PKCS11_MAX_LABEL_SIZE    30
#endif

/****************************************************************************/
/* The following configuration options are for fine tuning of the library   */
/****************************************************************************/

/** Defines if the library will produce a static function list or use an
 * externally defined one. This is an optimization that allows for a statically
 * linked library to include only the PKCS#11 functions that the application
 * intends to use. Otherwise compilers will not be able to optimize out the unusued
 * functions */
#ifndef PKCS11_EXTERNAL_FUNCTION_LIST
    #define PKCS11_EXTERNAL_FUNCTION_LIST    1
#endif

/** Static Search Attribute Cache in bytes (variable number of attributes based
 *    on size and memory requirements) */
#ifndef PKCS11_SEARCH_CACHE_SIZE
    #define PKCS11_SEARCH_CACHE_SIZE    250
#endif

/** Support for configuring a "blank" or new device */
#ifndef PKCS11_TOKEN_INIT_SUPPORT
    #define PKCS11_TOKEN_INIT_SUPPORT    1
#endif

/** Include the monotonic hardware feature as an object */
#ifndef PKCS11_MONOTONIC_ENABLE
    #define PKCS11_MONOTONIC_ENABLE    0
#endif


#include "pkcs11/cryptoki.h"
#include <stddef.h>
typedef struct _pkcs11_slot_ctx   * pkcs11_slot_ctx_ptr;
typedef struct _pkcs11_lib_ctx    * pkcs11_lib_ctx_ptr;
typedef struct _pkcs11_object     * pkcs11_object_ptr;

#if PKCS11_USE_STATIC_CONFIG
    CK_RV pkcs11_config_interface( pkcs11_slot_ctx_ptr pSlot );
#endif
CK_RV pkcs11_config_load_objects( pkcs11_slot_ctx_ptr pSlot );
CK_RV pkcs11_config_load( pkcs11_slot_ctx_ptr slot_ctx );
CK_RV pkcs11_config_cert( pkcs11_lib_ctx_ptr pLibCtx,
                          pkcs11_slot_ctx_ptr pSlot,
                          pkcs11_object_ptr pObject,
                          CK_ATTRIBUTE_PTR pcLabel );
CK_RV pkcs11_config_key( pkcs11_lib_ctx_ptr pLibCtx,
                         pkcs11_slot_ctx_ptr pSlot,
                         pkcs11_object_ptr pObject,
                         CK_ATTRIBUTE_PTR pcLabel );
CK_RV pkcs11_config_remove_object( pkcs11_lib_ctx_ptr pLibCtx,
                                   pkcs11_slot_ctx_ptr pSlot,
                                   pkcs11_object_ptr pObject );

void pkcs11_config_init_private( pkcs11_object_ptr pObject,
                                 char * label,
                                 size_t len );
void pkcs11_config_init_public( pkcs11_object_ptr pObject,
                                char * label,
                                size_t len );
void pkcs11_config_init_cert( pkcs11_object_ptr pObject,
                              char * label,
                              size_t len );

#endif /* PKCS11_CONFIG_H_ */
