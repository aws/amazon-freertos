/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLE_LL_
#define H_BLE_LL_

#include "stats/stats.h"
#include "os/os_cputime.h"
#include "nimble/nimble_opt.h"
#include "nimble/nimble_npl.h"
#include "controller/ble_phy.h"

#ifdef MYNEWT
#include "controller/ble_ll_ctrl.h"
#include "hal/hal_system.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if MYNEWT_VAL(OS_CPUTIME_FREQ) != 32768
#error 32.768kHz clock required
#endif

#if defined(MYNEWT) && MYNEWT_VAL(BLE_LL_VND_EVENT_ON_ASSERT)
#ifdef NDEBUG
#define BLE_LL_ASSERT(cond) (void(0))
#else
#define BLE_LL_ASSERT(cond) \
    if (!(cond)) { \
        if (hal_debugger_connected()) { \
            assert(0);\
        } else {\
            ble_ll_hci_ev_send_vendor_err(__FILE__, __LINE__); \
            while(1) {}\
        }\
    }
#endif
#else
#define BLE_LL_ASSERT(cond) assert(cond)
#endif
/*
 * XXX:
 * I guess this should not depend on the 32768 crystal to be honest. This
 * should be done for TIMER0 as well since the rf clock chews up more current.
 * Deal with this later.
 *
 * Another note: BLE_XTAL_SETTLE_TIME should be bsp related (I guess). There
 * should be a note in there that the converted usecs to ticks value of this
 * should not be 0. Thus: if you are using a 32.768 os cputime freq, the min
 * value of settle time should be 31 usecs. I would suspect all settling times
 * would exceed 31 usecs.
 */

/* Determines if we need to turn on/off rf clock */
#undef BLE_XCVR_RFCLK

/* We will turn on/off rf clock */
#if MYNEWT_VAL(BLE_XTAL_SETTLE_TIME) != 0
#define BLE_XCVR_RFCLK

#endif

#if MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_2M_PHY) || MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_CODED_PHY)
#define BLE_LL_BT5_PHY_SUPPORTED    (1)
#else
#define BLE_LL_BT5_PHY_SUPPORTED    (0)
#endif

/* Controller revision. */
#define BLE_LL_SUB_VERS_NR      (0x0000)

/* Timing jitter as per spec is +/16 usecs */
#define BLE_LL_JITTER_USECS         (16)

/* Packet queue header definition */
STAILQ_HEAD(ble_ll_pkt_q, os_mbuf_pkthdr);

/*
 * Global Link Layer data object. There is only one Link Layer data object
 * per controller although there may be many instances of the link layer state
 * machine running.
 */
struct ble_ll_obj
{
    /* Supported features */
    uint32_t ll_supp_features;

    /* Current Link Layer state */
    uint8_t ll_state;

    /* Number of ACL data packets supported */
    uint8_t ll_num_acl_pkts;

    /* ACL data packet size */
    uint16_t ll_acl_pkt_size;

    /* Preferred PHY's */
    uint8_t ll_pref_tx_phys;
    uint8_t ll_pref_rx_phys;

#ifdef BLE_XCVR_RFCLK
    uint8_t ll_rfclk_state;
    uint16_t ll_xtal_ticks;
    uint32_t ll_rfclk_start_time;
    struct hal_timer ll_rfclk_timer;
#endif

    /* Task event queue */
    struct ble_npl_eventq ll_evq;

    /* Wait for response timer */
    struct hal_timer ll_wfr_timer;

    /* Packet receive queue (and event). Holds received packets from PHY */
    struct ble_npl_event ll_rx_pkt_ev;
    struct ble_ll_pkt_q ll_rx_pkt_q;

    /* Packet transmit queue */
    struct ble_npl_event ll_tx_pkt_ev;
    struct ble_ll_pkt_q ll_tx_pkt_q;

    /* Data buffer overflow event */
    struct ble_npl_event ll_dbuf_overflow_ev;

    /* Number of completed packets event */
    struct ble_npl_event ll_comp_pkt_ev;

    /* HW error callout */
    struct ble_npl_callout ll_hw_err_timer;
};
extern struct ble_ll_obj g_ble_ll_data;

