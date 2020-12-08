/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 * Header file that includes all API & helper functions
 */

#pragma once

#include "wiced_bluetooth_result.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef int wiced_bool_t;

/******************************************************
 * @cond      Macros
 ******************************************************/
#define WICED_TRUE 1
#define WICED_FALSE 0

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */

/******************************************************
 * @endcond    Enumerations
 ******************************************************/
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


/**
 * WICED Result Type
 */

typedef enum
{
    WICED_RESULT_LIST     ( WICED_            )  /*     0 -   999 */
	BT_RESULT_LIST		  ( WICED_BT_		  )  /*  8000 -  8999 */
} wiced_result_t;

/******************************************************
 *            Structures
 ******************************************************/

/******************************************************
 *            Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
