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

#include <assert.h>
#include <string.h>
#include "os/mynewt.h"
#include "bsp/bsp.h"
#include "hal/hal_bsp.h"
#include "hal/hal_gpio.h"
#include "hal/hal_flash.h"
#include "console/console.h"
#include "shell/shell.h"
#include "stats/stats.h"
#include "flash_map/flash_map.h"

/* BLE */
#include "nimble/ble.h"
#include "nimble/ble_hci_trans.h"
#include "nimble/hci_common.h"
#include "host/ble_hs.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_hci.h"
#include "controller/ble_ll_conn.h"
#include "controller/ble_ll_scan.h"
#include "controller/ble_ll_adv.h"

/* RAM HCI transport. */
#include "transport/ram/ble_hci_ram.h"

/* XXX: An app should not include private headers from a library.  The bletest
 * app uses some of nimble's internal details for logging.
 */
#include "../src/ble_hs_priv.h"
#include "bletest_priv.h"

#define BLETEST_TASK_PRIO               5

/* For LED toggling */
int g_led_pin;

/* A buffer for host advertising data */
uint8_t g_host_adv_data[BLE_HCI_MAX_ADV_DATA_LEN];
uint8_t g_host_adv_len;

/* Some application configurations */
#define BLETEST_ROLE_NONE               (0)
#define BLETEST_ROLE_ADVERTISER         (1)
#define BLETEST_ROLE_SCANNER            (2)
#define BLETEST_ROLE_INITIATOR          (3)

#if MYNEWT_VAL(BLETEST_ROLE) == BLETEST_ROLE_ADVERTISER
#define BLETEST_CFG_ROLE                BLETEST_ROLE_ADVERTISER
#endif
#if MYNEWT_VAL(BLETEST_ROLE) == BLETEST_ROLE_SCANNER
#define BLETEST_CFG_ROLE                BLETEST_ROLE_SCANNER
#endif
#if MYNEWT_VAL(BLETEST_ROLE) == BLETEST_ROLE_INITIATOR
#define BLETEST_CFG_ROLE                BLETEST_ROLE_INITIATOR
#endif

#ifndef BLETEST_CFG_ROLE
#error "No role defined! Must define a valid role in syscfg.yml in apps/bletest"
#endif

/* Advertiser config */
#define BLETEST_CFG_ADV_OWN_ADDR_TYPE   (BLE_HCI_ADV_OWN_ADDR_PUBLIC)
#define BLETEST_CFG_ADV_PEER_ADDR_TYPE  (BLE_HCI_ADV_PEER_ADDR_PUBLIC)
#define BLETEST_CFG_ADV_ITVL            (60000 / BLE_HCI_ADV_ITVL)
#define BLETEST_CFG_ADV_TYPE            BLE_HCI_ADV_TYPE_ADV_IND
#define BLETEST_CFG_ADV_FILT_POLICY     (BLE_HCI_ADV_FILT_NONE)
#define BLETEST_CFG_ADV_ADDR_RES_EN     (0)

/* Multi-adv config */
/*
 * Number of advertising instances to start up, not including the default
 * instance. The default instance is used to connect. If this number is greater
 * than the number of available advertising instances, we only use the number
 * of available advertising instances (defined by the configuration setting:
 * BLE_MULTI_ADV_INSTANCES.
 */
#define BLETEST_CFG_ADV_TEST_INSTANCES     (8)

struct bletest_multi_adv_interval
{
    uint8_t adv_type;
    /*
     * Note: if own addr type greater than 1, we use own addr field; otherwise
     * we use the set multi random address call to set the random address
     */
    uint8_t adv_own_addr_type;
    uint16_t adv_itvl;
};

/*
 * NOTE: currently, these are all NONCONN_IND. Thus, must be 100 msecs or
 * greater
 */
