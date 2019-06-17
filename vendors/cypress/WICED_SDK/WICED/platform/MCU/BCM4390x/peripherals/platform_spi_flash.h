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
 * API for accessing SPI layer.
 *
 */

#ifndef INCLUDED_SPI_LAYER_API_H
#define INCLUDED_SPI_LAYER_API_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "platform_appscr4.h"
#include "platform_mcu_peripheral.h"
#include "platform_m2m.h"
#include "spi_flash/spi_flash.h"
#include "platform.h"
#include "typedefs.h"
#include "sbchipc.h"
//#include "wiced.h"

/******************************************************
 *                      Macros
 ******************************************************/

 /* For reducing code size of Tiny-bootloader/bootloader, we can
* ignore below function, because :
*  1. Tiny-bootloader/bootloader is an single thread
*  2. Tiny-bootloader has no need to write any data to sflash
*/
#if ( defined( TINY_BOOTLOADER ) || defined( BOOTLOADER ) )
#define SFLASH_SINGLE_THREAD_MODE    (1)
#endif /* defined( TINY_BOOTLOADER ) || defined( BOOTLOADER ) */

#if defined( TINY_BOOTLOADER )
#define SFLASH_WRITE_DATA_DISABLE   (1)
#endif /* TINY_BOOTLOADER */

#define DIRECT_WRITE_BURST_LENGTH    (64)
#define MAX_NUM_BURST                (2)
#define FAST_READ_HIGH_SPEED_DIVIDER (2)

/* Define FAST_READ_DIVIDER and NORMAL_READ_DIVIDER in platform specific
 * platform.h to override the default values defined below
 * FAST_READ_DIVIDER and NORMAL_READ_DIVIDER should be even
 * values <= 32
 */
#if defined(PLATFORM_4390X_OVERCLOCK)
/* WAR, If PLATFORM_4390X_OVERCLOCK enabled, set NORMAL_READ_DIVIDER=12 and FAST_READ_DIVIDER=12, or read/write sflash data fail. */
    #if defined(FAST_READ_DIVIDER) || defined(NORMAL_READ_DIVIDER)
        #error "PLATFORM_4390X_OVERCLOCK will override NORMAL_READ_DIVIDER to 12 and FAST_READ_DIVIDER to 12"
    #else
        #define NORMAL_READ_DIVIDER          (12)
        #define FAST_READ_DIVIDER            (12)
    #endif /* if defined  ( FAST_READ_DIVIDER || NORMAL_READ_DIVIDER ) */
#else
    #ifndef NORMAL_READ_DIVIDER
        #define NORMAL_READ_DIVIDER          (6)
    #endif /* ifndef NORMAL_READ_DIVIDER */
    #ifndef FAST_READ_DIVIDER
        #define FAST_READ_DIVIDER            (6)
    #endif /* ifndef FAST_READ_DIVIDER */
#endif /*if defined(PLATFORM_4390X_OVERCLOCK) */

#ifndef SFLASH_DATA_READ_MODE
#define SFLASH_DATA_READ_MODE AUTO_SPEED_READ_DATA
#else
#pragma message ("!!! Sflash Configuration !!! : Defining SFLASH_DATA_READ_MODE in board file would override the default configuration of sflash, and may result in non-optimal performance")
#endif


#ifndef SFLASH_DATA_WRITE_MODE
#define SFLASH_DATA_WRITE_MODE AUTO_SPEED_WRITE_DATA
#else
#pragma message ("!!! Sflash Configuration !!! : Defining SFLASH_DATA_WRITE_MODE in board file would override the default configuration of sflash, and may result in non-optimal performance")
#endif

/* Use PMU clock with XIP enabled */
#ifdef APPLICATION_XIP_ENABLE
#define MAX_TIMEOUT_FOR_43909_BUSY   ( 3000 * (ILP_CLOCK / 1000 ) )
#else
#define MAX_TIMEOUT_FOR_43909_BUSY   (3000)
#endif /* APPLICATION_XIP_ENABLE */

#define SFLASH_4BYTE_ADDRESS_MASK    (0xFFFFFFFF)

 /* Use PMU clock with XIP enabled since host_rtos_get_time() does not work when interrupts
  * are disabled during sflash operation
  */
#ifdef APPLICATION_XIP_ENABLE
#define WICED_GET_TIME() \
        get_pmu_timer()
#else
#define WICED_GET_TIME() \
        host_rtos_get_time()
#endif /* APPLICATION_XIP_ENABLE */

#ifndef SFLASH_SECTOR_SIZE
/* 4K */
#define SFLASH_SECTOR_SIZE ( 4096 )
#endif


#ifndef SFLASH_BLOCK_SIZE
/* 64K */
#define SFLASH_BLOCK_SIZE ( 65536 )
#endif

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum{
    DMA_BUSY,
    SPI_BUSY
} busy_type_t;

