/** @file mw300_clock.h
*
*  @brief This file contains CLOCK driver module header
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

#ifndef __MW300_CLOCK_H__
#define __MW300_CLOCK_H__

#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  Clock 
 *  @{
 */
  
/** @defgroup Clock_Public_Types Clock_Public_Types
 *  @{
 */


/** 
 *  @brief clock source option 
 */
typedef enum
{
  CLK_XTAL_REF = 0,                         /*!< XTAL Clock from WLAN select */
  CLK_RC32M    = 1,                         /*!< RC32M Clock select */
  CLK_XTAL32K  = 2,                         /*!< XT32K Clock select */
  CLK_RC32K    = 3,                         /*!< RC32K Clock select */
  CLK_SFLL     = 4,                         /*!< SFLL Clock select */
  CLK_AUPLL    = 5,                         /*!< AUPLL Clock select */
  CLK_SYSTEM   = 6,                         /*!< System Clock select */
  CLK_REFCLK_AUD = 7,                       /*!< AUDIO Clock from WLAN select */
  CLK_REFCLK_USB = 8,                       /*!< USB Clock from WLAN select */
}CLK_Src_Type;

/** 
 *  @brief uart clock source option 
 */
typedef enum
{
  CLK_UART_FAST = 0,                         /*!< UART Fast Clock select */
  CLK_UART_SLOW = 1,                         /*!< UART Slow Clock select */
}CLK_UartSrc_Type;

/** 
 *  @brief crystal mode option 
 */
typedef enum
{
  CLK_OSC_INTERN = 0,                         /*!< Internal oscillator mode */
  CLK_OSC_EXTERN = 1,                         /*!< External oscillator mode */
}CLK_CrystalMode_Type;

/** 
 *  @brief gpt id option 
 */
typedef enum
{
  CLK_GPT_ID_0   = 0,                       /*!< GPT0 */
  CLK_GPT_ID_1   = 1,                       /*!< GPT1 */
  CLK_GPT_ID_2   = 2,                       /*!< GPT2 */
  CLK_GPT_ID_3   = 3,                       /*!< GPT3 */
}CLK_GptID_Type;

/** 
 *  @brief uart id option 
 */
typedef enum
{
  CLK_UART_ID_0   = 0,                      /*!< UART0 */
  CLK_UART_ID_1   = 1,                      /*!< UART1 */
  CLK_UART_ID_2   = 2,                      /*!< UART2 */
}CLK_UartID_Type;


/** 
 *  @brief ssp id option 
 */
typedef enum
{
  CLK_SSP_ID_0   = 0,                       /*!< SSP0 */
  CLK_SSP_ID_1   = 1,                       /*!< SSP1 */
  CLK_SSP_ID_2   = 2,                       /*!< SSP2 */
}CLK_SspID_Type;

/** 
 *  @brief clock module option 
 */