const struct bletest_multi_adv_interval
bletest_multi_adv_instances[BLETEST_CFG_ADV_TEST_INSTANCES] = {
    {BLE_HCI_ADV_TYPE_ADV_NONCONN_IND,
     BLE_HCI_ADV_OWN_ADDR_PUBLIC,
     (100000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_SCAN_IND,
     BLE_HCI_ADV_OWN_ADDR_RANDOM,
     (110000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_NONCONN_IND,
     BLE_HCI_ADV_OWN_ADDR_RANDOM,
     (120000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_NONCONN_IND,
     BLE_HCI_ADV_OWN_ADDR_PUBLIC,
     (130000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_SCAN_IND,
     BLE_HCI_ADV_OWN_ADDR_MAX + 1,
     (140000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_NONCONN_IND,
     BLE_HCI_ADV_OWN_ADDR_MAX + 1,
     (150000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_NONCONN_IND,
     BLE_HCI_ADV_OWN_ADDR_PUBLIC,
     (160000 / BLE_HCI_ADV_ITVL)},

    {BLE_HCI_ADV_TYPE_ADV_SCAN_IND,
     BLE_HCI_ADV_OWN_ADDR_PUBLIC,
     (170000 / BLE_HCI_ADV_ITVL)}
};

/*
 * Determines if own address contains random address or set through the
 * multi-adv set random address command
 */
#define BLETEST_CFG_MULTI_ADV_RANDOM_OWN    (0)

/* Scan config */
#define BLETEST_CFG_SCAN_ITVL           (700000 / BLE_HCI_SCAN_ITVL)
#define BLETEST_CFG_SCAN_WINDOW         (700000 / BLE_HCI_SCAN_ITVL)
#define BLETEST_CFG_SCAN_TYPE           (BLE_HCI_SCAN_TYPE_PASSIVE)
#define BLETEST_CFG_SCAN_OWN_ADDR_TYPE  (BLE_HCI_ADV_OWN_ADDR_PUBLIC)
#define BLETEST_CFG_SCAN_FILT_POLICY    (BLE_HCI_SCAN_FILT_NO_WL)
#define BLETEST_CFG_FILT_DUP_ADV        (1)

/* Connection config */
#define BLETEST_CFG_CONN_ITVL           (128)  /* in 1.25 msec increments */
#define BLETEST_CFG_SLAVE_LATENCY       (0)
#define BLETEST_CFG_INIT_FILTER_POLICY  (BLE_HCI_CONN_FILT_NO_WL)
#define BLETEST_CFG_CONN_SPVN_TMO       (1000)  /* 10 msec increments */
#define BLETEST_CFG_MIN_CE_LEN          (6)
#define BLETEST_CFG_MAX_CE_LEN          (BLETEST_CFG_CONN_ITVL)
#define BLETEST_CFG_CONN_PEER_ADDR_TYPE (BLE_HCI_CONN_PEER_ADDR_PUBLIC)
#define BLETEST_CFG_CONN_OWN_ADDR_TYPE  (BLE_HCI_ADV_OWN_ADDR_PUBLIC)
#define BLETEST_CFG_CONCURRENT_CONNS    (1)

/* Test packet config */
#define BLETEST_CFG_RAND_PKT_SIZE       (1)
#define BLETEST_CFG_SUGG_DEF_TXOCTETS   (251)
#define BLETEST_CFG_SUGG_DEF_TXTIME     \
    ble_ll_pdu_tx_time_get(BLETEST_CFG_SUGG_DEF_TXOCTETS + 4, BLE_PHY_1M)

/* Test configurations. One of these should be set to 1 */
#if !defined(BLETEST_CONCURRENT_CONN_TEST) && !defined(BLETEST_THROUGHPUT_TEST)
    #define BLETEST_CONCURRENT_CONN_TEST    (1)
#endif

/* BLETEST variables */
#undef BLETEST_ADV_PKT_NUM
#define BLETEST_MAX_PKT_SIZE            (247)
#define BLETEST_PKT_SIZE                (247)
#define BLETEST_STACK_SIZE              (256)
uint32_t g_next_os_time;
int g_bletest_state;
struct os_eventq g_bletest_evq;
struct os_callout g_bletest_timer;
struct os_task bletest_task;
bssnz_t os_stack_t bletest_stack[BLETEST_STACK_SIZE];
uint32_t g_bletest_conn_end;
int g_bletest_start_update;
uint32_t g_bletest_conn_upd_time;
uint8_t g_bletest_current_conns;
uint8_t g_bletest_cur_peer_addr[BLE_DEV_ADDR_LEN];
uint8_t g_last_handle_used;
uint8_t g_bletest_led_state;
uint32_t g_bletest_led_rate;
uint32_t g_bletest_next_led_time;
uint16_t g_bletest_handle;
uint16_t g_bletest_completed_pkts;
uint16_t g_bletest_outstanding_pkts;
uint16_t g_bletest_ltk_reply_handle;
uint32_t g_bletest_hw_id[4];
struct hci_create_conn g_cc;

/* --- For LE encryption testing --- */
/* Key: 0x4C68384139F574D836BCF34E9DFB01BF */
const uint8_t g_ble_ll_encrypt_test_key[16] =
{
    0x4c, 0x68, 0x38, 0x41, 0x39, 0xf5, 0x74, 0xd8,
    0x36, 0xbc, 0xf3, 0x4e, 0x9d, 0xfb, 0x01, 0xbf
};

/* Plaint text: 0x0213243546576879acbdcedfe0f10213 */
const uint8_t g_ble_ll_encrypt_test_plain_text[16] =
{
    0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79,
    0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1, 0x02, 0x13
};

/* Encrypted data: 0x99ad1b5226a37e3e058e3b8e27c2c666 */
const uint8_t g_ble_ll_encrypt_test_encrypted_data[16] =
{
    0x99, 0xad, 0x1b, 0x52, 0x26, 0xa3, 0x7e, 0x3e,
    0x05, 0x8e, 0x3b, 0x8e, 0x27, 0xc2, 0xc6, 0x66
};

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY) == 1)
uint8_t g_bletest_adv_irk[16] = {
    0xec, 0x02, 0x34, 0xa3, 0x57, 0xc8, 0xad, 0x05,
    0x34, 0x10, 0x10, 0xa6, 0x0a, 0x39, 0x7d, 0x9b
};

uint8_t g_bletest_init_irk[16] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10
};
#endif

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
/* LTK 0x4C68384139F574D836BCF34E9DFB01BF */
const uint8_t g_bletest_LTK[16] =
{
    0x4C,0x68,0x38,0x41,0x39,0xF5,0x74,0xD8,
    0x36,0xBC,0xF3,0x4E,0x9D,0xFB,0x01,0xBF
};
uint16_t g_bletest_EDIV = 0x2474;
uint64_t g_bletest_RAND = 0xABCDEF1234567890;
uint64_t g_bletest_SKDm = 0xACBDCEDFE0F10213;
uint64_t g_bletest_SKDs = 0x0213243546576879;
uint32_t g_bletest_IVm = 0xBADCAB24;
uint32_t g_bletest_IVs = 0xDEAFBABE;
#endif

#if (BLETEST_THROUGHPUT_TEST == 1)
void
bletest_completed_pkt(uint16_t handle)
{
    os_sr_t sr;

    OS_ENTER_CRITICAL(sr);
    if (handle == g_bletest_handle) {
        ++g_bletest_completed_pkts;
    }
    OS_EXIT_CRITICAL(sr);
}
#endif

#ifdef BLETEST_ADV_PKT_NUM
void
bletest_inc_adv_pkt_num(void)
{
    int rc;
    uint8_t *dptr;
    uint8_t digit;

    if (g_host_adv_len != 0) {
        dptr = &g_host_adv_data[18];
        while (dptr >= &g_host_adv_data[13]) {
            digit = *dptr;
            ++digit;
            if (digit == 58) {
                digit = 48;
                *dptr = digit;
                --dptr;
            } else {
                *dptr = digit;
                break;
            }
        }

        rc = bletest_hci_le_set_adv_data(g_host_adv_data, g_host_adv_len);
        assert(rc == 0);
    }
}
#endif

/**
 * Sets the advertising data to be sent in advertising pdu's which contain
 * advertising data.
 *
 * @param dptr
 * @return uint8_t
 */
uint8_t
bletest_set_adv_data(uint8_t *dptr, uint8_t *addr)
{
    uint8_t len;

    /* Place flags in first */
    dptr[0] = 0x02;
    dptr[1] = 0x01;     /* Flags identifier */
    dptr[2] = 0x06;
    dptr += 3;
    len = 3;

    /*  Add HID service */
    dptr[0] = 0x03;
    dptr[1] = 0x03;
    dptr[2] = 0x12;
    dptr[3] = 0x18;
    dptr += 4;
    len += 4;

    /* Add local name */
    dptr[0] = 12;   /* Length of this data, not including the length */
    dptr[1] = 0x09;
    dptr[2] = 'r';
    dptr[3] = 'u';
    dptr[4] = 'n';
    dptr[5] = 't';
    dptr[6] = 'i';
    dptr[7] = 'm';
    dptr[8] = 'e';
    dptr[9] = '-';
    dptr[10] = '0';
    dptr[11] = '0';
    dptr[12] = '7';
    dptr += 13;
    len += 13;

    /* Add local device address */
    dptr[0] = 0x08;
    dptr[1] = 0x1B;
    dptr[2] = 0x00;
    memcpy(dptr + 3, addr, BLE_DEV_ADDR_LEN);
    len += 9;

    g_host_adv_len = len;

    return len;
}

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
void
bletest_init_adv_instances(void)
{
    uint8_t i;
    int rc;
    uint8_t *addr;
    uint8_t adv_len;
    uint8_t inst_allowed;
    uint8_t rand_addr[BLE_DEV_ADDR_LEN];
    struct hci_multi_adv_params adv;

    inst_allowed = MYNEWT_VAL(BLE_MULTI_ADV_INSTANCES);
    if (inst_allowed > BLETEST_CFG_ADV_TEST_INSTANCES) {
        inst_allowed = BLETEST_CFG_ADV_TEST_INSTANCES;
    }

    /* Start up all the instances */
    for (i = 1; i <= inst_allowed; ++i) {
        memset(&adv, 0, sizeof(struct hci_multi_adv_params));

        adv.own_addr_type = bletest_multi_adv_instances[i-1].adv_own_addr_type;
        if (adv.own_addr_type == BLE_HCI_ADV_OWN_ADDR_PUBLIC) {
            addr = g_dev_addr;
        } else {
            memcpy(rand_addr, g_dev_addr, BLE_DEV_ADDR_LEN);
            rand_addr[5] |= 0xc0;
            rand_addr[0] = i;
            /*
             * NOTE: we overload own address type with a special case
             * to denote if we use own address or call to set multi random
             * address.
             */
            if (adv.own_addr_type == BLE_HCI_ADV_OWN_ADDR_RANDOM) {
                rc = bletest_hci_le_set_multi_rand_addr(rand_addr, i);
                assert(rc == 0);
                addr = rand_addr;
            } else {
                adv.own_addr_type = BLE_HCI_ADV_OWN_ADDR_RANDOM;
                addr = rand_addr;
                memcpy(adv.own_addr, addr, BLE_DEV_ADDR_LEN);
            }
        }

        adv.adv_type = bletest_multi_adv_instances[i - 1].adv_type;
        adv.adv_channel_map = 0x07;
        adv.adv_filter_policy = BLE_HCI_ADV_FILT_NONE;
        adv.peer_addr_type = BLE_HCI_ADV_PEER_ADDR_PUBLIC;
        adv_len = bletest_set_adv_data(&g_host_adv_data[0], addr);

        adv.adv_itvl_min = bletest_multi_adv_instances[i - 1].adv_itvl;
        adv.adv_itvl_max = bletest_multi_adv_instances[i - 1].adv_itvl;
        adv.adv_tx_pwr = -1 * i;

        /* Set the advertising parameters */
        rc = bletest_hci_le_set_multi_adv_params(&adv, i);
        assert(rc == 0);

        /* Set advertising data */
        if (adv_len != 0) {
            rc = bletest_hci_le_set_multi_adv_data(&g_host_adv_data[0], adv_len,
                                                   i);
            assert(rc == 0);

            /* Set scan response data */
            rc = bletest_hci_le_set_multi_scan_rsp_data(&g_host_adv_data[0],
                                                        adv_len, i);
            assert(rc == 0);
        }

        /* Set the advertising parameters */
        rc = bletest_hci_le_set_multi_adv_enable(1, i);
        assert(rc == 0);
    }
}

void
bletest_init_advertising(uint8_t instance, int8_t txpwr)
{
    int rc;
    int set_peer_addr;
    uint8_t adv_len;
    uint8_t *addr;
    uint8_t rand_addr[BLE_DEV_ADDR_LEN];
    struct hci_multi_adv_params adv;

    /* Make sure it is a valid instance */
    assert(instance < BLE_LL_ADV_INSTANCES);

    /* Just zero out advertising */
    set_peer_addr = 0;
    memset(&adv, 0, sizeof(struct hci_multi_adv_params));

    /* If we are using a random address, we need to set it */
    adv.own_addr_type = BLETEST_CFG_ADV_OWN_ADDR_TYPE;
    if (adv.own_addr_type & 1) {
        memcpy(rand_addr, g_dev_addr, BLE_DEV_ADDR_LEN);
        rand_addr[5] |= 0xc0;
        if (BLETEST_CFG_MULTI_ADV_RANDOM_OWN == 1) {
            addr = rand_addr;
            memcpy(adv.own_addr, addr, BLE_DEV_ADDR_LEN);
        } else {
            rc = bletest_hci_le_set_multi_rand_addr(rand_addr, instance);
            assert(rc == 0);
            addr = rand_addr;
        }
    } else {
        addr = g_dev_addr;
    }

    /* Set advertising parameters */
    adv.adv_type = BLETEST_CFG_ADV_TYPE;
    adv.adv_channel_map = 0x07;
    adv.adv_filter_policy = BLETEST_CFG_ADV_FILT_POLICY;
    if ((adv.adv_filter_policy & 1) || (BLETEST_CFG_ADV_ADDR_RES_EN == 1)) {
        set_peer_addr = 1;
    }
    adv.peer_addr_type = BLETEST_CFG_ADV_PEER_ADDR_TYPE;
    if ((adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) ||
        (adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD)) {
        set_peer_addr = 1;
        adv_len = 0;
    } else {
        adv_len = bletest_set_adv_data(&g_host_adv_data[0], addr);
    }

    /* Not allowed for multi-adv command */
    if (adv.own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM) {
        assert(0);
    }

    if (set_peer_addr) {
        memcpy(adv.peer_addr, g_bletest_cur_peer_addr, BLE_DEV_ADDR_LEN);
        if (adv.peer_addr_type == BLE_HCI_ADV_PEER_ADDR_RANDOM) {
            adv.peer_addr[5] |= 0xc0;
        }
    }

    console_printf("Trying to connect to %x.%x.%x.%x.%x.%x\n",
                   adv.peer_addr[0], adv.peer_addr[1], adv.peer_addr[2],
                   adv.peer_addr[3], adv.peer_addr[4], adv.peer_addr[5]);

    if (adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) {
        adv.adv_itvl_min = 0;
        adv.adv_itvl_max = 0;
    } else {
        adv.adv_itvl_min = BLETEST_CFG_ADV_ITVL;
        adv.adv_itvl_max = BLETEST_CFG_ADV_ITVL; /* Advertising interval */
    }

    adv.adv_tx_pwr = txpwr;

    /* Set the advertising parameters */
    rc = bletest_hci_le_set_multi_adv_params(&adv, instance);
    assert(rc == 0);

    /* Set advertising data */
    if (adv_len != 0) {
        rc = bletest_hci_le_set_multi_adv_data(&g_host_adv_data[0], adv_len,
                                               instance);
        assert(rc == 0);

        /* Set scan response data */
        rc = bletest_hci_le_set_multi_scan_rsp_data(&g_host_adv_data[0],adv_len,
                                                    instance);
        assert(rc == 0);
    }
}
#else
void
bletest_init_advertising(void)
{
    int rc;
    int set_peer_addr;
    uint8_t adv_len;
    uint8_t *addr;
    uint8_t rand_addr[BLE_DEV_ADDR_LEN];
    struct hci_adv_params adv;

    /* Just zero out advertising */
    set_peer_addr = 0;
    memset(&adv, 0, sizeof(struct hci_adv_params));

    /* If we are using a random address, we need to set it */
    adv.own_addr_type = BLETEST_CFG_ADV_OWN_ADDR_TYPE;
    if (adv.own_addr_type & 1) {
        memcpy(rand_addr, g_dev_addr, BLE_DEV_ADDR_LEN);
        rand_addr[5] |= 0xc0;
        rc = bletest_hci_le_set_rand_addr(rand_addr);
        assert(rc == 0);
        addr = rand_addr;
    } else {
        addr = g_dev_addr;
    }

    /* Set advertising parameters */
    adv.adv_type = BLETEST_CFG_ADV_TYPE;
    adv.adv_channel_map = 0x07;
    adv.adv_filter_policy = BLETEST_CFG_ADV_FILT_POLICY;
    if ((adv.adv_filter_policy & 1) || (BLETEST_CFG_ADV_ADDR_RES_EN == 1)) {
        set_peer_addr = 1;
    }
    adv.peer_addr_type = BLETEST_CFG_ADV_PEER_ADDR_TYPE;
    if ((adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) ||
        (adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD)) {
        set_peer_addr = 1;
        adv_len = 0;
    } else {
        adv_len = bletest_set_adv_data(&g_host_adv_data[0], addr);
    }

    if (adv.own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM) {
        set_peer_addr = 1;
    }

    if (set_peer_addr) {
        memcpy(adv.peer_addr, g_bletest_cur_peer_addr, BLE_DEV_ADDR_LEN);
        if (adv.peer_addr_type == BLE_HCI_ADV_PEER_ADDR_RANDOM) {
            adv.peer_addr[5] |= 0xc0;
        }
    }

    console_printf("Trying to connect to %x.%x.%x.%x.%x.%x\n",
                   adv.peer_addr[0], adv.peer_addr[1], adv.peer_addr[2],
                   adv.peer_addr[3], adv.peer_addr[4], adv.peer_addr[5]);

    if (adv.adv_type == BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD) {
        adv.adv_itvl_min = 0;
        adv.adv_itvl_max = 0;
    } else {
        adv.adv_itvl_min = BLETEST_CFG_ADV_ITVL;
        adv.adv_itvl_max = BLETEST_CFG_ADV_ITVL; /* Advertising interval */
    }

    /* Set the advertising parameters */
    rc = bletest_hci_le_set_adv_params(&adv);
    assert(rc == 0);

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY) == 1)
    if ((adv.own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM) ||
        (BLETEST_CFG_ADV_ADDR_RES_EN == 1)) {
        rc = bletest_hci_le_add_resolv_list(g_bletest_adv_irk,
                                            g_bletest_init_irk,
                                            adv.peer_addr,
                                            adv.peer_addr_type);
        assert(rc == 0);

        rc = bletest_hci_le_enable_resolv_list(1);
        assert(rc == 0);
    }
#endif

    /* Set advertising data */
    if (adv_len != 0) {
        rc = bletest_hci_le_set_adv_data(&g_host_adv_data[0], adv_len);
        assert(rc == 0);

        /* Set scan response data */
        rc = bletest_hci_le_set_scan_rsp_data(&g_host_adv_data[0], adv_len);
        assert(rc == 0);
    }
}
#endif  /* MULTI_ADV SUPPORT */
#endif  /* BLETEST_ROLE_ADVERTISER */

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_SCANNER)
void
bletest_init_scanner(void)
{
    int rc;
    uint8_t own_addr_type;
    uint8_t buf[BLE_HCI_CMD_HDR_LEN + BLE_HCI_SET_SCAN_PARAM_LEN];
    uint8_t add_whitelist;

    own_addr_type = BLETEST_CFG_SCAN_OWN_ADDR_TYPE;
    rc = ble_hs_hci_cmd_build_le_set_scan_params(BLETEST_CFG_SCAN_TYPE,
                                               BLETEST_CFG_SCAN_ITVL,
                                               BLETEST_CFG_SCAN_WINDOW,
                                               own_addr_type,
                                               BLETEST_CFG_SCAN_FILT_POLICY,
                                               buf, sizeof buf);
    assert(rc == 0);
    rc = ble_hs_hci_cmd_tx_empty_ack(buf);
    if (rc == 0) {
        add_whitelist = BLETEST_CFG_SCAN_FILT_POLICY;
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY) == 1)
        if (own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM) {
            rc = bletest_hci_le_add_resolv_list(g_bletest_init_irk,
                                                g_bletest_adv_irk,
                                                g_bletest_cur_peer_addr,
                                                BLETEST_CFG_ADV_PEER_ADDR_TYPE);
            assert(rc == 0);

            rc = bletest_hci_le_enable_resolv_list(1);
            assert(rc == 0);
        }
