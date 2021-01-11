/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * Low complexity subband codec (SBC) A2DP Application Programming Interface
 *
 */
#pragma once

/**
 * @cond DUAL_MODE
 * @addtogroup  wicedbt_a2dp_sbc    A2DP SBC Support
 * @ingroup     wicedbt_av_a2d_helper
 * This section describes A2DP Low complexity subband codec (SBC) API
 * @{
 */

/*****************************************************************************
**  Constants
*****************************************************************************/
/** the length of the SBC Media Payload header. */
#define A2D_SBC_MPL_HDR_LEN         1

/** the LOSC of SBC media codec capabilitiy */
#define A2D_SBC_INFO_LEN            6

/* for Codec Specific Information Element */
/**
 * @anchor A2D_SBC_SF
 * @name SBC Sampling Frequency.
 * @{
 */
#define A2D_SBC_IE_SAMP_FREQ_MSK    0xF0    /**< b7-b4 sampling frequency */
#define A2D_SBC_IE_SAMP_FREQ_16     0x80    /**< b7:16  kHz */
#define A2D_SBC_IE_SAMP_FREQ_32     0x40    /**< b6:32  kHz */
#define A2D_SBC_IE_SAMP_FREQ_44     0x20    /**< b5:44.1kHz */
#define A2D_SBC_IE_SAMP_FREQ_48     0x10    /**< b4:48  kHz */
/** @} A2D_SBC_SF */

/**
 * @anchor A2D_SBC_CH_MD
 * @name SBC Channel Mode.
 * @{
 */
#define A2D_SBC_IE_CH_MD_MSK        0x0F    /**< b3-b0 channel mode */
#define A2D_SBC_IE_CH_MD_MONO       0x08    /**< b3: mono */
#define A2D_SBC_IE_CH_MD_DUAL       0x04    /**< b2: dual */
#define A2D_SBC_IE_CH_MD_STEREO     0x02    /**< b1: stereo */
#define A2D_SBC_IE_CH_MD_JOINT      0x01    /**< b0: joint stereo */
/** @} A2D_SBC_CH_MD */

/**
 * @anchor A2D_SBC_BLOCK_LEN
 * @name SBC Block Length.
 * @{
 */
#define A2D_SBC_IE_BLOCKS_MSK       0xF0    /**< b7-b4 number of blocks */
#define A2D_SBC_IE_BLOCKS_4         0x80    /**< 4 blocks */
#define A2D_SBC_IE_BLOCKS_8         0x40    /**< 8 blocks */
#define A2D_SBC_IE_BLOCKS_12        0x20    /**< 12blocks */
#define A2D_SBC_IE_BLOCKS_16        0x10    /**< 16blocks */
/** @} A2D_SBC_BLOCK_LEN */

/**
 * @anchor A2D_SBC_SUBBAND_NUM
 * @name SBC Subband Number.
 * @{
 */
#define A2D_SBC_IE_SUBBAND_MSK      0x0C    /**< b3-b2 number of subbands */
#define A2D_SBC_IE_SUBBAND_4        0x08    /**< b3: 4 */
#define A2D_SBC_IE_SUBBAND_8        0x04    /**< b2: 8 */
/** @} A2D_SBC_SUBBAND_NUM */

/**
 * @anchor A2D_SBC_ALLOC_MD
 * @name SBC Allocation Method.
 * @{
 */
#define A2D_SBC_IE_ALLOC_MD_MSK     0x03    /**< b1-b0 allocation mode */
#define A2D_SBC_IE_ALLOC_MD_S       0x02    /**< b1: SNR */
#define A2D_SBC_IE_ALLOC_MD_L       0x01    /**< b0: loundess */
/** @} A2D_SBC_ALLOC_MD */

#define A2D_SBC_IE_MIN_BITPOOL      2       /**< Min Bitpool Value */
#define A2D_SBC_IE_MAX_BITPOOL      250     /**< Max Bitpool Value */

