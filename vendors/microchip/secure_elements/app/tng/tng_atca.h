/**
 * \file
 * \brief TNG Helper Functions
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

#ifndef TNG_ATCA_H
#define TNG_ATCA_H

#include "atca_basic.h"
#include "atcacert/atcacert_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup tng_ TNG API (tng_)
 *
 * \brief These methods provide some convenience functions (mostly around
 *        certificates) for TNG devices, which currently include
 *        ATECC608A-MAHTN-T.
 *
   @{ */

/** \brief Helper function to iterate through all trust cert definitions
 *
 * \param[in] index Map index
 *
 * \return non-null value if success, otherwise NULL
 */
const atcacert_def_t* tng_map_get_device_cert_def(int index);

/** \brief Get the TNG device certificate definition.
 *
 * \param[out] cert_def  TNG device certificate defnition is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS tng_get_device_cert_def(const atcacert_def_t **cert_def);

/** \brief Uses GenKey command to calculate the public key from the primary
 *         device public key.
 *
 *  \param[out] public_key  Public key will be returned here. Format will be
 *                          the X and Y integers in big-endian format.
 *                          64 bytes for P256 curve.
 *
 *  \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS tng_get_device_pubkey(uint8_t *public_key);

/** @} */

#ifdef __cplusplus
}
#endif

#endif