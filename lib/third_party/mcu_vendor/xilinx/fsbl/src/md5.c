/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/*****************************************************************************/
/**
*
* @file md5.c
*
* Contains code to calculate checksum using md5 algorithm
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 5.00a sgd	05/17/13 Initial release
*
*
* </pre>
*
* @note
*
******************************************************************************/
/****************************** Include Files *********************************/

#include "md5.h"

/******************************************************************************/
/**
*
* This function sets the memory
*
* @param	dest
*
* @param	ch
*
* @param	count
*
* @return	None
*
* @note		None
*
****************************************************************************/
inline void * MD5Memset( void *dest, int	ch, u32	count )
{
	register char *dst8 = (char*)dest;

	while( count-- )
		*dst8++ = ch;

	return dest;
}

/******************************************************************************/
/**
*
* This function copy the memory
*
* @param	dest
*
* @param	ch
*
* @param	count
*
* @return	None
*
* @note		None
*
****************************************************************************/
inline void * MD5Memcpy( void *dest, const void *src,
		 	 u32 count, boolean	doByteSwap )
{
	register char * dst8 = (char*)dest;
	register char * src8 = (char*)src;
	
	if( doByteSwap == FALSE ) {
		while( count-- )
			*dst8++ = *src8++;
	} else {
		count /= sizeof( u32 );
		
		while( count-- ) {
			dst8[ 0 ] = src8[ 3 ];
			dst8[ 1 ] = src8[ 2 ];
			dst8[ 2 ] = src8[ 1 ];
			dst8[ 3 ] = src8[ 0 ];
			
			dst8 += 4;
			src8 += 4;
		}
	}
	
	return dest;
}

