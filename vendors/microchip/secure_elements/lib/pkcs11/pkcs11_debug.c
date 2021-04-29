/**
 * \file
 * \brief PKCS11 Library Debugging
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
#include "pkcs11_os.h"
#include "atca_helpers.h"

#if PKCS11_DEBUG_ENABLE

/* Macro to convert constant to a table entry */
#define C2TE(x)     { x, #x }

typedef struct
{
    CK_ULONG    value;
    CK_CHAR_PTR name;
} pkcs11_debug_lookup_t;

static const pkcs11_debug_lookup_t pkcs11_debug_cko_names[] = {
    C2TE(CKO_DATA),
    C2TE(CKO_CERTIFICATE),
    C2TE(CKO_PUBLIC_KEY),
    C2TE(CKO_PRIVATE_KEY),
    C2TE(CKO_SECRET_KEY),
    C2TE(CKO_HW_FEATURE),
    C2TE(CKO_DOMAIN_PARAMETERS),
    C2TE(CKO_MECHANISM),
    C2TE(CKO_OTP_KEY),
    C2TE(CKO_VENDOR_DEFINED)
};
static const size_t pkcs11_debug_cko_names_count = sizeof(pkcs11_debug_cko_names) / sizeof(pkcs11_debug_lookup_t);

static const pkcs11_debug_lookup_t pkcs11_debug_cka_names[] = {
    C2TE(CKA_CLASS),
    C2TE(CKA_TOKEN),
    C2TE(CKA_PRIVATE),
    C2TE(CKA_LABEL),
    C2TE(CKA_APPLICATION),
    C2TE(CKA_VALUE),
    C2TE(CKA_OBJECT_ID),
    C2TE(CKA_CERTIFICATE_TYPE),
    C2TE(CKA_ISSUER),
    C2TE(CKA_SERIAL_NUMBER),
    C2TE(CKA_AC_ISSUER),
    C2TE(CKA_OWNER),
    C2TE(CKA_ATTR_TYPES),
    C2TE(CKA_TRUSTED),
    C2TE(CKA_CERTIFICATE_CATEGORY),
    C2TE(CKA_JAVA_MIDP_SECURITY_DOMAIN),
    C2TE(CKA_URL),
    C2TE(CKA_HASH_OF_SUBJECT_PUBLIC_KEY),
    C2TE(CKA_HASH_OF_ISSUER_PUBLIC_KEY),
    C2TE(CKA_NAME_HASH_ALGORITHM),
    C2TE(CKA_CHECK_VALUE),
    C2TE(CKA_KEY_TYPE),
    C2TE(CKA_SUBJECT),
    C2TE(CKA_ID),
    C2TE(CKA_SENSITIVE),
    C2TE(CKA_ENCRYPT),
    C2TE(CKA_DECRYPT),
    C2TE(CKA_WRAP),
    C2TE(CKA_UNWRAP),
    C2TE(CKA_SIGN),
    C2TE(CKA_SIGN_RECOVER),
    C2TE(CKA_VERIFY),
    C2TE(CKA_VERIFY_RECOVER),
    C2TE(CKA_DERIVE),
    C2TE(CKA_START_DATE),
    C2TE(CKA_END_DATE),
    C2TE(CKA_MODULUS),
    C2TE(CKA_MODULUS_BITS),
    C2TE(CKA_PUBLIC_EXPONENT),
    C2TE(CKA_PRIVATE_EXPONENT),
    C2TE(CKA_PRIME_1),
    C2TE(CKA_PRIME_2),
    C2TE(CKA_EXPONENT_1),
    C2TE(CKA_EXPONENT_2),
    C2TE(CKA_COEFFICIENT),
    C2TE(CKA_PUBLIC_KEY_INFO),
    C2TE(CKA_PRIME),
    C2TE(CKA_SUBPRIME),
    C2TE(CKA_BASE),
    C2TE(CKA_PRIME_BITS),
    C2TE(CKA_SUBPRIME_BITS),
    C2TE(CKA_SUB_PRIME_BITS),
    C2TE(CKA_VALUE_BITS),
    C2TE(CKA_VALUE_LEN),
    C2TE(CKA_EXTRACTABLE),
    C2TE(CKA_LOCAL),
    C2TE(CKA_NEVER_EXTRACTABLE),
    C2TE(CKA_ALWAYS_SENSITIVE),
    C2TE(CKA_KEY_GEN_MECHANISM),
    C2TE(CKA_MODIFIABLE),
    C2TE(CKA_COPYABLE),
    C2TE(CKA_DESTROYABLE),
    C2TE(CKA_EC_PARAMS),
    C2TE(CKA_EC_POINT),
    C2TE(CKA_SECONDARY_AUTH),
    C2TE(CKA_AUTH_PIN_FLAGS),
    C2TE(CKA_ALWAYS_AUTHENTICATE),
    C2TE(CKA_WRAP_WITH_TRUSTED),
    C2TE(CKA_WRAP_TEMPLATE),
    C2TE(CKA_UNWRAP_TEMPLATE),
    C2TE(CKA_DERIVE_TEMPLATE),
    C2TE(CKA_OTP_FORMAT),
    C2TE(CKA_OTP_LENGTH),
    C2TE(CKA_OTP_TIME_INTERVAL),
    C2TE(CKA_OTP_USER_FRIENDLY_MODE),
    C2TE(CKA_OTP_CHALLENGE_REQUIREMENT),
    C2TE(CKA_OTP_TIME_REQUIREMENT),
    C2TE(CKA_OTP_COUNTER_REQUIREMENT),
    C2TE(CKA_OTP_PIN_REQUIREMENT),
    C2TE(CKA_OTP_COUNTER),
    C2TE(CKA_OTP_TIME),
    C2TE(CKA_OTP_USER_IDENTIFIER),
    C2TE(CKA_OTP_SERVICE_IDENTIFIER),
    C2TE(CKA_OTP_SERVICE_LOGO),
    C2TE(CKA_OTP_SERVICE_LOGO_TYPE),
    C2TE(CKA_GOSTR3410_PARAMS),
    C2TE(CKA_GOSTR3411_PARAMS),
    C2TE(CKA_GOST28147_PARAMS),
    C2TE(CKA_HW_FEATURE_TYPE),
    C2TE(CKA_RESET_ON_INIT),
    C2TE(CKA_HAS_RESET),
    C2TE(CKA_PIXEL_X),
    C2TE(CKA_PIXEL_Y),
    C2TE(CKA_RESOLUTION),
    C2TE(CKA_CHAR_ROWS),
    C2TE(CKA_CHAR_COLUMNS),
    C2TE(CKA_COLOR),
    C2TE(CKA_BITS_PER_PIXEL),
    C2TE(CKA_CHAR_SETS),
    C2TE(CKA_ENCODING_METHODS),
    C2TE(CKA_MIME_TYPES),
    C2TE(CKA_MECHANISM_TYPE),
    C2TE(CKA_REQUIRED_CMS_ATTRIBUTES),
    C2TE(CKA_DEFAULT_CMS_ATTRIBUTES),
    C2TE(CKA_SUPPORTED_CMS_ATTRIBUTES),
    C2TE(CKA_ALLOWED_MECHANISMS),
    C2TE(CKA_VENDOR_DEFINED)
};

