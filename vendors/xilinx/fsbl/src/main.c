/******************************************************************************
*
* Copyright (C) 2012 - 2018 Xilinx, Inc.  All rights reserved.
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
* @file main.c
*
* The main file for the First Stage Boot Loader (FSBL).
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz	06/04/11	Initial release
* 2.00a mb	25/05/12	standalone based FSBL
* 3.00a np/mb	08/03/12	Added call to FSBL user hook - before handoff.
*				DDR ECC initialization added
* 				fsbl print with verbose added
* 				Performance measurement added
* 				Flushed the UART Tx buffer
* 				Added the performance time for ECC DDR init
* 				Added clearing of ECC Error Code
* 				Added the watchdog timer value
* 4.00a sgd 02/28/13	Code Cleanup
* 						Fix for CR#681014 - ECC init in FSBL should not
* 						                    call fabric_init()
* 						Fix for CR#689077 - FSBL hangs at Handoff clearing the
* 						                    TX UART buffer when using UART0
* 						                    instead of UART1
*						Fix for CR#694038 - FSBL debug logs always prints 14.3
*											as the Revision number - this is
*										    incorrect
*						Fix for CR#694039 - FSBL prints "unsupported silicon
*											version for v3.0" 3.0 Silicon
*                       Fix for CR#699475 - FSBL functionality is broken and
*                                           its not able to boot in QSPI/NAND
*                                           bootmode
*                       Removed DDR initialization check
*                       Removed DDR ECC initialization code
*						Modified hand off address check to 1MB
*						Added RSA authentication support
*						Watchdog disabled for AES E-Fuse encryption
* 5.00a sgd 05/17/13	Fallback support for E-Fuse encryption
*                       Fix for CR#708728 - Issues seen while making HP
*                                           interconnect 32 bit wide
* 6.00a kc  07/30/13    Fix for CR#708316 - PS7_init.tcl file should have
*                                           Error mechanism for all mask_poll
*                       Fix for CR#691150 - ps7_init does not check for
*                                           peripheral initialization failures
*                                           or timeout on polls
*                       Fix for CR#724165 - Partition Header used by FSBL is
*                                           not authenticated
*                       Fix for CR#724166 - FSBL doesn’t use PPK authenticated
*                                           by Boot ROM for authenticating
*                                           the Partition images
*                       Fix for CR#722979 - Provide customer-friendly
*                                           changelogs in FSBL
*                       Fix for CR#732865 - Backward compatibility for ps7_init
*                       					function
* 7.00a kc  10/18/13    Integrated SD/MMC driver
* 8.00a kc  02/20/14	Fix for CR#775631 - FSBL: FsblGetGlobalTimer() 
*											is not proper
* 9.00a kc  04/16/14	Fix for CR#724166 - SetPpk() will fail on secure
*		 									fallback unless FSBL* and FSBL
*		 									are identical in length
* 10.00a kc 07/24/14	Fix for CR#809336 - Minor code cleanup
*        kc 08/27/14	Fix for CR#820356 - FSBL compilation fails with
* 											IAR compiler
* 11.00a kv 10/08/14	Fix for CR#826030 - LinearBootDeviceFlag should
*											be initialized to 0 in IO mode
*											case
* 15.00a gan 07/21/16   Fix for CR# 953654 -(2016.3)FSBL -
* 											In pcap.c/pcap.h/main.c,
* 											Fabric Initialization sequence
* 											is modified to check the PL power
* 											before sequence starts and checking
* 											INIT_B reset status twice in case
* 											of failure.
* 16.00a bsv 03/26/18	Fix for CR# 996973  Add code under JTAG_ENABLE_LEVEL_SHIFTERS macro
* 											to enable level shifters in jtag boot mode.
* </pre>
*
* @note
* FSBL runs from OCM, Based on the boot mode selected, FSBL will copy
* the partitions from the flash device. If the partition is bitstream then
* the bitstream is programmed in the Fabric and for an partition that is
* an application , FSBL will copy the application into DDR and does a
* handoff.The application should not be starting at the OCM address,
* FSBL does not remap the DDR. Application should use DDR starting from 1MB
*
* FSBL can be stitched along with bitstream and application using bootgen
*
* Refer to fsbl.h file for details on the compilation flags supported in FSBL
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "fsbl.h"
#include "qspi.h"
#include "nand.h"
#include "nor.h"
#include "sd.h"
#include "pcap.h"
#include "image_mover.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xstatus.h"
#include "fsbl_hooks.h"
#include "xtime_l.h"

#ifdef XPAR_XWDTPS_0_BASEADDR
#include "xwdtps.h"
#endif

#ifdef STDOUT_BASEADDRESS
#ifdef XPAR_XUARTPS_0_BASEADDR
#include "xuartps_hw.h"
#endif
#endif

#ifdef RSA_SUPPORT
#include "rsa.h"
#endif

/************************** Constant Definitions *****************************/

#ifdef XPAR_XWDTPS_0_BASEADDR
#define WDT_DEVICE_ID		XPAR_XWDTPS_0_DEVICE_ID
#define WDT_EXPIRE_TIME		100
#define WDT_CRV_SHIFT		12
#endif

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#ifdef XPAR_XWDTPS_0_BASEADDR
XWdtPs Watchdog;		/* Instance of WatchDog Timer	*/
#endif
/************************** Function Prototypes ******************************/
extern int ps7_init();
extern char* getPS7MessageInfo(unsigned key);
#ifdef PS7_POST_CONFIG
extern int ps7_post_config();
#endif

