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
 * WICED 4390x SPI Sflash driver
 */
#include "platform_spi_flash.h"
#include <string.h>
#include "crypto_api.h"

#include "wiced_osl.h"
#include <hndsoc.h>
#include "wwd_assert.h"
/******************************************************
 *                      Macros
 ******************************************************/
/* Disable Read/Write data via M2M
 *
 * #define INDIRECT_ACCESS 1
 * */

#define TRANSFORM_4BYTES_DATA( data ) (  \
        ( ( data << 24 ) & ( 0xFF000000 ) ) | \
        ( ( data <<  8 ) & ( 0x00FF0000 ) ) | \
        ( ( data >>  8 ) & ( 0x0000FF00 ) ) | \
        ( ( data >> 24 ) & ( 0x000000FF ) ) )

/* QuadAddrMode(bit 24) of SFlashCtrl register only works after Chipcommon Core Revision 55 */
#define CHIP_SUPPORT_QUAD_ADDR_MODE( ccrev ) ( ccrev >= 55  )
#define M2M_START_WRITE ( 0xFFFFFFFF )

#define ALIGNMENT_4BYTES ( 0x03 )
#define SPI_GENERIC_MAX_DATA_LENGTH ( sizeof( uint32_t ) )

/* BULK DATA READ and WRITE */
#ifndef INDIRECT_ACCESS
#define READ_BULK_DATA( sflash_handle, control_register, data_point, target_data_length )  spi_sflash_access_m2m( sflash_handle, control_register, data_point, target_data_length, COMMAND_RXDATA )
#define WRITE_BULK_DATA( sflash_handle, control_register, data_point, target_data_length )  spi_sflash_access_m2m( sflash_handle, control_register, data_point, target_data_length, COMMAND_TXDATA )
#else
#define READ_BULK_DATA( sflash_handle, control_register, data_point, target_data_length )  spi_sflash_access_indirect_continue( sflash_handle, control_register, data_point, target_data_length, COMMAND_RXDATA )
#define WRITE_BULK_DATA( sflash_handle, control_register, data_point, target_data_length )  spi_sflash_access_indirect_continue( sflash_handle, control_register, data_point, target_data_length, COMMAND_TXDATA )
#endif /* INDIRECT_ACCESS */

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
static uint ccrev;

#ifndef SFLASH_SINGLE_THREAD_MODE
/* Backup sflash controller register of READ */
static sflash_controller_t sflash_controller_backup;
#endif /* SFLASH_SINGLE_THREAD_MODE */

/******************************************************
 *               Function Definitions
 ******************************************************/
static void spi_configure_control_register( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t* control_register,
        sflash_command_t command, bcm43909_sflash_actioncode_t actioncode, command_purpose_t purpose, uint32_t device_address );
static unsigned int spi_get_actioncode_purpose( sflash_command_t command, uint32_t data_length, command_purpose_t *purpose, bcm43909_sflash_actioncode_t *actioncode );
static unsigned int spi_sflash_access_indirect( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose );
#if ( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) )
static unsigned int spi_sflash_access_indirect_continue( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose );
#endif /* ( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) ) */

static int spi_wait_busy_done( busy_type_t busy_type );
#ifndef INDIRECT_ACCESS
static unsigned int spi_sflash_access_m2m( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose );
static int spi_sflash_read_m2m( bcm43909_sflash_ctrl_reg_t* control_register, uint32_t device_address, void* rx_data, uint32_t data_length, wiced_bool_t read_blocking );
#if !( defined( SFLASH_WRITE_DATA_DISABLE ) || defined( APPLICATION_XIP_ENABLE ) )
static int spi_sflash_write_m2m( bcm43909_sflash_ctrl_reg_t* control_register, uint32_t device_address, void* tx_data, uint32_t data_length );
#endif /* !( defined( SFLASH_WRITE_DATA_DISABLE ) || defined( APPLICATION_XIP_ENABLE ) ) */
#endif /* INDIRECT_ACCESS */

#ifndef SFLASH_SINGLE_THREAD_MODE
#ifdef APPLICATION_XIP_ENABLE
int bcm4390x_sflash_controller_configure_xip( sflash_handle_t* sflash_handle, sflash_command_t command )
{
    bcm43909_sflash_ctrl_reg_t *control_register = &sflash_controller_backup.control_register;
    uint32_t data_length = 4;
    bcm43909_sflash_actioncode_t actioncode;
    command_purpose_t purpose;

    actioncode = SFLASH_ACTIONCODE_ONLY;
    if ( spi_get_actioncode_purpose( command, data_length, &purpose, &actioncode ) != SFLASH_MSG_OK )
    {
        return SFLASH_MSG_ERROR;
    }

    /* configure 4390x sflash control register */
    spi_configure_control_register( sflash_handle, control_register, command, actioncode, purpose, 0 );

    control_register->bits.start_busy = 0;
    control_register->bits.use_opcode_reg = 1;
    sflash_controller_backup.serial_flash_divider = PLATFORM_CHIPCOMMON->clock_control.divider.bits.serial_flash_divider;

    return SFLASH_MSG_OK;
}
#endif /* APPLICATION_XIP_ENABLE */

void bcm4390x_sflash_controller_reset( sflash_controller_status_t status )
{
    if ( sflash_controller_backup.control_register.bits.opcode == 0 )
    {
        status = CLEAR;
    }

    switch ( status )
    {
        case IDLE:
            PLATFORM_CHIPCOMMON->clock_control.divider.bits.serial_flash_divider = sflash_controller_backup.serial_flash_divider;
            break;
        case CLEAR:
            /* Clear sflash controller register */
            memset( &sflash_controller_backup, 0 , sizeof( sflash_controller_backup ) );
            PLATFORM_CHIPCOMMON->clock_control.divider.bits.serial_flash_divider = NORMAL_READ_DIVIDER;
        default:
            break;
    }
    PLATFORM_CHIPCOMMON->sflash.control.raw = sflash_controller_backup.control_register.raw;
}
#endif /* SFLASH_SINGLE_THREAD_MODE */

void spi_layer_init()
{
    platform_gci_chipcontrol( PMU_CHIPCONTROL_APP_SFLASH_DRIVE_STRENGTH_MASK_REG, 0, PMU_CHIPCONTROL_APP_SFLASH_DRIVE_STRENGTH_MASK );

    /* Get chipc core rev to decide whether 4-bit write supported or not */
    ccrev = osl_get_corerev( CC_CORE_ID );

#ifndef SFLASH_SINGLE_THREAD_MODE
    /* Clear sflash controller register */
    memset( &sflash_controller_backup, 0 , sizeof( sflash_controller_backup ) );

    /* reset sflash controller */
    BCM4390x_SFLASH_CONTROLLER_RESET( IDLE );
    WPRINT_PLATFORM_DEBUG(("reset sflash controller\n"));
#endif/* SFLASH_SINGLE_THREAD_MODE */
}

void spi_layer_deinit()
{
    /* To be implement....*/

#ifndef SFLASH_SINGLE_THREAD_MODE
    /* reset sflash controller */
    BCM4390x_SFLASH_CONTROLLER_RESET( CLEAR );
    WPRINT_PLATFORM_DEBUG(("reset sflash controller\n"));
#endif/* SFLASH_SINGLE_THREAD_MODE */
}

#ifdef APPLICATION_XIP_ENABLE
uint32_t get_pmu_timer( void )
{
    uint32_t time = PLATFORM_PMU->pmutimer;

    if ( time != PLATFORM_PMU->pmutimer )
    {
        time = PLATFORM_PMU->pmutimer;
    }

    return time;
}
#endif /* APPLICATION_XIP_ENABLE */

int spi_sflash_send_command( sflash_handle_t* sflash_handle, sflash_command_t command, uint32_t device_address, void* data, uint32_t* data_length )
{
    bcm43909_sflash_ctrl_reg_t control_register;
    uint32_t target_data_length = *data_length;
    char* tx_data_pointer;
    char* rx_data_pointer;
    bcm43909_sflash_actioncode_t actioncode;
    command_purpose_t purpose;
    int result = SFLASH_MSG_OK;

    /* [ actioncode ] -
     * Initial actioncode as SFLASH_ACTIONCODE_ONLY (command / no address / no data),
     * It will be updated a proper value by spi_get_actioncode_purpose().  */
    actioncode = SFLASH_ACTIONCODE_ONLY;
    if ( ( result = spi_get_actioncode_purpose( command, target_data_length, &purpose, &actioncode ) ) != SFLASH_MSG_OK )
    {
        goto fail;
    }

    /* configure 4390x sflash control register */
    spi_configure_control_register( sflash_handle, &control_register, command, actioncode, purpose, device_address );

    /* Executing WRITE/READ/COMMAND */
    if ( purpose == COMMAND_TXDATA )
    {
        /* Write Data in Sflash */
        tx_data_pointer = data;
        if ( ( target_data_length > SPI_GENERIC_MAX_DATA_LENGTH ) &&
#if !( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) )
                ( target_data_length >= DIRECT_WRITE_BURST_LENGTH ) && ( ( PLATFORM_CHIPCOMMON->sflash.address & ALIGNMENT_4BYTES ) == 0x00 ) &&
                ( ( (unsigned long)tx_data_pointer & ALIGNMENT_4BYTES ) == 0x00 ) &&
#endif /* !( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) ) */
                ( control_register.bits.action_code == SFLASH_ACTIONCODE_3ADDRESS_4DATA ) )
        {
            *data_length = WRITE_BULK_DATA( sflash_handle, &control_register, tx_data_pointer, target_data_length );
        }
        else
        {
            *data_length = spi_sflash_access_indirect( sflash_handle, &control_register, tx_data_pointer, target_data_length, COMMAND_TXDATA );
        }

        if ( *data_length == 0 )
        {
            result = SFLASH_MSG_ERROR;
        }
    }
    else if ( purpose == COMMAND_RXDATA )
    {
        /* Read Data in Sflash */
        rx_data_pointer = data;

        if ( ( target_data_length > SPI_GENERIC_MAX_DATA_LENGTH ) &&
                ( control_register.bits.action_code == SFLASH_ACTIONCODE_3ADDRESS_4DATA ) )
        {
            *data_length = READ_BULK_DATA( sflash_handle, &control_register, rx_data_pointer, target_data_length );
        }
        else
        {
            *data_length = spi_sflash_access_indirect( sflash_handle, &control_register, rx_data_pointer, target_data_length, COMMAND_RXDATA );
        }

        if ( *data_length == 0 )
        {
            result = SFLASH_MSG_ERROR;
        }
    }
    else if ( purpose == COMMAND_ONLY )
    {
        /* Send command only to Sflash */
        PLATFORM_CHIPCOMMON->sflash.control.raw = control_register.raw;
        if ( spi_wait_busy_done( SPI_BUSY ) != SFLASH_MSG_OK )
        {
            result = SFLASH_MSG_ERROR;
        }
    }
    else
    {
        WPRINT_PLATFORM_ERROR( ( "!!! Unknown purpose(%d) for command(0x%x)\n", purpose, command ) );
        result = SFLASH_MSG_ERROR;
    }

fail:

    /* reset sflash controller */
    if ( sflash_handle->read_blocking != WICED_FALSE )
    {
        BCM4390x_SFLASH_CONTROLLER_RESET( IDLE );
    }

    WPRINT_PLATFORM_DEBUG(("reset sflash controller\n"));

    return result;
}

static void spi_configure_control_register( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t* control_register,
        sflash_command_t command, bcm43909_sflash_actioncode_t actioncode, command_purpose_t purpose, uint32_t device_address )
{
    control_register->raw = 0;
    control_register->bits.action_code = actioncode;
    control_register->bits.opcode = command;
    control_register->bits.start_busy = 1;

    /* Support 4 bytes address mode */
    if ( ( ( actioncode == SFLASH_ACTIONCODE_3ADDRESS_4DATA ) ||
           ( actioncode == SFLASH_ACTIONCODE_3ADDRESS_1DATA ) ||
           ( actioncode == SFLASH_ACTIONCODE_3ADDRESS ) ) &&
         ( SFLASH_SUPPORT_4BYTES_ADDRESS_CHECK ( sflash_handle ) == WICED_TRUE ) )
    {
        control_register->bits.use_four_byte_address_mode = 1;
        PLATFORM_CHIPCOMMON->sflash.address = device_address & SFLASH_4BYTE_ADDRESS_MASK;
    }
    else
    {
        PLATFORM_CHIPCOMMON->sflash.address = device_address & address_masks[ actioncode ];
    }

    /* configure divider of PLATFORM_CHIPCOMMON */
    PLATFORM_CHIPCOMMON->clock_control.divider.bits.serial_flash_divider = NORMAL_READ_DIVIDER;

    if ( purpose == COMMAND_TXDATA )
    {
        /* configure High Speed WRITE */
        if ( !CHIP_SUPPORT_QUAD_ADDR_MODE( ccrev ) )
        {
            control_register->bits.use_quad_address_mode = 0;
            control_register->bits.opcode = ( ( control_register->bits.use_four_byte_address_mode == 1 ) ? SFLASH_WRITE4B : SFLASH_WRITE );
            WPRINT_PLATFORM_DEBUG( ( "!!! Current CORE Revision (0x%x) didn't support Quad mode for Write data !!!\n", ccrev ) );
        }
        else
        {
            if ( ( command == SFLASH_X4IO_WRITE ) || ( command == SFLASH_QUAD_WRITE4B ) )
            {
                control_register->bits.use_quad_address_mode = ( ( sflash_handle->capabilities->speed_advance->high_speed_write_quad_address_mode == WICED_TRUE ) ? 1 : 0 );
            }

    #if defined(PLATFORM_4390X_OVERCLOCK)
            /* WAR, DO NOT USE sflash Quad mode command !!
             * Issue normal WRITE command(SFLASH_WRITE=0x02) when PLATFORM_4390X_OVERCLOCK enabled,or write data fail. */
            control_register->bits.use_quad_address_mode = 0;
            control_register->bits.opcode = ( ( control_register->bits.use_four_byte_address_mode == 1 ) ? SFLASH_WRITE4B : SFLASH_WRITE );
    #endif
        }
    }
    else if ( purpose == COMMAND_RXDATA )
    {
        /* configure FAST / High Speed READ */
        if ( ( command == SFLASH_FAST_READ ) ||( command == SFLASH_QUAD_READ ) || ( command == SFLASH_X4IO_READ ) ||
             ( command == SFLASH_FAST_READ4B ) ||( command == SFLASH_QUAD_READ4B ) || ( command == SFLASH_X4IO_READ4B ) )
        {
            if ( ( command == SFLASH_FAST_READ ) || ( command == SFLASH_FAST_READ4B ) )
            {
                control_register->bits.num_dummy_cycles = sflash_handle->capabilities->speed_advance->fast_read_dummy_cycle;
            }
            else
            {
                control_register->bits.num_dummy_cycles = sflash_handle->capabilities->speed_advance->high_speed_read_dummy_cycle;
                control_register->bits.mode_bit_enable = sflash_handle->capabilities->speed_advance->high_speed_read_mode_bit_support;
            }

#if defined(PLATFORM_4390X_OVERCLOCK)
            /* WAR, DO NOT USE sflash Quad mode command !!
             * Issue normal FAST READ command(SFLASH_FAST_READ=0x0B) when PLATFORM_4390X_OVERCLOCK enabled,or read data fail. */
            control_register->bits.num_dummy_cycles = sflash_handle->capabilities->speed_advance->fast_read_dummy_cycle;
            control_register->bits.mode_bit_enable = 0;
            control_register->bits.opcode = ( ( control_register->bits.use_four_byte_address_mode == 1 ) ? SFLASH_FAST_READ4B : SFLASH_FAST_READ );
#endif

            /* High_speed_mode should be set to 1 only if divider = 2 */
            control_register->bits.high_speed_mode = ( ( FAST_READ_DIVIDER == FAST_READ_HIGH_SPEED_DIVIDER ) ? 1 : 0 );
            PLATFORM_CHIPCOMMON->clock_control.divider.bits.serial_flash_divider = FAST_READ_DIVIDER;
        }
    }
}

static unsigned int spi_sflash_access_indirect( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose )
{
    char* rx_data_pointer = data_point;
#ifndef SFLASH_WRITE_DATA_DISABLE
    char* tx_data_pointer = data_point;
    uint32_t data_write = 0;
#endif /* SFLASH_WRITE_DATA_DISABLE */
    uint8_t sub_data_handle_count = 0;

#ifndef SFLASH_WRITE_DATA_DISABLE
    /* Write data in sflash */
    if ( purpose == COMMAND_TXDATA )
    {
        sub_data_handle_count = data_bytes[ control_register->bits.action_code ];
        data_write = *( ( uint32_t* )tx_data_pointer ) & data_masks[ control_register->bits.action_code ];

        if ( control_register->bits.action_code == SFLASH_ACTIONCODE_3ADDRESS_4DATA )
        {
            data_write = TRANSFORM_4BYTES_DATA( data_write );
        }
        PLATFORM_CHIPCOMMON->sflash.data = data_write;
        WPRINT_PLATFORM_DEBUG( ( "INDIRECT ACCESS for Write Data\n" ) );
    }

    if ( ( purpose == COMMAND_TXDATA ) || ( purpose == COMMAND_RXDATA ) )
#else
    if ( purpose == COMMAND_RXDATA )
#endif /* SFLASH_WRITE_DATA_DISABLE */
    {
        PLATFORM_CHIPCOMMON->sflash.control.raw = control_register->raw;

        if ( spi_wait_busy_done( SPI_BUSY ) != SFLASH_MSG_OK )
        {
            WPRINT_PLATFORM_ERROR( ( "%s, sflash is busy!\n", __FUNCTION__ ) );
            sub_data_handle_count = 0;
            goto fail;
        }
    }

    /* Read data in sflash */
    if ( purpose == COMMAND_RXDATA )
    {
        sub_data_handle_count = data_bytes[ control_register->bits.action_code ] ;
        memcpy( rx_data_pointer, ( char* ) &PLATFORM_CHIPCOMMON->sflash.data, sub_data_handle_count );
        WPRINT_PLATFORM_DEBUG( ( "INDIRECT ACCESS for Read Data \n" ) );
    }

fail :
    return sub_data_handle_count;
}

#if ( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) )
static unsigned int spi_sflash_access_indirect_continue( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose )
{
    uint32_t data_handle_count = 0;
    uint32_t sub_data_handle_count = 0;

    /* assert CS pin for continue read/write */
    control_register->bits.cs_active = 1;
    sub_data_handle_count = spi_sflash_access_indirect( sflash_handle, control_register, data_point, target_data_length, purpose );
    if ( sub_data_handle_count == 0 )
    {
        goto fail;
    }
    PLATFORM_CHIPCOMMON->sflash.address += sub_data_handle_count;
    data_point += sub_data_handle_count;
    data_handle_count += sub_data_handle_count;

    control_register->bits.action_code = SFLASH_ACTIONCODE_4DATA_ONLY;
    sub_data_handle_count = data_bytes[ control_register->bits.action_code ];
    while( ( ( target_data_length - data_handle_count ) >= SPI_GENERIC_MAX_DATA_LENGTH ) )
    {
        control_register->bits.start_busy = 1;

#ifndef SFLASH_WRITE_DATA_DISABLE
        if ( purpose == COMMAND_TXDATA )
        {
            PLATFORM_CHIPCOMMON->sflash.data = *( ( uint32_t* )data_point );
        }
#endif /* SFLASH_WRITE_DATA_DISABLE */

        PLATFORM_CHIPCOMMON->sflash.control.raw = control_register->raw;
        if ( spi_wait_busy_done( SPI_BUSY ) != SFLASH_MSG_OK )
        {
            sub_data_handle_count = 0;
            goto fail;
        }

        if ( purpose == COMMAND_RXDATA )
        {
            memcpy( data_point, ( char* ) &PLATFORM_CHIPCOMMON->sflash.data, sub_data_handle_count );
        }

        PLATFORM_CHIPCOMMON->sflash.address += sub_data_handle_count;
        data_point += sub_data_handle_count;
        data_handle_count += sub_data_handle_count;
    }

fail :
    /* deassert CS pin */
    PLATFORM_CHIPCOMMON->sflash.control.raw = 0;

    return data_handle_count;
}
#endif /* ( defined(INDIRECT_ACCESS) || defined(APPLICATION_XIP_ENABLE) ) */

static unsigned int spi_get_actioncode_purpose( sflash_command_t command, uint32_t data_length, command_purpose_t *purpose, bcm43909_sflash_actioncode_t *actioncode )
{
    int i = 0;

    while ( opcode_purpose_list[i].command != SFLASH_COMMAND_MAX_ENUM )
    {
        if ( opcode_purpose_list[i].command == command )
        {
            if ( opcode_purpose_list[i].with_address == WICED_TRUE )
            {
                if ( data_length >= 4 )
                {
                    *actioncode = SFLASH_ACTIONCODE_3ADDRESS_4DATA;
                }
                else if ( data_length >= 1 )
                {
                    *actioncode = SFLASH_ACTIONCODE_3ADDRESS_1DATA;
                }
                else if ( data_length == 0 )
                {
                    *actioncode = SFLASH_ACTIONCODE_3ADDRESS;
                }
            }
            else
            {
                if ( data_length == 4 )
                {
                    *actioncode = SFLASH_ACTIONCODE_4DATA;
                }
                else if ( data_length == 2 )
                {
                    *actioncode = SFLASH_ACTIONCODE_2DATA;
                }
                else if ( data_length == 1 )
                {
                    *actioncode = SFLASH_ACTIONCODE_1DATA;
                }
                else if ( data_length == 0 )
                {
                    *actioncode = SFLASH_ACTIONCODE_ONLY;
                }
            }

            *purpose = opcode_purpose_list[i].purpose;
            return SFLASH_MSG_OK;
        }
        i++;
    }
    return SFLASH_MSG_ERROR;
}

static int spi_wait_busy_done( busy_type_t busy_type )
{
    uint32_t start_time, end_time;
    uint32_t busy;

    /* Wait for 43909 controller until ready */
    start_time = WICED_GET_TIME();
    do
    {
      if ( busy_type == SPI_BUSY )
      {
          busy = PLATFORM_CHIPCOMMON->sflash.control.bits.start_busy;
      } else
      {
          busy = PLATFORM_CHIPCOMMON->sflash.control.bits.backplane_write_dma_busy;
      }

      end_time = WICED_GET_TIME();

      if ( end_time - start_time > MAX_TIMEOUT_FOR_43909_BUSY )
      {
          WPRINT_PLATFORM_ERROR( ( "43909 spi controller always busy over %ld ms ! ( default : %d ms)\n", ( end_time - start_time ), MAX_TIMEOUT_FOR_43909_BUSY ) );
          return SFLASH_MSG_ERROR;
      }
    } while ( busy == 1 );

    return SFLASH_MSG_OK;
}

#ifndef INDIRECT_ACCESS
static unsigned int spi_sflash_access_m2m( sflash_handle_t* sflash_handle, bcm43909_sflash_ctrl_reg_t *control_register, char* data_point, uint32_t target_data_length, command_purpose_t purpose )
{
    int sub_data_handle_count = 0;

    control_register->bits.use_opcode_reg = 1;

#ifndef SFLASH_WRITE_DATA_DISABLE
    /* Write data in sflash */
    if ( purpose == COMMAND_TXDATA )
    {
        WPRINT_PLATFORM_DEBUG( ( "M2M ACCESS for Write Data\n" ) );

#ifndef APPLICATION_XIP_ENABLE
        if ( ( sub_data_handle_count = spi_sflash_write_m2m( control_register, PLATFORM_CHIPCOMMON->sflash.address, data_point, target_data_length ) ) == SFLASH_MSG_ERROR )
#else
        if ( ( sub_data_handle_count = spi_sflash_access_indirect_continue( sflash_handle, control_register, data_point, target_data_length, purpose ) ) == SFLASH_MSG_ERROR )
#endif /* APPLICATION_XIP_ENABLE */
        {
            WPRINT_PLATFORM_ERROR( ( "%s, Write Data failed!\n", __FUNCTION__ ) );
            sub_data_handle_count = 0;
        }
    }
#endif /* SFLASH_WRITE_DATA_DISABLE */

    /* Read data in sflash */
    if ( purpose == COMMAND_RXDATA )
    {
        WPRINT_PLATFORM_DEBUG( ( "M2M ACCESS for Read Data \n" ) );
        if ( ( sub_data_handle_count = spi_sflash_read_m2m( control_register, PLATFORM_CHIPCOMMON->sflash.address, data_point, target_data_length, sflash_handle->read_blocking ) ) == SFLASH_MSG_ERROR )
        {
            WPRINT_PLATFORM_ERROR( ( "%s, Read Data failed!\n", __FUNCTION__ ) );
            sub_data_handle_count = 0;
        }
    }

    return sub_data_handle_count;
}

static int spi_sflash_read_m2m( bcm43909_sflash_ctrl_reg_t* control_register, uint32_t device_address, void* rx_data, uint32_t data_length, wiced_bool_t read_blocking )
{
    uint32_t data_handle_count;
    void* direct_address = ( void* ) ( SI_SFLASH + device_address );

    /* Configure sflash controller for M2M */
    PLATFORM_CHIPCOMMON->sflash.control.raw = control_register->raw;
    if ( spi_wait_busy_done( SPI_BUSY ) != SFLASH_MSG_OK )
    {
        return SFLASH_MSG_ERROR;
    }

    m2m_unprotected_dma_memcpy( rx_data, direct_address, data_length , read_blocking );
    WPRINT_PLATFORM_DEBUG( ( "M2M for Read Data\n" ) );

    /* Since we can read all data via M2M in one time,
     * so data_length is the amount of handled data. */
    data_handle_count = data_length;

    return data_handle_count;
}

#if !( defined( SFLASH_WRITE_DATA_DISABLE ) || defined( APPLICATION_XIP_ENABLE ) )
static int spi_sflash_write_m2m( bcm43909_sflash_ctrl_reg_t* control_register, uint32_t device_address, void* tx_data, uint32_t data_length )
{
    uint8_t num_burst;
    uint32_t i;
    uint32_t data_handle_count = 0;
    uint32_t* src_data_ptr;
    uint32_t* dst_data_ptr;

    num_burst = MIN( MAX_NUM_BURST, data_length / DIRECT_WRITE_BURST_LENGTH );
    data_handle_count = DIRECT_WRITE_BURST_LENGTH * ( 1 << ( num_burst - 1 ) );
    control_register->bits.num_burst = num_burst;
    control_register->bits.start_busy = 0;
    WPRINT_PLATFORM_DEBUG( ( "M2M for Write Data\n" ) );

    PLATFORM_CHIPCOMMON->sflash.control.raw = control_register->raw;

    src_data_ptr = ( uint32_t* ) tx_data;
    dst_data_ptr = ( uint32_t* ) ( SI_SFLASH + device_address );

    for ( i = 0; i < data_handle_count / sizeof(uint32_t); i++ )
    {
        *dst_data_ptr = *src_data_ptr++;
    }

    /* Additional write starts to issue the transaction to the SFLASH */
    *dst_data_ptr = M2M_START_WRITE;

    /* sflash state machine is still running. Do not change any bits in this register while this bit is high */
    if ( spi_wait_busy_done( DMA_BUSY ) != SFLASH_MSG_OK )
    {
        return SFLASH_MSG_ERROR;
    }

    return data_handle_count;
}
#endif /* !( defined( SFLASH_WRITE_DATA_DISABLE ) || defined( APPLICATION_XIP_ENABLE ) ) */
#endif /* INDIRECT_ACCESS */
