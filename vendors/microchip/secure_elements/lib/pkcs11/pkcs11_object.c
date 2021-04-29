/**
 * \file
 * \brief PKCS11 Library Object Handling Base
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
#include "cryptoauthlib.h"

#include "cryptoki.h"
#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_init.h"
#include "pkcs11_session.h"
#include "pkcs11_util.h"
#include "pkcs11_object.h"
#include "pkcs11_os.h"
#include "pkcs11_find.h"
#include "pkcs11_key.h"
#include "pkcs11_cert.h"


/**
 * \defgroup pkcs11 Object (pkcs11_object_)
   @{ */

#ifdef ATCA_NO_HEAP
static pkcs11_object pkcs11_object_store[PKCS11_MAX_OBJECTS_ALLOWED];
#endif

pkcs11_object_cache_t pkcs11_object_cache[PKCS11_MAX_OBJECTS_ALLOWED];

/** For object handle tracking */
static CK_OBJECT_HANDLE pkcs11_object_alloc_handle(void)
{
    static CK_OBJECT_HANDLE pkcs11_object_last_handle = 1;

    if (pkcs11_object_last_handle)
    {
        pkcs11_object_last_handle++;
    }

    return pkcs11_object_last_handle;
}

/**
 * CKA_CLASS == CKO_HW_FEATURE_TYPE
 * CKA_HW_FEATURE_TYPE == CKH_MONOTONIC_COUNTER
 */
const pkcs11_attrib_model pkcs11_object_monotonic_attributes[] = {
    /** Object Class - CK_OBJECT_CLASS */
    { CKA_CLASS,           pkcs11_object_get_class                                                      },
    /** Hardware Feature Type - CK_HW_FEATURE_TYPE */
    { CKA_HW_FEATURE_TYPE, pkcs11_object_get_type                                                       },
    /** Counter will reset to a previously returned value if the token is
        initialized using C_InitToken. */
    { CKA_RESET_ON_INIT,   pkcs11_attrib_false                                                          },
    /** Counter has been reset at least once at some point in time. */
    { CKA_HAS_RESET,       pkcs11_attrib_false                                                          },
    /** Current value of the monotonic counter. Big endian order. */
    { CKA_VALUE,           NULL_PTR                                                                     },
};

const CK_ULONG pkcs11_object_monotonic_attributes_count = PKCS11_UTIL_ARRAY_SIZE(pkcs11_object_monotonic_attributes);

///**
// * Mandatory Object Identification Fields for All objects not identified
// * as CKA_CLASS == CKO_HW_FEATURE
// */
//const pkcs11_attrib_model const pkcs11_object_storage_attributes[] = {
//    /** Object Class - CK_OBJECT_CLASS */
//    { CKA_CLASS,                        pkcs11_object_get_class },
//    /** CK_TRUE if object is a token object; CK_FALSE if object is a session object. Default is CK_FALSE. */
//    { CKA_TOKEN,                        pkcs11_attrib_true },
//    /** CK_TRUE if object is a private object; CK_FALSE if object is a public object. */
//    { CKA_PRIVATE,                      pkcs11_key_get_access_type },
//    /** CK_TRUE if object can be modified. Default is CK_TRUE. */
//    { CKA_MODIFIABLE,                   NULL_PTR },
//    /** Description of the object(default empty). */
//    { CKA_LABEL,                        pkcs11_object_get_name },
//    /** CK_TRUE if object can be copied using C_CopyObject.Defaults to CK_TRUE. */
//    { CKA_COPYABLE,                     pkcs11_attrib_false },
//    /** CK_TRUE if the object can be destroyed using C_DestroyObject. Default is CK_TRUE. */
//    { CKA_DESTROYABLE,                  pkcs11_attrib_false },
//};

///**
// * CKO_DATA - Object Attribute Model
// * Other than providing access to it, Cryptoki does not attach any special meaning to a data object.
// */
//const pkcs11_attrib_model pkcs11_object_data_attributes[] = {
//    /** Description of the application that manages the object(default empty) */
//    { CKA_APPLICATION,                  NULL_PTR },
//    /** DER - encoding of the object identifier indicating the data object type(default empty) */
//    { CKA_OBJECT_ID,                    NULL_PTR },
//    /** Value of the object(default empty) */
//    { CKA_VALUE,                        NULL_PTR }
//
//};

CK_RV pkcs11_object_alloc(pkcs11_object_ptr * ppObject)
{
    CK_ULONG i;
    CK_RV rv = CKR_OK;

    if (!ppObject)
    {
        rv = CKR_ARGUMENTS_BAD;
    }
    else
    {
        *ppObject = NULL;
    }

    for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED && CKR_OK == rv; i++)
    {
        if (!pkcs11_object_cache[i].object)
        {
#ifdef ATCA_NO_HEAP
            *ppObject = &pkcs11_object_store[i];
#else
            *ppObject = pkcs11_os_malloc(sizeof(pkcs11_object));
#endif
            if (*ppObject)
            {
                memset(*ppObject, 0, sizeof(pkcs11_object));
                pkcs11_object_cache[i].handle = pkcs11_object_alloc_handle();
                pkcs11_object_cache[i].object = *ppObject;
            }
            else
            {
                rv = CKR_HOST_MEMORY;
            }

            break;
        }
    }

    if (PKCS11_MAX_OBJECTS_ALLOWED == i)
    {
        rv = CKR_HOST_MEMORY;
    }

    return rv;
}