#endif
        if (add_whitelist & 1) {
            rc = bletest_hci_le_add_to_whitelist(g_bletest_cur_peer_addr,
                                                 BLE_ADDR_RANDOM);
            assert(rc == 0);
        }
    }
}

void
bletest_execute_scanner(void)
{
    int rc;

    /* Enable scanning */
    if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
        if (g_bletest_state) {
            rc = bletest_hci_le_set_scan_enable(0, BLETEST_CFG_FILT_DUP_ADV);
            assert(rc == 0);
            g_bletest_state = 0;
        } else {
            rc = bletest_hci_le_set_scan_enable(1, BLETEST_CFG_FILT_DUP_ADV);
            assert(rc == 0);
            g_bletest_state = 1;
        }
        g_next_os_time += (OS_TICKS_PER_SEC * 60);
    }
}
#endif

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_INITIATOR)
void
bletest_init_initiator(void)
{
    int rc;
    uint8_t rand_addr[BLE_DEV_ADDR_LEN];
    struct hci_create_conn *hcc;

    /* Enable initiating */
    hcc = &g_cc;
    hcc->conn_itvl_max = BLETEST_CFG_CONN_ITVL;
    hcc->conn_itvl_min = BLETEST_CFG_CONN_ITVL;
    hcc->conn_latency = BLETEST_CFG_SLAVE_LATENCY;
    hcc->filter_policy = BLETEST_CFG_INIT_FILTER_POLICY;
    hcc->supervision_timeout = BLETEST_CFG_CONN_SPVN_TMO;
    hcc->scan_itvl = BLETEST_CFG_SCAN_ITVL;
    hcc->scan_window = BLETEST_CFG_SCAN_WINDOW;
    hcc->peer_addr_type = BLETEST_CFG_CONN_PEER_ADDR_TYPE;
    memcpy(hcc->peer_addr, g_bletest_cur_peer_addr, BLE_DEV_ADDR_LEN);
    if (hcc->peer_addr_type == BLE_HCI_CONN_PEER_ADDR_RANDOM) {
        hcc->peer_addr[5] |= 0xc0;
    }
    hcc->own_addr_type = BLETEST_CFG_CONN_OWN_ADDR_TYPE;
    hcc->min_ce_len = BLETEST_CFG_MIN_CE_LEN;
    hcc->max_ce_len = BLETEST_CFG_MAX_CE_LEN;

    console_printf("Trying to connect to %x.%x.%x.%x.%x.%x\n",
                   hcc->peer_addr[0], hcc->peer_addr[1], hcc->peer_addr[2],
                   hcc->peer_addr[3], hcc->peer_addr[4], hcc->peer_addr[5]);

    /* If we are using a random address, we need to set it */
    if (hcc->own_addr_type == BLE_HCI_ADV_OWN_ADDR_RANDOM) {
        memcpy(rand_addr, g_dev_addr, BLE_DEV_ADDR_LEN);
        rand_addr[5] |= 0xc0;
        rc = bletest_hci_le_set_rand_addr(rand_addr);
        assert(rc == 0);
    }

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LL_PRIVACY) == 1)
        if ((hcc->peer_addr_type > BLE_HCI_CONN_PEER_ADDR_RANDOM) ||
            (hcc->own_addr_type > BLE_HCI_ADV_OWN_ADDR_RANDOM)) {
            rc = bletest_hci_le_add_resolv_list(g_bletest_init_irk,
                                                g_bletest_adv_irk,
                                                g_bletest_cur_peer_addr,
                                                BLETEST_CFG_ADV_PEER_ADDR_TYPE);
            assert(rc == 0);

            rc = bletest_hci_le_enable_resolv_list(1);
            assert(rc == 0);
        }
