/** @file mw300_qspi.h
*
*  @brief This file contains QSPI driver module header
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
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_QSPI_H__
#define __MW300_QSPI_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup QSPI 
 *  @{
 */

/** @defgroup QSPI_Public_Types QSPI_Public_Types
 *  @{
 */

/**
 *  @brief QSPI data pin type
 */
typedef enum
{
  QSPI_DATA_PIN_SINGLE,                    /*!< Use 1 serial interface pin  */
  QSPI_DATA_PIN_DUAL,                      /*!< Use 2 serial interface pins */
  QSPI_DATA_PIN_QUAD,                      /*!< Use 4 serial interface pins */
}QSPI_DataPin_Type;

/**
 *  @brief QSPI address pin type
 */
typedef enum
{
  QSPI_ADDR_PIN_SINGLE,                    /*!< Use 1 serial interface pin */
  QSPI_ADDR_PIN_AS_DATA,                   /*!< Use the number of pins as indicated in DATA_PIN */
}QSPI_AddrPin_Type;

/**
 *  @brief QSPI clock mode type
 */
typedef enum
{
  QSPI_POL0_PHA0,                          /*!< Inactive state of serial clock is low, data is latched on the rising edge of the serial clock   */
  QSPI_POL0_PHA1,                          /*!< Inactive state of serial clock is low, data is latched on the falling edge of the serial clock  */
  QSPI_POL1_PHA0,                          /*!< Inactive state of serial clock is high, data is latched on the falling edge of the serial clock */
  QSPI_POL1_PHA1,                          /*!< Inactive state of serial clock is high, data is latched on the rising edge of the serial clock  */
}QSPI_ClkMode_Type;


/**
 *  @brief QSPI capture clock edge type
 */
typedef enum
{
  QSPI_CAPT_EDGE_FIRST,                    /*!< Input data is captured on the first edge of the serial clock  */
  QSPI_CAPT_EDGE_SECOND,                   /*!< Input data is captured on the second edge of the serial clock */
}QSPI_CaptEdge_Type;

/**
 *  @brief QSPI transfer width type
 */
typedef enum
{
  QSPI_BYTE_LEN_1BYTE,                     /*!< 1 byte in each serial interface I/O transfer  */
  QSPI_BYTE_LEN_4BYTE,                     /*!< 4 bytes in each serial interface I/O transfer */
}QSPI_ByteLen_Type;

/**
 *  @brief QSPI prescaler type
 */
typedef enum
{
  QSPI_CLK_DIVIDE_1  = 0x1,                /*!< Serial interface clock prescaler is SPI clock / 1  */
  QSPI_CLK_DIVIDE_2  = 0x2,                /*!< Serial interface clock prescaler is SPI clock / 2  */
  QSPI_CLK_DIVIDE_3  = 0x3,                /*!< Serial interface clock prescaler is SPI clock / 3  */
  QSPI_CLK_DIVIDE_4  = 0x4,                /*!< Serial interface clock prescaler is SPI clock / 4  */
  QSPI_CLK_DIVIDE_5  = 0x5,                /*!< Serial interface clock prescaler is SPI clock / 5  */
  QSPI_CLK_DIVIDE_6  = 0x6,                /*!< Serial interface clock prescaler is SPI clock / 6  */
  QSPI_CLK_DIVIDE_7  = 0x7,                /*!< Serial interface clock prescaler is SPI clock / 7  */
  QSPI_CLK_DIVIDE_8  = 0x8,                /*!< Serial interface clock prescaler is SPI clock / 8  */
  QSPI_CLK_DIVIDE_9  = 0x9,                /*!< Serial interface clock prescaler is SPI clock / 9  */
  QSPI_CLK_DIVIDE_10 = 0xA,                /*!< Serial interface clock prescaler is SPI clock / 10 */
  QSPI_CLK_DIVIDE_11 = 0xB,                /*!< Serial interface clock prescaler is SPI clock / 11 */
  QSPI_CLK_DIVIDE_12 = 0xC,                /*!< Serial interface clock prescaler is SPI clock / 12 */
  QSPI_CLK_DIVIDE_13 = 0xD,                /*!< Serial interface clock prescaler is SPI clock / 13 */
  QSPI_CLK_DIVIDE_14 = 0xE,                /*!< Serial interface clock prescaler is SPI clock / 14 */
  QSPI_CLK_DIVIDE_15 = 0xF,                /*!< Serial interface clock prescaler is SPI clock / 15 */
  QSPI_CLK_DIVIDE_16 = 0x18,               /*!< Serial interface clock prescaler is SPI clock / 16 */
  QSPI_CLK_DIVIDE_18 = 0x19,               /*!< Serial interface clock prescaler is SPI clock / 18 */
  QSPI_CLK_DIVIDE_20 = 0x1A,               /*!< Serial interface clock prescaler is SPI clock / 20 */
  QSPI_CLK_DIVIDE_22 = 0x1B,               /*!< Serial interface clock prescaler is SPI clock / 22 */
  QSPI_CLK_DIVIDE_24 = 0x1C,               /*!< Serial interface clock prescaler is SPI clock / 24 */
  QSPI_CLK_DIVIDE_26 = 0x1D,               /*!< Serial interface clock prescaler is SPI clock / 26 */
  QSPI_CLK_DIVIDE_28 = 0x1E,               /*!< Serial interface clock prescaler is SPI clock / 28 */
  QSPI_CLK_DIVIDE_30 = 0x1F,               /*!< Serial interface clock prescaler is SPI clock / 30 */
}QSPI_PreScale_Type;

