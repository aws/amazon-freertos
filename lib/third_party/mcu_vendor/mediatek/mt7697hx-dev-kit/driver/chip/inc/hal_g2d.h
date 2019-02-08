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

#ifndef __HAL_G2D_H__
#define __HAL_G2D_H__

#include "hal_platform.h"

#ifdef HAL_G2D_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup G2D
 * @{
 * This section introduces the Graphics 2D (G2D) driver APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, G2D Manager function groups, enums, structures and functions.
 *
 * @section HAL_G2D_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b G2D                        | The Graphics 2D Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/2D_computer_graphics"> 2D computer graphics </a> in Wikipedia.|
 *
 * @section HAL_G2D_Features_Chapter Supported features
 *
 * - Support Rotation, Rectfill and Overlay APIs.\n
 *   The G2D hardware supports the following graphic algorithms.
 *   - \b Overlay \b : The hardware uses Overlay algorithm to overlay 2D layers.
 *   - \b Rotation \b : The hardware uses Rotation algorithm to rotate the layer by 90, 180, 270 degrees.
 *   - \b Rectfill \b : The hardware uses Rectfill algorithm to fill color to rectangle.
 *
 * @section HAL_G2D_Driver_Usage_Chapter How to use this driver
 * \subsection Overlay Overlay:
 * Support four layer overlay. Layer 0 is the bottom layer while layer 3 is the top layer. The G2D engine applies the overlay from the lowest (bottom) layer to highest (top) layer.
 * - The G2D engine's overlay sequence:\n
 *   Do background + layer0 (Get layer0').\n
 *   If alpha is enabled on layer 0, apply alpha blending formula according to the color format of layer 0.\n
 *   Do layer0 + layer1 first (results in layer1').\n
 *   If alpha is enabled on layer 1, apply alpha blending formula according to the color format of layer 1.\n
 *   Do layer1' + layer2 next (results in layer2').\n
 *   If alpha is enabled on layer 2, apply alpha blending formula according to the color format of layer 2.\n
 *   Do layer2' + layer3 next, and get result.\n
 *   If alpha is enabled on layer 3, apply alpha blending formula according to the color format of layer 3.\n
 *
 * - Related functions\n
 *   The configuration functions for layer 0 to 3 are listed below:\n
 *   #hal_g2d_overlay_set_layer_function()\n
 *   #hal_g2d_overlay_set_layer_rgb_buffer()\n
 *   #hal_g2d_overlay_set_layer_window()\n
 *   #hal_g2d_overlay_set_layer_source_key()\n
 *   #hal_g2d_overlay_set_layer_alpha()\n
 *   #hal_g2d_overlay_enable_layer()\n
 *   Once the layer is configured, call the function #hal_g2d_overlay_start() to start the G2D engine. Layers 0 to 3 will overlay to destination buffer.\n
 *
 * \subsection Rotation Rotation:
 * The rotation function supports seven options, including rotation by 90, 180 and 270 degrees, mirror, mirror and rotate by 90 degrees,\n
 * mirror and rotate by 180 degrees, mirror and rotate by 270 degrees. The rotation direction is clockwise.\n
 * - Related functions\n
 *   Call the function #hal_g2d_overlay_set_layer_rotation() to set the rotation angle, and call the function #hal_g2d_overlay_start() to start G2D engine.\n
 *
 * \subsection Rectfill Rectfill:
 * Rectangle fill enables to fill the user-defined color into a rectangle.\n
 * - Related functions\n
 *   Call the function #hal_g2d_overlay_set_layer_function() with #HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL for a layer,
 *   and call the function #hal_g2d_overlay_set_layer_window() to set the rectangle area,
 *   then call the function #hal_g2d_overlay_set_layer_rectfill_color() to set the filling color.\n
 *   After the settings are applied, call the function #hal_g2d_overlay_enable_layer() to enable the layer.\n
 *   Start G2D engine by using #hal_g2d_overlay_start().\n
 *
 * @section HAL_G2D_Driver_Example_Chapter How to use this driver
 * - The steps to apply graphics in your application development using G2D API are described below:\n
 *   - Step1: Get G2D handle by #hal_g2d_get_handle().
 *   - Step2: Set the parameters, destination buffer such as window.
 *   - Step3: Call the start function #hal_g2d_overlay_start().
 *   - Step4: Call the function #hal_g2d_get_status() to get the G2D handle's status.
 *   - Step5: Call the function #hal_g2d_release_handle() to release the G2D handle.
 *
 *   - Sample code: (The full source code can be found at project\mt2523_hdk\hal_examples\g2d_overlay_layer\src\main.c.)
 *     @code
 *     hal_g2d_handle_t *g2dHandle;
 *     hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_BLOCKING);
 *     hal_g2d_set_rgb_buffer(g2dHandle, output_buffer, 320*320*3, 320, 320, HAL_G2D_COLOR_FORMAT_RGB888);
 *     hal_g2d_overlay_set_roi_window(g2dHandle, 0, 0, 320, 320);
 *     hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
 *     hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0xFF);
 *     hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, input_buffer, 320*320*3, 320, 320, HAL_G2D_COLOR_FORMAT_RGB888);
 *     hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0, 0, 320, 320);
 *     hal_g2d_overlay_set_layer_source_key(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0x000000);
 *     hal_g2d_overlay_set_layer_rotation(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_ROTATE_ANGLE_0);
 *     hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
 *     hal_g2d_overlay_start(g2dHandle);
 *     hal_g2d_release_handle(g2dHandle);
 *     @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_g2d_enum Enum
 *  @{
 */

/** @brief This enum defines the return status of the G2D API. */
typedef enum {
    HAL_G2D_STATUS_HW_ERROR = -6,                     /**< Hardware error. */
    HAL_G2D_STATUS_NOT_SUPPORTEDED_COLOR_FORMAT = -5, /**< Unsupported color format. */
    HAL_G2D_STATUS_INVALID_PARAMETER = -4,            /**< Invalid parameter, such as failed on the range check. */
    HAL_G2D_STATUS_NOT_SUPPORTED = -3,                /**< The G2D function is not supported. */
    HAL_G2D_STATUS_BUSY = 0,                          /**< The G2D hardware is busy. */
    HAL_G2D_STATUS_OK = 1                             /**< Successful. */
} hal_g2d_status_t;

/** @brief This enum defines the G2D codec type. */
typedef enum {
    HAL_G2D_CODEC_TYPE_HW = 0 /**< Hardware codec. */
} hal_g2d_codec_type_t;

/** @brief This enum defines the G2D get handle mode. */
typedef enum {
    HAL_G2D_GET_HANDLE_MODE_BLOCKING = 0, /**< Blocking mode. */
    HAL_G2D_GET_HANDLE_MODE_NON_BLOCKING  /**< Non Blocking mode. */
} hal_g2d_get_handle_mode_t;

/** @brief This enum defines the G2D color format. */
typedef enum {
    HAL_G2D_COLOR_FORMAT_RGB565 = 0, /**< RGB565. */
    HAL_G2D_COLOR_FORMAT_RGB888,     /**< RGB888. */
    HAL_G2D_COLOR_FORMAT_BGR888,     /**< BGR888. */
    HAL_G2D_COLOR_FORMAT_ARGB8565,   /**< ARGB8565. */
    HAL_G2D_COLOR_FORMAT_ARGB6666,   /**< ARGB6666. */
    HAL_G2D_COLOR_FORMAT_ARGB8888,   /**< ARGB8888. */
    HAL_G2D_COLOR_FORMAT_PARGB8565,  /**< PARGB8565. */
    HAL_G2D_COLOR_FORMAT_PARGB6666,  /**< PARGB6666. */
    HAL_G2D_COLOR_FORMAT_PARGB8888,  /**< PARGB8888. */
    HAL_G2D_COLOR_FORMAT_UYVY        /**< UYVY. */
} hal_g2d_color_format_t;

/** @brief This enum defines the G2D supported functions. */
typedef enum {
    HAL_G2D_SUPPORTED_FUNCTION_BITBLT = 0, /**< Bitblt function. */
    HAL_G2D_SUPPORTED_FUNCTION_RECTFILL,   /**< Rectfill function. */
    HAL_G2D_SUPPORTED_FUNCTION_FONT,       /**< Font function. */
    HAL_G2D_SUPPORTED_FUNCTION_OVERLAY,    /**< Overlay function. */
    HAL_G2D_SUPPORTED_FUNCTION_DITHERING   /**< Dithering function. */
} hal_g2d_supported_function_t;

/** @brief This enum defines the G2D overlay layer. */
typedef enum {
    HAL_G2D_OVERLAY_LAYER0 = 0, /**< Layer 0. */
    HAL_G2D_OVERLAY_LAYER1 = 1, /**< Layer 1. */
    HAL_G2D_OVERLAY_LAYER2 = 2, /**< Layer 2. */
    HAL_G2D_OVERLAY_LAYER3 = 3  /**< Layer 3. */
} hal_g2d_overlay_layer_t;

/** @brief This enum defines the G2D overlay layer function. */
typedef enum {
    HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT = 0, /**< Font. */
    HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL, /**< Rectangle fill. */
    HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER    /**< Buffer. */
} hal_g2d_overlay_layer_function_t;

/** @brief This enum defines the G2D rotation angle. */
typedef enum {
    HAL_G2D_ROTATE_ANGLE_0 = 0,      /**< No rotation. */
    HAL_G2D_ROTATE_ANGLE_90,         /**< Rotate 90 degrees. */
    HAL_G2D_ROTATE_ANGLE_180,        /**< Rotate 180 degrees. */
    HAL_G2D_ROTATE_ANGLE_270,        /**< Rotate 270 degrees. */
    HAL_G2D_ROTATE_ANGLE_MIRROR_0,   /**< Mirror. */
    HAL_G2D_ROTATE_ANGLE_MIRROR_90,  /**< Mirror and then rotate 90 degrees. */
    HAL_G2D_ROTATE_ANGLE_MIRROR_180, /**< Mirror and then rotate 180 degrees. */
    HAL_G2D_ROTATE_ANGLE_MIRROR_270  /**< Mirror and then rotate 270 degrees. */
} hal_g2d_rotate_angle_t;

/** @brief This enum defines the G2D font format. */
typedef enum {
    HAL_G2D_FONT_FORMAT_1_BIT = 0, /**< 1-bit per font pixel. */
    HAL_G2D_FONT_FORMAT_2_BIT,     /**< 2-bit per font pixel. */
    HAL_G2D_FONT_FORMAT_4_BIT,     /**< 4-bit per font pixel. */
    HAL_G2D_FONT_FORMAT_8_BIT      /**< 8-bit per font pixel. */
} hal_g2d_font_format_t;

/** @brief This enum defines the G2D dithering mode. */
typedef enum {
    HAL_G2D_DITHERING_MODE_DISABLE = 0,  /**< Disable dithering. */
    HAL_G2D_DITHERING_MODE_RANDOM,       /**< Random dithering. */
    HAL_G2D_DITHERING_MODE_FIXED_PATTERN /**< Fixed-pattern dithering. */
} hal_g2d_dithering_mode_t;

/**
 * @}
 */

/*****************************************************************************
 * Structures
 *****************************************************************************/

/** @defgroup hal_g2d_hanele_struct Struct
 *  @{
 */

/** @brief This structure presents the G2D handle. */
typedef struct hal_g2d_hanele_struct_t hal_g2d_handle_t;

/**
 * @}
 */

/** @defgroup hal_g2d_typedef Typedef
 *  @{
 */

/** @brief Defines the prototype of the G2D callback function. The callback is invoked whenever the G2D engine completes an operation, such as draws an overlay, makes a rotation or fills a rectangle.\n
 *  Example code:\n
 *  @code
 *  hal_g2d_register_callback(g2dHandle, DrawLayerToDisply);
 *  ...
 *  hal_g2d_overlay_start(g2dHandle);
 *  @endcode
 *  The DrawLayerToDisplay() function will be called after the #hal_g2d_overlay_start() process is complete.\n
 */
typedef bool (*hal_g2d_callback_t)(void *);

/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief This function initializes the G2D module.
 * @return #HAL_G2D_STATUS_OK, if the initialization is successful.
 */
hal_g2d_status_t hal_g2d_init(void);

/**
 * @brief This function gets a G2D handle to access and control the G2D engine. After getting the handle, users can apply the G2D API to render images.
 * @param[out] handle_ptr is a pointer to the G2D handle, after the function returns successfully.
 * @param[in] codec_type specifies the G2D codec type. Currently, only the #HAL_G2D_CODEC_TYPE_HW is used.
 * @param[in] handle_mode specifies whether to use blocking mode or non-blocking mode to get the G2D handle. If in blocking mode, the function #hal_g2d_overlay_start() will block until G2D is no longer busy.
 * @return #HAL_G2D_STATUS_OK, if the operation to get the G2D handle completed successfully.\n
 *         #HAL_G2D_STATUS_BUSY, if the G2D hardware is busy and the handle_mode is set to #HAL_G2D_GET_HANDLE_MODE_NON_BLOCKING.
 */
hal_g2d_status_t hal_g2d_get_handle(hal_g2d_handle_t **handle_ptr, hal_g2d_codec_type_t codec_type, hal_g2d_get_handle_mode_t handle_mode);

/**
 * @brief This function releases the G2D handle and frees its associated resources occupied by this G2D handle. Call this function after all rendering processes are complete.
 * @param[in] handle is G2D handle.
 * @return #HAL_G2D_STATUS_OK, if the G2D handle is released successfully.\n
 *         #HAL_G2D_STATUS_BUSY, if the G2D hardware is busy.
 */
hal_g2d_status_t hal_g2d_release_handle(hal_g2d_handle_t *handle);

/**
 * @brief This function queries the range of the destination buffer.
 * @param[in] handle is the G2D handle.
 * @param[out] min_width is the minimum width.
 * @param[out] max_width is the maximum width.
 * @param[out] min_height is the minimum height.
 * @param[out] max_height is the maximum height.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_query_rgb_buffer_range(hal_g2d_handle_t *handle,
        uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height);

/**
 * @brief This function registers a user-defined callback function for the G2D completion events. The callback is invoked whenever the G2D engine completes an operation, such as draws an overlay, makes a rotation or fills a rectangle.
 * @param[in] handle specifies the G2D handle.
 * @param[in] callback specifies the callback function to be registered.
 * @return #HAL_G2D_STATUS_OK, if the registration is successful.
 */
hal_g2d_status_t hal_g2d_register_callback(hal_g2d_handle_t *handle, hal_g2d_callback_t callback);

/**
 * @brief The function sets the clipping window for the destination buffer. All data outside the clipping window will remain unchanged.\n
 * @param[in] handle specifies the G2D handle.
 * @param[in] x is the base vertex coordinate X of clipping window.
 * @param[in] y is the base vertex coordinate Y of clipping window.
 * @param[in] width is the width of the clipping window.
 * @param[in] height is the height of the clipping window.
 * @return #HAL_G2D_STATUS_OK, if the destination clipping window is set successfully.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the parameters are out of range. Supported range: -2048 <= x, y <= 2047; 1 <= x + width, y + height <= 2048.
 */
hal_g2d_status_t hal_g2d_set_clipping_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t width, int32_t height);

