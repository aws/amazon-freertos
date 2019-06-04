/** @file mw300_mpu.h
*
*  @brief This file contains MPU driver module header
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/***************************************************************************//**
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_MPU_H__
#define __MW300_MPU_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  MPU 
 *  @{
 */
  
/** @defgroup MPU_Public_Types MPU_Public_Types
 *  @brief MPU configuration structure type definition
 *  @{
 */


/** 
 *  @brief MPU region number type definition   
 */
typedef enum
{
  MPU_REGION_0 = 0,
  MPU_REGION_1,
  MPU_REGION_2,
  MPU_REGION_3,
  MPU_REGION_4,
  MPU_REGION_5,
  MPU_REGION_6,
  MPU_REGION_7
}MPU_RegionNo_Type;

/** 
 *  @brief MPU subregion number type definition   
 */
typedef enum
{
  MPU_SUBREGION_0 = 0x1,
  MPU_SUBREGION_1 = 0x2,
  MPU_SUBREGION_2 = 0x4,
  MPU_SUBREGION_3 = 0x8,
  MPU_SUBREGION_4 = 0x10,
  MPU_SUBREGION_5 = 0x20,
  MPU_SUBREGION_6 = 0x40,
  MPU_SUBREGION_7 = 0x80,
  MPU_SUBREGION_ALL = 0xff
}MPU_SubregionNo_Type;

/** 
 *  @brief MPU region size type definition   
 */
typedef enum
{
  MPU_REGION_SIZE_32B  =  4,
  MPU_REGION_SIZE_64B,
  MPU_REGION_SIZE_128B,
  MPU_REGION_SIZE_256B,
  MPU_REGION_SIZE_512B,
  MPU_REGION_SIZE_1KB,
  MPU_REGION_SIZE_2KB,
  MPU_REGION_SIZE_4KB,
  MPU_REGION_SIZE_8KB,
  MPU_REGION_SIZE_16KB,
  MPU_REGION_SIZE_32KB,
  MPU_REGION_SIZE_64KB,
  MPU_REGION_SIZE_128KB,
  MPU_REGION_SIZE_256KB,
  MPU_REGION_SIZE_512KB,
  MPU_REGION_SIZE_1MB,
  MPU_REGION_SIZE_2MB,
  MPU_REGION_SIZE_4MB,
  MPU_REGION_SIZE_8MB,
  MPU_REGION_SIZE_16MB,
  MPU_REGION_SIZE_32MB,
  MPU_REGION_SIZE_64MB,
  MPU_REGION_SIZE_128MB,
  MPU_REGION_SIZE_256MB,
  MPU_REGION_SIZE_512MB,
  MPU_REGION_SIZE_1GB,
  MPU_REGION_SIZE_2GB,
  MPU_REGION_SIZE_4GB
}MPU_RegionSize_Type;

/** 
 *  @brief MPU region access permission type definition   
 */
typedef enum
{
  MPU_REGION_PRI_NO_USER_NO,
  MPU_REGION_PRI_RW_USER_NO,
  MPU_REGION_PRI_RW_USER_RO,
  MPU_REGION_PRI_RW_USER_RW,
  MPU_REGION_PRI_UNPRE_USER_UNPRE,
  MPU_REGION_PRI_RO_USER_NO,
  MPU_REGION_PRI_RO_USER_RO,
}MPU_RegionAccess_Type;

/** 
 *  @brief MPU interrupt type definition   
 */
typedef enum
{
  MPU_INT_STACKING_ERR,
  MPU_INT_UNSTACKING_ERR,
  MPU_INT_DATA_ACCESS_VIOL,
  MPU_INT_INSTRUCION_ACCESS_VIOL,
  MPU_INT_ALL
}MPU_INT_Type;

/**      
 *  @brief MPU config struct type definition  
 */
typedef struct
{
  FunctionalState privDefMemEnable;      /*!< privileged default memory map access:
                                              ENABLE (1): the default memory map
                                                          will be used for privileged 
                                                          accesses as a background region. 
                                              DISABLE (0): the background region can not
                                                            be accessed  */
  
  FunctionalState hardFaultNMIEnable;    /*!< Enable/Disable MPU during the hard 
                                              fault handler and NMI handler: 
                                              ENABLE (1)
                                              DISABLE (0) */

}MPU_Config_Type;

