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

#ifndef INCLUDED_WWD_INTERNAL_H
#define INCLUDED_WWD_INTERNAL_H

#include "tlv.h"
#include "wwd_eapol.h"
#include "wwd_thread.h"
#include "wwd_rtos.h"
#include "wwd_bus_protocol.h"
#include "wwd_structures.h"

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************
 *             Constants
 ******************************************************/
typedef enum
{
    /* Note : If changing this, core_base_address must be changed also */
    WLAN_ARM_CORE = 0,
    SOCRAM_CORE   = 1,
    SDIOD_CORE    = 2
} device_core_t;

typedef enum
{
    WLAN_DOWN,
    WLAN_UP,
    WLAN_OFF
} wlan_state_t;

typedef enum
{
    WLAN_CORE_FLAG_NONE,
    WLAN_CORE_FLAG_CPU_HALT,
} wlan_core_flag_t;

/**
 * Enumeration of AKM (authentication and key management) suites. Table 8-140 802.11mc D3.0.
 */
typedef enum
{
    WICED_AKM_RESERVED                    = 0,
    WICED_AKM_8021X                       = 1,    /**< WPA2 enterprise                 */
    WICED_AKM_PSK                         = 2,    /**< WPA2 PSK                        */
    WICED_AKM_FT_8021X                    = 3,    /**< 802.11r Fast Roaming enterprise */
    WICED_AKM_FT_PSK                      = 4,    /**< 802.11r Fast Roaming PSK        */
    WICED_AKM_8021X_SHA256                = 5,
    WICED_AKM_PSK_SHA256                  = 6,
    WICED_AKM_TDLS                        = 7,    /**< Tunneled Direct Link Setup      */
    WICED_AKM_SAE_SHA256                  = 8,
    WICED_AKM_FT_SAE_SHA256               = 9,
    WICED_AKM_AP_PEER_KEY_SHA256          = 10,
    WICED_AKM_SUITEB_8021X_HMAC_SHA256    = 11,
    WICED_AKM_SUITEB_8021X_HMAC_SHA384    = 12,
    WICED_AKM_SUITEB_FT_8021X_HMAC_SHA384 = 13,
} wiced_akm_suite_t;

/**
 * Enumeration of cipher suites. Table 8-138 802.11mc D3.0.
 */
typedef enum
{
    WICED_CIPHER_GROUP                 = 0,   /**< Use group cipher suite                                        */
    WICED_CIPHER_WEP_40                = 1,   /**< WEP-40                                                        */
    WICED_CIPHER_TKIP                  = 2,   /**< TKIP                                                          */
    WICED_CIPHER_RESERVED              = 3,   /**< Reserved                                                      */
    WICED_CIPHER_CCMP_128              = 4,   /**< CCMP-128 - default pairwise and group cipher suite in an RSNA */
    WICED_CIPHER_WEP_104               = 5,   /**< WEP-104 - also known as WEP-128                               */
    WICED_CIPHER_BIP_CMAC_128          = 6,   /**< BIP-CMAC-128 - default management frame cipher suite          */
    WICED_CIPHER_GROUP_DISALLOWED      = 7,   /**< Group address traffic not allowed                             */
    WICED_CIPHER_GCMP_128              = 8,   /**< GCMP-128 - default for 60 GHz STAs                            */
    WICED_CIPHER_GCMP_256              = 9,   /**< GCMP-256 - introduced for Suite B                             */
    WICED_CIPHER_CCMP_256              = 10,  /**< CCMP-256 - introduced for suite B                             */
    WICED_CIPHER_BIP_GMAC_128          = 11,  /**< BIP-GMAC-128 - introduced for suite B                         */
    WICED_CIPHER_BIP_GMAC_256          = 12,  /**< BIP-GMAC-256 - introduced for suite B                         */
    WICED_CIPHER_BIP_CMAC_256          = 13,  /**< BIP-CMAC-256 - introduced for suite B                         */
} wiced_80211_cipher_t;

/******************************************************
 *             Structures
 ******************************************************/

typedef struct
{
    wlan_state_t           state;
    wiced_country_code_t   country_code;
    wiced_aggregate_code_t aggregate_code;
    uint32_t               keep_wlan_awake;
} wwd_wlan_status_t;

#pragma pack(1)

/* 802.11 Information Element structures */

/* Country Information */
#define COUNTRY_INFO_IE_MINIMUM_LENGTH (6)

typedef struct
{
  uint8_t first_chan_num;
  uint8_t num_chans;
  uint8_t max_tx_pwr_level;
} country_chan_info_t;

