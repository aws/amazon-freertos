/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * ======== diskio.c ========
 * Generic definition of all disk APIs.  This should be shared amongst all media
 *
 */
#include <stddef.h>
#include <stdint.h>

#include "ffconf.h"
#include "diskio.h"

static diskio_fxns drive_fxn_table[FF_VOLUMES] = {
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

extern int32_t fatfs_getFatTime(void);

/*
 * ======== disk_register ========
 */
DRESULT disk_register(
        BYTE drive,
        DSTATUS (*d_init)  (BYTE drive),
        DSTATUS (*d_status)(BYTE drive),
        DRESULT (*d_read)  (BYTE drive, BYTE *buf, DWORD sector, UINT num),
        DRESULT (*d_write) (BYTE drive, const BYTE *buf, DWORD sector, UINT num),
        DRESULT (*d_ioctl) (BYTE drive, BYTE cmd, void * buf))
{

    if (drive >= FF_VOLUMES) {
        return RES_PARERR;
    }

    drive_fxn_table[drive].d_init   = d_init;
    drive_fxn_table[drive].d_status = d_status;
    drive_fxn_table[drive].d_read   = d_read;
    drive_fxn_table[drive].d_write  = d_write;
    drive_fxn_table[drive].d_ioctl  = d_ioctl;

    return RES_OK;
}

/*
 * ======== disk_unregister ========
 */
DRESULT disk_unregister(BYTE drive)
{

    if (drive >= FF_VOLUMES) {
        return RES_PARERR;
    }

    drive_fxn_table[drive].d_init   = NULL;
    drive_fxn_table[drive].d_status = NULL;
    drive_fxn_table[drive].d_read   = NULL;
    drive_fxn_table[drive].d_write  = NULL;
    drive_fxn_table[drive].d_ioctl  = NULL;

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    if (drive_fxn_table[pdrv].d_status == NULL) {
        return RES_PARERR;
    }
    else {
        /* call registered status function */
        return ((*(drive_fxn_table[pdrv].d_status)) (pdrv));
    }
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    if (drive_fxn_table[pdrv].d_init == NULL) {
        return RES_PARERR;
    }
    else {
        /* call registered init function */
        return ((*(drive_fxn_table[pdrv].d_init)) (pdrv));
    }
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    if (drive_fxn_table[pdrv].d_read == NULL) {
        return RES_PARERR;
    }
    else {
        /* call registered read function */
        return ((*(drive_fxn_table[pdrv].d_read)) (pdrv, buff, sector, count));
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    if (drive_fxn_table[pdrv].d_write == NULL) {
        return RES_PARERR;
    }
    else {
        /* call registered write function */
        return ((*(drive_fxn_table[pdrv].d_write)) (pdrv, buff, sector, count));
    }
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    if (drive_fxn_table[pdrv].d_ioctl == NULL) {
        return RES_PARERR;
    }
    else {
        /* call registered write function */
        return ((*(drive_fxn_table[pdrv].d_ioctl)) (pdrv, cmd, buff));
    }
}

/*
 * ======== get_fattime ========
 */
DWORD get_fattime(void)
{
    /* call TI dummy implementation or user defined hook function */
    return (fatfs_getFatTime());
}
