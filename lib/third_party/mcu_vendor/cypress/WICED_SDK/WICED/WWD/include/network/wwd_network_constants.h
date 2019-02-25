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
 *  Provides packet size constants which are useful for implementations of the
 *  network interface and buffer interface.
 */
#pragma once


#include "platform_cache_def.h"

#ifndef MAX
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#endif /* MAX */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Constants
 ******************************************************/

/**
 * The maximum size of the wwd_buffer_header_t structure (i.e. largest bus implementation)
 */
#define MAX_BUS_HEADER_LENGTH (12)

/**
 * The maximum size of the SDPCM + BDC header, including offsets and reserved space
 * 12 bytes - SDPCM header
 * 2 bytes  - Extra offset for SDPCM headers that come as 14 bytes
 * 4 bytes  - BDC header
 */
#define MAX_SDPCM_HEADER_LENGTH (18)

/**
 * The maximum space in bytes required for headers in front of the Ethernet header.
 * This definition allows WICED to use a pre-built bus-generic network stack library regardless of choice of bus.
 * Note: adjust accordingly if a new bus is added.
 */
#define WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX ( MAX_BUS_HEADER_LENGTH + MAX_SDPCM_HEADER_LENGTH )

/**
 * The space in bytes required after the end of an Ethernet packet
 */
#define WICED_LINK_TAIL_AFTER_ETHERNET_FRAME     ( 0 )

/**
 * The size of an Ethernet header
 */
#define WICED_ETHERNET_SIZE         (14)

/**
 * The size in bytes of the Link layer header i.e. the Wiced specific headers and the Ethernet header
 */
#define WICED_PHYSICAL_HEADER       (WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX + WICED_ETHERNET_SIZE)

/**
 * The size in bytes of the data after Link layer packet
 */
#define WICED_PHYSICAL_TRAILER      (WICED_LINK_TAIL_AFTER_ETHERNET_FRAME)

/**
 * The maximum size in bytes of the data part of an Ethernet frame
 */
#ifndef WICED_PAYLOAD_MTU
/* This change in MTU applicable to the FreeRTOS OS.
 * This change may be revisited in future development.
 */
#define WICED_PAYLOAD_MTU           (1500)
#endif

/* Maximum payload size of ICMP Ping Packet */
#define WICED_PING_PAYLOAD_MTU (10000)
/**
 * The maximum size in bytes of a packet used within Wiced
 */
#define WICED_WIFI_BUFFER_SIZE            (WICED_PAYLOAD_MTU + WICED_PHYSICAL_HEADER + WICED_PHYSICAL_TRAILER)

#ifndef WICED_ETHERNET_BUFFER_SIZE
#define WICED_ETHERNET_BUFFER_SIZE        0
#endif

/**
* Overridden at compile time to allow larger packets from Wi-Fi side.  Example: IOCTL or IOVAR needs more room for all the data.
*/
#define WICED_LINK_MTU              MAX( WICED_WIFI_BUFFER_SIZE, WICED_ETHERNET_BUFFER_SIZE )
#define WICED_LINK_MTU_ALIGNED      PLATFORM_L1_CACHE_ROUND_UP(WICED_LINK_MTU)

#define WICED_POOL_PAYLOAD_SIZE_128 128
#define WICED_POOL_PAYLOAD_SIZE_128_ALIGNED      PLATFORM_L1_CACHE_ROUND_UP(WICED_POOL_PAYLOAD_SIZE_128)
/**
 * Ethernet Ethertypes
 */
#define WICED_ETHERTYPE_IPv4    0x0800
#define WICED_ETHERTYPE_IPv6    0x86DD
#define WICED_ETHERTYPE_ARP     0x0806
#define WICED_ETHERTYPE_RARP    0x8035
#define WICED_ETHERTYPE_EAPOL   0x888E
#define WICED_ETHERTYPE_DOT1AS  0x88F7
#define WICED_ETHERTYPE_8021Q   0x8100

/** @endcond */

#ifdef __cplusplus
} /* extern "C" */
#endif
