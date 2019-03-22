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
* @file rsa.h
*
* This file contains the RSA algorithm functions
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 4.00a sg	02/28/13 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___RSA_H___
#define ___RSA_H___

#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/


#define RSA_PPK_MODULAR_SIZE			256
#define RSA_PPK_MODULAR_EXT_SIZE		256
#define RSA_PPK_EXPO_SIZE				64
#define RSA_SPK_MODULAR_SIZE			256
#define RSA_SPK_MODULAR_EXT_SIZE		256
#define RSA_SPK_EXPO_SIZE				64
#define RSA_SPK_SIGNATURE_SIZE			256
#define RSA_PARTITION_SIGNATURE_SIZE	256
#define RSA_SIGNATURE_SIZE				0x6C0 	/* Signature size in bytes */
#define RSA_HEADER_SIZE					4 		/* Signature header size in bytes */
#define RSA_MAGIC_WORD_SIZE				60		/* Magic word size in bytes */

void SetPpk(void );
u32 AuthenticatePartition(u8 *Buffer, u32 Size);
u32 RecreatePaddingAndCheck(u8 *signature, u8 *hash);

#ifdef __cplusplus
}
#endif

#endif /* ___RSA_H___ */
