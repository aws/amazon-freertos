/** @file mw300_interrupt.h
*
*  @brief This file contains MW300 interrupt handler header
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

#ifndef __MW300_INTERRUPT_H_
#define __MW300_INTERRUPT_H_

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

/**
 * \brief Interrupt Number Definition, according to the selected device 
 *        in @ref Library_configuration_section 
 */
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  specific Interrupt Numbers ***************************************************************/
  ExtPin0_IRQn                = 0,      /*!< External Pin 0 Interrupt                             */
  ExtPin1_IRQn                = 1,      /*!< External Pin 1 Interrupt                             */
  RTC_IRQn                    = 2,      /*!< RTC Interrupt                                        */
  CRC_IRQn                    = 3,      /*!< CRC Interrupt                                        */
  AES_IRQn                    = 4,      /*!< AES Interrupt                                        */
  I2C0_IRQn                   = 5,      /*!< I2C0 Interrupt                                       */
  I2C1_IRQn                   = 6,      /*!< I2C1 Interrupt                                       */
  RESERVED1_IRQn              = 7,      /*!< RESERVED Interrupt                                   */
  DMA_IRQn                    = 8,      /*!< DMA Interrupt                                        */
  GPIO_IRQn                   = 9,      /*!< GPIO Interrupt                                       */
  SSP0_IRQn                   = 10,     /*!< SSP0 Interrupt                                       */
  SSP1_IRQn                   = 11,     /*!< SSP1 Interrupt                                       */
  SSP2_IRQn                   = 12,     /*!< SSP2 Interrupt                                       */
  QSPI_IRQn                   = 13,     /*!< QSPI Interrupt                                      */
  GPT0_IRQn                   = 14,     /*!< GPT0 Interrupt                                       */
  GPT1_IRQn                   = 15,     /*!< GPT1 Interrupt                                       */
  GPT2_IRQn                   = 16,     /*!< GPT2 Interrupt                                       */
  GPT3_IRQn                   = 17,     /*!< GPT3 Interrupt                                       */
  UART0_IRQn                  = 18,     /*!< UART0 Interrupt                                      */
  UART1_IRQn                  = 19,     /*!< UART1 Interrupt                                      */ 
  UART2_IRQn                  = 20,     /*!< UART2 Interrupt                                      */
  RESERVED2_IRQn              = 21,     /*!< RESERVED Interrupt                                   */
  WDT_IRQn                    = 22,     /*!< WDT Interrupt                                        */
  RESERVED3_IRQn              = 23,     /*!< RESERVED Interrupt                                   */
  ADC0_IRQn                   = 24,     /*!< ADC0 Interrupt                                       */
  DAC_IRQn                    = 25,     /*!< DAC Interrupt                                        */
  ACOMPWKUP_IRQn              = 26,     /*!< ACOMP_WKUP Interrupt                                 */
  ACOMP_IRQn                  = 27,     /*!< ACOMP Interrupt                                      */
  SDIO_IRQn                   = 28,     /*!< SDIO Interrupt                                       */
  USB_IRQn                    = 29,     /*!< USB Interrupt                                        */
  RESERVED4_IRQn              = 30,     /*!< RESERVED Interrupt                                   */
  PLL_IRQn                    = 31,     /*!< PLL Interrupt                                        */
  RESERVED5_IRQn              = 32,     /*!< RESERVED Interrupt                                   */
  RC32M_IRQn                  = 33,     /*!< RC32M Interrupt                                      */
  ExtPin3_IRQn                = 34,     /*!< External Pin 3 Interrupt                             */
  ExtPin4_IRQn                = 35,     /*!< External Pin 4 Interrupt                             */
  ExtPin5_IRQn                = 36,     /*!< External Pin 5 Interrupt                             */
  ExtPin6_IRQn                = 37,     /*!< External Pin 6 Interrupt                             */
  ExtPin7_IRQn                = 38,     /*!< External Pin 7 Interrupt                             */
  ExtPin8_IRQn                = 39,     /*!< External Pin 8 Interrupt                             */
  ExtPin9_IRQn                = 40,     /*!< External Pin 9 Interrupt                             */
  ExtPin10_IRQn               = 41,     /*!< External Pin 10 Interrupt                            */
  ExtPin11_IRQn               = 42,     /*!< External Pin 11 Interrupt                            */
  ExtPin12_IRQn               = 43,     /*!< External Pin 12 Interrupt                            */
  ExtPin13_IRQn               = 44,     /*!< External Pin 13 Interrupt                            */
  ExtPin14_IRQn               = 45,     /*!< External Pin 14 Interrupt                            */
  ExtPin15_IRQn               = 46,     /*!< External Pin 15 Interrupt                            */
  ExtPin16_IRQn               = 47,     /*!< External Pin 16 Interrupt                            */
  ExtPin17_IRQn               = 48,     /*!< External Pin 17 Interrupt                            */
  ExtPin18_IRQn               = 49,     /*!< External Pin 18 Interrupt                            */
  ExtPin19_IRQn               = 50,     /*!< External Pin 19 Interrupt                            */
  ExtPin20_IRQn               = 51,     /*!< External Pin 20 Interrupt                            */
  ExtPin21_IRQn               = 52,     /*!< External Pin 21 Interrupt                            */
  ExtPin22_IRQn               = 53,     /*!< External Pin 22 Interrupt                            */
  ExtPin23_IRQn               = 54,     /*!< External Pin 23 Interrupt                            */
  ExtPin24_IRQn               = 55,     /*!< External Pin 24 Interrupt                            */
  ExtPin25_IRQn               = 56,     /*!< External Pin 25 Interrupt                            */
  ExtPin26_IRQn               = 57,     /*!< External Pin 26 Interrupt                            */
  ExtPin27_IRQn               = 58,     /*!< External Pin 27 Interrupt                            */
  RESERVED6_IRQn              = 59,     /*!< RESERVED Interrupt                                   */
  RESERVED7_IRQn              = 60,     /*!< RESERVED Interrupt                                   */
  BRNDET_IRQn                 = 61,     /*!< BRNDET Interrupt                                     */
  WIFIWKUP_IRQn               = 62,     /*!< WiFi Wakeup Interrupt                                */
} IRQn_Type;

#if defined ( __ICCARM__ ) || defined ( __CC_ARM )
#define WEAK __weak
#define ASM __asm
#define NAKED
#endif

#if defined ( __GNUC__ )
#define WEAK __attribute__ ((weak))
#define ASM __asm__ volatile
#define NAKED __attribute__ ((naked))
#endif

#endif /* __MW300_INTERRUPT_H */

/******************* (C) COPYRIGHT 2010 Marvell Technology *****END OF FILE****/
