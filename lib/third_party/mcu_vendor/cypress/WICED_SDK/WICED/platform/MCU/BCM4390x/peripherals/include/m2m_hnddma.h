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
 * Generic Broadcom Home Networking Division (HND) DMA engine SW interface
 * This supports the following chips: BCM42xx, 44xx, 47xx .
 *
 * $Id: hnddma.h 321146 2012-03-14 08:27:23Z deepakd $
 */

#ifndef    _m2m_hnddma_h_
#define    _m2m_hnddma_h_

#include <stdint.h>
#include "bcmdefs.h"
#include "typedefs.h"
#include "siutils.h"

#ifndef _hnddma_pub_
#define _hnddma_pub_
typedef const struct m2m_hnddma_pub m2m_hnddma_t;
//#include "hnddma.h"
#endif /* _hnddma_pub_ */

/* range param for dma_getnexttxp() and dma_txreclaim */
#ifndef txd_range
typedef enum {
    HNDDMA_RANGE_ALL        = 1,
    HNDDMA_RANGE_TRANSMITTED,
    HNDDMA_RANGE_TRANSFERED
} txd_range_t;
#endif




void m2m_dma_detach(m2m_hnddma_t *di);
void m2m_dma_txinit(m2m_hnddma_t *di);
int m2m_dma_txactive(m2m_hnddma_t *di_in);
int BCMFASTPATH m2m_dma_txfast(m2m_hnddma_t *di, void *p0, bool commit);
void BCMFASTPATH m2m_dma_txreclaim(m2m_hnddma_t *di, txd_range_t range);
void * BCMFASTPATH m2m_dma_getnexttxp(m2m_hnddma_t *di, txd_range_t range);
void m2m_dma_rxinit(m2m_hnddma_t *di);
bool m2m_dma_txreset(m2m_hnddma_t *di);
bool m2m_dma_rxreset(m2m_hnddma_t *di);
void * BCMFASTPATH m2m_dma_rx(m2m_hnddma_t *di);
bool BCMFASTPATH m2m_dma_rxfill(m2m_hnddma_t *di);
int m2m_dma_rxactive(m2m_hnddma_t *di_in);
void m2m_dma_rxreclaim(m2m_hnddma_t *di);
void m2m_dma_fifoloopbackenable(m2m_hnddma_t *di);
uint m2m_dma_ctrlflags(m2m_hnddma_t *di, uint mask, uint flags);
bool m2m_dma_txreset_inner(void *osh, void *txregs);
bool m2m_dma_rxreset_inner(void *osh, void *rxregs);

/*
 * Exported data structure (read-only)
 */
/* export structure */
struct m2m_hnddma_pub {
    uint        txavail;    /* # free tx descriptors */
    uint        dmactrlflags;    /* dma control flags */

    /* rx error counters */
    uint        rxgiants;    /* rx giant frames */
    uint        rxnobuf;    /* rx out of dma descriptors */
    /* tx error counters */
    uint        txnobuf;    /* tx out of dma descriptors */
    uint        txnodesc;    /* tx out of dma descriptors running count */
};


extern m2m_hnddma_t * m2m_dma_attach(osl_t *osh, const char *name, si_t *sih,
    volatile void *dmaregstx, volatile void *dmaregsrx,
    uint ntxd, uint nrxd, uint rxbufsize, int rxextheadroom, uint nrxpost,
    uint rxoffset, uint *msg_level);

#endif    /* _m2m_hnddma_h_ */
