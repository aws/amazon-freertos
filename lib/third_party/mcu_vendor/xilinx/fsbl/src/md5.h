/******************************************************************************
*
* Copyright (C) 2012 - 2014 Xilinx, Inc.  All rights reserved.
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
* @file md5.h
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 5.00a sgd	05/17/13 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___MD5_H___
#define ___MD5_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"

/************************** Constant Definitions *****************************/

#define MD5_SIGNATURE_BYTE_SIZE	64

/**************************** Type Definitions *******************************/

typedef u8 boolean;
typedef u8 signature[ MD5_SIGNATURE_BYTE_SIZE ];

struct MD5Context
	{
	u32			buffer[ 4 ];
	u32			bits[ 2 ];
	signature	intermediate;
	};
typedef struct MD5Context MD5Context;

/***************** Macros (Inline Functions) Definitions *********************/

/*
 * The four core functions - F1 is optimized somewhat
 */
#define F1( x, y, z ) ( z ^ ( x & ( y ^ z ) ) )
#define F2( x, y, z ) F1( z, x, y )
#define F3( x, y, z ) ( x ^ y ^ z )
#define F4( x, y, z ) ( y ^ ( x | ~z ) )


/*
 * This is the central step in the MD5 algorithm
 */
#define MD5_STEP( f, w, x, y, z, data, s ) \
	( w += f( x, y, z ) + data,  w = w << s | w >> ( 32 - s ),  w += x )


/************************** Function Prototypes ******************************/

void * MD5Memset( void *dest, int ch, u32 count );

void * MD5Memcpy( void *dest, const void *src, u32 count, boolean doByteSwap );

void MD5Transform( u32 *buffer, u32 *intermediate );

void MD5Init( MD5Context *context );

void MD5Update( MD5Context *context, u8 *buffer, u32 len, boolean doByteSwap );

void MD5Final( MD5Context *context, u8 *digest, boolean doByteSwap );

void md5( u8 *input, u32	len, u8 *digest, boolean doByteSwap );

/************************** Variable Definitions *****************************/

#ifdef __cplusplus
}
#endif


#endif /* ___MD5_H___ */
