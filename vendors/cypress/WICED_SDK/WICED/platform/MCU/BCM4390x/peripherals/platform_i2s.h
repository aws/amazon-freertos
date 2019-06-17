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
#pragma once

#include "wiced_platform.h"
#include "wiced_audio.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Maximum number of data bytes per DMA descriptor. */
#define I2S_DMA_DATA_BYTES_MAX      (4096)

#define I2S_MAX_CHANNELS            (2)
#define I2S_MAX_BYTES_PER_SAMPLE    (4)
#define I2S_MAX_FRAME_BYTES         (I2S_MAX_CHANNELS * I2S_MAX_BYTES_PER_SAMPLE)

/* I2S RX BufCount + RcvControl.RxOffset == 4KB will cause the DMA engine
 * to repeat previous 32-bytes at the 4KB boundary.  This is likely due to
 * the RxOffset mis-aligning the first frame.
*/
/* Maximum number of bytes of real audio payload data. */
#define I2S_TX_PERIOD_BYTES_MAX     (I2S_DMA_DATA_BYTES_MAX)
#define I2S_RX_PERIOD_BYTES_MAX     ((I2S_DMA_DATA_BYTES_MAX - I2S_DMA_RXOFS_BYTES) / I2S_MAX_FRAME_BYTES * I2S_MAX_FRAME_BYTES)
#define I2S_PERIOD_BYTES_MAX        (I2S_TX_PERIOD_BYTES_MAX > I2S_RX_PERIOD_BYTES_MAX ? I2S_TX_PERIOD_BYTES_MAX : I2S_RX_PERIOD_BYTES_MAX)

#define I2S_DMA_TXOFS_BYTES         (0)
/* RX direction always has a 4-byte offset. */
#define I2S_DMA_RXOFS_BYTES         (4)

#define I2S_DMA_OFS_BYTES           (I2S_DMA_TXOFS_BYTES > I2S_DMA_RXOFS_BYTES ? I2S_DMA_TXOFS_BYTES : I2S_DMA_RXOFS_BYTES)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    WICED_I2S_READ,
    WICED_I2S_WRITE
} wiced_i2s_transfer_t;

typedef enum
{
    WICED_I2S_SPDIF_MODE_OFF,
    WICED_I2S_SPDIF_MODE_ON,
} wiced_i2s_spdif_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

//typedef DMA_Stream_TypeDef dma_stream_registers_t;
typedef uint32_t           dma_channel_t;

typedef struct
{
    uint32_t sample_rate;
    uint16_t period_size;
    uint8_t bits_per_sample;
    uint8_t channels;
    wiced_i2s_spdif_mode_t i2s_spdif_mode;
} wiced_i2s_params_t;

typedef struct
{
    void*    next;
    void*    buffer;
    uint32_t buffer_size;
} platform_i2s_transfer_t;

/* the callback will give a neww buffer pointer to the i2s device */
typedef void(*wiced_i2s_tx_callback_t)(uint8_t** buffer, uint16_t* size, void* context);

/* returned buffer and new buffer to receive more data */
typedef void(*wiced_i2s_rx_callback_t)( uint8_t* buffer, uint16_t read_size,  uint8_t**rx_buffer, uint16_t*rx_buf_size );

/******************************************************
 *               Function Declarations
 ******************************************************/

wiced_result_t wiced_i2s_init                        ( wiced_audio_session_ref sh, wiced_i2s_t i2s, wiced_i2s_params_t* params, uint32_t* mclk);
wiced_result_t wiced_i2s_deinit                      ( wiced_i2s_t i2s);
wiced_result_t wiced_i2s_set_audio_buffer_details    ( wiced_i2s_t i2s, wiced_audio_buffer_header_t *audio_buffer_ptr);
wiced_result_t wiced_i2s_start                       ( wiced_i2s_t i2s);
wiced_result_t wiced_i2s_stop                        ( wiced_i2s_t i2s);
wiced_result_t wiced_i2s_get_current_hw_pointer      ( wiced_i2s_t i2s, uint32_t* hw_pointer );
wiced_result_t wiced_i2s_get_current_hw_buffer_weight( wiced_i2s_t i2s, uint32_t* weight );
wiced_result_t wiced_i2s_pll_set_fractional_divider  ( wiced_i2s_t i2s, float value );
wiced_result_t wiced_i2s_get_clocking_details        (const wiced_i2s_params_t *config, uint32_t *mclk );


#ifdef __cplusplus
} /* extern "C" */
#endif
