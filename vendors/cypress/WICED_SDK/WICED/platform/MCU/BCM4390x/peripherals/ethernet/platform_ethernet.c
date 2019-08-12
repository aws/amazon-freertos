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
 * WICED Ethernet driver.
 */

#include "typedefs.h"
#include "osl.h"
#include "bcmdevs.h"
#include "bcmgmacrxh.h"
#include "bcmendian.h"
#include "bcmenetphy.h"
#include "etc.h"
#include "etcgmac.h"

#include "platform_appscr4.h"
#include "platform_ethernet.h"
#include "platform_map.h"

#include "wiced.h"
#include "internal/wiced_internal_api.h"

#include "wwd_rtos_isr.h"
#include "platform/wwd_platform_interface.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef WICED_ETHERNET_LOOPBACK
#define WICED_ETHERNET_LOOPBACK                      LOOPBACK_MODE_NONE
#endif

#ifndef WICED_ETHERNET_NO_DEDICATED_WORKER_THREAD
#define WICED_ETHERNET_DEDICATED_WORKER_THREAD       (&et_instance.worker_thread)
#define WICED_ETHERNET_DEFAULT_WORKER_THREAD         WICED_ETHERNET_DEDICATED_WORKER_THREAD
#else
#define WICED_ETHERNET_DEFAULT_WORKER_THREAD         WICED_NETWORKING_WORKER_THREAD
#endif /* WICED_ETHERNET_NO_DEDICATED_WORKER_THREAD */

/*
 * Driver is designed to be able to use two worker threads (but also able to work with single one).
 * Why driver made to be able to work with two worker threads:
 *     Ethernet main operations and watchdog operations are guarded by mutex and cannot interrupt each other.
 *     But when no dedicated thread used, watchdog operations can run on lower priority thread
 *     and so thread used for main operations can be used for high priority non-ethernet tasks.
 */
#ifndef WICED_ETHERNET_WORKER_THREAD
#define WICED_ETHERNET_WORKER_THREAD                 WICED_ETHERNET_DEFAULT_WORKER_THREAD
#endif
#ifndef WICED_ETHERNET_WATCHDOG_THREAD
#define WICED_ETHERNET_WATCHDOG_THREAD               WICED_ETHERNET_DEFAULT_WORKER_THREAD
#endif

#define WICED_ETHERNET_CHECK_AND_RET(expr, err_ret)  do {if (!(expr)) {wiced_assert("failed\n", 0); return err_ret;}} while(0)
#define WICED_ETHERNET_CHECK(expr, err_action)         do {if (!(expr)) {wiced_assert("failed\n", 0); err_action;}} while(0)
#define WICED_ETHERNET_CHECKADV(speed_adv, bit)      ethernet_check_and_clear_adv(speed_adv, PLATFORM_ETHERNET_SPEED_ADV(bit))

#define WICED_ETHERNET_INC_ATOMIC(var, cond) \
    do \
    {  \
        uint32_t flags; \
        WICED_SAVE_INTERRUPTS(flags);    \
        if (cond) var++;                 \
        WICED_RESTORE_INTERRUPTS(flags); \
    } \
    while(0)

#define WICED_ETHERNET_DEC_ATOMIC(var, cond) \
    do \
    {  \
        uint32_t flags; \
        WICED_SAVE_INTERRUPTS(flags);    \
        if (cond) var--;                 \
        WICED_RESTORE_INTERRUPTS(flags); \
    } \
    while(0)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct et_info
{
    etc_info_t*                 etc;
    wiced_timed_event_t         wd_event;
    wiced_mutex_t               op_mutex;
    wiced_semaphore_t           barrier_sem;
    platform_ethernet_config_t* cfg;
#ifdef WICED_ETHERNET_DEDICATED_WORKER_THREAD
    wiced_worker_thread_t       worker_thread;
#endif /* WICED_ETHERNET_DEDICATED_WORKER_THREAD */
    wiced_osh_t                 osh;
    uint32_t                    watchdog_events;
    uint8_t                     isr_event_cnt;
    wiced_bool_t                started;
} et_info_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

extern platform_ethernet_config_t platform_ethernet_config;

static et_info_t                  et_instance;
static uint32_t                   et_loopback_mode = WICED_ETHERNET_LOOPBACK;

/******************************************************
 *             Function definitions
 ******************************************************/

