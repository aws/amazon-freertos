/*****************************************************************************
*
* Copyright (C) 2012 - 2016 Xilinx, Inc.  All rights reserved.
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
* @file pcap.c
*
* Contains code for enabling and accessing the PCAP
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	02/10/10	Initial release
* 2.00a jz	05/28/11	Add SD support
* 2.00a mb	25/05/12	using the EDK provided devcfg driver
* 						Nand/SD encryption and review comments
* 3.00a mb  16/08/12	Added the poll function
*						Removed the FPGA_RST_CTRL define
*						Added the flag for NON PS instantiated bitstream
* 4.00a sgd 02/28/13	Fix for CR#681014 - ECC init in FSBL should not call
*                                           fabric_init()
* 						Fix for CR#689026 - FSBL doesn't hold PL resets active
* 						                    during bit download
* 						Fix for CR#699475 - FSBL functionality is broken and
* 						                    its not able to boot in QSPI/NAND
* 						                    bootmode
*						Fix for CR#705664 - FSBL fails to decrypt the
*						                    bitstream when the image is AES
*						                    encrypted using non-zero key value
* 6.00a kc  08/30/13    Fix for CR#722979 - Provide customer-friendly
*                                           changelogs in FSBL
* 7.00a kc	10/25/13	Fix for CR#724620 - How to handle PCAP_MODE after
*						                    bitstream configuration
*						Fix for CR#726178 - FabricInit() PROG_B is kept active
*						                    for 5mS.
* 						Fix for CR#731839 - FSBL has to check the
* 											HMAC error status after decryption
*			12/04/13	Fix for CR#764382 - How to handle PCAP_MODE after
*						                    bitstream configuration - PCAP_MODE
*											and PCAP_PR bits are not modified
* 8.00a kc  2/20/14		Fix for CR#775631 - FSBL: FsblGetGlobalTimer() 
*						is not proper
* 10.00a kc 07/24/14    Fix for CR#809336 - Minor code cleanup
* 13.00a ssc 04/10/15   Fix for CR#846899 - Corrected logic to clear
*                                           DMA done count
* 15.00a gan 07/21/16   Fix for CR# 953654 -(2016.3)FSBL -
* 											In pcap.c/pcap.h/main.h,
* 											Fabric Initialization sequence
* 											is modified to check the PL power
* 											before sequence starts and checking
* 											INIT_B reset status twice in case
* 											of failure.
* 16.00a gan 08/02/16   Fix for CR# 955897 -(2016.3)FSBL -
* 											In pcap.c, check pl power
* 											through MCTRL register for
* 											3.0 and later versions of silicon.
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "pcap.h"
#include "nand.h"		/* For NAND geometry information */
#include "fsbl.h"
#include "image_mover.h"	/* For MoveImage */
#include "xparameters.h"
#include "xil_exception.h"
#include "xdevcfg.h"
#include "sleep.h"
#include "xtime_l.h"

#ifdef XPAR_XWDTPS_0_BASEADDR
#include "xwdtps.h"
#endif
/************************** Constant Definitions *****************************/
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are only defined here such that a user can easily
 * change all the needed parameters in one place.
 */

#define DCFG_DEVICE_ID		XPAR_XDCFG_0_DEVICE_ID

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
extern int XDcfgPollDone(u32 MaskValue, u32 MaxCount);

/************************** Variable Definitions *****************************/
/* Devcfg driver instance */
static XDcfg DcfgInstance;
XDcfg *DcfgInstPtr;
extern u32 Silicon_Version;
#ifdef XPAR_XWDTPS_0_BASEADDR
extern XWdtPs Watchdog;	/* Instance of WatchDog Timer	*/
#endif