static void Update_MultiBootRegister(void);
/* Exception handlers */
static void RegisterHandlers(void);
static void Undef_Handler (void);
static void SVC_Handler (void);
static void PreFetch_Abort_Handler (void);
static void Data_Abort_Handler (void);
static void IRQ_Handler (void);
static void FIQ_Handler (void);


#ifdef XPAR_XWDTPS_0_BASEADDR
int InitWatchDog(void);
u32 ConvertTime_WdtCounter(u32 seconds);
void  CheckWDTReset(void);
#endif

u32 NextValidImageCheck(void);

u32 DDRInitCheck(void);

/************************** Variable Definitions *****************************/
/*
 * Base Address for the Read Functionality for Image Processing
 */
u32 FlashReadBaseAddress = 0;
/*
 * Silicon Version
 */
u32 Silicon_Version;

/*
 * Boot Device flag
 */
u8 LinearBootDeviceFlag=0;

u32 PcapCtrlRegVal;

u8 SystemInitFlag;

extern ImageMoverType MoveImage;
extern XDcfg *DcfgInstPtr;
extern u8 BitstreamFlag;
#ifdef XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR
extern u32 QspiFlashSize;
#endif
/*****************************************************************************/
/**
*
* This is the main function for the FSBL ROM code.
*
*
* @param	None.
*
* @return
*		- XST_SUCCESS to indicate success
*		- XST_FAILURE.to indicate failure
*
* @note
*
****************************************************************************/
int main(void)
{
	u32 BootModeRegister = 0;
	u32 HandoffAddress = 0;
	u32 Status = XST_SUCCESS;
	u32 RegVal;
	/*
	 * PCW initialization for MIO,PLL,CLK and DDR
	 */
	Status = ps7_init();
	if (Status != FSBL_PS7_INIT_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"PS7_INIT_FAIL : %s\r\n",
						getPS7MessageInfo(Status));
		OutputStatus(PS7_INIT_FAIL);
		/*
		 * Calling FsblHookFallback instead of Fallback
		 * since, devcfg driver is not yet initialized
		 */
		FsblHookFallback();
	}

	/*
	 * Unlock SLCR for SLCR register write
	 */
	SlcrUnlock();

	/* If Performance measurement is required 
	 * then read the Global Timer value , Please note that the
	 * time taken for mio, clock and ddr initialisation
	 * done in the ps7_init function is not accounted in the FSBL
	 *
	 */
#ifdef FSBL_PERF
	XTime tCur = 0;
	FsblGetGlobalTime(&tCur);
#endif

	/*
	 * Flush the Caches
	 */
	Xil_DCacheFlush();

	/*
	 * Disable Data Cache
	 */
	Xil_DCacheDisable();

	/*
	 * Register the Exception handlers
	 */
	RegisterHandlers();
	
	/*
	 * Print the FSBL Banner
	 */
	fsbl_printf(DEBUG_GENERAL,"\n\rXilinx First Stage Boot Loader \n\r");
	fsbl_printf(DEBUG_GENERAL,"Release %d.%d	%s-%s\r\n",
			SDK_RELEASE_YEAR, SDK_RELEASE_QUARTER,
			__DATE__,__TIME__);

#ifdef XPAR_PS7_DDR_0_S_AXI_BASEADDR

    /*
     * DDR Read/write test 
     */
	Status = DDRInitCheck();
	if (Status == XST_FAILURE) {
		fsbl_printf(DEBUG_GENERAL,"DDR_INIT_FAIL \r\n");
		/* Error Handling here */
		OutputStatus(DDR_INIT_FAIL);
		/*
		 * Calling FsblHookFallback instead of Fallback
		 * since, devcfg driver is not yet initialized
		 */
		FsblHookFallback();
	}


	/*
	 * PCAP initialization
	 */
	Status = InitPcap();
	if (Status == XST_FAILURE) {
		fsbl_printf(DEBUG_GENERAL,"PCAP_INIT_FAIL \n\r");
		OutputStatus(PCAP_INIT_FAIL);
		/*
		 * Calling FsblHookFallback instead of Fallback
		 * since, devcfg driver is not yet initialized
		 */
		FsblHookFallback();
	}

	fsbl_printf(DEBUG_INFO,"Devcfg driver initialized \r\n");

	/*
	 * Get the Silicon Version
	 */
	GetSiliconVersion();

#ifdef XPAR_XWDTPS_0_BASEADDR
	/*
	 * Check if WDT Reset has occurred or not
	 */
	CheckWDTReset();

	/*
	 * Initialize the Watchdog Timer so that it is ready to use
	 */
	Status = InitWatchDog();
	if (Status == XST_FAILURE) {
		fsbl_printf(DEBUG_GENERAL,"WATCHDOG_INIT_FAIL \n\r");
		OutputStatus(WDT_INIT_FAIL);
		FsblFallback();
	}
	fsbl_printf(DEBUG_INFO,"Watchdog driver initialized \r\n");
