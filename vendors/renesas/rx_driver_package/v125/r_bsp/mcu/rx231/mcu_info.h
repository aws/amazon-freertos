/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_info.h
* Device(s)    : RX231
* Description  : Information about the MCU.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 29.12.2014 1.00     First Release
*         : 30.09.2015 1.01     Changed Minor version to 3.01
*         : 01.10.2016 1.02     Changed the value of the following macro definition.
*                               - BSP_LPTSRCCLK_HZ
*         : 15.05.2017 1.10     Added the following macro definition.
*                               - BSP_PACKAGE_WFLGA64
*                               Deleted the following macro definition.
*                               - BSP_PACKAGE_LQFP64
*                               Deleted following package.
*                               - FK = 0x3 = LQFP/64/0.80
*                               - LJ = 0xA = TFLGA/100/0.65
*                               Deleted the following ROM, RAM, and Data Flash Capacity.
*                               - 3 = 0x3 = 64KB/12KB/8KB
*                               Changed the value of the following macro definition.
*                               - BSP_CFG_MCU_PART_MEMORY_SIZE - Changed the value from "5 = 0x5 = 128KB/20KB/8KB"
*                                                               to "5 = 0x5 = 128KB/32KB/8KB".
*         : 01.11.2017 2.00     Added definition not select clock for LPT clock source.
*                               Added the following macro definition.
*                               - BSP_ILOCO_HZ
*         : 28.02.2019 3.00     Added macro definition of MCU functions.
*                               Added the following macro definition.
*                               - BSP_MCU_CPU_VERSION
*                               - CPU_CYCLES_PER_LOOP
*                               Fixed coding style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Gets MCU configuration information. */
#include "r_bsp_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef MCU_INFO
#define MCU_INFO

/* MCU CPU Version */
#define BSP_MCU_CPU_VERSION    (2)

/* CPU cycles. Known number of RXv2 CPU cycles required to execute the delay_wait() loop */
#define CPU_CYCLES_PER_LOOP    (4)

/* MCU Series. */
#if   BSP_CFG_MCU_PART_SERIES == 0x0
    #define BSP_MCU_SERIES_RX200    (1)
#else
    #error "ERROR - BSP_CFG_MCU_PART_SERIES - Unknown MCU Series chosen in r_bsp_config.h"
#endif

/* This macro means that this MCU is part of the RX23x collection of MCUs (i.e. RX231). */
#define BSP_MCU_RX23_ALL            (1)

/* MCU Group name. */
#if   BSP_CFG_MCU_PART_GROUP == 0x0
    #define BSP_MCU_RX231           (1)
#else
    #error "ERROR - BSP_CFG_MCU_PART_GROUP - Unknown MCU Group chosen in r_bsp_config.h"
#endif

/* Package. */
#if   BSP_CFG_MCU_PART_PACKAGE == 0x0
    #define BSP_PACKAGE_LFQFP100    (1)
    #define BSP_PACKAGE_PINS        (100)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x1
    #define BSP_PACKAGE_WFLGA64     (1)
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x2
    #define BSP_PACKAGE_LFQFP64     (1)
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x5
    #define BSP_PACKAGE_HWQFN64     (1)
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x6
    #define BSP_PACKAGE_LFQFP48     (1)
    #define BSP_PACKAGE_PINS        (48)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x7
    #define BSP_PACKAGE_HWQFN48     (1)
    #define BSP_PACKAGE_PINS        (48)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x9
    #define BSP_PACKAGE_TFLGA100    (1)
    #define BSP_PACKAGE_PINS        (100)
#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

/* Memory size of your MCU. */
#if   BSP_CFG_MCU_PART_MEMORY_SIZE == 0x5
    #define BSP_ROM_SIZE_BYTES              (131072)
    #define BSP_RAM_SIZE_BYTES              (32768)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x6
    #define BSP_ROM_SIZE_BYTES              (262144)
    #define BSP_RAM_SIZE_BYTES              (32768)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x7
    #define BSP_ROM_SIZE_BYTES              (393216)
    #define BSP_RAM_SIZE_BYTES              (65536)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x8
    #define BSP_ROM_SIZE_BYTES              (524288)
    #define BSP_RAM_SIZE_BYTES              (65536)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#else
    #error "ERROR - BSP_CFG_MCU_PART_MEMORY_SIZE - Unknown memory size chosen in r_bsp_config.h"
#endif

/* These macros define clock speeds for fixed speed clocks. */
#define BSP_LOCO_HZ                 (4000000)
#define BSP_SUB_CLOCK_HZ            (32768)
#define BSP_ILOCO_HZ                (15000)

/* Define frequency of HOCO. */
#if   BSP_CFG_HOCO_FREQUENCY == 0
    #define BSP_HOCO_HZ                 (32000000)
#elif BSP_CFG_HOCO_FREQUENCY == 3
    #define BSP_HOCO_HZ                 (54000000)
#else
    #error "ERROR - Invalid HOCO frequency chosen in r_bsp_config.h! Set valid value for BSP_CFG_HOCO_FREQUENCY."
#endif

