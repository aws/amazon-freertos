/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __HAL_SPI_MASTER_H__
#define __HAL_SPI_MASTER_H__
#include "hal_platform.h"

#ifdef HAL_SPI_MASTER_MODULE_ENABLED


/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * This section introduces the Serial Peripheral Interface Master (SPI_Master) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_SPI_MASTER_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DMA                        | Direct Memory Access. DMA is a feature of computer systems that allows certain hardware subsystems to access main system memory independent from the central processing unit (CPU).|
 * |\b FIFO                       | First In, First Out. FIFO is a method for organizing and manipulating a data buffer, where the first entry, or 'head' of the queue, is processed first.|
 * |\b GPIO                       | General Purpose Inputs-Outputs. For more details, please refer to @ref GPIO.|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M series processors. For more details, please refer to <a href="http://infocenter.arm.com/help/topic/com.arm.doc.100166_0001_00_en/arm_cortexm4_processor_trm_100166_0001_00_en.pdf"> ARM Cortex-M4 technical reference manual</a>.|
 * |\b SPI                        | Serial Peripheral Interface. The Serial Peripheral Interface bus is a synchronous serial communication interface specification used for short distance communication. For more information, please check <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 *
 * @section HAL_SPI_MASTER_Features_Chapter Supported features
 *
 * @}
 * @}
 */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* This controller supports a wide range of SPI slave devices with both half-duplex and full-duplex transaction.
