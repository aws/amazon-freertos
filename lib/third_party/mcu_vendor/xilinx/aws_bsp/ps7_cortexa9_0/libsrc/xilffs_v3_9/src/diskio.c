/******************************************************************************
*
* Copyright (C) 2013 - 2018 Xilinx, Inc.  All rights reserved.
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
* @file diskio.c
*		This file is the glue layer between file system and
*		driver.
*		Description related to SD driver:
*		Process to use file system with SD
*		Select xilffs in SDK when creating a BSP
*		In SDK, set "fs_interface" to 1 to select SD interface.
*		This glue layer can currently be used only with one
*		SD controller enabled.
*		In order to use eMMC, in SDK set "Enable MMC" to 1. If not,
*		SD support is enabled by default.
*
*		Description:
*		This glue layer initializes the host controller and SD card
*		in disk_initialize. If SD card supports it, 4-bit mode and
*		high speed mode will be enabled.
*		The default block size is 512 bytes.
*		disk_read and disk_write functions are used to read and
*		write files using ADMA2 in polled mode.
*		The file system can be used to read from and write to an
*		SD card that is already formatted as FATFS.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a hk   10/17/13 First release
* 2.0   hk   02/12/14 Corrected status check in disk initialize. CR# 772072.
* 2.1   hk   04/16/14 Move check for ExtCSD high speed bit set inside if
*                     condition for high speed support.
*                     Include xil_types.h irrespective of xsdps.h. CR# 797086.
* 2.2   hk   07/28/14 Make changes to enable use of data cache.
* 3.0	sk	 12/04/14 Added support for micro SD without
* 					  WP/CD. CR# 810655.
*					  Make changes for prototypes of disk_read and
*					  disk_write according to latest version.
*			 12/15/14 Modified the code according to MISRAC 2012 Compliant.
*					  Updated the FatFs to R0.10b
*					  Removed alignment for local buffers as CacheInvalidate
*					  will take care of it.
*		sg   03/03/15 Added card detection check logic
*		     04/28/15 Card detection only in case of card detection signal
* 3.1   sk   06/04/15 Added support for SD1.
* 3.2   sk   11/24/15 Considered the slot type before checking the CD/WP pins.
* 3.3   sk   04/01/15 Added one second delay for checking CD pin.
* 3.4   sk   06/09/16 Added support for mkfs.
* 3.8   mj   07/31/17 Added support for RAM based FATfs.
*       mn   12/04/17 Resolve errors in XilFFS for ARMCC compiler
* 3.9   mn   04/18/18 Resolve build warnings for xilffs library
*
* </pre>
*
* @note
*
******************************************************************************/
#include "diskio.h"
#include "ff.h"
#include "xil_types.h"

#ifdef FILE_SYSTEM_INTERFACE_SD
#include "xsdps.h"		/* SD device driver */
#endif
#include "sleep.h"
#include "xil_printf.h"

#define HIGH_SPEED_SUPPORT	0x01U
#define WIDTH_4_BIT_SUPPORT	0x4U
#define SD_CLK_25_MHZ		25000000U
#define SD_CLK_26_MHZ		26000000U
#define SD_CLK_52_MHZ		52000000U
#define EXT_CSD_DEVICE_TYPE_BYTE	196
#define EXT_CSD_4_BIT_WIDTH_BYTE	183
#define EXT_CSD_HIGH_SPEED_BYTE		185
#define EXT_CSD_DEVICE_TYPE_HIGH_SPEED	0x3
#define SD_CD_DELAY		10000U

#ifdef FILE_SYSTEM_INTERFACE_RAM
#include "xparameters.h"

static char *dataramfs = NULL;

#define BLOCKSIZE       1U
#define SECTORSIZE      512U
#define SECTORCNT       (RAMFS_SIZE / SECTORSIZE)
#endif

/*--------------------------------------------------------------------------

	Public Functions

---------------------------------------------------------------------------*/

