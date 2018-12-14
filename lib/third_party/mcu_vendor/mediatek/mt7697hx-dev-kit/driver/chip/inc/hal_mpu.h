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

#ifndef __HAL_MPU_H__
#define __HAL_MPU_H__
#include "hal_platform.h"

#ifdef HAL_MPU_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup MPU
 * @{
 * This section describes the programming interfaces of MPU HAL driver.
 * The MPU is an optional component in ARMv7-M architecture, that provides control of access rights on physical addresses. The processor supports a standard ARMv7 Protected Memory System Architecture model. The MPU divides the memory map into a number of regions, and defines the location, size, access permissions and memory attributes of each region. The Cortex-M4 MPU defines eight separate memory regions as well as a background region. The Cortex-M4 MPU memory map is unified. This means instruction accesses and data
accesses have the same region settings. The MPU provides full support for:
 * - Protection regions.
 *  - Independent attribute settings for each region.
 * - Overlapping protection regions, with ascending region priority:
 *  - 7 = highest priority
 *  - 0 = lowest priority
 * - Access permissions.
 * - Exporting memory attributes to the system.
 * \n
 * .
 * MPU mismatches and permission violations invoke the programmable-priority MemManage fault handler.
 * \n
 * You can use the MPU to:
 * - Enforce privilege rules.
 * - Separate processes.
 * - Enforce access rules.
 *
 * @section HAL_MPU_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the MPU driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b MPU                        | Memory Protection Unit. For more information, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0403e.b/index.html">ARMv7-M Architecture Reference Manual</a>.|
 *
 *
 * @section HAL_MPU_Features_Chapter Supported features
 * The MPU HAL driver enables configuration of location, size and access permissions for each region. The configuration of memory attributes (TEX, C, B bits field of the Region Attribute and Size Register) is not supported. Call the MPU APIs to configure the MPU, as shown below:
 * - \b Configure \b each \b region \b for \b different \b access \b permissions. \n
 *   Call hal_mpu_region_configure(), to configure the specified region. The first parameter is of #hal_mpu_region_t type, to indicate which region is to be configured. The second parameter is of #hal_mpu_region_config_t type, and defines the location, size and access permissions of the region. The sub region feature of MPU is also supported by defining mpu_subregion_mask field of type #hal_mpu_region_config_t.
 * - \b Enable \b the \b MPU \b feature \b inside \b the \b MCU. \n
 *   To enable the MPU settings for the configured region(s), user has to follow these two steps:
 *   - Step1: Call hal_mpu_region_enable() to enable the MPU region(s).
 *   - Step2: Call hal_mpu_enable() to enable the MPU function. \n
 *   .
 *   Note, that at least one region of the memory map must be enabled for the system to operate when hal_mpu_enable() is called, unless the PRIVDEFENA field in the structure #hal_mpu_config_t is set to TRUE. This structure is a parameter in the MPU initialization function hal_mpu_init(). If the PRIVDEFENA field is set to TRUE and no regions are enabled, then only privileged software can operate. If the PRIVDEFENA is set to TRUE and the MPU is enable as well:
 *   - Any access by privileged software that does not address an enabled memory region behaves as defined by the default memory map.
 *   - Any access by unprivileged software that does not address an enabled memory region causes a MemManage fault. \n
 *   .
 *   When hal_mpu_disable() is called and PRIVDEFENA field is set to FALSE, the system uses the default memory map. This has the same memory attributes as if the MPU is not implemented. The default memory map applies to accesses from both privileged and unprivileged software. \n
 *   When the MPU is enabled, accesses to the System Control Space and vector table are always permitted. Other areas are accessible based on regions and whether PRIVDEFENA is set to TRUE. \n
 *    - Unless HFNMIENA is set to TRUE, the MPU is not enabled when the processor is executing an exception handler with priority -1 or -2. These priorities are only possible when handling a hard fault or NMI (Non Maskable Interrupt) exception, or when FAULTMASK is enabled. Setting the HFNMIENA field to TRUE enables the MPU when operating with these two priorities.
 *    - User should make sure that the unused region(s) is (are) disabled when the MPU is enabled.
 * - \b Set \b the \b PRIVDEFENA \b and \b HFNMIENA \b features \b of \b the \b MPU. \n
 *   User can set the PRIVDEFENA and HFNMIENA features of the MPU, by calling hal_mpu_init(). For more information about PRIVDEFENA and HFNMIENA, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0403e.b/index.html">ARMv7-M Architecture Reference Manual</a>.
 *
 * @section HAL_MPU_Driver_Usage_Chapter How to use this driver
 *  The MPU HAL driver APIs enable the user to perform the following.
 * - \b Set \b location, \b size \b and \b access \b permissions \b for \b each \b region.
 *   - As described above in the section @ref HAL_MPU_Features_Chapter, user can accomplish this by calling hal_mpu_region_configure(). The sample code is shown below:
 *   - Sample code:
 *   @code
 * 	 hal_mpu_region_t region = HAL_MPU_REGION_0;
 * 	 hal_mpu_region_config_t region_config;
 *
 * 	 memset(region_config, 0, sizeof(hal_mpu_region_config_t));//Note, call memset() to make sure all memory regions are set to 0.
 * 	 region_config.mpu_region_address = 0x10000000;//The start address of the region that is configured.
 * 	 region_config.mpu_region_size = HAL_MPU_REGION_SIZE_1KB;//The size of the region.
 * 	 region_config.mpu_region_access_permission = HAL_MPU_FULL_ACCESS;//Access permission is full access.
 * 	 region_config.mpu_subregion_mask = 0x00;//Unmask all sub regions.
 * 	 region_config.mpu_xn = FALSE;//Enables the instruction access.
 *
 * 	 hal_mpu_region_configure(region, &region_config);
 *
 *
 *   @endcode
 * - \b Enable \b MPU \b function \b inside \b the \b MCU. \n
*    First call hal_mpu_region_enable() to enable the region(s), then hal_mpu_enable(), to enable the MPU. The sample code is shown below:
*    - Sample code:
*    @code
*
* 	 hal_mpu_region_t region = HAL_MPU_REGION_0;
*
* 	 hal_mpu_region_enable(region);//Enables the region.
* 	 hal_mpu_enable();//Enables the MPU.
*
*    @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_mpu_enum Enum
 *  @{
 */

/** @brief This enum defines the MPU API return status. */
typedef enum {
    HAL_MPU_STATUS_INVALID_PARAMETER = -6,	    /**< Invalid parameter */
    HAL_MPU_STATUS_ERROR_BUSY = -5,		        /**< MPU is busy */
    HAL_MPU_STATUS_ERROR_REGION = -4,           /**< MPU region number error, MPU region is not a value of type #hal_mpu_region_t */
    HAL_MPU_STATUS_ERROR_REGION_ADDRESS = -3,   /**< MPU region address error, MPU region address is not valid */
    HAL_MPU_STATUS_ERROR_REGION_SIZE = -2,      /**< MPU region size error, MPU region size is not a value of type #hal_mpu_region_size_t */
    HAL_MPU_STATUS_ERROR = -1,                  /**< MPU error, errors other than reasons described above */
    HAL_MPU_STATUS_OK = 0                       /**< MPU ok */
} hal_mpu_status_t;

/** @brief MPU region number. */
typedef enum {
    HAL_MPU_REGION_0 = 0,                       /**< MPU region 0 */
    HAL_MPU_REGION_1 = 1,                       /**< MPU region 1 */
    HAL_MPU_REGION_2 = 2,                       /**< MPU region 2 */
    HAL_MPU_REGION_3 = 3,                       /**< MPU region 3 */
    HAL_MPU_REGION_4 = 4,                       /**< MPU region 4 */
    HAL_MPU_REGION_5 = 5,                       /**< MPU region 5 */
    HAL_MPU_REGION_6 = 6,                       /**< MPU region 6 */
    HAL_MPU_REGION_7 = 7,                       /**< MPU region 7 */
    HAL_MPU_REGION_MAX                          /**< Max MPU region number (invalid) */
} hal_mpu_region_t;

/** @brief MPU region size. */
typedef enum {
    HAL_MPU_REGION_SIZE_MIN = 3,                /**< Minimum MPU size (invalid) */
    HAL_MPU_REGION_SIZE_32B = 4,                /**< MPU region size is 32B */
    HAL_MPU_REGION_SIZE_64B = 5,                /**< MPU region size is 64B */
    HAL_MPU_REGION_SIZE_128B = 6,               /**< MPU region size is 128B */
    HAL_MPU_REGION_SIZE_256B = 7,               /**< MPU region size is 256B */
    HAL_MPU_REGION_SIZE_512B = 8,               /**< MPU region size is 512B */
    HAL_MPU_REGION_SIZE_1KB = 9,                /**< MPU region size is 1KB */
    HAL_MPU_REGION_SIZE_2KB = 10,               /**< MPU region size is 2KB */
    HAL_MPU_REGION_SIZE_4KB = 11,               /**< MPU region size is 4KB */
    HAL_MPU_REGION_SIZE_8KB = 12,               /**< MPU region size is 8KB */
    HAL_MPU_REGION_SIZE_16KB = 13,              /**< MPU region size is 16KB */
    HAL_MPU_REGION_SIZE_32KB = 14,              /**< MPU region size is 32KB */
    HAL_MPU_REGION_SIZE_64KB = 15,              /**< MPU region size is 64KB */
    HAL_MPU_REGION_SIZE_128KB = 16,             /**< MPU region size is 128KB */
    HAL_MPU_REGION_SIZE_256KB = 17,             /**< MPU region size is 256KB */
    HAL_MPU_REGION_SIZE_512KB = 18,             /**< MPU region size is 512KB */
    HAL_MPU_REGION_SIZE_1MB = 19,               /**< MPU region size is 1MB */
    HAL_MPU_REGION_SIZE_2MB = 20,               /**< MPU region size is 2MB */
    HAL_MPU_REGION_SIZE_4MB = 21,               /**< MPU region size is 4MB */
    HAL_MPU_REGION_SIZE_8MB = 22,               /**< MPU region size is 8MB */
    HAL_MPU_REGION_SIZE_16MB = 23,              /**< MPU region size is 16MB */
    HAL_MPU_REGION_SIZE_32MB = 24,              /**< MPU region size is 32MB */
    HAL_MPU_REGION_SIZE_64MB = 25,              /**< MPU region size is 64MB */
    HAL_MPU_REGION_SIZE_128MB = 26,             /**< MPU region size is 128MB */
    HAL_MPU_REGION_SIZE_256MB = 27,             /**< MPU region size is 256MB */
    HAL_MPU_REGION_SIZE_512MB = 28,             /**< MPU region size is 512MB */
    HAL_MPU_REGION_SIZE_1GB = 29,               /**< MPU region size is 1GB */
    HAL_MPU_REGION_SIZE_2GB = 30,               /**< MPU region size is 2GB */
    HAL_MPU_REGION_SIZE_4GB = 31,               /**< MPU region size is 4GB */
    HAL_MPU_REGION_SIZE_MAX                     /**< Max MPU region size (invalid) */
} hal_mpu_region_size_t;

/** @brief MPU access permission indicates the access rights of the specified region. */
typedef enum {
    HAL_MPU_NO_ACCESS = 0,                      /**< No access for both the privileged and unprivileged software */
    HAL_MPU_PRIVILEGED_ACCESS_ONLY = 1,         /**< Access from privileged software only */
    HAL_MPU_PRIVILEGED_READ_WRITE_UNPRIVILEGED_READ_ONLY = 2,  /**<  Read and write for the privileged software, read only for the unprivileged software */
    HAL_MPU_FULL_ACCESS = 3,                    /**< Full access for both the privileged and unprivileged software */
    HAL_MPU_UNPREDICTABLE = 4,                  /**< Reserved access permission, results in unpredictable MPU behavior */
    HAL_MPU_PRIVILEGED_READ_ONLY = 5,           /**< Read only for the privileged software */
    HAL_MPU_READONLY = 6                        /**< Read only for both the privileged and unprivileged software */
} hal_mpu_access_permission_t;

/**
 * @}
 */

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @defgroup hal_mpu_struct Struct
 *  @{
 */

/** @brief MPU configuration structure for the MPU features "PRIVDEFENA" and "HFNMIENA". */
typedef struct {
    bool privdefena;                          /**< Enables MPU privileged default memory map */
    bool hfnmiena;                            /**< MPU enabled or not during the HardFault and NMI handler */
} hal_mpu_config_t;

/** @brief MPU region config structure contains the start address, the size and access rights of the region, the sub region feature is also included in the structure. */
typedef struct {
    uint32_t mpu_region_address;                /**< MPU region start address */
    hal_mpu_region_size_t mpu_region_size;      /**< MPU region size */
    hal_mpu_access_permission_t mpu_region_access_permission; /**< MPU region access permission */
    uint8_t mpu_subregion_mask;                 /**< MPU sub region mask*/
    bool mpu_xn;                                /**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted */
} hal_mpu_region_config_t;

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief 	MPU initialization function.
 * @param[in] mpu_config is the configuration information for MPU.
 * @return
 * #HAL_MPU_STATUS_OK, MPU is successfully initialized. \n
 * #HAL_MPU_STATUS_INVALID_PARAMETER, mpu_config is NULL. \n
 * #HAL_MPU_STATUS_ERROR_BUSY, MPU is busy.
 */
hal_mpu_status_t hal_mpu_init(const hal_mpu_config_t *mpu_config);

/**
 * @brief 	MPU deinitialization function. This function resets the MPU registers to their default values.
 * @return
 * #HAL_MPU_STATUS_OK, MPU is successfully deinitialized.
 */
hal_mpu_status_t hal_mpu_deinit(void);

/**
 * @brief 	MPU enable function. Enables the MPU settings during a memory access. @sa hal_mpu_disable().
 * @return
 * #HAL_MPU_STATUS_OK, MPU is successfully enabled .
 */
hal_mpu_status_t hal_mpu_enable(void);

/**
 * @brief MPU disable function. Disables the MPU settings. @sa hal_mpu_enable().
 * @return
 * #HAL_MPU_STATUS_OK, MPU is successfully disabled.
 */
hal_mpu_status_t hal_mpu_disable(void);

/**
 * @brief 	MPU region enable function. Enables the specified region, when the hal_mpu_enable() is called, the settings of the corresponding region take effect. @sa hal_mpu_enable().
 * @param[in] region is the region that is enabled, this parameter can only be a value of type #hal_mpu_region_t.
 * @return
 * #HAL_MPU_STATUS_OK, MPU region is successfully enabled. \n
 * #HAL_MPU_STATUS_ERROR_REGION, the region is invalid.
 */
hal_mpu_status_t hal_mpu_region_enable(hal_mpu_region_t region);

/**
 * @brief 	MPU region disable function. Disables the specified region, when this function is called, the settings of corresponding region are disabled even if the hal_mpu_enable() function is called. @sa hal_mpu_disable().
 * @param[in] region is the region that is disabled, this parameter can only be a value of type #hal_mpu_region_t.
 * @return
 * #HAL_MPU_STATUS_OK, MPU region is successfully disabled. \n
 * #HAL_MPU_STATUS_ERROR_REGION, the region is invalid.
 */
hal_mpu_status_t hal_mpu_region_disable(hal_mpu_region_t region);

/**
 * @brief 	MPU region configuration function.
 * @param[in] region is the region that is configured.
 * @param[in] region_config is the configuration information of the region.
 * @return
 * #HAL_MPU_STATUS_OK, MPU region is successfully configured. \n
 * #HAL_MPU_STATUS_INVALID_PARAMETER, region_config is NULL. \n
 * #HAL_MPU_STATUS_ERROR_REGION, the region is invalid. \n
 * #HAL_MPU_STATUS_ERROR_REGION_SIZE, the region size is invalid. \n
 * #HAL_MPU_STATUS_ERROR_REGION_ADDRESS, the region address is invalid.
 */
hal_mpu_status_t hal_mpu_region_configure(hal_mpu_region_t region, const hal_mpu_region_config_t *region_config);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/

#endif /* HAL_MPU_MODULE_ENABLED */
#endif /* __HAL_MPU_H__ */

