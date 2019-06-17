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

#include "wwd_constants.h"
#include "platform_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifdef DEBUG
#define WICED_AUDIO_DEBUG_PRINT(x)
#define WICED_AUDIO_INFO_PRINT(x)     do { printf x; } while(0)
#define WICED_AUDIO_ERROR_PRINT(x)    do { printf x; } while(0)
#else
#define WICED_AUDIO_DEBUG_PRINT(x)
#define WICED_AUDIO_INFO_PRINT(x)
#define WICED_AUDIO_ERROR_PRINT(x)    do { printf x; } while(0)
#endif

#if defined(PLATFORM_L1_CACHE_SHIFT)
#define WICED_AUDIO_BUFFER_BYTE_ALIGNMENT_SHIFT     PLATFORM_L1_CACHE_SHIFT
#endif

#define WICED_AUDIO_BUFFER_RESERVED_BYTES   (4)

#if defined(WICED_AUDIO_BUFFER_BYTE_ALIGNMENT_SHIFT)
#define WICED_AUDIO_BUFFER_ALIGNMENT_BYTES  (1U << WICED_AUDIO_BUFFER_BYTE_ALIGNMENT_SHIFT)
#define WICED_AUDIO_BUFFER_ALIGNMENT_MASK   (WICED_AUDIO_BUFFER_ALIGNMENT_BYTES - 1)
#define WICED_AUDIO_BUFFER_ROUND_UP(a)      (((uint32_t)(a) + WICED_AUDIO_BUFFER_ALIGNMENT_MASK) & ~(WICED_AUDIO_BUFFER_ALIGNMENT_MASK))
#define WICED_AUDIO_BUFFER_ROUND_DOWN(a)    ((uint32_t)(a) & ~(WICED_AUDIO_BUFFER_ALIGNMENT_MASK))
#define WICED_AUDIO_BUFFER_PTR_ROUND_UP(p)  ((void*)WICED_AUDIO_BUFFER_ROUND_UP((uint32_t)(p)))
#define WICED_AUDIO_BUFFER_OFFSET(a)        ((uint32_t)(a) & (WICED_AUDIO_BUFFER_ALIGNMENT_MASK) )

#define WICED_AUDIO_BUFFER_HEADER_SIZEOF \
    WICED_AUDIO_BUFFER_ROUND_UP(sizeof(wiced_audio_buffer_header_t))

#define WICED_AUDIO_BUFFER_PERIOD_SIZEOF(_BYTES_PER_PERIOD_) \
    WICED_AUDIO_BUFFER_ROUND_UP((_BYTES_PER_PERIOD_) + WICED_AUDIO_BUFFER_RESERVED_BYTES)

#else
#define WICED_AUDIO_BUFFER_ALIGNMENT_BYTES  ((uint32_t)0)
#define WICED_AUDIO_BUFFER_ALIGNMENT_MASK   ((uint32_t)0)
#define WICED_AUDIO_BUFFER_ROUND_UP(a)      ((uint32_t)(a))
#define WICED_AUDIO_BUFFER_ROUND_DOWN(a)    ((uint32_t)(a))
#define WICED_AUDIO_BUFFER_PTR_ROUND_UP(p)  ((void*)(p))
#define WICED_AUDIO_BUFFER_OFFSET(a)        ((uint32_t)0)

#define WICED_AUDIO_BUFFER_HEADER_SIZEOF    0
#define WICED_AUDIO_BUFFER_PERIOD_SIZEOF(_BYTES_PER_PERIOD_) _BYTES_PER_PERIOD_
#endif

#ifndef WICED_AUDIO_BUFFER_PERIOD_RESERVED_BYTES
#define WICED_AUDIO_BUFFER_PERIOD_RESERVED_BYTES (0)
#endif

#define WICED_AUDIO_BUFFER_ARRAY_ELEMENT_SIZEOF(_BYTES_PER_PERIOD_) \
    (WICED_AUDIO_BUFFER_HEADER_SIZEOF + WICED_AUDIO_BUFFER_PERIOD_SIZEOF(_BYTES_PER_PERIOD_))

#define WICED_AUDIO_BUFFER_ARRAY_SIZEOF(_PERIODS_, _BYTES_PER_PERIOD_) \
    (WICED_AUDIO_BUFFER_ARRAY_ELEMENT_SIZEOF(_BYTES_PER_PERIOD_) * (_PERIODS_))

#define WICED_AUDIO_BUFFER_ARRAY_DIM_SIZEOF(_PERIODS_, _BYTES_PER_PERIOD_) \
    (WICED_AUDIO_BUFFER_ALIGNMENT_MASK + WICED_AUDIO_BUFFER_ARRAY_SIZEOF(_PERIODS_, _BYTES_PER_PERIOD_))

