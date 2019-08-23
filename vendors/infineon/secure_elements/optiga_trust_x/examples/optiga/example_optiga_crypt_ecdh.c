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
* \file example_optiga_crypt_ecdh.c
*
* \brief    This file provides the example for ECDH operation using #optiga_crypt_ecdh.
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"


// Peer public key details for the ECDH operation
static uint8_t peer_public_key [] = 
{
    //Bit string format
    0x03,
        //Remaining length
        0x42,
            //Unused bits
            0x00,
            //Compression format
            0x04,
            //Public Key
            0x94, 0x89, 0x2F, 0x09, 0xEA, 0x4E, 0xCA, 0xBC, 0x6A, 0x4E, 0xF2, 0x06, 0x36, 0x26, 0xE0, 0x5D, 
            0xE0, 0xD5, 0xF9, 0x77, 0xEA, 0xC3, 0xB2, 0x70, 0xAC, 0xE2, 0x19, 0x00, 0xF5, 0xDB, 0x56, 0xE7, 
            0x37, 0xBB, 0xBE, 0x46, 0xE4, 0x49, 0x76, 0x38, 0x25, 0xB5, 0xF8, 0x94, 0x74, 0x9E, 0x1A, 0xB6, 
            0x5A, 0xF1, 0x29, 0xD7, 0x3A, 0xB6, 0x9B, 0x80, 0xAC, 0xC5, 0xE1, 0xC3, 0x10, 0xF2, 0x16, 0xC6,             
};
static public_key_from_host_t peer_public_key_details =
{
    (uint8_t *)&peer_public_key,
    sizeof(peer_public_key),
    (uint8_t)OPTIGA_ECC_NIST_P_256,
};

/**
 * The below example demonstrates the generation of 
 * shared secret using #optiga_crypt_ecdh with a session based approach.
 *
 */
optiga_lib_status_t example_optiga_crypt_ecdh(void)
{
    optiga_lib_status_t return_status;
    optiga_key_id_t optiga_key_id;

    //To store the generated public key as part of Generate key pair
    uint8_t public_key [100];
    uint16_t public_key_length = sizeof(public_key);

    //To store the generated shared secret as part of ECDH
    uint8_t shared_secret [32];

    do
    {
        /**
         * Generate ECC Key pair - To use the private key with ECDH in the next step 
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage as Key Agreement
         *       - Store the Private key with in OPTIGA Session
         *       - Export Public Key
         */
        optiga_key_id = OPTIGA_SESSION_ID_E100;
        return_status = optiga_crypt_ecc_generate_keypair(OPTIGA_ECC_NIST_P_256,
                                                          (uint8_t)OPTIGA_KEY_USAGE_KEY_AGREEMENT,
                                                          FALSE,
                                                          &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
			// Key pair generation failed
            break;
        }


        /**
         * Perform ECDH using the Peer Public key
         *       - Use ECC NIST P 256 Curve
         *       - Provide the peer public key details
         *       - Export the generated shared secret
         */
        return_status = optiga_crypt_ecdh(optiga_key_id,
                                          &peer_public_key_details, 
                                          TRUE,
                                          shared_secret);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
			//ECDH Operation failed.
            break;
        }


    } while(FALSE);

    return return_status;
}

/**
* @}
*/
