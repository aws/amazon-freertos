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
* @file fsbl.h
*
* Contains the function prototypes, defines and macros for the
* First Stage Boot Loader (FSBL) functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a	jz	03/04/11	Initial release
* 2.00a	mb 	06/06/12	Removed the qspi define, will be picked from
*						xparameters.h file
* 3.00a np/mb 08/08/12	Added the error codes for the FSBL hook errors.
* 						Added the debug levels
* 4.00a sgd 02/28/13	Removed DDR initialization check
*                       Removed DDR ECC initialization code
*						Modified hand off address check to 1MB
*						Added RSA authentication support
*						Removed LPBK_DLY_ADJ register setting code as we use
* 					 	divisor 8
*						Removed check for Fabric is already initialized
*
* 						CR's fixed and description
* 						689026:	FSBL doesn't hold PL resets active during
* 						bit download
* 						Resolution: PL resets are released just before
* 						handoff
*
* 						689077:	FSBL hangs at Handoff clearing the
* 						TX UART buffer
*						Resolution: STDOUT_BASEADDRESS macro value changes
*						based UART select, hence used STDOUT_BASEADDRESS
*						as UART base address
*
* 						695578: FSBL failed to load standalone application
* 						in secure bootmode
*               		Resolution: Application will be placed at load address
*               		instead of DDR temporary address
*
*               		699475: FSBL functionality is broken and its
*               		not able to boot in QSPI/NAND bootmode
*               		Resolution: New flags are added DevCfg driver
*               		for handling loopback
*               		XDCFG_CONCURRENT_NONSEC_READ_WRITE
*                       XDCFG_CONCURRENT_SECURE_READ_WRITE
*
*               		683145: Define stack area for FIQ, UNDEF modes
*               		in linker file
*               		Resolution: FSBL linker modified to create stack area
*               		for FIQ, UNDEF
*                       
*                       705664: FSBL fails to decrypt the bitstream when 
*                       the image is AES encrypted using non-zero key value
*                       Resolution: Fabric cleaning will not be done
*                       for AES-E-Fuse encryption
*                       
*                       Watchdog disabled for AES E-Fuse encryption
*
* 5.00a sgd 05/17/13    Fallback support for E-Fuse encryption
*                       Added QSPI Flash Size > 128Mbit support
* 					    QSPI Dual Stack support
* 					    Added Md5 checksum support
*
*                       CR's fixed and description
*                       692045	FSBL: Linker script of FSBL has PHDR workaround,
* 					    this needs to be fixed
* 					    Resolution: Removed PHDR from Linker file
*                       
*                       704287	FSBL: fsbl.h file has a few error codes that 
*                       are not used by FSBL, that needs to be removed
*                       Resolution: Removed unused error codes
*
*                       704379	FSBL: Check if DDR is in proper state before
*                       handoff
* 					    Resolution: Added DDR initialization check
* 					                           
*                       709077	If FSBL_DEBUG and FSBL_DEBUG_INFO are defined, 
*                       the debug level is FSBL_DEBUG only.
*                       
*                       710128 FSBL: Linux boot failing without load attribute
*                       set for Linux partitions in BIF
*                       Resolution: FSBL will load partitions with valid load
*                       address and stop loading if any invalid load address
*
*                       708728 Issues seen while making HP interconnect
*                       32 bit wide
*                       Resolution: ps7_post_config function generated by PCW
*                       will be called after Bit stream download
*                       Added MMC support
* 6.00a	kc	07/31/2013	CR's fixed and description
* 						724166 FSBL doesn’t use PPK authenticated by Boot ROM
* 						 for authenticating the Partition images
* 						Resolution: FSBL now uses the PPK left by Boot ROM in
* 						OCM for authencating the SPK
*
* 						724165 Partition Header used by FSBL is not
* 						authenticated
* 						Resolution: FSBL now authenticates the partition header
*
* 						691150 ps7_init does not check for peripheral
* 						initialization failures or timeout on polls
* 						Resolution: Return value of ps7_init() is now checked
* 						by FSBL and prints the error string
*
* 						708316  PS7_init.tcl file should have Error mechanism
* 						for all mask_poll
* 						Resolution: Return value of ps7_init() is now checked
* 						by FSBL and prints the error string
*
* 						732062 FSBL fails to build if UART not available
* 						Resolution: Added define to call xil_printf only
* 						if uart is defined
*
* 						722979 Provide customer-friendly changelogs in FSBL
* 						Resolution: Added CR description for all the files
*
* 						732865 Backward compatibility for ps7_init function
*						Resolution: Added a new define for ps7_init success
*						and value is defined based on ps7_init define
*
*						Fix for CR#739711 - FSBL not able to read Large
*						QSPI (512M) in IO Mode
*						Resolution: Modified the address calculation
*						algorithm in dual parallel mode for QSPI
*
* 7.00a kc  10/18/13    Integrated SD/MMC driver
*			10/23/13	Support for armcc compiler added
*						741003 FSBL has to check the HMAC error status after 
*						decryption
*						Resolution: Added code for checking the error status 
*						after PCAP completion
*						739968 FSBL should do the QSPI config settings for 
*						Dual parallel configuration in IO mode
*						Resolution: Added QSPI config settings in qspi.c
*						724620 FSBL: How to handle PCAP_MODE after bitstream 
*						configuration.
*						Resolution: PCAP_MODE and PCAP_PR bits are now cleared  
* 						after PCAP transfer completion
*						726178 In the 14.6 FSBL function FabricInit() PROG_B 
*						is kept active for 5mS.
*						Resolution: PROG_B is now kept active for 5mS only incase 
*						if efuse is the aes key source.
*						755245 FSBL does not load partition if eMMC has only 
*						one partition
*						Resolution: Changed the if condition for MMC
*			12/04/13    764382 FSBL: How to handle PCAP_MODE after bitstream 
*						configuration
*						Resolution: Reverted back the changes of 724620. PCAP_MODE
*						and PCAP_PR bits are not changed
* 8.00a kc  01/16/13    767798 Fsbl MD5 Checksum failiure for encrypted images
* 						Resolution: For checksum enabled partitions, total 
*						total partition image length is copied now.
*						761895 FSBL should authenticate image only if
*						partition owner was not set to u-boot
*						Resolution: Partition owner check added in 
*						image_mover.c
* 			02/20/14	775631 - FSBL: FsblGetGlobalTimer() is not proper
*						Resolution: Function argument is updated from value
*						to pointer to reflect updated value
* 9.00a kc  04/16/14	773866 - SetPpk() will fail on secure fallback
*						unless FSBL* and FSBL are identical in length
*						Resolution: PPK is set only once now.
*						785778 - FSBL takes 8 seconds to
* 						authenticate (RSA) a bitstream on zc706
* 						Resolution: Data Caches are enabled only for
* 						authentication.
* 						791245 - Use of xilrsa in fsbl
* 						Resolution: Rsa library is removed from fsbl source
* 						and xilrsa is used from BSP
* 10.00a kc 07/15/14	804595 Zynq FSBL - Issues with
* 						fallback image offset handling using MD5
* 						Resolution: Updated the checksum offset to add with
* 						image base address
* 						782309 Fallback support for AES
* 						encryption with E-Fuse - Enhancement
* 						Resolution: Same as 773866
* 						809336 Minor code cleanup
* 						Resolution Minor code changes
*        kc 08/27/14	820356 - FSBL compilation fails with IAR compiler
* 						Resolution: Change of __asm__ to __asm
* 11.00a kv 10/08/14	826030 - FSBL:LinearBootDeviceFlag is not initialized
*						in IO mode case.Due to which the variable is
*						remaining in unknown state.
*						Resolution: LinearBootDeviceFlag is initialized 0
*						in main.c
* 12.00a ssc 12/11/14	839182 - FSBL -In the file sd.c, f_mount is called with
*                       two arguments but f_mount is expecting the 3 arguments
*                       from build 2015.1_1210_1, causing compilation error.
*						Resolution: Arguments for f_mount in InitSD() are
*						changed as per new signature.
* 13.00a ssc 04/10/15	846899 - FSBL -In the file pcap.c, to clear DMA done
*                       count, devcfg.INT_STS register is written to, which is
*                       not correct.
*                       Resolution: Corresponding fields in the devcfg.STATUS
*                       register are written to, for clearing DMA done count.
* 14.00a gan 01/13/16   869081 -(2016.1)FSBL -In qspi.c, FSBL picks the qspi
*						read command from LQSPI_CFG register instead of hard
*		   				coded read command (0x6B).
* 15.00a gan 07/21/16   953654 -(2016.3)FSBL -In pcap.c/pcap.h/main.c,
* 						Fabric Initialization sequence is modified to check
* 						the PL power before sequence starts and checking INIT_B
* 						reset status twice in case of failure.
* 16.00a gan 08/02/16   Fix for CR# 955897 -(2016.3)FSBL -
* 						In pcap.c, check pl power through MCTRL register
* 						for 3.0 and later versions of silicon.
* 17.00a bsv 27/03/18	Fix for CR# 996973  Add code under JTAG_ENABLE_LEVEL_SHIFTERS macro
* 						to enable level shifters in jtag boot mode.
* </pre>
*
* </pre>
*
* @note
*
* Flags in FSBL
*
* FSBL_PERF
*
* This Flag can be set at compilation time. This flag is set for
* measuring the performance of FSBL.That is the time taken to execute is
* measured.when this flag is set.Execution time with reference to
* global timer is taken here
*
* Total Execution time is the time taken for executing FSBL till handoff
* to any application .
* If there is a bitstream in the partition header then the
* execution time includes the copying of the bitstream to DDR
* (in case of SD/NAND bootmode)
* and programming the devcfg dma is accounted.
*
* FSBL provides two debug levels
* DEBUG GENERAL - fsbl_printf under this category will appear only when the
* FSBL_DEBUG flag is set during compilation
* DEBUG_INFO - fsbl_printf under this category will appear when the
* FSBL_DEBUG_INFO flag is set during compilation
* For a more detailed output log can be used.
* FSBL_DEBUG_RSA - Define this macro to print more detailed values used in
* RSA functions
* These macros are input to the fsbl_printf function
*
* DEBUG LEVELS
* FSBL_DEBUG level is level 1, when this flag is set all the fsbl_prints
* that are with the DEBUG_GENERAL argument are shown
* FSBL_DEBUG_INFO is level 2, when this flag is set during the
* compilation , the fsbl_printf with DEBUG_INFO will appear on the com port
*
* DEFAULT LEVEL
* By default no print messages will appear.
*
* NON_PS_INSTANTIATED_BITSTREAM
*
* FSBL will not enable the level shifters for a NON PS instantiated
* Bitstream.This flag can be set during compilation for a NON PS instantiated
* bitstream
*
* ECC_ENABLE
* This flag will be defined in the ps7_init.h file when ECC is enabled
* in the DDR configuration (XPS GUI)
*
* RSA_SUPPORT
* This flag is used to enable authentication feature
* Default this macro disabled, reason to avoid increase in code size
*
* MMC_SUPPORT
* This flag is used to enable MMC support feature
*
* JTAG_ENABLE_LEVEL_SHIFTERS
* FSBL will not enable the level shifters for jtag boot mode. This flag can be
* set during compilation for jtag boot mode to enable level shifters.
*
*******************************************************************************/
#ifndef XIL_FSBL_H
#define XIL_FSBL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xil_io.h"
#include "xparameters.h"
#include "xpseudo_asm.h"
#include "xil_printf.h"
#include "pcap.h"
#include "fsbl_debug.h"
#include "ps7_init.h"
#ifdef FSBL_PERF
#include "xtime_l.h"
#include <stdio.h>
#endif