/*
 * Global variables
 */
static DSTATUS Stat[2] = {STA_NOINIT, STA_NOINIT};	/* Disk status */

#ifdef FILE_SYSTEM_INTERFACE_SD
static XSdPs SdInstance[2];
static u32 BaseAddress;
static u32 CardDetect;
static u32 WriteProtect;
static u32 SlotType[2];
static u8 HostCntrlrVer[2];
#endif

/*-----------------------------------------------------------------------*/
/* Get Disk Status							*/
/*-----------------------------------------------------------------------*/

/*****************************************************************************/
/**
*
* Gets the status of the disk.
* In case of SD, it checks whether card is present or not.
*
* @param	pdrv - Drive number
*
* @return
*		0		Status ok
*		STA_NOINIT	Drive not initialized
*		STA_NODISK	No medium in the drive
*		STA_PROTECT	Write protected
*
* @note		In case Card detect signal is not connected,
*		this function will not be able to check if card is present.
*
******************************************************************************/
DSTATUS disk_status (
		BYTE pdrv	/* Drive number (0) */
)
{
	DSTATUS s = Stat[pdrv];
	u32 StatusReg;
	u32 DelayCount = 0;

#ifdef FILE_SYSTEM_INTERFACE_SD
		if (SdInstance[pdrv].Config.BaseAddress == (u32)0) {
#ifdef XPAR_XSDPS_1_DEVICE_ID
				if(pdrv == 1) {
						BaseAddress = XPAR_XSDPS_1_BASEADDR;
						CardDetect = XPAR_XSDPS_1_HAS_CD;
						WriteProtect = XPAR_XSDPS_1_HAS_WP;
				} else {
#endif
						BaseAddress = XPAR_XSDPS_0_BASEADDR;
						CardDetect = XPAR_XSDPS_0_HAS_CD;
						WriteProtect = XPAR_XSDPS_0_HAS_WP;
#ifdef XPAR_XSDPS_1_DEVICE_ID
				}
#endif
				HostCntrlrVer[pdrv] = (u8)(XSdPs_ReadReg16(BaseAddress,
						XSDPS_HOST_CTRL_VER_OFFSET) & XSDPS_HC_SPEC_VER_MASK);
				if (HostCntrlrVer[pdrv] == XSDPS_HC_SPEC_V3) {
					SlotType[pdrv] = XSdPs_ReadReg(BaseAddress,
							XSDPS_CAPS_OFFSET) & XSDPS_CAPS_SLOT_TYPE_MASK;
				} else {
					SlotType[pdrv] = 0;
				}
		}
		StatusReg = XSdPs_GetPresentStatusReg((u32)BaseAddress);
		if (SlotType[pdrv] != XSDPS_CAPS_EMB_SLOT) {
			if (CardDetect) {
				while ((StatusReg & XSDPS_PSR_CARD_INSRT_MASK) == 0U) {
					if (DelayCount == 500U) {
						s = STA_NODISK | STA_NOINIT;
						goto Label;
					} else {
						/* Wait for 10 msec */
						usleep(SD_CD_DELAY);
						DelayCount++;
						StatusReg = XSdPs_GetPresentStatusReg((u32)BaseAddress);
					}
				}
			}
			s &= ~STA_NODISK;
			if (WriteProtect) {
					if ((StatusReg & XSDPS_PSR_WPS_PL_MASK) == 0U){
						s |= STA_PROTECT;
						goto Label;
					}
			}
			s &= ~STA_PROTECT;
		} else {
			s &= ~STA_NODISK & ~STA_PROTECT;
		}


Label:
		Stat[pdrv] = s;
#endif

		return s;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive						 */
/*-----------------------------------------------------------------------*/
/*****************************************************************************/
/**
*
* Initializes the drive.
* In case of SD, it initializes the host controller and the card.
* This function also selects additional settings such as bus width,
* speed and block size.
*
* @param	pdrv - Drive number
*
* @return	s - which contains an OR of the following information
*		STA_NODISK	Disk is not present
*		STA_NOINIT	Drive not initialized
*		STA_PROTECT	Drive is write protected
*		0 or only STA_PROTECT both indicate successful initialization.
*
* @note
*
******************************************************************************/
DSTATUS disk_initialize (
		BYTE pdrv	/* Physical drive number (0) */
)
{
	DSTATUS s;
	s32 Status;
#ifdef FILE_SYSTEM_INTERFACE_SD
	XSdPs_Config *SdConfig;
#endif

	s = disk_status(pdrv);
	if ((s & STA_NODISK) != 0U) {
		return s;
	}

	/* If disk is already initialized */
	if ((s & STA_NOINIT) == 0U) {
		return s;
	}

#ifdef FILE_SYSTEM_INTERFACE_SD
	if (CardDetect) {
			/*
			 * Card detection check
			 * If the HC detects the No Card State, power will be cleared
			 */
			while(!((XSDPS_PSR_CARD_DPL_MASK |
					XSDPS_PSR_CARD_STABLE_MASK |
					XSDPS_PSR_CARD_INSRT_MASK) ==
					( XSdPs_GetPresentStatusReg((u32)BaseAddress) &
					(XSDPS_PSR_CARD_DPL_MASK |
					XSDPS_PSR_CARD_STABLE_MASK |
					XSDPS_PSR_CARD_INSRT_MASK))));
	}

	/*
	 * Initialize the host controller
	 */
	SdConfig = XSdPs_LookupConfig((u16)pdrv);
	if (NULL == SdConfig) {
		s |= STA_NOINIT;
		return s;
	}

	Status = XSdPs_CfgInitialize(&SdInstance[pdrv], SdConfig,
					SdConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		s |= STA_NOINIT;
		return s;
	}

	Status = XSdPs_CardInitialize(&SdInstance[pdrv]);
	if (Status != XST_SUCCESS) {
		s |= STA_NOINIT;
		return s;
	}


	/*
	 * Disk is initialized.
	 * Store the same in Stat.
	 */
	s &= (~STA_NOINIT);

	Stat[pdrv] = s;
#endif

#ifdef FILE_SYSTEM_INTERFACE_RAM
	/* Assign RAMFS address value from xparameters.h */
	dataramfs = (char *)RAMFS_START_ADDR;

	/* Clearing No init Status for RAM */
	s &= (~STA_NOINIT);
	Stat[pdrv] = s;
#endif

	return s;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)							 */
/*-----------------------------------------------------------------------*/
/*****************************************************************************/
/**
*
* Reads the drive
* In case of SD, it reads the SD card using ADMA2 in polled mode.
*
* @param	pdrv - Drive number
* @param	*buff - Pointer to the data buffer to store read data
* @param	sector - Start sector number
* @param	count - Sector count
*
* @return
*		RES_OK		Read successful
*		STA_NOINIT	Drive not initialized
*		RES_ERROR	Read not successful
*
* @note
*
******************************************************************************/
DRESULT disk_read (
		BYTE pdrv,	/* Physical drive number (0) */
		BYTE *buff,	/* Pointer to the data buffer to store read data */
		DWORD sector,	/* Start sector number (LBA) */
		UINT count	/* Sector count (1..128) */
)
{
	DSTATUS s;
#ifdef FILE_SYSTEM_INTERFACE_SD
	s32 Status;
	DWORD LocSector = sector;
#endif

	s = disk_status(pdrv);

	if ((s & STA_NOINIT) != 0U) {
		return RES_NOTRDY;
	}
	if (count == 0U) {
		return RES_PARERR;
	}

#ifdef FILE_SYSTEM_INTERFACE_SD
	/* Convert LBA to byte address if needed */
	if ((SdInstance[pdrv].HCS) == 0U) {
		LocSector *= (DWORD)XSDPS_BLK_SIZE_512_MASK;
	}

	Status  = XSdPs_ReadPolled(&SdInstance[pdrv], (u32)LocSector, count, buff);
	if (Status != XST_SUCCESS) {
		return RES_ERROR;
	}
#endif

#ifdef FILE_SYSTEM_INTERFACE_RAM
	memcpy(buff, dataramfs + (sector * SECTORSIZE), count * SECTORSIZE);
#endif

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions						*/
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,				/* Physical drive number (0) */
	BYTE cmd,				/* Control code */
	void *buff				/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;

#ifdef FILE_SYSTEM_INTERFACE_SD
	void *LocBuff = buff;
	if ((disk_status(pdrv) & STA_NOINIT) != 0U) {	/* Check if card is in the socket */
		return RES_NOTRDY;
	}

	res = RES_ERROR;
	switch (cmd) {
		case (BYTE)CTRL_SYNC :	/* Make sure that no pending write process */
			res = RES_OK;
			break;

		case (BYTE)GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
			(*((DWORD *)(void *)LocBuff)) = (DWORD)SdInstance[pdrv].SectorCount;
			res = RES_OK;
			break;

		case (BYTE)GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			(*((DWORD *)((void *)LocBuff))) = ((DWORD)128);
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
			break;
	}
#endif

#ifdef FILE_SYSTEM_INTERFACE_RAM
	switch (cmd) {
	case (BYTE)CTRL_SYNC:
		break;
	case (BYTE)GET_BLOCK_SIZE:
		*(WORD *)buff = BLOCKSIZE;
		break;
	case (BYTE)GET_SECTOR_SIZE:
		*(WORD *)buff = SECTORSIZE;
		break;
	case (BYTE)GET_SECTOR_COUNT:
		*(DWORD *)buff = SECTORCNT;
		break;
	default:
		res = RES_PARERR;
		break;
	}
#endif

	return res;
}

/******************************************************************************/
/**
*
* This function is User Provided Timer Function for FatFs module
*
* @param	None
*
* @return	DWORD
*
* @note		None
*
****************************************************************************/

DWORD get_fattime (void)
{
	return	((DWORD)(2010U - 1980U) << 25U)	/* Fixed to Jan. 1, 2010 */
		| ((DWORD)1 << 21)
		| ((DWORD)1 << 16)
		| ((DWORD)0 << 11)
		| ((DWORD)0 << 5)
		| ((DWORD)0 >> 1);
}

/*****************************************************************************/
/**
*
* Reads the drive
* In case of SD, it reads the SD card using ADMA2 in polled mode.
*
* @param	pdrv - Drive number
* @param	*buff - Pointer to the data to be written
* @param	sector - Sector address
* @param	count - Sector count
*
* @return
*		RES_OK		Read successful
*		STA_NOINIT	Drive not initialized
*		RES_ERROR	Read not successful
*
* @note
*
******************************************************************************/
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DSTATUS s;
#ifdef FILE_SYSTEM_INTERFACE_SD
	s32 Status;
	DWORD LocSector = sector;
#endif

	s = disk_status(pdrv);
	if ((s & STA_NOINIT) != 0U) {
		return RES_NOTRDY;
	}
	if (count == 0U) {
		return RES_PARERR;
	}

#ifdef FILE_SYSTEM_INTERFACE_SD
	/* Convert LBA to byte address if needed */
	if ((SdInstance[pdrv].HCS) == 0U) {
		LocSector *= (DWORD)XSDPS_BLK_SIZE_512_MASK;
	}

	Status  = XSdPs_WritePolled(&SdInstance[pdrv], (u32)LocSector, count, buff);
	if (Status != XST_SUCCESS) {
		return RES_ERROR;
	}

#endif

#ifdef FILE_SYSTEM_INTERFACE_RAM
	memcpy(dataramfs + (sector * SECTORSIZE), buff, count * SECTORSIZE);
#endif

	return RES_OK;
}