/******************************************************************************/
/**
*
* This function is the core of the MD5 algorithm,
* this alters an existing MD5 hash to
* reflect the addition of 16 longwords of new data. MD5Update blocks
* the data and converts bytes into longwords for this routine.
*
* Use binary integer part of the sine of integers (Radians) as constants.
* Calculated as:
*
* for( i = 0; i < 63; i++ )
*     k[ i ] := floor( abs( sin( i + 1 ) ) × pow( 2, 32 ) )
*
* Following number is the per-round shift amount.
*
* @param	dest
*
* @param	ch
*
* @param	count
*
* @return	None
*
* @note		None
*
****************************************************************************/
void MD5Transform( u32 *buffer, u32 *intermediate )
{
	register u32 a, b, c, d;
	
	a = buffer[ 0 ];
	b = buffer[ 1 ];
	c = buffer[ 2 ];
	d = buffer[ 3 ];

	MD5_STEP( F1, a, b, c, d, intermediate[  0 ] + 0xd76aa478,  7 );
	MD5_STEP( F1, d, a, b, c, intermediate[  1 ] + 0xe8c7b756, 12 );
	MD5_STEP( F1, c, d, a, b, intermediate[  2 ] + 0x242070db, 17 );
	MD5_STEP( F1, b, c, d, a, intermediate[  3 ] + 0xc1bdceee, 22 );
	MD5_STEP( F1, a, b, c, d, intermediate[  4 ] + 0xf57c0faf,  7 );
	MD5_STEP( F1, d, a, b, c, intermediate[  5 ] + 0x4787c62a, 12 );
	MD5_STEP( F1, c, d, a, b, intermediate[  6 ] + 0xa8304613, 17 );
	MD5_STEP( F1, b, c, d, a, intermediate[  7 ] + 0xfd469501, 22 );
	MD5_STEP( F1, a, b, c, d, intermediate[  8 ] + 0x698098d8,  7 );
	MD5_STEP( F1, d, a, b, c, intermediate[  9 ] + 0x8b44f7af, 12 );
	MD5_STEP( F1, c, d, a, b, intermediate[ 10 ] + 0xffff5bb1, 17 );
	MD5_STEP( F1, b, c, d, a, intermediate[ 11 ] + 0x895cd7be, 22 );
	MD5_STEP( F1, a, b, c, d, intermediate[ 12 ] + 0x6b901122,  7 );
	MD5_STEP( F1, d, a, b, c, intermediate[ 13 ] + 0xfd987193, 12 );
	MD5_STEP( F1, c, d, a, b, intermediate[ 14 ] + 0xa679438e, 17 );
	MD5_STEP( F1, b, c, d, a, intermediate[ 15 ] + 0x49b40821, 22 );
	
	MD5_STEP( F2, a, b, c, d, intermediate[  1 ] + 0xf61e2562,  5 );
	MD5_STEP( F2, d, a, b, c, intermediate[  6 ] + 0xc040b340,  9 );
	MD5_STEP( F2, c, d, a, b, intermediate[ 11 ] + 0x265e5a51, 14 );
	MD5_STEP( F2, b, c, d, a, intermediate[  0 ] + 0xe9b6c7aa, 20 );
	MD5_STEP( F2, a, b, c, d, intermediate[  5 ] + 0xd62f105d,  5 );
	MD5_STEP( F2, d, a, b, c, intermediate[ 10 ] + 0x02441453,  9 );
	MD5_STEP( F2, c, d, a, b, intermediate[ 15 ] + 0xd8a1e681, 14 );
	MD5_STEP( F2, b, c, d, a, intermediate[  4 ] + 0xe7d3fbc8, 20 );
	MD5_STEP( F2, a, b, c, d, intermediate[  9 ] + 0x21e1cde6,  5 );
	MD5_STEP( F2, d, a, b, c, intermediate[ 14 ] + 0xc33707d6,  9 );
	MD5_STEP( F2, c, d, a, b, intermediate[  3 ] + 0xf4d50d87, 14 );
	MD5_STEP( F2, b, c, d, a, intermediate[  8 ] + 0x455a14ed, 20 );
	MD5_STEP( F2, a, b, c, d, intermediate[ 13 ] + 0xa9e3e905,  5 );
	MD5_STEP( F2, d, a, b, c, intermediate[  2 ] + 0xfcefa3f8,  9 );
	MD5_STEP( F2, c, d, a, b, intermediate[  7 ] + 0x676f02d9, 14 );
	MD5_STEP( F2, b, c, d, a, intermediate[ 12 ] + 0x8d2a4c8a, 20 );
	
	MD5_STEP( F3, a, b, c, d, intermediate[  5 ] + 0xfffa3942,  4 );
	MD5_STEP( F3, d, a, b, c, intermediate[  8 ] + 0x8771f681, 11 );
	MD5_STEP( F3, c, d, a, b, intermediate[ 11 ] + 0x6d9d6122, 16 );
	MD5_STEP( F3, b, c, d, a, intermediate[ 14 ] + 0xfde5380c, 23 );
	MD5_STEP( F3, a, b, c, d, intermediate[  1 ] + 0xa4beea44,  4 );
	MD5_STEP( F3, d, a, b, c, intermediate[  4 ] + 0x4bdecfa9, 11 );
	MD5_STEP( F3, c, d, a, b, intermediate[  7 ] + 0xf6bb4b60, 16 );
	MD5_STEP( F3, b, c, d, a, intermediate[ 10 ] + 0xbebfbc70, 23 );
	MD5_STEP( F3, a, b, c, d, intermediate[ 13 ] + 0x289b7ec6,  4 );
	MD5_STEP( F3, d, a, b, c, intermediate[  0 ] + 0xeaa127fa, 11 );
	MD5_STEP( F3, c, d, a, b, intermediate[  3 ] + 0xd4ef3085, 16 );
	MD5_STEP( F3, b, c, d, a, intermediate[  6 ] + 0x04881d05, 23 );
	MD5_STEP( F3, a, b, c, d, intermediate[  9 ] + 0xd9d4d039,  4 );
	MD5_STEP( F3, d, a, b, c, intermediate[ 12 ] + 0xe6db99e5, 11 );
	MD5_STEP( F3, c, d, a, b, intermediate[ 15 ] + 0x1fa27cf8, 16 );
	MD5_STEP( F3, b, c, d, a, intermediate[  2 ] + 0xc4ac5665, 23 );
	
	MD5_STEP( F4, a, b, c, d, intermediate[  0 ] + 0xf4292244,  6 );
	MD5_STEP( F4, d, a, b, c, intermediate[  7 ] + 0x432aff97, 10 );
	MD5_STEP( F4, c, d, a, b, intermediate[ 14 ] + 0xab9423a7, 15 );
	MD5_STEP( F4, b, c, d, a, intermediate[  5 ] + 0xfc93a039, 21 );
	MD5_STEP( F4, a, b, c, d, intermediate[ 12 ] + 0x655b59c3,  6 );
	MD5_STEP( F4, d, a, b, c, intermediate[  3 ] + 0x8f0ccc92, 10 );
	MD5_STEP( F4, c, d, a, b, intermediate[ 10 ] + 0xffeff47d, 15 );
	MD5_STEP( F4, b, c, d, a, intermediate[  1 ] + 0x85845dd1, 21 );
	MD5_STEP( F4, a, b, c, d, intermediate[  8 ] + 0x6fa87e4f,  6 );
	MD5_STEP( F4, d, a, b, c, intermediate[ 15 ] + 0xfe2ce6e0, 10 );
	MD5_STEP( F4, c, d, a, b, intermediate[  6 ] + 0xa3014314, 15 );
	MD5_STEP( F4, b, c, d, a, intermediate[ 13 ] + 0x4e0811a1, 21 );
	MD5_STEP( F4, a, b, c, d, intermediate[  4 ] + 0xf7537e82,  6 );
	MD5_STEP( F4, d, a, b, c, intermediate[ 11 ] + 0xbd3af235, 10 );
	MD5_STEP( F4, c, d, a, b, intermediate[  2 ] + 0x2ad7d2bb, 15 );
	MD5_STEP( F4, b, c, d, a, intermediate[  9 ] + 0xeb86d391, 21 );

	buffer[ 0 ] += a;
	buffer[ 1 ] += b;
	buffer[ 2 ] += c;
	buffer[ 3 ] += d;
	
}