/* Functions required by underlying GMAC driver */

static void ethernet_config_adv_speed( et_info_t* et );
static void ethernet_reclaim_packets( et_info_t* et );

void et_intrson( et_info_t* et )
{
    (*et->etc->chops->intrson)( et->etc->ch );
}

/* Note: Changes etc->linkstate = TRUE before calling this function */
void et_link_up( void )
{
    WPRINT_PLATFORM_DEBUG( ( "ethernet: link up\n" ) );

    wiced_network_notify_link_up( WICED_ETHERNET_INTERFACE );

    wiced_ethernet_link_up_handler( );
}

/* Note: Changes etc->linkstate = FALSE before calling this function */
void et_link_down( et_info_t* et )
{
    WPRINT_PLATFORM_DEBUG( ( "ethernet: link down\n" ) );

    wiced_ethernet_link_down_handler( );

    wiced_network_notify_link_down( WICED_ETHERNET_INTERFACE );
}

void et_reset( et_info_t* et )
{
    etc_info_t* etc = et->etc;
    etc_reset( etc );
}

void et_init( et_info_t* et, uint options )
{
    etc_info_t* etc = et->etc;

    etc->loopbk = et_loopback_mode;

    ethernet_config_adv_speed( et );

    et_reset( et );
    etc_init( etc, options );
}

int et_up( et_info_t* et )
{
    etc_info_t* etc = et->etc;

    WPRINT_PLATFORM_DEBUG( ( "ethernet: up\n" ) );

    etc_up( etc );

    etc_watchdog( etc );

    return 0;
}

int et_down( et_info_t* et, int reset )
{
    etc_info_t* etc = et->etc;

    WPRINT_PLATFORM_DEBUG( ( "ethernet: down\n" ) );

    etc->linkstate = FALSE;
    etc_down( etc, reset );

    ethernet_reclaim_packets( et );

    return 0;
}

int et_phy_reset( etc_info_t* etc )
{
    wiced_gpio_init( WICED_GMAC_PHY_RESET, OUTPUT_PUSH_PULL );

    /* Keep RESET low for 2 us */
    wiced_gpio_output_low( WICED_GMAC_PHY_RESET );
    OSL_DELAY( 2 );

    /* Keep RESET high for at least 2 us */
    wiced_gpio_output_high( WICED_GMAC_PHY_RESET );
    OSL_DELAY( 2 );

    return 1;
}

int et_set_addrs( etc_info_t* etc )
{
    platform_ethernet_config_t* cfg;
    wiced_mac_t* mac;

    if ( platform_ethernet_get_config( &cfg ) != PLATFORM_SUCCESS )
    {
        return 0;
    }

    mac = &cfg->mac_addr;

    etc->phyaddr = cfg->phy_addr;

    memcpy( &etc->cur_etheraddr.octet[0], &mac->octet[0], ETHER_ADDR_LEN );

    memcpy( &etc->perm_etheraddr.octet[0], &mac->octet[0], ETHER_ADDR_LEN );

#ifdef WPRINT_ENABLE_NETWORK_INFO
    WPRINT_NETWORK_INFO( ( "Ethernet MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\r\n",
        mac->octet[0], mac->octet[1], mac->octet[2], mac->octet[3], mac->octet[4], mac->octet[5] ) );
#endif

    return 1;
}

/* Driver functions */

static void ethernet_config_phy_interface( et_info_t* et )
{
    uint32_t interface_val;

    switch ( et->cfg->phy_interface )
    {
        case PLATFORM_ETHERNET_PHY_MII:
            interface_val = GCI_CHIPCONTROL_GMAC_INTERFACE_MII;
            break;

        case PLATFORM_ETHERNET_PHY_RMII:
            interface_val = GCI_CHIPCONTROL_GMAC_INTERFACE_RMII;
            break;

        default:
            wiced_assert( "unknown interface" , 0 );
            interface_val = GCI_CHIPCONTROL_GMAC_INTERFACE_MII;
            break;
    }

    platform_gci_chipcontrol( GCI_CHIPCONTROL_GMAC_INTERFACE_REG,
                              GCI_CHIPCONTROL_GMAC_INTERFACE_MASK,
                              interface_val);
}

