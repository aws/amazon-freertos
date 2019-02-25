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

/** @file
 *
 */

#include <string.h>
#include "wwd_buffer.h"
#include "wiced.h"
#include "platform.h"
#include "platform_audio.h"

/******************************************************
 *                      Macros
 ******************************************************/

/* these must be defined in the platform's platform.h file */

/* common audio device defines */
#ifndef PLATFORM_DEFAULT_AUDIO_INPUT
#error  "Platform must define PLATFORM_DEFAULT_AUDIO_INPUT."
#endif
#ifndef PLATFORM_AUDIO_NUM_INPUTS
#error  "Platform must define PLATFORM_AUDIO_NUM_INPUTS."
#endif
#ifndef PLATFORM_DEFAULT_AUDIO_OUTPUT
#error  "Platform must define PLATFORM_DEFAULT_AUDIO_OUTPUT."
#endif
#ifndef PLATFORM_AUDIO_NUM_OUTPUTS
#error  "Platform must define PLATFORM_AUDIO_NUM_OUTPUTS."
#endif

/******************************************************
 *                    Constants
 ******************************************************/

#define PLATFORM_AUDIO_RATES_STRING_MAX         64
#define PLATFORM_AUDIO_SIZES_STRING_MAX         32

/******************************************************
 *               Variables Definitions
 ******************************************************/

static char built_rates_string[PLATFORM_AUDIO_RATES_STRING_MAX];
static char built_sizes_string[PLATFORM_AUDIO_SIZES_STRING_MAX];

/******************************************************
 *               Function Definitions
 ******************************************************/

/** Get Audio Device Info.
 *
 * @param[in]       io_type : input / output Device type
 *                            platform_input_output_t
 * @param[in]       index   : index into list (max = platform_audio_device_count(io_type) - 1)
 *
 * @return  pointer to audio info structure
 *          NULL if bad argument(s)
 */
const platform_audio_device_info_t* platform_audio_device_get_info(platform_audio_direction_t io_type, uint16_t index)
{
#if (PLATFORM_AUDIO_NUM_INPUTS > 0)
    if (io_type == PLATFORM_AUDIO_DEVICE_INPUT)
    {
        if (index < PLATFORM_AUDIO_NUM_INPUTS)
        {
            return &platform_audio_input_devices[index];
        }
    }
#endif

#if (PLATFORM_AUDIO_NUM_OUTPUTS > 0)
    if (io_type == PLATFORM_AUDIO_DEVICE_OUTPUT)
    {
        if (index < PLATFORM_AUDIO_NUM_OUTPUTS)
        {
            return &platform_audio_output_devices[index];
        }
    }

#endif
    /* if there are no inputs or outputs */
    UNUSED_PARAMETER(io_type);
    UNUSED_PARAMETER(index);
    return NULL;
}

/** Get Audio Device Type
 *
 * @param[in]       device_info   : pointer to partially filled out device information
 *                                  Required fields:
 *                                      direction
 *                                      port_type
 *                                  Optional fields: (checked if they are non-zero)
 *                                      num_channels
 *                                      sample_sizes
 *                                      sample_rates
 *                                  Ignored fields:
 *                                      device_id
 *                                      device_name
 *                                      description
 *
 * @return  platform_audio_device_id_t
 *          AUDIO_DEVICE_ID_NONE on error (no devices match requirements)
 */
