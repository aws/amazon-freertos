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
#ifndef _CRYPTO_CORE_H
#define _CRYPTO_CORE_H

#include "typedefs.h"
#include <sbhnddma.h>
#include <stdint.h>
#include "platform_constants.h"

/******************************************************
 *                      Macros
 ******************************************************/

#ifndef PAD
#define _PADLINE(line)  pad ## line
#define _XSTR(line) _PADLINE(line)
#define PAD     _XSTR(__LINE__)
#endif

/******************************************************
 *                    Constants
 ******************************************************/

#define SPUM_BIGENDIAN          1
#define SPUM_LITTLEENDIAN       0
#define SPUM_INPUT_FIFO_SHIFT   11
#define SPUM_OUTPUT_FIFO_SHIFT  12

#define MH_SCTX                 0x80000000
#define MH_BDESC                0x20000000
#define MH_BD                   0x08000000

#define MH_SIZE                 4 /* 4 Bytes */
#define ECH_SIZE                4 /* 4 Bytes */
#define SCTX1_SIZE              4 /* 4 Bytes */
#define SCTX2_SIZE              4 /* 4 Bytes */
#define SCTX3_SIZE              4 /* 4 Bytes */

#define BDESC_SIZE              12 /* 12 Bytes */
#define BD_HDR_SIZE             4 /* 4 Bytes */

#define SCTX2_INBOUND_SHIFT     31
#define SCTX2_ORDER_SHIFT       30
#define SCTX2_CRYPT_ALGO_SHIFT  21
#define SCTX2_CRYPT_MODE_SHIFT  18
#define SCTX2_CRYPT_OPTYPE_SHIFT  16
#define SCTX2_HASH_ALGO_SHIFT   13
#define SCTX2_HASH_MODE_SHIFT   10
#define SCTX2_HASH_OPTYPE_SHIFT 8

#define SCTX_IV                 4
#define SCTX3_IV_FLAGS_SHIFT    5
#define SCTX3_ICV_SHIFT         8

#define BD_PREVLEN_BLOCKSZ      64

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef union
{
    uint32_t raw;
    struct {
        unsigned int cryptostart : 1; /* [ 0 ]    */
        unsigned int softreset : 1;   /* [ 1 ]    */
        unsigned int reserved1 : 4;   /* [ 5:2 ]  */
        unsigned int disablespum : 1; /* [ 6 ]    */
        unsigned int reserved : 25;   /* [ 31:7 ] */
    } bits;
} crypto_cryptocontrol_t;

typedef union
{
    uint32_t raw;
} crypto_cryptostatus_t;

typedef union
{
    uint32_t raw;
} crypto_cryptostatus2_t;

typedef union
{
    uint32_t raw;
} crypto_cryptobiststatus_t;

typedef union
{
    uint32_t raw;
} crypto_spumdebug0_t;

typedef union
{
    uint32_t raw;
} crypto_spumdebug1_t;

typedef union
{
    uint32_t raw;
} crypto_cryptointstatus_t;

typedef union
{
    uint32_t raw;
} crypto_cryptointmask_t;

typedef union
{
    uint32_t raw;
} crypto_indmasize_t;

typedef union
{
    uint32_t raw;
} crypto_outdmasize_t;

typedef union
{
    uint32_t raw;
} crypto_fifostatus_t;

typedef union
{
    uint32_t raw;
} crypto_txqstatus_t;

typedef union
{
    uint32_t raw;
} crypto_txqstatus_0_t;

typedef union
{
    uint32_t raw;
} crypto_txqstatus_1_t;

typedef union
{
    uint32_t raw;
} crypto_spumctrl_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_0_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_1_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_2_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_3_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_4_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_5_t;

typedef union
{
    uint32_t raw;
} crypto_spumstatus_6_t;

typedef union
{
    uint32_t raw;
} crypto_clkctrlstatus_t;

typedef union
{
    uint32_t raw;
} crypto_xmtctrl_t;

typedef union
{
    uint32_t raw;
} crypto_xmtptr_t;

typedef union
{
    uint32_t raw;
} crypto_xmtaddrlow_t;

typedef union
{
    uint32_t raw;
} crypto_xmtaddrhigh_t;