#endif

	/*
	 * Get PCAP controller settings
	 */
	PcapCtrlRegVal = XDcfg_GetControlRegister(DcfgInstPtr);

	/*
	 * Check for AES source key
	 */
	if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
		/*
		 * For E-Fuse AES encryption Watch dog Timer disabled and
		 * User not allowed to do system reset
		 */
#ifdef	XPAR_XWDTPS_0_BASEADDR
		fsbl_printf(DEBUG_INFO,"Watchdog Timer Disabled\r\n");
		XWdtPs_Stop(&Watchdog);
#endif
		fsbl_printf(DEBUG_INFO,"User not allowed to do "
								"any system resets\r\n");
	}

	/*
	 * Store FSBL run state in Reboot Status Register
	 */
	MarkFSBLIn();

	/*
	 * Read bootmode register
	 */
	BootModeRegister = Xil_In32(BOOT_MODE_REG);
	BootModeRegister &= BOOT_MODES_MASK;

	/*
	 * QSPI BOOT MODE
	 */
#ifdef XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR

#ifdef MMC_SUPPORT
	/*
	 * To support MMC boot
	 * QSPI boot mode detection ignored
	 */
	if (BootModeRegister == QSPI_MODE) {
		BootModeRegister = MMC_MODE;
	}
#endif

	if (BootModeRegister == QSPI_MODE) {
		fsbl_printf(DEBUG_GENERAL,"Boot mode is QSPI\n\r");
		InitQspi();
		MoveImage = QspiAccess;
		fsbl_printf(DEBUG_INFO,"QSPI Init Done \r\n");
	} else
#endif

	/*
	 * NAND BOOT MODE
	 */
#ifdef XPAR_PS7_NAND_0_BASEADDR
	if (BootModeRegister == NAND_FLASH_MODE) {
		/*
	 	* Boot ROM always initialize the nand at lower speed
	 	* This is the chance to put it to an optimum speed for your nand
	 	* device
	 	*/
		fsbl_printf(DEBUG_GENERAL,"Boot mode is NAND\n");

		Status = InitNand();
		if (Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL,"NAND_INIT_FAIL \r\n");
			/*
			 * Error Handling here
			 */
			OutputStatus(NAND_INIT_FAIL);
			FsblFallback();
		}
		MoveImage = NandAccess;
		fsbl_printf(DEBUG_INFO,"NAND Init Done \r\n");
	} else
#endif

	/*
	 * NOR BOOT MODE
	 */
	if (BootModeRegister == NOR_FLASH_MODE) {
		fsbl_printf(DEBUG_GENERAL,"Boot mode is NOR\n\r");
		/*
		 * Boot ROM always initialize the nor at lower speed
		 * This is the chance to put it to an optimum speed for your nor
		 * device
		 */
		InitNor();
		fsbl_printf(DEBUG_INFO,"NOR Init Done \r\n");
		MoveImage = NorAccess;
	} else

	/*
	 * SD BOOT MODE
	 */
#if defined(XPAR_PS7_SD_0_S_AXI_BASEADDR) || defined(XPAR_XSDPS_0_BASEADDR)

	if (BootModeRegister == SD_MODE) {
		fsbl_printf(DEBUG_GENERAL,"Boot mode is SD\r\n");

		/*
		 * SD initialization returns file open error or success
		 */
		Status = InitSD("BOOT.BIN");
		if (Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL,"SD_INIT_FAIL\r\n");
			OutputStatus(SD_INIT_FAIL);
			FsblFallback();
		}
		MoveImage = SDAccess;
		fsbl_printf(DEBUG_INFO,"SD Init Done \r\n");
	} else

	if (BootModeRegister == MMC_MODE) {
		fsbl_printf(DEBUG_GENERAL,"Booting Device is MMC\r\n");

		/*
		 * MMC initialization returns file open error or success
		 */
		Status = InitSD("BOOT.BIN");
		if (Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL,"MMC_INIT_FAIL\r\n");
			OutputStatus(SD_INIT_FAIL);
			FsblFallback();
		}
		MoveImage = SDAccess;
		fsbl_printf(DEBUG_INFO,"MMC Init Done \r\n");
	} else

#endif

	/*
	 * JTAG  BOOT MODE
	 */
	if (BootModeRegister == JTAG_MODE) {
		fsbl_printf(DEBUG_GENERAL,"Boot mode is JTAG\r\n");

		RegVal = Xil_In32(XPS_DEV_CFG_APB_BASEADDR + XDCFG_INT_STS_OFFSET);
		/** If bitstream was loaded in jtag boot mode prior to running FSBL */
		if(RegVal & XDCFG_IXR_PCFG_DONE_MASK)
		{
#ifdef PS7_POST_CONFIG
		ps7_post_config();
		/*
		 * Unlock SLCR for SLCR register write
		 */
		SlcrUnlock();
#endif
		}
		/*
		 * Stop the Watchdog before JTAG handoff
		 */
#ifdef	XPAR_XWDTPS_0_BASEADDR
		XWdtPs_Stop(&Watchdog);
#endif
		/*
		 * Clear our mark in reboot status register
		 */
		ClearFSBLIn();

		/*
		 * SLCR lock
		 */
		SlcrLock();

		FsblHandoffJtagExit();
	} else {
		fsbl_printf(DEBUG_GENERAL,"ILLEGAL_BOOT_MODE \r\n");
		OutputStatus(ILLEGAL_BOOT_MODE);
		/*
		 * fallback starts, no return
		 */
		FsblFallback();
	}

	fsbl_printf(DEBUG_INFO,"Flash Base Address: 0x%08lx\r\n", FlashReadBaseAddress);

	/*
	 * Check for valid flash address
	 */
	if ((FlashReadBaseAddress != XPS_QSPI_LINEAR_BASEADDR) &&
			(FlashReadBaseAddress != XPS_NAND_BASEADDR) &&
			(FlashReadBaseAddress != XPS_NOR_BASEADDR) &&
			(FlashReadBaseAddress != XPS_SDIO0_BASEADDR)) {
		fsbl_printf(DEBUG_GENERAL,"INVALID_FLASH_ADDRESS \r\n");
		OutputStatus(INVALID_FLASH_ADDRESS);
		FsblFallback();
	}

	/*
	 * NOR and QSPI (parallel) are linear boot devices
	 */
	if ((FlashReadBaseAddress == XPS_NOR_BASEADDR)) {
		fsbl_printf(DEBUG_INFO, "Linear Boot Device\r\n");
		LinearBootDeviceFlag = 1;
	}

