/** @file     mw300_pinmux.c
 *
 *  @brief    This file provides PINMUX functions.
 *
 *  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved.
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

#include "mw300.h"
#include "mw300_driver.h"
#include "mw300_gpio.h"
#include "mw300_pinmux.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @defgroup PINMUX PINMUX
 *  @brief GPIO pinmux driver modules
 *  @{
 */

/** @defgroup PINMUX_Private_Type
 *  @{
 */

/*@} end of group PINMUX_Private_Type */

/** @defgroup PINMUX_Private_Defines
 *  @{
 */

/*@} end of group PINMUX_Private_Defines */

/** @defgroup PINMUX_Private_Variables
 *  @{
 */

#ifdef  DEBUG

#ifdef PACKAGE_88_PIN
/**  
 *  @brief GPIO pinumux function mask value definition for 88 pin package
 */
static const uint8_t PinMuxFunMask[80]={
  0xFF,    /*!< GPIO0  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO1  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO2  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO3  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO4  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO5  pinmux function check value: 11111111b */
  0x7F,    /*!< GPIO6  pinmux function check value: 01111111b */
  0x7F,    /*!< GPIO7  pinmux function check value: 01111111b */
  0x7F,    /*!< GPIO8  pinmux function check value: 01111111b */
  0x7F,    /*!< GPIO9  pinmux function check value: 01111111b */
  0x7F,    /*!< GPIO10 pinmux function check value: 01111111b */
  0xDF,    /*!< GPIO11 pinmux function check value: 11011111b */
  0xDF,    /*!< GPIO12 pinmux function check value: 11011111b */
  0xDF,    /*!< GPIO13 pinmux function check value: 11011111b */
  0xCF,    /*!< GPIO14 pinmux function check value: 11001111b */
  0xC3,    /*!< GPIO15 pinmux function check value: 11000011b */
  0xFB,    /*!< GPIO16 pinmux function check value: 11111011b */
  0xD7,    /*!< GPIO17 pinmux function check value: 11010111b */
  0xCF,    /*!< GPIO18 pinmux function check value: 11001111b */
  0xCF,    /*!< GPIO19 pinmux function check value: 11001111b */
  0xCF,    /*!< GPIO20 pinmux function check value: 11001111b */
  0xCF,    /*!< GPIO21 pinmux function check value: 11001111b */
  0x03,    /*!< GPIO22 pinmux function check value: 00000011b */
  0x37,    /*!< GPIO23 pinmux function check value: 00110111b */
  0xEF,    /*!< GPIO24 pinmux function check value: 11101111b */
  0x97,    /*!< GPIO25 pinmux function check value: 10010111b */
  0x97,    /*!< GPIO26 pinmux function check value: 10010111b */
  0xDF,    /*!< GPIO27 pinmux function check value: 11011111b */
  0xB7,    /*!< GPIO28 pinmux function check value: 10110111b */
  0x37,    /*!< GPIO29 pinmux function check value: 00110111b */
  0x3F,    /*!< GPIO30 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO31 pinmux function check value: 00111111b  */
  0x3F,    /*!< GPIO32 pinmux function check value: 00111111b */
  0x3F,    /*!< GPIO33 pinmux function check value: 00111111b */
  0xD3,    /*!< GPIO34 pinmux function check value: 11010011b */
  0xDF,    /*!< GPIO35 pinmux function check value: 11011111b */
  0xDF,    /*!< GPIO36 pinmux function check value: 11011111b */
  0xC7,    /*!< GPIO37 pinmux function check value: 11000111b */
  0xCF,    /*!< GPIO38 pinmux function check value: 11001111b */
  0xFF,    /*!< GPIO39  pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO40 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO41 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO42 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO43 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO44 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO45 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO46 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO47 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO48 pinmux function check value: 11111111b */
  0xFF,    /*!< GPIO49 pinmux function check value: 11111111b */
};
#elif  defined(PACKAGE_68_PIN)
/**  
 *  @brief GPIO pinumux function mask value definition for 68 pin package
 */
