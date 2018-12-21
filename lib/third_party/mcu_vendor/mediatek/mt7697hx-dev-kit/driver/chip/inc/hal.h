/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_H__
#define __HAL_H__

/**
 * @addtogroup HAL
 * @{
 * This section introduces the HAL driver APIs including terms and acronyms, supported features, software architecture,
 * details on how to use this driver, HAL function groups, enums, structures and functions.
 * @section HAL_Overview_1_Chapter    Terms and acronyms
 *
 * |Acronyms                         |Definition                                                           |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ACCDET               | Accessory detector
 * |\b ADC                  | Analog-to-digital converter
 * |\b AES                  | Advanced Encryption Standard
 * |\b CMSIS                | Cortex Microcontroller Software Interface Standard
 * |\b CTP                  | Capacitive Touch Panel
 * |\b EINT                 | External interrupt
 * |\b GDMA                 | General direct memory access
 * |\b GPC                  | General purpose counter
 * |\b GPIO                 | General purpose Input Output
 * |\b GPT                  | General purpose timer
 * |\b I2C                  | Inter-integrated circuit
 * |\b I2S                  | Inter-integrated sound
 * |\b LCD                  | Liquid Crystal Display
 * |\b LCM                  | LCD Module
 * |\b MD5                  | Message-Digest Algorithm
 * |\b MPU                  | Memory protect unit
 * |\b PWM                  | Pulse Width Modulation
 * |\b RTC                  | Real-time clock
 * |\b SD                   | Secure Digital Memory Card
 * |\b SDIO                 | Secure Digital Input and Output
 * |\b SHA                  | Secure Hash Algorithm
 * |\b SPI                  | Serial Peripheral Interface
 * |\b UART                 | Universal asynchronous receiver/transmitter
 * |\b USB                  | Universal Serial Bus
 * |\b WDT                  | Watchdog timer
 * |\b TRNG                 | True random number generator
 *
 * @section HAL_Overview_2_Chapter    Overview of HAL drivers
 *
 * - \b Introduction \n
 *   MediaTek LinkIt™ SDK hardware abstraction layer (HAL) driver is embedded software ensuring maximized portability across IoT portfolio designed for MediaTek LinkIt for real-time OS (RTOS) development platform.\n
 *   The HAL driver includes:\n
 *   - Drivers for the peripheral modules, such as UART and I2C.\n
 *   - Drivers for the system modules, such as CACHE, MPU and FLASH.\n
 *   - APIs of the modules.\n
 *
 *   The HAL driver has both standard and advanced features and is easy to use. It makes the upper hardware abstraction layer portable.\n
 *   The HAL layer provides a generic set of APIs to interact with the upper layers, such as application, middleware and the OS. The APIs are defined interfaces for MediaTek IoT portfolio, so the upper layer written with the HAL API is portable across the MediaTek IoT platforms.\n
 *   The HAL driver covers rich modules, including all peripheral modules, such as UART, I2C and SPI. It also includes number of system modules, such as CACHE, MPU, FLASH and DMA.\n
 *   The HAL complies with the general naming convention and common coding style. Each module of the HAL has its distinctive folder structure along with supporting examples.
 * - \b HAL \b driver \b architecture \n
 * The HAL driver architecture is shown below:\n
 * @image html hal_overview_driver_architecture.png
 * - \b HAL \b folder \b structure \b and \b file \b inclusion \b model \n
 *  - \b HAL \b folder \b structure \n
 *    The HAL module folder structure is shown below:
 * @image html hal_overview_folder_structure.png
 *    - Driver: All common driver files, such as board driver, HAL driver and CMSIS.\n
 *    - Driver/board: The driver files that are associated with the board, such as CTP or LCM.\n
 *    - Driver/chip/inc: The public header files of the HAL.\n
 *    - Driver/chip/mtxxxx: The driver files that are specific to the chipset, such as UART, I2C, GPT, PWM, WDT, etc.\n
 *    - Driver/CMSIS: The files that are associated with the CMSIS-CORE.\n
 *
 *  - \b File \b inclusion \b model \n
 * @image html hal_overview_file_inclusion_model.png
 *
 * - \b HAL \b driver \b rules \n
 *  - \b HAL \b file  \b naming \b convention \n
 *       The HAL public header files are named as hal_{feature}.h, such as hal_adc.h.\n
 *       The HAL driver files are all in driver/chip/mtxxxx/inc and driver/chip/mtxxxx/src,
 *       they are named as hal_{feature}.c or hal_{feature}_{sub_feature}.c,
 *       such as hal_cache.c and hal_cache_internal.c.\n
 *  - \b HAL \b public \b API \b naming \b convention \n
 *       |                       |naming convention                                                             |
 *       |-----------------------|------------------------------------------------------------------------|
 *       |function names         | hal_{feature}_{sub_feature}(), such as #hal_uart_init()
 *       |enum names             | Type name:hal_{feature}_{sub_feature}_t, such as #hal_uart_baudrate_t. \n Enum member names:HAL_{FEATURE}_{SUB_FEATURE}, such as #HAL_UART_BAUDRATE_110.
 *       |struct name            | Type name:hal_{feature}_{sub_feature}_t,such as #hal_uart_config_t.\n Struct member names must be lowercase.
 *       |macro names            | HAL_{FEATURE}_{SUB_FEATURE}, such as #HAL_NVIC_QUERY_EXCEPTION_NUMBER
 *       |function pointer names | hal_{feature}_{sub_feature}_t(), such as void (*#hal_uart_callback_t)().\n
 *
 * - \b HAL \b configuration \n
 *  - \b hal_feature_config.h \n
 *      Every project should include hal_feature_config.h for HAL configuration.\n
 *      If certain HAL modules are not used, they can be removed from the feature options by undefining the macro, such as undefine the option of HAL_ADC_MODULE_ENABLED.\n
 *     For customized parameter settings, such as on the LinkIt 2523, if MTK_SYSTEM_CLOCK_208M, MTK_SYSTEM_CLOCK_104M or MTK_SYSTEM_CLOCK_26M is defined, the system MCU frequency will be set to 208MHz, 104MHz or 26MHz at the bootup.\n
 *  - \b hal_feature_config_template.h \n
 *       This is a template file in driver/chip/mtxxxx/inc. It can be referred to when creating a new hal_feature_config.h.
 * \n
 * \n
 * @section HAL_Overview_3_Chapter    HAL Driver Model
 * Some of the drivers can operate in two modes: polling and interrupt.
 * The UART HAL driver, for example, can operate in polling and interrupt modes during data communication. The polling mode is suitable for read and write operations of small amount of data in low frequency. The interrupt mode is suitable for read and write operations of small amount of data in high frequency. In UART DMA mode, for example, an interrupt is triggered once the DMA is complete.
 * - \b Polling \b mode \b architecture \n
 *   Most of the driver modules support the polling mode, which is the basic feature in HAL.\n
 *   For example, call the #hal_gpt_delay_ms() API to apply a millisecond time delay in the GPT. The GPT HAL driver will poll the GPT hardware, if the time delay is reached.\n
 *   The polling mode driver architecture is shown below:
 *   @image html hal_overview_driver_polling_architecture.png
 *
 * - \b Interrupt \b mode \b architecture \n
 *   To improve the portability, the HAL driver hides the OS dependence by preventing the upper layer to call the OS level services directly. The HAL drivers uses an interrupt handler to provide the upper layer with a callback mechanism.
 *
 *   To use the interrupt handler of a driver module, a callback should be registered with the interrupt handler. When the interrupt occurs, the callback is invoked in Cortex-M4 handler mode.
 *   Call the #hal_uart_send_dma() API to send data in the UART interrupt mode (UART DMA mode). The UART HAL driver will copy the data to DMA buffer and start the DMA operation while the DMA sends data to the UART hardware. When the DMA operation completes, the DMA interrupt is issued. The UART HAL driver will invoke the user callback in interrupt handler to notify that the data has been sent out by the UART.
 *   Another example of the interrupt is the GPT interrupt mode. Call the #hal_gpt_start_timer_ms() API to start a timer. When the timer expires, the GPT hardware issues an interrupt. Then the GPT HAL driver will invoke the user callback in interrupt handler to notify that the timer has expired.
 *   The interrupt mode driver architecture is shown below:
 *   @image html hal_overview_interrupt_mode_architecture.png
 *
 *   All HAL APIs are thread-safe and available for ISR. Developers may call HAL APIs in any FreeRTOS task or in an ISR.\n
 *   However, some hardware have limited resources, and the corresponding APIs return "ERROR_BUSY" status when there is a resource conflict due to the re-entrance. Developers should always check the return values from HAL APIs.\n
 *   For example, I2C master supports only one user at a time, so #hal_i2c_master_init() returns #HAL_I2C_STATUS_ERROR_BUSY when another user is using I2C master.\n
 *
 * @}
 */