/******************************************************************************/
/**
*
* This function transfer data using PCAP
*
* @param 	SourceDataPtr is a pointer to where the data is read from
* @param 	DestinationDataPtr is a pointer to where the data is written to
* @param 	SourceLength is the length of the data to be moved in words
* @param 	DestinationLength is the length of the data to be moved in words
* @param 	SecureTransfer indicated the encryption key location, 0 for
* 			non-encrypted
*
* @return
*		- XST_SUCCESS if the transfer is successful
*		- XST_FAILURE if the transfer fails
*
* @note		 None
*
****************************************************************************/
u32 PcapDataTransfer(u32 *SourceDataPtr, u32 *DestinationDataPtr,
				u32 SourceLength, u32 DestinationLength, u32 SecureTransfer)
{
	u32 Status;
	u32 IntrStsReg;
	u32 PcapTransferType = XDCFG_CONCURRENT_NONSEC_READ_WRITE;

	/*
	 * Check for secure transfer
	 */
	if (SecureTransfer) {
		PcapTransferType = XDCFG_CONCURRENT_SECURE_READ_WRITE;
	}

#ifdef FSBL_PERF
	XTime tXferCur = 0;
	FsblGetGlobalTime(&tXferCur);
#endif

	/*
	 * Clear the PCAP status registers
	 */
	Status = ClearPcapStatus();
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"PCAP_CLEAR_STATUS_FAIL \r\n");
		return XST_FAILURE;
	}

#ifdef	XPAR_XWDTPS_0_BASEADDR
	/*
	 * Prevent WDT reset
	 */
	XWdtPs_RestartWdt(&Watchdog);
#endif

	/*
	 * PCAP single DMA transfer setup
	 */
	SourceDataPtr = (u32*)((u32)SourceDataPtr | PCAP_LAST_TRANSFER);
	DestinationDataPtr = (u32*)((u32)DestinationDataPtr | PCAP_LAST_TRANSFER);

	/*
	 * Transfer using Device Configuration
	 */
	Status = XDcfg_Transfer(DcfgInstPtr, (u8 *)SourceDataPtr,
					SourceLength,
					(u8 *)DestinationDataPtr,
					DestinationLength, PcapTransferType);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"Status of XDcfg_Transfer = %lu \r \n",Status);
		return XST_FAILURE;
	}

	/*
	 * Dump the PCAP registers
	 */
	PcapDumpRegisters();

	/*
	 * Poll for the DMA done
	 */
	Status = XDcfgPollDone(XDCFG_IXR_DMA_DONE_MASK, MAX_COUNT);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"PCAP_DMA_DONE_FAIL \r\n");
		return XST_FAILURE;
	}

	fsbl_printf(DEBUG_INFO,"DMA Done ! \n\r");
		
	/*
	 * Check for errors
	 */
	IntrStsReg = XDcfg_IntrGetStatus(DcfgInstPtr);
	if (IntrStsReg & FSBL_XDCFG_IXR_ERROR_FLAGS_MASK) {
		fsbl_printf(DEBUG_INFO,"Errors in PCAP \r\n");
		return XST_FAILURE;
	}

	/*
	 * For Performance measurement
	 */
#ifdef FSBL_PERF
	XTime tXferEnd = 0;
	fsbl_printf(DEBUG_GENERAL,"Time taken is ");
	FsblMeasurePerfTime(tXferCur,tXferEnd);
#endif

	return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function loads PL partition using PCAP
