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

#ifndef __HAL_SPI_SLAVE_H__
#define __HAL_SPI_SLAVE_H__

#include "hal_platform.h"

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

#ifdef HAL_SPI_SLAVE_FEATURE_SW_CONTROL
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * This section introduces the Serial Periheral Interface(SPI) Slave APIs including terms and acronyms, supported features,
 * software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_SPI_SLAVE_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b DMA                  | Direct Memory Access. DMA is a feature of computer systems that allows certain hardware subsystems to access main system memory independent from the central processing unit (CPU).|
 * |\b GPIO                 | General Purpose Inputs-Outputs. For more details, please refer to @ref GPIO.|
 * |\b IRQ                  | Interrupt Request. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Interrupt_request_(PC_architecture)"> IRQ</a>.|
 * |\b MISO                 | Master Input, Slave Output. Output from the SPI slave.|
 * |\b MOSI                 | Master Output, Slave Input. Output from the SPI master.|
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M series processors. For more details, please refer to <a href="http://infocenter.arm.com/help/topic/com.arm.doc.100166_0001_00_en/arm_cortexm4_processor_trm_100166_0001_00_en.pdf"> ARM Cortex-M4 technical reference manual</a>.|
 * |\b SCLK                 | Serial Clock. Output from the SPI master.|
 * |\b SPI                  | Serial Peripheral Interface. The SPI bus is a synchronous serial communication interface specification used for short distance communication. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 * |\b SS                   | Slave Select. Output from the SPI master, active low.|
 *
 * @section HAL_SPI_SLAVE_Features_Chapter Supported features
 *
 * Emerging sensor applications support SPI interface communication. An example application is a sensor hub. An SPI slave interface is required
 * to communicate with an SPI master interface. This controller is a common SPI slave interface with a defined specification
 * for communication and it only supports the DMA mode. The SPI slave hardware is enabled to receive a set of commands from the
 * SPI master to handle specific operations. The SPI slave software provides automatic error handling and status update,
 * if the SPI slave hardware detects any malfunction.
 * - \b Support \b the \b defined \b commands. \n
 *  - Power-On command(poweron):
 *    This should be the first command the SPI master sends to the SPI slave during a transaction. The length of each Power-On command is 1 byte.
 *  - Power-Off command(poweroff):
 *    This should be the last command the SPI master sends to the SPI slave during a transaction. The length of each Power-Off command is 1 byte.
 *  - Configure write command:
 *    Use this command to configure parameters for the write operation. The command includes a configure write command(1 byte), addr_id(4 bytes) and
 *    (data_length-1)(4 bytes). The total length of this command is 9 bytes, addr_id is used to distinguish between different SPI slave devices. The user needs to make
 *    sure different SPI slave devices have different addr_id in one system, data_length is the length of the data to be transferred.
 *  - Write data command:
 *    This command is used to write data to the SPI slave device. The command includes a write data command(1 byte), transfer_length(4 bytes)
 *    and the total length of this command is (1+transfer_length) bytes, so that slave select signal stays valid for the SPI master to finish the transaction.
 *    Note, that transfer_length must be configured with the same value as the data_length in the write command.
 *  - Configure read command:
 *    This command is used to configure parameters for the read operation. The command includes a configure read command(1 byte), addr_id(4 bytes) and
 *    (data_length-1)(4 bytes). The total length of this command is 9 bytes, addr_id is used to distinguish between different SPI slave devices, the user needs to make
 *    sure different SPI slave devices have different addr_id in one system, data_length is the length of the data to be transferred.
 *  - Read data command:
 *    This command is used to read data from the SPI slave device. The command includes a read data command(1 byte), transfer_length(4 bytes)
 *    and the total length of this command is (1+transfer_length) bytes, so that slave select signal stays valid for the SPI master to finish the transaction.
 *    Note, that transfer_length must be configured with the same value as the data_length in the read command.
 *  - Read status command:
 *    This command is used for the master to get the SPI slave status. The command includes a read status command(1 byte) and
 *    the total length of this command is 2 bytes, so that slave select signal stays valid for master to get the status of SPI slave device.
 *  - Write status command:
 *    This command is used for the master to clear the SPI slave device's error status. The command includes a write status command(1 byte)
 *    and the bitmap of status to be cleared(1 byte), so the total length of this command is 2 bytes.
 *
 * - \b Support \b the \b defined \b command \b pattern. \n
 *  In order to use the SPI slave controller, user must make sure the SPI master uses the defined command to send the data out, otherwise it will result in a state machine
 *  error. The basic rules of the defined command pattern are shown below:
 *  Write command pattern:
 *  - The configure write command must be followed by write data command with no other commands running in between.
 *  Read command pattern:
 *  - The configure read command must be followed by read data command with no other commands running in between.
 *  - The poweron command must be the first command to start the data transaction. If the SPI slave is no longer in use, the SPI master should send the poweroff command to
 *    the slave. It's possible to have multiple read and write command patterns between the Power-On and Power-Off commands.
 *  - Except for the read status command and write status command, any other command sent from the SPI master
 *    should be checked whether the command is accepted by the SPI slave successfully by sending a read status command.
 *
 * - \b Support \b the \b DMA \b mode. \n
 *  In DMA mode, functions return a value once the SPI slave hardware register and DMA configuration are set. When functions return,
 *  the transaction is usually unfinished. After it's finished, an interrupt is triggered and related user callback
 *  is called in the SPI slave interrupt service routine (ISR).
 *
 * @section HAL_SPI_SLAVE_Architecture_Chapter Software architecture of the SPI slave
 *
 * - \b The \b DMA \b mode \b architecture. \n
 *   The DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
 *
 * @section HAL_SPI_SLAVE_Driver_Usage_Chapter How to use this driver
 *
 * - \b Using \b the \b SPI \b slave \b in \b the \b DMA \b mode.
 *  Call #hal_pinmux_set_function() to pinmux GPIO pins to four SPI pins (SS, SCLK, MOSI, and MISO) based on the user's hardware platform design.
 *  Then call #hal_spi_slave_register_callback() to register a callback function. Since the SPI slave's behavior is passive, most of the operations are carried
 *  through a callback function. If a Power-On interrupt occurred, call #hal_spi_slave_init() in the callback function to apply
 *  basic settings for SPI slave hardware. If the function's return status is #HAL_SPI_SLAVE_STATUS_ERROR_BUSY, then the SPI slave controller is currently
 *  busy, user should wait until it's idle. Once the transaction is over, if a Power-Off interrupt occurred, call #hal_spi_slave_deinit() in the callback
 *  function to release the SPI slave resource to make it available for other users. The steps are shown below:
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux, if the EPT tool hasn't been used to configure the related pinmux.
 *           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
 *  - Step3: Call #hal_spi_slave_register_callback() to register a user callback.
 *  - Step4: Call #hal_spi_slave_set_command() to set a user-defined command that matches the SPI master user-defined command. Apply this function to avoid the default settings.
 *  - Step5: Call #hal_spi_slave_set_early_miso() to change the SPI timing.
 *  - Step6: Call #hal_spi_slave_init() in the callback function, if Power-On interrupt occurred, to initialize a single SPI slave controller.
 *  - Step7: Call #hal_spi_slave_send() in the callback function, if configure read finish interrupt occurred, to send data in the DMA mode.
 *  - Step8: Call #hal_spi_slave_receive() in the callback function, if configure write finish interrupt occurred, to receive data in the DMA mode.
 *  - Step9: Call #hal_spi_slave_deinit() in the callback function, if Power-Off interrupt occurred, to deinitialize the SPI slave that is no longer in use.
 *  - sample code:
 *    @code
 *       uint16_t slave_status;
 *       hal_spi_slave_config_t spi_configure;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(SPIS_PIN_NUMBER_CS);
 *       hal_gpio_init(SPIS_PIN_NUMBER_SLK);
 *       hal_gpio_init(SPIS_PIN_NUMBER_MOSI);
 *       hal_gpio_init(SPIS_PIN_NUMBER_MISO);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = SPIS_PIN_FUNC_CS.
 *       hal_pinmux_set_function(SPIS_PIN_NUMBER_CS, function_index);
 *       //function_index = SPIS_PIN_FUNC_SLK.
 *       hal_pinmux_set_function(SPIS_PIN_NUMBER_SLK, function_index);
 *       //function_index = SPIS_PIN_FUNC_MOSI.
 *       hal_pinmux_set_function(SPIS_PIN_NUMBER_MOSI, function_index);
 *       //function_index = SPIS_PIN_FUNC_MISO.
 *       hal_pinmux_set_function(SPIS_PIN_NUMBER_MISO, function_index);
 *
 *       hal_spi_slave_register_callback(slave_port,user_spi_callback,NULL)// Register a user callback.
 *
 *       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_set_command(slave_port,command,value)) {
 *          //error handle
 *       }
 *
 *       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_set_early_miso(slave_port,early_miso)) {
 *           //error handle
 *       }
 *    @endcode
 *    @code
 *       // Callback function sample code. This function should be passed to driver while calling hal_spi_slave_register_callback().
 *       hal_spi_slave_status_t spi_slave_callback (hal_spi_slave_transaction_status_t status,void *user_data)
 *       {
 *
 *           uint16_t slave_status;
 *           const uint8_t *data = 0x04000000;  // Note: the data buffer address must be a non-cacheable address.
 *           uint32_t size = 0x00000010;
 *           uint8_t *buffer = 0x04000400;  // Note: the data buffer address must be a non-cacheable address.
 *           hal_spi_slave_config_t spi_configure;
 *           if (HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION == (status.fsm_status)) {
 *               // Normal FSM behavior.
 *               slave_status = status.slave_status;
 *               switch (slave_status) {
 *                   case SPISLV_IRQ_POWERON_IRQ_MASK:
 *                       hal_spi_slave_config_t spi_configure;
 *                       spi_configure.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
 *                       spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
 *                       spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
 *                       spi_configure.timeout_threshold = 0xFFFFFFFF;
 *
 *                       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_init(HAL_SPI_SLAVE_0, &spi_configure)) {
 *                           // error handle
 *                       }
 *                       break;
 *                   case SPISLV_IRQ_POWEROFF_IRQ_MASK:
 *
 *                       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_deinit(HAL_SPI_SLAVE_0)) {
 *                           // error handle
 *                       }
 *                       break;
 *                   case SPISLV_IRQ_CRD_FINISH_IRQ_MASK:
 *
 *                       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_send(HAL_SPI_SLAVE_0, data, size)) {
 *                           // error handle
 *                       }
 *                       break;
 *                   case SPISLV_IRQ_CWR_FINISH_IRQ_MASK:
 *
 *                       if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_receive(HAL_SPI_SLAVE_0, buffer, size)) {
 *                           // error handle
 *                       }
 *                       break;
 *                   case SPISLV_IRQ_RD_FINISH_IRQ_MASK:
 *                       // Send data to the SPI master successfully.
 *                       break;
 *                   case SPISLV_IRQ_WR_FINISH_IRQ_MASK:
 *                       // User now can get the data from an earlier defined address.
 *                       break;
 *                   case SPISLV_IRQ_RD_ERR_IRQ_MASK:
 *                       // It depends on the user application whether to reserve a data buffer for re-transmit or not.
 *                       break;
 *                   case SPISLV_IRQ_WR_ERR_IRQ_MASK:
 *                       // Wrong data in the predefined address. Abandon the data.
 *                       break;
 *                   case SPISLV_IRQ_TIMEOUT_ERR_IRQ_MASK:
 *                       // User may extend timeout threshold value and re-run send/receive data.
 *                       break;
 *                   default:
 *                       break;
 *               }
 *           } else if (HAL_SPI_SLAVE_FSM_INVALID_OPERATION != (status.fsm_status)) {
 *                      // SPI slave state machine error occurred. The user should check why an incorrect command is sent from the SPI master side,
 *                      // and take the following actions according to the current state machine status.
 *               switch (status.fsm_status) {
 *                   case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, current state machine is poweroff\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, current state machine is poweroff\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, current state machine is CR\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW, current state machine is CR\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, current state machine is CW\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR, current state machine is CW\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR, current state machine is poweron\n");
 *                       break;
 *                   case HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW:
 *                       printf("HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, current state machine is poweron\n");
 *                       break;
 *                   default:
 *                       break;
 *               }
 *           } else {
 *               // Invalid FSM behavior.
 *               // The FSM is set to PWRON_STA for all cases. Call restart from the related configure command to send or receive data.
 *           }
 *       }
 *    @endcode
 *
 */
