/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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

#include "hal_platform.h"

#ifndef __HAL_DISPLAY_DSI_H__
#define __HAL_DISPLAY_DSI_H__

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup DISPLAY_DSI
 * @{
 * This section introduces the display DSI APIs including terms and acronyms,
 * supported features, software architecture, how to use the APIs, DSI function groups, all enumerations, structures and functions.
 * DSI is an optional output interface for the LCD engine. Add the compile option \#define HAL_DISPLAY_LCD_MODULE_ENABLED in hal_feature_config.h to enable the LCD engine.
 *
 * @section DISPLAY_DSI_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details																  |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DSI                      | The Display Serial Interface is a specification by the Mobile Industry Processor Interface (MIPI). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Display_Serial_Interface"> Display Serial Interface </a> in Wikipedia.
 * |\b BTA                      | The first data lane (lane 0) that travels from the DSI host to the DSI device is capable of a bus turnaround (BTA) operation that allows it to reverse transmission direction.
 * |\b LP                       | Low power mode. In low power mode, the high speed clock is disabled and signal clocking information is embedded in the data. It is applied to send configuration information and commands.
 * |\b HS                       | High-speed mode. This mode enables the high-speed clock that acts as the bit clock for the data lanes. HS mode is still designed to reduce power usage due to its low voltage signaling and parallel transfer ability.
 * |\b TE                       | Synchronize signals sent from LCM to LCD engine to prevent tearing effect.
 * |\b ULPS                     | Ultra Low Power State. The voltage of data lane and clock lane is 0 for power saving operations.
 * |\b Spread spectrum          | Spread spectrum is the method by which a signal generated with a particular bandwidth is deliberately spread in the frequency domain, resulting in a signal with a wider bandwidth. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Spread_spectrum"> Spread spectrum </a> in Wikipedia.
 * |\b D-PHY                    | A flexible, low-cost, high speed serial interface solution for communication interconnection between components inside a mobile device that adheres to MIPI Alliance interface specifications.
 * |\b PLL                      | A phase-locked loop or phase lock loop is a control system that generates an output signal to match to the input signal's phase. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Phase-locked_loop"> Phase-locked loop </a> in Wikipedia.
 * |\b ROI                      | Region of Interest, the region will be transferred, including width and height.
 * |\b MTCMOS                   | Multi-threshold CMOS is a variation of CMOS chip technology that has transistors with multiple threshold voltages (Vth) in order to optimize delay or power. For more information, please refer to<a href="https://en.wikipedia.org/wiki/Multi-threshold_CMOS"> Multi-threshold CMOS </a> in Wikipedia.
 *
 * @section DISPLAY_DSI_Features_Chapter Supported features
 *
 * - \b Command \b mode \b transfer.\n
 *   Send frame buffer to the LCM manually in command mode. Command mode saves transfer power and memory bandwidth.
 *
 * - \b External \b TE \b and \b Internal \b TE.\n
 *  - External TE: Connect to the TE pin from LCM as input signal for MT2523.
 *  - Internal TE: No need to connect to the TE pin from LCM as input signal for MT2523. MT2523 can get TE signals using BTA command.
 *
 * @section DISPLAY_DSI_Driver_Usage_Chapter How to use this driver
 *  1. Call #hal_display_lcd_init() and #hal_display_dsi_init() to initialize the LCD and DSI hardware.
 *  2. Call #hal_display_lcd_config_roi() to set the ROI information.
 *  3. Call #hal_display_lcd_config_layer() to set the layer data.
 *  4. Call blockwrite function from the LCM driver function table to start the transfer.
 *  - sample code:
 *    @code
 *      hal_display_lcd_roi_output_t roi_para;
 *      hal_display_lcd_layer_input_t layer_para;
 *
 *      ret = hal_display_lcd_init();
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling
 *      }
 *
 *      ret = hal_display_dsi_init();
 *      if (HAL_DISPLAY_DSI_STATUS_OK != ret) {
 *          // Error handling
 *      }
 *
 *      // Configure the ROI.
 *      ret = hal_display_lcd_config_roi(&roi_para);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling
 *      }
 *
 *      // Configure the layer.
 *      ret = hal_display_lcd_config_layer(&layer_data);
 *      if (HAL_DISPLAY_LCD_STATUS_OK != ret) {
 *          // Error handling
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
 *          // Add this to start DMA mode.
 *          hal_display_lcd_toggle_DMA();
 *
 *          data_array[0] = 0x002C3909;
 *          hal_display_dsi_set_command_queue(data_array, 1, 1);
 *      }
 *    @endcode
 *