static const size_t pkcs11_debug_cka_names_count = sizeof(pkcs11_debug_cka_names) / sizeof(pkcs11_debug_lookup_t);

static const pkcs11_debug_lookup_t pkcs11_debug_ckr_names[] = {
    C2TE(CKR_OK),
    C2TE(CKR_CANCEL),
    C2TE(CKR_HOST_MEMORY),
    C2TE(CKR_SLOT_ID_INVALID),
    C2TE(CKR_GENERAL_ERROR),
    C2TE(CKR_FUNCTION_FAILED),
    C2TE(CKR_ARGUMENTS_BAD),
    C2TE(CKR_NO_EVENT),
    C2TE(CKR_NEED_TO_CREATE_THREADS),
    C2TE(CKR_CANT_LOCK),
    C2TE(CKR_ATTRIBUTE_READ_ONLY),
    C2TE(CKR_ATTRIBUTE_SENSITIVE),
    C2TE(CKR_ATTRIBUTE_TYPE_INVALID),
    C2TE(CKR_ATTRIBUTE_VALUE_INVALID),
    C2TE(CKR_ACTION_PROHIBITED),
    C2TE(CKR_DATA_INVALID),
    C2TE(CKR_DATA_LEN_RANGE),
    C2TE(CKR_DEVICE_ERROR),
    C2TE(CKR_DEVICE_MEMORY),
    C2TE(CKR_DEVICE_REMOVED),
    C2TE(CKR_ENCRYPTED_DATA_INVALID),
    C2TE(CKR_ENCRYPTED_DATA_LEN_RANGE),
    C2TE(CKR_FUNCTION_CANCELED),
    C2TE(CKR_FUNCTION_NOT_PARALLEL),
    C2TE(CKR_FUNCTION_NOT_SUPPORTED),
    C2TE(CKR_KEY_HANDLE_INVALID),
    C2TE(CKR_KEY_SIZE_RANGE),
    C2TE(CKR_KEY_TYPE_INCONSISTENT),
    C2TE(CKR_KEY_NOT_NEEDED),
    C2TE(CKR_KEY_CHANGED),
    C2TE(CKR_KEY_NEEDED),
    C2TE(CKR_KEY_INDIGESTIBLE),
    C2TE(CKR_KEY_FUNCTION_NOT_PERMITTED),
    C2TE(CKR_KEY_NOT_WRAPPABLE),
    C2TE(CKR_KEY_UNEXTRACTABLE),
    C2TE(CKR_MECHANISM_INVALID),
    C2TE(CKR_MECHANISM_PARAM_INVALID),
    C2TE(CKR_OBJECT_HANDLE_INVALID),
    C2TE(CKR_OPERATION_ACTIVE),
    C2TE(CKR_OPERATION_NOT_INITIALIZED),
    C2TE(CKR_PIN_INCORRECT),
    C2TE(CKR_PIN_INVALID),
    C2TE(CKR_PIN_LEN_RANGE),
    C2TE(CKR_PIN_EXPIRED),
    C2TE(CKR_PIN_LOCKED),
    C2TE(CKR_SESSION_CLOSED),
    C2TE(CKR_SESSION_COUNT),
    C2TE(CKR_SESSION_HANDLE_INVALID),
    C2TE(CKR_SESSION_PARALLEL_NOT_SUPPORTED),
    C2TE(CKR_SESSION_READ_ONLY),
    C2TE(CKR_SESSION_EXISTS),
    C2TE(CKR_SESSION_READ_ONLY_EXISTS),
    C2TE(CKR_SESSION_READ_WRITE_SO_EXISTS),
    C2TE(CKR_SIGNATURE_INVALID),
    C2TE(CKR_SIGNATURE_LEN_RANGE),
    C2TE(CKR_TEMPLATE_INCOMPLETE),
    C2TE(CKR_TEMPLATE_INCONSISTENT),
    C2TE(CKR_TOKEN_NOT_PRESENT),
    C2TE(CKR_TOKEN_NOT_RECOGNIZED),
    C2TE(CKR_TOKEN_WRITE_PROTECTED),
    C2TE(CKR_UNWRAPPING_KEY_HANDLE_INVALID),
    C2TE(CKR_UNWRAPPING_KEY_SIZE_RANGE),
    C2TE(CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT),
    C2TE(CKR_USER_ALREADY_LOGGED_IN),
    C2TE(CKR_USER_NOT_LOGGED_IN),
    C2TE(CKR_USER_PIN_NOT_INITIALIZED),
    C2TE(CKR_USER_TYPE_INVALID),
    C2TE(CKR_USER_ANOTHER_ALREADY_LOGGED_IN),
    C2TE(CKR_USER_TOO_MANY_TYPES),
    C2TE(CKR_WRAPPED_KEY_INVALID),
    C2TE(CKR_WRAPPED_KEY_LEN_RANGE),
    C2TE(CKR_WRAPPING_KEY_HANDLE_INVALID),
    C2TE(CKR_WRAPPING_KEY_SIZE_RANGE),
    C2TE(CKR_WRAPPING_KEY_TYPE_INCONSISTENT),
    C2TE(CKR_RANDOM_SEED_NOT_SUPPORTED),
    C2TE(CKR_RANDOM_NO_RNG),
    C2TE(CKR_DOMAIN_PARAMS_INVALID),
    C2TE(CKR_CURVE_NOT_SUPPORTED),
    C2TE(CKR_BUFFER_TOO_SMALL),
    C2TE(CKR_SAVED_STATE_INVALID),
    C2TE(CKR_INFORMATION_SENSITIVE),
    C2TE(CKR_STATE_UNSAVEABLE),
    C2TE(CKR_CRYPTOKI_NOT_INITIALIZED),
    C2TE(CKR_CRYPTOKI_ALREADY_INITIALIZED),
    C2TE(CKR_MUTEX_BAD),
    C2TE(CKR_MUTEX_NOT_LOCKED),
    C2TE(CKR_NEW_PIN_MODE),
    C2TE(CKR_NEXT_OTP),
    C2TE(CKR_EXCEEDED_MAX_ITERATIONS),
    C2TE(CKR_FIPS_SELF_TEST_FAILED),
    C2TE(CKR_LIBRARY_LOAD_FAILED),
    C2TE(CKR_PIN_TOO_WEAK),
    C2TE(CKR_PUBLIC_KEY_INVALID),
    C2TE(CKR_FUNCTION_REJECTED),
    C2TE(CKR_VENDOR_DEFINED)
};
static const size_t pkcs11_debug_ckr_names_count = sizeof(pkcs11_debug_ckr_names) / sizeof(pkcs11_debug_lookup_t);

