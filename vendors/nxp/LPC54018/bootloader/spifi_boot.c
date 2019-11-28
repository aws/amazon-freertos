/*
 * Copyright 2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_wwdt.h"
#include "fsl_power.h"

#include "spifi_boot.h"
#include "mflash_drv.h"


/* Validates the boot image at given address and returns pointer to image header.
 * If the validation fails the return value is NULL.
 */
static struct boot_image_header *boot_get_image_header(const void *addr)
{
    struct boot_image *boot_image = (struct boot_image *)addr;
    struct boot_image_header *boot_image_header;

    /* Check image marker */
    if (boot_image->image_marker != BOOT_IMAGE_MARKER)
        return NULL;

    /* Check header offset for max value */
    if (boot_image->header_offset > 0x19C)
        return NULL;

    boot_image_header = (struct boot_image_header *)((uint32_t)boot_image + boot_image->header_offset);

    /* Check header marker */
    if (boot_image_header->header_marker != BOOT_HEADER_MARKER)
        return NULL;

    return boot_image_header;
}


static int boot_image_validate(const void *addr)
{
    struct boot_image_header *bih;

    /* get the image header */
    bih = boot_get_image_header(addr);

    /* check load address */
    if (bih == NULL || bih->load_address != BOOT_EXEC_IMAGE_ADDR)
    {
        return -1;
    }

    return 0;
}


/* Validates boot image and copies it to given address of FLASH, returns number of bytes copied upon success */
static int32_t boot_image_copy(void *flash_dst, const void *img)
{
    struct boot_image_header *boot_image_header;
    int32_t result;
    int32_t copy_length;

    boot_image_header = boot_get_image_header(img);
    if (boot_image_header == NULL)
        return 0;

    if (boot_image_header->image_length == 0)
        return 0;

    copy_length = boot_image_header->image_length + 4;
    result = mflash_drv_write(flash_dst, (const uint8_t *)img, copy_length);
    if (result < 0)
    {
        return result;
    }

    return copy_length;
}


/* Reads update control block */
int32_t boot_ucb_read(struct boot_ucb *ucbp)
{
    if (!ucbp)
    {
        return -1;
    }

    *ucbp = *(struct boot_ucb *)(BOOT_UCB_ADDR);

    if ((ucbp->signature != BOOT_UCB_SIGNATURE) || (ucbp->version != BOOT_UCB_VERSION))
    {
        /* the update control block is invalid, forge consistent structure with default/undefinded values */
        memset((void *)ucbp, 0xFF, sizeof(struct boot_ucb));
        ucbp->signature = BOOT_UCB_SIGNATURE;
        ucbp->version = BOOT_UCB_VERSION;
        ucbp->state = BOOT_STATE_UNDEF;

        return -1;
    }

    return 0;
}


/* Writes update control block */
int32_t boot_ucb_write(const struct boot_ucb *ucbp)
{
    return mflash_drv_write((void *)BOOT_UCB_ADDR, (void *)ucbp, sizeof(struct boot_ucb));
}


/* Erases update control block */
int32_t boot_ucb_erase(void)
{
    /* write is used instead of plain erase to preserve rest of the FLASH sector */
    struct boot_ucb ucb;
    memset((void *)&ucb, 0xFF, sizeof(ucb));
    return mflash_drv_write((void *)BOOT_UCB_ADDR, (void *)&ucb, sizeof(ucb));
}


#define portAIRCR_REG ( * ( ( volatile uint32_t * ) 0xE000ED0C ) )
#define VECTKEY_SHIFT 16
#define VECTKEY_VAL 0x5FA
#define SYSRESETREQ_MASK 0x04

void boot_cpureset(void)
{
    portAIRCR_REG = (VECTKEY_VAL << VECTKEY_SHIFT) | SYSRESETREQ_MASK;

    /* wait for reset */
    while (1)
    {
    }
}


/* Schedules update for next reboot by filling in the update control block structure */
int32_t boot_update_request(void *update_img, void *backup_storage)
{
    struct boot_ucb ucb;
    int32_t result;

    memset((void *)&ucb, 0xFF, sizeof(ucb));

    /* backup active image to spare area for rollback */
    if (backup_storage)
    {
        result = boot_image_copy(backup_storage, (void *)BOOT_EXEC_IMAGE_ADDR);
        if (result < 0)
        {
            return result;
        }
    }

    /* prepare and write update control block */
    ucb.signature = BOOT_UCB_SIGNATURE;
    ucb.version = BOOT_UCB_VERSION;
    ucb.state = BOOT_STATE_NEW;
    ucb.update_img = update_img;
    ucb.rollback_img = backup_storage;

    /* store update control block in FLASH and return */
    return boot_ucb_write(&ucb);
}

/* Once update image is running and approved, overwrite the rollback image */
int32_t boot_overwrite_rollback(void)
{
    struct boot_ucb ucb;
    int32_t result = 0;

    if (boot_ucb_read(&ucb) == 0 && ucb.rollback_img != NULL)
    {
        result = boot_image_copy(ucb.rollback_img, ucb.update_img);
    }
    return result;
}