* For half-duplex transaction, the controller can send data sequence to SPI slave, then receive data sequence from the SPI slave, asynchronously.
* For full-duplex transaction, the controller can send and receive data sequence from the SPI slave at the same time.
*
* - \b Supported \b transaction \b formats. \n
*  - Half-duplex transaction:
*    There is only one valid transaction at a time on a single direction; either send or receive. The functions using half-duplex transaction are:
*   - #hal_spi_master_send_polling().
*   - #hal_spi_master_send_dma().
*
*  - Full-duplex transaction:
*    There are two valid mutually inclusive transactions; send and receive. Functions using full-duplex transaction are:
*   - #hal_spi_master_send_and_receive_polling().
*   - #hal_spi_master_send_and_receive_dma().
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* This controller supports a wide range of SPI slave devices with both directional transaction or half-duplex transaction.
* For half-duplex transaction, the controller can send data sequence to SPI slave, then receive data sequence from the SPI slave, asynchronously.
* For both directional transaction, the controller can send and receive data sequence from the SPI slave at the same time only in data phase.
*
* - \b Supported \b transaction \b formats. \n
*  - Half-duplex transaction:
*    There is only one valid transaction at a time on a single direction; either send or receive. The function using half-duplex transaction is:
*   - #hal_spi_master_send_polling().
*   - #hal_spi_master_send_and_receive_polling().
*
*  - Both directional transaction:
*    There are two valid mutually inclusive transactions; send and receive. The function using both directional transaction is:
*   - #hal_spi_master_send_and_receive_polling_both_directional().
*
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b Supported \b transaction \b mode. \n
*  - Polling mode transaction:
*    In polling mode, #hal_spi_master_send_polling() and #hal_spi_master_send_and_receive_polling() return a value once the transaction is complete.
*    Then the user checks the output of the return value for the transaction result and error handling, if any.
*
*    See @ref HAL_SPI_MASTER_Architecture_Chapter for the software architecture of polling mode.
*
* @}
* @}
*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
*  - DMA mode transaction:
*    In DMA mode, #hal_spi_master_send_dma() and #hal_spi_master_send_and_receive_dma() return a value once the SPI hardware register is assigned and the DMA configured.
*    During this process the transaction is usually incomplete, once it completes, an interrupt is triggered and a related user callback
*    is called in the SPI interrupt service routine.
*
*    See @ref HAL_SPI_MASTER_Architecture_Chapter for the software architecture of DMA mode.
* @}
* @}
*/
#endif
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Architecture_Chapter Software architecture of the SPI
* - \b Polling \b mode \b architecture. \n
*   Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
* @}
* @}
*/
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* - \b DMA \b mode \b architecture. \n
*   DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
* @}
* @}
*/
#endif
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Driver_Usage_Chapter How to use this driver
*
* - \b Using \b SPI \b master \b in \b polling \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.\n
*  Once the transaction is complete, call #hal_spi_master_deinit() function to release the SPI master resource.\n
*  The steps are shown below:
*  - Step 1. Call #hal_gpio_init() to initialize the pins, if the EPT tool hasn't been used to configure the related pinmux.
*  - Step 2. Call #hal_pinmux_set_function() to set the GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
*  - Step 3. Call #hal_spi_master_init() to initialize the SPI master. If the SPI master is already initialized by another user, user will get #HAL_SPI_MASTER_STATUS_ERROR_BUSY.
*  - Step 4. Call #hal_spi_master_send_polling() to send data in the polling mode.
*  - Step 5. Call #hal_spi_master_send_and_receive_polling() to send and receive data in the polling mode.
*  - Step 6. Call #hal_spi_master_deinit() to deinitialize the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_spi_master_config_t spi_config;
*       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
*       uint8_t status_cmd = SPIS_STATUS_CMD;
*       uint8_t status_receive[2];
*       uint8_t data[2] = {0x7E, 0x55};
*       uint32_t size = 2;
*       spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
*       spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
*       spi_config.clock_frequency = 1000000;
*       spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
*       spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
*       status_receive[1] = 0;
*       spi_send_and_receive_config.receive_length = 2;
*       spi_send_and_receive_config.send_length = 1;
*       spi_send_and_receive_config.send_data = &status_cmd;
*       spi_send_and_receive_config.receive_buffer = status_receive;
*
*       // Initialize the GPIO, set GPIO pinmux (if EPT tool hasn't been used to configure the related pinmux).
*       hal_gpio_init(HAL_GPIO_25);
*       hal_gpio_init(HAL_GPIO_26);
*       hal_gpio_init(HAL_GPIO_27);
*       hal_gpio_init(HAL_GPIO_28);
*       // Call hal_pinmux_set_function() to set the GPIO pinmux, for more information, please refer to hal_pinmux_define.h.
*       // No need to configure the pinmux, if EPT is used.
*       hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_MA_SPI3_A_CS);// Set the clock select (CS) pin.
*       hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MA_SPI3_A_SCK);// Set the serial clock (SCK) pin.
*       hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MA_SPI3_A_MOSI);// Set the master out, slave in (MOSI) pin.
*       hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MA_SPI3_A_MISO);// Set the master in, slave out (MISO) pin.
*       // Initialize the SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == hal_spi_master_init(HAL_SPI_MASTER_3,&spi_config)) {
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_3,data,size)) {
*               // Error handler;
*           }
*           // Send and receive data at the same time.
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_3,&spi_send_and_receive_config)) {
*               // Error handler;
*           }
*           hal_spi_master_deinit(HAL_SPI_MASTER_3); //Must call deinit, if the SPI master is no longer in use.
*       } else {
*           // Error handler;
*       }
*    @endcode
*
* - \b Using \b SPI \b master \b in \b DMA \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.\n
*  Then call #hal_spi_master_register_callback() to register a callback function.\n
*  Once a transaction is complete, call #hal_spi_master_deinit() function \b in \b your \b callback \b function to release the SPI master resource.\n
*  The steps are shown below:
*  - Step 1. Call #hal_gpio_init() to inite the pins, if EPT tool hasn't been used to configure the related pinmux.
*  - Step 2. Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
*  - Step 3. Call #hal_spi_master_init() to init one SPI master. If the SPI master is already initialized by another user, user will get #HAL_SPI_MASTER_STATUS_ERROR_BUSY.
*  - Step 4. Call #hal_spi_master_register_callback() to register a user callback.
*  - Step 5. Call #hal_spi_master_send_dma() to send data in the DMA mode.
*  - Step 6. Call #hal_spi_master_send_and_receive_dma() to send and receive data in the DMA mode.
*  - Step 7. Call #hal_spi_master_deinit() to deinit the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_spi_master_config_t spi_config;
*       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
*       uint8_t status_cmd = SPIS_STATUS_CMD;
*       uint8_t status_receive[2];
*       spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
*       spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
*       spi_config.clock_frequency = 1000000;
*       spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
*       spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
*       status_receive[1] = 0;
*       spi_send_and_receive_config.receive_length = 2;
*       spi_send_and_receive_config.send_length = 1;
*       spi_send_and_receive_config.send_data = &status_cmd;
*       spi_send_and_receive_config.receive_buffer = status_receive;
*
*       // Initialize the GPIO, set GPIO pinmux (if EPT tool hasn't been used to configure the related pinmux).
*       hal_gpio_init(HAL_GPIO_25);
*       hal_gpio_init(HAL_GPIO_26);
*       hal_gpio_init(HAL_GPIO_27);
*       hal_gpio_init(HAL_GPIO_28);
*       // Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please refer to hal_pinmux_define.h
*       // No need to configure the pinmux if EPT is used.
*       hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_MA_SPI3_A_CS);// Set the clock select (CS) pin.
*       hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_MA_SPI3_A_SCK);// Set the serial clock (SCK) pin.
*       hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_MA_SPI3_A_MOSI);// Set the master out, slave in (MOSI) pin.
*       hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_MA_SPI3_A_MISO);// Set the master in, slave out (MISO) pin.
*       // Initialize the SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == hal_spi_master_init(HAL_SPI_MASTER_3 ,&spi_config)) {
*           hal_spi_master_register_callback(HAL_SPI_MASTER_3 ,user_spi_callback,NULL)// Register a user callback.
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(HAL_SPI_MASTER_3 ,data,size)) {
*               // Error handler;
*           }
*           // Send and receive data at the same time.
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_dma(HAL_SPI_MASTER_3, &spi_send_and_receive_config)) {
*               // Error handler;
*           }
*       } else {
*           // Error handler;
*       }
*    @endcode
*    @code
*       // Callback function sample code. Pass this function to driver, while calling #hal_spi_master_register_callback().
*       void user_spi_callback (hal_spi_master_callback_event_t event,void *user_data)
*       {
*           if (HAL_SPI_MASTER_EVENT_SEND_FINISHED == event) {
*               // Send finish event handler;
*               // User code;
*               hal_spi_master_deinit(HAL_SPI_MASTER_3); // Must call deinit when you don't use it.
*           } else if (HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED == event) {
*               // Receive finish event handler;
*               // User code;
*               hal_spi_master_deinit(HAL_SPI_MASTER_3); // Must call deinit when you don't use it.
*           }
*       }
*    @endcode
* @}
* @}
*/
#else
/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
* @section HAL_SPI_MASTER_Driver_Usage_Chapter How to use this driver
*
* - \b Using \b SPI \b master \b in \b polling \b mode. \n
*  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (CS_N, SCK, MOSI, and MISO) based on the user's hardware platform design.\n
*  Once a transaction is complete, call #hal_spi_master_deinit() function to release the SPI master resource.\n
*  The steps are shown below:
*  - Step 1. Call #hal_gpio_init() to inite the pins, if EPT tool hasn't been used to configure the related pinmux.
*  - Step 2. Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
*           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO. Note, user
*           should configure the chip select pin as GPIO mode, this driver will set the chip select signal as valid before starting the transaction,
*           then set the chip select signal as invalid after finishing the transaction.
*  - Step 3. Call #hal_spi_master_init() to initialize one SPI master.
*  - Step 4. Call #hal_spi_master_send_polling() to send data in the polling mode.
*  - Step 5. Call #hal_spi_master_send_and_receive_polling() to send and receive data in the polling mode.
*  - Step 6. Call #hal_spi_master_deinit() to deinitialize the SPI master, if it's no longer in use.
*  - sample code:
*    @code
*       hal_spi_master_config_t spi_config;
*       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
*       uint8_t status_cmd = SPIS_STATUS_CMD;
*       uint8_t status_receive[2];
*       uint8_t send_data[2] = {0x7E, 0x55};
*       uint32_t size = 2;
*       spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
*       spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
*       spi_config.clock_frequency = 1000000;
*       spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
*       spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
*       status_receive[1] = 0;
*       spi_send_and_receive_config.receive_length = 2;
*       spi_send_and_receive_config.send_length = 1;
*       spi_send_and_receive_config.send_data = &status_cmd;
*       spi_send_and_receive_config.receive_buffer = status_receive;
*
*       // Initialize the GPIO, set GPIO pinmux (if EPT tool hasn't been used to configure the related pinmux).
*       hal_gpio_init(HAL_GPIO_29);
*       hal_gpio_init(HAL_GPIO_30);
*       hal_gpio_init(HAL_GPIO_31);
*       hal_gpio_init(HAL_GPIO_32);
*       // Chip select pin should be configured as GPIO mode. No need to configure pinmux if EPT is used.
*       hal_pinmux_set_function(HAL_GPIO_29, HAL_GPIO_29_SPI_MOSI_M_CM4);// Set the master out, slave in (MOSI) pin.
*       hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_SPI_MISO_M_CM4);// Set the master in, slave out (MISO) pin.
*       hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_SPI_SCK_M_CM4);// Set the serial clock (SCK) pin.
*       hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_GPIO32);
*
*       // Initialize the SPI master.
*       if (HAL_SPI_MASTER_STATUS_OK == hal_spi_master_init(HAL_SPI_MASTER_0,&spi_config)) {
*           // Send data;
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(HAL_SPI_MASTER_0,data,size)) {
*               // Error handler;
*           }
*           // Send and receive data at the same time.
*           if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_0,&spi_send_and_receive_config)) {
*               // Error handler;
*           }
*           hal_spi_master_deinit(HAL_SPI_MASTER_0); //Must call deinit when don't use SPI master
*       } else {
*           // Error handler;
*       }
*    @endcode
* @}
* @}
*/
#endif