typedef union
{
    uint32_t raw;
} crypto_xmtstatus0_t;

typedef union
{
    uint32_t raw;
} crypto_xmtstatus1_t;

typedef union
{
    uint32_t raw;
} crypto_rcvctrl_t;

typedef union
{
    uint32_t raw_t;
} crypto_rcvptr_t;

typedef union
{
    uint32_t raw;
} crypto_rcvaddrlow_t;

typedef union
{
    uint32_t raw;
} crypto_rcvaddrhigh_t;

typedef union
{
    uint32_t raw;
} crypto_rcvstatus0_t;

typedef union
{
    uint32_t raw;
} crypto_rcvstatus1_t;

typedef union
{
    uint32_t raw;
} crypto_spum_ctrl_t;

typedef union
{
    uint32_t raw;
} crypto_spum_status_t;

typedef union
{
    uint32_t raw;
} crypto_spum_kekbaseaddr_t;

typedef union
{
    uint32_t raw;
} crypto_spum_kekeccaddr_t;

typedef struct crypto_devdmaregs
{
        dma64regs_t tx;
        uint32      PAD[ 2 ];
        dma64regs_t rx;
        uint32      PAD[ 2 ];
} crypto_devdmaregs_t;

typedef struct
{
    crypto_cryptocontrol_t          cryptocontrol;      /* Offset 0x0000 */
    crypto_cryptostatus_t           cryptostatus;       /* Offset 0x0004 */
    crypto_cryptostatus2_t          cryptostatus2;      /* Offset 0x0008 */
    crypto_cryptobiststatus_t       cryptobiststatus;   /* Offset 0x000C */
    uint32 PAD[ 2 ];
    crypto_spumdebug0_t             spumdebug0;         /* Offset 0x0018 */
    crypto_spumdebug1_t             spumdebug1;         /* Offset 0x001C */
    crypto_cryptointstatus_t        cryptointstatus;    /* Offset 0x0020 */
    crypto_cryptointmask_t          cryptointmask;      /* Offset 0x0024 */
    crypto_indmasize_t              indmasize;          /* Offset 0x0028 */
    crypto_outdmasize_t             outdmasize;         /* Offset 0x002C */
    crypto_fifostatus_t             fifostatus;         /* Offset 0x0030 */
    crypto_txqstatus_t              txqstatus;          /* Offset 0x0034 */
    crypto_txqstatus_0_t            txqstatus_0;        /* Offset 0x0038 */
    crypto_txqstatus_1_t            txqstatus_1;        /* Offset 0x003C */
    crypto_spumctrl_t               spumctrl;           /* Offset 0x0040 */
    crypto_spumstatus_0_t           spumstatus_0;       /* Offset 0x0044 */
    crypto_spumstatus_1_t           spumstatus_1;       /* Offset 0x0048 */
    crypto_spumstatus_2_t           spumstatus_2;       /* Offset 0x004C */
    crypto_spumstatus_3_t           spumstatus_3;       /* Offset 0x0050 */
    crypto_spumstatus_4_t           spumstatus_4;       /* Offset 0x0054 */
    crypto_spumstatus_5_t           spumstatus_5;       /* Offset 0x0058 */
    crypto_spumstatus_6_t           spumstatus_6;       /* Offset 0x005C */
    uint32 PAD[ 96 ];
    crypto_clkctrlstatus_t          clkctrlstatus;      /* Offset 0x01E0 */
    uint32 PAD[ 7 ];
    crypto_devdmaregs_t             dmaregs;         /* Offset 0x200 - 0x234 */
    uint32 PAD[ 112 ];
    crypto_spum_ctrl_t              spum_ctrl;          /* Offset 0x0400 */
    uint32 PAD[ 3 ];
    crypto_spum_status_t            spum_status;        /* Offset 0x0410 */
    crypto_spum_kekbaseaddr_t       spum_kekbaseaddr;   /* Offset 0x0414 */
    uint32 PAD[ 2 ];
    crypto_spum_kekeccaddr_t        spum_kekeccaddr;    /* Offset 0x0420 */

} crypto_regs_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#endif //_CRYPTO_CORE_H