static wiced_bool_t ethernet_check_and_clear_adv( uint32_t *speed_adv, uint32_t mode )
{
    wiced_bool_t ret = ( *speed_adv & mode ) != 0;

    *speed_adv &= ~mode;

    return ret;
}

static void ethernet_config_adv_speed( et_info_t* et )
{
    uint32_t    adv = et->cfg->speed_adv;
    etc_info_t* etc = et->etc;

    if (adv & PLATFORM_ETHERNET_SPEED_ADV(AUTO) )
    {
        return;
    }

    etc->advertise = 0;
    etc->advertise2 = 0;

    if ( WICED_ETHERNET_CHECKADV( &adv, 10HALF ) )
    {
        etc->advertise |= ADV_10HALF;
    }
    if ( WICED_ETHERNET_CHECKADV( &adv, 10FULL ) )
    {
        etc->advertise |= ADV_10FULL;
    }
    if ( WICED_ETHERNET_CHECKADV( &adv, 100HALF ) )
    {
        etc->advertise |= ADV_100HALF;
    }
    if ( WICED_ETHERNET_CHECKADV( &adv, 100FULL ) )
    {
        etc->advertise |= ADV_100FULL;
    }
    if ( WICED_ETHERNET_CHECKADV( &adv, 1000HALF ) )
    {
        etc->advertise2 |= ADV_1000HALF;
    }
    if ( WICED_ETHERNET_CHECKADV( &adv, 1000FULL ) )
    {
        etc->advertise2 |= ADV_1000FULL;
    }

    wiced_assert( "not all adv mode handled", adv == 0 );
}

static void ethernet_config_force_speed( et_info_t* et )
{
    platform_ethernet_speed_mode_t speed_force = et->cfg->speed_force;
    int force_mode                             = ET_AUTO;

    if ( speed_force == PLATFORM_ETHERNET_SPEED_AUTO )
    {
        return;
    }

    switch ( speed_force )
    {
        case PLATFORM_ETHERNET_SPEED_10HALF:
            force_mode = ET_10HALF;
            break;

        case PLATFORM_ETHERNET_SPEED_10FULL:
            force_mode = ET_10FULL;
            break;

        case PLATFORM_ETHERNET_SPEED_100HALF:
            force_mode = ET_100HALF;
            break;

        case PLATFORM_ETHERNET_SPEED_100FULL:
            force_mode = ET_100FULL;
            break;

        case PLATFORM_ETHERNET_SPEED_1000HALF:
            force_mode = ET_1000HALF;
            break;

        case PLATFORM_ETHERNET_SPEED_1000FULL:
            force_mode = ET_1000FULL;
            break;

        default:
            wiced_assert( "unknown force mode", 0 );
            break;
    }

    etc_ioctl( et->etc, ETCSPEED, &force_mode );
}

static void ethernet_sendup( et_info_t* et, wiced_buffer_t buffer )
{
    bcmgmacrxh_t* rxh = (bcmgmacrxh_t *)PKTDATA( NULL, buffer ); /* packet buffer starts with rxhdr */
    etc_info_t*   etc = et->etc;

    /* check for reported frame errors */
    if ( rxh->flags & htol16( GRXF_CRC | GRXF_OVF | GRXF_OVERSIZE ) )
    {
        etc->rxerror++;
        PKTFREE( etc->osh, buffer, FALSE );
        return;
    }

    /* strip off rxhdr */
    PKTPULL( NULL, buffer, HWRXOFF );

    /* update statistic */
    etc->rxframe++;
    etc->rxbyte += PKTLEN( NULL, buffer );

    /* indicate to upper layer */
    host_network_process_ethernet_data( buffer, WWD_ETHERNET_INTERFACE );
}

static void ethernet_events( et_info_t* et, uint events )
{
    etc_info_t*   etc   = et->etc;
    struct chops* chops = etc->chops;
    void*         ch    = etc->ch;

    if ( events & INTR_TX )
    {
        chops->txreclaim( ch, FALSE );
    }

    if ( events & INTR_RX )
    {
        while ( TRUE )
        {
            wiced_buffer_t buffer = chops->rx( ch );
            if ( buffer == NULL )
            {
                break;
            }

            ethernet_sendup( et, buffer );
        }

        chops->rxfill( ch );
    }

    if ( events & INTR_ERROR )
    {
        if ( chops->errors( ch ) )
        {
            et_down( et, 1 );
            et_up( et );
        }
    }
}