/* Executes image at given address */
void boot_app_exec(const void *addr)
{
    const uint32_t *app_vectors;
    uint32_t app_sp;
    void (*app_entry)(void);

    struct boot_image_header *bih;

    /* validate the image and check load address */
    bih = boot_get_image_header(addr);
    if (bih == NULL || bih->load_address != (uint32_t)(addr))
    {
      return;
    }

    app_vectors = (const uint32_t *)addr;
    app_sp = app_vectors[0];
    app_entry = (void (*)(void))app_vectors[1];

    /* disable all interrupt sources */
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICER[1] = 0xFFFFFFFF;

    /* clear all pending interrupts */
    NVIC->ICPR[0] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;

    /* diable sytick and clear interrupt */
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    /* set up vector table */
    SCB->VTOR = (uint32_t)app_vectors;

    /* set up stack pointer */
    __set_MSP(app_sp);
    __enable_irq();

    app_entry();
}


void boot_wdten(void)
{
    wwdt_config_t config;
    uint32_t wdtFreq;

    POWER_DisablePD(kPDRUNCFG_PD_WDT_OSC);

    wdtFreq = CLOCK_GetFreq(kCLOCK_WdtOsc) / 4; /* prescaled by 4 */
    WWDT_GetDefaultConfig(&config);

    /* Set watchdog timeout to approximately 60 s */
    config.timeoutValue = 60 * wdtFreq;
    /* Configure WWDT to reset on timeout */
    config.enableWatchdogReset = true;

    /* wdog refresh */
    WWDT_Init(WWDT, &config);
}


void boot_wdtdis(void)
{
    POWER_EnablePD(kPDRUNCFG_PD_WDT_OSC);
}


/* Bootloader entry point */
int32_t boot_run(void)
{
    struct boot_ucb ucb;
    void *exec_image = (void *)BOOT_EXEC_IMAGE_ADDR;

    PRINTF("\r\nSPIFI bootloader " BOOT_VERSION_STRING "\r\n");

    /* load update control block */
    boot_ucb_read(&ucb);

    /* update control block is present, process it */
    switch (ucb.state)
    {
    case BOOT_STATE_UNDEF:
    case BOOT_STATE_VOID:
        /* nothing to do be done */
        break;

    case BOOT_STATE_NEW:
        /* new update image available, flash it and switch to test mode */
        if (0 != boot_image_validate(ucb.update_img))
        {
            /* the update image is invalid, erase the update control block and execute the current image */
            PRINTF(BOOT_PROMPT_STRING "Invalid update image!\r\n");
            ucb.state = BOOT_STATE_VOID;
        }
        else
        {
            PRINTF(BOOT_PROMPT_STRING "Installing update... ");
            if (boot_image_copy((void *)BOOT_EXEC_IMAGE_ADDR, ucb.update_img) > 0)
            {
                PRINTF("OK\r\n");
                ucb.state = BOOT_STATE_PENDING_COMMIT;
            }
            else
            {
                PRINTF("ERROR\r\n");
                exec_image = NULL; /* the current image might be corrupted */
                ucb.state = BOOT_STATE_INVALID; /* set state to invalid to got for rollback upon next boot */
            }
        }
        if (boot_ucb_write(&ucb) != 0)
        {
            PRINTF(BOOT_PROMPT_STRING "ERROR writing update control block\r\n");
        }
        break;

    /* reboot from test mode or image explicitly rejected, rollback */
    case BOOT_STATE_PENDING_COMMIT:
    case BOOT_STATE_INVALID:
        if (0 != boot_image_validate(ucb.rollback_img))
        {
            /* the rollback image is invalid, just try executing the current one as last resort solution */
            PRINTF(BOOT_PROMPT_STRING "No rollback image, executing the current one... ");
            ucb.state = BOOT_STATE_VOID;
        }
        else
        {
            PRINTF(BOOT_PROMPT_STRING "Rolling back to previous image... ");
            if (boot_image_copy((void *)BOOT_EXEC_IMAGE_ADDR, ucb.rollback_img) > 0)
            {
                PRINTF("OK\r\n");
                ucb.state = BOOT_STATE_VOID;
            }
            else
            {
                /* flashing failed, set state to invalid to go for rollback upon next boot */
                PRINTF("ERROR\r\n");
                exec_image = NULL; /* the current image might be corrupted */
                ucb.state = BOOT_STATE_VOID; /* as the state is void, next reboot would attempt to execute whatever is there */
                break;
            }
        }
        if (boot_ucb_write(&ucb) != 0)
        {
            PRINTF(BOOT_PROMPT_STRING "ERROR writing update control block\r\n");
        }
        break;


    /* any other state should result in erasing the update control block */
    default:
        PRINTF(BOOT_PROMPT_STRING "Unexpected state, erasing update control block... ");
        if (boot_ucb_erase() == 0)
        {
            PRINTF("OK\r\n");
        }
        else
        {
            PRINTF("ERROR\r\n");
        }
        break;
    }

    DbgConsole_Flush();

    if (exec_image != NULL)
    {
        PRINTF(BOOT_PROMPT_STRING "About to execute application...\r\n");
        if (ucb.state == BOOT_STATE_PENDING_COMMIT)
        {
            /* enable watchdog */
            PRINTF(BOOT_PROMPT_STRING "Enabling watchdog...\r\n");
            boot_wdten();
        }
        boot_app_exec((void *)BOOT_EXEC_IMAGE_ADDR);
        PRINTF(BOOT_PROMPT_STRING "Application exec failed.\r\n");
    }
    else
    {
        PRINTF(BOOT_PROMPT_STRING "No valid image to boot.\r\n");
    }

    DbgConsole_Flush();
    return -1;
}