typedef enum
{
  CLK_QSPI0    = 0,                         /*!< QSPI0 Clock, gating & div */
  CLK_RTC,                                  /*!< RTC Clock, gating */
  CLK_UART0,                                /*!< UART0 Clock, gating */
  CLK_UART1,                                /*!< UART1 Clock, gating */
  CLK_I2C0,                                 /*!< I2C0 Clock, gating & div */
  CLK_SSP0     = 5,                         /*!< SSP0 Clock, gating & div */
  CLK_SSP1,                                 /*!< SSP1 Clock, gating & div */
  CLK_GPT0,                                 /*!< GPT0 Clock, gating & div */
  CLK_GPT1,                                 /*!< GPT1 Clock, gating & div */
  CLK_UART2,                                /*!< UART2 Clock, gating */
  CLK_SSP2     = 11,                        /*!< SSP2 Clock, gating & div */
  CLK_I2C1,                                 /*!< I2C1 Clock, gating */
  CLK_I2C2,                                 /*!< I2C2 Clock, gating */
  CLK_GPT2,                                 /*!< GPT2 Clock, gating & div */
  CLK_GPT3     = 15,                        /*!< GPT3 Clock, gating & div */
  CLK_WDT,                                  /*!< WDT Clock, gating & div */
  CLK_SDIO     = 18,                        /*!< SDIO Clock, gating & div */
//  CLK_DMAC,                                 /*!< DMA Controller Clock, gating */
  CLK_PMU,                                  /*!< PMU Clock, gating & div */  
  CLK_USBC     = 20,                        /*!< USB Controller  Clock, gating */
  CLK_ADC,                                  /*!< ADC Clock, gating */
  CLK_PDM      = 24,                        /*!< PDM Clock, gating & div */
  CLK_GPIO     = 25,                        /*!< GPIO Clock, gating */
  CLK_USBC_AHB,                             /*!< USBC AHB Clock, gating */
  
  CLK_ACOMP_MCLK       = 30,                /*!< ACOMP MCLK Clock, gating */
  CLK_DAC_MCLK,                             /*!< DAC MCLK Clock, gating */
  CLK_ADC0_MCLK     = 33,                   /*!< ADC0 MCLK Clock, gating */
  CLK_BG_MCLK,                              /*!< BG Clock, gating */
  CLK_FLASHC   = 35,                         /*!< Flash Clock, gating */
  CLK_DMA,                                  /*!< DMA Clock, gating */
  CLK_RC32M_REF,                           /*!< RC32M Ref Clock, gating & div */
  
  CLK_CORE,                                 /*!< HCLK/FCLK/AHB Clock, div */
  CLK_APB0,                                 /*!< APB0 Clock, div */
  CLK_APB1    = 40,                         /*!< APB1 Clock, div */
  CLK_GPT_SAMPLE,                           /*!< GPT Sample Clock, div */  
  CLK_GAU,                                  /*!< GAU Clock, div */  
}CLK_Module_Type;

/** 
 *  @brief calibration option(manual or Auto) 
 */
typedef enum
{
  CLK_MANUAL_CAL  = 0,                      /*!< RC32K/RC32M Manual Calibration option */
  CLK_AUTO_CAL    = 1,                      /*!< RC32K/RC32M Auto Calibration option */
}CLK_RCCal_Type;

/**  
 *  @brief clock interrupt type definition 
 */
typedef enum
{
  RC32M_CALDON        = 0,                  /*!< RC32M calibration done interrupt */
  RC32M_CKRDY         = 1,                  /*!< RC32M clock out ready interrupt */
}RC32M_INT_Type; 

/**  
 *  @brief RC32M output frequency definition 
 */
typedef enum
{
  RC32M_SELFREQ_16M        = 0,                  /*!< RC32M output frequency 16 MHz */
  RC32M_SELFREQ_32M        = 1,                  /*!< RC32M output frequency 32 MHz */
}RC32M_SELFREQ_Type; 

/**  
 *  @brief clock fraction structure definition 
 */
typedef struct
{
  uint32_t clkDividend;                     /*!< RC32M calibration done interrupt */
  uint32_t clkDivisor;                      /*!< RC32M clock out ready interrupt */
}CLK_Fraction_Type; 

/** 
 *  @brief SFLL configure structure definition 
 */
typedef struct
{
  CLK_Src_Type     refClockSrc;             /*!< only accept CLK_RC32M (default) and CLK_MAINXTAL */  
  
  uint32_t         refDiv;                  /*!< Divider for reference clock, 8-bit, range 2~255 */
                                            /*!< 0.2 MHz <= Fref (frequency of Reference clock / refDiv) <= 0.4 MHz */
  
  uint32_t         fbDiv;                   /*!< Divider for feedback clock, 11-bit */  
  
  uint32_t         kvco;                    /*!< VCO setting, 2-bit */
                                            /*!< Select KVCO per VCO target frequency */
  
  uint32_t         postDiv;                 /*!< Post divider, 2-bit */
                                            /*!< 2'b00, Fout = Fvco/1 */
                                            /*!< 2'b01, Fout = Fvco/2 */  
                                            /*!< 2'b10, Fout = Fvco/4 */  
                                            /*!< 2'b11, Fout = Fvco/8 */  
}CLK_SfllConfig_Type;