/**
* @addtogroup HAL
* @{
* @addtogroup SPI_MASTER
* @{
*/

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_spi_master_enum Enum
  * @{
  */

/** @brief SPI master clock polarity. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_POLARITY0 = 0,                     /**< Clock polarity is 0. */
    HAL_SPI_MASTER_CLOCK_POLARITY1 = 1                      /**< Clock polarity is 1. */
} hal_spi_master_clock_polarity_t;


/** @brief SPI master clock format. */
typedef enum {
    HAL_SPI_MASTER_CLOCK_PHASE0 = 0,                         /**< Clock format is 0. */
    HAL_SPI_MASTER_CLOCK_PHASE1 = 1                          /**< Clock format is 1. */
} hal_spi_master_clock_phase_t;


/** @brief SPI master transaction bit order. */
typedef enum {
    HAL_SPI_MASTER_LSB_FIRST = 0,                       /**< Send and receive operations use LSB first. */
    HAL_SPI_MASTER_MSB_FIRST = 1                        /**< Send and receive operations use MSB first. */
} hal_spi_master_bit_order_t;


#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/** @brief SPI master data transfer byte order.
 *         Before calling #hal_spi_master_send_polling() or #hal_spi_master_send_and_receive_polling(), ensure the \b byte_order
 *         parameter defined in the #hal_spi_master_advanced_config_t structure and configured by #hal_spi_master_set_advanced_config()
 *         is #HAL_SPI_MASTER_LITTLE_ENDIAN, because setting it as #HAL_SPI_MASTER_BIG_ENDIAN has no effect in send direction, but has effect in receive direction.
 */
