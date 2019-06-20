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

#ifndef __HAL_DISPLAY_LCD_H__
#define __HAL_DISPLAY_LCD_H__

#include "hal_platform.h"

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup DISPLAY_LCD
 * @{
 * This section introduces the LCD APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, LCD function groups, all enumerations, structures and functions.
 * DSI is an optional output interface for the LCD engine. Add the compile option \#define HAL_DISPLAY_DSI_MODULE_ENABLED in hal_feature_config.h to enable the DSI engine.
 *
 * @section DISPLAY_LCD_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                    |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ROI                      | Region of Interest, indicates the region to be transferred, including width and height.
 * |\b DMA                      | Direct Memory Access, the data can be accessed through the LCD engine without CPU. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> Direct memory access </a> in Wikipedia.
 * |\b TE                       | Synchronize signals sent from LCM to LCD engine to prevent tearing effect (TE).
 * |\b DSI                      | The Display Serial Interface (DSI) is a specification by the Mobile Industry Processor Interface (MIPI). For more information, please refer ro <a href="https://en.wikipedia.org/wiki/Display_Serial_Interface"> Display Serial Interface </a> in Wikipedia.
 * |\b MTCMOS                   | Multi-threshold CMOS is a variation of CMOS chip technology that has transistors with multiple threshold voltages (Vth) in order to optimize delay or power. For more information, please refer to<a href="https://en.wikipedia.org/wiki/Multi-threshold_CMOS"> Multi-threshold CMOS </a> in Wikipedia.
 *
 * @section DISPLAY_LCD_Features_Chapter Supported features
 *
 * - \b Support \b DMA \b and \b CPU \b mode. \n
 *   LCD engine can transfer pixel data in two different modes:
 *  - \b DMA \b mode: In this mode, the LCD engine fetches the data from DRAM using DMA
 *      and transfers to the LCM module. After transmission, the LCD engine triggers a transfer complete interrupt. \n
 *  - \b CPU \b mode: In this mode, the LCD engine sends the buffer manually; the user needs to assign each pixel. \n
 *
 * - \b Support \b callback \b function \b registration.
 *     Call #hal_display_lcd_register_callback() to register a callback function when the LCD transfers data in DMA mode.
 *     Once the transfer is complete, the LCD engine triggers an interrupt to execute the callback function.
 *
 * @section DISPLAY_LCD_Driver_Usage_Chapter How to use this driver
 *
 * - Use LCD in DMA mode. \n
 *  - Step1: Call #hal_display_lcd_init() to initialize the LCD engine.
 *  - Step2: Call #hal_display_lcd_config_roi() to set the ROI information.
 *  - Step3: Call #hal_display_lcd_config_layer() to set the layer data.
 *  - Step4: Call blockwrite funtion in LCM driver function table to start the transfer.
 *  - sample code:
 *    @code
 *      hal_display_lcd_roi_output_t roi_para;
 *      hal_display_lcd_layer_input_t layer_para;
 *
 *      ret = hal_display_lcd_init();
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling.
 *      }
 *
 *      // Configure the ROI.
 *      ret = hal_display_lcd_config_roi(&roi_para);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret ){
 *          // Error handling.
 *      }
 *
 *      // Configure the layer.
 *      ret = hal_display_lcd_config_layer(&layer_para);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling.
 *      }
 *
 *      // Update the screen.
 *      MainLCD->BlockWrite(startx, starty, endx, endy);
 *
 *      LCM driver:
 *      LCD_Blockwrite()
 *      {
 *          // Set the ROI data to the LCM.
 *
 *          // Call this function to transfer data in DMA mode. If the TE is connected to MT2523, the parameter is set to 1, otherwise it is set to 0.
 *          hal_display_lcd_start_dma(1);
 *      }
 *    @endcode
 *
 * - Use LCD in CPU mode. \n
 *  - Step1: Write a for loop to transfer pixel data in CPU mode using the blockwrite function of the LCM driver.
 *  - Step2: Call #hal_display_lcd_init() to initialize the LCD engine.
 *  - Step3: Call #hal_display_lcd_config_roi() to set the ROI information.
 *  - Step4: Call #hal_display_lcd_config_layer() to set the layer data.
 *  - Step5: Call blockwrite funtion in LCM driver function table to start transfer.
 *  - sample code:
 *    @code
 *      hal_display_lcd_roi_output_t roi_para;
 *      hal_display_lcd_layer_input_t layer_para;
 *
 *      ret = hal_display_lcd_init();
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *      // Error handling.
 *      }
 *
 *      // Configure the ROI.
 *      ret = hal_display_lcd_config_roi(&roi_para);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling.
 *      }
 *
 *      // Configure the layer.
 *      ret = hal_display_lcd_config_layer(&layer_para);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling.
 *      }
 *
 *      // Update the screen.
 *      MainLCD->BlockWrite(startx, starty, endx, endy);
 *
 *      LCM driver:
 *      LCD_Blockwrite()
 *      {
 *          // Set the ROI data to the LCM.
 *
 *          // Write a for loop to transfer data
 *          for (i = 0; i < height; i++)
 *          {
 *              for (j = 0; j < width; j++)
 *              {
 *                  //Send pixel data.
 *              }
 *          }
 *      }
 *
 *    @endcode
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The layer rotation degree. */
typedef enum {
    HAL_DISPLAY_LCD_LAYER_ROTATE_0 = 0x0,                           /**< 0 degree. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_90 = 0x1,                          /**< 90 degrees. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_180 = 0x2,                         /**< 180 degrees. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_270 = 0x3,                         /**< 270 degrees. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_0_FLIP = 0x4,                      /**< 0 degree rotation and flip. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_90_FLIP = 0x5,                     /**< 90 degrees rotation and flip. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_180_FLIP = 0x6,                    /**< 180 degrees rotation and flip. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_270_FLIP = 0x7,                    /**< 270 degrees rotation and flip. */
    HAL_DISPLAY_LCD_LAYER_ROTATE_NUM                                /**< The number of layer rotation options (invalid layer rotation). */
} hal_display_lcd_layer_rotate_t;