CK_RV pkcs11_object_free(pkcs11_object_ptr pObject)
{
    CK_ULONG i;

    for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        if (pObject == pkcs11_object_cache[i].object)
        {
            /* Delink it */
            pkcs11_object_cache[i].object = NULL_PTR;
            pkcs11_object_cache[i].handle = 0;
        }
    }

    if (pObject)
    {
        if (pObject->data)
        {
            if (pObject->flags & PKCS11_OBJECT_FLAG_SENSITIVE)
            {
                (void)pkcs11_util_memset((CK_VOID_PTR)pObject->data, pObject->size, 0, pObject->size);
            }
#ifndef ATCA_NO_HEAP
            if (pObject->data && (pObject->flags & PKCS11_OBJECT_FLAG_DYNAMIC))
            {
                pkcs11_os_free(pObject->data);
            }
#endif
        }

        (void)pkcs11_util_memset(pObject, sizeof(pkcs11_object), 0, sizeof(pkcs11_object) );

#ifndef ATCA_NO_HEAP
        pkcs11_os_free(pObject);
#endif
    }

    return CKR_OK;
}

CK_RV pkcs11_object_check(pkcs11_object_ptr * ppObject, CK_OBJECT_HANDLE hObject)
{
    CK_ULONG i;

    if (!hObject)
    {
        return CKR_OBJECT_HANDLE_INVALID;
    }

    for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        if (hObject == pkcs11_object_cache[i].handle)
        {
            break;
        }
    }
    if (i == PKCS11_MAX_OBJECTS_ALLOWED)
    {
        return CKR_OBJECT_HANDLE_INVALID;
    }
    else if (ppObject)
    {
        *ppObject = pkcs11_object_cache[i].object;
    }

    return CKR_OK;
}

CK_RV pkcs11_object_get_handle(pkcs11_object_ptr pObject, CK_OBJECT_HANDLE_PTR phObject)
{
    CK_ULONG i;

    if (!phObject || !pObject)
    {
        return CKR_ARGUMENTS_BAD;
    }

    for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        if (pObject == pkcs11_object_cache[i].object)
        {
            *phObject = pkcs11_object_cache[i].handle;
            break;
        }
    }
    if (i == PKCS11_MAX_OBJECTS_ALLOWED)
    {
        return CKR_OBJECT_HANDLE_INVALID;
    }

    return CKR_OK;
}


CK_RV pkcs11_object_get_name(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (obj_ptr->name)
    {
        return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)obj_ptr->name, strlen((char*)obj_ptr->name));
    }
    else
    {
        return pkcs11_attrib_fill(pAttribute, NULL_PTR, 0);
    }
}

CK_RV pkcs11_object_get_class(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    return pkcs11_attrib_fill(pAttribute, &obj_ptr->class_id, sizeof(obj_ptr->class_id));
}

CK_RV pkcs11_object_get_type(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    return pkcs11_attrib_fill(pAttribute, &obj_ptr->class_type, sizeof(obj_ptr->class_type));
}

CK_RV pkcs11_object_get_destroyable(CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    pkcs11_object_ptr obj_ptr = (pkcs11_object_ptr)pObject;

    if (!obj_ptr)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (obj_ptr->flags & PKCS11_OBJECT_FLAG_DESTROYABLE)
    {
        return pkcs11_attrib_true(pObject, pAttribute);
    }
    else
    {
        return pkcs11_attrib_false(pObject, pAttribute);
    }
}

CK_RV pkcs11_object_get_size(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ULONG_PTR pulSize)
{
    pkcs11_object_ptr pObject;
    CK_RV rv;

    rv = pkcs11_init_check(NULL, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pulSize)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(NULL_PTR, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pObject, hObject);
    if (rv)
    {
        return rv;
    }

    *pulSize = pObject->size;

    return CKR_OK;
}

