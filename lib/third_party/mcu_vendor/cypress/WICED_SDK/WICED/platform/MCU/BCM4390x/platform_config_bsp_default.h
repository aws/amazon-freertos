/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
 * Defines BSP-related configuration.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Constant to be used to define others */
#ifdef WICED_NO_WIFI
#define PLATFORM_WLAN_PRESENT             0
#else
#define PLATFORM_WLAN_PRESENT             1
#endif

/* Define which frequency CPU run */
#ifndef PLATFORM_CPU_CLOCK_FREQUENCY
#if defined(PLATFORM_4390X_OVERCLOCK)
#define PLATFORM_CPU_CLOCK_FREQUENCY      PLATFORM_CPU_CLOCK_FREQUENCY_480_MHZ
#else
#define PLATFORM_CPU_CLOCK_FREQUENCY      PLATFORM_CPU_CLOCK_FREQUENCY_320_MHZ
#endif /* PLATFORM_4390X_OVERCLOCK */
#endif

/* Common switch to be used to enable/disable whether the backplane clock is
 * sourced from the ARM clock by the local divide-by-2.
 */
#ifndef PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE
#define PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE  0
#endif /* PLATFORM_BACKPLANE_ON_CPU_CLOCK_ENABLE */

/* Common switch to be used to enable/disable various modules powersaving. */
#ifndef PLATFORM_POWERSAVE_DEFAULT
#define PLATFORM_POWERSAVE_DEFAULT        0
#endif

/* Define WLAN powersave feature en/dis */
#ifndef PLATFORM_WLAN_POWERSAVE
#define PLATFORM_WLAN_POWERSAVE           PLATFORM_WLAN_PRESENT
#endif

/* Define cores powersave feature en/dis */
#ifndef PLATFORM_CORES_POWERSAVE
#define PLATFORM_CORES_POWERSAVE          PLATFORM_POWERSAVE_DEFAULT
#endif

/* Define APPS domain powersave feature en/dis */
#ifndef PLATFORM_APPS_POWERSAVE
#define PLATFORM_APPS_POWERSAVE           PLATFORM_POWERSAVE_DEFAULT
#endif

/* Define ticks powersave feature en/dis */
#ifndef PLATFORM_TICK_POWERSAVE
#define PLATFORM_TICK_POWERSAVE           PLATFORM_APPS_POWERSAVE
#endif

/* Define initial state of MCU power-save enable/disable API */
#ifndef PLATFORM_MCU_POWERSAVE_INIT_STATE
#define PLATFORM_MCU_POWERSAVE_INIT_STATE 1
#endif

/* Define MCU powersave mode set during platform initialization */
#ifndef PLATFORM_MCU_POWERSAVE_MODE_INIT
#define PLATFORM_MCU_POWERSAVE_MODE_INIT  PLATFORM_MCU_POWERSAVE_MODE_SLEEP
#endif

/* Define tick powersave mode set during platform initialization */
#ifndef PLATFORM_TICK_POWERSAVE_MODE_INIT
#define PLATFORM_TICK_POWERSAVE_MODE_INIT PLATFORM_TICK_POWERSAVE_MODE_TICKLESS_IF_MCU_POWERSAVE_ENABLED
#endif

/* Define RTOS timers default configuration */
//
#ifndef PLATFORM_TICK_TINY
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_TICK_TINY                1
#else
#define PLATFORM_TICK_TINY                0
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_TICK_TINY */
//
#ifndef PLATFORM_TICK_CPU
#define PLATFORM_TICK_CPU                 !PLATFORM_TICK_TINY
#endif
//
#ifndef PLATFORM_TICK_PMU
#if PLATFORM_APPS_POWERSAVE
#define PLATFORM_TICK_PMU                 !PLATFORM_TICK_TINY
#else
#define PLATFORM_TICK_PMU                 0
#endif /* PLATFORM_APPS_POWERSAVE */
#endif /* PLATFORM_TICK_PMU */

/* By default enable ticks statistic */
#ifndef PLATFORM_TICK_STATS
#define PLATFORM_TICK_STATS               !PLATFORM_TICK_TINY
#endif

/* By default switch off WLAN powersave statistic */
#ifndef PLATFORM_WLAN_POWERSAVE_STATS
#define PLATFORM_WLAN_POWERSAVE_STATS     0
#endif

/* By default use external LPO */
#ifndef PLATFORM_LPO_CLOCK_EXT
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_LPO_CLOCK_EXT            0
#else
#define PLATFORM_LPO_CLOCK_EXT            1
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_LPO_CLOCK_EXT */