/** @brief The return status of display LCD API. */
typedef enum {
    HAL_DISPLAY_LCD_STATUS_INITIALIZATION_FAILED = -8,                          /**< The LCD initialization failed. */
    HAL_DISPLAY_LCD_STATUS_COLOR_INITIALIZATION_FAILED = -7,                    /**< The color initialization failed. */
    HAL_DISPLAY_LCD_STATUS_DSI_INITIALIZATION_FAILED = -6,                      /**< The DSI initialization failed.*/
    HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER = -5,                           /**< Invalid parameter. */
    HAL_DISPLAY_LCD_STATUS_COLOR_INVALID_PARAMETER = -4,                     /**< Invalid color parameter. */
    HAL_DISPLAY_LCD_STATUS_DSI_INVALID_PARAMETER = -3,                       /**< Invalid DSI parameter. */
    HAL_DISPLAY_LCD_STATUS_POWER_FAILED = -2,                               /**< Failed to apply display power settings. */
    HAL_DISPLAY_LCD_STATUS_ERROR = -1,                                  /**< An error occurred; please check the LCD engine status. */
    HAL_DISPLAY_LCD_STATUS_OK = 0,                                      /**< Successful. */
} hal_display_lcd_status_t;

/** @brief The serial interface width options. */
typedef enum {
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_8 = 0,                            /**< 8 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_9,                                /**< 9 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_16,                               /**< 16 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_18,                               /**< 18 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_24,                               /**< 24 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_32,                               /**< 32 bits. */
    HAL_DISPLAY_LCD_INTERFACE_WIDTH_NUM                               /**< The number of the width selections (invalid width). */
} hal_display_lcd_interface_width_t;