*/

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_display_dsi_enum Enum
  * @{
  */
/** @brief The return status of DSI API. */
typedef enum {
    HAL_DISPLAY_DSI_STATUS_INITIALIZATION_FAILED = -6,      /**< The DSI initialization has failed. */
    HAL_DISPLAY_DSI_STATUS_POWER_FAILED = -5,               /**< The DSI power configuration has failed. */
    HAL_DISPLAY_DSI_STATUS_COMMAND_QUEUE_FAILED = -4,       /**< Unable to complete sending data using a command queue. */
    HAL_DISPLAY_DSI_STATUS_INVALID_PARAMETER = -3,          /**< Invalid parameter. */
    HAL_DISPLAY_DSI_STATUS_READ_FAILED = -2,                /**< Unable to receive data transfer from the LCM driver IC. */
    HAL_DISPLAY_DSI_STATUS_ERROR = -1,                      /**< An error occurred; please check the DSI engine status. */
    HAL_DISPLAY_DSI_STATUS_OK = 0,                          /**< The DSI operation completed successfully. */
} hal_display_dsi_status_t;

/** @brief The DSI transfer mode selections. */
typedef enum {
    HAL_DISPLAY_DSI_TRANSFER_MODE_LP = 0,               /**< The DSI always transfers in low power mode. */
    HAL_DISPLAY_DSI_TRANSFER_MODE_HS,                   /**< The DSI always transfers in HS mode. */
    HAL_DISPLAY_DSI_TRANSFER_MODE_AUTO,                 /**< The DSI will switch to low power / HS mode automatically according to the command type. */
    HAL_DISPLAY_DSI_TRANSFER_MODE_NUM                   /**< The number of DSI transfer modes (invalid mode). */
} hal_display_dsi_transfer_mode_t;

/**
  * @}
  */

/** @defgroup hal_display_dsi_struct Struct
  * @{
  */

/** @brief This event structure presents the DSI D-PHY timing setting structure, as shown below.
 *  @image html hal_display_dsi_DPHY_timing_1.png
 *  @image html hal_display_dsi_DPHY_timing_2.png
 */
typedef struct {
    uint32_t    da_hs_trail;                /**< Time that the transmitter drives the flipped differential state after last payload data bit of HS transmission burst. */
    uint32_t    da_hs_zero;                 /**< Time that the transmitter drives the HS-0 state prior to transmitting the Sync sequence. */
    uint32_t    da_hs_prep;                 /**< Time that the transmitter drives the Data Lane LP-00 Line state immediately before the HS-0 Line state starts the HS transmission. */
    uint32_t    lpx;                        /**< Transmitted length of any Low-Power state period to display module. */
    uint32_t    da_hs_exit;                 /**< Time that the transmitter drives LP-11 following an HS burst. */
    uint32_t    ta_get;                     /**< Time that the MCU drives the Bridge state (LP-00) after accepting control during a Link Turnaround. */
    uint32_t    ta_sure;                    /**< Time that the MCU waits after the LP-10 state before transmitting the Bridge state (LP-00) during a Link Turnaround. */
    uint32_t    ta_go;                      /**< Time that the display module drives the Bridge state (LP-00) before releasing control during a Link Turnaround. */
    uint32_t    clk_hs_trail;               /**< Time that the transmitter drives the HS-0 state after the last payload clock bit of HS transmission burst. */
    uint32_t    clk_hs_zero;                /**< Time that the transmitter drives the HS-0 state prior to starting the Clock. */
    uint32_t    clk_hs_post;                /**< Time that the transmitter continues to send HS clock after the last associated Data Lane has transitioned to LP Mode. */
    uint32_t    clk_hs_prep;                /**< Time that the HS clock shall be driven by the transmitter prior to any associated Data Lane beginning the transition from LP to HS mode. */
} hal_display_dsi_dphy_timing_struct_t;

/**
  * @}
  */

/**
 * @brief This function initializes the DSI engine's hardware registers. It sets the default transfer bit rate to 300Mbps.
 *
 * @param[in] ssc_enable specifies whether the spread spectrum is enabled.
 *
 * @return      To indicate whether this function call is successful or not.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_INITIALIZATION_FAILED, the DSI engine register initialization has failed. Call #hal_display_dsi_power_on() before setting the DSI engine registers.
 */
hal_display_dsi_status_t hal_display_dsi_init(bool ssc_enable);

/**
 * @brief This function de-initializes the DSI engine's hardware registers. It disables the DSI engine.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation to de-initialize the DSI engine has failed.
 */hal_display_dsi_status_t hal_display_dsi_deinit(void);

/**
  * @brief This function enables the DSI engine power. Before accessing the registers of the DSI engine or transferring data in DMA mode, please turn the power on.
  *
  * @return     To indicate whether this function call is successful or not.
  *             #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
  *             #HAL_DISPLAY_DSI_STATUS_POWER_FAILED, the operation to enable the power has failed.
  */
hal_display_dsi_status_t hal_display_dsi_power_on(void);

/**
 * @brief This function disables DSI engine power. After accessing registers of the DSI engine or transferring data in DMA mode, please turn the power off to avoid power leakage.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_POWER_FAILED, the operation to disable the power has failed.
 */
hal_display_dsi_status_t hal_display_dsi_power_off(void);

/**
 * @brief This function enables the TE detection of DSI engine. If the TE pin is connected to MT2523, the TE detection should be enabled to avoid a tearing-effect.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_enable_te(void);

/**
 * @brief This function sets commands/data into command queue to send to the LCM driver IC.
 *
 * @param[in] pData is an array with commands and data.
 * @param[in] size is the size of the input array.
 * @param[in] forced_update specifies whether to send data to LCM driver IC immediately.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_COMMAND_QUEUE_FAILED, the operation has failed, please check the input data.
 */
hal_display_dsi_status_t hal_display_dsi_set_command_queue(uint32_t *pData, uint8_t size, bool forced_update);

/**
 * @brief This function sets commands/data into command queue to send to the LCM driver IC.
 *
 * @param[in] command is the command to send.
 * @param[in] size is the size of the data array.
 * @param[in] data is an array with data that should be sent based on the send command.
 * @param[in] forced_update specifies whether to send data to LCM driver IC immediately.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_COMMAND_QUEUE_FAILED, the operation has failed, please check the input data.
 */
hal_display_dsi_status_t hal_display_dsi_set_command_queue_ex(uint32_t command, uint32_t size, uint32_t *data, bool forced_update);

/**
 * @brief This function reads data from the LCM driver IC.
 *
 * @param[in] command is the read command to send.
 * @param[in] buffer is an array to store the data sent from the LCM driver IC.
 * @param[in] size is the number of bytes to read.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_READ_FAILED, the operation has failed, please check the input data.
 */
hal_display_dsi_status_t hal_display_dsi_read_register(uint8_t command, uint8_t *buffer, uint8_t size);

/**
 * @brief This function configures the DSI engine to enter the ULPS state.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_enter_ulps(void);

/**
 * @brief This function configures the DSI engine to exit the ULPS state.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_exit_ulps(void);

/**
 * @brief This function configures the DSI D-PHY timing.
 *
 * @param[in] timing specifies the DSI timing parameters, please refer to #hal_display_dsi_dphy_timing_struct_t.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_set_dphy_timing(hal_display_dsi_dphy_timing_struct_t *timing);

/**
 * @brief This function configures the DSI D-PHY timing automatically using the PLL clock speed.
 *
 * @param[in] pll_clock is the PLL clock speed in MHz.
 * @param[in] ssc_enable specifies if the spread spectrum clock is enabled or not.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_set_clock(uint32_t pll_clock, bool ssc_enable);

/**
 * @brief This function configures the transfer mode.
 *
 * @param[in] mode is the transfer mode, please refer to #hal_display_dsi_transfer_mode_t.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_set_transfer_mode(hal_display_dsi_transfer_mode_t mode);

/**
 * @deprecated Please use #hal_display_dsi_start_bta_transfer() instead.
 *
 * This function starts the BTA type transfer. Add this function at the end of LCM blockwrite function to start transferring data and to use the internal TE.
 *
 * @param[in] command specifies the memory write command.
 *
 * @return    None.
 */
void hal_dsiplay_dsi_start_bta_transfer(uint32_t command);

/**
 * @brief This function starts the BTA type transfer. Add this function at the end of LCM blockwrite function to start transferring data and to use the internal TE.
 *
 * @param[in] command specifies the memory write command.
 *
 * @return    None.
 */
void hal_display_dsi_start_bta_transfer(uint32_t command);


/**
 * @brief This function executes the restore function when exiting from deep sleep.
 *
 * @return    None.
 */
void hal_display_dsi_restore_callback(void);

/**
 * @brief This function applies all settings to the DSI engine. To save power, keep the MTCMOS mostly turned off.
 *        During the screen update, turn the MTCMOS on and then apply the settings to the DSI engine to ensure complete configuration.
 *
 * @return      Indicates whether this function call is successful.
 *              #HAL_DISPLAY_DSI_STATUS_OK, the operation completed successfully.
 *              #HAL_DISPLAY_DSI_STATUS_ERROR, the operation has failed.
 */
hal_display_dsi_status_t hal_display_dsi_apply_setting(void);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_DISPALY_DSI_MODULE_ENABLED */
#endif /* __HAL_DISPLAY_DSI_H__ */