#define WICED_AUDIO_BUFFER_ARRAY_PTR(_PTR_) \
    WICED_AUDIO_BUFFER_PTR_ROUND_UP(_PTR_)

/******************************************************
 *                    Constants
 ******************************************************/

#define MAX_NUM_AUDIO_DEVICES (platform_audio_get_device_count())

/**
 * The WICED audio driver declares a static buffer internally for use when
 * the wiced_audio_create_buffer() parameters specify a NULL an audio buffer.
 * The following definitions control the sizing of the audio driver buffer.
 */

#ifndef WICED_AUDIO_DEVICE_PERIODS
#define WICED_AUDIO_DEVICE_PERIODS          ( 12 )
#endif

#ifndef WICED_AUDIO_DEVICE_PERIOD_SIZE
#define WICED_AUDIO_DEVICE_PERIOD_SIZE      ( 1024 )
#endif

#ifndef WICED_AUDIO_MAX_BUFFER_SIZE
#define WICED_AUDIO_MAX_BUFFER_SIZE         WICED_AUDIO_BUFFER_ARRAY_DIM_SIZEOF(WICED_AUDIO_DEVICE_PERIODS, WICED_AUDIO_DEVICE_PERIOD_SIZE)
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * WICED audio output type
 */
typedef enum
{
    SPEAKERS,
    HEADPHONES,
    SPEAKERS_AND_HEADPHONES
} wiced_audio_output_t;

/**
 * WICED audio device channel
 */
typedef enum
{
    WICED_PLAY_CHANNEL,
    WICED_RECORD_CHANNEL
} wiced_audio_device_channel_t;

/**
 * WICED audio event type
 */
typedef enum
{
    WICED_AUDIO_PERIOD_ELAPSED = 0,
    WICED_AUDIO_UNDERRUN       = 1,
    WICED_AUDIO_HW_ERROR       = 2,
} wiced_audio_platform_event_t;

/**
 * WICED audio device-specific IOCTL
 */
typedef enum
{
    WICED_AUDIO_IOCTL_SET_DSP_EFFECT        = 0,
    WICED_AUDIO_IOCTL_SET_DAC_OUTPUT_MIXING,
    WICED_AUDIO_IOCTL_GET_DATA_OFFSET,
    WICED_AUDIO_IOCTL_GET_DATA_BUFFER,
    WICED_AUDIO_IOCTL_REG_READ,
    WICED_AUDIO_IOCTL_REG_WRITE,
    WICED_AUDIO_IOCTL_GET_WUPD_PARAMS,
    WICED_AUDIO_IOCTL_SET_WUPD_PARAMS,
} wiced_audio_device_ioctl_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef void (*wiced_audio_data_provider_t)( uint8_t** buffer, uint16_t* size, void* context);
typedef struct wiced_audio_session_t* wiced_audio_session_ref;

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * WICED audio configuration
 */
typedef struct
{
    uint32_t sample_rate;               /**< The rate at which the samples are captured or played back, measured in Hertz (Hz)(e.g. 48000, 96000, etc) */
    uint8_t  bits_per_sample;           /**< The number of bits in each audio sample (16, 24, 32)                                                      */
    uint8_t  channels;                  /**< The number of audio channels (e.g. 2 for stereo)                                                          */
    uint8_t  frame_size;                /**< The number of channels * bits_per_sample (container size) / 8                                             */
    uint8_t  volume;                    /**< Attenuation (gain) - 0 to 100 scale where 0 is off and 100 is max gain                                    */
} wiced_audio_config_t;

/**
 * WICED audio device interface
 */
typedef struct
{
    int                          port;     /*< uses platform specific I2S port @ref wiced_i2s_t */
    wiced_audio_device_channel_t channel;
    platform_audio_port_type_t   type;
} wiced_audio_data_port_t;

typedef struct
{
    uint8_t left_channel_select;
    uint8_t right_channel_select;
} wiced_audio_dac_output_mixing_t;

typedef struct
{
    uint8_t*  buffer;
    uint32_t  buffer_size;
    uint32_t  data_offset;
    uint32_t  data_length;
    uint32_t  timeout_msec;
} wiced_audio_data_buffer_t;

typedef struct
{
    uint32_t  start;
    uint32_t  end;
    uint32_t  timeout_msec;
} wiced_audio_data_offset_t;

typedef struct
{
    uint32_t address;
    uint32_t value;
} wiced_audio_register_value_t;

