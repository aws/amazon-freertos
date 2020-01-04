#ifndef TNG_ATCA_H
#define TNG_ATCA_H

#include "basic/atca_basic.h"

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

#define TNG22_PRIMARY_KEY_SLOT 0
#define TNGTN_PRIMARY_KEY_SLOT 1

typedef enum
{
    TNGTYPE_UNKNOWN,
    TNGTYPE_22,
    TNGTYPE_TN
} tng_type_t;

/** \brief Get the type of TNG device.
 *
 * \param[out] type  TNG device type is returned here.
 *
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS tng_get_type(tng_type_t* type);

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