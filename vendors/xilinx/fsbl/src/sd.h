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
* @file sd.h
*
* This file contains the interface for the Secure Digital (SD) card
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a bh	03/10/11 Initial release
* 7.00a kc  10/18/13 Integrated SD/MMC driver
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___SD_H___
#define ___SD_H___


#ifdef __cplusplus
extern "C" {
#endif


/************************** Function Prototypes ******************************/

#if defined(XPAR_PS7_SD_0_S_AXI_BASEADDR) || defined(XPAR_XSDPS_0_BASEADDR)
u32 InitSD(const char *);

u32 SDAccess( u32 SourceAddress,
		u32 DestinationAddress,
		u32 LengthWords);

void ReleaseSD(void);
#endif
/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif


#endif /* ___SD_H___ */