/* Define various HIB parameters */
// Disable various HIB parameters by one shot
#ifndef PLATFORM_HIB_NOT_AVAILABLE
#define PLATFORM_HIB_NOT_AVAILABLE        0
#endif
#if PLATFORM_HIB_NOT_AVAILABLE
#define PLATFORM_HIB_ENABLE               0
#define PLATFORM_HIB_CLOCK_AS_EXT_LPO     0
#define PLATFORM_HIB_CLOCK_POWER_UP       0
#endif /* PLATFORM_HIB_NOT_AVAILABLE */
// HIB provide clock to be used as external LPO, use it by default
#ifndef PLATFORM_HIB_CLOCK_AS_EXT_LPO
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_HIB_CLOCK_AS_EXT_LPO     0
#else
#define PLATFORM_HIB_CLOCK_AS_EXT_LPO     1
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_HIB_CLOCK_AS_EXT_LPO */
// Power-up clocks in bootloader
#ifndef PLATFORM_HIB_CLOCK_POWER_UP
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_HIB_CLOCK_POWER_UP       1
#else
#define PLATFORM_HIB_CLOCK_POWER_UP       0
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_HIB_CLOCK_POWER_UP */
//
#ifndef PLATFORM_HIB_ENABLE
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_HIB_ENABLE               0
#else
#define PLATFORM_HIB_ENABLE               1
#endif /* BOOTLOADER || TINY_BOOTLOADER */
#endif /* PLATFORM_HIB_ENABLE */

/* Define DDR default configuration */
#ifndef PLATFORM_DDR_HEAP_SIZE_CONFIG
#define PLATFORM_DDR_HEAP_SIZE_CONFIG     0 /* can be defined per-application */
#endif
//
#ifndef PLATFORM_DDR_CODE_AND_DATA_ENABLE
#define PLATFORM_DDR_CODE_AND_DATA_ENABLE 0 /* can be defined per-platform as it require to specifically build application and bootloader same time */
#endif
//
#if PLATFORM_DDR_CODE_AND_DATA_ENABLE
#ifdef PLATFORM_NO_DDR
#if PLATFORM_NO_DDR != 0
#error "Misconfiguration"
#endif /* PLATFORM_NO_DDR */
#else
#define PLATFORM_NO_DDR                   0
#endif /* PLATFORM_NO_DDR */
#endif /* PLATFORM_DDR_CODE_AND_DATA_ENABLE */
//
#if PLATFORM_DDR_CODE_AND_DATA_ENABLE
#ifdef BOOTLOADER
#define PLATFORM_DDR_SKIP_INIT            0
#else
#define PLATFORM_DDR_SKIP_INIT            1
#endif /* BOOTLOADER */
#else
#define PLATFORM_DDR_SKIP_INIT            0
#endif /* PLATFORM_DDR_CODE_AND_DATA_ENABLE */
//
#ifndef PLATFORM_NO_DDR
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_NO_DDR                   1
#else
#define PLATFORM_NO_DDR                   0
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_NO_DDR */

/* Define SPI flash default configuration */
#ifndef PLATFORM_NO_SFLASH_WRITE
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#if defined(PLATFORM_HAS_OTA) || defined(OTA2_SUPPORT)
#define PLATFORM_NO_SFLASH_WRITE          0
#else
#define PLATFORM_NO_SFLASH_WRITE          1
#endif /* OTA2_SUPPORT           */
#else
#define PLATFORM_NO_SFLASH_WRITE          0
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_NO_SFLASH_WRITE */

/* Define backplane configuration */
#ifndef PLATFORM_NO_BP_INIT
#if defined(BOOTLOADER) || defined(TINY_BOOTLOADER)
#define PLATFORM_NO_BP_INIT               1
#else
#define PLATFORM_NO_BP_INIT               0
#ifndef PLATFORM_BP_TIMEOUT
#define PLATFORM_BP_TIMEOUT               0xFF
#endif /* PLATFORM_BP_TIMEOUT */
#endif /* BOOTLODER || TINY_BOOTLOADER */
#endif /* PLATFORM_NO_BP_INIT */

 /* TraceX storage buffer in DDR */