static wiced_result_t ethernet_barrier_event( void* arg )
{
    et_info_t* et = arg;

    wiced_rtos_set_semaphore( &et->barrier_sem );

    return WICED_SUCCESS;
}

static void ethernet_issue_barrier( et_info_t* et, wiced_worker_thread_t* worker_thread)
{
    /*
     * On entry to this function event rescheduling has to be disabled.
     * Function push event to tail of work queue and wait till event (and so all preceding) is handled.
     */

    while ( wiced_rtos_send_asynchronous_event( worker_thread, ethernet_barrier_event, et ) != WICED_SUCCESS )
    {
        wiced_rtos_delay_milliseconds( 10 );
    }
    wiced_rtos_get_semaphore( &et->barrier_sem, WICED_WAIT_FOREVER );
}

static wiced_result_t ethernet_isr_event( void* arg )
{
    et_info_t*     et    = arg;
    etc_info_t*    etc   = et->etc;
    struct chops*  chops = etc->chops;
    void*          ch    = etc->ch;
    wiced_mutex_t* mutex = &et->op_mutex;
    uint events;

    WICED_ETHERNET_DEC_ATOMIC( et->isr_event_cnt, WICED_TRUE );

    wiced_rtos_lock_mutex( mutex );

    if ( et->started )
    {
        events = chops->getintrevents( ch, FALSE ) | et->watchdog_events;
        et->watchdog_events = 0;

        chops->intrsoff( ch ); /* disable and ack interrupts retrieved via getintrevents */

        if ( events & INTR_NEW )
        {
            ethernet_events( et, events );
        }

        chops->intrson( ch );

        platform_irq_enable_irq( GMAC_ExtIRQn );
    }

    wiced_rtos_unlock_mutex( mutex );

    return WICED_SUCCESS;
}

static void ethernet_schedule_isr_event( et_info_t* et )
{
    /* If scheduling failed, it will be tried again in watchdog. */
    WICED_ETHERNET_INC_ATOMIC( et->isr_event_cnt,
        (et->isr_event_cnt == 0) && (wiced_rtos_send_asynchronous_event( WICED_ETHERNET_WORKER_THREAD, ethernet_isr_event, et ) == WICED_SUCCESS ) );
}

static wiced_result_t ethernet_watchdog_event( void* arg )
{
    et_info_t*     et    = arg;
    wiced_mutex_t* mutex = &et->op_mutex;

    wiced_rtos_lock_mutex( mutex );

    if ( et->started )
    {
        etc_watchdog( et->etc );

        et->watchdog_events = INTR_NEW | INTR_RX | INTR_TX;
        ethernet_schedule_isr_event( et );
    }

    wiced_rtos_unlock_mutex( mutex );

    return WICED_SUCCESS;
}

static void ethernet_reclaim_packets( et_info_t* et )
{
    etc_info_t*   etc   = et->etc;
    struct chops* chops = etc->chops;
    void*         ch    = etc->ch;

    (*chops->txreclaim)( ch, TRUE );
    (*chops->rxreclaim)( ch );
}

static wiced_result_t ethernet_create_worker_thread( wiced_bool_t create )
{
    wiced_result_t result = WICED_SUCCESS;

#ifdef WICED_ETHERNET_DEDICATED_WORKER_THREAD
    if ( create )
    {
        result = wiced_rtos_create_worker_thread( WICED_ETHERNET_DEDICATED_WORKER_THREAD, WICED_NETWORK_WORKER_PRIORITY, NETWORKING_WORKER_THREAD_STACK_SIZE, NETWORKING_WORKER_THREAD_QUEUE_SIZE );
    }
    else
    {
        result = wiced_rtos_delete_worker_thread( WICED_ETHERNET_DEDICATED_WORKER_THREAD );
    }
#endif /* WICED_ETHERNET_DEDICATED_WORKER_THREAD */

    return result;
}

/* Driver interface */

wiced_bool_t platform_ethernet_is_inited( void )
{
    et_info_t* et = &et_instance;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return WICED_FALSE;
    }

    return ( et->etc != NULL ) ? WICED_TRUE : WICED_FALSE;
}

wiced_bool_t platform_ethernet_is_ready_to_transceive( void )
{
    et_info_t* et = &et_instance;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return WICED_FALSE;
    }

    return ( platform_ethernet_is_inited( ) && et->started ) ? WICED_TRUE : WICED_FALSE;
}

