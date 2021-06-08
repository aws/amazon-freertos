/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * MPEG-2, 4 AAC A2DP Application Programming Interface
 *
 */
#pragma once

/**
 * @cond DUAL_MODE
 * @addtogroup  wicedbt_a2dp_mpeg_2_4    MPEG-2,4 AAC Support
 * @ingroup     wicedbt_av_a2d_helper
 * This section describes A2DP MPEG-2,4 AAC Audio codec API
 * @{
 */

/*****************************************************************************
**  Constants
*****************************************************************************/

/** the LOSC of MPEG_2, 4 AAC media codec capabilitiy */
#define A2D_M24_INFO_LEN            8

/** for Codec Specific Information Element */

/**
 * @anchor A2D_M24_OBJ
 * @name MPEG-2,4 AAC Object
 * @{
 */
#define A2D_M24_IE_OBJ_MSK          0xF0    /**< b7-b4 object type. b3-b0 is RFA,not used */
#define A2D_M24_IE_OBJ_2LC          0x80    /**< b7: MPEG-2 AAC LC */
#define A2D_M24_IE_OBJ_4LC          0x40    /**< b6: MPEG-4 AAC LC */
#define A2D_M24_IE_OBJ_4LTP         0x20    /**< b5: MPEG-4 AAC LTP */
#define A2D_M24_IE_OBJ_4S           0x10    /**< b4: MPEG-4 AAC scalable */
/** @} A2D_M24_OBJ */

/**
 * @anchor A2D_M24_SF
 * @name MPEG-2,4 AAC Sampling Frequency
 * @{
 */
#define A2D_M24_IE_SAMP_FREQ_MSK    0xFFF0    /**< sampling frequency */
#define A2D_M24_IE_SAMP_FREQ_8      0x8000    /**< b7:8  kHz */
#define A2D_M24_IE_SAMP_FREQ_11     0x4000    /**< b6:11  kHz */
#define A2D_M24_IE_SAMP_FREQ_12     0x2000    /**< b5:12  kHz */
#define A2D_M24_IE_SAMP_FREQ_16     0x1000    /**< b4:16  kHz */
#define A2D_M24_IE_SAMP_FREQ_22     0x0800    /**< b3:22.05kHz */
#define A2D_M24_IE_SAMP_FREQ_24     0x0400    /**< b2:24  kHz */
#define A2D_M24_IE_SAMP_FREQ_32     0x0200    /**< b1:32  kHz */
#define A2D_M24_IE_SAMP_FREQ_44     0x0100    /**< b0:44.1kHz */
#define A2D_M24_IE_SAMP_FREQ_48     0x0080    /**< b7:48  kHz */
#define A2D_M24_IE_SAMP_FREQ_64     0x0040    /**< b6:64  kHz */
#define A2D_M24_IE_SAMP_FREQ_88     0x0020    /**< b5:88  kHz */
#define A2D_M24_IE_SAMP_FREQ_96     0x0010    /**< b4:96  kHz */
/** @} A2D_M24_SF */

/**
 * @anchor A2D_M24_CH_MD
 * @name MPEG-2,4 AAC Channel
 * @{
 */
#define A2D_M24_IE_CHNL_MSK         0x0C    /**< b3-b2 channels */
#define A2D_M24_IE_CHNL_1           0x08    /**< b3: 1 channel */
#define A2D_M24_IE_CHNL_2           0x04    /**< b2: 2 channels */
/** @} A2D_M24_CH_MD */

#define A2D_M24_IE_VBR_MSK          0x80    /**< b7: VBR */

/**
 * @anchor A2D_M24_BITRATE
 * @name MPEG-2,4 AAC Bitrate
 * @{
 */
#define A2D_M24_IE_BITRATE3_MSK     0x7F0000    /**< octect3*/
#define A2D_M24_IE_BITRATE45_MSK    0x00FFFF    /**< octect4, 5*/
#define A2D_M24_IE_BITRATE_MSK      0x7FFFFF    /**< b7-b0 of octect 3, all of octect4, 5*/
/** @} A2D_M24_BITRATE */


/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/** data type for the MPEG-2, 4 AAC Codec Information Element*/
typedef struct
{
    uint8_t       obj_type;   /**< Object type \ref A2D_M24_OBJ */
    uint16_t      samp_freq;  /**< Sampling frequency \ref A2D_M24_SF*/
    uint8_t       chnl;       /**< Channel mode \ref A2D_M24_CH_MD */
    uint8_t       vbr;        /**< Variable Bit Rate */
    uint32_t      bitrate;    /**< Bit rate index \ref A2D_M24_BITRATE */
} wiced_bt_a2d_m24_cie_t;

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/**
 *
 * This function is called by an application to build
 * the MPEG-2, 4 AAC Media Codec Capabilities byte sequence
 * beginning from the LOSC octet.
 *
 * @param[in]  media_type : Indicates Audio, or Multimedia.
 * @param[in]  p_ie       : MPEG-2, 4 AAC Codec Information Element information.
 * @param[out] p_result   : the resulting codec info byte sequence.
 *
 * @return    <b> A2D_SUCCESS </b> if function execution succeeded. \n
 *            <b> Error status code </b>, otherwise.
 */
wiced_bt_a2d_status_t wiced_bt_a2d_bld_m24info(uint8_t media_type, wiced_bt_a2d_m24_cie_t *p_ie,
                                               uint8_t *p_result);

/**
 *
 * This function is called by an application to parse
 * the MPEG-2, 4 AAC Media Codec Capabilities byte sequence
 * beginning from the LOSC octet.
 *
 * @param[in]  p_info   :  the byte sequence to parse.
 * @param[in]  for_caps :  TRUE, if the byte sequence is for get capabilities response.
 * @param[out] p_ie     :  MPEG-2, 4 AAC Codec Information Element information.
 *
 * @return     <b> A2D_SUCCESS </b> if function execution succeeded. \n
 *             <b> Error status code </b>, otherwise.
 */
wiced_bt_a2d_status_t wiced_bt_a2d_pars_m24info(wiced_bt_a2d_m24_cie_t *p_ie, uint8_t *p_info,
                                               wiced_bool_t for_caps);

/** @} wicedbt_a2dp_mpeg_2_4 */
/* @endcond*/

#ifdef __cplusplus
}
#endif
