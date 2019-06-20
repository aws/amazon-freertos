/** @file mw300_driver.h
*
*  @brief This file contains Driver common module header
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

#ifndef __MW300_DRIVER_H__
#define __MW300_DRIVER_H__

#include "mw300.h"
/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  DRIVER_COMMON 
 *  @{
 */
  
/** @defgroup DRIVER_Public_Types DRIVER_Public_Types
 *  @brief DRIVER type definition
 *  @{
 */

/**
 * @brief Functional State Definition
 */
typedef enum 
{
  DISABLE  = 0, 
  ENABLE   = 1
}FunctionalState;

/**
 * @brief status type definition
 */
typedef enum 
{
  ERROR = 0, 
  DSUCCESS = !ERROR
}Status;

/**
 * @brief Flag status Type definition
 */
typedef enum 
{
  RESET = 0, 
  SET = !RESET
}FlagStatus, IntStatus;

/**
 * @brief Logical status Type definition
 */
typedef enum 
{
  LOGIC_LO = 0, 
  LOGIC_HI = !LOGIC_LO
}LogicalStatus;

/**
 * @brief Active status Type definition
 */
typedef enum 
{
  DEACTIVE = 0, 
  ACTIVE = !DEACTIVE
}ActiveStatus;

/**
 * @brief interrupt mask Type definition
 */
typedef enum 
{
  UNMASK = 0, 
  MASK = 1
}IntMask_Type;

/**
 * @brief Null Type definition
 */
#ifndef NULL
#define NULL   0
#endif

/**  
 *  @brief Interrupt callback function type
 */
typedef void (intCallback_Type)(void);

/**
 * @brief MW300 peripheral type definitions
 */
typedef enum
{
  INT_EXTPIN0,
  INT_EXTPIN1,
  INT_RTC,
  INT_CRC,
  INT_AES,
  INT_I2C0,
  INT_I2C1,
  INT_DMA_CH0,
  INT_DMA_CH1,
  INT_DMA_CH2,
  INT_DMA_CH3,
  INT_DMA_CH4,
  INT_DMA_CH5,
  INT_DMA_CH6,
  INT_DMA_CH7,
  INT_DMA_CH8,
  INT_DMA_CH9,
  INT_DMA_CH10,
  INT_DMA_CH11,
  INT_DMA_CH12,
  INT_DMA_CH13,
  INT_DMA_CH14,
  INT_DMA_CH15,
  INT_DMA_CH16,
  INT_DMA_CH17,
  INT_DMA_CH18,
  INT_DMA_CH19,
  INT_DMA_CH20,
  INT_DMA_CH21,
  INT_DMA_CH22,
  INT_DMA_CH23,
  INT_DMA_CH24,
  INT_DMA_CH25,
  INT_DMA_CH26,
  INT_DMA_CH27,
  INT_DMA_CH28,
  INT_DMA_CH29,
  INT_DMA_CH30,
  INT_DMA_CH31,
  INT_GPIO,
  INT_SSP0,
  INT_SSP1,
  INT_SSP2,
  INT_QSPI,
  INT_GPT0,
  INT_GPT1,
  INT_GPT2,
  INT_GPT3,
  INT_UART0,
  INT_UART1,
  INT_UART2,
  INT_WDT,
  INT_ADC0,
  INT_DAC,
  INT_ACOMP,
  INT_SDIO,
  INT_USB,
  INT_RC32M,
  INT_MPU,
  INT_WIFIWAKEUP,
  LAST,
}INT_Peripher_Type; 

/*@} end of group DRIVER_Public_Types */


/** @defgroup DRIVER_Public_Constants
 *  @{
 */ 
/** @defgroup DRIVER_FUNCTIONALSTATE    
 *  @{
 */
#define PARAM_FUNCTIONALSTATE(State) (((State) == DISABLE) || ((State) == ENABLE))
/*@} end of group DRIVER_FUNCTIONALSTATE */

/** @defgroup DRIVER_INT_PERIPH    
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH)       ((INT_PERIPH) < LAST)
/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK     
 *  @{
 */
#define IS_INTMASK(INTMASK)          (((INTMASK) == MASK) || ((INTMASK) == UNMASK))
/*@} end of group DRIVER_INT_MASK */

/*@} end of group DRIVER_Public_Constants */

/** @defgroup DRIVER_Public_Macro
 *  @{
 */

#ifdef  DEBUG
/*************************************************************************************//**
* @brief		The CHECK_PARAM macro is used for function's parameters check.
* 				It is used only if the library is compiled in DEBUG mode.
* @param[in]	expr: - If expr is false, it calls check_failed() function
*               which reports the name of the source file and the source
*               line number of the call that failed.
*                     - If expr is true, it returns no value.
* @return  none
*****************************************************************************************/
void check_failed(uint8_t *file, uint32_t line);
#define CHECK_PARAM(expr) ((expr) ? (void)0 : check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define CHECK_PARAM(expr) ((void)0)
#endif /* DEBUG */

/*@} end of group DRIVER_Public_Macro */

/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */
extern  intCallback_Type ** intCbfArra[];
void install_int_callback(INT_Peripher_Type intPeriph, uint32_t intType, intCallback_Type * cbFun);

/*@} end of group DRIVER_Public_FunctionDeclaration */

/*@} end of group DRIVER_COMMON  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_DRIVER_H__ */
