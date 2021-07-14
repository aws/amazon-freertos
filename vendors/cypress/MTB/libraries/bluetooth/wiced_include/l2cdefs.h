/****************************************************************************
**
**  Name:       l2cdefs.h
**
**   Function:   This file contains L2CAP protocol definitions
**
**
**
**  Copyright (c) 1999-2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#ifndef L2CDEFS_H
#define L2CDEFS_H

/**
 * @anchor L2CAP_CMD_CODE
 * @name L2CAP command codes
 * @{
 *
 * L2CAP command codes.
 */
#define L2CAP_CMD_REJECT                    0x01    /**< Reject Command */
#define L2CAP_CMD_CONN_REQ                  0x02    /**< Connection Request Command */
#define L2CAP_CMD_CONN_RSP                  0x03    /**< Connection Response */
#define L2CAP_CMD_CONFIG_REQ                0x04    /**< Configuration Request */
#define L2CAP_CMD_CONFIG_RSP                0x05    /**< Configuration Response */
#define L2CAP_CMD_DISC_REQ                  0x06    /**< Disconnect Request */
#define L2CAP_CMD_DISC_RSP                  0x07    /**< Disconnect Response */
#define L2CAP_CMD_ECHO_REQ                  0x08    /**< Echo Request */
#define L2CAP_CMD_ECHO_RSP                  0x09    /**< Echo Response */
#define L2CAP_CMD_INFO_REQ                  0x0A    /**< Command Information Request */
#define L2CAP_CMD_INFO_RSP                  0x0B    /**< Command Information Response */
#define L2CAP_CMD_AMP_CONN_REQ              0x0C    /**< AMP Connection Request */
#define L2CAP_CMD_AMP_CONN_RSP              0x0D    /**< AMP Connection Response */
#define L2CAP_CMD_AMP_MOVE_REQ              0x0E    /**< AMP Move Request */
#define L2CAP_CMD_AMP_MOVE_RSP              0x0F    /**< AMP Move Response */
#define L2CAP_CMD_AMP_MOVE_CFM              0x10    /**< AMP Move Confirm */
#define L2CAP_CMD_AMP_MOVE_CFM_RSP          0x11    /**< AMP Move Confirm Response */
#define L2CAP_CMD_BLE_UPDATE_REQ            0x12    /**< BLE Connection Param Update Request */
#define L2CAP_CMD_BLE_UPDATE_RSP            0x13    /**< BLE Connection Param Update Response */
#define L2CAP_CMD_LE_COC_CONN_REQ           0x14    /**< LE COC Connection Request */
#define L2CAP_CMD_LE_COC_CONN_RSP           0x15    /**< LE COC Connection Response */
#define L2CAP_CMD_CRB_CREDITS               0x16    /**< Credits */
#define L2CAP_CMD_ECRB_CONNECT_REQ          0x17    /**< Enhanced credit-based connection request       */
#define L2CAP_CMD_ECRB_CONNECT_RSP          0x18    /**< Enhanced credit-based connection response      */
#define L2CAP_CMD_ECRB_RECONFIG_REQ         0x19    /**< Enhanced credit-based reconfiguration request  */
#define L2CAP_CMD_ECRB_RECONFIG_RSP         0x1A    /**< Enhanced credit-based reconfiguration response */
/** @} L2CAP_CMD_CODE */

/**
 * @anchor L2CAP_PKT_HEADER_LEN
 * @name L2CAP command codes
 * @{
 *
 * L2CAP packet and header lengths.
 */
