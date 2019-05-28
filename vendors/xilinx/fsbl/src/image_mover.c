/******************************************************************************
*
* Copyright (C) 2011 - 2014 Xilinx, Inc.  All rights reserved.
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
* @file image_mover.c
*
* Move partitions to either DDR to execute or to program FPGA.
* It performs partition walk.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz	05/24/11	Initial release
* 2.00a jz	06/30/11	Updated partition header defs for 64-byte
*			 			alignment change in data2mem tool
* 2.00a mb	05/25/12	Updated for standalone based bsp FSBL
* 			 			Nand/SD encryption and review comments
* 3.00a np	08/30/12	Added FSBL user hook calls
* 						(before and after bitstream download.)
* 4.00a sgd	02/28/13	Fix for CR#691148 Secure bootmode error in devcfg test
*						Fix for CR#695578 FSBL failed to load standalone 
*						application in secure bootmode
*
* 4.00a sgd	04/23/13	Fix for CR#710128 FSBL failed to load standalone 
*						application in secure bootmode
* 5.00a kc	07/30/13	Fix for CR#724165 Partition Header used by FSBL 
*						is not authenticated
* 						Fix for CR#724166 FSBL doesn�t use PPK authenticated 
*						by Boot ROM for authenticating the Partition images 
* 						Fix for CR#732062 FSBL fails to build if UART not 
*						available 
* 7.00a kc  10/30/13    Fix for CR#755245 FSBL does not load partition
*                       if eMMC has only one partition
* 8.00a kc  01/16/13    Fix for CR#767798  FSBL MD5 Checksum failure
* 						for encrypted images
*						Fix for CR#761895 FSBL should authenticate image
*						only if partition owner was not set to u-boot
* 9.00a kc  04/16/14    Fix for CR#785778  FSBL takes 8 seconds to 
* 						authenticate (RSA) a bitstream on zc706
* 10.00a kc 07/15/14	Fix for CR#804595 Zynq FSBL - Issues with
* 						fallback image offset handling using MD5
* 						Fix for PR#782309 Fallback support for AES
* 						encryption with E-Fuse - Enhancement
*
* </pre>
*
* @note
*	A partition is either an executable or a bitstream to program FPGA
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "fsbl.h"
#include "image_mover.h"
#include "xil_printf.h"
#include "xreg_cortexa9.h"
#include "pcap.h"
#include "fsbl_hooks.h"
#include "md5.h"

#ifdef XPAR_XWDTPS_0_BASEADDR
#include "xwdtps.h"
#endif

#ifdef RSA_SUPPORT
#include "rsa.h"
#include "xil_cache.h"
#endif
/************************** Constant Definitions *****************************/

/* We are 32-bit machine */
#define MAXIMUM_IMAGE_WORD_LEN 0x40000000
#define MD5_CHECKSUM_SIZE   16

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
u32 ValidateParition(u32 StartAddr, u32 Length, u32 ChecksumOffset);
u32 GetPartitionChecksum(u32 ChecksumOffset, u8 *Checksum);
u32 CalcPartitionChecksum(u32 SourceAddr, u32 DataLength, u8 *Checksum);

/************************** Variable Definitions *****************************/
/*
 * Partition information flags
 */
u8 EncryptedPartitionFlag;
u8 PLPartitionFlag;
u8 PSPartitionFlag;
u8 SignedPartitionFlag;
u8 PartitionChecksumFlag;
u8 BitstreamFlag;
u8 ApplicationFlag;

u32 ExecutionAddress;
ImageMoverType MoveImage;

/*
 * Header array
 */
PartHeader PartitionHeader[MAX_PARTITION_NUMBER];
u32 PartitionCount;
u32 FsblLength;

#ifdef XPAR_XWDTPS_0_BASEADDR
extern XWdtPs Watchdog;	/* Instance of WatchDog Timer	*/
#endif

extern u32 Silicon_Version;
extern u32 FlashReadBaseAddress;
extern u8 LinearBootDeviceFlag;
extern XDcfg *DcfgInstPtr;

