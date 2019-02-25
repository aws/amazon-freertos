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
 * BCM43XX M2M DMA core hardware definitions.
 *
 * $Id:m2mdma _core.h 421139 2013-08-30 17:56:15Z kiranm $
 */
#ifndef	_M2MDMA_CORE_H
#define	_M2MDMA_CORE_H

#include <typedefs.h>
#include <sbhnddma.h>
/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif


/* dma regs to control the flow between host2dev and dev2host  */
typedef struct m2m_devdmaregs {
	dma64regs_t	tx;
	uint32 		PAD[2];
	dma64regs_t	rx;
	uint32 		PAD[2];
} m2m_devdmaregs_t;


typedef struct dmaintregs {
	uint32 intstatus;
	uint32 intmask;
} dmaintregs_t;

#define I_DE            (1 << 10) /* descriptor read error */
#define I_DA            (1 << 11) /* errors while transferring data to or from memory */
#define I_DP            (1 << 12) /* descriptor programming errors */
#define I_RU            (1 << 13) /* channel cannot process an incoming frame because no descriptors are available */
#define I_RI            (1 << 16) /* interrupt from the RX channel */
#define I_XI            (1 << 24) /* interrupt from the TX error */

#define I_ERRORS        (I_DE | I_DA | I_DP)
#define I_DMA           (I_ERRORS | I_RI)

/* SB side: M2M DMA core registers */
typedef struct sbpm2mregs {
	uint32 control;		/* Core control 0x0 */
	uint32 capabilities;	/* Core capabilities 0x4 */
	uint32 intcontrol;	/* Interrupt control 0x8 */
	uint32 PAD[5];
	uint32 intstatus;	/* Interrupt Status  0x20 */
	uint32 PAD[3];
	dmaintregs_t intregs[8]; /* 0x30 - 0x6c */
	uint32 PAD[36];
	uint32 intrxlazy[8];	/* 0x100 - 0x11c */
	uint32 PAD[48];
	uint32 clockctlstatus;  /* 0x1e0 */
	uint32 workaround;	/* 0x1e4 */
	uint32 powercontrol;	/* 0x1e8 */
	uint32 PAD[5];
	m2m_devdmaregs_t dmaregs[8]; /* 0x200 - 0x3f4 */
} sbm2mregs_t;


#define M2M_CAPABILITIES_CHANNEL_COUNT_SHIFT             0
#define M2M_CAPABILITIES_CHANNEL_COUNT_MASK             (0xF << M2M_CAPABILITIES_CHANNEL_COUNT_SHIFT)

#endif	/* _M2MDMA_CORE_H */