*
* @param 	SourceDataPtr is a pointer to where the data is read from
* @param 	DestinationDataPtr is a pointer to where the data is written to
* @param 	SourceLength is the length of the data to be moved in words
* @param 	DestinationLength is the length of the data to be moved in words
* @param 	SecureTransfer indicated the encryption key location, 0 for
* 			non-encrypted
*
* @return
*		- XST_SUCCESS if the transfer is successful
*		- XST_FAILURE if the transfer fails
*
* @note		 None
*
****************************************************************************/
u32 PcapLoadPartition(u32 *SourceDataPtr, u32 *DestinationDataPtr,
		u32 SourceLength, u32 DestinationLength, u32 SecureTransfer)
{
	u32 Status;
	u32 IntrStsReg;
	u32 PcapTransferType = XDCFG_NON_SECURE_PCAP_WRITE;

	/*
	 * Check for secure transfer
	 */
	if (SecureTransfer) {
		PcapTransferType = XDCFG_SECURE_PCAP_WRITE;
	}

#ifdef FSBL_PERF
	XTime tXferCur = 0;
	FsblGetGlobalTime(&tXferCur);
#endif

	/*
	 * Clear the PCAP status registers
	 */
	Status = ClearPcapStatus();
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"PCAP_CLEAR_STATUS_FAIL \r\n");
		return XST_FAILURE;
	}

	/*
	 * For Bitstream case destination address will be 0xFFFFFFFF
	 */
	DestinationDataPtr = (u32*)XDCFG_DMA_INVALID_ADDRESS;

	/*
	 * New Bitstream download initialization sequence
	 */
	Status = FabricInit();
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


#ifdef	XPAR_XWDTPS_0_BASEADDR
	/*
	 * Prevent WDT reset
	 */
	XWdtPs_RestartWdt(&Watchdog);
#endif

	/*
	 * PCAP single DMA transfer setup
	 */
	SourceDataPtr = (u32*)((u32)SourceDataPtr | PCAP_LAST_TRANSFER);
	DestinationDataPtr = (u32*)((u32)DestinationDataPtr | PCAP_LAST_TRANSFER);

	/*
	 * Transfer using Device Configuration
	 */
	Status = XDcfg_Transfer(DcfgInstPtr, (u8 *)SourceDataPtr,
					SourceLength,
					(u8 *)DestinationDataPtr,
					DestinationLength, PcapTransferType);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"Status of XDcfg_Transfer = %lu \r \n",Status);
		return XST_FAILURE;
	}


	/*
	 * Dump the PCAP registers
	 */
	PcapDumpRegisters();


	/*
	 * Poll for the DMA done
	 */
	Status = XDcfgPollDone(XDCFG_IXR_DMA_DONE_MASK, MAX_COUNT);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"PCAP_DMA_DONE_FAIL \r\n");
		return XST_FAILURE;
	}

	fsbl_printf(DEBUG_INFO,"DMA Done ! \n\r");

	/*
	 * Poll for FPGA Done
	 */
	Status = XDcfgPollDone(XDCFG_IXR_PCFG_DONE_MASK, MAX_COUNT);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"PCAP_FPGA_DONE_FAIL\r\n");
		return XST_FAILURE;
	}

	fsbl_printf(DEBUG_INFO,"FPGA Done ! \n\r");
	
	/*
	 * Check for errors
	 */
	IntrStsReg = XDcfg_IntrGetStatus(DcfgInstPtr);
	if (IntrStsReg & FSBL_XDCFG_IXR_ERROR_FLAGS_MASK) {
		fsbl_printf(DEBUG_INFO,"Errors in PCAP \r\n");
		return XST_FAILURE;
	}

	/*
	 * For Performance measurement
	 */
#ifdef FSBL_PERF
	XTime tXferEnd = 0;
	fsbl_printf(DEBUG_GENERAL,"Time taken is ");
	FsblMeasurePerfTime(tXferCur,tXferEnd);