#include "hal_feature_config.h"
#include "hal_log.h"

/*****************************************************************************
* module header file include
*****************************************************************************/
#ifdef HAL_ACCDET_MODULE_ENABLED
#include "hal_accdet.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
#include "hal_adc.h"
#endif

#ifdef HAL_AES_MODULE_ENABLED
#include "hal_aes.h"
#endif

#ifdef HAL_DAC_MODULE_ENABLED
#include "hal_dac.h"
#endif

#ifdef HAL_DES_MODULE_ENABLED
#include "hal_des.h"
#endif

#ifdef HAL_AUDIO_MODULE_ENABLED
#include "hal_audio.h"
#endif

#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_clock.h"
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif

#ifdef HAL_DES_MODULE_ENABLED
#include "hal_des.h"
#endif

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_eint.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
#include "hal_flash.h"
#endif

#ifdef HAL_GDMA_MODULE_ENABLED
#include "hal_gdma.h"
#endif

#ifdef HAL_GPC_MODULE_ENABLED
#include "hal_gpc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#include "hal_gpio.h"
#endif

#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt.h"
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#include "hal_i2c_master.h"
#endif

#ifdef HAL_I2S_MODULE_ENABLED
#include "hal_i2s.h"
#endif

#ifdef HAL_IRRX_MODULE_ENABLED
#include "hal_irrx.h"
#endif

