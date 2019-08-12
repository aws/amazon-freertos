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

#ifndef __HAL_SDIO_H__
#define __HAL_SDIO_H__
#include "hal_platform.h"

#ifdef HAL_SDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * This section introduces the SDIO driver APIs including terms and acronyms, supported
 * features, software architecture, details on how to use this driver, SDIO function groups,enums, structures and functions.
 *
 * @section HAL_SDIO_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b MSDC                       | MS/SD memory card controller, that supports the MultiMediaCard System Specification Version 4.41,SD Memory Card Specification Version 2.0 and SDIO Card Specification Version 2.0.|
 * |\b SDIO                       | Secure Digital Input and Output.For an introduction to the SDIO, please refer to <a href="https://en.wikipedia.org/wiki/Secure_Digital#SDIO">SDIO in Wikipedia </a>. |
 *
 * @section HAL_SDIO_Features_Chapter Supported features
 *
 * - \b Supports \b DMA \b  and \b MCU \b modes. \n
 *   - \b DMA \b mode: In this mode, the DMA hardware carries data from the RAM to the MSDC FIFO and transmits to the SDIO slave or
 *                 reads data from the MSDC FIFO to the RAM directly.
 *   - \b MCU \b mode: In this mode, the MCU writes data from the RAM to the MSDC FIFO and transmits to the SDIO slave or
 *                 reads data from the MSDC FIFO to the RAM directly.
 *   .
 *   There is one API for the DMA mode, #hal_sdio_execute_command53_dma() to read data from the SDIO card or write data to the card.
 *   Similarly, there is one API for the MCU mode, #hal_sdio_execute_command53() to read data from the SDIO card or write data to the card.
 *   COMMAND53 offers the simplest means to read and write multiple SDIO slave registers. For COMMAND53, SDIO cards may transfer data in either
 *   a multi-byte (1 to 512 bytes) or an optional block format, while the SDIO cards are fixed in the block transfer mode. For more details about the COMMAND53, please
 *   refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.3.\n
 * \n
 * - \b Supports \b 1-bit \b or \b 4-bit \b bus \b width. \n
 *         1-bit bus width or 4-bit bus width is available to select for data transmission. For 1-bit bus width, only data line 0 (SDIO_DAT0) is enabled to transfer data.
 *     The data lines 1-3 (SDIO_DAT[3:1]) cannot be used. All four data lines (SDIO_DAT[3:0]) are available in the 4-bit bus width mode. The corresponding API is #hal_sdio_set_bus_width(). \n
 * \n
 *
 * @section HAL_SDIO_Architecture_Chapter Software architecture of the SDIO
 *
 *    Call #hal_sdio_init() to initialize the MSDC and the SDIO slave. Then the SDIO slave gets into a transfer state.
 * Call #hal_sdio_execute_command53(), #hal_sdio_execute_command53_dma() or #hal_sdio_execute_command52() to write data to the SDIO slave or read data from the SDIO slave.
 * Other supported APIs can also be called, such as #hal_sdio_set_clock(). Use COMMAND52 to access a single SDIO slave register and COMMAND53 to read and write multiple SDIO slave registers.
 * For more details about COMMAND52 and COMMAND53, please refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.1 and Chapter 5.3 .\n \n
 * \n
 *
 *
 * @section HAL_SDIO_Driver_Usage_Chapter How to use this driver
 *
 * - Read from or write to the SDIO slave with COMMAND53 in the MCU mode. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Call #hal_sdio_execute_command53() to read data from or write data to the SDIO card.
 *  - Read sample code:
 *    @code

 *        hal_sdio_command53_config_t config;
 *        ret = hal_sdio_init();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }

 *        config.direction = HAL_SDIO_DIRECTION_READ;
 *        config.function = HAL_SDIO_FUNCTION_0;
 *        config.is_block = true;
 *        config.count = 2;
 *        config.address = address;
 *        config.buffer = buffer;
 *        ret = hal_sdio_execute_command53(HAL_SDIO_PORT_0, &config);
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }
 *    @endcode
 *  - Write sample code:
 *    @code

 *        hal_sdio_command53_config_t config;
 *        ret = hal_sdio_init();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }

 *        config.direction = HAL_SDIO_DIRECTION_WRITE;
 *        config.function = HAL_SDIO_FUNCTION_0;
 *        config.is_block = true;
 *        config.count = 2;
 *        config.address = address;
 *        config.buffer = buffer;
 *        ret = hal_sdio_execute_command53(HAL_SDIO_PORT_0, &config);
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }
 *    @endcode
 * - Read from or write to the SDIO slave in the DMA mode. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Call #hal_sdio_execute_command53_dma() to read data from or write data to the SDIO card.
 *  - Read sample code:
 *      @code

 *        hal_sdio_command53_config_t config;
 *        ret = hal_sdio_init();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *           // Error handler.
 *        }

 *        config.direction = HAL_SDIO_DIRECTION_READ;
 *        config.function = HAL_SDIO_FUNCTION_0;
 *        config.is_block = true;
 *        config.count = 2;
 *        config.address = address;
 *        config.buffer = buffer;
 *        ret = hal_sdio_execute_command53_dma(HAL_SDIO_PORT_0, &config);
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }
 *    @endcode
 *  - Write sample code:
 *      @code

 *       hal_sdio_command53_config_t config;
 *       ret = hal_sdio_init();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *       }

 *       config.direction = HAL_SDIO_DIRECTION_WRITE;
 *       config.function = HAL_SDIO_FUNCTION_0;
 *       config.is_block = true;
 *       config.count = 2;
 *       config.address = address;
 *       config.buffer = buffer;
 *       ret = hal_sdio_execute_command53(HAL_SDIO_PORT_0, &config);
 *       if (HAL_SDIO_STATUS_OK != ret) {
 *           // Error handler.
 *       }
 *    @endcode
 * - Set the SDIO output clock. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Set the SDIO output clock.
 *      @code
 *        ret = hal_sdio_init();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *             // Error handler.
 *        }

 *        ret = hal_sdio_set_clock();
 *        if (HAL_SDIO_STATUS_OK != ret) {
 *            // Error handler.
 *        }
 *    @endcode
 * - Get the SDIO output clock. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Get the SDIO output clock.
 *      @code
 *        ret = hal_sdio_init();
 *          if (HAL_SDIO_STATUS_OK != ret) {
 *              // Error handler.
 *         }

 *          ret = hal_sdio_get_clock();
 *          if (HAL_SDIO_STATUS_OK != ret) {
 *              // Error handler.
 *          }
 *    @endcode
 * - Set the SDIO bus width. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Set the SDIO bus width.
 *      @code
 *         ret = hal_sdio_init();
 *         if (HAL_SDIO_STATUS_OK != ret) {
 *             // Error handler.
 *         }

 *         ret = hal_sdio_set_bus_width();
 *         if (HAL_SDIO_STATUS_OK != ret) {
 *             // Error handler.
 *         }
 *    @endcode
 * - Set the SDIO block size. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Set the SDIO block size.
 *      @code
 *         ret = hal_sdio_init();
 *         if (HAL_SDIO_STATUS_OK != ret) {
 *             // Error handler.
 *         }

 *          ret = hal_sdio_set_block_size();
 *          if (HAL_SDIO_STATUS_OK != ret) {
 *              // Error handler.
 *          }
 *    @endcode
 * - Get the SDIO block size. \n
 *  - Step1: Call #hal_sdio_init() to initialize the MSDC and the SDIO slave to transfer states.
 *  - Step2: Get the SDIO block size.
 *      @code
 *         ret = hal_sdio_init();
 *          if (HAL_SDIO_STATUS_OK != ret) {
 *              // Error handler.
 *         }

 *          ret = hal_sdio_get_block size();
 *          if (HAL_SDIO_STATUS_OK != ret) {
 *              // Error handler.
 *          }
 *    @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_sdio_enum Enum
  * @{
  */

/** @brief  This enum defines the SDIO API return type. */
typedef enum {
    HAL_SDIO_STATUS_BUSY                       = -2,                        /**<  The SDIO bus is busy error occurred. */
    HAL_SDIO_STATUS_ERROR                      = -1,                        /**<  An error occurred and the operation failed. */
    HAL_SDIO_STATUS_OK                         =  0,                        /**<  No error occurred during the function call. */
} hal_sdio_status_t;


/** @brief  This enum define the SDIO bus width types*/
typedef enum {
    HAL_SDIO_BUS_WIDTH_1 = 1,                                        /**< The SDIO bus width is 1 bit. */
    HAL_SDIO_BUS_WIDTH_4 = 2                                         /**< The SDIO bus width is 4 bits. */
} hal_sdio_bus_width_t;


/** @brief   This enum defines the SDIO command direction types*/
typedef enum {
    HAL_SDIO_DIRECTION_READ  = 0,                                               /**< The SDIO read command. */
    HAL_SDIO_DIRECTION_WRITE = 1                                                /**< The SDIO write command. */
} hal_sdio_direction_t;


/** @brief   This enum defines the SDIO command operation types. */
typedef enum {
    HAL_SDIO_FIXED_ADDRESS        = 0,                                           /**< The SDIO multi-byte read/write at a fixed address. */
    HAL_SDIO_INCREMENTING_ADDRESS = 1                                            /**< The SDIO multi-byte read/write at an incrementing address. */
} hal_sdio_command53_operation_t;


/** @brief   This enum defines the SDIO command function number. */
typedef enum {
    HAL_SDIO_FUNCTION_0 = 0,                                              /**< The SDIO function 0, this function selects the common I/O area register to read or write. */
    HAL_SDIO_FUNCTION_1 = 1                                               /**< The SDIO function 1, this function selects the I/O function 1 register to read or write.*/
} hal_sdio_function_id_t;


/**
  * @}
  */

/** @defgroup hal_sd_struct Struct
  * @{
  */


/** @brief   This structure defines the SDIO initialization parameters. */
typedef struct {
    hal_sdio_bus_width_t bus_width;                                    /**< Bus width for the SDIO read/write operation. */
    uint32_t clock;                                                    /**< The SDIO bus clock. */
} hal_sdio_config_t;


/** @brief   This structure defines the SDIO COMMAND53 configuration parameter. For more details about the COMMAND53, please
 *   refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.3.*/
typedef struct {
    hal_sdio_direction_t direction;                                    /**< Read/write direction for the SDIO COMMAND53. */
    hal_sdio_function_id_t function;                                   /**< Function type for the SDIO COMMAND53. */
    bool block;                                                        /**< Indicates whether read/write is in a block mode or not. */
    hal_sdio_command53_operation_t operation;                          /**< Operation mode for the SDIO COMMAND53. */
    uint16_t count;                                                    /**< Byte or block count. */
    uint32_t address;                                                  /**< Read/write address of the SDIO COMMAND53. */
    uint32_t buffer;                                                   /**< Buffer address for data transfer. Must be a non-cacheable and 4 bytes aligned address for the DMA mode. */
} hal_sdio_command53_config_t;


/** @brief   This structure defines the SDIO COMMAND52 configuration parameter. For more details about the COMMAND52, please
 *   refer to <a href="https://www.sdcard.org/downloads/pls/simplified_specs/archive/partE1_200.pdf">SDIO Simplified Specification V2.0 </a> Chapter 5.1. */
typedef struct {
    hal_sdio_direction_t direction;                                    /**< Read/write direction for the SDIO COMMAND52. */
    hal_sdio_function_id_t function;                                   /**< Function type for the SDIO COMMAND52.*/
    bool read_after_write;                                             /**< Indicates whether read is after write or not. */
    bool stop;                                                         /**< Indicates if the data transfer stopped or not. */
    uint8_t data;                                                      /**< Write or read back data.*/
    uint32_t address;                                                  /**< Read/write address of the SDIO COMMAND52. */
} hal_sdio_command52_config_t;

/**
  * @}
  */


/**
 * @brief  This function initializes the MSDC hardware and SDIO slave. It can also be used to set the MSDC pin output clock and bus width.
 * @param[in] sdio_port is the initialization configuration port. For more details about this parameter, please refer to #hal_sdio_port_t.
 * @param[in] sdio_config is the initialization configuration parameter. For more details about this parameter, please refer to #hal_sdio_config_t.
 * @return    Indicates whether this function call is successful or not.
 *            If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an initialization error occurred.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 * @sa  #hal_sdio_deinit()
 */
hal_sdio_status_t hal_sdio_init(hal_sdio_port_t sdio_port, hal_sdio_config_t *sdio_config);


/**
 * @brief     This function deinitializes the MSDC and the SDIO slave settings.
 * @param[in] sdio_port is the MSDC deinitialization port.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 * @sa  #hal_sdio_init()
 */
hal_sdio_status_t hal_sdio_deinit(hal_sdio_port_t sdio_port);


/**
 * @brief     Set the output clock of the MSDC.
 * @param[in] sdio_port is the MSDC port to set clock.
 * @param[in] clock is the expected output clock of the MSDC.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_clock(hal_sdio_port_t sdio_port, uint32_t clock);


/**
 * @brief     Get the output clock of the MSDC.
 * @param[in] sdio_port is the MSDC port to get clock.
 * @param[out] clock is the current output clock of the MSDC.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_status_t hal_sdio_get_clock(hal_sdio_port_t sdio_port, uint32_t *clock);


/**
 * @brief     This function sets the bus widths for the MSDC and SDIO slave.
 * @param[in] sdio_port is the MSDC port to set.
 * @param[in] bus_width is the SDIO bus width.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_bus_width(hal_sdio_port_t sdio_port, hal_sdio_bus_width_t bus_width);


/**
 * @brief     This function sets the transaction block size of the MSDC.
 * @param[in] sdio_port is the MSDC port to set.
 * @param[in] function is the SDIO function to set block size.
 * @param[in] block_size is the SDIO transaction block size.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_set_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t block_size);


/**
 * @brief     This function gets the transaction block size of the MSDC.
 * @param[in] sdio_port is the MSDC port to get blcok size.
 * @param[in] function is the SDIO function to get block size.
 * @param[out] block_size is the SDIO transaction block size.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 */
hal_sdio_status_t hal_sdio_get_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t *block_size);


/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND52.
 * @param[in] sdio_port is the MSDC port to read or write.
 * @param[in] command52_config is the configuration parameter pointer of the COMMAND52.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command52(hal_sdio_port_t sdio_port, hal_sdio_command52_config_t *command52_config);



/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND53 MCU mode.
 * @param[in] sdio_port is the MSDC port to read.
 * @param[in] command53_config is the configuration parameter pointer of the COMMAND53.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command53(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config);


/**
 * @brief     This function reads from or writes to the SDIO slave with COMMAND53 DMA mode. This API may block the application task.
 * @param[in] sdio_port is the MSDC port to read.
 * @param[in] command53_config is the configuration parameter pointer of the COMMAND53.
 * @return    If the return value is #HAL_SDIO_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_SDIO_STATUS_ERROR, an error occurred, such as a wrong parameter is given.
 *            If the return value is #HAL_SDIO_STATUS_BUSY, the MSDC is busy.
 */
hal_sdio_status_t hal_sdio_execute_command53_dma(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config);


#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/
#endif /*HAL_SDIO_MODULE_ENABLED*/
#endif /* __HAL_SDIO_H__ */

