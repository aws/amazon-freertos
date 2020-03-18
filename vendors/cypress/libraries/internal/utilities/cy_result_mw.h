/*
 * $ Copyright Cypress Semiconductor Apache2 $
 */
#pragma once

#include <stdint.h>
#include "cy_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @addtogroup group_utils_enums
 *
 * Cypress middleware APIs return results of type cy_rslt_t and comprise of three parts:
 * - module base
 * - type
 * - error code
 *
 * \par Result Format
 *
   \verbatim
              Module base             Type    Library specific error code
      +------------------------------+------+------------------------------+
      |CY_RSLT_MODULE_MIDDLEWARE_BASE| 0x2  |           Error Code         |
      +------------------------------+------+------------------------------+
                14-bits               2-bits            16-bits

   Refer to the macro section of this document for library specific error codes.
   \endverbatim
 *
 * The data structure cy_rslt_t is part of cy_result.h in MBED OS PSoC6 target platform, located in <mbed-os/targets/TARGET_Cypress/TARGET_PSOC6/psoc6csp/core_lib/include>
 *
 * Module base: This base is derived from CY_RSLT_MODULE_MIDDLEWARE_BASE (defined in cy_result.h) and is an offset of the CY_RSLT_MODULE_MIDDLEWARE_BASE
 *              The details of the offset and the middleware base are defined below
 *
 * Type: This type is defined in cy_result.h and can be one of CY_RSLT_TYPE_FATAL, CY_RSLT_TYPE_ERROR, CY_RSLT_TYPE_WARNING or CY_RSLT_TYPE_INFO. AWS library error codes are of type CY_RSLT_TYPE_ERROR
 *
 * Library specific error code: These error codes are library specific and defined in macro section of the respetcive libraries
 *
 * Helper macros used for creating the library specific result are provided as part of cy_result.h
 *
 * Each middleware module has been reserved with 128 error codes
 *
 *
 *
 * @{
 */
 /* !!! ALWAYS ADD MODULE BASES AT THE END. DO NOT INSERT NEW MODULES IN BETWEEN EXISTING MODULES !!!
 * The expectation is that all middleware modules shall define their base out of this file so
 * that the error code space can be reserved and allotted efficiently
 */

/** CY middleware results offset */
#define CY_RSLT_MODULE_MW_OFFSET                         128

/** MDNS module base */
#define CY_RSLT_MODULE_MDNS_BASE                         CY_RSLT_MODULE_MIDDLEWARE_BASE
/** AWS IoT module base */
#define CY_RSLT_MODULE_AWS_BASE                          CY_RSLT_MODULE_MDNS_BASE + CY_RSLT_MODULE_MW_OFFSET
/** JSON parser module base */
#define CY_RSLT_MODULE_JSON_BASE                         CY_RSLT_MODULE_AWS_BASE + CY_RSLT_MODULE_MW_OFFSET
/** Linked list module base */
#define CY_RSLT_MODULE_LINKED_LIST_BASE                  CY_RSLT_MODULE_JSON_BASE + CY_RSLT_MODULE_MW_OFFSET
/** command console module base */
#define CY_RSLT_MODULE_COMMAND_CONSOLE_BASE              CY_RSLT_MODULE_LINKED_LIST_BASE + CY_RSLT_MODULE_MW_OFFSET
/** HTTP server module base */
#define CY_RSLT_MODULE_HTTP_SERVER                       CY_RSLT_MODULE_COMMAND_CONSOLE_BASE + CY_RSLT_MODULE_MW_OFFSET
/** WiFi supplicant module base */
#define CY_RSLT_MODULE_WIFI_SUPPLICANT_BASE              CY_RSLT_MODULE_HTTP_SERVER + CY_RSLT_MODULE_MW_OFFSET
/** CY middleware TCP/IP module base */
#define CY_RSLT_MODULE_TCPIP_BASE                        CY_RSLT_MODULE_WIFI_SUPPLICANT_BASE + CY_RSLT_MODULE_MW_OFFSET
/** CY generic middleware module base */
#define CY_RSLT_MODULE_MW_BASE                           CY_RSLT_MODULE_TCPIP_BASE + CY_RSLT_MODULE_MW_OFFSET
/** CY middleware TLS module base */
#define CY_RSLT_MODULE_TLS_BASE                          CY_RSLT_MODULE_MW_BASE + CY_RSLT_MODULE_MW_OFFSET

/**
 * @}
 */

/**
 * @defgroup generic_mw_defines CY generic middleware results/error codes
 * @ingroup group_utils_enums
 * @{
 */
/** Generic middleware error code start */
#define CY_RSLT_MODULE_MW_ERR_CODE_START       (0)
/** Generic middleware base error code */
#define CY_RSLT_MW_ERR_BASE                   CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_MW_BASE, CY_RSLT_MODULE_MW_ERR_CODE_START)
/** Generic middleware error */
#define CY_RSLT_MW_ERROR                      ( CY_RSLT_MW_ERR_BASE + 1 )
/** Generic middleware timeout */
#define CY_RSLT_MW_TIMEOUT                    ( CY_RSLT_MW_ERR_BASE + 2 )
/** Generic middleware bad argument */        
#define CY_RSLT_MW_BADARG                     ( CY_RSLT_MW_ERR_BASE + 3 )
/** Generic middleware out of heap memory */        
#define CY_RSLT_MW_OUT_OF_HEAP_SPACE          ( CY_RSLT_MW_ERR_BASE + 4 )
/** Generic middleware pending operation */        
#define CY_RSLT_MW_PENDNG                     ( CY_RSLT_MW_ERR_BASE + 5 )
/** Generic middleware unsupported method */        
#define CY_RSLT_MW_UNSUPPORTED                ( CY_RSLT_MW_ERR_BASE + 6 )
/** Generic middleware buffer unavailable */        
#define CY_RSLT_MW_BUFFER_UNAVAIL_TEMPORARILY ( CY_RSLT_MW_ERR_BASE + 7 )

/**
 * @}
 */

/** 
 * @defgroup tcpip_mw_defines CY middleware TCP/IP socket results/error codes
 * @ingroup group_utils_enums
 * @{
 */
/** CY middleware TCP/IP error code start */
#define CY_RSLT_MODULE_TCPIP_ERR_CODE_START       (0)
/** CY middleware TCPIP/IP base error code */
#define CY_RSLT_TCPIP_ERR_BASE                CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_TCPIP_BASE, CY_RSLT_MODULE_TCPIP_ERR_CODE_START)
/** CY middleware TCP/IP generic error */        
#define CY_RSLT_TCPIP_ERROR                   ( CY_RSLT_TCPIP_ERR_BASE + 1 )
/** CY middleware TCP/IP timeout */        
#define CY_RSLT_TCPIP_TIMEOUT                 ( CY_RSLT_TCPIP_ERR_BASE + 2 )
/** CY middleware TCP/IP out of memory */        
#define CY_RSLT_TCPIP_ERROR_NO_MEMORY         ( CY_RSLT_TCPIP_ERR_BASE + 3 )
/** CY middleware TCP/IP error opening socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_OPEN       ( CY_RSLT_TCPIP_ERR_BASE + 4 )
/** CY middleware TCP/IP error binding socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_BIND       ( CY_RSLT_TCPIP_ERR_BASE + 5 )
/** CY middleware TCP/IP error listening to socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_LISTEN     ( CY_RSLT_TCPIP_ERR_BASE + 6 )
/** CY middleware TCP/IP error accepting socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_ACCEPT     ( CY_RSLT_TCPIP_ERR_BASE + 7 )
/** CY middleware TCP/IP error with TLS operation */
#define CY_RSLT_TCPIP_ERROR_TLS_OPERATION     ( CY_RSLT_TCPIP_ERR_BASE + 8 )
/** CY middleware TCP/IP max sockets bound */
#define CY_RSLT_TCPIP_ERROR_NO_MORE_SOCKET    ( CY_RSLT_TCPIP_ERR_BASE + 9 )
/** CY middleware TCP/IP error sending data */
#define CY_RSLT_TCPIP_ERROR_SEND              ( CY_RSLT_TCPIP_ERR_BASE + 10)
/** CY middleware TCP/IP error receiving data */
#define CY_RSLT_TCPIP_ERROR_RECEIVE           ( CY_RSLT_TCPIP_ERR_BASE + 11)
/** CY middleware TCP/IP error in setting socket options */
#define CY_RSLT_TCPIP_ERROR_SOCKET_OPTIONS    ( CY_RSLT_TCPIP_ERR_BASE + 12 )


/**
 * @}
 */

/** 
 * @defgroup tls_mw_defines CY middleware TLS results/error codes
 * @ingroup group_utils_enums
 * @{
 */
/** CY middleware TLS error code start */
#define CY_RSLT_MODULE_TLS_ERR_CODE_START       (0)
/** CY middleware TLS base error code */
#define CY_RSLT_TLS_ERR_BASE                  CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_TLS_BASE, CY_RSLT_MODULE_TLS_ERR_CODE_START)
/** CY middleware TLS generic error */        
#define CY_RSLT_MODULE_TLS_ERROR              ( CY_RSLT_TLS_ERR_BASE + 1 )
/** CY middleware TLS timeout */        
#define CY_RSLT_MODULE_TLS_TIMEOUT            ( CY_RSLT_TLS_ERR_BASE + 2 )
/** CY middleware TLS bad argument */        
#define CY_RSLT_MODULE_TLS_BADARG             ( CY_RSLT_TLS_ERR_BASE + 3 )
/** CY middleware TLS out of memory */        
#define CY_RSLT_MODULE_TLS_OUT_OF_HEAP_SPACE  ( CY_RSLT_TLS_ERR_BASE + 4 )
/** CY middleware TLS bad input */        
#define CY_RSLT_MODULE_TLS_BAD_INPUT_DATA     ( CY_RSLT_TLS_ERR_BASE + 5 )
/** CY middleware TLS error parsing private key */        
#define CY_RSLT_MODULE_TLS_PARSE_KEY          ( CY_RSLT_TLS_ERR_BASE + 6 )
/** CY middleware TLS error parsing certificate */        
#define CY_RSLT_MODULE_TLS_PARSE_CERTIFICATE  ( CY_RSLT_TLS_ERR_BASE + 7 )
/** CY middleware TLS unsupported method */        
#define CY_RSLT_MODULE_TLS_UNSUPPORTED        ( CY_RSLT_TLS_ERR_BASE + 8 )
/** CY middleware TLS handshake failure */        
#define CY_RSLT_MODULE_TLS_HANDSHAKE_FAILURE  ( CY_RSLT_TLS_ERR_BASE + 9 )

/**
 * @}
 */

#ifdef __cplusplus
} /*extern "C" */
#endif        

