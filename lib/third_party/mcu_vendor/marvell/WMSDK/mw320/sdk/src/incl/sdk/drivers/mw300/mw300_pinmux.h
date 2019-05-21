/** @file mw300_pinmux.h
*
*  @brief This file contains PINMUX driver module header
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

#ifndef __MW300_PINMUX_H__
#define __MW300_PINMUX_H__

#include "mw300.h"
#include "mw300_driver.h"
#include "mw300_gpio.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup PINMUX 
 *  @{
 */
  
/** @defgroup PINMUX_Public_Types PINMUX_Public_Types 
 *  @{
 */

/**  
 *  @brief GPIO Pinmux function type definition 
 */
typedef enum
{
  PINMUX_FUNCTION_0 = 0,                   /*!< GPIO pin mux function 0 define */
  PINMUX_FUNCTION_1,                       /*!< GPIO pin mux function 1 define */
  PINMUX_FUNCTION_2,                       /*!< GPIO pin mux function 2 define */
  PINMUX_FUNCTION_3,                       /*!< GPIO pin mux function 3 define */
  PINMUX_FUNCTION_4,                       /*!< GPIO pin mux function 4 define */
  PINMUX_FUNCTION_5,                       /*!< GPIO pin mux function 5 define */
  PINMUX_FUNCTION_6,                       /*!< GPIO pin mux function 6 define */
  PINMUX_FUNCTION_7,                       /*!< GPIO pin mux function 7 define */
}GPIO_PinMuxFunc_Type;

/**  
 *  @brief GPIO pin mode type definition 
 */
typedef enum
{
  PINMODE_DEFAULT = 0,                      /*!< GPIO pin mode default define */
  PINMODE_PULLUP,                          /*!< GPIO pin mode pullup define */
  PINMODE_PULLDOWN,                        /*!< GPIO pin mode pulldown define */
  PINMODE_NOPULL,                          /*!< GPIO pin mode nopull define */
  PINMODE_TRISTATE,                        /*!< GPIO pin mode tristate define */
}GPIO_PINMODE_Type;

/*@} end of group PINMUX_Public_Types */

/** @defgroup PINMUX_Public_Constants
 *  @{
 */ 

/** @defgroup GPIO_Pinmux        
 *  @{
 */
/* GPIO0 pinmux function define */
#define GPIO0_GPIO0                            PINMUX_FUNCTION_0
#define GPIO0_GPT0_CH0                         PINMUX_FUNCTION_1
#define GPIO0_UART0_CTSn                       PINMUX_FUNCTION_2
#define GPIO0_SSP0_CLK                         PINMUX_FUNCTION_3

/* GPIO1 pinmux function define */
#define GPIO1_GPIO1                            PINMUX_FUNCTION_0
#define GPIO1_GPT0_CH1                         PINMUX_FUNCTION_1
#define GPIO1_UART0_RTSn                       PINMUX_FUNCTION_2
#define GPIO1_SSP0_FRM                         PINMUX_FUNCTION_3

/* GPIO2 pinmux function define */
#define GPIO2_GPIO2                            PINMUX_FUNCTION_0
#define GPIO2_GPT0_CH2                         PINMUX_FUNCTION_1
#define GPIO2_UART0_TXD                        PINMUX_FUNCTION_2
#define GPIO2_SSP0_TXD                         PINMUX_FUNCTION_3

/* GPIO3 pinmux function define */
#define GPIO3_GPIO3                            PINMUX_FUNCTION_0
#define GPIO3_GPT0_CH3                         PINMUX_FUNCTION_1
#define GPIO3_UART0_RXD                        PINMUX_FUNCTION_2
#define GPIO3_SSP0_RXD                         PINMUX_FUNCTION_3

/* GPIO4 pinmux function define */
#define GPIO4_GPIO4                            PINMUX_FUNCTION_0
#define GPIO4_GPT0_CH4                         PINMUX_FUNCTION_1
#define GPIO4_I2C0_SDA                         PINMUX_FUNCTION_2
#define GPIO4_AUDIO_CLK                        PINMUX_FUNCTION_3

/* GPIO5 pinmux function define */
#define GPIO5_GPIO5                            PINMUX_FUNCTION_0
#define GPIO5_GPT0_CH5                         PINMUX_FUNCTION_1
#define GPIO5_I2C0_SCL                         PINMUX_FUNCTION_2