/** 
 *  @brief AUPLL configure structure definition 
 */
typedef struct
{
  uint32_t         refDiv;                  /*!< Divider for reference clock, 4-bit, range: 3 ~ 10 */
  
  uint32_t         fbDiv;                   /*!< Divider for feedback clock, 6-bit, range: 27, 29, 31 ~ 36, 38, 39, 42, 46 */  
  
  uint32_t         icp;                     /*!< charge pump current control, 2-bit, reserved now */
                                            /*!< 2'b00, 2.5 uA */
                                            /*!< 2'b01, 5 uA */  
                                            /*!< 2'b10, 7.5 uA */
                                            /*!< 2'b11, 10 uA */  
  
  uint32_t         postDivPattern;          /*!< Pattern divider for ouput clock, 2-bit, range: 0 ~ 2 */
                                            /*!< 2'b00, pattern = 4.0 */
                                            /*!< 2'b01, pattern = 3.0 */
                                            /*!< 2'b10, pattern = 4.5 */
  
  uint32_t         postDivModulo;           /*!< Modulo divider for ouput clock, 3-bit, range: 0 ~ 2, 4, 7 */
                                            /*!< 3'b000, modulo = 1 */
                                            /*!< 3'b001, modulo = 2 */
                                            /*!< 3'b010, modulo = 4 */
                                            /*!< 3'b100, modulo = 8 */
                                            /*!< 3'b111, modulo = 16 */
  
  FunctionalState  enaDither;               /* ENA_DITHER status */
                                            /*!< 1'b0, Integration only */
                                            /*!< 1'b1, Integration + MASH */  
  
  uint32_t         fract;                   /*!< Fractional PLL feedback divider 20-bits, range: 0 ~ 0x10000 */
  
  FunctionalState  vcoClk_X2;               /* Enable or disable VCOCLK_X2 */
                                            /*!< 1'b0, Disable VCOCLK_X2 */
                                            /*!< 1'b1, Enable VCOCLK_X2 */  
}CLK_AupllConfig_Type;

/*@} end of group Clock_Public_Types definitions */


/** @defgroup Clock_Public_Constants
 *  @{
 */ 

/** @defgroup Clock_Command
 *  @{
 */ 

/*@} end of group Clock_Command */
  

/** @defgroup Clock_Interrupt_Flag
 *  @{
 */

/*@} end of group Clock_Interrupt_Flag */

/*@} end of group Clock_Public_Constants */

/** @defgroup Clock_Public_Macro
 *  @{
 */

/** @defgroup CLK_MAINXTAL_FREQUENCY        
 *  @{
 */
#define CLK_MAINXTAL_FREQUENCY   38400000
/*@} end of group CLK_MAINXTAL_FREQUENCY */

/** @defgroup SYS_Clock_Source        
 *  @{
 */
#define IS_SYSTEM_CLOCK_SOURCE(SOURCE)   ( (((SOURCE) >= 0) && ((SOURCE) <= 1)) || ((SOURCE) == 4) )
/*@} end of group SYS_Clock_Source */

/** @defgroup Clock_Source        
 *  @{
 */
#define IS_REF_CLOCK_SOURCE(SOURCE)       ( ((SOURCE) == 0) || (((SOURCE) >= 7) && ((SOURCE) <= 8)) )
/*@} end of group SYS_Clock_Source */

/** @defgroup SFLL_Ref_Source        
 *  @{
 */
#define IS_SFLL_REF_SOURCE(REF_CLOCK_SRC)   ( ((REF_CLOCK_SRC) >= 0) && ((REF_CLOCK_SRC) <= 1) )
/*@} end of group SFLL_Ref_Source */

/** @defgroup SFLL_Ref_Div        
 *  @{
 */
#define IS_SFLL_REF_DIV(REF_DIV)   ( ((REF_DIV) >= 2) && ((REF_DIV) <= 255) )
/*@} end of group SFLL_Ref_Div */

/** @defgroup SFLL_FB_Div        
 *  @{
 */
