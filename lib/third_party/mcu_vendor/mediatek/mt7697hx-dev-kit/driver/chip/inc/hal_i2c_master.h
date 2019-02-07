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
#ifndef __HAL_I2C_MASTER_H__
#define __HAL_I2C_MASTER_H__
#include "hal_platform.h"

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 *
 * @section HAL_I2C_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DMA                        | Direct Memory Access. For an introduction to DMA, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> DMA in Wikipedia</a>.|
 * |\b GPIO                       | General Purpose Inputs-Outputs. For more details, please refer to the @ref GPIO module in HAL. |
 * |\b I2C                        | Inter-Integrated Circuit. I2C is typically used to attach low-speed peripheral ICs to processors and microcontrollers. For an introduction to I2C, please refer to <a href="https://en.wikipedia.org/wiki/I%C2%B2C"> I2C in Wikipedia</a>.|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual</a>.|
 *
 * @section HAL_I2C_Features_Chapter Supported features
 * - \b Supported \b transaction \b modes
 *  - \b Polling \b mode \b transaction
 *    In polling mode, the functions with the suffix "_polling" will not return any value until the transaction is complete.
 *    After the function returns, the return value should be checked and the error status should be handled.
 *
 *  - \b DMA \b mode \b transaction
 *    In DMA mode, the functions with the suffix "_dma" return a value once the I2C and DMA hardware registers are assigned.
 *    These functions do not wait for the transaction to finish. After the transaction finishes, an interrupt is triggered and the user callback in the I2C ISR
 *    routine is called. After the function returns, the return value should be checked and the error status should be handled.
 *  - \b Extended \b DMA \b mode \b transaction
 *    In extended DMA mode, the functions with the suffix "_dma_ex" return a value once the I2C and DMA hardware registers are assigned.
 *    Compared to the DMA mode transaction, Extended DMA mode has more flexibility to configure the waveform
 *    pattern sent out by the I2C master.
 *
 *  Note:  \n
 *       1. For more infomation about the transaction length and waveform pattern for each API, please refer to @ref HAL_I2C_Transaction_Pattern_Chapter. \n
 *       2. It is required to use the pull-up resistors on the SCL and SDA of the I2C interface. Please refer to related HDK user guide for more details.
 *
 * @section HAL_I2C_Architecture_Chapter Software architecture of the I2C
 * - \b Polling \b mode \b architecture. \n
 *   Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
 * - \b DMA \b mode \b architecture. \n
 *   DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
 *
 *
 * @section HAL_I2C_Driver_Usage_Chapter How to use this driver
 * - \b Using \b I2C \b in \b polling \b mode.  \n
 *  To use I2C driver in a polling mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function() to select the pinmux function. After setting the pinmux, call #hal_i2c_master_get_running_status() to check the I2C
 *  status. If the status is #HAL_I2C_STATUS_BUS_BUSY, wait till the status becomes #HAL_I2C_STATUS_IDLE. Once the data is successfully
 *  transmitted, call #hal_i2c_master_deinit() to release the I2C resource for other users.\n
 *  Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_i2c_master_init() to initialize the I2C master.
 *  - Step4: Call #hal_i2c_master_send_polling() to send data in a polling mode.
 *  - Step5: Call #hal_i2c_master_receive_polling() to receive data in a polling mode.
 *  - Step6: Call #hal_i2c_master_deinit() to de-allocate the I2C master if it is no longer in use.
 *  - Sample code:
 *    @code
 *       hal_i2c_config_t i2c_config;
 *       uint8_t slave_address = 0X50;
 *       const uint8_t send_data[8] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19}; //the data that used to send
 *       uint8_t receive_data[8] = {0};
 *       volatile uint32_t size = 8;
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_27);
 *       hal_gpio_init(HAL_GPIO_28);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_27_I2C1_CLK
 *       hal_pinmux_set_function(HAL_GPIO_27, function_index);
 *       //function_index = HAL_GPIO_28_I2C1_DATA
 *       hal_pinmux_set_function(HAL_GPIO_28, function_index);
 *
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_config)) {
 *           //Send data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_send_polling(HAL_I2C_MASTER_0, slave_address, send_data, size)) {
 *               //Error handler;
 *           }
 *           //Receive data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_polling(HAL_I2C_MASTER_0, slave_address, receive_data, size)) {
 *               //Error handler;
 *           }
 *           hal_i2c_master_deinit(HAL_I2C_MASTER_0); //Call this function if the I2C is no longer in use.
 *
 *       } else {
 *           //Error handler;
 *       }
 *
 *    @endcode
 *
 * - \b Using \b I2C \b in \b DMA \b mode. \n
 *  To use I2C driver in DMA mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function to select the pinmux function. After setting the pinmux, call #hal_i2c_master_get_running_status() to check the I2C
 *  status. If the status is #HAL_I2C_STATUS_BUS_BUSY, user should wait till the status is #HAL_I2C_STATUS_IDLE, then
 *  call #hal_i2c_master_register_callback() to register a callback function. Once the data transaction is complete,
 *  call #hal_i2c_master_deinit() in your callback function registered by #hal_i2c_master_register_callback() to release the I2C resource to other users. Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_i2c_master_init() to initialize the I2C master.
 *  - Step4: Call #hal_i2c_master_register_callback() to register a user callback.
 *  - Step5: Call #hal_i2c_master_send_dma() to send data within a DMA mode.
 *  - Step6: Call #hal_i2c_master_receive_dma() to receive data in a DMA mode.
 *  - Step7: Call #hal_i2c_master_deinit() to de-initialize the I2C master if it is no longer in use.
 *  - Sample code:
 *    @code
 *       hal_i2c_config_t i2c_config;
 *       uint8_t slave_address = 0X50;
 *       const uint8_t send_data[8] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19}; //the data that used to send
 *       uint8_t receive_data[8] = {0};
 *       volatile uint32_t size = 8;
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_27);
 *       hal_gpio_init(HAL_GPIO_28);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_27_I2C1_CLK
 *       hal_pinmux_set_function(HAL_GPIO_27, function_index);
 *       //function_index = HAL_GPIO_28_I2C1_DATA
 *       hal_pinmux_set_function(HAL_GPIO_28, function_index);
 *
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_config)) {
 *           hal_i2c_master_register_callback(HAL_I2C_MASTER_0, i2c_user_callback, (void *) &user_data)//Register a user callback.
 *           //Send data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_send_dma(HAL_I2C_MASTER_0, slave_address, send_data, size)) {
 *               //Error handler;
 *           }
 *           //Receive data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_dma(HAL_I2C_MASTER_0, slave_address, receive_data, size)) {
 *               //Error handler;
 *           }
 *       } else {
 *           //Error handler;
 *       }
 *       //hal_i2c_master_deinit(i2c_port);// Call this function if the I2C is no longer in use.
 *
 *    @endcode
 *    @code
 *       // Callback function sample code. This function should be passed to driver while calling the function #hal_i2c_master_register_callback().
 *       void i2c_user_callback (uint8_t slave_address,hal_i2c_callback_event_t event,void *user_data)
 *       {
 *           if(HAL_I2C_EVENT_ACK_ERROR == event) {
 *               //ACK error handler;
 *           } else if(HAL_I2C_EVENT_NACK_ERROR == event) {
 *               //NACK error handler;
 *           } else if(HAL_I2C_EVENT_TIMEOUT_ERROR == event) {
 *               // Timeout handler;
 *           } else if(HAL_I2C_EVENT_SUCCESS == event) {
 *               // Do something;
 *               // The slave_address indicates which device is using I2C.
 *           }
 *       }
 *
 *    @endcode
 *
 * @}
 * @}
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 *
 * @section HAL_I2C_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DMA                        | Direct Memory Access. For an introduction to DMA, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> DMA in Wikipedia</a>.|
 * |\b GPIO                       | General Purpose Inputs-Outputs. For more details, please refer to the @ref GPIO module in HAL. |
 * |\b I2C                        | Inter-Integrated Circuit. I2C is typically used to attach low-speed peripheral ICs to processors and microcontrollers. For an introduction to I2C, please refer to <a href="https://en.wikipedia.org/wiki/I%C2%B2C"> I2C in Wikipedia</a>.|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual</a>.|
 *
 * @section HAL_I2C_Features_Chapter Supported features
 * - \b Supported \b transaction \b modes
 *  - \b Polling \b mode \b transaction
 *    In polling mode, the functions with the suffix "_polling" will not return any value until the transaction is complete.
 *    After the function returns, the return value should be checked and the error status should be handled.
 *
 *  - \b DMA \b mode \b transaction
 *    In DMA mode, the functions with the suffix "_dma" return a value once the I2C and DMA hardware registers are assigned.
 *    These functions do not wait for the transaction to finish. After the transaction finishes, an interrupt is triggered and the user callback in the I2C ISR
 *    routine is called. After the function returns, the return value should be checked and the error status should be handled.
 *
 *  Note:  \n
 *       1. For more infomation about the transaction length and waveform pattern for each API, please refer to @ref HAL_I2C_Transaction_Pattern_Chapter. \n
 *       2. It is required to use the pull-up resistors on the SCL and SDA of the I2C interface. Please refer to related HDK user guide for more details.
 *
 * @section HAL_I2C_Architecture_Chapter Software architecture of the I2C
 * - \b Polling \b mode \b architecture. \n
 *   Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
 * - \b DMA \b mode \b architecture. \n
 *   DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
 *
 *
 * @section HAL_I2C_Driver_Usage_Chapter How to use this driver
 * - \b Using \b I2C \b in \b polling \b mode.  \n
 *  To use I2C driver in a polling mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function() to select the pinmux function. After setting the pinmux, call #hal_i2c_master_get_running_status() to check the I2C
 *  status. If the status is #HAL_I2C_STATUS_BUS_BUSY, wait till the status becomes #HAL_I2C_STATUS_IDLE. Once the data is successfully
 *  transmitted, call #hal_i2c_master_deinit() to release the I2C resource for other users.\n
 *  Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_i2c_master_init() to initialize the I2C master.
 *  - Step4: Call #hal_i2c_master_send_polling() to send data in a polling mode.
 *  - Step5: Call #hal_i2c_master_receive_polling() to receive data in a polling mode.
 *  - Step6: Call #hal_i2c_master_deinit() to de-allocate the I2C master if it is no longer in use.
 *  - Sample code:
 *    @code
 *       hal_i2c_config_t i2c_config;
 *       uint8_t slave_address = 0X50;
 *       const uint8_t send_data[8] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19}; //the data that used to send
 *       uint8_t receive_data[8] = {0};
 *       volatile uint32_t size = 8;
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *       hal_gpio_init(gpio_pin); //init the gpio pin
 *       hal_pinmux_set_function(gpio_pin, function_index);//Set the pinmux
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_config)) {
 *           //Send data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_send_polling(HAL_I2C_MASTER_0, slave_address, send_data, size)) {
 *               //Error handler;
 *           }
 *           //Receive data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_polling(HAL_I2C_MASTER_0, slave_address, receive_data, size)) {
 *               //Error handler;
 *           }
 *           hal_i2c_master_deinit(HAL_I2C_MASTER_0); //Call this function if the I2C is no longer in use.
 *
 *       } else {
 *           //Error handler;
 *       }
 *
 *    @endcode
 *
 * - \b Using \b I2C \b in \b DMA \b mode. \n
 *  To use I2C driver in DMA mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function to select the pinmux function. After setting the pinmux, call #hal_i2c_master_get_running_status() to check the I2C
 *  status. If the status is #HAL_I2C_STATUS_BUS_BUSY, user should wait till the status is #HAL_I2C_STATUS_IDLE, then
 *  call #hal_i2c_master_register_callback() to register a callback function. Once the data transaction is complete,
 *  call #hal_i2c_master_deinit() in your callback function registered by #hal_i2c_master_register_callback() to release the I2C resource to other users. Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_i2c_master_init() to initialize the I2C master.
 *  - Step4: Call #hal_i2c_master_register_callback() to register a user callback.
 *  - Step5: Call #hal_i2c_master_send_dma() to send data within a DMA mode.
 *  - Step6: Call #hal_i2c_master_receive_dma() to receive data in a DMA mode.
 *  - Step7: Call #hal_i2c_master_deinit() to de-initialize the I2C master if it is no longer in use.
 *  - Sample code:
 *    @code
 *       hal_i2c_config_t i2c_config;
 *       uint8_t slave_address = 0X50;
 *       const uint8_t send_data[8] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19}; //the data that used to send
 *       uint8_t receive_data[8] = {0};
 *       volatile uint32_t size = 8;
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *       hal_gpio_init(gpio_pin); //init the gpio pin
 *       hal_pinmux_set_function(gpio_pin, function_index);//Set the pinmux
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_config)) {
 *           hal_i2c_master_register_callback(HAL_I2C_MASTER_0, i2c_user_callback, (void *) &user_data)//Register a user callback.
 *           //Send data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_send_dma(HAL_I2C_MASTER_0, slave_address, send_data, size)) {
 *               //Error handler;
 *           }
 *           //Receive data
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_dma(HAL_I2C_MASTER_0, slave_address, receive_data, size)) {
 *               //Error handler;
 *           }
 *       } else {
 *           //Error handler;
 *       }
 *       //hal_i2c_master_deinit(i2c_port);// Call this function if the I2C is no longer in use.
 *
 *    @endcode
 *    @code
 *       // Callback function sample code. This function should be passed to driver while calling the function #hal_i2c_master_register_callback().
 *       void i2c_user_callback (uint8_t slave_address,hal_i2c_callback_event_t event,void *user_data)
 *       {
 *           if(HAL_I2C_EVENT_ACK_ERROR == event) {
 *               //ACK error handler;
 *           } else if(HAL_I2C_EVENT_NACK_ERROR == event) {
 *               //NACK error handler;
 *           } else if(HAL_I2C_EVENT_TIMEOUT_ERROR == event) {
 *               // Timeout handler;
 *           } else if(HAL_I2C_EVENT_SUCCESS == event) {
 *               // Do something;
 *               // The slave_address indicates which device is using I2C.
 *           }
 *       }
 *
 *    @endcode
 *
 * @}
 * @}
 */