typedef enum {
    HAL_SPI_MASTER_LITTLE_ENDIAN = 0,                       /**< Send and receive operations use little-endian format. */
    HAL_SPI_MASTER_BIG_ENDIAN    = 1                        /**< Send and receive operations use big-endian format. */
} hal_spi_master_byte_order_t;


/** @brief SPI master chip select polarity. */
typedef enum {
    HAL_SPI_MASTER_CHIP_SELECT_LOW  = 0,                /**< Chip select polarity is active low. */
    HAL_SPI_MASTER_CHIP_SELECT_HIGH = 1                 /**< Chip select polarity is active high. */
} hal_spi_master_chip_select_polarity_t;


/** @brief SPI master tolerance tick delay timing (based on SPI system clock) setting. */
typedef enum {
    HAL_SPI_MASTER_NO_GET_TICK_MODE = 0,              /**< Don't use timing tolerance ability. */
    HAL_SPI_MASTER_GET_TICK_DELAY1  = 1,              /**< Delay 1 SPI system clock cycle for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY2  = 2,              /**< Delay 2 SPI system clock cycles for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY3  = 3,              /**< Delay 3 SPI system clock cycles for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY4  = 4,              /**< Delay 4 SPI system clock cycles for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY5  = 5,              /**< Delay 5 SPI system clock cycles for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY6  = 6,              /**< Delay 6 SPI system clock cycles for tick delay. */
    HAL_SPI_MASTER_GET_TICK_DELAY7  = 7               /**< Delay 7 SPI system clock cycles for tick delay. */
} hal_spi_master_get_tick_mode_t;


