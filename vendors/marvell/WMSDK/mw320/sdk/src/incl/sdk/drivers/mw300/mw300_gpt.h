/** @file mw300_gpt.h
*
*  @brief This file contains GPT driver module header
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

#ifndef __MW300_GPT_H__
#define __MW300_GPT_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  GPT 
 *  @{
 */
  
/** @defgroup GPT_Public_Types GPT_Public_Types
 *  @brief GPT configuration structure type definition
 *  @{
 */

/**  
 *  @brief GPT No. type definition 
 */
typedef enum
{
  GPT0_ID = 0,                      /*!< GPT0 module define */
  GPT1_ID = 1,                    /*!< GPT1 module define */
  GPT2_ID = 2,                      /*!< GPT2 module define */
  GPT3_ID = 3,                    /*!< GPT3 module define */  
}GPT_ID_Type;

/**         
 *  @brief GPT clock source type definition
 */
typedef enum
{
  GPT_CLOCK_0      = 0,         /*!< Clock 0 */
  GPT_CLOCK_1      = 1,         /*!< Clock 1 */
}GPT_ClockSrc_Type;

/**         
 *  @brief Counter value register update mode type definition
 */
typedef enum
{
  GPT_CNT_VAL_UPDATE_NORMAL = 0,     /*!< Counter value register update mode: auto-update normal */
  GPT_CNT_VAL_UPDATE_FAST   = 1,     /*!< Counter value register update mode: auto-update fast */
  GPT_CNT_VAL_UPDATE_OFF    = 3,     /*!< Counter value register update mode: off */
}GPT_CntUpdate_Type;

/**         
 *  @brief GPT input capture edge type definition
 */
typedef enum
{
  GPT_IC_RISING_EDGE      = 0,     /*!< Input capture rising edge selected */
  GPT_IC_FALLING_EDGE     = 1,     /*!< Input capture falling edge selected */
}GPT_ICEdge_Type;

/**         
 *  @brief GPT channel function type definition
 */
typedef enum
{ 
  GPT_CH_FUNC_NO            = 0,    /*!< GPT channel function: no function */
  GPT_CH_FUNC_INPUT         = 1,    /*!< GPT channel function: input capture */
  GPT_CH_FUNC_ONESHOT_PULSE = 4,    /*!< GPT channel function: one_shot edge */
  GPT_CH_FUNC_ONESHOT_EDGE  = 5,    /*!< GPT channel function: one_shot pulse */
  GPT_CH_FUNC_PWM_EDGE      = 6,    /*!< GPT channel function: edge-aligned PWM */
  GPT_CH_FUNC_PWM_CENTER    = 7,    /*!< GPT channel function: center-aligned PWM */
}GPT_ChannelFunc_Type;

/**         
 *  @brief GPT channel number type definition
 */
typedef enum
{
  GPT_CH_0           = 0,        /*!< GPT channel No.: 0 */
  GPT_CH_1           = 1,        /*!< GPT channel No.: 1 */  
  GPT_CH_2           = 2,        /*!< GPT channel No.: 2 */
  GPT_CH_3           = 3,        /*!< GPT channel No.: 3 */    
  GPT_CH_4           = 4,        /*!< GPT channel No.: 4 */
  GPT_CH_5           = 5,        /*!< GPT channel No.: 5 */
}GPT_ChannelNumber_Type;

/**         
 *  @brief GPT channel PWM and one-shot waveform polarity type definition
 */
typedef enum
{
  GPT_CH_WAVE_POL_POS,      /*!< GPT channel waveform polarity: positive */
  GPT_CH_WAVE_POL_NEG       /*!< GPT channel waveform polarity: negative */
}GPT_ChannelWavePol_Type;

/**  
 *  @brief GPT interrupt type definition 
 */
