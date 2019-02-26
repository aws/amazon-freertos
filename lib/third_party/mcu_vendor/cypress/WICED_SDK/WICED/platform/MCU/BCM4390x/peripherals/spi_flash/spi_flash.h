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

/**
 * @file
 *
 * API for accessing SPI flash chips.
 *
 */

#ifndef INCLUDED_SPI_FLASH_API_H
#define INCLUDED_SPI_FLASH_API_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "wiced.h"
#include <stdint.h>
#include "wwd_constants.h"
#include <platform_toolchain.h>
#include <platform_secure_flash.h>

/******************************************************
 *                      Macros
 ******************************************************/
/* Enable sflash debug Macro */
#define SFLASH_DEBUG_SUPPORT (1)

 /* For reducing code size of Tiny-bootloader/bootloader, we can
  * ignore below function, because :
  *  1. Tiny-bootloader/bootloader is an single thread
  *  2. Tiny-bootloader has no need to write any data to sflash
  */
#if ( defined( TINY_BOOTLOADER ) || defined( BOOTLOADER ) )
#undef SFLASH_DEBUG_SUPPORT
#define SFLASH_DEBUG_SUPPORT 0
#define SFLASH_SINGLE_THREAD_MODE    (1)
#endif /* defined( TINY_BOOTLOADER ) || defined( BOOTLOADER ) */
#if defined( TINY_BOOTLOADER )
#undef SFLASH_WRITE_DATA_DISABLE
#define SFLASH_WRITE_DATA_DISABLE   (1)
#endif /* TINY_BOOTLOADER */

#if ( SFLASH_DEBUG_SUPPORT == 1 )
#ifdef WPRINT_ENABLE_SFLASH_INFO
#define WPRINT_SFLASH_INFO( args ) WPRINT_MACRO( args )
#else
#define WPRINT_SFLASH_INFO( args )
#endif /* WPRINT_ENABLE_SFLASH_INFO */
#ifdef WPRINT_ENABLE_SFLASH_ERROR
#define WPRINT_SFLASH_ERROR( args ) WPRINT_MACRO( args )
#else
#define WPRINT_SFLASH_ERROR( args )
#endif /* WPRINT_ENABLE_SFLASH_ERROR */
#ifdef WPRINT_ENABLE_SFLASH_DEBUG
#define WPRINT_SFLASH_DEBUG( args ) WPRINT_MACRO( args )
#else
#define WPRINT_SFLASH_DEBUG( args )
#endif /* WPRINT_ENABLE_SFLASH_DEBUG */
#else
#define WPRINT_SFLASH_INFO( args )
#define WPRINT_SFLASH_ERROR( args )
#define WPRINT_SFLASH_DEBUG( args )
#endif /* ( SFLASH_DEBUG_SUPPORT == 1 ) */

#ifndef SFLASH_SINGLE_THREAD_MODE
#define PLATFORM_INTERFACE_SEMAPHORE( a, b )  platform_interface_semaphore( a, b )
#ifdef TARGETOS_nuttx
#define BCM4390X_SFLASH_SEMAPHORE_INIT()    platform_interface_semaphore( INTERFACE_4390x_SFLASH, SEMAPHORE_INIT )
#define BCM4390X_SFLASH_SEMAPHORE_DEINIT()  platform_interface_semaphore( INTERFACE_4390x_SFLASH, SEMAPHORE_DEINIT )
#define BCM4390X_SFLASH_SEMAPHORE_GET()     platform_interface_semaphore( INTERFACE_4390x_SFLASH, SEMAPHORE_GET )
#define BCM4390X_SFLASH_SEMAPHORE_SET()     platform_interface_semaphore( INTERFACE_4390x_SFLASH, SEMAPHORE_SET )
#endif /* TARGETOS_nuttx */
#else
#define PLATFORM_INTERFACE_SEMAPHORE( a, b )  SFLASH_MSG_OK
#ifdef TARGETOS_nuttx
#define BCM4390X_SFLASH_SEMAPHORE_INIT()    SFLASH_MSG_OK
#define BCM4390X_SFLASH_SEMAPHORE_DEINIT()
#define BCM4390X_SFLASH_SEMAPHORE_GET()
#define BCM4390X_SFLASH_SEMAPHORE_SET()
#endif /* TARGETOS_nuttx */
#endif /* SFLASH_SINGLE_THREAD_MODE */