#define L2CAP_PKT_OVERHEAD      4           /**< Length and CID                       */
#define L2CAP_CMD_OVERHEAD      4           /**< Cmd code, Id and length              */
#define L2CAP_CMD_REJECT_LEN    2           /**< Reason (data is optional)            */
#define L2CAP_CONN_REQ_LEN      4           /**< PSM and source CID                   */
#define L2CAP_CONN_RSP_LEN      8           /**< Dest CID, source CID, reason, status */
#define L2CAP_CONFIG_REQ_LEN    4           /**< Dest CID, flags (data is optional)   */
#define L2CAP_CONFIG_RSP_LEN    6           /**< Dest CID, flags, result,data optional*/
#define L2CAP_DISC_REQ_LEN      4           /**< Dest CID, source CID                 */
#define L2CAP_DISC_RSP_LEN      4           /**< Dest CID, source CID                 */
#define L2CAP_ECHO_REQ_LEN      0           /**< Data is optional                     */
#define L2CAP_ECHO_RSP_LEN      0           /**< Data is optional                     */
#define L2CAP_INFO_REQ_LEN      2           /**< Info type                            */
#define L2CAP_INFO_RSP_LEN      4           /**< Info type, result (data is optional) */
#define L2CAP_BCST_OVERHEAD     2           /**< Additional broadcast packet overhead */
#define L2CAP_UCD_OVERHEAD      2           /**< Additional connectionless packet overhead */

#define L2CAP_AMP_CONN_REQ_LEN  5           /**< PSM, CID, and remote controller ID   */
#define L2CAP_AMP_MOVE_REQ_LEN  3           /**< CID and remote controller ID         */
#define L2CAP_AMP_MOVE_RSP_LEN  4           /**< CID and result                       */
#define L2CAP_AMP_MOVE_CFM_LEN  4           /**< CID and result                       */
#define L2CAP_AMP_MOVE_CFM_RSP_LEN  2       /**< CID                                  */

#define L2CAP_CMD_BLE_UPD_REQ_LEN   8       /**< Min and max interval, latency, tout  */
#define L2CAP_CMD_BLE_UPD_RSP_LEN   2       /**< Conn update response length  */

#define L2CAP_LE_COC_CONN_REQ_LEN   10      /**< LE_PSM, src CID, MTU, MPS, init credits  */
#define L2CAP_LE_COC_CONN_RSP_LEN   10      /**< Dest CID, MTU, MPS, init credits, result */
#define L2CAP_CRB_CREDITS_LEN        4      /**< Local CID, connection credits to add                */

#define L2CAP_ECRB_BASE_CONN_REQ_LEN 8      /**< LE_PSM, MTU, MPS, init credits */
#define L2CAP_ECRB_BASE_CONN_RSP_LEN 8      /**< MTU, MPS, init credits, result */
#define L2CAP_ECRB_BASE_RECONFIG_REQ_LEN 4  /**< MTU, MPS */
#define L2CAP_ECRB_RECONFIG_RSP_LEN  2      /**< result */
#define L2CAP_ECRB_MAX_CONN_REQ_LEN 18      /**< LE_PSM, MTU, MPS, init credits + up to 5 CIDs */
#define L2CAP_ECRB_MAX_CONN_RSP_LEN 18      /**< MTU, MPS, init credits, result + up to 5 CIDs */
#define L2CAP_ECRB_MAX_RECONFIG_REQ_LEN 14  /**< MTU, MPS + up to 5 CIDs */
/** @} L2CAP_PKT_HEADER_LEN */

/* Define the packet boundary flags
*/
#define L2CAP_PKT_START_FLUSHABLE       2
#define L2CAP_PKT_START_NON_FLUSHABLE   0
#define L2CAP_COMPLETE_FLUSHABLE_PKT    3       /* complete flushable L2CAP packet */
#define L2CAP_PKT_START                 2
#define L2CAP_PKT_CONTINUE              1
#define L2CAP_MASK_FLAG            0x0FFF
#define L2CAP_PKT_TYPE_SHIFT            12
#define L2CAP_PKT_TYPE_MASK             3

#define L2CAP_BLE_CONN_MIN_MPS_SIZE           23
#define L2CAP_BLE_CONN_MAX_MPS_SIZE           65533
#define L2CAP_BLE_CONN_MIN_MTU_SIZE           23
#define L2CAP_CRB_CONN_SDU_LEN_FIELD_SIZE     2         /* Applies to LE-COC and ECRB channel types */


#define L2CAP_CONN_TIMEOUT           0xEEEE   /**< Connection timeout @note Internal use only */
#define L2CAP_CONN_REJECTED          253      /**< Connection reject @note Internal use only */
#define L2CAP_CONN_AMP_FAILED        254      /**< Connection failed @note Internal use only */
#define L2CAP_CONN_NO_LINK           255      /**< NO Link @note Internal use only */
#define L2CAP_CONN_CANCEL            256      /**< L2CAP connection cancelled @note Internal use only */