#endif

    bletest_hci_le_create_connection(hcc);
}

void
bletest_execute_initiator(void)
{
    int i;
    int rc;
    int8_t rssi;
    uint16_t handle;
    uint8_t new_chan_map[5];

    /*
     * Determine if there is an active connection for the current handle
     * we are trying to create. If so, start looking for the next one
     */
    if (g_bletest_current_conns < BLETEST_CFG_CONCURRENT_CONNS) {
        handle = g_bletest_current_conns + 1;
        if (ble_ll_conn_find_active_conn(handle)) {
            /* Set LED to slower blink rate */
            g_bletest_led_rate = OS_TICKS_PER_SEC;

            /* Ask for version information */
            rc = bletest_hci_rd_rem_version(handle);

            /* Ask for remote used features */
            rc = bletest_hci_le_read_rem_used_feat(handle);

            /* Scanning better be stopped! */
            assert(ble_ll_scan_enabled() == 0);

            /* Add to current connections */
            if (!rc) {
                ++g_bletest_current_conns;

                /* Move to next connection */
                if (g_bletest_current_conns < BLETEST_CFG_CONCURRENT_CONNS) {
                    /* restart initiating */
                    g_bletest_cur_peer_addr[5] += 1;
                    g_dev_addr[5] += 1;
                    bletest_init_initiator();
                }
            }
        } else {
            if (ble_ll_scan_enabled() == 0) {
                bletest_hci_le_create_connection(&g_cc);
            }
        }
    } else {
        if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
            if ((g_bletest_state == 1) || (g_bletest_state == 3)) {
                for (i = 0; i < g_bletest_current_conns; ++i) {
                    if (ble_ll_conn_find_active_conn(i + 1)) {
                        bletest_hci_le_rd_chanmap(i+1);
                    }
                }
            } else if (g_bletest_state == 2) {
                new_chan_map[0] = 0;
                new_chan_map[1] = 0x3;
                new_chan_map[2] = 0;
                new_chan_map[3] = 0x1F;
                new_chan_map[4] = 0;
                bletest_hci_le_set_host_chan_class(new_chan_map);
            } else if (g_bletest_state == 4) {
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
                struct hci_start_encrypt hsle;
                for (i = 0; i < g_bletest_current_conns; ++i) {
                    if (ble_ll_conn_find_active_conn(i + 1)) {
                        hsle.connection_handle = i + 1;
                        hsle.encrypted_diversifier = g_bletest_EDIV;
                        hsle.random_number = g_bletest_RAND;
                        swap_buf(hsle.long_term_key, (uint8_t *)g_bletest_LTK,
                                 16);
                        bletest_hci_le_start_encrypt(&hsle);
                    }
                }
#endif
            } else if (g_bletest_state == 8) {
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
                struct hci_start_encrypt hsle;
                for (i = 0; i < g_bletest_current_conns; ++i) {
                    if (ble_ll_conn_find_active_conn(i + 1)) {
                        hsle.connection_handle = i + 1;
                        hsle.encrypted_diversifier = g_bletest_EDIV;
                        hsle.random_number = ~g_bletest_RAND;
                        swap_buf(hsle.long_term_key, (uint8_t *)g_bletest_LTK,
                                 16);
                        bletest_hci_le_start_encrypt(&hsle);
                    }
                }
#endif
            } else {
                for (i = 0; i < g_bletest_current_conns; ++i) {
                    if (ble_ll_conn_find_active_conn(i + 1)) {
                        ble_hs_hci_util_read_rssi(i+1, &rssi);
                    }
                }
            }

            ++g_bletest_state;
            if (g_bletest_state > 9) {
                g_bletest_state = 9;
            }
            g_next_os_time = os_time_get() + OS_TICKS_PER_SEC * 3;
        }
    }
}
#endif

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
/*
 * Test wrapper to get packets. Only get a packet if we have more than half
 * left
 */
