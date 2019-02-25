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
 *  Declares platform SDIO Host/Device functions
 */
#pragma once

#include "wiced_platform.h"
#include "platform/wwd_sdio_interface.h"    /* To use sdio_command_t, sdio_transfer_mode_t, and sdio_block_size_t */
#include <typedefs.h>
#include <bcmsdbus.h>   /* bcmsdh to/from specific controller APIs */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Constants
 ******************************************************/

typedef struct
{
    uint8_t       write_data;           /* 0 - 7 */
    unsigned int  _stuff2          : 1; /* 8     */
    unsigned int  register_address :17; /* 9-25  */
    unsigned int  _stuff           : 1; /* 26    */
    unsigned int  raw_flag         : 1; /* 27    */
    unsigned int  function_number  : 3; /* 28-30 */
    unsigned int  rw_flag          : 1; /* 31    */
} sdio_cmd52_argument_t;

typedef struct
{
    unsigned int  count            : 9; /* 0-8   */
    unsigned int  register_address :17; /* 9-25  */
    unsigned int  op_code          : 1; /* 26    */
    unsigned int  block_mode       : 1; /* 27    */
    unsigned int  function_number  : 3; /* 28-30 */
    unsigned int  rw_flag          : 1; /* 31    */
} sdio_cmd53_argument_t;

typedef union
{
    uint32_t              value;
    sdio_cmd52_argument_t cmd52;
    sdio_cmd53_argument_t cmd53;
} sdio_cmd_argument_t;

typedef enum
{
    SDIO_READ = 0,
    SDIO_WRITE = 1
} sdio_transfer_direction_t;

/* Configure callback to client when host receive client interrupt */
typedef void (*sdio_isr_callback_function_t)(void *);

/******************************************************
 *             Global declarations
 ******************************************************/


/******************************************************
 *             Structures
 ******************************************************/

/** @endcond */

/** @addtogroup platif Platform Interface
 *  @{
 */

/** @name SDIO Bus Functions
 *  Functions that enable WICED to use an SDIO bus
 *  on a particular hardware platform.
 */
/**@{*/


/******************************************************
 *             Function declarations
 ******************************************************/

/**
 * Initializes the SDIO Bus
 *
 * This function set up the interrupts, enable power and clocks to the bus.
 *
 * @return WICED_SUCCESS or Error code
 */
extern wiced_result_t platform_sdio_host_init( sdio_isr_callback_function_t client_check_isr_function );

/**
 * De-Initializes the SDIO Bus
 *
 * This function does the reverse of @ref platform_sdio_host_init.
 *
 * @return WICED_SUCCESS or Error code
 */
extern wiced_result_t platform_sdio_host_deinit( void );

/**
 * Transfers SDIO data
 *
 * Implemented in the WICED Platform interface, which is specific to the
 * platform in use.
 * WICED uses this function as a generic way to transfer data
 * across an SDIO bus.
 * Please refer to the SDIO specification.
 *
 * @param direction         : Direction of transfer - Write = to Wi-Fi device,
 *                                                    Read  = from Wi-Fi device
 * @param command           : The SDIO command number
 * @param mode              : Indicates whether transfer will be byte mode or block mode
 * @param block_size        : The block size to use (if using block mode transfer)
 * @param argument          : The argument of the particular SDIO command
 * @param data              : A pointer to the data buffer used to transmit or receive
 * @param data_size         : The length of the data buffer
 * @param response_expected : Indicates if a response is expected - RESPONSE_NEEDED = Yes
 *                                                                  NO_RESPONSE     = No
 * @param response  : A pointer to a variable which will receive the SDIO response.
 *                    Can be null if the caller does not care about the response value.
 *
 * @return WICED_SUCCESS if successful, otherwise an error code
 */
extern wiced_result_t platform_sdio_host_transfer( sdio_transfer_direction_t direction, sdio_command_t command, sdio_transfer_mode_t mode, sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t* data, uint16_t data_size, sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t* response );

/**
 * Switch SDIO bus to high speed mode
 *
 * When SDIO starts, it must be in a low speed mode
 * This function switches it to high speed mode (up to 50MHz)
 *
 */
extern void platform_sdio_host_enable_high_speed( void );

/**
 * Enables the bus interrupt
 *
 * This function is called once the system is ready to receive interrupts
 */
extern wiced_result_t platform_sdio_host_enable_interrupt( void );

/**
 * Disables the bus interrupt
 *
 * This function is called to stop the system supplying any more interrupts
 */
extern wiced_result_t platform_sdio_host_disable_interrupt( void );

/**
 * Read Block(s) from SD/MMC card
 *
 * @param address           : Block address in byte offset
 * @param count             : Number of blocks to read
 * @param data              : data pointer to read
 *
 * @return WICED_SUCCESS if successful, otherwise an error code
 */
extern wiced_result_t platform_sdio_host_read_block( uint32_t address, uint32_t count, uint8_t* data );

/**
 * Write Block(s) from SD/MMC card
 *
 * @param address           : Block address in byte offset
 * @param count             : Number of blocks to write
 * @param data              : data pointer to write
 *
 * @return WICED_SUCCESS if successful, otherwise an error code
 */
extern wiced_result_t platform_sdio_host_write_block( uint32_t address, uint32_t count, const uint8_t* data );

/** @} */
/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