#ifdef HAL_IRTX_MODULE_ENABLED
#include "hal_irtx.h"
#endif

#ifdef HAL_ISINK_MODULE_ENABLED
#include "hal_isink.h"
#endif

#ifdef HAL_KEYPAD_MODULE_ENABLED
#include "hal_keypad.h"
#endif

#ifdef HAL_MD5_MODULE_ENABLED
#include "hal_md5.h"
#endif

#ifdef HAL_MPU_MODULE_ENABLED
#include "hal_mpu.h"
#endif

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic.h"
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#include "hal_pwm.h"
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#include "hal_rtc.h"
#endif

#ifdef HAL_SD_MODULE_ENABLED
#include "hal_sd.h"
#endif

#ifdef HAL_SDIO_MODULE_ENABLED
#include "hal_sdio.h"
#endif

#ifdef HAL_SHA_MODULE_ENABLED
#include "hal_sha.h"
#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#include "hal_spi_master.h"
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#include "hal_spi_slave.h"
#endif

#ifdef HAL_TRNG_MODULE_ENABLED
#include "hal_trng.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
#include "hal_uart.h"
#endif

#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#endif

#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#endif

#ifdef HAL_G2D_MODULE_ENABLED
#include "hal_g2d.h"
#endif

#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* __HAL_H__ */