/* GPIO6 pinmux function define */
#define GPIO6_TDO                              PINMUX_FUNCTION_0
#define GPIO6_GPIO6                            PINMUX_FUNCTION_1
#define GPIO6_I2C1_SDA                         PINMUX_FUNCTION_2
#define GPIO6_DIG_POR                          PINMUX_FUNCTION_3
#define GPIO6_RC32M                            PINMUX_FUNCTION_4
#define GPIO6_AUPLL_DIGTP0                     PINMUX_FUNCTION_5

/* GPIO7 pinmux function define */
#define GPIO7_TCK                              PINMUX_FUNCTION_0
#define GPIO7_GPIO7                            PINMUX_FUNCTION_1
#define GPIO7_UART2_CTSn                       PINMUX_FUNCTION_2
#define GPIO7_SSP2_CLK                         PINMUX_FUNCTION_3
#define GPIO7_I2C0_SDA                         PINMUX_FUNCTION_4
#define GPIO7_AUPLL_DIGTP1                     PINMUX_FUNCTION_5

/* GPIO8 pinmux function define */ 
#define GPIO8_TMS                              PINMUX_FUNCTION_0
#define GPIO8_GPIO8                            PINMUX_FUNCTION_1
#define GPIO8_UART2_RTSn                       PINMUX_FUNCTION_2
#define GPIO8_SSP2_FRM                         PINMUX_FUNCTION_3
#define GPIO8_I2C0_SCL                         PINMUX_FUNCTION_4
#define GPIO8_AUPLL_DIGTP2                     PINMUX_FUNCTION_5

/* GPIO9 pinmux function define */ 
#define GPIO9_TDI                              PINMUX_FUNCTION_0
#define GPIO9_GPIO9                            PINMUX_FUNCTION_1
#define GPIO9_UART2_TXD                        PINMUX_FUNCTION_2
#define GPIO9_SSP2_TXD                         PINMUX_FUNCTION_3
#define GPIO9_I2C1_SDA                         PINMUX_FUNCTION_4
#define GPIO9_AUPLL_DIGTP3                     PINMUX_FUNCTION_5

/* GPIO10 pinmux function define */ 
#define GPIO10_TRST_N                          PINMUX_FUNCTION_0
#define GPIO10_GPIO10                          PINMUX_FUNCTION_1
#define GPIO10_UART2_RXD                       PINMUX_FUNCTION_2
#define GPIO10_SSP2_RXD                        PINMUX_FUNCTION_3
#define GPIO10_I2C1_SCL                        PINMUX_FUNCTION_4
#define GPIO10_PHY_MON0                        PINMUX_FUNCTION_5

/* GPIO11 pinmux function define */ 
#define GPIO11_GPIO11                          PINMUX_FUNCTION_0
#define GPIO11_GPT2_CH0                        PINMUX_FUNCTION_1
#define GPIO11_UART1_CTSn                      PINMUX_FUNCTION_2
#define GPIO11_SSP1_CLK                        PINMUX_FUNCTION_3

/* GPIO12 pinmux function define */ 
#define GPIO12_GPIO12                          PINMUX_FUNCTION_0
#define GPIO12_GPT2_CH1                        PINMUX_FUNCTION_1
#define GPIO12_UART1_RTSn                      PINMUX_FUNCTION_2
#define GPIO12_SSP1_FRM                        PINMUX_FUNCTION_3

/* GPIO13 pinmux function define */ 
#define GPIO13_GPIO13                          PINMUX_FUNCTION_0
#define GPIO13_GPT2_CH2                        PINMUX_FUNCTION_1
#define GPIO13_UART1_TXD                       PINMUX_FUNCTION_2
#define GPIO13_SSP1_TXD                        PINMUX_FUNCTION_3

/* GPIO14 pinmux function define */ 
#define GPIO14_GPIO14                          PINMUX_FUNCTION_0
#define GPIO14_GPT2_CH3                        PINMUX_FUNCTION_1
#define GPIO14_UART1_RXD                       PINMUX_FUNCTION_2
#define GPIO14_SSP1_RXD                        PINMUX_FUNCTION_3