typedef enum
{
  GPT_INT_CH0,                   /*!< GPT channel 0 int flag */
  GPT_INT_CH1,                   /*!< GPT channel 1 int flag */
  GPT_INT_CH2,                   /*!< GPT channel 2 int flag */
  GPT_INT_CH3,                   /*!< GPT channel 3 int flag */
  GPT_INT_CH4,                   /*!< GPT channel 4 int flag */
  GPT_INT_CH5,                   /*!< GPT channel 5 int flag */
  
  GPT_INT_CH0_ERR,             /*!< GPT channel 0 error int flag */
  GPT_INT_CH1_ERR,             /*!< GPT channel 1 error int flag */
  GPT_INT_CH2_ERR,             /*!< GPT channel 2 error int flag */
  GPT_INT_CH3_ERR,             /*!< GPT channel 3 error int flag */
  GPT_INT_CH4_ERR,             /*!< GPT channel 4 error int flag */
  GPT_INT_CH5_ERR,             /*!< GPT channel 5 error int flag */
  
  GPT_INT_CNT_UPP,             /*!< GPT counter upper overflow flag */
                                 
  GPT_INT_DMA0_OF,            /*!< GPT DMA channel 0 overflow flag */
  GPT_INT_DMA1_OF,           /*!< GPT DMA channel 1 overflow flag */ 
  
  GPT_INT_ALL                 /*!< All GPT interrupt flag */
}GPT_INT_Type; 

/**  
 *  @brief GPT status type definition 
 */
typedef enum
{
  GPT_STATUS_CH0,                   /*!< GPT channel 0 int flag */
  GPT_STATUS_CH1,                   /*!< GPT channel 1 int flag */
  GPT_STATUS_CH2,                   /*!< GPT channel 2 int flag */
  GPT_STATUS_CH3,                   /*!< GPT channel 3 int flag */
  GPT_STATUS_CH4,                   /*!< GPT channel 4 int flag */
  GPT_STATUS_CH5,                   /*!< GPT channel 5 int flag */
  
  GPT_STATUS_CH0_ERR,             /*!< GPT channel 0 error int flag */
  GPT_STATUS_CH1_ERR,             /*!< GPT channel 1 error int flag */
  GPT_STATUS_CH2_ERR,             /*!< GPT channel 2 error int flag */
  GPT_STATUS_CH3_ERR,             /*!< GPT channel 3 error int flag */
  GPT_STATUS_CH4_ERR,             /*!< GPT channel 4 error int flag */
  GPT_STATUS_CH5_ERR,             /*!< GPT channel 5 error int flag */
  
  GPT_STATUS_CNT_UPP,             /*!< GPT counter upper overflow flag */
                                 
  GPT_STATUS_DMA0_OF,            /*!< GPT DMA channel 0 overflow flag */
  GPT_STATUS_DMA1_OF,               /*!< GPT DMA channel 1 overflow flag */ 
  
  GPT_STATUS_ALL                 /*!< All GPT interrupt flag */
}GPT_Status_Type; 

/**  
 *  @brief GPT input filter type definition 
 */
typedef enum
{
  GPT_INPUT_FILTER_1,             /*!< GPT input filter : 1 */
  GPT_INPUT_FILTER_2,             /*!< GPT input filter : 2 */
  GPT_INPUT_FILTER_3,             /*!< GPT input filter : 3 */
  GPT_INPUT_FILTER_4,             /*!< GPT input filter : 4 */
  GPT_INPUT_FILTER_5,             /*!< GPT input filter : 5 */
  GPT_INPUT_FILTER_6,             /*!< GPT input filter : 6 */
  GPT_INPUT_FILTER_7,             /*!< GPT input filter : 7 */
}GPT_InputFilter_Type; 

/**  
 *  @brief GPT DMA channel type definition 
 */
typedef enum
{
  GPT_DMA0,             /*!< GPT DMA channel : 0 */
  GPT_DMA1,             /*!< GPT DMA channel : 1 */
}GPT_DMAChannel_Type; 

/**      
 *  @brief GPT config struct type definition  
 */
