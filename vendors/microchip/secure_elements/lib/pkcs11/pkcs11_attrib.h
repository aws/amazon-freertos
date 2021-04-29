/**
 * \file
 * \brief PKCS11 Library Object Attribute Handling
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
