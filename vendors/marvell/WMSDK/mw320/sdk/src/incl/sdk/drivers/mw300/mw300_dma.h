/** @file mw300_dma.h
*
*  @brief This file contains DMA driver module header
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

#ifndef __MW300_DMA_H__
#define __MW300_DMA_H__

#include "mw300.h"
#include "mw300_driver.h"

/* MAX transfer size supported by DMAC is 8191 bytes
 * with the restriction that SOURCE and DESTINATION
 * ADDRESSES should align with the DMA transfer width
 */
#define DMA_MAX_CHUNK_SIZE 8191

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup DMA 
 *  @{
 */
  
/** @defgroup DMA_Public_Types DMA_Public_Types 
 *  @{
 */

/**  
 *  @brief DMA Channel number type definition 
 */ 
typedef enum
{
  CHANNEL_0 = 0,                           /*!< Channel 0 define, priority level 0, the highest priority */
  CHANNEL_1,                               /*!< Channel 1 define, priority level 0, the highest priority */
  CHANNEL_2,                               /*!< Channel 2 define, priority level 0, the highest priority */
  CHANNEL_3,                               /*!< Channel 3 define, priority level 0, the highest priority */
  CHANNEL_4,                               /*!< Channel 4 define, priority level 1 */
  CHANNEL_5,                               /*!< Channel 5 define, priority level 1 */ 
  CHANNEL_6,                               /*!< Channel 6 define, priority level 1 */ 
  CHANNEL_7,                               /*!< Channel 7 define, priority level 1 */
  CHANNEL_8,                               /*!< Channel 8 define, priority level 2 */
  CHANNEL_9,                               /*!< Channel 9 define, priority level 2 */
  CHANNEL_10,                              /*!< Channel 10 define, priority level 2 */
  CHANNEL_11,                              /*!< Channel 11 define, priority level 2 */
  CHANNEL_12,                              /*!< Channel 12 define, priority level 3, the lowest priority */ 
  CHANNEL_13,                              /*!< Channel 13 define, priority level 3, the lowest priority */ 
  CHANNEL_14,                              /*!< Channel 14 define, priority level 3, the lowest priority */
  CHANNEL_15,                              /*!< Channel 15 define, priority level 3, the lowest priority */
  CHANNEL_16,                              /*!< Channel 16 define, priority level 0, the highest priority */
  CHANNEL_17,                              /*!< Channel 17 define, priority level 0, the highest priority */
  CHANNEL_18,                              /*!< Channel 18 define, priority level 0, the highest priority */
  CHANNEL_19,                              /*!< Channel 19 define, priority level 0, the highest priority */ 
  CHANNEL_20,                              /*!< Channel 20 define, priority level 1 */ 
  CHANNEL_21,                              /*!< Channel 21 define, priority level 1 */
  CHANNEL_22,                              /*!< Channel 22 define, priority level 1 */
  CHANNEL_23,                              /*!< Channel 23 define, priority level 1 */
  CHANNEL_24,                              /*!< Channel 24 define, priority level 2 */
  CHANNEL_25,                              /*!< Channel 25 define, priority level 2 */
  CHANNEL_26,                              /*!< Channel 26 define, priority level 2 */ 
  CHANNEL_27,                              /*!< Channel 27 define, priority level 2 */ 
  CHANNEL_28,                              /*!< Channel 28 define, priority level 3, the lowest priority */
  CHANNEL_29,                              /*!< Channel 29 define, priority level 3, the lowest priority */ 
  CHANNEL_30,                              /*!< Channel 30 define, priority level 3, the lowest priority */ 
  CHANNEL_31,                              /*!< Channel 31 define, priority level 3, the lowest priority */ 
}DMA_Channel_Type;

/**  
 *  @brief DMA peripheral mapping type definition 
 */
typedef enum
{
  DMA_PER0_GPT0_0 = 0,                      /*!< PER0 GPT0 CH0 mapping */
  DMA_PER1_GPT0_1 = 0x01,                   /*!< PER1 GPT0 CH1 mapping */
  DMA_PER2_GPT1_0 = 0x02,                   /*!< PER2 GPT1 CH0 mapping */
  DMA_PER3_GPT1_1 = 0x03,                   /*!< PER3 GPT1 CH1 mapping */
  DMA_PER4_I2C0_RX = 0x04,                  /*!< PER4 I2C0 RX mapping */
  DMA_PER5_I2C0_TX = 0x05,                  /*!< PER5 I2C0 TX mapping */
  DMA_PER6_QSPI0_RX = 0x06,                 /*!< PER6 QSPI0 RX mapping */
  DMA_PER7_QSPI0_TX = 0x07,                 /*!< PER7 QSPI0 TX mapping */
  DMA_PER10_SSP0_RX = 0x0A,             /*!< PER10 SSP0 RX mapping */
  DMA_PER11_SSP0_TX = 0x0B,             /*!< PER11 SSP0 TX mapping */
  DMA_PER12_SSP1_RX = 0x0C,             /*!< PER12 SSP1 RX mapping */
  DMA_PER13_SSP1_TX = 0x0D,                 /*!< PER13 SSP1 TX mapping */
  DMA_PER14_UART0_RX = 0x0E,                 /*!< PER14 UART0 RX mapping */
  DMA_PER15_UART0_TX = 0x0F,            /*!< PER15 UART0 TX mapping */
  DMA_PER16_UART1_RX = 0x10,            /*!< PER16 UART1 RX mapping */
  DMA_PER17_UART1_TX = 0x11,            /*!< PER17 UART1 TX mapping */
  DMA_PER24_ADC0 = 0x18,            /*!< PER24 ADC0 mapping */
  DMA_PER26_DAC0 = 0x1A,            /*!< PER26 DAC0 mapping */
  DMA_PER27_DAC1 = 0x1B,            /*!< PER27 DAC1 mapping */
  DMA_PER32_GPT2_0 = 0x20,                  /*!< PER32 GPT2 CH0 mapping */
  DMA_PER33_GPT2_1 = 0x21,                /*!< PER33 GPT2 CH1 mapping */
  DMA_PER34_GPT3_0 = 0x22,            /*!< PER34 GPT3 CH0 mapping */
  DMA_PER35_GPT3_1 = 0x23,            /*!< PER35 GPT3 CH1 mapping */
  DMA_PER36_I2C1_RX = 0x24,            /*!< PER36 I2C1 RX mapping */
  DMA_PER37_I2C1_TX = 0x25,            /*!< PER37 I2C1 TX mapping */
  DMA_PER38_I2C2_RX = 0x26,            /*!< PER38 I2C2 RX mapping */
  DMA_PER39_I2C2_TX = 0x27,            /*!< PER39 I2C2 TX mapping */
  DMA_PER40_SSP2_RX = 0x28,             /*!< PER40 SSP2 RX mapping */
  DMA_PER41_SSP2_TX = 0x29,             /*!< PER41 SSP2 TX mapping */
  DMA_PER42_UART2_RX = 0x2A,            /*!< PER42 UART2 RX mapping */
  DMA_PER43_UART2_TX = 0x2B,            /*!< PER43 UART2 TX mapping */
  DMA_PER56_AES_CRC_IN = 0x38,            /*!< PER56 AES CRC IN mapping */
  DMA_PER57_AES_CRC_OUT = 0x39,            /*!< PER57 AES CRC OUT mapping */
}DMA_PerMapping_Type;

/**  
 *  @brief DMA transfer type definition 
 */ 
typedef enum
{
  DMA_MEM_TO_MEM = 0,                      /*!< Memory to Memory */
  DMA_MEM_TO_PER,                          /*!< Memory to peripheral */
  DMA_PER_TO_MEM,                          /*!< Peripheral to memory */
}DMA_TransfType_Type;

/**  
 *  @brief DMA transfer burst length definition 
 */ 
typedef enum
{
  DMA_ITEM_1 = 0,                          /*!< 1 item */
  DMA_ITEM_4,                              /*!< 4 items */
  DMA_ITEM_8,                              /*!< 8 items */
  DMA_ITEM_16,                             /*!< 16 items */ 
}DMA_BurstLength_Type;

/**  
 *  @brief DMA transfer address increment definition 
 */ 
typedef enum
{
  DMA_ADDR_NOCHANGE =0,                    /*!< Address nochange */
  DMA_ADDR_INC,                            /*!< Address increment */                      
}DMA_AddrInc_Type;

/**  
 *  @brief DMA transfer width definition 
 */ 
typedef enum
{  
  DMA_TRANSF_WIDTH_8 = 1,                  /*!< 8 bits */
  DMA_TRANSF_WIDTH_16,                     /*!< 16 bits */
  DMA_TRANSF_WIDTH_32,                     /*!< 32 bits */
}DMA_TransfWidth_Type;

/**  
 *  @brief DMA transfer handshaking type definition 
 */ 
typedef enum
{
  DMA_HW_HANDSHAKING,                      /*!< Hardware handshaking interface */
  DMA_SW_HANDSHAKING,                      /*!< Software handshaking interface */
}DMA_Handshaking_Type;

/**  
 *  @brief DMA Configuration Structure type definition 
 */  
typedef struct
{
  uint32_t srcDmaAddr;                     /*!< Source address of DMA transfer */

  uint32_t destDmaAddr;                    /*!< Destination address of DMA transfer */

  DMA_TransfType_Type transfType;          /*!< Transfer type and flow control
                                                0: Memory to Memory
                                                1: Memory to peripheral
                                                2: Peripheral to memory */
                                            
  DMA_BurstLength_Type burstLength;        /*!< Number of data items for burst transaction length.
                                                Each item width is as same as tansfer width.
                                                0: 1 item 
                                                1: 4 items
                                                2: 8 items
                                                3: 16 items */

  DMA_AddrInc_Type srcAddrInc;             /*!< Source address increment 
                                                0: No change
                                                1: Increment */
                                         
  DMA_AddrInc_Type destAddrInc;            /*!< Destination address increment 
                                                0: No change
                                                1: Increment */

  DMA_TransfWidth_Type transfWidth;        /*!< Transfer width 
                                                0: reserved
                                                1: 8  bits
                                                2: 16  bits
                                                3: 32  bits*/
                                         
  uint32_t transfLength;                   /*!< Transfer length     
                                                0~8191bytes
                                                maximum transfer length (8k-1) bytes*/                        
}DMA_CFG_Type;

/**  
 *  @brief DMA source/destination type definition 
 */
typedef enum
{
  DMA_SOURCE,                               /*!< Dma source transaction direction */
  DMA_DESTINATION,                         /*!< Dma destination transaction direction */
}DMA_TransDir_Type;

/**  
 *  @brief DMA transaction mode type definition 
 */
typedef enum
{
  DMA_SINGLE,                               /*!< Single transaction */
  DMA_BURST,                               /*!< Burst transaction */
}DMA_TransMode_Type;

/**  
 *  @brief DMA interrupt type definition 
 */
typedef enum
{
  INT_DMA_TRANS_COMPLETE,                  /*!< Dma transfer complete interrupt */
  INT_BLK_TRANS_COMPLETE,                  /*!< Block transfer complete interrupt */
  INT_BUS_ERROR,                           /*!< Bus Error interrupt */
  INT_ADDRESS_ERROR,                       /*!< Address Error interrupt */
  INT_CH_ALL,                              /*!< All interrupts for channelx */                 
}DMA_Int_Type; 

/*@} end of group DMA_Public_Types */

/** @defgroup DMA_Public_Constants
 *  @{
 */ 
                                      
/** @defgroup DMA_Channel        
 *  @{
 */
#define IS_DMA_CHANNEL(CHANNEL)        ((CHANNEL) <= 31)

/*@} end of group DMA_Channel */

/** @defgroup DMA_Tansfer_Direction       
 *  @{
 */
#define IS_DMA_TRANS_DIR(DIR)          (((DIR) == DMA_SOURCE) || ((DIR) == DMA_DESTINATION))
                                           
/*@} end of group DMA_Tansfer_Direction */

/** @defgroup DMA_Tansfer_Mode       
 *  @{
 */
#define IS_DMA_TRANS_MODE(MODE)        (((MODE) == DMA_SINGLE) || ((MODE) == DMA_BURST))
                                           
/*@} end of group DMA_Tansfer_Mode */

/** @defgroup DMA_Tansfer_Block_Size       
 *  @{
 */
#define IS_DMA_BLK_SIZE(BLKSIZE)        ((BLKSIZE) < 8192)

/*@} end of group DMA_Tansfer_Block_Size */

/** @defgroup DMA_Ack_Delay_Cycle       
 *  @{
 */
#define IS_DMA_DELAY_CYCLE(DELAYCYCLE)        ((DELAYCYCLE) < 1024)

/*@} end of group DMA_Ack_Delay_Cycle */

/** @defgroup DMA_Int_Type       
 *  @{
 */
#define IS_DMA_INT(INT)                (((INT) == INT_DMA_TRANS_COMPLETE)  || \
                                        ((INT) == INT_BLK_TRANS_COMPLETE)  || \
                                        ((INT) == INT_BUS_ERROR)  || \
                                        ((INT) == INT_ADDRESS_ERROR) || \
                                        ((INT) == INT_CH_ALL))                                           
