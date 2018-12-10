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

#ifndef __HAL_SD_H__
#define __HAL_SD_H__

#include "hal_platform.h"

#ifdef HAL_SD_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 * This section  introduces the Secure Digital Memory Dard/Embedded Multi Media Card(SD/eMMC) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, SD/eMMC function groups, enums, structures and functions.
 *
 * @section HAL_SD_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b CID                        | Card identification number register. For an introduction to the CID, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a> or <a href="http://www.jedec.org/standards-documents/docs/jesd84-a441">MultiMediaCard System Specification Version 4.41 </a>.|
 * |\b CSD                        | Card Specific Data register. For an introduction to the CSD, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a> or <a href="http://www.jedec.org/standards-documents/docs/jesd84-a441">MultiMediaCard System Specification Version 4.41 </a>.|
 * |\b DSR                        | Driver Stage Register. For an introduction to the DSR, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a> or <a href="http://www.jedec.org/standards-documents/docs/jesd84-a441">MultiMediaCard System Specification Version 4.41 </a>.|
 * |\b MSDC                       | MS/SD memory card controller, that support the MultiMediaCard System Specification Version 4.41,SD Memory Card Specification Version 2.0 and SDIO Card Specification Version 2.0.|
 * |\b OCR                        | Operation Conditions Register. For an introduction to the OCR, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a> or <a href="http://www.jedec.org/standards-documents/docs/jesd84-a441">MultiMediaCard System Specification Version 4.41 </a>.|
 * |\b RCA                        | Relative Card Address register. For an introduction to the RCA, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a> or <a href="http://www.jedec.org/standards-documents/docs/jesd84-a441">MultiMediaCard System Specification Version 4.41 </a>.|
 * |\b SD                         | Secure Digital Memory Card. For an introduction to the Secure Digital Memory Card, please refer to <a href="https://en.wikipedia.org/wiki/Secure_Digital"> Secure Digital in Wikipedia </a>. |
 * |\b VSS                        | Voltage Source Supply (ground), or power supply ground.|
 * |\b eMMC                       | Embedded Multi Media Card. For an introduction to the Embedded Multi Media Card, please refer to <a href="https://en.wikipedia.org/wiki/MultiMediaCard"> MultiMediaCard in Wikipedia </a>.|
 *
 * @section HAL_SD_Features_Chapter Supported features
 *
 * - \b Supports \b DMA \b  and \b MCU \b modes. \n
 *   - \b DMA \b mode: In this mode, the DMA hardware carries data directly from the RAM to the MSDC FIFO and Stores in the SD/eMMC card or
 *                 read data directly from the MSDC FIFO to the RAM. Base on the consideration of transmission speed, this mode is recommended when each transmission is greater than 2 blocks.
 *   - \b MCU \b mode: In this mode, the MCU writes data from the RAM to the MSDC FIFO and Stores in the SD/eMMC card or
 *                 read data from the MSDC FIFO to the RAM. Base on the consideration of transmission speed, this mode is recommended when each transmission is less than or equal to 2 blocks.
 *   .
 *   There are two APIs for the DMA mode, #hal_sd_read_blocks_dma() to read data from the SD/eMMC card and #hal_sd_write_blocks_dma() to write data to the card.
 *   Similarly, there are two APIs for the MCU mode, #hal_sd_read_blocks() to read data from the SD/eMMC card and #hal_sd_write_blocks() to write data to the card.  \n
 * \n
 * - \b Supports \b 1-bit \b or \b 4-bit \b bus \b width. \n
 *         1-bit bus width or 4-bit bus width is available to select for data transmission. For 1-bit bus width, only data line 0 (DAT0) is enabled to transfer data, The data line 1-3 (DAT[3:1]) cannot be in used.
 *     All four data line (DAT[3:0]) are available in the 4-bit bus width mode. The corresponding API is #hal_sd_set_bus_width(). \n
 * \n
 * - \b Supports \b card \b detection. \n
 *         Connect one of the EINT pins to the SD card VSS2 pin on the device, then register EINT callback function in the software.
 *     Once the SD card is inserted or removed, an EINT interrupt occurs and the registered callback is executed. Please note that the SD card detection
 *     (HAL_SD_CARD_DETECTION) only works for the SD card. If the eMMC card is in use, the SD card detection (HAL_SD_CARD_DETECTION) should be switched off. The
 *     EPT (Easy Pinmux Tool) is used to configure the SD card detection pin for the EINT mode. Follow the steps below to configure the SD card detection pin in the EPT tool.\n
 *     - Step1: Select the correct GPIO pin for the EINT mode and PU-47K_ohms in EPT
 *              tool's GPIO Setting window.
 *     - Step2: Select the corresponding EINT pin's name for 'HAL_MSDC_EINT' in
 *              EPT tool's EINT Setting window.
 *     - Step3: Switch on the corresponding feature option (HAL_SD_CARD_DETECTION).
 *
 * @section HAL_SD_Architecture_Chapter Software architecture of the SD/eMMC
 *
 *    Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card. Then the SD/eMMC card gets into a transfer state and
 * users can call #hal_sd_write_blocks() or #hal_sd_write_blocks_dma() to write data to the SD/eMMC card, or call #hal_sd_read_blocks() or #hal_sd_read_blocks_dma() to read data
 * from the SD/eMMC card. Other supported APIs can also be called, such as #hal_sd_erase_sectors(). Register an EINT callback function for card detection and a callback
 * function will execute when the card is inserted or plugged out. \n
 *
 *
 * @section HAL_SD_Driver_Usage_Chapter How to use this driver
 *
 * - Read from or write to the SD/eMMC in the MCU mode. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_read_blocks() to read the SD/eMMC card data.
 *  - Step5: Call #hal_sd_write_blocks() to write data to the SD/eMMC card.
 *  - Sample code:
 *    @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t read_buffer;
 *       uint32_t write_buffer;
 *       uint32_t start_address;
 *       uint32_t block_number;
 *
 *       sd_port=HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *           //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_read_blocks(sd_port, &read_buffer, start_address, block_number)) {
 *           //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_write_blocks(sd_port, &write_data, start_address, block_number)) {
 *           //error handle
 *       }
 *    @endcode
 * - Read from or write to the SD/eMMC in the DMA mode. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_read_blocks_dma() to read the SD/eMMC card data.
 *  - Step5: Call #hal_sd_write_blocks_dma() to write data to the SD/eMMC card.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t read_buffer;
 *       uint32_t write_buffer;
 *       uint32_t start_address;
 *       uint32_t block_number;
 *
 *       sd_port=HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *           //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_read_blocks_dma(sd_port, &read_buffer, start_address, block_number)) {
 *           //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_write_blocks_dma(sd_port, &read_buffer, start_address, block_number)) {
 *           //error handle
 *       }
 *    @endcode
 * - Erase the SD/eMMC sector. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_erase_sector_size() to get the erase sector unit size.
 *  - Step5: Call #hal_sd_erase_sectors() to erase the SD/eMMC card.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *
 *       uint32_t  sector_size;
 *       uint32_t  sector_number;
 *       uint32_t  erase_size;
 *       uint32_t  start_sector;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_EARSE_DONE != hal_sd_get_erase_sector_size(sd_port, &sector_size)) {
 *           //error handle
 *       }
 *       sector_number = erase_size / sector_size;
 *       if (HAL_SD_STATUS_EARSE_DONE != hal_sd_erase_sectors(sd_port, start_sector, sector_number)) {
 *           //error handle
 *       }
 *    @endcode
 * - Get the SD/eMMC capacity. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_capacity() to get the SD/eMMC card capacity.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint64_t card_capacity;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_capacity(sd_port, &card_capacity)) {
 *          //error handle
 *       }
 *      @endcode
 * - Get the SD/eMMC CSD register value. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_csd() to get the SD/eMMC card CSD register value.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       hal_pinmux_set_function(gpio_pin, function_index);//Set the GPIO pinmux.
 *       uint32_t card_csd;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_csd(sd_port, &card_csd)) {
 *          //error handle
 *       }
 *      @endcode
 * - Get the SD/eMMC CID register value. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_cid() to get the SD/eMMC card CID register value.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t card_cid;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_cid(sd_port, &card_cid)) {
 *          //error handle
 *       }
 *      @endcode
 * - Get the SD/eMMC OCR register value. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_ocr() to get the SD/eMMC card OCR register value.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t card_ocr;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_ocr(sd_port, &card_ocr)) {
 *          //error handle
 *       }
 *      @endcode
 * - Get the SD/eMMC card type. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_card_type() to get the SD/eMMC card OCR register value.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       hal_sd_card_type_t card_type;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_card_type(sd_port, &card_type)) {
 *          //error handle
 *       }
 *      @endcode
 * - Get the SD/eMMC erase sector size. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Step4: Call #hal_sd_get_erase_sector_size() to get the SD/eMMC card OCR register value.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t erase_sector_size;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_get_erase_sector_size(sd_port, &erase_sector_size)) {
 *          //error handle
 *       }
 *      @endcode
 * - Retry mechanism is used for read from or write to the SD/eMMC in the DMA mode. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API again if init fail and the retry times is less than 3.
 *  - Step4: Call #hal_sd_read_blocks_dma() to read the SD/eMMC card data. Call this API again if read fail and the retry times is less than 3.
 *  - Step5: Call #hal_sd_write_blocks_dma() to write data to the SD/eMMC card. Call this API again if write fail and the retry times is less than 3.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *       uint32_t read_buffer;
 *       uint32_t write_buffer;
 *       uint32_t start_address;
 *       uint32_t block_number;
 *       uint32_t retry;
 *
 *       sd_port=HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *       retry = 0;
 *       while (retry < 3) {
 *           if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *               retry++;
 *           } else {
 *               break;
 *           }
 *       }
 *
 *       if (3 <= retry) {
 *           //error handle
 *       }
 *
 *       retry = 0;
 *       while (retry < 3) {
 *           if (HAL_SD_STATUS_OK != hal_sd_read_blocks_dma(sd_port, &read_buffer, start_address, block_number)) {
 *               //error handle
 *           } else {
 *               break;
 *           }
 *       }
 *
 *       if (3 <= retry) {
 *           //error handle
 *       }
 *
 *       retry = 0;
 *       while (retry < 3) {
 *           if (HAL_SD_STATUS_OK != hal_sd_write_blocks_dma(sd_port, &read_buffer, start_address, block_number)) {
 *               //error handle
 *           } else {
 *               break;
 *           }
 *       }
 *
 *       if (3 <= retry) {
 *           //error handle
 *       }
 *    @endcode
 */

/**
* @}
* @}
*/

#ifdef HAL_SD_CARD_DETECTION
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 * - Register the SD card detection callback. \n
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux if the EPT is not in use.
 *  - Step3: Call #hal_sd_register_card_detection_callback() to register the SD card detection callback.
 *  - Step4: Call #hal_sd_init() to initialize the MSDC and the SD/eMMC card transfer states. Call this API only once.
 *  - Sample code:
 *      @code
 *       hal_sd_port_t sd_port;
 *       hal_sd_config_t sd_config;
 *
 *       sd_port = HAL_SD_PORT_0;
 *       sd_config.bus_width=HAL_SD_BUS_WIDTH_4;
 *       //Set output clock. The output clock is recommended set to 45000(kHz) and no need to modify.
 *       sd_config.clock=45000;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *       hal_gpio_init(HAL_GPIO_33);
 *       hal_gpio_init(HAL_GPIO_34);
 *       hal_gpio_init(HAL_GPIO_35);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_30_MC0_CK
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);
 *       //function_index = HAL_GPIO_31_MC0_CM0
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index);
 *       //function_index = HAL_GPIO_32_MC0_DA0
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);
 *       //function_index = HAL_GPIO_33_MC0_DA1
 *       hal_pinmux_set_function(HAL_GPIO_33, function_index);
 *       //function_index = HAL_GPIO_34_MC0_DA2
 *       hal_pinmux_set_function(HAL_GPIO_34, function_index);
 *       //function_index = HAL_GPIO_35_MC0_DA3
 *       hal_pinmux_set_function(HAL_GPIO_35, function_index);
 *
 *     void sd_detection_callback(hal_sd_card_event_t sd_detection_event, void *user_data)
 *     {
 *          if (HAL_SD_EVENT_CARD_INSERTED == sd_detection_event) {
 *             //Card insert handler.
 *          } else if (HAL_SD_EVENT_CARD_REMOVED == sd_detection_event) {
 *             //Card remove handler.
 *          }
 *      }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_register_card_detection_callback(sd_port, sd_detection_callback, NULL)) {
 *          //error handle
 *       }
 *
 *
 *       if (HAL_SD_STATUS_OK != hal_sd_init(sd_port, &sd_config)) {
 *          //error handle
 *       }
 *      @endcode
 *
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_sd_enum Enum
  * @{
  */

/** @brief  This enum defines the SD/eMMC API's return type. */
typedef enum {
    HAL_SD_STATUS_BUSY                       = -2,                        /**<  The SD/eMMC bus is busy error occurred. */
    HAL_SD_STATUS_ERROR                      = -1,                        /**<  An error occurred and the operation failed. */
    HAL_SD_STATUS_OK                         =  0,                        /**<  No error occurred during the function call. */
} hal_sd_status_t;


/** @brief  This enum defines the SD/eMMC bus width types*/
typedef enum {
    HAL_SD_BUS_WIDTH_1 = 1,                                        /**<  The SD/eMMC read/write data with 1 bit bus width. */
    HAL_SD_BUS_WIDTH_4 = 2                                         /**<  The SD/eMMC read/write data with 4 bit bus width. */
} hal_sd_bus_width_t;


/** @brief  This enum define the SD/eMMC card type*/
typedef enum {
    HAL_SD_TYPE_SD_CARD            = 0,                          /** SD card. */
    HAL_SD_TYPE_MMC_CARD           = 1,                          /** MMC card. */
    HAL_SD_TYPE_SD20_LCS_CARD      = 2,                          /** SD 2.0 card and capacity less than 2GB. */
    HAL_SD_TYPE_SD20_HCS_CARD      = 3,                          /** SD 2.0 card and capacity larger than 2GB. */
    HAL_SD_TYPE_MMC42_CARD         = 4,                          /** EMMC 4.2 and above card. */
    HAL_SD_TYPE_CARD_TYPE_MAX      = 5,
    HAL_SD_TYPE_UNKNOWN_CARD       = 0xff,                       /** Unknown card. */
} hal_sd_card_type_t;

#ifdef HAL_SD_CARD_DETECTION
/** @brief This enum defines the card detection result event. The detection event is passed to the user defined callback function.
  *  For more details, please refer to function #hal_sd_register_card_detection_callback().
  */
typedef enum {
    HAL_SD_EVENT_CARD_INSERTED = 0,           /**<  An SD card is inserted. */
    HAL_SD_EVENT_CARD_REMOVED = 1,            /**<  An SD card is removed. */
} hal_sd_card_event_t;
#endif

/**
  * @}
  */


/** @defgroup hal_sd_struct Struct
  * @{
  */

/** @brief   This structure define the SD/eMMC initialize parameter */
typedef struct {
    hal_sd_bus_width_t bus_width;                                  /**< The SD/eMMC read/write bus width. */
    uint32_t           clock;                                      /**< The MSDC I/O bus clock, the unit is kHz. */
} hal_sd_config_t;

/**
  * @}
  */

#ifdef HAL_SD_CARD_DETECTION
/** @defgroup hal_sd_typedef Typedef
  * @{
  */
/** @brief    This defines the callback function prototype.
 *         Register a callback function to handle a card detection.
 *         This function is called after the card is inserted or plugged out.
 *           For more details about the callback, please refer to #hal_sd_register_card_detection_callback().
 *  @param [in] event is the event of the card detection. This parameter specifies whether the SD card is inserted or not.
 *              For more details about the event type, please refer to #hal_sd_card_event_t.
 *  @param [in] user_data is the user defined parameter obtained from #hal_sd_register_card_detection_callback() function.
 */
typedef void (*hal_sd_callback_t)(hal_sd_card_event_t event, void *user_data);
/**
  * @}
  */
#endif

/**
 * @brief  This function initializes the MSDC and SD/eMMC card. It can also set the MSDC output clock and bus width.
 *            The MSDC output clock is recommended set to 45000(kHz) as good and no need to modify.
 * @param[in] sd_port is the initialization configuration port. For more details about this parameter, please refer to #hal_sd_port_t.
 * @param[in] sd_config is the initialization configuration parameter. For more details about this parameter, please refer to #hal_sd_config_t.
 * @return    Indicates whether this function call is successful or not.
 *            If the return value is #HAL_SD_STATUS_OK,the call successded, else the initialization failed.
 * @sa  #hal_sd_deinit()
 */
hal_sd_status_t hal_sd_init(hal_sd_port_t sd_port, hal_sd_config_t *sd_config);


/**
 * @brief     This function deinitializes the MSDC and the SD/eMMC settings.
 * @param[in] sd_port is the MSDC deinitialization port.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 * @sa  #hal_sd_init()
 */
hal_sd_status_t hal_sd_deinit(hal_sd_port_t sd_port);

#ifdef HAL_SD_CARD_DETECTION
/**
 * @brief This function registers a callback function to detect a card.
 * @param[in] sd_port is the MSDC port  to detect a card.
 * @param[in] sd_callback is the function pointer of the callback.
 *            The callback function is called once the SD/eMMC card is inserted or plugged out.
 * @param[in] user_data is the callback parameter.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 */
hal_sd_status_t hal_sd_register_card_detection_callback(hal_sd_port_t sd_port, hal_sd_callback_t sd_callback, void *user_data);
#endif

/**
 * @brief     This function sets a bus width for MSDC and the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to be set.
 * @param[in] bus_width is the SD/eMMC card's bus width.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, set the bus width error occurred.
 */
hal_sd_status_t hal_sd_set_bus_width(hal_sd_port_t sd_port, hal_sd_bus_width_t bus_width);


/**
 * @brief     This function sets the output clock of the MSDC.
 * @param[in] sd_port is the MSDC port to set the clock.
 * @param[in] clock is the expected output clock of the MSDC. It should be less than 50000, the unit is kHz.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_set_clock(hal_sd_port_t sd_port, uint32_t clock);


/**
 * @brief     This function gets the output clock of the MSDC.
 * @param[in] sd_port is the MSDC port to get the clock.
 * @param[out] clock is the current output clock of the MSDC, the unit is kHz.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 */
hal_sd_status_t hal_sd_get_clock(hal_sd_port_t sd_port, uint32_t *clock);


/**
 * @brief     This function gets the card capacity of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to get the card capacity.
 * @param[out] capacity is the SD/eMMC card capacity, the unit is bytes.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 */
hal_sd_status_t hal_sd_get_capacity(hal_sd_port_t sd_port, uint64_t *capacity);


/**
 * @brief     This function gets the sector size of the SD/eMMC card, mainly for erase purpose.
 * @param[in] sd_port is the MSDC port to get erase sector size.
 * @param[out] erase_sector_size is the card erase sector size of the SD/eMMC card, the unit is bytes.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_erase_sector_size(hal_sd_port_t sd_port, uint32_t *erase_sector_size);


/**
 * @brief     This function erases the card sectors of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to erase the card sectors.
 * @param[in] start_sector is the start address of a sector on the SD/eMMC card to erase.
 * @param[in] sector_number is the sector number of the SD/eMMC card to erase.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_erase_sectors(hal_sd_port_t sd_port, uint32_t start_sector, uint32_t sector_number);


/**
 * @brief     This function reads data from the SD/eMMC card in the MCU mode.
 *               Base on the consideration of transmission speed, this API is recommended when each transmission is less than or equal to 2 blocks.
 * @param[in] sd_port is the MSDC port to read.
 * @param[out] read_buffer is the address to store the data read from the card.
 * @param[in] start_address is the start address on the SD/eMMC card to read from.
 * @param[in] block_number is the block number on the SD/eMMC card to read.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_read_blocks(hal_sd_port_t sd_port, uint32_t *read_buffer, uint32_t start_address, uint32_t block_number);


/**
 * @brief     This function writes data to the SD/eMMC card in the MCU mode.
 *               Base on the consideration of transmission speed, this API is recommended when each transmission is less than or equal to 2 blocks.
 * @param[in] sd_port is the MSDC port to write.
 * @param[in] write_data is the address to store the data that will be written.
 * @param[in] start_address is the start address on the SD/eMMC card to write into.
 * @param[in] block_number is the block number on the SD/eMMC card to write.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_write_blocks(hal_sd_port_t sd_port, const uint32_t *write_data, uint32_t start_address, uint32_t block_number);

/**
 * @brief      This function reads data from the SD/eMMC card in the DMA mode. This API may block the application task.
 *                Base on the consideration of transmission speed, this API is recommended when each transmission is greater than 2 blocks.
 * @param[in] sd_port is the MSDC port to read.
 * @param[out] read_buffer is the address to store data read from the card. The address must be a noncacheable and 4 bytes aligned address.
 * @param[in] start_address is the read start address of the SD/eMMC card.
 * @param[in] block_number is the block number on the SD/eMMC card to read.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_read_blocks_dma(hal_sd_port_t sd_port, uint32_t *read_buffer, uint32_t start_address, uint32_t block_number);


/**
 * @brief     This function writes data to the SD/eMMC card in the DMA mode. This API may block the application task.
 *               Base on the consideration of transmission speed, this API is recommended when each transmission is greater than 2 blocks.
 * @param[in] sd_port is the MSDC port to write.
 * @param[in] write_data is the address to store the data that will be written, the address must be a noncacheable and 4 bytes aligned address.
 * @param[in] start_address is the start address on the SD/eMMC card to write into.
 * @param[in] block_number is the block number of the SD/eMMC card to write.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            #HAL_SD_STATUS_BUSY, the MSDC is busy.
 */
hal_sd_status_t hal_sd_write_blocks_dma(hal_sd_port_t sd_port, const uint32_t *write_data, uint32_t start_address, uint32_t block_number);


/**
 * @brief     This function gets the CSD register value of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to get the CSD register value.
 * @param[out] csd is the CSD register value of the SD/eMMC card.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_csd(hal_sd_port_t sd_port, uint32_t *csd);


/**
 * @brief     This function gets the CID register value of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port  to get the CID register value.
 * @param[out] cid is the CID value of the SD/eMMC card.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_cid(hal_sd_port_t sd_port, uint32_t *cid);


/**
 * @brief     This function gets the card type of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to get the card type.
 * @param[out] card_type is the current card type of the SD/eMMC card.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_card_type(hal_sd_port_t sd_port, hal_sd_card_type_t *card_type);


/**
 * @brief     This function gets the OCR register value of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to get the OCR register value.
 * @param[out] ocr is the OCR value of the SD/eMMC card.
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_ocr(hal_sd_port_t sd_port, uint32_t *ocr);


/**
 * @brief     This function gets the card status of the SD/eMMC card.
 * @param[in] sd_port is the MSDC port to get the card status.
 * @param[out] card_status is a pointer to the card status that is read from the card status register.For an introduction to the card status register, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/part1_200.pdf">SD Memory Card Specification Version 2.0 </a>
 * @return    #HAL_SD_STATUS_OK, if the operation completed successfully.
 *            #HAL_SD_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sd_status_t hal_sd_get_card_status(hal_sd_port_t sd_port, uint32_t *card_status);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_SD_MODULE_ENABLED*/
#endif /* __HAL_SD_H__ */


