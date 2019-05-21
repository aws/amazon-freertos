/** @file mw300_sdio.h
*
*  @brief This file contains SDIO driver module header
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

#ifndef __MW300_SDIO_H__
#define __MW300_SDIO_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  SDIO
 *  @{
 */

/** @defgroup  SDIO_Public_Types
 *  @{
 */

/**
 *  @brief SDIO bus voltage level type definition
 */
typedef enum
{
  SDIOC_VOLTAGE_3P3 = 0x07,                /*!< SDIO bus voltage select 3.3V define */
  SDIOC_VOLTAGE_3P0 = 0x06,                /*!< SDIO bus voltage select 3.0V define */
  SDIOC_VOLTAGE_1P8 = 0x05,                /*!< SDIO bus voltage select 1.8V define */
 
}SDIOC_BusVoltage_Type;

/**
 *  @brief SDIO speed mode type definition
 */
typedef enum
{
  SDIOC_SPEED_NORMAL = 0x00,               /*!< SDIO normal speed mode define */
  SDIOC_SPEED_HIGH,                        /*!< SDIO high speed mode define */

}SDIOC_SpeedMode_Type;

/**
 *  @brief SDIO data width type definition
 */
typedef enum
{
  SDIOC_BUS_WIDTH_1 = 0x00,                /*!< SDIO data width 1 bit mode define */
  SDIOC_BUS_WIDTH_4,                       /*!< SDIO data width 4 bits mode define */

}SDIOC_BusWide_Type;

/**
 *  @brief SDIO command type definition
 */
typedef enum
{
  SDIOC_CMD_NORMAL  = 0x00,                /*!< Normal command */
  SDIOC_CMD_SUSPEND,                       /*!< Suspend command */
  SDIOC_CMD_RESUME,                        /*!< Resume command */
  SDIOC_CMD_ABORT,                         /*!< Abort command */
    
}SDIOC_CMD_Type;

/**
 *  @brief SDIO data present select definition
 */
typedef enum
{
  SDIOC_DATA_PRESENT_NO  = 0x00,            /*!< No data present */
  SDIOC_DATA_PRESENT_YES,                   /*!< Data present */
    
}SDIOC_DataPreSel_Type;

/**
 *  @brief SDIO response type definition
 */
typedef enum
{
  SDIOC_RESP_NO = 0x00,                     /*!< No response */
  SDIOC_RESP_LONG,                          /*!< Response length 136 bits */
  SDIOC_RESP_SHORT,                         /*!< Response length 48 bits*/
  SDIOC_RESP_SHORT_CHK,                     /*!< Response length 48 bits, check busy after response */

}SDIOC_Response_Type;

/**
 *  @brief SDIO response number definition
 */
typedef enum
{
  SDIOC_RESP_REG_0 = 0x00,                  /*!< Command response register0 */
  SDIOC_RESP_REG_1,                         /*!< Command response register1 */
  SDIOC_RESP_REG_2,                         /*!< Command response register2 */
  SDIOC_RESP_REG_3,                         /*!< Command response register3 */

}SDIOC_ResponseNum_Type;

/**
 *  @brief SDIO block select definition
 */
typedef enum
{
  SDIOC_BLOCK_SINGLE   = 0x00,             /*!< Single block */
  SDIOC_BLOCK_MULTIPLE,                    /*!< Multiple block */

}SDIOC_BlkSel_Type;

/**
 *  @brief SDIO data transfer direction select definition
 */
typedef enum
{
  SDIOC_TRANSDIR_WRITE = 0x00,             /*!< Write(Host to Card) */
  SDIOC_TRANSDIR_READ,                     /*!< Read(Card to Host) */

}SDIOC_TransferDir_Type;

/**
 *  @brief SDIO LED control type definition
 */
typedef enum
{
  SDIOC_LED_OFF = 0x00,                    /*!< LED on */
  SDIOC_LED_ON,                            /*!< LED off */

}SDIOC_LED_Type;

