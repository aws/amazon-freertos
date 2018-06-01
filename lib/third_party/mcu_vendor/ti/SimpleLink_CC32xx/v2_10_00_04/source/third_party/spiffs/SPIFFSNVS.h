/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       SPIFFSNVS.h
 *  @brief      Module to enable the SPI Flash File System (SPIFFS) on the
 *              TI NVS driver
 *
 *  The SPIFFSNVS header file should be included in an application as follows:
 *  @code
 *  #include <third_party/spiffs/SPIFFSNVS.h>
 *  @endcode
 *
 *  SPIFFSNVS is designed to provide interface functions required by the SPIFFS
 *  to read/write/erase NOR flash memory.  These functions are implemented using
 *  the TI NVS driver.  By leveraging the NVS driver, application code using
 *  SPIFFS is not tied to a physical memory type (like SPI flash).  The NVS
 *  driver abstracts the physical memory interface so SPIFFS application code
 *  can run on any NVS driver instance.  Changing between internal flash or
 *  external SPI flash can be done by having SPIFFS use a different NVS driver
 *  instance.  SPIFFS will operate within the memory region which has been
 *  allocated for the NVS driver instance.
 *
 *  It is highly recommended that you read SPIFFS design and usage documentation
 *  before using SPIFFS.  Documentation available here:
 *      https://github.com/pellepl/spiffs
 *
 *  ## Using SPIFFS and SPIFFSNVS #
 *
 *  To mount a SPIFFS file system some configuration parameters and RAM must be
 *  provided at runtime (all sizes in bytes):
 *      * physical block size (also known as sector size)
 *      * amount of memory allocated for SPIFFS
 *      * logical block size
 *      * logical page size
 *      * A RAM work buffer
 *      * A RAM file descriptor cache
 *      * A RAM read/write cache
 *
 *  NVS drivers are aware of the physical block size and the amount of memory
 *  allocated (values are set in driver configuration; see board files).  The
 *  logical block size must be an integer multiple of the physical block size
 *  (a.k.a. sector size):
 *          logicalBlockSize = n * physicalBlockSize
 *
 *  @note It is recommended to set logical block size equal to the physical
 *  block size when starting with SPIFFS.  This can be changed later when
 *  optimizing the file system for your application.
 *
 *  The logical block size must also be an integer multiple of the logical page
 *  size:  logicalBlockSize = i * logicalPageSize
 *
 *  A statically allocated RAM work buffer must be provided.  This buffer
 *  must be (2 * logicalPageSize) in length.  A statically allocated RAM
 *  file descriptor cache must also be provided. File descriptors are 44 bytes
 *  for the default SPIFFS configuration.  The cache must be large enough to
 *  store as many file descriptors as desired.  Start with 4 file descriptors
 *  and modify when optimizing for your application.  Finally, SPIFFS requires
 *  a read/write cache be provided.  Start with a (2 * logicalPageSize) size
 *  cache; this can be increased or reduced later.
 *
 *  As an example assume that we want to use SPIFFS with a NVS driver instance
 *  that has 128k of memory and the the physical block size is 4096 bytes. In
 *  this case the logical block size can be set to 8192 bytes (16 logical
 *  blocks). Now we can set the logical page size to 256 bytes (32 logical pages
 *  per logical block).
 *
 *  @note SPIFFS always keeps 2 logical blocks free; in the example above there
 *  would only be 14 logical blocks available for storage (16k is unusable). The
 *  logical block size can be reduced to have more space for data. The logical
 *  block size and logical page size should be changed to optimize the file
 *  system for your application.
 *
 *  Knowing the logical block and logical page sizes desired; SPIFFSNVS_config()
 *  must be used to initialize the #spiffs_config #spiffs structures.
 *  SPIFFSNVS_config() requires a #SPIFFSNVS_Data object be provided.  This
 *  object is used by SPIFFSNVS when reading/writing to flash memory.  It also
 *  stores references to OS objects used for thread safety.  Each SPIFFS file
 *  system instance must have it's own #SPIFFSNVS_Data object and these objects
 *  must reside in persistent memory (not on a task stack or memory lost during
 *  low power modes).  The #spiffs.user_data field is used to store a pointer to
 *  its respective #SPIFFSNVS_Data object.  Users must not change #spiffs_config
 *  and #spiffs structures after SPIFFSNVS_config() has been called.
 *
 *  @code
 *  #define SPIFFS_LOGICAL_BLOCK_SIZE    (4096)
 *  #define SPIFFS_LOGICAL_PAGE_SIZE     (128)
 *  #define SPIFFS_FILE_DESCRIPTOR_SIZE  (44)
 *
 *  static uint8_t spiffsWorkBuffer[SPIFFS_LOGICAL_PAGE_SIZE * 2];
 *  static uint8_t spiffsFileDescriptorCache[SPIFFS_FILE_DESCRIPTOR_SIZE * 4];
 *  static uint8_t spiffsReadWriteCache[SPIFFS_LOGICAL_PAGE_SIZE * 2];
 *
 *  spiffs           fs;
 *  spiffs_config    fsConfig;
 *  SPIFFSNVS_Object spiffsnvs;
 *
 *  status = SPIFFSNVS_config(&spiffsnvs, Board_NVSINTERNAL, &fs, &fsConfig,
 *      SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
 *   if (status != SPIFFSNVS_STATUS_SUCCESS) {
 *       // Handle error condition...
 *   }
 *
 *  status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
 *      spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
 *      spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
 *  @endcode
 *
 *  ## Logical block and page size restrictions on CC13XX and CC26XX devices #
 *
 * Flash memory on CC13XX and CC26XX devices is divided into rows of 128 or
 * 256 bytes (refer to datasheet for exact size).  These rows can support up to
 * 83 write operations before suffering effects of row disturb in which data
 * can be corrupted.  Setting the logical page size very small or setting
 * the logical block size too large can lead to many logical pages in a logical
 * block.  Normal use and updates to pages can cause more than 83 write
 * operations on a logical block's index page (first page in the logical block).
 * The following conditions must be followed to prevent exceeding the 83 write
 * limit:
 *
 *   * Select a logical page size that is equal to or an integer multiple of
 *     the device's physical row size (i.e. 128 or 256).  Next select a logical
 *     block size to ensure the amount of logical pages in a logical block
 *     does not exceed 32:
 *         (logicalBlockSize / logicalPageSize <= 32).
 *
 * It is the user's responsibility to make sure the logical page size is equal
 * to or an integer multiple of the physical row size.  SPIFFSNVS_init() will
 * verify and return #SPIFFSNVS_STATUS_INV_PAGE_SIZE if the amount of logical
 * pages in a logical block is > 32.
 *
 *******************************************************************************
 */

#ifndef THIRD_PARTY_SPIFFS_SPIFFSNVS_H_
#define THIRD_PARTY_SPIFFS_SPIFFSNVS_H_

#include <stdint.h>

#include "spiffs_config.h"
#include "spiffs.h"

#include <ti/drivers/NVS.h>
#include <ti/drivers/dpl/MutexP.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup SPIFFSNVS_STATUS SPIFFSNVS status codes
 *  @{
 */

/*!
 *  @brief   Successful status code returned by:
 *  SPIFFSNVS_config()
 *
 *  Returned if the API was executed successfully.
 */
#define SPIFFSNVS_STATUS_SUCCESS           (0)

/*!
 *  @brief   Generic error status code returned by:
 *  SPIFFSNVS_config()
 *
 *  Returned if the API was not executed successfully.
 */
#define SPIFFSNVS_STATUS_ERROR             (-1)

/*!
 *  @brief An error status code returned by SPIFFSNVS_config()
 *
 *  SPIFFSNVS_config() will return this value if the NVS index is invalid.
 */
#define SPIFFSNVS_STATUS_INV_NVS_IDX       (-2)

/*!
 *  @brief An error status code returned by SPIFFSNVS_config()
 *
 *  Error status code returned if the logicalBlockSize argument is not
 *  an integer multiple of the physical block size.
 *  (e.g., logicalBlockSize = n * physicalBlockSize; where n is a positive
 *  integer)
 */
#define SPIFFSNVS_STATUS_INV_BLOCK_SIZE    (-3)

/*!
 *  @brief An error status code returned by SPIFFSNVS_config()
 *
 *  Error status code returned if the logicalBlockSize argument is not
 *  an integer multiple of the logicalPageSize argument.
 *  (e.g., logicalBlockSize = n * logicalPageSize; where n is a positive
 *  integer)
 */
#define SPIFFSNVS_STATUS_INV_PAGE_SIZE     (-4)

/** @} end SPIFFSNVS_STATUS group */

/*!
 *  @brief    SPIFFSNVS Lock
 *
 *  Lock mechanism to ensure thread safety.
 */
typedef struct SPIFFSNVS_Lock_ {
    MutexP_Handle mutex;      /*!< Handle to mutex for thread-safety */
    uintptr_t     keys[2];    /*!< Key store */
    u32_t         count;      /*!< Maintains count of times lock is acquired */
} SPIFFSNVS_Lock;

/*!
  *  @brief    SPIFFSNVS data object
  *
  *  Memory required to store the NVS handle used to read/write memory and
  *  the locking mechanism used to provide thread safety to SPIFFS.  Each
  *  SPIFFS file system must have it's own SPIFFSNVS_Data object and it must
  *  be persistent.
  */
typedef struct SPIFFSNVS_Data_ {
    NVS_Handle     nvsHandle;    /*!< Handle to NVS driver instance */
    SPIFFSNVS_Lock lock;         /*!< Thread-safety lock object */
} SPIFFSNVS_Data;

/*!
 *  @brief  Initializes #spiffs, #spiffs_config and #SPIFFSNVS_Data structures
 *          for SPIFFS to interface with the NVS driver
 *
 *  This function initializes fields in the #spiffs, #spiffs_config and
 *  #SPIFFSNVS_Data structures to enable SPIFFS to interface with a NVS driver
 *  instance.  This function performs the following actions:
 *      1.  Opens the TI NVS driver instance
 *      2.  Verifies valid logical block and logical page sizes are used
 *      3.  Creates a lock used by SPIFFS for thread-safety
 *      4.  Sets fields in #spiffs, #spiffs_config and #SPIFFSNVS_Data
 *          structures to enable SPIFFS to use the TI NVS as the interface to
 *          flash memory.
 *
 *  The #spiffs and #spiffs_config structures are used to mount the file system.
 *  This function must be called only once for each SPIFFS instance.
 *
 *  @param  spiffsnvsData       Pointer to a #SPIFFSNVS_Data structure
 *
 *  @param  nvsIndex            Index of the NVS driver to be used to
 *                              read/write flash memory.
 *
 *  @param  fs                  Pointer to a #spiffs file system object
 *
 *  @param  fsConfig            Pointer to a #spiffs_config structure
 *
 *  @param  logicalBlockSize    Logical block size in bytes
 *
 *  @param  logicalPageSize     Logical page size in bytes
 *
 *  @return #SPIFFSNVS_STATUS_SUCCESS if structures were configured correctly.
 *  @return #SPIFFSNVS_STATUS_INV_NVS_IDX if an invalid index is used.
 *  @return #SPIFFSNVS_STATUS_INV_BLOCK_SIZE if an invalid logical block size is
 *          used.
 *  @return #SPIFFSNVS_STATUS_INV_PAGE_SIZE if an invalid logical page size is
 *          used.
 */
s32_t SPIFFSNVS_config(SPIFFSNVS_Data *spiffsnvsData,
    u32_t nvsIndex, spiffs *fs, spiffs_config *fsConfig,
    u32_t logicalBlockSize, u32_t logicalPageSize);

#ifdef __cplusplus
}
#endif

#endif /* THIRD_PARTY_SPIFFS_SPIFFSNVS_H_ */