#define SFLASHID_MASK                       ( 0x00FFFFFF )
#define SFLASHID( manufactory, type, size ) ( ( uint32_t )( ( ( size ) << 16 ) | ( ( type ) << 8 ) | ( manufactory ) ) & SFLASHID_MASK )
/* All sflash ID */
#define SFLASH_ID_MX25L8006E           SFLASHID( 0xC2, 0x20, 0x14 )
#define SFLASH_ID_MX25L1606E           SFLASHID( 0xC2, 0x20, 0x15 )
#define SFLASH_ID_MX25L6433F           SFLASHID( 0xC2, 0x20, 0x17 )
#define SFLASH_ID_MX25L12835F          SFLASHID( 0xC2, 0x20, 0x18 )
#define SFLASH_ID_MX25L25635F          SFLASHID( 0xC2, 0x20, 0x19 )
#define SFLASH_ID_MX25U1635F           SFLASHID( 0xC2, 0x25, 0x35 )
#define SFLASH_ID_MX66U51235F          SFLASHID( 0xC2, 0x25, 0x3A )
#define SFLASH_ID_SST25VF080B          SFLASHID( 0xBF, 0x25, 0x8E )
#define SFLASH_ID_EN25QH16             SFLASHID( 0x1C, 0x30, 0x15 )
#define SFLASH_ID_ISSI25CQ032          SFLASHID( 0x7F, 0x9D, 0x46 )
#define SFLASH_ID_N25Q512A             SFLASHID( 0x20, 0xBB, 0x20 )
#define SFLASH_ID_ISSI25LP064          SFLASHID( 0x9D, 0x60, 0x17 )
#define SFLASH_ID_N25Q064A             SFLASHID( 0x20, 0xBA, 0x17 )
#define SFLASH_ID_W25Q64FV             SFLASHID( 0xEF, 0x40, 0x17 )
#define SFLASH_ID_S25FL064L            SFLASHID( 0x01, 0x60, 0x17 )
#define SFLASH_ID_S25FL128L            SFLASHID( 0x01, 0x60, 0x18 )
#define SFLASH_ID_S25FL256L            SFLASHID( 0x01, 0x60, 0x19 )
#define SFLASH_ID_DEFAULT              SFLASHID( 0x00, 0x00, 0x00 )
#define FRAM_ID_FM25W256               SFLASHID( 0x01, 0x19, 0x02 )

#define FLASH_TIMEOUT_MS             ( 150000 )
#ifdef APPLICATION_XIP_ENABLE  /* Maximum timeout for sflash busy */
#define MAX_TIMEOUT_FOR_FLASH_BUSY   ( FLASH_TIMEOUT_MS * (ILP_CLOCK / 1000 ) )
#else
#define MAX_TIMEOUT_FOR_FLASH_BUSY   ( FLASH_TIMEOUT_MS )
#endif /* APPLICATION_XIP_ENABLE */

#define KBYTE   ( 1024 )
#define MBYTE   ( 1024*KBYTE )
#define SFLASH_SIZE_16MBYTE ( 16 * MBYTE )
#define SFLASH_SUPPORT_4BYTES_ADDRESS_CHECK( sflash ) \
        ( sflash->capabilities->total_size > SFLASH_SIZE_16MBYTE ) \
        ? WICED_TRUE : WICED_FALSE

#define SFLASH_MSG_ERROR ( -1 )
#define SFLASH_MSG_OK    ( 0 )

#define BLOCK_PROTECT_MODE_TO_SIZE( mode, value ) \
    do { \
        int loop_i; \
        for( loop_i = 0; loop_i < BLOCK_PRTOECT_MAX_ENUM; loop_i++ ) \
        { \
            if ( loop_i == BLOCK_PRTOECT_TOP_0KB ) \
            { \
                value = 0; \
            } \
            else if ( loop_i == BLOCK_PRTOECT_TOP_64KB ) \
            { \
                value = 64; \
            } \
            else \
            { \
                value *= 2; \
            } \
            if ( loop_i == mode ) \
            { \
                break; \
            } \
        } \
    } while (0)