#endif

#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{

 * - \b Using \b I2C \b in \b extended \b DMA \b mode. \n
 *  To use I2C driver in extended DMA mode, configure the GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function to select the pinmux function. After setting the pinmux, call #hal_i2c_master_get_running_status() to check the I2C
 *  status. If the status is #HAL_I2C_STATUS_BUS_BUSY, user should wait till the status is #HAL_I2C_STATUS_IDLE, then
 *  call #hal_i2c_master_register_callback() to register a callback function. Once the data transaction is complete,
 *  call #hal_i2c_master_deinit() in your callback function to release the I2C resource. The steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO Pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_i2c_master_init() to initialize the I2C master.
 *  - Step4: Call #hal_i2c_master_register_callback() to register a user callback.
 *  - Step5: Call #hal_i2c_master_send_dma_ex() to send data in the extended DMA mode.
 *  - Step6: Call #hal_i2c_master_receive_dma_ex() to receive data in the extended DMA mode.
 *  - Step7: Call #hal_i2c_master_deinit() to de-initialize the I2C master if it is no longer in use.
 *  - Sample code:
 *    @code
 *       hal_i2c_send_config_t send_config;
 *       hal_i2c_config_t i2c_config;
 *       hal_i2c_receive_config_t i2c_receive_config;
 *
 *       i2c_receive_config.slave_address = I2C_EEPROM_SLAVE_ADDRESS;
 *       i2c_receive_config.receive_buffer = i2c_receive_data;
 *       i2c_receive_config.receive_packet_length = 1;
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *       i2c_send_config.slave_address = I2C_EEPROM_SLAVE_ADDRESS;
 *       i2c_send_config.send_data = i2c_send_data;
 *       i2c_send_config.send_packet_length = 1;
 *       i2c_receive_config.receive_bytes_in_one_packet = 5;
 *
 *       hal_gpio_init(HAL_GPIO_36);
 *       hal_gpio_init(HAL_GPIO_37);
 *       //Call hal_pinmux_set_function() to set GPIO pinmux.
 *       hal_pinmux_set_function(HAL_GPIO_36, HAL_GPIO_36_SCL0);
 *       hal_pinmux_set_function(HAL_GPIO_37, HAL_GPIO_37_SDA0);
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_config)) {
 *           hal_i2c_master_register_callback(HAL_I2C_MASTER_0, i2c_user_callback, (void *) &user_data)// Register a user callback.
 *           //Send data by calling the extended DMA API.
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_send_dma_ex(HAL_I2C_MASTER_0, &send_config)) {
 *               //Error handler;
 *           }
 *           if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_dma_ex(HAL_I2C_MASTER_0, &i2c_receive_config)) {
 *               //Error handler;
 *           }
 *       } else {
 *               //Error handler;

 *       }
 *
 *    @endcode
 *    @code
 *       // Callback function sample code. This function should be passed to driver while calling the function #hal_i2c_master_register_callback().
 *       void i2c_user_callback (uint8_t slave_address,hal_i2c_callback_event_t event,void *user_data)
 *       {
 *           if(HAL_I2C_EVENT_ACK_ERROR == event) {
 *               //ACK error handling;
 *           } else if(HAL_I2C_EVENT_NACK_ERROR == event) {
 *               //NACK error handling;
 *           } else if(HAL_I2C_EVENT_TIMEOUT_ERROR == event) {
 *               // Timeout handling;
 *           } else if(HAL_I2C_EVENT_SUCCESS == event) {
 *               // Do something;
 *               // The slave_address indicates which device is using I2C.
 *           }
 *           hal_i2c_master_deinit(HAL_I2C_MASTER_0);// Call this function if the I2C is no longer in use.
 *       }
 *
 *    @endcode
 *
