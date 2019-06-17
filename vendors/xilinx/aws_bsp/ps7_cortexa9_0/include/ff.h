/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.10b    (C)ChaN, 2014
/----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2014, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/

#ifndef FAT_FS
#define FAT_FS	8051	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "xil_types.h"
#include "integer.h"	/* Basic integer types */
#include "ffconf.h"		/* FatFs configuration options */

#if FAT_FS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif



/* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition resolution table */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index */

#else							/* Single partition configuration */
#define LD2PD(vol) (BYTE)((vol))	/* Each logical drive is bound to the same physical drive number */
#define LD2PT(vol) 0U			/* Find first valid partition or in SFD */

#endif



/* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 at non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef WCHAR TCHAR;
#define T(x) L ## x
#define TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define T(x) (x)
#define TEXT(x) (x)
#endif

#endif



/* File system object structure (FATFS) */

typedef struct {
	BYTE	fs_type;		/* FAT sub-type (0:Not mounted) */
	BYTE	drv;			/* Physical drive number */
	BYTE	csize;			/* Sectors per cluster (1,2,4...128) */
	BYTE	n_fats;			/* Number of FAT copies (1 or 2) */
	BYTE	wflag;			/* win[] flag (b0:dirty) */
	BYTE	fsi_flag;		/* FSINFO flags (b7:disabled, b0:dirty) */
	WORD	id;				/* File system mount ID */
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) */
#if _MAX_SS != _MIN_SS
	WORD	ssize;			/* Bytes per sector (512, 1024, 2048 or 4096) */
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
	DWORD	last_clust;		/* Last allocated cluster */
	DWORD	free_clust;		/* Number of free clusters */
#endif
#if _FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) */
#endif
	DWORD	n_fatent;		/* Number of FAT entries, = number of clusters + 2 */
	DWORD	fsize;			/* Sectors per FAT */
	DWORD	volbase;		/* Volume start sector */
	DWORD	fatbase;		/* FAT start sector */
	DWORD	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	DWORD	database;		/* Data start sector */
	DWORD	winsect;		/* Current sector appearing in the win[] */
#ifdef __ICCARM__
#pragma data_alignment = 32
	BYTE	win[_MAX_SS];
#pragma data_alignment = 4
#else
	BYTE	win[_MAX_SS] __attribute__ ((aligned(32)));	/* Disk access window for Directory, FAT (and Data on tiny cfg) */
#endif
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the related file system object (**do not change order**) */
	WORD	id;				/* Owner file system mount ID (**do not change order**) */
	BYTE	flag;			/* Status flags */
	BYTE	err;			/* Abort flag (error code) */
	DWORD	fptr;			/* File read/write pointer (Zeroed on file open) */
	DWORD	fsize;			/* File size */
	DWORD	sclust;			/* File start cluster (0:no cluster chain, always 0 when fsize is 0) */
	DWORD	clust;			/* Current cluster of fpter (not valid when fprt is 0) */
	DWORD	dsect;			/* Sector number appearing in buf[] (0:invalid) */
#if !_FS_READONLY
	DWORD	dir_sect;		/* Sector number containing the directory entry */
	BYTE*	dir_ptr;		/* Pointer to the directory entry in the win[] */
#endif
#if _USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (Nulled on file open) */
#endif
#if _FS_LOCK
	UINT	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
#if !_FS_TINY
#ifdef __ICCARM__
#pragma data_alignment = 32
	BYTE	buf[_MAX_SS];	/* File data read/write buffer */
#pragma data_alignment = 4
#else
	BYTE	buf[_MAX_SS] __attribute__ ((aligned(32)));	/* File data read/write buffer */
#endif
#endif
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object (**do not change order**) */
	WORD	id;				/* Owner file system mount ID (**do not change order**) */
	WORD	index;			/* Current read/write index number */
	DWORD	sclust;			/* Table start cluster (0:Root dir) */
	DWORD	clust;			/* Current cluster */
	DWORD	sect;			/* Current sector */
	BYTE*	dir;			/* Pointer to the current SFN entry in the win[] */
	BYTE*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _FS_LOCK
	UINT	lockid;			/* File lock ID (index of file semaphore table Files[]) */
#endif
#if _USE_LFN
	WCHAR*	lfn;			/* Pointer to the LFN working buffer */
	WORD	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} DIR;



/* File status structure (FILINFO) */

