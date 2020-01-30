/**
 * \file
 * \brief PKCS11 Library Operating System Abstraction Functions
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

#include "pkcs11_os.h"
#include "pkcs11_util.h"

/**
 * \defgroup pkcs11 OS Abstraction (pkcs11_so_)
   @{ */

/**
 * \brief Convert HAL return codes to PKCS11 return values
 * \param[IN] status Code returned by hal call
 */
static CK_RV pkcs11_os_convert_rv(ATCA_STATUS status)
{
    return (ATCA_FUNC_FAIL == status) ? CKR_CANT_LOCK : pkcs11_util_convert_rv(status);
}

/**
 * \brief Application callback for creating a mutex object
 * \param[IN/OUT] ppMutex location to receive ptr to mutex
 */
CK_RV pkcs11_os_create_mutex(CK_VOID_PTR_PTR ppMutex)
{
    return pkcs11_os_convert_rv(hal_create_mutex(ppMutex, "atpkcs11"));
}

/*
 * \brief Application callback for destroying a mutex object
 * \param[IN] pMutex pointer to mutex
 */
CK_RV pkcs11_os_destroy_mutex(CK_VOID_PTR pMutex)
{
    return pkcs11_os_convert_rv(hal_destroy_mutex(pMutex));
}

/*
 * \brief Application callback for locking a mutex
 * \param[IN] pMutex pointer to mutex
 */
CK_RV pkcs11_os_lock_mutex(CK_VOID_PTR pMutex)
{
    return pkcs11_os_convert_rv(hal_lock_mutex(pMutex));
}

/*
 * \brief Application callback for unlocking a mutex
 * \param[IN] pMutex pointer to mutex
 */
CK_RV pkcs11_os_unlock_mutex(CK_VOID_PTR pMutex)
{
    return pkcs11_os_convert_rv(hal_unlock_mutex(pMutex));
}

/** @} */