#define BLOCK_PROTECT_SIZE_TO_MODE( value, mode ) \
    do { \
        int loop_i; \
        int count=0; \
        for( loop_i = 0; loop_i < BLOCK_PRTOECT_MAX_ENUM; loop_i++ ) \
        { \
            if ( loop_i == BLOCK_PRTOECT_TOP_0KB ) \
            { \
                count = 0; \
            } \
            else if ( loop_i == BLOCK_PRTOECT_TOP_64KB ) \
            { \
                count = 64; \
            } \
            else \
            { \
                count *= 2; \
            } \
            if ( count == value ) \
            { \
                mode = loop_i; \
                break; \
            } \
        } \
    } while (0)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
/* SFLASH command */
typedef enum
{
    SFLASH_WRITE_STATUS_REGISTER = 0x01,
    SFLASH_READ_STATUS_REGISTER = 0x05,
    SFLASH_READ_STATUS_REGISTER2 = 0x35,
    SFLASH_WRITE_DISABLE = 0x04,
    SFLASH_WRITE_ENABLE = 0x06,
    SFLASH_READ = 0x03,
    SFLASH_READ4B = 0x13,
    SFLASH_FAST_READ = 0x0B,
    SFLASH_FAST_READ4B = 0x0C,
    SFLASH_QUAD_READ = 0x6B,
    SFLASH_QUAD_READ4B = 0x6C,
    SFLASH_X4IO_READ = 0xEB,
    SFLASH_X4IO_READ4B = 0xEC,
    SFLASH_WRITE = 0x02,
    SFLASH_WRITE4B = 0x12,
    SFLASH_QUAD_WRITE = 0x32,
    SFLASH_QUAD_WRITE4B = 0x34,
    SFLASH_X4IO_WRITE = 0x38,
    SFLASH_4K_ERASE = 0x20,
    SFLASH_4K_ERASE4B = 0x21,
    SFLASH_32K_ERASE = 0x52,
    SFLASH_64K_ERASE = 0xD8,
    SFLASH_64K_ERASE4B = 0xDC,
    SFLASH_WRITE_ENABLE_VOLATILE_REGISTER = 0x50, /* Cypress/Spansion only for enabling write volatile configuration register (SR1V, CR1V, CR2V, CR3V)*/
    SFLASH_WRITE_ENH_VOLATILE_REGISTER = 0x61, /* Micron only for write enhance volatile register. */
    SFLASH_READ_ENH_VOLATILE_REGISTER = 0x65,  /* Micron only for read enhance volatile register. */
    SFLASH_CHIP_ERASE = 0xC7,
    SFLASH_RESET_ENABLE = 0x66,
    SFLASH_RESET = 0x99,
    SFLASH_READID_JEDEC_ID = 0x9F,
    SFLASH_EN4B = 0xB7,
    SFLASH_EX4B = 0xE9,
    SFLASH_COMMAND_MAX_ENUM
} sflash_command_t;

typedef enum{
    AUTO_SPEED_READ_DATA,
    NORMAL_READ_DATA,
    FAST_READ_DATA,
    HIGH_SPEED_READ_DATA,
#ifdef APPLICATION_XIP_ENABLE
    CONFIG_XIP_READ_DATA,
#endif /* APPLICATION_XIP_ENABLE */
} read_data_mode_t;

typedef enum{
    AUTO_SPEED_WRITE_DATA,
    NORMAL_WRITE_DATA,
    HIGH_SPEED_WRITE_DATA,
} write_data_mode_t;

typedef enum{
    ERASE_SECTOR,
    ERASE_BLOCK,
    ERASE_ALL,
} erase_data_mode_t;

typedef enum{
    READ_PROCESS,
    WRITE_PROCESS,
} read_write_mode_t;

typedef enum{
    BLOCK_PRTOECT_TOP_0KB,
    BLOCK_PRTOECT_TOP_64KB,
    BLOCK_PRTOECT_TOP_128KB,
    BLOCK_PRTOECT_TOP_256KB,
    BLOCK_PRTOECT_TOP_512KB,
    BLOCK_PRTOECT_TOP_1024KB,
    BLOCK_PRTOECT_TOP_2048KB,
    BLOCK_PRTOECT_TOP_4096KB,
    BLOCK_PRTOECT_TOP_8192KB,
    BLOCK_PRTOECT_TOP_16384KB,
    BLOCK_PRTOECT_TOP_32768KB,
    BLOCK_PRTOECT_MAX_ENUM
} block_protect_action_t;

