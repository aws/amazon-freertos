/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
 
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hal_file.h"

static T_HAL_FILE gFiles[MAX_FILE_NUM];


int32_t hal_file_init( void )
{
    int32_t       idx;
    T_HAL_FILE   *pfile;
        
    memset(&gFiles[0], 0, sizeof(T_HAL_FILE)*MAX_FILE_NUM);    
    
    for ( idx=0; idx<MAX_FILE_NUM; idx++)
    { 
        pfile = &gFiles[idx];
        pfile->faddr = FILE_BASE + MAX_FILE_SIZE*idx;        
        hal_flash_read(pfile->faddr, (uint8_t *)pfile, FILE_HEADER_SIZE);
    }
    
    return pdTRUE;
}


int32_t hal_file_find(char *pfName, int32_t *pfIdx)
{
    int32_t     idx;
    
    *pfIdx = -1;
       
    for ( idx=0; idx<MAX_FILE_NUM; idx++)
    { 
        if ( strcmp(gFiles[idx].fname, pfName) == 0 )
        {
            if ( pfIdx )
                *pfIdx = idx;
            return pdTRUE;
        }    
        else if ( (*pfIdx== -1) && ( (strlen(gFiles[idx].fname)==0) || (gFiles[idx].fname[0]==0xff)) )
        {
            if ( pfIdx )
                *pfIdx = idx; //Free File Index
        }         
    }
         
    return pdFALSE;
}


int32_t hal_file_read(char * pcFileName, uint8_t *pucData, uint32_t *pulDataSize )
{
    int32_t              ret;
    int32_t              fIdx;
    hal_flash_status_t   status;
    
    ret = hal_file_find(pcFileName, &fIdx);
    if ( ret == pdFALSE )
    {
        return pdFALSE;
    }    
        
    status = hal_flash_read(gFiles[fIdx].faddr+FILE_HEADER_SIZE, pucData, gFiles[fIdx].fsize);
    if (status != HAL_FLASH_STATUS_OK) 
    {
        return pdFALSE;
    }
    
    *pulDataSize = gFiles[fIdx].fsize;
                
    return pdTRUE;
}


int32_t hal_file_write(char * pcFileName, uint8_t * pucData, uint32_t ulDataSize )
{
    int32_t              ret;
    int32_t              fIdx;
    hal_flash_status_t   status;
    
    ret = hal_file_find(pcFileName, &fIdx);
    if (ret==pdFALSE) 
    {
        if (fIdx==-1)
        {    
            return pdFALSE;
        }
        strcpy(gFiles[fIdx].fname,  pcFileName);
    }    

    gFiles[fIdx].fsize = ulDataSize;
    hal_flash_erase(gFiles[fIdx].faddr, HAL_FLASH_BLOCK_4K);
    status = hal_flash_write(gFiles[fIdx].faddr, (uint8_t *)&gFiles[fIdx], FILE_HEADER_SIZE);
    if (status != HAL_FLASH_STATUS_OK) {
        return pdFALSE;
    }
    
    status = hal_flash_write(gFiles[fIdx].faddr+FILE_HEADER_SIZE, pucData, ulDataSize);
    if (status != HAL_FLASH_STATUS_OK) {
        return pdFALSE;
    }
                
    return pdTRUE;
}


