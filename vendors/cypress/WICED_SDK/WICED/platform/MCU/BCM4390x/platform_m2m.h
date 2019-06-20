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
 *  Broadcom WLAN M2M Protocol interface
 *
 */

#pragma once

#include <stdint.h>
#include "wwd_constants.h"
#include "platform_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 *             Macros
 ******************************************************/

#if PLATFORM_WLAN_POWERSAVE
#define M2M_POWERSAVE_COMM_TX_BEGIN()          m2m_powersave_comm_begin( )
#define M2M_POWERSAVE_COMM_TX_END()            m2m_powersave_comm_end( WICED_TRUE )
#define PLATFORM_M2M_DMA_SYNC_INIT             0
#else
#define M2M_POWERSAVE_COMM_TX_BEGIN()
#define M2M_POWERSAVE_COMM_TX_END()            1
#define PLATFORM_M2M_DMA_SYNC_INIT             1
#endif

#if PLATFORM_M2M_DMA_SYNC_INIT
#define M2M_INIT_DMA_SYNC()                    m2m_init_dma( )
#define M2M_INIT_DMA_ASYNC()
#else
#define M2M_INIT_DMA_SYNC()
#define M2M_INIT_DMA_ASYNC()                   m2m_init_dma( )
#endif

/******************************************************
 *             Constants
 ******************************************************/

/******************************************************
 *             Enumerations
 ******************************************************/

/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/

/******************************************************
 *             Function declarations
 ******************************************************/

void         m2m_signal_txdone                 ( void );
uint32_t     m2m_read_intstatus                ( wiced_bool_t* signal_txdone );
void         m2m_init_dma                      ( void );
void         m2m_deinit_dma                    ( void );
int          m2m_is_dma_inited                 ( void );
void         m2m_dma_tx_reclaim                ( void );
void*        m2m_read_dma_packet               ( uint16_t** hwtag );
void         m2m_refill_dma                    ( void );
int          m2m_rxactive_dma                  ( void );
int          m2m_dma_tx_data                   ( void* data );
void         m2m_wlan_dma_deinit               ( void );
void         m2m_switch_off_dma_post_completion( void );
void         m2m_post_dma_completion_operations( void* destination, uint32_t byte_count );
int          m2m_unprotected_dma_memcpy        ( void* destination, const void* source, uint32_t byte_count, wiced_bool_t blocking );

#if PLATFORM_WLAN_POWERSAVE
void         m2m_powersave_comm_begin          ( void );
wiced_bool_t m2m_powersave_comm_end            ( wiced_bool_t tx );
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
