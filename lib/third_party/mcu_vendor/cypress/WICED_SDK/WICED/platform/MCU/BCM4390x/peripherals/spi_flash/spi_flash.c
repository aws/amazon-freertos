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

/*
 * WICED common Sflash driver
 *
 * History :
 * 2015/11/20
 * Add Sflash feature support:
 *      1. Quad mode enable/disable/query
 *      2. Block protect configure/query
 *          - 64KB protect
 *          - 128KB protect
 *          - 256KB protect
 *          - 512KB protect
 *          - 1024KB protect
 *          - 2048KB protect
 *          - 4096KB protect
 *          - 8192KB protect
 *          - 16384KB protect
 *          - 32768KB protect
 *      3. HIGH SPEED for READ/WRITE
 *      4. Erase mode : Sector(4KB), Block(64KB), All(all chip)
 *      5. Reset Sflash
 * Testing report:
 *      1. Had testing Read/Write/Erase and configure
 *         block protect/Quad mode with below Sflash,
 *         they all work well :
 *         (1) SFLASH_ID_MX25L6433F (MACRONIX)
 *         (2) SFLASH_ID_N25Q064A   (MICRON)
 *         (3) SFLASH_ID_W25Q64FV   (WINBOND)
 */

#include "spi_flash.h"
#include "platform_spi_flash.h"
#include "platform/wwd_platform_interface.h"
#include <string.h>

#ifdef APPLICATION_XIP_ENABLE
#include "platform_cache.h"
#include "cr4.h"
#endif /* APPLICATION_XIP_ENABLE */
/******************************************************
 *                      Macros
 ******************************************************/
/* Status register bit of Common (MACRONIX) sflash */
#define STATUS_REGISTER_BIT_BUSY                    ( 0x1 )
#define STATUS_REGISTER_BIT_WRITE_ENABLE            ( 0x2 )
#define STATUS_REGISTER_BIT_BLOCK_PROTECT_0         ( 0x4 )
#define STATUS_REGISTER_BIT_BLOCK_PROTECT_1         ( 0x8 )
#define STATUS_REGISTER_BIT_BLOCK_PROTECT_2         ( 0x10 )
#define STATUS_REGISTER_BIT_BLOCK_PROTECT_3         ( 0x20 )
#define STATUS_REGISTER_BIT_QUAD_ENABLE             ( 0x40 )
#define STATUS_REGISTER_BIT_REGISTER_WRITE_PROTECT  ( 0x80 )

/* Status register bit of MICRON sflash */
#ifdef SFLASH_SUPPORT_MICRON_PARTS
#define MICRON_STATUS_REGISTER_BIT_BLOCK_PROTECT_3 ( 0x40 )
#define MICRON_SFLASH_ENH_VOLATILE_STATUS_REGISTER_HOLD ( 0x10 ) /* HOLD# */
#endif /* SFLASH_SUPPORT_MICRON_PARTS */

/* Status register bit of WINBOND sflash */
#ifdef SFLASH_SUPPORT_WINBOND_PARTS
#define WINBOND_STATUS_REGISTER_BIT_QUAD_ENABLE ( 0x200 )
#define WINBOND_STATUS_REGISTER_DATA_LENGTH ( 2 )
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */

/* Status register bit of CYPRESS sflash */
#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
#define CYPRESS_STATUS_REGISTER_BIT_QUAD_ENABLE ( 0x200 )
#define CYPRESS_STATUS_REGISTER_DATA_LENGTH ( 2 )
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */

/* Stage of initialize sflash */
#define WICED_SFLASH_INIT_STAGE_0 ( 0 )
#define WICED_SFLASH_INIT_STAGE_1 ( 1 )
#define WICED_SFLASH_INIT_STAGE_2 ( 2 )
#define WICED_SFLASH_INIT_STAGE_CLEAR_ALL ( 99 )

#ifdef APPLICATION_XIP_ENABLE
#define WICED_IS_XIP_CODE_SEGMENT( destination, size) \
    ( ( (destination) >= XIP_INSTRUCTION_START_ADDRESS ) && ( (destination) + (size) <= (XIP_INSTRUCTION_START_ADDRESS + XIP_INSTRUCTION_REGION_LENGTH) ) )
#endif /* APPLICATION_XIP_ENABLE */
/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
/**
 * Common (MACRONIX) Sflash action which will link to sflash_action_t.
 */
static int common_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t common_sflash_is_quad_mode( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static int common_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level ) __attribute__ ( ( unused ) );
static int common_sflash_set_write_enable( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t common_sflash_is_write_enable( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static int common_sflash_reset( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static wiced_bool_t common_sflash_is_busy( sflash_handle_t* sflash_handle );

static int common_sflash_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode,
        sflash_command_t normal_read, sflash_command_t fast_read, sflash_command_t highspeed_read, sflash_command_t highspeed_x4io_read );
static int common_sflash_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode,
        sflash_command_t normal_write, sflash_command_t highspeed_write, sflash_command_t highspeed_x4io_write);
static int common_sflash_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode,
        sflash_command_t erase_4K, sflash_command_t erase_64K, sflash_command_t erase_all );

static int common_sflash_3bytes_address_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_3bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_3bytes_address_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_4bytes_address_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_4bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode ) __attribute__ ( ( unused ) );
static int common_sflash_4bytes_address_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode ) __attribute__ ( ( unused ) );

#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
/**
 * MACRONIX Sflash action which will link to sflash_action_t.
 */
static int macronix_sflash_4bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode ) __attribute__ ( ( unused ) );
#endif /* SFLASH_SUPPORT_MACRONIX_PARTS */

#ifdef SFLASH_SUPPORT_MICRON_PARTS
/**
 * MICRON Sflash action which will link to sflash_action_t.
 */
static int micron_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t micron_sflash_is_quad_mode( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static int micron_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode ) __attribute__ ( ( unused ) );
static int micron_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level ) __attribute__ ( ( unused ) );
#endif /* SFLASH_SUPPORT_MICRON_PARTS */

#ifdef SFLASH_SUPPORT_WINBOND_PARTS
/**
 * WINBOND Sflash action which will link to sflash_action_t.
 */
static int winbond_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t winbond_sflash_is_quad_mode( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static int winbond_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode ) __attribute__ ( ( unused ) );
static int winbond_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level ) __attribute__ ( ( unused ) );
static int winbond_sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode);
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */

#ifdef SFLASH_SUPPORT_SST_PARTS
/**
 * SST Sflash action which will link to sflash_action_t.
 */
static int sst_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t sst_sflash_is_quad_mode( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
#endif /* SFLASH_SUPPORT_SST_PARTS */

#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
/**
 * Cypress Sflash action which will link to sflash_action_t.
 */
static int cypress_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode );
static wiced_bool_t cypress_sflash_is_quad_mode( sflash_handle_t* sflash_handle ) __attribute__ ( ( unused ) );
static int cypress_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode ) __attribute__ ( ( unused ) );
static int cypress_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level ) __attribute__ ( ( unused ) );
static int cypress_sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode);
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */
#if ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) )
static int cypress_fram_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode ) __attribute__ ( ( unused ) );
static int cypress_fram_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode ) __attribute__ ( ( unused ) );
static int cypress_fram_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode ) __attribute__ ( ( unused ) );
#endif /* ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) ) */

/* Sflash driver function */
/**
 * Base on Sflash ID to detect sflash parameter table.
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param sflash_id         Sflash ID
 */
static void sflash_setup_capability( sflash_handle_t* sflash_handle, uint32_t sflash_id );

/**
 * Sending sflash command to SPI controller
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param command           Sflash command
 * @param data              refer point for send/receive data.
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_generic_command( sflash_handle_t* sflash_handle, sflash_command_t command, void* data );

/**
 * For setup and query status register
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param mask              specific bit of status register
 * @param data              configured bit of status register
 * @param mode              setup or query status register
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode);

/**
 * Setup Quad mode of Sflash
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param mode              Turn on/off Quad mode
 * @param mask_bit          The status register bit of Quad mode
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode, uint32_t mask_bit );

/**
 * Setup Block Protect of Sflash
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param mode              send block protect mode to status register.
 * @param mask_bit          The status register bit of Block Protect
 * @param map_bit           Mapping each block protect bits with each block protect mode
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode, uint32_t mask_bit, uint32_t* map_bit );

/**
 * Query Block Protect of Sflash
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param protect_level     Retrieve current block protect mode from status register.
 * @param mask_bit          The status register bit of Block Protect
 * @param map_bit           Mapping each block protect bits with each block protect mode
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level, uint32_t mask_bit, uint32_t* map_bit );

/**
 * Setup Write Enable of Sflash ( We should set Write Enable on before any action of write data )
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param mode              setup Write Enable on/off
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_set_write_enable( sflash_handle_t* sflash_handle, onoff_action_t mode );

/**
 * Sflash reset, all non-volatile register/data will be clear.
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_reset( sflash_handle_t* sflash_handle );

/**
 * Use for query specific bit of status register.
 * ex :
 *      is_quad_mode
 *      is_write_enable
 *      is_busy
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param mask_bit          specific bit of status register
 * @return                  if specific bit had been set as 1, then return Yes(WICED_TRUE) or return No(WICED_FALSE)
 */
static wiced_bool_t sflash_is_status_register_bit_set( sflash_handle_t* sflash_handle, uint32_t mask_bit );

/**
 * After any action of write, will call sflash_wait_busy_done to wait for sflash completely ( not busy )
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
static int sflash_wait_busy_done( sflash_handle_t* sflash_handle );

/**
 * Each Sflash command has it's own data length to show that how many data will be read/write after issuing command.
 * @param command           Sflash command
 * @return                  return data length
 */
static uint32_t sflash_get_command_data_length( sflash_command_t command );

/**
 * De-initialize sflash by stage level.
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param init_stage        The stage of sflash init
 * @return                  return none
 */
static void sflash_deinit_by_stage( sflash_handle_t* sflash_handle, int8_t init_stage );

/**
 * Processing Read/Write data
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param command           Sflash command
 * @param device_address    Will process Read/Write in spec address
 * @param data              Read/Write data
 * @param data_len          data length
 * @param mode              select READ/WRITE mode
 * @return                  return success/fail
 */
static int sflash_read_write_process( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t data_len, read_write_mode_t mode );

