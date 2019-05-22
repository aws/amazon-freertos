/** @file mw300_flashc.h
*
*  @brief This file contains FLASHC driver module header
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
 * @date     23-Dec-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_FLASHC_H__
#define __MW300_FLASHC_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  88MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  FLASHC 
 *  @{
 */
  

/** @defgroup FLASHC_Public_Types FLASHC_Public_Types
 *  @brief FLASHC configuration structure type definition
 *  @{
 */

/**
 *  @brief FLASHC configuration way selection type
 */
typedef enum
{
  FLASHC_CFG_HW,                    /*!< Use hardware configuration  */
  FLASHC_CFG_SW,                    /*!< Use software configuration  */
}FLASHC_CFG_Selection_Type;

/**
 *  @brief FLASHC hardware configuration type
 */
typedef enum
{
  FLASHC_HW_CMD_R,                    /*!< read data(0x03)  */
  FLASHC_HW_CMD_FR,                   /*!< fast read data(0x0B)  */
  FLASHC_HW_CMD_FRDO,                 /*!< fast read dual output (0x3B)  */
  FLASHC_HW_CMD_FRQO,                 /*!< fast read quad output (0x6B)  */
  FLASHC_HW_CMD_FRDIO,                /*!< fast read dual I/O (0xBB)  */
  FLASHC_HW_CMD_FRDIOC,               /*!< fast read dual I/O with continuous read (0xBB)  */
  FLASHC_HW_CMD_FRQIO,                /*!< fast read quad I/O (0xEB)  */
  FLASHC_HW_CMD_FRQIOC,               /*!< fast read quad I/O with continuous read (0xEB)  */
  FLASHC_HW_CMD_WRQIO,                /*!< word read quad I/O (0xE7)  */
  FLASHC_HW_CMD_WRQIOC,               /*!< word read quad I/O with continuous read(0xE7)  */
  FLASHC_HW_CMD_OWRQIO,               /*!< Octal word read quad I/O (0xE3) */
  FLASHC_HW_CMD_OWRQIOC,              /*!< Octal word read quad I/O with continuous read(0xE3) */
  FLASHC_HW_CMD_ECRD,                 /*!< exit from continuous read mode (dual) */
  FLASHC_HW_CMD_ECRQ,                 /*!< exit from continuous read mode (quad) */
}FLASHC_HW_CFG_Type;

/**
 *  @brief FLASHC data pin type
 */
typedef enum
{
  FLASHC_DATA_PIN_SINGLE,                    /*!< Use 1 serial interface pin  */
  FLASHC_DATA_PIN_DUAL,                      /*!< Use 2 serial interface pins */
  FLASHC_DATA_PIN_QUAD,                      /*!< Use 4 serial interface pins */
}FLASHC_DataPin_Type;

/**
 *  @brief FLASHC address pin type
 */
typedef enum
{
  FLASHC_ADDR_PIN_SINGLE,                    /*!< Use 1 serial interface pin */
  FLASHC_ADDR_PIN_AS_DATA,                   /*!< Use the number of pins as indicated in DATA_PIN */
}FLASHC_AddrPin_Type;

/**
 *  @brief FLASHC clock mode type
 */
typedef enum
{
  FLASHC_POL0_PHA0,                          /*!< Inactive state of serial clock is low, data is latched on the rising edge of the serial clock   */
  FLASHC_POL0_PHA1,                          /*!< Inactive state of serial clock is low, data is latched on the falling edge of the serial clock  */
  FLASHC_POL1_PHA0,                          /*!< Inactive state of serial clock is high, data is latched on the falling edge of the serial clock */
  FLASHC_POL1_PHA1,                          /*!< Inactive state of serial clock is high, data is latched on the rising edge of the serial clock  */
}FLASHC_ClkMode_Type;


/**
 *  @brief FLASHC capture clock edge type
 */
typedef enum
{
  FLASHC_CAPT_EDGE_FIRST,                    /*!< Input data is captured on the first edge of the serial clock  */
  FLASHC_CAPT_EDGE_SECOND,                   /*!< Input data is captured on the second edge of the serial clock */
}FLASHC_CaptEdge_Type;

/**
 *  @brief FLASHC transfer width type
 */
