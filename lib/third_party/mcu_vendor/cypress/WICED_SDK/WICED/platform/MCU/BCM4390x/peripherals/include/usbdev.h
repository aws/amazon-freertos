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
 * USB device driver API
 *
 * $Id: usbdev.h 431472 2013-10-23 18:58:59Z rsomu $
 */

#ifndef _usbdev_h_
#define _usbdev_h_

#include <typedefs.h>
#include <bcmutils.h>
#include <usbstd.h>
#include <sbchipc.h>
#include <sbhnddma.h>
#include <hndsoc.h>
#include <hnddma.h>
#include <hndpmu.h>
#include <osl.h>
#include <sbusbd.h>
#include <bcmendian.h>




/* this file is shared by rte/wl and rte/et and all tunables need stubs */
#ifndef USBCTL_RXBUFS
#define USBCTL_RXBUFS           2 /* Number of Control Endpoint buffers */
#elif (USBCTL_RXBUFS == 0)
#undef USBCTL_RXBUFS
#define USBCTL_RXBUFS           2 /* Number of Control Endpoint buffers */
#endif
#ifndef USBBULK_RXBUFS
#define USBBULK_RXBUFS          1  /* Number of Bulk Endpoint Rx Buffers */ /* Caution! MUST use only one rx buf for Wiced usb device driver architecture!!! */
#endif
#ifndef USBBULK_RXBUF_GIANT
#define USBBULK_RXBUF_GIANT     0   /* use large rx buffer, 0: PKTBUFSZ, 1: large one */
#endif

#ifndef USB_NTXD
#define USB_NTXD                4 //16     /* Length of Tx Packet Queue */ /* Fine tune for Wiced usb device driver */
#endif
#ifndef USB_NRXD
#define USB_NRXD                8 //4 //256        /* Length of Rx Packet Queue */ /* Fine tune for Wiced usb device driver */
#endif

#ifndef USBCTLBUFSZ
#if defined(DONGLEBUILD)
#define USBCTLBUFSZ             PKTBUFSZ
#else
#define USBCTLBUFSZ             256 //128     /* Enlarge DMA rxbufsize for Control Endpoint */
#endif /* defined(DONGLEBUILD) */
#endif /* USBCTLBUFSZ */

#ifndef USB_RX_MAX_VALID_PKTSZ
#define USB_RX_MAX_VALID_PKTSZ  7000
#endif /* USB_RX_MAX_VALID_PKTSZ */
#define USB_RXOFFSET (sizeof(usbdev_sb_rxh_t))

#define LPM_DS_THRESH   0xa /* 800 usec HIRD */
#define DEF_USBINTMASK (I_SETUP_MASK | I_DATAERR_MASK | I_LPM_SLEEP | I_SUS_RES | \
        I_RESET | I_CFG | I_DMA_MASK | I_TXDONE_MASK)

/* SW WAR : Deep Sleep is disabled */
#define DEF_LPMCONTROL (LC_DOZE_ENAB)
#define LPM_INT_THRESH 0xf

/* reserve one to prevent ch_tx() from dropping pkt and kick of flow control */
#define USB_DESCRIPTOR_FC_RESERVE   1

#define USB_TXQ_LEN (2*USB_NTXD)    /* sw tx queue to cushion the dma ring */
#define USB_RXQ_LEN (USB_NRXD)  /* sw rx queue to cushion the dma ring */

/* USB flow control thresholds in ch_sendnext() */
#ifndef USB_TXQ_DATAHIWAT
#define USB_TXQ_DATAHIWAT (USB_NTXD - USB_DESCRIPTOR_FC_RESERVE)
#endif
#ifndef USB_TXQ_DATALOWAT
#define USB_TXQ_DATALOWAT (USB_NTXD >> 1)
#endif

#define PKTTAILROOM(osh, lb)        ((((struct lbuf *)(lb))->end)-(((struct lbuf *)(lb))->tail))
#define PKTLINK(lb)                 host_buffer_get_queue_next( (wiced_buffer_t) lb )
#define PKTSETLINK(lb, x)           host_buffer_set_queue_next( (wiced_buffer_t) lb, (wiced_buffer_t) x)

