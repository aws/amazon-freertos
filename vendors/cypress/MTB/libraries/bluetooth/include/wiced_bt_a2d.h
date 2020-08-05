/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Bluetooth A2DP Application Programming Interface
 *
 */
#pragma once

#include "wiced_bt_sdp.h"

/**
 *
 * @defgroup    wicedbt_av_a2d_helper      A2DP Helper Functions
 * @ingroup     wicedbt_av_helper
 *
 * @addtogroup  wicedbt_av_a2d_helper        A2DP Profile Helper Functions
 * @ingroup     wicedbt_av_helper
 *
 * Advanced Audio Distribution Profile
 *
 * @{
 */
/*****************************************************************************
**  constants
*****************************************************************************/

/* Profile supported features */
#define A2D_SUPF_PLAYER     0x0001
#define A2D_SUPF_MIC        0x0002
#define A2D_SUPF_TUNER      0x0004
#define A2D_SUPF_MIXER      0x0008

#define A2D_SUPF_HEADPHONE  0x0001
#define A2D_SUPF_SPEAKER    0x0002
#define A2D_SUPF_RECORDER   0x0004
#define A2D_SUPF_AMP        0x0008

/* AV Media Types */
#define A2D_MEDIA_TYPE_AUDIO    0x00    /* audio media type + RFA */
#define A2D_MEDIA_TYPE_VIDEO    0x10    /* video media type + RFA */
#define A2D_MEDIA_TYPE_MULTI    0x20    /* multimedia media type + RFA */

/* AV Media Codec Type (Audio Codec ID) */
#define A2D_MEDIA_CT_SBC        0x00    /* SBC media codec type */
#define A2D_MEDIA_CT_M12        0x01    /* MPEG-1, 2 Audio media codec type */
#define A2D_MEDIA_CT_M24        0x02    /* MPEG-2, 4 AAC media codec type */
#define A2D_MEDIA_CT_ATRAC      0x04    /* ATRAC family media codec type */
#define A2D_MEDIA_CT_VEND       0xFF    /* Vendor specific */
#define A2D_MEDIA_CT_APTX       A2D_MEDIA_CT_VEND    /* APTX media codec type */


/**
 * @anchor A2D_STATUS
 * @name A2DP status codes.
 * @{
 */

#define A2D_SUCCESS           0     /**< Success */
#define A2D_FAIL              0x0A  /**< Failed */
#define A2D_BUSY              0x0B  /**< wiced_bt_a2d_find_service is already in progress */
#define A2D_INVALID_PARAMS    0x0C  /**< bad parameters */
#define A2D_WRONG_CODEC       0x0D  /**< wrong codec info */
#define A2D_BAD_CODEC_TYPE    0xC1  /**< Media Codec Type is not valid  */
#define A2D_NS_CODEC_TYPE     0xC2  /**< Media Codec Type is not supported */
#define A2D_BAD_SAMP_FREQ     0xC3  /**< Sampling Frequency is not valid or multiple values have been selected  */
#define A2D_NS_SAMP_FREQ      0xC4  /**< Sampling Frequency is not supported  */
#define A2D_BAD_CH_MODE       0xC5  /**< Channel Mode is not valid or multiple values have been selected  */
#define A2D_NS_CH_MODE        0xC6  /**< Channel Mode is not supported */
#define A2D_BAD_SUBBANDS      0xC7  /**< None or multiple values have been selected for Number of Subbands */
#define A2D_NS_SUBBANDS       0xC8  /**< Number of Subbands is not supported */
#define A2D_BAD_ALLOC_MTHD    0xC9  /**< None or multiple values have been selected for Allocation Method */
#define A2D_NS_ALLOC_MTHD     0xCA  /**< Allocation Method is not supported */
#define A2D_BAD_MIN_BITPOOL   0xCB  /**< Minimum Bitpool Value is not valid */
#define A2D_NS_MIN_BITPOOL    0xCC  /**< Minimum Bitpool Value is not supported */
#define A2D_BAD_MAX_BITPOOL   0xCD  /**< Maximum Bitpool Value is not valid */
#define A2D_NS_MAX_BITPOOL    0xCE  /**< Maximum Bitpool Value is not supported */
#define A2D_BAD_LAYER         0xCF  /**< None or multiple values have been selected for Layer */
#define A2D_NS_LAYER          0xD0  /**< Layer is not supported */
#define A2D_NS_CRC            0xD1  /**< CRC is not supported */
#define A2D_NS_MPF            0xD2  /**< MPF-2 is not supported */
#define A2D_NS_VBR            0xD3  /**< VBR is not supported */
#define A2D_BAD_BIT_RATE      0xD4  /**< None or multiple values have been selected for Bit Rate */
#define A2D_NS_BIT_RATE       0xD5  /**< Bit Rate is not supported */
#define A2D_BAD_OBJ_TYPE      0xD6  /**< Either 1) Object type is not valid (b3-b0) or 2) None or multiple values have been selected for Object Type */
#define A2D_NS_OBJ_TYPE       0xD7  /**< Object type is not supported */
#define A2D_BAD_CHANNEL       0xD8  /**< None or multiple values have been selected for Channels */
#define A2D_NS_CHANNEL        0xD9  /**< Channels is not supported */
#define A2D_BAD_BLOCK_LEN     0xDD  /**< None or multiple values have been selected for Block Length */
#define A2D_BAD_CP_TYPE       0xE0  /**< The requested CP Type is not supported. */
#define A2D_BAD_CP_FORMAT     0xE1  /**< The format of Content Protection Service Capability/Content Protection Scheme Dependent Data is not correct. */

typedef uint8_t wiced_bt_a2d_status_t;

/** @} A2D_STATUS */

/* the return values from wiced_bt_a2d_bits_set() */
#define A2D_SET_ONE_BIT         1   /* one and only one bit is set */
#define A2D_SET_ZERO_BIT        0   /* all bits clear */
#define A2D_SET_MULTL_BIT       2   /* multiple bits are set */

/*****************************************************************************
**  type definitions
*****************************************************************************/

/*****************************************************************************
**  external function declarations
*****************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif
/*
 *
 * Function         wiced_bt_a2d_set_trace_level
 *
 *                  Sets the trace level for A2D. If 0xff is passed, the
 *                  current trace level is returned.
 *
 * @param[in]       new_level:  The level to set the A2D tracing to:
 *                              0xff-returns the current setting.
 *                              0-turns off tracing.
 *                              >= 1-Errors.
 *                              >= 2-Warnings.
 *                              >= 3-APIs.
 *                              >= 4-Events.
 *                              >= 5-Debug.
 *
 * @return          The new trace level or current trace level if
 *                  the input parameter is 0xff.
 *
 */
uint8_t wiced_bt_a2d_set_trace_level (uint8_t new_level);

/**
 * Function         wiced_bt_a2d_bits_set
 *
 *                  Check the number of bits set in a given mask (used to parse stream configuration masks)
 *
 * @param[in]       mask :  mask to check
 *
 * @return          A2D_SET_ONE_BIT, if one and only one bit is set
 *                  A2D_SET_ZERO_BIT, if all bits clear
 *                  A2D_SET_MULTL_BIT, if multiple bits are set
 */
uint8_t wiced_bt_a2d_bits_set(uint8_t mask);

#ifdef __cplusplus
}
#endif

/** @} wicedbt_a2dp */