#ifdef	XPAR_XWDTPS_0_BASEADDR
	/*
	 * Prevent WDT reset
	 */
	XWdtPs_RestartWdt(&Watchdog);
#endif

	/*
	 * This used only in case of E-Fuse encryption
	 * For image search
	 */
	SystemInitFlag = 1;

	/*
	 * Load boot image
	 */
	HandoffAddress = LoadBootImage();

	fsbl_printf(DEBUG_INFO,"Handoff Address: 0x%08lx\r\n",HandoffAddress);

	/*
	 * For Performance measurement
	 */
#ifdef FSBL_PERF
	XTime tEnd = 0;
	fsbl_printf(DEBUG_GENERAL,"Total Execution time is ");
	FsblMeasurePerfTime(tCur,tEnd);
#endif

	/*
	 * FSBL handoff to valid handoff address or
	 * exit in JTAG
	 */
	FsblHandoff(HandoffAddress);

#else
	OutputStatus(NO_DDR);
	FsblFallback();
#endif

	return Status;
}

/******************************************************************************/
/**
*
* This function reset the CPU and goes for Boot ROM fallback handling
*
* @param	None
*
* @return	None
*
* @note		None
*
****************************************************************************/
void FsblFallback(void)
{
	u32 RebootStatusReg;
	u32 Status;
	u32 HandoffAddr;
	u32 BootModeRegister;

	/*
	 * Read bootmode register
	 */
	BootModeRegister = Xil_In32(BOOT_MODE_REG);
	BootModeRegister &= BOOT_MODES_MASK;

	/*
	 * Fallback support check
	 */
	if (!((BootModeRegister == QSPI_MODE) ||
			(BootModeRegister == NAND_FLASH_MODE) ||
			(BootModeRegister == NOR_FLASH_MODE))) {
		fsbl_printf(DEBUG_INFO,"\r\n"
				"This Boot Mode Doesn't Support Fallback\r\n");
		ClearFSBLIn();
		FsblHookFallback();
	}

	/*
	 * update the Multiboot Register for Golden search hunt
	 */
	Update_MultiBootRegister();

	/*
	 * Notify Boot ROM something is wrong
	 */
	RebootStatusReg =  Xil_In32(REBOOT_STATUS_REG);

	/*
	 * Set the FSBL Fail mask
	 */
	Xil_Out32(REBOOT_STATUS_REG, RebootStatusReg | FSBL_FAIL_MASK);

	/*
	 * Barrier for synchronization
	 */
		__asm(
			"dsb\n\t"
			"isb"
		);

	/*
	 * Check for AES source key
	 */
	if (PcapCtrlRegVal & XDCFG_CTRL_PCFG_AES_FUSE_MASK) {
		/*
		 * Next valid image search can happen only
		 * when system initialization done
		 */
		if (SystemInitFlag == 1) {
			/*
			 * Clean the Fabric
			 */
			Status = FabricInit();
			if(Status != XST_SUCCESS){
				ClearFSBLIn();
				FsblHookFallback();
			}

#ifdef RSA_SUPPORT

			/*
			 * Making sure PPK is set for efuse error cases
			 */
			SetPpk();
#endif

			/*
			 * Search for next valid image
			 */
			Status = NextValidImageCheck();
			if(Status != XST_SUCCESS){
				fsbl_printf(DEBUG_INFO,"\r\nNo Image Found\r\n");
				ClearFSBLIn();
				FsblHookFallback();
			}

			/*
			 * Load next valid image
			 */
			HandoffAddr = LoadBootImage();

			/*
			 * Handoff to next image
			 */
			FsblHandoff(HandoffAddr);
		} else {
			fsbl_printf(DEBUG_INFO,"System Initialization Failed\r\n");
			fsbl_printf(DEBUG_INFO,"\r\nNo Image Search\r\n");
			ClearFSBLIn();
			FsblHookFallback();
		}
	}

	/*
	 * Reset PS, so Boot ROM will restart
	 */
	Xil_Out32(PS_RST_CTRL_REG, PS_RST_MASK);
}