#define USB_RXBND               (4)

#define MALLOCZ_PERSIST MALLOC /* Caution!!! Need to do additional bzero() outside!!! */
#define MFREE_PERSIST MFREE

#define OSL_PKTTAG_SZ 32

struct lbuf {
    struct lbuf    *next;        /* next lbuf in a chain of lbufs forming one packet */
    struct lbuf    *link;        /* first lbuf of next packet in a list of packets */
    uchar        *head;        /* fixed start of buffer */
    uchar        *end;        /* fixed end of buffer */
    uchar        *data;        /* variable start of data */
    uchar        *tail;            /* end of data */
    uint16        len;        /* nbytes of data */
    uint16        flags;        /* private flags; don't touch */
    uint16        dmapad;        /* padding to be added for tx dma */
    uint8        dataOff;    /* offset to beginning of data in 4-byte words */
    uint8        refcnt;        /* external references to this lbuf */
    void        *pool;        /* BCMPKTPOOL */
    uint32        pkttag[(OSL_PKTTAG_SZ + 3) / 4];  /* 4-byte-aligned packet tag area */
};

#define    LBUFSZ        ((int)sizeof(struct lbuf))

/* Total maximum packet buffer size including lbuf header */
//#define MAXPKTBUFSZ         1920        /* enough to fit a 1500 MTU plus overhead */

//#define PKTBUFSZ            (MAXPKTBUFSZ - LBUFSZ) //#USBDevice#: Wiced usb device driver using wiced packet buffer
#define PKTBUFSZ            1500 //#USBDevice#: TODO: Should we use WICED_LINK_MTU more proper???


/* Timing */
typedef void (*to_fun_t)(void *arg);

typedef struct _ctimeout {
    struct _ctimeout *next;
    uint32 ms;
    to_fun_t fun;
    void *arg;
    bool expired;
} ctimeout_t;

typedef struct hndrte_timer
{
    uint32    *context;        /* first field so address of context is timer struct ptr */
    void    *data;
    void    (*mainfn)(struct hndrte_timer *);
    void    (*auxfn)(void *context);
    ctimeout_t t;
    int    interval;
    int    set;
    int    periodic;
    bool    _freedone;
} hndrte_timer_t, hndrte_task_t;

typedef hndrte_timer_t dngl_timer_t;
typedef hndrte_task_t dngl_task_t;

/* indexes into tunables array */
#define CTL_RXBUFS          0
#define BULK_RXBUFS         1
#define BULK_RXBUF_GIANT    2
#define NTXD                3
#define NRXD                4
#define CTLBUFSZ            5
#define TXQ_LEN             6
#define RXQ_LEN             7
#define DATAHIWAT           8
#define DATALOWAT           9
#define RXBND               10
#define RXBURSTLEN          11
#define TXBURSTLEN          12
#define MAXTUNABLE          20

struct usbdev_sb {
    void *drv;          /* Driver handle */
    uint id;            /* USB device core ID */
    uint rev;           /* USB device core revision */
    osl_t *osh;         /* os (Driver PCI) handle */
    usbdev_sb_regs_t *regs;     /* USB registers */
    si_t *sih;          /* SiliconBackplane handle */
    struct dngl_bus *bus;       /* high level USB bus logic handle */
    uint32 usbintmask;      /* Current USB interrupt mask */
    uint32 intstatus;       /* cached interrupt status bits */
    uint32 fatal;           /* the set of fatal DMA errors */
    hnddma_t *di[DMA_MAX + 1];  /* DMA engine handles + 1 for Setup Data, rev 3 */
    struct spktq txq[DMA_MAX];  /* Transmit packet queue (single priority queues) */
    uint *txavail[DMA_MAX];     /* Pointer to DMA txavail variable */
    uint rxoffset[DMA_MAX];     /* rxoffset in unit of byte, where data starts */
    uint rxbufsize[DMA_MAX];    /* size in [bytes] as specified during dma_attach */