/************************** Constant Definitions *****************************/
/*
 * SDK release version
 */
#define SDK_RELEASE_YEAR	2018
#define SDK_RELEASE_QUARTER	2

#define WORD_LENGTH_SHIFT	2

/*
 * On a Successful handoff to an application FSBL sets this SUCCESS code
 */
#define SUCCESSFUL_HANDOFF		0x1	/* Successful Handoff */

/*
 * Backward compatibility for ps7_init
 */
#ifdef NEW_PS7_ERR_CODE
#define FSBL_PS7_INIT_SUCCESS	PS7_INIT_SUCCESS
#else
#define FSBL_PS7_INIT_SUCCESS	(1)
#endif

/*
 * ERROR CODES
 * The following are the Error codes that FSBL uses
 * If the Debug prints are enabled only then the error codes will be
 * seen on the com port.Without the debug prints enabled no error codes will
 * be visible.There are not saved in any register
 * Boot Mode States used for error and status output
 * Error codes are defined below
 */
#define ILLEGAL_BOOT_MODE		0xA000 /**< Illegal boot mode */
#define ILLEGAL_RETURN			0xA001 /**< Illegal return */
#define PCAP_INIT_FAIL			0xA002 /**< Pcap driver Init Failed */
#define DECRYPTION_FAIL			0xA003 /**< Decryption Failed */
#define BITSTREAM_DOWNLOAD_FAIL	0xA004 /**< Bitstream download fail */
#define DMA_TRANSFER_FAIL		0xA005 /**< DMA Transfer Fail */
#define INVALID_FLASH_ADDRESS	0xA006 /**< Invalid Flash Address */
#define DDR_INIT_FAIL			0xA007 /**< DDR Init Fail */
#define NO_DDR					0xA008 /**< DDR missing */
#define SD_INIT_FAIL			0xA009 /**< SD Init fail */
#define NAND_INIT_FAIL			0xA00A /**< Nand Init Fail */
#define PARTITION_MOVE_FAIL		0xA00B /**< Partition move fail */
#define AUTHENTICATION_FAIL		0xA00C /**< Authentication fail */
#define INVALID_HEADER_FAIL		0xA00D /**< Invalid header fail */
#define GET_HEADER_INFO_FAIL	0xA00E /**< Get header fail */
#define INVALID_LOAD_ADDRESS_FAIL	0xA00F /**< Invalid load address fail */
#define PARTITION_CHECKSUM_FAIL		0xA010 /**< Partition checksum fail */
#define RSA_SUPPORT_NOT_ENABLED_FAIL	0xA011 /**< RSA not enabled fail */
#define PS7_INIT_FAIL			0xA012 /**< ps7 Init Fail */
/*
 * FSBL Exception error codes
 */