#endif

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function Initializes the PCAP driver.
*
* @param	none
*
* @return
*		- XST_SUCCESS if the pcap driver initialization is successful
*		- XST_FAILURE if the pcap driver initialization fails
*
* @note	 none
*
****************************************************************************/
int InitPcap(void)
{
	XDcfg_Config *ConfigPtr;
	int Status = XST_SUCCESS;
	DcfgInstPtr = &DcfgInstance;

	/*
	 * Initialize the Device Configuration Interface driver.
	 */
	ConfigPtr = XDcfg_LookupConfig(DCFG_DEVICE_ID);

	Status = XDcfg_CfgInitialize(DcfgInstPtr, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO, "XDcfg_CfgInitialize failed \n\r");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
/******************************************************************************/
/**
*
* This function programs the Fabric for use.
*
* @param	None
*
* @return
*		- XST_SUCCESS if the Fabric  initialization is successful
*		- XST_FAILURE if the Fabric  initialization fails
* @note		None
*
****************************************************************************/
u32 FabricInit(void)
{
	u32 PcapReg; 
	u32 PcapCtrlRegVal;
	u32 StatusReg;
	u32 MctrlReg;
	u32 PcfgInit;
	u32 TimerExpired=0;
	XTime tCur=0;
	XTime tEnd=0;


	/*
	 * Set Level Shifters DT618760 - PS to PL enabling
	 */
	Xil_Out32(PS_LVL_SHFTR_EN, LVL_PS_PL);
	fsbl_printf(DEBUG_INFO,"Level Shifter Value = 0x%lx \r\n",
				Xil_In32(PS_LVL_SHFTR_EN));

	/*
	 * Get DEVCFG controller settings
	 */
	PcapReg = XDcfg_ReadReg(DcfgInstPtr->Config.BaseAddr,
				XDCFG_CTRL_OFFSET);

	/*
	 * Check the PL power status
	 */
	if(Silicon_Version >= SILICON_VERSION_3)
	{
		MctrlReg = XDcfg_GetMiscControlRegister(DcfgInstPtr);

		if((MctrlReg & XDCFG_MCTRL_PCAP_PCFG_POR_B_MASK) !=
				XDCFG_MCTRL_PCAP_PCFG_POR_B_MASK)
		{
			fsbl_printf(DEBUG_INFO,"Fabric not powered up\r\n");
			return XST_FAILURE;
		}
	}


	/*
	 * Setting PCFG_PROG_B signal to high
	 */
	XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
				(PcapReg | XDCFG_CTRL_PCFG_PROG_B_MASK));

	/*
	 * Check for AES source key
	 */
	PcapCtrlRegVal = XDcfg_GetControlRegister(DcfgInstPtr);
	if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
	/*
	 * 5msec delay
	 */
		usleep(5000);
	}
	
	/*
	 * Setting PCFG_PROG_B signal to low
	 */
	XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
				(PcapReg & ~XDCFG_CTRL_PCFG_PROG_B_MASK));

	/*
	 * Check for AES source key
	 */
	if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
	/*
	 * 5msec delay
	 */
		usleep(5000);
	}

	/*
	 * Polling the PCAP_INIT status for Reset or timeout
	 */

	XTime_GetTime(&tCur);
	do
	{
		PcfgInit = (XDcfg_GetStatusRegister(DcfgInstPtr) &
				XDCFG_STATUS_PCFG_INIT_MASK);
		if(PcfgInit == 0)
		{
			break;
		}
		XTime_GetTime(&tEnd);
		if((u64)((u64)tCur + (COUNTS_PER_MILLI_SECOND*30)) > (u64)tEnd)
		{
			TimerExpired = 1;
		}

	} while(!TimerExpired);

	if(TimerExpired == 1)
	{
		TimerExpired = 0;
		/*
		 * Came here due to expiration and PCAP_INIT is set.
		 * Retry PCFG_PROG_B High -> Low again
		 */

		/*
		 * Setting PCFG_PROG_B signal to high
		 */
		XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
					(PcapReg | XDCFG_CTRL_PCFG_PROG_B_MASK));

		/*
		 * Check for AES source key
		 */
		PcapCtrlRegVal = XDcfg_GetControlRegister(DcfgInstPtr);
		if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
			/*
			 * 5msec delay
			 */
			usleep(5000);
		}

		/*
		 * Setting PCFG_PROG_B signal to low
		 */
		XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
					(PcapReg & ~XDCFG_CTRL_PCFG_PROG_B_MASK));

		/*
		 * Check for AES source key
		 */
		if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
			/*
			 * 5msec delay
			 */
			usleep(5000);
		}
		/*
		 * Polling the PCAP_INIT status for Reset or timeout (second iteration)
		 */

		XTime_GetTime(&tCur);
		do
		{
			PcfgInit = (XDcfg_GetStatusRegister(DcfgInstPtr) &
					XDCFG_STATUS_PCFG_INIT_MASK);
			if(PcfgInit == 0)
			{
				break;
			}
			XTime_GetTime(&tEnd);
			if((u64)((u64)tCur + (COUNTS_PER_MILLI_SECOND*30)) > (u64)tEnd)
			{
				TimerExpired = 1;
			}

		} while(!TimerExpired);

		if(TimerExpired == 1)
		{
			/*
			 * Came here due to PCAP_INIT is not getting reset
			 * for PCFG_PROG_B signal High -> Low
			 */
			fsbl_printf(DEBUG_INFO,"Fabric Init failed\r\n");
			return XST_FAILURE;
		}
	}

	/*
	 * Setting PCFG_PROG_B signal to high
	 */
	XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr, XDCFG_CTRL_OFFSET,
			(PcapReg | XDCFG_CTRL_PCFG_PROG_B_MASK));

	/*
	 * Polling the PCAP_INIT status for Set
	 */
	while(!(XDcfg_GetStatusRegister(DcfgInstPtr) &
			XDCFG_STATUS_PCFG_INIT_MASK));

	/*
	 * Get Device configuration status
	 */
	StatusReg = XDcfg_GetStatusRegister(DcfgInstPtr);
	fsbl_printf(DEBUG_INFO,"Devcfg Status register = 0x%lx \r\n",StatusReg);

	fsbl_printf(DEBUG_INFO,"PCAP:Fabric is Initialized done\r\n");

	return XST_SUCCESS;
}
/******************************************************************************/
/**
*
* This function Clears the PCAP status registers.
*
* @param	None
*
* @return
*		- XST_SUCCESS if the pcap status registers are cleared
*		- XST_FAILURE if errors are there
*		- XST_DEVICE_BUSY if Pcap device is busy
* @note		None
*
****************************************************************************/
u32 ClearPcapStatus(void)
{

	u32 StatusReg;
	u32 IntStatusReg;

	/*
	 * Clear it all, so if Boot ROM comes back, it can proceed
	 */
	XDcfg_IntrClear(DcfgInstPtr, 0xFFFFFFFF);

	/*
	 * Get PCAP Interrupt Status Register
	 */
	IntStatusReg = XDcfg_IntrGetStatus(DcfgInstPtr);
	if (IntStatusReg & FSBL_XDCFG_IXR_ERROR_FLAGS_MASK) {
		fsbl_printf(DEBUG_INFO,"FATAL errors in PCAP %lx\r\n",
				IntStatusReg);
		return XST_FAILURE;
	}

	/*
	 * Read the PCAP status register for DMA status
	 */
	StatusReg = XDcfg_GetStatusRegister(DcfgInstPtr);

	fsbl_printf(DEBUG_INFO,"PCAP:StatusReg = 0x%.8lx\r\n", StatusReg);

	/*
	 * If the queue is full, return w/ XST_DEVICE_BUSY
	 */
	if ((StatusReg & XDCFG_STATUS_DMA_CMD_Q_F_MASK) ==
			XDCFG_STATUS_DMA_CMD_Q_F_MASK) {

		fsbl_printf(DEBUG_INFO,"PCAP_DEVICE_BUSY\r\n");
		return XST_DEVICE_BUSY;
	}

	fsbl_printf(DEBUG_INFO,"PCAP:device ready\r\n");

	/*
	 * There are unacknowledged DMA commands outstanding
	 */
	if ((StatusReg & XDCFG_STATUS_DMA_CMD_Q_E_MASK) !=
			XDCFG_STATUS_DMA_CMD_Q_E_MASK) {

		IntStatusReg = XDcfg_IntrGetStatus(DcfgInstPtr);

		if ((IntStatusReg & XDCFG_IXR_DMA_DONE_MASK) !=
				XDCFG_IXR_DMA_DONE_MASK){
			/*
			 * Error state, transfer cannot occur
			 */
			fsbl_printf(DEBUG_INFO,"PCAP:IntStatus indicates error\r\n");
			return XST_FAILURE;
		}
		else {
			/*
			 * clear out the status
			 */
			XDcfg_IntrClear(DcfgInstPtr, XDCFG_IXR_DMA_DONE_MASK);
		}
	}

	if ((StatusReg & XDCFG_STATUS_DMA_DONE_CNT_MASK) != 0) {
		XDcfg_SetStatusRegister(DcfgInstPtr, StatusReg |
				XDCFG_STATUS_DMA_DONE_CNT_MASK);
	}

	fsbl_printf(DEBUG_INFO,"PCAP:Clear done\r\n");

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function prints PCAP register status.
*
* @param	none
*
* @return	none
*
* @note		none
*
****************************************************************************/
void PcapDumpRegisters (void) {

	fsbl_printf(DEBUG_INFO,"PCAP register dump:\r\n");

	fsbl_printf(DEBUG_INFO,"PCAP CTRL 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_CTRL_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_CTRL_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP LOCK 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_LOCK_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_LOCK_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP CONFIG 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_CFG_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_CFG_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP ISR 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_INT_STS_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_INT_STS_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP IMR 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_INT_MASK_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_INT_MASK_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP STATUS 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_STATUS_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_STATUS_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP DMA SRC ADDR 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_SRC_ADDR_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_SRC_ADDR_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP DMA DEST ADDR 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_DEST_ADDR_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_DEST_ADDR_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP DMA SRC LEN 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_SRC_LEN_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_SRC_LEN_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP DMA DEST LEN 0x%x: 0x%08lx\r\n",
			XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_DEST_LEN_OFFSET,
			Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_DMA_DEST_LEN_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP ROM SHADOW CTRL 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_ROM_SHADOW_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_ROM_SHADOW_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP MBOOT 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_MULTIBOOT_ADDR_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_MULTIBOOT_ADDR_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP SW ID 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_SW_ID_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_SW_ID_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP UNLOCK 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_UNLOCK_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_UNLOCK_OFFSET));
	fsbl_printf(DEBUG_INFO,"PCAP MCTRL 0x%x: 0x%08lx\r\n",
		XPS_DEV_CFG_APB_BASEADDR + XDCFG_MCTRL_OFFSET,
		Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_MCTRL_OFFSET));
}