/******************************************************************************/
/**
*
* This function hands the A9/PS to the loaded user code.
*
* @param	none
*
* @return	none
*
* @note		This function does not return.
*
****************************************************************************/
void FsblHandoff(u32 FsblStartAddr)
{
	u32 Status;

	/*
	 * Enable level shifter
	 */
	if(BitstreamFlag) {
		/*
		 * FSBL will not enable the level shifters for a NON PS instantiated
		 * Bitstream
		 * CR# 671028
		 * This flag can be set during compilation for a NON PS instantiated
		 * bitstream
		 */
#ifndef NON_PS_INSTANTIATED_BITSTREAM
#ifdef PS7_POST_CONFIG
		ps7_post_config();
		/*
		 * Unlock SLCR for SLCR register write
		 */
		SlcrUnlock();
#else
	/*
	 * Set Level Shifters DT618760
	 */
	Xil_Out32(PS_LVL_SHFTR_EN, LVL_PL_PS);
	fsbl_printf(DEBUG_INFO,"Enabling Level Shifters PL to PS "
			"Address = 0x%x Value = 0x%x \n\r",
			PS_LVL_SHFTR_EN, Xil_In32(PS_LVL_SHFTR_EN));

	/*
	 * Enable AXI interface
	 */
	Xil_Out32(FPGA_RESET_REG, 0);
	fsbl_printf(DEBUG_INFO,"AXI Interface enabled \n\r");
	fsbl_printf(DEBUG_INFO, "FPGA Reset Register "
			"Address = 0x%x , Value = 0x%x \r\n",
			FPGA_RESET_REG ,Xil_In32(FPGA_RESET_REG));
#endif
#endif
	}

	/*
	 * FSBL user hook call before handoff to the application
	 */
	Status = FsblHookBeforeHandoff();
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"FSBL_HANDOFF_HOOK_FAIL\r\n");
 		OutputStatus(FSBL_HANDOFF_HOOK_FAIL);
		FsblFallback();
	}

#ifdef XPAR_XWDTPS_0_BASEADDR
	XWdtPs_Stop(&Watchdog);
#endif

	/*
	 * Clear our mark in reboot status register
	 */
	ClearFSBLIn();

	if(FsblStartAddr == 0) {
		/*
		 * SLCR lock
		 */
		SlcrLock();

		fsbl_printf(DEBUG_INFO,"No Execution Address JTAG handoff \r\n");
		FsblHandoffJtagExit();
	} else {
		fsbl_printf(DEBUG_GENERAL,"SUCCESSFUL_HANDOFF\r\n");
		OutputStatus(SUCCESSFUL_HANDOFF);
		FsblHandoffExit(FsblStartAddr);
	}

	OutputStatus(ILLEGAL_RETURN);

	FsblFallback();
}

/******************************************************************************/
/**
*
* This function outputs the status for the provided State in the boot process.
*
* @param	State is where in the boot process the output is desired.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void OutputStatus(u32 State)
{
#ifdef STDOUT_BASEADDRESS
#ifdef XPAR_XUARTPS_0_BASEADDR
	u32 UartReg = 0;
#endif

	fsbl_printf(DEBUG_GENERAL,"FSBL Status = 0x%.4lx\r\n", State);
	/*
	 * The TX buffer needs to be flushed out
	 * If this is not done some of the prints will not appear on the
	 * serial output
	 */
#ifdef XPAR_XUARTPS_0_BASEADDR
	UartReg = Xil_In32(STDOUT_BASEADDRESS + XUARTPS_SR_OFFSET);
	while ((UartReg & XUARTPS_SR_TXEMPTY) != XUARTPS_SR_TXEMPTY) {
		UartReg = Xil_In32(STDOUT_BASEADDRESS + XUARTPS_SR_OFFSET);
	}
#endif
#endif
}

/******************************************************************************/
/**
*
* This function handles the error and lockdown processing and outputs the status
* for the provided State in the boot process.
*
* This function is called upon exceptions.
*
* @param	State - where in the boot process the error occured.
*
* @return	None.
*
* @note		This function does not return, the PS block is reset
*
****************************************************************************/
void ErrorLockdown(u32 State) 
{
	/*
	 * Store the error status
	 */
	OutputStatus(State);

	/*
	 * Fall back
	 */
	FsblFallback();
}

/******************************************************************************/
/**
*
* This function copies a memory region to another memory region
*
* @param 	s1 is starting address for destination
* @param 	s2 is starting address for the source
* @param 	n is the number of bytes to copy
*
* @return	Starting address for destination
*
****************************************************************************/
void *(memcpy_rom)(void * s1, const void * s2, u32 n)
{
	char *dst = (char *)s1;
	const char *src = (char *)s2;

	/*
	 * Loop and copy
	 */
	while (n-- != 0)
		*dst++ = *src++;
	return s1;
}
/******************************************************************************/
/**
*
* This function copies a string to another, the source string must be null-
* terminated.
*
* @param 	Dest is starting address for the destination string
* @param 	Src is starting address for the source string
*
* @return	Starting address for the destination string
*
****************************************************************************/
char *strcpy_rom(char *Dest, const char *Src)
{
	unsigned i;
	for (i=0; Src[i] != '\0'; ++i)
		Dest[i] = Src[i];
	Dest[i] = '\0';
	return Dest;
}


