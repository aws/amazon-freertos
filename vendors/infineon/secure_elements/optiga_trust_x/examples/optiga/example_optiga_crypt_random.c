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
* \file example_optiga_crypt_get_random.c
*
* \brief   This file provides the example for generation of random using 
*          #optiga_crypt_random.
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"

/**
 * The below example demonstrates the generation of random using OPTIGA.
 *
 * Example for #optiga_crypt_random
 *
 */
optiga_lib_status_t example_optiga_crypt_random(void)
{
    uint8_t random_data_buffer [32];

    optiga_lib_status_t return_status;

    do
    {
        /**
         * Generate Random -
         *       - Specify the Random type as TRNG
         */
        return_status = optiga_crypt_random(OPTIGA_RNG_TYPE_TRNG, 
                                            random_data_buffer,
                                            sizeof(random_data_buffer));

        if (OPTIGA_LIB_SUCCESS != return_status)
        {
            break;
        }

    } while(FALSE);

    return return_status;
}
/**
* @}
*/