/*****************************************************************************/
/**
*
* This function
*
* @param
*
* @return
*
*
* @note		None
*
****************************************************************************/
u32 LoadBootImage(void)
{
	u32 RebootStatusRegister = 0;
	u32 MultiBootReg = 0;
	u32 ImageStartAddress = 0;
	u32 PartitionNum;
	u32 PartitionDataLength;
	u32 PartitionImageLength;
	u32 PartitionTotalSize;
	u32 PartitionExecAddr;
	u32 PartitionAttr;
	u32 ExecAddress = 0;
	u32 PartitionLoadAddr;
	u32 PartitionStartAddr;
	u32 PartitionChecksumOffset;
	u8 ExecAddrFlag = 0 ;
	u32 Status;
	PartHeader *HeaderPtr;
	u32 EfuseStatusRegValue;
#ifdef RSA_SUPPORT
	u32 HeaderSize;
#endif
	/*
	 * Resetting the Flags
	 */
	BitstreamFlag = 0;
	ApplicationFlag = 0;

	RebootStatusRegister = Xil_In32(REBOOT_STATUS_REG);
	fsbl_printf(DEBUG_INFO,
			"Reboot status register: 0x%08lx\r\n",RebootStatusRegister);

	if (Silicon_Version == SILICON_VERSION_1) {
		/*
		 * Clear out fallback mask from previous run
		 * We start from the first partition again
		 */
		if ((RebootStatusRegister & FSBL_FAIL_MASK) ==
				FSBL_FAIL_MASK) {
			fsbl_printf(DEBUG_INFO,
					"Reboot status shows previous run falls back\r\n");
			RebootStatusRegister &= ~(FSBL_FAIL_MASK);
			Xil_Out32(REBOOT_STATUS_REG, RebootStatusRegister);
		}

		/*
		 * Read the image start address
		 */
		ImageStartAddress = *(u32 *)BASEADDR_HOLDER;
	} else {
		/*
		 * read the multiboot register
		 */
		MultiBootReg =  XDcfg_ReadReg(DcfgInstPtr->Config.BaseAddr,
				XDCFG_MULTIBOOT_ADDR_OFFSET);

		fsbl_printf(DEBUG_INFO,"Multiboot Register: 0x%08lx\r\n",MultiBootReg);

		/*
		 * Compute the image start address
		 */
		ImageStartAddress = (MultiBootReg & PCAP_MBOOT_REG_REBOOT_OFFSET_MASK)
									* GOLDEN_IMAGE_OFFSET;
	}

	fsbl_printf(DEBUG_INFO,"Image Start Address: 0x%08lx\r\n",ImageStartAddress);

	/*
	 * Get partitions header information
	 */
	Status = GetPartitionHeaderInfo(ImageStartAddress);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL, "Partition Header Load Failed\r\n");
		OutputStatus(GET_HEADER_INFO_FAIL);
		FsblFallback();
	}

	/*
	 * RSA is not implemented in 1.0 and 2.0
	 * silicon
	 */
	if ((Silicon_Version != SILICON_VERSION_1) &&
			(Silicon_Version != SILICON_VERSION_2)) {
		/*
		 * Read Efuse Status Register
		 */
		EfuseStatusRegValue = Xil_In32(EFUSE_STATUS_REG);
		if (EfuseStatusRegValue & EFUSE_STATUS_RSA_ENABLE_MASK) {
			fsbl_printf(DEBUG_GENERAL,"RSA enabled for Chip\r\n");
#ifdef RSA_SUPPORT
			/*
			 * Set the Ppk
			 */
			SetPpk();

			/*
			 * Read partition header with signature
			 */
			Status = GetImageHeaderAndSignature(ImageStartAddress,
					(u32 *)DDR_TEMP_START_ADDR);
			if (Status != XST_SUCCESS) {
				fsbl_printf(DEBUG_GENERAL,
						"Read Partition Header signature Failed\r\n");
				OutputStatus(GET_HEADER_INFO_FAIL);
				FsblFallback();
			}
			HeaderSize=TOTAL_HEADER_SIZE+RSA_SIGNATURE_SIZE;

			Status = AuthenticatePartition((u8 *)DDR_TEMP_START_ADDR, HeaderSize);
			if (Status != XST_SUCCESS) {
				fsbl_printf(DEBUG_GENERAL,
						"Partition Header signature Failed\r\n");
				OutputStatus(GET_HEADER_INFO_FAIL);
				FsblFallback();
			}
#else
			/*
			 * In case user not enabled RSA authentication feature
			 */
			fsbl_printf(DEBUG_GENERAL,"RSA_SUPPORT_NOT_ENABLED_FAIL\r\n");
			OutputStatus(RSA_SUPPORT_NOT_ENABLED_FAIL);
			FsblFallback();
#endif
		}
	}

#ifdef MMC_SUPPORT
	/*
	 * In case of MMC support
	 * boot image preset in MMC will not have FSBL partition
	 */
	PartitionNum = 0;
#else
	/*
	 * First partition header was ignored by FSBL
	 * As it contain FSBL partition information
	 */
	PartitionNum = 1;
