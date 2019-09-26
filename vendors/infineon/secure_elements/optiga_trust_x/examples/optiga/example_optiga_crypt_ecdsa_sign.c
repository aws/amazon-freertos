/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file example_optiga_crypt_ecdsa_sign.c
*
* \brief   This file provides the example for ECDSA Sign operation using #optiga_crypt_ecdsa_sign.
* 
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"

/**
 * The below example demonstrates the signing of digest using
 * the Private key in OPTIGA Key store.
 *
 * Example for #optiga_crypt_ecdsa_sign
 *
 */
optiga_lib_status_t example_optiga_crypt_ecdsa_sign(void)
{
    uint8_t digest [] = {
        // Digest to be signed.. 
        // Size of digest to be chosen based on Curve
        0x61, 0xC7, 0xDE, 0xF9, 0x0F, 0xD5, 0xCD, 0x7A, 
        0x8B, 0x7A, 0x36, 0x41, 0x04, 0xE0, 0x0D, 0x82, 
        0x38, 0x46, 0xBF, 0xB7, 0x70, 0xEE, 0xBF, 0x8F, 
        0x40, 0x25, 0x2E, 0x0A, 0x21, 0x42, 0xAF, 0x9C,
    };   

    uint8_t signature [80];     //To store the signture generated
    uint16_t signature_length = sizeof(signature);

    optiga_lib_status_t return_status;

    do
    {

        /**
         * Sign the digest -
         *       - Use Private key from Key Store ID E0F0 
         */
        return_status = optiga_crypt_ecdsa_sign(digest,
                                                sizeof(digest),
												OPTIGA_KEY_STORE_ID_E0F0,
                                                signature, 
                                                &signature_length);

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
			// Signature generation failed
            break;
        }

    } while(FALSE);

    return return_status;
}

/**
* @}
*/