#define IS_SFLL_FB_DIV(FB_DIV)   ( ((FB_DIV) >= 1) && ((FB_DIV) <= 2047) )
/*@} end of group SFLL_FB_Div */

/** @defgroup SFLL_KVCO        
 *  @{
 */
#define IS_SFLL_KVCO(KVCO)   ( ((KVCO) >= 0) && ((KVCO) <= 7) )
/*@} end of group SFLL_KVCO */

/** @defgroup SFLL_POSTDIV        
 *  @{
 */
#define IS_SFLL_POSTDIV(POSTDIV)   ( ((POSTDIV) >= 0) && ((POSTDIV) <= 3) )
/*@} end of group SFLL_POSTDIV */

/** @defgroup AUPLL_Ref_Div        
 *  @{
 */
#define IS_AUPLL_REF_DIV(REF_DIV)   ( ((REF_DIV) >= 0x03) && ((REF_DIV) <= 0x0A) )
/*@} end of group AUPLL_Ref_Div */

/** @defgroup AUPLL_FB_Div        
 *  @{
 */
#define IS_AUPLL_FB_DIV(FB_DIV)     ( ((FB_DIV) = 27) \
                                    || ((FB_DIV) == 29) \
                                    || ((((FB_DIV) >= 31)) && ((FB_DIV) <= 36)) \
                                    || ((((FB_DIV) >= 38)) && ((FB_DIV) <= 39)) \
                                    || ((FB_DIV) == 42) \
                                    || ((FB_DIV) == 46) )
/*@} end of group AUPLL_FB_Div */

/** @defgroup AUPLL_ICP
 *  @{
 */
#define IS_AUPLL_ICP(ICP)   ( ((ICP) >= 0) && ((ICP) <= 3) )
/*@} end of group AUPLL_ICP */

/** @defgroup AUPLL_UPDATE_RATE        
 *  @{
 */
//#define IS_AUPLL_UPDATE_RATE(RATE)   ( (RATE >= 0) && (RATE <= 1) )
/*@} end of group AUPLL_UPDATE_RATE */

/** @defgroup AUPLL_POSTDIV_PATTERN       
 *  @{
 */
#define IS_AUPLL_POSTDIV_PATTERN(POSTDIV_PATTERN)   ( ((POSTDIV_PATTERN) >= 0) && ((POSTDIV_PATTERN) <= 2) )
/*@} end of group AUPLL_POSTDIV_PATTERN */

/** @defgroup AUPLL_POSTDIV_MODULO        
 *  @{
 */
#define IS_AUPLL_POSTDIV_MODULO(POSTDIV_MODULO)     ( ( ((POSTDIV_MODULO) >= 0) && ((POSTDIV_MODULO) <= 2) )  \
                                                    || ((POSTDIV_MODULO) == 4) \
                                                    || ((POSTDIV_MODULO) >= 7) )
/*@} end of group AUPLL_POSTDIV_MODULO */

/** @defgroup OSC_MODE        
 *  @{
 */
#define IS_OSC_MODE(MODE)   ( ((MODE) >= 0) && ((MODE) <= 1) )
/*@} end of group OSC_MODE */

/** @defgroup RC_CALIBRATION_MODE        
 *  @{
 */
#define IS_RC_CALIBRATION_MODE(MODE)   ( ((MODE) >= 0) && ((MODE) <= 1) )
/*@} end of group RC_CALIBRATION_MODE */

/** @defgroup CLK_SRC_STATUS        
 *  @{
 */
#define IS_CLK_SRC_STATUS(CLKSRC)   ( ((CLKSRC) >= 0) && ((CLKSRC) <= 5) )
/*@} end of group CLK_SRC_STATUS  */

/** @defgroup GATED_CLK_MODULE        
 *  @{
 */
#define IS_GATED_CLK_MODULE(MODULE)   ( ((MODULE) >= 0) && ((MODULE) <= 37) )
/*@} end of group GATED_CLK_MODULE  */

/** @defgroup DIVIDED_CLK_MODULE        
 *  @{
 */
