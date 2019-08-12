/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef DISKIO_DEFINED
#define DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#define USE_WRITE	1	/* 1: Enable disk_write function */
#define USE_IOCTL	1	/* 1: Enable disk_ioctl fucntion */

#include "integer.h"
#include "xil_types.h"

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01U	/* Drive not initialized */
#define STA_NODISK		0x02U	/* No medium in the drive */
#define STA_PROTECT		0x04U	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (used by FatFs) */
#define CTRL_SYNC			0U	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1U	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2U	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3U	/* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR	4U	/* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_POWER			5U	/* Get/Set power status */
#define CTRL_LOCK			6U	/* Lock/Unlock media removal */
#define CTRL_EJECT			7U	/* Eject media */
#define CTRL_FORMAT			8U	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10U	/* Get card type */
#define MMC_GET_CSD			11U	/* Get CSD */
#define MMC_GET_CID			12U	/* Get CID */
#define MMC_GET_OCR			13U	/* Get OCR */
#define MMC_GET_SDSTAT		14U	/* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20U	/* Get F/W revision */
#define ATA_GET_MODEL		21U	/* Get model name */
#define ATA_GET_SN			22U	/* Get serial number */

#ifdef __cplusplus
}
#endif

#endif