    struct spktq rxq[DMA_MAX];  /* Receive packet queue (single priority queues) */
    bool flowctl_rx[DMA_MAX];   /* if true: no rx buffer refilling */
    bool flowctl_tx[DMA_MAX];   /* flow control upper layer for tx */
    int txpktpend[DMA_MAX];     /* DMA pending pkt count */
    int txpktpend_bytes[DMA_MAX];   /* DMA pending pkt bytes */
    uint flowctl_tx_cnt[DMA_MAX];   /* tx flow control counters */
    uint flowctl_rx_cnt[DMA_MAX];   /* rx flow control counters */

    uint32 dmaintmask[DMA_MAX]; /* SW notion of HW intmask for each DMA engine */
    struct {
        uint8 tag;      /* expected tag of control OUT data frame */
        uint32 len;     /* expected length of control OUT data frame */
    } ctrloutexpected[DMA_MAX];
    bool suspended;         /* Device is USB suspended */
    bool up;
    bool needtxaligned;     /* TRUE = core does not handle unaligned tx buffers */
    bool setupdma;          /* TRUE = core supports 6th DMA engine for Setup Data */
    bool fsmode;            /* TRUE = only advertise Full Speed mode */
    bool dma64cap;          /* TRUE = core supports dma64 */
    bool bulktimeractive;   /* TRUE = bulk ep rxfill timer is active */
    uint32 bulkrxfillct;    /* number of bufs posted to bulk ep */
    uint32 bulkoutdma;      /* bulk ep dma index */
    dngl_timer_t *bulkrxfill;   /* bulk rxfill timer struct */
    uint dma_txrecompose_cnt;   /* counter for tx recompose */
    uint dma_rxrecompose_cnt;   /* counter for rx recompose */

    int pendpkt;
    int pendbyte;
    int stuckcnt;
    uint32 tunables[MAXTUNABLE]; /* array of tunables */
    bool hsic;              /* 1=device operating in HSIC mode; 0=USB mode */
    bool disconnect;        /* 1=device re-enumerates at init time */
    bool lpmsleep;          /* Device is in LPM Sleep or Resume */
    uint32 lpmminmask;      /* Min mask modified and restored during LPM Sleep/Resume */
    bool msgtrace;          /* 1=firmware was built with msg trace enabled */
    dngl_timer_t *resume_timer; /* timer to immediately resume the device */
    bool disconnected;      /* 1=device presently disconnected from host */
    bool signaledwakeup;    /* 1=device signaled Remote Wake-up */
    uint32 hsicphyctrl1;
    uint32 hsicphyctrl2;
    uint32 usbdevctrl;
    uint32 usbflags;
    bool got_hsicphyctrl1;
    bool got_hsicphyctrl2;
    bool got_usbdevctrl;
    bool got_utmi_ctl;
    uint32 utmi_ctl;
#ifdef BCMPKTPOOL
    pktpool_t *pktpool;
#endif
    bool logtrace;          /* 1=firmware was built with log trace enabled */
    uint32 resched_status[DMA_MAX]; /* reschedule DMA int status */

    uint32 dpc_suspend_cnt;   /* counter for continuous suspend interrupt in dpc */
    uint32 drv_ctrl_flags;    /* Driver control flags */
};

#define CH_CTRL_HW_ZLP_WAR                  0x0001          /* HW ZPL workaround enabled */

#define usbdev_ch_t	struct usbdev_sb

/* Device operations */
extern struct dngl_bus * usbdev_attach(osl_t *osh, void *ch, si_t *sih);
extern void usbdev_detach(struct dngl_bus *bus);
extern uint usbdev_mps(struct dngl_bus *bus);
extern void *usbdev_setup(struct dngl_bus *bus, int ep, void *p, int *errp, int *dir);
extern uint usbdev_setcfg(struct dngl_bus *bus, int n);
extern int usbdev_getcfg(struct dngl_bus *bus);
extern void usbdev_rx(struct dngl_bus *bus, int ep, void *p);
extern void usbdev_txstop(struct dngl_bus *bus, int ep);
extern void usbdev_txstart(struct dngl_bus *bus, int ep);
extern void usbdev_reset(struct dngl_bus *bus);
extern void usbdev_sof(struct dngl_bus *bus);
extern void usbdev_suspend(struct dngl_bus *bus);
extern void usbdev_resume(struct dngl_bus *bus);
extern void usbdev_speed(struct dngl_bus *bus, int high);
/* return dngl handle */
extern void *usbdev_dngl(struct dngl_bus *bus);
extern bool usbdev_oobresume(struct dngl_bus *bus, bool active);
#ifdef OOB_WAKE
extern bool usbdev_oob_connected(struct dngl_bus *bus);
#else
#define usbdev_oob_connected(bus) 1
#endif