/* Link layer statistics */
STATS_SECT_START(ble_ll_stats)
    STATS_SECT_ENTRY(hci_cmds)
    STATS_SECT_ENTRY(hci_cmd_errs)
    STATS_SECT_ENTRY(hci_events_sent)
    STATS_SECT_ENTRY(bad_ll_state)
    STATS_SECT_ENTRY(bad_acl_hdr)
    STATS_SECT_ENTRY(no_bufs)
    STATS_SECT_ENTRY(rx_adv_pdu_crc_ok)
    STATS_SECT_ENTRY(rx_adv_pdu_crc_err)
    STATS_SECT_ENTRY(rx_adv_bytes_crc_ok)
    STATS_SECT_ENTRY(rx_adv_bytes_crc_err)
    STATS_SECT_ENTRY(rx_data_pdu_crc_ok)
    STATS_SECT_ENTRY(rx_data_pdu_crc_err)
    STATS_SECT_ENTRY(rx_data_bytes_crc_ok)
    STATS_SECT_ENTRY(rx_data_bytes_crc_err)
    STATS_SECT_ENTRY(rx_adv_malformed_pkts)
    STATS_SECT_ENTRY(rx_adv_ind)
    STATS_SECT_ENTRY(rx_adv_direct_ind)
    STATS_SECT_ENTRY(rx_adv_nonconn_ind)
    STATS_SECT_ENTRY(rx_adv_ext_ind)
    STATS_SECT_ENTRY(rx_scan_reqs)
    STATS_SECT_ENTRY(rx_scan_rsps)
    STATS_SECT_ENTRY(rx_connect_reqs)
    STATS_SECT_ENTRY(rx_scan_ind)
    STATS_SECT_ENTRY(rx_aux_connect_rsp)
    STATS_SECT_ENTRY(adv_txg)
    STATS_SECT_ENTRY(adv_late_starts)
    STATS_SECT_ENTRY(adv_resched_pdu_fail)
    STATS_SECT_ENTRY(adv_drop_event)
    STATS_SECT_ENTRY(sched_state_conn_errs)
    STATS_SECT_ENTRY(sched_state_adv_errs)
    STATS_SECT_ENTRY(scan_starts)
    STATS_SECT_ENTRY(scan_stops)
    STATS_SECT_ENTRY(scan_req_txf)
    STATS_SECT_ENTRY(scan_req_txg)
    STATS_SECT_ENTRY(scan_rsp_txg)
    STATS_SECT_ENTRY(aux_missed_adv)
    STATS_SECT_ENTRY(aux_scheduled)
    STATS_SECT_ENTRY(aux_received)
    STATS_SECT_ENTRY(aux_fired_for_read)
    STATS_SECT_ENTRY(aux_allocated)
    STATS_SECT_ENTRY(aux_freed)
    STATS_SECT_ENTRY(aux_sched_cb)
    STATS_SECT_ENTRY(aux_conn_req_tx)
    STATS_SECT_ENTRY(aux_conn_rsp_tx)
    STATS_SECT_ENTRY(aux_conn_rsp_err)
    STATS_SECT_ENTRY(aux_scan_req_tx)
    STATS_SECT_ENTRY(aux_scan_rsp_err)
    STATS_SECT_ENTRY(aux_chain_cnt)
    STATS_SECT_ENTRY(aux_chain_err)
    STATS_SECT_ENTRY(adv_evt_dropped)
    STATS_SECT_ENTRY(scan_timer_stopped)
    STATS_SECT_ENTRY(scan_timer_restarted)
STATS_SECT_END
extern STATS_SECT_DECL(ble_ll_stats) ble_ll_stats;

/* States */
#define BLE_LL_STATE_STANDBY        (0)
#define BLE_LL_STATE_ADV            (1)
#define BLE_LL_STATE_SCANNING       (2)
#define BLE_LL_STATE_INITIATING     (3)
#define BLE_LL_STATE_CONNECTION     (4)
#define BLE_LL_STATE_DTM            (5)