/**
 * @brief The color of the pixel that matches to the combination of the old set (alpha0, red0, green0, blue0) is replaced with the combination of the new set (alpha1, red1, green1, blue1).
 * @param[in] handle specifies the G2D handle.
 * @param[in] alpha0 is the old alpha color component.
 * @param[in] red0 is the old red color component.
 * @param[in] green0 is the old green color component.
 * @param[in] blue0 is the old blue color component.
 * @param[in] alpha1 is the new alpha color component.
 * @param[in] red1 is the new red color component.
 * @param[in] green1 is the new green color component.
 * @param[in] blue1 is the new blue color component.
 * @return #HAL_G2D_STATUS_OK, if the operation completed successfully.
 */
hal_g2d_status_t hal_g2d_replace_color(hal_g2d_handle_t *handle,
        uint8_t alpha0, uint8_t red0, uint8_t green0, uint8_t blue0, uint8_t alpha1, uint8_t red1, uint8_t green1, uint8_t blue1);

/**
 * @deprecated Please use #hal_g2d_overlay_set_layer_source_key() instead.
 * This function sets the source key value.
 * @param[in] handle specifies the G2D handle.
 * @param[in] alpha is the alpha color component of source key color.
 * @param[in] red is the red color component of source key color.
 * @param[in] green is the green color component of source key color.
 * @param[in] blue is the blue color component of source key color.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_set_source_key(hal_g2d_handle_t *handle, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @deprecated Please use #hal_g2d_set_rgb_buffer() instead.
 * This function sets the destination buffer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] buffer is the address of the destination buffer.
 * @param[in] size is the buffer size.
 * @param[in] width is the width of the destination buffer.
 * @param[in] height is the height of the destination buffer.
 * @param[in] color_format specifies the color format(GRAY, RGB565, RGB888, ARGB8888, etc.).
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of parameters are out of the range, for example, the width is greater than 0x2000.
 */