static sflash_interface_t* platform_interface_get( hw_interface_t hw_interface );
static int platform_interface_init( hw_interface_t hw_interface );
static int platform_interface_deinit( hw_interface_t hw_interface );
static int platform_interface_transfer_receive( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length );
#if ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) )
static int platform_interface_handle_spi_segments( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length, const wiced_spi_device_t* spi );
#endif /* ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) ) */

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
struct sflash_capabilities_table_element
{
        uint32_t device_id;
        sflash_capabilities_t capabilities;
};

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct sflash_capabilities_table_element sflash_capabilities_table_element_t;

/******************************************************
 *               Variables Definitions
 ******************************************************/
static host_semaphore_type_t host_semaphore0 = {0};
static sflash_handle_t sflash_handle_save0 = {0};
#ifdef ENABLE_SPI_0
static host_semaphore_type_t host_semaphore1 = {0};
static sflash_handle_t sflash_handle_save1 = {0};
#endif /* ENABLE_SPI_0 */
#ifdef ENABLE_SPI_1
static host_semaphore_type_t host_semaphore2 = {0};
static sflash_handle_t sflash_handle_save2 = {0};
#endif /* ENABLE_SPI_1 */

static sflash_interface_t sflash_interface[] =
{
        { INTERFACE_4390x_SFLASH, 0, &sflash_handle_save0, &host_semaphore0, WICED_FALSE },
#ifdef ENABLE_SPI_0
        { INTERFACE_4390x_SPI_0, 0, &sflash_handle_save1, &host_semaphore1, WICED_FALSE },
#endif /* ENABLE_SPI_0 */
#ifdef ENABLE_SPI_1
        { INTERFACE_4390x_SPI_1, 0, &sflash_handle_save2, &host_semaphore2, WICED_FALSE },
#endif /* ENABLE_SPI_1 */
};

const opcode_purpose_t opcode_purpose_list[] =
{
#ifndef SFLASH_WRITE_DATA_DISABLE
        { SFLASH_WRITE,                         COMMAND_TXDATA, WICED_TRUE },
        { SFLASH_QUAD_WRITE,                    COMMAND_TXDATA, WICED_TRUE },
        { SFLASH_X4IO_WRITE,                    COMMAND_TXDATA, WICED_TRUE },
        { SFLASH_WRITE4B,                       COMMAND_TXDATA, WICED_TRUE },
        { SFLASH_QUAD_WRITE4B,                  COMMAND_TXDATA, WICED_TRUE },
        { SFLASH_4K_ERASE,                      COMMAND_ONLY,   WICED_TRUE },
        { SFLASH_32K_ERASE,                     COMMAND_ONLY,   WICED_TRUE },
        { SFLASH_64K_ERASE,                     COMMAND_ONLY,   WICED_TRUE },
        { SFLASH_CHIP_ERASE,                    COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_4K_ERASE4B,                    COMMAND_ONLY,   WICED_TRUE },
        { SFLASH_64K_ERASE4B,                   COMMAND_ONLY,   WICED_TRUE },
        { SFLASH_WRITE_ENH_VOLATILE_REGISTER,   COMMAND_TXDATA, WICED_FALSE },
        { SFLASH_WRITE_STATUS_REGISTER,         COMMAND_TXDATA, WICED_FALSE },
#endif /* SFLASH_WRITE_DATA_DISABLE */
        { SFLASH_READ,                          COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_FAST_READ,                     COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_QUAD_READ,                     COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_X4IO_READ,                     COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_READ4B,                        COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_FAST_READ4B,                   COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_QUAD_READ4B,                   COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_X4IO_READ4B,                   COMMAND_RXDATA, WICED_TRUE },
        { SFLASH_READ_STATUS_REGISTER,          COMMAND_RXDATA, WICED_FALSE },
        { SFLASH_READ_STATUS_REGISTER2,         COMMAND_RXDATA, WICED_FALSE },
        { SFLASH_READ_ENH_VOLATILE_REGISTER,    COMMAND_RXDATA, WICED_FALSE },
        { SFLASH_READID_JEDEC_ID,               COMMAND_RXDATA, WICED_FALSE },
        { SFLASH_RESET_ENABLE,                  COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_RESET,                         COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_WRITE_DISABLE,                 COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_WRITE_ENABLE,                  COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_EN4B,                          COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_EX4B,                          COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_WRITE_ENABLE_VOLATILE_REGISTER,COMMAND_ONLY,   WICED_FALSE },
        { SFLASH_COMMAND_MAX_ENUM,              COMMAND_UNKNOWN_PURPOSE,   WICED_FALSE },
};

/* Structure (sflash_action_t/sflash_speed_advance_t) for Common sflash */
static const sflash_action_t common_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         common_sflash_is_quad_mode,
        .set_block_protect =    common_sflash_set_block_protect,
        .query_block_protect =  common_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_3bytes_address_write_data,
        .erase_data =           common_sflash_3bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        common_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            common_sflash_3bytes_address_read_data
};

#ifdef USE_COMMON_ACTION_OVER_128Mb
static const sflash_action_t common_action_over_128Mbit = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         common_sflash_is_quad_mode,
        .set_block_protect =    common_sflash_set_block_protect,
        .query_block_protect =  common_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_4bytes_address_write_data,
        .erase_data =           common_sflash_4bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        common_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            common_sflash_4bytes_address_read_data
};
#endif /* #ifdef USE_COMMON_ACTION_OVER_128Mb */

#if defined(SFLASH_SUPPORT_MACRONIX_PARTS) || defined (SFLASH_SUPPORT_SST_PARTS)
static const sflash_speed_advance_t common_speed_config = {
        .fast_read_dummy_cycle =             8,
        .high_speed_read_dummy_cycle =       6,
        .high_speed_read_x4io_support =     WICED_TRUE,
        .high_speed_read_mode_bit_support = WICED_FALSE,
        .high_speed_write_support =         WICED_TRUE,
        .high_speed_write_x4io_support =    WICED_TRUE,
        .high_speed_write_quad_address_mode = WICED_TRUE
};
#endif /* #if defined(SFLASH_SUPPORT_MACRONIX_PARTS) || defined (SFLASH_SUPPORT_SST_PARTS) */

static const sflash_speed_advance_t common_lowspeed_config = {
        .fast_read_dummy_cycle =            0,
        .high_speed_read_dummy_cycle =      0,
        .high_speed_read_x4io_support =     WICED_FALSE,
        .high_speed_read_mode_bit_support = WICED_FALSE,
        .high_speed_write_support =         WICED_FALSE,
        .high_speed_write_x4io_support =    WICED_FALSE,
        .high_speed_write_quad_address_mode = WICED_TRUE
};

#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
/* Structure (sflash_action_t/sflash_speed_advance_t) for MACRONIX sflash */
static const sflash_action_t macronix_action_over_128Mbit = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         common_sflash_is_quad_mode,
        .set_block_protect =    common_sflash_set_block_protect,
        .query_block_protect =  common_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           macronix_sflash_4bytes_address_write_data,
        .erase_data =           common_sflash_4bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        common_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            common_sflash_4bytes_address_read_data
};
#endif

#ifdef SFLASH_SUPPORT_MICRON_PARTS
/* Structure (sflash_action_t/sflash_speed_advance_t) for MICRON sflash */
static const sflash_action_t micron_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         micron_sflash_is_quad_mode,
        .set_block_protect =    micron_sflash_set_block_protect,
        .query_block_protect =  micron_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_3bytes_address_write_data,
        .erase_data =           common_sflash_3bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        micron_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            common_sflash_3bytes_address_read_data
};

static const sflash_speed_advance_t micron_speed_config = {
        .fast_read_dummy_cycle =            8,
        .high_speed_read_dummy_cycle =      10,
        .high_speed_read_x4io_support =     WICED_TRUE,
        .high_speed_read_mode_bit_support = WICED_FALSE,
        .high_speed_write_support =         WICED_TRUE,
        .high_speed_write_x4io_support =    WICED_FALSE,
        .high_speed_write_quad_address_mode = WICED_TRUE
};
#endif /* SFLASH_SUPPORT_MICRON_PARTS */

#ifdef SFLASH_SUPPORT_WINBOND_PARTS
/* Structure (sflash_action_t/sflash_speed_advance_t) for WINBOND sflash */
static const sflash_action_t winbond_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         winbond_sflash_is_quad_mode,
        .set_block_protect =    winbond_sflash_set_block_protect,
        .query_block_protect =  winbond_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_3bytes_address_write_data,
        .erase_data =           common_sflash_3bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        winbond_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      winbond_sflash_status_register,
        .read_data =            common_sflash_3bytes_address_read_data
};

static const sflash_speed_advance_t winbond_speed_config = {
        .fast_read_dummy_cycle =            8,
        .high_speed_read_dummy_cycle =      4,
        .high_speed_read_x4io_support =     WICED_TRUE,
        .high_speed_read_mode_bit_support = WICED_TRUE,
        .high_speed_write_support =         WICED_TRUE,
        .high_speed_write_x4io_support =    WICED_FALSE,
        .high_speed_write_quad_address_mode = WICED_TRUE
};
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */

#ifdef SFLASH_SUPPORT_SST_PARTS
/* Structure (sflash_action_t) for SST sflash */
static const sflash_action_t sst_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         sst_sflash_is_quad_mode,
        .set_block_protect =    common_sflash_set_block_protect,
        .query_block_protect =  common_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_3bytes_address_write_data,
        .erase_data =           common_sflash_3bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        sst_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            common_sflash_3bytes_address_read_data
};
#endif /* SFLASH_SUPPORT_SST_PARTS */

#ifdef SFLASH_SUPPORT_ISSI_PARTS
/* Structure (sflash_speed_advance_t) for ISSI sflash */
static const sflash_speed_advance_t issi_speed_config = {
       .fast_read_dummy_cycle =            8,
       .high_speed_read_dummy_cycle =      4,
       .high_speed_read_x4io_support =     WICED_TRUE,
       .high_speed_read_mode_bit_support = WICED_TRUE,
       .high_speed_write_support =         WICED_TRUE,
       .high_speed_write_x4io_support =    WICED_FALSE,
       .high_speed_write_quad_address_mode = WICED_TRUE
};
#endif /* SFLASH_SUPPORT_ISSI_PARTS */