/* Chip operations */
#ifdef USB_XDCI
extern void usbdev_attach_xdci(struct dngl_bus *bus);
extern void usbdev_detach_xdci(struct dngl_bus *bus);
extern bool xdci_match(uint vendor, uint device);
extern int xdci_usb30(void *ch);
extern int xdci_tx(void *ch, int ep_index, void *p);
extern int xdci_resume(void *ch);
extern int xdci2wlan_isr(void *ch);
extern int xdci2wlan_irq_on(void *ch);
extern int xdci2wlan_irq_off(void *ch);
extern int xdci_dpc(void *ch);
extern  void xdci_ep_stall(void *ch, int ep_index);
extern void xdci_ep_detach(void *ch, int ep_index);
extern uint32 xdci_ep_attach(void *ch, const usb_endpoint_descriptor_t *endpoint,
    const usb_endpoint_companion_descriptor_t *sscmp,
    int config, int interface, int alternate);
extern uint32
BCMATTACHFN(xdci_init)(void *ch, bool disconnect, bool connect_delay);
extern void *
BCMATTACHFN(xdci_attach)(void *drv, uint vendor, uint device, osl_t *osh, void *regs, uint bus);
extern void
BCMATTACHFN(xdci_detach)(void *ch, bool disable);
extern si_t *xdci_get_sih(void *ch);
extern void xdci_rxflowcontrol(void *ch, int ep, bool state);
extern  void xdci_indicate_start(void *ch, uint32 start, uint32 no_dis);
extern int xdci_indicate_speed(void *ch, uint32 speed);
#endif /* USB_XDCI */

extern bool ch_match(uint vendor, uint device);
extern struct usbdev_sb * ch_attach(void *drv, uint vendor, uint device, osl_t *osh,
                                    void *regs, uint bus);
extern void ch_detach(struct usbdev_sb *ch, bool disable);
extern void ch_intrsupd(struct usbdev_sb *ch);
extern void ch_intrsoff(struct usbdev_sb *ch);
extern void ch_intrson(struct usbdev_sb *ch);
extern uint32 ch_intstatus(struct usbdev_sb *ch);
extern void ch_init(struct usbdev_sb *ch, bool disconnect);
extern void ch_dumpregs(struct usbdev_sb *ch, struct bcmstrbuf *b);
extern void ch_rxflowcontrol(struct usbdev_sb *ch, int ep, bool state);
extern bool ch_dispatch(struct usbdev_sb *ch);
extern bool ch_dpc(struct usbdev_sb *ch);
extern int ch_tx(struct usbdev_sb *ch, int ep, void *p);
extern void ch_tx_resp(struct usbdev_sb *ch, int ep, void *p);
extern uint ep_attach(struct usbdev_sb *ch, const usb_endpoint_descriptor_t *endpoint,
                      int config, int interface, int alternate);
extern void ep_detach(struct usbdev_sb *ch, int ep);
extern int ch_loopback(struct usbdev_sb *ch, char *buf, uint count);
extern int ch_usb20(struct usbdev_sb *ch);
extern int ch_resume(struct usbdev_sb *ch);
extern void ch_pr46794WAR(struct usbdev_sb *ch);
/* return dngl handle */
extern void *ch_dngl(struct usbdev_sb *ch);
struct dngl_bus *ch_bus(struct usbdev_sb *ch);
/* return sih handle */
extern si_t * ch_get_sih(struct usbdev_sb *ch);
extern bool ch_hsic(struct usbdev_sb *ch);
#ifdef BCMUSB_NODISCONNECT
extern bool ch_hispeed(struct usbdev_sb *ch);
#endif
extern void ch_disconnect(struct usbdev_sb *ch);
extern void ch_devready(struct usbdev_sb *ch, uint32 delay);
extern void ch_leave_suspend_wrapper(struct usbdev_sb *ch);
extern void usbdev_devready(struct dngl_bus *bus, bool active, bool called_from_trap);
extern bool ch_txpending(struct usbdev_sb *ch);
#ifdef BCMDBG
extern void do_usbdevdump_cmd(uint32 arg, uint argc, char *argv[]);
#endif