platform_audio_device_id_t platform_audio_device_get_type(platform_audio_device_info_t* device_info)
{
    if (device_info == NULL)
    {
        return AUDIO_DEVICE_ID_NONE;
    }

    if (device_info->direction == PLATFORM_AUDIO_DEVICE_INPUT)
    {
#if (PLATFORM_AUDIO_NUM_INPUTS > 0)
        int i;
        for (i = 0; i < PLATFORM_AUDIO_NUM_INPUTS; i++)
        {
            if ((platform_audio_input_devices[i].direction == device_info->direction) &&
                (platform_audio_input_devices[i].port_type == device_info->port_type))
            {
                if ((device_info->num_channels != 0 ) &&
                    (platform_audio_input_devices[i].num_channels < device_info->num_channels))
                {
                    continue;
                }
                if ((device_info->sample_sizes != 0 ) &&
                    ((platform_audio_input_devices[i].sample_sizes & device_info->sample_sizes) == 0))
                {
                    continue;
                }
                if ((device_info->sample_rates != 0 ) &&
                    ((platform_audio_input_devices[i].sample_rates & device_info->sample_rates) == 0))
                {
                    continue;
                }

                return platform_audio_input_devices[i].device_id;
            }
        }
#endif
    }
    else if (device_info->direction == PLATFORM_AUDIO_DEVICE_OUTPUT)
    {
#if (PLATFORM_AUDIO_NUM_OUTPUTS > 0)
        int i;
        for (i = 0; i < PLATFORM_AUDIO_NUM_OUTPUTS; i++)
        {
            if ((platform_audio_output_devices[i].direction == device_info->direction) &&
                (platform_audio_output_devices[i].port_type == device_info->port_type))
            {
                if ((device_info->num_channels != 0 ) &&
                    (platform_audio_output_devices[i].num_channels < device_info->num_channels))
                {
                    continue;
                }
                if ((device_info->sample_sizes != 0 ) &&
                    ((platform_audio_output_devices[i].sample_sizes & device_info->sample_sizes) == 0))
                {
                    continue;
                }
                if ((device_info->sample_rates != 0 ) &&
                    ((platform_audio_output_devices[i].sample_rates & device_info->sample_rates) == 0))
                {
                    continue;
                }

                return platform_audio_output_devices[i].device_id;
            }
        }
#endif
    }
    return AUDIO_DEVICE_ID_NONE;
}

/** Get Device information using the device id
 *
 * @param[in]       device_id : platform_audio_device_id_t
 *
 * @return  ptr to device info structure
 *          NULL on error
 */
const platform_audio_device_info_t* platform_audio_device_get_info_by_id(platform_audio_device_id_t device_id)
{
    int i;

    if (device_id < AUDIO_DEVICE_ID_OUTPUT_NONE)
    {
        for (i = 0; i < PLATFORM_AUDIO_NUM_INPUTS; i++)
        {
            if (platform_audio_input_devices[i].device_id == device_id)
            {
                return &platform_audio_input_devices[i];
            }
        }
    }
    else if ((device_id > AUDIO_DEVICE_ID_OUTPUT_NONE) &&
             (device_id < AUDIO_DEVICE_ID_MAX))
    {
        for (i = 0; i < PLATFORM_AUDIO_NUM_OUTPUTS; i++)
        {
            if (platform_audio_output_devices[i].device_id == device_id)
            {
                return &platform_audio_output_devices[i];
            }
        }
    }
    return NULL;
}

/*************************************************************
 *              Utility routines
 ************************************************************ */

/** Get a string of the Port type for debug logging
 *
 * @param[in]   type    : platform_audio_port_type_t
 * @return      description string ("Unknown" on error)
 */
const char* platform_audio_device_get_port_string(platform_audio_port_type_t type)
{
    switch(type)
    {
        case PLATFORM_AUDIO_LINE:
            return "Line";
        case PLATFORM_AUDIO_DIFFERENTIAL_LINE:
            return "Differential Line";
        case PLATFORM_AUDIO_MIC:
            return "Mic";
        case PLATFORM_AUDIO_DIFFERENTIAL_MIC:
            return "Differential Mic";
        case PLATFORM_AUDIO_DIGITAL_MIC:
            return "Digital Mic";
        case PLATFORM_AUDIO_HEADPHONE:
            return "Headphones";
        case PLATFORM_AUDIO_I2S:
            return "I2S";
        case PLATFORM_AUDIO_SPDIF:
            return "SPDIF";
        default:
            return "UNKNOWN";
    };
}

/** Get a string of the sample rates for debug logging
 *
 * @param[in]   sample_rates  : platform_audio_sample_rates_t
 * @return      description string
 */
