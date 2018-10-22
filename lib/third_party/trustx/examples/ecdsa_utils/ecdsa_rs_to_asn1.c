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
* \file asn1_to_ecdsa_rs.c
*
* \brief   This file provides the example to convert asn1 encoding to raw r and s components of the ECDSA signature
* 
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"


// Encode two integers in DER format
// TAG + LENGTH needs 2 bytes
// if the highest bit of the integer is set we need an extra byte
#define  DER_OVERHEAD ((2 + 1) * 2)

optiga_lib_status_t ecdsa_rs_to_asn1(const uint8_t  *r, size_t r_len,
									 const uint8_t  *s, size_t s_len,
									  	   uint8_t  *asn_sig, size_t *asn_sig_len)
{
    uint32_t index;
	optiga_lib_status_t return_status = OPTIGA_LIB_ERROR;

	do {
		if(*asn_sig_len < (r_len + s_len + DER_OVERHEAD)) {
			// not enough space in output buffer
			break;
		}

		index = 0;

		// R component
		asn_sig[index + 0] = 0x02;
		asn_sig[index + 1] = 0x20;
		if (r[0] & 0x80)
		{
			asn_sig[index + 1] += 1;
			asn_sig[index + 2] =  0;
			index++;
		}
		memcpy(&asn_sig[index + 2], &r[0], r_len);
		index += r_len + 2;

		// S component
		asn_sig[index + 0] = 0x02;
		asn_sig[index + 1] = 0x20;
		if (s[0] & 0x80)
		{
			asn_sig[index + 1] += 1;
			asn_sig[index + 2] =  0;
			index++;
		}
		memcpy(&asn_sig[index + 2], &s[0], s_len);
		index += s_len + 2;

		*asn_sig_len = index; // Return total length of ASN.1-encoded data structure
		
		return_status = OPTIGA_LIB_SUCCESS;
		
	}while(FALSE);
       
	return return_status;
}

/**
* @}
*/