extern void ch_mdio_wreg(struct usbdev_sb *ch, uint16 addr, uint16 data);
extern uint16 ch_mdio_rreg(struct usbdev_sb *ch, uint16 addr);
extern int process_lpm_settings(struct usbdev_sb *ch);
extern void enter_lpm_sleep(struct usbdev_sb *ch);
extern void leave_lpm_sleep(struct usbdev_sb *ch);

extern void ch_setcfg(struct usbdev_sb *ch, int n);
extern void ch_sendnext(struct usbdev_sb *ch, int i);
extern void ch_sendup(struct usbdev_sb *ch, int ep);
extern void ch_setup(struct usbdev_sb *ch, int i);
extern int ch_devreq(struct usbdev_sb *ch);
extern void ch_rxfill(struct usbdev_sb *ch, int dma);

extern void ch_waitfor_ht(struct usbdev_sb *ch, bool request_ht, const char *fn);
#ifdef HARD_DISCONNECT
extern uint32 ch_reset(struct usbdev_sb *ch);
#else
extern uint32 BCMATTACHFN(ch_reset)(struct usbdev_sb *ch);
#endif
extern int  ch_dma(struct usbdev_sb *ch, int i);
extern void ch_dma_dotxstatus(struct usbdev_sb *ch, int queue);
extern bool ch_dma_recv(struct usbdev_sb *ch, int queue);
extern int  ch_dmaerrors(struct usbdev_sb *ch, uint32 dmaintstatus, int i);
extern void ch_dma_dataerr(struct usbdev_sb *ch, int ep);
extern void ch_txrecompose(struct usbdev_sb *ch, int ep, int i, uint32 offset, bool keepfirst);
extern void ch_rxrecompose(struct usbdev_sb *ch, int ep, int i, uint32 offset);
extern void ch_rx_q_and_send(struct usbdev_sb *ch, int ep, int i, void *p, uint16 flags);
extern void ch_enter_suspend(struct usbdev_sb *ch);
extern void ch_leave_suspend(struct usbdev_sb *ch);

extern void *ch_dma_rx_giantframe(struct usbdev_sb *ch, hnddma_t *di, void *p, uint16 resid, int16 rxbufsize);
extern void *ch_retxprep(struct usbdev_sb *ch, void *p, uint32 offset);


/* Bus API operations */
void usbdev_bus_softreset(struct dngl_bus *bus);
int usbdev_bus_binddev(void *bus, void *dev);
int usbdev_bus_unbinddev(void *bus, void *dev);
#ifdef BCMUSBDEV_BMAC
int usbdev_bus_tx(struct dngl_bus *bus, void *pi, uint32 ep_index);
#else
int usbdev_bus_tx(struct dngl_bus *bus, void *p);
#endif /* BCMUSBDEV_BMAC */

#ifdef BCMUSBDEV_COMPOSITE
void usbd_stall_ep(struct dngl_bus *bus, int ep);
void ch_stall_ep(struct usbdev_sb *ch, int8 hw_ep);
#endif /* BCMUSBDEV_COMPOSITE */

void usbdev_bus_sendctl(struct dngl_bus *bus, void *p);
void usbdev_bus_rxflowcontrol(struct dngl_bus *bus, bool state, int prio);
uint32 usbdev_bus_iovar(struct dngl_bus *usbdev, char *buf,
                        uint32 inlen, uint32 *outlen, bool set);
void usbdev_bus_resume(struct dngl_bus *bus);
void usbdev_bus_pr46794WAR(struct dngl_bus *bus);

#ifdef BCMDBG
void usbdev_bus_dumpregs(void);
void usbdev_bus_loopback(void);
void usbdev_bus_xmit(int len, int clen, bool ctl);
uint usbdev_bus_msgbits(uint *newbits);
#endif