/**
 *  @brief SDIO Configuration Structure type definition
 */
typedef struct
{
  FunctionalState clkEnable;               /*!< Clock enable.
                                                - ENABLE : enable Clock
                                                - DISABLE: disable Clock */

  SDIOC_BusWide_Type busWidth;             /*!< Data width select.
                                                - SDIOC_BUS_WIDTH_4: data width 4 bits mode
                                                - SDIOC_BUS_WIDTH_1: data width 1 bit mode */

  SDIOC_SpeedMode_Type speedMode;          /*!< Speed mode select .
                                                - SDIOC_SPEED_NORMAL: normal speed mode
                                                - SDIOC_SPEED_HIGH  : high speed mode */

  SDIOC_BusVoltage_Type busVoltSel;        /*!< Bus voltage select.
                                                - SDIOC_VOLTAGE_3P3: SDIO bus voltage select 3.3V
                                                - SDIOC_VOLTAGE_3P0: SDIO bus voltage select 3.0V
                                                - SDIOC_VOLTAGE_1P8: SDIO bus voltage select 1.8V */

  FunctionalState readWaitCtrl;           /*!< Read wait control.
                                                - ENABLE:  enable read wait control
                                                - DISABLE: disable read wait control */
   
  FunctionalState blkGapInterrupt;        /*!< Interrupt at block gap.
                                                - ENABLE:  enable interrupt detection at the block gap for multiple block transfer
                                                - DISABLE: card can't signal an interrupt during multiple block transfer */

}SDIOC_CFG_Type;

/**
 *  @brief SDIO command configuration definition
 */
typedef struct
{
  uint32_t arg;                           /*!< SDIO command argument which specified as bit 39-8 of command-format */

  SDIOC_Response_Type resp;                /*!< SDIO response type.
                                                - SDIOC_RESP_NO       : No response
                                                - SDIOC_RESP_LONG     : Response length 136 bits
                                                - SDIOC_RESP_SHORT    : Response length 48 bits
                                                - SDIOC_RESP_SHORT_CHK: Response length 48 bits, check busy after response */

  uint32_t cmdIndex;                      /*!< Command index*/

  SDIOC_CMD_Type cmdType;                  /*!< Command type.
                                                - SDIOC_CMD_NORMAL : Normal command
                                                - SDIOC_CMD_SUSPEND: Suspend command
                                                - SDIOC_CMD_RESUME : Resume command
                                                - SDIOC_CMD_ABORT  : Abort command */
   
  SDIOC_DataPreSel_Type dataPreSel;        /*!< Data present select.
                                                - SDIOC_DATA_PRESENT_NO : No data present 
                                                - SDIOC_DATA_PRESENT_YES: Data present */

  FunctionalState cmdIndexCheck;          /*!< Command index check enable.
                                                - ENABLE : enable the command index check
                                                - DISABLE: disable the command index check */                                                                                                                                                                              

  FunctionalState cmdCrcCheck;            /*!< Command crc check enable.
                                                - ENABLE : enable the command crc check
                                                - DISABLE: disable the command crc check */

}SDIOC_CmdCfg_Type;

/**
 *  @brief SDIO transfer type definition
 */