#ifdef SFLASH_SUPPORT_EON_PARTS
/* Structure (sflash_speed_advance_t) for EON sflash */
static const sflash_speed_advance_t eon_speed_config = {
       .fast_read_dummy_cycle =            8,
       .high_speed_read_dummy_cycle =      6,
       .high_speed_read_x4io_support =     WICED_TRUE,
       .high_speed_read_mode_bit_support = WICED_FALSE,
       .high_speed_write_support =         WICED_FALSE,
       .high_speed_write_x4io_support =    WICED_FALSE,
       .high_speed_write_quad_address_mode = WICED_TRUE
};
#endif /* SFLASH_SUPPORT_EON_PARTS */

#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
/* Structure (sflash_action_t/sflash_speed_advance_t) for CYPRESS sflash */
static const sflash_action_t cypress_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         cypress_sflash_is_quad_mode,
        .set_block_protect =    cypress_sflash_set_block_protect,
        .query_block_protect =  cypress_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_3bytes_address_write_data,
        .erase_data =           common_sflash_3bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        cypress_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      cypress_sflash_status_register,
        .read_data =            common_sflash_3bytes_address_read_data
};

static const sflash_action_t cypress_action_over_128Mbit = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         cypress_sflash_is_quad_mode,
        .set_block_protect =    common_sflash_set_block_protect,
        .query_block_protect =  common_sflash_query_block_protect,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                common_sflash_reset,
        .write_data =           common_sflash_4bytes_address_write_data,
        .erase_data =           common_sflash_4bytes_address_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        cypress_sflash_set_quad_mode,
        .is_busy =              common_sflash_is_busy,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      cypress_sflash_status_register,
        .read_data =            common_sflash_4bytes_address_read_data
};

static const sflash_speed_advance_t cypress_speed_config = {
        .fast_read_dummy_cycle =            8,
        .high_speed_read_dummy_cycle =      8,
        .high_speed_read_x4io_support =     WICED_FALSE,
        .high_speed_read_mode_bit_support = WICED_FALSE,
        .high_speed_write_support =         WICED_TRUE,
        .high_speed_write_x4io_support =    WICED_FALSE,
        .high_speed_write_quad_address_mode = WICED_FALSE
};
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */

#if ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) )
static const sflash_action_t cypress_fram_action = {
#ifndef SFLASH_WRITE_DATA_DISABLE
        .is_quad_mode =         NULL,
        .set_block_protect =    NULL,
        .query_block_protect =  NULL,
        .is_write_enable =      common_sflash_is_write_enable,
        .reset =                NULL,
        .write_data =           cypress_fram_write_data,
        .erase_data =           cypress_fram_erase_data,
#endif /* SFLASH_WRITE_DATA_DISABLE */
        .set_quad_mode =        NULL,
        .is_busy =              NULL,
        .set_write_enable =     common_sflash_set_write_enable,
        .status_register =      sflash_status_register,
        .read_data =            cypress_fram_read_data
};

static const sflash_speed_advance_t cypress_fram_speed_config = {
        .fast_read_dummy_cycle =            0,
        .high_speed_read_dummy_cycle =      0,
        .high_speed_read_x4io_support =     WICED_FALSE,
        .high_speed_read_mode_bit_support = WICED_FALSE,
        .high_speed_write_support =         WICED_FALSE,
        .high_speed_write_x4io_support =    WICED_FALSE,
        .high_speed_write_quad_address_mode = WICED_FALSE
};
#endif /* ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) ) */

/* Configure table for all sflash */
static const sflash_capabilities_table_element_t sflash_capabilities_tables[] =
{
#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
        { SFLASH_ID_MX25L8006E,     { 1*MBYTE,    1,    .action = &common_action,   .speed_advance = &common_lowspeed_config } },
        { SFLASH_ID_MX25L1606E,     { 2*MBYTE,    1,    .action = &common_action,   .speed_advance = &common_speed_config } },
        { SFLASH_ID_MX25L6433F,     { 8*MBYTE,    256,  .action = &common_action,   .speed_advance = &common_speed_config } },
        { SFLASH_ID_MX25L12835F,    { 16*MBYTE,   256,  .action = &common_action,   .speed_advance = &common_speed_config } },
        { SFLASH_ID_MX25L25635F,    { 32*MBYTE,   256,  .action = &macronix_action_over_128Mbit,   .speed_advance = &common_speed_config } },
#endif /* SFLASH_SUPPORT_MACRONIX_PARTS */
#ifdef SFLASH_SUPPORT_SST_PARTS
        { SFLASH_ID_SST25VF080B,    { 1*MBYTE,    1,    .action = &sst_action,      .speed_advance = &common_speed_config } },
#endif /* SFLASH_SUPPORT_SST_PARTS */
#ifdef SFLASH_SUPPORT_EON_PARTS
        { SFLASH_ID_EN25QH16,       { 2*MBYTE,    1,    .action = &common_action,   .speed_advance = &eon_speed_config } },
#endif /* SFLASH_SUPPORT_EON_PARTS */
#ifdef SFLASH_SUPPORT_ISSI_PARTS
        { SFLASH_ID_ISSI25CQ032,    { 4*MBYTE,    256,  .action = &common_action,   .speed_advance = &issi_speed_config } },
        { SFLASH_ID_ISSI25LP064,    { 8*MBYTE,    256,  .action = &common_action,   .speed_advance = &issi_speed_config } },
#endif /* SFLASH_SUPPORT_ISSI_PARTS */
#ifdef SFLASH_SUPPORT_MICRON_PARTS
        { SFLASH_ID_N25Q064A,       { 8*MBYTE,    256,  .action = &micron_action,   .speed_advance = &micron_speed_config } },
#endif /* SFLASH_SUPPORT_MICRON_PARTS */
#ifdef SFLASH_SUPPORT_WINBOND_PARTS
        { SFLASH_ID_W25Q64FV,       { 8*MBYTE,    256,  .action = &winbond_action,  .speed_advance = &winbond_speed_config } },
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */
#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
        { SFLASH_ID_S25FL064L,       { 8*MBYTE,    256,  .action = &cypress_action,  .speed_advance = &cypress_speed_config } },
        { SFLASH_ID_S25FL128L,       { 16*MBYTE,   256,  .action = &cypress_action,  .speed_advance = &cypress_speed_config } },
        { SFLASH_ID_S25FL256L,       { 32*MBYTE,   256,  .action = &cypress_action_over_128Mbit,  .speed_advance = &cypress_speed_config } },
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */
#if ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) )
        { FRAM_ID_FM25W256,          { 256*KBYTE,  1,  .action = &cypress_fram_action,  .speed_advance = &cypress_fram_speed_config } },
#endif /* #if ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) ) */
        { SFLASH_ID_DEFAULT,         { 2*MBYTE,    1,    .action = &common_action,   .speed_advance = &common_lowspeed_config } }
};

