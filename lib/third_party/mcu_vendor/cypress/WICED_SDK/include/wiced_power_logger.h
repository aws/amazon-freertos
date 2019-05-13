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
 * WICED Power Logger (WPL) Tool:
 * This header exposes APIs to start WPL module and to tap the power events. These
 * APIs are enabled only when WPL is enabled.
 */

/*****************************************************************************/
/** @defgroup wpl       WPL
 *
 * WICED Power Logger (WPL) tool provides an estimate of power consumed by a target
 * device (or platform). The WPL tool consists of two main components: Target-WPL and Host-WPL.
 * The Target-WPL is a Wiced module that captures the power events from different components on the target
 * platform and sends these to the Host-WPL. The Host-WPL runs on the PC to receive power events
 * over UART and displays the real-time power plot. It refers to a platform-specific power database (xml)
 * and estimates current based on the power events and duration.
 *
 */

/*****************************************************************************/
#ifndef __WICED_POWER_LOGGER_H_
#define __WICED_POWER_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/** @addtogroup wpl_event       WPL Power Event
 *  @ingroup wpl
 *
 * A WPL power event is a 3-tuple of (WPL Processor, Power Event, Event State).
 *
 *  @{
 */

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
/**
 * @brief WPL Processors (power consuming component) available on the target platform is identified by this ID
 *
 */
typedef enum {
    EVENT_PROC_ID_MCU,       /**< MCU Processor ID. */
    EVENT_PROC_ID_WIFI,      /**< Wi-Fi Processor ID. */
    EVENT_PROC_ID_BT,        /**< BT Processor ID. */
    EVENT_PROC_ID_MAX,       /**< Processor ID Count. */
} cpl_procid_t;

/**
 * @brief Event ID: Supported Power Events for WPL Processors
 *
 */
typedef enum {
    EVENT_ID_POWERSTATE,     /**< Power State Event ID. */
    EVENT_ID_FLASH,          /**< Flash Event ID. */
    EVENT_ID_UART,           /**< UART Event ID. */
    EVENT_ID_WIFI_DATA,      /**< Wi-Fi Event ID. */
    EVENT_ID_I2S,            /**< I2S Event ID. */
    EVENT_ID_PROFILING,      /**< Function Profiling Event ID. */
    EVENT_ID_BT_DATA,        /**< BT Event ID. */
    EVENT_ID_I2C,            /**< I2C Event ID. */
    EVENT_ID_SPI_SFLASH,     /**< SPI_SFLASH Event ID. */
    EVENT_ID_SDIO,           /**< SDIO Event ID. */
    EVENT_ID_SPI_1,          /**< SPI_1 Event ID. */
    EVENT_ID_MAX,            /**< Event ID Count. */
} cpl_event_id_t;

/**
 * @brief Possible State Descriptors for MCU Processor + Powerstate Event combination
 *
 */
typedef enum {
    EVENT_DESC_POWER_ACTIVE1,     /**< Active State-1 Descriptor ID. */
    EVENT_DESC_POWER_ACTIVE2,     /**< Active State-2 Descriptor ID. */
    EVENT_DESC_POWER_SLEEP,       /**< Sleep State Descriptor ID. */
    EVENT_DESC_POWER_DEEPSLEEP,   /**< Deep Sleep State Descriptor ID. */
    EVENT_DESC_POWER_OFF,         /**< Off State Descriptor ID. */
    EVENT_DESC_POWER_HIBERNATE,   /**< Hibernate State Descriptor ID. */
    EVENT_DESC_POWER_PDS,         /**< PDS State Descriptor ID. */
    EVENT_DESC_MAX,               /**< Power Descriptor ID Count. */
} cpl_event_power_state_t;

/**
 * @brief Possible State Descriptors for BT Processor + BT Event combination
 *
 */
typedef enum
{
    EVENT_DESC_BT_POWER_OFF,            /**< BT Off State Descriptor ID. */
    EVENT_DESC_BT_POWER_IDLE,           /**< BT Idle State Descriptor ID. */
    EVENT_DESC_BT_POWER_TX,             /**< BT Tx Descriptor ID. */
    EVENT_DESC_BT_POWER_RX,             /**< BT Rx Descriptor ID. */
    EVENT_DESC_BT_POWER_TX_PDS,         /**< BT Tx PDS Descriptor ID. */
    EVENT_DESC_BT_POWER_RX_PDS,         /**< BT Rx PDS Descriptor ID. */
    EVENT_DESC_BT_POWER_DEEP_SLEEP,     /**< BT Deep Sleep Descriptor ID. */
    EVENT_DESC_BT_MAX,                  /**< BT Descriptor ID Count. */
} cpl_event_bt_power_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + UART Event combination
 *
 */
typedef enum {
    EVENT_DESC_UART_IDLE,            /**< UART Idle State Descriptor ID. */
    EVENT_DESC_UART_TX,              /**< UART tx State Descriptor ID. */
    EVENT_DESC_UART_RX,              /**< UART rx State Descriptor ID. */
    EVENT_DESC_UART_MAX,             /**< UART Descriptor ID Count. */
} cpl_event_uart_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + I2C Event combination
 *
 */
typedef enum {
    EVENT_DESC_I2C_IDLE,            /**< I2C Idle State Descriptor ID. */
    EVENT_DESC_I2C_TX,              /**< I2C tx State Descriptor ID. */
    EVENT_DESC_I2C_RX,              /**< I2C rx State Descriptor ID. */
    EVENT_DESC_I2C_MAX,             /**< I2C Descriptor ID Count. */
} cpl_event_i2c_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + SPI_SFLASH Event combination
 *
 */
typedef enum {
    EVENT_DESC_SPI_SFLASH_IDLE,     /**< SPI-SFLASH Idle State Descriptor ID. */
    EVENT_DESC_SPI_SFLASH_READ,     /**< SPI-SFLASH Read State Descriptor ID. */
    EVENT_DESC_SPI_SFLASH_WRITE,    /**< SPI-SFLASH Write State Descriptor ID. */
    EVENT_DESC_SPI_SFLASH_ERASE,    /**< SPI-SFLASH Erase State Descriptor ID. */
    EVENT_DESC_SPI_SFLASH_MAX,      /**< I2C Descriptor ID Count. */
} cpl_event_spi_sflash_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + SDIO Event combination
 *
 */
typedef enum {
    EVENT_DESC_SDIO_IDLE,     /**< SDIO Idle State Descriptor ID. */
    EVENT_DESC_SDIO_READ,     /**< SDIO Read State Descriptor ID. */
    EVENT_DESC_SDIO_WRITE,    /**< SDIO Write State Descriptor ID. */
    EVENT_DESC_SDIO_MAX,      /**< SDIO Descriptor ID Count. */
} cpl_event_sdio_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + SPI Event combination
 *
 */
typedef enum {
    EVENT_DESC_SPI_OFF,      /**< SPI OFF State Descriptor ID. */
    EVENT_DESC_SPI_IDLE,     /**< SPI Idle State Descriptor ID. */
    EVENT_DESC_SPI_READ,     /**< SPI Read State Descriptor ID. */
    EVENT_DESC_SPI_WRITE,    /**< SPI Write State Descriptor ID. */
    EVENT_DESC_SPI_MAX,      /**< SPI Descriptor ID Count. */
} cpl_event_spi_state_t;

/**
 * @brief Possible State Descriptors for MCU Processor + Function Profiling combination
 *
 */
typedef enum {
    EVENT_DESC_FUNC_IDLE,            /**< Function Idle State Descriptor ID. */
    EVENT_DESC_FUNC_TIME,            /**< Function Time State Descriptor ID. */
} cpl_event_profiling_state_t;

/**
 * @brief Possible State Descriptors for WiFi Processor + WiFi Data Event combination
 *
 */
typedef enum {
    EVENT_DESC_WIFI_IDLE,            /**< Wi-Fi idle State Descriptor ID. */
    EVENT_DESC_WIFI_BAND,            /**< Wi-Fi Band Descriptor ID. */
    EVENT_DESC_WIFI_BW,              /**< Wi-Fi Bandwidth Descriptor ID. */
    EVENT_DESC_WIFI_PMMODE,          /**< Wi-Fi PM mode Descriptor ID. */
    EVENT_DESC_WIFI_RATE_TYPE,       /**< Wi-Fi Rate Type Descriptor ID. */
    EVENT_DESC_WIFI_UP_STATUS,       /**< Wi-Fi Join Status Descriptor ID. */
    EVENT_DESC_WIFI_TXRX_FRAG,       /**< Wi-Fi Management/Control frames count Descriptor ID. */
    EVENT_DESC_WIFI_SCAN_TIME,       /**< Wi-Fi Scan Time Descriptor ID. */
    EVENT_DESC_WIFI_JOIN_TIME,       /**< Wi-Fi Join Time Descriptor ID. */
    EVENT_DESC_WIFI_RATE0,           /**< Wi-Fi Rate0 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE1,           /**< Wi-Fi Rate1 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE2,           /**< Wi-Fi Rate2 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE3,           /**< Wi-Fi Rate3 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE4,           /**< Wi-Fi Rate4 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE5,           /**< Wi-Fi Rate5 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE6,           /**< Wi-Fi Rate6 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE7,           /**< Wi-Fi Rate7 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE8,           /**< Wi-Fi Rate8 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_RATE9,           /**< Wi-Fi Rate9 tx/rx counters data Descriptor ID. */
    EVENT_DESC_WIFI_MAX,             /**< Wi-Fi Descriptor ID Count. */
} cpl_event_wifi_state_t;

/**
 * @brief Possible State Descriptor Values for WiFi Processor + WiFi Data Event + WiFi Rate Type combination
 *
 */
typedef enum {
    EVENT_DESC_WIFI_MCS_RATE = 1,    /**< Wi-Fi MCS Rate Type ID. */
    EVENT_DESC_WIFI_VHT_RATE,        /**< Wi-Fi VHT Rate Type ID. */
} cpl_event_wifi_rate_type_t;

/** @} */

/******************************************************
 *                 Type Definitions
 ******************************************************/
/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Prototypes
 ******************************************************/

/** @addtogroup wpl_api       WPL Functions
 *  @ingroup wpl
 *
 * WPL provides APIs to log power events in the platform.
 *  @{
 */

/**
 * Logs time for a supported power event. Current time is logged as the start time for the specified event_state.
 * The time spent from previous logging is added into the previous event_state
 *
 * @param           proc_id : Processor ID
 * @param           event_id : Event ID
 * @param           event_state : Descriptor ID, represents different states assumed by the power events (for example, an MCU Power event can have event descriptors like: Active, Sleep etc.)
 * @return          NO return value.
 */

#ifdef WICED_POWER_LOGGER_ENABLE
void cpl_event_state_update( uint8_t proc_id, uint8_t event_id, uint8_t event_state );
#define WICED_POWER_LOGGER( proc_id, event_id, event_state ) cpl_event_state_update( proc_id, event_id, event_state )
#else
#define WICED_POWER_LOGGER( proc_id, event_id, event_state )
#endif

/**
 * Logs data value for a supported power event. Resets the current data value of event_state with the specified data
 *
 * @param           proc_id : Processor ID
 * @param           event_id : Event ID
 * @param           event_state : Descriptor ID, represents different states assumed by the power events (for example, an MCU Power event can have event descriptors like: Active, Sleep etc.)
 * @param           data : Data value to be put for event_state.
 * @return          NO return value.
 */

#ifdef WICED_POWER_LOGGER_ENABLE
void cpl_log_reset_event_data( uint8_t proc_id, uint8_t event_id, uint8_t event_state, uint32_t data );
#define WICED_POWER_LOGGER_DATA( proc_id, event_id, event_state, data ) cpl_log_reset_event_data( proc_id, event_id, event_state, data )
#else
#define WICED_POWER_LOGGER_DATA( proc_id, event_id, event_state, data )
#endif


/**
 * Initializes and starts WPL module.
 *
 * @param           NO parameters required.
 * @return          NO return value.
 */
#ifdef WICED_POWER_LOGGER_ENABLE
void wpl_start(void);
#define WPL_START() wpl_start()
#else
#define WPL_START()
#endif
/** @} */

#ifdef __cplusplus
extern "C" }
#endif

#endif //__WICED_POWER_LOGGER_H_
