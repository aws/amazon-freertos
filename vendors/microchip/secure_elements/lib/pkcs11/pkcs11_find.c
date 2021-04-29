/**
 * \file
 * \brief PKCS11 Library Object Find/Searching
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

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "pkcs11_init.h"
#include "pkcs11_os.h"
#include "pkcs11_slot.h"
#include "pkcs11_session.h"
#include "pkcs11_find.h"
#include "pkcs11_util.h"

/**
 * \defgroup pkcs11 Find (pkcs11_find_)
   @{ */

//#ifdef ATCA_NO_HEAP
static CK_BYTE pkcs11_find_template_cache[PKCS11_SEARCH_CACHE_SIZE];
//#endif

/**
 * \brief Copy an array of CK_ATTRIBUTE structures
 */
static CK_RV pkcs11_find_copy_template(CK_BYTE_PTR pBuffer, CK_ULONG ulBufSize, CK_ATTRIBUTE_PTR pTemplate, const CK_ULONG ulCount)
{
    CK_ULONG bytes_required;
    CK_ATTRIBUTE_PTR pCopy;
    CK_ULONG i;

    if (!pBuffer || !ulBufSize || !pTemplate || !ulCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    /* Start with the amount of memory required to hold the Array */
    bytes_required = sizeof(CK_ATTRIBUTE) * ulCount;

    /* Check the template pValue requirements */
    for (i = 0; i < ulCount; i++)
    {
        if (pTemplate[i].pValue && pTemplate[i].ulValueLen)
        {
            bytes_required += pTemplate[i].ulValueLen;

            /* Check for overflow */
            if (bytes_required < pTemplate[i].ulValueLen)
            {
                return CKR_HOST_MEMORY;
            }
        }
    }

    if (ulBufSize < bytes_required)
    {
        return CKR_HOST_MEMORY;
    }

    /* Good to copy */
    bytes_required = sizeof(CK_ATTRIBUTE) * ulCount;

    /* Set up the Copied Template */
    pCopy = (CK_ATTRIBUTE_PTR)pBuffer;

    /* Move to the data section */
    pBuffer += bytes_required;

    /* Copy the template */
    for (i = 0; i < ulCount; i++)
    {
        pCopy[i].type = pTemplate[i].type;

        /* Sanity check the input to see if we need to allocate any memory for this */
        if (pTemplate[i].pValue && pTemplate[i].ulValueLen)
        {
            pCopy[i].pValue = pBuffer;
            pCopy[i].ulValueLen = pTemplate[i].ulValueLen;
            pBuffer += pTemplate[i].ulValueLen;
            memcpy(pCopy[i].pValue, pTemplate[i].pValue, pTemplate[i].ulValueLen);
        }
        else
        {
            pCopy[i].pValue = NULL_PTR;
            pCopy[i].ulValueLen = 0;
        }
    }

    return CKR_OK;
}

static pkcs11_attrib_model_ptr pkcs11_find_attrib(const pkcs11_attrib_model_ptr pAttributeList, const CK_ULONG ulCount, const CK_ATTRIBUTE_PTR pTemplate)
{
    CK_ULONG i;
    pkcs11_attrib_model_ptr pAttribute = NULL_PTR;

    if (pAttributeList && ulCount && pTemplate)
    {
        for (i = 0; i < ulCount; i++)
        {
            pAttribute = &pAttributeList[i];
            if (pAttribute->type == pTemplate->type)
            {
                return pAttribute;
            }
        }
    }

    return NULL_PTR;
}

static pkcs11_attrib_model_ptr pkcs11_find_attrib_match(pkcs11_object_ptr pObject, const pkcs11_attrib_model_ptr pAttributeList, const CK_ULONG ulCount, const CK_ATTRIBUTE_PTR pTemplate)
{
    CK_BBOOL found = FALSE;
    pkcs11_attrib_model_ptr pAttribute = NULL_PTR;

    pAttribute = pkcs11_find_attrib(pAttributeList, ulCount, pTemplate);

    if (pAttribute && pObject)
    {
        CK_BBOOL must_full_match = FALSE;

        /* Special rules - only return CKO_HW_FEATURE (Sec 4.3.2) or CKO_MECHANISM (Sec 4.12.2) if the search attributes contain them */
        if (CKA_CLASS == pAttribute->type)
        {
            if ((CKO_HW_FEATURE == pObject->class_id) || (CKO_MECHANISM == pObject->class_id))
            {
                must_full_match = TRUE;
            }
        }

        /* Search criteria has a value to we must match it */
        if (pTemplate->pValue && pAttribute->func)
        {
            CK_ATTRIBUTE temp = { 0 };
#ifdef ATCA_NO_HEAP
            CK_UTF8CHAR buf[PKCS11_MAX_LABEL_SIZE];
            temp.pValue = buf;
            temp.ulValueLen = sizeof(buf);
#else
            temp.pValue = pkcs11_os_malloc(pTemplate->ulValueLen);
            temp.ulValueLen = pTemplate->ulValueLen;
#endif

            /* Get the attribute */
            if (!pAttribute->func(pObject, &temp))
            {
                if ((temp.ulValueLen == pTemplate->ulValueLen))
                {
                    if (!memcmp(temp.pValue, pTemplate->pValue, pTemplate->ulValueLen))
                    {
                        found = TRUE;
                    }
                }
            }
#ifndef ATCA_NO_HEAP
            if (temp.pValue)
            {
                pkcs11_os_free(temp.pValue);
            }
#endif
        }
        else if (!must_full_match)
        {
            found = TRUE;
        }
    }

    if (found)
    {
        return pAttribute;
    }

    return NULL_PTR;
}

static CK_OBJECT_HANDLE pkcs11_find_handle(const CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_ULONG_PTR index)
{
    CK_ULONG i;
    CK_ULONG j;
    CK_OBJECT_HANDLE rv = NULL_PTR;

    /* Finds the first or next match - Iterate through objects */
    for (i = (index) ? *index : 0; i < PKCS11_MAX_OBJECTS_ALLOWED; i++)
    {
        pkcs11_object_ptr pObject = pkcs11_object_cache[i].object;
        if (pObject)
        {
            /* Iterate through attribute list */
            for (j = 0; j < ulCount; j++)
            {
                /* See if a match failed */
                if (!pkcs11_find_attrib_match(pObject, (const pkcs11_attrib_model_ptr)pObject->attributes,
                                              pObject->count, &pTemplate[j]))
                {
                    break;
                }
            }
            if (j == ulCount)
            {
                /* Full match */
                if (ulCount)
                {
                    rv = pkcs11_object_cache[i].handle;
                    break;
                }
                else if ((CKO_HW_FEATURE != pObject->class_id) && (CKO_MECHANISM != pObject->class_id))
                {
                    /* Special condition where ulCount is zero we match all
                       objects except HW Features and Mechanisms */
                    rv = pkcs11_object_cache[i].handle;
                    break;
                }
            }
        }
    }

    if (index)
    {
        *index = i;
    }

    return rv;
}

CK_RV pkcs11_find_init(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
    pkcs11_session_ctx_ptr pSession;
    CK_ULONG index = 0;
    CK_RV rv;

    rv = pkcs11_init_check(NULL, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pTemplate && ulCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    pkcs11_debug_attributes(pTemplate, ulCount);

    /* Check Public/Private Session info - although I don't think we can actually
        get private unless we're using a shared key system - and that will only be
        for secured data and not key info */

    if (pkcs11_find_handle(pTemplate, ulCount, &index))
    {
        if (ulCount)
        {
            rv = pkcs11_find_copy_template(pkcs11_find_template_cache, PKCS11_SEARCH_CACHE_SIZE, pTemplate, ulCount);
            if (rv)
            {
                return rv;
            }
            pSession->attrib_list = (CK_ATTRIBUTE_PTR)pkcs11_find_template_cache;
        }
        else
        {
            pSession->attrib_list = NULL_PTR;
        }
        pSession->attrib_count = ulCount;
        pSession->object_index = index;
        pSession->object_count = 1;
        index++;
        while (pkcs11_find_handle(pSession->attrib_list, ulCount, &index))
        {
            pSession->object_count++;
            index++;
        }
    }

    return CKR_OK;
}

CK_RV pkcs11_find_continue(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE_PTR phObject, CK_ULONG ulMaxObjectCount, CK_ULONG_PTR pulObjectCount)
{
    pkcs11_session_ctx_ptr pSession;
    CK_ULONG i;
    CK_RV rv;

    rv = pkcs11_init_check(NULL, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!phObject || !pulObjectCount)
    {
        return CKR_ARGUMENTS_BAD;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    *pulObjectCount = (pSession->object_count < ulMaxObjectCount) ? pSession->object_count : ulMaxObjectCount;

    for (i = 0; i < *pulObjectCount; i++)
    {
        phObject[i] = pkcs11_find_handle(pSession->attrib_list, pSession->attrib_count, &pSession->object_index);
        if (!phObject[i])
        {
            pSession->object_count = 0;
            *pulObjectCount = i;
            break;
        }

        pSession->object_index++;

        if (pSession->object_count)
        {
            pSession->object_count--;
        }
    }

    return CKR_OK;
}

CK_RV pkcs11_find_finish(CK_SESSION_HANDLE hSession)
{
    pkcs11_session_ctx_ptr pSession;
    CK_RV rv;

    rv = pkcs11_init_check(NULL, FALSE);
    if (rv)
    {
        return rv;
    }

    rv = pkcs11_session_check(&pSession, hSession);
    if (rv)
    {
        return rv;
    }

    pSession->attrib_list = NULL_PTR;
    pSession->attrib_count = 0;
    pSession->object_count = 0;

    return CKR_OK;
}

CK_RV pkcs11_find_get_attribute(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
    pkcs11_lib_ctx_ptr pLibCtx;
    pkcs11_session_ctx_ptr pSession;
    pkcs11_object_ptr pObject;
    CK_ULONG i;
    CK_RV rv;

    rv = pkcs11_init_check(&pLibCtx, FALSE);
    if (rv)
    {
        return rv;
    }

    if (!pTemplate || !ulCount)
    {
        return CKR_ARGUMENTS_BAD;
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

    pkcs11_debug_attributes(pTemplate, ulCount);

    for (i = 0; i < ulCount; i++)
    {
        pkcs11_attrib_model_ptr pAttribute = pkcs11_find_attrib((const pkcs11_attrib_model_ptr)pObject->attributes,
                                                                pObject->count, &pTemplate[i]);

        if (!pAttribute)
        {
            /* 2. Otherwise, if the specified value for the object is invalid(the object does not possess such an
               attribute), then the ulValueLen field in that triple is modified to hold the value CK_UNAVAILABLE_INFORMATION. */

            pTemplate[i].ulValueLen = CK_UNAVAILABLE_INFORMATION;
            if (!rv)
            {
                rv = CKR_ATTRIBUTE_TYPE_INVALID;
            }
        }
        else if (pAttribute->func)
        {
            if (CKR_OK == pkcs11_lock_context(pLibCtx))
            {
                /* Attribute function found so try to execute it */
                CK_RV temp = pAttribute->func(pObject, &pTemplate[i]);
                if (!rv)
                {
                    rv = temp;
                }
                pkcs11_unlock_context(pLibCtx);
            }
        }
        else
        {
            /* Assume if there is no function for the attribute we're keeping it private */
            pTemplate[i].ulValueLen = CK_UNAVAILABLE_INFORMATION;
            if (!rv)
            {
                rv = CKR_ATTRIBUTE_SENSITIVE;
            }
        }
    }

    return rv;
}

/** @} */