static const uint8_t PinMuxFunMask[80]={
    0xFF,     /*!< GPIO0  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO1  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO2  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO3  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO4  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO5  pinmux function check value: 11111111b */
    0x7F,     /*!< GPIO6  pinmux function check value: 01111111b */
    0x7F,     /*!< GPIO7  pinmux function check value: 01111111b */
    0x7F,     /*!< GPIO8  pinmux function check value: 01111111b */
    0x7F,     /*!< GPIO9  pinmux function check value: 01111111b */
    0x7F,     /*!< GPIO10 pinmux function check value: 01111111b */
    0xDF,     /*!< GPIO11 pinmux function check value: 11011111b */
    0xDF,     /*!< GPIO12 pinmux function check value: 11011111b */
    0xDF,     /*!< GPIO13 pinmux function check value: 11011111b */
    0xCF,     /*!< GPIO14 pinmux function check value: 11001111b */
    0xC3,     /*!< GPIO15 pinmux function check value: 11000011b */
    0xFB,     /*!< GPIO16 pinmux function check value: 11111011b */
    0xD7,     /*!< GPIO17 pinmux function check value: 11010111b */
    0xCF,     /*!< GPIO18 pinmux function check value: 11001111b */
    0xCF,     /*!< GPIO19 pinmux function check value: 11001111b */
    0xCF,     /*!< GPIO20 pinmux function check value: 11001111b */
    0xCF,     /*!< GPIO21 pinmux function check value: 11001111b */
    0x03,     /*!< GPIO22 pinmux function check value: 00000011b */
    0x37,     /*!< GPIO23 pinmux function check value: 00110111b */
    0xEF,     /*!< GPIO24 pinmux function check value: 11101111b */
    0x97,     /*!< GPIO25 pinmux function check value: 10010111b */
    0x97,     /*!< GPIO26 pinmux function check value: 10010111b */
    0xDF,     /*!< GPIO27 pinmux function check value: 11011111b */
    0xB7,     /*!< GPIO28 pinmux function check value: 10110111b */
    0x37,     /*!< GPIO29 pinmux function check value: 00110111b */
    0x3F,     /*!< GPIO30 pinmux function check value: 00111111b */
    0x3F,     /*!< GPIO31 pinmux function check value: 00111111b  */
    0x3F,     /*!< GPIO32 pinmux function check value: 00111111b */
    0x3F,     /*!< GPIO33 pinmux function check value: 00111111b */
    0xD3,     /*!< GPIO34 pinmux function check value: 11010011b */
    0xDF,     /*!< GPIO35 pinmux function check value: 11011111b */
    0xDF,     /*!< GPIO36 pinmux function check value: 11011111b */
    0xC7,     /*!< GPIO37 pinmux function check value: 11000111b */
    0xCF,     /*!< GPIO38 pinmux function check value: 11001111b */
    0xFF,     /*!< GPIO39  pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO40 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO41 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO42 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO43 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO44 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO45 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO46 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO47 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO48 pinmux function check value: 11111111b */
    0xFF,     /*!< GPIO49 pinmux function check value: 11111111b */
};

#endif

/** @defgroup GPIO_PINMUX_FUNCTION_MASK     
 *  @{
 */
#define IS_GPIO_PINMUXFUNMASK(GPIONO, PINMUXFUNMASK)           ((PinMuxFunMask[GPIONO]) && (1 << PINMUXFUNMASK))

#else
/** @defgroup GPIO_PINMUX_FUNCTION_MASK     
 *  @{
 */
#define IS_GPIO_PINMUXFUNMASK(GPIONO, PINMUXFUNMASK)           1

#endif

/*@} end of group GPIO_PINMUX_FUNCTION_MASK */

/*@} end of group PINMUX_Private_Variables */

/** @defgroup PINMUX_Global_Variables
 *  @{
 */

/*@} end of group PINMUX_Global_Variables */

/** @defgroup PINMUX_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group PINMUX_Private_FunctionDeclaration */

/** @defgroup PINMUX_Private_Functions
 *  @{
 */

/*@} end of group PINMUX_Private_Functions */

/** @defgroup PINMUX_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      GPIO pinmux function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  PinMuxFun:  GPIO pin function, should be GPIO_PinMuxFunc_Type
 *
 * @return Status: ERROR or DSUCCESS
 *
 *******************************************************************************/
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type PinMuxFun)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMUXFUN(PinMuxFun));
  CHECK_PARAM(IS_GPIO_PINMUXFUNMASK(gpioNo, PinMuxFun));
  
  /* When function to be set is not GPIO function, clear PIO_PULL_SEL_R to make the pin to default mode */
  /* For GPIO6~10, GPIO22~26 and GPIO28~33, function 1 is GPIO function, for other GPIOs, function 0 is GPIO function */
  if(((gpioNo>=GPIO_6)&&(gpioNo<=GPIO_10))||((gpioNo>=GPIO_22)&&(gpioNo<=GPIO_26))||((gpioNo>=GPIO_28)&&(gpioNo<=GPIO_33)))
  {
    if(PinMuxFun==PINMUX_FUNCTION_1)
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07);
    }
    else
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07); 
    }
  }
  else
  {
    if(PinMuxFun==PINMUX_FUNCTION_0)
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07);
    }
    else
    {
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.FSEL_XR = (PinMuxFun & 0x07); 
    }
  }
}

/****************************************************************************//**
 * @brief      GPIO pin mode function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  gpioPinMode:  GPIO pin mode, should be PINMODE_DEFAULT, PINMODE_PULLUP, 
 *                           PINMODE_PULLDOWN, PINMODE_NOPULL or PINMODE_TRISTATE.
 *                           when this pin is not configured as GPIO function, 
 *                           or the data transfer direction is not input,
 *                           PINMODE_PULLUP, PINMODE_PULLDOWN or PINMODE_TRISTATE has no use.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMODE(gpioPinMode));
 
  switch(gpioPinMode)
  {
    case PINMODE_DEFAULT:
      /* Default */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 0;
      break;
      
    case PINMODE_PULLUP:
      /* Pullup */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;      
      break;

    case PINMODE_PULLDOWN:
      /* Pulldown */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 1;    
      break;

     case PINMODE_NOPULL:
      /* Nopull */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;    
      break;

    case PINMODE_TRISTATE:
      /* Tristate */
      PINMUX->GPIO_PINMUX[gpioNo].BF.DI_EN = 0;                                                                        
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo].BF.PIO_PULLDN_R = 0;    
      break;

    default:
      break;
  }
}

/*@} end of group PINMUX_Public_Functions */

/*@} end of group PINMUX_definitions */

/*@} end of group MW300_Periph_Driver */