/* for media payload header */
#define A2D_SBC_HDR_F_MSK           0x80    /**< SBC Fragmented packet mask */
#define A2D_SBC_HDR_S_MSK           0x40    /**< SBC starting packet mask*/
#define A2D_SBC_HDR_L_MSK           0x20    /**< SBC last packet mask */
#define A2D_SBC_HDR_NUM_MSK         0x0F    /**< SBC number of frame mask */

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/** data type for the SBC Codec Information Element*/
typedef struct
{
    uint8_t   samp_freq;      /**< Sampling frequency \ref A2D_SBC_SF */
    uint8_t   ch_mode;        /**< Channel mode \ref A2D_SBC_CH_MD */
    uint8_t   block_len;      /**< Block length \ref A2D_SBC_BLOCK_LEN */
    uint8_t   num_subbands;   /**< Number of subbands \ref A2D_SBC_SUBBAND_NUM */
    uint8_t   alloc_mthd;     /**< Allocation method \ref A2D_SBC_ALLOC_MD */
    uint8_t   max_bitpool;    /**< Maximum bitpool */
    uint8_t   min_bitpool;    /**< Minimum bitpool */
} wiced_bt_a2d_sbc_cie_t;



#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Check if control block descrambling needs to be initiated
 *
 * @param[in]       p_pkt   : Pointer to the incoming data
 *
 * @return          None
 */
void wiced_bt_a2d_sbc_chk_fr_init(uint8_t *p_pkt);

/**
 *
 * Descramble packet
 *
 * @param[in]       p_pkt   : Pointer to the incoming data
 * @param[in]       len     : Size of the data
 *
 * @return          None
 */
void wiced_bt_a2d_sbc_descramble(uint8_t *p_pkt, uint16_t len);

/**
 * Build SBC Media Codec Capabilities byte sequence
 * (beginning from the LOSC octet)
 *
 * @param[in]       media_type  : Media type, audio or multimedia
 * @param[in]       p_ie        : Pointer to the SBC codec information element
 * @param[out]      p_result    : Pointer to the built codec info byte stream
 *
 * @return          Status code (see @ref A2D_STATUS "A2DP status codes")
 *                  <b> A2D_SUCCESS </b> if successful, otherwise error.
 */
wiced_bt_a2d_status_t wiced_bt_a2d_bld_sbc_info(uint8_t media_type, const wiced_bt_a2d_sbc_cie_t *p_ie,
                                                uint8_t *p_result);

/**
 * Parse SBC Media Codec Capabilities byte sequence
 * (beginning from the LOSC octet)
 *
 * @param[in]       p_info      : Pointer to the byte stream to parse
 * @param[in]       for_caps    : True if the byte stream is for get capabilities response
 * @param[out]      p_ie        : Pointer to the parsed codec info byte sequence
 *
 * @return          Status code (see @ref A2D_STATUS "A2DP status codes")
 *                  <b> A2D_SUCCESS </b> if successful, otherwise error.
 */
wiced_bt_a2d_status_t wiced_bt_a2d_pars_sbc_info(wiced_bt_a2d_sbc_cie_t *p_ie, uint8_t *p_info,
                                                 wiced_bool_t for_caps);

/**
 *
 * Build SBC Media Payload header
 *
 * @param[in]       frag    : 1, if fragmented. 0, otherwise.
 * @param[in]       start   : 1, if the starting packet of a fragmented frame.
 * @param[in]       last    : 1, if the last packet of a fragmented frame.
 * @param[in]       num     : If frag is 1, this is the number of remaining fragments
 *                            (including this fragment) of this frame.
 *                            If frag is 0, this is the number of frames in this packet.
 * @param[out]      p_dst   : Pointer to the built media payload header byte
 *
 * @return          None
 */
void wiced_bt_a2d_bld_sbc_mpl_hdr(uint8_t *p_dst, wiced_bool_t frag, wiced_bool_t start,
                                  wiced_bool_t last, uint8_t num);

/**
 *
 * Parse SBC Media Payload header
 *
 * @param[in]       p_src   : Pointer to the byte stream to parse
 * @param[out]      p_frag  : 1, if fragmented. 0, otherwise.
 * @param[out]      p_start : 1, if the starting packet of a fragmented frame.
 * @param[out]      p_last  : 1, if the last packet of a fragmented frame.
 * @param[out]      p_num   : If frag is 1, this is the number of remaining fragments
 *                            (including this fragment) of this frame.
 *                            If frag is 0, this is the number of frames in this packet.
 *
 * @return          None
 */
void wiced_bt_a2d_pars_sbc_mpl_hdr(uint8_t *p_src, wiced_bool_t *p_frag,
                                   wiced_bool_t *p_start, wiced_bool_t *p_last,
                                   uint8_t *p_num);
#ifdef __cplusplus
}
#endif

/** @} wicedbt_a2dp_sbc */
/* @endcond*/