/* GPIO15 pinmux function define */ 
#define GPIO15_GPIO15                          PINMUX_FUNCTION_0
#define GPIO15_GPT2_CH4                        PINMUX_FUNCTION_1

/* GPIO16 pinmux function define */ 
#define GPIO16_GPIO16                          PINMUX_FUNCTION_0
#define GPIO16_STRAP1                          PINMUX_FUNCTION_1
#define GPIO16_AUDIO_CLK                       PINMUX_FUNCTION_3
#define GPIO16_PHY_MON7                        PINMUX_FUNCTION_5

/* GPIO17 pinmux function define */ 
#define GPIO17_GPIO17                          PINMUX_FUNCTION_0
#define GPIO17_GPT3_CH0                        PINMUX_FUNCTION_1
#define GPIO17_I2C1_SCL                        PINMUX_FUNCTION_2

/* GPIO18 pinmux function define */ 
#define GPIO18_GPIO18                          PINMUX_FUNCTION_0
#define GPIO18_GPT3_CH1                        PINMUX_FUNCTION_1
#define GPIO18_I2C1_SDA                        PINMUX_FUNCTION_2
#define GPIO18_SSP1_CLK                        PINMUX_FUNCTION_3

/* GPIO19 pinmux function define */ 
#define GPIO19_GPIO19                          PINMUX_FUNCTION_0
#define GPIO19_GPT3_CH2                        PINMUX_FUNCTION_1
#define GPIO19_I2C1_SCL                        PINMUX_FUNCTION_2
#define GPIO19_SSP1_FRM                        PINMUX_FUNCTION_3

/* GPIO20 pinmux function define */ 
#define GPIO20_GPIO20                          PINMUX_FUNCTION_0
#define GPIO20_GPT3_CH3                        PINMUX_FUNCTION_1
#define GPIO20_I2C0_SDA                        PINMUX_FUNCTION_2
#define GPIO20_SSP1_TXD                        PINMUX_FUNCTION_3

/* GPIO21 pinmux function define */ 
#define GPIO21_GPIO21                          PINMUX_FUNCTION_0
#define GPIO21_GPT3_CH4                        PINMUX_FUNCTION_1
#define GPIO21_I2C0_SCL                        PINMUX_FUNCTION_2
#define GPIO21_SSP1_RXD                        PINMUX_FUNCTION_3

/* GPIO22 pinmux function define */ 
#define GPIO22_WAKE_UP0                        PINMUX_FUNCTION_0
#define GPIO22_GPIO22                          PINMUX_FUNCTION_1

/* GPIO23 pinmux function define */ 
#define GPIO23_WAKE_UP1                        PINMUX_FUNCTION_0
#define GPIO23_GPIO23                          PINMUX_FUNCTION_1
#define GPIO23_UART0_CTSn                      PINMUX_FUNCTION_2
#define GPIO23_SFLL_200M                       PINMUX_FUNCTION_4
#define GPIO23_COMP_IN_P                       PINMUX_FUNCTION_5

/* GPIO24 pinmux function define */ 
#define GPIO24_OSC32K                          PINMUX_FUNCTION_0
#define GPIO24_GPIO24                          PINMUX_FUNCTION_1
#define GPIO24_UART0_RXD                       PINMUX_FUNCTION_2
#define GPIO24_GPT1_CH5                        PINMUX_FUNCTION_3
#define GPIO24_COMP_IN_N                       PINMUX_FUNCTION_5

/* GPIO25 pinmux function define */ 
#define GPIO25_XTAL32K_IN                      PINMUX_FUNCTION_0
#define GPIO25_GPIO25                          PINMUX_FUNCTION_1
#define GPIO25_I2C1_SDA                        PINMUX_FUNCTION_2

/* GPIO26 pinmux function define */ 
#define GPIO26_XTAL32K_OUT                     PINMUX_FUNCTION_0
#define GPIO26_GPIO26                          PINMUX_FUNCTION_1
#define GPIO26_I2C1_SCL                        PINMUX_FUNCTION_2

/* GPIO27 pinmux function define */ 
#define GPIO27_GPIO27                          PINMUX_FUNCTION_0
#define GPIO27_STRAP0                          PINMUX_FUNCTION_1
#define GPIO27_UART0_TXD                       PINMUX_FUNCTION_2
#define GPIO27_DRVVBUS                         PINMUX_FUNCTION_3
#define GPIO27_RC32K                           PINMUX_FUNCTION_4

