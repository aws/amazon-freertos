/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       SDFatFS.h
 *
 *  @brief      File Allocation Table File System (FATFS) Driver
 *
 *  The SDFatFS header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/SDFatFS.h>
 *  #include <ti/drivers/SD.h>
 *  @endcode
 *
 *  @anchor ti_drivers_SDFatFS_Overview
 *  # Overview #
 *
 *  The SDFatFS driver is designed to hook into FatFs by implementing a
 *  set of functions that FatFs needs to call to perform basic block data
 *  transfers. This driver makes use of the SD driver for lower level disk IO
 *  operations.
 *
 *  The only functions that should be called by the application are the
 *  standard driver framework functions (_open, _close, etc...).
 *
 *  The application may use the FatFs APIs or the standard C
 *  runtime file I/O calls (fopen, fclose, etc...) given that SDFatFS_open has
 *  has been successfully called. After the SDFatFS_close API is called,
 *  ensure the application does NOT make any file I/O calls.
 *
 *  ## Opening the driver #
 *
 *  @code
 *  SDFatFS_Handle handle;
 *
 *  handle = SDFatFS_open(CONFIG_SDFatFS0, driveNum, NULL);
 *  if (handle == NULL) {
 *      //Error opening SDFatFS driver
 *      while (1);
 *  }
 *  @endcode
 *
 *  # Instrumentation #
 *
 *  The SDFatFS driver interface produces log statements if
 *  instrumentation is enabled.
 *
 *  Diagnostics Mask | Log details                   |
 *  ---------------- | -----------                   |
 *  Diags_USER1      | basic operations performed    |
 *  Diags_USER2      | detailed operations performed |
 *  ============================================================================
 */

#ifndef ti_drivers_SDFatFS__include
#define ti_drivers_SDFatFS__include

#include <stdint.h>

#include <ti/drivers/SD.h>

#include <third_party/fatfs/ff.h>
#include <third_party/fatfs/diskio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief SDFatFS Object
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    uint_fast32_t driveNum;
    DSTATUS       diskState;
    FATFS         filesystem; /* FATFS data object */
    SD_Handle     sdHandle;
} SDFatFS_Object;

/*!
 *  @brief SDFatFS Global configuration
 *
 *  The #SDFatFS_Config structure contains a single pointer used to characterize
 *  the SDFatFS driver implementation.
 *
 *  This structure needs to be defined before calling SDFatFS_init() and it must
 *  not be changed thereafter.
 *
 *  @sa SDFatFS_init()
 */
typedef struct {
    /*! Pointer to a SDFatFS object */
    SDFatFS_Object *object;
} SDFatFS_Config;


/*!
 *  @brief A handle that is returned from a SDFatFS_open() call.
 */
typedef SDFatFS_Config *SDFatFS_Handle;

/*!
 *  @brief Function to open a SDFatFS instance on the specified drive.
 *
 *  Function to mount the FatFs filesystem and register the SDFatFS disk
 *  I/O functions with the FatFS module.
 *
 *  @param idx Logical peripheral number indexed into the HWAttrs
 *             table.
 *  @param drive Drive Number
 */
extern SDFatFS_Handle SDFatFS_open(uint_least8_t idx, uint_least8_t drive);

/*!
 *  @brief Function to close a SDFatFS instance specified by the SDFatFS
 *         handle.
 *
 *         This function unmounts the file system mounted by SDFatFS_open() and
 *         unregisters the SDFatFS driver from the FatFs module.
 *
 *  @pre SDFatFS_open() had to be called first.
 *
 *  @param handle A #SDFatFS_Handle returned from SDFatFS_open()
 *
 *  @sa SDFatFS_open()
 */
extern void SDFatFS_close(SDFatFS_Handle handle);

/*!
 *  Function to initialize a SDFatFS instance
 */
extern void SDFatFS_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_SDFatFS__include */
