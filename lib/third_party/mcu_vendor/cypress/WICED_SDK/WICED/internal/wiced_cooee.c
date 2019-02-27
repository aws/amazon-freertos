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
#include "wiced.h"
#include "stdint.h"
#include "stddef.h"
#include "wwd_network_interface.h"
#include "wwd_buffer_interface.h"
#include "tlv.h"
#include "wiced_security_internal.h"
#include "wiced_internal_api.h"
#include <string.h>

/******************************************************
 *                      Macros
 ******************************************************/

#define COOEE_GET_BIT(x, bit)     ( ((uint32_t*)(x))[(bit) / 32] &  (uint32_t)(1 << ((bit)&0x1F)) )
#define COOEE_SET_BIT(x, bit)     ( ((uint32_t*)(x))[(bit) / 32] |= (uint32_t)(1 << ((bit)&0x1F)) )

/******************************************************
 *                    Constants
 ******************************************************/

#define ETHERNET_ADDRESS_LENGTH     (6)

#define PACKET_SIZE_INDEX_OFFSET    (80)
#define MAX_NUMBER_OF_COOEE_BYTES   (512)

#define COOEE_CHANNEL_DWELL_TIME    (100 * MILLISECONDS)

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

#pragma pack(1)
typedef struct
{
    uint8_t type;
    uint8_t flags;
    uint16_t duration;
    uint8_t address1[ETHERNET_ADDRESS_LENGTH];
    uint8_t address2[ETHERNET_ADDRESS_LENGTH];
    uint8_t address3[ETHERNET_ADDRESS_LENGTH];
    uint16_t ether_type;
} ieee80211_header_t;

typedef struct
{
    uint8_t  header1;
    uint8_t  header2;
    uint8_t  nonce[8];
    uint8_t  data[1];
} wiced_cooee_header_t;

#pragma pack()

/******************************************************
 *               Static Function Declarations
 ******************************************************/

#ifdef WICED_COOEE_ENABLE_SCANNING
static void cooee_scan_callback( wiced_scan_result_t** result_ptr, void* user_data );
#endif

static void process_packet(wiced_buffer_t buffer);
static void cooee_process_raw_packet( wiced_buffer_t buffer, wwd_interface_t interface );

/******************************************************
 *               Variable Definitions
 ******************************************************/

static wiced_cooee_workspace_t* workspace;
static wiced_cooee_header_t* cooee_header;


/*
 * NOTE: Some of the following const objects are safely typecast to non-const variants
 * as wiced_wifi_add_packet_filter() only copies the contents. Generally this should
 * be avoided and is a bad thing to do.
 */