/* Clock source select (CKSEL).
   0 = Low Speed On-Chip Oscillator  (LOCO)
   1 = High Speed On-Chip Oscillator (HOCO)
   2 = Main Clock Oscillator
   3 = Sub-Clock Oscillator
   4 = PLL Circuit
*/ 
#if   BSP_CFG_CLOCK_SOURCE == 0
    #define BSP_SELECTED_CLOCK_HZ   (BSP_LOCO_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 1
    #define BSP_SELECTED_CLOCK_HZ   (BSP_HOCO_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 2
    #define BSP_SELECTED_CLOCK_HZ   (BSP_CFG_XTAL_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 3
    #define BSP_SELECTED_CLOCK_HZ   (BSP_SUB_CLOCK_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 4
    #define BSP_SELECTED_CLOCK_HZ   ((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL)
#else
    #error "ERROR - BSP_CFG_CLOCK_SOURCE - Unknown clock source chosen in r_bsp_config.h"
#endif

/* USB clock speed in Hz. */
#if   BSP_CFG_USB_CLOCK_SOURCE == 0
    #define BSP_UCLK_HZ             (BSP_ICLK_HZ)
#elif BSP_CFG_USB_CLOCK_SOURCE == 1
    #define BSP_UCLK_HZ             ((BSP_CFG_XTAL_HZ/BSP_CFG_UPLL_DIV) * BSP_CFG_UPLL_MUL)
#else
    #error "ERROR - BSP_CFG_USB_CLOCK_SOURCE - Unknown usb clock source chosen in r_bsp_config.h"
#endif

/* SSI clock speed in Hz */
#define BSP_SSISCK_HZ               (BSP_CFG_XTAL_HZ)

/* LPT clock speed in Hz */
#if   BSP_CFG_LPT_CLOCK_SOURCE == 0
    #define BSP_LPTSRCCLK_HZ        (BSP_SUB_CLOCK_HZ)
#elif BSP_CFG_LPT_CLOCK_SOURCE == 1
    #define BSP_LPTSRCCLK_HZ        (BSP_ILOCO_HZ)                              /* IWDTCLK typical frequency */
#elif BSP_CFG_LPT_CLOCK_SOURCE == 2
    /* LPT none used */
#else
    #error "ERROR - BSP_CFG_LPT_CLOCK_SOURCE - Unknown lpt clock source chosen in r_bsp_config.h"
#endif

/* CAN clock speed in Hz */
#define BSP_CANCLK_HZ               (BSP_CFG_XTAL_HZ)

/* System clock speed in Hz. */
#define BSP_ICLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_ICK_DIV)
/* Peripheral Module Clock A speed in Hz. */
#define BSP_PCLKA_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKA_DIV)
/* Peripheral Module Clock B speed in Hz. */
#define BSP_PCLKB_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKB_DIV)
/* Peripheral Module Clock D speed in Hz. */
#define BSP_PCLKD_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKD_DIV)
/* FlashIF clock speed in Hz. */
#define BSP_FCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_FCK_DIV)
/* External bus clock speed in Hz. */
#define BSP_BCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_BCK_DIV)

/* Null argument definitions. */
#define FIT_NO_FUNC                 ((void (*)(void *))0x10000000)  /* Reserved space on RX */
#define FIT_NO_PTR                  ((void *)0x10000000)            /* Reserved space on RX */

/* Mininum and maximum IPL levels available for this MCU. */
#define BSP_MCU_IPL_MAX             (0xF)
#define BSP_MCU_IPL_MIN             (0)

/* Maximum frequency on Middle-speed operating mode. */
#define BSP_MIDDLE_SPEED_MAX_FREQUENCY      (12000000)
/* Maximum frequency to not need memory wait. */
#define BSP_MEMORY_NO_WAIT_MAX_FREQUENCY    (32000000)

/* MCU functions */
#define BSP_MCU_REGISTER_WRITE_PROTECTION
#define BSP_MCU_RCPC_PRC0
#define BSP_MCU_RCPC_PRC1
#define BSP_MCU_RCPC_PRC2
#define BSP_MCU_RCPC_PRC3
#define BSP_MCU_FLOATING_POINT
#define BSP_MCU_EXCEPTION_TABLE
#define BSP_MCU_EXCEP_SUPERVISOR_INST_ISR
#define BSP_MCU_EXCEP_ACCESS_ISR
#define BSP_MCU_EXCEP_UNDEFINED_INST_ISR
#define BSP_MCU_EXCEP_FLOATING_POINT_ISR
#define BSP_MCU_NON_MASKABLE_ISR
#define BSP_MCU_UNDEFINED_INTERRUPT_SOURCE_ISR
#define BSP_MCU_BUS_ERROR_ISR

#define BSP_MCU_NMI_EXC_NMI_PIN
#define BSP_MCU_NMI_OSC_STOP_DETECT
#define BSP_MCU_NMI_WDT_ERROR
#define BSP_MCU_NMI_IWDT_ERROR
#define BSP_MCU_NMI_LVD1
#define BSP_MCU_NMI_LVD2
#define BSP_MCU_NMI_VBATT

#endif /* MCU_INFO */