#define EXCEPTION_ID_UNDEFINED_INT	0xA301 /**< Undefined INT Exception */
#define EXCEPTION_ID_SWI_INT		0xA302 /**< SWI INT Exception */
#define EXCEPTION_ID_PREFETCH_ABORT_INT	0xA303 /**< Prefetch Abort xception */
#define EXCEPTION_ID_DATA_ABORT_INT	0xA304 /**< Data Abort Exception */
#define EXCEPTION_ID_IRQ_INT		0xA305 /**< IRQ Exception Occurred */
#define EXCEPTION_ID_FIQ_INT		0xA306 /**< FIQ Exception Occurred */

/*
 * FSBL hook routine failures
 */
#define FSBL_HANDOFF_HOOK_FAIL		0xA401 /**< FSBL handoff hook failed */
#define FSBL_BEFORE_BSTREAM_HOOK_FAIL	0xA402 /**< FSBL before bit stream
						download hook failed */
#define FSBL_AFTER_BSTREAM_HOOK_FAIL	0xA403 /**< FSBL after bitstream
						download hook failed */

/*
 * Watchdog related Error codes
 */
#define WDT_RESET_OCCURED		0xA501 /**< WDT Reset happened in FSBL */
#define WDT_INIT_FAIL			0xA502 /**< WDT driver INIT failed */