static const uint8_t cooee_upper_address_mask[]  = {0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t cooee_upper_address_match[] = {0x01, 0x00, 0x5e, 0x7e};

static const uint8_t cooee_beacon_address_mask[]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t cooee_beacon_address_match[] = {0x01, 0x00, 0x5e, 0x76, 0x00, 0x00};

static const wiced_packet_filter_t a1_settings =
{
    .id           = 1,
    .rule         = WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING,
    .offset       = 4,
    .mask_size    = 4,
    .mask         = (uint8_t*)cooee_upper_address_mask,
    .pattern      = (uint8_t*)cooee_upper_address_match,
};

static const wiced_packet_filter_t a3_settings =
{
    .id           = 2,
    .rule         = WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING,
    .offset       = 16,
    .mask_size    = 4,
    .mask         = (uint8_t*)cooee_upper_address_mask,
    .pattern      = (uint8_t*)cooee_upper_address_match,
};

static const wiced_packet_filter_t beacon_a1_settings =
{
    .id           = 3,
    .rule         = WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING,
    .offset       = 4,
    .mask_size    = 4,
    .mask         = (uint8_t*)cooee_beacon_address_mask,
    .pattern      = (uint8_t*)cooee_beacon_address_match,
};

static const wiced_packet_filter_t beacon_a3_settings =
{
    .id           = 4,
    .rule         = WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING,
    .offset       = 16,
    .mask_size    = 4,
    .mask         = (uint8_t*)cooee_beacon_address_mask,
    .pattern      = (uint8_t*)cooee_beacon_address_match,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef WICED_COOEE_ENABLE_SCANNING
static void cooee_scan_callback( wiced_scan_result_t** result_ptr, void* user_data )
{
    if (result_ptr == NULL)
    {
        scan_complete = WICED_TRUE;
    }
}
#endif

wiced_result_t wiced_wifi_cooee( wiced_cooee_workspace_t* cooee_workspace )
{
    wiced_scan_result_t ap;
    uint8_t             cooee_nonce[13];
    tlv8_data_t*        security_key_tlv;
    uint32_t            content_length;
    mbedtls_aes_context       aes_ctx;
    tlv8_data_t*        tlv;
    wiced_mac_t         bogus_scan_mac = {.octet={0,0,0,0,0,0}};
    wiced_bool_t        initiator_details_printed;
    uint16_t            previous_received_byte_count;

    workspace = cooee_workspace;
    cooee_header = (wiced_cooee_header_t*)workspace->received_cooee_data;

    wiced_wifi_add_packet_filter( &a1_settings );
    wiced_wifi_add_packet_filter( &a3_settings );
    wiced_wifi_add_packet_filter( &beacon_a1_settings );
    wiced_wifi_add_packet_filter( &beacon_a3_settings );

    wwd_wifi_set_raw_packet_processor( cooee_process_raw_packet );

try_cooee_again:
    memset(&workspace->sniff_complete, 0, sizeof(workspace->sniff_complete));
    wiced_rtos_init_semaphore( &workspace->sniff_complete );
    workspace->received_byte_count  = 0;
    memset(workspace->received_segment_bitmap, 0, sizeof(workspace->received_segment_bitmap));
    memset(workspace->initiator_mac.octet, 0, sizeof(wiced_mac_t));
    memset(workspace->ap_bssid.octet, 0, sizeof(wiced_mac_t));
    workspace->wiced_cooee_complete = WICED_FALSE;

    wiced_wifi_enable_packet_filter( 1 );
    wiced_wifi_enable_packet_filter( 2 );
    wiced_wifi_enable_packet_filter( 3 );
    wiced_wifi_enable_packet_filter( 4 );

    wwd_wifi_enable_monitor_mode( );

    /* Scan through the channel list until we find something */
    initiator_details_printed = WICED_FALSE;
    previous_received_byte_count = 0;

    while (wiced_rtos_get_semaphore( &workspace->sniff_complete, 1 * SECONDS ) == WICED_TIMEOUT)
    {
        if ( ( memcmp( workspace->initiator_mac.octet, bogus_scan_mac.octet, sizeof(wiced_mac_t) ) != 0 ) && ( initiator_details_printed != WICED_TRUE ) )
        {
            WPRINT_WICED_INFO(("\nEasy Setup detected\nTransmitter : %2X:%2X:%2X:%2X:%2X:%2X\nAccess Point: %2X:%2X:%2X:%2X:%2X:%2X\n",
                workspace->initiator_mac.octet[0], workspace->initiator_mac.octet[1], workspace->initiator_mac.octet[2], workspace->initiator_mac.octet[3], workspace->initiator_mac.octet[4], workspace->initiator_mac.octet[5],
                workspace->ap_bssid.octet[0], workspace->ap_bssid.octet[1], workspace->ap_bssid.octet[2], workspace->ap_bssid.octet[3], workspace->ap_bssid.octet[4], workspace->ap_bssid.octet[5] ));

            initiator_details_printed = WICED_TRUE;
        }

        if (workspace->received_byte_count != previous_received_byte_count)
        {
            previous_received_byte_count = workspace->received_byte_count;

            WPRINT_WICED_INFO(("%u of %u bytes received\n", workspace->received_byte_count, cooee_header->header2 ));
        }
#ifdef WICED_COOEE_ENABLE_SCANNING
        if (scan_complete == WICED_TRUE)
        {
            if (wiced_wifi_scan(WWD_SCAN_TYPE_PASSIVE, WICED_BSS_TYPE_INFRASTRUCTURE, NULL, &bogus_scan_mac, NULL, NULL, cooee_scan_callback, NULL, NULL) == WICED_SUCCESS)
            {
                scan_complete = WICED_FALSE;
            }
        }
#endif
    }

    WPRINT_WICED_INFO( ("\nCooee payload received\n") );

    wwd_wifi_disable_monitor_mode( );

    /* Prepare the nonce which is 8 bytes from header + "wiced" */
    memcpy( cooee_nonce, cooee_header->nonce, 8 );
    memcpy( &cooee_nonce[8], "wiced", 5 );

    /* Extract from header how much data there is */
    content_length =  (uint32_t) ( ( ( cooee_header->header1 & 0x0F ) << 8 ) + cooee_header->header2 - 10 );

    /* Decrypt the data */
    {
        uint8_t* cooee_key;
        wiced_dct_read_lock( (void**) &cooee_key, WICED_FALSE, DCT_SECURITY_SECTION, offsetof(platform_dct_security_t, cooee_key), COOEE_KEY_SIZE );

        mbedtls_aes_setkey_dec( &aes_ctx, cooee_key, 128 );

        wiced_dct_read_unlock( cooee_key, WICED_FALSE );
    }

    if (aes_decrypt_ccm( &aes_ctx, content_length, 10, cooee_nonce, sizeof(cooee_nonce),(uint8_t*) cooee_header, &workspace->received_cooee_data[10], &workspace->received_cooee_data[10] ) != 0)
    {
        WPRINT_WICED_INFO( ("Cooee payload decryption failed\n") );
        goto return_with_error;
    }

    /* Process the content */
    memset(&ap, 0, sizeof(ap));
    tlv = (tlv8_data_t*) cooee_header->data;

    /* Process the mandatory SSID */
    if ( tlv->type != WICED_COOEE_SSID )
    {
        goto return_with_error;
    }
    ap.SSID.length = tlv->length;
    memcpy( ap.SSID.value, tlv->data, tlv->length );

    /* Process the mandatory security key */
    tlv = (tlv8_data_t*) ( &tlv->data[tlv->length] );
    if ( tlv->type != WICED_COOEE_WPA_KEY && tlv->type != WICED_COOEE_WEP_KEY )
    {
        goto return_with_error;
    }
    security_key_tlv = tlv;

    /* Process the mandatory host address */
    workspace->user_processed_data = &tlv->data[tlv->length];

    WPRINT_WICED_INFO( ("SSID: %s\n", ap.SSID.value) );
    WPRINT_WICED_INFO( ("PSK : %.64s\n", security_key_tlv->data) );

    wiced_wifi_disable_packet_filter( 1 );
    wiced_wifi_disable_packet_filter( 2 );
    wiced_wifi_disable_packet_filter( 3 );
    wiced_wifi_disable_packet_filter( 4 );

    /* Setup the AP details */
    ap.security = WICED_SECURITY_WPA2_MIXED_PSK;
    ap.channel = 1;
    memcpy(ap.BSSID.octet, workspace->ap_bssid.octet, sizeof(wiced_mac_t));
    ap.band = WICED_802_11_BAND_2_4GHZ;
    ap.bss_type = WICED_BSS_TYPE_INFRASTRUCTURE;

    /* Store AP credentials into DCT */
    WPRINT_WICED_INFO( ("Storing received credentials in DCT\n\n") );

    {
        wiced_config_ap_entry_t* ap_zero;

        wiced_dct_read_lock( (void**) &ap_zero, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );

        memcpy(&ap_zero->details, &ap, sizeof(wiced_ap_info_t));
        ap_zero->security_key_length = security_key_tlv->length;
        memcpy( ap_zero->security_key, security_key_tlv->data, security_key_tlv->length);

        wiced_dct_write( ap_zero, DCT_WIFI_CONFIG_SECTION, offsetof(platform_dct_wifi_config_t, stored_ap_list), sizeof(wiced_config_ap_entry_t) );
    }

    wwd_wifi_set_raw_packet_processor( NULL );

    return WICED_SUCCESS;

return_with_error:
    WPRINT_WICED_INFO(("Easy Setup failed\n"));
    wiced_wifi_disable_packet_filter( 1 );
    wiced_wifi_disable_packet_filter( 2 );
    wiced_wifi_disable_packet_filter( 3 );
    wiced_wifi_disable_packet_filter( 4 );
    wiced_rtos_deinit_semaphore( &workspace->sniff_complete );
    goto try_cooee_again;
}

void cooee_process_raw_packet( wiced_buffer_t buffer, wwd_interface_t interface )
{
    UNUSED_PARAMETER( interface );

    if (workspace->wiced_cooee_complete == WICED_FALSE )
    {
        process_packet(buffer);
    }

    host_buffer_release( buffer, WWD_NETWORK_RX );
}

static void process_packet(wiced_buffer_t buffer)
{
    ieee80211_header_t* header;
    uint8_t* data = NULL;
    uint8_t* initiator = NULL;
    uint8_t* bssid = NULL;
    uint16_t extra_offset = 0;
    uint16_t packet_length;

    header = (ieee80211_header_t*) host_buffer_get_current_piece_data_pointer( buffer );

    if ( memcmp( header->address3, cooee_beacon_address_match, 6 ) == 0 )
    {
        if ( workspace->size_of_zero_data_packet == 0 )
        {
            workspace->size_of_zero_data_packet = (uint16_t) ( host_buffer_get_current_piece_size( buffer ) - 2 );
        }
        return;
    }
    else if ( memcmp( header->address1, cooee_beacon_address_match, 6 ) == 0 )
    {
        if ( workspace->size_of_zero_data_packet == 0 )
        {
            workspace->size_of_zero_data_packet = host_buffer_get_current_piece_size( buffer );
        }
        return;
    }

    /* Check if we have NOT found the Cooee beacon and know the size of a zero data length packet */
    if ( workspace->size_of_zero_data_packet == 0 )
    {
        return;
    }

    /* Check for Cooee data packets */
    if ( memcmp( header->address3, cooee_upper_address_match, 4 ) == 0 )
    {
        data      = &header->address3[4];
        initiator = header->address2;
        bssid     = header->address1;
        extra_offset = 2;
    }
    else if ( memcmp( header->address1, cooee_upper_address_match, 4 ) == 0 )
    {
        data      = &header->address1[4];
        initiator = header->address3;
        bssid     = header->address2;
    }
    else
    {
        return;
    }

    /* Make sure the packet length is valid */
    packet_length = host_buffer_get_current_piece_size( buffer );
    if ( packet_length < MAX_NUMBER_OF_COOEE_BYTES + PACKET_SIZE_INDEX_OFFSET )
    {
        uint16_t index;

        /* Check if this is the first packet we've heard */
        if ( workspace->received_byte_count == 0 )
        {
            /* Store the MAC address of the AP and the sender and only listen to this sender from now on */
            memcpy( workspace->initiator_mac.octet, initiator, sizeof(wiced_mac_t) );
            memcpy( workspace->ap_bssid.octet, bssid, sizeof(wiced_mac_t) );
        }

        index = (uint16_t) ( ( packet_length - workspace->size_of_zero_data_packet - extra_offset ) );
        if ( COOEE_GET_BIT(workspace->received_segment_bitmap, index/2) == 0 )
        {
            workspace->received_cooee_data[index] = data[0];
            workspace->received_cooee_data[index + 1] = data[1];
            COOEE_SET_BIT( workspace->received_segment_bitmap, index/2 );
            workspace->received_byte_count = (uint16_t) ( workspace->received_byte_count + 2 );

            /* Check if we have all the data we need */
            if ( ( cooee_header->header2 != 0 ) && ( workspace->received_byte_count >= cooee_header->header2 ) )
            {
                int a;
                for (a = 0; a < cooee_header->header2/2; ++a)
                {
                    if (COOEE_GET_BIT(workspace->received_segment_bitmap, a) == 0)
                    {
                        return;
                    }
                }
                workspace->wiced_cooee_complete = WICED_TRUE;
                wiced_rtos_set_semaphore( &workspace->sniff_complete );
            }
        }
    }
}