/******************************************************************************/
/**
*
* This function sets FSBL is running mask in reboot status register
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void MarkFSBLIn(void)
{
	Xil_Out32(REBOOT_STATUS_REG,
		Xil_In32(REBOOT_STATUS_REG) | FSBL_IN_MASK);
}


/******************************************************************************/
/**
*
* This function clears FSBL is running mask in reboot status register
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void ClearFSBLIn(void) 
{
	Xil_Out32(REBOOT_STATUS_REG,
		(Xil_In32(REBOOT_STATUS_REG)) &	~(FSBL_FAIL_MASK));
}

/******************************************************************************/
/**
*
* This function Registers the Exception Handlers
*
* @param	None.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
static void RegisterHandlers(void) 
{
	Xil_ExceptionInit();

	 /*
	 * Initialize the vector table. Register the stub Handler for each
	 * exception.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_UNDEFINED_INT,
					(Xil_ExceptionHandler)Undef_Handler,
					(void *) 0);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_SWI_INT,
					(Xil_ExceptionHandler)SVC_Handler,
					(void *) 0);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_PREFETCH_ABORT_INT,
				(Xil_ExceptionHandler)PreFetch_Abort_Handler,
				(void *) 0);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_DATA_ABORT_INT,
				(Xil_ExceptionHandler)Data_Abort_Handler,
				(void *) 0);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)IRQ_Handler,(void *) 0);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_FIQ_INT,
			(Xil_ExceptionHandler)FIQ_Handler,(void *) 0);

	Xil_ExceptionEnable();

}

static void Undef_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"UNDEFINED_HANDLER\r\n");
	ErrorLockdown (EXCEPTION_ID_UNDEFINED_INT);
}

static void SVC_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"SVC_HANDLER \r\n");
	ErrorLockdown (EXCEPTION_ID_SWI_INT);
}

static void PreFetch_Abort_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"PREFETCH_ABORT_HANDLER \r\n");
	ErrorLockdown (EXCEPTION_ID_PREFETCH_ABORT_INT);
}

static void Data_Abort_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"DATA_ABORT_HANDLER \r\n");
	ErrorLockdown (EXCEPTION_ID_DATA_ABORT_INT);
}

static void IRQ_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"IRQ_HANDLER \r\n");
	ErrorLockdown (EXCEPTION_ID_IRQ_INT);
}

static void FIQ_Handler (void)
{
	fsbl_printf(DEBUG_GENERAL,"FIQ_HANDLER \r\n");
	ErrorLockdown (EXCEPTION_ID_FIQ_INT);
}


/******************************************************************************/
/**
*
* This function Updates the Multi boot Register to enable golden image
* search for boot rom
*
* @param None
*
* @return
* return  none
*
****************************************************************************/
static void Update_MultiBootRegister(void)
{
	u32 MultiBootReg = 0;

	if (Silicon_Version != SILICON_VERSION_1) {
		/*
		 * Read the mulitboot register
		 */
		MultiBootReg =	XDcfg_ReadReg(DcfgInstPtr->Config.BaseAddr,
					XDCFG_MULTIBOOT_ADDR_OFFSET);

		/*
		 * Incrementing multiboot register by one
		 */
		MultiBootReg++;

		XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr,
				XDCFG_MULTIBOOT_ADDR_OFFSET,
				MultiBootReg);

		fsbl_printf(DEBUG_INFO,"Updated MultiBootReg = 0x%08lx\r\n",
				MultiBootReg);
	}
}


/******************************************************************************
*
* This function reset the CPU and goes for Boot ROM fallback handling
*
* @param	None
*
* @return	None
*
* @note		None
*
*******************************************************************************/

u32 GetResetReason(void)
{
	u32 Regval;

	/* We are using REBOOT_STATUS_REG, we have to use bits 23:16 */
	/* for storing the RESET_REASON register value*/
	Regval = ((Xil_In32(REBOOT_STATUS_REG) >> 16) & 0xFF);

	return Regval;
}


/******************************************************************************
*
* This function Gets the ticks from the Global Timer
*
* @param	Current time
*
* @return
*			None
*
* @note		None
*
*******************************************************************************/
#ifdef FSBL_PERF
void FsblGetGlobalTime (XTime *tCur)
{
	XTime_GetTime(tCur);
}


/******************************************************************************
*
* This function Measures the execution time
*
* @param	Current time , End time
*
* @return
*			None
*
* @note		None
*
*******************************************************************************/
void FsblMeasurePerfTime (XTime tCur, XTime tEnd)
{
	double tDiff = 0.0;
	double tPerfSeconds;
	XTime_GetTime(&tEnd);
	tDiff  = (double)tEnd - (double)tCur;

	/*
	 * Convert tPerf into Seconds
	 */
	tPerfSeconds = tDiff/COUNTS_PER_SECOND;

#if defined(STDOUT_BASEADDRESS)
	printf("%f seconds \r\n",tPerfSeconds);
#endif

}
#endif