/**
 * @anchor L2CAP_MOVE_RESULT
 * @name L2CAP Move Channel Response result codes
 * @{
 *
 * L2CAP Move Channel Response result codes
 */
#define L2CAP_MOVE_OK                   0       /**< OK */
#define L2CAP_MOVE_PENDING              1       /**< Pending */
#define L2CAP_MOVE_CTRL_ID_NOT_SUPPORT  2       /**< Control ID Not Supprted */
#define L2CAP_MOVE_SAME_CTRLR_ID        3       /**< Same Control ID */
#define L2CAP_MOVE_CONFIG_NOT_SUPPORTED 4       /**< Configuration Not Supported */
#define L2CAP_MOVE_CHAN_COLLISION       5       /**< Channel Collision */
#define L2CAP_MOVE_NOT_ALLOWED          6       /**< Not Allowed */
/** @} L2CAP_MOVE_RESULT */


/* Define L2CAP Move Channel Confirmation result codes
*/
#define L2CAP_MOVE_CFM_OK               0   /**< Channel Move confirm Ok */
#define L2CAP_MOVE_CFM_REFUSED          1   /**< Channel Move confirm Refused */


/* Define the L2CAP command reject reason codes
*/
#define L2CAP_CMD_REJ_NOT_UNDERSTOOD    0       /**< Command Not Understood */
#define L2CAP_CMD_REJ_MTU_EXCEEDED      1       /**< MTU Exceeded */
#define L2CAP_CMD_REJ_INVALID_CID       2       /**< Invalid CID */


/**
 * @anchor L2CAP_PREDIFINED_CIDS
 * @name L2CAP Predefined CIDs
 * @{
 *
 * L2CAP Predefined CIDs
 */
#define L2CAP_SIGNALLING_CID            1           /**< Signalling CID */
#define L2CAP_CONNECTIONLESS_CID        2           /**< Connectionless CID */
#define L2CAP_AMP_CID                   3           /**< AMP CID */
#define L2CAP_ATT_CID                   4           /**< Attribute CID */
#define L2CAP_BLE_SIGNALLING_CID        5           /**< BLE Signalling CID */
#define L2CAP_SMP_CID                   6           /**< SMP CID */
#define L2CAP_SMP_BR_CID                7           /**< SMP BR CID */
#define L2CAP_ASSIGNED_NUM_START        0x0020      /**< Channel Assigned Number Start */
#define L2CAP_ASSIGNED_NUM_END          0x003E      /**< Channel Assigned Number End */
#define L2CAP_AMP_TEST_CID              0x003F      /**< AMP Test CID */
#define L2CAP_BASE_APPL_CID             0x0040      /**< Dynamic : Base CID */
#define L2CAP_BLE_CONN_MAX_CID          0x007F      /**< Max BLE CID */
/** @} L2CAP_PREDIFINED_CIDS */

/**
 * @anchor L2CAP_CHAN_MASK
 * @name Fixed Channels mask bits
 * @{
 *
 * Fixed Channels mask bits
 */
#define L2CAP_FIXED_CHNL_SIG_BIT        (1 << L2CAP_SIGNALLING_CID)     /**< Signal Channel Supported (Mandatory) */
#define L2CAP_FIXED_CHNL_CNCTLESS_BIT   (1 << L2CAP_CONNECTIONLESS_CID) /**< Connectionless Reception             */
#define L2CAP_FIXED_CHNL_AMP_BIT        (1 << L2CAP_AMP_CID)            /**< AMP Manager Supported                */
#define L2CAP_FIXED_CHNL_ATT_BIT        (1 << L2CAP_ATT_CID)            /**< Attribute protocol Supported         */
#define L2CAP_FIXED_CHNL_BLE_SIG_BIT    (1 << L2CAP_BLE_SIGNALLING_CID) /**< BLE Signalling Supported             */
#define L2CAP_FIXED_CHNL_SMP_BIT        (1 << L2CAP_SMP_CID)            /**< BLE Security Manager Supported       */
#define L2CAP_FIXED_CHNL_SMP_BR_BIT     (1 << L2CAP_SMP_BR_CID)         /**< Security Manager over BR Supported   */
/** @} L2CAP_CHAN_MASK */


