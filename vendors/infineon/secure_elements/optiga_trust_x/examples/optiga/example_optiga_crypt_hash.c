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
* \file example_optiga_crypt_calc_hash.c
*
* \brief    This file provides the example for hashing operations using 
*           #optiga_crypt_hash_start, #optiga_crypt_hash_update and 
*           #optiga_crypt_hash_finalize.
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"

/**
 * Prepare the hash context
 */
#define OPTIGA_HASH_CONTEXT_INIT(hash_context,p_context_buffer,context_buffer_size,hash_type) \
{                                                               \
    hash_context.context_buffer = p_context_buffer;             \
    hash_context.context_buffer_length = context_buffer_size;   \
    hash_context.hash_algo = hash_type;                         \
}

/**
 * The below example demonstrates the generation of digest using
 * optiga_crypt_hash_xxxx operations.
 *
 * Example for #optiga_crypt_hash_start, #optiga_crypt_hash_update, 
 * #optiga_crypt_hash_finalize
 *
 */
optiga_lib_status_t example_optiga_crypt_hash(void)
{
    optiga_lib_status_t return_status;

    uint8_t hash_context_buffer [130];
    optiga_hash_context_t hash_context;

    uint8_t data_to_hash [] = {"OPITGA, Infineon Technologies AG"};
    hash_data_from_host_t hash_data_host;

    uint8_t digest [32];

    do
    {
        OPTIGA_HASH_CONTEXT_INIT(hash_context,hash_context_buffer,  \
                                 sizeof(hash_context_buffer),OPTIGA_HASH_TYPE_SHA_256);

        //Hash start
        return_status = optiga_crypt_hash_start(&hash_context);
        if(return_status != OPTIGA_LIB_SUCCESS)
        {
            break;
        }

        //Hash update   
        hash_data_host.buffer = data_to_hash;
        hash_data_host.length = sizeof(data_to_hash);

        return_status = optiga_crypt_hash_update(&hash_context,
        		                                 // OPTIGA_CRYPT_OID_DATA stands for OID
        										 OPTIGA_CRYPT_HOST_DATA,
                                                 &hash_data_host);
        if(return_status != OPTIGA_LIB_SUCCESS)
        {
            break;
        }

        // hash finalize
        return_status = optiga_crypt_hash_finalize(&hash_context,
                                                   digest);

        if(return_status != OPTIGA_LIB_SUCCESS)
        {
            break;
        }


    } while(FALSE);

    return return_status;
}
/**
* @}
*/