static struct os_mbuf *
bletest_get_packet(void)
{
    struct os_mbuf *om;

    om = NULL;
    if (os_msys_num_free() >= 5) {
        om = os_msys_get_pkthdr(0, sizeof(struct ble_mbuf_hdr));
    }
    return om;
}

static struct os_mbuf *
bletest_send_packet(uint16_t handle)
{
    int j;
    uint8_t val;
    struct os_mbuf *om;
    uint16_t pktlen;

    om = bletest_get_packet();
    if (om) {
        /* set payload length */
#if BLETEST_THROUGHPUT_TEST
        pktlen = BLETEST_PKT_SIZE;
#else
#if (BLETEST_CFG_RAND_PKT_SIZE == 1)
        pktlen = rand() % (BLETEST_MAX_PKT_SIZE + 1);
#else
        pktlen = BLETEST_PKT_SIZE;
#endif
#endif

        /* Put the HCI header in the mbuf */
        put_le16(om->om_data, handle);
        put_le16(om->om_data + 2, pktlen + 4);

        /* Place L2CAP header in packet */
        put_le16(om->om_data + 4, pktlen);
        om->om_data[6] = 0;
        om->om_data[7] = 0;
        om->om_len = 8;
        OS_MBUF_PKTHDR(om)->omp_len = 8;

        /* Fill with incrementing pattern (starting from 1) */
        for (j = 0; j < pktlen; ++j) {
            val = j + 1;
            os_mbuf_append(om, &val, 1);
        }

        /* Transmit it */
        ble_hci_trans_hs_acl_tx(om);
    }

    return om;
}