/* GPIO28 pinmux function define */
#define GPIO28_QSPI_SSn                        PINMUX_FUNCTION_0
#define GPIO28_GPIO28                          PINMUX_FUNCTION_1
#define GPIO28_I2C0_SDA                        PINMUX_FUNCTION_2
#define GPIO28_PHY_MON1                        PINMUX_FUNCTION_4
#define GPIO28_GPT1_CH0                        PINMUX_FUNCTION_5

/* GPIO29 pinmux function define */  
#define GPIO29_QSPI_CLK                        PINMUX_FUNCTION_0
#define GPIO29_GPIO29                          PINMUX_FUNCTION_1
#define GPIO29_I2C0_SCL                        PINMUX_FUNCTION_2
#define GPIO29_PHY_MON2                        PINMUX_FUNCTION_4
#define GPIO29_GPT1_CH1                        PINMUX_FUNCTION_5

/* GPIO30 pinmux function define */ 
#define GPIO30_QSPI_D0                         PINMUX_FUNCTION_0
#define GPIO30_GPIO30                          PINMUX_FUNCTION_1
#define GPIO30_UART0_CTSn                      PINMUX_FUNCTION_2
#define GPIO30_SSP0_CLK                        PINMUX_FUNCTION_3
#define GPIO30_PHY_MON3                        PINMUX_FUNCTION_4
#define GPIO30_GPT1_CH2                        PINMUX_FUNCTION_5

/* GPIO31 pinmux function define */ 
#define GPIO31_QSPI_D1                         PINMUX_FUNCTION_0
#define GPIO31_GPIO31                          PINMUX_FUNCTION_1
#define GPIO31_UART0_RTSn                      PINMUX_FUNCTION_2
#define GPIO31_SSP0_FRM                        PINMUX_FUNCTION_3
#define GPIO31_PHY_MON4                        PINMUX_FUNCTION_4
#define GPIO31_GPT1_CH3                        PINMUX_FUNCTION_5

/* GPIO32 pinmux function define */ 
#define GPIO32_QSPI_D2                         PINMUX_FUNCTION_0
#define GPIO32_GPIO32                          PINMUX_FUNCTION_1
#define GPIO32_UART0_TXD                       PINMUX_FUNCTION_2
#define GPIO32_SSP0_TXD                        PINMUX_FUNCTION_3
#define GPIO32_PHY_MON5                        PINMUX_FUNCTION_4
#define GPIO32_GPT1_CH4                        PINMUX_FUNCTION_5

/* GPIO33 pinmux function define */ 
#define GPIO33_QSPI_D3                         PINMUX_FUNCTION_0
#define GPIO33_GPIO33                          PINMUX_FUNCTION_1
#define GPIO33_UART0_RXD                       PINMUX_FUNCTION_2
#define GPIO33_SSP0_RXD                        PINMUX_FUNCTION_3
#define GPIO33_PHY_MON6                        PINMUX_FUNCTION_4
#define GPIO33_GPT1_CH5                        PINMUX_FUNCTION_5

/* GPIO34 pinmux function define */ 
#define GPIO34_GPIO34                          PINMUX_FUNCTION_0
#define GPIO34_GPT3_CH5                        PINMUX_FUNCTION_1

/* GPIO35 pinmux function define */ 
#define GPIO35_GPIO35                          PINMUX_FUNCTION_0
#define GPIO35_GPT0_CLKIN                      PINMUX_FUNCTION_1
#define GPIO35_UART1_CTSn                      PINMUX_FUNCTION_2
#define GPIO35_SSP1_CLK                        PINMUX_FUNCTION_3

/* GPIO36 pinmux function define */ 
#define GPIO36_GPIO36                          PINMUX_FUNCTION_0
#define GPIO36_GPT1_CLKIN                      PINMUX_FUNCTION_1
#define GPIO36_UART1_RTSn                      PINMUX_FUNCTION_2
#define GPIO36_SSP1_FRM                        PINMUX_FUNCTION_3

/* GPIO37 pinmux function define */ 
#define GPIO37_GPIO37                          PINMUX_FUNCTION_0
#define GPIO37_GPT2_CH5                        PINMUX_FUNCTION_1
#define GPIO37_UART0_RTSn                      PINMUX_FUNCTION_2