/**
 *  @brief QSPI read or write type
 */
typedef enum
{
  QSPI_R_EN,                               /*!< Read data from the serial interface */
  QSPI_W_EN,                               /*!< Write data to the serial interface  */
}QSPI_RW_Type;

/** 
 *  @brief QSPI interrupt type definition
 */
typedef enum
{
  QSPI_XFER_DONE,                          /*!< QSPI transfer done flag */
  QSPI_XFER_RDY,                           /*!< QSPI serial interface transfer ready flag */
  QSPI_RFIFO_DMA_BURST,                    /*!< QSPI read FIFO DMA burst flag */
  QSPI_WFIFO_DMA_BURST,                    /*!< QSPI write FIFO DMA burst flag */
  QSPI_RFIFO_EMPTY,                        /*!< QSPI read FIFO empty flag */
  QSPI_RFIFO_FULL,                         /*!< QSPI read FIFO full flag */
  QSPI_WFIFO_EMPTY,                        /*!< QSPI write FIFO empty flag */
  QSPI_WFIFO_FULL,                         /*!< QSPI write FIFO full flag */
  QSPI_RFIFO_UNDRFLW,                      /*!< QSPI read FIFO underflow flag */
  QSPI_RFIFO_OVRFLW,                       /*!< QSPI read FIFO overflow flag */
  QSPI_WFIFO_UNDRFLW,                      /*!< QSPI write FIFO underflow flag */
  QSPI_WFIFO_OVRFLW,                       /*!< QSPI write FIFO overflow flag */
  QSPI_INT_ALL,                            /*!< All QSPI interrupt flags */
}QSPI_INT_Type; 

/**  
 *  @brief QSPI status bit type definition 
 */
typedef enum
{
  QSPI_STATUS_XFER_RDY,                    /*!< Serial interface transfer ready */
  QSPI_STATUS_RFIFO_EMPTY,	           /*!< Read FIFO empty */
  QSPI_STATUS_RFIFO_FULL,	           /*!< Read FIFO full */
  QSPI_STATUS_WFIFO_EMPTY,	           /*!< Write FIFO empty */
  QSPI_STATUS_WFIFO_FULL,	           /*!< Write FIFO full */
  QSPI_STATUS_RFIFO_UNDRFLW,	           /*!< Read FIFO underflow */
  QSPI_STATUS_RFIFO_OVRFLW,	           /*!< Read FIFO overflow */
  QSPI_STATUS_WFIFO_UNDRFLW,	           /*!< Write FIFO underflow */
  QSPI_STATUS_WFIFO_OVRFLW,	           /*!< Write FIFO overflow */
}QSPI_Status_Type;

/** 
 *  @brief QSPI dma read/write type 
 */
typedef enum
{
  QSPI_DMA_READ = 0,                       /*!< QSPI DMA read  */    
  QSPI_DMA_WRITE,                          /*!< QSPI DMA write */
}QSPI_DMA_Type;

/** 
 *  @brief QSPI dma read/write burst type
 */