/** @brief The serial interface width options for the 2-data lane mode. */
typedef enum {
    HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_16 = 2,                      /**< 16 bits. */
    HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_18 = 3,                      /**< 18 bits. */
    HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_24 = 4,                      /**< 24 bits. */
    HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_12 = 6,                      /**< 12 bits. */
    HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_NUM                          /**< The number of width selections (invalid width). */
} hal_display_lcd_interface_2pin_width_t;

/** @brief The starting byte width options. */
typedef enum {
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_8 = 0,               /**< 8 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_9,                   /**< 9 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_16,                  /**< 16 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_18,                  /**< 18 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_24,                  /**< 24 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_32,                  /**< 32 bits. */
    HAL_DISPLAY_LCD_INTERFACE_START_BYTE_WIDTH_NUM                  /**< The number of the width selections (invalid width). */
} hal_display_lcd_interface_start_byte_width_t;

/** @brief The input clock speed options for the LCD engine. */
typedef enum {
    HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ = 0,                        /**< 124MHz. */
    HAL_DISPLAY_LCD_INTERFACE_CLOCK_104MHZ = 1,                        /**< 104MHz. */
    HAL_DISPLAY_LCD_INTERFACE_CLOCK_91MHZ = 2,                         /**< 91MHz. */
    HAL_DISPLAY_LCD_INTERFACE_CLOCK_26MHZ = 3,                         /**< 26MHz. */
    HAL_DISPLAY_LCD_INTERFACE_CLOCK_RSVD = 4                           /**< Reserved (invalid clock speed). */
} hal_display_lcd_interface_clock_t;

/** @brief The color formats of the input layer. */
typedef enum {
    HAL_DISPLAY_LCD_LAYER_COLOR_8BPP_INDEX = 0,                     /**< 8 bits per pixel index mode. */
    HAL_DISPLAY_LCD_LAYER_COLOR_RGB565,                             /**< RGB565. */
    HAL_DISPLAY_LCD_LAYER_COLOR_UYVY422,                            /**< YUV422. */
    HAL_DISPLAY_LCD_LAYER_COLOR_RGB888,                             /**< RGB888. */
    HAL_DISPLAY_LCD_LAYER_COLOR_ARGB8888,                           /**< ARGB8888. */
    HAL_DISPLAY_LCD_LAYER_COLOR_PARGB8888,                          /**< PARGB8888. */
    HAL_DISPLAY_LCD_LAYER_COLOR_XRGB,                               /**< XRGB. */
    HAL_DISPLAY_LCD_LAYER_COLOR_ARGB6666,                           /**< ARGB6666. */
    HAL_DISPLAY_LCD_LAYER_COLOR_PARGB6666,                          /**< PARGB6666. */
    HAL_DISPLAY_LCD_LAYER_COLOR_4BIT_INDEX,                         /**< 4 bits per pixel index mode. */
    HAL_DISPLAY_LCD_LAYER_COLOR_2BIT_INDEX,                         /**< 2 bits per pixel index mode. */
    HAL_DISPLAY_LCD_LAYER_COLOR_1BIT_INDEX,                         /**< 1 bit per pixel index mode. */
    HAL_DISPLAY_LCD_LAYER_COLOR_NUM                                 /**< The number of the color formats (invalid color format). */
} hal_display_lcd_layer_source_color_format_t;

/** @brief The output color format to the LCM. */
typedef enum {
    HAL_DISPLAY_LCD_ROI_OUTPUT_RGB332 = 0,                          /**< RGB332. */
    HAL_DISPLAY_LCD_ROI_OUTPUT_RGB444,                              /**< RGB444. */
    HAL_DISPLAY_LCD_ROI_OUTPUT_RGB565,                              /**< RGB565. */
    HAL_DISPLAY_LCD_ROI_OUTPUT_RGB666,                              /**< RGB666. */
    HAL_DISPLAY_LCD_ROI_OUTPUT_RGB888,                              /**< RGB888. */
    HAL_DISPLAY_LCD_ROI_OUTPUT_NUM                                  /**< The number of output color formats (invalid color format). */
} hal_display_lcd_output_color_format_t;

