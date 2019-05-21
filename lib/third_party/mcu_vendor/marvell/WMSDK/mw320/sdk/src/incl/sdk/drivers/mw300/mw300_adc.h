/** @file mw300_adc.h
*
*  @brief This file contains ADC driver module header
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

#ifndef __MW300_ADC_H__
#define __MW300_ADC_H__

/* Includes ------------------------------------------------------------------ */
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  ADC 
 *  @{
 */
  
/** @defgroup ADC_Public_Types ADC_Public_Types
 *  @{
 */

/**  
 *  @brief ADC ID type
 */
typedef enum
{
  ADC0_ID = 0,                                /*!< ADC0 module */
}ADC_ID_Type;

/**  
 *  @brief ADC resolution type
 */
typedef enum
{
  ADC_RESOLUTION_12BIT       = 0,             /*!< 12-bit resolution */
  ADC_RESOLUTION_14BIT       = 1,             /*!< 14-bit resolution */
  ADC_RESOLUTION_16BIT       = 2,             /*!< 16-bit resolution */
  ADC_RESOLUTION_16BIT_AUDIO = 3,             /*!< 16-bit resolution for audio application */
}ADC_Resolution_Type;

/**  
 *  @brief ADC voltage reference source type
 */
typedef enum
{
  ADC_VREF_18          = 0,                   /*!< Internal 1.8V reference */
  ADC_VREF_12          = 1,                   /*!< Internal 1.2V reference */
  ADC_VREF_EXTERNAL    = 2,                   /*!< External single-ended reference though ADC_CH3 */
  ADC_VREF_FILTERED_12 = 3,                   /*!< Internal 1.2V reference with cap filter though ADC_CH3 */
}ADC_VrefSource_Type;

/**  
 *  @brief ADC input buffer gain type
 */
typedef enum
{
  ADC_GAIN_0P5 = 0,                           /*!< Input buffer gain is 0.5 */
  ADC_GAIN_1   = 1,                           /*!< Input buffer gain is 1 */
  ADC_GAIN_2   = 2,                           /*!< Input buffer gain is 2 */
}ADC_Gain_Type;

/**  
 *  @brief ADC clock divider ratio type
 */
typedef enum
{
  ADC_CLOCK_DIVIDER_1  = 1,                   /*!< Clock divider ratio is 1 */
  ADC_CLOCK_DIVIDER_2  = 2,                   /*!< Clock divider ratio is 2 */
  ADC_CLOCK_DIVIDER_3  = 3,                   /*!< Clock divider ratio is 3 */
  ADC_CLOCK_DIVIDER_4  = 4,                   /*!< Clock divider ratio is 4 */
  ADC_CLOCK_DIVIDER_5  = 5,                   /*!< Clock divider ratio is 5 */
  ADC_CLOCK_DIVIDER_6  = 6,                   /*!< Clock divider ratio is 6 */
  ADC_CLOCK_DIVIDER_7  = 7,                   /*!< Clock divider ratio is 7 */
  ADC_CLOCK_DIVIDER_8  = 8,                   /*!< Clock divider ratio is 8 */
  ADC_CLOCK_DIVIDER_9  = 9,                   /*!< Clock divider ratio is 9 */
  ADC_CLOCK_DIVIDER_10 = 10,                  /*!< Clock divider ratio is 10 */
  ADC_CLOCK_DIVIDER_11 = 11,                  /*!< Clock divider ratio is 11 */
  ADC_CLOCK_DIVIDER_12 = 12,                  /*!< Clock divider ratio is 12 */
  ADC_CLOCK_DIVIDER_13 = 13,                  /*!< Clock divider ratio is 13 */
  ADC_CLOCK_DIVIDER_14 = 14,                  /*!< Clock divider ratio is 14 */
  ADC_CLOCK_DIVIDER_15 = 15,                  /*!< Clock divider ratio is 15 */
  ADC_CLOCK_DIVIDER_16 = 16,                  /*!< Clock divider ratio is 16 */
  ADC_CLOCK_DIVIDER_17 = 17,                  /*!< Clock divider ratio is 17 */
  ADC_CLOCK_DIVIDER_18 = 18,                  /*!< Clock divider ratio is 18 */
  ADC_CLOCK_DIVIDER_19 = 19,                  /*!< Clock divider ratio is 19 */
  ADC_CLOCK_DIVIDER_20 = 20,                  /*!< Clock divider ratio is 20 */
  ADC_CLOCK_DIVIDER_21 = 21,                  /*!< Clock divider ratio is 21 */
  ADC_CLOCK_DIVIDER_22 = 22,                  /*!< Clock divider ratio is 22 */
  ADC_CLOCK_DIVIDER_23 = 23,                  /*!< Clock divider ratio is 23 */
  ADC_CLOCK_DIVIDER_24 = 24,                  /*!< Clock divider ratio is 24 */
  ADC_CLOCK_DIVIDER_25 = 25,                  /*!< Clock divider ratio is 25 */
  ADC_CLOCK_DIVIDER_26 = 26,                  /*!< Clock divider ratio is 26 */
  ADC_CLOCK_DIVIDER_27 = 27,                  /*!< Clock divider ratio is 27 */
  ADC_CLOCK_DIVIDER_28 = 28,                  /*!< Clock divider ratio is 28 */
  ADC_CLOCK_DIVIDER_29 = 29,                  /*!< Clock divider ratio is 29 */
  ADC_CLOCK_DIVIDER_30 = 30,                  /*!< Clock divider ratio is 30 */
  ADC_CLOCK_DIVIDER_31 = 31,                  /*!< Clock divider ratio is 31 */
  ADC_CLOCK_DIVIDER_32 = 32,                  /*!< Clock divider ratio is 32 */
}ADC_ClockDivider_Type;

/**  
 *  @brief ADC operation mode type
 */
typedef enum
{
  ADC_MODE_ADC     = 0,                       /*!< ADC mode */
  ADC_MODE_TSENSOR = 1,                       /*!< Temperature sensor mode */
}ADC_Mode_Type;

/**  
 *  @brief ADC input mode type
 */
typedef enum
{
  ADC_INPUT_SINGLE_ENDED = 0,                 /*!< Single-ended mode */
  ADC_INPUT_DIFFERENTIAL = 1,                 /*!< Differential mode */
}ADC_InputMode_Type;

/**  
 *  @brief ADC conversion mode type
 */
typedef enum
{
  ADC_CONVERSION_ONE_SHOT   = 0,              /*!< One shot mode */
  ADC_CONVERSION_CONTINUOUS = 1,              /*!< Continuous mode */
}ADC_ConversionMode_Type;

/**  
 *  @brief ADC scan length type
 */
typedef enum
{
  ADC_SCAN_LENGTH_1  = 0,                     /*!< Scan length is 1 */
  ADC_SCAN_LENGTH_2  = 1,                     /*!< Scan length is 2 */
  ADC_SCAN_LENGTH_3  = 2,                     /*!< Scan length is 3 */
  ADC_SCAN_LENGTH_4  = 3,                     /*!< Scan length is 4 */
  ADC_SCAN_LENGTH_5  = 4,                     /*!< Scan length is 5 */
  ADC_SCAN_LENGTH_6  = 5,                     /*!< Scan length is 6 */
  ADC_SCAN_LENGTH_7  = 6,                     /*!< Scan length is 7 */
  ADC_SCAN_LENGTH_8  = 7,                     /*!< Scan length is 8 */
  ADC_SCAN_LENGTH_9  = 8,                     /*!< Scan length is 9 */
  ADC_SCAN_LENGTH_10 = 9,                     /*!< Scan length is 10 */
  ADC_SCAN_LENGTH_11 = 10,                    /*!< Scan length is 11 */
  ADC_SCAN_LENGTH_12 = 11,                    /*!< Scan length is 12 */
  ADC_SCAN_LENGTH_13 = 12,                    /*!< Scan length is 13 */
  ADC_SCAN_LENGTH_14 = 13,                    /*!< Scan length is 14 */
  ADC_SCAN_LENGTH_15 = 14,                    /*!< Scan length is 15 */
  ADC_SCAN_LENGTH_16 = 15,                    /*!< Scan length is 16 */
}ADC_ScanLength_Type;

/**  
 *  @brief ADC scan channel type
 */
typedef enum
{
  ADC_SCAN_CHANNEL_0  = 0,                    /*!< Scan channel 0 */
  ADC_SCAN_CHANNEL_1  = 1,                    /*!< Scan channel 1 */
  ADC_SCAN_CHANNEL_2  = 2,                    /*!< Scan channel 2 */
  ADC_SCAN_CHANNEL_3  = 3,                    /*!< Scan channel 3 */
  ADC_SCAN_CHANNEL_4  = 4,                    /*!< Scan channel 4 */
  ADC_SCAN_CHANNEL_5  = 5,                    /*!< Scan channel 5 */
  ADC_SCAN_CHANNEL_6  = 6,                    /*!< Scan channel 6 */
  ADC_SCAN_CHANNEL_7  = 7,                    /*!< Scan channel 7 */
  ADC_SCAN_CHANNEL_8  = 8,                    /*!< Scan channel 8 */
  ADC_SCAN_CHANNEL_9  = 9,                    /*!< Scan channel 9 */
  ADC_SCAN_CHANNEL_10 = 10,                   /*!< Scan channel 10 */
  ADC_SCAN_CHANNEL_11 = 11,                   /*!< Scan channel 11 */
  ADC_SCAN_CHANNEL_12 = 12,                   /*!< Scan channel 12 */
  ADC_SCAN_CHANNEL_13 = 13,                   /*!< Scan channel 13 */
  ADC_SCAN_CHANNEL_14 = 14,                   /*!< Scan channel 14 */
  ADC_SCAN_CHANNEL_15 = 15,                   /*!< Scan channel 15 */
}ADC_ScanChannel_Type;

/**  
 *  @brief ADC channel source type
 */
typedef enum
{
  ADC_CH0             = 0,                    /*!< Single-ended mode, channel[0] and vssa */
  ADC_CH1             = 1,                    /*!< Single-ended mode, channel[1] and vssa */
  ADC_CH2             = 2,                    /*!< Single-ended mode, channel[2] and vssa */
  ADC_CH3             = 3,                    /*!< Single-ended mode, channel[3] and vssa */
  ADC_CH4             = 4,                    /*!< Single-ended mode, channel[4] and vssa */
  ADC_CH5             = 5,                    /*!< Single-ended mode, channel[5] and vssa */
  ADC_CH6             = 6,                    /*!< Single-ended mode, channel[6] and vssa */
  ADC_CH7             = 7,                    /*!< Single-ended mode, channel[7] and vssa */
  ADC_VBATS           = 8,                    /*!< Single-ended mode, vbat_s and vssa */
  ADC_VREF            = 9,                    /*!< Single-ended mode, vref_12 and vssa */
  ADC_DACA            = 10,                   /*!< Single-ended mode, daca and vssa */
  ADC_DACB            = 11,                   /*!< Single-ended mode, dacb and vssa */
  ADC_VSSA            = 12,                   /*!< Single-ended mode, vssa and vssa */
  ADC_TEMPP           = 15,                   /*!< Single-ended mode, temp_p and vssa */
  ADC_CH0_CH1         = 0,                    /*!< Differential mode, channel[0] and channel[1] */
  ADC_CH2_CH3         = 1,                    /*!< Differential mode, channel[2] and channel[3] */
  ADC_CH4_CH5         = 2,                    /*!< Differential mode, channel[4] and channel[5] */
  ADC_CH6_CH7         = 3,                    /*!< Differential mode, channel[6] and channel[7] */
  ADC_DACA_DACB       = 4,                    /*!< Differential mode, daca and dacb */
  ADC_VOICEP_VOICEN   = 5,                    /*!< Differential mode, voice_p and voice_n */
  ADC_TEMPP_TEMPN     = 15,                   /*!< Differential mode, temp_p and temp_n */
}ADC_ChannelSource_Type;

/**  
 *  @brief ADC average length type
 */
typedef enum
{
  ADC_AVERAGE_NONE = 0,                       /*!< Average length: no average */
  ADC_AVERAGE_2    = 1,                       /*!< Average length: 2 */
  ADC_AVERAGE_4    = 2,                       /*!< Average length: 4 */
  ADC_AVERAGE_8    = 3,                       /*!< Average length: 8 */
  ADC_AVERAGE_16   = 4,                       /*!< Average length: 16 */
}ADC_AverageLength_Type;

/**  
 *  @brief ADC result width type
 */
typedef enum
{
  ADC_RESULT_WIDTH_16 = 0,                    /*!< 16-bit final result buffer width */ 
  ADC_RESULT_WIDTH_32 = 1,                    /*!< 32-bit final result buffer width */
}ADC_ResultWidth_Type;

/**  
 *  @brief ADC temperature sensor mode type
 */
typedef enum
{
  ADC_SENSOR_INTERNAL = 0,                    /*!< Internal diode mode */
  ADC_SENSOR_EXTERNAL = 1,                    /*!< External diode mode */
}ADC_TSensorMode_Type;

/**  
 *  @brief ADC calibration voltage reference type
 */
typedef enum
{
  ADC_CAL_VREF_INTERNAL = 0,                  /*!< Internal vref as input for calibration */
  ADC_CAL_VREF_EXTERNAL = 1,                  /*!< External vref as input for calibration */
}ADC_CalVref_Type;

/**  
 *  @brief ADC DMA threshold type
 */
typedef enum
{
  ADC_DMA_THRESHOLD_1  = 0,                   /*!< DMA threshold: 1 data */
  ADC_DMA_THRESHOLD_4  = 1,                   /*!< DMA threshold: 4 data */
  ADC_DMA_THRESHOLD_8  = 2,                   /*!< DMA threshold: 8 data */
  ADC_DMA_THRESHOLD_16 = 3,                   /*!< DMA threshold: 16 data */
}ADC_DmaThreshold_Type;

/**  
 *  @brief ADC trigger source type
 */
typedef enum
{
  ADC_TRIGGER_SOURCE_GPT    = 0,              /*!< Trigger source 0: GPT0 for ADC0 */
  ADC_TRIGGER_SOURCE_ACOMP  = 1,              /*!< Trigger source 1: ACOMP0 for ADC0 */
  ADC_TRIGGER_SOURCE_GPIO40 = 2,              /*!< Trigger source 2: GPIO40 */
  ADC_TRIGGER_SOURCE_GPIO41 = 3,              /*!< Trigger source 3: GPIO41 */
}ADC_TrigSource_Type;

/**  
 *  @brief ADC audio pga gain type 
 */
typedef enum
{
  ADC_AUDIO_GAIN_4  = 0,                      /*!< Audio pga gain is 4 */
  ADC_AUDIO_GAIN_8  = 1,                      /*!< Audio pga gain is 8 */
  ADC_AUDIO_GAIN_16 = 2,                      /*!< Audio pga gain is 16 */
  ADC_AUDIO_GAIN_32 = 3,                      /*!< Audio pga gain is 32 */
}ADC_AudioGain_Type;

/**  
 *  @brief ADC input voice level threshold type
 */
typedef enum 
{
  ADC_VOICE_LEVEL_0 = 0,                      /*!< Input voice level >+255LSB or <-256LSB */
  ADC_VOICE_LEVEL_1 = 1,                      /*!< Input voice level >+511LSB or <-512LSB */
  ADC_VOICE_LEVEL_2 = 2,                      /*!< Input voice level >+1023LSB or <-1024LSB */
  ADC_VOICE_LEVEL_3 = 3,                      /*!< Input voice level >+2047LSB or <-2048LSB */
}ADC_VoiceLevel_Type;

/**  
 *  @brief ADC interrupt type
 */
typedef enum
{
  ADC_RDY,                                    /*!< Conversion data ready interrupt */
  ADC_GAINSAT,                                /*!< Gain correction saturation interrupt */
  ADC_OFFSAT,                                 /*!< Offset correction saturation interrupt */
  ADC_DATASAT_NEG,                            /*!< Data negative saturation interrupt */
  ADC_DATASAT_POS,                            /*!< Data positive saturation interrupt */
  ADC_FIFO_OVERRUN,                           /*!< FIFO overrun interrupt */
  ADC_FIFO_UNDERRUN,                          /*!< FIFO underrun interrupt */
  ADC_INT_ALL,                                /*!< All ADC interrupts */
}ADC_INT_Type;

/**  
 *  @brief ADC status type
 */
typedef enum
{
  ADC_STATUS_RDY,                             /*!< Conversion data ready status */
  ADC_STATUS_GAINSAT,                         /*!< Gain correction saturation status */
  ADC_STATUS_OFFSAT,                          /*!< Offset correction saturation status */
  ADC_STATUS_DATASAT_NEG,                     /*!< Data negative saturation status */
  ADC_STATUS_DATASAT_POS,                     /*!< Data positive saturation status */
  ADC_STATUS_FIFO_OVERRUN,                    /*!< FIFO overrun status */
  ADC_STATUS_FIFO_UNDERRUN,                   /*!< FIFO underrun status */
  ADC_STATUS_ACTIVE,                          /*!< Conversion active status */
  ADC_STATUS_FIFO_NE,                         /*!< FIFO not empty status */
  ADC_STATUS_FIFO_FULL,                       /*!< FIFO full status */
}ADC_Status_Type;

/**  
 *  @brief ADC configure type 
 */
typedef struct
{
  ADC_Resolution_Type adcResolution;          /*!< Configure ADC resolution
                                                   ADC_RESOLUTION_12BIT (0): 12-bit resolution
                                                   ADC_RESOLUTION_14BIT (1): 14-bit resolution
                                                   ADC_RESOLUTION_16BIT (2): 16-bit resolution
                                                   ADC_RESOLUTION_16BIT_AUDIO (3): 16-bit resolution for audio application */
  
  ADC_VrefSource_Type adcVrefSource;          /*!< Configure ADC reference source
                                                   ADC_VREF_18 (0): Internal 1.8V reference
                                                   ADC_VREF_12 (1): Internal 1.2V reference
                                                   ADC_VREF_EXTERNAL (2): External single-ended reference though ADC_CH3
                                                   ADC_VREF_FILTERED_12 (3): Internal 1.2V reference with cap filter though ADC_CH3 */
  
  ADC_Gain_Type adcGainSel;                   /*!< Configure ADC input buffer gain
                                                   ADC_GAIN_0P5 (0): Input buffer gain is 0.5 
                                                   ADC_GAIN_1 (1): Input buffer gain is 1
                                                   ADC_GAIN_2 (2): Input buffer gain is 2 */
  
  ADC_ClockDivider_Type adcClockDivider;      /*!< Configure ADC clock divider ratio
                                                   ADC_CLOCK_DIVIDER_4 (4): Clock divider ratio is 4 
                                                   ADC_CLOCK_DIVIDER_8 (8): Clock divider ratio is 8 
                                                   ADC_CLOCK_DIVIDER_16 (16): Clock divider ratio is 16 
                                                   ADC_CLOCK_DIVIDER_32 (32): Clock divider ratio is 32 */
  
}ADC_CFG_Type;

/*@} end of group ADC_Public_Types definitions */

/** @defgroup ADC_Public_Constants
 *  @{
 */ 

/**
 *  @brief Total ADC module number 
 */
#define ADC_NUMBER                2

/**
 *  @brief Total ADC interrupt number 
 */
#define ADC_INT_NUMBER            7

/**
 *  @brief Total ADC status number 
 */
#define ADC_STATUS_NUMBER         10

/**        
 *  @brief ADC module check
 */
#define IS_ADC_PERIPH(PERIPH)           ( ((PERIPH) >= 0) && ((PERIPH) < ADC_NUMBER) )

/**        
 *  @brief ADC interrupt type check
 */
#define IS_ADC_INT_TYPE(INT_TYPE)       ( ((INT_TYPE) >= 0) && ((INT_TYPE) <= ADC_INT_NUMBER) )

/**        
 *  @brief ADC status type check
 */
#define IS_ADC_STATUS_TYPE(STS_TYPE)    ( ((STS_TYPE) >= 0) && ((STS_TYPE) < ADC_STATUS_NUMBER) )

/*@} end of group ADC_Public_Constants */

/** @defgroup ADC_Public_Macro
 *  @{
 */

/*@} end of group ADC_Public_Macro */

/** @defgroup ADC_Public_FunctionDeclaration
 *  @brief ADC functions statement
 *  @{
 */
void ADC_Init(ADC_ID_Type adcID, ADC_CFG_Type* adcConfigSet);

void ADC_ModeSelect(ADC_ID_Type adcID, ADC_Mode_Type adcMode);
void ADC_InputModeSelect(ADC_ID_Type adcID, ADC_InputMode_Type adcInputMode);
void ADC_ConversionModeSelect(ADC_ID_Type adcID, ADC_ConversionMode_Type adcConversionMode);
void ADC_SetScanLength(ADC_ID_Type adcID, ADC_ScanLength_Type adcScanLength);
void ADC_SetScanChannel(ADC_ID_Type adcID, ADC_ScanChannel_Type adcScanChannel, ADC_ChannelSource_Type adcChannelSource);
void ADC_SetBufferMode(ADC_ID_Type adcID, FunctionalState adcInputBuf);
void ADC_SetAverageLength(ADC_ID_Type adcID, ADC_AverageLength_Type adcAverageLength);
void ADC_ResultWidthConfig(ADC_ID_Type adcID, ADC_ResultWidth_Type adcResultWidth);
void ADC_TSensorConfig(ADC_ID_Type adcID, ADC_TSensorMode_Type adcTSensorMode);

void ADC_Reset(ADC_ID_Type adcID);
void ADC_Enable(ADC_ID_Type adcID);
void ADC_Disable(ADC_ID_Type adcID);
void ADC_ConversionStart(ADC_ID_Type adcID);
void ADC_ConversionStop(ADC_ID_Type adcID);

Status ADC_SelfCalibration(ADC_ID_Type adcID, ADC_CalVref_Type adcCalVref);
Status ADC_UserCalibration(ADC_ID_Type adcID, int16_t adcOffsetCal, int16_t adcGainCal);
void ADC_GetSelfCalibrationData(ADC_ID_Type adcID, int16_t* adcOffsetCal, int16_t* adcGainCal);
void ADC_ResetSelfCalibratonData(ADC_ID_Type adcID);

uint8_t ADC_GetFifoDataCount(ADC_ID_Type adcID);
uint32_t ADC_GetConversionResult(ADC_ID_Type adcID);

void ADC_DmaThresholdSel(ADC_ID_Type adcID, ADC_DmaThreshold_Type dmaThreshold);
void ADC_DmaCmd(ADC_ID_Type adcID, FunctionalState cmd);

void ADC_TriggerSourceSel(ADC_ID_Type adcID, ADC_TrigSource_Type trigSource);
void ADC_TriggerCmd(ADC_ID_Type adcID, FunctionalState cmd);

void ADC_AudioGainSel(ADC_ID_Type adcID, ADC_AudioGain_Type adcAudioGain);
void ADC_AudioCmd(ADC_ID_Type adcID, FunctionalState cmd);
void ADC_VoiceLevelSel(ADC_ID_Type adcID, ADC_VoiceLevel_Type adcVoiceLevel);
void ADC_VoiceLevelDetectCmd(ADC_ID_Type adcID, FunctionalState cmd);

FlagStatus ADC_GetStatus(ADC_ID_Type adcID, ADC_Status_Type statusType);
IntStatus ADC_GetIntStatus(ADC_ID_Type adcID, ADC_INT_Type intType);
void ADC_IntMask(ADC_ID_Type adcID, ADC_INT_Type intType, IntMask_Type intMask);
void ADC_IntClr(ADC_ID_Type adcID, ADC_INT_Type intType);

/*@} end of group ADC_Public_FunctionDeclaration */

/*@} end of group ADC */

/*@} end of group MW300_Periph_Driver */
#endif
