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
* @file nor.c
*
* Contains code for the NOR FLASH functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 2.00a mb	25/05/12 mio init removed
* 3.00a sgd	30/01/13 Code cleanup
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "fsbl.h"
#include "nor.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;

/******************************************************************************/
/******************************************************************************/
/**
*
* This function initializes the controller for the NOR FLASH interface.
*
* @param	None
*
* @return	None
*
* @note		None.
*
****************************************************************************/
void InitNor(void)
{

	/*
	 * Set up the base address for access
	 */
	FlashReadBaseAddress = XPS_NOR_BASEADDR;
}

/******************************************************************************/
/**
*
* This function provides the NOR FLASH interface for the Simplified header
* functionality.
*
* @param	SourceAddress is address in FLASH data space
* @param	DestinationAddress is address in OCM data space
* @param	LengthBytes is the data length to transfer in bytes
*
* @return
*		- XST_SUCCESS if the write completes correctly
*		- XST_FAILURE if the write fails to completes correctly
*
* @note		None.
*
****************************************************************************/
u32 NorAccess(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
	u32 Data;
	u32 Count;
	u32 *SourceAddr;
	u32 *DestAddr;
	u32 LengthWords;

	/*
	 * check for non-word tail
	 * add bytes to cover the end
	 */
	if ((LengthBytes%4) != 0){

		LengthBytes += (4 - (LengthBytes & 0x00000003));
	}

	LengthWords = LengthBytes >> WORD_LENGTH_SHIFT;

	SourceAddr = (u32 *)(SourceAddress + FlashReadBaseAddress);
	DestAddr = (u32 *)(DestinationAddress);

	/*
	 * Word transfers, endianism isn't an issue
	 */
	for (Count=0; Count < LengthWords; Count++){

		Data = Xil_In32((u32)(SourceAddr++));
		Xil_Out32((u32)(DestAddr++), Data);
	}

	return XST_SUCCESS;
}