hal_g2d_status_t hal_set_rgb_buffer(hal_g2d_handle_t *handle,
        uint8_t *buffer, uint32_t size, uint32_t width, uint32_t height, hal_g2d_color_format_t color_format);

/**
 * @brief This function sets the destination buffer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] buffer is the address of the destination buffer.
 * @param[in] size is the buffer size.
 * @param[in] width is the width of the destination buffer.
 * @param[in] height is the height of the destination buffer.
 * @param[in] color_format specifies the color format(GRAY, RGB565, RGB888, ARGB8888, etc.).
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of parameters are out of range, for example, the width is greater than 0x2000.
 */
hal_g2d_status_t hal_g2d_set_rgb_buffer(hal_g2d_handle_t *handle,
        uint8_t *buffer, uint32_t size, uint32_t width, uint32_t height, hal_g2d_color_format_t color_format);

/**
 * @brief The function gets the status of G2D engine.
 * @param[in] handle is the G2D handle to get the G2D engine's status.
 * @return #HAL_G2D_STATUS_OK, if the G2D is available for use.\n
 *         #HAL_G2D_STATUS_BUSY, if the G2D hardware is busy.
 */
hal_g2d_status_t hal_g2d_get_status(hal_g2d_handle_t *handle);

/**
 * @brief This function queries whether a particular function type is supported by the G2D engine.
 * @param[in] codec_type specifies the codec type, hardware codec or software codec.
 * @param[in] function_type specifies the function type to be queried.
 * @return #HAL_G2D_STATUS_OK, if the queried function type is supported.\n
 *         #HAL_G2D_STATUS_NOT_SUPPORTED, the function type is not supported.
 */
