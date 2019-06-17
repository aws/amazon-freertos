/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.10b                (C)ChaN, 2014
/-----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------/
/ Feb 26,'06 R0.00  Prototype.
/
/ Apr 29,'06 R0.01  First stable version.
/
/ Jun 01,'06 R0.02  Added FAT12 support.
/                   Removed unbuffered mode.
/                   Fixed a problem on small (<32M) partition.
/ Jun 10,'06 R0.02a Added a configuration option (_FS_MINIMUM).
/
/ Sep 22,'06 R0.03  Added f_rename().
/                   Changed option _FS_MINIMUM to _FS_MINIMIZE.
/ Dec 11,'06 R0.03a Improved cluster scan algorithm to write files fast.
/                   Fixed f_mkdir() creates incorrect directory on FAT32.
/
/ Feb 04,'07 R0.04  Supported multiple drive system.
/                   Changed some interfaces for multiple drive system.
/                   Changed f_mountdrv() to f_mount().
/                   Added f_mkfs().
/ Apr 01,'07 R0.04a Supported multiple partitions on a physical drive.
/                   Added a capability of extending file size to f_lseek().
/                   Added minimization level 3.
/                   Fixed an endian sensitive code in f_mkfs().
/ May 05,'07 R0.04b Added a configuration option _USE_NTFLAG.
/                   Added FSINFO support.
/                   Fixed DBCS name can result FR_INVALID_NAME.
/                   Fixed short seek (<= csize) collapses the file object.
/
/ Aug 25,'07 R0.05  Changed arguments of f_read(), f_write() and f_mkfs().
/                   Fixed f_mkfs() on FAT32 creates incorrect FSINFO.
/                   Fixed f_mkdir() on FAT32 creates incorrect directory.
/ Feb 03,'08 R0.05a Added f_truncate() and f_utime().
/                   Fixed off by one error at FAT sub-type determination.
/                   Fixed btr in f_read() can be mistruncated.
/                   Fixed cached sector is not flushed when create and close without write.
/
/ Apr 01,'08 R0.06  Added fputc(), fputs(), fprintf() and fgets().
/                   Improved performance of f_lseek() on moving to the same or following cluster.
/
/ Apr 01,'09 R0.07  Merged Tiny-FatFs as a configuration option. (_FS_TINY)
/                   Added long file name feature.
/                   Added multiple code page feature.
/                   Added re-entrancy for multitask operation.
/                   Added auto cluster size selection to f_mkfs().
/                   Added rewind option to f_readdir().
/                   Changed result code of critical errors.
/                   Renamed string functions to avoid name collision.
/ Apr 14,'09 R0.07a Separated out OS dependent code on reentrant cfg.
/                   Added multiple sector size feature.
/ Jun 21,'09 R0.07c Fixed f_unlink() can return FR_OK on error.
/                   Fixed wrong cache control in f_lseek().
/                   Added relative path feature.
/                   Added f_chdir() and f_chdrive().
/                   Added proper case conversion to extended character.
/ Nov 03,'09 R0.07e Separated out configuration options from ff.h to ffconf.h.
/                   Fixed f_unlink() fails to remove a sub-directory on _FS_RPATH.
/                   Fixed name matching error on the 13 character boundary.
/                   Added a configuration option, _LFN_UNICODE.
/                   Changed f_readdir() to return the SFN with always upper case on non-LFN cfg.
/
/ May 15,'10 R0.08  Added a memory configuration option. (_USE_LFN = 3)
/                   Added file lock feature. (_FS_SHARE)
/                   Added fast seek feature. (_USE_FASTSEEK)
/                   Changed some types on the API, XCHAR->TCHAR.
/                   Changed .fname in the FILINFO structure on Unicode cfg.
/                   String functions support UTF-8 encoding files on Unicode cfg.
/ Aug 16,'10 R0.08a Added f_getcwd().
/                   Added sector erase feature. (_USE_ERASE)
/                   Moved file lock semaphore table from fs object to the bss.
/                   Fixed a wrong directory entry is created on non-LFN cfg when the given name contains ';'.
/                   Fixed f_mkfs() creates wrong FAT32 volume.
/ Jan 15,'11 R0.08b Fast seek feature is also applied to f_read() and f_write().
/                   f_lseek() reports required table size on creating CLMP.
/                   Extended format syntax of f_printf().
/                   Ignores duplicated directory separators in given path name.
/
/ Sep 06,'11 R0.09  f_mkfs() supports multiple partition to complete the multiple partition feature.
/                   Added f_fdisk().
/ Aug 27,'12 R0.09a Changed f_open() and f_opendir() reject null object pointer to avoid crash.
/                   Changed option name _FS_SHARE to _FS_LOCK.
/                   Fixed assertion failure due to OS/2 EA on FAT12/16 volume.
/ Jan 24,'13 R0.09b Added f_setlabel() and f_getlabel().
/
/ Oct 02,'13 R0.10  Added selection of character encoding on the file. (_STRF_ENCODE)
/                   Added f_closedir().
/                   Added forced full FAT scan for f_getfree(). (_FS_NOFSINFO)
/                   Added forced mount feature with changes of f_mount().
/                   Improved behavior of volume auto detection.
/                   Improved write throughput of f_puts() and f_printf().
/                   Changed argument of f_chdrive(), f_mkfs(), disk_read() and disk_write().
/                   Fixed f_write() can be truncated when the file size is close to 4GB.
/                   Fixed f_open(), f_mkdir() and f_setlabel() can return incorrect error code.
/ Jan 15,'14 R0.10a Added arbitrary strings as drive number in the path name. (_STR_VOLUME_ID)
/                   Added a configuration option of minimum sector size. (_MIN_SS)
/                   2nd argument of f_rename() can have a drive number and it will be ignored.
/                   Fixed f_mount() with forced mount fails when drive number is >= 1.
/                   Fixed f_close() invalidates the file object without volume lock.
/                   Fixed f_closedir() returns but the volume lock is left acquired.
/                   Fixed creation of an entry with LFN fails on too many SFN collisions.
/ May 19,'14 R0.10b Fixed a hard error in the disk I/O layer can collapse the directory entry.
/                   Fixed LFN entry is not deleted on delete/rename an object with lossy converted SFN.
/---------------------------------------------------------------------------*/
/**
*
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 3.9   mn   04/18/18 Resolve build warnings for xilffs library
*
******************************************************************************/
#include "xparameters.h"
#if (defined FILE_SYSTEM_INTERFACE_SD) || (defined FILE_SYSTEM_INTERFACE_RAM)
#include "ff.h"			/* FatFs configurations and declarations */
#include "diskio.h"		/* Declarations of low level disk I/O functions */
#include "xil_printf.h"



/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if FAT_FS != 8051	/* Revision ID */
#error Wrong include file (ff.h).
#endif


/* Reentrancy related */
#if _FS_REENTRANT
#if _USE_LFN == 1
#error Static LFN work area cannot be used at thread-safe configuration.
#endif
#define	ENTER_FF(fs)		{ if (!lock_fs(fs)) return FR_TIMEOUT; }
#define	LEAVE_FF(fs, res)	{ unlock_fs((fs), (res)); return (res); }
#else
#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return ((res))
#endif

#define	ABORT(fs, res)		{ fp->err = (BYTE)(res); LEAVE_FF((fs), (res)); }


/* Definitions of sector size */
#if (_MAX_SS < _MIN_SS) || (_MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096) || (_MIN_SS != 512 && _MIN_SS != 1024 && _MIN_SS != 2048 && _MIN_SS != 4096)
#error Wrong sector size configuration.
#endif
#if _MAX_SS == _MIN_SS
#define	SS(fs)	_MAX_SS	/* Fixed sector size */
#else
#define	SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* File access control feature */
#if _FS_LOCK
#if _FS_READONLY
#error _FS_LOCK must be 0 at read-only cfg.
#endif
typedef struct {
	FATFS *fs;		/* Object ID 1, volume (NULL:blank entry) */
	DWORD clu;		/* Object ID 2, directory (0:root) */
	WORD idx;		/* Object ID 3, directory index */
	WORD ctr;		/* Object open counter, 0:none, 0x01..0xFF:read mode open count, 0x100:write mode */
} FILESEM;
#endif



/* DBCS code ranges and SBCS extend character conversion table */

#if _CODE_PAGE == 932	/* Japanese Shift-JIS */
#define DF1S	0x81U	/* DBC 1st byte range 1 start */
#define DF_1E	0x9FU	/* DBC 1st byte range 1 end */
#define DF_2S	0xE0U	/* DBC 1st byte range 2 start */
#define DF_2E	0xFCU	/* DBC 1st byte range 2 end */
#define DS_1S	0x40U	/* DBC 2nd byte range 1 start */
#define DS_1E	0x7EU	/* DBC 2nd byte range 1 end */
#define DS_2S	0x80U	/* DBC 2nd byte range 2 start */
#define DS_2E	0xFCU	/* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#define DF1S	0x81U
#define DF_1E	0xFEU
#define DS_1S	0x40U
#define DS_1E	0x7EU
#define DS_2S	0x80U
#define DS_2E	0xFEU

#elif _CODE_PAGE == 949	/* Korean */
#define DF1S	0x81U
#define DF_1E	0xFEU
#define DS_1S	0x41U
#define DS_1E	0x5AU
#define DS_2S	0x61U
#define DS_2E	0x7AU
#define _DS3S	0x81U
#define _DS3E	0xFEU

#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#define DF1S	0x81U
#define DF_1E	0xFEU
#define DS_1S	0x40U
#define DS_1E	0x7EU
#define DS_2S	0xA1U
#define DS_2E	0xFEU

#elif _CODE_PAGE == 437	/* U.S. (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 720	/* Arabic (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x45,0x41,0x84,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x49,0x49,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 737	/* Greek (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
				0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xE7,0xE8,0xF1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 775	/* Baltic (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 850	/* Multilingual Latin 1 (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 852	/* Latin 2 (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F,0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 855	/* Cyrillic (OEM) */
#define DF1S	0
#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F,0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
				0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
				0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 857	/* Turkish (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x98,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0x59,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 858	/* Multilingual Latin 1 + Euro (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 862	/* Hebrew (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 866	/* Russian (OEM) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x90,0x91,0x92,0x93,0x9d,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 874	/* Thai (OEM, Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1250 /* Central Europe (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xA3,0xB4,0xB5,0xB6,0xB7,0xB8,0xA5,0xAA,0xBB,0xBC,0xBD,0xBC,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1251 /* Cyrillic (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x80,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
				0xA0,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xA5,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xA3,0xBD,0xBD,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF}

#elif _CODE_PAGE == 1252 /* Latin 1 (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0xAd,0x9B,0x8C,0x9D,0xAE,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1253 /* Greek (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xA2,0xB8,0xB9,0xBA, \
				0xE0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xFB,0xBC,0xFD,0xBF,0xFF}

#elif _CODE_PAGE == 1254 /* Turkish (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1255 /* Hebrew (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1256 /* Arabic (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x8C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x41,0xE1,0x41,0xE3,0xE4,0xE5,0xE6,0x43,0x45,0x45,0x45,0x45,0xEC,0xED,0x49,0x49,0xF0,0xF1,0xF2,0xF3,0x4F,0xF5,0xF6,0xF7,0xF8,0x55,0xFA,0x55,0x55,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1257 /* Baltic (Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xBC,0xBD,0xBE,0xAF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1258 /* Vietnam (OEM, Windows) */
#define DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0xAC,0x9D,0x9E,0x9F, \
				0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xEC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0x9F}

#elif _CODE_PAGE == 1	/* ASCII (for only non-LFN cfg) */
#if _USE_LFN
#error Cannot use LFN feature without valid code page.
#endif
#define DF1S	0

#else
#error Unknown code page

#endif


/* Character code support macros */
#define IsUpper(c)	(((c)>=(BYTE)'A')&&((c)<=(BYTE)'Z'))
#define IsLower(c)	(((c)>=(BYTE)'a')&&((c)<=(BYTE)'z'))
#define IsDigit(c)	(((c)>=(BYTE)'0')&&((c)<=(BYTE)'9'))

#if DF1S		/* Code page is DBCS */

#ifdef DF_2S	/* Two 1st byte areas */
#define IsDBCS1(c)	((((BYTE)(c) >= DF1S) && ((BYTE)(c) <= DF_1E)) || (((BYTE)(c) >= DF_2S) && ((BYTE)(c) <= DF_2E)))
#else			/* One 1st byte area */
#define IsDBCS1(c)	(((BYTE)(c) >= DF1S) && ((BYTE)(c) <= DF_1E))
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	((((BYTE)(c) >= DS_1S) && ((BYTE)(c) <= DS_1E)) || (((BYTE)(c) >= DS_2S) && ((BYTE)(c) <= DS_2E)) || (((BYTE)(c) >= _DS3S) && ((BYTE)(c) <= _DS3E)))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	((((BYTE)(c) >= DS_1S) && ((BYTE)(c) <= DS_1E)) || (((BYTE)(c) >= DS_2S) && ((BYTE)(c) <= DS_2E)))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* DF1S */


/* Name status flags */
#define NS			11		/* Index of name status byte in fn[] */
#define NS_LOSS		0x01U	/* Out of 8.3 format */
#define NS_LFN		0x02U	/* Force to create LFN entry */
#define NS_LAST		0x04U	/* Last segment */
#define NS_BODY		0x08U	/* Lower case flag (body) */
#define NS_EXT		0x10U	/* Lower case flag (ext) */
#define NS_DOT		0x20U	/* Dot entry */


/* FAT sub-type boundaries */
#define MIN_FAT16	4086U	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526U	/* Minimum number of clusters for FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */

#define BS_jmpBoot			0U		/* Jump instruction (3) */
#define BS_OEMName			3U		/* OEM name (8) */
#define BPB_BytsPerSec		11U		/* Sector size [byte] (2) */
#define BPB_SecPerClus		13U		/* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt		14U		/* Size of reserved area [sector] (2) */
#define BPB_NumFATs			16U		/* Number of FAT copies (1) */
#define BPB_RootEntCnt		17U		/* Number of root directory entries for FAT12/16 (2) */
#define BPB_TotSec16		19U		/* Volume size [sector] (2) */
#define BPB_Media			21U		/* Media descriptor (1) */
#define BPB_FATSz16			22U		/* FAT size [sector] (2) */
#define BPB_SecPerTrk		24U		/* Track size [sector] (2) */
#define BPB_NumHeads		26U		/* Number of heads (2) */
#define BPB_HiddSec			28U		/* Number of special hidden sectors (4) */
#define BPB_TotSec32		32U		/* Volume size [sector] (4) */
#define BS_DrvNum			36U		/* Physical drive number (2) */
#define BS_BootSig			38U		/* Extended boot signature (1) */
#define BS_VolID			39U		/* Volume serial number (4) */
#define BS_VolLab			43U		/* Volume label (8) */
#define BS_FilSysType		54U		/* File system type (1) */
#define BPB_FATSz32			36U		/* FAT size [sector] (4) */
#define BPB_ExtFlags		40U		/* Extended flags (2) */
#define BPB_FSVer			42U		/* File system version (2) */
#define BPB_RootClus		44U		/* Root directory first cluster (4) */
#define BPB_FSInfo			48U		/* Offset of FSINFO sector (2) */
#define BPB_BkBootSec		50U		/* Offset of backup boot sector (2) */
#define BS_DrvNum32			64U		/* Physical drive number (2) */
#define BS_BootSig32		66U		/* Extended boot signature (1) */
#define BS_VolID32			67U		/* Volume serial number (4) */
#define BS_VolLab32			71U		/* Volume label (8) */
#define BS_FilSysType32		82U		/* File system type (1) */
#define	FSI_LeadSig			0U		/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484U		/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488U	/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492U	/* FSI: Last allocated cluster (4) */
#define MBR_Table			446U	/* MBR: Partition table offset (2) */
#define	SZ_PTE				16U		/* MBR: Size of a partition table entry */
#define BS_55AA				510U		/* Signature word (2) */

#define	DIR_Name			0U		/* Short file name (11) */
#define	DIR_Attr			11U		/* Attribute (1) */
#define	DIR_NTres			12U		/* NT flag (1) */
#define DIR_CrtTimeTenth	13U		/* Created time sub-second (1) */
#define	DIR_CrtTime			14U		/* Created time (2) */
#define	DIR_CrtDate			16U		/* Created date (2) */
#define DIR_LstAccDate		18U		/* Last accessed date (2) */
#define	DIR_FstClusHI		20U		/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22U		/* Modified time (2) */
#define	DIR_WrtDate			24U		/* Modified date (2) */
#define	DIR_FstClusLO		26U		/* Lower 16-bit of first cluster (2) */
#define	DIR_FileSize		28U		/* File size (4) */
#define	LDIR_Ord			0U		/* LFN entry order and LLE flag (1) */
#define	LDIR_Attr			11U		/* LFN attribute (1) */
#define	LDIR_Type			12U		/* LFN type (1) */
#define	LDIR_Chksum			13U		/* Sum of corresponding SFN entry */
#define	LDIR_FstClusLO		26U		/* Filled by zero (0) */
#define	SZ_DIR				32U		/* Size of a directory entry */
#define	LLE					0x40U	/* Last long entry flag in LDIR_Ord */
#define	DDE					0xE5U	/* Deleted directory entry mark in DIR_Name[0] */
#define	NDDE				0x05U	/* Replacement of the character collides with DDE */




/*------------------------------------------------------------*/
/* Module private work area                                   */
/*------------------------------------------------------------*/
/* Note that uninitialized variables with static duration are
/  guaranteed zero/null as initial value. If not, either the
/  linker or start-up routine is out of ANSI-C standard.
*/

#if _VOLUMES >= 1 || _VOLUMES <= 10
static
FATFS *FatFs[_VOLUMES];		/* Pointer to the file system objects (logical drives) */
#else
#error Number of volumes must be 1 to 10.
#endif

#if _FS_RPATH && _VOLUMES >= 2
static
BYTE CurrVol;				/* Current drive */
#endif

#if _FS_LOCK
static
FILESEM	Files[_FS_LOCK];	/* Open object lock semaphores */
#endif

#if _USE_LFN == 0			/* No LFN feature */
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		(dobj).fn = sfn
#define	FREE_BUF()

#elif _USE_LFN == 1			/* LFN feature with static working buffer */
static
WCHAR LfnBuf[_MAX_LFN+1];
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = LfnBuf; }
#define	FREE_BUF()

#elif _USE_LFN == 2 		/* LFN feature with dynamic working buffer on the stack */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR lbuf[_MAX_LFN+1]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = lbuf; }
#define	FREE_BUF()

#elif _USE_LFN == 3 		/* LFN feature with dynamic working buffer on the heap */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR *lfn
#define INIT_BUF(dobj)		{ lfn = ff_memalloc((_MAX_LFN + 1) * 2); \
							  if (!lfn) LEAVE_FF((dobj).fs, FR_NOT_ENOUGH_CORE); \
							  (dobj).lfn = lfn;	(dobj).fn = sfn; }
#define	FREE_BUF()			ff_memfree(lfn)

#else
#error Wrong LFN configuration.
#endif


#ifdef _EXCVT
static
const BYTE ExCvt[] = _EXCVT;	/* Upper conversion table for extended characters */
#endif

#if _MULTI_PARTITION
PARTITION VolToPart[] = {
    {0, 1},     /* Logical drive 0 ==> Physical drive 0, 1st partition */
    {0, 2},      /* Logical drive 1 ==> Physical drive 0, 2nd partition */
    {0, 3},     /* Logical drive 2 ==> Physical drive 0, 3rd partition */
    {0, 4},     /* Logical drive 3 ==> Physical drive 0, 4th partition */
	{1, 1},     /* Logical drive 4 ==> Physical drive 1, 1st partition */
    {1, 2},     /* Logical drive 5 ==> Physical drive 1, 2nd partition */
	{1, 3},     /* Logical drive 6 ==> Physical drive 1, 3rd partition */
    {1, 4}     /* Logical drive 7 ==> Physical drive 1, 4th partition */
};
#endif




/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Copy memory to memory */
static
void mem_cpy (void* dst, const void* src, UINT cnt) {
	BYTE *d = (BYTE*)(void *)dst;
	const BYTE *s = src;

#if _WORD_ACCESS == 1
	while (cnt >= sizeof (int)) {
		*(int*)d = *(int*)s;
		d += sizeof (int); s += sizeof (int);
		cnt -= sizeof (int);
	}
#endif
	while ((cnt) > 0U){
		*d=*s;
		d+=1U;
		s+=1U;
		cnt-=1U;
	}
}

/* Fill memory */
static
void mem_set (void* dst, s32 val, UINT cnt) {
	BYTE *d = (BYTE*)(void *)dst;

	while ((cnt) > 0U){
		*d = (BYTE)val;
		d+=1U;
		cnt-=1U;
	}
}

