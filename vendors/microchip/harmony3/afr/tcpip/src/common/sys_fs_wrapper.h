/*******************************************************************************
  Header file for Harmony File System Wrapper for TCPIP

  File Name:
    sys_fs_wrapper.h

  Summary:
    Header file for file system wrapper functions

  Description:
    This file is a header.
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/








// DOM-IGNORE-END


#ifndef SYS_FS_WRAPPER_H
#define	SYS_FS_WRAPPER_H

#include "system/fs/src/sys_fs_local.h"

/* Enumeration for File operations*/
typedef enum
{
    /* UNMOUNT */
    FS_UNMOUNT		/*DOM-IGNORE-BEGIN*/ = (0x00u)	 /*DOM-IGNORE-END*/,

    /* OPEN */
    FS_OPEN,

    /*STAT */
    FS_STAT

} SYS_FS_WRAPPER_FUNCTIONS;

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_FileOpen_Wrapper(const char *fname,
                                 SYS_FS_FILE_OPEN_ATTRIBUTES attributes);

//******************************************************************************
/*Function:
  SYS_FS_RESULT SYS_FS_FileStat_Wrapper(const char *fname,
                                      SYS_FS_FSTAT *buf);

***************************************************************************/
SYS_FS_RESULT SYS_FS_FileStat_Wrapper(const char *fname,
                                      SYS_FS_FSTAT *buf);

//******************************************************************************
/*Function:
  SYS_FS_RESULT SYS_FS_Unmount_Wrapper(const char *mountName);

***************************************************************************/
SYS_FS_RESULT SYS_FS_Unmount_Wrapper(const char *mountName);

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_FileNameGet_Wrapper(SYS_FS_HANDLE handle, 
 *                                              uint8_t* cName, uint16_t wLen);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_FileNameGet_Wrapper(SYS_FS_HANDLE handle, uint8_t* cName, uint16_t wLen);

//******************************************************************************
/*Function:
  SYS_FS_HANDLE SYS_FS_DirOpen_Wrapper(const char *fname);

***************************************************************************/
SYS_FS_HANDLE SYS_FS_DirOpen_Wrapper(const char *fname);

//******************************************************************************
/*Function:
  SYS_FS_RESULT SYS_FS_FileDelete_Wrapper(const char *fname);

***************************************************************************/
SYS_FS_RESULT SYS_FS_FileDelete_Wrapper(const char *fname);

#endif	/* SYS_FS_WRAPPER_H */