* @}
* @}
*/
#endif

/**
* @addtogroup HAL
* @{
* @addtogroup I2C_MASTER
* @{
*/

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* Enums
*****************************************************************************/

/** @defgroup hal_i2c_master_enum Enum
  * @{
  */

/** @brief This enum defines the transaction result event. These result is passed through a callback function.
  *  For more details, please refer to #hal_i2c_master_register_callback().
  */
typedef enum {
    HAL_I2C_EVENT_SLAVE_ERROR = -4,         /**<  An Slave address ACK error occurred during transaction. */
    HAL_I2C_EVENT_ACK_ERROR = -3,           /**<  An Data ACK error occurred during transaction. */
    HAL_I2C_EVENT_NACK_ERROR = -2,          /**<  A NACK error occurred during transaction.*/
    HAL_I2C_EVENT_TIMEOUT_ERROR = -1,       /**<  A timeout error occurred during transaction.*/
    HAL_I2C_EVENT_SUCCESS = 0               /**<  The transaction completed wihtout any error. */
} hal_i2c_callback_event_t;


/** @brief This enum defines the HAL interface return value. */
typedef enum {
    HAL_I2C_STATUS_TIMEOUT = -5,
    HAL_I2C_STATUS_ERROR = -4,                        /**<  An error occurred and the transaction has failed. */
    HAL_I2C_STATUS_ERROR_BUSY = -3,                   /**<  The I2C bus is busy, an error occurred. */
    HAL_I2C_STATUS_INVALID_PORT_NUMBER = -2,          /**<  A wrong port number is given. */
    HAL_I2C_STATUS_INVALID_PARAMETER = -1,            /**<  A wrong parameter is given. */
    HAL_I2C_STATUS_OK = 0                             /**<  No error occurred during the function call. */
} hal_i2c_status_t;