typedef enum
{
  FLASHC_BYTE_LEN_1BYTE,                     /*!< 1 byte in each serial interface I/O transfer  */
  FLASHC_BYTE_LEN_4BYTE,                     /*!< 4 bytes in each serial interface I/O transfer */
}FLASHC_ByteLen_Type;

/**
 *  @brief FLASHC prescaler type
 */
typedef enum
{
  FLASHC_CLK_DIVIDE_1  = 0x1,                /*!< Serial interface clock prescaler is SPI clock / 1  */
  FLASHC_CLK_DIVIDE_2  = 0x2,                /*!< Serial interface clock prescaler is SPI clock / 2  */
  FLASHC_CLK_DIVIDE_3  = 0x3,                /*!< Serial interface clock prescaler is SPI clock / 3  */
  FLASHC_CLK_DIVIDE_4  = 0x4,                /*!< Serial interface clock prescaler is SPI clock / 4  */
  FLASHC_CLK_DIVIDE_5  = 0x5,                /*!< Serial interface clock prescaler is SPI clock / 5  */
  FLASHC_CLK_DIVIDE_6  = 0x6,                /*!< Serial interface clock prescaler is SPI clock / 6  */
  FLASHC_CLK_DIVIDE_7  = 0x7,                /*!< Serial interface clock prescaler is SPI clock / 7  */
  FLASHC_CLK_DIVIDE_8  = 0x8,                /*!< Serial interface clock prescaler is SPI clock / 8  */
  FLASHC_CLK_DIVIDE_9  = 0x9,                /*!< Serial interface clock prescaler is SPI clock / 9  */
  FLASHC_CLK_DIVIDE_10 = 0xA,                /*!< Serial interface clock prescaler is SPI clock / 10 */
  FLASHC_CLK_DIVIDE_11 = 0xB,                /*!< Serial interface clock prescaler is SPI clock / 11 */
  FLASHC_CLK_DIVIDE_12 = 0xC,                /*!< Serial interface clock prescaler is SPI clock / 12 */
  FLASHC_CLK_DIVIDE_13 = 0xD,                /*!< Serial interface clock prescaler is SPI clock / 13 */
  FLASHC_CLK_DIVIDE_14 = 0xE,                /*!< Serial interface clock prescaler is SPI clock / 14 */
  FLASHC_CLK_DIVIDE_15 = 0xF,                /*!< Serial interface clock prescaler is SPI clock / 15 */
  FLASHC_CLK_DIVIDE_16 = 0x18,               /*!< Serial interface clock prescaler is SPI clock / 16 */
  FLASHC_CLK_DIVIDE_18 = 0x19,               /*!< Serial interface clock prescaler is SPI clock / 18 */
  FLASHC_CLK_DIVIDE_20 = 0x1A,               /*!< Serial interface clock prescaler is SPI clock / 20 */
  FLASHC_CLK_DIVIDE_22 = 0x1B,               /*!< Serial interface clock prescaler is SPI clock / 22 */
  FLASHC_CLK_DIVIDE_24 = 0x1C,               /*!< Serial interface clock prescaler is SPI clock / 24 */
  FLASHC_CLK_DIVIDE_26 = 0x1D,               /*!< Serial interface clock prescaler is SPI clock / 26 */
  FLASHC_CLK_DIVIDE_28 = 0x1E,               /*!< Serial interface clock prescaler is SPI clock / 28 */
  FLASHC_CLK_DIVIDE_30 = 0x1F,               /*!< Serial interface clock prescaler is SPI clock / 30 */
}FLASHC_PreScaler_Type;

/** 
 *  @brief FLASHC INSTR_CNT type
 */
typedef enum
{
  FLASHC_INSTR_CNT_0BYTE,                    /*!< Instrution count: 0 byte  */
  FLASHC_INSTR_CNT_1BYTE,                    /*!< Instrution count: 1 byte  */
  FLASHC_INSTR_CNT_2BYTE,                    /*!< Instrution count: 2 bytes */
}FLASHC_INSTR_CNT_TYPE;

/** 
 *  @brief FLASHC ADDR_CNT type
 */
typedef enum
{
  FLASHC_ADDR_CNT_0BYTE,                     /*!< Address count: 0 byte  */
  FLASHC_ADDR_CNT_1BYTE,                     /*!< Address count: 1 byte  */
  FLASHC_ADDR_CNT_2BYTE,                     /*!< Address count: 2 bytes */
  FLASHC_ADDR_CNT_3BYTE,                     /*!< Address count: 3 bytes */
  FLASHC_ADDR_CNT_4BYTE,                     /*!< Address count: 4 bytes */
}FLASHC_ADDR_CNT_TYPE;