/* bit map of protect block for common sflash */
static uint32_t block_protect_bit_map[ BLOCK_PRTOECT_MAX_ENUM ] =
{
        [ BLOCK_PRTOECT_TOP_0KB ] =       0,
        [ BLOCK_PRTOECT_TOP_64KB ] =      STATUS_REGISTER_BIT_BLOCK_PROTECT_0,
        [ BLOCK_PRTOECT_TOP_128KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_256KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_512KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_1024KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_2048KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_4096KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_8192KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_3,
        [ BLOCK_PRTOECT_TOP_16384KB ] =   STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_3,
        [ BLOCK_PRTOECT_TOP_32768KB ] =   STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_3
};

#ifdef SFLASH_SUPPORT_MICRON_PARTS
/* bit map of protect block for MICRON sflash */
static uint32_t micron_block_protect_bit_map[ BLOCK_PRTOECT_MAX_ENUM ] =
{
        [ BLOCK_PRTOECT_TOP_0KB ] =       0,
        [ BLOCK_PRTOECT_TOP_64KB ] =      STATUS_REGISTER_BIT_BLOCK_PROTECT_0,
        [ BLOCK_PRTOECT_TOP_128KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_256KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_512KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_1024KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_2048KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_4096KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_8192KB ] =    MICRON_STATUS_REGISTER_BIT_BLOCK_PROTECT_3,
};
#endif /* SFLASH_SUPPORT_MICRON_PARTS */

#ifdef SFLASH_SUPPORT_WINBOND_PARTS
/* bit map of protect block for WINBOND sflash */
static uint32_t winbond_block_protect_bit_map[ BLOCK_PRTOECT_MAX_ENUM ] =
{
        [ BLOCK_PRTOECT_TOP_0KB ] =       0,
        [ BLOCK_PRTOECT_TOP_128KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0,
        [ BLOCK_PRTOECT_TOP_256KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_512KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_1024KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_2048KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_4096KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_8192KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2
};
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */

#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
/* bit map of protect block for CYPRESS sflash */
static uint32_t cypress_block_protect_bit_map[ BLOCK_PRTOECT_MAX_ENUM ] =
{
        [ BLOCK_PRTOECT_TOP_0KB ] =       0,
        [ BLOCK_PRTOECT_TOP_128KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0,
        [ BLOCK_PRTOECT_TOP_256KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_512KB ] =     STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1,
        [ BLOCK_PRTOECT_TOP_1024KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_2048KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_4096KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2,
        [ BLOCK_PRTOECT_TOP_8192KB ] =    STATUS_REGISTER_BIT_BLOCK_PROTECT_0 | STATUS_REGISTER_BIT_BLOCK_PROTECT_1 | STATUS_REGISTER_BIT_BLOCK_PROTECT_2
};
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */

/*************************
 * Common Sflash function*
 *************************/
static int common_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    return sflash_set_quad_mode( sflash_handle, mode, STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static wiced_bool_t common_sflash_is_quad_mode( sflash_handle_t* sflash_handle )
{
    return sflash_is_status_register_bit_set( sflash_handle, STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static int common_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_3;
    return sflash_set_block_protect( sflash_handle, mode, mask, block_protect_bit_map );
}

static int common_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_3;
    return sflash_query_block_protect( sflash_handle, protect_level, mask, block_protect_bit_map );
}

static int common_sflash_set_write_enable( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    return sflash_set_write_enable( sflash_handle, mode );
}

static wiced_bool_t common_sflash_is_write_enable( sflash_handle_t* sflash_handle )
{
    return sflash_is_status_register_bit_set( sflash_handle, STATUS_REGISTER_BIT_WRITE_ENABLE );
}

static int common_sflash_reset( sflash_handle_t* sflash_handle )
{
    return sflash_reset( sflash_handle );
}

static wiced_bool_t common_sflash_is_busy( sflash_handle_t* sflash_handle )
{
    return sflash_is_status_register_bit_set( sflash_handle, STATUS_REGISTER_BIT_BUSY );
}

static int common_sflash_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode,
        sflash_command_t normal_read, sflash_command_t fast_read, sflash_command_t highspeed_read, sflash_command_t highspeed_x4io_read )
{
    int result = SFLASH_MSG_OK;
    char* rx_data_ptr = ( char* ) rx_data;
    sflash_command_t command;

    switch ( mode )
    {
#ifdef APPLICATION_XIP_ENABLE
        case CONFIG_XIP_READ_DATA :
#endif /* APPLICATION_XIP_ENABLE */
        case AUTO_SPEED_READ_DATA :
            if ( sflash_handle->capabilities->speed_advance->high_speed_read_dummy_cycle > 0 )
            {
                command = ( sflash_handle->capabilities->speed_advance->high_speed_read_x4io_support == WICED_TRUE ? highspeed_x4io_read : highspeed_read );
            }
            else if ( sflash_handle->capabilities->speed_advance->fast_read_dummy_cycle > 0 )
            {
                command = fast_read;
            }
            else
            {
                command = normal_read;
            }
            WPRINT_SFLASH_DEBUG( ( "Speed : AUTO_SPEED_READ_DATA (0x%x)\n", command ) );
            break;
        case HIGH_SPEED_READ_DATA :
            command = ( sflash_handle->capabilities->speed_advance->high_speed_read_x4io_support == WICED_TRUE ? highspeed_x4io_read : highspeed_read );
            WPRINT_SFLASH_DEBUG( ( "Speed : HIGH_SPEED_READ_DATA (0x%x)\n", command ) );
            break;
        case FAST_READ_DATA :
            command = fast_read;
            WPRINT_SFLASH_DEBUG( ( "Speed : FAST_READ_DATA (0x%x)\n", command ) );
            break;
        case NORMAL_READ_DATA :
            command = normal_read;
            WPRINT_SFLASH_DEBUG( ( "Speed : NORMAL_READ_DATA (0x%x)\n", command ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Wrong READ SPEED mode !!!\n" ) );
            return SFLASH_MSG_ERROR;
            break;
    };

#ifdef APPLICATION_XIP_ENABLE
    if ( mode == CONFIG_XIP_READ_DATA )
    {
        return bcm4390x_sflash_controller_configure_xip( sflash_handle, command );
    }
#endif

#if ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) )
            /* For generic SPI, it only support 1-bit READ */
            if ( ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_0  ) ||
                    ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_1  ) )
            {
                command = SFLASH_READ;
            }
#endif /* ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) ) */

    result = sflash_read_write_process( sflash_handle, command, device_address, rx_data_ptr, data_len, READ_PROCESS );

    return result;
}

static int common_sflash_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode,
        sflash_command_t normal_write, sflash_command_t highspeed_write, sflash_command_t highspeed_x4io_write)
{
    int result = SFLASH_MSG_OK;
    char* tx_data_ptr = ( char* ) tx_data;
    sflash_command_t command;

    if ( sflash_handle->write_allowed != SFLASH_WRITE_ALLOWED )
    {
        WPRINT_SFLASH_ERROR( ( "!!! Warning, You do not have permission to write data. (%s) !!!\n", __FUNCTION__ ) );
        return SFLASH_MSG_ERROR;
    }

    switch ( mode )
    {
        case AUTO_SPEED_WRITE_DATA :
            if ( sflash_handle->capabilities->speed_advance->high_speed_write_support == WICED_TRUE )
            {
                command = ( sflash_handle->capabilities->speed_advance->high_speed_write_x4io_support == WICED_TRUE ? highspeed_x4io_write : highspeed_write );
            }
            else
            {
                command = SFLASH_WRITE;
            }
            WPRINT_SFLASH_DEBUG( ( "Speed : AUTO_SPEED_WRITE_DATA (0x%x)\n", command ) );
            break;
        case HIGH_SPEED_WRITE_DATA :
            command = ( sflash_handle->capabilities->speed_advance->high_speed_write_x4io_support == WICED_TRUE ? highspeed_x4io_write : highspeed_write );
            WPRINT_SFLASH_DEBUG( ( "Speed : HIGH_SPEED_WRITE_DATA (0x%x)\n", command ) );
            break;
        case NORMAL_WRITE_DATA :
            command = normal_write;
            WPRINT_SFLASH_DEBUG( ( "Speed : NORMAL_WRITE_DATA (0x%x)\n", command ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Wrong WRITE SPEED mode !!!\n" ) );
            return SFLASH_MSG_ERROR;
            break;
    };

#if ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) )
            /* For generic SPI, it only support 1-bit WRITE */
            if ( ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_0  ) ||
                    ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_1  ) )
            {
                command = SFLASH_WRITE;
            }
#endif /* ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) ) */

    result = sflash_read_write_process( sflash_handle, command, device_address, tx_data_ptr, data_len, WRITE_PROCESS );

    return result;
}

static int common_sflash_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode,
        sflash_command_t erase_4K, sflash_command_t erase_64K, sflash_command_t erase_all )
{
    int result = SFLASH_MSG_OK;
    uint32_t data_length = 0;
    sflash_command_t command;

    if ( sflash_handle->write_allowed != SFLASH_WRITE_ALLOWED )
    {
        WPRINT_SFLASH_ERROR( ( "!!! Warning, You do not have permission to erase data. (%s) !!!\n", __FUNCTION__ ) );
        return SFLASH_MSG_ERROR;
    }

    switch ( mode )
    {
        case ERASE_ALL :
            command = erase_all;
            WPRINT_SFLASH_DEBUG( ( "ERASE : all chip erase !\n" ) );
            break;
        case ERASE_BLOCK :
            command = erase_64K;
            WPRINT_SFLASH_DEBUG( ( "ERASE : erase 64KB on address(0x%lx) !\n", device_address ) );
            break;
        case ERASE_SECTOR :
            command = erase_4K;
            WPRINT_SFLASH_DEBUG( ( "ERASE : erase 4KB on address(0x%lx) !\n", device_address ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Wrong ERASE mode !!!\n" ) );
            return SFLASH_MSG_ERROR;
            break;
    };

    result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
    if (result != SFLASH_MSG_OK)
    {
        goto erase_fail;
    }
    if ( mode == ERASE_ALL )
    {
        result = sflash_generic_command( sflash_handle, command, NULL );
    }
    else
    {
        result = platform_interface_transfer_receive( sflash_handle, command, device_address, NULL, &data_length );
    }
    if (result != SFLASH_MSG_OK)
    {
        goto erase_fail;
    }
    result = sflash_wait_busy_done( sflash_handle );
    if (result != SFLASH_MSG_OK)
    {
        goto erase_fail;
    }
    WPRINT_SFLASH_DEBUG( ( "done!\n" ) );

erase_fail:

    return result;
}

static int common_sflash_3bytes_address_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode )
{
    return common_sflash_read_data( sflash_handle, device_address, rx_data, data_len, mode, SFLASH_READ, SFLASH_FAST_READ, SFLASH_QUAD_READ, SFLASH_X4IO_READ);
}

static int common_sflash_3bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode )
{
    return common_sflash_write_data( sflash_handle, device_address, tx_data, data_len, mode, SFLASH_WRITE, SFLASH_QUAD_WRITE, SFLASH_X4IO_WRITE );
}

static int common_sflash_3bytes_address_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode )
{
    return common_sflash_erase_data( sflash_handle, device_address, mode, SFLASH_4K_ERASE, SFLASH_64K_ERASE, SFLASH_CHIP_ERASE );
}

static int common_sflash_4bytes_address_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode )
{
    return common_sflash_read_data( sflash_handle, device_address, rx_data, data_len, mode, SFLASH_READ4B, SFLASH_FAST_READ4B, SFLASH_QUAD_READ4B, SFLASH_X4IO_READ4B);
}

static int common_sflash_4bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode )
{
    return common_sflash_write_data( sflash_handle, device_address, tx_data, data_len, mode, SFLASH_WRITE4B, SFLASH_QUAD_WRITE4B, SFLASH_QUAD_WRITE4B );
}

static int common_sflash_4bytes_address_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode )
{
    return common_sflash_erase_data( sflash_handle, device_address, mode, SFLASH_4K_ERASE4B, SFLASH_64K_ERASE4B, SFLASH_CHIP_ERASE );
}

/*****************************************
 *   Area of sflash specific definition  *
 *****************************************/
 /* SFLASH - MACRONIX */
#ifdef SFLASH_SUPPORT_MACRONIX_PARTS
static int macronix_sflash_4bytes_address_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode )
{
    int result = SFLASH_MSG_OK;
    char* tx_data_ptr = ( char* ) tx_data;
    sflash_command_t command;

    if ( sflash_handle->write_allowed != SFLASH_WRITE_ALLOWED )
    {
        WPRINT_SFLASH_ERROR( ( "!!! Warning, You do not have permission to write data. (%s) !!!\n", __FUNCTION__ ) );
        return SFLASH_MSG_ERROR;
    }

    switch ( mode )
    {
        case AUTO_SPEED_WRITE_DATA :
            if ( sflash_handle->capabilities->speed_advance->high_speed_write_support == WICED_TRUE )
            {
                command = ( sflash_handle->capabilities->speed_advance->high_speed_write_x4io_support == WICED_TRUE ? SFLASH_X4IO_WRITE : SFLASH_QUAD_WRITE );
            }
            else
            {
                command = SFLASH_WRITE;
            }
            WPRINT_SFLASH_DEBUG( ( "Speed : AUTO_SPEED_WRITE_DATA (0x%x)\n", command ) );
            break;
        case HIGH_SPEED_WRITE_DATA :
            command = ( sflash_handle->capabilities->speed_advance->high_speed_write_x4io_support == WICED_TRUE ? SFLASH_X4IO_WRITE : SFLASH_QUAD_WRITE );
            WPRINT_SFLASH_DEBUG( ( "Speed : HIGH_SPEED_WRITE_DATA (0x%x)\n", command ) );
            break;
        case NORMAL_WRITE_DATA :
            command = SFLASH_WRITE;
            WPRINT_SFLASH_DEBUG( ( "Speed : NORMAL_WRITE_DATA (0x%x)\n", command ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Wrong WRITE SPEED mode !!!\n" ) );
            return SFLASH_MSG_ERROR;
            break;
    };

    result = sflash_generic_command( sflash_handle, SFLASH_EN4B, NULL );
    if ( result != SFLASH_MSG_OK )
    {
        goto fail;
    }
    result = sflash_read_write_process( sflash_handle, command, device_address, tx_data_ptr, data_len, WRITE_PROCESS );
    if ( result != SFLASH_MSG_OK )
    {
        goto fail;
    }
    result = sflash_generic_command( sflash_handle, SFLASH_EX4B, NULL );
    if ( result != SFLASH_MSG_OK )
    {
        goto fail;
    }
fail :
    return result;
}
#endif

 /* SFLASH - MICRON */
#ifdef SFLASH_SUPPORT_MICRON_PARTS
static int micron_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    int result;
    uint32_t data = 0;

    UNUSED_PARAMETER( mode );

    /* Disable function of HOLD# for Quad mode */
    result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - set WRITE_ENABLE, Fail!!!\n", __FUNCTION__ ) );
        return result;
    }
    result = sflash_generic_command( sflash_handle, SFLASH_READ_ENH_VOLATILE_REGISTER, &data );
    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - read SFLASH_READ_ENH_VOLATILE_REGISTER, Fail!!!\n", __FUNCTION__ ) );
        return result;
    }
    data &= ( unsigned char )~( MICRON_SFLASH_ENH_VOLATILE_STATUS_REGISTER_HOLD );
    result = sflash_generic_command( sflash_handle, SFLASH_WRITE_ENH_VOLATILE_REGISTER, &data );
    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - write SFLASH_READ_ENH_VOLATILE_REGISTER, Fail!!!\n", __FUNCTION__ ) );
        return result;
    }
    WPRINT_SFLASH_INFO( ( "Ignoring HOLD# for Quad mode, done!\n" ) );
    WPRINT_SFLASH_INFO( ( "MICRON Sflash not support to configure Quad mode. ( default : enabled )\n" ) );
    return result;
}