/* GPIO38 pinmux function define */ 
#define GPIO38_GPIO38                          PINMUX_FUNCTION_0
#define GPIO38_GPT2_CLKIN                      PINMUX_FUNCTION_1
#define GPIO38_UART1_TXD                       PINMUX_FUNCTION_2
#define GPIO38_SSP1_TXD                        PINMUX_FUNCTION_3

/* GPIO39 pinmux function define */ 
#define GPIO39_GPIO39                          PINMUX_FUNCTION_0
#define GPIO39_GPT3_CLKIN                      PINMUX_FUNCTION_1
#define GPIO39_UART1_RXD                       PINMUX_FUNCTION_2
#define GPIO39_SSP1_RXD                        PINMUX_FUNCTION_3
#define GPIO39_RC32M_CLKIN                     PINMUX_FUNCTION_5

/* GPIO40 pinmux function define */ 
#define GPIO40_GPIO40                          PINMUX_FUNCTION_0
#define GPIO40_GAU_TRIGGER0                    PINMUX_FUNCTION_1
#define GPIO40_ACOMP0_GPIO_OUT                 PINMUX_FUNCTION_2
#define GPIO40_ACOMP1_GPIO_OUT                 PINMUX_FUNCTION_3

/* GPIO41 pinmux function define */ 
#define GPIO41_GPIO41                          PINMUX_FUNCTION_0
#define GPIO41_GAU_TRIGGER1                    PINMUX_FUNCTION_1
#define GPIO41_ACOMP0_EDGE_PULSE               PINMUX_FUNCTION_2
#define GPIO41_ACOMP1_EDGE_PULSE               PINMUX_FUNCTION_3
#define GPIO41_TRACE_CLKOUT                    PINMUX_FUNCTION_5

/* GPIO42 pinmux function define */ 
#define GPIO42_GPIO42                          PINMUX_FUNCTION_0
#define GPIO42_ADC0_IN0                        PINMUX_FUNCTION_1
#define GPIO42_ACOMP_IN0                       PINMUX_FUNCTION_1
#define GPIO42_TS_INP                          PINMUX_FUNCTION_1
#define GPIO42_VOICE_P                         PINMUX_FUNCTION_1
#define GPIO42_UART1_CTSn                      PINMUX_FUNCTION_2
#define GPIO42_SSP1_CLK                        PINMUX_FUNCTION_3
#define GPIO42_TRACE_DATA0                     PINMUX_FUNCTION_4
#define GPIO42_PHY_MON8                        PINMUX_FUNCTION_5

/* GPIO43 pinmux function define */ 
#define GPIO43_GPIO43                          PINMUX_FUNCTION_0
#define GPIO43_ADC0_IN1                        PINMUX_FUNCTION_1
#define GPIO43_ACOMP_IN1                       PINMUX_FUNCTION_1
#define GPIO43_TS_INN                          PINMUX_FUNCTION_1
#define GPIO43_DACB                            PINMUX_FUNCTION_1
#define GPIO43_VOICE_N                         PINMUX_FUNCTION_1
#define GPIO43_UART1_RTSn                      PINMUX_FUNCTION_2
#define GPIO43_SSP1_FRM                        PINMUX_FUNCTION_3
#define GPIO43_TRACE_DATA1                     PINMUX_FUNCTION_4
#define GPIO43_PHY_MON9                        PINMUX_FUNCTION_5

/* GPIO44 pinmux function define */ 
#define GPIO44_GPIO44                          PINMUX_FUNCTION_0
#define GPIO44_ADC0_IN2                        PINMUX_FUNCTION_1
#define GPIO44_ACOMP_IN2                       PINMUX_FUNCTION_1
#define GPIO44_DACA                            PINMUX_FUNCTION_1
#define GPIO44_DEBUG_P                         PINMUX_FUNCTION_1
#define GPIO44_UART1_TXD                       PINMUX_FUNCTION_2
#define GPIO44_SSP1_TXD                        PINMUX_FUNCTION_3
#define GPIO43_TRACE_DATA2                     PINMUX_FUNCTION_4
#define GPIO44_PHY_MON10                       PINMUX_FUNCTION_5