typedef struct
{
    tlv8_header_t tlv_header;                 /* id, length                                                          */
    char          ccode[2];                   /* dot11CountryString MIB octet 1~2, two letter country code           */
    uint8_t       env;                        /* dot11CountryString MIB octet 3, indicate indoor/outdoor environment */
    country_chan_info_t country_chan_info[1]; /* At least one country channel info triples                           */
} country_info_ie_fixed_portion_t;


/* Robust Secure Network */
typedef struct
{
    tlv8_header_t tlv_header;          /* id, length */
    uint16_t version;
    uint32_t group_key_suite;          /* See wiced_80211_cipher_t for values */
    uint16_t pairwise_suite_count;
    uint32_t pairwise_suite_list[1];
} rsn_ie_fixed_portion_t;

#define RSN_IE_MINIMUM_LENGTH (8)

typedef struct
{
    tlv8_header_t tlv_header;          /* id, length */
    uint8_t  oui[4];
} vendor_specific_ie_header_t;

#define VENDOR_SPECIFIC_IE_MINIMUM_LENGTH (4)

/* WPA IE */
typedef struct
{
    vendor_specific_ie_header_t vendor_specific_header;
    uint16_t version;
    uint32_t multicast_suite;
    uint16_t unicast_suite_count;
    uint8_t  unicast_suite_list[1][4];
} wpa_ie_fixed_portion_t;

#define WPA_IE_MINIMUM_LENGTH (12)

typedef struct
{
    uint16_t akm_suite_count;
    uint32_t akm_suite_list[1];
} akm_suite_portion_t;

typedef struct
{
    tlv8_header_t tlv_header;          /* id, length */
    uint16_t      ht_capabilities_info;
    uint8_t       ampdu_parameters;
    uint8_t       rx_mcs[10];
    uint16_t      rxhighest_supported_data_rate;
    uint8_t       tx_supported_mcs_set;
    uint8_t       tx_mcs_info[3];
    uint16_t      ht_extended_capabilities;
    uint32_t      transmit_beam_forming_capabilities;
    uint8_t       antenna_selection_capabilities;
} ht_capabilities_ie_t;

#define HT_CAPABILITIES_INFO_LDPC_CODING_CAPABILITY        ( 1 <<  0 )
#define HT_CAPABILITIES_INFO_SUPPORTED_CHANNEL_WIDTH_SET   ( 1 <<  1 )
#define HT_CAPABILITIES_INFO_SM_POWER_SAVE_OFFSET          ( 1 <<  2 )
#define HT_CAPABILITIES_INFO_SM_POWER_SAVE_MASK            ( 3 <<  2 )
#define HT_CAPABILITIES_INFO_HT_GREENFIELD                 ( 1 <<  4 )
#define HT_CAPABILITIES_INFO_SHORT_GI_FOR_20MHZ            ( 1 <<  5 )
#define HT_CAPABILITIES_INFO_SHORT_GI_FOR_40MHZ            ( 1 <<  6 )
#define HT_CAPABILITIES_INFO_TX_STBC                       ( 1 <<  7 )
#define HT_CAPABILITIES_INFO_RX_STBC_OFFSET                ( 1 <<  8 )
#define HT_CAPABILITIES_INFO_RX_STBC_MASK                  ( 3 <<  8 )
#define HT_CAPABILITIES_INFO_HT_DELAYED_BLOCK_ACK          ( 1 << 10 )
#define HT_CAPABILITIES_INFO_MAXIMUM_A_MSDU_LENGTH         ( 1 << 11 )
#define HT_CAPABILITIES_INFO_DSSS_CCK_MODE_IN_40MHZ        ( 1 << 12 )
/* bit 13 reserved */
#define HT_CAPABILITIES_INFO_40MHZ_INTOLERANT              ( 1 << 14 )
#define HT_CAPABILITIES_INFO_L_SIG_TXOP_PROTECTION_SUPPORT ( 1 << 15 )

typedef unsigned int uint;
typedef struct
{
    uint buf;
    uint buf_size;
    uint idx;
    uint out_idx; /* output index */
} hnd_log_t;

#define CBUF_LEN 128