/*@} end of group DMA_Int_Type */

/*@} end of group DMA_Public_Macro */

/** @defgroup DMA_Public_FunctionDeclaration
 *  @brief DMA functions declaration
 *  @{
 */
void DMA_ChannelInit(DMA_Channel_Type channelx, DMA_CFG_Type * dmaCfgStruct);
void DMA_Enable(DMA_Channel_Type channelx);
void DMA_Disable(DMA_Channel_Type channelx);
void DMA_Stop(DMA_Channel_Type channelx);
void DMA_SetPeripheralType(DMA_Channel_Type channelx, DMA_PerMapping_Type perDmaInter);
void DMA_SetAckDelayCycle(uint32_t delaycycle);
FlagStatus DMA_GetChannelEnableStatus(DMA_Channel_Type channelx);
void DMA_IntMask(DMA_Channel_Type channelx, DMA_Int_Type intType, IntMask_Type intMask);
FlagStatus DMA_GetChannelIntStatus(DMA_Channel_Type channelx, DMA_Int_Type intType);
void DMA_IntClr(DMA_Channel_Type channelx, DMA_Int_Type intType);
void DMA_IRQHandler(void);

/*@} end of group DMA_Public_FunctionDeclaration */

/*@} end of group DMA */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_DMA_H__ */