/** @brief The layers. */
typedef enum {
    HAL_DISPLAY_LCD_LAYER0 = 0,                                     /**< Layer 0. */
    HAL_DISPLAY_LCD_LAYER1,                                         /**< Layer 1. */
    HAL_DISPLAY_LCD_LAYER2,                                         /**< Layer 2. */
    HAL_DISPLAY_LCD_LAYER3,                                         /**< Layer 3. */
    HAL_DISPLAY_LCD_LAYER_NUM                                       /**< The number of layers (invalid layer). */
} hal_display_lcd_layer_t;

/** @brief The driving current options. */
typedef enum {
    HAL_DISPLAY_LCD_DRIVING_CURRENT_4MA = 0,                                /**< 4mA. */
    HAL_DISPLAY_LCD_DRIVING_CURRENT_8MA,                                    /**< 8mA. */
    HAL_DISPLAY_LCD_DRIVING_CURRENT_12MA,                                   /**< 12mA. */
    HAL_DISPLAY_LCD_DRIVING_CURRENT_16MA,                                   /**< 16mA. */
    HAL_DISPLAY_LCD_DRIVING_CURRENT_NUM                                     /**< The number of driving current options (invalid driving current). */
} hal_display_lcd_driving_current_t;

/** @brief Power domain voltage options of the output pin. */
typedef enum {
    HAL_DISPLAY_LCD_POWER_DOMAIN_1V8 = 0,                           /**< 1.8V. */
    HAL_DISPLAY_LCD_POWER_DOMAIN_2V8,                               /**< 2.8V. */
    HAL_DISPLAY_LCD_POWER_DOMAIN_NUM                                /**< The number of power domains (invalid power domain). */
} hal_display_lcd_power_domain_t;

/** @brief The output ports of serial interface. */
typedef enum {
    HAL_DISPLAY_LCD_INTERFACE_SERIAL_0 = 0,                                /**< The output port is serial 0. */
    HAL_DISPLAY_LCD_INTERFACE_SERIAL_1,                                    /**< The output port is serial 1. */
    HAL_DISPLAY_LCD_INTERFACE_NUM                                          /**< The number of output ports (invalid output ports). */
} hal_display_lcd_interface_port_t;


/** @defgroup hal_display_lcd_struct Struct
  * @{
  */

/** @brief This event structure presents the input layer parameters.
 *
 *  The coordinate relationship between different layers is shown below:
 *  @image html hal_display_lcd_layer_coordinate.png
 *
 *  The pixel data format with RGB color swap and byte swap is shown below:
 *  @image html hal_display_lcd_RGB_BYTE_swap.png
 */
typedef struct {
    uint8_t                             layer_enable;               /**< The number of the layer to be updated. */
    uint8_t                             alpha;                      /**< Configure alpha blending. */
    bool                                alpha_flag;                 /**< Enable or disable the alpha blending. */
    hal_display_lcd_layer_rotate_t      rotate;                     /**< Layer rotation degree. */
    bool                                source_key_flag;            /**< Enable or disable the source key. */
    bool                                rgb_swap_flag;              /**< Enable or disable the RGB swap. */
    bool                                byte_swap_flag;             /**< Enable or disable the byte swap. */
    bool                                dither_flag;                /**< Enable or disable the dithering function. */
    hal_display_lcd_layer_source_color_format_t     color_format;   /**< Set the color format in this layer. */
    bool                                destination_key_flag;       /**< Enable or disable the destination key. */
    uint32_t                            color_key;                  /**< Set the color key in this layer. */
    uint16_t                            window_x_offset;            /**< The starting x coordinate of the layer buffer to update. */
    uint16_t                            window_y_offset;            /**< The starting y coordinate of the layer buffer to update. */
    uint32_t                            buffer_address;             /**< The start address of the layer to update. */
    uint16_t                            row_size;                   /**< The row size of output layer to update. */
    uint16_t                            column_size;                /**< The column size of output layer to update. */
    uint16_t                            pitch;                      /**< The pitch of output layer to update. */
} hal_display_lcd_layer_input_t;

