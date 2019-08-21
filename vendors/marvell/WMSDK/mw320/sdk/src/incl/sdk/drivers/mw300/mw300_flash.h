/** @file mw300_flash.h
*
*  @brief This file contains FLASH driver module header
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

/****************************************************************************//*
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_FLASH_H
#define __MW300_FLASH_H

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup FLASH 
 *  @{
 */
  
/** @defgroup FLASH_Public_Types FLASH_Public_Types
 *  @{
 */

/**  
 *  @brief FLASH interface type
 */
typedef enum
{
  FLASH_INTERFACE_QSPI,         /*!< QSPI is selected as the interface to access the flash */     
  FLASH_INTERFACE_FLASHC        /*!< Flash controller is selected as the interface to the flash */
}FLASH_Interface_Type;

/**  
 *  @brief FLASH protection type
 */ 
typedef enum
{
  FLASH_PROT_NONE         = 0x00,      /*!< None protection */  
  FLASH_PROT_UPPER_64KB   = 0x04,      /*!< Protect upper 64KB   0x0F0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_128KB  = 0x08,      /*!< Protect upper 128KB  0x0E0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_256KB  = 0x0C,      /*!< Protect upper 256KB  0x0C0000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_512KB  = 0x10,      /*!< Protect upper 512KB  0x080000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_64KB   = 0x24,      /*!< Protect lower 64KB   0x000000 ~ 0x00FFFF */
  FLASH_PROT_LOWER_128KB  = 0x28,      /*!< Protect lower 128KB  0x000000 ~ 0x01FFFF */
  FLASH_PROT_LOWER_256KB  = 0x2C,      /*!< Protect lower 256KB  0x000000 ~ 0x03FFFF */
  FLASH_PROT_LOWER_512KB  = 0x30,      /*!< Protect lower 512KB  0x000000 ~ 0x07FFFF */
  FLASH_PROT_ALL          = 0x14,      /*!< Protect all          0x000000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_4KB    = 0x44,      /*!< Protect upper 4KB    0x0FF000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_8KB    = 0x48,      /*!< Protect upper 8KB    0x0FE000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_16KB   = 0x4C,      /*!< Protect upper 16KB   0x0FC000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_32KB   = 0x50,      /*!< Protect upper 32KB   0x0F8000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_4KB    = 0x64,      /*!< Protect lower 4KB    0x000000 ~ 0x000FFF */
  FLASH_PROT_LOWER_8KB    = 0x68,      /*!< Protect lower 8KB    0x000000 ~ 0x001FFF */
  FLASH_PROT_LOWER_16KB   = 0x6C,      /*!< Protect lower 16KB   0x000000 ~ 0x003FFF */
  FLASH_PROT_LOWER_32KB   = 0x70,      /*!< Protect lower 32KB   0x000000 ~ 0x007FFF */
  FLASH_PROT_LOWER_960KB  = 0x84,      /*!< Protect lower 960KB  0x000000 ~ 0x0EFFFF */
  FLASH_PROT_LOWER_896KB  = 0x88,      /*!< Protect lower 896KB  0x000000 ~ 0x0DFFFF */
  FLASH_PROT_LOWER_768KB  = 0x8C,      /*!< Protect lower 960KB  0x000000 ~ 0x0BFFFF */
  FLASH_PROT_UPPER_960KB  = 0xA4,      /*!< Protect upper 960KB  0x010000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_896KB  = 0xA8,      /*!< Protect upper 896KB  0x020000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_768KB  = 0xAC,      /*!< Protect upper 768KB  0x040000 ~ 0x0FFFFF */
  FLASH_PROT_LOWER_1020KB = 0xC4,      /*!< Protect lower 1020KB 0x000000 ~ 0x0FEFFF */
  FLASH_PROT_LOWER_1016KB = 0xC8,      /*!< Protect lower 1016KB 0x000000 ~ 0x0FDFFF */
  FLASH_PROT_LOWER_1008KB = 0xCC,      /*!< Protect lower 1008KB 0x000000 ~ 0x0FBFFF */
  FLASH_PROT_LOWER_992KB  = 0xD0,      /*!< Protect lower 992KB  0x000000 ~ 0x0F7FFF */
  FLASH_PROT_UPPER_1020KB = 0xE4,      /*!< Protect upper 1020KB 0x001000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_1016KB = 0xE8,      /*!< Protect upper 1016KB 0x002000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_1008KB = 0xEC,      /*!< Protect upper 1008KB 0x004000 ~ 0x0FFFFF */
  FLASH_PROT_UPPER_992KB  = 0xF0,      /*!< Protect upper 992KB  0x008000 ~ 0x0FFFFF */
 }FLASH_Protection_Type; 

