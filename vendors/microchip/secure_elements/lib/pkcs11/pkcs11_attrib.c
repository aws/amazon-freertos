/**
 * \file
 * \brief PKCS11 Library Object Attributes Handling
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
#include "pkcs11_attrib.h"
#include "cryptoauthlib.h"

/**
 * \defgroup pkcs11 Attributes (pkcs11_attrib_)
   @{ */


/**
 * \brief Perform the nessasary checks and copy data into an attribute structure
 *
 * The ulValueLen field is modified to hold the exact length of the specified attribute for the object.
 * In the special case of an attribute whose value is an array of attributes, for example CKA_WRAP_TEMPLATE, where
 * it is passed in with pValue not NULL, then if the pValue of elements within the array is NULL_PTR then the
 * ulValueLen of elements within the array will be set to the required length. If the pValue of elements within the
 * array is not NULL_PTR, then the ulValueLen element of attributes within the array MUST reflect the space that
 * the corresponding pValue points to, and pValue is filled in if there is sufficient room.Therefore it is
 * important to initialize the contents of a buffer before calling C_GetAttributeValue to get such an array value.
 * If any ulValueLen within the array isn't large enough, it will be set to CK_UNAVAILABLE_INFORMATION and the
 * function will return CKR_BUFFER_TOO_SMALL, as it does if an attribute in the pTemplate argument has ulValueLen
 * too small Note that any attribute whose value is an array of attributes is identifiable by virtue of the
 * attribute type having the CKF_ARRAY_ATTRIBUTE bit set.
 */
CK_RV pkcs11_attrib_fill(CK_ATTRIBUTE_PTR pAttribute, const CK_VOID_PTR pData, const CK_ULONG ulSize)
{
    if (!pAttribute)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (pAttribute->pValue)
    {
        /* 1. If the specified attribute(i.e., the attribute specified by the type field) for the object cannot be revealed
           because the object is sensitive or unextractable, then the ulValueLen field in that triple is modified to
           hold the value CK_UNAVAILABLE_INFORMATION. //if (!rv) rv = CKR_ATTRIBUTE_SENSITIVE; */

        if (ulSize <= pAttribute->ulValueLen)
        {
            /* 4. Otherwise, if the length specified in ulValueLen is large enough to hold the value of the specified
               attribute for the object, then that attribute is copied into the buffer located at pValue */
            if (pData)
            {
                memcpy(pAttribute->pValue, pData, ulSize);
            }
            else
            {
                /* This case isn't covered by the PKCS11 specification as it always assumes that the internals of the
                   library should be rational about providing some data if an attribute is matched. */
                memset(pAttribute->pValue, 0, pAttribute->ulValueLen);
            }
        }
        else
        {
            /* 5. Otherwise, the ulValueLen field is modified to hold the value CK_UNAVAILABLE_INFORMATION. */
            pAttribute->ulValueLen = CK_UNAVAILABLE_INFORMATION;
            return CKR_BUFFER_TOO_SMALL;
        }
    }

    pAttribute->ulValueLen = ulSize;

    return CKR_OK;
}

/**
 * \brief Helper function to write a numerical value to an attribute buffer
 */
CK_RV pkcs11_attrib_value(CK_ATTRIBUTE_PTR pAttribute, const CK_ULONG ulValue, const CK_ULONG ulSize)
{
    return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)&ulValue, ulSize);
}

static const CK_BBOOL cbFalse = CK_FALSE;
CK_RV pkcs11_attrib_false(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    ((void)pObject);
    return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)&cbFalse, sizeof(cbFalse));
}

static const CK_BBOOL cbTrue = CK_TRUE;
CK_RV pkcs11_attrib_true(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    ((void)pObject);
    return pkcs11_attrib_fill(pAttribute, (const CK_VOID_PTR)&cbTrue, sizeof(cbTrue));
}

CK_RV pkcs11_attrib_empty(const CK_VOID_PTR pObject, CK_ATTRIBUTE_PTR pAttribute)
{
    ((void)pObject);
    return pkcs11_attrib_fill(pAttribute, NULL, 0);
}


/** @} */

