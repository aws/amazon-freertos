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
/*
* Custom Reliable Multicast/ioctl definitions for
* Broadcom 802.11abg Networking Device Driver
*
* Definitions subject to change without notice.
*
* Copyright (C) 2010, Broadcom Corporation
* All Rights Reserved.
*
* THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
* KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
* SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
*
*/
#ifndef _wl_relmcast_h
#define _wl_relmcast_h_

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions for Reliable Multicast */
#define WL_RMC_CNT_VERSION       1
#define WL_RMC_TR_VERSION       1
#define WL_RMC_MAX_CLIENT       32
#define WL_RMC_FLAG_INBLACKLIST       1
#define WL_RMC_FLAG_ACTIVEACKER       2
#define WL_RMC_FLAG_RELMCAST       4
#define WL_RMC_MAX_TABLE_ENTRY     4

#define WL_RMC_VER           1
#define WL_RMC_INDEX_ACK_ALL       255
#define WL_RMC_NUM_OF_MC_STREAMS   4
#define WL_RMC_MAX_TRS_PER_GROUP   1
#define WL_RMC_MAX_TRS_IN_ACKALL   1
#define WL_RMC_ACK_MCAST0          0x02
#define WL_RMC_ACK_MCAST_ALL       0x01
#define WL_RMC_ACTF_TIME_MIN       300     /* time in ms */
#define WL_RMC_ACTF_TIME_MAX       20000 /* time in ms */
#define WL_RMC_MAX_NUM_TRS       32     /* maximun transmitters allowed */
#define WL_RMC_ARTMO_MIN           350     /* time in ms */
#define WL_RMC_ARTMO_MAX           40000     /* time in ms */

enum rmc_opcodes {
    RELMCAST_ENTRY_OP_DISABLE = 0,   /* Disable multi-cast group */
    RELMCAST_ENTRY_OP_DELETE  = 1,   /* Delete multi-cast group */
    RELMCAST_ENTRY_OP_ENABLE  = 2,   /* Enable multi-cast group */
    RELMCAST_ENTRY_OP_ACK_ALL = 3    /* Enable ACK ALL bit in AMT */
};

/* RMC operational modes */
enum rmc_modes {
    WL_RMC_MODE_RECEIVER    = 0,     /* Receiver mode by default */
    WL_RMC_MODE_TRANSMITTER = 1,     /* Transmitter mode using wl ackreq */
    WL_RMC_MODE_INITIATOR   = 2     /* Initiator mode using wl ackreq */
};

/* Each RMC mcast client info */
typedef struct wl_relmcast_client {
    uint8_t flag;            /* status of client such as AR, R, or blacklisted */
    int16_t rssi;            /* rssi value of RMC client */
    struct ether_addr addr;        /* mac address of RMC client */
} wl_relmcast_client_t;

/* RMC Counters */
typedef struct wl_rmc_cnts {
    uint16_t  version;        /* see definition of WL_CNT_T_VERSION */
    uint16_t  length;            /* length of entire structure */
    uint16_t    dupcnt;            /* counter for duplicate rmc MPDU */
    uint16_t    ackreq_err;        /* counter for wl ackreq error    */
    uint16_t    af_tx_err;        /* error count for action frame transmit   */
    uint16_t    null_tx_err;        /* error count for rmc null frame transmit */
    uint16_t    af_unicast_tx_err;    /* error count for rmc unicast frame transmit */
    uint16_t    mc_no_amt_slot;        /* No mcast AMT entry available */
    /* Unused. Keep for rom compatibility */
    uint16_t    mc_no_glb_slot;        /* No mcast entry available in global table */
    uint16_t    mc_not_mirrored;    /* mcast group is not mirrored */
    uint16_t    mc_existing_tr;        /* mcast group is already taken by transmitter */
    uint16_t    mc_exist_in_amt;    /* mcast group is already programmed in amt */
    /* Unused. Keep for rom compatibility */
    uint16_t    mc_not_exist_in_gbl;    /* mcast group is not in global table */
    uint16_t    mc_not_exist_in_amt;    /* mcast group is not in AMT table */
    uint16_t    mc_utilized;        /* mcast addressed is already taken */
    uint16_t    mc_taken_other_tr;    /* multi-cast addressed is already taken */
    uint32_t    rmc_rx_frames_mac;      /* no of mc frames received from mac */
    uint32_t    rmc_tx_frames_mac;      /* no of mc frames transmitted to mac */
    uint32_t    mc_null_ar_cnt;         /* no. of times NULL AR is received */
    uint32_t    mc_ar_role_selected;    /* no. of times took AR role */
    uint32_t    mc_ar_role_deleted;    /* no. of times AR role cancelled */
    uint32_t    mc_noacktimer_expired;  /* no. of times noack timer expired */
    uint16_t  mc_no_wl_clk;           /* no wl clk detected when trying to access amt */
    uint16_t  mc_tr_cnt_exceeded;     /* No of transmitters in the network exceeded */
} wl_rmc_cnts_t;

/* RMC Status */
typedef struct wl_relmcast_st {
    uint8_t         ver;        /* version of RMC */
    uint8_t         num;        /* number of clients detected by transmitter */
    wl_relmcast_client_t clients[WL_RMC_MAX_CLIENT];
    uint16_t        err;        /* error status (used in infra) */
    uint16_t        actf_time;    /* action frame time period */
} wl_relmcast_status_t;

/* Entry for each STA/node */
typedef struct wl_rmc_entry {
    /* operation on multi-cast entry such add,
     * delete, ack-all
     */
    int8_t flag;
    struct ether_addr addr;        /* multi-cast group mac address */
} wl_rmc_entry_t;

/* RMC table */
typedef struct wl_rmc_entry_table {
    uint8_t   index;            /* index to a particular mac entry in table */
    uint8_t   opcode;            /* opcodes or operation on entry */
    wl_rmc_entry_t entry[WL_RMC_MAX_TABLE_ENTRY];
} wl_rmc_entry_table_t;

typedef struct wl_rmc_trans_elem {
    struct ether_addr tr_mac;    /* transmitter mac */
    struct ether_addr ar_mac;    /* ar mac */
    uint16_t artmo;            /* AR timeout */
    uint8_t amt_idx;            /* amt table entry */
    uint16_t flag;            /* entry will be acked, not acked, programmed, full etc */
} wl_rmc_trans_elem_t;

/* RMC transmitters */
typedef struct wl_rmc_trans_in_network {
    uint8_t         ver;        /* version of RMC */
    uint8_t         num_tr;        /* number of transmitters in the network */
    wl_rmc_trans_elem_t trs[WL_RMC_MAX_NUM_TRS];
} wl_rmc_trans_in_network_t;



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _wl_relmcast_h */