#endif

	while (PartitionNum < PartitionCount) {

		fsbl_printf(DEBUG_INFO, "Partition Number: %lu\r\n", PartitionNum);

		HeaderPtr = &PartitionHeader[PartitionNum];

		/*
		 * Print partition header information
		 */
		HeaderDump(HeaderPtr);

		/*
		 * Validate partition header
		 */
		Status = ValidateHeader(HeaderPtr);
		if (Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL, "INVALID_HEADER_FAIL\r\n");
			OutputStatus(INVALID_HEADER_FAIL);
			FsblFallback();
		}

		/*
		 * Load partition header information in to local variables
		 */
		PartitionDataLength = HeaderPtr->DataWordLen;
		PartitionImageLength = HeaderPtr->ImageWordLen;
		PartitionExecAddr = HeaderPtr->ExecAddr;
		PartitionAttr = HeaderPtr->PartitionAttr;
		PartitionLoadAddr = HeaderPtr->LoadAddr;
		PartitionChecksumOffset = HeaderPtr->CheckSumOffset;
		PartitionStartAddr = HeaderPtr->PartitionStart;
		PartitionTotalSize = HeaderPtr->PartitionWordLen;

		/*
		 * Partition owner should be FSBL to validate the partition
		 */
		if ((PartitionAttr & ATTRIBUTE_PARTITION_OWNER_MASK) !=
				ATTRIBUTE_PARTITION_OWNER_FSBL) {
			/*
			 * if FSBL is not the owner of partition,
			 * skip this partition, continue with next partition
			 */
			 fsbl_printf(DEBUG_INFO, "Skipping partition %0lx\r\n",
			 							PartitionNum);
			/*
			 * Increment partition number
			 */
			PartitionNum++;
			continue;
		}

		if (PartitionAttr & ATTRIBUTE_PL_IMAGE_MASK) {
			fsbl_printf(DEBUG_INFO, "Bitstream\r\n");
			PLPartitionFlag = 1;
			PSPartitionFlag = 0;
			BitstreamFlag = 1;
			if (ApplicationFlag == 1) {
#ifdef STDOUT_BASEADDRESS
				xil_printf("\r\nFSBL Warning !!!"
						"Bitstream not loaded into PL\r\n");
                xil_printf("Partition order invalid\r\n");
#endif
				break;
			}
		}

		if (PartitionAttr & ATTRIBUTE_PS_IMAGE_MASK) {
			fsbl_printf(DEBUG_INFO, "Application\r\n");
			PSPartitionFlag = 1;
			PLPartitionFlag = 0;
			ApplicationFlag = 1;
		}

		/*
		 * Encrypted partition will have different value
		 * for Image length and data length
		 */
		if (PartitionDataLength != PartitionImageLength) {
			fsbl_printf(DEBUG_INFO, "Encrypted\r\n");
			EncryptedPartitionFlag = 1;
		} else {
			EncryptedPartitionFlag = 0;
		}

		/*
		 * Check for partition checksum check
		 */
		if (PartitionAttr & ATTRIBUTE_CHECKSUM_TYPE_MASK) {
			PartitionChecksumFlag = 1;
		} else {
			PartitionChecksumFlag = 0;
		}

		/*
		 * RSA signature check
		 */
		if (PartitionAttr & ATTRIBUTE_RSA_PRESENT_MASK) {
			fsbl_printf(DEBUG_INFO, "RSA Signed\r\n");
			SignedPartitionFlag = 1;
		} else {
			SignedPartitionFlag = 0;
		}

		/*
		 * Load address check
		 * Loop will break when PS load address zero and partition is
		 * un-signed or un-encrypted
		 */
		if ((PSPartitionFlag == 1) && (PartitionLoadAddr < DDR_START_ADDR)) {
			if ((PartitionLoadAddr == 0) &&
					(!((SignedPartitionFlag == 1) ||
							(EncryptedPartitionFlag == 1)))) {
				break;
			} else {
				fsbl_printf(DEBUG_GENERAL,
						"INVALID_LOAD_ADDRESS_FAIL\r\n");
				OutputStatus(INVALID_LOAD_ADDRESS_FAIL);
				FsblFallback();
			}
		}

		if (PSPartitionFlag && (PartitionLoadAddr > DDR_END_ADDR)) {
			fsbl_printf(DEBUG_GENERAL,
					"INVALID_LOAD_ADDRESS_FAIL\r\n");
			OutputStatus(INVALID_LOAD_ADDRESS_FAIL);
			FsblFallback();
		}

        /*
         * Load execution address of first PS partition
         */
        if (PSPartitionFlag && (!ExecAddrFlag)) {
        	ExecAddrFlag++;
        	ExecAddress = PartitionExecAddr;
        }

		/*
		 * FSBL user hook call before bitstream download
		 */
		if (PLPartitionFlag) {
			Status = FsblHookBeforeBitstreamDload();
			if (Status != XST_SUCCESS) {
				fsbl_printf(DEBUG_GENERAL,"FSBL_BEFORE_BSTREAM_HOOK_FAIL\r\n");
				OutputStatus(FSBL_BEFORE_BSTREAM_HOOK_FAIL);
				FsblFallback();
			}
		}

		/*
		 * Move partitions from boot device
		 */
		Status = PartitionMove(ImageStartAddress, HeaderPtr);
		if (Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL,"PARTITION_MOVE_FAIL\r\n");
			OutputStatus(PARTITION_MOVE_FAIL);
			FsblFallback();
		}

		if ((SignedPartitionFlag) || (PartitionChecksumFlag)) {
			if(PLPartitionFlag) {
				/*
				 * PL partition loaded in to DDR temporary address
				 * for authentication and checksum verification
				 */
				PartitionStartAddr = DDR_TEMP_START_ADDR;
			} else {
				PartitionStartAddr = PartitionLoadAddr;
			}

			if (PartitionChecksumFlag) {
				/*
				 * Validate the partition data with checksum
				 */
				Status = ValidateParition(PartitionStartAddr,
						(PartitionTotalSize << WORD_LENGTH_SHIFT),
						ImageStartAddress  +
						(PartitionChecksumOffset << WORD_LENGTH_SHIFT));
				if (Status != XST_SUCCESS) {
					fsbl_printf(DEBUG_GENERAL,"PARTITION_CHECKSUM_FAIL\r\n");
					OutputStatus(PARTITION_CHECKSUM_FAIL);
					FsblFallback();
				}

				fsbl_printf(DEBUG_INFO, "Partition Validation Done\r\n");
			}

			/*
			 * Authentication Partition
			 */
			if (SignedPartitionFlag == 1 ) {
#ifdef RSA_SUPPORT
				Xil_DCacheEnable();
				Status = AuthenticatePartition((u8*)PartitionStartAddr,
						(PartitionTotalSize << WORD_LENGTH_SHIFT));
				if (Status != XST_SUCCESS) {
					Xil_DCacheFlush();
		        	Xil_DCacheDisable();
					fsbl_printf(DEBUG_GENERAL,"AUTHENTICATION_FAIL\r\n");
					OutputStatus(AUTHENTICATION_FAIL);
					FsblFallback();
				}
				fsbl_printf(DEBUG_INFO,"Authentication Done\r\n");
				Xil_DCacheFlush();
                Xil_DCacheDisable();
#else
				/*
				 * In case user not enabled RSA authentication feature
				 */
				fsbl_printf(DEBUG_GENERAL,"RSA_SUPPORT_NOT_ENABLED_FAIL\r\n");
				OutputStatus(RSA_SUPPORT_NOT_ENABLED_FAIL);
				FsblFallback();
#endif
			}

			/*
			 * Decrypt PS partition
			 */
			if (EncryptedPartitionFlag && PSPartitionFlag) {
				Status = DecryptPartition(PartitionStartAddr,
						PartitionDataLength,
						PartitionImageLength);
				if (Status != XST_SUCCESS) {
					fsbl_printf(DEBUG_GENERAL,"DECRYPTION_FAIL\r\n");
					OutputStatus(DECRYPTION_FAIL);
					FsblFallback();
				}
			}

			/*
			 * Load Signed PL partition in Fabric
			 */
			if (PLPartitionFlag) {
				Status = PcapLoadPartition((u32*)PartitionStartAddr,
						(u32*)PartitionLoadAddr,
						PartitionImageLength,
						PartitionDataLength,
						EncryptedPartitionFlag);
				if (Status != XST_SUCCESS) {
					fsbl_printf(DEBUG_GENERAL,"BITSTREAM_DOWNLOAD_FAIL\r\n");
					OutputStatus(BITSTREAM_DOWNLOAD_FAIL);
					FsblFallback();
				}
			}
		}


		/*
		 * FSBL user hook call after bitstream download
		 */
		if (PLPartitionFlag) {
			Status = FsblHookAfterBitstreamDload();
			if (Status != XST_SUCCESS) {
				fsbl_printf(DEBUG_GENERAL,"FSBL_AFTER_BSTREAM_HOOK_FAIL\r\n");
				OutputStatus(FSBL_AFTER_BSTREAM_HOOK_FAIL);
				FsblFallback();
			}
		}
		/*
		 * Increment partition number
		 */
		PartitionNum++;
	}

	return ExecAddress;
}