typedef enum{
    QUERY_ACTION = 1,
    SETUP_ACTION = 2,
} getset_action_t;

typedef enum{
    TURN_ON = 1,
    TURN_OFF = 0,
} onoff_action_t;

typedef enum
{
    SFLASH_WRITE_NOT_ALLOWED = 0,
    SFLASH_WRITE_ALLOWED = 1,
} sflash_write_allowed_t;

typedef enum
{
    INTERFACE_4390x_SFLASH = 0,
    INTERFACE_4390x_SPI_0 = 1,
    INTERFACE_4390x_SPI_1 = 2,
}hw_interface_t;

typedef enum{
    COMMAND_TXDATA,
    COMMAND_RXDATA,
    COMMAND_ONLY,
    COMMAND_UNKNOWN_PURPOSE
} command_purpose_t;

typedef enum
{
    SEMAPHORE_INIT = 0,
    SEMAPHORE_DEINIT = 1,
    SEMAPHORE_GET = 2,
    SEMAPHORE_SET = 3
}semaphore_configure_t;
/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct opcode_purpose opcode_purpose_t;

typedef struct sflash_action sflash_action_t;
typedef struct sflash_speed_advance sflash_speed_advance_t;
typedef struct sflash_capabilities sflash_capabilities_t;
typedef struct sflash_handle sflash_handle_t;
typedef struct sflash_interface sflash_interface_t;

typedef int ( *sflash_write_t ) ( const sflash_handle_t* handle, unsigned long device_address, const void* data_addr, unsigned int size );
typedef int ( *sflash_read_t ) ( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size );

struct opcode_purpose
{
        sflash_command_t command;
        command_purpose_t purpose;
        wiced_bool_t with_address;
};

struct sflash_action
{
    int ( *set_quad_mode )( sflash_handle_t* sflash_handle, onoff_action_t mode );
    wiced_bool_t ( *is_quad_mode )( sflash_handle_t* sflash_handle );
    int ( *set_block_protect )( sflash_handle_t* sflash_handle, block_protect_action_t mode );
    int ( *query_block_protect )( sflash_handle_t* sflash_handle, uint32_t* protect_level );
    int ( *set_write_enable )( sflash_handle_t* sflash_handle, onoff_action_t mode );
    wiced_bool_t ( *is_write_enable )( sflash_handle_t* sflash_handle );
    wiced_bool_t ( *is_busy )( sflash_handle_t* sflash_handle );
    int ( *reset )( sflash_handle_t* sflash_handle );
    int ( *status_register )( sflash_handle_t* sflash_handle, void* mask, void* data, getset_action_t mode );
    int ( *read_data )( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode );
    int ( *write_data )( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode );
    int ( *erase_data )( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode );
};

struct sflash_speed_advance
{
    /* ### fast_read_dummy_cycle ###
     * If fast_read_dummy_cycle as 0, it means not support fast read. */
    uint8_t fast_read_dummy_cycle;

    /* ### high_speed_read_dummy_cycle ###
     * If high_speed_read_dummy_cycle as 0, it means not support Quad mode (high speed mode). */
    uint8_t high_speed_read_dummy_cycle;

    /* ### high_speed_read_x4io_support ###
     * There're two kind of mode for high speed read, command are :
     *     - SFLASH_QUAD_READ ( 0x6B ), we call 1-1-4 (1-bit cmd, 1-bit address, 4-bit data)
     *     - SFLASH_X4IO_READ ( 0xEB ), we call 1-4-4 (1-bit cmd, 4-bit address, 4-bit data)
     * False : support SFLASH_QUAD_READ
     * True  : support SFLASH_X4IO_READ */
    wiced_bool_t high_speed_read_x4io_support;

    /* ### high_speed_read_mode_bit_support ###
     * Some Sflash can reduce instruction overhead by setting the "continuous read mode bits" after address bits.
     * The detail, please refer Sflash document.
     * False : not support (default)
     * True  : support */
    wiced_bool_t high_speed_read_mode_bit_support;

    /* ### high_speed_write_support ###
     * Quad mode for write :
     * False : disable
     * True  : enable */
    wiced_bool_t high_speed_write_support;

