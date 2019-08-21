/** @file mw300_rtc.h
*
*  @brief This file contains RTC driver module header
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

#ifndef __MW300_RTC_H__
#define __MW300_RTC_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  RTC 
 *  @{
 */
  
/** @defgroup RTC_Public_Types RTC_Public_Types
 *  @brief RTC configuration structure type definition
 *  @{
 */


/** 
 *  @brief RTC counter value register update mode type definition   
 */
typedef enum
{
  RTC_CNT_VAL_UPDATE_OFF       = 0,   /*!< Counter value register update mode: off */
  RTC_CNT_VAL_UPDATE_AUTO      = 2,   /*!< Counter value register update mode: Auto  */
}RTC_CntValUpdateMode_Type;

/**      
 *  @brief RTC config struct type definition  
 */
typedef struct
{
  RTC_CntValUpdateMode_Type CntValUpdateMode;      /*!< Counter value register update mode:
                                                        RTC_CNT_VAL_UPDATE_OFF (0):  Off
                                                        RTC_CNT_VAL_UPDATE_AUTO (2): Auto   */
  
                                            
  uint32_t clockDivider;                          /*!< Clock divider value(range: 0~15).
                                                       The divided clock is calculated by:
                                                       CLK_div = CLK / (2^clockDivider)*/  
  
  uint32_t uppVal;                                /*!< Counter overflow value */

}RTC_Config_Type;


/**  
 *  @brief RTC interrupt type definition 
 */
typedef enum
{  
  RTC_INT_CNT_ALARM = 0,                    /*!< RTC channel 0 cup int flag */ 
  RTC_INT_CNT_UPP,                   /*!< RTC counter upper overflow flag */ 
  
  RTC_INT_ALL,                             /*!< All RTC interrupts flag */
}RTC_INT_Type; 

/*@} end of group RTC_Public_Types definitions */


/** @defgroup RTC_Public_Constants
 *  @{
 */ 

/** @defgroup GPT_INT_MASK       
 *  @{
 */     
#define RTC_INT_CNT_ALARM_MSK            0x00008000
#define RTC_INT_CNT_UPP_MSK              0x00010000
#define RTC_INT_ALL_MSK                  0x00018000
/*@} end of group GPT_INT_MASK */

/*@} end of group RTC_Public_Constants */

/** @defgroup RTC_Public_Macro
 *  @{
 */

/**
 *  @brief Total interrupt number 
 */
#define RTC_INT_NUMBER            2

/**        
 *  @brief RTC interrupt type check
 */
#define IS_RTC_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= RTC_INT_NUMBER)

/*@} end of group RTC_Public_Constants */

/** @defgroup RTC_Public_Macro
 *  @{
 */

/*@} end of group RTC_Public_Macro */


/** @defgroup RTC_Public_FunctionDeclaration
 *  @brief RTC functions statement
 *  @{
 */
void RTC_Init(RTC_Config_Type* rtcConfig);
Status RTC_CounterReset(void);
FunctionalState RTC_GetCntStatus(void);
void RTC_Start(void);
void RTC_Stop(void);
void RTC_SetCounterUppVal( uint32_t uppVal);
uint32_t RTC_GetCounterUppVal(void);
void RTC_SetCounterAlarmVal(uint32_t alarmVal);
uint32_t RTC_GetCounterVal(void);
void RTC_IntMask(RTC_INT_Type intType, IntMask_Type intMsk);
FlagStatus RTC_GetStatus(RTC_INT_Type intType);
void RTC_IntClr(RTC_INT_Type intType);
void RTC_IRQHandler(void);

/*@} end of group RTC_Public_FunctionDeclaration */

/*@} end of group RTC  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_RTC_H__ */
