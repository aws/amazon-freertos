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
 * Definitions for API from sdio common code (bcmsdh) to individual
 * host controller drivers.
 *
 * $Id: bcmsdbus.h 408158 2013-06-17 22:15:35Z linm $
 */

#ifndef	_sdio_api_h_
#define	_sdio_api_h_

/*
 * XXX The following were:
 * 	incorrectly in bcmsdio.h
 * 	incorrectly named using SDIOH which indicates BRCM SDIO FPGA host controller
 */

#define SDIOH_API_RC_SUCCESS                          (0x00)
#define SDIOH_API_RC_FAIL	                      (0x01)
#define SDIOH_API_SUCCESS(status) (status == 0)

#define SDIOH_READ              0	/* Read request */
#define SDIOH_WRITE             1	/* Write request */

#define SDIOH_DATA_FIX          0	/* Fixed addressing */
#define SDIOH_DATA_INC          1	/* Incremental addressing */

#define SDIOH_CMD_TYPE_NORMAL   0       /* Normal command */
#define SDIOH_CMD_TYPE_APPEND   1       /* Append command */
#define SDIOH_CMD_TYPE_CUTTHRU  2       /* Cut-through command */

#define SDIOH_DATA_PIO          0       /* PIO mode */
#define SDIOH_DATA_DMA          1       /* DMA mode */

/* Max number of glommed pkts */
#ifdef CUSTOM_MAX_TXGLOM_SIZE
#define SDPCM_MAXGLOM_SIZE  CUSTOM_MAX_TXGLOM_SIZE
#else
#define SDPCM_MAXGLOM_SIZE	40
#endif /* CUSTOM_MAX_TXGLOM_SIZE */

#define SDPCM_TXGLOM_CPY 0			/* SDIO 2.0 should use copy mode */
#define SDPCM_TXGLOM_MDESC	1		/* SDIO 3.0 should use multi-desc mode */

#ifdef CUSTOM_DEF_TXGLOM_SIZE
#define SDPCM_DEFGLOM_SIZE  CUSTOM_DEF_TXGLOM_SIZE
#else
#define SDPCM_DEFGLOM_SIZE SDPCM_MAXGLOM_SIZE
#endif /* CUSTOM_DEF_TXGLOM_SIZE */

#if SDPCM_DEFGLOM_SIZE > SDPCM_MAXGLOM_SIZE
#warning "SDPCM_DEFGLOM_SIZE cannot be higher than SDPCM_MAXGLOM_SIZE!!"
#undef SDPCM_DEFGLOM_SIZE
#define SDPCM_DEFGLOM_SIZE SDPCM_MAXGLOM_SIZE
#endif

typedef int SDIOH_API_RC;

/* SDio Host structure */
typedef struct sdioh_info sdioh_info_t;

/* callback function, taking one arg */
typedef void (*sdioh_cb_fn_t)(void *);

extern SDIOH_API_RC sdioh_interrupt_register(sdioh_info_t *si, sdioh_cb_fn_t fn, void *argh);
extern SDIOH_API_RC sdioh_interrupt_deregister(sdioh_info_t *si);

/* query whether SD interrupt is enabled or not */
extern SDIOH_API_RC sdioh_interrupt_query(sdioh_info_t *si, bool *onoff);

/* enable or disable SD interrupt */
extern SDIOH_API_RC sdioh_interrupt_set(sdioh_info_t *si, bool enable_disable);

#if defined(DHD_DEBUG) || defined(BCMDBG)
extern bool sdioh_interrupt_pending(sdioh_info_t *si);
#endif

/* read or write one byte using cmd52 */
extern SDIOH_API_RC sdioh_request_byte(sdioh_info_t *si, uint rw, uint fnc, uint addr, uint8 *byte);

/* read or write 2/4 bytes using cmd53 */
extern SDIOH_API_RC sdioh_request_word(sdioh_info_t *si, uint cmd_type, uint rw, uint fnc,
	uint addr, uint32 *word, uint nbyte);