static void
bletest_execute_advertiser(void)
{
    int i;
#if (BLETEST_CONCURRENT_CONN_TEST == 1)
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
    uint16_t mask;
    uint16_t reply_handle;
#endif
#endif
    int rc;
    uint16_t handle;
    struct os_mbuf *om;
#if (BLETEST_THROUGHPUT_TEST == 1)
    os_sr_t sr;
    uint16_t completed_pkts;
#endif

    /* See if we should start advertising again */
    if (g_bletest_current_conns < BLETEST_CFG_CONCURRENT_CONNS) {
        handle = g_bletest_current_conns + 1;
        if (ble_ll_conn_find_active_conn(handle)) {
            /* Set LED to slower blink rate */
            g_bletest_led_rate = OS_TICKS_PER_SEC;

#if (BLETEST_THROUGHPUT_TEST == 1)
            /* Set next os time to 10 seconds after 1st connection */
            if (g_next_os_time == 0) {
                g_next_os_time = os_time_get() + (10 * OS_TICKS_PER_SEC);
                g_bletest_handle = handle;
            }
#endif

            /* advertising better be stopped! */
            assert(ble_ll_adv_enabled() == 0);

            /* Send the remote used features command */
            rc = bletest_hci_le_read_rem_used_feat(handle);
            if (rc) {
                return;
            }

            /* Send the remote read version command */
            rc = bletest_hci_rd_rem_version(handle);
            if (rc) {
                return;
            }

            /* set conn update time */
            g_bletest_conn_upd_time = os_time_get() + (OS_TICKS_PER_SEC * 5);
            g_bletest_start_update = 1;

            /* Add to current connections */
            ++g_bletest_current_conns;

            /* Move to next connection */
            if (g_bletest_current_conns < BLETEST_CFG_CONCURRENT_CONNS) {
                /* restart initiating */
                g_bletest_cur_peer_addr[5] += 1;
                g_dev_addr[5] += 1;
#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
                bletest_init_advertising(0,0);
                bletest_hci_le_set_multi_adv_enable(1, 0);
#else
                bletest_init_advertising();
                bletest_hci_le_set_adv_enable(1);
#endif
            }
        } else {
            /* If we failed to start advertising we should keep trying */
            if (ble_ll_adv_enabled() == 0) {
#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
                bletest_hci_le_set_multi_adv_enable(1, 0);
#else
                bletest_hci_le_set_adv_enable(1);
#endif
            }
        }
    }
#if 0
    if (g_bletest_start_update) {
        if ((int32_t)(os_time_get() - g_bletest_conn_upd_time) >= 0) {
            bletest_send_conn_update(1);
            g_bletest_start_update = 0;
        }
    }
#endif

#if (BLETEST_CONCURRENT_CONN_TEST == 1)
    /* See if it is time to hand a data packet to the connection */
    if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
        /* Do we need to send a LTK reply? */
        mask = 1;
        reply_handle = 1;
        while (g_bletest_ltk_reply_handle && mask) {
            if (g_bletest_ltk_reply_handle & mask) {
                bletest_send_ltk_req_reply(reply_handle);
                //bletest_send_ltk_req_neg_reply(reply_handle);
                g_bletest_ltk_reply_handle &= ~mask;
            }
            ++reply_handle;
            mask <<= 1;
        }
#endif
        if (g_bletest_current_conns) {
            for (i = 0; i < g_bletest_current_conns; ++i) {
                if ((g_last_handle_used == 0) ||
                    (g_last_handle_used > g_bletest_current_conns)) {
                    g_last_handle_used = 1;
                }
                handle = g_last_handle_used;
                if (ble_ll_conn_find_active_conn(handle)) {
                    om = bletest_send_packet(handle);
                    if (om) {
                        /* Increment last handle used */
                        ++g_last_handle_used;
                    }
                } else {
                    ++g_last_handle_used;
                }
            }
        }
        g_next_os_time = os_time_get() + OS_TICKS_PER_SEC;
    }