static wiced_bool_t micron_sflash_is_quad_mode( sflash_handle_t* sflash_handle )
{
    UNUSED_PARAMETER( sflash_handle );
    return WICED_TRUE;
}

static int micron_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2 |
                    MICRON_STATUS_REGISTER_BIT_BLOCK_PROTECT_3;
    return sflash_set_block_protect( sflash_handle, mode, mask, micron_block_protect_bit_map );
}

static int micron_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t *protect_level )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2 |
                    MICRON_STATUS_REGISTER_BIT_BLOCK_PROTECT_3;
    return sflash_query_block_protect( sflash_handle, protect_level, mask, micron_block_protect_bit_map );
}
#endif /* SFLASH_SUPPORT_MICRON_PARTS */

 /* SFLASH - WINBOND */
#ifdef SFLASH_SUPPORT_WINBOND_PARTS
static int winbond_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    return sflash_set_quad_mode( sflash_handle, mode, WINBOND_STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static wiced_bool_t winbond_sflash_is_quad_mode( sflash_handle_t* sflash_handle )
{
    return sflash_is_status_register_bit_set( sflash_handle, WINBOND_STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static int winbond_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2;
    return sflash_set_block_protect( sflash_handle, mode, mask, winbond_block_protect_bit_map );
}

static int winbond_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2;
    return sflash_query_block_protect( sflash_handle, protect_level, mask, winbond_block_protect_bit_map );
}

static int winbond_sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode)
{
    int result;
    uint32_t write_data_len = WINBOND_STATUS_REGISTER_DATA_LENGTH;
    uint32_t *target_data = ( ( uint32_t* ) data );
    uint32_t *target_mask = ( ( uint32_t* ) mask );
    uint32_t temp_data = *( ( uint32_t* ) data );
    uint32_t target_data2;

    result = sflash_generic_command( sflash_handle, SFLASH_READ_STATUS_REGISTER, target_data );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }
    result = sflash_generic_command( sflash_handle, SFLASH_READ_STATUS_REGISTER2, &target_data2 );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }
    *target_data = ( unsigned short )( ( target_data2 << 8 ) | *target_data );

    switch ( mode )
    {
        case SETUP_ACTION :
            if ( ( *target_data & *target_mask ) == temp_data )
            {
                WPRINT_SFLASH_DEBUG( ( "Status register is no change!\n" ) );
                return SFLASH_MSG_OK;
            }

            *target_data &= ~( *target_mask );
            *target_data |= temp_data;
            result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            result = platform_interface_transfer_receive( sflash_handle, SFLASH_WRITE_STATUS_REGISTER, 0, target_data, &write_data_len );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            result = sflash_wait_busy_done( sflash_handle );
            if ( result != SFLASH_MSG_OK )
            {
                return SFLASH_MSG_ERROR;
            }
            WPRINT_SFLASH_DEBUG( ( "[SETUP_ACTION] done!\n" ) );
            break;
        case QUERY_ACTION :
            WPRINT_SFLASH_DEBUG( ( "[QUERY_ACTION] Latest register status = %d\n", *( ( int* ) data ) ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Warning, use wrong mode for %s !!!\n", __FUNCTION__ ) );
            return SFLASH_MSG_ERROR;
            break;
    }
    return result;
}
#endif /* SFLASH_SUPPORT_WINBOND_PARTS */

 /* SFLASH - SST */
#ifdef SFLASH_SUPPORT_SST_PARTS
static int sst_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    UNUSED_PARAMETER( sflash_handle );
    UNUSED_PARAMETER( mode );

    WPRINT_SFLASH_ERROR( ( "SST Sflash not support Quad mode.\n" ) );
    return SFLASH_MSG_ERROR;
}

static wiced_bool_t sst_sflash_is_quad_mode( sflash_handle_t* sflash_handle )
{
    UNUSED_PARAMETER( sflash_handle );
    return WICED_FALSE;
}
#endif /* SFLASH_SUPPORT_SST_PARTS */

#ifdef SFLASH_SUPPORT_CYPRESS_PARTS
static int cypress_sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    return sflash_set_quad_mode( sflash_handle, mode, CYPRESS_STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static wiced_bool_t cypress_sflash_is_quad_mode( sflash_handle_t* sflash_handle )
{
    return sflash_is_status_register_bit_set( sflash_handle, CYPRESS_STATUS_REGISTER_BIT_QUAD_ENABLE );
}

static int cypress_sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2;
    return sflash_set_block_protect( sflash_handle, mode, mask, cypress_block_protect_bit_map );
}

static int cypress_sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level )
{
    uint32_t mask = STATUS_REGISTER_BIT_BLOCK_PROTECT_0 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_1 |
                    STATUS_REGISTER_BIT_BLOCK_PROTECT_2;
    return sflash_query_block_protect( sflash_handle, protect_level, mask, cypress_block_protect_bit_map );
}

static int cypress_sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode)
{
    int result;
    uint32_t write_data_len = CYPRESS_STATUS_REGISTER_DATA_LENGTH;
    uint32_t register_data=0, register_data2=0, new_register_data=0;
    uint32_t bit_on = *( ( uint32_t* ) data );
    uint32_t bit_mask = *( ( uint32_t* ) mask );

    result = sflash_generic_command( sflash_handle, SFLASH_READ_STATUS_REGISTER, &register_data );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }
    result = sflash_generic_command( sflash_handle, SFLASH_READ_STATUS_REGISTER2, &register_data2 );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }

    new_register_data = ( unsigned short )( ( register_data2 << 8 ) | register_data );

    switch ( mode )
    {
        case SETUP_ACTION :
            if ( ( new_register_data & bit_mask ) == bit_on )
            {
                WPRINT_SFLASH_DEBUG( ( "Status register is no change!\n" ) );
                return SFLASH_MSG_OK;
            }

            new_register_data &= ~( bit_mask );
            new_register_data |= bit_on;

            if ( bit_mask == CYPRESS_STATUS_REGISTER_BIT_QUAD_ENABLE )
            {
                result = sflash_generic_command( sflash_handle, SFLASH_WRITE_ENABLE_VOLATILE_REGISTER, NULL );
            }
            else
            {
                result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
            }
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }

            result = platform_interface_transfer_receive( sflash_handle, SFLASH_WRITE_STATUS_REGISTER, 0, &new_register_data, &write_data_len );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            result = sflash_wait_busy_done( sflash_handle );
            if ( result != SFLASH_MSG_OK )
            {
                return SFLASH_MSG_ERROR;
            }
            WPRINT_SFLASH_DEBUG( ( "[SETUP_ACTION] done!\n" ) );
            break;
        case QUERY_ACTION :
            *( ( uint32_t* ) data) = new_register_data;
            WPRINT_SFLASH_DEBUG( ( "[QUERY_ACTION] Latest register status = %ld\n", new_register_data ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Warning, use wrong mode for %s !!!\n", __FUNCTION__ ) );
            return SFLASH_MSG_ERROR;
            break;
    }
    return result;
}
#endif /* SFLASH_SUPPORT_CYPRESS_PARTS */

#if ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) )
static int cypress_fram_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode )
{
    UNUSED_PARAMETER( mode );
    return platform_interface_transfer_receive( sflash_handle, SFLASH_READ, device_address, rx_data, &data_len );
}

static int cypress_fram_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode )
{
    int result;

    UNUSED_PARAMETER( mode );

    result = sflash_generic_command( sflash_handle, SFLASH_WRITE_ENABLE, NULL );
    if (result != SFLASH_MSG_OK)
    {
        return result;
    }

    return platform_interface_transfer_receive( sflash_handle, SFLASH_WRITE, device_address, tx_data, &data_len );
}

static int cypress_fram_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode )
{
    UNUSED_PARAMETER( sflash_handle );
    UNUSED_PARAMETER( device_address );
    UNUSED_PARAMETER( mode );
    return SFLASH_MSG_OK;
}
#endif /* ( defined( FRAM_SPI_0 ) || defined( FRAM_SPI_1 ) ) */

/*********************************
 * Sflash driver private function *
 *********************************/
static int sflash_read_id( sflash_handle_t* sflash_handle, void* rx_data )
{
    int result;

    result = sflash_generic_command( sflash_handle, SFLASH_READID_JEDEC_ID, rx_data );
    *( ( uint32_t* )rx_data ) &= SFLASHID_MASK;
    WPRINT_SFLASH_DEBUG( ( "Get Sflash ID (0x%lx)\n", *( ( uint32_t* )rx_data ) ) );

    return result;
}