/* read or write any buffer using cmd53 */
extern SDIOH_API_RC sdioh_request_buffer(sdioh_info_t *si, uint pio_dma, uint fix_inc,
	uint rw, uint fnc_num, uint32 addr, uint regwidth, uint32 buflen, uint8 *buffer,
	void *pkt);

/* get cis data */
extern SDIOH_API_RC sdioh_cis_read(sdioh_info_t *si, uint fuc, uint8 *cis, uint32 length);

extern SDIOH_API_RC sdioh_cfg_read(sdioh_info_t *si, uint fuc, uint32 addr, uint8 *data);
extern SDIOH_API_RC sdioh_cfg_write(sdioh_info_t *si, uint fuc, uint32 addr, uint8 *data);

/* query number of io functions */
extern uint sdioh_query_iofnum(sdioh_info_t *si);

/* handle iovars */
extern int sdioh_iovar_op(sdioh_info_t *si, const char *name,
                          void *params, int plen, void *arg, int len, bool set);

/* Issue abort to the specified function and clear controller as needed */
extern int sdioh_abort(sdioh_info_t *si, uint fnc);

/* Start and Stop SDIO without re-enumerating the SD card. */
extern int sdioh_start(sdioh_info_t *si, int stage);
extern int sdioh_stop(sdioh_info_t *si);

/* Wait system lock free */
extern int sdioh_waitlockfree(sdioh_info_t *si);

/* Reset and re-initialize the device */
extern int sdioh_sdio_reset(sdioh_info_t *si);

#ifdef BCMINTERNAL
/* Diags */
extern SDIOH_API_RC sdioh_test_diag(sdioh_info_t *si);
#endif /* BCMINTERNAL */

#ifdef BCMSPI
/* Function to pass gSPI specific device-status bits to dhd. */
extern uint32 sdioh_get_dstatus(sdioh_info_t *si);

/* chipid and chiprev info for lower layers to control sw WAR's for hw bugs. */
extern void sdioh_chipinfo(sdioh_info_t *si, uint32 chip, uint32 chiprev);
extern void sdioh_dwordmode(sdioh_info_t *si, bool set);
#endif /* BCMSPI */

#if defined(BCMSDIOH_STD)
	/*
	 * XXX - Only STD host supports cmd14 sleep.
	 * XXX - Using define instead of empty stubs for other hosts for now.
	 */
	#define SDIOH_SLEEP_ENABLED
#endif
extern SDIOH_API_RC sdioh_sleep(sdioh_info_t *si, bool enab);

/* GPIO support */
extern SDIOH_API_RC sdioh_gpio_init(sdioh_info_t *sd);
extern bool sdioh_gpioin(sdioh_info_t *sd, uint32 gpio);
extern SDIOH_API_RC sdioh_gpioouten(sdioh_info_t *sd, uint32 gpio);
extern SDIOH_API_RC sdioh_gpioout(sdioh_info_t *sd, uint32 gpio, bool enab);

/* SD/MMC read and write block(s) */
extern SDIOH_API_RC sdioh_sdmmc_read_block(sdioh_info_t *sd, uint32_t address, uint8_t* data);
extern SDIOH_API_RC sdioh_sdmmc_read_blocks(sdioh_info_t *sd, uint32_t address, uint32_t count, uint8_t* data);
extern SDIOH_API_RC sdioh_sdmmc_write_block(sdioh_info_t *sd, uint32_t address, const uint8_t* data);
extern SDIOH_API_RC sdioh_sdmmc_write_blocks(sdioh_info_t *sd, uint32_t address, uint32_t count, const uint8_t* data);
extern SDIOH_API_RC sdioh_sdmmc_transfer_error_check(sdioh_info_t *sd, const char* func_name);

#endif /* _sdio_api_h_ */