/*
 * SLCR Registers
 */
#define PS_RST_CTRL_REG			(XPS_SYS_CTRL_BASEADDR + 0x200)
#define FPGA_RESET_REG			(XPS_SYS_CTRL_BASEADDR + 0x240)
#define RESET_REASON_REG		(XPS_SYS_CTRL_BASEADDR + 0x250)
#define RESET_REASON_CLR		(XPS_SYS_CTRL_BASEADDR + 0x254)
#define REBOOT_STATUS_REG		(XPS_SYS_CTRL_BASEADDR + 0x258)
#define BOOT_MODE_REG			(XPS_SYS_CTRL_BASEADDR + 0x25C)
#define PS_LVL_SHFTR_EN			(XPS_SYS_CTRL_BASEADDR + 0x900)

/*
 * Efuse Status Register
 */
#define EFUSE_STATUS_REG			(0xF800D010)  /**< Efuse Status Register */
#define EFUSE_STATUS_RSA_ENABLE_MASK (0x400)  /**< Status of RSA enable */

/*
 * PS reset control register define
 */
#define PS_RST_MASK			0x1	/**< PS software reset */

/*
 * SLCR BOOT Mode Register defines
 */
#define BOOT_MODES_MASK			0x00000007 /**< FLASH types */

/*
 * Boot Modes
 */
#define JTAG_MODE			0x00000000 /**< JTAG Boot Mode */
#define QSPI_MODE			0x00000001 /**< QSPI Boot Mode */
#define NOR_FLASH_MODE		0x00000002 /**< NOR Boot Mode */
#define NAND_FLASH_MODE		0x00000004 /**< NAND Boot Mode */
#define SD_MODE				0x00000005 /**< SD Boot Mode */
#define MMC_MODE			0x00000006 /**< MMC Boot Device */

#define RESET_REASON_SRST		0x00000020 /**< Reason for reset is SRST */
#define RESET_REASON_SWDT		0x00000001 /**< Reason for reset is SWDT */

/*
 * Golden image offset
 */