/**
 * @anchor L2CAP_CONFIG_CODE
 * @name L2CAP configuration result codes
 * @{
 *
 * L2CAP configuration result codes
 */
#define L2CAP_CFG_OK                    0       /**< Config OK */
#define L2CAP_CFG_UNACCEPTABLE_PARAMS   1       /**< Unacceptable Parameters */
#define L2CAP_CFG_FAILED_NO_REASON      2       /**< Failed with No reason */
#define L2CAP_CFG_UNKNOWN_OPTIONS       3       /**< Config unknown option */
#define L2CAP_CFG_PENDING               4       /**< Config pending */
#define L2CAP_CFG_FLOW_SPEC_REJECTED    5       /**< spec rejected */
/** @} L2CAP_CONFIG_CODE */


/**
 * @anchor L2CAP_CONFIG_TYPE
 * @name L2CAP configuration option types
 * @{
 *
 * L2CAP configuration option types
 */
#define L2CAP_CFG_TYPE_MTU              0x01        /**< MTU configuration */
#define L2CAP_CFG_TYPE_FLUSH_TOUT       0x02        /**< Flush Timeout configuration */
#define L2CAP_CFG_TYPE_QOS              0x03        /**< QOS configuration */
#define L2CAP_CFG_TYPE_FCR              0x04        /**< Retransmission & FLow Control configuration  */
#define L2CAP_CFG_TYPE_FCS              0x05        /**< Frame Check Sequence configuration */
#define L2CAP_CFG_TYPE_EXT_FLOW         0x06        /**< Extended Flow Specification configuration */
#define L2CAP_CFG_TYPE_EXT_WIN_SIZE     0x07        /**< Extended Windown Size configuration */
/** @} L2CAP_CONFIG_TYPE */

/**
 * @anchor L2CAP_CONFIG_TYPE_LEN
 * @name L2CAP configuration option type lens
 * @{
 *
 * L2CAP configuration option type len
 */
#define L2CAP_CFG_MTU_OPTION_LEN        2           /**< MTU option length    */
#define L2CAP_CFG_FLUSH_OPTION_LEN      2           /**< Flush option len     */
#define L2CAP_CFG_QOS_OPTION_LEN        22          /**< QOS option length    */
#define L2CAP_CFG_FCR_OPTION_LEN        9           /**< FCR option length    */
#define L2CAP_CFG_FCS_OPTION_LEN        1           /**< FCR option length    */
#define L2CAP_CFG_EXT_FLOW_OPTION_LEN   16          /**< Extended Flow Spec   */
#define L2CAP_CFG_EXT_WIN_SIZE_LEN      2           /**< Ext window size length */
#define L2CAP_CFG_OPTION_OVERHEAD       2           /**< Type and length      */
/** @} L2CAP_CONFIG_TYPE_LEN */

/* Configuration Cmd/Rsp Flags mask
*/
#define L2CAP_CFG_FLAGS_MASK_CONT       0x0001      /**< Flags mask: Continuation */

/* FCS Check Option values
*/
#define L2CAP_CFG_FCS_BYPASS            0       /**< Bypass the FCS in streaming or ERTM modes */
#define L2CAP_CFG_FCS_USE               1       /**< Use the FCS in streaming or ERTM modes [default] */

/* Default values for configuration
*/
#define L2CAP_NO_AUTOMATIC_FLUSH        0xFFFF      /**< No Automatic Flush */
#define L2CAP_NO_RETRANSMISSION         0x0001      /**< No Retranmission */