typedef struct
{
   uint32_t sc_enabled;
   double   sc_passthrough_gain_db;
   double   aec_ref_bulk_delay_msec;
   double   aec_mic_bulk_delay_msec;
   double   dtd_threshold;
   double   mic_gain_db;
   double   recording_gain_db;
   double   trigger_gain_db;
} wiced_audio_wupd_params_t;

typedef union
{
    uint8_t                         dsp_effect_mode;
    wiced_audio_dac_output_mixing_t dac_output_mode;
    wiced_audio_data_offset_t       data_offset;
    wiced_audio_data_buffer_t       data_buffer;
    wiced_audio_register_value_t    register_value;
    wiced_audio_wupd_params_t       wupd_params;
} wiced_audio_device_ioctl_data_t;

/**
 * WICED audio device interface
 * @note : Only used by device drivers
 */
typedef struct
{
    platform_audio_device_id_t  device_id;
    void*                       audio_device_driver_specific;
    wiced_result_t (*audio_device_init)            ( void* device_data, wiced_audio_data_port_t* data_port );
    wiced_result_t (*audio_device_deinit)          ( void* device_data );
    wiced_result_t (*audio_device_configure)       ( void* device_data, wiced_audio_config_t* config, uint32_t* mclk );
    wiced_result_t (*audio_device_start_streaming) ( void* device_data );
    wiced_result_t (*audio_device_stop_streaming)  ( void* device_data );
    wiced_result_t (*audio_device_set_volume)      ( void* device_data, double decibles );
    wiced_result_t (*audio_device_set_treble)      ( void* device_data, uint8_t percentage );
    wiced_result_t (*audio_device_set_bass)        ( void* device_data, uint8_t percentage );
    wiced_result_t (*audio_device_get_volume_range)( void* device_data, double* min_volume_decibels, double* max_volume_decibels);
    wiced_result_t (*audio_device_ioctl)           ( void* device_data, wiced_audio_device_ioctl_t cmd, wiced_audio_device_ioctl_data_t* cmd_data );
} wiced_audio_device_interface_t;

/**
 * @note : Only used by device drivers
 */
typedef struct wiced_audio_buffer_header
{
    struct wiced_audio_buffer_header* next;
    uint8_t*                          data_start;
    uint8_t*                          data_end;
} wiced_audio_buffer_header_t;

/**
 * WICED audio device class
 */
typedef struct
{
    wiced_audio_device_interface_t* audio_devices;
    int                             device_count;
} audio_device_class_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
/*****************************************************************************/
/**
 *
 *  @defgroup multimedia     WICED Multimedia
 *  @ingroup  multimedia
 *
 *  WICED API's for Multimedia processing
 *
 *  @addtogroup audio    WICED Audio API
 *  @ingroup multimedia
 *
 * This library implements core WICED Audio APIs for audio capture and playback and managing
 * resources used by the I2S audio hardware found on Wiced Audio Edition (WAE) Platforms.
 *
 *  @{
 */
/*****************************************************************************/

