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
 *
 * Minimal debug/trace/assert driver definitions for
 * Broadcom Home Networking Division 10/100 Mbit/s Ethernet
 * Device Driver.
 *
 * $Id: et_dbg.h 404499 2013-05-28 01:06:37Z sudhirbs $
 */

#ifndef _et_dbg_
#define _et_dbg_

#ifdef	BCMDBG
struct ether_header;
extern void etc_prhdr(char *msg, struct ether_header *eh, uint len, int unit);
extern void etc_prhex(char *msg, uchar *buf, uint nbytes, int unit);
/*
 * et_msg_level is a bitvector:
 *	0	errors
 *	1	function-level tracing
 *	2	one-line frame tx/rx summary
 *	3	complex frame tx/rx in hex
 */
#define	ET_ERROR(args)	if (!(et_msg_level & 1)) ; else printf args
#define	ET_TRACE(args)	if (!(et_msg_level & 2)) ; else printf args
#define	ET_PRHDR(msg, eh, len, unit)	if (!(et_msg_level & 4)) ; else etc_prhdr(msg, eh, len, unit)
#define	ET_PRPKT(msg, buf, len, unit)	if (!(et_msg_level & 8)) ; else etc_prhex(msg, buf, len, unit)
#else	/* BCMDBG */
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#define	ET_PRHDR(msg, eh, len, unit)
#define	ET_PRPKT(msg, buf, len, unit)
#endif	/* BCMDBG */

extern uint32 et_msg_level;

#ifdef BCMINTERNAL
#define	ET_LOG(fmt, a1, a2)	if (!(et_msg_level & 0x10000)) ; else bcmlog(fmt, a1, a2)
#else
#define	ET_LOG(fmt, a1, a2)
#endif

/* include port-specific tunables */
#ifdef NDIS
#include <et_ndis.h>
#elif defined(__ECOS)
#include <et_ecos.h>
#elif defined(vxworks)
#include <et_vx.h>
#elif defined(linux)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
#include <linux/config.h>
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36) */
#include <et_linux.h>
#elif defined(PMON)
#include <et_pmon.h>
#elif defined(_CFE_)
#include <et_cfe.h>
#elif defined(_HNDRTE_)
#include <et_rte.h>
#elif defined(__NetBSD__)
#include <et_bsd.h>
#elif defined(BCM_WICED)
#include <et_wiced.h>
#else
#error
#endif

#endif /* _et_dbg_ */