#define L2CAP_DEFAULT_MTU               (672)                       /**< Default MTU */
#define L2CAP_DEFAULT_FLUSH_TO          L2CAP_NO_AUTOMATIC_FLUSH    /**< Default Flush settings */
#define L2CAP_DEFAULT_SERV_TYPE         1                           /**< Default service type */
#define L2CAP_DEFAULT_TOKEN_RATE        0                           /**< Default Token rate */
#define L2CAP_DEFAULT_BUCKET_SIZE       0                           /**< Default Bucket size */
#define L2CAP_DEFAULT_PEAK_BANDWIDTH    0                           /**< Default Peak bandwidth */
#define L2CAP_DEFAULT_LATENCY           0xFFFFFFFF                  /**< Default Letency */
#define L2CAP_DEFAULT_DELAY             0xFFFFFFFF                  /**< Default Delay */
#define L2CAP_DEFAULT_FCS               L2CAP_CFG_FCS_USE           /**< Default FCS settings */


/* Define the L2CAP disconnect result codes
*/
#define L2CAP_DISC_OK                   0               /**< L2CAP Disconnect Success */
#define L2CAP_DISC_TIMEOUT              0xEEEE          /**< L2CAP Disconnection Timeout */

/* Define the L2CAP info resp result codes
*/
#define L2CAP_INFO_RESP_RESULT_SUCCESS          0       /**< L2CAP Info response success */
#define L2CAP_INFO_RESP_RESULT_NOT_SUPPORTED    1       /**< L2CAP info response result not supported */

/* Define the info-type fields of information request & response
*/
#define L2CAP_CONNLESS_MTU_INFO_TYPE      0x0001    /**< Connection less MTU info type */
#define L2CAP_EXTENDED_FEATURES_INFO_TYPE 0x0002    /**< Extended feature Info Type. Used in Information Req/Response */
#define L2CAP_FIXED_CHANNELS_INFO_TYPE    0x0003    /**< Fix channel Info Type. Used in AMP                      */

#define L2CAP_CONNLESS_MTU_INFO_SIZE           2    /**< Connectionless MTU size          */
#define L2CAP_EXTENDED_FEATURES_ARRAY_SIZE     4    /**< Extended features array size     */
#define L2CAP_FIXED_CHNL_ARRAY_SIZE            8    /**< Fixed channel array size         */

/* Extended features mask bits
*/
#define L2CAP_EXTFEA_RTRANS         0x00000001    /**< Retransmission Mode (Not Supported)    */
#define L2CAP_EXTFEA_FC             0x00000002    /**< Flow Control Mode   (Not Supported)    */
#define L2CAP_EXTFEA_QOS            0x00000004    /**< QOS Mode */
#define L2CAP_EXTFEA_ENH_RETRANS    0x00000008    /**< Enhanced retransmission mode           */
#define L2CAP_EXTFEA_STREAM_MODE    0x00000010    /**< Streaming Mode                         */
#define L2CAP_EXTFEA_NO_CRC         0x00000020    /**< Optional FCS (if set No FCS desired)   */
#define L2CAP_EXTFEA_EXT_FLOW_SPEC  0x00000040    /**< Extended flow spec                     */
#define L2CAP_EXTFEA_FIXED_CHNLS    0x00000080    /**< Fixed channels                         */
#define L2CAP_EXTFEA_EXT_WINDOW     0x00000100    /**< Extended Window Size                   */
#define L2CAP_EXTFEA_UCD_RECEPTION  0x00000200    /**< Unicast Connectionless Data Reception  */
#define L2CAP_EXTFEA_ECRB_SUPPORTED 0x00000400    /**< Unicast Connectionless Data Reception  */

/** Mask for LE supported features used in Information Response (default to none) */
#ifndef L2CAP_BLE_EXTFEA_MASK
#define L2CAP_BLE_EXTFEA_MASK 0
#endif

/** Define a value that tells L2CAP to use the default MPS */
#define L2CAP_DEFAULT_ERM_MPS           0x0000

#define L2CAP_FCR_OVERHEAD         2   /**< Control word                 */
#define L2CAP_FCS_LEN              2   /**< FCS takes 2 bytes */
#define L2CAP_SDU_LEN_OVERHEAD     2   /**< SDU length field is 2 bytes */
#define L2CAP_SDU_LEN_OFFSET       2   /**< SDU length offset is 2 bytes */
#define L2CAP_EXT_CONTROL_OVERHEAD 4   /**< Extended Control Field       */
#define L2CAP_MAX_HEADER_FCS       (L2CAP_PKT_OVERHEAD + L2CAP_EXT_CONTROL_OVERHEAD + L2CAP_SDU_LEN_OVERHEAD + L2CAP_FCS_LEN)
                                   /**< length(2), channel(2), control(4), SDU length(2) FCS(2) */