typedef struct
{
  SDIOC_BlkSel_Type blkSel;                /*!< Multi/ Single block select.
                                                - SDIOC_BLOCK_SINGLE  : Single block
                                                - SDIOC_BLOCK_MULTIPLE: Multiple block */

  SDIOC_TransferDir_Type transferDir;      /*!< Data transfer direction select.
                                                - SDIOC_TRANSDIR_WRITE: Write(Host to Card)
                                                - SDIOC_TRANSDIR_READ : Read(Card to Host) */

  FunctionalState blkCntEnable;           /*!< Block count enable.
                                                - ENABLE : enable block count
                                                - DISABLE: disable block count */

  uint32_t blkCnt;                        /*!< Block count for current transfer.
                                                - 0x0000: stop count
                                                - 0x0001: 1 block
                                                - 0x0002: 2 blocks
                                                ..................
                                                ..................
                                                - 0xFFFF: 65535 blocks */

  uint32_t blkSize;                       /*!< Transfer block size.
                                                - 0x000: no data transfer
                                                - 0x001: 1 byte
                                                - 0x002: 2 bytes
                                                - 0x003: 3 bytes
                                                - 0x004: 4 bytes
                                                ..................
                                                - 0x1FF: 511 bytes
                                                - 0x200: 512 bytes
                                                ..................
                                                - 0x800: 2048 bytes
                                                - 0x801 - 0xFFF: reserved */ 
                                                
  uint32_t dataTimeoutCnt;                /*!< Data timeout counter value.
                                                - 0b1111: Reserved
                                                - 0b1110: (timeout clock)* 2^27
                                                ...............................
                                                ...............................
                                                - 0b0001: (timeout clock)* 2^14
                                                - 0b0000: (timeout clock)* 2^13 */
                                                
  FunctionalState dmaEnabe;                /*!< DMA mode select
                                                - ENABLE: DMA enable
                                                - DISABLE: DMA disable */

}SDIOC_Transfer_Type;

/**
 *  @brief SDIO DMA configuration definition
 */
typedef struct
{
  uint32_t dmaBufSize;                    /*!< Host DMA buffer size.
                                                - 0b000: 4 KB
                                                - 0b001: 8 KB
                                                - 0b010: 16 KB
                                                - 0b011: 32 KB
                                                - 0b100: 64 KB
                                                - 0b101: 128 KB
                                                - 0b110: 256 KB
                                                - 0b111: 512 KB */

  uint32_t dmaSystemAddr;                 /*!< DMA system address */

}SDIOC_DmaCfg_Type;

/**
 *  @brief SDIO interrupt type definition
 */
typedef enum
{
  SDIOC_INT_CMDCOMP,                       /*!< Command complete interrupt */

  SDIOC_INT_XFRCOMP,                       /*!< Transfer complete interrupt */

  SDIOC_INT_BGEVNT,                        /*!< Block gap event */

  SDIOC_INT_DMAINT,                        /*!< DMA interrupt */

  SDIOC_INT_BUFWRRDY,                      /*!< Buffer write ready interrupt */

  SDIOC_INT_BUFRDRDY,                      /*!< Buffer read ready interrupt */    

  SDIOC_INT_CDINS,                         /*!< Card insertion interrupt */

  SDIOC_INT_CDREM,                         /*!< Card remove interrupt */    

  SDIOC_INT_CDINT,                         /*!< Card interrupt */

  SDIOC_INT_ERRINT,                        /*!< Error interrupt */

  SDIOC_INT_CTOERR,                        /*!< Command timeout error interrupt */

  SDIOC_INT_CCRCERR,                       /*!< Command crc error interrupt */

  SDIOC_INT_CENDERR,                       /*!< Command end bit error interrupt */

  SDIOC_INT_CIDXERR,                       /*!< Command index error interrupt */

  SDIOC_INT_DTOERR,                        /*!< Data timeout error interrupt */

  SDIOC_INT_DCRCERR,                       /*!< Data crc error interrupt */

  SDIOC_INT_DENDERR,                       /*!< Data end bit error interrupt */

  SDIOC_INT_ILMTERR,                       /*!< Current limit error interrupt */

  SDIOC_INT_AHBTERR,                       /*!< AHB Target error interrupt */ 

  SDIOC_INT_ALL,                           /*!< All SDIO interrupt */
        
}SDIOC_INT_Type;


/**
 *  @brief SDIO status type definition
 */