#define IS_DIVIDED_CLK_MODULE(MODULE)  ( ((MODULE) == 11)    \
                                       || (((MODULE) >= 3) && ((MODULE) <= 11))    \
                                       || ((MODULE) == 11)                       \
                                       || (((MODULE) >= 14) && ((MODULE) <= 19))   \
                                       || (((MODULE) >= 22) && ((MODULE) <= 24))   \
                                       || (((MODULE) >= 27) && ((MODULE) <= 29))   \
                                       || (((MODULE) >= 37) && ((MODULE) <= 42)) )
/*@} end of group DIVIDED_CLK_MODULE  */

/** @defgroup APB_I2C_DIVIDER        
 *  @{
 */
#define IS_APB_I2C_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 3) )
/*@} end of group APB_I2C_DIVIDER  */

/** @defgroup QSPI_FLASH_GPTSAMP_DIVIDER        
 *  @{
 */
#define IS_QSPI_FLASH_GPTSAMP_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 7) )
/*@} end of group QSPI_FLASH_GPTSAMP_DIVIDER  */


/** @defgroup SSP_DIVIDER        
 *  @{
 */
#define IS_SSP_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 31) )
/*@} end of group SSP_DIVIDER  */

/** @defgroup CORE_GPT_DIVIDER        
 *  @{ 
 */
#define IS_CORE_GPT_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 63) )
/*@} end of group CORE_GPT_DIVIDER  */

/** @defgroup GAU_Clock_Source        
 *  @{
 */
#define IS_GAU_Clock_Source(SOURCE)   ( (((SOURCE) >= 0) && ((SOURCE) <= 1)) || ((SOURCE) == 5) )
/*@} end of group GAU_Clock_Source */

/** @defgroup GPT_Clock_Source        
 *  @{
 */
#define IS_GPT_Clock_Source(SOURCE)   ( (((SOURCE) >= 0) && ((SOURCE) <= 3)) || ((SOURCE) == 6) )
/*@} end of group GPT_Clock_Source */

/** @defgroup GAU_RC32M_DIVIDER        
 *  @{ 
 */
#define IS_GAU_RC32M_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 31) )
/*@} end of group GAU_RC32M_DIVIDER  */

/** @defgroup PDM_DIVIDER        
 *  @{ 
 */
#define IS_PDM_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 31) )
/*@} end of group CORE_GPT_DIVIDER  */

/** @defgroup PDM_Clock_Source        
 *  @{
 */
#define IS_PDM_Clock_Source(SOURCE)   ( ((SOURCE) >= 5) && ((SOURCE) <= 6) )
/*@} end of group SSP_Clock_Source */

/** @defgroup SDIO_PMU_DIVIDER        
 *  @{
 */
#define IS_SDIO_PMU_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 15) )
/*@} end of group SDIO_PMU_DIVIDER  */

/** @defgroup SSP_Clock_Source        
 *  @{
 */
#define IS_SSP_Clock_Source(SOURCE)   ( ((SOURCE) >= 5) && ((SOURCE) <= 6) )
/*@} end of group SSP_Clock_Source */

/** @defgroup RTC_Clock_Source        
 *  @{
 */
#define IS_RTC_Clock_Source(SOURCE)   ( ((SOURCE) >= 2) && ((SOURCE) <= 3) )
/*@} end of group RTC_Clock_Source */

/** @defgroup UART_Clock_Source        
 *  @{
 */
#define IS_UART_Clock_Source(SOURCE)   ( ((SOURCE) >= 0) && ((SOURCE) <= 1) )
/*@} end of group UART_Clock_Source */

/** @defgroup UART_Dividend        
 *  @{
 */
#define IS_UART_Dividend(DIVIDEND)   ( ((DIVIDEND) >= 1) && ((DIVIDEND) <= 2047) )
/*@} end of group UART_Dividend */

/** @defgroup UART_Divisor        
 *  @{
 */
#define IS_UART_Divisor(DIVISOR)   ( ((DIVISOR) >= 1) && ((DIVISOR) <= 8191) )
/*@} end of group UART_Divisor */