/**  
 *  @brief FLASH read mode
 */ 
typedef enum
{
  FLASH_NORMAL_READ,                  /*!< Normal read mode */
  FLASH_FAST_READ,                    /*!< Fast read mode */
  FLASH_FAST_READ_DUAL_OUT,           /*!< Fast read dual output mode */
  FLASH_FAST_READ_DUAL_IO,            /*!< Fast read dual IO mode */
  FLASH_FAST_READ_QUAD_OUT,           /*!< Fast read quad output mode */
  FLASH_FAST_READ_QUAD_IO,            /*!< Fast read quad IO mode */
  FLASH_WORD_FAST_READ_QUAD_IO,       /*!< Word fast read quad IO mode */
  FLASH_OCTAL_WORD_FAST_READ_QUAD_IO, /*!< Octal word fast read quad IO mode */
}FLASH_ReadMode_Type; 

/**  
 *  @brief FLASH program mode
 */ 
typedef enum
{
  FLASH_PROGRAM_NORMAL,               /*!< Normal page program mode */
  FLASH_PROGRAM_QUAD,                 /*!< Quad page program mode   */
}FLASH_ProgramMode_Type; 


/**  
 *  @brief FLASH status type
 */ 
typedef enum
{
  FLASH_STATUS_LO,                    /*!< STATUS[7:0]  */
  FLASH_STATUS_HI,                    /*!< STATUS[15:8] */
}FLASH_Status_Type; 

/** @defgroup FLASH_INSTRUCTIONS       
 *  @{
 */     
#define FLASH_INS_CODE_WE      0x06   /*!< Write enable */
#define FLASH_INS_CODE_WE_VSR  0x50   /*!< Write enable for volatile status register */
#define FLASH_INS_CODE_WD      0x04   /*!< Write disable */
#define FLASH_INS_CODE_RSR1    0x05   /*!< Read status register 1 */
#define FLASH_INS_CODE_RSR2    0x35   /*!< Read status register 2 */
#define FLASH_INS_CODE_WSR1     0x01   /*!< Write status register 1 */
#define FLASH_INS_CODE_WSR2     0x31   /*!< Write status register 2 */
#define FLASH_INS_CODE_PP      0x02   /*!< Page program */
#define FLASH_INS_CODE_QPP     0x32   /*!< Quad page program */
#define FLASH_INS_CODE_QPP_MX     0x38   /*!< Quad page program for Macronix chip*/
#define FLASH_INS_CODE_SE      0x20   /*!< Sector(4k) erase */
#define FLASH_INS_CODE_BE_32K  0x52   /*!< Block(32k) erase */
#define FLASH_INS_CODE_BE_64K  0xD8   /*!< Block(64k) erase */
#define FLASH_INS_CODE_CE      0xC7   /*!< Chip erase */
#define FLASH_INS_CODE_PD      0xB9   /*!< Power down */

