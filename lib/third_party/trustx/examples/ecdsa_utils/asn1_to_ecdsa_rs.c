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

optiga_lib_status_t asn1_to_ecdsa_rs(const uint8_t * asn1, size_t asn1_len,
									 uint8_t * rs, size_t * rs_len)
{
	optiga_lib_status_t return_status = OPTIGA_LIB_ERROR;
	const uint8_t * cur = asn1;
	// points to first invalid mem-location
	const uint8_t * end = asn1 + asn1_len;
	
	do {
		if(asn1 == NULL || rs == NULL || rs_len == NULL) {
			break;
		}

		if(asn1_len == 0 || *rs_len == 0) {
			break;
		}

		if(*cur != 0x02) {
			// wrong tag type
			break;
		}

		if((cur + 2) >= end) {
			// prevented out-of-bounds read
			break;
		}
		
		// move to length value
		cur++;
		uint8_t r_len = *cur;

		// move to first data value
		cur++;

		// check for stuffing bits
		if(*cur == 0x00) {
			cur++;
			r_len--;
		}

		// check for out-of-bounds read
		if((cur + r_len) >= end) {
			// prevented out-of-bounds read
			break;
		}
		// check for out-of-bounds write
		if((rs + r_len) > (rs + *rs_len)) {
			// prevented out-of-bounds write
			break;
		}
		
		// copy R component to output
		memcpy(rs, cur, r_len);

		// move to next tag
		cur += r_len;

		if(*cur != 0x02) {
			// wrong tag type
			break;
		}

		if((cur + 2) >= end) {
			// prevented out-of-bounds read
			break;
		}
		cur++;
		uint8_t s_len = *cur;
		cur++;

		if(*cur == 0x00) {
			cur++;
			s_len--;
		}

		// check for out-of-bounds read
		if((cur + s_len) > end) {
			// prevented out-of-bounds read
			break;
		}
		// check for out-of-bounds write
		if((rs + r_len + s_len) > (rs + *rs_len)) {
			// prevented out-of-bounds write
			break;
		}

		memcpy(rs + r_len, cur, s_len);
		
		return_status = OPTIGA_LIB_SUCCESS;
		
	}while(FALSE);


    return return_status;
}

/**
* @}
*/