hal_g2d_status_t hal_g2d_query_support(hal_g2d_codec_type_t codec_type, hal_g2d_supported_function_t function_type);

/**
 * @brief This function enables a layer. The layer will be put onto the destination buffer only when it is enabled.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer specifies the layer that to be enabled. (layer 0, layer 1, layer 2 or layer 3).
 * @param[in] show_layer is the flag indicating whether the specified layer should be shown or not.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_enable_layer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, bool show_layer);

/**
 * @brief This function sets an overlay layer function to a specific layer. For example, to do the overlay only, set the function to HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to assign the function to.
 * @param[in] function specifies the function to be assigned to the layer, such as #HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_function(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, hal_g2d_overlay_layer_function_t function);

/**
 * @brief This function sets rectangle fill color to a specific layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set the color for the rectangle fill.
 * @param[in] rectfill_color is the color for the rectangle fill.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_rectfill_color(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t rectfill_color);

/**
 * @deprecated Please use #hal_g2d_overlay_set_layer_rgb_buffer() instead.
 * This function sets a buffer for a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set a RGB buffer for.
 * @param[in] buffer is the buffer address.
 * @param[in] size is the buffer size.
 * @return #HAL_G2D_STATUS_OK, if set layer buffer information is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_buffer(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint8_t *buffer, uint32_t size);

/**
 * @brief This function sets the RGB buffer information to this layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to enable.
 * @param[in] buffer is the buffer address.
 * @param[in] size is the buffer size.
 * @param[in] width is the buffer width.
 * @param[in] height is the buffer height.
 * @param[in] color_format is the color format to apply to the buffer.
 * @return #HAL_G2D_STATUS_OK, if set RGB buffer information is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of the parameters are out of range, such as the width is greater than 0x2000.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_rgb_buffer(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint8_t *buffer, uint32_t size,
        uint32_t width, uint32_t height, hal_g2d_color_format_t color_format);

/**
 * @deprecated Please use #hal_g2d_overlay_set_layer_rgb_buffer() instead.
 * This function sets a YUV buffer for a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set the YUV buffer for.
 * @param[in] buffer is the buffer address.
 * @param[in] size is the buffer size.
 * @param[in] width is the buffer width.
 * @param[in] height is the buffer height.
 * @param[in] color_format is the color format to apply to the buffer.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of the parameters are out of range, such as the width is greater than 0x2000.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_yuv_buffer(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint8_t *buffer[3], uint32_t size[3],
        uint32_t width, uint32_t height, hal_g2d_color_format_t color_format);

/**
 * @brief This function sets the rectangle to a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set a rectangle for.
 * @param[in] x is the coordinate X of the rectangle.
 * @param[in] y is the coordinate Y of the rectangle.
 * @param[in] width is the width of the rectangle.
 * @param[in] height is the height of the rectangle.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of the parameters are out of the range, such as the width is greater than 0x2000.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_window(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, int32_t x, int32_t y, uint32_t width, uint32_t height);

/**
 * @brief This function enables the source key for a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to enable the source key for.
 * @param[in] src_key_value is the source key value.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_source_key(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t src_key_value);

/**
 * @brief This function sets the rotation for a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set the rotation for.
 * @param[in] rotation is the layer's rotation angle.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_rotation(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, hal_g2d_rotate_angle_t rotation);

/**
 * @brief This function sets the alpha to a layer.
 * @param[in] handle specifies the G2D handle.
 * @param[in] layer is the layer to set the alpha for.
 * @param[in] alpha_value is alpha value for the layer.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_alpha(hal_g2d_handle_t *handle,
        hal_g2d_overlay_layer_t layer, uint32_t alpha_value);

/**
 * @brief This function sets the background color.
 * @param[in] handle specifies the G2D handle.
 * @param[in] bg_color is the background color.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_background_color(hal_g2d_handle_t *handle, uint32_t bg_color);

/**
 * @brief This function sets the destination alpha.
 * @param[in] handle specifies the G2D handle.
 * @param[in] alpha_value is the alpha value.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_alpha(hal_g2d_handle_t *handle, uint32_t alpha_value);

/**
 * @brief This function sets the coordinates in the destination window. Call the function #hal_g2d_overlay_set_roi_window() to set the width and height.
 * @param[in] handle specifies the G2D handle.
 * @param[in] x is coordinate X of the destination window.
 * @param[in] y is coordinate Y of the destination window.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_overlay_set_window(hal_g2d_handle_t *handle, int32_t x, int32_t y);

/**
 * @brief This function sets the ROI(region of interest) window.
 * @param[in] handle specifies the G2D handle.
 * @param[in] x is the coordinate X of the ROI window.
 * @param[in] y is the coordinate Y of the ROI window.
 * @param[in] width is the width of the ROI window.
 * @param[in] height is the height of the ROI window.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of the parameters are out of range, such as X, Y, width or height is over 2048.
 */