typedef enum
{
  SDIOC_STATUS_CCMDINHBT,                  /*!< Command inhibit(CMD) status */

  SDIOC_STATUS_DCMDINHBT,                  /*!< Command inhibit(DAT) status */

  SDIOC_STATUS_DATACTV,                    /*!< DAT line active  */

  SDIOC_STATUS_WRACTV,                     /*!< Write transfer active */

  SDIOC_STATUS_RDACTV,                     /*!< Read transfer active */

  SDIOC_STATUS_BUFWREN,                    /*!< Buffer write enable */

  SDIOC_STATUS_BUFRDEN,                    /*!< Buffer read enable */

  SDIOC_STATUS_CINDSTD,                    /*!< Card inserted */ 

  SDIOC_STATUS_CDSTBL,                     /*!< Card state stable */ 

  SDIOC_STATUS_CDDETLVL,                   /*!< Card detect pin level */

  SDIOC_STATUS_WPSWLVL,                    /*!< Write protect switch pin level */

  SDIOC_STATUS_LWRDATLVL_DAT0,             /*!< Data[0] line signal level */
 
  SDIOC_STATUS_LWRDATLVL_DAT1,             /*!< Data[1] line signal level */

  SDIOC_STATUS_LWRDATLVL_DAT2,             /*!< Data[2] line signal level */

  SDIOC_STATUS_LWRDATLVL_DAT3,             /*!< Data[3] line signal level */

  SDIOC_STATUS_CMDLVL,                     /*!< CMD line signal level */ 

  SDIOC_STATUS_UPRDATLVL_DAT4,             /*!< DAT[4] line signal level */ 

  SDIOC_STATUS_UPRDATLVL_DAT5,             /*!< DAT[5] line signal level */ 

  SDIOC_STATUS_UPRDATLVL_DAT6,             /*!< DAT[6] line signal level */ 

  SDIOC_STATUS_UPRDATLVL_DAT7,             /*!< DAT[7] line signal level */ 
  
}SDIOC_STATUS_Type;

/*@} end of group SDIO_Public_Types definitions */

/** @defgroup SDIO_Public_Constants
 *  @{
 */

/** @defgroup SDIOC_Voltage_Mode
 *  @{
 */
#define IS_SDIOC_VOLTAGE(VOLTAGE)             (((VOLTAGE) >= 4) && ((VOLTAGE) <= 7))

/*@} end of group SDIOC_Voltage_Mode definitions*/

/** @defgroup SDIOC_Data_Present_Type
 *  @{
 */
#define IS_SDIOC_DATA_PRESENT_TYPE(DATAPRE)   (((DATAPRE) == SDIOC_DATA_PRESENT_NO) || ((DATAPRE) == SDIOC_DATA_PRESENT_YES))

/*@} end of group SDIOC_Data_Present_Type definitions*/

/** @defgroup SDIOC_Resp_Type
 *  @{
 */
#define IS_SDIOC_RESP_TYPE(RESP_TYPE)         (((RESP_TYPE) >= 0) && ((RESP_TYPE) < 4))

/*@} end of group SDIOC_Resp_Type definitions*/

/** @defgroup SDIOC_RespNum_Type
 *  @{
 */
#define IS_SDIOC_RESP_NUM(RESP_NUM)           (((RESP_NUM) >= 0) && ((RESP_NUM) < 4))

/*@} end of group SDIOC_RespNum_Type definitions*/


/** @defgroup SDIOC_Block_Type
 *  @{
 */
#define IS_SDIOC_BLOCK_TYPE(BLK_TYPE)         (((BLK_TYPE) == SDIOC_BLOCK_SINGLE) || ((BLK_TYPE) == SDIOC_BLOCK_MULTIPLE))

/*@} end of group SDIOC_Block_Type definitions*/

/** @defgroup SDIOC_Transfer_Dir
 *  @{
 */
#define IS_SDIOC_TRANS_DIR(DIR)               (((DIR) == SDIOC_TRANSDIR_WRITE) || ((DIR) == SDIOC_TRANSDIR_WRITE))

