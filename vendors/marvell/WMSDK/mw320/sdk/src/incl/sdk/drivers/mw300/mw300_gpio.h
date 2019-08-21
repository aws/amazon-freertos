/** @file mw300_gpio.h
*
*  @brief This file contains GPIO driver module header
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

#ifndef __MW300_GPIO_H__
#define __MW300_GPIO_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup GPIO 
 *  @{
 */
  
/** @defgroup GPIO_Public_Types GPIO_Public_Types 
 *  @{
 */

/**  
 *  @brief GPIO No. type definition 
 */
typedef enum
{
  GPIO_0,                           /*!< GPIO0  Pin define */
  GPIO_1,                           /*!< GPIO1  Pin define */
  GPIO_2,                           /*!< GPIO2  Pin define */
  GPIO_3,                           /*!< GPIO3  Pin define */
  GPIO_4,                           /*!< GPIO4  Pin define */
  GPIO_5,                           /*!< GPIO5  Pin define */
  GPIO_6,                           /*!< GPIO6  Pin define */
  GPIO_7,                           /*!< GPIO7  Pin define */
  GPIO_8,                           /*!< GPIO8  Pin define */
  GPIO_9,                           /*!< GPIO9  Pin define */
  GPIO_10,                          /*!< GPIO10 Pin define */
  GPIO_11,                          /*!< GPIO11 Pin define */
  GPIO_12,                          /*!< GPIO12 Pin define */
  GPIO_13,                          /*!< GPIO13 Pin define */
  GPIO_14,                          /*!< GPIO14 Pin define */
  GPIO_15,                          /*!< GPIO15 Pin define */
  GPIO_16,                          /*!< GPIO16 Pin define */
  GPIO_17,                          /*!< GPIO17 Pin define */
  GPIO_18,                          /*!< GPIO18 Pin define */
  GPIO_19,                          /*!< GPIO19 Pin define */
  GPIO_20,                          /*!< GPIO20 Pin define */
  GPIO_21,                          /*!< GPIO21 Pin define */
  GPIO_22,                          /*!< GPIO22 Pin define */
  GPIO_23,                          /*!< GPIO23 Pin define */
  GPIO_24,                          /*!< GPIO24 Pin define */
  GPIO_25,                          /*!< GPIO25 Pin define */
  GPIO_26,                          /*!< GPIO26 Pin define */
  GPIO_27,                          /*!< GPIO27 Pin define */
  GPIO_28,                          /*!< GPIO28 Pin define */
  GPIO_29,                          /*!< GPIO29 Pin define */
  GPIO_30,                          /*!< GPIO30 Pin define */
  GPIO_31,                          /*!< GPIO31 Pin define */ 
  GPIO_32,                          /*!< GPIO32 Pin define */
  GPIO_33,                          /*!< GPIO33 Pin define */
  GPIO_34,                          /*!< GPIO34 Pin define */
  GPIO_35,                          /*!< GPIO35 Pin define */
  GPIO_36,                          /*!< GPIO36 Pin define */
  GPIO_37,                          /*!< GPIO37 Pin define */
  GPIO_38,                          /*!< GPIO38 Pin define */
  GPIO_39,                          /*!< GPIO39 Pin define */
  GPIO_40,                          /*!< GPIO40 Pin define */
  GPIO_41,                          /*!< GPIO41 Pin define */
  GPIO_42,                          /*!< GPIO42 Pin define */
  GPIO_43,                          /*!< GPIO43 Pin define */
  GPIO_44,                          /*!< GPIO44 Pin define */
  GPIO_45,                          /*!< GPIO45 Pin define */
  GPIO_46,                          /*!< GPIO46 Pin define */
  GPIO_47,                          /*!< GPIO47 Pin define */
  GPIO_48,                          /*!< GPIO48 Pin define */
  GPIO_49,                          /*!< GPIO49 Pin define */
}GPIO_NO_Type;

/**  
 *  @brief GPIO port type definition 
 */
typedef enum 
{
  GPIO_PORT_0 = 0 ,
  GPIO_PORT_1     , 
}GPIO_Port_Type;