/*****************************************************************************/
/**
*
* This function loads all partition header information in global array
*
* @param	ImageAddress is the start address of the image
*
* @return	- XST_SUCCESS if Get partition Header information successful
*			- XST_FAILURE if Get Partition Header information failed
*
* @note		None
*
****************************************************************************/
u32 GetPartitionHeaderInfo(u32 ImageBaseAddress)
{
    u32 PartitionHeaderOffset;
    u32 Status;


    /*
     * Get the length of the FSBL from BootHeader
     */
    Status = GetFsblLength(ImageBaseAddress, &FsblLength);
    if (Status != XST_SUCCESS) {
    	fsbl_printf(DEBUG_GENERAL, "Get Header Start Address Failed\r\n");
    	return XST_FAILURE;
    }

    /*
    * Get the start address of the partition header table
    */
    Status = GetPartitionHeaderStartAddr(ImageBaseAddress,
    				&PartitionHeaderOffset);
    if (Status != XST_SUCCESS) {
    	fsbl_printf(DEBUG_GENERAL, "Get Header Start Address Failed\r\n");
    	return XST_FAILURE;
    }

    /*
     * Header offset on flash
     */
    PartitionHeaderOffset += ImageBaseAddress;

    fsbl_printf(DEBUG_INFO,"Partition Header Offset:0x%08lx\r\n",
    		PartitionHeaderOffset);

    /*
     * Load all partitions header data in to global variable
     */
    Status = LoadPartitionsHeaderInfo(PartitionHeaderOffset,
    				&PartitionHeader[0]);
    if (Status != XST_SUCCESS) {
    	fsbl_printf(DEBUG_GENERAL, "Header Information Load Failed\r\n");
    	return XST_FAILURE;
    }

    /*
     * Get partitions count from partitions header information
     */
	PartitionCount = GetPartitionCount(&PartitionHeader[0]);

    fsbl_printf(DEBUG_INFO, "Partition Count: %lu\r\n", PartitionCount);

    /*
     * Partition Count check
     */
    if (PartitionCount >= MAX_PARTITION_NUMBER) {
        fsbl_printf(DEBUG_GENERAL, "Invalid Partition Count\r\n");
		return XST_FAILURE;
#ifndef MMC_SUPPORT
    } else if (PartitionCount <= 1) {
        fsbl_printf(DEBUG_GENERAL, "There is no partition to load\r\n");
		return XST_FAILURE;
#endif
	}

    return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function goes to the partition header of the specified partition
*
* @param	ImageAddress is the start address of the image
*
* @return	Offset Partition header address of the image
*
* @return	- XST_SUCCESS if Get Partition Header start address successful
* 			- XST_FAILURE if Get Partition Header start address failed
*
* @note		None
*
****************************************************************************/
u32 GetPartitionHeaderStartAddr(u32 ImageAddress, u32 *Offset)
{
	u32 Status;

	Status = MoveImage(ImageAddress + IMAGE_PHDR_OFFSET, (u32)Offset, 4);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"Move Image failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function goes to the partition header of the specified partition
*
* @param	ImageAddress is the start address of the image
*
* @return	Offset to Image header table address of the image
*
* @return	- XST_SUCCESS if Get Partition Header start address successful
* 			- XST_FAILURE if Get Partition Header start address failed
*
* @note		None
*
****************************************************************************/
u32 GetImageHeaderStartAddr(u32 ImageAddress, u32 *Offset)
{
	u32 Status;

	Status = MoveImage(ImageAddress + IMAGE_HDR_OFFSET, (u32)Offset, 4);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"Move Image failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* This function gets the length of the FSBL
*
* @param	ImageAddress is the start address of the image
*
* @return	FsblLength is the length of the fsbl
*
* @return	- XST_SUCCESS if fsbl length reading is successful
* 			- XST_FAILURE if fsbl length reading failed
*
* @note		None
*
****************************************************************************/
u32 GetFsblLength(u32 ImageAddress, u32 *FsblLength)
{
	u32 Status;

	Status = MoveImage(ImageAddress + IMAGE_TOT_BYTE_LEN_OFFSET,
							(u32)FsblLength, 4);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"Move Image failed reading FsblLength\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#ifdef RSA_SUPPORT
/*****************************************************************************/
/**
*
* This function goes to read the image headers and its signature. Image
* header consists of image header table, image headers, partition
* headers
*
* @param	ImageBaseAddress is the start address of the image header
*
* @return	Offset Partition header address of the image
*
* @return	- XST_SUCCESS if Get Partition Header start address successful
* 			- XST_FAILURE if Get Partition Header start address failed
*
* @note		None
*
****************************************************************************/
u32 GetImageHeaderAndSignature(u32 ImageBaseAddress, u32 *Offset)
{
	u32 Status;
	u32 ImageHeaderOffset;

	/*
	 * Get the start address of the partition header table
	 */
	Status = GetImageHeaderStartAddr(ImageBaseAddress, &ImageHeaderOffset);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL, "Get Header Start Address Failed\r\n");
		return XST_FAILURE;
	}

	Status = MoveImage(ImageBaseAddress+ImageHeaderOffset, (u32)Offset,
							TOTAL_HEADER_SIZE + RSA_SIGNATURE_SIZE);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"Move Image failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
#endif
/*****************************************************************************/
/**
*
* This function get the header information of the all the partitions and load into
* global array
*
* @param	PartHeaderOffset Offset address where the header information present
*
* @param	Header Partition header pointer
*
* @return	- XST_SUCCESS if Load Partitions Header information successful
*			- XST_FAILURE if Load Partitions Header information failed
*
* @note		None
*
****************************************************************************/
u32 LoadPartitionsHeaderInfo(u32 PartHeaderOffset,  PartHeader *Header)
{
	u32 Status;

	Status = MoveImage(PartHeaderOffset, (u32)Header, sizeof(PartHeader)*MAX_PARTITION_NUMBER);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"Move Image failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function dumps the partition header.
*
* @param	Header Partition header pointer
*
* @return	None
*
* @note		None
*
******************************************************************************/
void HeaderDump(PartHeader *Header)
{
	fsbl_printf(DEBUG_INFO, "Header Dump\r\n");
	fsbl_printf(DEBUG_INFO, "Image Word Len: 0x%08lx\r\n",
									Header->ImageWordLen);
	fsbl_printf(DEBUG_INFO, "Data Word Len: 0x%08lx\r\n",
									Header->DataWordLen);
	fsbl_printf(DEBUG_INFO, "Partition Word Len:0x%08lx\r\n",
									Header->PartitionWordLen);
	fsbl_printf(DEBUG_INFO, "Load Addr: 0x%08lx\r\n",
									Header->LoadAddr);
	fsbl_printf(DEBUG_INFO, "Exec Addr: 0x%08lx\r\n",
									Header->ExecAddr);
	fsbl_printf(DEBUG_INFO, "Partition Start: 0x%08lx\r\n",
									Header->PartitionStart);
	fsbl_printf(DEBUG_INFO, "Partition Attr: 0x%08lx\r\n",
									Header->PartitionAttr);
	fsbl_printf(DEBUG_INFO, "Partition Checksum Offset: 0x%08lx\r\n",
										Header->CheckSumOffset);
	fsbl_printf(DEBUG_INFO, "Section Count: 0x%08lx\r\n",
									Header->SectionCount);
	fsbl_printf(DEBUG_INFO, "Checksum: 0x%08lx\r\n",
									Header->CheckSum);
}


/******************************************************************************/
/**
*
* This function calculates the partitions count from header information
*
* @param	Header Partition header pointer
*
* @return	Count Partition count
*
* @note		None
*
*******************************************************************************/
u32 GetPartitionCount(PartHeader *Header)
{
    u32 Count=0;
    struct HeaderArray *Hap;

    for(Count = 0; Count < MAX_PARTITION_NUMBER; Count++) {
        Hap = (struct HeaderArray *)&Header[Count];
        if(IsLastPartition(Hap)!=XST_FAILURE)
            break;
    }

	return Count;
}

/******************************************************************************/
/**
* This function check whether the current partition is the end of partitions
*
* The partition is the end of the partitions if it looks like this:
*	0x00000000
*	0x00000000
*	....
*	0x00000000
*	0x00000000
*	0xFFFFFFFF
*
* @param	H is a pointer to struct HeaderArray
*
* @return
*		- XST_SUCCESS if it is the last partition
*		- XST_FAILURE if it is not last partition
*
****************************************************************************/
u32 IsLastPartition(struct HeaderArray *H)
{
	int Index;

	if (H->Fields[PARTITION_HDR_CHECKSUM_WORD_COUNT] != 0xFFFFFFFF) {
		return	XST_FAILURE;
	}

	for (Index = 0; Index < PARTITION_HDR_WORD_COUNT - 1; Index++) {

        if (H->Fields[Index] != 0x0) {
			return XST_FAILURE;
		}
	}

    return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function validates the partition header.
*
* @param	Header Partition header pointer
*
* @return
*		- XST_FAILURE if bad header.
* 		- XST_SUCCESS if successful.
*
* @note		None
*
*******************************************************************************/
u32 ValidateHeader(PartHeader *Header)
{
	struct HeaderArray *Hap;

    Hap = (struct HeaderArray *)Header;

	/*
	 * If there are no partitions to load, fail
	 */
	if (IsEmptyHeader(Hap) == XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL, "IMAGE_HAS_NO_PARTITIONS\r\n");
	    return XST_FAILURE;
	}

	/*
	 * Validate partition header checksum
	 */
	if (ValidatePartitionHeaderChecksum(Hap) != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL, "PARTITION_HEADER_CORRUPTION\r\n");
		return XST_FAILURE;
	}

    /*
     * Validate partition data size
     */
	if (Header->ImageWordLen > MAXIMUM_IMAGE_WORD_LEN) {
		fsbl_printf(DEBUG_GENERAL, "INVALID_PARTITION_LENGTH\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/******************************************************************************/
/**
* This function check whether the current partition header is empty.
* A partition header is considered empty if image word length is 0 and the
* last word is 0.
*
* @param	H is a pointer to struct HeaderArray
*
* @return
*		- XST_SUCCESS , If the partition header is empty
*		- XST_FAILURE , If the partition header is NOT empty
*
* @note		Caller is responsible to make sure the address is valid.
*
*
****************************************************************************/
u32 IsEmptyHeader(struct HeaderArray *H)
{
	int Index;

	for (Index = 0; Index < PARTITION_HDR_WORD_COUNT; Index++) {
		if (H->Fields[Index] != 0x0) {
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function checks the header checksum If the header checksum is not valid
* XST_FAILURE is returned.
*
* @param	H is a pointer to struct HeaderArray
*
* @return
*		- XST_SUCCESS is header checksum is ok
*		- XST_FAILURE if the header checksum is not correct
*
* @note		None.
*
****************************************************************************/
u32 ValidatePartitionHeaderChecksum(struct HeaderArray *H)
{
	u32 Checksum;
	u32 Count;

	Checksum = 0;

	for (Count = 0; Count < PARTITION_HDR_CHECKSUM_WORD_COUNT; Count++) {
		/*
		 * Read the word from the header
		 */
		Checksum += H->Fields[Count];
	}

	/*
	 * Invert checksum, last bit of error checking
	 */
	Checksum ^= 0xFFFFFFFF;

	/*
	 * Validate the checksum
	 */
	if (H->Fields[PARTITION_HDR_CHECKSUM_WORD_COUNT] != Checksum) {
	    fsbl_printf(DEBUG_GENERAL, "Error: Checksum 0x%8.8lx != 0x%8.8lx\r\n",
			Checksum, H->Fields[PARTITION_HDR_CHECKSUM_WORD_COUNT]);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function load the partition from boot device
*
* @param	ImageBaseAddress Base address on flash
* @param	Header Partition header pointer
*
* @return
*		- XST_SUCCESS if partition move successful
*		- XST_FAILURE if check failed move failed
*
* @note		None
*
*******************************************************************************/
u32 PartitionMove(u32 ImageBaseAddress, PartHeader *Header)
{
    u32 SourceAddr;
    u32 Status;
    u8 SecureTransferFlag = 0;
    u32 LoadAddr;
    u32 ImageWordLen;
    u32 DataWordLen;

	SourceAddr = ImageBaseAddress;
	SourceAddr += Header->PartitionStart<<WORD_LENGTH_SHIFT;
	LoadAddr = Header->LoadAddr;
	ImageWordLen = Header->ImageWordLen;
	DataWordLen = Header->DataWordLen;

	/*
	 * Add flash base address for linear boot devices
	 */
	if (LinearBootDeviceFlag) {
		SourceAddr += FlashReadBaseAddress;
	}

	/*
	 * Partition encrypted
	 */
	if(EncryptedPartitionFlag) {
		SecureTransferFlag = 1;
	}

	/*
	 * For Signed or checksum enabled partition, 
	 * Total partition image need to copied to DDR
	 */
	if (SignedPartitionFlag || PartitionChecksumFlag) {
		ImageWordLen = Header->PartitionWordLen;
		DataWordLen = Header->PartitionWordLen;
	}

	/*
	 * Encrypted and Signed PS partition need to be loaded on to DDR
	 * without decryption
	 */
	if (PSPartitionFlag &&
			(SignedPartitionFlag || PartitionChecksumFlag) &&
			EncryptedPartitionFlag) {
		SecureTransferFlag = 0;
	}

	/*
	 * CPU is used for data transfer in case of non-linear
	 * boot device
	 */
	if (!LinearBootDeviceFlag) {
		/*
		 * PL partition copied to DDR temporary location
		 */
		if (PLPartitionFlag) {
			LoadAddr = DDR_TEMP_START_ADDR;
		}

		Status = MoveImage(SourceAddr,
						LoadAddr,
						(ImageWordLen << WORD_LENGTH_SHIFT));
		if(Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL, "Move Image Failed\r\n");
			return XST_FAILURE;
		}

		/*
		 * As image present at load address
		 */
		SourceAddr = LoadAddr;
	}

	if ((LinearBootDeviceFlag && PLPartitionFlag &&
			(SignedPartitionFlag || PartitionChecksumFlag)) ||
				(LinearBootDeviceFlag && PSPartitionFlag) ||
				((!LinearBootDeviceFlag) && PSPartitionFlag && SecureTransferFlag)) {
		/*
		 * PL signed partition copied to DDR temporary location
		 * using non-secure PCAP for linear boot device
		 */
		if(PLPartitionFlag){
			SecureTransferFlag = 0;
			LoadAddr = DDR_TEMP_START_ADDR;
		}

		/*
		 * Data transfer using PCAP
		 */
		Status = PcapDataTransfer((u32*)SourceAddr,
						(u32*)LoadAddr,
						ImageWordLen,
						DataWordLen,
						SecureTransferFlag);
		if(Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL, "PCAP Data Transfer Failed\r\n");
			return XST_FAILURE;
		}

		/*
		 * As image present at load address
		 */
		SourceAddr = LoadAddr;
	}

	/*
	 * Load Bitstream partition in to fabric only
	 * if checksum and authentication bits are not set
	 */
	if (PLPartitionFlag && (!(SignedPartitionFlag || PartitionChecksumFlag))) {
		Status = PcapLoadPartition((u32*)SourceAddr,
					(u32*)Header->LoadAddr,
					Header->ImageWordLen,
					Header->DataWordLen,
					EncryptedPartitionFlag);
		if(Status != XST_SUCCESS) {
			fsbl_printf(DEBUG_GENERAL, "PCAP Bitstream Download Failed\r\n");
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function load the decrypts partition
*
* @param	StartAddr Source start address
* @param	DataLength Data length in words
* @param	ImageLength Image length in words
*
* @return
*		- XST_SUCCESS if decryption successful
*		- XST_FAILURE if decryption failed
*
* @note		None
*
*******************************************************************************/
u32 DecryptPartition(u32 StartAddr, u32 DataLength, u32 ImageLength)
{
	u32 Status;
	u8 SecureTransferFlag =1;

	/*
	 * Data transfer using PCAP
	 */
	Status = PcapDataTransfer((u32*)StartAddr,
					(u32*)StartAddr,
					ImageLength,
					DataLength,
					SecureTransferFlag);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_GENERAL,"PCAP Data Transfer failed \r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function Validate Partition Data by using checksum preset in image
*
* @param	Partition header pointer
* @param	Partition check sum offset
* @return
*		- XST_SUCCESS if partition data is ok
*		- XST_FAILURE if partition data is corrupted
*
* @note		None
*
*******************************************************************************/
u32 ValidateParition(u32 StartAddr, u32 Length, u32 ChecksumOffset)
{
    u8  Checksum[MD5_CHECKSUM_SIZE];
    u8  CalcChecksum[MD5_CHECKSUM_SIZE];
    u32 Status;
    u32 Index;

#ifdef	XPAR_XWDTPS_0_BASEADDR
	/*
	 * Prevent WDT reset
	 */
	XWdtPs_RestartWdt(&Watchdog);
#endif

    /*
     * Get checksum from flash
     */
    Status = GetPartitionChecksum(ChecksumOffset, &Checksum[0]);
    if(Status != XST_SUCCESS) {
            return XST_FAILURE;
    }

    fsbl_printf(DEBUG_INFO, "Actual checksum\r\n");

    for (Index = 0; Index < MD5_CHECKSUM_SIZE; Index++) {
    	fsbl_printf(DEBUG_INFO, "0x%0x ",Checksum[Index]);
    }

    fsbl_printf(DEBUG_INFO, "\r\n");

    /*
     * Calculate checksum for the partition
     */
    Status = CalcPartitionChecksum(StartAddr, Length, &CalcChecksum[0]);
	if(Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    fsbl_printf(DEBUG_INFO, "Calculated checksum\r\n");

    for (Index = 0; Index < MD5_CHECKSUM_SIZE; Index++) {
        	fsbl_printf(DEBUG_INFO, "0x%0x ",CalcChecksum[Index]);
    }

    fsbl_printf(DEBUG_INFO, "\r\n");

    /*
     * Compare actual checksum with the calculated checksum
     */
	for (Index = 0; Index < MD5_CHECKSUM_SIZE; Index++) {
        if(Checksum[Index] != CalcChecksum[Index]) {
            fsbl_printf(DEBUG_GENERAL, "Error: "
            		"Partition DataChecksum 0x%0x!= 0x%0x\r\n",
			Checksum[Index], CalcChecksum[Index]);
		    return XST_FAILURE;
        }
    }

    return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function gets partition checksum from flash
*
* @param	Check sum offset
* @param	Checksum pointer
* @return
*		- XST_SUCCESS if checksum read success
*		- XST_FAILURE if unable get checksum
*
* @note		None
*
*******************************************************************************/
u32 GetPartitionChecksum(u32 ChecksumOffset, u8 *Checksum)
{
    u32 Status;

    Status = MoveImage(ChecksumOffset, (u32)Checksum, MD5_CHECKSUM_SIZE);
    if(Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}


/******************************************************************************/
/**
*
* This function calculates the checksum preset in image
*
* @param 	Start address
* @param 	Length of the data
* @param 	Checksum pointer
*
* @return
*		- XST_SUCCESS if Checksum calculate successful
*		- XST_FAILURE if Checksum calculate failed
*
* @note		None
*
*******************************************************************************/
u32 CalcPartitionChecksum(u32 SourceAddr, u32 DataLength, u8 *Checksum)
{
	/*
	 * Calculate checksum using MD5 algorithm
	 */
	md5((u8*)SourceAddr, DataLength, Checksum, 0 );

    return XST_SUCCESS;
}