/* defines for host handshake gpios taken from nvram */
#define USBGPIO_INVALID     0xFF
#define USBGPIO_GPIO_MASK   0x1F
#define USBGPIO_DEFAULTPULL_BIT 0x40
#define USBGPIO_POLARITYBIT_SHIFT 7

/* General USB Flags (to save OTP overhead) */
#define USBFL_HOSTRDY_TO_M      BITFIELD_MASK(8)
#define USBFL_HOSTRDY_TO_S      0
#define USBFL_HOSTRDY_TO_INFINITE   0xff        /* Wait 0-254 seconds or 0xff infinite */
#define USBFL_HOSTRDY_GPIO_M        BITFIELD_MASK(3)
#define USBFL_HOSTRDY_GPIO_S        8
#define USBFL_HOSTRDY_GPIOPOL_M BITFIELD_MASK(1)
#define USBFL_HOSTRDY_GPIOPOL_S 11
#define USBFL_HOSTRDY_WAIT_M        BITFIELD_MASK(1)
#define USBFL_HOSTRDY_WAIT_S        12
#define USBFL_PULLENABLE_M      BITFIELD_MASK(1)
#define USBFL_PULLENABLE_S      13

#define USB_GLITCHFILTER_CNT_MAX        (10)

#define USB_HOSTRDY_WAIT_ENAB(flags)    (GFIELD((flags), USBFL_HOSTRDY_WAIT))
#define USB_HOSTRDY_GPULL_ENAB(flags)   (GFIELD((flags), USBFL_HOSTRDY_GPIOPULL))
#define USB_PULLENABLE_ENAB(flags)  (GFIELD((flags), USBFL_PULLENABLE))

#ifdef WL_WOWL_MEDIA
extern void ch_wowldown(struct usbdev_sb *ch);
#endif

#ifdef BCMUSBDEV_COMPOSITE
/* USB-BT Interface/Endpoint control
 * bits 2:0     - Number of BT interfaces in addition to default WLAN
 * bit  3       - disable WLAN intr ep
 * bits 5:4     - additional WLAN bi eps
 * bits 7:6     - additional WLAN bo eps
 * bit  8   - enable IAD
 * bits 11:9    - SCO/ISO ep mps
 * bits 13:12   - Device Class: BT, WLAN
 * bit 14       - WLAN at interface 0
 * bits 15  - Reserved
 */
#define USB_IF_NUM_MASK         0x07
#define USB_WLANIF_INTR_EP_MASK     0x08
#define USB_WLANIF_INTR_EP_SHIFT    3
#define USB_BI_EP_MASK          0x30
#define USB_BI_EP_SHIFT         4
#define USB_B0_EP_MASK          0xc0
#define USB_B0_EP_SHIFT         6
#define USB_IAD_MASK            0x100
#define USB_ISO_MPS_MASK        0xe00
#define USB_ISO_MPS_SHIFT       9
#define USB_DEVCLASS_MASK       0x3000
#define USB_DEVCLASS_SHIFT      12
#define USB_DEVCLASS_BT         0x01
#define USB_DEVCLASS_WLAN       0x02
#define USB_WL_INTF_MASK        0x4000
#define USB_WL_INTF_SHIFT       14

extern void ep_config_reset(struct usbdev_sb *ch);
extern void ep_config(struct usbdev_sb *ch, const usb_endpoint_descriptor_t *endpoint,
    int config, int interface, int alternate);
#endif /* BCMUSBDEV_COMPOSITE */

#ifndef BCMUSBDEV_COMPOSITE
#define VUSBD_ENAB(bus) (0)
#else
#if defined(BCMROMBUILD) || defined(BCM_VUSBD)
#define VUSBD_ENAB(bus) (bus->vusbd_enab)
#else
#define VUSBD_ENAB(bus) (0)
#endif
#endif /* BCM_VUSB */

#if defined(BCM_BOOTLOADER) && defined(BCMTCAM)
void tcam_stub_pre(void);
void tcam_stub_post(void);
#endif /* BCM_BOOTLOADER && BCMTCAM */

/* Special printf function replacement used in ISR */
extern int usbdev_isr_debug_printf(const char *format, ...);



#endif  /* _usbdev_h_ */