/* LL Features */
#define BLE_LL_FEAT_LE_ENCRYPTION    (0x00000001)
#define BLE_LL_FEAT_CONN_PARM_REQ    (0x00000002)
#define BLE_LL_FEAT_EXTENDED_REJ     (0x00000004)
#define BLE_LL_FEAT_SLAVE_INIT       (0x00000008)
#define BLE_LL_FEAT_LE_PING          (0x00000010)
#define BLE_LL_FEAT_DATA_LEN_EXT     (0x00000020)
#define BLE_LL_FEAT_LL_PRIVACY       (0x00000040)
#define BLE_LL_FEAT_EXT_SCAN_FILT    (0x00000080)
#define BLE_LL_FEAT_LE_2M_PHY        (0x00000100)
#define BLE_LL_FEAT_STABLE_MOD_ID_TX (0x00000200)
#define BLE_LL_FEAT_STABLE_MOD_ID_RX (0x00000400)
#define BLE_LL_FEAT_LE_CODED_PHY     (0x00000800)
#define BLE_LL_FEAT_EXT_ADV          (0x00001000)
#define BLE_LL_FEAT_PERIODIC_ADV     (0x00002000)
#define BLE_LL_FEAT_CSA2             (0x00004000)
#define BLE_LL_FEAT_LE_POWER_CLASS_1 (0x00008000)
#define BLE_LL_FEAT_MIN_USED_CHAN    (0x00010000)

/* This is initial mask, so if feature exchange will not happen,
 * but host will want to use this procedure, we will try. If not
 * succeed, feature bit will be cleared.
 * Look at LL Features above to find out what is allowed
 */
#define BLE_LL_CONN_INITIAL_FEATURES    (0x00000022)

#define BLE_LL_CONN_CLEAR_FEATURE(connsm, feature)   (connsm->conn_features &= ~(feature))

/* LL timing */
#define BLE_LL_IFS                  (150)       /* usecs */
#define BLE_LL_MAFS                 (300)       /* usecs */

/*
 * BLE LL device address. Note that element 0 of the array is the LSB and
 * is sent over the air first. Byte 5 is the MSB and is the last one sent over
 * the air.
 */
#define BLE_DEV_ADDR_LEN            (6)     /* bytes */

struct ble_dev_addr
{
    uint8_t u8[BLE_DEV_ADDR_LEN];
};

#define BLE_IS_DEV_ADDR_STATIC(addr)        ((addr->u8[5] & 0xc0) == 0xc0)
#define BLE_IS_DEV_ADDR_RESOLVABLE(addr)    ((addr->u8[5] & 0xc0) == 0x40)
#define BLE_IS_DEV_ADDR_UNRESOLVABLE(addr)  ((addr->u8[5] & 0xc0) == 0x00)

/*
 * LL packet format
 *
 *  -> Preamble         (1/2 bytes)
 *  -> Access Address   (4 bytes)
 *  -> PDU              (2 to 257 octets)
 *  -> CRC              (3 bytes)
 */
#define BLE_LL_PREAMBLE_LEN     (1)
#define BLE_LL_ACC_ADDR_LEN     (4)
#define BLE_LL_CRC_LEN          (3)
#define BLE_LL_PDU_HDR_LEN      (2)
#define BLE_LL_MAX_PAYLOAD_LEN  (255)
#define BLE_LL_MIN_PDU_LEN      (BLE_LL_PDU_HDR_LEN)
#define BLE_LL_MAX_PDU_LEN      ((BLE_LL_PDU_HDR_LEN) + (BLE_LL_MAX_PAYLOAD_LEN))
#define BLE_LL_CRCINIT_ADV      (0x555555)

/* Access address for advertising channels */
#define BLE_ACCESS_ADDR_ADV             (0x8E89BED6)

/*
 * Advertising PDU format:
 * -> 2 byte header
 *      -> LSB contains pdu type, txadd and rxadd bits.
 *      -> MSB contains length (6 bits). Length is length of payload. Does
 *         not include the header length itself.
 * -> Payload (max 37 bytes)
 */
#define BLE_ADV_PDU_HDR_TYPE_MASK           (0x0F)
#define BLE_ADV_PDU_HDR_CHSEL_MASK          (0x20)
#define BLE_ADV_PDU_HDR_TXADD_MASK          (0x40)
#define BLE_ADV_PDU_HDR_RXADD_MASK          (0x80)