    /* There're two kind of mode for high speed write, command are :
    *     - SFLASH_QUAD_WRITE ( 0x32 ), we call 1-1-4 (1-bit cmd, 1-bit address, 4-bit data)
    *     - SFLASH_X4IO_WRITE ( 0x38 ), we call 1-4-4 (1-bit cmd, 4-bit address, 4-bit data)
    * False : support SFLASH_QUAD_WRITE
    * True  : support SFLASH_X4IO_WRITE */
    wiced_bool_t high_speed_write_x4io_support;

    /* ### high_speed_write_quad_address_mode ###
     * Some Sflash always use 1-bit address (1-1-4) for write data, no matter command is SFLASH_QUAD_WRITE or SFLASH_X4IO_WRITE.
     * False : if command SFLASH_X4IO_WRITE, use 1-bit address for write data
     * True  : if command SFLASH_X4IO_WRITE, use 4-bit address for write data (default) */
    wiced_bool_t high_speed_write_quad_address_mode;
};

struct sflash_capabilities
{
    uint32_t total_size;
    uint32_t max_page_size;
    const sflash_speed_advance_t* speed_advance;
    const sflash_action_t* action;
};

typedef struct sflash_handle
{
    hw_interface_t  hw_interface;
    uint32_t device_id;
    void* platform_peripheral;
    const sflash_capabilities_t* capabilities;
    sflash_write_allowed_t write_allowed;
    wiced_bool_t read_blocking;
    securesflash_handle_t* securesflash_handle;
} sflash_handle_t;

struct sflash_interface
{
        const hw_interface_t hw_interface;
        uint32_t sflash_status_init_count;
        sflash_handle_t* sflash_handle_save;
        host_semaphore_type_t* host_semaphore;
        wiced_bool_t host_semaphore_flag;
};
/******************************************************
 *               Variables Definitions
 ******************************************************/
extern const opcode_purpose_t opcode_purpose_list[];

/******************************************************
 *               Function Definitions
 ******************************************************/
/**
 * sflash_init
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param peripheral_id     An ID value which is passed to the underlying sflash_platform_init function
 * @param write_allowed_in  Determines whether writing will be allowed to this sflash handle*
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int bcm4390x_sflash_init( sflash_handle_t* sflash_handle, void* peripheral_id, sflash_write_allowed_t write_allowed_in );

/**
 * sflash_deinit
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int bcm4390x_sflash_deinit( sflash_handle_t* sflash_handle );

/**
 * sflash_read_data
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param device_address    read data from specific sflash address.
 * @param rx_data           refer point for receive data
 * @param data_len          data length
 * @param mode              using read_data_mode_t for read data
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int bcm4390x_sflash_read_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* rx_data, uint32_t data_len, read_data_mode_t mode );

/**
 * sflash_write_data
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param device_address    write data to specific sflash address.
 * @param tx_data           refer point for send data
 * @param data_len          data length
 * @param mode              using write_data_mode_t for write data
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int bcm4390x_sflash_write_data( sflash_handle_t* sflash_handle, uint32_t device_address, void* tx_data, uint32_t data_len, write_data_mode_t mode );

/**
 * sflash_erase_data
 * @param sflash_handle     Handle structure that was initialized with @ref sflash_init
 * @param device_address    erase data from specific sflash address.
 * @param mode              using erase_data_mode_t for erase data
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int bcm4390x_sflash_erase_data( sflash_handle_t* sflash_handle, uint32_t device_address, erase_data_mode_t mode );

#ifndef SFLASH_SINGLE_THREAD_MODE
/**
 * platform_interface_semaphore
 * @param hw_interface      interface (ex : INTERFACE_4390x_SFLASH / INTERFACE_4390x_SPI_0 / INTERFACE_4390x_SPI_1 )
 * @param status            semaphore status (ex : SEMAPHORE_INIT / SEMAPHORE_DEINIT / SEMAPHORE_GET / SEMAPHORE_SET )
 * @return                  return success(SFLASH_MSG_OK) or fail(SFLASH_MSG_ERROR)
 */
int platform_interface_semaphore( hw_interface_t hw_interface, semaphore_configure_t status );
#endif /* SFLASH_SINGLE_THREAD_MODE */

/******************************************************
 * Compatible function/API for previous sflash driver *
 ******************************************************/
