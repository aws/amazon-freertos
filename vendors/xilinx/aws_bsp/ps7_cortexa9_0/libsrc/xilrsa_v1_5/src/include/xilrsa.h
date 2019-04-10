/******************************************************************************
*
* Copyright (C) 2014 - 17 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xilrsa.h
* @addtogroup xilrsa_apis	XilRSA APIs and Descriptions
* @{
* @cond xilrsa_internal
* This file contains the RSA algorithm functions
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.0   hk   27/01/14 First release
* 1.4   vns  07/06/17 Added dooxygen tags.
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___XIL_RSA_H___
#define ___XIL_RSA_H___

#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/

/*
 * Digit size selection (32 or 16-bit). If supported by the CPU/compiler,
 * 32-bit digits are approximately 4 times faster
 */

//#define RSA_DIGIT_16
#define RSA_DIGIT_32

/*
 * RSA loop unrolling selection
 * RSA main loop can be unrolled 2, 4 or 8 ways
 */
#define RSA_UNROLL	1

/*
 * Select if ARM-optimized code is to be used. Only GCC for ARM is supported
 */
//#define RSA_ARM_OPTIMIZED

/*
 * Check the compatibility of the selection
 */
#if defined(RSA_DIGIT_16) && defined(RSA_DIGIT_32)
	#error Please select a digit size
#endif
#if !defined(RSA_DIGIT_16) && !defined(RSA_DIGIT_32)
	#error Please select just one digit size
#endif
#if (!defined(__GNUC__) || !defined(__arm__)) && defined(RSA_ARM_OPTIMIZED)
	#error Assembly level code is only supported for the GCC/ARM combination
#endif
#if (RSA_UNROLL != 1) && (RSA_UNROLL != 2) && (RSA_UNROLL != 4) && (RSA_UNROLL != 8)
	#error Only 1, 2, 4, and 8 unrolling are supported
#endif

#ifdef RSA_DIGIT_16
#define RSA_DIGIT	unsigned short
#define RSA_SDIGIT	short
#define RSA_DDIGIT	unsigned long
#endif
#ifdef RSA_DIGIT_32
#define RSA_DIGIT	unsigned long
#define RSA_SDIGIT	long
#define RSA_DDIGIT	unsigned long long
#endif

#define RSA_NUMBER	RSA_DIGIT *
#define RSA_NBITS	2048
#define RSA_NDIGITS	(RSA_NBITS/(sizeof(RSA_DIGIT)*8))
#define RSA_NBYTES	(RSA_NDIGITS*sizeof(RSA_DIGIT))

/*
 * Double-digit to single digit conversion
 */
#define RSA_MSB(x)  (x >> (sizeof(RSA_DIGIT)*8))
#define RSA_LSB(x)  (x & (RSA_DIGIT)~0)

#define SHA_BLKSIZE		512
#define SHA_BLKBYTES	(SHA_BLKSIZE/8)
#define SHA_BLKWORDS	(SHA_BLKBYTES/4)

#define SHA_VALSIZE		256
#define SHA_VALBYTES	(SHA_VALSIZE/8)
#define SHA_VALWORDS	(SHA_VALBYTES/4)

/*
 * SHA-256 context structure
 * Includes SHA-256 state, coalescing buffer to collect the processed strings, and
 * total byte length counter (used both to manage the buffer and for padding)
 */
 //! [sha2_context]
typedef struct
{
	unsigned int state[8];
	unsigned char buffer[SHA_BLKBYTES];
	unsigned long long bytes;
} sha2_context;
//! [sha2_context]
/** @}
@endcond */

/*
 * RSA-2048 user interfaces
 */
/*****************************************************************************/
/**
 * @brief
 * This function is used to encrypt the data using 2048 bit private key.
 *
 * @param	modular		A char pointer which contains the key modulus
 * @param	modular_ext	A char pointer which contains the key modulus
 *		extension
 * @param	exponent	A char pointer which contains the private key
 *		exponent
 * @param	result		A char pointer which contains the encrypted data
 *
 * @return	None
 *
 ******************************************************************************/
void rsa2048_exp(const unsigned char *base, const unsigned char * modular,
		const unsigned char *modular_ext, const unsigned char *exponent,
		unsigned char *result);
/*****************************************************************************/
/**
 * @brief
 * This function is used to decrypt the data using 2048 bit public key
 *
 * @param	a	RSA_NUMBER containing the decrypted data.
 * @param	x	RSA_NUMBER containing the input data
 * @param	e	Unsigned number containing the public key exponent
 * @param	m	RSA_NUMBER containing the public key modulus
 * @param	rrm	RSA_NUMBER containing the public key modulus extension.
 *
 * @return	None
 *
 ******************************************************************************/
void rsa2048_pubexp(RSA_NUMBER a, RSA_NUMBER x,
		unsigned long e, RSA_NUMBER m, RSA_NUMBER rrm);

/*
 * SHA-256 user interfaces
 */
/*****************************************************************************/
/**
 * @brief
 * This function calculates the hash for the input data using SHA-256
 * algorithm. This function internally calls the sha2_init, updates and
 * finishes functions and updates the result.
 *
 * @param	In	Char pointer which contains the input data.
 * @param	Size	Length of the input data
 * @param	Out	Pointer to location where resulting hash will be
 *		written.
 *
 * @return	None
 *
 ******************************************************************************/
void sha_256(const unsigned char *in, const unsigned int size, unsigned char *out);
/*****************************************************************************/
/**
 * @brief
 * This function initializes the SHA2 context.
 *
 * @param	ctx 	Pointer to sha2_context structure that stores status and
 *		buffer.
 *
 * @return	None
 *
 ******************************************************************************/
void sha2_starts(sha2_context *ctx);
/*****************************************************************************/
/**
 * @brief
 * This function adds the input data to SHA256 calculation.
 *
 * @param	ctx 	Pointer to sha2_context structure that stores status and
 * 		buffer.
 * @param	input 	Pointer to the data to add.
 * @param	Out 	Length of the input data.
 *
 * @return	None
 *
 ******************************************************************************/
void sha2_update(sha2_context *ctx, unsigned char* input, unsigned int ilen);
/*****************************************************************************/
/**
 * @brief
 * This function finishes the SHA calculation.
 *
 * @param	ctx 	Pointer to sha2_context structure that stores status and
 *		buffer.
 * @param	output 	Pointer to the calculated hash data.
 *
 * @return	None
 *
 *
 ******************************************************************************/
void sha2_finish(sha2_context *ctx, unsigned char* output);

/*
 * Preprocessing interface (pre-computation of R*R mod M)
 */
/**@cond xilrsa_internal */
void modular_ext(const unsigned char *modular, unsigned char *res);
/** @}
@endcond */

#ifdef __cplusplus
}
#endif

#endif /* ___XIL_RSA_H___ */
/** @} */