/* Advertising channel PDU types */
#define BLE_ADV_PDU_TYPE_ADV_IND            (0)
#define BLE_ADV_PDU_TYPE_ADV_DIRECT_IND     (1)
#define BLE_ADV_PDU_TYPE_ADV_NONCONN_IND    (2)
#define BLE_ADV_PDU_TYPE_SCAN_REQ           (3)
#define BLE_ADV_PDU_TYPE_SCAN_RSP           (4)
#define BLE_ADV_PDU_TYPE_CONNECT_REQ        (5)
#define BLE_ADV_PDU_TYPE_ADV_SCAN_IND       (6)
#define BLE_ADV_PDU_TYPE_ADV_EXT_IND        (7)
#define BLE_ADV_PDU_TYPE_AUX_ADV_IND        BLE_ADV_PDU_TYPE_ADV_EXT_IND
#define BLE_ADV_PDU_TYPE_AUX_SCAN_RSP       BLE_ADV_PDU_TYPE_ADV_EXT_IND
#define BLE_ADV_PDU_TYPE_AUX_SYNC_IND       BLE_ADV_PDU_TYPE_ADV_EXT_IND
#define BLE_ADV_PDU_TYPE_AUX_CHAIN_IND      BLE_ADV_PDU_TYPE_ADV_EXT_IND
#define BLE_ADV_PDU_TYPE_AUX_CONNECT_REQ    BLE_ADV_PDU_TYPE_CONNECT_REQ
#define BLE_ADV_PDU_TYPE_AUX_SCAN_REQ       BLE_ADV_PDU_TYPE_SCAN_REQ
#define BLE_ADV_PDU_TYPE_AUX_CONNECT_RSP    (8)

/* Extended Header Length (6b) + AdvMode (2b) */
#define BLE_LL_EXT_ADV_HDR_LEN          (1)

#define BLE_LL_EXT_ADV_ADVA_BIT         (0)
#define BLE_LL_EXT_ADV_TARGETA_BIT      (1)
#define BLE_LL_EXT_ADV_RFU_BIT          (2)
#define BLE_LL_EXT_ADV_DATA_INFO_BIT    (3)
#define BLE_LL_EXT_ADV_AUX_PTR_BIT      (4)
#define BLE_LL_EXT_ADV_SYNC_INFO_BIT    (5)
#define BLE_LL_EXT_ADV_TX_POWER_BIT     (6)

#define BLE_LL_EXT_ADV_FLAGS_SIZE       (1)
#define BLE_LL_EXT_ADV_ADVA_SIZE        (6)
#define BLE_LL_EXT_ADV_TARGETA_SIZE     (6)
#define BLE_LL_EXT_ADV_DATA_INFO_SIZE   (2)
#define BLE_LL_EXT_ADV_AUX_PTR_SIZE     (3)
#define BLE_LL_EXT_ADV_SYNC_INFO_SIZE   (18)
#define BLE_LL_EXT_ADV_TX_POWER_SIZE    (1)

#define BLE_LL_EXT_ADV_MODE_NON_CONN    (0x00)
#define BLE_LL_EXT_ADV_MODE_CONN        (0x01)
#define BLE_LL_EXT_ADV_MODE_SCAN        (0x02)

/* If Channel Selection Algorithm #2 is supported */
#define BLE_ADV_PDU_HDR_CHSEL               (0x20)

/*
 * TxAdd and RxAdd bit definitions. A 0 is a public address; a 1 is a
 * random address.
 */
#define BLE_ADV_PDU_HDR_TXADD_RAND          (0x40)
#define BLE_ADV_PDU_HDR_RXADD_RAND          (0x80)

/*
 * Data Channel format
 *
 *  -> Header (2 bytes)
 *      -> LSB contains llid, nesn, sn and md
 *      -> MSB contains length (8 bits)
 *  -> Payload (0 to 251)
 *  -> MIC (0 or 4 bytes)
 */