#endif

#if (BLETEST_THROUGHPUT_TEST == 1)
    /* Nothing to do if no connections */
    if (!g_bletest_current_conns) {
        return;
    }

    /* See if it is time to start throughput testing */
    if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
        /* Keep window full */
        OS_ENTER_CRITICAL(sr);
        completed_pkts = g_bletest_completed_pkts;
        g_bletest_completed_pkts = 0;
        OS_EXIT_CRITICAL(sr);

        assert(g_bletest_outstanding_pkts >= completed_pkts);
        g_bletest_outstanding_pkts -= completed_pkts;

        while (g_bletest_outstanding_pkts < 20) {
            om = bletest_send_packet(g_bletest_handle);
            if (om) {
                ++g_bletest_outstanding_pkts;
            }
        }
    }
#endif /* XXX: throughput test */
}
#endif /* XXX: BLETEST_ROLE_ADVERTISER */

/**
 * Main bletest function. Called by the task timer every 50 msecs.
 *
 */
void
bletest_execute(void)
{
    /* Toggle LED at set rate */
    if ((int32_t)(os_time_get() - g_bletest_next_led_time) >= 0) {
        hal_gpio_toggle(LED_BLINK_PIN);
        g_bletest_next_led_time = os_time_get() + g_bletest_led_rate;
    }
#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
    bletest_execute_advertiser();
#endif
#if (BLETEST_CFG_ROLE == BLETEST_ROLE_SCANNER)
    bletest_execute_scanner();
#endif
#if (BLETEST_CFG_ROLE == BLETEST_ROLE_INITIATOR)
    bletest_execute_initiator();
#endif
}

/**
 * Callback when BLE test timer expires.
 *
 * @param arg
 */
void
bletest_timer_cb(struct os_event *ev)
{
    /* Call the bletest code */
    bletest_execute();

    /* Re-start the timer (run every 10 msecs) */
    os_callout_reset(&g_bletest_timer, OS_TICKS_PER_SEC / 100);
}

/**
 * BLE test task
 *
 * @param arg
 */
void
bletest_task_handler(void *arg)
{
    int rc;
    uint64_t rand64;
    uint64_t event_mask;

    /* Set LED blink rate */
    g_bletest_led_rate = OS_TICKS_PER_SEC / 20;

    /* Wait one second before starting test task */
    os_time_delay(OS_TICKS_PER_SEC);

    /* Initialize the host timer */
    os_callout_init(&g_bletest_timer, &g_bletest_evq, bletest_timer_cb,
                    NULL);

    ble_hs_dbg_set_sync_state(BLE_HS_SYNC_STATE_GOOD);

    /* Send the reset command first */
    rc = bletest_hci_reset_ctlr();
    assert(rc == 0);

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
    /* Initialize the advertiser */
    console_printf("Starting BLE test task as advertiser\n");
#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
    /* Start up all advertising instances except default one */
    bletest_init_adv_instances();

    /* Start advertising on instance 0 at 0 dbm */
    bletest_init_advertising(0, 0);
#else
    bletest_init_advertising();
#endif
#endif

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_SCANNER)
    /* Initialize the scanner */
    console_printf("Starting BLE test task as scanner\n");
    bletest_init_scanner();