/** @brief SPI master sample edge of MISO. */
typedef enum {
    HAL_SPI_MASTER_SAMPLE_POSITIVE = 0,               /**< Sample edge of MISO is positive. */
    HAL_SPI_MASTER_SAMPLE_NEGATIVE = 1                /**< Sample edge of MISO is negative. */
} hal_spi_master_sample_select_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
/** @brief SPI master chip select deassert. */
typedef enum {
    HAL_SPI_MASTER_DEASSERT_DISABLE = 0,              /**< Chip select deassert is disabled. */
    HAL_SPI_MASTER_DEASSERT_ENABLE  = 1               /**< Chip select deassert is enabled. */
} hal_spi_master_deassert_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
/** @brief SPI master pad macro select. */
typedef enum {
    HAL_SPI_MASTER_MACRO_GROUP_A = 0,                   /**< SPI uses PAD group A. */
    HAL_SPI_MASTER_MACRO_GROUP_B = 1,                   /**< SPI uses PAD group B. */
    HAL_SPI_MASTER_MACRO_GROUP_C = 2                    /**< SPI uses PAD group C. */
} hal_spi_master_macro_select_t;
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @brief SPI master callback event.*/
typedef enum {
    HAL_SPI_MASTER_EVENT_SEND_FINISHED      = 0,             /**< SPI master send is complete. */
    HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED   = 1              /**< SPI master receive is complete. */
} hal_spi_master_callback_event_t;
#endif

/** @brief SPI master status. */
typedef enum {
    HAL_SPI_MASTER_STATUS_ERROR             = -4,            /**< An error occurred during SPI master function call. */
    HAL_SPI_MASTER_STATUS_ERROR_BUSY        = -3,            /**< The SPI master is busy. */
    HAL_SPI_MASTER_STATUS_ERROR_PORT        = -2,            /**< Invalid port for the SPI master. */
    HAL_SPI_MASTER_STATUS_INVALID_PARAMETER = -1,            /**< Invalid parameter for the SPI master. */
    HAL_SPI_MASTER_STATUS_OK                = 0              /**< The SPI master operation completed successfully. */
} hal_spi_master_status_t;


/** @brief SPI master execution status. */
typedef enum {
    HAL_SPI_MASTER_BUSY              = 0,             /**< The SPI master is busy. */
    HAL_SPI_MASTER_IDLE              = 1              /**< The SPI master is idle. */
} hal_spi_master_running_status_t;

/**
  * @}
  */

/** @defgroup hal_spi_master_struct Struct
  * @{
  */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @brief SPI master configuration. */
typedef struct {
    uint32_t clock_frequency;                                                /**< SPI master clock frequency setting, the range is from 30000Hz to 13000000Hz. Normally user can set the clock frequency to 13000000Hz. */
    hal_spi_master_slave_port_t slave_port;                  /**< SPI slave device selection. */
    hal_spi_master_bit_order_t bit_order;                    /**< SPI master bit order setting. */
    hal_spi_master_clock_polarity_t polarity;                /**< SPI master clock polarity setting. */
    hal_spi_master_clock_phase_t phase;                      /**< SPI master clock phase setting. */
} hal_spi_master_config_t;
#else
/** @brief SPI master configuration. */
typedef struct {
    uint32_t clock_frequency;                                               /**< SPI master clock frequency setting, the range is from 30000Hz to 12000000Hz. Normally user can set the clock frequency to 12000000Hz. If the hardware environment is not suitable for the highest frequency setting.  */
    hal_spi_master_slave_port_t slave_port;                  /**< SPI slave device selection. */
    hal_spi_master_bit_order_t bit_order;                    /**< SPI master bit order setting. */
    hal_spi_master_clock_polarity_t polarity;                /**< SPI master clock polarity setting. */
    hal_spi_master_clock_phase_t phase;                      /**< SPI master clock phase setting. */
} hal_spi_master_config_t;
#endif

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/** @brief SPI master advanced configuration. */
typedef struct {
    hal_spi_master_byte_order_t byte_order;                     /**< SPI master byte order setting. */
    hal_spi_master_chip_select_polarity_t chip_polarity;        /**< SPI master chip select active polarity setting. */
    hal_spi_master_get_tick_mode_t get_tick;                    /**< SPI clock timing tolerance setting. User may need to change this setting when high frequency is used. */
    hal_spi_master_sample_select_t sample_select;               /**< SPI MISO sample edge setting. */
} hal_spi_master_advanced_config_t;
#endif