/**  
 *  @brief GPIO data direction type definition 
 */

typedef enum
{
  GPIO_INPUT = 0,                             /*!< Set GPIO port data direction as input */  
  GPIO_OUTPUT,                              /*!< Set GPIO port data direction as output */
}GPIO_Dir_Type;

/**  
 *  @brief GPIO data Input/Output type definition 
 */
typedef enum
{
  GPIO_IO_LOW = 0,                            /*!< Set GPIO port data Input/Output value as low  */  
  GPIO_IO_HIGH,                             /*!< Set GPIO port data Input/Output value as high */
}GPIO_IO_Type;

/**  
 *  @brief GPIO interrut level type definition 
 */
typedef enum
{
  GPIO_INT_RISING_EDGE = 0,                    /*!< Interrupt type: Rising edge */                                       
  GPIO_INT_FALLING_EDGE,                       /*!< Interrupt type: Falling edge */
  GPIO_INT_BOTH_EDGES,                         /*!< Interrupt type: Rising edge and Falling edge */
  GPIO_INT_DISABLE,                            /*!< Disable interrupt */
}GPIO_Int_Type;
 
/*@} end of group GPIO_Public_Types */

/** @defgroup GPIO_Public_Constants
 *  @{
 */ 
#define GPIO_MinNo        GPIO_0
#define GPIO_MaxNo        GPIO_49

/** @defgroup GPIO_Num        
 *  @{
 */
#define IS_GPIO_NO(GPIONO)                     ((GPIONO) <= GPIO_MaxNo)

/*@} end of group GPIO_Num */

/** @defgroup GPIO_DataDir       
 *  @{
 */
#define IS_GPIO_DDR(GPIODDR)                   (((GPIODDR) == GPIO_INPUT) || ((GPIODDR) == GPIO_OUTPUT))
                                                                                     
/*@} end of group GPIO_DataDir */

/** @defgroup GPIO_IO      
 *  @{
 */
#define IS_GPIO_IO(GPIODDRIO)                  (((GPIODDRIO) == GPIO_IO_LOW) || ((GPIODDRIO) == GPIO_IO_HIGH))

/*@} end of group GPIO_IO */

/** @defgroup GPIO_INT_TYPE      
 *  @{
 */
#define IS_GPIO_INTTYPE(INTTYPE)               (((INTTYPE) == GPIO_INT_RISING_EDGE) || ((INTTYPE) == GPIO_INT_FALLING_EDGE) || ((INTTYPE) == GPIO_INT_BOTH_EDGES) || ((INTTYPE) == GPIO_INT_DISABLE))

/*@} end of group GPIO_INT_TYPE */
  
/*@} end of group GPIO_Public_Constants */

/** @defgroup GPIO_Public_Macro
 *  @{
 */

/*@} end of group GPIO_Public_Macro */

/** @defgroup GPIO_Public_FunctionDeclaration
 *  @brief GPIO functions declaration
 *  @{
 */
void GPIO_SetPinDir(GPIO_NO_Type gpioNo, GPIO_Dir_Type dir);
void GPIO_WritePinOutput(GPIO_NO_Type gpioNo, GPIO_IO_Type bitVal);
GPIO_IO_Type GPIO_ReadPinLevel(GPIO_NO_Type gpioNo);
void GPIO_WritePortOutput(GPIO_Port_Type portNo, GPIO_IO_Type level, uint32_t bitMask);
uint32_t GPIO_ReadPortLevel(GPIO_Port_Type portNo);
void GPIO_SetPortPinDir(GPIO_Port_Type portNo, uint32_t bitMask);
void GPIO_IntConfig(GPIO_NO_Type gpioNo, GPIO_Int_Type intType);
void GPIO_IntMask(GPIO_NO_Type gpioNo, IntMask_Type intMask);
FlagStatus GPIO_GetIntStatus(GPIO_NO_Type gpioNo);
void GPIO_IntClr(GPIO_NO_Type gpioNo);

/*@} end of group GPIO_Public_FunctionDeclaration */

/*@} end of group GPIO */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_GPIO_H__ */