/* Compare memory to memory */
static
s32 mem_cmp (const void* dst, const void* src, UINT cnt) {
	const BYTE *d = dst, *s = src;
	s32 r = (s32)0;
	UINT LocCnt = cnt;
	while (((LocCnt)  != 0U) && ((r = (s32)(*d - *s)) == 0)) {
		d+=1U;
		s+=1U;
		LocCnt-=1U;
	}
	return r;
}

/* Check if chr is contained in the string */
static
s32 chk_chr (const char* str, s32 chr) {
	char Status;
	const char *LocStr = str;
	while (((*LocStr) != (char)0) && ((*LocStr != chr) != 0U)){
		LocStr+=1;
	}
	Status = *LocStr;
	return (s32)Status;
}



/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
#if _FS_REENTRANT
static
s32 lock_fs (
	FATFS* fs		/* File system object */
)
{
	return ff_req_grant(fs->sobj);
}


static
void unlock_fs (
	FATFS* fs,		/* File system object */
	FRESULT res		/* Result code to be returned */
)
{
	if (fs &&
		res != FR_NOT_ENABLED &&
		res != FR_INVALID_DRIVE &&
		res != FR_INVALID_OBJECT &&
		res != FR_TIMEOUT) {
		ff_rel_grant(fs->sobj);
	}
}
#endif




/*-----------------------------------------------------------------------*/
/* File lock control functions                                           */
/*-----------------------------------------------------------------------*/
#if _FS_LOCK

static
FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dp,		/* Directory object pointing the file to be checked */
	s32 acc			/* Desired access type (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i, be;

	/* Search file semaphore table */
	for (i = be = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs) {	/* Existing entry */
			if (Files[i].fs == dp->fs &&	 	/* Check if the object matched with an open object */
				Files[i].clu == dp->sclust &&
				Files[i].idx == dp->index) break;
		} else {			/* Blank entry */
			be = 1;
		}
	}
	if (i == _FS_LOCK)	/* The object is not opened */
		return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;	/* Is there a blank entry for new object? */

	/* The object has been opened. Reject any open against writing file and all write mode open */
	return (acc || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}


static
s32 enq_lock (void)	/* Check if an entry is available for a new object */
{
	UINT i;

	for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
	return (i == _FS_LOCK) ? 0 : 1;
}


static
UINT inc_lock (	/* Increment object open counter and returns its index (0:Internal error) */
	DIR* dp,	/* Directory object pointing the file to register or increment */
	s32 acc		/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i;


	for (i = 0; i < _FS_LOCK; i++) {	/* Find the object */
		if (Files[i].fs == dp->fs &&
			Files[i].clu == dp->sclust &&
			Files[i].idx == dp->index) break;
	}

	if (i == _FS_LOCK) {				/* Not opened. Register it as new. */
		for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
		if (i == _FS_LOCK) return 0;	/* No free entry to register (int err) */
		Files[i].fs = dp->fs;
		Files[i].clu = dp->sclust;
		Files[i].idx = dp->index;
		Files[i].ctr = 0;
	}

	if (acc && Files[i].ctr) return 0;	/* Access violation (int err) */

	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */

	return i + 1;
}


static
FRESULT dec_lock (	/* Decrement object open counter */
	UINT i			/* Semaphore index (1..) */
)
{
	WORD n;
	FRESULT res;


	if (--i < _FS_LOCK) {	/* Shift index number origin from 0 */
		n = Files[i].ctr;
		if (n == 0x100) n = 0;		/* If write mode open, delete the entry */
		if (n) n--;					/* Decrement read mode open count */
		Files[i].ctr = n;
		if (!n) Files[i].fs = 0;	/* Delete the entry if open count gets zero */
		res = FR_OK;
	} else {
		res = FR_INT_ERR;			/* Invalid index nunber */
	}
	return res;
}


static
void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
{
	UINT i;

	for (i = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs == fs) Files[i].fs = 0;
	}
}
#endif




/*-----------------------------------------------------------------------*/
/* Move/Flush disk access window in the file system object               */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT sync_window (
	FATFS* fs		/* File system object */
)
{
	DWORD wsect;
	UINT nf;

	if (fs->wflag != 0U) {	/* Write back the sector if it is dirty */
		wsect = fs->winsect;	/* Current sector number */
		if (disk_write(fs->drv, fs->win, wsect, 1U) != RES_OK) {
			return FR_DISK_ERR;
		}
		fs->wflag = 0U;
		if ((wsect - fs->fatbase) < fs->fsize) {		/* Is it in the FAT area? */
			nf = (UINT)fs->n_fats;
			for (; nf >= 2U; nf--) {	/* Reflect the change to all FAT copies */
				wsect += fs->fsize;
				(void)disk_write(fs->drv, fs->win, wsect, 1U);
			}
		}
	}
	return FR_OK;
}
#endif


static
FRESULT move_window (
	FATFS* fs,		/* File system object */
	DWORD sector	/* Sector number to make appearance in the fs->win[] */
)
{
	if (sector != fs->winsect) {	/* Changed current window */
#if !_FS_READONLY
		if (sync_window(fs) != FR_OK) {
			return FR_DISK_ERR;
		}
#endif
		if (disk_read(fs->drv, fs->win, sector, 1U) != RES_OK) {
			return FR_DISK_ERR;
		}
			fs->winsect = sector;
	}

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Synchronize file system and strage device                             */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT sync_fs (	/* FR_OK: successful, FR_DISK_ERR: failed */
	FATFS* fs		/* File system object */
)
{
	FRESULT res;

	BYTE *lp;
	res = sync_window(fs);
	if (res == FR_OK) {
		/* Update FSINFO sector if needed */
		if (((fs->fs_type == FS_FAT32) != 0U) && ((fs->fsi_flag == 1U) != 0U)) {
			/* Create FSINFO structure */
			mem_set(fs->win, 0, SS(fs));
			lp = fs->win+BS_55AA;
			ST_WORD(lp, 0xAA55U);
			lp = fs->win+FSI_LeadSig;
			ST_DWORD(lp, 0x41615252);
			lp = fs->win+FSI_StrucSig;
			ST_DWORD(lp, 0x61417272);
			lp = fs->win+FSI_Free_Count;
			ST_DWORD(lp, fs->free_clust);
			lp = fs->win+FSI_Nxt_Free;
			ST_DWORD(lp, fs->last_clust);
			/* Write it into the FSINFO sector */
			fs->winsect = fs->volbase + 1U;
			(void)disk_write(fs->drv, fs->win, fs->winsect, 1U);
			fs->fsi_flag = 0U;
		}
		/* Make sure that no pending write process in the physical drive */
		if (disk_ioctl(fs->drv, CTRL_SYNC, (void*)NULL) != RES_OK) {
			res = FR_DISK_ERR;
		}
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/


DWORD clust2sect (	/* !=0: Sector number, 0: Failed - invalid cluster# */
	FATFS* fs,		/* File system object */
	DWORD clst		/* Cluster# to be converted */
)
{
	DWORD LocalCluster = clst;
	DWORD res;
	LocalCluster -= 2U;
	if (LocalCluster >= (fs->n_fatent - 2)){
		res = ((DWORD)0U);		/* Invalid cluster# */
	}
	else {
		res = ((LocalCluster * fs->csize) + fs->database);
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/


DWORD get_fat (	/* 0xFFFFFFFF:Disk error, 1:Internal error, Else:Cluster status */
	FATFS* fs,	/* File system object */
	DWORD clst	/* Cluster# to get the link information */
)
{
	UINT wc, bc, temp;
	BYTE *p;
	DWORD Status = 0xFFFFFFFFU;


	if (((clst < 2U) != 0U) || ((clst >= fs->n_fatent) != 0U)){	/* Chack range */
		Status = (DWORD)1;
	}
	else{
	switch (fs->fs_type) {
	case FS_FAT12 :
		bc = (UINT)clst; bc += bc / 2;
		if (move_window(fs, fs->fatbase + ((DWORD)bc / (DWORD)SS(fs))) > 0){
			break;
		}
		wc = (UINT)fs->win[bc % (UINT)SS(fs)]; bc++;
		if (move_window(fs, fs->fatbase + ((DWORD)bc / (DWORD)SS(fs))) > 0){
			break;
		}
		temp = fs->win[bc % (UINT)SS(fs)] << 4;
		temp = temp << 4;
		wc |= temp;
		Status = (DWORD)(((clst & (DWORD)1) == (DWORD)(1U)) ? (DWORD)((DWORD)wc >> 4) : (DWORD)((DWORD)wc & (DWORD)0xFFFU));
		break;

	case FS_FAT16 :
		if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) > 0){
			break;
		}
		p = &fs->win[(clst * 2) % SS(fs)];
		Status = (DWORD)LD_WORD(p);
		break;

	case FS_FAT32 :
		if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) > 0){
			break;
		}
		p = &fs->win[(clst * 4) % SS(fs)];
		Status = (LD_DWORD(p) & (DWORD)0x0FFFFFFFU);
		break;
	default :
		/* This is added for Misrac Compliance */
		break;
	}
	}
	return Status;	/* An error occurred at the disk I/O layer */
}




/*-----------------------------------------------------------------------*/
/* FAT access - Change value of a FAT entry                              */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY

FRESULT put_fat (
	FATFS* fs,	/* File system object */
	DWORD clst,	/* Cluster# to be changed in range of 2 to fs->n_fatent - 1 */
	DWORD val	/* New value to mark the cluster */
)
{
	UINT bc;
	BYTE *p;
	FRESULT res;
	DWORD LocVal = val;

	if (((clst < 2U) != 0U) || ((clst >= fs->n_fatent) != 0U)) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;
			res = move_window(fs, fs->fatbase + ((DWORD)bc / (DWORD)SS(fs)));
			if (res != FR_OK){
				break;
			}
			p = &fs->win[bc % SS(fs)];
			*p = (((clst & 1U) == 1U) ? ((*p & 0x0FU) | ((BYTE)LocVal << 4)) : (BYTE)LocVal);
			bc++;
			fs->wflag = 1U;
			res = move_window(fs, fs->fatbase + ((DWORD)bc / (DWORD)SS(fs)));
			if (res != FR_OK){
				break;
			}
			p = &fs->win[bc % SS(fs)];
			*p = (((clst & 1U) == 1U) ? (BYTE)(LocVal >> 4) : ((*p & 0xF0U) | ((BYTE)(LocVal >> 8) & 0x0FU)));
			break;

		case FS_FAT16 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
			if (res != FR_OK){
				break;
			}
			p = &fs->win[(clst * 2) % SS(fs)];
			ST_WORD(p, (WORD)LocVal);
			break;

		case FS_FAT32 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
			if (res != FR_OK){
				break;
			}
			p = &fs->win[(clst * 4) % SS(fs)];
			LocVal |= LD_DWORD(p) & 0xF0000000U;
			ST_DWORD(p, LocVal);
			break;

		default :
			res = FR_INT_ERR;
			break;
		}
		fs->wflag = 1U;
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT remove_chain (
	FATFS* fs,			/* File system object */
	DWORD clst			/* Cluster# to remove a chain from */
)
{
	FRESULT res;
	DWORD nxt;
	DWORD LocClst = clst;
#if _USE_ERASE
	DWORD scl = LocClst, ecl = LocClst, rt[2];
#endif

	if (((LocClst < 2U) != 0U) || ((LocClst >= fs->n_fatent) != 0U)) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		res = FR_OK;
		while (LocClst < fs->n_fatent) {			/* Not a last link? */
			nxt = get_fat(fs, LocClst);			/* Get cluster status */
			if (nxt == 0U) {
				break;				/* Empty cluster? */
			}
			if (nxt == 1U) { res = FR_INT_ERR; break; }	/* Internal error? */
			if (nxt == 0xFFFFFFFFU) { res = FR_DISK_ERR; break; }	/* Disk error? */
			res = put_fat(fs, LocClst, 0U);			/* Mark the cluster "empty" */
			if (res != FR_OK) {
				break;
			}
			if (fs->free_clust != 0xFFFFFFFFU) {	/* Update FSInfo */
				fs->free_clust++;
				fs->fsi_flag |= 1U;
			}
#if _USE_ERASE
			if (ecl + 1 == nxt) {	/* Is next cluster contiguous? */
				ecl = nxt;
			} else {				/* End of contiguous clusters */
				rt[0] = clust2sect(fs, scl);					/* Start sector */
				rt[1] = clust2sect(fs, ecl) + fs->csize - 1;	/* End sector */
				disk_ioctl(fs->drv, CTRL_ERASE_SECTOR, rt);		/* Erase the block */
				scl = ecl = nxt;
			}
#endif
			LocClst = nxt;	/* Next cluster */
		}
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch or Create a cluster chain                      */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
	FATFS* fs,			/* File system object */
	DWORD clst			/* Cluster# to stretch. 0 means create a new chain. */
)
{
	DWORD cs, ncl, scl;
	FRESULT res;


	if (clst == 0U) {		/* Create a new chain */
		scl = fs->last_clust;			/* Get suggested start point */
		if (((!scl) != 0U) || ((scl >= fs->n_fatent) != 0U)){
			scl = 1U;
		}
	}
	else {					/* Stretch the current chain */
		cs = get_fat(fs, clst);			/* Check the cluster status */
		if (cs < 2U) {
			return (DWORD)1;			/* Invalid value */
		}
		if (cs == (DWORD)0xFFFFFFFFU) {
			return cs;	/* A disk error occurred */
		}
		if (cs < fs->n_fatent) {
			return cs;	/* It is already followed by next cluster */
		}
		scl = clst;
	}

	ncl = scl;				/* Start cluster */
	for (;;) {
		ncl++;							/* Next cluster */
		if (ncl >= fs->n_fatent) {		/* Wrap around */
			ncl = 2U;
			if (ncl > scl) {
				return 0;	/* No free cluster */
			}
		}
		cs = get_fat(fs, ncl);			/* Get the cluster status */
		if (cs == 0U) {
			break;				/* Found a free cluster */
		}
		if ((cs == 0xFFFFFFFFU) || (cs == 1U)) {/* An error occurred */
			return cs;
		}
		if (ncl == scl) {
			return 0;		/* No free cluster */
		}
	}

	res = put_fat(fs, ncl, 0x0FFFFFFFU);	/* Mark the new cluster "last link" */
	if ((res == FR_OK) && (clst != 0U)) {
		res = put_fat(fs, clst, ncl);	/* Link it to the previous one if needed */
	}
	if (res == FR_OK) {
		fs->last_clust = ncl;			/* Update FSINFO */
		if (fs->free_clust != 0xFFFFFFFFU) {
			fs->free_clust--;
			fs->fsi_flag |= 1U;
		}
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFFU : 1U;
	}

	return ncl;		/* Return new cluster number or error code */
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

#if _USE_FASTSEEK
static
DWORD clmt_clust (	/* <2:Error, >=2:Cluster number */
	FIL* fp,		/* Pointer to the file object */
	DWORD ofs		/* File offset to be converted to cluster# */
)
{
	DWORD cl, ncl, *tbl;


	tbl = fp->cltbl + 1;	/* Top of CLMT */
	cl = ofs / SS(fp->fs) / fp->fs->csize;	/* Cluster order from top of the file */
	for (;;) {
		ncl = *tbl++;			/* Number of cluters in the fragment */
		if (ncl == ((DWORD)0U)) {
			return 0;		/* End of table? (error) */
		}
		if (cl < ncl) {
			break;	/* In this fragment? */
		}
		cl -= ncl; tbl++;		/* Next fragment */
	}
	return cl + *tbl;	/* Return the cluster number */
}
#endif	/* _USE_FASTSEEK */




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_sdi (
	DIR* dp,		/* Pointer to directory object */
	UINT idx		/* Index of directory table */
)
{
	WORD LocalDirectory = (WORD)idx;
	DWORD clst, sect;
	UINT ic;


	dp->index = (WORD)LocalDirectory;	/* Current index */
	clst = dp->sclust;		/* Table start cluster (0:root) */
	if (((clst == 1U) !=0) || ((clst >= dp->fs->n_fatent) !=0)) {	/* Check start cluster range */
		return FR_INT_ERR;
	}
	if ((clst == ((DWORD)0U)) && ((dp->fs->fs_type == FS_FAT32) != 0U)) {	/* Replace cluster# 0 with root cluster# if in FAT32 */
		clst = dp->fs->dirbase;
	}

	if (clst == 0U) {	/* Static table (root-directory in FAT12/16) */
		if (LocalDirectory >= dp->fs->n_rootdir) {	/* Is index out of range? */
			return FR_INT_ERR;
		}
		sect = dp->fs->dirbase;
	}
	else {				/* Dynamic table (root-directory in FAT32 or sub-directory) */
		ic = (UINT)((UINT)(SS(dp->fs) / SZ_DIR) * (UINT)dp->fs->csize);	/* Entries per cluster */
		while (LocalDirectory >= ic) {	/* Follow cluster chain */
			clst = get_fat(dp->fs, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFFU) {
				 return FR_DISK_ERR;	/* Disk error */
			}
			if (((clst < 2U) != 0U) || ((clst >= dp->fs->n_fatent) != 0U)) {	/* Reached to end of table or internal error */
				return FR_INT_ERR;
			}
			LocalDirectory -= (WORD)ic;
		}
		sect = clust2sect(dp->fs, clst);
	}
	dp->clust = clst;	/* Current cluster# */
	if (sect == ((DWORD)0U)) {return FR_INT_ERR;}
	dp->sect = sect + ((DWORD)idx / ((DWORD)SS(dp->fs) / (DWORD)SZ_DIR));					/* Sector# of the directory entry */
	dp->dir = dp->fs->win + ((idx % (SS(dp->fs) / SZ_DIR)) * SZ_DIR);	/* Ptr to the entry in the sector */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory table index next                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_next (	/* FR_OK:Succeeded, FR_NO_FILE:End of table, FR_DENIED:Could not stretch */
	DIR* dp,		/* Pointer to the directory object */
	s32 stretch		/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD clst;
	UINT i;


	i = ((UINT)dp->index + (UINT)1U);
	if (((!(i & 0xFFFFU)) != 0U) || ((!dp->sect) != 0U)) {	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;
	}

	if ((!(i % (SS(dp->fs) / SZ_DIR))) != 0U) {	/* Sector changed? */
		dp->sect++;					/* Next sector */

		if ((!dp->clust) != 0U) {		/* Static table */
			if (i >= dp->fs->n_rootdir) {	/* Report EOT if it reached end of static table */
				return FR_NO_FILE;
			}
		}
		else {					/* Dynamic table */
			if (((i / (SS(dp->fs) / SZ_DIR)) & (dp->fs->csize - 1U)) == 0U) {	/* Cluster changed? */
				clst = get_fat(dp->fs, dp->clust);				/* Get next cluster */
				if (clst <= 1U) {
					return FR_INT_ERR;
				}
				if (clst == 0xFFFFFFFFU) {
					return FR_DISK_ERR;
				}
				if (clst >= dp->fs->n_fatent) {					/* If it reached end of dynamic table, */
#if !_FS_READONLY
					UINT c;
					if (stretch == ((s32)0)) {
						return FR_NO_FILE;			/* If do not stretch, report EOT */
					}
					clst = create_chain(dp->fs, dp->clust);		/* Stretch cluster chain */
					if (clst == 0U) {
						return FR_DENIED;			/* No free cluster */
					}
					if (clst == 1U) {
						return FR_INT_ERR;
					}
					if (clst == 0xFFFFFFFFU) {
						return FR_DISK_ERR;
					}
					/* Clean-up stretched table */
					if ((sync_window(dp->fs)) != 0U) {
						return FR_DISK_ERR;/* Flush disk access window */
					}
					mem_set(dp->fs->win, 0, SS(dp->fs));		/* Clear window buffer */
					dp->fs->winsect = clust2sect(dp->fs, clst);	/* Cluster start sector */
					for (c = 0U; c < dp->fs->csize; c++) {		/* Fill the new cluster with 0 */
						dp->fs->wflag = 1U;
						if ((sync_window(dp->fs)) != 0U) {
							return FR_DISK_ERR;
						}
						dp->fs->winsect++;
					}
					dp->fs->winsect -= c;						/* Rewind window offset */
#else
					if (stretch == ((s32)0U)) {
						return FR_NO_FILE;			/* If do not stretch, report EOT (this is to suppress warning) */
					}
					return FR_NO_FILE;							/* Report EOT */
#endif
				}
				dp->clust = clst;				/* Initialize data for new cluster */
				dp->sect = clust2sect(dp->fs, clst);
			}
		}
	}

	dp->index = (WORD)i;	/* Current index */
	dp->dir = dp->fs->win + ((i % (SS(dp->fs) / SZ_DIR)) * SZ_DIR);	/* Current entry in the window */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Reserve directory entry                          */
/*-----------------------------------------------------------------------*/

#if !_FS_READONLY
static
FRESULT dir_alloc (
	DIR* dp,	/* Pointer to the directory object */
	UINT nent	/* Number of contiguous entries to allocate (1-21) */
)
{
	FRESULT res;
	UINT n;


	res = dir_sdi(dp, 0U);
	if (res == FR_OK) {
		n = 0U;
		do {
			res = move_window(dp->fs, dp->sect);
			if (res != FR_OK) {
				break;
			}
			if ((dp->dir[0] == DDE) || (dp->dir[0] == 0U)) {	/* Is it a blank entry? */
				n += (UINT)1;
				if (n == nent) {
					break;	/* A block of contiguous entries is found */
				}
			} else {
				n = 0U;					/* Not a blank entry. Restart to search */
			}
			res = dir_next(dp, 1);		/* Next entry with table stretch enabled */
		} while (res == FR_OK);
	}
	if (res == FR_NO_FILE) {
		res = FR_DENIED;	/* No directory entry to allocate */
	}
	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* Directory handling - Load/Store start cluster number                  */
/*-----------------------------------------------------------------------*/

static
DWORD ld_clust (
	FATFS* fs,	/* Pointer to the fs object */
	BYTE* dir	/* Pointer to the directory entry */
)
{
	DWORD cl;

	cl = (DWORD)LD_WORD(dir+DIR_FstClusLO);
	if (fs->fs_type == FS_FAT32) {
		cl |= (DWORD)(LD_WORD(dir+DIR_FstClusHI)) << 16;
	}

	return cl;
}


#if !_FS_READONLY
static
void st_clust (
	BYTE* dir,	/* Pointer to the directory entry */
	DWORD cl	/* Value to be set */
)
{
	ST_WORD(dir+DIR_FstClusLO, cl);
	ST_WORD(dir+DIR_FstClusHI, cl >> 16);
}
#endif




/*-----------------------------------------------------------------------*/
/* LFN handling - Test/Pick/Fit an LFN segment from/to directory entry   */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* Offset of LFN characters in the directory entry */


static
s32 cmp_lfn (			/* 1:Matched, 0:Not matched */
	WCHAR* lfnbuf,		/* Pointer to the LFN to be compared */
	BYTE* dir			/* Pointer to the directory entry containing a part of LFN */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & ~LLE) - 1) * 13;	/* Get offset in the LFN buffer */
	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);	/* Pick an LFN character from the entry */
		if (wc) {	/* Last character has not been processed */
			wc = ff_wtoupper(uc);		/* Convert it to upper case */
			if (i >= _MAX_LFN || wc != ff_wtoupper(lfnbuf[i++]))	/* Compare it */
				return 0;				/* Not matched */
		} else {
			if (uc != 0xFFFF) {
				return 0;	/* Check filler */
			}
		}
	} while (++s < 13);				/* Repeat until all characters in the entry are checked */

	if ((dir[LDIR_Ord] & LLE) && wc && lfnbuf[i]) {	/* Last segment matched but different length */
		return 0;
	}

	return 1;						/* The part of LFN matched */
}



static
s32 pick_lfn (			/* 1:Succeeded, 0:Buffer overflow */
	WCHAR* lfnbuf,		/* Pointer to the Unicode-LFN buffer */
	BYTE* dir			/* Pointer to the directory entry */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);		/* Pick an LFN character from the entry */
		if (wc) {	/* Last character has not been processed */
			if (i >= _MAX_LFN) {
				return 0;	/* Buffer overflow? */
			}
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) {
				return 0;		/* Check filler */
			}
		}
	} while (++s < 13);						/* Read all character in the entry */

	if (dir[LDIR_Ord] & LLE) {				/* Put terminator if it is the last LFN part */
		if (i >= _MAX_LFN) {
			return 0;		/* Buffer overflow? */
		}
		lfnbuf[i] = 0;
	}

	return 1;
}


