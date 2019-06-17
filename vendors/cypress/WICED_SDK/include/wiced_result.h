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
 * Header file that includes all API & helper functions
 */

#pragma once

#include "wwd_constants.h"
#include "wiced_resource.h"
#include "platform_constants.h"
#include "wiced_constants.h"
#include "wiced_bluetooth_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 * @cond      Macros
 ******************************************************/

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */

/*
 * Values: 0 - 999
 */
#define WICED_RESULT_LIST( prefix ) \
    RESULT_ENUM( prefix, SUCCESS,                        0 ),   /**< Success */                        \
    RESULT_ENUM( prefix, PENDING,                        1 ),   /**< Pending */                        \
    RESULT_ENUM( prefix, TIMEOUT,                        2 ),   /**< Timeout */                        \
    RESULT_ENUM( prefix, PARTIAL_RESULTS,                3 ),   /**< Partial results */                \
    RESULT_ENUM( prefix, ERROR,                          4 ),   /**< Error */                          \
    RESULT_ENUM( prefix, BADARG,                         5 ),   /**< Bad Arguments */                  \
    RESULT_ENUM( prefix, BADOPTION,                      6 ),   /**< Mode not supported */             \
    RESULT_ENUM( prefix, UNSUPPORTED,                    7 ),   /**< Unsupported function */           \
    RESULT_ENUM( prefix, OUT_OF_HEAP_SPACE,              8 ),   /**< Dynamic memory space exhausted */ \
    RESULT_ENUM( prefix, NOTUP,                          9 ),   /**< Interface is not currently Up */  \
    RESULT_ENUM( prefix, UNFINISHED,                    10 ),   /**< Operation not finished yet */     \
    RESULT_ENUM( prefix, CONNECTION_LOST,               11 ),   /**< Connection to server lost */      \
    RESULT_ENUM( prefix, NOT_FOUND,                     12 ),   /**< Item not found */                 \
    RESULT_ENUM( prefix, PACKET_BUFFER_CORRUPT,         13 ),   /**< Packet buffer corrupted */        \
    RESULT_ENUM( prefix, ROUTING_ERROR,                 14 ),   /**< Routing error */                  \
    RESULT_ENUM( prefix, BADVALUE,                      15 ),   /**< Bad value */                      \
    RESULT_ENUM( prefix, WOULD_BLOCK,                   16 ),   /**< Function would block */           \
    RESULT_ENUM( prefix, ABORTED,                       17 ),   /**< Operation aborted */              \
    RESULT_ENUM( prefix, CONNECTION_RESET,              18 ),   /**< Connection has been reset */      \
    RESULT_ENUM( prefix, CONNECTION_CLOSED,             19 ),   /**< Connection is closed */           \
    RESULT_ENUM( prefix, NOT_CONNECTED,                 20 ),   /**< Connection is not connected */    \
    RESULT_ENUM( prefix, ADDRESS_IN_USE,                21 ),   /**< Address is in use */              \
    RESULT_ENUM( prefix, NETWORK_INTERFACE_ERROR,       22 ),   /**< Network interface error */        \
    RESULT_ENUM( prefix, ALREADY_CONNECTED,             23 ),   /**< Socket is already connected */    \
    RESULT_ENUM( prefix, INVALID_INTERFACE,             24 ),   /**< Interface specified in invalid */ \
    RESULT_ENUM( prefix, SOCKET_CREATE_FAIL,            25 ),   /**< Socket creation failed */         \
    RESULT_ENUM( prefix, INVALID_SOCKET,                26 ),   /**< Socket is invalid */              \
    RESULT_ENUM( prefix, CORRUPT_PACKET_BUFFER,         27 ),   /**< Packet buffer is corrupted */     \
    RESULT_ENUM( prefix, UNKNOWN_NETWORK_STACK_ERROR,   28 ),   /**< Unknown network stack error */    \
    RESULT_ENUM( prefix, NO_STORED_AP_IN_DCT,           29 ),   /**< DCT contains no AP credentials */ \
    RESULT_ENUM( prefix, STA_JOIN_FAILED,               30 ),   /**< Join failed */                    \
    RESULT_ENUM( prefix, PACKET_BUFFER_OVERFLOW,        31 ),   /**< Packet buffer overflow */         \
    RESULT_ENUM( prefix, ALREADY_INITIALIZED,           32 ),   /**< Module has already been inited */