#else
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 *
 * This section introduces the SPI Slave APIs including terms and acronyms, supported features,
 * software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_SPI_SLAVE_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b DMA                  | Direct Memory Access. DMA is a feature of computer systems that allows certain hardware subsystems to access main system memory independent from the central processing unit (CPU).|
 * |\b GPIO                 | General Purpose Inputs-Outputs. For more details, please refer to @ref GPIO.|
 * |\b IRQ                  | Interrupt Request. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Interrupt_request_(PC_architecture)"> IRQ</a>.|
 * |\b MISO                 | Master Input, Slave Output. Output from the SPI slave.|
 * |\b MOSI                 | Master Output, Slave Input. Output from the SPI master.|
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M series processors. For more details, please refer to <a href="http://infocenter.arm.com/help/topic/com.arm.doc.100166_0001_00_en/arm_cortexm4_processor_trm_100166_0001_00_en.pdf"> ARM Cortex-M4 technical reference manual</a>.|
 * |\b SCLK                 | Serial Clock. Output from the SPI master.|
 * |\b SPI                  | Serial Peripheral Interface. The Serial Peripheral Interface bus is a synchronous serial communication interface specification used for short distance communication. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 * |\b SS                   | Slave Select. Output from the SPI master, active low.|
 *
 * @section HAL_SPI_SLAVE_Features_Chapter Supported features
 *
 * Emerging sensor applications support SPI interface communication. An SPI slave interface is required
 * to communicate with an SPI master interface. This controller is the SPI slave interface with five functional registers for communication.
 * - \b Support \b functional \b registers. \n
 *  There are six registers in this slave controller:
 *  - Reg00 - reads data from the bus.
 *  - Reg01 - writes data to the bus.
 *  - Reg02 - the address to write/read to/from the bus, the configured value should correspond to a physical address in the system.
 *  - Reg03 - the bus protocol command.
 *  - Reg04 - the polling status to check whether the bus is idle or busy.
 *  - Reg05 - the software IRQ command.
 *
 * - \b Support \b standard \b mode \b and \b fast \b mode \b programming \b sequences. \n
 *  Fast mode supports address auto increment to reduce bus address write time when the SPI slave accesses the system memory. Example 1
 *  and 2 use standard mode. Example 3 and 4 use fast mode. The default driver settings are on fast mode to provide better throughput. \n
 *  Example 1: Write 0x0123_4567 data to the address 0x1013_0004. \n
 *  - Step1:
 *      Prepare command to access the bus, the SPI master asserts the following data on MOSI. \n
 *      {8'h80,8'h04,16'h4567} // Put bus write data[15:0] into the SPI slave reg01[15:0]. \n
 *      {8'h80,8'h06,16'h0123} // Put bus write data[31:16] into the SPI slave reg01[31:16]. \n
 *      {8'h80,8'h08,16'h0004} // Put bus address[15:0] into the SPI slave reg02[15:0]. \n
 *      {8'h80,8'h0A,16'h1013} // Put bus address[31:16] into the SPI slave reg02[31:16]. \n
 *
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b1}} // Start the bus write access.
 *
 *  Example 2: Read 0x0123_4567 data from the address 0x1013_0004.
 *             Note: Check if the status is busy before reading data from the SPI slave controller. \n
 *  - Step1:
 *      Prepare command to access the bus, the SPI master asserts the following data on MOSI. \n
 *      {8'h80,8'h08,16'h0004} // Put bus address[15:0] into the SPI slave reg02[15:0]. \n
 *      {8'h80,8'h0A,16'h1013} // Put bus address[31:16] into the SPI slave reg02[31:16]. \n
 *
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b0}} // Access the bus to read data. \n
 *  - Step2:
 *      Wait bus accessing done, the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h10,16'hxxxx} // Read the SPI slave bus interface status. \n
 *      Wait until the MISO returned data bit[0]=0, make sure that bus access has finished. \n
 *  - Step3:
 *      Get read data, the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h00,16'hxxxx} // Get bus read data[15:0] from the SPI slave reg00[15:0], \n
 *      MISO return data 16'h4567. \n
 *      {8'h00,8'h02,16'hxxxx} // Get bus read data[31:16] from the SPI slave reg00[31:16], \n
 *      MISO return data 16'h0123.
 *
 *  Example 3: Write 0x0123_4567 data to the address 0x1013_0004.
 *             Write 0x89ab_cdef data to the address 0x1013_0008.
 *             SPI master sends software IRQ command to trigger SPI slave IRQ. \n
 *  - Step1:
 *      Prepare command to access the bus, the SPI master asserts the following data on MOSI. \n
 *      {8'h80,8'h04,16'h4567} // Put bus write data[15:0] into the SPI slave reg01[15:0]. \n
 *      {8'h80,8'h06,16'h0123} // Put bus write data[31:16] into the SPI slave reg01[31:16]. \n
 *      {8'h80,8'h08,16'h0004} // Put bus address[15:0] into the SPI slave reg02[15:0]. \n
 *      {8'h80,8'h0A,16'h1013} // Put bus address[31:16] into the SPI slave reg02[31:16]. \n
 *
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b1}} // Start the bus write access. \n
 *  - Step2:
 *      // Address will auto-increment by 4 bytes after the last master transaction. \n
 *      Prepare command to access the bus, the SPI master asserts the following data on MOSI. \n
 *      {8'h80,8'h04,16'hcdef} // Put bus write data[15:0] into the SPI slave reg01[15:0]. \n
 *      {8'h80,8'h06,16'h89ab} // Put bus write data[31:16] into the SPI slave reg01[31:16]. \n
 *      // No need to write the bus address. \n
 *
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b1}} // Start the bus write access. \n
 *  - Step3:
 *      Prepare software IRQ command to access the bus, the SPI master asserts the following data on MOSI. \n
 *      {8'h80,8'h14,16'h0001} // Write 1 into the SPI slave reg05[0]. \n
 *
 *  Example 4: Read 0x0123_4567 data from the address 0x1013_0004.
 *             Read 0x89ab_cdef data from the address 0x1013_0008.
 *             Note: User needs to check busy status before reading the bus data from the SPI slave controller. \n
 *  - Step1:
 *      Prepare command for bus accessing, SPI master asserts following data on MOSI respectively. \n
 *      {8'h80,8'h08,16'h0004} // Put bus address[15:0] into the SPI slave reg02[15:0]. \n
 *      {8'h80,8'h0A,16'h1013} // Put bus address[31:16] into the SPI slave reg02[31:16]. \n
 *
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b0}} // Start the bus read access. \n
 *  - Step2:
 *      Wait till the bus access is complete, then the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h10,16'hxxxx} // Read the SPI slave bus interface status. \n
 *      Wait until the MISO returned data bit[0]=0, make sure that bus access has finished. \n
 *  - Step3:
 *      Get read data, the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h00,16'hxxxx} // Get bus read data[15:0] from the SPI slave reg00[15:0], \n
 *      MISO return data 16'h4567. \n
 *      {8'h00,8'h02,16'hxxxx} // Get bus read data[31:16] from the SPI slave reg00[31:16], \n
 *      MISO return data 16'h0123. \n
 *  - Step4:
 *      // No need to write address into reg02. \n
 *      // User can start reading from the bus immediately. \n
 *      {8'h80,8'h0c,{13'b0,2'b10,1'b0}} // Start the bus read access. \n
 *  - Step5:
 *      Wait till the bus access is complete, then the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h10,16'hxxxx} // Read the SPI slave bus interface status. \n
 *      Wait until the MISO returned data bit[0]=0, make sure that bus access has finished. \n
 *  - Step6:
 *      Get read data at address 0x1013_0008, the SPI master asserts the following data on MOSI. \n
 *      {8'h00,8'h00,16'hxxxx} // Get bus read data[15:0] from the SPI slave reg00[15:0], \n
 *      MISO return data 16'hcdef. \n
 *      {8'h00,8'h02,16'hxxxx} // Get bus read data[31:16] from the SPI slave reg00[31:16], \n
 *      MISO return data 16'h89ab. \n
 *
 * @section HAL_SPI_SLAVE_Architecture_Chapter Software architecture of the SPI slave
 *  The architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
 *
 * @section HAL_SPI_SLAVE_Driver_Usage_Chapter How to use this driver
 *
 *  Call #hal_pinmux_set_function() to pinmux the GPIO pins to four SPI pins (SS, SCLK, MOSI, and MISO) based on the user's hardware platform design.
 *  Please note that user should choose the SPI pins for SPI slave, which can support 20MHz SPI clock, to get a better throughput.
 *  Then call #hal_spi_slave_init() to apply basic settings for SPI slave hardware. After that call #hal_spi_slave_register_callback() to register
 *  a user callback function. The steps are shown below:
 *  - Step1: Call #hal_gpio_init() to init the pins, if EPT tool hasn't been used to configure the related pinmux.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux, if the EPT tool wasn't used to configure the related pinmux.
 *           For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
 *  - Step3: Call #hal_spi_slave_init() to configure the SPI slave hardware settings.
 *  - Step4: Call #hal_spi_slave_register_callback() to register a user callback.
 *  - An example implementation of the SPI slave.
 *    @code
 *       hal_spi_slave_config_t spi_configure;
 *       spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
 *       spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
 *
 *       //Init GPIO, set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).
 *       hal_gpio_init(HAL_GPIO_29);
 *       hal_gpio_init(HAL_GPIO_30);
 *       hal_gpio_init(HAL_GPIO_31);
 *       hal_gpio_init(HAL_GPIO_32);
 *
 *       //Call hal_pinmux_set_function() to set GPIO pinmux, for more information, please reference hal_pinmux_define.h
 *       //Need not to configure pinmux if EPT tool is used.
 *       //function_index = HAL_GPIO_29_SPI_MOSI_S_CM4.
 *       hal_pinmux_set_function(HAL_GPIO_29, function_index);// No need to configure pinmux, if EPT tool is used.
 *       //function_index = HAL_GPIO_30_SPI_MISO_S_CM4.
 *       hal_pinmux_set_function(HAL_GPIO_30, function_index);// No need to configure pinmux, if EPT tool is used.
 *       //function_index = HAL_GPIO_31_SPI_SCK_S_CM4.
 *       hal_pinmux_set_function(HAL_GPIO_31, function_index); // No need to configure pinmux, if EPT tool is used.
 *       //function_index = HAL_GPIO_32_SPI_CS_0_S_CM4.
 *       hal_pinmux_set_function(HAL_GPIO_32, function_index);// No need to configure pinmux, if EPT tool is used.
 *
 *       ret_status = hal_spi_slave_init(HAL_SPI_SLAVE_0, &spi_configure);
 *       hal_spi_slave_register_callback(HAL_SPI_SLAVE_0,user_spi_callback,NULL)// Register a user callback.
 *    @endcode
 *
 */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_spi_slave_enum Enum
  * @{
  */
#ifdef HAL_SPI_SLAVE_FEATURE_SW_CONTROL
/** @brief SPI slave transaction bit order definition. */
typedef enum {
    HAL_SPI_SLAVE_LSB_FIRST = 0,                       /**< Both send and receive data transfer is the LSB first. */
    HAL_SPI_SLAVE_MSB_FIRST = 1                        /**< Both send and receive data transfer is the MSB first. */
} hal_spi_slave_bit_order_t;


/** @brief The SPI slave sends MISO data half SCLK cycle early. */
typedef enum {
    HAL_SPI_SLAVE_EARLY_MISO_DISABLE = 0,            /**< Disables send MISO data half SCLK cycle early. */
    HAL_SPI_SLAVE_EARLY_MISO_ENABLE  = 1             /**< Enables send MISO data half SCLK cycle early. */
} hal_spi_slave_early_miso_t;


/** @brief SPI slave running status. */
typedef enum {
    HAL_SPI_SLAVE_BUSY              = 0,             /**< The SPI slave is busy. */
    HAL_SPI_SLAVE_IDLE              = 1              /**< The SPI slave is idle. */
} hal_spi_slave_running_status_t;


/********************* SPI Slave FSM *******************************
**cmd\status    PWROFF    PWRON    ConfigRead(CR)    ConfigWrite(CW)
** Poweroff       N         Y         Abnormal          Abnormal
** Poweron        Y         N             N                N
**ConfigRead      N         Y         Abnormal          Abnormal
**  Read          N         N             Y             Abnormal
**ConfigWrite     N         Y         Abnormal          Abnormal
**  Write         N         N         Abnormal             Y

Note: N: invalid operation.
      Y: the operation completed successfully.
      Abnormal: an error occurred during the operation.
*******************************************************************/
/** @brief SPI slave finite state machine status. */
typedef enum {
    HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION      = 0x00,         /**< SPI slave success operation. */
    HAL_SPI_SLAVE_FSM_INVALID_OPERATION      = 0x01,         /**< SPI slave invalid operation. */
    HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR  = 0x02,         /**< SPI slave error, received Power-Off command after Configure Read command. */
    HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW  = 0x03,         /**< SPI slave error, received Power-Off command after Configure Write command. */
    HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR     = 0x04,         /**< SPI slave error, received continous Configure Read command. */
    HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW      = 0x05,         /**< SPI slave error, received Configure Read command after Configure Write command. */
    HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW     = 0x06,         /**< SPI slave error, received continous Configure Write command. */
    HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR      = 0x07,         /**< SPI slave error, received Configure Write command after Configure Read command. */
    HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR   = 0x08,         /**< SPI slave error, received Write command after Configure Read command. */
    HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW    = 0x09          /**< SPI slave error, received Read command after Configure Write command. */
} hal_spi_slave_fsm_status_t;


/** @brief The SPI slave command enum. */
typedef enum {
    HAL_SPI_SLAVE_CMD_WS        = 0,       /**< Write Status command. */
    HAL_SPI_SLAVE_CMD_RS        = 1,       /**< Read Status command. */
    HAL_SPI_SLAVE_CMD_WR        = 2,       /**< Write Data command. */
    HAL_SPI_SLAVE_CMD_RD        = 3,       /**< Read Data command. */
    HAL_SPI_SLAVE_CMD_POWEROFF  = 4,       /**< Power-Off command. */
    HAL_SPI_SLAVE_CMD_POWERON   = 5,       /**< Power-On command. */
    HAL_SPI_SLAVE_CMD_CW        = 6,       /**< Configure Write command. */
    HAL_SPI_SLAVE_CMD_CR        = 7        /**< Configure Read command. */
} hal_spi_slave_command_type_t;
#endif

/** @brief SPI slave clock polarity definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_POLARITY0 = 0,                 /**< Clock polarity is 0. */
    HAL_SPI_SLAVE_CLOCK_POLARITY1 = 1                  /**< Clock polarity is 1. */
} hal_spi_slave_clock_polarity_t;


/** @brief SPI slave clock format definition. */
typedef enum {
    HAL_SPI_SLAVE_CLOCK_PHASE0 = 0,                    /**< Clock format is 0. */
    HAL_SPI_SLAVE_CLOCK_PHASE1 = 1                     /**< Clock format is 1. */
} hal_spi_slave_clock_phase_t;


/** @brief SPI slave status. */
typedef enum {
    HAL_SPI_SLAVE_STATUS_ERROR             = -4,            /**< An error occurred during the SPI slave API call. */
    HAL_SPI_SLAVE_STATUS_ERROR_BUSY        = -3,            /**< The SPI slave was busy during the SPI slave API call. */
    HAL_SPI_SLAVE_STATUS_ERROR_PORT        = -2,            /**< Invalid SPI slave port number. */
    HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER = -1,            /**< Invalid parameter. */
    HAL_SPI_SLAVE_STATUS_OK                = 0              /**< The SPI slave API call was successful. */
} hal_spi_slave_status_t;

/**
  * @}
  */

/** @defgroup hal_spi_slave_struct Struct
  * @{
  */
#ifdef HAL_SPI_SLAVE_FEATURE_SW_CONTROL
/** @brief SPI slave configure. */
typedef struct {
    uint32_t timeout_threshold;                      /**< The SPI slave timeout threshold setting. */
    hal_spi_slave_bit_order_t bit_order;             /**< The SPI slave bit order setting. */
    hal_spi_slave_clock_polarity_t polarity;         /**< The SPI slave clock polarity setting. */
    hal_spi_slave_clock_phase_t phase;               /**< The SPI slave clock phase setting. */
} hal_spi_slave_config_t;


/** @brief SPI slave transaction status definition. */
typedef struct {
    hal_spi_slave_fsm_status_t fsm_status;           /**< An item from hal_spi_slave_fsm_status_t. */
    uint32_t interrupt_status;                       /**< The interrupt status of the SPI slave controller. */
} hal_spi_slave_transaction_status_t;
#else
/** @brief SPI slave configure. */
typedef struct {
    hal_spi_slave_clock_polarity_t polarity;         /**< The SPI slave clock polarity setting. */
    hal_spi_slave_clock_phase_t phase;               /**< The SPI slave clock phase setting. */
} hal_spi_slave_config_t;
#endif
/**
  * @}
  */

/** @defgroup hal_spi_slave_typedef Typedef
  * @{
  */

#ifdef HAL_SPI_SLAVE_FEATURE_SW_CONTROL
/** @brief  This defines the callback function prototype.
 *          User should register a callback function to use the SPI slave. This function is called in the SPI slave ISR,
 *          if an interrupt is triggered. For more details about the callback function, please refer to #hal_spi_slave_register_callback().
 *          For more details about the callback architecture, please refer to @ref HAL_SPI_SLAVE_Architecture_Chapter.
 *  @param[in] status is the transaction status for the current transaction, including state machine status and interrupt status.
 *              For details about the status type, please refer to #hal_spi_slave_transaction_status_t.
 *  @param[in] user_data is a user defined parameter obtained from #hal_spi_slave_register_callback().
 *  @sa  #hal_spi_slave_register_callback()
 */
typedef void (* hal_spi_slave_callback_t)(hal_spi_slave_transaction_status_t status, void *user_data);
#else
/** @brief  This defines the callback function prototype.
 *          User may register a callback function to use the SPI slave. This function is called in the SPI slave interrupt
 *          service routine if an interrupt is triggered. For more details about the callback function, please refer to #hal_spi_slave_register_callback().
 *  @param[in] user_data is a user defined parameter obtained from #hal_spi_slave_register_callback().
 */
typedef void (* hal_spi_slave_callback_t)(void *user_data);
#endif
/**
  * @}
  */

#ifdef HAL_SPI_SLAVE_FEATURE_SW_CONTROL
/**
 * @brief     This function initializes the SPI slave and sets user defined common parameters such as timeout threshold,
 *            bit order, clock polarity and clock phase.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] spi_configure is the SPI slave configure parameters. Details are described at #hal_spi_slave_config_t.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR, if the SPI slave port is in use. \n
 *            #HAL_SPI_SLAVE_STATUS_ERROR_BUSY, if the SPI slave port is busy and not available for use; \n
 *            #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 * @sa #hal_spi_slave_deinit()
 */
hal_spi_slave_status_t hal_spi_slave_init(hal_spi_slave_port_t spi_port, hal_spi_slave_config_t *spi_configure);


/**
 * @brief     This function resets the SPI slave, gates its clock and disables interrupts.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 * @sa #hal_spi_slave_init()
 */
hal_spi_slave_status_t hal_spi_slave_deinit(hal_spi_slave_port_t spi_port);


/**
 * @brief     This function registers user's callback in the SPI slave driver. This function should always be called when using the SPI slave
 *            driver, the callback will be called in SPI ISR.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] callback_function is the callback function given by user, which will be called at SPI slave interrupt service routine.
 * @param[in] user_data is a parameter given by user and will pass to user while the callback function is called. See the last parameter of #hal_spi_slave_callback_t.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refer to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 */
hal_spi_slave_status_t hal_spi_slave_register_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t callback_function, void *user_data);


/**
 * @brief     This function sends data asynchronously with DMA mode, this function should be called from user's callback function.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] data is the buffer of data to be sent, this parameter cannot be NULL, also the address must be as non-cacheable address.
 * @param[in] size is the number of bytes to send.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 */
hal_spi_slave_status_t hal_spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size);


/**
 * @brief     This function receives data asynchronously with DMA mode. This function should be called from user's callback function.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] buffer is the data buffer where the received data are stored, this parameter cannot be NULL, also the address must be as non-cacheable address.
 * @param[in] size is the number of bytes to receive.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 */
hal_spi_slave_status_t hal_spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size);


/**
 * @brief     This function configures the SPI slave send MISO early half SCLK cycle parameter.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] early_miso is the parameter to enable the send MISO data half SCLK cycle early feature or disable it.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 */
hal_spi_slave_status_t hal_spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso);


/**
 * @brief     This function configures the SPI slave command value.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] command is the SPI slave command type, the value is defined at #hal_spi_slave_command_type_t.
 * @param[in] value is the command value that needs to be configured.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 * @par       Example
 * Sample code please refers to @ref HAL_SPI_SLAVE_Driver_Usage_Chapter.
 */
hal_spi_slave_status_t hal_spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value);
#else
/**
 * @brief     This function initializes the SPI slave and sets user defined common parameters such as
 *            clock polarity and clock phase, always setup internal configuration for 20MHz SPI clock for better throughput.
 *            Note that the SPI slave supports only MSB bit order and can't be config to LSB bit order.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] spi_configure is the SPI slave configure parameters. Details are described at #hal_spi_slave_config_t.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 */
hal_spi_slave_status_t hal_spi_slave_init(hal_spi_slave_port_t spi_port, hal_spi_slave_config_t *spi_configure);


/**
 * @brief     This function registers user's callback in the SPI slave driver. This function may be called when using the SPI slave
 *            driver, the callback will be called in SPI interrupt service routine.
 * @param[in] spi_port is the SPI slave port number, the value is defined at #hal_spi_slave_port_t.
 * @param[in] callback_function is the callback function given by user, which will be called at SPI slave interrupt service routine.
 * @param[in] user_data is a parameter given by user and will pass to user while the callback function is called.
 * @return    #HAL_SPI_SLAVE_STATUS_ERROR_PORT, if the SPI slave port is invalid. \n
 *            #HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user. \n
 *            #HAL_SPI_SLAVE_STATUS_OK, if this function returned successfully.
 */
hal_spi_slave_status_t hal_spi_slave_register_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t callback_function, void *user_data);

#endif

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/
#endif /* __HAL_SPI_SLAVE_H__ */