/** @brief This event structure presents the output ROI parameters.
 *
 *  The ROI coordinates are shown below:
 *  @image html hal_display_lcd_roi_coordinate.png
 *
 */
typedef struct {
    uint16_t    target_start_x;             /**< The starting x coordinate of the LCM to update. */
    uint16_t    target_start_y;             /**< The starting y coordinate of the LCM to update. */
    uint16_t    target_end_x;               /**< The ending x coordinate of the LCM to update. */
    uint16_t    target_end_y;               /**< The ending y coordinate of the LCM to update. */
    uint16_t    roi_offset_x;               /**< The ROI window offset x from main window. */
    uint16_t    roi_offset_y;               /**< The ROI window offset y from main window. */
    uint32_t    roi_background_color;       /**< The background color of the ROI to update. */
    uint32_t    main_lcd_output;            /**< The output format of the LCD engine to use. */
} hal_display_lcd_roi_output_t;

/** @brief This event structure presents the timing parameters of the serial interface . */
typedef struct {
    hal_display_lcd_interface_port_t    port_number;    /**< The port number of serial interface. */
    uint32_t                css;                        /**< Set the width of chip select setup time. */
    uint32_t                csh;                        /**< Set the width of chip select hold time. */
    uint32_t                rd_low;                     /**< Set the low width of the output clock while receiving data. */
    uint32_t                rd_high;                     /**< Set the high width of the output clock while receiving data. */
    uint32_t                wr_low;                     /**< Set the low width of the output clock while sending data. */
    uint32_t                wr_high;                     /**< Set the high width of the output clock while sending data. */
    hal_display_lcd_interface_clock_t   clock_freq;     /**< The frequency of input clock to the LCD engine to set. */
} hal_display_lcd_interface_timing_t;

/** @brief This event structure presents the parameters of the serial interface. */
typedef struct {
    hal_display_lcd_interface_port_t    port_number;                     /**< The port number of the serial interface. */
    bool                                three_wire_mode;                 /**< Enable or disable 3 wire mode. */
    bool                                single_a0_mode;                  /**< Enable or disable single A0 mode. */
    bool                                cs_stay_low_mode;                /**< Enable or disable chip select stay low mode. */
    bool                                read_from_SDI;                   /**< Read from the SDI or not. */
    hal_display_lcd_interface_width_t   width;                           /**< Serial interface width. */
    hal_display_lcd_driving_current_t   driving_current;                 /**< The driving current. */
    bool                                hw_cs;                           /**< Enable or disable hardware(auto) chip select. */
    hal_display_lcd_power_domain_t      power_domain;                    /**< The output pin's power domain. */
    bool                                start_byte_mode;                 /**< Enable or disable start byte mode. */
} hal_display_lcd_interface_mode_t;

/** @brief This event structure presents the start byte mode parameters of the serial interface. */
typedef struct {
    hal_display_lcd_interface_port_t                port_number;                /**< The port number of the serial interface. */
    hal_display_lcd_interface_start_byte_width_t    start_byte_width;           /**< The start byte width. */
    uint32_t                                        byte_switch;                /**< The start byte mode. */
    uint32_t                                        wr_start_byte;              /**< The first start byte setting in sending data/command to the LCM. */
    uint32_t                                        wr_start_byte2;             /**< The second start byte setting in sending data/command to the LCM. */
    uint32_t                                        rd_start_byte;              /**< The start byte setting in receiving data from the LCM. */
} hal_display_lcd_interface_start_byte_mode_t;

