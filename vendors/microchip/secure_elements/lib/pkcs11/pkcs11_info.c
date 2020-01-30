/**
 * \file
 * \brief PKCS11 Library Information Functions
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

#include "pkcs11_config.h"
#include "pkcs11_init.h"
#include "pkcs11_slot.h"
#include "pkcs11_session.h"
#include "pkcs11_util.h"
#include "cryptoauthlib.h"

#include <stdio.h>

/**
 * \defgroup pkcs11 Information (pkcs11_)
   @{ */

const char pkcs11_lib_manufacturer_id[] = "Microchip Technology Inc";
const char pkcs11_lib_description[] = "Cryptoauthlib PKCS11 Interface";

/**
 * \brief Obtains general information about Cryptoki
 */
CK_RV pkcs11_get_lib_info(CK_INFO_PTR pInfo)
{
    pkcs11_lib_ctx_ptr ctx = pkcs11_get_context();

    if (!ctx || !ctx->initialized)
    {
        return CKR_CRYPTOKI_NOT_INITIALIZED;
    }

    if (!pInfo)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Must be zero for 2.40 */
    pInfo->flags = 0;

    /* PKCS11 Specification Version */
    pInfo->cryptokiVersion.major = CRYPTOKI_VERSION_MAJOR;
    pInfo->cryptokiVersion.minor = CRYPTOKI_VERSION_MINOR;

    /* Cryptoauthlib Version */
    pInfo->libraryVersion.major = ATCA_LIB_VER_MAJOR;
    pInfo->libraryVersion.minor = ATCA_LIB_VER_MINOR;

    /* Set up the identifier strings */
    snprintf((char*)pInfo->manufacturerID, sizeof(pInfo->manufacturerID), pkcs11_lib_manufacturer_id);
    snprintf((char*)pInfo->libraryDescription, sizeof(pInfo->libraryDescription), pkcs11_lib_description);

    /* Make sure strings are escaped properly */
    pkcs11_util_escape_string(pInfo->manufacturerID, sizeof(pInfo->manufacturerID));
    pkcs11_util_escape_string(pInfo->libraryDescription, sizeof(pInfo->libraryDescription));

    return CKR_OK;
}

/** @} */