/**      
 *  @brief MPU region config struct type definition  
 */
typedef struct
{
  MPU_RegionSize_Type size;                  /*!< set the region size:
                                                  MPU_REGION_SIZE_32B(4),
                                                  MPU_REGION_SIZE_64B,
                                                  MPU_REGION_SIZE_128B,
                                                  MPU_REGION_SIZE_256B,
                                                  MPU_REGION_SIZE_512B,
                                                  MPU_REGION_SIZE_1KB,
                                                  MPU_REGION_SIZE_2KB,
                                                  MPU_REGION_SIZE_4KB,
                                                  MPU_REGION_SIZE_8KB,
                                                  MPU_REGION_SIZE_16KB,
                                                  MPU_REGION_SIZE_32KB,
                                                  MPU_REGION_SIZE_64KB,
                                                  MPU_REGION_SIZE_128KB,
                                                  MPU_REGION_SIZE_256KB,
                                                  MPU_REGION_SIZE_512KB,
                                                  MPU_REGION_SIZE_1MB,
                                                  MPU_REGION_SIZE_2MB,
                                                  MPU_REGION_SIZE_4MB,
                                                  MPU_REGION_SIZE_8MB,
                                                  MPU_REGION_SIZE_16MB,
                                                  MPU_REGION_SIZE_32MB,
                                                  MPU_REGION_SIZE_64MB,
                                                  MPU_REGION_SIZE_128MB,
                                                  MPU_REGION_SIZE_256MB,
                                                  MPU_REGION_SIZE_512MB,
                                                  MPU_REGION_SIZE_1GB,
                                                  MPU_REGION_SIZE_2GB,
                                                  MPU_REGION_SIZE_4GB */
  
  uint32_t baseAddress;                      /*!< set the base address of this region. The significant bit
                                                  is [31: N].N is dependent on the region size. For example, 
                                                  for a 64k region, N= 16,the [15:0] bits will be ignored. */
  
  MPU_RegionAccess_Type accessPermission;    /*!< set the access permission of the region: 
                                                  MPU_REGION_PRI_NO_USER_NO(0): no access for both privileged or user access
                                                  MPU_REGION_PRI_RW_USER_NO(1): priviliged - read/write, user - no access
                                                  MPU_REGION_PRI_RW_USER_RO(2): priviliged - read/write, user - read only
                                                  MPU_REGION_PRI_RW_USER_RW(3): priviliged - read/write, user - read/write
                                                  MPU_REGION_PRI_UNPRE_USER_UNPRE(4): priviliged - unpredictable, user - unpredictable
                                                  MPU_REGION_PRI_RO_USER_NO(5): priviliged - read only, user - no access
                                                  MPU_REGION_PRI_RO_USER_RO(6): priviliged - read only, user - read only */
  
  FunctionalState instructionAccess;          /*!< Enable/Disable instruction access:
                                                   ENABLE (1): enable instuction fetch
                                                               from this region 
                                                   DISABLE (0): disable instuction 
                                                                fetch from this region  */


}MPU_RegionConfig_Type;


/*@} end of group MPU_Public_Types definitions */


/** @defgroup MPU_Public_Constants
 *  @{
 */ 



/*@} end of group MPU_Public_Constants */

/** @defgroup MPU_Public_Macro
 *  @{
 */

/*@} end of group MPU_Public_Macro */


/** @defgroup MPU_Public_FunctionDeclaration
 *  @brief MPU functions statement
 *  @{
 */

void MPU_Enable(void);
void MPU_Disable(void);
void MPU_RegionEnable(MPU_RegionNo_Type regionNo);
void MPU_RegionDisable(MPU_RegionNo_Type regionNo);
void MPU_SubregionEnable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo);
void MPU_SubregionDisable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo);

void MPU_Init(MPU_Config_Type* mpuConfig);
void MPU_RegionConfig(MPU_RegionNo_Type regionNo, MPU_RegionConfig_Type* regionConfig);

void MPU_IntEnable(void);
void MPU_IntDisable(void);

/*@} end of group MPU_Public_FunctionDeclaration */

/*@} end of group MPU  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_MPU_H__ */