const char* platform_audio_device_get_sample_rates_string(platform_audio_sample_rates_t sample_rates)
{
        /* if you add to this, make sure built_rates_string[PLATFORM_AUDIO_RATES_STRING_MAX] is big enough! */
    built_rates_string[0] = '\0';

#define BUILT_RATE_STRING_BYTES_REMAINING (sizeof(built_rates_string) - strlen(built_rates_string) - 1)
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_8KHZ)      ?  " 8"     : "  "     ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_11_025KHZ) ?  " 11.025": "       "), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_12KHZ)     ?  " 12"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_16KHZ)     ?  " 16"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_22_05KHZ)  ?  " 22.05" : "      " ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_24KHZ)     ?  " 24"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_32KHZ)     ?  " 32"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_44_1KHZ)   ?  " 44.1"  : "     "  ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_48KHZ)     ?  " 48"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_64KHZ)     ?  " 64"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_88_2KHZ)   ?  " 88.2"  : "     "  ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_96KHZ)     ?  " 96"    : "   "    ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_128KHZ)    ? " 128"    : "    "   ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_176_4KHZ)  ? " 176.4"  : "      " ), BUILT_RATE_STRING_BYTES_REMAINING);
    strncat(built_rates_string, ((sample_rates & PLATFORM_AUDIO_SAMPLE_RATE_192KHZ)    ? " 192"    : "    "   ), BUILT_RATE_STRING_BYTES_REMAINING);
#undef BUILT_RATE_STRING_BYTES_REMAINING

    return built_rates_string;
}

/** Get a string of the sample sizes for debug logging
 *
 * @param[in]   sample_sizes  : platform_audio_sample_sizes_t
 * @return      description string
 */
const char* platform_audio_device_get_sample_sizes_string(platform_audio_sample_sizes_t sample_sizes)
{
    /* if you add to this, make sure built_sizes_string[PLATFORM_AUDIO_SIZES_STRING_MAX]    is big enough! */
    built_sizes_string[0] = '\0';

#define BUILT_SIZES_STRING_BYTES_REMAINING (sizeof(built_sizes_string) - strlen(built_sizes_string) - 1)
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_8_BIT)  ?  " 8" :  "  "), BUILT_SIZES_STRING_BYTES_REMAINING);
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_10_BIT) ? " 10" : "   "), BUILT_SIZES_STRING_BYTES_REMAINING);
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_16_BIT) ? " 16" : "   "), BUILT_SIZES_STRING_BYTES_REMAINING);
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_20_BIT) ? " 20" : "   "), BUILT_SIZES_STRING_BYTES_REMAINING);
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_24_BIT) ? " 24" : "   "), BUILT_SIZES_STRING_BYTES_REMAINING);
    strncat(built_sizes_string, ((sample_sizes & PLATFORM_AUDIO_SAMPLE_SIZE_32_BIT) ? " 32" : "   "), BUILT_SIZES_STRING_BYTES_REMAINING);
#undef BUILT_SIZES_STRING_BYTES_REMAINING

    return built_sizes_string;
}

/** Print audio device info
 *
 * @param audio_input_device_id     : current audio input device (AUDIO_DEVICE_ID_NONE - don't print input information)
 * @param in_dirty                  : !=0, show input device as dirty (not saved to Application DCT)
 * @param audio_output_device_id    : current audio output device (AUDIO_DEVICE_ID_NONE - don't print output information)
 * @param out_dirty                 : !=0, show output device as dirty (not saved to Application DCT)
 * @param verbose                   : print multi-line information
 *
 */