#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
/** @brief SPI master chip select configuration. */
typedef struct {
    uint32_t chip_select_setup_count;                 /**< SPI master chip select setup count setting. Unit in count of SPI
                                                           base clock. Range(0~32767). The chip select setup time is
                                                           (cs_setup_count+1)*CLK_PERIOD, where CLK_PERIOD is the cycle time
                                                           of the clock the SPI engine adopts. */
    uint32_t chip_select_hold_count;                  /**< SPI master chip select hold count setting. Unit in count of SPI
                                                           base clock. Range(0~32767). The chip select hold time is
                                                           (cs_hold_count+1)*CLK_PERIOD. */
    uint32_t chip_select_idle_count;                  /**< SPI master chip select idle count setting. Unit in count of SPI
                                                           base clock. Range(0~255). The chip select idle time between consecutive
                                                           transaction is (cs_idle_count+1)*CLK_PERIOD. */
} hal_spi_master_chip_select_timing_t;
#endif


/** @brief SPI master send and receive configuration. */
typedef struct {
    uint8_t *send_data;                               /**< A pointer to send data, this parameter cannot be NULL. */
    uint32_t send_length;                             /**< The number of bytes to send, no more than 4. */
    uint8_t *receive_buffer;                          /**< A pointer to the received data, this parameter cannot be NULL. */
    uint32_t receive_length;                          /**< The total number of send and received bytes. */
} hal_spi_master_send_and_receive_config_t;


/**
  * @}
  */
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/** @defgroup hal_spi_master_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype.
 *          User should register a callback function when using the SPI in DMA mode, this function will be called in the SPI interrupt
 *          service routine after a transaction is complete. More details about the callback architecture, please refer to
 *          @ref HAL_SPI_MASTER_Architecture_Chapter.
 *  @param [in] event is the transaction event for the current transaction, user can get the transaction result from this parameter, please
 *              refer to #hal_spi_master_callback_event_t for details about the event type.
 *  @param [in] user_data is the parameter which is given by user via #hal_spi_master_register_callback().
 *  @sa  #hal_spi_master_register_callback()
 */
typedef void (*hal_spi_master_callback_t)(hal_spi_master_callback_event_t event, void *user_data);

/**
  * @}
  */
#endif

/**
 * @brief     This function is mainly used to initialize the SPI master and set user defined common parameters, including clock frequency,
 *            bit order, clock polarity, clock phase and default settings.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_config are the SPI master configuration parameters. Details are described in #hal_spi_master_config_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR, an unclassified error is detected. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *       hal_spi_master_config_t spi_config;
 *       spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
 *       spi_config.clock_frequency = 0x400000;
 *       spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
 *       spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
 *       ret = hal_spi_master_init(HAL_SPI_MASTER_1, &spi_config);
 * @endcode
 * @sa #hal_spi_master_deinit()
 */
hal_spi_master_status_t hal_spi_master_init(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config);

/**
 * @brief     This function resets the SPI master, gates its clock, disables interrupts.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_init()
 */
hal_spi_master_status_t hal_spi_master_deinit(hal_spi_master_port_t master_port);

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
/**
 * @brief     SPI master advanced configuration function.
 *            User can call this function to customize more settings for the SPI device operation.
 *            For more information about the settings, please refer to #hal_spi_master_advanced_config_t.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] advanced_config provides the advanced configuration parameters for the SPI master. Details are described in #hal_spi_master_advanced_config_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @note      This function must be called after #hal_spi_master_init().
 * @par       Example
 * @code
 *       hal_spi_master_advanced_config_t advanced_config;
 *       advanced_config.byte_order = HAL_SPI_MASTER_BIG_ENDIAN;
 *       advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
 *       advanced_config.get_tick = HAL_SPI_MASTER_GET_TICK_DELAY1;
 *       advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_NEGATIVE;
 *       ret = hal_spi_master_set_advanced_config(HAL_SPI_MASTER_1, &advanced_config);
 * @endcode
 */
hal_spi_master_status_t hal_spi_master_set_advanced_config(hal_spi_master_port_t master_port,
        hal_spi_master_advanced_config_t *advanced_config);
#endif


/**
 * @brief     This function is used to send data synchronously with FIFO mode. This function doesn't return until the transfer is complete.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] data is the buffer of data to be sent, this parameter cannot be NULL.
 * @param[in] size is the number of bytes to send. Note user can't send data size larger than #HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE bytes.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR, an unclassified error is detected. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *       ret = hal_spi_master_send_polling(HAL_SPI_MASTER_1,data,size);
 * @endcode
 * @sa #hal_spi_master_send_and_receive_polling()
 */
hal_spi_master_status_t hal_spi_master_send_polling(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size);

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function is used to send data asynchronously with DMA mode. This function returns immediately, before calling this function,
 *            call #hal_spi_master_register_callback() to register a callback, then wait for the transaction to complete and call the callback in the
 *            SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] data is a pointer to the send data, this parameter cannot be NULL, also the address must be non-cacheable.
 * @param[in] size is the number of bytes to send.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_and_receive_dma()
 */
hal_spi_master_status_t hal_spi_master_send_dma(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size);
#endif

/**
 * @brief     This function supports SPI send and receive simultaneous transaction in FIFO mode. This function doesn't return until the transfer is complete.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_send_and_receive_config is a structure with data buffer and data size, please refer to #hal_spi_master_send_and_receive_config_t, 
 *            for more details. User needs to determine if the received data is valid based on the SPI device's datasheet.
 *            For example, if <a href="http://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16375.pdf"> ADIS16375 </a>
 *            is used in both directional communication mode, the data is received correctly while sending command is valid.
 *            <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf"> PAH8001EI-2G </a> sensor is used, the data is received incorrectly while
 *            sending command is invalid.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR, an unclassified error is detected. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *       spi_send_and_receive_config.receive_buffer = receive_buffer;
 *       spi_send_and_receive_config.send_data = send_data;
 *       spi_send_and_receive_config.send_length = send_length;
 *       spi_send_and_receive_config.receive_length = receive_length;
 *       ret = hal_spi_master_send_and_receive_polling(HAL_SPI_MASTER_1, &spi_send_and_receive_config);
 * @endcode
 * @sa  #hal_spi_master_send_polling()
 */
hal_spi_master_status_t hal_spi_master_send_and_receive_polling(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config);

/**
 * @brief     This function supports SPI send and receive simultaneously transcation using FIFO in both directional data mode; command stage is in half-duplex mode and data stage is in full-duplex mode. This function doesn't return until the transfer is complete.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_send_and_receive_config is a structure with data buffer and data size. For more details, please refer to #hal_spi_master_send_and_receive_config_t.
 *            In current implementation, the SPI master can only receive 12 bytes out of each 16 byte transfer.
 *            receive_length defines the total number of clock cycles to receive data. For example, if the receive_length is 64,
 *            the actual number of bytes received from the SPI slave is only 48 bytes. The implementation on the slave side should also consider adding 4 dummy bytes before every 12 bytes to prepare the package data. 
 * @return    #HAL_SPI_MASTER_STATUS_ERROR, an unclassified error is detected. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *       hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *       spi_send_and_receive_config.receive_buffer = receive_buffer;
 *       spi_send_and_receive_config.send_data = send_data;
 *       spi_send_and_receive_config.send_length = send_length;
 *       spi_send_and_receive_config.receive_length = receive_length;
 *       ret = hal_spi_master_send_and_receive_polling_both_directional(HAL_SPI_MASTER_1, &spi_send_and_receive_config);
 * @endcode
 * @sa  #hal_spi_master_send_polling()
 */
hal_spi_master_status_t hal_spi_master_send_and_receive_polling_both_directional(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config);

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function supports send and receive asynchronously with DMA mode. This function returns immediately, before calling this function,
 *            user should call #hal_spi_master_register_callback() to register a callback, then wait for the transaction to complete and call the callback in the
 *            SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] spi_send_and_receive_config is a structure with data buffer and data size. For more details, please refer to #hal_spi_master_send_and_receive_config_t.
 *            But user should judge whether the data received while sending command is valid or not, and this information can
 *            get from SPI device's datasheet. For example, if user use <a href="http://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16375.pdf"> ADIS16375 </a>
 *            as full-duplex communication mode, the data is received correctly while sending command is valid. If user use
 *            <a href="http://www.thaieasyelec.com/downloads/EFDV423/PAH8001EI-2G.pdf"> PAH8001EI-2G </a> sensor, the data is received incorrectly while
 *            sending command is invalid. The address parameters cannot be NULL, also the address must be non-cacheable.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_dma()
 */
hal_spi_master_status_t hal_spi_master_send_and_receive_dma(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config);
#endif

/**
 * @brief      This function gets current running status of the SPI master. Note, this API can only be called after #hal_spi_master_init().
 * @param[in]  master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[out] running_status is the current running status.
 *             #HAL_SPI_MASTER_BUSY, the SPI master is busy. \n
 *             #HAL_SPI_MASTER_IDLE, the SPI master is in idle state, unable to transfer data.
 * @return     #HAL_SPI_MASTER_STATUS_ERROR, an unclassified error is detected. \n
 *             #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *             #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *             #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par        Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_get_running_status(hal_spi_master_port_t master_port,
        hal_spi_master_running_status_t *running_status);


#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
/**
 * @brief     This function is used to configure SPI master chip select timing parameter.
 *            Call this function to customize chip select signal timing
 *            if the default SPI master chip select signal timing doesn't match SPI device's requirement.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] chip_select_timing is the parameter settings for chip select timing.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @note      This function must be called after #hal_spi_master_init().
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_chip_select_timing(hal_spi_master_port_t master_port,
        hal_spi_master_chip_select_timing_t chip_select_timing);
#endif


#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
/**
 * @brief     SPI master chip select de-assertion mode configuration.
 *            Call this function to enable the deassert feature if the SPI device
 *            requires switching the chip select signal from invalid to valid after each byte transfer.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] deassert is the parameter to set SPI master chip select signal behavior after sending one byte.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @note      This function must be called after #hal_spi_master_init().
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_deassert(hal_spi_master_port_t master_port,
        hal_spi_master_deassert_t deassert);
#endif


#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
/**
 * @brief     SPI master macro group configuration. If the GPIO selected doesn't belong to SPI pad macro group A,
              call this function to configure it to with the right pad macro.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] macro_select is the parameter for macro group selection.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_ERROR_BUSY, the SPI master port is busy and not available for use. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @note      This function must be called after #hal_spi_master_init().
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 */
hal_spi_master_status_t hal_spi_master_set_macro_selection(hal_spi_master_port_t master_port,
        hal_spi_master_macro_select_t macro_select);
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
/**
 * @brief     This function is used to register user's callback to SPI master driver. Call this function for DMA mode only,
 *            the callback will be called in SPI interrupt service routine.
 * @param[in] master_port is the SPI master port number, the value is defined in #hal_spi_master_port_t.
 * @param[in] callback is a user-defined callback function, which is called at SPI master interrupt service routine.
 * @param[in] user_data is a user-defined parameter and will pass to user while the callback function is called. Please refer to the last parameter of #hal_spi_master_callback_t.
 * @return    #HAL_SPI_MASTER_STATUS_ERROR_PORT, the master_port parameter is invalid. \n
 *            #HAL_SPI_MASTER_STATUS_INVALID_PARAMETER, an invalid input parameter is given by user. \n
 *            #HAL_SPI_MASTER_STATUS_OK, the operation completed successfully.
 * @par       Example
 * Sample code, please refer to @ref HAL_SPI_MASTER_Driver_Usage_Chapter.
 * @sa #hal_spi_master_send_dma(), #hal_spi_master_send_and_receive_dma()
 */
hal_spi_master_status_t hal_spi_master_register_callback(hal_spi_master_port_t master_port,
        hal_spi_master_callback_t callback,
        void *user_data);
#endif

#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_SPI_MASTER_MODULE_ENABLED*/
#endif /* __HAL_SPI_MASTER_H__ */