typedef struct
{
    /* Virtual UART
     *   When there is no UART (e.g. Quickturn), the host should write a complete
     *   input line directly into cbuf and then write the length into vcons_in.
     *   This may also be used when there is a real UART (at risk of conflicting with
     *   the real UART).  vcons_out is currently unused.
     */
    volatile uint vcons_in;
    volatile uint vcons_out;

    /* Output (logging) buffer
     *   Console output is written to a ring buffer log_buf at index log_idx.
     *   The host may read the output when it sees log_idx advance.
     *   Output will be lost if the output wraps around faster than the host polls.
     */
    hnd_log_t log;

    /* Console input line buffer
     *   Characters are read one at a time into cbuf until <CR> is received, then
     *   the buffer is processed as a command line.  Also used for virtual UART.
     */
    uint cbuf_idx;
    char cbuf[ CBUF_LEN ];
} hnd_cons_t;

typedef struct wifi_console
{
    uint count; /* Poll interval msec counter */
    uint log_addr; /* Log struct address (fixed) */
    hnd_log_t log; /* Log struct (host copy) */
    uint bufsize; /* Size of log buffer */
    char *buf; /* Log buffer (host copy) */
    uint last; /* Last buffer read index */
} wifi_console_t;

typedef struct
{
    uint flags;
    uint trap_addr;
    uint assert_exp_addr;
    uint assert_file_addr;
    uint assert_line;
    uint console_addr;
    uint msgtrace_addr;
    uint fwid;
} wlan_shared_t;

#ifdef WWD_IOCTL_LOG_ENABLE
#define WWD_IOCTL_LOG_SIZE 32
#define WWD_IOVAR_STRING_SIZE 128
#define WWD_MAX_DATA_SIZE 64

#define WWD_IOCTL_LOG_ADD(x, y) wwd_ioctl_log_add(x, y)
#define WWD_IOCTL_LOG_ADD_EVENT(x, y, z) wwd_ioctl_log_add_event(x, y, z)
#define WWD_IOCTL_PRINT(x) wwd_ioctl_print(x)

typedef struct
{
    uint32_t ioct_log;
    uint8_t is_this_event;
    uint8_t data[WWD_MAX_DATA_SIZE];
    uint32_t data_size;
    uint16_t flag;
    uint32_t reason;
} wwd_ioctl_log_t;

void wwd_ioctl_log_add(uint32_t cmd, wiced_buffer_t buffer);
void wwd_ioctl_log_add_event(uint32_t cmd, uint16_t flag, uint32_t data);

#else
#define WWD_IOCTL_LOG_ADD(x, y)
#define WWD_IOCTL_LOG_ADD_EVENT(x, y, z)
#define WWD_IOCTL_PRINT(x)
#endif /* WWD_IOCTL_LOG_ENABLE */
wwd_result_t wwd_ioctl_print(void);

#pragma pack()
/******************************************************
 *             Constants
 ******************************************************/
#define SICF_CPUHALT             (0x0020)

/******************************************************
 *             Function declarations
 ******************************************************/

extern wiced_bool_t wwd_wifi_ap_is_up;
extern uint8_t      wwd_tos_map[8];
extern eapol_packet_handler_t wwd_eapol_packet_handler;

/* Device core control functions */
extern wwd_result_t wwd_disable_device_core    ( device_core_t core_id, wlan_core_flag_t core_flag );
extern wwd_result_t wwd_reset_device_core      ( device_core_t core_id, wlan_core_flag_t core_flag );
extern wwd_result_t wwd_reset_core             ( device_core_t core_id,uint32_t bits, uint32_t resetbits );
extern wwd_result_t wwd_wlan_armcore_run       ( device_core_t core_id, wlan_core_flag_t core_flag );
extern wwd_result_t wwd_device_core_is_up      ( device_core_t core_id );
extern wwd_result_t wwd_wifi_set_down          ( void );
extern wwd_result_t wwd_wifi_set_up            ( void );
extern void         wwd_wait_for_wlan_event    ( host_semaphore_type_t* transceive_semaphore );

/* Chip specific functions */
extern wwd_result_t wwd_allow_wlan_bus_to_sleep     ( void );
extern wwd_result_t wwd_ensure_wlan_bus_is_up       ( void );

extern wwd_result_t wwd_chip_specific_init          ( void );
extern wwd_result_t wwd_chip_specific_socsram_init  ( void );
extern wwd_result_t wwd_wifi_read_wlan_log_unsafe( uint32_t wlan_shared_address, char* buffer, uint32_t buffer_size );

extern void wwd_wifi_peek( uint32_t address, uint8_t register_length, /*@out@*/ uint8_t* value );
extern void wwd_wifi_poke( uint32_t address, uint8_t register_length, uint32_t value );
extern uint32_t wwd_wifi_get_btc_params( uint32_t address, wwd_interface_t interface );

/******************************************************
 *             Global variables
 ******************************************************/

extern wwd_wlan_status_t wwd_wlan_status;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_INTERNAL_H */
