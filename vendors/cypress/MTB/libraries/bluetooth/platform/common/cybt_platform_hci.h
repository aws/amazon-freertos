/*******************************************************************************
* \file cybt_platform_hci.h
*
* \brief
* Provides API to access BT HCI transport.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
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
*******************************************************************************/

#ifndef CYBT_PLATFORM_HCI_H
#define CYBT_PLATFORM_HCI_H

#include "cybt_result.h"

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define HCI_UART_TYPE_HEADER_SIZE  (1)         /**< HCI UART header size */
#define HCI_UART_DEFAULT_BAUDRATE  (115200)    /**< HCI UART default controller 
                                               *    baudrate
                                               */


/*****************************************************************************
 *                           Type Definitions
 *****************************************************************************/
/**
 * HCI packet type
 */
typedef enum
{
    HCI_PACKET_TYPE_IGNORE   = 0x00,
    HCI_PACKET_TYPE_COMMAND  = 0x01,
    HCI_PACKET_TYPE_ACL      = 0x02,
    HCI_PACKET_TYPE_SCO      = 0x03,
    HCI_PACKET_TYPE_EVENT    = 0x04,
    HCI_PACKET_TYPE_LOOPBACK = 0xFF
} hci_packet_type_t;

/**
 * HCI Event packet header
 */
typedef struct
{
    uint8_t           event_code;
    uint8_t           content_length;
} hci_event_packet_header_t;

/**
 * HCI ACL packet header
 */
typedef struct
{
    uint16_t          hci_handle;
    uint16_t          content_length;
} hci_acl_packet_header_t;

/**
 * HCI SCO packet header
 */
typedef struct
{
    uint16_t          hci_handle;
    uint8_t           content_length;
} hci_sco_packet_header_t;

/**
 * HCI Loopback packet header
 */
typedef struct
{
    uint8_t           content_length;
} hci_loopback_packet_header_t;


#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 *                           Function Declarations
 ****************************************************************************/

/**
 * Initialize and open HCI transport.
 *
 * @return the status of open operation
 */
cybt_result_t cybt_platform_hci_open(void);


/**
 * Set the new baudrate of HCI UART trasnport.
 *
 * @param[in] req_size: the requested size of memory
 *
 * @returns the status of set baudrate operation
 */
cybt_result_t cybt_platform_hci_set_baudrate(uint32_t baudrate);


/**
 * Write data to HCI transport.
 *
 * Note: This fuction shall be returned only when all data was written done.
 *
 * @param[in] type  : HCI packet type
 * @param[in] p_data: the pointer of the data to be written
 * @param[in] lenght: the length of the data to be written
 *
 * @returns the status of write operation
 */
cybt_result_t cybt_platform_hci_write(hci_packet_type_t type,
                                                  uint8_t          *p_data,
                                                  uint32_t         length
                                                 );


/**
 * Read data from HCI transpot.
 *
 * @param[in] type  : HCI packet type
 * @param[in] p_data: the pointer of received buffer
 * @param[in/out] p_length: The pointer of requested/actual length.
 *                          The request read length shall be specified
 *                          through this parameter. The actual read length
 *                          shall be provided in the same parameter,
 *                          along with function return.
 * @param[in] timeout_ms: the timeout (millisecond) to wait to
 *                        receive data from HCI transport
 *
 * @returns the status of read operation
 */
cybt_result_t cybt_platform_hci_read(hci_packet_type_t type,
                                                uint8_t           *p_data,
                                                uint32_t          *p_length,
                                                uint32_t          timeout_ms
                                               );


/**
 * Close HCI transport.
 *
 * @returns the status of close operation
 */
cybt_result_t cybt_platform_hci_close(void);


/**
 * Control HCI RX data indication interrupt .
 *
 * Note: This fuction shall be returned only when all data was written done.
 *
 * @param[in] enable  : enable or disable the interrupt
 *
 * @returns
 */
void cybt_platform_hci_irq_rx_data_ind(bool enable);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

