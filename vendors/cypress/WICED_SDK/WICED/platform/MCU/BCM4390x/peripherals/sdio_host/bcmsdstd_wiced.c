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
 * 'Standard' SDIO HOST CONTROLLER driver - RTOS portion
 * $Id: bcmsdstd_wiced.c 455390 2014-02-13 22:14:56Z frankyl $
 */

#include <typedefs.h>
#include <bcmutils.h>
#include <sdio.h>    /* SDIO Device and Protocol Specs */
#include <sdioh.h> /* SDIO Host Controller Spec header file */
#include <bcmsdbus.h>    /* bcmsdh to/from specific controller APIs */
#include <sdiovar.h>    /* to get msglevel bit values */
#include <bcmsdstd.h>
#include <bcmdevs.h>

#include "wwd_rtos.h"
#include "platform_sdio.h"

/******************************************************
 *                      Macros
 ******************************************************/

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

struct sdos_info
{
    sdioh_info_t *sd;
    host_semaphore_type_t lock;
};

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/


/* Register for interrupts */
int
sdstd_register_irq(sdioh_info_t *sd, uint irq)
{
    wiced_result_t wiced_result;

    UNUSED_PARAMETER(irq);

    /* Enable host interrupt */
    wiced_result = platform_sdio_host_enable_interrupt();

    return ((wiced_result == WICED_SUCCESS) ? SUCCESS : ERROR);
}

/* Free irq */
void
sdstd_free_irq(uint irq, sdioh_info_t *sd)
{
    UNUSED_PARAMETER(irq);

    /* Disable host interrupt */
    platform_sdio_host_disable_interrupt();
}

/* Map Host controller registers */

uint32 *
sdstd_reg_map(osl_t *osh, ulong addr, int size)
{
    UNUSED_PARAMETER(osh);
    UNUSED_PARAMETER(size);
    return (uint32 *)(addr);
}

void
sdstd_reg_unmap(osl_t *osh, ulong addr, int size)
{
    UNUSED_PARAMETER(osh);
    UNUSED_PARAMETER(addr);
    UNUSED_PARAMETER(size);
}

int
sdstd_osinit(sdioh_info_t *sd)
{
    struct sdos_info *sdos;
    wwd_result_t     result;

    sdos = (struct sdos_info*)MALLOC(sd->osh, sizeof(struct sdos_info));
    sd->sdos_info = (void*)sdos;
    if (sdos == NULL)
    {
        return BCME_NOMEM;
    }

    sdos->sd = sd;

    result = host_rtos_init_semaphore( &sdos->lock );
    if ( result != WWD_SUCCESS )
    {
        return BCME_ERROR;
    }

    host_rtos_set_semaphore ( &sdos->lock, WICED_FALSE );

    return BCME_OK;
}

void
sdstd_osfree(sdioh_info_t *sd)
{
    struct sdos_info *sdos;
    wwd_result_t     result;
    ASSERT(sd && sd->sdos_info);

    sdos = (struct sdos_info *)sd->sdos_info;

    result = host_rtos_deinit_semaphore( &sdos->lock );
    ASSERT( result == WWD_SUCCESS );
    MFREE(sd->osh, sdos, sizeof(struct sdos_info));
}

/* Interrupt enable/disable */
SDIOH_API_RC
sdioh_interrupt_set(sdioh_info_t *sd, bool enable)
{
    struct sdos_info *sdos;

    sd_trace(("%s: %s\n", __FUNCTION__, enable ? "Enabling" : "Disabling"));

    sdos = (struct sdos_info *)sd->sdos_info;
    ASSERT(sdos);

    if (!(sd->host_init_done && sd->card_init_done))
    {
        sd_err(("%s: Card & Host are not initted - bailing\n", __FUNCTION__));
        return SDIOH_API_RC_FAIL;
    }

    if (enable && !(sd->intr_handler))
    {
        sd_err(("%s: no handler registered, will not enable\n", __FUNCTION__));
        return SDIOH_API_RC_FAIL;
    }

    /* Ensure atomicity for enable/disable calls */
    host_rtos_get_semaphore(&sdos->lock, NEVER_TIMEOUT, WICED_FALSE);

    sd->client_intr_enabled = enable;
    if (enable && !sd->lockcount)
    {
        sdstd_devintr_on(sd);
    }
    else
    {
        sdstd_devintr_off(sd);
    }

    host_rtos_set_semaphore(&sdos->lock, WICED_FALSE);

    return SDIOH_API_RC_SUCCESS;
}