/** @brief This event structure presents the 2-data lane mode parameters of the serial interface. */
typedef struct {
    hal_display_lcd_interface_port_t            port_number;        /**< The port number of the serial interface. */
    bool                                        two_data_flag;      /**< Enable or disable 2-data lane. */
    hal_display_lcd_interface_2pin_width_t      two_data_width;     /**< The output pixel width in 2-data lane mode. */
} hal_display_lcd_interface_2_data_lane_t;

/**
  * @}
  */

/** @defgroup hal_display_lcd_typedef Typedef
* @{
*/

/** @brief This function defines the prototype of the callback function registered by #hal_display_lcd_register_callback() when using DMA mode.
* Call this function after an interrupt is triggered by the received TE signal or the LCD DMA transfer is complete.
* For more details about the callback function, please refer to #hal_display_lcd_register_callback().
* @param[in] data is a pointer to the input data.
*/

typedef void (*hal_display_lcd_callback_t)(void *data);

/**
  * @}
*/

/**
 * @brief This function initializes the LCD engine's hardware registers. It sets the default values of the LCD engine.
 *
 * @param[in] main_command_address is the register address to be written when sending a command to the LCM.
 * @param[in] main_data_address is the register address to be written when sending data to the LCM.
 * @param[in] main_lcd_output_format is the output color format that the LCD engine sends to the LCM.
 *
 * @return    To indicate whether this function call is successful or not.
 *            #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *            #HAL_DISPLAY_LCD_STATUS_INITIALIZATION_FAILED, the LCD engine's registers failed to initialize. Call #hal_display_lcd_turn_on_mtcmos() before initializing the LCD engine's registers.
 *            #HAL_DISPLAY_LCD_STATUS_COLOR_INITIALIZATION_FAILED, the color engine's registers failed to initialize. Call #hal_display_lcd_turn_on_mtcmos() before initializing the color engine's registers.
 */
hal_display_lcd_status_t hal_display_lcd_init(uint32_t main_command_address, uint32_t main_data_address, uint32_t main_lcd_output_format);
/**
 * @brief This function sets the timing configuration to the LCD engine.
 *
 * @param[in] timing_settings are the timing parameters, please refer to #hal_display_lcd_interface_timing_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_set_interface_timing(hal_display_lcd_interface_timing_t timing_settings);

/**
 * @brief This function sets the serial interface mode configuration to the LCD engine.
 *
 * @param[in] mode_settings are the interface parameters, please refer to #hal_display_lcd_interface_mode_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_set_interface_mode(hal_display_lcd_interface_mode_t mode_settings);

/**
 * @brief This function sets the serial interface 2-data lane mode configuration to the LCD engine.
 *
 * @param[in] port is the serial port to be configured.
 * @param[in] flag specifies if 2-data lane is enabled or not.
 * @param[in] two_data_width is the data width in 2-data lane mode, please refer to #hal_display_lcd_interface_2pin_width_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_set_interface_2data_lane_mode(hal_display_lcd_interface_port_t port, bool flag, hal_display_lcd_interface_2pin_width_t two_data_width);

/**
 * @brief This function sets the layer information to the LCD engine.
 *
 * @param[in] layer_data is the layer information, please refer to #hal_display_lcd_layer_input_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data);

/**
 * @brief This function sets the ROI settings to the LCD engine.
 *
 * @param[in] roi_settings are the roi settings, please refer to #hal_display_lcd_roi_output_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_config_roi(hal_display_lcd_roi_output_t *roi_settings);

/**
 * @brief This function sets the start byte mode's settings to the LCD engine.
 *
 * @param[in] start_byte_settings are the start byte settings, please refer to #hal_display_lcd_interface_start_byte_mode_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_config_start_byte(hal_display_lcd_interface_start_byte_mode_t *start_byte_settings);

/**
 * @brief This function sets the index color table settings to the LCD engine.
 *
 * @param[in] index_table specifies the color mapping table.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_set_index_color_table(uint32_t *index_table);

/**
 * @brief This function sets the callback function when the LCD DMA transfer is complete.
 *
 * @param[in] lcd_callback represents the callback function.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_callback_t hal_display_lcd_register_callback(hal_display_lcd_callback_t lcd_callback);

/**
 * @brief This function sets the TE settings to the LCD engine.
 *
 * @param[in] frame_rate is the frame rate of the LCM self-refresh.
 * @param[in] back_porch is the line number of the back porch.
 * @param[in] front_porch is the line number of the front porch.
 * @param[in] width is the width of the LCM.
 * @param[in] height is the height of the LCM.
 * @param[in] main_lcd_output is the LCD output format, please refer to #hal_display_lcd_output_color_format_t.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_init_te(uint32_t frame_rate, uint32_t back_porch, uint32_t front_porch, uint32_t width, uint32_t height, uint32_t main_lcd_output);

/**
 * @brief This function calculates the delay lines and sets the delay lines to the LCD engine.
 *
 * @param[in] width is the width of the LCM.
 * @param[in] height is the height of the LCM.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_calculate_te(uint32_t width, uint32_t height);

/**
 * @brief This function produces a reset waveform and reset the LCM.
 *
 * @param[in] low_time specifies the duration of reset pin keeps in low (ms).
 * @param[in] high_time specifies the duration of reset pin keeps in high (ms).
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_toggle_reset(uint32_t low_time, uint32_t high_time);

/**
 * @brief This function starts the LCD engine transfer in DMA mode.
 *
 * @param[in] wait_te specifies whether the LCD engine has to wait for the TE signals to start the DMA transfer. If the TE is connected to MT2523, the parameter is set to 1, otherwise it is set to 0.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_start_dma(bool wait_te);

/**
 * @brief This function gets the address to the layer buffer.
 *
 * @param[in] layer is the layer to return the address for.
 *
 * @return    The address to the specific layer.
 */
