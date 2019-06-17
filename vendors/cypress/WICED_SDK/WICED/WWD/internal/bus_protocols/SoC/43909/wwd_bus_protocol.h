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
#ifndef INCLUDED_SOC_4390_WWD_BUS_PROTOCOL_H
#define INCLUDED_SOC_4390_WWD_BUS_PROTOCOL_H


#include "wwd_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *             Macros
 ******************************************************/

#define WWD_BUS_HAS_HEADER

#define WWD_BUS_HEADER_SIZE                          (sizeof(wwd_bus_header_t))

/* read the section "Interrupt and Status logic" in TWIKI */
#ifdef M2M_RX_POLL_MODE
#define WWD_BUS_USE_STATUS_REPORT_SCHEME             (1==1)
#else
#define WWD_BUS_USE_STATUS_REPORT_SCHEME             (1==0)
#endif

#define WWD_BUS_MAX_BACKPLANE_TRANSFER_SIZE          ( WICED_PAYLOAD_MTU )
#define WWD_BUS_BACKPLANE_READ_PADD_SIZE             ( 0 )

#define WWD_WLAN_KEEP_AWAKE()
#define WWD_WLAN_LET_SLEEP()
#define WWD_WLAN_MAY_SLEEP()                         (1==1)

/******************************************************
 *             Structures
 ******************************************************/

#pragma pack(1)

typedef struct
{
    char  dma_descriptor[8];
} wwd_bus_header_t;

#pragma pack()

/******************************************************
 *             Function declarations
 ******************************************************/

/******************************************************
 *             Global variables
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* ifndef INCLUDED_SOC_4390_WWD_BUS_PROTOCOL_H */
