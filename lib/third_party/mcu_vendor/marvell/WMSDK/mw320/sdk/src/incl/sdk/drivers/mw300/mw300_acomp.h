/** @file mw300_acomp.h
*
*  @brief This file contains ACOMP driver module header
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

#ifndef __MW300_ACOMP_H__
#define __MW300_ACOMP_H__

/* Includes ------------------------------------------------------------------ */
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  ACOMP 
 *  @{
 */
  
/** @defgroup ACOMP_Public_Types ACOMP_Public_Types
 *  @{
 */

/**
 *  @brief ACOMP ID type
 */
typedef enum
{
  ACOMP_ACOMP0,	                          /*!< Index for ACOMP0 */
  ACOMP_ACOMP1 	                          /*!< Index for ACOMP1 */
}ACOMP_ID_Type;

/**
 *  @brief ACOMP hysteresis level type
 */
typedef enum
{
  ACOMP_HYSTER_0MV,                       /*!< Hysteresis level = 0mv */
  ACOMP_HYSTER_10MV,                      /*!< Hysteresis level = 10mv */
  ACOMP_HYSTER_20MV,                      /*!< Hysteresis level = 20mv */
  ACOMP_HYSTER_30MV,                      /*!< Hysteresis level = 30mv */
  ACOMP_HYSTER_40MV,                      /*!< Hysteresis level = 40mv */
  ACOMP_HYSTER_50MV,                      /*!< Hysteresis level = 50mv */
  ACOMP_HYSTER_60MV,                      /*!< Hysteresis level = 60mv */
  ACOMP_HYSTER_70MV,                      /*!< Hysteresis level = 70mv */
}ACOMP_HysterLevel_Type;

/**
 *  @brief ACOMP warm-up time type
 */
typedef enum
{
  ACOMP_WARMTIME_16CLK,                     /*!< ACOMP warmup time is 16 clocks */
  ACOMP_WARMTIME_32CLK,                     /*!< ACOMP warmup time is 32 clocks */
  ACOMP_WARMTIME_64CLK,                     /*!< ACOMP warmup time is 64 clocks */
  ACOMP_WARMTIME_128CLK,                    /*!< ACOMP warmup time is 128 clocks */
}ACOMP_WarmupTime_Type;

/** 
 *  @brief ACOMP positive channel type
 */
typedef enum
{
  ACOMP_POS_CH_GPIO42,                         /*!< Gpio42 selection */
  ACOMP_POS_CH_GPIO43,                         /*!< Gpio43 selection */
  ACOMP_POS_CH_GPIO44,                         /*!< Gpio44 selection */
  ACOMP_POS_CH_GPIO45,                         /*!< Gpio45 selection */
  ACOMP_POS_CH_GPIO46,                         /*!< Gpio46 selection */
  ACOMP_POS_CH_GPIO47,                         /*!< Gpio47 selection */
  ACOMP_POS_CH_GPIO48,                         /*!< Gpio48 selection */
  ACOMP_POS_CH_GPIO49,                         /*!< Gpio49 selection */
  ACOMP_POS_CH_DACA,                           /*!< DACA selection */
  ACOMP_POS_CH_DACB,                           /*!< DACB selection */
}ACOMP_PosChannel_Type;

/** 
 *  @brief ACOMP negative channel type
 */
typedef enum
{
  ACOMP_NEG_CH_GPIO42,                         /*!< Gpio42 selection */
  ACOMP_NEG_CH_GPIO43,                         /*!< Gpio43 selection */
  ACOMP_NEG_CH_GPIO44,                         /*!< Gpio44 selection */
  ACOMP_NEG_CH_GPIO45,                         /*!< Gpio45 selection */
  ACOMP_NEG_CH_GPIO46,                         /*!< Gpio46 selection */
  ACOMP_NEG_CH_GPIO47,                         /*!< Gpio47 selection */
  ACOMP_NEG_CH_GPIO48,                         /*!< Gpio48 selection */
  ACOMP_NEG_CH_GPIO49,                         /*!< Gpio49 selection */
  ACOMP_NEG_CH_DACA,                           /*!< DACA selection */
  ACOMP_NEG_CH_DACB,                           /*!< DACB selection */
  ACOMP_NEG_CH_VREF1P2,                        /*!< Vref1p2 selection */
  ACOMP_NEG_CH_AVSS,                           /*!< AVSS selection */
  ACOMP_NEG_CH_VIO_0P25,                       /*!< VIO Scaling factor 0.25 */
  ACOMP_NEG_CH_VIO_0P50,                       /*!< VIO Scaling factor 0.50 */
  ACOMP_NEG_CH_VIO_0P75,                       /*!< VIO Scaling factor 0.75 */
  ACOMP_NEG_CH_VIO_1P00                        /*!< VIO Scaling factor 1.00 */
}ACOMP_NegChannel_Type;

/** 
 *  @brief ACOMP power mode type
 */
typedef enum
{
  ACOMP_PWMODE_1 = 0,                     /*!< Power mode 1 */
  ACOMP_PWMODE_2,                         /*!< Power mode 2 */
  ACOMP_PWMODE_3,                         /*!< Power mode 3 */
}ACOMP_PowerMode_Type;

/** 
 *  @brief ACOMP synchronous/asynchronous output type to pin
 */
typedef enum
{
  ACOMP_PIN_OUT_SYN           = 0,        /*!< Enable ACOMP synchronous pin output */
  ACOMP_PIN_OUT_ASYN          = 1,        /*!< Enable ACOMP asynchronous pin output */
  ACOMP_PIN_OUT_SYN_INVERTED  = 2,        /*!< Enable ACOMP inverted synchronous pin output */
  ACOMP_PIN_OUT_ASYN_INVERTED = 3,        /*!< Enable ACOMP inverted asynchronous pin output */
  ACOMP_PIN_OUT_DISABLE       = 4,        /*!< Diable ACOMP pin output */
}ACOMP_PinOut_Type;

/** 
 *  @brief ACOMP edge pule trigger source type definition
 */
typedef enum
{
  ACOMP_EDGEPULSE_DIS,               /*!< edge pulse function is disable */
  ACOMP_EDGEPULSE_RISING,            /*!< Rising edge can trigger edge pulse */
  ACOMP_EDGEPULSE_FALLING,           /*!< Falling edge can trigger edge pulse */  
  ACOMP_EDGEPULSE_BOTHEDGE           /*!< Both edge can trigger edge pulse */
}ACOMP_EdgePulseTrigSrc_Type;

/** 
 *  @brief ACOMP interrupt trigger type definition
 */
typedef enum
{
  ACOMP_INTTRIG_LOW_LEVEL     = 0,   /*!< low level triggered */
  ACOMP_INTTRIG_HIGH_LEVEL    = 1,   /*!< high level triggered */
  ACOMP_INTTRIG_FALLING_EDGE  = 2,   /*!< falling edge triggered */
  ACOMP_INTTRIG_RISING_EDGE   = 3    /*!< rising edge triggered */
}ACOMP_IntTrig_Type;

/** 
 *  @brief ACOMP interrupt type definition
 */
typedef enum
{
  ACOMP_INT_OUT_0,                           /*!< ACOMP0 synchronized output interrupt flag */  
  ACOMP_INT_OUTA_0,                          /*!< ACOMP0 asynchronized output interrupt flag */
  ACOMP_INT_OUT_1,                           /*!< ACOMP1 synchronized output interrupt flag */  
  ACOMP_INT_OUTA_1,                          /*!< ACOMP1 asynchronized output interrupt flag */  
  ACOMP_INT_ALL                              /*!< All ACOMP interrupt flags */
}ACOMP_INT_Type; 

/** 
 *  @brief ACOMP status type definition
 */
typedef enum
{
  ACOMP_STATUS_OUT,                           /*!< Synchronized output status */  
  ACOMP_STATUS_OUTA,                          /*!< Asynchronized output status */
  ACOMP_STATUS_ACTIVE,                        /*!< activity status */
}ACOMP_Status_Type;

/**
 *  @brief ACOMP configure structure definition
 */
typedef struct
{
  ACOMP_HysterLevel_Type posHyster;       /*!< Configure hysteresis for postive input
                                                                                  ACOMP_HYSTER_0MV (0): Hysteresis level = 0mv
                                                                                  ACOMP_HYSTER_10MV (1): Hysteresis level = 10mv
                                                                                  ACOMP_HYSTER_20MV (2): Hysteresis level = 20mv
                                                                                  ACOMP_HYSTER_30MV (3): Hysteresis level = 30mv
                                                                                  ACOMP_HYSTER_40MV (4): Hysteresis level = 40mv
                                                                                  ACOMP_HYSTER_50MV (5): Hysteresis level = 50mv
                                                                                  ACOMP_HYSTER_60MV (6): Hysteresis level = 60mv
                                                                                  ACOMP_HYSTER_70MV (7): Hysteresis level = 70mv */
                                                                                  
  ACOMP_HysterLevel_Type negHyster;       /*!< Configure hysteresis for negtive input
                                                                                  ACOMP_HYSTER_0MV (0): Hysteresis level = 0mv
                                                                                  ACOMP_HYSTER_10MV (1): Hysteresis level = 10mv
                                                                                  ACOMP_HYSTER_20MV (2): Hysteresis level = 20mv
                                                                                  ACOMP_HYSTER_30MV (3): Hysteresis level = 30mv
                                                                                  ACOMP_HYSTER_40MV (4): Hysteresis level = 40mv
                                                                                  ACOMP_HYSTER_50MV (5): Hysteresis level = 50mv
                                                                                  ACOMP_HYSTER_60MV (6): Hysteresis level = 60mv
                                                                                  ACOMP_HYSTER_70MV (7): Hysteresis level = 70mv */
                                                                                  
  LogicalStatus inactValue;               /*!< Configure output value for inactive state */

  ACOMP_PowerMode_Type powerMode;         /*!< Configure power mode
                                                                                  ACOMP_PWMODE_1 (0): Power mode 1
                                                                                  ACOMP_PWMODE_2 (1): Power mode 2
                                                                                  ACOMP_PWMODE_3 (2): Power mode 3 */
                                                                                  
  ACOMP_WarmupTime_Type warmupTime;       /*!< Configure warm-up time
                                                                                  ACOMP_WARMTIME_16CLK (0): ACOMP warmup time is 16 clocks
                                                                                  ACOMP_WARMTIME_32CLK (1): ACOMP warmup time is 32 clocks
                                                                                  ACOMP_WARMTIME_64CLK (2): ACOMP warmup time is 64 clocks
                                                                                  ACOMP_WARMTIME_128CLK (3): ACOMP warmup time is 128 clocks */
																		  
  ACOMP_PinOut_Type outPinMode;           /*!< Configure pin out mode
                                                                                  ACOMP_PIN_OUT_ASYN (0): Enable ACOMP asynchronous pin output
                                                                                  ACOMP_PIN_OUT_SYN (1): Enable ACOMP synchronous pin output
                                                                                  ACOMP_PIN_OUT_ASYN_INVERTED (2): Enable ACOMP inverted asynchronous pin output
                                                                                  ACOMP_PIN_OUT_SYN_INVERTED (3): Enable ACOMP inverted synchronous pin output
                                                                                  ACOMP_PIN_OUT_DISABLE (4): Diable ACOMP pin output */
}ACOMP_CFG_Type;

/*@} end of group ACOMP_Public_Types definitions */


/** @defgroup ACOMP_Public_Constants
 *  @{
 */ 

/**
 *  @brief Total ACOMP number 
 */
#define ACOMP_NUMBER        2

/**
 *  @brief Total interrupt number 
 */
#define ACOMP_INT_NUMBER           4

 /**
 *  @brief Total status number 
 */
#define ACOMP_STATUS_NUMBER            3

/**        
 *  @brief ACOMP modules check
 */
#define IS_ACOMP_PERIPH(PERIPH)        ((PERIPH) < ACOMP_NUMBER)
 
/**        
 *  @brief ACOMP interrupt type check
 */
#define IS_ACOMP_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= ACOMP_INT_NUMBER)

/**        
 *  @brief ACOMP status type check
 */
#define IS_ACOMP_STATUS_TYPE(STS_TYPE)    ((STS_TYPE) < ACOMP_STATUS_NUMBER)

/**        
 *  @brief ACOMP interrupt triggger source type check
 */
#define IS_ACOMP_INT_TRIG_SRC_TYPE(TRIG_TYPE)    ((TRIG_TYPE) <= 3)

/**        
 *  @brief ACOMP wake up interrupt triggger source type check
 */
#define IS_ACOMP_WAKEUP_INT_TRIG_SRC_TYPE(TRIG_TYPE)     ((TRIG_TYPE) <= 1)

/*@} end of group ACOMP_Public_Constants */

/** @defgroup ACOMP_Public_Macro
 *  @{
 */

/*@} end of group ACOMP_Public_Macro */


/** @defgroup ACOMP_Public_FunctionDeclaration
 *  @brief ACOMP functions statement
 *  @{
 */

void ACOMP_Init(ACOMP_ID_Type acompIdx, ACOMP_CFG_Type* acompConfigSet);
void ACOMP_ChannelConfig(ACOMP_ID_Type acompIdx, ACOMP_PosChannel_Type posChannel, ACOMP_NegChannel_Type negChannel);
void ACOMP_EdgePulseConfig(ACOMP_ID_Type acompIdx, ACOMP_EdgePulseTrigSrc_Type trigSrc);
void ACOMP_IntTrigSrcConfig(ACOMP_ID_Type acompIdx, ACOMP_IntTrig_Type intTrigSel);
void ACOMP_WakeUpIntTrigSrcConfig(ACOMP_ID_Type acompIdx, ACOMP_IntTrig_Type intTrigSel);

void ACOMP_Enable(ACOMP_ID_Type acompIdx);
void ACOMP_Disable(ACOMP_ID_Type acompIdx);
void ACOMP_Reset(ACOMP_ID_Type acompIdx);

LogicalStatus ACOMP_GetResult(ACOMP_ID_Type acompIdx);
FlagStatus ACOMP_GetStatus(ACOMP_ID_Type acompIdx, ACOMP_Status_Type statusType);

IntStatus ACOMP_GetIntStatus(ACOMP_INT_Type intType);
void ACOMP_IntMask(ACOMP_INT_Type intType, IntMask_Type intMask);
void ACOMP_IntClr(ACOMP_INT_Type intType);
void ACOMP_IRQHandler(void);

/*@} end of group ACOMP_Public_FunctionDeclaration */

/*@} end of group ACOMP  */

/*@} end of group MW300_Periph_Driver */
#endif