typedef enum
{
  QSPI_DMA_1DATA = 0,                      /*!< One data(each of width BYTE_LEN) is available in the Read/Write FIFO before a receive/transmit burst request is made to the DMA   */
  QSPI_DMA_4DATA,                          /*!< Four data(each of width BYTE_LEN) is available in the Read/Write FIFO before a receive/transmit burst request is made to the DMA  */
  QSPI_DMA_8DATA,                          /*!< Eight data(each of width BYTE_LEN) is available in the Read/Write FIFO before a receive/transmit burst request is made to the DMA */
  QSPI_DMA_DISABLE,                        /*!< Disable DMA read/write */    
}QSPI_DMA_Data_Type;

/**
 *  @brief QSPI  global configure type
 */
typedef struct
{
  QSPI_DataPin_Type dataPinMode;           /*!< Configure QSPI data pin */
  QSPI_AddrPin_Type addrPinMode;           /*!< Configure QSPI addr pin */
  QSPI_ClkMode_Type clkMode;               /*!< Configure QSPI clock mode */
  QSPI_CaptEdge_Type captEdge;             /*!< Configure QSPI capture clock edge */
  QSPI_ByteLen_Type byteLen;               /*!< Configure QSPI length each serial transfer */
  QSPI_PreScale_Type preScale;             /*!< Configure QSPI prescaler for serial interface */
}QSPI_CFG_Type;

/** 
 *  @brief QSPI INSTR_CNT type
 */
typedef enum
{
  QSPI_INSTR_CNT_0BYTE,                    /*!< Instrution count: 0 byte  */
  QSPI_INSTR_CNT_1BYTE,                    /*!< Instrution count: 1 byte  */
  QSPI_INSTR_CNT_2BYTE,                    /*!< Instrution count: 2 bytes */
}QSPI_INSTR_CNT_TYPE;

/** 
 *  @brief QSPI ADDR_CNT type
 */
typedef enum
{
  QSPI_ADDR_CNT_0BYTE,                     /*!< Address count: 0 byte  */
  QSPI_ADDR_CNT_1BYTE,                     /*!< Address count: 1 byte  */
  QSPI_ADDR_CNT_2BYTE,                     /*!< Address count: 2 bytes */
  QSPI_ADDR_CNT_3BYTE,                     /*!< Address count: 3 bytes */
  QSPI_ADDR_CNT_4BYTE,                     /*!< Address count: 4 bytes */
}QSPI_ADDR_CNT_TYPE;

/** 
 *  @brief QSPI RM_CNT type
 */
typedef enum
{
  QSPI_RM_CNT_0BYTE,                      /*!< Read mode count: 0 byte  */
  QSPI_RM_CNT_1BYTE,                      /*!< Read mode count: 1 byte  */
  QSPI_RM_CNT_2BYTE,                      /*!< Read mode count: 2 bytes */
}QSPI_RM_CNT_TYPE;

/** 
 *  @brief QSPI DUMMY_CNT type
 */
typedef enum
{
  QSPI_DUMMY_CNT_0BYTE,                    /*!< Dummy count: 0 byte  */
  QSPI_DUMMY_CNT_1BYTE,                    /*!< Dummy count: 1 byte  */
  QSPI_DUMMY_CNT_2BYTE,                    /*!< Dummy count: 2 bytes */
  QSPI_DUMMY_CNT_3BYTE,                    /*!< Dummy count: 3 bytes */
}QSPI_DUMMY_CNT_TYPE;

/*@} end of group QSPI_Public_Types definitions */

/** @defgroup QSPI_Public_Constants
 *  @{
 */ 
  
/** @defgroup QSPI_INT_TYPE       
 *  @{
 */
#define IS_QSPI_INT_TYPE(INT_TYPE)        ((INT_TYPE) <= QSPI_INT_ALL)

/*@} end of group QSPI_INT_TYPE */


/** @defgroup QSPI_DMA_TYPE       
 *  @{
 */
#define IS_QSPI_DMA_TYPE(DMA_TYPE)        (((DMA_TYPE) == QSPI_DMA_READ) || ((DMA_TYPE) == QSPI_DMA_WRITE))

/*@} end of group QSPI_DMA_TYPE */

/** @defgroup QSPI_STATUS       
 *  @{
 */