#if !_FS_READONLY
static
void fit_lfn (
	const WCHAR* lfnbuf,	/* Pointer to the LFN buffer */
	BYTE* dir,				/* Pointer to the directory entry */
	BYTE ord,				/* LFN order (1-20) */
	BYTE sum				/* SFN sum */
)
{
	UINT i, s;
	WCHAR wc;


	dir[LDIR_Chksum] = sum;			/* Set check sum */
	dir[LDIR_Attr] = AM_LFN;		/* Set attribute. LFN entry */
	dir[LDIR_Type] = 0;
	ST_WORD(dir+LDIR_FstClusLO, 0);

	i = (ord - 1) * 13;				/* Get offset in the LFN buffer */
	s = wc = 0;
	do {
		if (wc != 0xFFFF) {
			wc = lfnbuf[i++];	/* Get an effective character */
		}
		ST_WORD(dir+LfnOfs[s], wc);	/* Put it */
		if (!wc) {
			wc = 0xFFFF;		/* Padding characters following last character */
		}
	} while (++s < 13);
	if (wc == 0xFFFF || !lfnbuf[i]) {
		ord |= LLE;	/* Bottom LFN part is the start of LFN sequence */
	}
	dir[LDIR_Ord] = ord;			/* Set the LFN order */
}

#endif
#endif




/*-----------------------------------------------------------------------*/
/* Create numbered name                                                  */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
void gen_numname (
	BYTE* dst,			/* Pointer to the buffer to store numbered SFN */
	const BYTE* src,	/* Pointer to SFN */
	const WCHAR* lfn,	/* Pointer to LFN */
	UINT seq			/* Sequence number */
)
{
	BYTE ns[8], c;
	UINT i, j;


	mem_cpy(dst, src, 11);

	if (seq > 5) {	/* On many collisions, generate a hash number instead of sequential number */
		WCHAR wc;
		DWORD sr = seq;

		while (*lfn) {	/* Create a CRC */
			wc = *lfn++;
			for (i = 0; i < 16; i++) {
				sr = (sr << 1) + (wc & 1);
				wc >>= 1;
				if (sr & 0x10000) {
					sr ^= 0x11021;
				}
			}
		}
		seq = (UINT)sr;
	}

	/* itoa (hexdecimal) */
	i = 7;
	do {
		c = (seq % 16) + '0';
		if (c > '9') {
			c += 7;
		}
		ns[i--] = c;
		seq /= 16;
	} while (seq);
	ns[i] = '~';

	/* Append the number */
	for (j = 0; j < i && dst[j] != ' '; j++) {
		if (IsDBCS1(dst[j])) {
			if (j == i - 1) {
				break;
			}
			j++;
		}
	}
	do {
		dst[j++] = (i < 8) ? ns[i++] : ' ';
	} while (j < 8);
}
#endif




/*-----------------------------------------------------------------------*/
/* Calculate sum of an SFN                                               */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
BYTE sum_sfn (
	const BYTE* dir		/* Pointer to the SFN entry */
)
{
	BYTE sum = 0;
	UINT n = 11;

	do sum = (sum >> 1) + (sum << 7) + *dir++; while (--n);
	return sum;
}
#endif




/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_find (
	DIR* dp			/* Pointer to the directory object linked to the file name */
)
{
	FRESULT res;
	BYTE c, *directory;
#if _USE_LFN
	BYTE a, ord, sum;
#endif

	res = dir_sdi(dp, 0U);			/* Rewind directory object */
	if (res == FR_OK) {

#if _USE_LFN
	ord = sum = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
#endif
	do {
		res = move_window(dp->fs, dp->sect);
		if (res != FR_OK) {
			break;
		}
		directory = dp->dir;					/* Ptr to the directory entry of current index */
		c = *(directory+DIR_Name);
		if (c == 0U) { res = FR_NO_FILE; break; }	/* Reached to end of table */
#if _USE_LFN	/* LFN configuration */
		a = directory[DIR_Attr] & AM_MASK;
		if (c == DDE || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (dp->lfn) {
					if (c & LLE) {		/* Is it start of LFN sequence? */
						sum = directory[LDIR_Chksum];
						c &= ~LLE; ord = c;	/* LFN start order */
						dp->lfn_idx = dp->index;	/* Start index of LFN */
					}
					/* Check validity of the LFN entry and compare it with given name */
					ord = (c == ord && sum == directory[LDIR_Chksum] && cmp_lfn(dp->lfn, directory)) ? ord - 1 : 0xFF;
				}
			} else {					/* An SFN entry is found */
				if (!ord && sum == sum_sfn(directory)) {
					break;	/* LFN matched? */
				}
				if (!(dp->fn[NS] & NS_LOSS) && !mem_cmp(directory, dp->fn, 11)) {
					break;	/* SFN matched? */
				}
				ord = 0xFF; dp->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
			}
		}
#else		/* Non LFN configuration */
		if (((!mem_cmp(directory, dp->fn, 11U)) != 0)&&((!(*(directory+DIR_Attr) & AM_VOL)) != 0U)) { /* Is it a valid entry? */
			break;
		}
#endif
		res = dir_next(dp, 0);		/* Next entry */
	} while (res == FR_OK);
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1 || _USE_LABEL || _FS_RPATH >= 2
static
FRESULT dir_read (
	DIR* dp,		/* Pointer to the directory object */
	s32 vol			/* Filtered by 0:file/directory or 1:volume label */
)
{
	FRESULT res;
	BYTE a, c, *directory;
#if _USE_LFN
	BYTE ord = 0xFF, sum = 0xFF;
#endif

	res = FR_NO_FILE;
	while ((dp->sect) != 0U) {
		res = move_window(dp->fs, dp->sect);
		if (res != FR_OK) {
			break;
		}
		directory = dp->dir;					/* Ptr to the directory entry of current index */
		c = *(directory+DIR_Name);
		if (c == 0U) { res = FR_NO_FILE; break; }	/* Reached to end of table */
		a = (*(directory+DIR_Attr)) & AM_MASK;
#if _USE_LFN	/* LFN configuration */
		if (c == DDE || (!_FS_RPATH && c == '.') || (s32)(a == AM_VOL) != vol) {	/* An entry without valid data */
			ord = 0xFFU;
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (c & LLE) {			/* Is it start of LFN sequence? */
					sum = directory[LDIR_Chksum];
					c &= ~LLE; ord = c;
					dp->lfn_idx = dp->index;
				}
				/* Check LFN validity and capture it */
				ord = (c == ord && sum == directory[LDIR_Chksum] && pick_lfn(dp->lfn, directory)) ? ord - 1 : 0xFF;
			} else {					/* An SFN entry is found */
				if (ord || sum != sum_sfn(directory)) {	/* Is there a valid LFN? */
					dp->lfn_idx = 0xFFFFU;		/* It has no LFN. */
				}
				break;
			}
		}
#else		/* Non LFN configuration */
		if (((c != DDE) != 0U) && ((_FS_RPATH || (c != (BYTE)'.')) != 0) && (a != AM_LFN) && (((a == AM_VOL) == vol)!= 0U)) {	/* Is it a valid entry? */
			break;
		}
#endif
		res = dir_next(dp, 0);				/* Next entry */
		if (res != FR_OK) {
			break;
		}
	}

	if (res != FR_OK) {
		dp->sect = 0U;
	}

	return res;
}
#endif	/* _FS_MINIMIZE <= 1 || _USE_LABEL || _FS_RPATH >= 2 */