/******************************************************************************/
/**
*
* This function Start MD5 accumulation
* Set bit count to 0 and buffer to mysterious initialization constants
*
* @param
*
* @return	None
*
* @note		None
*
****************************************************************************/
inline void MD5Init( MD5Context *context )
{
	
	context->buffer[ 0 ] = 0x67452301;
	context->buffer[ 1 ] = 0xefcdab89;
	context->buffer[ 2 ] = 0x98badcfe;
	context->buffer[ 3 ] = 0x10325476;

	context->bits[ 0 ] = 0;
	context->bits[ 1 ] = 0;
	
}


/******************************************************************************/
/**
*
* This function updates context to reflect the concatenation of another
* buffer full of bytes
*
* @param
*
* @param
*
* @param
*
* @param
*
* @return	None
*
* @note		None
*
****************************************************************************/
inline void MD5Update( MD5Context *context, u8 *buffer,
		   u32 len, boolean	doByteSwap )
{
	register u32	temp;
	register u8 *	p;
	
	/*
	 * Update bitcount
	 */

	temp = context->bits[ 0 ];
	
	if( ( context->bits[ 0 ] = temp + ( (u32)len << 3 ) ) < temp ) {
		/*
		 * Carry from low to high
		 */
		context->bits[ 1 ]++;
	}
		
	context->bits[ 1 ] += len >> 29;
	
	/*
	 * Bytes already in shsInfo->data
	 */
	
	temp = ( temp >> 3 ) & 0x3f;

	/*
	 * Handle any leading odd-sized chunks
	 */

	if( temp ) {
		p = (u8 *)context->intermediate + temp;

		temp = MD5_SIGNATURE_BYTE_SIZE - temp;
		
		if( len < temp ) {
			MD5Memcpy( p, buffer, len, doByteSwap );
			return;
		}
		
		MD5Memcpy( p, buffer, temp, doByteSwap );
		
		MD5Transform( context->buffer, (u32 *)context->intermediate );
		
		buffer += temp;
		len    -= temp;
		
	}
		
	/*
	 * Process data in 64-byte, 512 bit, chunks
	 */

	while( len >= MD5_SIGNATURE_BYTE_SIZE ) {
		MD5Memcpy( context->intermediate, buffer, MD5_SIGNATURE_BYTE_SIZE,
				 doByteSwap );
		
		MD5Transform( context->buffer, (u32 *)context->intermediate );
		
		buffer += MD5_SIGNATURE_BYTE_SIZE;
		len    -= MD5_SIGNATURE_BYTE_SIZE;
		
	}

	/*
	 * Handle any remaining bytes of data
	 */
	MD5Memcpy( context->intermediate, buffer, len, doByteSwap );
	
}

/******************************************************************************/
/**
*
* This function final wrap-up - pad to 64-byte boundary with the bit pattern
* 1 0* (64-bit count of bits processed, MSB-first
*
* @param
*
* @param
*
* @param
*
* @param
*
* @return	None
*
* @note		None
*
****************************************************************************/
inline void MD5Final( MD5Context *context, u8 *digest,
		  boolean doByteSwap )
{
	u32		count;
	u8 *	p;
	
	/*
	 * Compute number of bytes mod 64
	 */
	count = ( context->bits[ 0 ] >> 3 ) & 0x3F;

	/*
	 * Set the first char of padding to 0x80. This is safe since there is
	 * always at least one byte free
	 */
	p = context->intermediate + count;
	*p++ = 0x80;

	/*
	 * Bytes of padding needed to make 64 bytes
	 */
	count = MD5_SIGNATURE_BYTE_SIZE - 1 - count;

	/*
	 * Pad out to 56 mod 64
	 */
	if( count < 8 ) {
		/*
		 * Two lots of padding: Pad the first block to 64 bytes
		 */
		MD5Memset( p, 0, count );
		
		MD5Transform( context->buffer, (u32 *)context->intermediate );

		/*
		 * Now fill the next block with 56 bytes
		 */
		MD5Memset( context->intermediate, 0, 56 );
	} else {
		/*
		 * Pad block to 56 bytes
		 */
		MD5Memset( p, 0, count - 8 );
	}

	/*
	 * Append length in bits and transform
	 */
	( (u32 *)context->intermediate )[ 14 ] = context->bits[ 0 ];
	( (u32 *)context->intermediate )[ 15 ] = context->bits[ 1 ];

	MD5Transform( context->buffer, (u32 *)context->intermediate );
	
	/*
	 * Now return the digest
	 */
	MD5Memcpy( digest, context->buffer, 16, doByteSwap );
}

/******************************************************************************/
/**
*
* This function calculate and store in 'digest' the MD5 digest of 'len' bytes at
* 'input'. 'digest' must have enough space to hold 16 bytes
*
* @param
*
* @param
*
* @param
*
* @param
*
* @return	None
*
* @note		None
*
****************************************************************************/
void md5( u8 *input, u32 len, u8 *digest, boolean doByteSwap )
{
	MD5Context context;

	MD5Init( &context );
	
	MD5Update( &context, input, len, doByteSwap );
	
	MD5Final( &context, digest, doByteSwap );
}