#define FILESYSTEM_RESULT_LIST( prefix ) \
        RESULT_ENUM( prefix, SUCCESS,                       0 ),   /**< Success */               \
        RESULT_ENUM( prefix, PENDING,                       1 ),   /**< Pending */               \
        RESULT_ENUM( prefix, TIMEOUT,                       2 ),   /**< Timeout */               \
        RESULT_ENUM( prefix, PARTIAL_RESULTS,               3 ),   /**< Partial results */       \
        RESULT_ENUM( prefix, ERROR,                         4 ),   /**< Error */                 \
        RESULT_ENUM( prefix, BADARG,                        5 ),   /**< Bad Arguments */         \
        RESULT_ENUM( prefix, BADOPTION,                     6 ),   /**< Mode not supported */    \
        RESULT_ENUM( prefix, UNSUPPORTED,                   7 ),   /**< Unsupported function */  \
        RESULT_ENUM( prefix, DISK_ERROR,                10008 ),   /**< Low level error accessing media */        \
        RESULT_ENUM( prefix, PATH_NOT_FOUND,            10009 ),   /**< Path was not found in filesystem */        \
        RESULT_ENUM( prefix, MEDIA_NOT_READY,           10010 ),   /**< Media is not present or ready for access */          \
        RESULT_ENUM( prefix, ACCESS_DENIED,             10011 ),   /**< Access denied due to permissions  */      \
        RESULT_ENUM( prefix, WRITE_PROTECTED,           10012 ),   /**< Media is write protected */    \
        RESULT_ENUM( prefix, OUT_OF_SPACE,              10013 ),   /**< No free space left on media  */ \
        RESULT_ENUM( prefix, FILENAME_BUFFER_TOO_SMALL, 10014 ),   /**< Filename buffer was too small when retrieving directory contents  */ \
        RESULT_ENUM( prefix, END_OF_RESOURCE,           10015 ),   /**< End of file/directory reached  */ \
        RESULT_ENUM( prefix, FILESYSTEM_INVALID,        10016 ),   /**< Filesystem has an unrecoverable error */ \
        RESULT_ENUM( prefix, BLOCK_SIZE_BAD,            10017 ),   /**< Block size is invalid - not a multiple or sub-multiple of DEFAULT_SECTOR_SIZE */

/******************************************************
 * @endcond    Enumerations
 ******************************************************/

/**
 * WICED Result Type
 */

#ifndef WICED_AMAZON_FREERTOS_SDK
typedef enum
{
    WICED_RESULT_LIST     ( WICED_            )  /*     0 -   999 */
    WWD_RESULT_LIST       ( WICED_WWD_        )  /*  1000 -  1999 */
    WLAN_RESULT_LIST      ( WICED_WLAN_       )  /*  2000 -  2999 */
    RESOURCE_RESULT_LIST  ( WICED_RESOURCE_   )  /*  4000 -  4999 */
    PLATFORM_RESULT_LIST  ( WICED_PLATFORM_   )  /*  6000 -  6999 */
    TCPIP_RESULT_LIST     ( WICED_TCPIP_      )  /*  7000 -  7999 */
    BT_RESULT_LIST        ( WICED_BT_         )  /*  8000 -  8999 */
    FILESYSTEM_RESULT_LIST( WICED_FILESYSTEM_ )  /* 10000 - 10999 */
} wiced_result_t;

#else
typedef enum
{
    WICED_RESULT_LIST     ( WICED_            )  /*     0 -   999 */
    WWD_RESULT_LIST       ( WICED_WWD_        )  /*  1000 -  1999 */
    WLAN_RESULT_LIST      ( WICED_WLAN_       )  /*  2000 -  2999 */
    PLATFORM_RESULT_LIST  ( WICED_PLATFORM_   )  /*  6000 -  6999 */
    TCPIP_RESULT_LIST     ( WICED_TCPIP_      )  /*  7000 -  7999 */
    FILESYSTEM_RESULT_LIST( WICED_FILESYSTEM_ )  /* 10000 - 10999 */
} wiced_result_t;
#endif
/******************************************************
 *            Structures
 ******************************************************/

/******************************************************
 *            Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