hal_g2d_status_t hal_g2d_overlay_set_roi_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, uint32_t width, uint32_t height);

/**
 * @brief This function starts the overlay.
 * @param[in] handle specifies the G2D handle.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_BUSY, if the G2D hardware is busy.
 */
hal_g2d_status_t hal_g2d_overlay_start(hal_g2d_handle_t *handle);

/**
 * @brief This function sets the dithering of the destination.
 * @param[in] handle is the G2D handle.
 * @param[in] mode is the dithering mode.
 * @param[in] red_bit is the dithering bit in red channel.
 * @param[in] green_bit is the dithering bit in green channel.
 * @param[in] blue_bit is the dithering bit in blue channel.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user.\n
 */
hal_g2d_status_t hal_g2d_set_dithering(hal_g2d_handle_t *handle, hal_g2d_dithering_mode_t mode, uint8_t red_bit, uint8_t green_bit, uint8_t blue_bit);

/**
 * @brief This function sets the font format for a layer.
 * @param[in] handle is the G2D handle.
 * @param[in] layer is the layer to set the font format for.
 * @param[in] format is the format of the font bitstream.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user.\n
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_font_format(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, hal_g2d_font_format_t format);

/**
 * @brief This function sets the font color for a layer.
 * @param[in] handle is the G2D handle.
 * @param[in] layer is the layer to set the font color for.
 * @param[in] color is the color to draw the font.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if an invalid parameter is given by user.\n
 */
