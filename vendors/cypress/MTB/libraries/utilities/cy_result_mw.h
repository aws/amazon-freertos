/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
 * The data structure cy_rslt_t is part of cy_result.h located in <core_lib/include>
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

/** MDNS module base */
#define CY_RSLT_MODULE_MDNS_BASE                         CY_RSLT_MODULE_MIDDLEWARE_BASE
/** AWS IoT module base */
#define CY_RSLT_MODULE_AWS_BASE                          CY_RSLT_MODULE_MIDDLEWARE_BASE + 1
/** JSON parser module base */
#define CY_RSLT_MODULE_JSON_BASE                         CY_RSLT_MODULE_MIDDLEWARE_BASE + 2
/** Linked list module base */
#define CY_RSLT_MODULE_LINKED_LIST_BASE                  CY_RSLT_MODULE_MIDDLEWARE_BASE + 3
/** command console module base */
#define CY_RSLT_MODULE_COMMAND_CONSOLE_BASE              CY_RSLT_MODULE_MIDDLEWARE_BASE + 4
/** HTTP server module base */
#define CY_RSLT_MODULE_HTTP_SERVER                       CY_RSLT_MODULE_MIDDLEWARE_BASE + 5
/** WiFi supplicant module base */
#define CY_RSLT_MODULE_WIFI_SUPPLICANT_BASE              CY_RSLT_MODULE_MIDDLEWARE_BASE + 6
/** TCP/IP module base */
#define CY_RSLT_MODULE_TCPIP_BASE                        CY_RSLT_MODULE_MIDDLEWARE_BASE + 7
/** Generic middleware module base */
#define CY_RSLT_MODULE_MW_BASE                           CY_RSLT_MODULE_MIDDLEWARE_BASE + 8
/** TLS module base */
#define CY_RSLT_MODULE_TLS_BASE                          CY_RSLT_MODULE_MIDDLEWARE_BASE + 9
/** Secure Sockets module base */
#define CY_RSLT_MODULE_SECURE_SOCKETS_BASE               CY_RSLT_MODULE_MIDDLEWARE_BASE + 10
/** WiFi Connection Manager (WCM) module base */
#define CY_RSLT_MODULE_WCM_BASE                          CY_RSLT_MODULE_MIDDLEWARE_BASE + 11
/** lwIP WHD port module base */
#define CY_RSLT_MODULE_LWIP_WHD_PORT_BASE                CY_RSLT_MODULE_MIDDLEWARE_BASE + 12
/** Over The Air Update Module base (OTA) */
#define CY_RSLT_MODULE_OTA_UPDATE_BASE                   CY_RSLT_MODULE_MIDDLEWARE_BASE + 13

/**
 * @}
 */

/**
 * @defgroup generic_mw_defines generic middleware results/error codes
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
 * @defgroup tcpip_mw_defines TCP/IP socket results/error codes
 * @ingroup group_utils_enums
 * @{
 */
/** TCP/IP error code start */
#define CY_RSLT_MODULE_TCPIP_ERR_CODE_START       (0)
/** TCPIP/IP base error code */
#define CY_RSLT_TCPIP_ERR_BASE                CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_TCPIP_BASE, CY_RSLT_MODULE_TCPIP_ERR_CODE_START)
/** TCP/IP generic error */
#define CY_RSLT_TCPIP_ERROR                   ( CY_RSLT_TCPIP_ERR_BASE + 1 )
/** TCP/IP timeout */
#define CY_RSLT_TCPIP_TIMEOUT                 ( CY_RSLT_TCPIP_ERR_BASE + 2 )
/** TCP/IP out of memory */
#define CY_RSLT_TCPIP_ERROR_NO_MEMORY         ( CY_RSLT_TCPIP_ERR_BASE + 3 )
/** TCP/IP error opening socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_OPEN       ( CY_RSLT_TCPIP_ERR_BASE + 4 )
/** TCP/IP error binding socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_BIND       ( CY_RSLT_TCPIP_ERR_BASE + 5 )
/** TCP/IP error listening to socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_LISTEN     ( CY_RSLT_TCPIP_ERR_BASE + 6 )
/** TCP/IP error accepting socket */
#define CY_RSLT_TCPIP_ERROR_SOCKET_ACCEPT     ( CY_RSLT_TCPIP_ERR_BASE + 7 )
/** TCP/IP error with TLS operation */
#define CY_RSLT_TCPIP_ERROR_TLS_OPERATION     ( CY_RSLT_TCPIP_ERR_BASE + 8 )
/** TCP/IP max sockets bound */
#define CY_RSLT_TCPIP_ERROR_NO_MORE_SOCKET    ( CY_RSLT_TCPIP_ERR_BASE + 9 )
/** TCP/IP error sending data */
#define CY_RSLT_TCPIP_ERROR_SEND              ( CY_RSLT_TCPIP_ERR_BASE + 10)
/** TCP/IP error receiving data */
#define CY_RSLT_TCPIP_ERROR_RECEIVE           ( CY_RSLT_TCPIP_ERR_BASE + 11)
/** TCP/IP error in setting socket options */
#define CY_RSLT_TCPIP_ERROR_SOCKET_OPTIONS    ( CY_RSLT_TCPIP_ERR_BASE + 12 )


/**
 * @}
 */

/** 
 * @defgroup tls_mw_defines TLS results/error codes
 * @ingroup group_utils_enums
 * @{
 */
/** TLS error code start */
#define CY_RSLT_MODULE_TLS_ERR_CODE_START        (0)
/** TLS base error code */
#define CY_RSLT_TLS_ERR_BASE                     CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_TLS_BASE, CY_RSLT_MODULE_TLS_ERR_CODE_START)
/** TLS generic error */
#define CY_RSLT_MODULE_TLS_ERROR                 ( CY_RSLT_TLS_ERR_BASE + 1 )
/** TLS timeout error */
#define CY_RSLT_MODULE_TLS_TIMEOUT               ( CY_RSLT_TLS_ERR_BASE + 2 )
/** TLS bad argument */
#define CY_RSLT_MODULE_TLS_BADARG                ( CY_RSLT_TLS_ERR_BASE + 3 )
/** TLS out of memory */
#define CY_RSLT_MODULE_TLS_OUT_OF_HEAP_SPACE     ( CY_RSLT_TLS_ERR_BASE + 4 )
/** TLS bad input */
#define CY_RSLT_MODULE_TLS_BAD_INPUT_DATA        ( CY_RSLT_TLS_ERR_BASE + 5 )
/** TLS error parsing private key */
#define CY_RSLT_MODULE_TLS_PARSE_KEY             ( CY_RSLT_TLS_ERR_BASE + 6 )
/** TLS error parsing certificate */
#define CY_RSLT_MODULE_TLS_PARSE_CERTIFICATE     ( CY_RSLT_TLS_ERR_BASE + 7 )
/** TLS unsupported method */
#define CY_RSLT_MODULE_TLS_UNSUPPORTED           ( CY_RSLT_TLS_ERR_BASE + 8 )
/** TLS handshake failure */
#define CY_RSLT_MODULE_TLS_HANDSHAKE_FAILURE     ( CY_RSLT_TLS_ERR_BASE + 9 )
/** TLS socket connection closed by peer */
#define CY_RSLT_MODULE_TLS_CONNECTION_CLOSED     ( CY_RSLT_TLS_ERR_BASE + 10 )
/** TLS socket not connected */
#define CY_RSLT_MODULE_TLS_SOCKET_NOT_CONNECTED  ( CY_RSLT_TLS_ERR_BASE + 11 )

/**
 * @}
 */

#ifdef __cplusplus
} /*extern "C" */
#endif        