typedef struct
{
  GPT_ClockSrc_Type clockSrc;        /*!< GPT counter clock source selection 
                                          GPT_CLOCK_0 (0): Clock 0                                     
                                          GPT_CLOCK_1 (1): Clock 1 */  
  
  GPT_CntUpdate_Type cntUpdate;     /*!< GPT counter value update mode selection
                                         GPT_CNT_VAL_UPDATE_NORMAL (0): auto-update normal 
                                         GPT_CNT_VAL_UPDATE_FAST (1): auto-update fast 
                                         GPT_CNT_VAL_UPDATE_OFF (3): update off */
                                          
  uint32_t clockDivider;             /*!< Clock divider value(range: 0~15).
                                          The divided clock is calculated by:
                                          CLK_div = CLK / (2^clockDivider)*/                                      

  uint32_t clockPrescaler;           /*!< Clock prescaler value(range: 0~127).
                                          The divided clock is calculated by : 
                                          CLK_ps = CLK / (clockPrescaler + 1)*/
  
  uint32_t uppVal;                   /*!< Counter overflow value */

}GPT_Config_Type;


/**         
 *  @brief GPT input config type definition
 */
typedef struct
{
  uint32_t sampleClkDivider;           /*!< Input trigger sampling clock divider (range: 0~7). 
                                          It can be calculated by:
                                          SCLK_DIV = SCLK / (2 ^ trigClkDivider) */

  GPT_InputFilter_Type trigFilter;   /*!< Input trigger filtering.
                                          It indicates how many consecutive cycles the input trigger should be sampled 
                                          before it is considered as a valid edge.
                                          GPT_INPUT_FILTER_1 (0): 1 
                                          GPT_INPUT_FILTER_2 (1): 2 
                                          GPT_INPUT_FILTER_3 (2): 3 
                                          GPT_INPUT_FILTER_4 (3): 4
                                          GPT_INPUT_FILTER_5 (4): 5 
                                          GPT_INPUT_FILTER_6 (5): 6 
                                          GPT_INPUT_FILTER_7 (6): 7 */

}GPT_InputConfig_Type;


/**         
 *  @brief GPT channel config type definition
 */
typedef struct
{
  GPT_ChannelWavePol_Type polarity;     /*!< GPT channel PWM and one-shot waveform polarity selection
                                             GPT_CH_WAVE_POL_POS(0): positive 
                                             GPT_CH_WAVE_POL_NEG(1): negative */
  
  uint32_t cntMatchVal0;                /*!< Channel counter match value 0 */
                                      
  uint32_t cntMatchVal1;                /*!< Channel counter match value 1 */
  
}GPT_ChannelOutputConfig_Type;


/*@} end of group GPT_Public_Types definitions */


/** @defgroup GPT_Public_Constants
 *  @{
 */ 

/**         
 *  @brief GPT channel reset definition
 */
#define GPT_RST_CH0           (0x01 << 0)
#define GPT_RST_CH1           (0x01 << 1)
#define GPT_RST_CH2           (0x01 << 2)
#define GPT_RST_CH3           (0x01 << 3)
#define GPT_RST_CH4           (0x01 << 4)
#define GPT_RST_CH5           (0x01 << 5)
#define GPT_RST_ALL_CH         0x3f


/** @defgroup GPT_INT_MASK       
 *  @{
 */     
#define GPT_INT_CH0_MSK              0x00000001
#define GPT_INT_CH1_MSK              0x00000002
#define GPT_INT_CH2_MSK              0x00000004
#define GPT_INT_CH3_MSK              0x00000008
#define GPT_INT_CH4_MSK              0x00000010
#define GPT_INT_CH5_MSK              0x00000020
#define GPT_INT_CH_ALL_MSK           0x0000003F

#define GPT_INT_CH0_ERR_MSK          0x00000100
#define GPT_INT_CH1_ERR_MSK          0x00000200
#define GPT_INT_CH2_ERR_MSK          0x00000400
#define GPT_INT_CH3_ERR_MSK          0x00000800
#define GPT_INT_CH4_ERR_MSK          0x00001000
#define GPT_INT_CH5_ERR_MSK          0x00002000
#define GPT_INT_CH_ALL_ERR_MSK       0x00003F00

#define GPT_INT_CNT_UPP_MSK          0x00010000