#endif

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_INITIATOR)
    /* Initialize the scanner */
    console_printf("Starting BLE test task as initiator\n");
    bletest_init_initiator();
#endif

    /* Read unique HW id */
    rc = hal_bsp_hw_id((void *)&g_bletest_hw_id[0], sizeof(g_bletest_hw_id));
    assert(rc == 16);
    console_printf("HW id=%04x%04x%04x%04x\n",
                   (unsigned int)g_bletest_hw_id[0],
                   (unsigned int)g_bletest_hw_id[1],
                   (unsigned int)g_bletest_hw_id[2],
                   (unsigned int)g_bletest_hw_id[3]);

    /* Set the event mask we want to display */
    event_mask = 0x7FF;
    rc = bletest_hci_le_set_event_mask(event_mask);
    assert(rc == 0);

    /* Turn on all events */
    event_mask = 0xffffffffffffffff;
    rc = bletest_hci_set_event_mask(event_mask);
    assert(rc == 0);

    /* Read device address */
    rc = bletest_hci_rd_bd_addr();
    assert(rc == 0);

    /* Read local features */
    rc = bletest_hci_rd_local_feat();
    assert(rc == 0);

    /* Read local commands */
    rc = bletest_hci_rd_local_supp_cmd();
    assert(rc == 0);

    /* Read version */
    rc = bletest_hci_rd_local_version();
    assert(rc == 0);

    /* Read supported states */
    rc = bletest_hci_le_read_supp_states();
    assert(rc == 0);

    /* Read maximum data length */
    rc = bletest_hci_le_rd_max_datalen();
    assert(rc == 0);

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_DATA_LEN_EXT) == 1)
    /* Read suggested data length */
    rc = bletest_hci_le_rd_sugg_datalen();
    assert(rc == 0);

    /* write suggested default data length */
    rc = bletest_hci_le_write_sugg_datalen(BLETEST_CFG_SUGG_DEF_TXOCTETS,
                                           BLETEST_CFG_SUGG_DEF_TXTIME);
    assert(rc == 0);

    /* Read suggested data length */
    rc = bletest_hci_le_rd_sugg_datalen();
    assert(rc == 0);

    /* Set data length (note: we know there is no connection; just a test) */
    rc = bletest_hci_le_set_datalen(0x1234, BLETEST_CFG_SUGG_DEF_TXOCTETS,
                                    BLETEST_CFG_SUGG_DEF_TXTIME);
    assert(rc != 0);
#endif

    /* Encrypt a block */
#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
    rc = bletest_hci_le_encrypt((uint8_t *)g_ble_ll_encrypt_test_key,
                                (uint8_t *)g_ble_ll_encrypt_test_plain_text);
    assert(rc == 0);
#endif

    /* Get a random number */
    rc = ble_hs_hci_util_rand(&rand64, 8);
    assert(rc == 0);

    /* Wait some time before starting */
    os_time_delay(OS_TICKS_PER_SEC);

    /* Init state */
    g_bletest_state = 0;

    /* Begin advertising if we are an advertiser */
#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
    rc = bletest_hci_le_set_multi_adv_enable(1, 0);
    assert(rc == 0);
#else
    rc = bletest_hci_le_set_adv_enable(1);
    assert(rc == 0);
#endif
#endif

    bletest_timer_cb(NULL);

    while (1) {
        os_eventq_run(&g_bletest_evq);
    }
}

/**
 * main
 *
 * The main task for the project. This function initializes the packages,
 * then starts serving events from default event queue.
 *
 * @return int NOTE: this function should never return!
 */
int
main(void)
{
    int rc;

    /* Initialize OS */
    sysinit();

    /* Dummy device address */
#if BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER
    g_dev_addr[0] = 0x00;
    g_dev_addr[1] = 0x00;
    g_dev_addr[2] = 0x00;
    g_dev_addr[3] = 0x88;
    g_dev_addr[4] = 0x88;
    g_dev_addr[5] = 0x08;

    g_bletest_cur_peer_addr[0] = 0x00;
    g_bletest_cur_peer_addr[1] = 0x00;
    g_bletest_cur_peer_addr[2] = 0x00;
    g_bletest_cur_peer_addr[3] = 0x99;
    g_bletest_cur_peer_addr[4] = 0x99;
    g_bletest_cur_peer_addr[5] = 0x09;
#else
    g_dev_addr[0] = 0x00;
    g_dev_addr[1] = 0x00;
    g_dev_addr[2] = 0x00;
    g_dev_addr[3] = 0x99;
    g_dev_addr[4] = 0x99;
    g_dev_addr[5] = 0x09;

    g_bletest_cur_peer_addr[0] = 0x00;
    g_bletest_cur_peer_addr[1] = 0x00;
    g_bletest_cur_peer_addr[2] = 0x00;
    g_bletest_cur_peer_addr[3] = 0x88;
    g_bletest_cur_peer_addr[4] = 0x88;
    g_bletest_cur_peer_addr[5] = 0x08;
#endif

    /* Set the led pin as an output */
    g_led_pin = LED_BLINK_PIN;
    hal_gpio_init_out(g_led_pin, 1);

    /* Initialize eventq for bletest task */
    os_eventq_init(&g_bletest_evq);

    rc = os_task_init(&bletest_task, "bletest", bletest_task_handler, NULL,
                      BLETEST_TASK_PRIO, OS_WAIT_FOREVER, bletest_stack,
                      BLETEST_STACK_SIZE);
    assert(rc == 0);

    while (1) {
        os_eventq_run(os_eventq_dflt_get());
    }
    /* Never returns */

    /* os start should never return. If it does, this should be an error */
    assert(0);

    return rc;
}