/** @brief This enum defines the I2C bus status. */
typedef enum {
    HAL_I2C_STATUS_IDLE = 0,                         /**<  The I2C bus is idle. */
    HAL_I2C_STATUS_BUS_BUSY = 1                    /**<  The I2C bus is busy. */
} hal_i2c_running_type_t;

/** @brief This enum defines the I2C transaction speed.  */
typedef enum {
    HAL_I2C_FREQUENCY_50K  = 0,          /**<  50kbps. */
    HAL_I2C_FREQUENCY_100K = 1,          /**<  100kbps. */
    HAL_I2C_FREQUENCY_200K = 2,          /**<  200kbps. */
    HAL_I2C_FREQUENCY_300K = 3,          /**<  300kbps. */
    HAL_I2C_FREQUENCY_400K = 4,          /**<  400kbps. */
    HAL_I2C_FREQUENCY_1M   = 5,          /**<  1mbps. */
    HAL_I2C_FREQUENCY_MAX                /**<  The total number of supported I2C frequencies (invalid I2C frequency).*/
} hal_i2c_frequency_t;

/**
  * @}
  */

/*****************************************************************************
* Structures
*****************************************************************************/

/** @defgroup hal_i2c_master_struct Struct
  * @{
  */


/** @brief This structure defines the configuration settings to initialize the I2C master.  */
typedef struct {
    hal_i2c_frequency_t frequency;             /**<  The transfer speed. Please refer to #hal_i2c_frequency_t for speed definition. */
} hal_i2c_config_t;