static void sflash_setup_capability( sflash_handle_t* sflash_handle, uint32_t sflash_id )
{
    const sflash_capabilities_table_element_t* capabilities_element = sflash_capabilities_tables;

    while ( ( capabilities_element->device_id != SFLASH_ID_DEFAULT ) &&
            ( capabilities_element->device_id != sflash_id ) )
    {
        capabilities_element++;
    }

    sflash_handle->read_blocking = WICED_TRUE;
    sflash_handle->capabilities = &capabilities_element->capabilities;
    sflash_handle->device_id = capabilities_element->device_id;

    if ( capabilities_element->device_id != sflash_id )
    {
        WPRINT_SFLASH_INFO( ( "!!!  Sflash will work in generic 1-bit mode ( unknown sflash id : 0x%lx )  !!! \n", sflash_id ) );
    }
}

static int sflash_generic_command( sflash_handle_t* sflash_handle, sflash_command_t command, void* data )
{
    int result;
    uint32_t data_length = sflash_get_command_data_length( command );

    result = platform_interface_transfer_receive( sflash_handle, command, 0, data, &data_length );

    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "[%s] - issue sflash command(0x%x) fail !\n\n", __FUNCTION__, command ) );
    }
    return result;
}

static int sflash_status_register( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode )
{
    int result;
    uint32_t *target_data = ( ( uint32_t* ) data );
    uint32_t *target_mask = ( ( uint32_t* ) mask );
    uint32_t temp_data = *( ( uint32_t* ) data );

    result = sflash_generic_command( sflash_handle, SFLASH_READ_STATUS_REGISTER, target_data );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }

    switch ( mode )
    {
        case SETUP_ACTION :
            if ( ( *target_data & *target_mask ) == temp_data )
            {
                WPRINT_SFLASH_DEBUG( ( "Status register is no change!\n" ) );
                return SFLASH_MSG_OK;
            }

            *target_data &= ~( *target_mask );
            *target_data |= temp_data;
            result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            result = sflash_generic_command( sflash_handle, SFLASH_WRITE_STATUS_REGISTER, target_data );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            result = sflash_wait_busy_done( sflash_handle );
            if ( result != SFLASH_MSG_OK )
            {
                return result;
            }
            WPRINT_SFLASH_DEBUG( ( "[SETUP_ACTION] done!\n" ) );
            break;
        case QUERY_ACTION :
            WPRINT_SFLASH_DEBUG( ( "[QUERY_ACTION] Latest register status = %d\n", *( ( int* ) data ) ) );
            break;
        default :
            WPRINT_SFLASH_ERROR( ( "!!! Warning, use wrong mode for %s !!!\n", __FUNCTION__ ) );
            return SFLASH_MSG_ERROR;
            break;
    }
    return result;
}

static int sflash_set_quad_mode( sflash_handle_t* sflash_handle, onoff_action_t mode, uint32_t mask_bit )
{
    uint32_t data = 0;
    uint32_t mask = mask_bit;

    if ( mode == TURN_OFF )
    {
        data &= ~( mask );
    }
    else
    {
        data |= ( mask );
    }
    return sflash_handle->capabilities->action->status_register( sflash_handle, &mask, &data, SETUP_ACTION );
}

static int sflash_set_block_protect( sflash_handle_t* sflash_handle, block_protect_action_t mode, uint32_t mask_bit, uint32_t* map_bit )
{
    uint32_t data = 0;
    uint32_t mode_to_size = 0;
    uint32_t mask = mask_bit;

    BLOCK_PROTECT_MODE_TO_SIZE( mode, mode_to_size );
    if ( sflash_handle->capabilities->total_size < ( mode_to_size * KBYTE ) )
    {
        WPRINT_SFLASH_ERROR( ( "WARNING!! You want to protect size of %d bytes is large than Sflash total size ( %d bytes )\n", ( int )( mode_to_size * KBYTE ), ( int )sflash_handle->capabilities->total_size ) );
        WPRINT_SFLASH_ERROR( ( "Executing Protect ALL!\n" ) );
        BLOCK_PROTECT_SIZE_TO_MODE( ( int )(sflash_handle->capabilities->total_size / KBYTE), mode );
    }

    data = ( ( uint32_t* ) map_bit )[ mode ];

    return sflash_handle->capabilities->action->status_register( sflash_handle, &mask, &data, SETUP_ACTION );
}

static int sflash_query_block_protect( sflash_handle_t* sflash_handle, uint32_t* protect_level, uint32_t mask_bit, uint32_t* map_bit )
{
    int result;
    uint32_t i;
    uint32_t data = 0;
    uint32_t mask = mask_bit;

    result = sflash_handle->capabilities->action->status_register( sflash_handle, &mask, &data, QUERY_ACTION );
    if (result != SFLASH_MSG_OK)
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Query fail !!!\n", __FUNCTION__ ) );
    }
    else
    {
        for( i = 0; i < BLOCK_PRTOECT_MAX_ENUM; i++ )
        {
            if ( ( ( uint32_t* ) map_bit )[ i ] == ( data & mask ) )
            {
                *protect_level = i;
                return SFLASH_MSG_OK;;
            }
        }
    }
    return SFLASH_MSG_ERROR;
}

static int sflash_set_write_enable( sflash_handle_t* sflash_handle, onoff_action_t mode )
{
    int result;

    if ( mode == TURN_OFF )
    {
        result = sflash_generic_command( sflash_handle, SFLASH_WRITE_DISABLE, NULL );
    }
    else
    {
        result = sflash_generic_command( sflash_handle, SFLASH_WRITE_ENABLE, NULL );
    }
    result = sflash_wait_busy_done( sflash_handle );
    return result;
}

static int sflash_reset( sflash_handle_t* sflash_handle )
{
    int result;

    result = sflash_generic_command( sflash_handle, SFLASH_RESET_ENABLE, NULL );
    if ( result != SFLASH_MSG_OK )
    {
        return result;
    }
    result = sflash_generic_command( sflash_handle, SFLASH_RESET, NULL );
    return result;
}

static wiced_bool_t sflash_is_status_register_bit_set( sflash_handle_t* sflash_handle, uint32_t mask_bit )
{
    int result;
    uint32_t data = 0;
    uint32_t mask = mask_bit;

    result = sflash_handle->capabilities->action->status_register( sflash_handle, &mask, &data, QUERY_ACTION );
    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Query fail!\n", __FUNCTION__ ) );
    }
    else
    {
        if ( ( data & mask ) != 0 )
        {
            return WICED_TRUE;
        }
    }
    return WICED_FALSE;
}

static int sflash_wait_busy_done( sflash_handle_t* sflash_handle )
{
    uint32_t start_time, end_time;
    start_time = WICED_GET_TIME();
    while ( sflash_handle->capabilities->action->is_busy( sflash_handle ) == WICED_TRUE )
    {
        end_time = WICED_GET_TIME();
        if ( end_time - start_time > MAX_TIMEOUT_FOR_FLASH_BUSY )
        {
            WPRINT_SFLASH_ERROR( ( "Sflash always busy over %ld ms ! ( default : %d ms)\n", ( end_time - start_time ), MAX_TIMEOUT_FOR_FLASH_BUSY ) );
            return SFLASH_MSG_ERROR;
        }
    }
    return SFLASH_MSG_OK;
}

static uint32_t sflash_get_command_data_length( sflash_command_t command )
{
    if ( ( command == SFLASH_WRITE_STATUS_REGISTER ) || ( command == SFLASH_READ_STATUS_REGISTER) ||
         ( command == SFLASH_READ_STATUS_REGISTER2) || ( command == SFLASH_WRITE_ENH_VOLATILE_REGISTER ) ||
         ( command == SFLASH_READ_ENH_VOLATILE_REGISTER ) )
    {
        return 1;
    }
    else if ( command == SFLASH_READID_JEDEC_ID )
    {
        return 4;
    }
    return 0;
}

static void sflash_deinit_by_stage( sflash_handle_t* sflash_handle, int8_t init_stage )
{
    sflash_interface_t* interface = NULL;

    interface = platform_interface_get( sflash_handle->hw_interface );

    switch ( init_stage )
    {
        case WICED_SFLASH_INIT_STAGE_CLEAR_ALL:
            /* De-Initialize semaphore */
            if ( PLATFORM_INTERFACE_SEMAPHORE( interface->hw_interface, SEMAPHORE_DEINIT ) != SFLASH_MSG_OK )
            {
                WPRINT_SFLASH_ERROR( ( "!!! [%s] - Semaphore de-initialize fail!!!\n", __FUNCTION__ ) );
            }
            /* Fall-Through */
        case WICED_SFLASH_INIT_STAGE_2:
            /* clear sflash_handle */
            WPRINT_SFLASH_DEBUG(("de-initialize sflash_handle\n"));
            memset( sflash_handle, 0, sizeof( sflash_handle_t ) );
            /* Fall-Through */
        case WICED_SFLASH_INIT_STAGE_1:
            /* de-initialize HW */
            WPRINT_SFLASH_DEBUG(("de-initialize HW\n"));
            platform_interface_deinit( interface->hw_interface );
            break;
        default:
            /* de-initialize sflash with wrong init stage */
            WPRINT_SFLASH_ERROR(("de-initialize sflash with wrong init stage! (%d)\n", init_stage));
            break;
    }
}

static int sflash_read_write_process( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t data_len, read_write_mode_t mode )
{
    int result = SFLASH_MSG_OK;
    char* data_ptr = ( char* ) data;
    uint32_t data_size;

    while ( data_len > 0 )
    {
        if ( mode == READ_PROCESS )
        {
            data_size = data_len;
            result = platform_interface_transfer_receive( sflash_handle, command, device_address, data_ptr, &data_size );
            if ( result != SFLASH_MSG_OK )
            {
                //exit the while loop when encounter error.
                break;
            }
        }
        else if ( mode == WRITE_PROCESS )
        {
            uint32_t max_page_size = sflash_handle->capabilities->max_page_size;
            data_size = ( data_len >= max_page_size )? max_page_size : data_len;
            /* All transmitted data must not go beyond the end of the current page in a write */
            data_size = MIN( max_page_size - ( device_address % max_page_size ), data_size );

            result = sflash_handle->capabilities->action->set_write_enable( sflash_handle, TURN_ON );
            if (result != SFLASH_MSG_OK)
            {
                //exit the while loop when encounter error.
                break;
            }
            result = platform_interface_transfer_receive( sflash_handle, command, device_address, data_ptr, &data_size );
            if (result != SFLASH_MSG_OK)
            {
                //exit the while loop when encounter error.
                break;
            }
            result = sflash_wait_busy_done( sflash_handle );
            if (result != SFLASH_MSG_OK)
            {
                //exit the while loop when encounter error.
                break;
            }
        }

        data_len -= data_size;
        data_ptr += data_size;
        device_address += data_size;
        WPRINT_SFLASH_DEBUG( ( "done!\n" ) );
    }
    return result;
}

