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
 * Structure used by apps whose drivers access SDIO drivers.
 * Pulled out separately so dhdu and wlu can both use it.
 *
 * $Id: sdiovar.h 241182 2011-02-17 21:50:03Z gmo $
 */

#ifndef _sdiovar_h_
#define _sdiovar_h_

#include <typedefs.h>

/* require default structure packing */
#define BWL_DEFAULT_PACKING
#include <packed_section_start.h>

typedef struct sdreg {
	int func;
	int offset;
	int value;
} sdreg_t;

/* Common msglevel constants */
#define SDH_ERROR_VAL		0x0001	/* Error */
#define SDH_TRACE_VAL		0x0002	/* Trace */
#define SDH_INFO_VAL		0x0004	/* Info */
#define SDH_DEBUG_VAL		0x0008	/* Debug */
#define SDH_DATA_VAL		0x0010	/* Data */
#define SDH_CTRL_VAL		0x0020	/* Control Regs */
#define SDH_LOG_VAL		0x0040	/* Enable bcmlog */
#define SDH_DMA_VAL		0x0080	/* DMA */

#define NUM_PREV_TRANSACTIONS	16

#ifdef BCMSPI
/* Error statistics for gSPI */
struct spierrstats_t {
	uint32  dna;	/* The requested data is not available. */
	uint32  rdunderflow;	/* FIFO underflow happened due to current (F2, F3) rd command */
	uint32  wroverflow;	/* FIFO underflow happened due to current (F1, F2, F3) wr command */

	uint32  f2interrupt;	/* OR of all F2 related intr status bits. */
	uint32  f3interrupt;	/* OR of all F3 related intr status bits. */

	uint32  f2rxnotready;	/* F2 FIFO is not ready to receive data (FIFO empty) */
	uint32  f3rxnotready;	/* F3 FIFO is not ready to receive data (FIFO empty) */

	uint32  hostcmddataerr;	/* Error in command or host data, detected by CRC/checksum
	                         * (optional)
	                         */
	uint32  f2pktavailable;	/* Packet is available in F2 TX FIFO */
	uint32  f3pktavailable;	/* Packet is available in F2 TX FIFO */

	uint32	dstatus[NUM_PREV_TRANSACTIONS];	/* dstatus bits of last 16 gSPI transactions */
	uint32  spicmd[NUM_PREV_TRANSACTIONS];
};
#endif /* BCMSPI */

#include <packed_section_end.h>

#endif /* _sdiovar_h_ */