/** @brief This structure defines the I2C bus status. */
typedef struct {
    hal_i2c_running_type_t running_status;   /**<  The running status is defined in #hal_i2c_running_type_t. */
} hal_i2c_running_status_t;

#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
/** @brief This structure defines the configuration settings for send_to_receive transaction format. For details about send_to_receive format, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter*/
typedef struct {
    uint8_t slave_address;                     /**<  The slave device address. */
    const uint8_t *send_data;                  /**<  The send data buffer. */
    uint32_t send_length;                      /**<  The length of data to send. The unit is in bytes. */
    uint8_t *receive_buffer;                   /**<  The receive data buffer. */
    uint32_t receive_length;                   /**<  The length of data to receive. The unit is in bytes. */
} hal_i2c_send_to_receive_config_t;
#endif

#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA

/** @brief This structure defines the configuration parameters for extended DMA send transaction format.
 *   For more details about send_packet_length and send_bytes_in_one_packet, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
*/
typedef struct {
    uint8_t slave_address;                     /**<  The slave device address. */
    const uint8_t *send_data;                  /**<  The send data buffer. */
    uint32_t send_packet_length;               /**<   The packet length. */
    uint32_t send_bytes_in_one_packet;         /**<   The number of bytes in one packet. */
} hal_i2c_send_config_t;