/******************************************************************************
*
* This function initializes the Watchdog driver and starts the timer
*
* @param	None
*
* @return
*		- XST_SUCCESS if the Watchdog driver is initialized
*		- XST_FAILURE if Watchdog driver initialization fails
*
* @note		None
*
*******************************************************************************/
#ifdef XPAR_XWDTPS_0_BASEADDR
int InitWatchDog(void)
{
	u32 Status = XST_SUCCESS;
	XWdtPs_Config *ConfigPtr; 	/* Config structure of the WatchDog Timer */
	u32 CounterValue = 1;

	ConfigPtr = XWdtPs_LookupConfig(WDT_DEVICE_ID);
	Status = XWdtPs_CfgInitialize(&Watchdog,
				ConfigPtr,
				ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO,"Watchdog Driver init Failed \n\r");
		return XST_FAILURE;
	}

	/*
	 * Setting the divider value
	 */
	XWdtPs_SetControlValue(&Watchdog,
			XWDTPS_CLK_PRESCALE,
			XWDTPS_CCR_PSCALE_4096);
	/*
	 * Convert time to  Watchdog counter reset value
	 */
	CounterValue = ConvertTime_WdtCounter(WDT_EXPIRE_TIME);

	/*
	 * Set the Watchdog counter reset value
	 */
	XWdtPs_SetControlValue(&Watchdog,
			XWDTPS_COUNTER_RESET,
			CounterValue);
	/*
	 * enable reset output, as we are only using this as a basic counter
	 */
	XWdtPs_EnableOutput(&Watchdog, XWDTPS_RESET_SIGNAL);

	/*
	 * Start the Watchdog timer
	 */
	XWdtPs_Start(&Watchdog);

	XWdtPs_RestartWdt(&Watchdog);

	return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function checks whether WDT reset has happened during FSBL run
*
* If WDT reset happened during FSBL run, then need to fallback
*
* @param	None.
*
* @return
*		None
*
* @note		None
*
****************************************************************************/
void CheckWDTReset(void)
{
	u32 ResetReason;
	u32 RebootStatusRegister;

	RebootStatusRegister = Xil_In32(REBOOT_STATUS_REG);

	/*
	 *  For 1.0 Silicon the reason for Reset is in the ResetReason Register
	 * Hence this register can be read to know the cause for previous reset
	 * that happened.
	 * Check if that reset is a Software WatchDog reset that happened
	 */
	if (Silicon_Version == SILICON_VERSION_1) {
		ResetReason = Xil_In32(RESET_REASON_REG);
	} else {
		ResetReason = GetResetReason();
	}
	/*
	 * If the FSBL_IN_MASK Has not been cleared, WDT happened
	 * before FSBL exits
	 */
	if ((ResetReason & RESET_REASON_SWDT) == RESET_REASON_SWDT ) {
		if ((RebootStatusRegister & FSBL_FAIL_MASK) == FSBL_IN_MASK) {
			/*
			 * Clear the SWDT Reset bit
			 */
			ResetReason &= ~RESET_REASON_SWDT;
			if (Silicon_Version == SILICON_VERSION_1) {
				/*
				 * for 1.0 Silicon we need to write
				 * 1 to the RESET REASON Clear register 
				 */
				Xil_Out32(RESET_REASON_CLR, 1);
			} else {
				Xil_Out32(REBOOT_STATUS_REG, ResetReason);
			}

			fsbl_printf(DEBUG_GENERAL,"WDT_RESET_OCCURED \n\r");
		}
	}
}


/******************************************************************************
*
* This function converts time into Watchdog counter value
*
* @param	watchdog expire time in seconds
*
* @return
*			Counter value for Watchdog
*
* @note		None
*
*******************************************************************************/
u32 ConvertTime_WdtCounter(u32 seconds)
{
	double time = 0.0;
	double CounterValue;
	u32 Crv = 0;
	u32 Prescaler,PrescalerValue;

	Prescaler = XWdtPs_GetControlValue(&Watchdog, XWDTPS_CLK_PRESCALE);

	if (Prescaler == XWDTPS_CCR_PSCALE_0008)
		PrescalerValue = 8;
	if (Prescaler == XWDTPS_CCR_PSCALE_0064)
		PrescalerValue = 64;
	if (Prescaler == XWDTPS_CCR_PSCALE_4096)
		PrescalerValue = 4096;

	time = (double)(PrescalerValue) / (double)XPAR_PS7_WDT_0_WDT_CLK_FREQ_HZ;

	CounterValue = seconds / time;

	Crv = (u32)CounterValue;
	Crv >>= WDT_CRV_SHIFT;

	return Crv;
}

#endif


/******************************************************************************
*
* This function Gets the Silicon Version stores in global variable
*
* @param	None
*
* @return 	None
*
* @note		None
*
*******************************************************************************/
void GetSiliconVersion(void)
{
	/*
	 * Get the silicon version
	 */
	Silicon_Version = XDcfg_GetPsVersion(DcfgInstPtr);
	if(Silicon_Version == SILICON_VERSION_3_1) {
		fsbl_printf(DEBUG_GENERAL,"Silicon Version 3.1\r\n");
	} else {
		fsbl_printf(DEBUG_GENERAL,"Silicon Version %lu.0\r\n",
				Silicon_Version + 1);
	}
}