/** 
 *  @brief FLASHC RM_CNT type
 */
typedef enum
{
  FLASHC_RM_CNT_0BYTE,                      /*!< Read mode count: 0 byte  */
  FLASHC_RM_CNT_1BYTE,                      /*!< Read mode count: 1 byte  */
  FLASHC_RM_CNT_2BYTE,                      /*!< Read mode count: 2 bytes */
}FLASHC_RM_CNT_TYPE;

/** 
 *  @brief FLASHC DUMMY_CNT type
 */
typedef enum
{
  FLASHC_DUMMY_CNT_0BYTE,                    /*!< Dummy count: 0 byte  */
  FLASHC_DUMMY_CNT_1BYTE,                    /*!< Dummy count: 1 byte  */
  FLASHC_DUMMY_CNT_2BYTE,                    /*!< Dummy count: 2 bytes */
}FLASHC_DUMMY_CNT_TYPE;


/**
 *  @brief FLASHC  software configure type
 */
typedef struct
{
  FLASHC_DataPin_Type dataPinMode;           /*!< Configure FLASHC data pin */
  FLASHC_AddrPin_Type addrPinMode;           /*!< Configure FLASHC addr pin */
  FLASHC_ByteLen_Type byteLen;               /*!< Configure FLASHC length each serial transfer */
  FLASHC_DUMMY_CNT_TYPE dummyCnt;            /*!< Dummy count */
  FLASHC_RM_CNT_TYPE rmCnt;                  /*!< read mode count */
  uint16_t readMode;                         /*!< read mode */
  FLASHC_ADDR_CNT_TYPE addrCnt;              /*!< address count */
  FLASHC_INSTR_CNT_TYPE instrCnt;            /*!< instruction count */
  uint16_t instrucion;                       /*!< instruction */
}FLASHC_SW_CFG_Type;

/**
 *  @brief FLASHC  timing configure type
 */
typedef struct
{
  FLASHC_ClkMode_Type clkMode;               /*!< Configure FLASHC clock mode */
  FLASHC_PreScaler_Type preScaler;           /*!< Configure FLASHC prescaler for serial interface */
  FLASHC_CaptEdge_Type captEdge;             /*!< Configure FLASHC capture clock edge */
  uint8_t clkOutDly;                         /*!< Delay on the outgoing clock to flash */
  uint8_t clkInDly;                          /*!< Delay on the clock that capture read data from flash */
  uint8_t dinDly;                            /*!< Delay on the incoming data from flash */
}FLASHC_Timing_CFG_Type;

/*@} end of group DRIVER_Public_Types */


/** @defgroup DRIVER_Public_Constants
 *  @{
 */ 
/*@} end of group DRIVER_Public_Constants */


/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */

Status FLASHC_FlushCache(void);
void FLASHC_CacheCntEn(void);
Status FLASHC_GetCacheCnt(uint32_t *HitCnt, uint32_t *MissCnt);
void FLASHC_ResetCacheCnt(void);
void FLASHC_CacheModeEn(FunctionalState state);
void FLASHC_SramModeEn(FunctionalState state);
void FLASHC_CfgSelection(FLASHC_CFG_Selection_Type cfgSel);
void FLASHC_SWCfg(FLASHC_SW_CFG_Type *config);
void FLASHC_HWCfg(FLASHC_HW_CFG_Type config);
void FLASHC_TimingCfg(FLASHC_Timing_CFG_Type *config);
void FLASHC_OffsetAddrEnable(uint32_t offsetAddr);
void FLASHC_OffsetAddrDisable(void);
Status FLASHC_ExitDualContReadStat(void);
Status FLASHC_ExitQuadContReadStat(void);
FlagStatus FLASHC_ReadExitContReadStat(void);
int FLASH_SetCmdType_QuadModeRead(uint32_t jedecID);
/*@} end of group FLASHC_Public_FunctionDeclaration */

/*@} end of group FLASHC  */

/*@} end of group 88MW300_Periph_Driver */
#endif /* __MW300_FLASHC_H__ */