/** @brief This structure defines the configuration parameters for extended DMA receive transaction format.
 *   For more details about receive_packet_length and receive_bytes_in_one_packet, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
*/
typedef struct {
    uint8_t slave_address;                     /**<  The slave device address. */
    uint8_t *receive_buffer;                   /**<  The receive data buffer. */
    uint32_t receive_packet_length;            /**<  The packet length. */
    uint32_t receive_bytes_in_one_packet;      /**<  The bytes in one packet. */
} hal_i2c_receive_config_t;

/** @brief This structure defines the configuration parameters for extended DMA send to receive transaction format.
 *   For more details about send_packet_length, send_bytes_in_one_packet, receive_packet_length and receive_bytes_in_one_packet, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 */
typedef struct {
    uint8_t slave_address;                     /**<  The slave device address. */
    const uint8_t *send_data;                  /**<  The send data buffer. */
    uint8_t *receive_buffer;                   /**<  The receive data buffer. */
    uint32_t send_packet_length;               /**<  The send packet length. */
    uint32_t send_bytes_in_one_packet;         /**<  The number of bytes in one packet. */
    uint32_t receive_packet_length;            /**<  The receive packet length. */
    uint32_t receive_bytes_in_one_packet;      /**<  The number of bytes in one packet. */
} hal_i2c_send_to_receive_config_ex_t;

#endif


/**
  * @}
  */

/** @defgroup hal_i2c_master_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype.
 *          User should register a callback function while using DMA mode.
 *          This function is called after the transaction finishes in the I2C ISR routine.
 *          For more details about the callback, please refer to #hal_i2c_master_register_callback().
 *          For more details about callback architecture, please refer to @ref HAL_I2C_Architecture_Chapter.
 *  @param [in] slave_address is a user defined slave address to send or receive data.
 *  @param [in] event is the transaction event of current transaction. It also provides the transaction result.
 *              For more details about the event type, please refer to #hal_i2c_callback_event_t.
 *  @param [in] user_data is the user defined parameter obtained from #hal_i2c_master_register_callback() function.
 *  @sa  #hal_i2c_master_register_callback()
 */
typedef void (*hal_i2c_callback_t)(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data);

/**
  * @}
  */

/*****************************************************************************
* Functions
*****************************************************************************/