/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT dir_register (	/* FR_OK:Successful, FR_DENIED:No free entry or too many SFN collision, FR_DISK_ERR:Disk error */
	DIR* dp				/* Target directory with object name to be created */
)
{
	FRESULT res;
#if _USE_LFN	/* LFN configuration */
	UINT n, nent;
	BYTE sn[12], *fn, sum;
	WCHAR *lfn;


	fn = dp->fn; lfn = dp->lfn;
	mem_cpy(sn, fn, 12);

	if (_FS_RPATH && (sn[NS] & NS_DOT)) {		/* Cannot create dot entry */
		return FR_INVALID_NAME;
	}

	if (sn[NS] & NS_LOSS) {			/* When LFN is out of 8.3 format, generate a numbered name */
		fn[NS] = 0; dp->lfn = 0;			/* Find only SFN */
		for (n = 1; n < 100; n++) {
			gen_numname(fn, sn, lfn, n);	/* Generate a numbered name */
			res = dir_find(dp);				/* Check if the name collides with existing SFN */
			if (res != FR_OK) {
				break;
			}
		}
		if (n == 100) {
			return FR_DENIED;		/* Abort if too many collisions */
		}
		if (res != FR_NO_FILE) {
			return res;	/* Abort if the result is other than 'not collided' */
		}
		fn[NS] = sn[NS]; dp->lfn = lfn;
	}

	if (sn[NS] & NS_LFN) {			/* When LFN is to be created, allocate entries for an SFN + LFNs. */
		for (n = 0; lfn[n]; n++) ;
		nent = (n + 25) / 13;
	} else {						/* Otherwise allocate an entry for an SFN  */
		nent = 1;
	}
	res = dir_alloc(dp, nent);		/* Allocate entries */

	if (res == FR_OK && --nent) {	/* Set LFN entry if needed */
		res = dir_sdi(dp, dp->index - nent);
		if (res == FR_OK) {
			sum = sum_sfn(dp->fn);	/* Sum value of the SFN tied to the LFN */
			do {					/* Store LFN entries in bottom first */
				res = move_window(dp->fs, dp->sect);
				if (res != FR_OK) {
					break;
				}
				fit_lfn(dp->lfn, dp->dir, (BYTE)nent, sum);
				dp->fs->wflag = 1;
				res = dir_next(dp, 0);	/* Next entry */
			} while (res == FR_OK && --nent);
		}
	}
#else	/* Non LFN configuration */
	res = dir_alloc(dp, 1U);		/* Allocate an entry for SFN */
#endif

	if (res == FR_OK) {				/* Set SFN entry */
		res = move_window(dp->fs, dp->sect);
		if (res == FR_OK) {
			mem_set(dp->dir, 0, SZ_DIR);	/* Clean the entry */
			mem_cpy(dp->dir, dp->fn, 11U);	/* Put SFN */
#if _USE_LFN
			dp->dir[DIR_NTres] = *(dp->fn+NS) & (NS_BODY | NS_EXT);	/* Put NT flag */
#endif
			dp->fs->wflag = 1U;
		}
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY && !_FS_MINIMIZE
static
FRESULT dir_remove (	/* FR_OK: Successful, FR_DISK_ERR: A disk error */
	DIR* dp				/* Directory object pointing the entry to be removed */
)
{
	FRESULT res;
	WORD temp;
#if _USE_LFN	/* LFN configuration */
	UINT i;
	i = dp->index;	/* SFN index */
	res = dir_sdi(dp, (WORD)((dp->lfn_idx == 0xFFFF) ? i : dp->lfn_idx));	/* Goto the SFN or top of the LFN entries */
	if (res == FR_OK) {
		do {
			res = move_window(dp->fs, dp->sect);
			if (res != FR_OK) {
				break;
			}
			mem_set(dp->dir, 0, SZ_DIR);	/* Clear and mark the entry "deleted" */
			*dp->dir = DDE;
			dp->fs->wflag = 1;
			if (dp->index >= i) {
				break;	/* When reached SFN, all entries of the object has been deleted. */
			}
			res = dir_next(dp, 0);		/* Next entry */
		} while (res == FR_OK);
		if (res == FR_NO_FILE) {
			res = FR_INT_ERR;
		}
	}

#else			/* Non LFN configuration */
	temp = dp->index;
	res = dir_sdi(dp, temp);
	if (res == FR_OK) {
		res = move_window(dp->fs, dp->sect);
		if (res == FR_OK) {
			mem_set(dp->dir, 0, SZ_DIR);	/* Clear and mark the entry "deleted" */
			*dp->dir = DDE;
			dp->fs->wflag = 1U;
		}
	}
#endif

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2
static
void get_fileinfo (		/* No return code */
	DIR* dp,			/* Pointer to the directory object */
	FILINFO* fno	 	/* Pointer to the file information to be filled */
)
{
	UINT i;
	TCHAR *p, c;


	p = fno->fname;
	if (dp->sect!=((DWORD)0U)) {		/* Get SFN */
		BYTE *dir = dp->dir;

		i = 0U;
		while (i < 11U) {		/* Copy name body and extension */
			c = (TCHAR)(*(dir+i));
			i += (UINT)1;
			if (c == ' ') {
				continue;			/* Skip padding spaces */
			}
			if (c == (BYTE)NDDE) {
				c = (TCHAR)DDE;	/* Restore replaced DDE character */
			}
			if (i == (UINT)9) {
				*p = '.';			/* Insert a . if extension is exist */
				p += 1U;
			}
#if _USE_LFN
			if (IsUpper(c) && (dir[DIR_NTres] & (i >= 9U ? NS_EXT : NS_BODY))) {
				c += 0x20;			/* To lower */
			}
#if _LFN_UNICODE
			if (IsDBCS1(c) && i != 8 && i != 11 && IsDBCS2(dir[i])) {
				c = c << 8 | dir[i++];
			}
			c = ff_convert(c, 1);	/* OEM -> Unicode */
			if (c == ((TCHAR)0U)) {
				c = '?';
			}
#endif
#endif
			*p = c;
			p += 1U;
		}
		fno->fattrib = (*(dir+DIR_Attr));				/* Attribute */
		fno->fsize = LD_DWORD(dir+DIR_FileSize);	/* Size */
		fno->fdate = LD_WORD(dir+DIR_WrtDate);		/* Date */
		fno->ftime = LD_WORD(dir+DIR_WrtTime);		/* Time */
	}
	*p = 0;		/* Terminate SFN string by a \0 */

#if _USE_LFN
	if (fno->lfname) {
		WCHAR w, *lfn;

		i = 0; p = fno->lfname;
		if (dp->sect && fno->lfsize && dp->lfn_idx != 0xFFFF) {	/* Get LFN if available */
			lfn = dp->lfn;
			while ((w = *lfn++) != 0) {		/* Get an LFN character */
#if !_LFN_UNICODE
				w = ff_convert(w, 0);		/* Unicode -> OEM */
				if (!w) { i = 0; break; }	/* No LFN if it could not be converted */
				if (DF1S && w >= 0x100) {	/* Put 1st byte if it is a DBC (always false on SBCS cfg) */
					p[i++] = (TCHAR)(w >> 8);
				}
#endif
				if (i >= fno->lfsize - 1) { i = 0; break; }	/* No LFN if buffer overflow */
				p[i++] = (TCHAR)w;
			}
		}
		p[i] = 0;	/* Terminate LFN string by a \0 */
	}
#endif
}
#endif /* _FS_MINIMIZE <= 1 || _FS_RPATH >= 2*/




/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/

static
FRESULT create_name (
	DIR* dp,			/* Pointer to the directory object */
	const TCHAR** path	/* Pointer to pointer to the segment in the path string */
)
{
#if _USE_LFN	/* LFN configuration */
	BYTE b, cf;
	WCHAR w, *lfn;
	UINT i, ni, si, di;
	const BYTE *p;

	/* Create LFN in Unicode */
	for (p = *path; *p == '/' || *p == '\\'; p++) ;	/* Strip duplicated separator */
	lfn = dp->lfn;
	si = di = 0;
	for (;;) {
		w = p[si++];					/* Get a character */
		if (w < ' ' || w == '/' || w == '\\') {
			break;	/* Break on end of segment */
		}
		if (di >= _MAX_LFN)	{			/* Reject too long name */
			return FR_INVALID_NAME;
		}
#if !_LFN_UNICODE
		w &= 0xFF;
		if (IsDBCS1(w)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			b = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(b)) {
				return FR_INVALID_NAME;	/* Reject invalid sequence */
			}
			w = (w << 8) + b;			/* Create a DBC */
		}
		w = ff_convert(w, 1);			/* Convert ANSI/OEM to Unicode */
		if (!w) {
			return FR_INVALID_NAME;	/* Reject invalid code */
		}
#endif
		if (w < 0x80 && chk_chr("\"*:<>\?|\x7F", w)) {/* Reject illegal characters for LFN */
			return FR_INVALID_NAME;
		}
		lfn[di++] = w;					/* Store the Unicode character */
	}
	*path = &p[si];						/* Return pointer to the next segment */
	cf = (w < ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */
#if _FS_RPATH
	if ((di == 1 && lfn[di-1] == '.') || /* Is this a dot entry? */
		(di == 2 && lfn[di-1] == '.' && lfn[di-2] == '.')) {
		lfn[di] = 0;
		for (i = 0; i < 11; i++)
			dp->fn[i] = (i < di) ? '.' : ' ';
		dp->fn[i] = cf | NS_DOT;		/* This is a dot entry */
		return FR_OK;
	}
#endif
	while (di) {						/* Strip trailing spaces and dots */
		w = lfn[di-1];
		if (w != ' ' && w != '.') {
			break;
		}
		di--;
	}
	if (!di) {
		return FR_INVALID_NAME;	/* Reject nul string */
	}

	lfn[di] = 0;						/* LFN is created */

	/* Create SFN in directory form */
	mem_set(dp->fn, ' ', 11);
	for (si = 0; lfn[si] == ' ' || lfn[si] == '.'; si++) ;	/* Strip leading spaces and dots */
	if (si) {
		cf |= NS_LOSS | NS_LFN;
	}
	while (di && lfn[di - 1] != '.') {
		di--;	/* Find extension (di<=si: no extension) */
	}

	b = i = 0; ni = 8;
	for (;;) {
		w = lfn[si++];					/* Get an LFN character */
		if ((!w) != 0U) {
			break;					/* Break on end of the LFN */
		}
		if (w == ' ' || (w == '.' && si != di)) {	/* Remove spaces and dots */
			cf |= NS_LOSS | NS_LFN; continue;
		}

		if (i >= ni || si == di) {		/* Extension or end of SFN */
			if (ni == 11) {				/* Long extension */
				cf |= NS_LOSS | NS_LFN; break;
			}
			if (si != di) {
				cf |= NS_LOSS | NS_LFN;	/* Out of 8.3 format */
			}
			if (si > di) {
				break;			/* No extension */
			}
			si = di; i = 8; ni = 11;	/* Enter extension section */
			b <<= 2; continue;
		}

		if (w >= 0x80) {				/* Non ASCII character */
#ifdef _EXCVT
			w = ff_convert(w, 0);		/* Unicode -> OEM code */
			if (w) {
				w = ExCvt[w - 0x80];	/* Convert extended character to upper (SBCS) */
			}
#else
			w = ff_convert(ff_wtoupper(w), 0);	/* Upper converted Unicode -> OEM code */
#endif
			cf |= NS_LFN;				/* Force create LFN entry */
		}

		if (DF1S && w >= 0x100) {		/* Double byte character (always false on SBCS cfg) */
			if (i >= ni - 1) {
				cf |= NS_LOSS | NS_LFN; i = ni; continue;
			}
			dp->fn[i++] = (BYTE)(w >> 8);
		} else {						/* Single byte character */
			if (!w || chk_chr("+,;=[]", w)) {	/* Replace illegal characters for SFN */
				w = '_'; cf |= NS_LOSS | NS_LFN;/* Lossy conversion */
			} else {
				if (IsUpper(w)) {		/* ASCII large capital */
					b |= 2;
				} else {
					if (IsLower(w)) {	/* ASCII small capital */
						b |= 1; w -= 0x20;
					}
				}
			}
		}
		dp->fn[i++] = (BYTE)w;
	}

	if (dp->fn[0] == DDE) {
		dp->fn[0] = NDDE;	/* If the first character collides with deleted mark, replace it with 0x05 */
	}

	if (ni == 8) {
		b <<= 2;
	}
	if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03) {	/* Create LFN entry when there are composite capitals */
		cf |= NS_LFN;
	}
	if (!(cf & NS_LFN)) {						/* When LFN is in 8.3 format without extended character, NT flags are created */
		if ((b & 0x03) == 0x01) {
			cf |= NS_EXT;	/* NT flag (Extension has only small capital) */
		}
		if ((b & 0x0C) == 0x04) {
			cf |= NS_BODY;	/* NT flag (Filename has only small capital) */
		}
	}

	dp->fn[NS] = cf;	/* SFN is created */

	return FR_OK;


#else	/* Non-LFN configuration */
	BYTE b, c, d, *sfn;
	UINT ni, si, i;
	const BYTE *p;
	p = ((const BYTE *)(*path));
	/* Create file name in directory form */
	for (; (*p == (BYTE)'/') || (*p == (BYTE)'\\'); ) {p++;}/* Strip duplicated separator */
	sfn = dp->fn;
	mem_set(sfn, (s32)' ', 11U);
	si =  0U;
	i =  0U;
	b = 0U; ni = 8U;
#if _FS_RPATH
	if (p[si] == '.') { /* Is this a dot entry? */
		for (;;) {
			c = (BYTE)p[si++];
			if (c != '.' || si >= 3) {
				break;
			}
			sfn[i++] = c;
		}
		if (c != '/' && c != '\\' && c > ' ') {
			return FR_INVALID_NAME;
		}
		*path = &p[si];									/* Return pointer to the next segment */
		sfn[NS] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT;	/* Set last segment flag if end of path */
		return FR_OK;
	}
#endif
	for (;;) {
		c = ((BYTE)(*(p+si)));
		si+=1U;
		if (((c <= (BYTE)' ') != 0) || ((c == (BYTE)'/') != 0) || ((c == (BYTE)'\\') != 0)) {
			break;	/* Break on end of segment */
		}
		if ((c == (BYTE)'.') || (i >= ni)) {
			if ((ni != 8U) || (c != (BYTE)'.')) {
				return FR_INVALID_NAME;
			}
			i = 8U; ni = 11U;
			b <<= 2U; continue;
		}
		if (c >= 0x80U) {				/* Extended character? */
			b |= 3U;						/* Eliminate NT flag */
#ifdef _EXCVT
			c = ExCvt[c - 0x80U];		/* To upper extended characters (SBCS cfg) */
#else
#if !DF1S
			return FR_INVALID_NAME;		/* Reject extended characters (ASCII cfg) */
#endif
#endif
		}
		if (IsDBCS1(c)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			d = ((BYTE)(*(p+si)));			/* Get 2nd byte */
			si+=1U;
			if ((!IsDBCS2(d)) || (i >= (ni - 1))) {	/* Reject invalid DBC */
				return FR_INVALID_NAME;
			}
			*(sfn+i) = c;
			i+=1U;
			*(sfn+i) = d;
			i+=1U;
		} else {						/* Single byte code */
			if ((chk_chr("\"*+,:;<=>\?[]|\x7F", c)) != 0) {	/* Reject illegal chrs for SFN */
				return FR_INVALID_NAME;
			}
			if (IsUpper(c)) {			/* ASCII large capital? */
				b |= 2U;
			} else {
				if (IsLower(c)) {		/* ASCII small capital? */
					b |= 1U; c -= 0x20U;
				}
			}
			*(sfn+i) = c;
			i+=1U;
		}
	}
	*path = ((const TCHAR *)(p+si));						/* Return pointer to the next segment */
	c = (c <= (BYTE)' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */

	if (i == (UINT)0U){
		return FR_INVALID_NAME;		/* Reject nul string */
	}
	if ((*(sfn+0)) == DDE){
		*(sfn+0) = NDDE;	/* When first char collides with 0xE5, replace it with 0x05 */
	}
	if (ni == 8U){
		b <<= 2U;
	}
	if ((b & 0x03U) == 0x01U){
		c |= NS_EXT;	/* NT flag (Name extension has only small capital) */
	}
	if ((b & 0x0CU) == 0x04U){
		c |= NS_BODY;	/* NT flag (Name body has only small capital) */
	}

	*(sfn+NS) = c;		/* Store NT flag, File name is created */

	return FR_OK;
#endif
}




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static
FRESULT follow_path (	/* FR_OK(0): successful, !=0: error code */
	DIR* dp,			/* Directory object to return last directory and found object */
	const TCHAR* path	/* Full-path string to find a file or directory */
)
{
	FRESULT res;
	BYTE *directory, NameStatus;
	TCHAR Status = *path;
	const TCHAR *LocPath = path;

#if _FS_RPATH
	if (*LocPath == '/' || *LocPath == '\\') { /* There is a heading separator */
		LocPath++;	dp->sclust = 0;		/* Strip it and start from the root dir */
	} else {							/* No heading separator */
		dp->sclust = dp->fs->cdir;	/* Start from the current dir */
	}
#else
	if ((*LocPath == '/') || (*LocPath == '\\')) {	/* Strip heading separator if exist */
		LocPath++;
	}
	dp->sclust = 0U;						/* Start from the root dir */
#endif
	Status = *LocPath;
	if ((UINT)Status < (UINT)' ') {				/* Null path name is the origin directory itself */
		res = dir_sdi(dp, 0U);
		dp->dir = 0U;
	} else {								/* Follow path */
		for (;;) {
			res = create_name(dp, &LocPath);	/* Get a segment name of the path */
			if (res != FR_OK) {
				break;
			}
			res = dir_find(dp);				/* Find an object with the sagment name */
			NameStatus = *(dp->fn+NS);
			if (res != FR_OK) {				/* Failed to find the object */
				if (res == FR_NO_FILE) {	/* Object is not found */
					if (_FS_RPATH && ((NameStatus & NS_DOT) != 0U)) {	/* If dot entry is not exist, */
						dp->sclust = 0U; dp->dir = 0U;	/* it is the root directory and stay there */
						if ((!(NameStatus & NS_LAST))!= 0U) {
							continue;	/* Continue to follow if not last segment */
						}
						res = FR_OK;					/* Ended at the root directroy. Function completed. */
					} else {							/* Could not find the object */
						if ((!(NameStatus & NS_LAST)) != 0U) {
							res = FR_NO_PATH;	/* Adjust error code if not last segment */
						}
					}
				}
				break;
			}
			if ((NameStatus & NS_LAST) != 0U) {
				break;			/* Last segment matched. Function completed. */
			}
			directory = dp->dir;						/* Follow the sub-directory */
			if ((!((*(directory+DIR_Attr)) & AM_DIR)) != 0U) {	/* It is not a sub-directory and cannot follow */
				res = FR_NO_PATH; break;
			}
			dp->sclust = (DWORD)ld_clust(dp->fs, directory);
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Get logical drive number from path name                               */
/*-----------------------------------------------------------------------*/

static
s32 get_ldnumber (		/* Returns logical drive number (-1:invalid drive) */
	const TCHAR** path	/* Pointer to pointer to the path name */
)
{
	const TCHAR *tp, *tt;
	UINT i;
	s32 vol = -1;


	if ((*path)!=NULL) {	/* If the pointer is not a null */
		tt = *path;
		for (; (*tt >= '!') && (*tt != ':'); ) {tt += 1U;}	/* Find ':' in the path */
		if (*tt == ':') {	/* If a ':' is exist in the path name */
			tp = *path;
			i = (*tp - '0');
			tp += 1U;
			if ((i < 10U) && (tp == tt)) {	/* Is there a numeric drive id? */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (s32)i;
					tt += 1U;
					*path = ((TCHAR *)tt);
				}
			} else {	/* No numeric drive number */
#if _STR_VOLUME_ID		/* Find string drive id */
				static const char* const str[] = {_VOLUME_STRS};
				const char *sp;
				char c;
				TCHAR tc;

				i = 0; tt += 1U;
				do {
					sp = str[i]; tp = *path;
					do {	/* Compare a string drive id with path name */
						c = *sp++; tc = *tp++;
						if (IsLower(tc)) {
							tc -= 0x20;
						}
					} while (c && (TCHAR)c == tc);
				} while ((c || tp != tt) && ++i < _VOLUMES);	/* Repeat for each id until pattern match */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (s32)i;
					*path = tt;
				}
#endif
			}
		}
		else {
#if _FS_RPATH && _VOLUMES >= 2
			vol = CurrVol;	/* Current drive */
#else
			vol = 0;		/* Drive 0 */
#endif
		}
	}
	return vol;
}




/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT boot sector                   */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (	/* 0:FAT boor sector, 1:Valid boor sector but not FAT, 2:Not a boot sector, 3:Disk error */
	FATFS* fs,	/* File system object */
	DWORD sect	/* Sector# (lba) to check if it is an FAT boot record or not */
)
{
	fs->wflag = 0U; fs->winsect = 0xFFFFFFFFU;	/* Invaidate window */
	if (move_window(fs, sect) != FR_OK)	{		/* Load boot record */
		return (BYTE)3;
	}

	if (LD_WORD(fs->win+BS_55AA) != 0xAA55U) {	/* Check boot record signature (always placed at offset 510 even if the sector size is >512) */
		return (BYTE)2;
	}

	if ((LD_DWORD(fs->win+BS_FilSysType) & 0xFFFFFFU) == 0x544146U) {		/* Check "FAT" string */
		return (BYTE)0;
	}
	if ((LD_DWORD(fs->win+BS_FilSysType32) & 0xFFFFFFU) == 0x544146U) {	/* Check "FAT" string */
		return (BYTE)0;
	}

	return (BYTE)1;
}




/*-----------------------------------------------------------------------*/
/* Find logical drive and check if the volume is mounted                 */
/*-----------------------------------------------------------------------*/

static
FRESULT find_volume (	/* FR_OK(0): successful, !=0: any error occurred */
	FATFS** rfs,		/* Pointer to pointer to the found file system object */
	const TCHAR** path,	/* Pointer to pointer to the path name (drive number) */
	BYTE wmode			/* !=0: Check write protection for write access */
)
{
	static WORD Fsid;					/* File system mount ID */
	BYTE fmt;
	s32 vol;
	DSTATUS stat;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat;
	WORD nrsv;
	FATFS *fs;


	/* Get logical drive number from the path name */
	(*rfs) = 0;
	vol = get_ldnumber(path);
	if (vol < 0) {return FR_INVALID_DRIVE;}

	/* Check if the file system object is valid or not */
	fs = FatFs[vol];					/* Get pointer to the file system object */
	if ((!fs) != 0U) {
		return FR_NOT_ENABLED;		/* Is the file system object available? */
	}

	ENTER_FF(fs);						/* Lock the volume */
	*rfs = fs;							/* Return pointer to the file system object */

	if (fs->fs_type != 0U) {					/* If the volume has been mounted */
		stat = disk_status(fs->drv);
		if ((!(stat & STA_NOINIT)) != 0U) {		/* and the physical drive is kept initialized */
			if (!_FS_READONLY && (wmode != (BYTE)0U) && ((stat & STA_PROTECT) != (BYTE)0U)) {/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
			}
			return FR_OK;				/* The file system object is valid */
		}
	}

	/* The file system object is not valid. */
	/* Following code attempts to mount the volume. (analyze BPB and initialize the fs object) */

	fs->fs_type = 0U;					/* Clear the file system object */
	fs->drv = LD2PD(vol);				/* Bind the logical drive and a physical drive */
	stat = disk_initialize(fs->drv);	/* Initialize the physical drive */
	if ((stat & STA_NOINIT) != (BYTE)0U) {				/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no medium or hard error */
	}
	if (!_FS_READONLY && (wmode != (BYTE)0U)&& ((stat & STA_PROTECT) != (BYTE)0U)) {	/* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
	}
#if _MAX_SS != _MIN_SS						/* Get sector size (multiple sector size cfg only) */
	if (disk_ioctl(fs->drv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK
		|| SS(fs) < _MIN_SS || SS(fs) > _MAX_SS) {
		 return FR_DISK_ERR;
	}
#endif
	/* Find an FAT partition on the drive. Supports only generic partitioning, FDISK and SFD. */
	bsect = 0U;
	fmt = check_fs(fs, bsect);					/* Load sector 0 and check if it is an FAT boot sector as SFD */
	if ((fmt == 1U) || (((!fmt) != (BYTE)0U) && ((LD2PT(vol)) != 0U))) {	/* Not an FAT boot sector or forced partition number */
		UINT i;
		DWORD br[4];

		for (i = 0U; i < 4U; i++) {			/* Get partition offset */
			BYTE *pt = fs->win+MBR_Table + ((WORD)i * (WORD)SZ_PTE);
			br[i] = ((*(pt+4)) != (BYTE)0U) ? LD_DWORD((pt+8U)) : 0U;
		}
		i = LD2PT(vol);						/* Partition number: 0:auto, 1-4:forced */
		if (i != 0U) {
			i--;
		}
		do {								/* Find an FAT volume */
			bsect = br[i];
			fmt = (bsect!=(DWORD)0U) ? check_fs(fs, bsect) : 2U;	/* Check the partition */
			i += (UINT)1;
		} while ((!LD2PT(vol)) && (fmt != (BYTE)0U)&& (i < 4U));
	}
	if (fmt == 3U) {
		return FR_DISK_ERR;		/* An error occured in the disk I/O layer */
	}
	if (fmt != 0U) {
		return FR_NO_FILESYSTEM;		/* No FAT volume is found */
	}

	/* An FAT volume is found. Following code initializes the file system object */

	if (LD_WORD(fs->win+BPB_BytsPerSec) != (WORD)SS(fs)) {	/* (BPB_BytsPerSec must be equal to the physical sector size) */
		return FR_NO_FILESYSTEM;
	}

	fasize = ((DWORD)LD_WORD(fs->win+BPB_FATSz16));				/* Number of sectors per FAT */
	if ((!fasize) != 0U) {
		fasize = (DWORD)LD_DWORD(fs->win+BPB_FATSz32);
	}
	fs->fsize = fasize;

	fs->n_fats = fs->win[BPB_NumFATs];					/* Number of FAT copies */
	if ((fs->n_fats != 1U) && (fs->n_fats != 2U)) {			/* (Must be 1 or 2) */
		return FR_NO_FILESYSTEM;
	}
	fasize *= (DWORD)fs->n_fats;								/* Number of sectors for FAT area */

	fs->csize = fs->win[BPB_SecPerClus];				/* Number of sectors per cluster */
	if (((!fs->csize) != 0U) || ((fs->csize & (fs->csize - 1U)) != 0U)) {	/* (Must be power of 2) */
		return FR_NO_FILESYSTEM;
	}

	fs->n_rootdir = LD_WORD(fs->win+BPB_RootEntCnt);	/* Number of root directory entries */
	if ((fs->n_rootdir % (SS(fs) / SZ_DIR)) != 0U) {				/* (Must be sector aligned) */
		return FR_NO_FILESYSTEM;
	}

	tsect = ((DWORD)LD_WORD(fs->win+BPB_TotSec16));				/* Number of sectors on the volume */
	if ((!tsect) != 0U) {
		tsect = (DWORD)LD_DWORD(fs->win+BPB_TotSec32);
	}

	nrsv = LD_WORD(fs->win+BPB_RsvdSecCnt);				/* Number of reserved sectors */
	if ((!nrsv) != 0U) {
		return FR_NO_FILESYSTEM;					/* (Must not be 0) */
	}

	/* Determine the FAT sub type */
	sysect = (DWORD)nrsv + fasize + (fs->n_rootdir / (SS(fs) / SZ_DIR));	/* RSV+FAT+DIR */
	if (tsect < sysect) {
		return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
	}
	nclst = (tsect - sysect) / ((DWORD)fs->csize);				/* Number of clusters */
	if ((!nclst) != 0U) {
		return FR_NO_FILESYSTEM;				/* (Invalid volume size) */
	}
	fmt = FS_FAT12;
	if (nclst >= MIN_FAT16) {
		fmt = FS_FAT16;
	}
	if (nclst >= MIN_FAT32) {
		fmt = FS_FAT32;
	}

	/* Boundaries and Limits */
	fs->n_fatent = nclst + 2U;							/* Number of FAT entries */
	fs->volbase = bsect;								/* Volume start sector */
	fs->fatbase = bsect + nrsv; 						/* FAT start sector */
	fs->database = bsect + sysect;						/* Data start sector */
	if (fmt == FS_FAT32) {
		if (fs->n_rootdir != 0U) {
			return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
		}
		fs->dirbase = LD_DWORD(fs->win+BPB_RootClus);	/* Root directory start cluster */
		szbfat = fs->n_fatent * 4U;						/* (Needed FAT size) */
	} else {
		if ((!fs->n_rootdir) != 0U)	{
			return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
		}
		fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
		szbfat = (((DWORD)fmt) == FS_FAT16) ?					/* (Needed FAT size) */
			(fs->n_fatent * ((DWORD)2U)) : (((fs->n_fatent * (DWORD)3U) / (DWORD)2U) + (fs->n_fatent & (DWORD)1U));
	}
	if (fs->fsize < ((szbfat + (SS(fs) - 1)) / SS(fs))) {	/* (BPB_FATSz must not be less than needed) */
		return FR_NO_FILESYSTEM;
	}

#if !_FS_READONLY
	/* Initialize cluster allocation information */
	fs->last_clust = 0xFFFFFFFFU;
	fs->free_clust = 0xFFFFFFFFU;

	/* Get fsinfo if available */
	fs->fsi_flag = 0x80U;
#if (_FS_NOFSINFO & 3) != 3U
	if ((fmt == FS_FAT32)				/* Enable FSINFO only if FAT32 and BPB_FSInfo is 1 */
		&& (LD_WORD(fs->win+BPB_FSInfo) == 1U)
		&& (move_window(fs, bsect + 1U) == FR_OK))
	{
		fs->fsi_flag = 0U;
		if ((LD_WORD(fs->win+BS_55AA) == 0xAA55U)	/* Load FSINFO data if available */
			&& (LD_DWORD(fs->win+FSI_LeadSig) == 0x41615252U)
			&& (LD_DWORD(fs->win+FSI_StrucSig) == 0x61417272U))
		{
#if (_FS_NOFSINFO & 1) == 0
			fs->free_clust = LD_DWORD(fs->win+FSI_Free_Count);
#endif
#if (_FS_NOFSINFO & 2) == 0
			fs->last_clust = LD_DWORD(fs->win+FSI_Nxt_Free);
#endif
		}
	}
#endif
#endif
	fs->fs_type = fmt;	/* FAT sub-type */
	Fsid += (WORD)1;
	fs->id = Fsid;	/* File system mount ID */
#if _FS_RPATH
	fs->cdir = 0;		/* Set current directory to root */
#endif
#if _FS_LOCK			/* Clear file lock semaphores */
	clear_lock(fs);
#endif

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/directory object is valid or not                    */
/*-----------------------------------------------------------------------*/

static
FRESULT validate (	/* FR_OK(0): The object is valid, !=0: Invalid */
	void* obj		/* Pointer to the object FIL/DIR to check validity */
)
{
	FIL *fil = (FIL *)(void *)obj;	/* Assuming offset of .fs and .id in the FIL/DIR structure is identical */
	FRESULT res;

	if (((!fil) != 0U) || ((!fil->fs) != 0U) || ((!fil->fs->fs_type) != 0U) || (fil->fs->id != fil->id)) {
		res = FR_INVALID_OBJECT;
	}
	else {
		ENTER_FF(fil->fs);		/* Lock file system */

		if ((disk_status(fil->fs->drv) & STA_NOINIT) != (BYTE)0U) {
			res = FR_NOT_READY;
		}
		else {
			res = FR_OK;
		}
	}
	return res;
}




/*--------------------------------------------------------------------------

   Public Functions

--------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	FATFS* fs,			/* Pointer to the file system object (NULL:unmount)*/
	const TCHAR* path,	/* Logical drive number to be mounted/unmounted */
	BYTE opt			/* 0:Do not mount (delayed mount), 1:Mount immediately */
)
{
	FATFS *cfs;
	s32 vol;
	FRESULT res;
	const TCHAR *rp = path;


	vol = get_ldnumber(&rp);
	if (vol < 0) {return FR_INVALID_DRIVE;}
	cfs = FatFs[vol];					/* Pointer to fs object */

	if (cfs != NULL) {
#if _FS_LOCK
		clear_lock(cfs);
#endif
#if _FS_REENTRANT						/* Discard sync object of the current volume */
		if ((!ff_del_syncobj(cfs->sobj)) != 0U) {
			return FR_INT_ERR;
		}
#endif
		cfs->fs_type = 0U;				/* Clear old fs object */
	}

	if (fs != NULL) {
		fs->fs_type = 0U;				/* Clear new fs object */
#if _FS_REENTRANT						/* Create sync object for the new volume */
		if ((!ff_cre_syncobj((BYTE)vol, &fs->sobj)) != 0U) {
			return FR_INT_ERR;
		}
#endif
	}
	FatFs[vol] = fs;					/* Register new fs object */

	if (((!fs) != 0U) || (opt != 1U)) {
		return FR_OK;	/* Do not mount now, it will be mounted later */
	}

	res = find_volume(&fs, &path, 0U);	/* Force mounted the volume */
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL* fp,			/* Pointer to the blank file object */
	const TCHAR* path,	/* Pointer to the file name */
	BYTE mode			/* Access mode and file open mode flags */
)
{
	FRESULT res;
	DIR dj = {0};
	BYTE *dir;
	DEF_NAMEBUF;


	if ((!fp) != 0U) {
		return FR_INVALID_OBJECT;
	}
	fp->fs = NULL;			/* Clear file object */

	/* Get logical drive number */
#if !_FS_READONLY
	mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW;
	res = find_volume(&dj.fs, &path, (BYTE)(mode & ~FA_READ));
#else
	mode &= FA_READ;
	res = find_volume(&dj.fs, &path, 0U);
#endif
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		dir = dj.dir;
#if !_FS_READONLY	/* R/W configuration */
		if (res == FR_OK) {
			if ((!dir) != 0U)	{/* Default directory itself */
				res = FR_INVALID_NAME;
			}
#if _FS_LOCK
			else
				res = chk_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
#endif
		}
		/* Create or Open a file */
		if ((mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW)) != (BYTE)0U) {
			DWORD dw, cl;

			if (res != FR_OK) {					/* No file, create new */
				if (res == FR_NO_FILE) {			/* There is no file to open, create a new entry */
#if _FS_LOCK
					res = enq_lock() ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES;
#else
					res = dir_register(&dj);
#endif
				}
				mode |= FA_CREATE_ALWAYS;		/* File is created */
				dir = dj.dir;					/* New entry */
			}
			else {								/* Any object is already existing */
				if ((*(dir+DIR_Attr) & (AM_RDO | AM_DIR)) != 0U) {	/* Cannot overwrite it (R/O or DIR) */
					res = FR_DENIED;
				} else {
					if ((mode & FA_CREATE_NEW) != (BYTE)0U) {	/* Cannot create as new file */
						res = FR_EXIST;
					}
				}
			}
			if ((res == FR_OK) && ((mode & FA_CREATE_ALWAYS) != (BYTE)0U)) {	/* Truncate it if overwrite mode */
				dw = get_fattime();				/* Created time */
				ST_DWORD(dir+DIR_CrtTime, dw);
				(*(dir+DIR_Attr)) = 0U;				/* Reset attribute */
				ST_DWORD(dir+DIR_FileSize, 0);	/* size = 0 */
				cl = ld_clust(dj.fs, dir);		/* Get start cluster */
				st_clust(dir, 0U);				/* cluster = 0 */
				dj.fs->wflag = 1U;
				if (cl != (DWORD)0U) {						/* Remove the cluster chain if exist */
					dw = dj.fs->winsect;
					res = remove_chain(dj.fs, cl);
					if (res == FR_OK) {
						dj.fs->last_clust = cl - 1;	/* Reuse the cluster hole */
						res = move_window(dj.fs, dw);
					}
				}
			}
		}
		else {	/* Open an existing file */
			if (res == FR_OK) {					/* Follow succeeded */
				if ((*(dir+DIR_Attr) & AM_DIR) != 0U) {	/* It is a directory */
					res = FR_NO_FILE;
				} else {
					if (((mode & FA_WRITE) != 0U) && (((*(dir+DIR_Attr)) & AM_RDO) != 0U)) {/* R/O violation */
						res = FR_DENIED;
					}
				}
			}
		}
		if (res == FR_OK) {
			if ((mode & FA_CREATE_ALWAYS) != 0U) {		/* Set file change flag if created or overwritten */
				mode |= FA__WRITTEN;
			}
			fp->dir_sect = dj.fs->winsect;		/* Pointer to the directory entry */
			fp->dir_ptr = dir;
#if _FS_LOCK
			fp->lockid = inc_lock(&dj, (mode & ~FA_READ) ? 1 : 0);
			if ((!fp->lockid) != 0U) {
				res = FR_INT_ERR;
			}
#endif
		}

#else				/* R/O configuration */
		if (res == FR_OK) {					/* Follow succeeded */
			dir = dj.dir;
			if ((!dir) != 0U) {						/* Current directory itself */
				res = FR_INVALID_NAME;
			} else {
				if (((*(dir+DIR_Attr)) & AM_DIR) != 0U)	{/* It is a directory */
					res = FR_NO_FILE;
				}
			}
		}
#endif
		FREE_BUF();

		if (res == FR_OK) {
			fp->flag = mode;					/* File access mode */
			fp->err = 0U;						/* Clear error flag */
			fp->sclust = ld_clust(dj.fs, dir);	/* File start cluster */
			fp->fsize = LD_DWORD(dir+DIR_FileSize);	/* File size */
			fp->fptr = 0U;						/* File pointer */
			fp->dsect = 0U;
#if _USE_FASTSEEK
			fp->cltbl = 0;						/* Normal seek mode */
#endif
			fp->fs = dj.fs;	 					/* Validate file object */
			fp->id = fp->fs->id;
		}
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL* fp, 		/* Pointer to the file object */
	void* buff,		/* Pointer to data buffer */
	UINT btr,		/* Number of bytes to read */
	UINT* br		/* Pointer to number of bytes read */
)
{
	FRESULT res;
	DWORD clst, sect, remain;
	UINT rcnt, cc;
	BYTE csect, *rbuff = (BYTE*)(void *)buff;


	*br = 0U;	/* Clear read byte counter */

	res = validate(fp);							/* Check validity */
	if (res != FR_OK) {
		LEAVE_FF(fp->fs, res);
	}
	if (fp->err != (BYTE)0U) {								/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	}
	if ((!(fp->flag & FA_READ)) != 0U) { 					/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	}
	remain = fp->fsize - fp->fptr;
	if (btr > (UINT)remain) {
		btr = (UINT)remain;		/* Truncate btr by remaining bytes */
	}
	while(btr != (UINT)0U) {								/* Repeat until all data read */

		if ((fp->fptr % SS(fp->fs)) == 0U) {		/* On the sector boundary? */
			csect = (BYTE)(((BYTE)((DWORD)fp->fptr / (DWORD)SS(fp->fs))) & (BYTE)(fp->fs->csize - (BYTE)1U));	/* Sector offset in the cluster */
			if ((!csect) != 0U) {						/* On the cluster boundary? */
				if (fp->fptr == 0U) {			/* On the top of the file? */
					clst = fp->sclust;			/* Follow from the origin */
				} else {						/* Middle or end of the file */
#if _USE_FASTSEEK
					if (fp->cltbl) {
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					}
					else
#endif
						clst = get_fat(fp->fs, fp->clust);	/* Follow cluster chain on the FAT */
				}
				if (clst < 2U) {
					ABORT(fp->fs, FR_INT_ERR);
				}
				if (clst == 0xFFFFFFFFU) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if ((!sect) != 0U) {
				ABORT(fp->fs, FR_INT_ERR);
			}
			sect += csect;
			cc = btr / SS(fp->fs);				/* When remaining bytes >= sector size, */
			if (cc != 0U) {							/* Read maximum contiguous sectors directly */
				if ((csect + cc) > fp->fs->csize) {	/* Clip at cluster boundary */
					cc = (UINT)(fp->fs->csize - csect);
				}
				if (disk_read(fp->fs->drv, rbuff, sect, (BYTE)cc) != RES_OK) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
#if !_FS_READONLY && _FS_MINIMIZE <= 2			/* Replace one of the read sectors with cached data if it contains a dirty sector */
#if _FS_TINY
				if ((fp->fs->wflag != 0U) && (((fp->fs->winsect - sect) < cc) != 0U)) {
					mem_cpy(rbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), fp->fs->win, SS(fp->fs));
				}
#else
				if (((fp->flag & FA__DIRTY) != (BYTE)0U) && ((fp->dsect - sect) < cc)) {
					mem_cpy(rbuff + ((fp->dsect - sect) * SS(fp->fs)), fp->buf, SS(fp->fs));
				}
#endif
#endif
				rcnt = SS(fp->fs) * cc;			/* Number of bytes transferred */
				rbuff += rcnt; fp->fptr += rcnt; *br += rcnt;btr -= rcnt;
				continue;
			}
#if !_FS_TINY
			if (fp->dsect != sect) {			/* Load data sector if not in cache */
#if !_FS_READONLY
				if ((fp->flag & FA__DIRTY) != (BYTE)0U) {		/* Write-back dirty sector cache */
					if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1U) != RES_OK) {
						ABORT(fp->fs, FR_DISK_ERR);
					}
					fp->flag &= ~FA__DIRTY;
				}
#endif
				if (disk_read(fp->fs->drv, fp->buf, sect, 1U) != RES_OK) {	/* Fill sector cache */
					ABORT(fp->fs, FR_DISK_ERR);
				}
			}
#endif
			fp->dsect = sect;
		}
		rcnt = (UINT)SS(fp->fs) - (UINT)(fp->fptr % SS(fp->fs));	/* Get partial sector data from sector buffer */
		if (rcnt > btr) {
			rcnt = btr;
		}
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect) != 0U) {		/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		}
		mem_cpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#else
		mem_cpy(rbuff, (void *)&fp->buf[fp->fptr % SS(fp->fs)], rcnt);	/* Pick partial sector */
#endif
		rbuff += rcnt; fp->fptr += rcnt; *br += rcnt;btr -= rcnt;
	}

	LEAVE_FF(fp->fs, FR_OK);
}




#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
	FIL* fp,			/* Pointer to the file object */
	const void *buff,	/* Pointer to the data to be written */
	UINT btw,			/* Number of bytes to write */
	UINT* bw			/* Pointer to number of bytes written */
)
{
	FRESULT res;
	DWORD clst, sect;
	UINT wcnt, cc;
	const BYTE *wbuff = buff;
	BYTE csect;
	UINT LocBtw = btw;

	*bw = 0U;	/* Clear write byte counter */

	res = validate(fp);						/* Check validity */
	if (res != FR_OK) {
		LEAVE_FF(fp->fs, res);
	}
	if (fp->err != (BYTE)0U) {							/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	}
	if ((!(fp->flag & FA_WRITE)) != 0U) {			/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	}
	if ((fp->fptr + LocBtw) < fp->fptr) {
		LocBtw = 0U;	/* File size cannot reach 4GB */
	}

	while(LocBtw != (UINT)0U) {							/* Repeat until all data written */

		if ((fp->fptr % SS(fp->fs)) == 0U) {	/* On the sector boundary? */
			csect = (BYTE)((fp->fptr / ((DWORD)SS(fp->fs))) & (DWORD)(fp->fs->csize - 1U));	/* Sector offset in the cluster */
			if ((!csect) != 0U) {					/* On the cluster boundary? */
				if (fp->fptr == 0U) {		/* On the top of the file? */
					clst = fp->sclust;		/* Follow from the origin */
					if (clst == 0U)	{		/* When no cluster is allocated, */
						clst = create_chain(fp->fs, 0U);	/* Create a new cluster chain */
					}
				} else {					/* Middle or end of the file */
#if _USE_FASTSEEK
					if (fp->cltbl)
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					else
#endif
						clst = create_chain(fp->fs, fp->clust);	/* Follow or stretch cluster chain on the FAT */
				}
				if (clst == 0U) {
					break;		/* Could not allocate a new cluster (disk full) */
				}
				if (clst == 1U) {
					ABORT(fp->fs, FR_INT_ERR);
				}
				if (clst == 0xFFFFFFFFU) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->clust = clst;			/* Update current cluster */
				if (fp->sclust == 0U) {
					fp->sclust = clst;	/* Set start cluster if the first write */
				}
			}
#if _FS_TINY
			if (fp->fs->winsect == fp->dsect && (sync_window(fp->fs) != 0U)) {	/* Write-back sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
			}
#else
			if ((fp->flag & FA__DIRTY) != (BYTE)0U) {		/* Write-back sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1U) != RES_OK) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->flag &= ~FA__DIRTY;
			}
#endif
			sect = clust2sect(fp->fs, fp->clust);	/* Get current sector */
			if ((!sect) != 0U) {
				ABORT(fp->fs, FR_INT_ERR);
			}
			sect += csect;
			cc = LocBtw / SS(fp->fs);			/* When remaining bytes >= sector size, */
			if (cc != 0U) {						/* Write maximum contiguous sectors directly */
				if ((csect + cc) > fp->fs->csize) {	/* Clip at cluster boundary */
					cc = (UINT)(fp->fs->csize - csect);
				}
				if (disk_write(fp->fs->drv, wbuff, sect, (BYTE)cc) != RES_OK) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
#if _FS_MINIMIZE <= 2
#if _FS_TINY
				if ((fp->fs->winsect - sect) < cc) {	/* Refill sector cache if it gets invalidated by the direct write */
					mem_cpy(fp->fs->win, wbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), SS(fp->fs));
					fp->fs->wflag = 0U;
				}
#else
				if ((fp->dsect - sect) < cc) { /* Refill sector cache if it gets invalidated by the direct write */
					mem_cpy(fp->buf, wbuff + ((fp->dsect - sect) * SS(fp->fs)), SS(fp->fs));
					fp->flag &= ~FA__DIRTY;
				}
#endif
#endif
				wcnt = SS(fp->fs) * cc;		/* Number of bytes transferred */
				wbuff += wcnt; fp->fptr += (DWORD)wcnt; *bw += wcnt; LocBtw -= wcnt;
				continue;
			}
#if _FS_TINY
			if (fp->fptr >= fp->fsize) {	/* Avoid silly cache filling at growing edge */
				if (sync_window(fp->fs) != 0U) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->fs->winsect = sect;
			}
#else
			if (fp->dsect != sect) {		/* Fill sector cache with file data */
				if (((fp->fptr < fp->fsize) != 0U) &&
					(disk_read(fp->fs->drv, fp->buf, sect, 1U) != RES_OK)) {
						ABORT(fp->fs, FR_DISK_ERR);
				}
			}
#endif
			fp->dsect = sect;
		}
		wcnt = (((UINT)SS(fp->fs)) - ((UINT)fp->fptr % SS(fp->fs)));/* Put partial sector into file I/O buffer */
		if (wcnt > LocBtw) {
			wcnt = LocBtw;
		}
#if _FS_TINY
		if (move_window(fp->fs, fp->dsect) != 0U) {	/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		}
		mem_cpy((void *)&fp->fs->win[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->fs->wflag = 1U;
#else
		mem_cpy((void *)&fp->buf[fp->fptr % SS(fp->fs)], wbuff, wcnt);	/* Fit partial sector */
		fp->flag |= FA__DIRTY;
#endif
		wbuff += wcnt; fp->fptr += (DWORD)wcnt; *bw += wcnt; LocBtw -= wcnt;
	}

	if (fp->fptr > fp->fsize) {
		fp->fsize = fp->fptr;	/* Update file size if needed */
	}
	fp->flag |= FA__WRITTEN;						/* Set file change flag */

	LEAVE_FF(fp->fs, FR_OK);
}




/*-----------------------------------------------------------------------*/
/* Synchronize the File                                                  */
/*-----------------------------------------------------------------------*/

FRESULT f_sync (
	FIL* fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD tm;
	BYTE *directory;


	res = validate(fp);					/* Check validity of the object */
	if (res == FR_OK) {
		if ((fp->flag & FA__WRITTEN) != 0U) {	/* Has the file been written? */
			/* Write-back dirty buffer */
#if !_FS_TINY
			if ((fp->flag & FA__DIRTY) != (BYTE)0U) {
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1U) != RES_OK) {
					LEAVE_FF(fp->fs, FR_DISK_ERR);
				}
				fp->flag &= ~FA__DIRTY;
			}
#endif
			/* Update the directory entry */
			res = move_window(fp->fs, fp->dir_sect);
			if (res == FR_OK) {
				directory = fp->dir_ptr;
				*(directory+DIR_Attr) |= AM_ARC;					/* Set archive bit */
				ST_DWORD(directory+DIR_FileSize, fp->fsize);		/* Update file size */
				st_clust(directory, fp->sclust);					/* Update start cluster */
				tm = get_fattime();							/* Update updated time */
				ST_DWORD(directory+DIR_WrtTime, tm);
				ST_WORD(directory+DIR_LstAccDate, 0);
				fp->flag &= ~FA__WRITTEN;
				fp->fs->wflag = 1U;
				res = sync_fs(fp->fs);
			}
		}
	}

	LEAVE_FF(fp->fs, res);
}

