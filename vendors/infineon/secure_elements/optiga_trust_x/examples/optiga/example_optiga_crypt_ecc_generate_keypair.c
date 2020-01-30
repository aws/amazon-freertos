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
* \file example_optiga_crypt_ecc_generate_key_pair.c
*
* \brief   This file provides the example for generation of ecc keypair using 
*          optiga_crypt_ecc_generate_keypair.
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"


/**
 * The below example demonstrates the generation of 
 * ECC Key pair using #optiga_crypt_ecc_generate_keypair with a session based approach.
 *
 */
optiga_lib_status_t example_optiga_crypt_ecc_generate_keypair(void)
{
    optiga_lib_status_t return_status;
    optiga_key_id_t optiga_key_id;

    //To store the generated public key as part of Generate key pair
    uint8_t public_key [100];
    uint16_t public_key_length = sizeof(public_key);

    do
    {
        /**
         * Generate ECC Key pair  
         *       - Use ECC NIST P 256 Curve
         *       - Specify the Key Usage (Key Agreement or Sign based on requirement)
         *       - Store the Private key in OPTIGA Key store
         *       - Export Public Key
         */
        optiga_key_id = OPTIGA_KEY_STORE_ID_E0F1;
        //for Session based, use OPTIGA_KEY_ID_SESSION_BASED as key id as shown below.
        //optiga_key_id = OPTIGA_KEY_ID_SESSION_BASED;
        return_status = optiga_crypt_ecc_generate_keypair(OPTIGA_ECC_NIST_P_256,
                                                          (uint8_t)OPTIGA_KEY_USAGE_SIGN,
                                                          FALSE,
														  &optiga_key_id,
                                                          public_key,
                                                          &public_key_length);
        if (OPTIGA_LIB_SUCCESS != return_status)
        {
			//Key pair generation failed
            break;
        }

    } while(FALSE);

    return return_status;
}
/**
* @}
*/