#define GPT_INT_DMA0_OVF_MSK         0x01000000
#define GPT_INT_DMA1_OVF_MSK         0x02000000

#define GPT_INT_ALL_MSK              0x03013F3F

/*@} end of group GPT_INT_MASK */

/**
 *  @brief Total channel number 
 */
#define GPT_NUMBER        4

/**
 *  @brief Total channel number 
 */
#define GPT_CHANNEL_NUMBER        6

/**
 *  @brief Total channel number 
 */
#define GPT_DMA_CHANNEL_NUMBER        2

/**
 *  @brief Total interrupt number 
 */
#define GPT_INT_NUMBER            15

 /**
 *  @brief Total status number 
 */
#define GPT_STATUS_NUMBER            15

/**        
 *  @brief GPT modules check
 */
#define IS_GPT_PERIPH(PERIPH)        ((PERIPH) < GPT_NUMBER)
 
/**        
 *  @brief GPT interrupt type check
 */
#define IS_GPT_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= GPT_INT_NUMBER)

/**        
 *  @brief GPT status type check
 */
#define IS_GPT_STATUS_TYPE(STS_TYPE)    ((STS_TYPE) <= GPT_STATUS_NUMBER)
                                                                                         
/**        
 *  @brief GPT channel number check
 */
#define IS_GPT_CHANNEL_ID(CHID)    ((CHID) < GPT_CHANNEL_NUMBER)

/**        
 *  @brief GPT DMA channel number check
 */
#define IS_GPT_DMA_CHANNEL_ID(CHID)    ((CHID) < GPT_DMA_CHANNEL_NUMBER)



/*@} end of group GPT_Public_Constants */

/** @defgroup GPT_Public_Macro
 *  @{
 */


/*@} end of group GPT_Public_Macro */

/** @defgroup GPT_Public_FunctionDeclaration
 *  @brief GPT functions statement
 *  @{
 */


void GPT_Init(GPT_ID_Type gptID, GPT_Config_Type* gptConfig);
void GPT_InputConfig(GPT_ID_Type gptID, GPT_InputConfig_Type* gptInputConfig);
Status GPT_CounterReset(GPT_ID_Type gptID);
void GPT_Start(GPT_ID_Type gptID);
void GPT_Stop(GPT_ID_Type gptID);
void GPT_ChannelFuncSelect(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, GPT_ChannelFunc_Type channelFunc);
void GPT_ChannelOutputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID,GPT_ChannelOutputConfig_Type* gptChannelConfig);
void GPT_ChannelInputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, GPT_ICEdge_Type edgeSel);
void GPT_ChannelReset(GPT_ID_Type gptID, uint32_t resetChannelNo);
void GPT_SetCounterUppVal(GPT_ID_Type gptID, uint32_t uppVal);
void GPT_SetChannelMatchVal0(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cmr0);
void GPT_SetChannelMatchVal1(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cmr1);
void GPT_SetChannelBothMatchVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID,uint32_t cmr0, uint32_t cmr1);
uint32_t GPT_GetCaptureVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID); 
uint32_t GPT_GetCounterVal(GPT_ID_Type gptID);
void GPT_DMACmd(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, FunctionalState state);
void GPT_DMAChannelSelect(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, GPT_ChannelNumber_Type channelID);
void GPT_TrigCmd(GPT_ID_Type gptID, FunctionalState state);
void GPT_TrigConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t triggerDelay);
void GPT_IntMask(GPT_ID_Type gptID, GPT_INT_Type intType, IntMask_Type intMsk);
FlagStatus GPT_GetStatus(GPT_ID_Type gptID, GPT_Status_Type status);
void GPT_StatusClr(GPT_ID_Type gptID, GPT_Status_Type status);
void GPT0_IRQHandler(void);
void GPT1_IRQHandler(void);
void GPT2_IRQHandler(void);
void GPT3_IRQHandler(void);


/*@} end of group GPT_Public_FunctionDeclaration */

/*@} end of group GPT  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_GPT_H__ */