/* Protect against reentrancy (disable device interrupts while executing) */
void
sdstd_lock(sdioh_info_t *sd)
{
    struct sdos_info *sdos;

    sdos = (struct sdos_info *)sd->sdos_info;
    ASSERT(sdos);

    sd_trace(("%s: %d\n", __FUNCTION__, sd->lockcount));

    host_rtos_get_semaphore(&sdos->lock, NEVER_TIMEOUT, WICED_FALSE);
    if (sd->lockcount)
    {
        sd_err(("%s: Already locked!\n", __FUNCTION__));
        ASSERT(sd->lockcount == 0);
    }
    if (sd->client_intr_enabled && sd->use_client_ints)
    {
        sdstd_devintr_off(sd);
    }
    sd->lockcount++;
    host_rtos_set_semaphore(&sdos->lock, WICED_FALSE);
}

/* Enable client interrupt */
void
sdstd_unlock(sdioh_info_t *sd)
{
    struct sdos_info *sdos;

    sd_trace(("%s: %d, %d\n", __FUNCTION__, sd->lockcount, sd->client_intr_enabled));
    ASSERT(sd->lockcount > 0);

    sdos = (struct sdos_info *)sd->sdos_info;
    ASSERT(sdos);

    host_rtos_get_semaphore(&sdos->lock, NEVER_TIMEOUT, WICED_FALSE);
    if (--sd->lockcount == 0 && sd->client_intr_enabled)
    {
        sdstd_devintr_on(sd);
    }
    host_rtos_set_semaphore(&sdos->lock, WICED_FALSE);
}

void
sdstd_os_lock_irqsave(sdioh_info_t *sd, ulong* flags)
{
    struct sdos_info *sdos = (struct sdos_info *)sd->sdos_info;
    host_rtos_get_semaphore(&sdos->lock, NEVER_TIMEOUT, WICED_FALSE);
}
void
sdstd_os_unlock_irqrestore(sdioh_info_t *sd, ulong* flags)
{
    struct sdos_info *sdos = (struct sdos_info *)sd->sdos_info;
    host_rtos_set_semaphore(&sdos->lock, WICED_FALSE);
}

void
sdstd_waitlockfree(sdioh_info_t *sd)
{
}

/* Returns 0 for success, -1 for interrupted, -2 for timeout */
int
sdstd_waitbits(sdioh_info_t *sd, uint16 norm, uint16 err, bool yield, uint16 *bits)
{
    sd_trace(("%s: int 0x%04x, err 0x%04x, yield %d\n", __FUNCTION__, norm, err, yield));

    /* Clear the "interrupt happened" flag and last intrstatus */
    sd->got_hcint = FALSE;
    sd->last_intrstatus = 0;

    /* XXX Yielding not implemented yet, use spin wait */
    sdstd_spinbits(sd, norm, err);

    sd_trace(("%s: last_intrstatus 0x%04x\n", __FUNCTION__, sd->last_intrstatus));

    *bits = sd->last_intrstatus;

    return 0;
}

#ifdef DHD_DEBUG
void sdstd_enable_disable_periodic_timer(sdioh_info_t *sd, uint val)
{
    sd_err(("%s: NOT IMPLEMENTED\n", __FUNCTION__));
}
#endif /* debugging purpose */

void
sdstd_3_start_tuning(sdioh_info_t *sd)
{
    sd_err(("%s: NOT IMPLEMENTED\n", __FUNCTION__));
}

void
sdstd_3_osinit_tuning(sdioh_info_t *sd)
{
    sd_err(("%s: NOT IMPLEMENTED\n", __FUNCTION__));
}

void
sdstd_3_osclean_tuning(sdioh_info_t *sd)
{
    sd_err(("%s: NOT IMPLEMENTED\n", __FUNCTION__));
}