void platform_audio_print_device_list(const platform_audio_device_id_t audio_input_device_id, uint8_t in_dirty,
                                      const platform_audio_device_id_t audio_output_device_id, uint8_t out_dirty,
                                      int verbose)
{

#if (PLATFORM_AUDIO_NUM_INPUTS > 0)
    if ((PLATFORM_AUDIO_NUM_INPUTS > 0) &&
        (audio_input_device_id != AUDIO_DEVICE_ID_NONE))
    {
        uint16_t  i;
        const platform_audio_device_info_t* device_info;
        WPRINT_APP_INFO(("    Audio Input Device(s):\n"));
        for (i = 0; i < PLATFORM_AUDIO_NUM_INPUTS; i++)
        {
            device_info = platform_audio_device_get_info(PLATFORM_AUDIO_DEVICE_INPUT, i);
            if (device_info != NULL)
            {
                if (verbose == 0)
                {
                    if (device_info->device_id == audio_input_device_id)
                    {
                        WPRINT_APP_INFO(("       0x%02x %s  %s %c\n", device_info->device_id,
                                                   device_info->device_name, device_info->description,
                                                   (in_dirty != 0) ? '*' : ' '));
                    }
                }
                else
                {
                    WPRINT_APP_INFO(("   %d:  0x%02x %s  %s      %s\n", i, device_info->device_id,
                                                   device_info->device_name, device_info->description,
                                                   (device_info->device_id == audio_input_device_id) ? "<== Current" : ""));
                    WPRINT_APP_INFO(("                input type: %s\n", platform_audio_device_get_port_string(device_info->port_type)));
                    WPRINT_APP_INFO(("              num channels: %d\n", device_info->num_channels));
                    WPRINT_APP_INFO(("        sample rates (kHz):%s\n", platform_audio_device_get_sample_rates_string(device_info->sample_rates)));
                    WPRINT_APP_INFO(("       sample sizes (bits):%s\n", platform_audio_device_get_sample_sizes_string(device_info->sample_sizes)));
                }
            }
        }
    }
#endif

#if (PLATFORM_AUDIO_NUM_OUTPUTS > 0)
    if ((PLATFORM_AUDIO_NUM_OUTPUTS > 0) &&
        (audio_output_device_id != AUDIO_DEVICE_ID_NONE) &&
        (audio_output_device_id != AUDIO_DEVICE_ID_OUTPUT_NONE))
    {
        uint16_t  i;
        const platform_audio_device_info_t* device_info;
        WPRINT_APP_INFO(("    Audio Output Device(s):\n"));
        for (i = 0; i < PLATFORM_AUDIO_NUM_OUTPUTS; i++)
        {
            device_info = platform_audio_device_get_info(PLATFORM_AUDIO_DEVICE_OUTPUT, i);
            if (device_info != NULL)
            {
                if (verbose == 0)
                {
                    if (device_info->device_id == audio_output_device_id)
                    {
                        WPRINT_APP_INFO(("       0x%02x %s  %s %c\n", device_info->device_id,
                                                   device_info->device_name, device_info->description,
                                                   (out_dirty != 0) ? '*' : ' '));
                    }
                }
                else
                {
                    WPRINT_APP_INFO(("   %d:  0x%02x %s  %s      %s\n", i, device_info->device_id,
                                                   device_info->device_name, device_info->description,
                                                   (device_info->device_id == audio_output_device_id) ? "<== Current" : ""));
                    WPRINT_APP_INFO(("               output type: %s\n", platform_audio_device_get_port_string(device_info->port_type)));
                    WPRINT_APP_INFO(("              num channels: %d\n", device_info->num_channels));
                    WPRINT_APP_INFO(("        sample rates (kHz):%s\n", platform_audio_device_get_sample_rates_string(device_info->sample_rates)));
                    WPRINT_APP_INFO(("       sample sizes (bits):%s\n", platform_audio_device_get_sample_sizes_string(device_info->sample_sizes)));
                }
            }
        }
    }
#endif

    /* if no audio devices defined */
    UNUSED_PARAMETER(audio_input_device_id);
    UNUSED_PARAMETER(in_dirty);
    UNUSED_PARAMETER(audio_output_device_id);
    UNUSED_PARAMETER(out_dirty);
    UNUSED_PARAMETER(verbose);
}

/** Get total number of audio devices
 *
 * @return device count @ref uint8_t
 */
uint8_t platform_audio_get_device_count(void)
{
    return ((uint8_t)PLATFORM_AUDIO_NUM_OUTPUTS + (uint8_t)PLATFORM_AUDIO_NUM_INPUTS);
}