/* GPIO45 pinmux function define */ 
#define GPIO45_GPIO45                          PINMUX_FUNCTION_0
#define GPIO45_ADC0_IN3                        PINMUX_FUNCTION_1
#define GPIO45_ACOMP_IN3                       PINMUX_FUNCTION_1
#define GPIO45_EXT_VREF                        PINMUX_FUNCTION_1
#define GPIO45_UART1_RXD                       PINMUX_FUNCTION_2
#define GPIO45_SSP1_RXD                        PINMUX_FUNCTION_3
#define GPIO43_TRACE_DATA3                     PINMUX_FUNCTION_4
#define GPIO45_PHY_MON11                       PINMUX_FUNCTION_5

/* GPIO46 pinmux function define */ 
#define GPIO46_GPIO46                          PINMUX_FUNCTION_0
#define GPIO46_ADC0_IN4                        PINMUX_FUNCTION_1
#define GPIO46_ACOMP_IN4                       PINMUX_FUNCTION_1
#define GPIO46_UART2_CTSn                      PINMUX_FUNCTION_2
#define GPIO46_SSP2_CLK                        PINMUX_FUNCTION_3
#define GPIO46_PHY_MON12                       PINMUX_FUNCTION_5

/* GPIO47 pinmux function define */ 
#define GPIO47_GPIO47                          PINMUX_FUNCTION_0
#define GPIO47_ADC0_IN5                        PINMUX_FUNCTION_1
#define GPIO47_ACOMP_IN5                       PINMUX_FUNCTION_1
#define GPIO47_UART2_RTSn                      PINMUX_FUNCTION_2
#define GPIO47_SSP2_FRM                        PINMUX_FUNCTION_3
#define GPIO47_PHY_MON13                       PINMUX_FUNCTION_5

/* GPIO48 pinmux function define */ 
#define GPIO48_GPIO48                          PINMUX_FUNCTION_0
#define GPIO48_ADC0_IN6                        PINMUX_FUNCTION_1
#define GPIO48_ACOMP_IN6                       PINMUX_FUNCTION_1
#define GPIO48_UART2_TXD                       PINMUX_FUNCTION_2
#define GPIO48_SSP2_TXD                        PINMUX_FUNCTION_3
#define GPIO48_PHY_MON14                       PINMUX_FUNCTION_5

/* GPIO49 pinmux function define */ 
#define GPIO49_GPIO49                          PINMUX_FUNCTION_0
#define GPIO49_ADC0_IN7                        PINMUX_FUNCTION_1
#define GPIO49_ACOMP_IN7                       PINMUX_FUNCTION_1
#define GPIO49_UART2_RXD                       PINMUX_FUNCTION_2
#define GPIO49_SSP2_RXD                        PINMUX_FUNCTION_3
#define GPIO49_PHY_MON15                       PINMUX_FUNCTION_5

/*@} end of group GPIO_Pinmux */

/** @defgroup GPIO_PINMUX     
 *  @{
 */
#define IS_GPIO_PINMUXFUN(PINMUXFUN)           ((PINMUXFUN) <= PINMUX_FUNCTION_7)

/*@} end of group GPIO_PINMUX */

/** @defgroup GPIO_PIN_MODE     
 *  @{
 */
#define IS_GPIO_PINMODE(PINMODE)               (((PINMODE) == PINMODE_DEFAULT) || \
                                                ((PINMODE) == PINMODE_PULLUP) || \
                                                ((PINMODE) == PINMODE_PULLDOWN) || \
                                                ((PINMODE) == PINMODE_NOPULL) || \
                                                ((PINMODE) == PINMODE_TRISTATE))

/*@} end of group GPIO_PIN_MODE */

/*@} end of group PINMUX_Public_Constants */

/** @defgroup PINMUX_Public_Macro
 *  @{
 */
#define PACKAGE_88_PIN

/*@} end of group PINMUX_Public_Macro */

/** @defgroup PINMUX_Public_FunctionDeclaration
 *  @brief GPIO pinmux functions declaration
 *  @{
 */
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type pinMuxFun);
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode);

/*@} end of group PINMUX_Public_FunctionDeclaration */

/*@} end of group PINMUX */

/*@} end of group MW300_Periph_Driver */ 

#endif /* __MW300_PINMUX_H__ */
