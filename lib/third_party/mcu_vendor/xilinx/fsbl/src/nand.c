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
* @file nand.c
*
* Contains code for the NAND FLASH functionality. Bad Block management
* is simple: skip the bad blocks and keep going.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 2.00a  mb	25/05/12 fsbl changes for standalone bsp based
* 3.00a sgd	30/01/13 Code cleanup
* 5.00a sgd	17/05/13 Support for Multi Boot
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xparameters.h"
#include "fsbl.h"
#ifdef XPAR_PS7_NAND_0_BASEADDR
#include "nand.h"
#include "xnandps_bbm.h"


/************************** Constant Definitions *****************************/

#define NAND_DEVICE_ID		XPAR_XNANDPS_0_DEVICE_ID

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static u32 XNandPs_CalculateLength(XNandPs *NandInstPtr,
										u64 Offset,
										u32 Length);

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;
extern u32 FlashOffsetAddress;

XNandPs *NandInstPtr;
XNandPs NandInstance; /* XNand Instance. */

/******************************************************************************/
/**
*
* This function initializes the controller for the NAND FLASH interface.
*
* @param	none
*
* @return
*		- XST_SUCCESS if the controller initializes correctly
*		- XST_FAILURE if the controller fails to initializes correctly
*
* @note		none.
*
****************************************************************************/
u32 InitNand(void)
{

	u32 Status;
	XNandPs_Config *ConfigPtr;

	/*
	 * Set up pointers to instance and the config structure
	 */
	NandInstPtr = &NandInstance;

	/*
	 * Initialize the flash driver.
	 */
	ConfigPtr = XNandPs_LookupConfig(NAND_DEVICE_ID);

	if (ConfigPtr == NULL) {
		fsbl_printf(DEBUG_GENERAL,"Nand Driver failed \n \r");
		return XST_FAILURE;
	}

	Status = XNandPs_CfgInitialize(NandInstPtr, ConfigPtr,
			ConfigPtr->SmcBase,ConfigPtr->FlashBase);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"NAND intialization failed \n \r");
		return XST_FAILURE;
	}

	/*
	 * Set up base address for access
	 */
	FlashReadBaseAddress = XPS_NAND_BASEADDR;

	fsbl_printf(DEBUG_INFO,"InitNand: Geometry = 0x%x\r\n",
		NandInstPtr->Geometry.FlashWidth);

	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"InitNand: Status = 0x%.8x\r\n", 
				Status);
		return XST_FAILURE;
	}

	/*
	 * set up the FLASH access pointers
	 */
	fsbl_printf(DEBUG_INFO,"Nand driver initialized \n\r");

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function provides the NAND FLASH interface for the Simplified header
* functionality. This function handles bad blocks.
*
* The source address is the absolute good address, bad blocks are skipped
* without incrementing the source address.
*
* @param	SourceAddress is address in FLASH data space, absolute good address
* @param	DestinationAddress is address in OCM data space
*
* @return	XST_SUCCESS if the transfer completes correctly
*		XST_FAILURE if the transfer fails to completes correctly
*
* @note	none.
*
****************************************************************************/
u32 NandAccess(u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
	u32 ActLen;
	u32 BlockOffset;
	u32 Block;
	u32 Status;
	u32 BytesLeft = LengthBytes;
	u32 BlockSize = NandInstPtr->Geometry.BlockSize;
	u8 *BufPtr = (u8 *)DestinationAddress;
	u32 ReadLen;
	u32 BlockReadLen;
	u32 Offset;
	u32 TmpAddress = 0 ;
	u32 BlockCount = 0;
	u32 BadBlocks = 0;

	/*
	 * First get bad blocks before the source address
	 */
	while (TmpAddress < SourceAddress) {
		while (XNandPs_IsBlockBad(NandInstPtr, BlockCount) ==
				XST_SUCCESS) {
			BlockCount ++;
			BadBlocks ++;
		}

		TmpAddress += BlockSize;
		BlockCount ++;
	}

	Offset = SourceAddress + BadBlocks * BlockSize;

	/*
	 * Calculate the actual length including bad blocks
	 */
	ActLen = XNandPs_CalculateLength(NandInstPtr, Offset, LengthBytes);

	/*
	 *  Check if the actual length cross flash size
	 */
	if (Offset + ActLen > NandInstPtr->Geometry.DeviceSize) {
		return XST_FAILURE;
	}

	while (BytesLeft > 0) {
		BlockOffset = Offset & (BlockSize - 1);
		Block = (Offset & ~(BlockSize - 1))/BlockSize;
		BlockReadLen = BlockSize - BlockOffset;

		Status = XNandPs_IsBlockBad(NandInstPtr, Block);
		if (Status == XST_SUCCESS) {
			/* Move to next block */
			Offset += BlockReadLen;
			continue;
		}

		/*
		 * Check if we cross block boundary
		 */
		if (BytesLeft < BlockReadLen) {
			ReadLen = BytesLeft;
		} else {
			ReadLen = BlockReadLen;
		}

		/*
		 * Read from the NAND flash
		 */
		Status = XNandPs_Read(NandInstPtr, Offset, ReadLen, BufPtr, NULL);
		if (Status != XST_SUCCESS) {
			return Status;
		}
		BytesLeft -= ReadLen;
		Offset += ReadLen;
		BufPtr += ReadLen;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function returns the length including bad blocks from a given offset and
* length.
*
* @param	NandInstPtr is the pointer to the XNandPs instance.
* @param	Offset is the flash data address to read from.
* @param	Length is number of bytes to read.
*
* @return
*		- Return actual length including bad blocks.
*
* @note		None.
*
******************************************************************************/
static u32 XNandPs_CalculateLength(XNandPs *NandInstPtr,
									u64 Offset,
									u32 Length)
{
	u32 BlockSize = NandInstPtr->Geometry.BlockSize;
	u32 CurBlockLen;
	u32 CurBlock;
	u32 Status;
	u32 TempLen = 0;
	u32 ActLen = 0;

	while (TempLen < Length) {
		CurBlockLen = BlockSize - (Offset & (BlockSize - 1));
		CurBlock = (Offset & ~(BlockSize - 1))/BlockSize;

		/*
		 * Check if the block is bad
		 */
		Status = XNandPs_IsBlockBad(NandInstPtr, CurBlock);
		if (Status != XST_SUCCESS) {
			/* Good Block */
			TempLen += CurBlockLen;
		}
		ActLen += CurBlockLen;
		Offset += CurBlockLen;
		if (Offset >= NandInstPtr->Geometry.DeviceSize) {
			break;
		}
	}

	return ActLen;
}

#endif