/******************************************************************************
*
* This function HeaderChecksum will calculates the header checksum and
* compares with checksum read from flash
*
* @param 	FlashOffsetAddress Flash offset address
*
* @return
*		- XST_SUCCESS if ID matches
*		- XST_FAILURE if ID mismatches
*
* @note		None
*
*******************************************************************************/
u32 HeaderChecksum(u32 FlashOffsetAddress){
	u32 Checksum = 0;
	u32 Count;
	u32 TempValue = 0;

	for (Count = 0; Count < IMAGE_HEADER_CHECKSUM_COUNT; Count++) {
		/*
		 * Read the word from the header
		 */
		MoveImage(FlashOffsetAddress + IMAGE_WIDTH_CHECK_OFFSET + (Count*4), (u32)&TempValue, 4);

		/*
		 * Update checksum
		 */
		Checksum += TempValue;
	}

	/*
	 * Invert checksum, last bit of error checking
	 */
	Checksum ^= 0xFFFFFFFF;
	MoveImage(FlashOffsetAddress + IMAGE_CHECKSUM_OFFSET, (u32)&TempValue, 4);

	/*
	 * Validate the checksum
	 */
	if (TempValue != Checksum){
		fsbl_printf(DEBUG_INFO, "Checksum = %8.8lx\r\n", Checksum);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/******************************************************************************
*
* This function ImageCheckID will do check for XLNX pattern
*
* @param	FlashOffsetAddress Flash offset address
*
* @return
*		- XST_SUCCESS if ID matches
*		- XST_FAILURE if ID mismatches
*
* @note		None
*
*******************************************************************************/
u32 ImageCheckID(u32 FlashOffsetAddress){
	u32 ID;

	/*
	 * Read in the header info
	 */
	MoveImage(FlashOffsetAddress + IMAGE_IDENT_OFFSET, (u32)&ID, 4);

	/*
	 * Check the ID, make sure image is XLNX format
	 */
	if (ID != IMAGE_IDENT){
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/******************************************************************************
*
* This function NextValidImageCheck search for valid boot image
*
* @param	None
*
* @return
*		- XST_SUCCESS if valid image found
*		- XST_FAILURE if no image found
*
* @note		None
*
*******************************************************************************/
u32 NextValidImageCheck(void)
{
	u32 ImageBaseAddr;
	u32 MultiBootReg;
	u32 BootDevMaxSize=0;

	fsbl_printf(DEBUG_GENERAL, "Searching For Next Valid Image");
	
	/*
	 * Setting variable with maximum flash size based on boot mode
	 */
#ifdef XPAR_PS7_QSPI_LINEAR_0_S_AXI_BASEADDR
	if (FlashReadBaseAddress == XPS_QSPI_LINEAR_BASEADDR) {
		BootDevMaxSize = QspiFlashSize;
	}
#endif

	if (FlashReadBaseAddress == XPS_NAND_BASEADDR) {
		BootDevMaxSize  = NAND_FLASH_SIZE;
	}

	if (FlashReadBaseAddress == XPS_NOR_BASEADDR) {
		BootDevMaxSize  = NOR_FLASH_SIZE;
	}

	/*
	 * Read the multiboot register
	 */
	MultiBootReg =  XDcfg_ReadReg(DcfgInstPtr->Config.BaseAddr,
			XDCFG_MULTIBOOT_ADDR_OFFSET);

	/*
	 * Compute the image start address
	 */
	ImageBaseAddr = (MultiBootReg & PCAP_MBOOT_REG_REBOOT_OFFSET_MASK)
								* GOLDEN_IMAGE_OFFSET;
	
	/*
	 * Valid image search continue till end of the flash
	 * With increment 32KB in each iteration
	 */
	while (ImageBaseAddr < BootDevMaxSize) {

		fsbl_printf(DEBUG_INFO,".");

		/*
		 * Valid image search using XLNX pattern at fixed offset
		 * and header checksum
		 */
		if ((ImageCheckID(ImageBaseAddr) == XST_SUCCESS) &&
				(HeaderChecksum(ImageBaseAddr) == XST_SUCCESS)) {

			fsbl_printf(DEBUG_GENERAL, "\r\nImage found, offset: 0x%.8lx\r\n",
					ImageBaseAddr);
			/*
			 * Update multiboot register
			 */
			XDcfg_WriteReg(DcfgInstPtr->Config.BaseAddr,
					XDCFG_MULTIBOOT_ADDR_OFFSET,
					MultiBootReg);

			return XST_SUCCESS;
		}

		/*
		 * Increment mulitboot count
		 */
		MultiBootReg++;

		/*
		 * Compute the image start address
		 */
		ImageBaseAddr = (MultiBootReg & PCAP_MBOOT_REG_REBOOT_OFFSET_MASK)
							* GOLDEN_IMAGE_OFFSET;
	}

	return XST_FAILURE;
}

/******************************************************************************/
/**
*
* This function Checks for the ddr initialization completion
*
* @param	None.
*
* @return
*		- XST_SUCCESS if the initialization is successful
*		- XST_FAILURE if the  initialization is NOT successful
*
* @note		None.
*
****************************************************************************/
u32 DDRInitCheck(void)
{
	u32 ReadVal;

	/*
	 * Write and Read from the DDR location for sanity checks
	 */
	Xil_Out32(DDR_START_ADDR, DDR_TEST_PATTERN);
	ReadVal = Xil_In32(DDR_START_ADDR);
	if (ReadVal != DDR_TEST_PATTERN) {
		return XST_FAILURE;
	}

	/*
	 * Write and Read from the DDR location for sanity checks
	 */
	Xil_Out32(DDR_START_ADDR + DDR_TEST_OFFSET, DDR_TEST_PATTERN);
	ReadVal = Xil_In32(DDR_START_ADDR + DDR_TEST_OFFSET);
	if (ReadVal != DDR_TEST_PATTERN) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