typedef enum{
    IDLE,
    CLEAR
} sflash_controller_status_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct sflash_controller sflash_controller_t;
struct sflash_controller
{
        bcm43909_sflash_ctrl_reg_t control_register;
        unsigned int serial_flash_divider;
};

/******************************************************
 *               Variables Definitions
 ******************************************************/
static const uint32_t address_masks[ SFLASH_ACTIONCODE_MAX_ENUM ] =
{
        [ SFLASH_ACTIONCODE_ONLY ]           = 0x00,
        [ SFLASH_ACTIONCODE_1DATA ]          = 0x00,
        [ SFLASH_ACTIONCODE_3ADDRESS ]       = 0x00ffffff,
        [ SFLASH_ACTIONCODE_3ADDRESS_1DATA ] = 0x00ffffff,
        [ SFLASH_ACTIONCODE_3ADDRESS_4DATA ] = 0x00ffffff,
        [ SFLASH_ACTIONCODE_2DATA ]          = 0x00,
        [ SFLASH_ACTIONCODE_4DATA ]          = 0x00,
        [ SFLASH_ACTIONCODE_4DATA_ONLY ]     = 0x00,
};

static const uint32_t data_masks[ SFLASH_ACTIONCODE_MAX_ENUM ] =
{
        [ SFLASH_ACTIONCODE_ONLY ]           = 0x00000000,
        [ SFLASH_ACTIONCODE_1DATA ]          = 0x000000ff,
        [ SFLASH_ACTIONCODE_3ADDRESS ]       = 0x00000000,
        [ SFLASH_ACTIONCODE_3ADDRESS_1DATA ] = 0x000000ff,
        [ SFLASH_ACTIONCODE_3ADDRESS_4DATA ] = 0xffffffff,
        [ SFLASH_ACTIONCODE_2DATA ]          = 0x0000ffff,
        [ SFLASH_ACTIONCODE_4DATA ]          = 0xffffffff,
        [ SFLASH_ACTIONCODE_4DATA_ONLY ]     = 0xffffffff,
};

static const uint8_t data_bytes[ SFLASH_ACTIONCODE_MAX_ENUM ] =
{
        [ SFLASH_ACTIONCODE_ONLY ]           = 0,
        [ SFLASH_ACTIONCODE_1DATA ]          = 1,
        [ SFLASH_ACTIONCODE_3ADDRESS ]       = 0,
        [ SFLASH_ACTIONCODE_3ADDRESS_1DATA ] = 1,
        [ SFLASH_ACTIONCODE_3ADDRESS_4DATA ] = 4,
        [ SFLASH_ACTIONCODE_2DATA ]          = 2,
        [ SFLASH_ACTIONCODE_4DATA ]          = 4,
        [ SFLASH_ACTIONCODE_4DATA_ONLY ]     = 4,
};

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifndef SFLASH_SINGLE_THREAD_MODE
#ifdef APPLICATION_XIP_ENABLE
int bcm4390x_sflash_controller_configure_xip( sflash_handle_t* sflash_handle, sflash_command_t command );
#endif /* APPLICATION_XIP_ENABLE */
int bcm4390x_sflash_semaphore_init( void );
void bcm4390x_sflash_semaphore_deinit( void );
int bcm4390x_sflash_semaphore_get( void );
int bcm4390x_sflash_semaphore_set( void );
void bcm4390x_sflash_controller_reset( sflash_controller_status_t status );
#define BCM4390X_SFLASH_SEMAPHORE_INIT()    bcm4390x_sflash_semaphore_init( )
#define BCM4390X_SFLASH_SEMAPHORE_DEINIT()  bcm4390x_sflash_semaphore_deinit( )
#define BCM4390X_SFLASH_SEMAPHORE_GET()     bcm4390x_sflash_semaphore_get( )
#define BCM4390X_SFLASH_SEMAPHORE_SET()     bcm4390x_sflash_semaphore_set( )
#define BCM4390x_SFLASH_CONTROLLER_RESET( x )   bcm4390x_sflash_controller_reset( x )
#else
#define BCM4390X_SFLASH_SEMAPHORE_INIT()    WICED_SUCCESS
#define BCM4390X_SFLASH_SEMAPHORE_DEINIT()
#define BCM4390X_SFLASH_SEMAPHORE_GET()
#define BCM4390X_SFLASH_SEMAPHORE_SET()
#define BCM4390x_SFLASH_CONTROLLER_RESET( x )
#endif /* SFLASH_SINGLE_THREAD_MODE */

void spi_layer_init( void );
void spi_layer_deinit( void );
int spi_sflash_send_command( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length );

#ifdef APPLICATION_XIP_ENABLE
uint32_t get_pmu_timer( void );
#endif /* get_pmu_timer */

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_SPI_LAYER_API_H */