/**
 * @brief This function initializes the I2C master before starting a transaction.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] i2c_config Is the configuration parameter to initialize the I2C. Details are described at #hal_i2c_config_t.
 * @return #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *         #HAL_I2C_STATUS_INVALID_PARAMETER, an invalid transfer_frequency is given; \n
 *         #HAL_I2C_STATUS_OK, the operation completed successfully.
 *         #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @note   #hal_i2c_master_deinit() must be called when the I2C is no longer in use, release the I2C resource for the other users to use this I2C master.
 *         Please DO NOT call #hal_i2c_master_init() in interrupt handler, it may cause deadlock.
 *         In a multi-task applications, if #hal_i2c_master_init() returns error #HAL_I2C_STATUS_ERROR_BUSY, it is suggested to call functions that can yield CPU and try
 *         again later.
 * @par       Example
 *    @code
 *
 * hal_i2c_status_t i2c_master_init(void)
 * {
 *   hal_i2c_port_t i2c_port;
 *   hal_i2c_config_t i2c_config;
 *   hal_i2c_status_t error_status;
 *
 *   uint32_t try_times = 0;
 *
 *   i2c_port = HAL_I2C_MASTER_0;
 *   i2c_config.frequency = HAL_I2C_FREQUENCY_400K;
 *
 *   while (try_times < 10) {
 *       error_status = hal_i2c_master_init(i2c_port, &i2c_config);
 *       if (error_status == HAL_I2C_STATUS_ERROR_BUSY) {
 *           vTaskDelay((portTickType)100 / portTICK_RATE_MS);
 *           try_times ++;
 *       } else {
 *         break;
 *       }
 *    }
 *   return error_status;
 * }
 *
 *    @endcode
 *
 * @sa #hal_i2c_master_deinit()
 */
hal_i2c_status_t hal_i2c_master_init(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config);


/**
 * @brief This function releases the I2C master after the transaction is over. Call this function, if the I2C is no longer in use.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @return  #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *          #HAL_I2C_STATUS_OK, the operation completed successfully.
 * @note   This function must be called when the I2C is no longer in use, release the I2C resource for the other users to use this I2C master.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_init()
 */
hal_i2c_status_t hal_i2c_master_deinit(hal_i2c_port_t i2c_port);


/**
 * @brief This function sets the transaction speed. Apply this function to change the transaction speed without using #hal_i2c_master_init().
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] frequency is an enum value defined in #hal_i2c_frequency_t. Only value of type #hal_i2c_frequency_t is accepted.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, an invalid speed is given; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully.
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 * @code
 *       ret = hal_i2c_master_config_speed(HAL_I2C_MASTER_1,HAL_I2C_SPEED_400K);
 * @endcode
 * @sa #hal_i2c_master_init()
 */
hal_i2c_status_t hal_i2c_master_set_frequency(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency);




/**
 * @brief This function registers a callback function while using DMA mode.
 *        The callback function will be called at I2C ISR routine after the I2C triggers an interrupt.
 *        Always call this function to register a callback function while using DMA mode.
 *        Refer to @ref HAL_I2C_Architecture_Chapter for DMA architecture.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] i2c_callback is the user-defined callback function called at I2C ISR routine.
 * @param[in] user_data is a user-defined input data returned during the callback function's call. See the last parameter of #hal_i2c_callback_t.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL function pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK,the operation completed successfully.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 */
hal_i2c_status_t hal_i2c_master_register_callback(hal_i2c_port_t i2c_port, hal_i2c_callback_t i2c_callback, void *user_data);


/**
 * @brief This function sends data to I2C slave in polling mode.
 *  This function will not return until the transaction is complete. For more details about polling mode, see polling mode in
 *  @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] slave_address is the I2C slave address.
 * @param[in] data is the data buffer to send.
 * @param[in] size is the data size to send. The maximum value is #HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR, a hardware error occurred during the transaction.\n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_dma()
 */
hal_i2c_status_t hal_i2c_master_send_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size);


/**
 * @brief This function sends data to I2C slave in DMA mode.
 *  This function returns a value immediately after configuring the hardware registers. For more details about DMA mode, see
 *  DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all I2C masters support DMA mode. For more details about DMA mode,see #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] slave_address is the I2C slave address.
 * @param[in] data is the data buffer to send.
 * @param[in] size is the data size to send. The maximum value is #HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, an NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_polling()
 */
hal_i2c_status_t hal_i2c_master_send_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size);


/**
 * @brief This function receives data from I2C slave in a polling mode.
 *  This function will not return a value until the transaction is finished. For more details about polling mode, see
 *  polling mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] slave_address is the I2C slave address.
 * @param[out] buffer is the data buffer to receive.
 * @param[in] size is the data size to receive. The maximum value is #HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR, a hardware error occurred during the transaction. \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_receive_dma()
 */
hal_i2c_status_t hal_i2c_master_receive_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size);



/**
 * @brief This function receives data from I2C slave in a DMA mode.
 *  This function will return a value immediately after configuring the hardware registers. For more details of DMA mode, see
 *  DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all masters support DMA mode. For more details about DMA mode, see #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in] slave_address is the I2C slave address.
 * @param[out] buffer is the data buffer to receive.
 * @param[in] size is the data size to receive. The maximum value is #HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_receive_polling()
 */