const char * pkcs11_debug_get_name(const CK_ULONG value, const pkcs11_debug_lookup_t* table, const size_t size)
{
    int i;
    char * rv = NULL;

    for (i = 0; (size > i) && (NULL == rv); i++)
    {
        if (value == table[i].value)
        {
            rv = (char*)table[i].name;
        }
    }

    if (NULL == rv)
    {
        rv = "";
    }

    return rv;
}

void pkcs11_debug_attributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
    int i;

    for (i = 0; i < ulCount && pTemplate; i++, pTemplate++)
    {
        const char * name = pkcs11_debug_get_name(pTemplate->type, pkcs11_debug_cka_names, pkcs11_debug_cka_names_count);
        switch (pTemplate->type)
        {
        case CKA_CLASS:
        {
            unsigned int object_id = *((CK_OBJECT_CLASS*)pTemplate->pValue);
            const char * object_name = pkcs11_debug_get_name(object_id, pkcs11_debug_cko_names, pkcs11_debug_cko_names_count);
            PKCS11_DEBUG_NOFILE("%s(%X):%d:%s(%x)\r\n", name, (unsigned int)pTemplate->type, (int)pTemplate->ulValueLen, object_name, object_id);
            break;
        }
        case CKA_LABEL:
            PKCS11_DEBUG_NOFILE("%s(%X):%d:%s\r\n", name, (unsigned int)pTemplate->type, (int)pTemplate->ulValueLen, (char*)pTemplate->pValue);
            break;
        default:
#ifndef ATCA_NO_HEAP
            {
                size_t buf_len = pTemplate->ulValueLen * 3 + 1;
                char * buffer = pkcs11_os_malloc(buf_len);
                memset(buffer, 0, buf_len);
                if (buffer)
                {
                    (void)atcab_bin2hex_(pTemplate->pValue, pTemplate->pValue, buffer, &buf_len, false, true, true);
                    PKCS11_DEBUG_NOFILE("%s(%X):%d:%s:\r\n", name, (unsigned int)pTemplate->type, (int)pTemplate->ulValueLen, buffer);
                    pkcs11_os_free(buffer);
                }
            }
#else
            PKCS11_DEBUG_NOFILE("%s(%X):%d:%p:\r\n", name, (unsigned int)pTemplate->type, (int)pTemplate->ulValueLen, pTemplate->pValue);
#endif
            break;
        }
    }
}

const char * pkcs11_debug_get_ckr_name(CK_RV rv)
{
    return pkcs11_debug_get_name(rv, pkcs11_debug_ckr_names, pkcs11_debug_ckr_names_count);
}

#endif