/** Initialize the audio driver for an audio device.
 *
 * @param[in]     device_id   : The id of the audio device to be used.
 * @param[in.out] sh          : A pointer to the audio session handle to be initialized.
 * @param[in]     period_size : Audio buffer period size.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_init                        ( const platform_audio_device_id_t device_1d, wiced_audio_session_ref* sh, uint16_t period_size );

/** Configure the audio driver for a specific audio format.
 *
 * @param[in] sh     : The audio session handle.
 * @param[in] config : Pointer to the audio configuration to use.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_configure                   ( wiced_audio_session_ref sh, wiced_audio_config_t* config );

/** Create the buffer used by the audio driver.
 *
 * @note If allocator is null and buffer_ptr_aligned is null, then the buffer will be allocated and owned by the audio driver.
 *       Not all platforms support using an allocator function.
 *
 * @param[in] sh                 : The audio session handle.
 * @param[in] size               : Size of the audio buffer - must be a multiple of the period size.
 * @param[in] buffer_ptr_aligned : Optional pointer to an existing audio buffer to use.
 * @param[in] allocator          : Optional pointer to allocation function to use for allocating audio buffer.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_create_buffer               ( wiced_audio_session_ref sh, uint16_t size, uint8_t* buffer_ptr_aligned, void*(*allocator)(uint16_t size));

/** Set the volume for the audio driver.
 *
 * @param[in] sh     : The audio session handle.
 * @param[in] volume : The new volume.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_set_volume                  ( wiced_audio_session_ref sh, double volume_in_db );

/** Get the volume range for the audio driver.
 *
 * @note The audio volume range is dependant upon the audio device being used.
 *
 * @param[in]  sh               : The audio session handle.
 * @param[out] min_volume_in_db : Returned minimum volume for the audio device.
 * @param[out] max_volume_in_db : Returned maximum volume for the audio device.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_get_volume_range            ( wiced_audio_session_ref sh, double *min_volume_in_db, double *max_volume_in_db );

/** De-initialize the audio driver.
 *
 * @param[in] sh     : The audio session handle.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_deinit                      ( wiced_audio_session_ref sh );

/** Get a portion of the audio buffer that is available for writing (playback) or reading (capture).
 *
 * @note If an underrun has ocurred during playback, WICED_ERROR will be returned.
 *
 * @param[in]     sh   : The audio session handle.
 * @param[out]    ptr  : Address of the buffer pointer to set.
 * @param[in,out] size : Size of the audio buffer requested. Returned value is the size available.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_get_buffer                  ( wiced_audio_session_ref sh, uint8_t** ptr, uint16_t* size);

/** Return the current offset of the hardware pointer in the audio buffer.
 *
 * @param[in]  sh          : The audio session handle.
 * @param[out] hw_pointer  : Address of the variable to receive the offset.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_get_current_hw_pointer      ( wiced_audio_session_ref sh, uint32_t* hw_pointer);

/** Start the audio driver.
 *
 * @param[in] sh : The audio session handle.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_start                       ( wiced_audio_session_ref sh );

/** Stop the audio driver.
 *
 * @note After an underrun event, the audio driver must be stopped and started again.
 *
 * @param[in] sh : The audio session handle.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_stop                        ( wiced_audio_session_ref sh );

/** Release a portion of audio buffer to the driver.
 *
 * @param[in] sh   : The audio session handle.
 * @param[in] size : Size of buffer (in bytes) being released.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_release_buffer              ( wiced_audio_session_ref sh, uint16_t size);

/** Send an event to the audio driver.
 *
 * @note This routine is used by the underlying I2S driver to send events to the audio driver.
 *
 * @param[in] sh    : The audio session handle.
 * @param[in] event : The event being sent.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_buffer_platform_event       ( wiced_audio_session_ref sh, wiced_audio_platform_event_t event);

/** Get the number of periods of data in the audio buffer.
 *
 * @note This routine is used by the underlying I2S driver to query the audio driver.
 *
 * @param[in] sh    : The audio session handle.
 *
 * @return The number of periods in the audio buffer.
 */
uint16_t       wiced_audio_buffer_platform_get_periods ( wiced_audio_session_ref sh );

/** Wait until the requested size is available in the audio buffer.
 *
 * @param[in] sh      : The audio session handle.
 * @param[in] size    : The buffer size to wait for.
 * @param[in] timeout : The wait timeout value in milliseconds.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_wait_buffer                 ( wiced_audio_session_ref sh, uint16_t size, uint32_t timeout );

/** Return how many bytes are currently in the audio buffer.
 *
 * @param[in] sh      : The audio session handle.
 * @param[out] weight : Pointer to a variable to receive the current buffer weight in bytes.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_get_current_buffer_weight   ( wiced_audio_session_ref sh, uint32_t* weight );

/** Send an ioctl to the audio chip driver.
 *
 * @note Values passed with cmd_data are dependent upon the driver for the audio device being used.
 *
 * @param[in] sh   : The audio session handle.
 * @param[in] cmd  : The IOCTL command sent directly to the audio device driver @ref wiced_audio_device_ioctl_t
 * @param[in] cmd  : Data associated with the IOCTL command @ref wiced_audio_device_ioctl_data_t
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_device_ioctl                ( wiced_audio_session_ref sh, wiced_audio_device_ioctl_t cmd, wiced_audio_device_ioctl_data_t* cmd_data );

/** Return how many frames of audio data are currently in the audio buffer.
 *
 * @param[in] sh       : The audio session handle.
 * @param[out] latency : Pointer to a variable to receive the current buffer weight in audio frames.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_get_latency                 ( wiced_audio_session_ref sh, uint32_t* latency );

/** Register an audio device with the audio driver.
 *
 * @param[in] device_id   : The id of the audio device being registered.
 * @param[in] interface   : Pointer to the device interface structure for the audio device.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_register_audio_device             ( const platform_audio_device_id_t device_id, wiced_audio_device_interface_t* interface );

/** Set the PLL fractional divider
 *
 * @param[in] sh    : The audio session handle.
 * @param[in] value : PPM offset from the base frequency.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_set_pll_fractional_divider  ( wiced_audio_session_ref sh, float value );

/** Set the audio period size.
 *
 * @note The driver must be in the stopped state when updating the period size.
 *
 * @param[in] sh          : The audio session handle.
 * @param[in] period_size : The period size to use.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_audio_update_period_size          ( wiced_audio_session_ref sh, uint16_t period_size );

#ifdef __cplusplus
} /* extern "C" */
#endif
