/** @file mw300_dac.h
*
*  @brief This file contains DAC driver module header
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

/****************************************************************************//**
 * @file     mw300_dac.h
 * @brief    DAC driver module header file.
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *******************************************************************************/

#ifndef __MW300_DAC_H__
#define __MW300_DAC_H__

/* Includes ------------------------------------------------------------------ */
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  DAC 
 *  @{
 */
  
/** @defgroup DAC_Public_Types DAC_Public_Types
 *  @{
 */


/**  
 *  @brief DAC wave generation type 
 */

typedef enum
{
  DAC_WAVE_NORMAL             = 0,    /*!< No predefined waveform, effective to A or B channel */
  DAC_WAVE_TRIANGLE           = 1,    /*!< Triangle wave, effective only to A channel */
  DAC_WAVE_SINE               = 2,    /*!< Sine wave, effective only to A channel */
  DAC_WAVE_NOISE_DIFFERENTIAL = 3,    /*!< Noise wave,  effective only to A channel;  
                                                                           Differential mode, one's complemental code from A data, 
                                                                           effective only to B channel */
}DAC_ChannelWave_Type;


/**  
 *  @brief DAC output range type 
 */
typedef enum
{
  DAC_RANGE_SMALL  = 0,                   /*!< DAC output small range */
  DAC_RANGE_MIDDLE = 1,                   /*!< DAC output middle range */
  DAC_RANGE_LARGE  = 3                    /*!< DAC output large range */
}DAC_ChannelRange_Type;


/**  
 *  @brief DAC pad output type 
 */
typedef enum
{
  DAC_OUTPUT_INTERNAL,                    /*!< Enable internal output but disable output to pad */
  DAC_OUTPUT_PAD                          /*!< Enable output to pad but disable internal output */
}DAC_ChannelOutput_Type;

/**  
 *  @brief DAC conversion rate type 
 */
typedef enum
{
  DAC_CONVERSION_RATE_62P5KHZ,            /*!< Conversion rate 62.5KHz */  
  DAC_CONVERSION_RATE_125KHZ,             /*!< Conversion rate 125KHz */  
  DAC_CONVERSION_RATE_250KHZ,             /*!< Conversion rate 250KHz */  
  DAC_CONVERSION_RATE_500KHZ,             /*!< Conversion rate 500KHz */
}DAC_ConversionRate_Type;

/**  
 *  @brief DAC reference source type
 */
typedef enum
{
  DAC_VREF_INTERNAL,                      /*!< Internal Vref source */  
  DAC_VREF_EXTERNAL                       /*!< External Vref source  */  
}DAC_VrefSource_Type;

/**  
 *  @brief DAC channel ID type 
 */
typedef enum
{
  DAC_CH_A,	                          /*!< Index for DAC channel A */
  DAC_CH_B 	                          /*!< Index for DAC channel B */
}DAC_ChannelID_Type;

/**  
 *  @brief DAC timing correlated type 
 */
typedef enum
{
  DAC_NON_TIMING_CORRELATED,                  /*!< DAC non-timing-correlated mode */
  DAC_TIMING_CORRELATED 	              /*!< DAC timing-correlated mode */
}DAC_TimingMode_Type;

/**  
 *  @brief DAC channel status type 
 */
typedef enum
{
  DAC_NEW_DATA_READY,                     /*!< DAC channel has new data for conversion */
  DAC_CONVERSION_DONE                     /*!< DAC channel conversion is done */    
}DAC_ChannelStatus_Type;

/**
 *  @brief DAC trigger source type
 */
typedef enum
{
  DAC_GPT2_TRIGGER     = 0,               /*!< GPT2 trigger source */
  DAC_GPT3_TRIGGER     = 1,               /*!< GPT3 trigger source */
  DAC_GPIO40_TRIGGER   = 2,               /*!< GPIO40 trigger source */
  DAC_GPIO41_TRIGGER   = 3                /*!< GPIO41 trigger source */
}DAC_TriggerSource_Type;

/**
 * @brief DAC GPIO trigger type
 */
typedef enum
{
  DAC_GPIO_TRIGGER_RISING    = 1,         /*!< Rising edge trigger*/
  DAC_GPIO_TRIGGER_FAILING   = 2,         /*!< Failing edge trigger*/
  DAC_GPIO_TRIGGER_BOTH      = 3          /*!< Rising and Failing edge trigger*/
}DAC_GPIOTrigger_Type;
/**  
 *  @brief DAC triangle maximum amplitude type
 */
typedef enum
{
  DAC_TRIANGLE_AMPLITUDE_63  = 0,         /*!< DAC triangle amplitude 63 lsb */
  DAC_TRIANGLE_AMPLITUDE_127,             /*!< DAC triangle amplitude 127 lsb */
  DAC_TRIANGLE_AMPLITUDE_191,             /*!< DAC triangle amplitude 191 lsb */
  DAC_TRIANGLE_AMPLITUDE_255,             /*!< DAC triangle amplitude 255 lsb */
  DAC_TRIANGLE_AMPLITUDE_319,             /*!< DAC triangle amplitude 319 lsb */
  DAC_TRIANGLE_AMPLITUDE_383,             /*!< DAC triangle amplitude 383 lsb */
  DAC_TRIANGLE_AMPLITUDE_447,             /*!< DAC triangle amplitude 447 lsb */
  DAC_TRIANGLE_AMPLITUDE_511,             /*!< DAC triangle amplitude 511 lsb */
  DAC_TRIANGLE_AMPLITUDE_575,             /*!< DAC triangle amplitude 575 lsb */
  DAC_TRIANGLE_AMPLITUDE_639,             /*!< DAC triangle amplitude 639 lsb */
  DAC_TRIANGLE_AMPLITUDE_703,             /*!< DAC triangle amplitude 703 lsb */
  DAC_TRIANGLE_AMPLITUDE_767,             /*!< DAC triangle amplitude 767 lsb */
  DAC_TRIANGLE_AMPLITUDE_831,             /*!< DAC triangle amplitude 831 lsb */
  DAC_TRIANGLE_AMPLITUDE_895,             /*!< DAC triangle amplitude 895 lsb */
  DAC_TRIANGLE_AMPLITUDE_959,             /*!< DAC triangle amplitude 959 lsb */
  DAC_TRIANGLE_AMPLITUDE_1023             /*!< DAC triangle amplitude 1023 lsb */
}DAC_TriangleMamp_Type;

/**  
 *  @brief DAC triangle step size type 
 */
typedef enum
{
  DAC_TRIANGLE_STEPSIZE_1 = 0,            /*!< DAC triangle step size 1 lsb */
  DAC_TRIANGLE_STEPSIZE_3,                /*!< DAC triangle step size 3 lsb */
  DAC_TRIANGLE_STEPSIZE_15,               /*!< DAC triangle step size 15 lsb */
  DAC_TRIANGLE_STEPSIZE_511               /*!< DAC triangle step size 511 lsb */
}DAC_TriangleStepSize_Type;

/**  
 *  @brief DAC triangle waveform type 
 */
typedef enum
{
  DAC_TRIANGLE_FULL,                      /*!< DAC full triangle waveform */
  DAC_TRIANGLE_HALF                       /*!< DAC half triangle waveform */
}DAC_TriangleWaveform_Type;

/**  
 *  @brief DAC interrupt type definition 
 */
typedef enum
{
  DAC_CH_A_RDY,                           /*!< DAC channel a data ready flag */
  DAC_CH_B_RDY,                           /*!< DAC channel b data ready flag */
  DAC_CH_A_TO,                            /*!< DAC channel a timeout flag */
  DAC_CH_B_TO,                            /*!< DAC channel b timeout flag */
  DAC_TRIA_OVERFLOW,                      /*!< DAC triangle wave configuration overflow flag */
  DAC_INT_ALL                             /*!< All the DAC interrupt flags */   
}DAC_INT_Type; 

/**  
 *  @brief DAC status type definition 
 */
typedef enum
{
  DAC_STATUS_CH_A_RDY,                    /*!< DAC channel a data ready status */
  DAC_STATUS_CH_B_RDY,                    /*!< DAC channel b data ready status */
  DAC_STATUS_CH_A_TO,                     /*!< DAC channel a timeout status */
  DAC_STATUS_CH_B_TO,                     /*!< DAC channel b timeout status */
  DAC_STATUS_TRIA_OVERFLOW,               /*!< DAC triangle wave configuration overflow status */
  DAC_STATUS_CH_A_DV,                     /*!< DAC channel a conversion status */
  DAC_STATUS_CH_B_DV,                     /*!< DAC channel b conversion status */   
}DAC_Status_Type; 

/**  
 *  @brief DAC global configure type 
 */
typedef struct
{
  DAC_ConversionRate_Type conversionRate; /*!< Configure DAC conversion rate
                                                                                  DAC_CONVERSION_RATE_62P5KHZ (0): Conversion rate 62.5KHz
                                                                                  DAC_CONVERSION_RATE_125KHZ (1): Conversion rate 125KHz
                                                                                  DAC_CONVERSION_RATE_250KHZ (2): Conversion rate 250KHz
                                                                                  DAC_CONVERSION_RATE_500KHZ (3): Conversion rate 500KHz */
                                                                                  
  DAC_VrefSource_Type refSource;          /*!< Configure DAC vref source
                                                                                  DAC_VREF_INTERNAL (0): Internal Vref source
                                                                                  DAC_VREF_EXTERNAL (1): External Vref source from GPIO5 */

  DAC_ChannelRange_Type rangeSelect;      /*!< Configure channel output range
                                                                                  DAC_RANGE_SMALL (0): DAC output small range
                                                                                  DAC_RANGE_MIDDLE (1): DAC output middle range
                                                                                  DAC_RANGE_LARGE (2):  DAC output large range */
}DAC_CFG_Type;

/**  
 *  @brief DAC channel configure type 
 */
typedef struct
{
  DAC_ChannelWave_Type waveMode;          /*!< Configure channel wave generation mode
                                                                                  DAC_WAVE_NORMAL (0): No predefined waveform, effective to A or B channel
                                                                                  DAC_WAVE_TRIANGLE (1): Triangle wave, effective only to A channel 
                                                                                  DAC_WAVE_SINE (2): Sine wave, effective only to A channel
                                                                                  DAC_WAVE_NOISE_DIFFERENTIAL (3): Noise wave,  effective only to A channel; or
                                                                                                                                           Differential mode, effective only to B channel */                                                                                
  DAC_ChannelOutput_Type outMode;         /*!< Configure channel output mode
                                                                                  DAC_OUTPUT_INTERNAL (0): Enable internal output but disable output to pad
                                                                                  DAC_OUTPUT_PAD (1):  Enable output to pad but disable internal output */
                                                                                  
  DAC_TimingMode_Type timingMode;         /*!< Configure channel timing mode
                                                                                  DAC_NON_TIMING_CORRELATED (0): non-timing-correlated mode
                                                                                  DAC_TIMING_CORRELATED (1): timing-correlated mode */
  
}DAC_ChannelConfig_Type;

/**  
 *  @brief DAC triangle configure type 
 */
typedef struct
{
  DAC_TriangleMamp_Type triangleMamp;             /*!< Configure triangle maximum value
                                                                                                DAC_TRIANGLE_AMPLITUDE_63 (0): 63 lsb
                                                                                                DAC_TRIANGLE_AMPLITUDE_127 (1): 127 lsb 
                                                                                                ... ...
                                                                                                DAC_TRIANGLE_AMPLITUDE_959 (14) : 959 lsb
                                                                                                DAC_TRIANGLE_AMPLITUDE_1023 (15) : 1023 lsb */	
                                                                                                
  DAC_TriangleStepSize_Type triangleStepSize;     /*!< Configure triangle step size 
                                                                                                DAC_TRIANGLE_STEPSIZE_1 (0): 1 lsb
                                                                                                DAC_TRIANGLE_STEPSIZE_3 (1): 3 lsb
                                                                                                DAC_TRIANGLE_STEPSIZE_15 (2): 15 lsb
                                                                                                DAC_TRIANGLE_STEPSIZE_511 (3): 511 lsb */
                                                                                                
  DAC_TriangleWaveform_Type triangleWaveform;     /*!< Configure triangle waveform type
                                                                                                DAC_TRIANGLE_FULL (0): Full triangle waveform 
                                                                                                DAC_TRIANGLE_HALF (1): Half triangle waveform */
                                                                                                
  uint32_t triangleBase;                          /*!< Configure triangle minimum value */
}DAC_TriangleConfig_Type;



/*@} end of group DAC_Public_Types definitions */


/** @defgroup DAC_Public_Constants
 *  @{
 */

/**
 *  @brief Total channel number 
 */
#define DAC_CHANNEL_NUMBER        2

/**
 *  @brief Total interrupt number 
 */
#define DAC_INT_NUMBER            5

 /**
 *  @brief Total status number 
 */
#define DAC_STATUS_NUMBER         7

/**        
 *  @brief DAC interrupt type check
 */
#define IS_DAC_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= DAC_INT_NUMBER)

/**        
 *  @brief DAC status type check
 */
#define IS_DAC_STATUS_TYPE(STS_TYPE)    ((STS_TYPE) < DAC_STATUS_NUMBER)

/**        
 *  @brief DAC channel number check
 */
#define IS_DAC_CHANNEL_ID(CHID)    ((CHID) < DAC_CHANNEL_NUMBER)

/**        
 *  @brief DAC channel A wave mode check
 */
#define IS_DAC_CHANNEL_A_WAVE(CH_WAVE)   ((CH_WAVE) <= 3)

/**        
 *  @brief DAC channel B wave mode check
 */
#define IS_DAC_CHANNEL_B_WAVE(CH_WAVE)   (((CH_WAVE) == 0)||((CH_WAVE) == 3))

/**
 *  @brief DAC trigger source check
 */
#define IS_DAC_TRIGGER_SOURCE(TR_SOURCE)  ((TR_SOURCE>=0)&&(TR_SOURCE<4))

/**
 *  @brief DAC GPIO trigger type check
 */
#define IS_DAC_GPIO_TRIGGER(GT_Type)     ((GT_Type>0)&&(GT_Type<4))

/*@} end of group DAC_Public_Constants */

/** @defgroup DAC_Public_Macro
 *  @{
 */

/*@} end of group DAC_Public_Macro */


/** @defgroup DAC_Public_FunctionDeclaration
 *  @brief DAC functions statement
 *  @{
 */

void DAC_Init(DAC_CFG_Type* dacConfigSet);
void DAC_TriangleConfig(DAC_TriangleConfig_Type* triangleConfig);
void DAC_ChannelConfig(DAC_ChannelID_Type dacChId, DAC_ChannelConfig_Type* channelConfig);
void DAC_SetChannelData(DAC_ChannelID_Type dacChId, uint16_t channelData);

void DAC_ChannelEnable(DAC_ChannelID_Type dacChId);
void DAC_ChannelDisable(DAC_ChannelID_Type dacChId);
void DAC_ChannelReset(DAC_ChannelID_Type dacChId);

void DAC_DmaCmd(DAC_ChannelID_Type dacChId, FunctionalState newCmd);

FlagStatus DAC_GetStatus(DAC_Status_Type statusType);

IntStatus DAC_GetIntStatus(DAC_INT_Type intType);
void DAC_IntMask(DAC_INT_Type intType, IntMask_Type intMask);
void DAC_IntClr(DAC_INT_Type intType);

void DAC_TriggerSourceConfig(DAC_ChannelID_Type dacChId, DAC_TriggerSource_Type triggerSource);
void DAC_GPIOTriggerTypeConfig(DAC_ChannelID_Type dacChId, DAC_GPIOTrigger_Type triggerType);
void DAC_TriggerCmd(DAC_ChannelID_Type dacChId, FunctionalState newCmd);
/*@} end of group DAC_Public_FunctionDeclaration */

/*@} end of group DAC  */

/*@} end of group MW300_Periph_Driver */
#endif