uint32_t hal_display_lcd_get_layer_address(hal_display_lcd_layer_t layer);

/**
 * @brief This function executes the restore functions when exiting from deep sleep.
 *
 * @return    None.
 */
void hal_display_lcd_restore_callback(void);

/**
 * @brief This function turns on the MTCMOS. To save power, no need to turn the MTCMOS on.
 *        During the screen update, turn the MTCMOS on and then apply all settings to the LCD engine to ensure the parameters are set to the LCD engine.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_POWER_FAILED, turning on the LCD engine's power has failed.
 */
hal_display_lcd_status_t hal_display_lcd_turn_on_mtcmos(void);

/**
 * @brief This function turns off the MTCMOS. To save power, turn the MTCMOS off after the LCD engine completes the transfer.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_POWER_FAILED, turning off the LCD engine's power has failed.
 */
hal_display_lcd_status_t hal_display_lcd_turn_off_mtcmos(void);

/**
 * @brief This function sets the input clock of the LCD engine.
 *
 * @param[in] clock specifies the speed of input clock of the LCD engine.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER, the parameter needs to be verified. The parameter is out of bound.
 */
hal_display_lcd_status_t hal_display_lcd_set_clock(hal_display_lcd_interface_clock_t clock);

/**
 * @brief This function applies all settings to the LCD engine. To save power, keep the MTCMOS mostly turned off.
 *        During the screen update, turn the MTCMOS on and then apply all settings to the LCD engine to ensure the parameters are set to the LCD engine.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_ERROR, the operation has failed.
 */
hal_display_lcd_status_t hal_display_lcd_apply_setting(void);

/**
 * @brief This function resets the layer to default settings after updating the screen.
 *
 * @return  To indicate whether this function call is successful or not.
 *          #HAL_DISPLAY_LCD_STATUS_OK, the operation completed successfully.
 *          #HAL_DISPLAY_LCD_STATUS_ERROR, the operation has failed.
 */
hal_display_lcd_status_t hal_display_lcd_set_layer_to_default(void);
#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_DISPLAY_LCD_MODULE_ENABLED */
#endif /* __HAL_DISPLAY_LCD_H__ */