/** Part of L2CAP_MINIMUM_OFFSET that is not part of L2CAP */
#define L2CAP_OFFSET_WO_L2HDR  (L2CAP_MINIMUM_OFFSET-(L2CAP_PKT_OVERHEAD+L2CAP_FCR_OVERHEAD))

/* SAR bits in the control word
*/
#define L2CAP_FCR_UNSEG_SDU    0x0000   /**< Control word to begin with for unsegmented PDU*/
#define L2CAP_FCR_START_SDU    0x4000   /**< ..for Starting PDU of a semented SDU */
#define L2CAP_FCR_END_SDU      0x8000   /**< ..for ending PDU of a segmented SDU */
#define L2CAP_FCR_CONT_SDU     0xc000   /**< ..for continuation PDU of a segmented SDU */

/* Supervisory frame types
*/
#define L2CAP_FCR_SUP_RR        0x0000   /**< Supervisory frame - RR                          */
#define L2CAP_FCR_SUP_REJ       0x0001   /**< Supervisory frame - REJ                         */
#define L2CAP_FCR_SUP_RNR       0x0002   /**< Supervisory frame - RNR                         */
#define L2CAP_FCR_SUP_SREJ      0x0003   /**< Supervisory frame - SREJ                        */

#define L2CAP_FCR_SAR_BITS           0xC000    /**< Mask to get the SAR bits from control word */
#define L2CAP_FCR_SAR_BITS_SHIFT     14        /**< Bits to shift right to get the SAR bits from ctrl-word */

#define L2CAP_FCR_S_FRAME_BIT         0x0001   /**< Mask to check if a PDU is S-frame */
#define L2CAP_FCR_REQ_SEQ_BITS        0x3F00   /**< Mask to get the req-seq from control word */
#define L2CAP_FCR_REQ_SEQ_BITS_SHIFT  8        /**< Bits to shift right to get the req-seq from ctrl-word */
#define L2CAP_FCR_TX_SEQ_BITS         0x007E   /**< Mask on get the tx-seq from control word */
#define L2CAP_FCR_TX_SEQ_BITS_SHIFT   1        /**< Bits to shift right to get the tx-seq from ctrl-word */

#define L2CAP_FCR_F_BIT               0x0080   /**< F-bit in the control word (Sup and I frames)  */
#define L2CAP_FCR_P_BIT               0x0010   /**< P-bit in the control word (Sup frames only)   */

#define L2CAP_FCR_F_BIT_SHIFT         7
#define L2CAP_FCR_P_BIT_SHIFT         4

#define L2CAP_FCR_SEG_BITS            0xC000   /**< Mask to get the segmentation bits from ctrl-word */
#define L2CAP_FCR_SUP_SHIFT           2        /**< Bits to shift right to get the S-bits from ctrl-word */
#define L2CAP_FCR_SUP_BITS            0x000C   /**< Mask to get the supervisory bits from ctrl-word */

#define L2CAP_FCR_INIT_CRC            0        /**< Initial state of the CRC register */
#define L2CAP_FCR_SEQ_MODULO          0x3F     /**< Mask for sequence numbers (range 0 - 63) */

/*************************************************************************************************
** The following definitions are only used for internal testing of ERTM at the application level
*************************************************************************************************/
/* L2CA_SetupErtmTest() corruption test types */
#define L2CAP_FCR_TTYPE_CORR_IFRAME     0       /**< Corrupt one or more I-frames, based on count   */
#define L2CAP_FCR_TTYPE_CORR_SFRAME     1       /**< Corrupt an S-frame, (acknowledgement) */
#define L2CAP_FCR_TTYPE_STOP_TEST       2       /**< Used when turning off a test */
#define L2CAP_FCR_TTYPE_GET_CID         3       /**< Returns RFCOMM cid when '0' is passed in cid argument */

/* L2CA_SetupErtmTest() Freq */
#define L2CAP_FCR_FREQ_NORMAL           0       /**< A single test is run */
#define L2CAP_FCR_FREQ_RANDOM           1       /**< Randomly loses or corrupts a packet */

 /**
 * @addtogroup  l2cap
 *
 * @{
 */