#define BLE_LL_DATA_HDR_LLID_MASK       (0x03)
#define BLE_LL_DATA_HDR_NESN_MASK       (0x04)
#define BLE_LL_DATA_HDR_SN_MASK         (0x08)
#define BLE_LL_DATA_HDR_MD_MASK         (0x10)
#define BLE_LL_DATA_HDR_RSRVD_MASK      (0xE0)
#define BLE_LL_DATA_PDU_MAX_PYLD        (251)
#define BLE_LL_DATA_MIC_LEN             (4)

/* LLID definitions */
#define BLE_LL_LLID_RSRVD               (0)
#define BLE_LL_LLID_DATA_FRAG           (1)
#define BLE_LL_LLID_DATA_START          (2)
#define BLE_LL_LLID_CTRL                (3)

/*
 * CONNECT_REQ
 *      -> InitA        (6 bytes)
 *      -> AdvA         (6 bytes)
 *      -> LLData       (22 bytes)
 *          -> Access address (4 bytes)
 *          -> CRC init (3 bytes)
 *          -> WinSize (1 byte)
 *          -> WinOffset (2 bytes)
 *          -> Interval (2 bytes)
 *          -> Latency (2 bytes)
 *          -> Timeout (2 bytes)
 *          -> Channel Map (5 bytes)
 *          -> Hop Increment (5 bits)
 *          -> SCA (3 bits)
 *
 *  InitA is the initiators public (TxAdd=0) or random (TxAdd=1) address.
 *  AdvaA is the advertisers public (RxAdd=0) or random (RxAdd=1) address.
 *  LLData contains connection request data.
 *      aa: Link Layer's access address
 *      crc_init: The CRC initialization value used for CRC calculation.
 *      winsize: The transmit window size = winsize * 1.25 msecs
 *      winoffset: The transmit window offset =  winoffset * 1.25 msecs
 *      interval: The connection interval = interval * 1.25 msecs.
 *      latency: connection slave latency = latency
 *      timeout: Connection supervision timeout = timeout * 10 msecs.
 *      chanmap: contains channel mapping indicating used and unused data
 *               channels. Only bits that are 1 are usable. LSB is channel 0.
 *      hop_inc: Hop increment used for frequency hopping. Random value in
 *               range of 5 to 16.
 */
#define BLE_CONNECT_REQ_LEN         (34)
#define BLE_CONNECT_REQ_PDU_LEN     (BLE_CONNECT_REQ_LEN + BLE_LL_PDU_HDR_LEN)

#define BLE_SCAN_REQ_LEN            (12)
#define BLE_SCAN_RSP_MAX_LEN        (37)
#define BLE_SCAN_RSP_MAX_EXT_LEN    (251)

/*--- External API ---*/
/* Initialize the Link Layer */
void ble_ll_init(void);

/* Reset the Link Layer */
int ble_ll_reset(void);

/* 'Boolean' function returning true if address is a valid random address */
int ble_ll_is_valid_random_addr(uint8_t *addr);

/* Calculate the amount of time in microseconds a PDU with payload length of
 * 'payload_len' will take to transmit on a PHY 'phy_mode'. */
uint32_t ble_ll_pdu_tx_time_get(uint16_t payload_len, int phy_mode);

/* Calculate maximum octets of PDU payload which can be transmitted during
 * 'usecs' on a PHY 'phy_mode'. */
uint16_t ble_ll_pdu_max_tx_octets_get(uint32_t usecs, int phy_mode);

/* Is this address a resolvable private address? */
int ble_ll_is_rpa(uint8_t *addr, uint8_t addr_type);

/* Is 'addr' our device address? 'addr_type' is public (0) or random (!=0) */
int ble_ll_is_our_devaddr(uint8_t *addr, int addr_type);

/* Get identity address 'addr_type' is public (0) or random (!=0) */
uint8_t *ble_ll_get_our_devaddr(uint8_t addr_type);

/**
 * Called to put a packet on the Link Layer transmit packet queue.
 *
 * @param txpdu Pointer to transmit packet
 */
void ble_ll_acl_data_in(struct os_mbuf *txpkt);

/**
 * Allocate a pdu (chain) for reception.
 *
 * @param len Length of PDU. This includes the PDU header as well as payload.
 * Does not include MIC if encrypted.
 *
 * @return struct os_mbuf* Pointer to mbuf chain to hold received packet
 */