hal_g2d_status_t hal_g2d_overlay_set_layer_font_color(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint32_t color);

/**
 * @deprecated Please use #hal_g2d_overlay_set_layer_window() instead.
 * This function sets the rectangle fill region.
 * @param[in] handle is the G2D handle.
 * @param[in] x is the coordinate X of the region for the rectangle fill.
 * @param[in] y is the coordinate Y of the region for the rectangle fill.
 * @param[in] width is width of rectangle fill.
 * @param[in] height is height of rectangle fill.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_INVALID_PARAMETER, if the values of the parameters are out of range, such as X, Y, width or height is over 2048.
 */
hal_g2d_status_t hal_g2d_rectfill_set_destination_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t width, int32_t height);

/**
 * @deprecated Please use #hal_g2d_overlay_set_layer_rectfill_color() instead.
 * This function sets the color for the rectangle fill.
 * @param[in] handle specifies the G2D handle.
 * @param[in] color is the color to fill the rectangle.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_rectfill_set_color(hal_g2d_handle_t *handle, uint32_t color);

/**
 * @deprecated Please use #hal_g2d_query_rgb_buffer_range() instead.
 * This function queries the range of the destination window.
 * @param[in] handle specifies the G2D handle.
 * @param[out] min_x is the minimum of the coordinate X.
 * @param[out] max_x is the maximum of the coordinate X.
 * @param[out] min_y is the minimum of the coordinate Y.
 * @param[out] max_y is the maximum of the coordinate Y.
 * @param[out] min_width is minimum of the width of the window.
 * @param[out] max_width is maximum of the width of the window.
 * @param[out] min_height is minimum of the height of the window.
 * @param[out] max_height is maximum of the height of the window.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.
 */
hal_g2d_status_t hal_g2d_rectangle_fill_query_window_range(hal_g2d_handle_t *handle,
        int32_t *min_x, int32_t *max_x, int32_t *min_y, int32_t *max_y,
        uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height);

/**
 * @deprecated Please use #hal_g2d_overlay_start() instead.
 * This function starts the rectangle fill operation.
 * @param[in] handle specifies the G2D handle.
 * @return #HAL_G2D_STATUS_OK, if the operation is successful.\n
 *         #HAL_G2D_STATUS_BUSY, if the G2D hardware is busy.
 */
hal_g2d_status_t hal_g2d_rectfill_start(hal_g2d_handle_t *handle);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_G2D_MODULE_ENABLED */
#endif /* __HAL_G2D_H__ */