static sflash_interface_t* platform_interface_get( hw_interface_t hw_interface )
{
    sflash_interface_t* interface = sflash_interface;
    uint8_t i;

    for( i = 0; i < ( sizeof( sflash_interface ) / sizeof( sflash_interface_t ) ); i++ )
    {
        if ( interface->hw_interface == hw_interface )
        {
            /* found interface */
            return interface;
        }
        interface++;
    }

    return NULL;
}

static int platform_interface_init( hw_interface_t hw_interface )
{
    int results = SFLASH_MSG_ERROR;

    if ( hw_interface == INTERFACE_4390x_SFLASH )
    {
        spi_layer_init();
        results = SFLASH_MSG_OK;
    }
#ifdef ENABLE_SPI_0
    else if ( hw_interface == INTERFACE_4390x_SPI_0 )
    {
#ifndef TINY_BOOTLOADER
        wiced_spi_init( &wiced_spi_fram );
        results = SFLASH_MSG_OK;
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_0 */
#ifdef ENABLE_SPI_1
    else if ( hw_interface == INTERFACE_4390x_SPI_1 )
    {
#ifndef TINY_BOOTLOADER
        wiced_spi_init( &wiced_spi_flash );
        results = SFLASH_MSG_OK;
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_1 */

    return results;
}

static int platform_interface_deinit( hw_interface_t hw_interface )
{
    int results = SFLASH_MSG_ERROR;

    if ( hw_interface == INTERFACE_4390x_SFLASH )
    {
        spi_layer_deinit();
        results = SFLASH_MSG_OK;
    }
#ifdef ENABLE_SPI_0
    else if ( hw_interface == INTERFACE_4390x_SPI_0 )
    {
#ifndef TINY_BOOTLOADER
        wiced_spi_deinit( &wiced_spi_fram );
        results = SFLASH_MSG_OK;
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_0 */
#ifdef ENABLE_SPI_1
    else if ( hw_interface == INTERFACE_4390x_SPI_1 )
    {
#ifndef TINY_BOOTLOADER
        wiced_spi_deinit( &wiced_spi_flash );
        results = SFLASH_MSG_OK;
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_1 */

    return results;
}

static int platform_interface_transfer_receive( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length )
{
    int result = SFLASH_MSG_OK;

    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        result = spi_sflash_send_command( sflash_handle, command, device_address, data, data_length );
    }
#ifdef ENABLE_SPI_0
    else if ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_0 )
    {
#ifndef TINY_BOOTLOADER
        result = platform_interface_handle_spi_segments( sflash_handle, command, device_address, data, data_length, &wiced_spi_fram );
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_0 */
#ifdef ENABLE_SPI_1
    else if ( sflash_handle->hw_interface == INTERFACE_4390x_SPI_1 )
    {
#ifndef TINY_BOOTLOADER
        result = platform_interface_handle_spi_segments( sflash_handle, command, device_address, data, data_length, &wiced_spi_flash );
#endif /* TINY_BOOTLOADER*/
    }
#endif /* ENABLE_SPI_1 */

    return result;
}

#if ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) )
static int platform_interface_handle_spi_segments( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length, const wiced_spi_device_t* spi )
{
    uint32_t i;
    uint32_t device_address_tmp_length = 0;
    char curr_device_address[3];
    void *data_MOSI = NULL;
    void *data_MISO = NULL;
    wiced_spi_message_segment_t segments[3];

    WPRINT_SFLASH_DEBUG( ( "!!! command = 0x%02x, device_address = 0x%08lx, data_length = %ld!!!\n", command, device_address, *data_length ) );

    for ( i = 0; i < ( sizeof( opcode_purpose_list ) / sizeof( opcode_purpose_list[0] ) ); i++ )
    {
        if ( opcode_purpose_list[i].command == command )
        {
            if ( opcode_purpose_list[i].purpose == COMMAND_TXDATA )
            {
                data_MOSI = data;
            }

            if ( opcode_purpose_list[i].purpose == COMMAND_RXDATA )
            {
                data_MISO = data;
            }

            if ( opcode_purpose_list[i].with_address == WICED_TRUE )
            {
                if ( sflash_handle->device_id == FRAM_ID_FM25W256 )
                {
                    curr_device_address[0] = (char)( ( device_address & 0x0000FF00 ) >> 8 );
                    curr_device_address[1] = (char)( ( device_address & 0x000000FF ) >> 0 );
                    device_address_tmp_length = 2;
                }
                else
                {
                    curr_device_address[0] = (char)( ( device_address & 0x00FF0000 ) >> 16 );
                    curr_device_address[1] = (char)( ( device_address & 0x0000FF00 ) >>  8 );
                    curr_device_address[2] = (char)( ( device_address & 0x000000FF ) >>  0 );
                    device_address_tmp_length = 3;
                }
            }
        }
    }

    segments[0].tx_buffer = &command;
    segments[0].rx_buffer = NULL;
    segments[0].length = 1;

    segments[1].tx_buffer = curr_device_address;
    segments[1].rx_buffer = NULL;
    segments[1].length = device_address_tmp_length;

    segments[2].tx_buffer = data_MOSI;
    segments[2].rx_buffer = data_MISO;
    segments[2].length = *data_length;

    return wiced_spi_transfer( spi, (wiced_spi_message_segment_t*) segments, 3 );
}
#endif /* ( ( defined( ENABLE_SPI_0 ) || defined( ENABLE_SPI_1 ) ) && !defined( TINY_BOOTLOADER ) ) */

/*********************************
 * Sflash driver public function *
 *********************************/
int bcm4390x_sflash_init( sflash_handle_t* sflash_handle, void* peripheral_id, sflash_write_allowed_t write_allowed_in )
{
    int result = SFLASH_MSG_OK;
    uint32_t sflash_status_id = SFLASH_ID_DEFAULT;
    sflash_interface_t* interface = NULL;
    hw_interface_t hw_interface;
#ifndef SFLASH_SINGLE_THREAD_MODE
    uint32_t flags = 0;
#endif /* SFLASH_SINGLE_THREAD_MODE */

    UNUSED_PARAMETER( peripheral_id );
    UNUSED_PARAMETER( write_allowed_in );

#ifndef SFLASH_SINGLE_THREAD_MODE
    /* Save INTERRUPTS here for make sure ONLY ONE thread can
     * go through wiced_sflash_init in one time. */
    WICED_SAVE_INTERRUPTS( flags );
#endif /* SFLASH_SINGLE_THREAD_MODE */

    if ( peripheral_id == NULL )
    {
        /* For compatibility of the older caller/apps always leave peripheral_id as 0 when accessing sflash interface of 4390x. */
        hw_interface = INTERFACE_4390x_SFLASH;
    }
    else
    {
        hw_interface = *( ( hw_interface_t* ) peripheral_id );
    }

    interface = platform_interface_get( hw_interface );

    if ( interface == NULL )
    {
        WPRINT_SFLASH_INFO( ( "!!! [%s] - The interface didn't exist !!!\n", __FUNCTION__ ) );
        result = SFLASH_MSG_ERROR;
        goto init_fail;
    }

    if ( ( interface->sflash_status_init_count == 0 ) &&
            ( interface->sflash_handle_save->capabilities == NULL ) )
    {
        /* Init stage 0, Initialize HW */
        platform_interface_init( hw_interface );
        interface->sflash_handle_save->hw_interface = hw_interface;

#if ( defined( ENABLE_SPI_0 ) & defined( FRAM_SPI_0 ) )
        if ( ( interface->sflash_handle_save->hw_interface == INTERFACE_4390x_SPI_0  ) && ( FRAM_SPI_0 != 0 ) )
        {
            /* assign FRAM ID manually */
            sflash_status_id = FRAM_SPI_0;
        }
#endif /* ( defined( ENABLE_SPI_0 ) & defined( FRAM_SPI_0 ) ) */
#if ( defined( ENABLE_SPI_1 ) & defined( FRAM_SPI_1 ) )
        if ( ( interface->sflash_handle_save->hw_interface == INTERFACE_4390x_SPI_1  ) && ( FRAM_SPI_1 != 0 ) )
        {
            /* assign FRAM ID manually */
            sflash_status_id = FRAM_SPI_1;
        }
#endif /* ( defined( ENABLE_SPI_1 ) & defined( FRAM_SPI_1 ) ) */

        if ( sflash_status_id == SFLASH_ID_DEFAULT )
        {
            /* Apply default sflash configuration for reading Sflash ID. */
            sflash_setup_capability( interface->sflash_handle_save, sflash_status_id );

            /* Init stage 1, Read Sflash ID */
            if ( ( result = sflash_read_id( interface->sflash_handle_save, &sflash_status_id ) ) != SFLASH_MSG_OK )
            {
                WPRINT_SFLASH_ERROR( ( "!!! [%s] - Get Sflash ID fail!!!\n", __FUNCTION__ ) );
                /* Initialize sflash fail, reset :
                 * 1. HW */
                sflash_deinit_by_stage( interface->sflash_handle_save, WICED_SFLASH_INIT_STAGE_1);
                goto init_fail;
            }
        }

        /* Apply specific sflash configuration with sflash ID. */
        sflash_setup_capability( interface->sflash_handle_save, sflash_status_id );

        /* Init stage 2, If Sflash support Quad mode, then enable it.*/
        if ( interface->sflash_handle_save->capabilities->speed_advance->high_speed_read_dummy_cycle > 0 )
        {
            result = interface->sflash_handle_save->capabilities->action->set_quad_mode( interface->sflash_handle_save, TURN_ON );
            if (result != SFLASH_MSG_OK)
            {
                WPRINT_SFLASH_ERROR( ( "!!! [%s] - enable Quad mode, fail!!!\n", __FUNCTION__ ) );
                /* Initialize sflash fail, reset :
                 * 1. HW
                 * 2. sflash_handle */
                sflash_deinit_by_stage( interface->sflash_handle_save, WICED_SFLASH_INIT_STAGE_2);
                goto init_fail;
            }
        }
    }

    /* Initialize semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( interface->hw_interface, SEMAPHORE_INIT ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Semaphore initialize fail!!!\n", __FUNCTION__ ) );
        sflash_deinit_by_stage( interface->sflash_handle_save, WICED_SFLASH_INIT_STAGE_CLEAR_ALL);
        goto init_fail;
    }

    memcpy( ( char * )sflash_handle, ( char * )interface->sflash_handle_save, sizeof( sflash_handle_t ) );
    interface->sflash_status_init_count++;

init_fail :
#ifndef SFLASH_SINGLE_THREAD_MODE
    WICED_RESTORE_INTERRUPTS( flags );
#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        INSTRUCTION_BARRIER();

        /* In XIP mode, we should configure sflash control register here for keeping highest speed of XIP-READ. */
        if ( ( result = bcm4390x_sflash_read_data( sflash_handle, 0, NULL, 0, CONFIG_XIP_READ_DATA ) ) != SFLASH_MSG_OK )
        {
                WPRINT_SFLASH_ERROR( ( "!!! [%s] - Configure XIP-READ failed!!!\n", __FUNCTION__ ) );
        }
    }
#endif /* APPLICATION_XIP_ENABLE */
#endif /* SFLASH_SINGLE_THREAD_MODE */

    return result;
}

int bcm4390x_sflash_deinit( sflash_handle_t* sflash_handle )
{
    sflash_interface_t* interface = NULL;
#ifndef SFLASH_SINGLE_THREAD_MODE
    uint32_t flags = 0;
#endif /* SFLASH_SINGLE_THREAD_MODE */

    if ( sflash_handle->capabilities == NULL )
    {
        WPRINT_SFLASH_ERROR(("%s, sflash was not initialized, yet.", __FUNCTION__));
        return SFLASH_MSG_ERROR;
    }

#ifndef SFLASH_SINGLE_THREAD_MODE
    /* Save INTERRUPTS here for make sure ONLY ONE thread can
     * go through wiced_sflash_init in one time. */
    WICED_SAVE_INTERRUPTS( flags );
#endif /* SFLASH_SINGLE_THREAD_MODE */

    interface = platform_interface_get( sflash_handle->hw_interface );

    if ( interface->sflash_status_init_count > 0 )
    {
        interface->sflash_status_init_count--;
        if ( interface->sflash_status_init_count == 0 )
        {
            sflash_deinit_by_stage( sflash_handle, WICED_SFLASH_INIT_STAGE_CLEAR_ALL );
        }
    }

#ifndef SFLASH_SINGLE_THREAD_MODE
    WICED_RESTORE_INTERRUPTS( flags );
#endif /* SFLASH_SINGLE_THREAD_MODE */

    /* To be implement... */
    return SFLASH_MSG_OK;
}

int bcm4390x_sflash_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode )
{
    int result;
#ifdef APPLICATION_XIP_ENABLE
    uint32_t flags = 0;
#endif /* APPLICATION_XIP_ENABLE */

    if ( sflash_handle->capabilities == NULL )
    {
        WPRINT_SFLASH_ERROR(("%s, sflash was not initialized, yet.", __FUNCTION__));
        return SFLASH_MSG_ERROR;
    }

    /* Get semaphore */
    if ( sflash_handle->read_blocking != WICED_FALSE )
    {
        if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_GET ) ) != SFLASH_MSG_OK )
        {
            WPRINT_SFLASH_ERROR( ( "!!! [%s] - Get Semaphore fail!!!\n", __FUNCTION__ ) );
            return result;
        }
    }