#define FLASH_INS_CODE_RD      0x03   /*!< Read data */
#define FLASH_INS_CODE_FR      0x0B   /*!< Fast read */
#define FLASH_INS_CODE_FRDO    0x3B   /*!< Fast read dual output */ 
#define FLASH_INS_CODE_FRQO    0x6B   /*!< Fast read quad output */ 
#define FLASH_INS_CODE_FRDIO   0xBB   /*!< Fast read dual IO */
#define FLASH_INS_CODE_FRQIO   0xEB   /*!< Fast read quad IO */
#define FLASH_INS_CODE_WFRQIO  0xE7   /*!< Word Fast read quad IO, A0 must be zero */
#define FLASH_INS_CODE_OWFRQIO 0xE3   /*!< Octal word Fast read quad IO, A[3:0] must be zero */
#define FLASH_INS_CODE_FDRST   0xFFFF /*!< Fast dual read mode reset */
#define FLASH_INS_CODE_FQRST   0xFF   /*!< Fast quad read mode reset */

#define FLASH_INS_CODE_RPD_DI  0xAB   /*!< Release power down or device ID */
#define FLASH_INS_CODE_RUID    0x4B   /*!< Read unique ID number */
#define FLASH_INS_CODE_JEDEC_ID    0x9F   /*!< Read JEDEC ID number */

/*@} end of group FLASH_INSTRUCTIONS */

/** @defgroup FLASH_CONSTANTS       
 *  @{
 */   
#define KILO_BYTE		   0x400    /*!< 1KB */
#define MEGA_BYTE		   0x100000 /*!< 1MB */

#define FLASH_32K_BLOCK_SIZE       0x8000   /*!< 32KB */
#define FLASH_64K_BLOCK_SIZE       0x10000  /*!< 64KB */

/*@} end of group FLASH_CONSTANTS */

/* Get the page number according to the address with the page space */
#define FLASH_PAGE_NUM(addr)        ((addr)>>8)

#define FLASH_NAME_SIZE		16

struct flash_device_config {
	char name[FLASH_NAME_SIZE];
	uint32_t jedec_id;
	uint32_t chip_size;
	uint32_t sector_size;
	uint32_t block_size;
	uint16_t page_size;
};


/*@} end of group FLASH_Public_Types definitions */

/** @defgroup FLASH_Public_Constants
 *  @{
 */ 

/*@} end of group FLASH_Public_Constants */

/** @defgroup FLASH_Public_Macro
 *  @{
 */

/*@} end of group FLASH_Public_Macro */

/** @defgroup FLASH_Public_FunctionDeclaration
 *  @brief FLASH functions statement
 *  @{
 */


void FLASH_PowerDown(FunctionalState newCmd);
void FLASH_SelectInterface(FLASH_Interface_Type interface);
FlagStatus FLASH_GetBusyStatus(void);
Status FLASH_SetProtectionMode(FLASH_Protection_Type protectMode);
Status FLASH_EraseAll(void);
Status FLASH_SectorErase(uint32_t sectorNumber);
Status FLASH_Block32KErase(uint32_t blockNumber);
Status FLASH_Block64KErase(uint32_t blockNumber);
Status FLASH_Erase(uint32_t startAddr, uint32_t endAddr);
uint32_t FLASH_Read(FLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num);
uint32_t FLASH_WordRead(FLASH_ReadMode_Type readMode, uint32_t address);
uint8_t FLASH_ByteRead(FLASH_ReadMode_Type readMode, uint32_t address);
Status FLASH_PageWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_Write(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_WordWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data);
Status FLASH_ByteWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data);
uint64_t FLASH_GetUniqueID(void);
void FLASH_SetWriteEnableBit(FunctionalState newCmd);
void FLASH_WriteEnableVSR(void);
uint8_t FLASH_GetStatus(FLASH_Status_Type statusIdx);
Status FLASH_WriteStatus(uint16_t status);
Status FLASH_IsSectorBlank(uint32_t sectorNumber);
Status FLASH_IsBlank(void);
void FLASH_ResetFastReadDual(void);
void FLASH_ResetFastReadQuad(void);
uint32_t FLASH_GetJEDECID(void);
Status FLASH_SetConfig(uint32_t jedecID);
const struct flash_device_config *FLASH_GetConfig(void);
/*@} end of group FLASH_Public_FunctionDeclaration */

/*@} end of group FLASH */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_FLASH_H__ */