#if defined(TX_ENABLE_EVENT_TRACE) && (PLATFORM_NO_DDR != 1)
#ifndef WICED_TRACEX_BUFFER_DDR_OFFSET
#define WICED_TRACEX_BUFFER_DDR_OFFSET    (0x0)
#endif
#define WICED_TRACEX_BUFFER_ADDRESS       ((uint8_t *)PLATFORM_DDR_BASE(WICED_TRACEX_BUFFER_DDR_OFFSET))
#endif

/* Define that platform require some fixup to use ALP clock. No need for chips starting from B1. */
#ifndef PLATFORM_ALP_CLOCK_RES_FIXUP
#define PLATFORM_ALP_CLOCK_RES_FIXUP      PLATFORM_WLAN_PRESENT
#endif

/* Define platform USB require some fixup to use ALP clock. Necessary for A0/B0/B1 chips. */
#ifndef PLATFORM_USB_ALP_CLOCK_RES_FIXUP
#define PLATFORM_USB_ALP_CLOCK_RES_FIXUP  PLATFORM_WLAN_PRESENT
#endif

/* Define that platform need WLAN assistance to wake-up */
#ifndef PLATFORM_WLAN_ASSISTED_WAKEUP
#define PLATFORM_WLAN_ASSISTED_WAKEUP     PLATFORM_WLAN_PRESENT
#endif

/* Define that by default platform no need extra hook */
#ifndef PLATFORM_IRQ_DEMUXER_HOOK
#define PLATFORM_IRQ_DEMUXER_HOOK         0
#endif

#ifndef PLATFORM_LPLDO_VOLTAGE
#define PLATFORM_LPLDO_VOLTAGE            PMU_REGULATOR_LPLDO1_0_9_V
#endif

#ifndef PLATFORM_NO_SDIO
#define PLATFORM_NO_SDIO                  0
#endif

#ifndef PLATFORM_NO_GMAC
#define PLATFORM_NO_GMAC                  0
#endif

#ifndef PLATFORM_NO_USB
#define PLATFORM_NO_USB                   0
#endif

#ifndef PLATFORM_NO_HSIC
#define PLATFORM_NO_HSIC                  0
#endif

#ifndef PLATFORM_NO_I2S
#define PLATFORM_NO_I2S                   0
#endif

#ifndef PLATFORM_NO_I2C
#define PLATFORM_NO_I2C                   0
#endif

#ifndef PLATFORM_NO_UART
#define PLATFORM_NO_UART                  0
#endif

#ifndef PLATFORM_NO_DDR
#define PLATFORM_NO_DDR                   0
#endif

#ifndef PLATFORM_NO_SPI
#define PLATFORM_NO_SPI                   0
#endif

#ifndef PLATFORM_NO_JTAG
#define PLATFORM_NO_JTAG                  0
#endif

#ifndef PLATFORM_NO_PWM
#define PLATFORM_NO_PWM                   0
#endif

#ifndef PLATFORM_NO_SOCSRAM_POWERDOWN
#define PLATFORM_NO_SOCSRAM_POWERDOWN     0
#endif

#ifndef PLATFORM_HIB_WAKE_CTRL_REG_RCCODE
#define PLATFORM_HIB_WAKE_CTRL_REG_RCCODE -1
#endif

/* PLL Channel 5 and 6 , which provide 24Mhz and 48Mhz freq are
 * disabled in WLAN FIRMWARE to minimize BBPLL spurs.
 * Enable these frequencies only if there is no WLAN FW running or
 * PLL Channel 5 and 6 are enabled in WLAN FW.
 *
 * There are two cases which can result in no WLAN FW running:
 *
 * 1. Loading of WLAN FW is disabled at build time by defining
 *    NO_WIFI or NO_WIFI_FIRMWARE or WICED_NO_WIFI.
 *
 * 2. Application does not call wiced_init() which starts WLAN FW.
 *
 * For case 1, the below macro sets PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED to 1
 * For case 2, Application should define PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED=1
 * in its makefile to use 24 and 48Mhz Apps CPU or Backplane frequencies.
 */
#ifndef PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED
#if defined( NO_WIFI ) || defined( NO_WIFI_FIRMWARE ) || defined( WICED_NO_WIFI )
#define PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED 1
#elif defined( PLATFORM_PLL_CHANNEL_5_AND_6_ENABLED_IN_WLAN_FW )
#define PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED 1
#else
#define PLATFORM_BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED 0
#endif
#endif /* ifndef BCM4390X_APPS_CPU_FREQ_24_AND_48_MHZ_ENABLED */

#ifndef __ASSEMBLER__
#include "platform_isr.h"
#include "platform_map.h"
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