typedef struct {
	DWORD	fsize;			/* File size */
	WORD	fdate;			/* Last modified date */
	WORD	ftime;			/* Last modified time */
	BYTE	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0U,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
FRESULT f_close (FIL* fp);											/* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
#if _USE_FORWARD
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
#endif
FRESULT f_lseek (FIL* fp, DWORD ofs);								/* Move file pointer of a file object */
FRESULT f_sync (FIL* fp);											/* Flush cached data of a writing file */
#if _FS_MINIMIZE <= 2
#if _FS_MINIMIZE <= 1
FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory */
FRESULT f_closedir (DIR* dp);										/* Close an open directory */
FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item */
#if _FS_MINIMIZE == 0
FRESULT f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status */
#if !_FS_READONLY
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfsys);
FRESULT f_truncate (FIL* fp);										/* Truncate file */
FRESULT f_unlink (const TCHAR* path);							/* Delete an existing file or directory */
FRESULT f_mkdir (const TCHAR* path);								/* Create a sub directory */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
FRESULT f_chmod (const TCHAR* path, BYTE value, BYTE mask);			/* Change attribute of the file/dir */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir */
#endif
#endif
#endif
#endif
DWORD clust2sect (FATFS* fs, DWORD clst);
DWORD get_fat (	FATFS* fs, DWORD clst);
FRESULT put_fat (FATFS* fs, DWORD clst, DWORD val);
#if _FS_RPATH >= 1
#if _VOLUMES >= 2
FRESULT f_chdrive (const TCHAR* path);								/* Change current drive */
#endif
FRESULT f_chdir (const TCHAR* path);								/* Change current directory */
#if _FS_RPATH >= 2
FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory */
#endif
#endif
#if _USE_LABEL
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label */
#if !_FS_READONLY
FRESULT f_setlabel (const TCHAR* label);							/* Set volume label */
#endif
#endif
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
#if _USE_MKFS && !_FS_READONLY
FRESULT f_mkfs (const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume */
#if _MULTI_PARTITION
FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
#endif
#endif
#if _USE_STRFUNC
TCHAR* f_gets (TCHAR* buff, s32 len, FIL* fp);						/* Get a string from the file */
#if !_FS_READONLY
int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file */
int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
#endif
#endif
#define f_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define file_size(fp) ((fp)->fsize)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !_FS_READONLY
DWORD get_fattime (void);
#endif

/* Unicode support functions */
#if _USE_LFN							/* Unicode - OEM code conversion */
WCHAR ff_convert (WCHAR chr, UINT dir);	/* OEM-Unicode bidirectional conversion */
WCHAR ff_wtoupper (WCHAR chr);			/* Unicode upper-case conversion */
#if _USE_LFN == 3						/* Memory functions */
void* ff_memalloc (UINT msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#endif
#endif

/* Sync functions */
#if _FS_REENTRANT
int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (_SYNC_t sobj);				/* Lock sync object */
void ff_rel_grant (_SYNC_t sobj);				/* Unlock sync object */
int ff_del_syncobj (_SYNC_t sobj);				/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01U
#define	FA_OPEN_EXISTING	0x00U

#if !_FS_READONLY
#define	FA_WRITE			0x02U
#define	FA_CREATE_NEW		0x04U
#define	FA_CREATE_ALWAYS	0x08U
#define	FA_OPEN_ALWAYS		0x10U
#define FA__WRITTEN			0x20U
#define FA__DIRTY			0x40U
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1U
#define FS_FAT16	2U
#define FS_FAT32	3U


/* File attribute bits for directory entry */

#define	AM_RDO	0x01U	/* Read only */
#define	AM_HID	0x02U	/* Hidden */
#define	AM_SYS	0x04U	/* System */
#define	AM_VOL	0x08U	/* Volume label */
#define AM_LFN	0x0FU	/* LFN entry */
#define AM_DIR	0x10U	/* Directory */
#define AM_ARC	0x20U	/* Archive */
#define AM_MASK	0x3FU	/* Mask of defined bits */


/* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFFU



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	(*(WORD*)(BYTE*)(ptr))=(WORD)(val)
#define	ST_DWORD(ptr,val)	(*(DWORD*)(BYTE*)(ptr))=(DWORD)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(((WORD)*((BYTE*)(ptr)+1U)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		((DWORD)(((DWORD)*((BYTE*)(ptr)+3U)<<24)|((DWORD)*((BYTE*)(ptr)+2U)<<16)|((WORD)*((BYTE*)(ptr)+1U)<<8)|*(BYTE*)(ptr)))
#define	ST_WORD(ptr,val)	(*((BYTE*)((void *)(ptr))))=((BYTE)(val)); (*((BYTE *)((ptr)+1U)))=((BYTE)((val)>>8))
#define	ST_DWORD(ptr,val)	(*((BYTE*)((void *)(ptr))))=((BYTE)(val)); (*((BYTE*)(void *)((ptr)+1U)))=((BYTE)((DWORD)(val)>>8)); (*((BYTE*)(void *)((ptr)+2U)))=((BYTE)((DWORD)(val)>>16)); (*((BYTE*)(void *)((ptr)+3U)))=((BYTE)((DWORD)(val)>>24))
#endif

#ifdef __cplusplus
}
#endif

#endif /* FAT_FS */