CK_RV pkcs11_object_find(pkcs11_object_ptr * ppObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
    int i;
    CK_ATTRIBUTE_PTR pName = NULL;
    CK_OBJECT_CLASS class = CKO_PRIVATE_KEY;    /* Unless specified assume private key object */

    if (!ppObject || !pTemplate || !ulCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Match Name and Class */
    for (i = 0; i < ulCount; i++, pTemplate++)
    {
        switch (pTemplate->type)
        {
        case CKA_LABEL:
            pName = pTemplate;
            break;
        case CKA_CLASS:
            class = *((CK_OBJECT_CLASS_PTR)pTemplate->pValue);
            break;
        default:
            break;
        }
    }

    if (pName)
    {
        for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
        {
            pkcs11_object_ptr pObj = pkcs11_object_cache[i].object;
            if (pObj)
            {
                if ((pObj->class_id == class) && (strlen((char*)pObj->name) == pName->ulValueLen))
                {
                    if (!memcmp(pObj->name, pName->pValue, pName->ulValueLen))
                    {
                        *ppObject = pObj;
                        break;
                    }
                }
            }
        }
    }

    return CKR_OK;
}

/**
 * \brief Create a new object on the token in the specified session using the given attribute template
 */
CK_RV pkcs11_object_create
(
    CK_SESSION_HANDLE    hSession,
    CK_ATTRIBUTE_PTR     pTemplate,
    CK_ULONG             ulCount,
    CK_OBJECT_HANDLE_PTR phObject
)
{
    CK_RV rv;
    pkcs11_object_ptr pObject = NULL;
    CK_ATTRIBUTE_PTR pLabel = NULL;
    CK_OBJECT_CLASS_PTR pClass = NULL;
    CK_ATTRIBUTE_PTR pData = NULL;
    int i;
    pkcs11_lib_ctx_ptr pLibCtx = NULL;
    pkcs11_session_ctx_ptr pSession = NULL;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    /* Look for supported/mandatory attributes */
    for (i = 0; i < ulCount; i++)
    {
        switch (pTemplate[i].type)
        {
        case CKA_LABEL:
            pLabel = &pTemplate[i];
            break;
        case CKA_CLASS:
            pClass = pTemplate[i].pValue;
            break;
        case CKA_VALUE:
        /* fall-through */
        case CKA_EC_POINT:
            pData = &pTemplate[i];
            break;
        default:
            break;
        }
    }

    rv =  pkcs11_object_find(&pObject, pTemplate, ulCount);

	if (rv)
    {
        return rv;
    }
	
    if (!pObject)
    {
        /* Allocate a new object */
        rv = pkcs11_object_alloc(&pObject);
    }

    if (pObject)
    {
        switch (*pClass)
        {
        case CKO_CERTIFICATE:
            rv = pkcs11_config_cert(pLibCtx, pSession->slot, pObject, pLabel);
            if (CKR_OK == rv)
            {
                rv = pkcs11_cert_x509_write(pObject, pData);
            }
            break;
        case CKO_PUBLIC_KEY:
            pObject->class_id = CKO_PUBLIC_KEY;
            if (CKR_OK == (rv = pkcs11_config_key(pLibCtx, pSession->slot, pObject, pLabel)))
            {
                rv = pkcs11_key_write(pSession, pObject, pData);
            }
            break;
        case CKO_PRIVATE_KEY:
            pObject->class_id = CKO_PRIVATE_KEY;
            if (CKR_OK == (rv = pkcs11_config_key(pLibCtx, pSession->slot, pObject, pLabel)))
            {
                rv = pkcs11_key_write(pSession, pObject, pData);
            }
            break;
        default:
            break;
        }
        if (CKR_OK == rv)
        {
            rv = pkcs11_object_get_handle(pObject, phObject);
        }
    }

    return rv;
}

/**
 * \brief Destroy the specified object
 */
CK_RV pkcs11_object_destroy(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)
{
    pkcs11_object_ptr pObject;
    CK_RV rv;
    pkcs11_lib_ctx_ptr pLibCtx = NULL;
    pkcs11_session_ctx_ptr pSession = NULL;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_object_check(&pObject, hObject);
    if (rv)
    {
        return rv;
    }

    if (pObject->flags & PKCS11_OBJECT_FLAG_DESTROYABLE)
    {
#if !PKCS11_USE_STATIC_CONFIG
        pkcs11_config_remove_object(pLibCtx, pSession->slot, pObject);
#endif
        return pkcs11_object_free(pObject);
    }
    else
    {
        return CKR_ACTION_PROHIBITED;
    }
}

/* Interal function to clean up resources */
CK_RV pkcs11_object_deinit(pkcs11_lib_ctx_ptr pContext)
{
    CK_RV rv = CKR_OK;
    int i;

    for (i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        pkcs11_object_ptr pObj = pkcs11_object_cache[i].object;
        if (pObj)
        {
            CK_RV tmp = pkcs11_object_free(pObj);
            if (!rv)
            {
                rv = tmp;
            }
        }
    }
    return rv;
}

#if ATCA_TA_SUPPORT
CK_RV pkcs11_object_load_handle_info(pkcs11_lib_ctx_ptr pContext)
{
    CK_RV rv = CKR_OK;
    uint8_t handle_info[TA_HANDLE_INFO_SIZE];

    for (int i = 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        pkcs11_object_ptr pObj = pkcs11_object_cache[i].object;
        if (pObj)
        {
            pObj->flags |= PKCS11_OBJECT_FLAG_TA_TYPE;
            if (ATCA_SUCCESS == talib_info_get_handle_info(atcab_get_device(), pObj->slot, handle_info))
            {
                memcpy(&pObj->handle_info, handle_info, sizeof(ta_element_attributes_t));
            }
            else
            {
                memset(&pObj->handle_info, 0, sizeof(ta_element_attributes_t));
            }

        }
    }
    return rv;
}
#endif

/** @} */