#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL *fp		/* Pointer to the file object to be closed */
)
{
	FRESULT res;


#if !_FS_READONLY
	res = f_sync(fp);					/* Flush cached data */
	if (res == FR_OK)
#endif
	{
		res = validate(fp);				/* Lock volume */
		if (res == FR_OK) {
#if _FS_REENTRANT
			FATFS *fs = fp->fs;
#endif
#if _FS_LOCK
			res = dec_lock(fp->lockid);	/* Decrement file open counter */
			if (res == FR_OK)
#endif
				fp->fs = NULL;				/* Invalidate file object */
#if _FS_REENTRANT
			unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
		}
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Change Current Directory or Current Drive, Get Current Directory      */
/*-----------------------------------------------------------------------*/

#if _FS_RPATH >= 1
#if _VOLUMES >= 2
FRESULT f_chdrive (
	const TCHAR* path		/* Drive number */
)
{
	int vol;


	vol = get_ldnumber(&path);
	if (vol < 0) {
		return FR_INVALID_DRIVE;
	}

	CurrVol = (BYTE)vol;

	return FR_OK;
}
#endif


FRESULT f_chdir (
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the path */
		FREE_BUF();
		if (res == FR_OK) {					/* Follow completed */
			if (!dj.dir) {
				dj.fs->cdir = dj.sclust;	/* Start directory itself */
			} else {
				if (dj.dir[DIR_Attr] & AM_DIR) {	/* Reached to the directory */
					dj.fs->cdir = ld_clust(dj.fs, dj.dir);
				}
				else {
					res = FR_NO_PATH;		/* Reached but a file */
				}
			}
		}
		if (res == FR_NO_FILE) {
			res = FR_NO_PATH;
		}
	}

	LEAVE_FF(dj.fs, res);
}


#if _FS_RPATH >= 2
FRESULT f_getcwd (
	TCHAR* buff,	/* Pointer to the directory path */
	UINT len		/* Size of path */
)
{
	FRESULT res;
	DIR dj;
	UINT i, n;
	DWORD ccl;
	TCHAR *tp;
	FILINFO fno;
	DEF_NAMEBUF;


	*buff = 0;
	/* Get logical drive number */
	res = find_volume(&dj.fs, (const TCHAR**)&buff, 0);	/* Get current volume */
	if (res == FR_OK) {
		INIT_BUF(dj);
		i = len;			/* Bottom of buffer (directory stack base) */
		dj.sclust = dj.fs->cdir;			/* Start to follow upper directory from current directory */
		while ((ccl = dj.sclust) != 0) {	/* Repeat while current directory is a sub-directory */
			res = dir_sdi(&dj, 1);			/* Get parent directory */
			if (res != FR_OK) {
				break;
			}
			res = dir_read(&dj, 0);
			if (res != FR_OK) {
				break;
			}
			dj.sclust = ld_clust(dj.fs, dj.dir);	/* Goto parent directory */
			res = dir_sdi(&dj, 0);
			if (res != FR_OK) {
				break;
			}
			do {							/* Find the entry links to the child directory */
				res = dir_read(&dj, 0);
				if (res != FR_OK) {
					break;
				}
				if (ccl == ld_clust(dj.fs, dj.dir)) {
					break;	/* Found the entry */
				}
				res = dir_next(&dj, 0);
			} while (res == FR_OK);
			if (res == FR_NO_FILE) {
				res = FR_INT_ERR;/* It cannot be 'not found'. */
			}
			if (res != FR_OK) {
				break;
			}
#if _USE_LFN
			fno.lfname = buff;
			fno.lfsize = i;
#endif
			get_fileinfo(&dj, &fno);		/* Get the directory name and push it to the buffer */
			tp = fno.fname;
#if _USE_LFN
			if (*buff) {
				tp = buff;
			}
#endif
			for (n = 0; tp[n]; n++) ;
			if (i < n + 3) {
				res = FR_NOT_ENOUGH_CORE; break;
			}
			while (n) buff[--i] = tp[--n];
			buff[--i] = '/';
		}
		tp = buff;
		if (res == FR_OK) {
#if _VOLUMES >= 2
			*tp++ = '0' + CurrVol;			/* Put drive number */
			*tp++ = ':';
#endif
			if (i == len) {					/* Root-directory */
				*tp++ = '/';
			} else {						/* Sub-directroy */
				do		/* Add stacked path str */
					*tp++ = buff[i++];
				while (i < len);
			}
		}
		*tp = 0;
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}
#endif /* _FS_RPATH >= 2 */
#endif /* _FS_RPATH >= 1 */



#if _FS_MINIMIZE <= 2
/*-----------------------------------------------------------------------*/
/* Seek File R/W Pointer                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_lseek (
	FIL* fp,		/* Pointer to the file object */
	DWORD ofs		/* File pointer from top of file */
)
{
	FRESULT res;
	DWORD LocOfs = ofs;

	res = validate(fp);					/* Check validity of the object */
	if (res != FR_OK) {
		LEAVE_FF(fp->fs, res);
	}
	if (fp->err != (BYTE)0U) {						/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	}

#if _USE_FASTSEEK
	if (fp->cltbl) {	/* Fast seek */
		DWORD cl, pcl, ncl, tcl, dsc, tlen, ulen, *tbl;

		if (ofs == CREATE_LINKMAP) {	/* Create CLMT */
			tbl = fp->cltbl;
			tlen = *tbl++; ulen = 2;	/* Given table size and required table size */
			cl = fp->sclust;			/* Top of the chain */
			if (cl) {
				do {
					/* Get a fragment */
					tcl = cl; ncl = 0; ulen += 2;	/* Top, length and used items */
					do {
						pcl = cl; ncl++;
						cl = get_fat(fp->fs, cl);
						if (cl <= 1) {
							ABORT(fp->fs, FR_INT_ERR);
						}
						if (cl == 0xFFFFFFFF) {
							ABORT(fp->fs, FR_DISK_ERR);
						}
					} while (cl == pcl + 1);
					if (ulen <= tlen) {		/* Store the length and top of the fragment */
						*tbl++ = ncl; *tbl++ = tcl;
					}
				} while (cl < fp->fs->n_fatent);	/* Repeat until end of chain */
			}
			*fp->cltbl = ulen;	/* Number of items used */
			if (ulen <= tlen) {
				*tbl = 0;		/* Terminate table */
			}
			else {
				res = FR_NOT_ENOUGH_CORE;	/* Given table size is smaller than required */
			}

		} else {						/* Fast seek */
			if (LocOfs > fp->fsize) {		/* Clip offset at the file size */
				LocOfs = fp->fsize;
			}
			fp->fptr = LocOfs;				/* Set file pointer */
			if (LocOfs) {
				fp->clust = clmt_clust(fp, ofs - 1);
				dsc = clust2sect(fp->fs, fp->clust);
				if ((!dsc) != 0U) {
					ABORT(fp->fs, FR_INT_ERR);
				}
				dsc += (LocOfs - 1) / SS(fp->fs) & (fp->fs->csize - 1);
				if (((fp->fptr % SS(fp->fs)) != 0U)&& ((dsc != fp->dsect) != 0U)) {	/* Refill sector cache if needed */
#if !_FS_TINY
#if !_FS_READONLY
					if (fp->flag & FA__DIRTY) {		/* Write-back dirty sector cache */
						if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1) != RES_OK) {
							ABORT(fp->fs, FR_DISK_ERR);
						}
						fp->flag &= ~FA__DIRTY;
					}
#endif
					if (disk_read(fp->fs->drv, fp->buf, dsc, 1) != RES_OK) {	/* Load current sector */
						ABORT(fp->fs, FR_DISK_ERR);
					}
#endif
					fp->dsect = dsc;
				}
			}
		}
	} else