#define GOLDEN_IMAGE_OFFSET		0x8000

/*
 * Silicon Version
 */
#define SILICON_VERSION_1 0
#define SILICON_VERSION_2 1
#define SILICON_VERSION_3 2
#define SILICON_VERSION_3_1 3

/*
 * DDR start address for storing the data temporarily(1M)
 * Need to finalize correct logic
 */
#ifdef XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define DDR_START_ADDR 	XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define DDR_END_ADDR	XPAR_PS7_DDR_0_S_AXI_HIGHADDR
#else
/*
 * In case of PL DDR, this macros defined based PL DDR address
 */
#define DDR_START_ADDR 	0x00
#define DDR_END_ADDR	0x00
#endif

#define DDR_TEMP_START_ADDR 	DDR_START_ADDR
/*
 * DDR test pattern
 */
#define DDR_TEST_PATTERN	0xAA55AA55
#define DDR_TEST_OFFSET		0x100000
/*
 *
 */
#define QSPI_DUAL_FLASH_SIZE	0x2000000; /*32MB*/
#define QSPI_SINGLE_FLASH_SIZE	0x1000000; /*16MB*/
#define NAND_FLASH_SIZE			0x8000000; /*128MB*/
#define NOR_FLASH_SIZE			0x2000000; /*32MB*/
#define	LQSPI_CFG_OFFSET		0xA0
#define LQSPI_CFG_DUAL_FLASH_MASK	0x40000000

/*
 * These are the SLCR lock and unlock macros
 */
#define SlcrUnlock()	Xil_Out32(XPS_SYS_CTRL_BASEADDR + 0x08, 0xDF0DDF0D)
#define SlcrLock()		Xil_Out32(XPS_SYS_CTRL_BASEADDR + 0x04, 0x767B767B)

#define IMAGE_HEADER_CHECKSUM_COUNT 10

/* Boot ROM Image defines */
#define IMAGE_WIDTH_CHECK_OFFSET        (0x020)	/**< 0xaa995566 Width Detection word */
#define IMAGE_IDENT_OFFSET              (0x024) /**< 0x584C4E58 "XLNX" */
#define IMAGE_ENC_FLAG_OFFSET           (0x028) /**< 0xA5C3C5A3 */
#define IMAGE_USR_DEF_OFFSET            (0x02C)	/**< undefined  could be used as  */
#define IMAGE_SOURCE_ADDR_OFFSET        (0x030)	/**< start address of image  */
#define IMAGE_BYTE_LEN_OFFSET           (0x034)	/**< length of image> in bytes  */
#define IMAGE_DEST_ADDR_OFFSET          (0x038)	/**< destination address in OCM */
#define IMAGE_EXECUTE_ADDR_OFFSET       (0x03c)	/**< address to start executing at */
#define IMAGE_TOT_BYTE_LEN_OFFSET       (0x040)	/**< total length of image in bytes */
#define IMAGE_QSPI_CFG_WORD_OFFSET      (0x044)	/**< QSPI configuration data */
#define IMAGE_CHECKSUM_OFFSET           (0x048) /**< Header Checksum offset */
#define IMAGE_IDENT                     (0x584C4E58) /**< XLNX pattern */

/* Reboot status register defines:
 * 0xF0000000 for FSBL fallback mask to notify Boot Rom
 * 0x60000000 for FSBL to mark that FSBL has not handoff yet
 * 0x00FFFFFF for user application to use across soft reset
 */
#define FSBL_FAIL_MASK		0xF0000000
#define FSBL_IN_MASK		0x60000000

/* The address that holds the base address for the image Boot ROM found */
#define BASEADDR_HOLDER		0xFFFFFFF8

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

void OutputStatus(u32 State);
void FsblFallback(void);

int FsblSetNextPartition(int Num);
void *(memcpy_rom)(void * s1, const void * s2, u32 n);
char *strcpy_rom(char *Dest, const char *Src);

void ClearFSBLIn(void);
void MarkFSBLIn(void);
void FsblHandoff(u32 FsblStartAddr);
u32 GetResetReason(void);

#ifdef FSBL_PERF
void FsblGetGlobalTime (XTime * tCur);
void FsblMeasurePerfTime (XTime tCur, XTime tEnd);
#endif
void GetSiliconVersion(void);
void FsblHandoffExit(u32 FsblStartAddr);
void FsblHandoffJtagExit();
/************************** Variable Definitions *****************************/
extern int SkipPartition;

/***************** Macros (Inline Functions) Definitions *********************/

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */
