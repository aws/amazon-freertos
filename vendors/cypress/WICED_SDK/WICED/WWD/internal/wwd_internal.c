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

#include <string.h>
#include <stdlib.h>
#include "wwd_management.h"
#include "RTOS/wwd_rtos_interface.h"
#include "internal/wwd_internal.h"
#include "internal/bus_protocols/wwd_bus_protocol_interface.h"
#include "internal/wwd_sdpcm.h"
#include "chip_constants.h"
#include "wwd_bcmendian.h"
#include "wwd_debug.h"
#include "wiced_low_power.h"
#include "network/wwd_buffer_interface.h"
#include "network/wwd_network_constants.h"
#include "wwd_assert.h"
#include "wifi_utils.h"
/******************************************************
 *                      Macros
 ******************************************************/

#ifndef SPINWAIT_POLL_PERIOD
#define SPINWAIT_POLL_PERIOD    10
#endif

#define SPINWAIT(exp, us) { \
    uint countdown = (us) + (SPINWAIT_POLL_PERIOD - 1); \
    while ((exp) && (countdown >= SPINWAIT_POLL_PERIOD)) { \
        host_rtos_delay_milliseconds(SPINWAIT_POLL_PERIOD); \
        countdown -= SPINWAIT_POLL_PERIOD; \
    } \
}
/******************************************************
 *             Constants
 ******************************************************/

#define AI_IOCTRL_OFFSET         (0x408)
#define SICF_FGC                 (0x0002)
#define SICF_CLOCK_EN            (0x0001)
#define AI_RESETCTRL_OFFSET      (0x800)
#define AI_RESETSTATUS_OFFSET    (0x804)
#define AIRC_RESET               (1)
#define WRAPPER_REGISTER_OFFSET  (0x100000)

#define WLAN_SHARED_VERSION_MASK    0x00ff
#define WLAN_SHARED_VERSION         0x0001
/******************************************************
 *             Structures
 ******************************************************/

/******************************************************
 *             Variables
 ******************************************************/

static const uint32_t core_base_address[] =
{
#ifdef WLAN_ARMCM3_BASE_ADDRESS
    (uint32_t) ( WLAN_ARMCM3_BASE_ADDRESS + WRAPPER_REGISTER_OFFSET  ),
#endif /* ifdef WLAN_ARMCM3_BASE_ADDRESS */
#ifdef WLAN_ARMCR4_BASE_ADDRESS
    (uint32_t) ( WLAN_ARMCR4_BASE_ADDRESS + WRAPPER_REGISTER_OFFSET  ),
#endif /* ifdef WLAN_ARMCR4_BASE_ADDRESS */
#ifdef SOCSRAM_BASE_ADDRESS
    (uint32_t) ( SOCSRAM_BASE_ADDRESS + WRAPPER_REGISTER_OFFSET ),
#endif
    (uint32_t) ( SDIO_BASE_ADDRESS )
};

wwd_wlan_status_t wwd_wlan_status =
{
    .state             = WLAN_OFF,
    .country_code      = WICED_COUNTRY_AUSTRALIA,
    .aggregate_code    = WICED_COUNTRY_AGGREGATE_XV_0,
    .keep_wlan_awake = 0,
};

static wifi_console_t *c;
wifi_console_t console;
static wlan_shared_t sh;
static uint console_addr = 0;
static uint WICED_DEEP_SLEEP_SAVED_VAR(con_lastpos) = 0;

/******************************************************
 *             Static Function Declarations
 ******************************************************/

static uint32_t wwd_get_core_address( device_core_t core_id );

/******************************************************
 *             Function definitions
 ******************************************************/

/*
 * Returns the base address of the core identified by the provided coreId
 */
uint32_t wwd_get_core_address( device_core_t core_id )
{
    return core_base_address[(int) core_id];
}

/*
 * Returns WWD_SUCCESS is the core identified by the provided coreId is up, otherwise WWD result code
 */
wwd_result_t wwd_device_core_is_up( device_core_t core_id )
{
    uint8_t regdata;
    uint32_t base;
    wwd_result_t result;

    base = wwd_get_core_address( core_id );

    /* Read the IO control register */
    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &regdata );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    /* Verify that the clock is enabled and something else is not on */
    if ( ( regdata & ( SICF_FGC | SICF_CLOCK_EN ) ) != (uint8_t) SICF_CLOCK_EN )
    {
        return WWD_CORE_CLOCK_NOT_ENABLED;
    }

    /* Read the reset control and verify it is not in reset */
    result = wwd_bus_read_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, &regdata );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }
    if ( ( regdata & AIRC_RESET ) != 0 )
    {
        return WWD_CORE_IN_RESET;
    }

    return WWD_SUCCESS;
}
/*
 * Resets the core identified by the provided coreId
 */
wwd_result_t wwd_reset_core( device_core_t core_id, uint32_t bits, uint32_t resetbits )
{
    uint32_t base = wwd_get_core_address( core_id );
    wwd_result_t result;
    uint8_t regdata;
    uint32_t loop_counter =10;

    /* ensure there are no pending backplane operations */
    SPINWAIT((((result = wwd_bus_read_backplane_value( base + AI_RESETSTATUS_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) && regdata != 0 ) , 300);

    /* put core into reset state */
     result = wwd_bus_write_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, (uint32_t) AIRC_RESET );
    (void) host_rtos_delay_milliseconds( (uint32_t) 10 ); /* Ignore return - nothing can be done if it fails */

    /* ensure there are no pending backplane operations */
    SPINWAIT((((result = wwd_bus_read_backplane_value( base + AI_RESETSTATUS_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) && regdata != 0 ) , 300);


    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1,( bits | resetbits |SICF_FGC |SICF_CLOCK_EN ));

    /* ensure there are no pending backplane operations */
      SPINWAIT((((result = wwd_bus_read_backplane_value( base + AI_RESETSTATUS_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) && regdata != 0 ) , 300);

      while (((result = wwd_bus_read_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) &&  regdata !=0  && --loop_counter != 0)
      {
              /* ensure there are no pending backplane operations */
          SPINWAIT((((result = wwd_bus_read_backplane_value( base + AI_RESETSTATUS_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) && regdata != 0 ) , 300);
              /* take core out of reset */
          result = wwd_bus_write_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, (uint32_t) 0 );

          /* ensure there are no pending backplane operations */
          SPINWAIT((((result = wwd_bus_read_backplane_value( base + AI_RESETSTATUS_OFFSET, (uint8_t) 1, &regdata )) == WWD_SUCCESS) && regdata != 0 ) , 300);
      }

      result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1,( bits | SICF_CLOCK_EN ));

      (void) host_rtos_delay_milliseconds( (uint32_t) 1); /* Ignore return - nothing can be done if it fails */


    return result;
}
/*
 * Disables the core identified by the provided coreId
 */
wwd_result_t wwd_disable_device_core( device_core_t core_id, wlan_core_flag_t core_flag )
{
    uint32_t base = wwd_get_core_address( core_id );
    wwd_result_t result;
    uint8_t junk;
    uint8_t regdata;

    /* Read the reset control */
    result = wwd_bus_read_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    /* Read the reset control and check if it is already in reset */
    result = wwd_bus_read_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, &regdata );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }
    if ( ( regdata & AIRC_RESET ) != 0 )
    {
        /* Core already in reset */
        return WWD_SUCCESS;
    }

    /* Write 0 to the IO control and read it back */
    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, ( core_flag == WLAN_CORE_FLAG_CPU_HALT )? SICF_CPUHALT : 0 );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    result = wwd_bus_write_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, (uint32_t) AIRC_RESET );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    return result;
}

/*
 * Resets the core identified by the provided coreId
 */
wwd_result_t wwd_reset_device_core( device_core_t core_id, wlan_core_flag_t core_flag )
{
    uint32_t base = wwd_get_core_address( core_id );
    wwd_result_t result;
    uint8_t junk;

    result = wwd_disable_device_core( core_id, core_flag );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, (uint32_t) ( SICF_FGC | SICF_CLOCK_EN | (( core_flag == WLAN_CORE_FLAG_CPU_HALT )? SICF_CPUHALT : 0 ) ) );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_write_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, 0 );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, (uint32_t) ( SICF_CLOCK_EN | ( ( core_flag == WLAN_CORE_FLAG_CPU_HALT )? SICF_CPUHALT : 0 ) ) );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    return result;
}

/*
 * Release ARM core to run instructions
 */
wwd_result_t wwd_wlan_armcore_run( device_core_t core_id, wlan_core_flag_t core_flag )
{
    uint32_t base = wwd_get_core_address( core_id );
    wwd_result_t result;
    uint8_t junk;

    /* Only work for WLAN arm core! */
    if (WLAN_ARM_CORE != core_id)
    {
        return WWD_UNSUPPORTED;
    }

    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, (uint32_t) ( SICF_FGC | SICF_CLOCK_EN | (( core_flag == WLAN_CORE_FLAG_CPU_HALT )? SICF_CPUHALT : 0 ) ) );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_write_backplane_value( base + AI_RESETCTRL_OFFSET, (uint8_t) 1, 0 );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    result = wwd_bus_write_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, (uint32_t) ( SICF_CLOCK_EN | ( ( core_flag == WLAN_CORE_FLAG_CPU_HALT )? SICF_CPUHALT : 0 ) ) );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    result = wwd_bus_read_backplane_value( base + AI_IOCTRL_OFFSET, (uint8_t) 1, &junk );
    if ( result != WWD_SUCCESS )
    {
        return result;
    }

    (void) host_rtos_delay_milliseconds( (uint32_t) 1 );

    return result;
}

wwd_result_t wwd_wifi_read_wlan_log_unsafe( uint32_t wlan_shared_address, char* buffer, uint32_t buffer_size )
{
    char ch;
    uint32_t n;
    uint32_t index;
    uint32_t address;
    wwd_result_t result = WWD_WLAN_ERROR;

    c = &console;

    if ( console_addr == 0 )
    {
        uint shared_addr;

        address = wlan_shared_address;
        result = wwd_bus_read_backplane_value( address, 4, (uint8_t*) &shared_addr );
        if ( result != WWD_SUCCESS )
        {
            goto done;
        }

        result = wwd_bus_transfer_backplane_bytes( BUS_READ, shared_addr, sizeof(wlan_shared_t), (uint8_t *) &sh);
        if ( result != WWD_SUCCESS )
        {
            goto done;
        }

        sh.flags            = ltoh32( sh.flags );
        sh.trap_addr        = ltoh32( sh.trap_addr );
        sh.assert_exp_addr  = ltoh32( sh.assert_exp_addr );
        sh.assert_file_addr = ltoh32( sh.assert_file_addr );
        sh.assert_line      = ltoh32( sh.assert_line );
        sh.console_addr     = ltoh32( sh.console_addr );
        sh.msgtrace_addr    = ltoh32( sh.msgtrace_addr );

        if ( ( sh.flags & WLAN_SHARED_VERSION_MASK ) != WLAN_SHARED_VERSION )
        {
            WPRINT_APP_INFO( ( "Readconsole: WLAN shared version is not valid\n\r") );
            result = WWD_WLAN_ERROR;
            goto done;
        }
        console_addr = sh.console_addr;
    }

    /* Read console log struct */
    address = console_addr + offsetof( hnd_cons_t, log );
    result = wwd_bus_transfer_backplane_bytes( BUS_READ, address, sizeof(c->log), (uint8_t*)&c->log );
    if ( result != WWD_SUCCESS )
    {
        goto done;
    }

    /* Allocate console buffer (one time only) */
    if ( c->buf == NULL )
    {
        c->bufsize = ltoh32( c->log.buf_size );
        c->buf = malloc( c->bufsize );
        if ( c->buf == NULL )
        {
            result = WWD_WLAN_ERROR;
            goto done;
        }
    }

    /* Retrieve last read position */
    c->last = con_lastpos;

    index = ltoh32( c->log.idx );

    /* Protect against corrupt value */
    if ( index > c->bufsize )
    {
        result = WWD_WLAN_ERROR;
        goto done;
    }

    /* Skip reading the console buffer if the index pointer has not moved */
    if ( index == c->last )
    {
        result = WWD_SUCCESS;
        goto done;
    }

    /* Read the console buffer */
    /* xxx this could optimize and read only the portion of the buffer needed, but
     * it would also have to handle wrap-around.
     */
    address = ltoh32( c->log.buf );
    result = wwd_bus_transfer_backplane_bytes( BUS_READ, address, c->bufsize, (uint8_t*) c->buf );
    if ( result != WWD_SUCCESS )
    {
        goto done;
    }

    while ( c->last != index )
    {
        for ( n = 0; n < buffer_size - 2; n++ )
        {
            if ( c->last == index )
            {
                /* This would output a partial line.  Instead, back up
                 * the buffer pointer and output this line next time around.
                 */
                if ( c->last >= n )
                {
                    c->last -= n;
                }
                else
                {
                    c->last = c->bufsize - n;
                }
                /* Save last read position */
                con_lastpos = c->last;

                result = WWD_SUCCESS;
                goto done;
            }
            ch = c->buf[ c->last ];
            c->last = ( c->last + 1 ) % c->bufsize;
            if ( ch == '\n' )
            {
                break;
            }
            buffer[ n ] = ch;
        }

        if ( n > 0 )
        {
            if ( buffer[ n - 1 ] == '\r' )
                n--;
            buffer[ n ] = 0;
            WPRINT_APP_INFO( ("CONSOLE: %s\n", buffer) );
        }
    }
    /* Save last read position */
    con_lastpos = c->last;
    result = WWD_SUCCESS;

done:
    return result;
}

void wwd_wifi_peek( uint32_t address, uint8_t register_length, /*@out@*/ uint8_t* value )
{
    uint8_t status;

    WWD_WLAN_KEEP_AWAKE();

    status = wwd_bus_read_backplane_value( address, register_length, value );

    if (status != WWD_SUCCESS)
    {
        WPRINT_WWD_ERROR(("%s: Error reading interrupt status\n", __FUNCTION__));
    }

    WWD_WLAN_LET_SLEEP();
}

void wwd_wifi_poke( uint32_t address, uint8_t register_length, uint32_t value )
{
    uint8_t status;

    WWD_WLAN_KEEP_AWAKE();

    status = wwd_bus_write_backplane_value( address, register_length, value );

    if (status != WWD_SUCCESS)
    {
        WPRINT_WWD_ERROR(("%s: Error clearing the interrupt status\n", __FUNCTION__));
    }

    WWD_WLAN_LET_SLEEP();
}

#ifdef WWD_IOCTL_LOG_ENABLE
wwd_ioctl_log_t wwd_ioctl_log[WWD_IOCTL_LOG_SIZE];
int wwd_ioctl_log_index;

void wwd_ioctl_log_add(uint32_t cmd, wiced_buffer_t buffer)
{
    uint8_t* data = ( host_buffer_get_current_piece_data_pointer( buffer ) + IOCTL_OFFSET );
    size_t data_size =  host_buffer_get_current_piece_size( buffer );

    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].ioct_log = cmd;
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].is_this_event = 0;
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].data_size = MIN(WWD_MAX_DATA_SIZE, data_size);
    memset(wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].data, 0, WWD_MAX_DATA_SIZE);
    memcpy(wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].data, data,
        wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].data_size);

    wwd_ioctl_log_index++;
}

void wwd_ioctl_log_add_event(uint32_t cmd, uint16_t flag, uint32_t reason)
{
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].is_this_event = 1;
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].ioct_log = cmd;
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].flag = flag;
    wwd_ioctl_log[wwd_ioctl_log_index % WWD_IOCTL_LOG_SIZE].reason = reason;

    wwd_ioctl_log_index++;

}
#endif /* WWD_IOCTL_LOG_ENABLE */


wwd_result_t wwd_ioctl_print(void)
{
#ifdef WWD_IOCTL_LOG_ENABLE

    int i;
    uint8_t* data = NULL;
    size_t iovar_string_size = 0;

    for(i = 0; i < WWD_IOCTL_LOG_SIZE; i++)
    {
        char iovar[WWD_IOVAR_STRING_SIZE]= {0};
        data = wwd_ioctl_log[i].data;

        if ( wwd_ioctl_log[i].ioct_log == WLC_SET_VAR || wwd_ioctl_log[i].ioct_log == WLC_GET_VAR )
        {
            while (!*data)
            {
                wwd_ioctl_log[i].data_size--;
                data++;
            }
            strcpy(iovar, (char*) data);
            iovar_string_size = strlen((const char *)data);
            data += iovar_string_size;
            wwd_ioctl_log[i].data_size -= iovar_string_size;
        }

        if (wwd_ioctl_log[i].is_this_event == 1)
        {
            wifi_utils_event_info_to_string(wwd_ioctl_log[i].ioct_log, wwd_ioctl_log[i].flag,
                wwd_ioctl_log[i].reason, iovar, sizeof(iovar)-1);
            WPRINT_MACRO(("\n<- E:%lu\t\t\tS:%d\t\t\t\tR:%lu\n%s\n", wwd_ioctl_log[i].ioct_log,
            wwd_ioctl_log[i].flag, wwd_ioctl_log[i].reason, iovar));
        }
        else if(wwd_ioctl_log[i].ioct_log == WLC_SET_VAR)
        {
            WPRINT_MACRO(("\n-> %s\n", iovar));
            wifi_utils_hexdump(data, wwd_ioctl_log[i].data_size);
        }
        else if (wwd_ioctl_log[i].ioct_log == WLC_GET_VAR)
        {
            WPRINT_MACRO(("\n<- %s\n", iovar));
            wifi_utils_hexdump(data, wwd_ioctl_log[i].data_size);
        }
        else if (wwd_ioctl_log[i].ioct_log != 0)
        {
            wifi_utils_ioctl_to_string(wwd_ioctl_log[i].ioct_log, iovar, sizeof(iovar)-1);
            WPRINT_MACRO(("\n%s:%lu\n", iovar, wwd_ioctl_log[i].ioct_log));
            wifi_utils_hexdump(data, wwd_ioctl_log[i].data_size);
        }
    }

    memset(wwd_ioctl_log, 0, sizeof(wwd_ioctl_log));
    wwd_ioctl_log_index = 0;
    return WWD_SUCCESS;

#else /* WWD_IOCTL_LOG_ENABLE */
    return WWD_DOES_NOT_EXIST;

#endif /* WWD_IOCTL_LOG_ENABLE */
}

uint32_t wwd_wifi_get_btc_params( uint32_t address, wwd_interface_t interface )
{
    uint32_t value = 0;
    wiced_buffer_t buffer;
    wiced_buffer_t response;
    uint32_t*      data;

    WWD_WLAN_KEEP_AWAKE();

    data = wwd_sdpcm_get_iovar_buffer( &buffer, 4, "btc_params" );
    if (data != NULL)
    {
        *data = address;
    }
    else
    {
        wiced_assert("wwd_sdpcm_get_iovar_buffer returned NULL value", (data == NULL));
        return value;
    }

    if ( wwd_sdpcm_send_iovar( SDPCM_GET, buffer, &response, interface ) == WWD_SUCCESS )
    {
        memcpy(&value, host_buffer_get_current_piece_data_pointer( response ), 4 );
        host_buffer_release( response, WWD_NETWORK_RX );
    }

    WWD_WLAN_LET_SLEEP();

    return value;
}

inline uint16_t bcmswap16( uint16_t val )
{
    return BCMSWAP16(val);
}

inline uint32_t bcmswap32( uint32_t val )
{
    return BCMSWAP32(val);
}

inline uint32_t bcmswap32by16( uint32_t val )
{
    return BCMSWAP32BY16(val);
}

/* Reverse pairs of bytes in a buffer (not for high-performance use) */
/* buf  - start of buffer of shorts to swap */
/* len  - byte length of buffer */
inline void bcmswap16_buf( uint16_t* buf, uint32_t len )
{
    len = len / 2;

    while ( ( len-- ) != 0 )
    {
        *buf = bcmswap16( *buf );
        buf++;
    }
}

/*
 * Store 16-bit value to unaligned little-endian byte array.
 */
inline void htol16_ua_store( uint16_t val, uint8_t* bytes )
{
    bytes[0] = (uint8_t) ( val & 0xff );
    bytes[1] = (uint8_t) ( val >> 8 );
}

/*
 * Store 32-bit value to unaligned little-endian byte array.
 */
inline void htol32_ua_store( uint32_t val, uint8_t* bytes )
{
    bytes[0] = (uint8_t) ( val & 0xff );
    bytes[1] = (uint8_t) ( ( val >> 8 ) & 0xff );
    bytes[2] = (uint8_t) ( ( val >> 16 ) & 0xff );
    bytes[3] = (uint8_t) ( val >> 24 );
}

/*
 * Store 16-bit value to unaligned network-(big-)endian byte array.
 */
inline void hton16_ua_store( uint16_t val, uint8_t* bytes )
{
    bytes[0] = (uint8_t) ( val >> 8 );
    bytes[1] = (uint8_t) ( val & 0xff );
}

/*
 * Store 32-bit value to unaligned network-(big-)endian byte array.
 */
inline void hton32_ua_store( uint32_t val, uint8_t* bytes )
{
    bytes[0] = (uint8_t) ( val >> 24 );
    bytes[1] = (uint8_t) ( ( val >> 16 ) & 0xff );
    bytes[2] = (uint8_t) ( ( val >> 8 ) & 0xff );
    bytes[3] = (uint8_t) ( val & 0xff );
}

/*
 * Load 16-bit value from unaligned little-endian byte array.
 */
inline uint16_t ltoh16_ua( const void* bytes )
{
    return (uint16_t) _LTOH16_UA((const uint8_t*)bytes);
}

/*
 * Load 32-bit value from unaligned little-endian byte array.
 */
inline uint32_t ltoh32_ua( const void* bytes )
{
    return (uint32_t) _LTOH32_UA((const uint8_t*)bytes);
}

/*
 * Load 16-bit value from unaligned big-(network-)endian byte array.
 */
inline uint16_t ntoh16_ua( const void* bytes )
{
    return (uint16_t) _NTOH16_UA((const uint8_t*)bytes);
}

/*
 * Load 32-bit value from unaligned big-(network-)endian byte array.
 */
inline uint32_t ntoh32_ua( const void* bytes )
{
    return (uint32_t) _NTOH32_UA((const uint8_t*)bytes);
}
