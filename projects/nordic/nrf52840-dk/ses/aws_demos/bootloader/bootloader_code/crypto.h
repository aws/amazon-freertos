#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <stdint.h>
#include "sdk_errors.h"

ret_code_t xCryptoInit();
ret_code_t xCryptoUnInit();

/**
 * @brief Verifies the image located at the specific address
 * @param Address of the image to be verified
 * @return NRF_SUCCESS if the image is correct, NRF_ERROR_CRYPTO_ECDSA_INVALID_SIGNATURE otherwise
 */
ret_code_t xVerifyImageSignature( uint8_t * pusImageStart );

#endif /* ifndef __CRYPTO_H */
