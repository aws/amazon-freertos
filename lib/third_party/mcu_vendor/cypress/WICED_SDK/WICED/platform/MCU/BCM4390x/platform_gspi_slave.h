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
#pragma once

#ifndef PLATFORM_GSPI_SLAVE_H_
#define PLATFORM_GSPI_SLAVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef PAD
#define    _PADLINE(line)    pad ## line
#define    _XSTR(line)    _PADLINE(line)
#define    PAD        _XSTR(__LINE__)
#endif

/******************************************************
 *             Structures
 ******************************************************/
/* core registers */
typedef volatile struct {
    uint32_t corecontrol;        /* CoreControl, 0x000, rev8 */
    uint32_t corestatus;        /* CoreStatus, 0x004, rev8  */
    uint32_t PAD[1];
    uint32_t biststatus;        /* BistStatus, 0x00c, rev8  */

    /* PCMCIA access */
    uint16_t pcmciamesportaladdr;    /* PcmciaMesPortalAddr, 0x010, rev8   */
    uint16_t PAD[1];
    uint16_t pcmciamesportalmask;    /* PcmciaMesPortalMask, 0x014, rev8   */
    uint16_t PAD[1];
    uint16_t pcmciawrframebc;        /* PcmciaWrFrameBC, 0x018, rev8   */
    uint16_t PAD[1];
    uint16_t pcmciaunderflowtimer;    /* PcmciaUnderflowTimer, 0x01c, rev8   */
    uint16_t PAD[1];

    /* interrupt */
    uint32_t intstatus;        /* IntStatus, 0x020, rev8   */
    uint32_t hostintmask;        /* IntHostMask, 0x024, rev8   */
    uint32_t intmask;            /* IntSbMask, 0x028, rev8   */
    uint32_t sbintstatus;        /* SBIntStatus, 0x02c, rev8   */
    uint32_t sbintmask;        /* SBIntMask, 0x030, rev8   */
    uint32_t funcintmask;        /* SDIO Function Interrupt Mask, SDIO rev4 */
    uint32_t PAD[2];
    uint32_t tosbmailbox;        /* ToSBMailbox, 0x040, rev8   */
    uint32_t tohostmailbox;        /* ToHostMailbox, 0x044, rev8   */
    uint32_t tosbmailboxdata;        /* ToSbMailboxData, 0x048, rev8   */
    uint32_t tohostmailboxdata;    /* ToHostMailboxData, 0x04c, rev8   */

    /* synchronized access to registers in SDIO clock domain */
    uint32_t sdioaccess;        /* SdioAccess, 0x050, rev8   */
    uint32_t PAD[3];

    /* PCMCIA frame control */
    uint8_t pcmciaframectrl;        /* pcmciaFrameCtrl, 0x060, rev8   */
    uint8_t PAD[3];
    uint8_t pcmciawatermark;        /* pcmciaWaterMark, 0x064, rev8   */
    uint8_t PAD[155];

    /* interrupt batching control */
    uint32_t intrcvlazy;        /* IntRcvLazy, 0x100, rev8 */
    uint32_t PAD[3];

    /* counters */
    uint32_t cmd52rd;            /* Cmd52RdCount, 0x110, rev8, SDIO: cmd52 reads */
    uint32_t cmd52wr;            /* Cmd52WrCount, 0x114, rev8, SDIO: cmd52 writes */
    uint32_t cmd53rd;            /* Cmd53RdCount, 0x118, rev8, SDIO: cmd53 reads */
    uint32_t cmd53wr;            /* Cmd53WrCount, 0x11c, rev8, SDIO: cmd53 writes */
    uint32_t abort;            /* AbortCount, 0x120, rev8, SDIO: aborts */
    uint32_t datacrcerror;        /* DataCrcErrorCount, 0x124, rev8, SDIO: frames w/bad CRC */
    uint32_t rdoutofsync;        /* RdOutOfSyncCount, 0x128, rev8, SDIO/PCMCIA: Rd Frm OOS */
    uint32_t wroutofsync;        /* RdOutOfSyncCount, 0x12c, rev8, SDIO/PCMCIA: Wr Frm OOS */
    uint32_t writebusy;        /* WriteBusyCount, 0x130, rev8, SDIO: dev asserted "busy" */
    uint32_t readwait;        /* ReadWaitCount, 0x134, rev8, SDIO: read: no data avail */
    uint32_t readterm;        /* ReadTermCount, 0x138, rev8, SDIO: rd frm terminates */
    uint32_t writeterm;        /* WriteTermCount, 0x13c, rev8, SDIO: wr frm terminates */
    uint32_t PAD[40];
    uint32_t clockctlstatus;        /* ClockCtlStatus, 0x1e0, rev8 */
    uint32_t PAD[7];
} gspi_slave_reg_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PLATFORM_GSPI_SLAVE_H_ */
