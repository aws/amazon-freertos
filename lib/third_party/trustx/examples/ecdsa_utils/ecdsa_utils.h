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
* \file ecdsa_utils.h
*
* \brief   This file utilities to ECDSA signature encoding/decoding
*
*
* \ingroup
* @{
*/
#ifndef _H_ECDSA_UTILS_H_
#define _H_ECDSA_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief decodes two ASN.1 integers to the R and S components of a ECC signature
 * @param[in] asn1 buffer containing the ASN.1 encoded R and S values
 * @param[in] asn1_len length of the asn1 buffer
 * @param[out] rs buffer where to write the R and S values
 * @param[in,out] rs_len length of the rs buffer, contains the bytes written afterwards
 * @returns OPTIGA_LIB_SUCCESS on success, OPTIGA_LIB_ERROR else
 */
optiga_lib_status_t asn1_to_ecdsa_rs(const uint8_t * asn1, size_t asn1_len,
									 uint8_t * rs, size_t * rs_len);

/**
 * @brief Encodes the ECDSA signature components (r, s) in ASN.1 format.
 *
 * @param[in]   r            Component r of the ECDSA signature
 * @param[in]   r_len        Length of the r component of the ECDSA signature
 * @param[in]   s            Component s of the ECDSA signature
 * @param[in]   s_len        Length of the s component of the ECDSA signature
 * @param[out]  asn_sig      Buffer where the resulting ASN.1-encoded ECDSA signature will be copied into
 * @param[out]  asn_sig_len  Length of the actual data that was copied into the output buffer
 * @returns     OPTIGA_LIB_SUCCESS on success, OPTIGA_LIB_ERROR on error
 */
optiga_lib_status_t ecdsa_rs_to_asn1(const uint8_t  *r, size_t r_len,
									 const uint8_t  *s, size_t s_len,
									  	   uint8_t  *asn_sig, size_t *asn_sig_len);

#ifdef __cplusplus
}
#endif

#endif // _H_ECDSA_UTILS_H_

/**
* @}
*/