#endif

	/* Normal Seek */
	{
		DWORD clst, bcs, nsect, ifptr;

		if (((LocOfs > fp->fsize) != 0U)					/* In read-only mode, clip offset with the file size */
#if !_FS_READONLY
			 && ((!(fp->flag & FA_WRITE)) != 0U)
#endif
			) {
				LocOfs = fp->fsize;
			}

		ifptr = fp->fptr;
		fp->fptr = 0U; nsect = 0U;
		if (LocOfs != (DWORD)0U) {
			bcs = (DWORD)fp->fs->csize * SS(fp->fs);	/* Cluster size (byte) */
			if ((ifptr > 0U) &&
				((((LocOfs - 1) / bcs) >= ((ifptr - 1) / bcs)) != 0U)) {	/* When seek to same or following cluster, */
				fp->fptr = (ifptr - 1) & ~(bcs - (DWORD)1);	/* start from the current cluster */
				LocOfs -= fp->fptr;
				clst = fp->clust;
			} else {									/* When seek to back cluster, */
				clst = fp->sclust;						/* start from the first cluster */
#if !_FS_READONLY
				if (clst == 0U) {						/* If no cluster chain, create a new chain */
					clst = create_chain(fp->fs, 0U);
					if (clst == 1U) {
						ABORT(fp->fs, FR_INT_ERR);
					}
					if (clst == 0xFFFFFFFFU) {
						ABORT(fp->fs, FR_DISK_ERR);
					}
					fp->sclust = clst;
				}
#endif
				fp->clust = clst;
			}
			if (clst != 0U) {
				while (LocOfs > bcs) {						/* Cluster following loop */
#if !_FS_READONLY
					if ((fp->flag & FA_WRITE) != (BYTE)0U) {			/* Check if in write mode or not */
						clst = create_chain(fp->fs, clst);	/* Force stretch if in write mode */
						if (clst == 0U) {				/* When disk gets full, clip file size */
							LocOfs = bcs; break;
						}
					} else
#endif
					{
						clst = get_fat(fp->fs, clst);	/* Follow cluster chain if not in write mode */
					}
					if (clst == 0xFFFFFFFFU) {
						ABORT(fp->fs, FR_DISK_ERR);
					}
					if ((clst <= 1U) || ((clst >= fp->fs->n_fatent) != 0U)) {
						ABORT(fp->fs, FR_INT_ERR);
					}
					fp->clust = clst;
					fp->fptr += bcs;
					LocOfs -= bcs;
				}
				fp->fptr += LocOfs;
				if ((LocOfs % SS(fp->fs)) != (UINT)0U) {
					nsect = clust2sect(fp->fs, clst);	/* Current sector */
					if ((!nsect) != 0U) {
						ABORT(fp->fs, FR_INT_ERR);
					}
					nsect += LocOfs / SS(fp->fs);
				}
			}
		}
		if (((fp->fptr % SS(fp->fs)) != 0U) && ((nsect != fp->dsect) != 0U)) {	/* Fill sector cache if needed */
#if !_FS_TINY
#if !_FS_READONLY
			if ((fp->flag & FA__DIRTY) != (BYTE)0U) {			/* Write-back dirty sector cache */
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1U) != RES_OK) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->flag &= ~FA__DIRTY;
			}
#endif
			if (disk_read(fp->fs->drv, fp->buf, nsect, 1U) != RES_OK) {	/* Fill sector cache */
				ABORT(fp->fs, FR_DISK_ERR);
			}
#endif
			fp->dsect = nsect;
		}
#if !_FS_READONLY
		if (fp->fptr > fp->fsize) {			/* Set file change flag if the file size is extended */
			fp->fsize = fp->fptr;
			fp->flag |= FA__WRITTEN;
		}
#endif
	}

	LEAVE_FF(fp->fs, res);
}