/*@} end of group SDIOC_Transfer_Dir definitions*/

/** @defgroup SDIOC_Speed_Mode
 *  @{
 */
#define IS_SDIOC_SPEED(SPEED)                (((SPEED) == SDIOC_SPEED_HIGH) || ((SPEED) == SDIOC_SPEED_NORMAL))

/*@} end of group SDIOC_Speed_Mode definitions*/

/** @defgroup SDIOC_LED_Type
 *  @{
 */
#define IS_SDIOC_LED_Type(LED_TYPE)          (((LED_TYPE) == SDIOC_LED_ON) || ((LED_TYPE) == SDIOC_LED_OFF))

/*@} end of group SDIOC_LED_Type definitions*/

/** @defgroup SDIOC_STATUS_TYPE
 *  @{
 */
#define IS_SDIOC_STATUS_TYPE(STATUS_TYPE)    (((STATUS_TYPE) >= 0) && ((STATUS_TYPE) < 20))

/*@} end of group SDIOC_STATUS_TYPE */

/** @defgroup SDIOC_INT_TYPE
 *  @{
 */
#define IS_SDIOC_INT_TYPE(INT_TYPE)          (((INT_TYPE) >= 0) && ((INT_TYPE) < 16))

/*@} end of group SDIOC_INT_TYPE */

/** @defgroup SDIOC_COMMAND_TYPE
 *  @{
 */
#define IS_SDIOC_COMMAND_TYPE(CMD_TYPE)      (((CMD_TYPE) >= 0) && ((CMD_TYPE) < 3))

/*@} end of group SDIOC_COMMAND_TYPE */

/** @defgroup SDIOC_INT_MASK
 *  @{
 */
#define IS_SDIOC_INTMASK(INTMASK)            (((INTMASK) == MASK) || ((INTMASK) == UNMASK))

/*@} end of group SDIOC_INT_MASK */


/*@} end of group SDIO_Public_Constants definitions*/


/** @defgroup SDIO_Public_Macro
 *  @{
 */
 
/*@} end of group SDIO_Public_Macro */


/** @defgroup SDIO_Public_FunctionDeclaration
 *  @brief SDIO function statement
 *  @{
 */
void SDIOC_Init(SDIOC_CFG_Type* sdioCfgStruct);
void SDIOC_SetTransferMode(SDIOC_Transfer_Type* transfMode);
void SDIOC_SendCmd(SDIOC_CmdCfg_Type* cmd);
uint32_t SDIOC_GetResponse(SDIOC_ResponseNum_Type rpx);
void SDIOC_DmaConfig(SDIOC_DmaCfg_Type* dmaCfg);
uint32_t SDIOC_ReadData(void);
void SDIOC_WriteData(uint32_t data);
void SDIOC_UpdateSysAddr(uint32_t addr);
uint32_t SDIOC_GetDmaBufsz(void);
void SDIOC_PowerOn(void);
void SDIOC_PowerOff(void);
Status SDIOC_Reset(void);
Status SDIOC_DataxLineReset(void);
Status SDIOC_CmdLineReset(void);
void SDIOC_LedCtrl(SDIOC_LED_Type ledCtrl);
FlagStatus SDIOC_GetStatus(SDIOC_STATUS_Type statusType);
uint32_t SDIOC_GetCapabilities(void);
FlagStatus SDIOC_GetIntStatus(SDIOC_INT_Type intType);
void SDIOC_IntMask(SDIOC_INT_Type intType, IntMask_Type maskState);
void SDIOC_IntSigMask(SDIOC_INT_Type intType, IntMask_Type maskState);
void SDIOC_IntClr(SDIOC_INT_Type intType);

/*@} end of group SDIO_Public_FunctionDeclaration */

/*@} end of group SDIO */

/*@} end of group MW300_Periph_Driver */

#endif /* End of __MW300_SDIO_H__ */