platform_result_t platform_ethernet_start( void )
{
    et_info_t*     et    = &et_instance;
    wiced_mutex_t* mutex = &et->op_mutex;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_ETHERNET_CHECK_AND_RET( et->etc != NULL, PLATFORM_ERROR );
    WICED_ETHERNET_CHECK_AND_RET( et->started == WICED_FALSE, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( wiced_rtos_register_timed_event( &et->wd_event, WICED_ETHERNET_WATCHDOG_THREAD,
        &ethernet_watchdog_event, et->cfg->wd_period_ms, et ) == WICED_SUCCESS, PLATFORM_ERROR );

    wiced_rtos_lock_mutex( mutex );

    et->etc->linkstate = FALSE; /* force watchdog to report if linkstate is up now */
    et->started = WICED_TRUE;
    platform_irq_enable_irq( GMAC_ExtIRQn );

    wiced_rtos_unlock_mutex( mutex );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_stop( void )
{
    et_info_t*     et    = &et_instance;
    wiced_mutex_t* mutex = &et->op_mutex;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_ETHERNET_CHECK_AND_RET( et->etc != NULL, PLATFORM_ERROR );
    WICED_ETHERNET_CHECK_AND_RET( et->started == WICED_TRUE, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( wiced_rtos_deregister_timed_event( &et->wd_event ) == WICED_SUCCESS, PLATFORM_ERROR );

    wiced_rtos_lock_mutex( mutex );

    et_link_down( et );
    et->started = WICED_FALSE;
    platform_irq_disable_irq( GMAC_ExtIRQn );

    wiced_rtos_unlock_mutex( mutex );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_init( void )
{
    et_info_t* et = &et_instance;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_ETHERNET_CHECK_AND_RET( et->etc == NULL, PLATFORM_ERROR );

    memset( et, 0, sizeof(*et) );

    platform_mcu_powersave_request_clock( PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT );

    WICED_ETHERNET_CHECK_AND_RET( ethernet_create_worker_thread( WICED_TRUE ) == WICED_SUCCESS, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( platform_ethernet_get_config( &et->cfg ) == PLATFORM_SUCCESS, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( wiced_rtos_init_mutex( &et->op_mutex ) == WICED_SUCCESS, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( wiced_rtos_init_semaphore( &et->barrier_sem ) == WICED_SUCCESS, PLATFORM_ERROR );

    ethernet_config_phy_interface( et );

    et->osh.rx_pktget_add_remove  = 0;
    et->osh.tx_pktfree_add_remove = -(int32_t)(HOST_BUFFER_RELEASE_REMOVE_AT_FRONT_FULL_SIZE - WICED_ETHERNET_SIZE); /* Only Ethernet header has to be stripped inside host_buffer_release() */

    et->etc = etc_attach( et,
                          VENDOR_BROADCOM,
                          BCM47XX_GMAC_ID,
                          0 /* unit */,
                          &et->osh /* osh */,
                          NULL /* regsva */ );
    WICED_ETHERNET_CHECK_AND_RET( et->etc != NULL, PLATFORM_ERROR ) ;

    ethernet_config_force_speed( et );

    et_up( et );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_deinit( void )
{
    platform_result_t result = PLATFORM_SUCCESS;
    et_info_t*        et     = &et_instance;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_ETHERNET_CHECK_AND_RET( et->etc != NULL, PLATFORM_ERROR );
    WICED_ETHERNET_CHECK_AND_RET( et->started == WICED_FALSE, PLATFORM_ERROR );

    ethernet_issue_barrier( et, WICED_ETHERNET_WATCHDOG_THREAD );
    if ( WICED_ETHERNET_WORKER_THREAD != WICED_ETHERNET_WATCHDOG_THREAD )
    {
        ethernet_issue_barrier( et, WICED_ETHERNET_WORKER_THREAD );
    }

    et_down( et, TRUE );

    etc_detach( et->etc );
    et->etc = NULL;

    WICED_ETHERNET_CHECK( wiced_rtos_deinit_semaphore( &et->barrier_sem ) == WICED_SUCCESS, result = PLATFORM_ERROR );

    WICED_ETHERNET_CHECK( wiced_rtos_deinit_mutex( &et->op_mutex ) == WICED_SUCCESS, result = PLATFORM_ERROR );

    WICED_ETHERNET_CHECK( ethernet_create_worker_thread( WICED_FALSE ) == WICED_SUCCESS, result = PLATFORM_ERROR );

    platform_mcu_powersave_release_clock( PLATFORM_MCU_POWERSAVE_CLOCK_BACKPLANE_ON_HT );

    return result;
}

WWD_RTOS_DEFINE_ISR( platform_ethernet_isr )
{
    et_info_t* et = &et_instance;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        wiced_assert( "GMAC not supported", 0 );
        return;
    }

    platform_irq_disable_irq( GMAC_ExtIRQn ); /* interrupts will be enabled in event handler */

    ethernet_schedule_isr_event( et );
}
WWD_RTOS_MAP_ISR( platform_ethernet_isr, GMAC_ISR )

platform_result_t platform_ethernet_send_data( wiced_buffer_t buffer )
{
    et_info_t*     et    = &et_instance;
    wiced_mutex_t* mutex = &et->op_mutex;
    etc_info_t*    etc;
    struct chops*  chops;
    void*          ch;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    etc = et->etc;
    WICED_ETHERNET_CHECK_AND_RET( etc != NULL, PLATFORM_ERROR );

    chops = etc->chops;
    WICED_ETHERNET_CHECK_AND_RET( chops != NULL, PLATFORM_ERROR );

    ch = etc->ch;
    WICED_ETHERNET_CHECK_AND_RET( ch != NULL, PLATFORM_ERROR );

    WICED_ETHERNET_CHECK_AND_RET( et->started == WICED_TRUE, PLATFORM_ERROR );

    etc->txframe++;
    etc->txbyte += PKTLEN( NULL, buffer );

    wiced_rtos_lock_mutex( mutex );

    chops->tx( ch, buffer );

    wiced_rtos_unlock_mutex( mutex );

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_get_config( platform_ethernet_config_t** config )
{
    static wiced_bool_t first_time = WICED_TRUE;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    if ( first_time == WICED_TRUE )
    {
        if ( host_platform_get_ethernet_mac_address( &platform_ethernet_config.mac_addr ) != WWD_SUCCESS )
        {
            return PLATFORM_ERROR;
        }
        first_time = WICED_FALSE;
    }

    *config = &platform_ethernet_config;

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_set_loopback_mode( platform_ethernet_loopback_mode_t loopback_mode )
{
    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    switch ( loopback_mode )
    {
        case PLATFORM_ETHERNET_LOOPBACK_DISABLE:
            et_loopback_mode = LOOPBACK_MODE_NONE;
            break;

        case PLATFORM_ETHERNET_LOOPBACK_DMA:
            et_loopback_mode = LOOPBACK_MODE_DMA;
            break;

        case PLATFORM_ETHERNET_LOOPBACK_PHY:
            et_loopback_mode = LOOPBACK_MODE_PHY;
            break;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_ethernet_control_multicast_address( wiced_mac_t* mac, wiced_bool_t add )
{
    et_info_t*        et = &et_instance;
    wiced_mutex_t*    mutex = &et->op_mutex;
    etc_info_t*       etc;
    struct ether_addr ethernet_mac_addr;
    int               result;

    if ( !PLATFORM_FEATURE_ENAB( GMAC ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    etc = et->etc;
    WICED_ETHERNET_CHECK_AND_RET( etc != NULL, PLATFORM_ERROR );

    memcpy( &ethernet_mac_addr.octet[0], &mac->octet[0], ETHER_ADDR_LEN );

    wiced_rtos_lock_mutex( mutex );
    if ( add )
    {
        result = etc_multicast_filter_add( etc, &ethernet_mac_addr );
    }
    else
    {
        result = etc_multicast_filter_remove( etc, &ethernet_mac_addr );
    }
    wiced_rtos_unlock_mutex( mutex );
    if ( result != SUCCESS )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

platform_result_t platform_ethernet_add_multicast_address( wiced_mac_t* mac )
{
    return platform_ethernet_control_multicast_address( mac, WICED_TRUE );
}

platform_result_t platform_ethernet_remove_multicast_address( wiced_mac_t* mac )
{
    return platform_ethernet_control_multicast_address( mac, WICED_FALSE );
}