#if _FS_MINIMIZE <= 1
/*-----------------------------------------------------------------------*/
/* Create a Directory Object                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
	DIR* dp,			/* Pointer to directory object to create */
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	FATFS* fs;
	DEF_NAMEBUF;


	if ((!dp) != 0U) {
		res = FR_INVALID_OBJECT;
	}
	else {
	/* Get logical drive number */
	res = find_volume(&fs, &path, 0U);
	if (res == FR_OK) {
		dp->fs = fs;
		INIT_BUF(*dp);
		res = follow_path(dp, path);			/* Follow the path to the directory */
		FREE_BUF();
		if (res == FR_OK) {						/* Follow completed */
			if (dp->dir != NULL) {						/* It is not the origin directory itself */
				if ((dp->dir[DIR_Attr] & AM_DIR) != (BYTE)0U)	{/* The object is a sub directory */
					dp->sclust = ld_clust(fs, dp->dir);
				}
				else {							/* The object is a file */
					res = FR_NO_PATH;
				}
			}
			if (res == FR_OK) {
				dp->id = fs->id;
				res = dir_sdi(dp, 0U);			/* Rewind directory */
#if _FS_LOCK
				if (res == FR_OK) {
					if (dp->sclust) {
						dp->lockid = inc_lock(dp, 0);	/* Lock the sub directory */
						if (!dp->lockid)
							res = FR_TOO_MANY_OPEN_FILES;
					} else {
						dp->lockid = 0;	/* Root directory need not to be locked */
					}
				}
#endif
			}
		}
		if (res == FR_NO_FILE) {
			res = FR_NO_PATH;
		}
	}
	if (res != FR_OK) {
		dp->fs = NULL;		/* Invalidate the directory object if function faild */
	}

	}
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Close Directory                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_closedir (
	DIR *dp		/* Pointer to the directory object to be closed */
)
{
	FRESULT res;


	res = validate(dp);
	if (res == FR_OK) {
#if _FS_REENTRANT
		FATFS *fs = dp->fs;
#endif
#if _FS_LOCK
		if (dp->lockid) {				/* Decrement sub-directory open counter */
			res = dec_lock(dp->lockid);
		}
		if (res == FR_OK)
#endif
			dp->fs = NULL;				/* Invalidate directory object */
#if _FS_REENTRANT
		unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entries in Sequence                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
	DIR* dp,			/* Pointer to the open directory object */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DEF_NAMEBUF;


	res = validate(dp);						/* Check validity of the object */
	if (res == FR_OK) {
		if ((!fno) != 0U) {
			res = dir_sdi(dp, 0U);			/* Rewind the directory object */
		} else {
			INIT_BUF(*dp);
			res = dir_read(dp, 0);			/* Read an item */
			if (res == FR_NO_FILE) {		/* Reached end of directory */
				dp->sect = 0U;
				res = FR_OK;
			}
			if (res == FR_OK) {				/* A valid entry is found */
				get_fileinfo(dp, fno);		/* Get the object information */
				res = dir_next(dp, 0);		/* Increment index for next */
				if (res == FR_NO_FILE) {
					dp->sect = 0U;
					res = FR_OK;
				}
			}
			FREE_BUF();
		}
	}

	LEAVE_FF(dp->fs, res);
}



#if _FS_MINIMIZE == 0
/*-----------------------------------------------------------------------*/
/* Get File Status                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_stat (
	const TCHAR* path,	/* Pointer to the file path */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DIR dj = {0};
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0U);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		if (res == FR_OK) {				/* Follow completed */
			if (dj.dir != NULL) {		/* Found an object */
				if (fno != NULL) {
					get_fileinfo(&dj, fno);
				}
			} else {			/* It is root directory */
				res = FR_INVALID_NAME;
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}



#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Get Number of Free Clusters                                           */
/*-----------------------------------------------------------------------*/

FRESULT f_getfree (
	const TCHAR* path,	/* Path name of the logical drive number */
	DWORD* nclst,		/* Pointer to a variable to return number of free clusters */
	FATFS** fatfsys		/* Pointer to return pointer to corresponding file system object */
)
{
	FRESULT res;
	FATFS *fs;
	DWORD n, clst, sect, stat;
	UINT i;
	BYTE fat, *p;


	/* Get logical drive number */
	res = find_volume(fatfsys, &path, 0U);
	fs = *fatfsys;
	if (res == FR_OK) {
		/* If free_clust is valid, return it without full cluster scan */
		if (fs->free_clust <= (fs->n_fatent - 2)) {
			*nclst = fs->free_clust;
		} else {
			/* Get number of free clusters */
			fat = fs->fs_type;
			n = 0U;
			if (fat == FS_FAT12) {
				clst = (DWORD)2;
				do {
					stat = get_fat(fs, clst);
					if (stat == (DWORD)0xFFFFFFFFU) { res = FR_DISK_ERR; break; }
					if (stat == 1U) { res = FR_INT_ERR; break; }
					if (stat == 0U) { n++; }
					clst += (DWORD)1;
				} while (clst < fs->n_fatent);
			} else {
				clst = fs->n_fatent;
				sect = fs->fatbase;
				i = 0U; p = 0U;
				do {
					if (i == (UINT)0U) {
						res = move_window(fs, sect);
						sect += (DWORD)1;
						if (res != FR_OK) {
							break;
						}
						p = fs->win;
						i = SS(fs);
					}
					if (fat == FS_FAT16) {
						if (LD_WORD(p) == 0U) {
							n++;
						}
						p += 2; i -= (UINT)2;
					} else {
						if ((LD_DWORD(p) & 0x0FFFFFFFU) == 0U) {
							n++;
						}
						p += 4; i -= (UINT)4;
					}
					clst -= (DWORD)1;
				} while (clst != (DWORD)0U);
			}
			fs->free_clust = n;
			fs->fsi_flag |= 1U;
			*nclst = n;
		}
	}
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Truncate File                                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_truncate (
	FIL* fp		/* Pointer to the file object */
)
{
	FRESULT res;
	DWORD ncl;


	res = validate(fp);						/* Check validity of the object */
	if (res == FR_OK) {
		if (fp->err != (BYTE) 0U) {						/* Check error */
			res = (FRESULT)fp->err;
		} else {
			if ((!(fp->flag & FA_WRITE)) != 0U) {		/* Check access mode */
				res = FR_DENIED;
			}
		}
	}
	if (res == FR_OK) {
		if (fp->fsize > fp->fptr) {
			fp->fsize = fp->fptr;	/* Set file size to current R/W point */
			fp->flag |= FA__WRITTEN;
			if (fp->fptr == 0U) {	/* When set file size to zero, remove entire cluster chain */
				res = remove_chain(fp->fs, fp->sclust);
				fp->sclust = 0U;
			} else {				/* When truncate a part of the file, remove remaining clusters */
				ncl = get_fat(fp->fs, fp->clust);
				res = FR_OK;
				if (ncl == (DWORD)0xFFFFFFFFU) {
					res = FR_DISK_ERR;
				}
				if (ncl == 1U) {
					res = FR_INT_ERR;
				}
				if ((res == FR_OK) && ((ncl < fp->fs->n_fatent) != 0U)) {
					res = put_fat(fp->fs, fp->clust, 0x0FFFFFFFU);
					if (res == FR_OK) {
						res = remove_chain(fp->fs, ncl);
					}
				}
			}
#if !_FS_TINY
			if ((res == FR_OK) && ((fp->flag & FA__DIRTY) != (BYTE)0U)) {
				if (disk_write(fp->fs->drv, fp->buf, fp->dsect, 1U) != RES_OK) {
					res = FR_DISK_ERR;
				}
				else {
					fp->flag &= ~FA__DIRTY;
				}
			}
#endif
		}
		if (res != FR_OK) {
			fp->err = (FRESULT)res;
		}
	}

	LEAVE_FF(fp->fs, res);
}




/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
	const TCHAR* path		/* Pointer to the file or directory path */
)
{
	FRESULT res;
	DIR dj = {0}, sdj = {0};
	BYTE *dir;
	DWORD dclst;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1U);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the file path */
		if (_FS_RPATH && (res == FR_OK) && ((dj.fn[NS] & NS_DOT) != (BYTE)0U)) {
			res = FR_INVALID_NAME;			/* Cannot remove dot entry */
		}
#if _FS_LOCK
		if (res == FR_OK) {
			res = chk_lock(&dj, 2);	/* Cannot remove open file */
		}
#endif
		if (res == FR_OK) {					/* The object is accessible */
			dir = dj.dir;
			if ((!dir) != 0U) {
				res = FR_INVALID_NAME;		/* Cannot remove the start directory */
			} else {
				if (((*(dir+DIR_Attr)) & AM_RDO) != (BYTE)0U) {
					res = FR_DENIED;		/* Cannot remove R/O object */
				}
			}
			dclst = ld_clust(dj.fs, dir);
			if ((res == FR_OK) && (((*(dir+DIR_Attr)) & AM_DIR) != (BYTE)0U)) {	/* Is it a sub-dir? */
				if (dclst < 2U) {
					res = FR_INT_ERR;
				} else {
					mem_cpy(&sdj, &dj, sizeof (DIR));	/* Check if the sub-directory is empty or not */
					sdj.sclust = dclst;
					res = dir_sdi(&sdj, 2U);		/* Exclude dot entries */
					if (res == FR_OK) {
						res = dir_read(&sdj, 0);	/* Read an item */
						if (res == FR_OK		/* Not empty directory */
#if _FS_RPATH
						|| dclst == dj.fs->cdir	/* Current directory */
#endif
						) {
							res = FR_DENIED;
						}
						if (res == FR_NO_FILE) {
							res = FR_OK;	/* Empty */
						}
					}
				}
			}
			if (res == FR_OK) {
				res = dir_remove(&dj);		/* Remove the directory entry */
				if (res == FR_OK) {
					if (dclst != (DWORD)0U) {				/* Remove the cluster chain if exist */
						res = remove_chain(dj.fs, dclst);
					}
					if (res == FR_OK) {
						res = sync_fs(dj.fs);
					}
				}
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Create a Directory                                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_mkdir (
	const TCHAR* path		/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj = {0};
	BYTE *dir, n;
	DWORD dsc, dcl, pcl, tm = get_fattime();
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1U);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);			/* Follow the file path */
		if (res == FR_OK) {
			res = FR_EXIST;		/* Any object with same name is already existing */
		}
		if (_FS_RPATH && (res == FR_NO_FILE) && ((dj.fn[NS] & NS_DOT) != (BYTE)0U)) {
			res = FR_INVALID_NAME;
		}
		if (res == FR_NO_FILE) {				/* Can create a new directory */
			dcl = create_chain(dj.fs, 0U);		/* Allocate a cluster for the new directory table */
			res = FR_OK;
			if (dcl == 0U) {
				res = FR_DENIED;		/* No space to allocate a new cluster */
			}
			if (dcl == 1U) {
				res = FR_INT_ERR;
			}
			if (dcl == 0xFFFFFFFFU) {
				res = FR_DISK_ERR;
			}
			if (res == FR_OK) {					/* Flush FAT */
				res = sync_window(dj.fs);
			}
			if (res == FR_OK) {					/* Initialize the new directory table */
				dsc = clust2sect(dj.fs, dcl);
				dir = dj.fs->win;
				mem_set(dir, 0, SS(dj.fs));
				mem_set(dir, (s32)' ', 11U);	/* Create "." entry */
				(*(dir+DIR_Name)) = ((BYTE)'.');
				(*(dir+DIR_Attr)) = AM_DIR;
				ST_DWORD(dir+DIR_WrtTime, tm);
				st_clust(dir, dcl);
				mem_cpy(dir+SZ_DIR, dir, SZ_DIR); 	/* Create ".." entry */
				(*(dir+SZ_DIR+1U)) = ((BYTE)'.'); pcl = dj.sclust;
				if ((dj.fs->fs_type == FS_FAT32) && (pcl == dj.fs->dirbase)) {
					pcl = 0U;
				}
				st_clust(dir+SZ_DIR, pcl);
				n = dj.fs->csize;
				for (; n; n--) {	/* Write dot entries and clear following sectors */
					dj.fs->winsect = dsc;
					dsc += (DWORD)1;
					dj.fs->wflag = 1U;
					res = sync_window(dj.fs);
					if (res != FR_OK) {
						break;
					}
					mem_set(dir, 0, SS(dj.fs));
				}
			}
			if (res == FR_OK) {
				res = dir_register(&dj);	/* Register the object to the directoy */
			}
			if (res != FR_OK) {
				(void)remove_chain(dj.fs, dcl);			/* Could not register, remove cluster chain */
			} else {
				dir = dj.dir;
				(*(dir+DIR_Attr)) = AM_DIR;				/* Attribute */
				ST_DWORD(dir+DIR_WrtTime, tm);		/* Created time */
				st_clust(dir, dcl);					/* Table start cluster */
				dj.fs->wflag = 1U;
				res = sync_fs(dj.fs);
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Change Attribute                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_chmod (
	const TCHAR* path,	/* Pointer to the file path */
	BYTE value,			/* Attribute bits */
	BYTE mask			/* Attribute mask to change */
)
{
	FRESULT res;
	DIR dj = {0};
	BYTE *dir;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1U);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);		/* Follow the file path */
		FREE_BUF();
		if (_FS_RPATH && (res == FR_OK) && ((dj.fn[NS] & NS_DOT) != (BYTE)0U)) {
			res = FR_INVALID_NAME;
		}
		if (res == FR_OK) {
			dir = dj.dir;
			if ((!dir) != 0U) {						/* Is it a root directory? */
				res = FR_INVALID_NAME;
			} else {						/* File or sub directory */
				mask &= AM_RDO|AM_HID|AM_SYS|AM_ARC;	/* Valid attribute mask */
				(*(dir+DIR_Attr)) = (value & mask) | ((*(dir+DIR_Attr)) & (BYTE)~mask);	/* Apply attribute change */
				dj.fs->wflag = 1U;
				res = sync_fs(dj.fs);
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Change Timestamp                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_utime (
	const TCHAR* path,	/* Pointer to the file/directory name */
	const FILINFO* fno	/* Pointer to the time stamp to be set */
)
{
	FRESULT res;
	DIR dj = {0};
	BYTE *dir;
	DEF_NAMEBUF;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 1U);
	if (res == FR_OK) {
		INIT_BUF(dj);
		res = follow_path(&dj, path);	/* Follow the file path */
		FREE_BUF();
		if (_FS_RPATH && (res == FR_OK) && ((dj.fn[NS] & NS_DOT) != (BYTE)0U)) {
			res = FR_INVALID_NAME;
		}
		if (res == FR_OK) {
			dir = dj.dir;
			if ((!dir) != 0U) {					/* Root directory */
				res = FR_INVALID_NAME;
			} else {					/* File or sub-directory */
				ST_WORD(dir+DIR_WrtTime, fno->ftime);
				ST_WORD(dir+DIR_WrtDate, fno->fdate);
				dj.fs->wflag = 1U;
				res = sync_fs(dj.fs);
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}




/*-----------------------------------------------------------------------*/
/* Rename File/Directory                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
	const TCHAR* path_old,	/* Pointer to the object to be renamed */
	const TCHAR* path_new	/* Pointer to the new name */
)
{
	FRESULT res;
	DIR djo = {0}, djn = {0};
	BYTE buf[21] = {0};
	BYTE *dir;
	DWORD dw;
	DEF_NAMEBUF;


	/* Get logical drive number of the source object */
	res = find_volume(&djo.fs, &path_old, 1U);
	if (res == FR_OK) {
		djn.fs = djo.fs;
		INIT_BUF(djo);
		res = follow_path(&djo, path_old);		/* Check old object */
		if (_FS_RPATH && (res == FR_OK) && ((djo.fn[NS] & NS_DOT) != (BYTE)0U)) {
			res = FR_INVALID_NAME;
		}
#if _FS_LOCK
		if (res == FR_OK) {
			res = chk_lock(&djo, 2);
		}
#endif
		if (res == FR_OK) {						/* Old object is found */
			if (djo.dir == NULL) {						/* Is root dir? */
				res = FR_NO_FILE;
			} else {
				mem_cpy(buf, djo.dir+DIR_Attr, 21U);		/* Save the object information except name */
				mem_cpy(&djn, &djo, sizeof (DIR));		/* Duplicate the directory object */
				if (get_ldnumber(&path_new) >= 0) {		/* Snip drive number off and ignore it */
					res = follow_path(&djn, path_new);	/* and check if new object is exist */
				}
				else {
					res = FR_INVALID_DRIVE;
				}
				if (res == FR_OK) {
					res = FR_EXIST;		/* The new object name is already existing */
				}
				if (res == FR_NO_FILE) { 				/* Is it a valid path and no name collision? */
/* Start critical section that any interruption can cause a cross-link */
					res = dir_register(&djn);			/* Register the new entry */
					if (res == FR_OK) {
						dir = djn.dir;					/* Copy object information except name */
						mem_cpy(dir+13, buf+2, 19U);
						(*(dir+DIR_Attr)) = buf[0] | AM_ARC;
						djo.fs->wflag = 1U;
						if ((djo.sclust != djn.sclust) && (((*(dir+DIR_Attr)) & AM_DIR) != (BYTE)0U)) {		/* Update .. entry in the directory if needed */
							dw = clust2sect(djo.fs, ld_clust(djo.fs, dir));
							if ((!dw) != 0U) {
								res = FR_INT_ERR;
							} else {
								res = move_window(djo.fs, dw);
								dir = djo.fs->win+SZ_DIR;	/* .. entry */
								if ((res == FR_OK) && ((*(dir+1)) == ((BYTE)'.'))) {
									dw = ((djo.fs->fs_type == FS_FAT32) && (djn.sclust == djo.fs->dirbase)) ? ((DWORD)0) : djn.sclust;
									st_clust(dir, dw);
									djo.fs->wflag = 1U;
								}
							}
						}
						if (res == FR_OK) {
							res = dir_remove(&djo);		/* Remove old entry */
							if (res == FR_OK) {
								res = sync_fs(djo.fs);
							}
						}
					}
/* End critical section */
				}
			}
		}
		FREE_BUF();
	}

	LEAVE_FF(djo.fs, res);
}

#endif /* !_FS_READONLY */
#endif /* _FS_MINIMIZE == 0 */
#endif /* _FS_MINIMIZE <= 1 */
#endif /* _FS_MINIMIZE <= 2 */



#if _USE_LABEL
/*-----------------------------------------------------------------------*/
/* Get volume label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_getlabel (
	const TCHAR* path,	/* Path name of the logical drive number */
	TCHAR* label,		/* Pointer to a buffer to return the volume label */
	DWORD* vsn			/* Pointer to a variable to return the volume serial number */
)
{
	FRESULT res;
	DIR dj;
	UINT i, j;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &path, 0);

	/* Get volume label */
	if (res == FR_OK && label) {
		dj.sclust = 0;					/* Open root directory */
		res = dir_sdi(&dj, 0);
		if (res == FR_OK) {
			res = dir_read(&dj, 1);		/* Get an entry with AM_VOL */
			if (res == FR_OK) {			/* A volume label is exist */
#if _USE_LFN && _LFN_UNICODE
				WCHAR w;
				i = j = 0;
				do {
					w = (i < 11) ? dj.dir[i++] : ' ';
					if (IsDBCS1(w) && (i < 11) && IsDBCS2(dj.dir[i])) {
						w = w << 8 | dj.dir[i++];
					}
					label[j++] = ff_convert(w, 1);	/* OEM -> Unicode */
				} while (j < 11);
#else
				mem_cpy(label, dj.dir, 11);
#endif
				j = 11;
				do {
					label[j] = 0;
					if ((!j) != 0U) {
						break;
					}
				} while (label[--j] == ' ');
			}
			if (res == FR_NO_FILE) {	/* No label, return nul string */
				label[0] = 0;
				res = FR_OK;
			}
		}
	}

	/* Get volume serial number */
	if (res == FR_OK && vsn) {
		res = move_window(dj.fs, dj.fs->volbase);
		if (res == FR_OK) {
			i = dj.fs->fs_type == FS_FAT32 ? BS_VolID32 : BS_VolID;
			*vsn = LD_DWORD(&dj.fs->win[i]);
		}
	}

	LEAVE_FF(dj.fs, res);
}



#if !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Set volume label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_setlabel (
	const TCHAR* label	/* Pointer to the volume label to set */
)
{
	FRESULT res;
	DIR dj;
	BYTE vn[11];
	UINT i, j, sl;
	WCHAR w;
	DWORD tm;


	/* Get logical drive number */
	res = find_volume(&dj.fs, &label, 1);
	if (res) {
		LEAVE_FF(dj.fs, res);
	}

	/* Create a volume label in directory form */
	vn[0] = 0;
	for (sl = 0; label[sl]; sl++) ;				/* Get name length */
	for ( ; sl && label[sl-1] == ' '; sl--) ;	/* Remove trailing spaces */
	if (sl) {	/* Create volume label in directory form */
		i = j = 0;
		do {
#if _USE_LFN && _LFN_UNICODE
			w = ff_convert(ff_wtoupper(label[i++]), 0);
#else
			w = (BYTE)label[i++];
			if (IsDBCS1(w)) {
				w = ((j < 10) && (i < sl) && IsDBCS2(label[i])) ? w << 8 | (BYTE)label[i++] : 0;
			}
#if _USE_LFN
			w = ff_convert(ff_wtoupper(ff_convert(w, 1)), 0);
#else
			if (IsLower(w)) {
				w -= 0x20;			/* To upper ASCII characters */
			}
#ifdef _EXCVT
			if (w >= 0x80) {
				w = ExCvt[w - 0x80];	/* To upper extended characters (SBCS cfg) */
			}
#else
			if (((!DF1S) != 0U) && ((w >= 0x80) != 0U)) {
				w = 0;		/* Reject extended characters (ASCII cfg) */
			}
#endif
#endif
#endif
			if ((!w) || chk_chr("\"*+,.:;<=>\?[]|\x7F", w) || j >= (UINT)((w >= 0x100) ? 10 : 11)) {/* Reject invalid characters for volume label */
				LEAVE_FF(dj.fs, FR_INVALID_NAME);
			}
			if (w >= 0x100) {
				vn[j++] = (BYTE)(w >> 8);
			}
			vn[j++] = (BYTE)w;
		} while (i < sl);
		while (j < 11) vn[j++] = ' ';
	}

	/* Set volume label */
	dj.sclust = 0;					/* Open root directory */
	res = dir_sdi(&dj, 0);
	if (res == FR_OK) {
		res = dir_read(&dj, 1);		/* Get an entry with AM_VOL */
		if (res == FR_OK) {			/* A volume label is found */
			if (vn[0]) {
				mem_cpy(dj.dir, vn, 11);	/* Change the volume label name */
				tm = get_fattime();
				ST_DWORD(dj.dir+DIR_WrtTime, tm);
			} else {
				dj.dir[0] = DDE;			/* Remove the volume label */
			}
			dj.fs->wflag = 1;
			res = sync_fs(dj.fs);
		} else {					/* No volume label is found or error */
			if (res == FR_NO_FILE) {
				res = FR_OK;
				if (vn[0]) {				/* Create volume label as new */
					res = dir_alloc(&dj, 1);	/* Allocate an entry for volume label */
					if (res == FR_OK) {
						mem_set(dj.dir, 0, SZ_DIR);	/* Set volume label */
						mem_cpy(dj.dir, vn, 11);
						dj.dir[DIR_Attr] = AM_VOL;
						tm = get_fattime();
						ST_DWORD(dj.dir+DIR_WrtTime, tm);
						dj.fs->wflag = 1;
						res = sync_fs(dj.fs);
					}
				}
			}
		}
	}

	LEAVE_FF(dj.fs, res);
}

#endif /* !_FS_READONLY */
#endif /* _USE_LABEL */



/*-----------------------------------------------------------------------*/
/* Forward data to the stream directly (available on only tiny cfg)      */
/*-----------------------------------------------------------------------*/
#if _USE_FORWARD && _FS_TINY

FRESULT f_forward (
	FIL* fp, 						/* Pointer to the file object */
	UINT (*func)(const BYTE*,UINT),	/* Pointer to the streaming function */
	UINT btf,						/* Number of bytes to forward */
	UINT* bf						/* Pointer to number of bytes forwarded */
)
{
	FRESULT res;
	DWORD remain, clst, sect;
	UINT rcnt;
	BYTE csect;


	*bf = 0;	/* Clear transfer byte counter */

	res = validate(fp);								/* Check validity of the object */
	if (res != FR_OK) {
		LEAVE_FF(fp->fs, res);
	}
	if (fp->err) {									/* Check error */
		LEAVE_FF(fp->fs, (FRESULT)fp->err);
	}
	if ((!(fp->flag & FA_READ)) != 0U) {						/* Check access mode */
		LEAVE_FF(fp->fs, FR_DENIED);
	}

	remain = fp->fsize - fp->fptr;
	if (btf > remain) {
		btf = (UINT)remain;			/* Truncate btf by remaining bytes */
	}

	for ( ;  btf && (*func)(0, 0);					/* Repeat until all data transferred or stream becomes busy */
		fp->fptr += rcnt, *bf += rcnt, btf -= rcnt) {
		csect = (BYTE)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));	/* Sector offset in the cluster */
		if ((fp->fptr % SS(fp->fs)) == 0) {			/* On the sector boundary? */
			if (!csect) {							/* On the cluster boundary? */
				clst = (fp->fptr == 0) ?			/* On the top of the file? */
					fp->sclust : get_fat(fp->fs, fp->clust);
				if (clst <= 1) {
					ABORT(fp->fs, FR_INT_ERR);
				}
				if (clst == 0xFFFFFFFF) {
					ABORT(fp->fs, FR_DISK_ERR);
				}
				fp->clust = clst;					/* Update current cluster */
			}
		}
		sect = clust2sect(fp->fs, fp->clust);		/* Get current data sector */
		if ((!sect) != 0U) {
			ABORT(fp->fs, FR_INT_ERR);
		}
		sect += csect;
		if (move_window(fp->fs, sect)) {				/* Move sector window */
			ABORT(fp->fs, FR_DISK_ERR);
		}
		fp->dsect = sect;
		rcnt = SS(fp->fs) - (WORD)(fp->fptr % SS(fp->fs));	/* Forward data from sector window */
		if (rcnt > btf) {
			rcnt = btf;
		}
		rcnt = (*func)(&fp->fs->win[(WORD)fp->fptr % SS(fp->fs)], rcnt);
		if ((!rcnt) != 0U) {
			ABORT(fp->fs, FR_INT_ERR);
		}
	}

	LEAVE_FF(fp->fs, FR_OK);
}
#endif /* _USE_FORWARD */



#if _USE_MKFS && !_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Create File System on the Drive                                       */
/*-----------------------------------------------------------------------*/
#define N_ROOTDIR	512		/* Number of root directory entries for FAT12/16 */
#define N_FATS		1		/* Number of FAT copies (1 or 2) */


FRESULT f_mkfs (
	const TCHAR* path,	/* Logical drive number */
	BYTE sfd,			/* Partitioning rule 0:FDISK, 1:SFD */
	UINT au				/* Allocation unit [bytes] */
)
{
	static const WORD vst[] = { 1024,   512,  256,  128,   64,    32,   16,    8,    4,    2,   0};
	static const WORD cst[] = {32768, 16384, 8192, 4096, 2048, 16384, 8192, 4096, 2048, 1024, 512};
	int vol;
	BYTE fmt, md, sys, *tbl, pdrv, part;
	DWORD n_clst, vs, n, wsect;
	UINT i;
	DWORD b_vol, b_fat, b_dir, b_data;	/* LBA */
	DWORD n_vol, n_rsv, n_fat, n_dir;	/* Size */
	FATFS *fs;
	DSTATUS stat;


	/* Check mounted drive and clear work area */
	vol = get_ldnumber(&path);
	if (vol < 0) {
		return FR_INVALID_DRIVE;
	}
	if (sfd > 1) {
		return FR_INVALID_PARAMETER;
	}
	if (au & (au - 1)) {
		return FR_INVALID_PARAMETER;
	}
	fs = FatFs[vol];
	if ((!fs) != 0U) {
		return FR_NOT_ENABLED;
	}
	fs->fs_type = 0;
	pdrv = LD2PD(vol);	/* Physical drive */
	part = LD2PT(vol);	/* Partition (0:auto detect, 1-4:get from partition table)*/

	/* Get disk statics */
	stat = disk_initialize(pdrv);
	if (stat & STA_NOINIT) {
		return FR_NOT_READY;
	}
	if (stat & STA_PROTECT) {
		return FR_WRITE_PROTECTED;
	}
#if _MAX_SS != _MIN_SS		/* Get disk sector size */
	if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK || (SS(fs) > _MAX_SS) || (SS(fs) < _MIN_SS)) {
		return FR_DISK_ERR;
	}
#endif
	if (_MULTI_PARTITION && part) {
		/* Get partition information from partition table in the MBR */
		if (disk_read(pdrv, fs->win, 0, 1) != RES_OK) {
			return FR_DISK_ERR;
		}
		if (LD_WORD(fs->win+BS_55AA) != 0xAA55) {
			return FR_MKFS_ABORTED;
		}
		tbl = &fs->win[MBR_Table + (part - 1) * SZ_PTE];
		if ((!tbl[4]) != 0U) {
			return FR_MKFS_ABORTED;	/* No partition? */
		}
		b_vol = LD_DWORD(tbl+8);	/* Volume start sector */
		n_vol = LD_DWORD(tbl+12);	/* Volume size */
	} else {
		/* Create a partition in this function */
		if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &n_vol) != RES_OK || (n_vol < 128)) {
			return FR_DISK_ERR;
		}
		b_vol = (sfd) ? 0 : 63;		/* Volume start sector */
		n_vol -= b_vol;				/* Volume size */
	}

	if ((!au) != 0U) {				/* AU auto selection */
		vs = n_vol / (2000 / (SS(fs) / 512));
		for (i = 0; vs < vst[i]; i++) ;
		au = cst[i];
	}
	au /= SS(fs);		/* Number of sectors per cluster */
	if (au == 0) {
		au = 1;
	}
	if (au > 128) {
		au = 128;
	}

	/* Pre-compute number of clusters and FAT sub-type */
	n_clst = n_vol / au;
	fmt = FS_FAT12;
	if (n_clst >= MIN_FAT16) {
		fmt = FS_FAT16;
	}
	if (n_clst >= MIN_FAT32) {
		fmt = FS_FAT32;
	}

	/* Determine offset and size of FAT structure */
	if (fmt == FS_FAT32) {
		n_fat = ((n_clst * 4) + 8 + SS(fs) - 1) / SS(fs);
		n_rsv = 32;
		n_dir = 0;
	} else {
		n_fat = (fmt == FS_FAT12) ? (n_clst * 3 + 1) / 2 + 3 : (n_clst * 2) + 4;
		n_fat = (n_fat + SS(fs) - 1) / SS(fs);
		n_rsv = 1;
		n_dir = (DWORD)N_ROOTDIR * SZ_DIR / SS(fs);
	}
	b_fat = b_vol + n_rsv;				/* FAT area start sector */
	b_dir = b_fat + n_fat * N_FATS;		/* Directory area start sector */
	b_data = b_dir + n_dir;				/* Data area start sector */
	if (n_vol < b_data + au - b_vol) {
		return FR_MKFS_ABORTED;	/* Too small volume */
	}

	/* Align data start sector to erase block boundary (for flash memory media) */
	if (disk_ioctl(pdrv, GET_BLOCK_SIZE, &n) != RES_OK || (!n) || (n > 32768)) {
		n = 1;
	}
	n = (b_data + n - 1) & ~(n - 1);	/* Next nearest erase block from current data start */
	n = (n - b_data) / N_FATS;
	if (fmt == FS_FAT32) {		/* FAT32: Move FAT offset */
		n_rsv += n;
		b_fat += n;
	} else {					/* FAT12/16: Expand FAT size */
		n_fat += n;
	}

	/* Determine number of clusters and final check of validity of the FAT sub-type */
	n_clst = (n_vol - n_rsv - n_fat * N_FATS - n_dir) / au;
	if (   (fmt == FS_FAT16 && n_clst < MIN_FAT16)
		|| (fmt == FS_FAT32 && n_clst < MIN_FAT32)) {
		return FR_MKFS_ABORTED;
	}

	/* Determine system ID in the partition table */
	if (fmt == FS_FAT32) {
		sys = 0x0C;		/* FAT32X */
	} else {
		if (fmt == FS_FAT12 && n_vol < 0x10000) {
			sys = 0x01;	/* FAT12(<65536) */
		} else {
			sys = (n_vol < 0x10000) ? 0x04 : 0x06;	/* FAT16(<65536) : FAT12/16(>=65536) */
		}
	}

	if (_MULTI_PARTITION && part) {
		/* Update system ID in the partition table */
		tbl = &fs->win[MBR_Table + (part - 1) * SZ_PTE];
		tbl[4] = sys;
		if (disk_write(pdrv, fs->win, 0, 1) != 0U) {	/* Write it to teh MBR */
			return FR_DISK_ERR;
		}
		md = 0xF8;
	} else {
		if (sfd) {	/* No partition table (SFD) */
			md = 0xF0;
		} else {	/* Create partition table (FDISK) */
			mem_set(fs->win, 0, SS(fs));
			tbl = fs->win+MBR_Table;	/* Create partition table for single partition in the drive */
			tbl[1] = 1;						/* Partition start head */
			tbl[2] = 1;						/* Partition start sector */
			tbl[3] = 0;						/* Partition start cylinder */
			tbl[4] = sys;					/* System type */
			tbl[5] = 254;					/* Partition end head */
			n = (b_vol + n_vol) / 63 / 255;
			tbl[6] = (BYTE)(n >> 2 | 63);	/* Partition end sector */
			tbl[7] = (BYTE)n;				/* End cylinder */
			ST_DWORD(tbl+8, 63);			/* Partition start in LBA */
			ST_DWORD(tbl+12, n_vol);		/* Partition size in LBA */
			ST_WORD(fs->win+BS_55AA, 0xAA55);	/* MBR signature */
			if (disk_write(pdrv, fs->win, 0, 1) != RES_OK) {	/* Write it to the MBR */
				return FR_DISK_ERR;
			}
			md = 0xF8;
		}
	}

	/* Create BPB in the VBR */
	tbl = fs->win;							/* Clear sector */
	mem_set(tbl, 0, SS(fs));
	mem_cpy(tbl, "\xEB\xFE\x90" "MSDOS5.0", 11);/* Boot jump code, OEM name */
	i = SS(fs);								/* Sector size */
	ST_WORD(tbl+BPB_BytsPerSec, i);
	tbl[BPB_SecPerClus] = (BYTE)au;			/* Sectors per cluster */
	ST_WORD(tbl+BPB_RsvdSecCnt, n_rsv);		/* Reserved sectors */
	tbl[BPB_NumFATs] = N_FATS;				/* Number of FATs */
	i = (fmt == FS_FAT32) ? 0 : N_ROOTDIR;	/* Number of root directory entries */
	ST_WORD(tbl+BPB_RootEntCnt, i);
	if (n_vol < 0x10000) {					/* Number of total sectors */
		ST_WORD(tbl+BPB_TotSec16, n_vol);
	} else {
		ST_DWORD(tbl+BPB_TotSec32, n_vol);
	}
	tbl[BPB_Media] = md;					/* Media descriptor */
	ST_WORD(tbl+BPB_SecPerTrk, 63);			/* Number of sectors per track */
	ST_WORD(tbl+BPB_NumHeads, 255);			/* Number of heads */
	ST_DWORD(tbl+BPB_HiddSec, b_vol);		/* Hidden sectors */
	n = get_fattime();						/* Use current time as VSN */
	if (fmt == FS_FAT32) {
		ST_DWORD(tbl+BS_VolID32, n);		/* VSN */
		ST_DWORD(tbl+BPB_FATSz32, n_fat);	/* Number of sectors per FAT */
		ST_DWORD(tbl+BPB_RootClus, 2);		/* Root directory start cluster (2) */
		ST_WORD(tbl+BPB_FSInfo, 1);			/* FSINFO record offset (VBR+1) */
		ST_WORD(tbl+BPB_BkBootSec, 6);		/* Backup boot record offset (VBR+6) */
		tbl[BS_DrvNum32] = 0x80;			/* Drive number */
		tbl[BS_BootSig32] = 0x29;			/* Extended boot signature */
		mem_cpy(tbl+BS_VolLab32, "NO NAME    " "FAT32   ", 19);	/* Volume label, FAT signature */
	} else {
		ST_DWORD(tbl+BS_VolID, n);			/* VSN */
		ST_WORD(tbl+BPB_FATSz16, n_fat);	/* Number of sectors per FAT */
		tbl[BS_DrvNum] = 0x80;				/* Drive number */
		tbl[BS_BootSig] = 0x29;				/* Extended boot signature */
		mem_cpy(tbl+BS_VolLab, "NO NAME    " "FAT     ", 19);	/* Volume label, FAT signature */
	}
	ST_WORD(tbl+BS_55AA, 0xAA55);			/* Signature (Offset is fixed here regardless of sector size) */
	if (disk_write(pdrv, tbl, b_vol, 1) != RES_OK) {	/* Write it to the VBR sector */
		return FR_DISK_ERR;
	}
	if (fmt == FS_FAT32) {					/* Write backup VBR if needed (VBR+6) */
		disk_write(pdrv, tbl, b_vol + 6, 1);
	}

	/* Initialize FAT area */
	wsect = b_fat;
	for (i = 0; i < N_FATS; i++) {		/* Initialize each FAT copy */
		mem_set(tbl, 0, SS(fs));			/* 1st sector of the FAT  */
		n = md;								/* Media descriptor byte */
		if (fmt != FS_FAT32) {
			n |= (fmt == FS_FAT12) ? 0x00FFFF00 : 0xFFFFFF00;
			ST_DWORD(tbl+0, n);				/* Reserve cluster #0-1 (FAT12/16) */
		} else {
			n |= 0xFFFFFF00;
			ST_DWORD(tbl+0, n);				/* Reserve cluster #0-1 (FAT32) */
			ST_DWORD(tbl+4, 0xFFFFFFFF);
			ST_DWORD(tbl+8, 0x0FFFFFFF);	/* Reserve cluster #2 for root directory */
		}
		if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK) {
			return FR_DISK_ERR;
		}
		mem_set(tbl, 0, SS(fs));			/* Fill following FAT entries with zero */
		for (n = 1; n < n_fat; n++) {		/* This loop may take a time on FAT32 volume due to many single sector writes */
			if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK) {
				return FR_DISK_ERR;
			}
		}
	}

	/* Initialize root directory */
	i = (fmt == FS_FAT32) ? au : (UINT)n_dir;
	do {
		if (disk_write(pdrv, tbl, wsect++, 1) != RES_OK) {
			return FR_DISK_ERR;
		}
	} while (--i);