/**
 * @anchor L2CAP_CONN_RESULT
 * @name L2CAP connection result codes
 * @{
 *
 * L2CAP connection result codes.
 */
#define L2CAP_CONN_OK                0      /**< Connection OK */
#define L2CAP_CONN_PENDING           1      /**< Connection Pending */
#define L2CAP_CONN_NO_PSM            2      /**< Connection NO PSM */
#define L2CAP_CONN_SECURITY_BLOCK    3      /**< Connection Security Block */
#define L2CAP_CONN_NO_RESOURCES      4      /**< Connection NO Resource */
#define L2CAP_CONN_BAD_CTLR_ID       5      /**< Bad Controller ID 
                                                @note AMP related */
#define L2CAP_BLE_CONN_BAD_AUTHENT   5      /**< BLE Connection Bad Authentication */
#define L2CAP_BLE_CONN_BAD_AUTHORIZ  6      /**< BLE Connection Bad Autherization */
#define L2CAP_BLE_CONN_BAD_KEY_SIZE  7      /**< BLE Connection Bad key size */
#define L2CAP_BLE_CONN_BAD_ENCRYPT   8      /**< BLE Connection Bad Encryption */
/** @} L2CAP_CONN_RESULT */


/*************************************************************************************************
** The following definitions are related to credit-based flow control mode
*************************************************************************************************/
#define     L2CAP_ECRB_MAX_CHANNELS_PER_CMD     5       /**< ECRB Max Channels per Command */
#define     L2CAP_ECRB_MIN_MTU                  64      /**< ECRB Min MTU */
#define     L2CAP_ECRB_MIN_MPS                  64      /**< ECRB Min MPS */

/* ECRB Result codes
*/
/**
 * @anchor L2CAP_ECRB_RESULT
 * @name L2CAP ECRB Result Code
 * @{
 *
 * L2CAP ECRB Result Code
 */
#define     L2CAP_ECRB_ALL_CONNS_SUCCESSFUL     0x0000      /**< All Connections Successful */
#define     L2CAP_ECRB_ALL_CONNS_REFUSED        0x0002      /**< All Connections Refused */
#define     L2CAP_ECRB_SOME_CONNS_REFUSED       0x0004      /**< Some Connections Refused */
#define     L2CAP_ECRB_INSUFF_AUTHENTICATION    0x0005      /**< Insufficient Authentication */
#define     L2CAP_ECRB_INSUFF_AUTHORIZATION     0x0006      /**< Insufficient Autherization */
#define     L2CAP_ECRB_INSUFF_KEY_SIZE          0x0007      /**< Insufficient Key size */
#define     L2CAP_ECRB_INSUFF_ENCRYPTION        0x0008      /**< Insufficient Encryption */
#define     L2CAP_ECRB_INVALID_SOURCE_CID       0x0009      /**< Invalid Source CID */
#define     L2CAP_ECRB_SOURCE_CID_IN_USE        0x000A      /**< Source CID in Used */
#define     L2CAP_ECRB_UNACCEPTABLE_PARAMS      0x000B      /**< Unacceptable Params */

#define     L2CAP_ECRB_RECONFIG_SUCCESSFUL      0x0000      /**< Reconfiguration Successful */
#define     L2CAP_ECRB_RECONFIG_FAIL_BAD_MTU    0x0001      /**< Reconfiguration fail : Bad MTU */
#define     L2CAP_ECRB_RECONFIG_FAIL_BAD_MPS    0x0002      /**< Reconfiguration fail : Bad MPS */
#define     L2CAP_ECRB_RECONFIG_FAIL_BAD_DCID   0x0003      /**< Reconfiguration fail : Bad DCID */
#define     L2CAP_ECRB_RECONFIG_FAIL_OTHER      0x0004      /**< Reconfiguration fail : Other Reason */


#define     L2CAP_ECRB_NOT_SUPPORTED_BY_PEER    0xF000      /**< Not Supported by Peer */
/** @} L2CAP_ECRB_RESULT */

/**@} l2cap*/

#endif