#define IS_QSPI_STATUS_BIT(BIT)           ((BIT) <= QSPI_STATUS_WFIFO_OVRFLW)

/*@} end of group QSPI_STATUS */

/** @defgroup QSPI_DMA_DATA_TYPE
 *  @{
 */
#define IS_QSPI_DMA_DATA_TYPE(DMA_DATA_TYPE)        ((DMA_DATA_TYPE) <= QSPI_DMA_DISABLE)

/*@} end of group QSPI_DMA_DATA_TYPE */

/** @defgroup QSPI_INSTR_CNT_TYPE
 *  @{
 */
#define IS_QSPI_INSTR_CNT_TYPE(INSTR_CNT_TYPE)      ((INSTR_CNT_TYPE) <= QSPI_INSTR_CNT_2BYTE)

/*@} end of group QSPI_INSTR_CNT_TYPE */

/** @defgroup QSPI_ADDR_CNT_TYPE
 *  @{
 */
#define IS_QSPI_ADDR_CNT_TYPE(ADDR_CNT_TYPE)        ((ADDR_CNT_TYPE) <= QSPI_ADDR_CNT_4BYTE)

/*@} end of group QSPI_ADDR_CNT_TYPE */

/** @defgroup QSPI_RM_CNT_TYPE
 *  @{
 */
#define IS_QSPI_RM_CNT_TYPE(RM_CNT_TYPE)            ((RM_CNT_TYPE) <= QSPI_RM_CNT_2BYTE)

/*@} end of group QSPI_RM_CNT_TYPE */

/** @defgroup QSPI_DUMMY_CNT_TYPE
 *  @{
 */
#define IS_QSPI_DUMMY_CNT_TYPE(DUMMY_CNT_TYPE)      ((DUMMY_CNT_TYPE) <= QSPI_DUMMY_CNT_3BYTE)

/*@} end of group QSPI_DUMMY_CNT_TYPE */

/*@} end of group QSPI_Public_Constants */

/** @defgroup QSPI_Public_Macro
 *  @{
 */

/*@} end of group QSPI_Public_Macro */

/** @defgroup QSPI_Public_FunctionDeclaration
 *  @brief QSPI functions statement
 *  @{
 */
void QSPI_Reset(void);
void QSPI_Init(QSPI_CFG_Type* qspiConfigSet);

void QSPI_IntMask(QSPI_INT_Type intType, IntMask_Type intMask);
void QSPI_IntClr(void);

Status QSPI_FlushFIFO(void);

void QSPI_SetHdrcnt(QSPI_INSTR_CNT_TYPE instrCnt, QSPI_ADDR_CNT_TYPE addrCnt, QSPI_RM_CNT_TYPE rmCnt, QSPI_DUMMY_CNT_TYPE dummyCnt);
void QSPI_SetInstrCnt(QSPI_INSTR_CNT_TYPE instrCnt);
void QSPI_SetAddrCnt(QSPI_ADDR_CNT_TYPE addrCnt);
void QSPI_SetRModeCnt(QSPI_RM_CNT_TYPE rmCnt);
void QSPI_SetDummyCnt(QSPI_DUMMY_CNT_TYPE dummyCnt);

void QSPI_SetInstr(uint32_t instruct);
void QSPI_SetAddr(uint32_t address);
void QSPI_SetRMode(uint32_t readMode);
void QSPI_SetDInCnt(uint32_t count);

void QSPI_SetSSEnable(FunctionalState newCmd);

void QSPI_StartTransfer(QSPI_RW_Type rw);
void QSPI_StopTransfer(void); 

void QSPI_WriteByte(uint8_t byte);
uint8_t QSPI_ReadByte(void);
void QSPI_WriteWord(uint32_t word);
uint32_t QSPI_ReadWord(void);

FlagStatus QSPI_GetStatus(QSPI_Status_Type qspiStatus);
FlagStatus QSPI_GetIntStatus(QSPI_INT_Type qspiIntStatus);

FlagStatus QSPI_IsTransferCompleted(void);

void QSPI_DmaCmd(QSPI_DMA_Type dmaCtrl, QSPI_DMA_Data_Type dmaDataCtrl);

/*@} end of group QSPI_Public_FunctionDeclaration */

/*@} end of group QSPI */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_QSPI_H__ */