/**
 *  Initializes a SPI Flash chip
 *
 *  Internally this initializes the associated SPI port, then
 *  reads the chip ID from the SPI flash to determine what type it is.
 *
 * @param[in] handle            Handle structure that will be used for this sflash instance - allocated by caller.
 * @param[in] peripheral_id     An ID value which is passed to the underlying sflash_platform_init function
 * @param[in] write_allowed_in  Determines whether writing will be allowed to this sflash handle
 *
 * @return @ref wiced_result_t
 */
int init_sflash( const sflash_handle_t* handle, void* peripheral_id, sflash_write_allowed_t write_allowed_in );

/**
 *  Reads data from a SPI Flash chip
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where reading will start
 * @param[out] data_addr         Destination buffer in memory that will receive the data
 * @param[in]  size              Number of bytes to read from the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_read( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size );

/**
 *  Erase one sector of a SPI Flash chip
 *
 * @param[in]  handle    Handle structure that was initialized with @ref init_sflash
 * @param[out] size      Variable which will receive the capacity size in bytes of the sflash chip
 *
 * @return @ref wiced_result_t
 */
int sflash_get_size( const sflash_handle_t* handle, unsigned long* size );

/**
 *  Erase the contents of a SPI Flash chip
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 *
 * @return @ref wiced_result_t
 */
int sflash_chip_erase( const sflash_handle_t* handle );

/**
 *  Write data to a SPI Flash chip
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where writing will start
 * @param[in]  data_addr         Pointer to the buffer in memory that contains the data being written
 * @param[in]  size              Number of bytes to write to the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_write( const sflash_handle_t* handle, unsigned long device_address, const void* data_addr, unsigned int size );

/**
 *  Erase one sector of a SPI Flash chip
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the sflash chip of the first byte of the sector to erase
 *
 * @return @ref wiced_result_t
 */
int sflash_sector_erase( const sflash_handle_t* handle, unsigned long device_address );

/**
 *  De-initializes a SPI Flash chip
 *
 * @param[in] handle            Handle structure that will be used for this sflash instance - allocated by caller.
 *
 * @return @ref wiced_result_t
 */
int deinit_sflash( const sflash_handle_t* handle );

/**
 *  Erase one block of a SPI Flash chip
 *
 * @param[in]  handle    Handle structure that was initialized with @ref init_sflash
 * @param[out] size      Variable which will receive the capacity size in bytes of the sflash chip
 *
 * @return @ref wiced_result_t
 */
int sflash_block_erase( const sflash_handle_t* handle, unsigned long device_address );

/**
 *  Secure-Write data to a SPI Flash chip (Encrypt and Authenticate data sector by sector before being written)
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where writing will start
 * @param[in]  data_addr         Pointer to the buffer in memory that contains the data being written
 * @param[in]  size              Number of bytes to write to the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_write_secure( const sflash_handle_t* handle, unsigned long device_address,  const void* data_addr, unsigned int size );

/**
 *  Reads data from a SPI Flash chip, Decrypts/Authenticates the data read, returns error if
 *  Authentication fails
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where reading will start
 * @param[out] data_addr         Destination buffer in memory that will receive the data
 * @param[in]  size              Number of bytes to read from the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_read_secure( const sflash_handle_t* handle, unsigned long device_address,  void* data_addr, unsigned int size );

/**
 *  Reads data from DCT, Decrypts/Authenticates the data read, returns error if
 *  Authentication fails
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where reading will start
 * @param[out] data_addr         Destination buffer in memory that will receive the data
 * @param[in]  size              Number of bytes to read from the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_dct_read_secure( const sflash_handle_t* const sflash_handle, unsigned long device_address, /*@out@*/ /*@dependent@*/ void* data_addr, unsigned int size );

/**
 *  Writes data to DCT, Encrypts and Sign the data
 *
 * @param[in]  handle            Handle structure that was initialized with @ref init_sflash
 * @param[in]  device_address    The location on the SPI flash where reading will start
 * @param[out] data_addr         Destination buffer in memory that will receive the data
 * @param[in]  size              Number of bytes to read from the chip
 *
 * @return @ref wiced_result_t
 */
int sflash_dct_write_secure ( const sflash_handle_t* const sflash_handle, unsigned long device_address,  /*@observer@*/ const void* const data_addr, unsigned int size );


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_SPI_FLASH_API_H */