#if _USE_ERASE	/* Erase data area if needed */
	{
		DWORD eb[2];

		eb[0] = wsect; eb[1] = wsect + (n_clst - ((fmt == FS_FAT32) ? 1 : 0)) * au - 1;
		disk_ioctl(pdrv, CTRL_ERASE_SECTOR, eb);
	}
#endif

	/* Create FSINFO if needed */
	if (fmt == FS_FAT32) {
		ST_DWORD(tbl+FSI_LeadSig, 0x41615252);
		ST_DWORD(tbl+FSI_StrucSig, 0x61417272);
		ST_DWORD(tbl+FSI_Free_Count, n_clst - 1);	/* Number of free clusters */
		ST_DWORD(tbl+FSI_Nxt_Free, 2);				/* Last allocated cluster# */
		ST_WORD(tbl+BS_55AA, 0xAA55);
		disk_write(pdrv, tbl, b_vol + 1, 1);	/* Write original (VBR+1) */
		disk_write(pdrv, tbl, b_vol + 7, 1);	/* Write backup (VBR+7) */
	}

	return (disk_ioctl(pdrv, CTRL_SYNC, (void *)0) == RES_OK) ? FR_OK : FR_DISK_ERR;
}



#if _MULTI_PARTITION
/*-----------------------------------------------------------------------*/
/* Divide Physical Drive                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_fdisk (
	BYTE pdrv,			/* Physical drive number */
	const DWORD szt[],	/* Pointer to the size table for each partitions */
	void* work			/* Pointer to the working buffer */
)
{
	UINT i, n, sz_cyl, tot_cyl, b_cyl, e_cyl, p_cyl;
	BYTE s_hd, e_hd, *p, *buf = (BYTE*)work;
	DSTATUS stat;
	DWORD sz_disk, sz_part, s_part;


	stat = disk_initialize(pdrv);
	if (stat & STA_NOINIT) {
		return FR_NOT_READY;
	}
	if (stat & STA_PROTECT) {
		return FR_WRITE_PROTECTED;
	}
	if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_disk)) {
		return FR_DISK_ERR;
	}

	/* Determine CHS in the table regardless of the drive geometry */
	for (n = 16; n < 256 && sz_disk / n / 63 > 1024; n *= 2) ;
	if (n == 256) {
		n--;
	}
	e_hd = n - 1;
	sz_cyl = 63 * n;
	tot_cyl = sz_disk / sz_cyl;

	/* Create partition table */
	mem_set(buf, 0, _MAX_SS);
	p = buf + MBR_Table; b_cyl = 0;
	for (i = 0; i < 4; i++, p += SZ_PTE) {
		p_cyl = (szt[i] <= 100U) ? (DWORD)tot_cyl * szt[i] / 100 : szt[i] / sz_cyl;
		if (!p_cyl) continue;
		s_part = (DWORD)sz_cyl * b_cyl;
		sz_part = (DWORD)sz_cyl * p_cyl;
		if (i == 0) {	/* Exclude first track of cylinder 0 */
			s_hd = 1;
			s_part += 63; sz_part -= 63;
		} else {
			s_hd = 0;
		}
		e_cyl = b_cyl + p_cyl - 1;
		if (e_cyl >= tot_cyl) {
			return FR_INVALID_PARAMETER;
		}

		/* Set partition table */
		p[1] = s_hd;						/* Start head */
		p[2] = (BYTE)((b_cyl >> 2) + 1);	/* Start sector */
		p[3] = (BYTE)b_cyl;					/* Start cylinder */
		p[4] = 0x06;						/* System type (temporary setting) */
		p[5] = e_hd;						/* End head */
		p[6] = (BYTE)((e_cyl >> 2) + 63);	/* End sector */
		p[7] = (BYTE)e_cyl;					/* End cylinder */
		ST_DWORD(p + 8, s_part);			/* Start sector in LBA */
		ST_DWORD(p + 12, sz_part);			/* Partition size */

		/* Next partition */
		b_cyl += p_cyl;
	}
	ST_WORD(p, 0xAA55);

	/* Write it to the MBR */
	return (disk_write(pdrv, buf, 0, 1) || disk_ioctl(pdrv, CTRL_SYNC, 0)) ? FR_DISK_ERR : FR_OK;
}


#endif /* _MULTI_PARTITION */
#endif /* _USE_MKFS && !_FS_READONLY */




#if _USE_STRFUNC
/*-----------------------------------------------------------------------*/
/* Get a string from the file                                            */
/*-----------------------------------------------------------------------*/

TCHAR* f_gets (
	TCHAR* buff,	/* Pointer to the string buffer to read */
	s32 len,		/* Size of string buffer (characters) */
	FIL* fp			/* Pointer to the file object */
)
{
	s32 n = 0;
	TCHAR c, *p = buff;
	BYTE s[2];
	UINT rc;


	while (n < len - 1) {	/* Read characters until buffer gets filled */
#if _USE_LFN && _LFN_UNICODE
#if _STRF_ENCODE == 3		/* Read a character in UTF-8 */
		f_read(fp, s, 1, &rc);
		if (rc != 1) {
			break;
		}
		c = s[0];
		if (c >= 0x80) {
			if (c < 0xC0) {
				continue;	/* Skip stray trailer */
			}
			if (c < 0xE0) {			/* Two-byte sequence */
				f_read(fp, s, 1, &rc);
				if (rc != 1) {
					break;
				}
				c = ((c & 0x1F) << 6 )| (s[0] & 0x3F);
				if (c < 0x80) {
					c = '?';
				}
			} else {
				if (c < 0xF0) {		/* Three-byte sequence */
					f_read(fp, s, 2, &rc);
					if (rc != 2) {
						break;
					}
					c = (c << 12) | ((s[0] & 0x3F) << 6) | (s[1] & 0x3F);
					if (c < 0x800) {
						c = '?';
					}
				} else {			/* Reject four-byte sequence */
					c = '?';
				}
			}
		}
#elif _STRF_ENCODE == 2		/* Read a character in UTF-16BE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) {
			break;
		}
		c = s[1] + (s[0] << 8);
#elif _STRF_ENCODE == 1		/* Read a character in UTF-16LE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) {
			break;
		}
		c = s[0] + (s[1] << 8);
#else						/* Read a character in ANSI/OEM */
		f_read(fp, s, 1, &rc);
		if (rc != 1) {
			break;
		}
		c = s[0];
		if (IsDBCS1(c)) {
			f_read(fp, s, 1, &rc);
			if (rc != 1) break;
			c = (c << 8) + s[0];
		}
		c = ff_convert(c, 1);	/* OEM -> Unicode */
		if ((!c) != 0U) {
			c = '?';
		}
#endif
#else						/* Read a character without conversion */
		f_read(fp, s, 1, &rc);
		if (rc != 1) {
			break;
		}
		c = s[0];
#endif
		if ((_USE_STRFUNC == 2) && (c == '\r')){
			 continue;	/* Strip '\r' */
		}
		*p++ = c;
		n++;
		if (c == '\n') {
			break;		/* Break on EOL */
		}
	}
	*p = 0;
	return n ? buff : 0;			/* When no data read (eof or error), return with error. */
}



#if !_FS_READONLY
#include <stdarg.h>
/*-----------------------------------------------------------------------*/
/* Put a character to the file                                           */
/*-----------------------------------------------------------------------*/

typedef struct {
	FIL* fp;
	s32 idx, nchr;
	BYTE buf[64];
} putbuff;


static
void putc_bfd (
	putbuff* pb,
	TCHAR c
)
{
	UINT bw;
	s32 i;


	if ((_USE_STRFUNC == 2) && (c == '\n')) {	 /* LF -> CRLF conversion */
		putc_bfd(pb, '\r');
	}

	i = pb->idx;	/* Buffer write index (-1:error) */
	if (i < 0) {
		return;
	}

#if _USE_LFN && _LFN_UNICODE
#if _STRF_ENCODE == 3			/* Write a character in UTF-8 */
	if (c < 0x80) {				/* 7-bit */
		pb->buf[i++] = (BYTE)c;
	} else {
		if (c < 0x800) {		/* 11-bit */
			pb->buf[i++] = (BYTE)(0xC0 | c >> 6);
		} else {				/* 16-bit */
			pb->buf[i++] = (BYTE)(0xE0 | c >> 12);
			pb->buf[i++] = (BYTE)(0x80 | (c >> 6 & 0x3F));
		}
		pb->buf[i++] = (BYTE)(0x80 | (c & 0x3F));
	}
#elif _STRF_ENCODE == 2			/* Write a character in UTF-16BE */
	pb->buf[i++] = (BYTE)(c >> 8);
	pb->buf[i++] = (BYTE)c;
#elif _STRF_ENCODE == 1			/* Write a character in UTF-16LE */
	pb->buf[i++] = (BYTE)c;
	pb->buf[i++] = (BYTE)(c >> 8);
#else							/* Write a character in ANSI/OEM */
	c = ff_convert(c, 0);	/* Unicode -> OEM */
	if ((!c) != 0U) {
		c = '?';
	}
	if (c >= 0x100) {
		pb->buf[i++] = (BYTE)(c >> 8);
	}
	pb->buf[i++] = (BYTE)c;
#endif
#else							/* Write a character without conversion */
	pb->buf[i++] = (BYTE)c;
#endif

	if (i >= (int)(sizeof pb->buf) - 3) {	/* Write buffered characters to the file */
		f_write(pb->fp, pb->buf, (UINT)i, &bw);
		i = (bw == (UINT)i) ? 0 : -1;
	}
	pb->idx = i;
	pb->nchr++;
}



int f_putc (
	TCHAR c,	/* A character to be output */
	FIL* fp		/* Pointer to the file object */
)
{
	putbuff pb;
	UINT nw;


	pb.fp = fp;			/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	putc_bfd(&pb, c);	/* Put a character */

	if (   pb.idx >= 0	/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) {
			return pb.nchr;
	}
	return EOF;
}




/*-----------------------------------------------------------------------*/
/* Put a string to the file                                              */
/*-----------------------------------------------------------------------*/

int f_puts (
	const TCHAR* str,	/* Pointer to the string to be output */
	FIL* fp				/* Pointer to the file object */
)
{
	putbuff pb;
	UINT nw;


	pb.fp = fp;				/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	while (*str) {			/* Put the string */
		putc_bfd(&pb, *str++);
	}

	if (   pb.idx >= 0		/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) {
			return pb.nchr;
	}
	return EOF;
}




/*-----------------------------------------------------------------------*/
/* Put a formatted string to the file                                    */
/*-----------------------------------------------------------------------*/

int f_printf (
	FIL* fp,			/* Pointer to the file object */
	const TCHAR* fmt,	/* Pointer to the format string */
	...					/* Optional arguments... */
)
{
	va_list arp;
	BYTE f, r;
	UINT nw, i, j, w;
	DWORD v;
	TCHAR c, d, s[16], *p;
	putbuff pb;


	pb.fp = fp;				/* Initialize output buffer */
	pb.nchr = pb.idx = 0;

	va_start(arp, fmt);

	for (;;) {
		c = *fmt++;
		if (c == 0) {
			break;			/* End of string */
		}
		if (c != '%') {				/* Non escape character */
			putc_bfd(&pb, c);
			continue;
		}
		w = f = 0;
		c = *fmt++;
		if (c == '0') {				/* Flag: '0' padding */
			f = 1; c = *fmt++;
		} else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		while (IsDigit(c)) {		/* Precision */
			w = w * 10 + c - '0';
			c = *fmt++;
		}
		if ((c == 'l') || (c == 'L')) {	/* Prefix: Size is long int */
			f |= 4; c = *fmt++;
		}
		if ((!c) != 0U) {
			break;
		}
		d = c;
		if (IsLower(d)) {
			d -= 0x20;
		}
		switch (d) {				/* Type is... */
		case 'S' :					/* String */
			p = va_arg(arp, TCHAR*);
			for (j = 0; p[j]; j++) ;
			if (!(f & 2)) {
				while (j++ < w) putc_bfd(&pb, ' ');
			}
			while (*p) {
				putc_bfd(&pb, *p++);
			}
			while (j++ < w) {
				putc_bfd(&pb, ' ');
			}
			continue;
		case 'C' :					/* Character */
			putc_bfd(&pb, (TCHAR)va_arg(arp, int)); continue;
		case 'B' :					/* Binary */
			r = 2; break;
		case 'O' :					/* Octal */
			r = 8; break;
		case 'D' :					/* Signed decimal */
		case 'U' :					/* Unsigned decimal */
			r = 10; break;
		case 'X' :					/* Hexdecimal */
			r = 16; break;
		default:					/* Unknown type (pass-through) */
			putc_bfd(&pb, c); continue;
		}

		/* Get an argument and put it in numeral */
		v = (f & 4) ? (DWORD)va_arg(arp, long) : ((d == 'D') ? (DWORD)(long)va_arg(arp, int) : (DWORD)va_arg(arp, unsigned int));
		if (d == 'D' && (v & 0x80000000)) {
			v = 0 - v;
			f |= 8;
		}
		i = 0;
		do {
			d = (TCHAR)(v % r); v /= r;
			if (d > 9) {
				d += (c == 'x') ? 0x27 : 0x07;
			}
			s[i++] = d + '0';
		} while (v && i < sizeof s / sizeof s[0]);
		if (f & 8) {
			s[i++] = '-';
		}
		j = i; d = (f & 1) ? '0' : ' ';
		while (((!(f & 2)) != 0U) && ((j++ < w) != 0U)) {
			putc_bfd(&pb, d);
		}
		do putc_bfd(&pb, s[--i]); while (i);
		while (j++ < w) {
			putc_bfd(&pb, d);
		}
	}

	va_end(arp);

	if (   pb.idx >= 0		/* Flush buffered characters to the file */
		&& f_write(pb.fp, pb.buf, (UINT)pb.idx, &nw) == FR_OK
		&& (UINT)pb.idx == nw) {
			return pb.nchr;
	}
	return EOF;
}

#endif /* !_FS_READONLY */
#endif /* _USE_STRFUNC */

#endif /* (defined FILE_SYSTEM_INTERFACE_SD) || (defined FILE_SYSTEM_INTERFACE_RAM) */