/** @defgroup WDT_DIVIDER        
 *  @{
 */
#define IS_WDT_DIVIDER(DIVIDER)   ( ((DIVIDER) >= 0) && ((DIVIDER) <= 63) )
/*@} end of group CORE_GPT_DIVIDER  */


/*@} end of group Clock_Public_Macro */

/** @defgroup Clock_Public_FunctionDeclaration
 *  @brief Clock functions statement
 *  @{
 */

/** Select RC32M as a reference clock to PLL
 *
 * The MC200/MW300 processor's SFLL can use either of the following
 * 2 clock sources as a reference clock:
 * 1) Externally connected crystal
 * 2) Internal RC oscillator.
 *
 * This function sets internal RC32M as a reference clock to SFLL. By default,
 * 38.4 MHz external crystal routed through WLAN is used as reference clock to
 * SFLL.
 * This function initially switches system clock to RC32M, disables PLL and
 * reconfigures it to use RC32M as a reference clock and sets system clock
 * source to SFLL.
 */
void CLK_RC32M_SfllRefClk(void);
void CLK_Xtal32KEnable(CLK_CrystalMode_Type oscMode);
void CLK_Xtal32KDisable(void);
void CLK_RC32MEnable(void);
void CLK_RC32MDisable(void);
void CLK_RC32KEnable(void);
void CLK_RC32KDisable(void);
int32_t CLK_RC32MCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);
int32_t CLK_RC32KCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);
void CLK_SfllEnable(CLK_SfllConfig_Type * sfllConfigSet);
void CLK_SfllDisable(void);
void CLK_AupllRefClkSrc(CLK_Src_Type clockSource);
void CLK_AupllEnable(CLK_AupllConfig_Type * aupllConfigSet);
void CLK_AupllDisable(void);
void CLK_SetAupllPostDiv(uint8_t postDivPattern, uint8_t postDivModulo);
FlagStatus CLK_GetClkStatus(CLK_Src_Type clockSource);
void CLK_SystemClkSrc(CLK_Src_Type clockSource);
uint32_t CLK_GetSystemClk(void);
void CLK_GAUClkSrc(CLK_Src_Type clockSource);
void CLK_GPTInternalClkSrc(CLK_GptID_Type gptIndex, CLK_Src_Type clockSource);
void CLK_SSPClkSrc(CLK_SspID_Type sspIndex, CLK_Src_Type clockSource);
void CLK_RTCClkSrc(CLK_Src_Type clockSource);
void CLK_PDMClkSrc(CLK_Src_Type clockSource);
void CLK_UARTDividerSet(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type uartClkFraction);
void CLK_SetUARTClkSrc(CLK_UartID_Type uartIndex, CLK_UartSrc_Type uartClockSource);
CLK_UartSrc_Type CLK_GetUARTClkSrc(CLK_UartID_Type uartIndex);
void CLK_GetUARTDivider(CLK_UartSrc_Type uartClockSource, CLK_Fraction_Type * uartClkFraction);
void CLK_ModuleClkEnable(CLK_Module_Type clkModule);
void CLK_ModuleClkDisable(CLK_Module_Type clkModule);
void CLK_ModuleClkDivider(CLK_Module_Type clkModule, uint32_t divider);
int32_t CLK_GetModuleClkDivider(CLK_Module_Type clkModule);
void CLK_RefClkEnable(CLK_Src_Type clkSrc);
void CLK_RefClkDisable(CLK_Src_Type clkSrc);
IntStatus RC32M_GetIntStatus(RC32M_INT_Type intType);
void RC32M_SelOutput32MHz(RC32M_SELFREQ_Type freqType);
void RC32M_IntMask(RC32M_INT_Type intType, IntMask_Type intMask);
void RC32M_IntClr(RC32M_INT_Type intType);
void RC32M_IRQHandler(void);

/*@} end of group Clock_Public_FunctionDeclaration */

/*@} end of group Clock */

/*@} end of group MW300_Periph_Driver */

#endif