hal_i2c_status_t hal_i2c_master_receive_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t  *buffer, uint32_t size);


#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
/**
 * @brief This function sends data to and then receives data from I2C slave in a polling mode.
 *  This function does not return until the transaction is finished. For more details about polling mode, see
 *  polling mode in @ref HAL_I2C_Features_Chapter chapter.
 *  For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in,out] i2c_send_to_receive_config is the configuration parameter for this API for both send and receive.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR, a hardware error occurred during the transaction. \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_to_receive_dma()
 */
hal_i2c_status_t hal_i2c_master_send_to_receive_polling(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config);



/**
 * @brief This function sends data to and then receives data from I2C slave in a  DMA mode.
 *  This function returns immediately after configuring the hardware registers. For more details of DMA mode, see
 *  DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all masters support DMA mode. For more details about DMA mode, see DMA mode of #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in,out] i2c_send_to_receive_config is the configuration parameter for this API for both send and receive.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_to_receive_polling()
 */
hal_i2c_status_t hal_i2c_master_send_to_receive_dma(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config);

#endif /* #ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE */

#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA

/**
 * @brief This function sends data to I2C slave in DMA mode.
 *  This function returns immediately after configuring the hardware registers. For more details about extended DMA mode, see
 *  extended DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all I2C masters support DMA mode. For more details about DMA mode, see #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number.
 * @param[in] i2c_send_config is the configuration parameter of this API for send.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, an NULL buffer pointer is given by user or either the send_packet_length or send_bytes_in_one_packet is invalid; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_dma(), #hal_i2c_master_send_polling().
 */
hal_i2c_status_t hal_i2c_master_send_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_config_t *i2c_send_config);

/**
 * @brief This function receives data from I2C slave in a extended DMA mode.
 *  This function returns immediately after configuring the hardware registers. For more details of extended DMA mode, see
 *  extended DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all I2C masters support DMA mode. For more details about DMA mode, see #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in,out] i2c_receive_config is the configuration parameter of this API for receive.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, an NULL buffer pointer is given by user or either the send_packet_length or send_bytes_in_one_packet is invalid; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_receive_dma(),#hal_i2c_master_receive_polling().
 */
hal_i2c_status_t hal_i2c_master_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_receive_config_t *i2c_receive_config);

/**
 * @brief This function sends data to and then receives data from I2C slave in a extended DMA mode.
 *  This function returns immediately after configuring the hardware registers. For more details of extended DMA mode, see
 *  extended DMA mode in @ref HAL_I2C_Features_Chapter chapter. For details about transaction length and transaction waveform pattern, see @ref HAL_I2C_Transaction_Pattern_Chapter chapter.
 *  Note, not all I2C masters support DMA mode. For more details about DMA mode, see #hal_i2c_port_t.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[in,out] i2c_send_to_receive_config_ex is the configuration parameter of this API for both send and receive.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_INVALID_PARAMETER, an NULL buffer pointer is given by user or either the send_packet_length or send_bytes_in_one_packet is invalid; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_STATUS_ERROR_BUSY, the I2C bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 * @sa #hal_i2c_master_send_to_receive_dma(), #hal_i2c_master_send_to_receive_polling().
 */
hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_t *i2c_send_to_receive_config_ex);

#endif


/**
 * @brief This function gets running status of the I2C master.
 *  Call this function to check if the I2C is idle or not before transferring data. If it's not idle, then the resource is currently in use,
 *  delay the operation until the I2C is idle.
 * @param[in] i2c_port is the I2C master port number. The value is defined in #hal_i2c_port_t.
 * @param[out] running_status
 *             #HAL_I2C_STATUS_BUS_BUSY, the I2C master is in busy status; \n
 *             #HAL_I2C_STATUS_IDLE, the I2C master is in idle status; User can use it to transmit data immediately.
 * @return   #HAL_I2C_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_STATUS_OK, the operation completed successfully.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Driver_Usage_Chapter
 */
hal_i2c_status_t hal_i2c_master_get_running_status(hal_i2c_port_t i2c_port, hal_i2c_running_status_t *running_status);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_I2C_MASTER_MODULE_ENABLED*/
#endif /* __HAL_I2C_MASTER_H__ */


