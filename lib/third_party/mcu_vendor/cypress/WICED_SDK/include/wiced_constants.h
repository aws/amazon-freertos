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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define TCPIP_RESULT_LIST( prefix ) \
        RESULT_ENUM( prefix, SUCCESS,                 0 ),   /**< Success */               \
        RESULT_ENUM( prefix, PENDING,                 1 ),   /**< Pending */               \
        RESULT_ENUM( prefix, TIMEOUT,                 2 ),   /**< Timeout */               \
        RESULT_ENUM( prefix, PARTIAL_RESULTS,         3 ),   /**< Partial results */       \
        RESULT_ENUM( prefix, ERROR,                   4 ),   /**< Error */                 \
        RESULT_ENUM( prefix, BADARG,                  5 ),   /**< Bad Arguments */         \
        RESULT_ENUM( prefix, BADOPTION,               6 ),   /**< Mode not supported */    \
        RESULT_ENUM( prefix, UNSUPPORTED,             7 ),   /**< Unsupported function */  \
        RESULT_ENUM( prefix, INVALID_PACKET,          7008 ),   /**< Invalid packet */        \
        RESULT_ENUM( prefix, INVALID_SOCKET,          7009 ),   /**< Invalid socket */        \
        RESULT_ENUM( prefix, WAIT_ABORTED,            7010 ),   /**< Wait aborted */          \
        RESULT_ENUM( prefix, PORT_UNAVAILABLE,        7011 ),   /**< Port unavailable */      \
        RESULT_ENUM( prefix, IN_PROGRESS,             7012 ),   /**< Action in progress */    \
        RESULT_ENUM( prefix, IP_ADDRESS_IS_NOT_READY, 7013 ),   /**< IP_ADDRESS_IS_NOT_READY */ \
        RESULT_ENUM( prefix, SOCKET_CLOSED,           7014 ),   /**< Socket closed */

/******************************************************
 *                   Enumerations
 ******************************************************/

/** Enumeration of WICED interfaces. \n
 * @note The config interface is a virtual interface that shares the softAP interface
 */
typedef enum
{
    WICED_STA_INTERFACE      = WWD_STA_INTERFACE,             /**< STA or Client Interface  */
    WICED_AP_INTERFACE       = WWD_AP_INTERFACE,              /**< softAP Interface         */
    WICED_P2P_INTERFACE      = WWD_P2P_INTERFACE,             /**< P2P Interface            */
    WICED_ETHERNET_INTERFACE = WWD_ETHERNET_INTERFACE,        /**< Ethernet Interface       */


    WICED_INTERFACE_MAX,       /** DO NOT USE - MUST BE AFTER ALL NORMAL INTERFACES - used for counting interfaces */
    WICED_CONFIG_INTERFACE   = WICED_AP_INTERFACE | (1 << 7), /**< config softAP Interface  */
} wiced_interface_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/


#ifdef __cplusplus
} /* extern "C" */
#endif