#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        WICED_SAVE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */
    if ( ( result = sflash_handle->capabilities->action->read_data( sflash_handle, device_address, rx_data, data_len, mode ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Read data fail!!!\n", __FUNCTION__ ) );
        return result;
    }
#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        INSTRUCTION_BARRIER();
        WICED_RESTORE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */

    /* Set semaphore */
    if ( sflash_handle->read_blocking != WICED_FALSE )
    {
        if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_SET ) ) != SFLASH_MSG_OK )
        {
            WPRINT_SFLASH_ERROR( ( "!!! [%s] - Set Semaphore fail!!!\n", __FUNCTION__ ) );
            return result;
        }
    }

    return result;
}

int bcm4390x_sflash_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode )
{
    int result;
#ifdef APPLICATION_XIP_ENABLE
    uint32_t flags = 0;
#endif /* APPLICATION_XIP_ENABLE */

    if ( sflash_handle->capabilities == NULL )
    {
        WPRINT_SFLASH_ERROR(("%s, sflash was not initialized, yet.", __FUNCTION__));
        return SFLASH_MSG_ERROR;
    }

    /* Get semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_GET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Get Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        platform_watchdog_kick();
        WICED_SAVE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */
    if ( ( result = sflash_handle->capabilities->action->write_data( sflash_handle, device_address, tx_data, data_len, mode ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Write data fail!!!\n", __FUNCTION__ ) );
        return result;
    }
#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        INSTRUCTION_BARRIER();

        /**
         * SFLASH is cacheable with XIP enabled.
         * Invalidate sflash content after write
         * to ensure following direct READ accesses
         *  are not out-of-date
         */
        if( WICED_IS_XIP_CODE_SEGMENT( ( SI_SFLASH + device_address ), data_len ) )
        {
            platform_icache_inv_range( ( void *)( SI_SFLASH + device_address ) , data_len );
        }
        else
        {
            platform_dcache_inv_range( ( void *)( SI_SFLASH + device_address ) , data_len );
        }

        WICED_RESTORE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */

    /* Set semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_SET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Set Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

    return result;
}

int bcm4390x_sflash_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode )
{
    int result;
#ifdef APPLICATION_XIP_ENABLE
    uint32_t data_len = 0;
    uint32_t flags = 0;
#endif /* APPLICATION_XIP_ENABLE */

    if ( sflash_handle->capabilities == NULL )
    {
        WPRINT_SFLASH_ERROR(("%s, sflash was not initialized, yet.", __FUNCTION__));
        return SFLASH_MSG_ERROR;
    }

    /* Get semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_GET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Get Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        platform_watchdog_kick();
        WICED_SAVE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */
    if ( ( result = sflash_handle->capabilities->action->erase_data( sflash_handle, device_address, mode ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Erase data fail!!!\n", __FUNCTION__ ) );
        return result;
    }
#ifdef APPLICATION_XIP_ENABLE
    if ( sflash_handle->hw_interface == INTERFACE_4390x_SFLASH )
    {
        INSTRUCTION_BARRIER();

        if ( mode == ERASE_SECTOR )
        {
            data_len = SFLASH_SECTOR_SIZE;
        }
        else if ( mode == ERASE_BLOCK )
        {
            data_len = SFLASH_BLOCK_SIZE;
        }

        if ( data_len > 0 )
        {
            if( WICED_IS_XIP_CODE_SEGMENT( ( SI_SFLASH + device_address ), data_len ) )
            {
                platform_icache_inv_range( ( void *)( SI_SFLASH + device_address ) , data_len );
            }
            else
            {
                platform_dcache_inv_range( ( void *)( SI_SFLASH + device_address ) , data_len );
            }
        }
        else
        {
            platform_dcache_inv_all();
            platform_icache_inv_all();
        }

        WICED_RESTORE_INTERRUPTS( flags );
    }
#endif /* APPLICATION_XIP_ENABLE */

    /* Set semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( sflash_handle->hw_interface, SEMAPHORE_SET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Set Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

    return result;
}

#ifndef SFLASH_SINGLE_THREAD_MODE
int platform_interface_semaphore( hw_interface_t hw_interface, semaphore_configure_t status )
{
    int results = SFLASH_MSG_ERROR;
    sflash_interface_t* interface = NULL;

    interface = platform_interface_get( hw_interface );

    if ( status == SEMAPHORE_INIT )
    {
        /* initialize semaphore for sflash lock */
        if ( interface->host_semaphore_flag == WICED_FALSE )
        {
            if ( host_rtos_init_semaphore( interface->host_semaphore ) == WWD_SUCCESS )
            {
                WPRINT_SFLASH_DEBUG( ( "semaphore init successfully.\n" ) );
                results = SFLASH_MSG_OK;
                interface->host_semaphore_flag = WICED_TRUE;
                host_rtos_set_semaphore( interface->host_semaphore, WICED_FALSE );
            }
            else
            {
                WPRINT_SFLASH_ERROR( ( "semaphore init failed.\n" ) );
            }
        }
        else
        {
            WPRINT_SFLASH_DEBUG( ( "semaphore had been initialized!\n" ) );
            results = SFLASH_MSG_OK;
        }
    }
    else if ( status == SEMAPHORE_DEINIT )
    {
        /* de-initialize sflash_semaphore for sflash lock */
        if ( interface->host_semaphore_flag == WICED_TRUE )
        {
            if ( host_rtos_deinit_semaphore( interface->host_semaphore ) == WWD_SUCCESS )
            {
                WPRINT_SFLASH_DEBUG( ( "semaphore de-init successfully.\n" ) );
                results = SFLASH_MSG_OK;
                interface->host_semaphore_flag = WICED_FALSE;
            }
            else
            {
                WPRINT_SFLASH_ERROR( ( "semaphore de-init failed.\n" ) );
            }
        }
        else
        {
            WPRINT_SFLASH_DEBUG( ( "semaphore had been de-initialized!\n" ) );
            results = SFLASH_MSG_OK;
        }
    }
    else if ( status == SEMAPHORE_GET )
    {
#if defined(TARGETOS_nuttx)
        /* Because the 'long' type ( tv_sec & tv_nsec of timespec ), 0xFFFFFFFF will overflow. */
        if ( host_rtos_get_semaphore( interface->host_semaphore, ( WICED_NEVER_TIMEOUT / 2 ), WICED_FALSE ) == WWD_SUCCESS )
#else
        if ( host_rtos_get_semaphore( interface->host_semaphore, WICED_NEVER_TIMEOUT, WICED_FALSE ) == WWD_SUCCESS )
#endif
        {
            WPRINT_SFLASH_DEBUG( ( "semaphore get successfully.\n" ) );
            results = SFLASH_MSG_OK;
        }
        else
        {
            WPRINT_SFLASH_ERROR( ( "semaphore get failed.\n" ) );
        }
    }
    else if ( status == SEMAPHORE_SET )
    {
        if ( host_rtos_set_semaphore( interface->host_semaphore, WICED_FALSE ) == WWD_SUCCESS )
        {
            WPRINT_SFLASH_DEBUG( ( "semaphore set successfully.\n" ) );
            results = SFLASH_MSG_OK;
        }
        else
        {
            WPRINT_SFLASH_ERROR( ( "semaphore set failed.\n" ) );
        }
    }

    return results;
}
#endif /* SFLASH_SINGLE_THREAD_MODE */