struct os_mbuf *ble_ll_rxpdu_alloc(uint16_t len);

/* Tell the Link Layer there has been a data buffer overflow */
void ble_ll_data_buffer_overflow(void);

/* Tell the link layer there has been a hardware error */
void ble_ll_hw_error(void);

/*--- PHY interfaces ---*/
struct ble_mbuf_hdr;

/* Called by the PHY when a packet has started */
int ble_ll_rx_start(uint8_t *rxbuf, uint8_t chan, struct ble_mbuf_hdr *hdr);

/* Called by the PHY when a packet reception ends */
int ble_ll_rx_end(uint8_t *rxbuf, struct ble_mbuf_hdr *rxhdr);

/* Helper callback to tx mbuf using ble_phy_tx() */
uint8_t ble_ll_tx_mbuf_pducb(uint8_t *dptr, void *pducb_arg, uint8_t *hdr_byte);
uint8_t ble_ll_tx_flat_mbuf_pducb(uint8_t *dptr, void *pducb_arg, uint8_t *hdr_byte);

/*--- Controller API ---*/
void ble_ll_mbuf_init(struct os_mbuf *m, uint8_t pdulen, uint8_t hdr);

/* Set the link layer state */
void ble_ll_state_set(uint8_t ll_state);

/* Get the link layer state */
uint8_t ble_ll_state_get(void);

/* Send an event to LL task */
void ble_ll_event_send(struct ble_npl_event *ev);

/* Hand received pdu's to LL task  */
void ble_ll_rx_pdu_in(struct os_mbuf *rxpdu);

/*
 * Set public address
 *
 * This can be used to set controller public address from vendor specific storage,
 * usually should be done in hal_bsp_init().
 * Shall be *only* called before LL is initialized, i.e. before sysinit stage.
 */
int ble_ll_set_public_addr(const uint8_t *addr);

/* Set random address */
int ble_ll_set_random_addr(uint8_t *addr, bool hci_adv_ext);

/* Enable wait for response timer */
void ble_ll_wfr_enable(uint32_t cputime);

/* Disable wait for response timer */
void ble_ll_wfr_disable(void);

/* Wait for response timer expiration callback */
void ble_ll_wfr_timer_exp(void *arg);

/* Read set of features supported by the Link Layer */
uint32_t ble_ll_read_supp_features(void);

/* Read set of states supported by the Link Layer */
uint64_t ble_ll_read_supp_states(void);

/* Check if octets and time are valid. Returns 0 if not valid */
int ble_ll_chk_txrx_octets(uint16_t octets);
int ble_ll_chk_txrx_time(uint16_t time);

/* Random numbers */
int ble_ll_rand_init(void);
void ble_ll_rand_sample(uint8_t rnum);
int ble_ll_rand_data_get(uint8_t *buf, uint8_t len);
void ble_ll_rand_prand_get(uint8_t *prand);
int ble_ll_rand_start(void);

static inline int
ble_ll_get_addr_type(uint8_t txrxflag)
{
    if (txrxflag) {
        return BLE_HCI_ADV_OWN_ADDR_RANDOM;
    }
    return BLE_HCI_ADV_OWN_ADDR_PUBLIC;
}

/* Convert usecs to ticks and round up to nearest tick */
static inline uint32_t
ble_ll_usecs_to_ticks_round_up(uint32_t usecs)
{
    return os_cputime_usecs_to_ticks(usecs + 30);
}

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
/* LTK 0x4C68384139F574D836BCF34E9DFB01BF */
extern const uint8_t g_bletest_LTK[];
extern uint16_t g_bletest_EDIV;
extern uint64_t g_bletest_RAND;
extern uint64_t g_bletest_SKDm;
extern uint64_t g_bletest_SKDs;
extern uint32_t g_bletest_IVm;
extern uint32_t g_bletest_IVs;
#endif

#if MYNEWT_VAL(BLE_LL_DIRECT_TEST_MODE)
void ble_ll_dtm_init();
#endif

#ifdef __cplusplus
}
#endif

#endif /* H_LL_ */
