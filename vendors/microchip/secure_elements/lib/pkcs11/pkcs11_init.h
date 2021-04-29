/**
 * \file
 * \brief PKCS11 Library Initialization & Context
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