/******************************************************************************/
/**
*
* This function Polls for the DMA done or FPGA done.
*
* @param	none
*
* @return
*		- XST_SUCCESS if polling for DMA/FPGA done is successful
*		- XST_FAILURE if polling for DMA/FPGA done fails
*
* @note		none
*
****************************************************************************/
int XDcfgPollDone(u32 MaskValue, u32 MaxCount)
{
	int Count = MaxCount;
	u32 IntrStsReg = 0;

	/*
	 * poll for the DMA done
	 */
	IntrStsReg = XDcfg_IntrGetStatus(DcfgInstPtr);
	while ((IntrStsReg & MaskValue) !=
				MaskValue) {
		IntrStsReg = XDcfg_IntrGetStatus(DcfgInstPtr);
		Count -=1;

		if (IntrStsReg & FSBL_XDCFG_IXR_ERROR_FLAGS_MASK) {
				fsbl_printf(DEBUG_INFO,"FATAL errors in PCAP %lx\r\n",
						IntrStsReg);
				PcapDumpRegisters();
				return XST_FAILURE;
		}

		if(!Count) {
			fsbl_printf(DEBUG_GENERAL,"PCAP transfer timed out \r\n");
			return XST_FAILURE;
		}
		if (Count > (MAX_COUNT-100)) {
			fsbl_printf(DEBUG_GENERAL,".");
		}
	}

	fsbl_printf(DEBUG_GENERAL,"\n\r");

	XDcfg_IntrClear(DcfgInstPtr, IntrStsReg & MaskValue);

	return XST_SUCCESS;
}
