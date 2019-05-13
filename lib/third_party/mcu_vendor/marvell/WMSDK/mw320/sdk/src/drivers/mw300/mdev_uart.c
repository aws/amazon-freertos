/** @file mdev_uart.c
 *
 *  @brief This file provides  mdev driver for UART
 *
 *  (C) Copyright 2014-2018 Marvell International Ltd. All Rights Reserved.
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell
 *  International Ltd or its suppliers and licensors. The Material contains
 *  trade secrets and proprietary and confidential information of Marvell or its
 *  suppliers and licensors. The Material is protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Material may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Marvell's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */


/*
 * mdev_uart.c: mdev driver for UART
 */

#include <board.h>
#include <wmtypes.h>
#include <wmstdio.h>
#include <mdev_uart.h>
#include <wm_os.h>
#include <pwrmgr.h>
#ifdef USE_UART_DMA_SUPPORT
#include <mdev_dma.h>
#endif /* USE_UART_DMA_SUPPORT */
#include <wmassert.h>
#include <system-work-queue.h>

#define NUM_UART_PORTS 3
#define UART_IRQn_BASE UART0_IRQn
#define UART_INT_BASE	INT_UART0
#define UART_FIFO_DEPTH 16

#define SEMAPHORE_NAME_LEN 25

/* #define CONFIG_UART_FUNC_DEBUG */
#ifdef CONFIG_UART_FUNC_DEBUG
#define uart_entry wmlog_entry
#define uart_exit wmlog_exit
#else
#define uart_entry(...)
#define uart_exit(...)
#endif

/* Device name */
static const char *mdev_uart_name[NUM_UART_PORTS] = {
	"MDEV_UART0",
	"MDEV_UART1",
#if (NUM_UART_PORTS > 2)
	 "MDEV_UART2",
#if (NUM_UART_PORTS > 3)
	"MDEV_UART3",
#endif
#endif
};

/* The device objects */
static mdev_t mdev_uart[NUM_UART_PORTS];

typedef struct uartdrv_ringbuf {
	int wr_idx;		/* write pointer */
	int rd_idx;		/* read pointer */
	int buf_size;		/* number of bytes in buffer *buf_p */
	int dma_block_size;     /* DMA block size */
	char *buf_p;		/* pointer to buffer storage */
} uartdrv_ringbuf_t;

typedef struct uartdev_priv_data {
	uartdrv_ringbuf_t rx_ringbuf;
	bool mode;
	bool dma;
	uint32_t num_dma_unread;	/* Number of bytes unread in fifo when
					 * using DMA on	a particular port */
	mdev_t *dma_rd_handle;		/* Stores dma read handle */
	bool is_read_blocking;
	uint32_t baudrate;
	uint32_t parity;
	uint32_t stopbits;
	flow_control_t flowctl;
	unsigned long tx_tout;
	unsigned long rx_tout;
} uartdev_data_t;

DMA_CFG_Type read_dma[NUM_UART_PORTS];

/* Device private data */
static uartdev_data_t uartdev_data[NUM_UART_PORTS];
static os_mutex_t uart_mutex[NUM_UART_PORTS];
static void (*uart_8bit_cb_p[NUM_UART_PORTS]) ();
static void (*uart_9bit_cb_p[NUM_UART_PORTS]) ();
static os_semaphore_t uart_block_read_sem[NUM_UART_PORTS];
#ifdef USE_UART_DMA_SUPPORT
static int uart_dma_read_start(UART_ID_Type id, uint8_t *buf, uint32_t nbytes);
#endif /* USE_UART_DMA_SUPPORT */

#define SET_RX_BUF_SIZE(uart_data_p, size)	\
	 uart_data_p->rx_ringbuf.buf_size = size
#define GET_RX_BUF_SIZE(uart_data_p) (uart_data_p->rx_ringbuf.buf_size)

#define SET_DMA_BLK_SIZE(uart_data_p, size)	\
	 uart_data_p->rx_ringbuf.dma_block_size = size
#define GET_DMA_BLK_SIZE(uart_data_p) (uart_data_p->rx_ringbuf.dma_block_size)

#define DEFINE_UART_CB(id, mode)	\
					static void \
					uart ## id ## _ ## mode ## bit_cb() {\
					uart_ ## mode ## bit_cb(id); }
#define GET_UART_CB(id, mode)		uart ## id ## _ ## mode ## bit_cb



void uart_ps_cb(power_save_event_t event, void *data)
{
	mdev_t *mdev_p = (mdev_t *) data;
	unsigned int uart_no = mdev_p->port_id;
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	switch (event) {
	case ACTION_ENTER_PM2:

	case ACTION_ENTER_PM3: {
		while (UART_GetLineStatus(uart_no, UART_LINESTATUS_TEMT)
			!= SET)
				;
		break;
			}

	case ACTION_EXIT_PM3:{
			UART_CFG_Type uartcfg;
			UART_FifoCfg_Type fifocfg;
			/* Initialize the hardware */
			uartcfg.baudRate = uart_data_p->baudrate;
			uartcfg.dataBits = UART_DATABITS_8;

			int status = uart_data_p->stopbits ? ENABLE : DISABLE;
			UART_SetStopBits(uart_no, status);

			uartcfg.parity = uart_data_p->parity;
			fifocfg.fifoEnable = ENABLE;

			fifocfg.rxFifoLevel = UART_RXFIFO_BYTE_1;
			fifocfg.txFifoLevel = UART_TXFIFO_HALF_EMPTY;
			fifocfg.fifoDmaEnable = DISABLE;
			fifocfg.peripheralBusType = UART_PERIPHERAL_BITS_8;
			if (uart_data_p->mode == UART_9BIT) {
				uartcfg.parity = UART_PARITY_EVEN;
				fifocfg.fifoEnable = DISABLE;
			}
			/* TODO: Verify autoFlowControl functionality after
			 * exit from PM3
			 */
			if (uart_data_p->flowctl == FLOW_CONTROL_HW)
				fifocfg.autoFlowControl = ENABLE;
			else
				fifocfg.autoFlowControl = DISABLE;

			board_uart_pin_config(uart_no);

			switch (uart_no) {
			case UART0_ID:
				CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_FAST);
				CLK_ModuleClkEnable(CLK_UART0);
				break;
			case UART1_ID:
				CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
				CLK_ModuleClkEnable(CLK_UART1);
				break;
			case UART2_ID:
				/* Not implemented */
				break;
			}
			UART_Enable(uart_no);
			UART_Init(uart_no, &uartcfg);
			UART_FifoConfig(uart_no, &fifocfg);
			UART_IntMask(uart_no, UART_INT_RDA, UNMASK);
		}
		break;
	default:
		break;
	}
}

/* ring buffer apis */
static int get_buf_rx_size(mdev_t *dev)
{
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) dev->private_data;
	return uart_data_p->rx_ringbuf.buf_size;
}

/* return value 0 => buffer is empty
 * return value 1 => buffer is full
 * return value x => x bytes free in rx buffer
 */
static int get_free_rx_buf_size(mdev_t *dev)
{
	int wr_ptr, rd_ptr, rx_ringbuf_size, free_space;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	wr_ptr = uart_data_p->rx_ringbuf.wr_idx;
	rd_ptr = uart_data_p->rx_ringbuf.rd_idx;
	rx_ringbuf_size = get_buf_rx_size(dev);
	free_space = ((rd_ptr - wr_ptr) + rx_ringbuf_size) % rx_ringbuf_size;
	/* In UART DMA transfer, DMA controller increments the destination
	 * address internally. To distinguish empty and full condition of the
	 * buffer, we need to check the actual received bytes.
	 */
	if (free_space == 0 && uart_data_p->num_dma_unread)
		return 1;
	return free_space;
}

static void deassert_cts(mdev_t *dev)
{
	uint8_t xoff = XOFF;

	if (get_free_rx_buf_size(dev) == UARTDRV_THRESHOLD_RX_BUF) {
		uart_drv_write(dev, (uint8_t *)(&xoff) , 1);
	}
}

static void assert_cts(mdev_t *dev)
{
	uint8_t xon = XON;

	if (get_free_rx_buf_size(dev) ==
	    (get_buf_rx_size(dev) - UARTDRV_THRESHOLD_RX_BUF)) {
		uart_drv_write(dev, (uint8_t *)(&xon), 1);
	}
}

static char read_rx_buf(mdev_t *dev)
{
	char ret;
	int idx;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	if (uart_data_p->flowctl == FLOW_CONTROL_SW) {
		assert_cts(dev);
	}
	idx = uart_data_p->rx_ringbuf.rd_idx;
	ret = uart_data_p->rx_ringbuf.buf_p[idx];
	uart_data_p->rx_ringbuf.rd_idx = (uart_data_p->rx_ringbuf.rd_idx + 1)
	    % get_buf_rx_size(dev);
	if (uart_data_p->dma == UART_DMA_ENABLE) {
		uint32_t save = os_enter_critical_section();
		uart_data_p->num_dma_unread--;
		os_exit_critical_section(save);
	}
	return ret;
}

static void write_rx_buf(mdev_t *dev, char wr_char)
{
	int idx;
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	idx = uart_data_p->rx_ringbuf.wr_idx;
	uart_data_p->rx_ringbuf.buf_p[idx] = wr_char;
	uart_data_p->rx_ringbuf.wr_idx = (uart_data_p->rx_ringbuf.wr_idx + 1)
	    % get_buf_rx_size(dev);
	if (uart_data_p->flowctl == FLOW_CONTROL_SW) {
		deassert_cts(dev);
	}
}

#ifdef USE_UART_DMA_SUPPORT
static void incr_rx_buf_ptr(mdev_t *dev, uint32_t size)
{
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	uart_data_p->rx_ringbuf.wr_idx = (uart_data_p->rx_ringbuf.wr_idx + size)
	    % get_buf_rx_size(dev);
}
#endif /* USE_UART_DMA_SUPPORT */

int uart_drv_rx_buf_reset(mdev_t *dev)
{
	if (dev ==  NULL) {
		UART_LOG("Unable to open device\r\n");
		return -WM_FAIL;
	}
	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) dev->private_data;
	uart_data_p->rx_ringbuf.rd_idx = 0;
	uart_data_p->rx_ringbuf.wr_idx = 0;
	return WM_SUCCESS;
}

/* UART callback functions */
static void uart_8bit_cb(int port_num)
{

	char recv_byte;
	mdev_t *dev = &mdev_uart[port_num];
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) dev->private_data;
	if (uart_data_p->is_read_blocking == true &&
	    get_free_rx_buf_size(dev) == 0)
		os_semaphore_put(&uart_block_read_sem[dev->port_id]);

	/* Receive new data */
	while (UART_GetLineStatus(port_num, UART_LINESTATUS_DR) == SET) {
		if (get_free_rx_buf_size(dev) != 1) {
			recv_byte = UART_ReceiveData(port_num);
			write_rx_buf(dev, recv_byte);
		} else {
			/* Rx Buffer is full, discard received data */
			if (uart_data_p->flowctl == FLOW_CONTROL_HW) {
				/* We need to mask the interrupt till the data
				   is read; otherwise we keep getting
				   data-ready interrupt infinitely */
				UART_IntMask(port_num, UART_INT_RDA, MASK);
				return;
			} else {
				UART_ReceiveData(port_num);
			}
		}
	}
}

static void uart_9bit_cb(int port_num)
{
	uint16_t data;
	mdev_t *dev = &mdev_uart[port_num];
	/* Receive new data */
	if (get_free_rx_buf_size(dev) != 1) {
		data = UART_Receive9bits(port_num);
		write_rx_buf(dev, (uint8_t) ((data >> 8) & 0x01));
		write_rx_buf(dev, (uint8_t) (data & 0xFF));
	} else {
		/* Rx Buffer is full, discard received data */
		UART_Receive9bits(port_num);
	}
}

DEFINE_UART_CB(0, 8)
DEFINE_UART_CB(1, 8)
DEFINE_UART_CB(0, 9)
DEFINE_UART_CB(1, 9)
#if (NUM_UART_PORTS > 2)
DEFINE_UART_CB(2, 8)
DEFINE_UART_CB(2, 9)
#if (NUM_UART_PORTS > 3)
DEFINE_UART_CB(3, 8)
DEFINE_UART_CB(3, 9)
#endif
#endif

/* Exposed APIs and helper functions */
int uart_drv_xfer_mode(UART_ID_Type port_id, UART_DMA_MODE dma_mode)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return -WM_FAIL;
	}

	if (!(dma_mode == UART_DMA_ENABLE || dma_mode == UART_DMA_DISABLE)) {
		return -WM_FAIL;
	}
	mdev_p = mdev_get_handle(mdev_uart_name[port_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
			mdev_uart_name[port_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	if ((uart_data_p->flowctl == FLOW_CONTROL_SW) &&
			(dma_mode == UART_DMA_ENABLE)) {
		UART_LOG("Port %d DMA cannot be enabled"
			 " when software flow control is enabled\r\n", port_id);
		return -WM_FAIL;
	}

	uart_data_p->dma = dma_mode;
	return WM_SUCCESS;
}

mdev_t *uart_drv_open(UART_ID_Type port_id, uint32_t baud)
{

	int ret;
	UART_CFG_Type uartcfg;
	UART_FifoCfg_Type fifocfg;
	uartdev_data_t *uart_data_p;
	mdev_t *mdev_p;
	uint32_t buf_size;

	if (port_id < 0 || port_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", port_id);
		return NULL;
	}

	mdev_p = mdev_get_handle(mdev_uart_name[port_id]);

	if (mdev_p == NULL) {
		UART_LOG("Unable to open device %s\r\n",
			 mdev_uart_name[port_id]);
		return NULL;
	}

	ret = os_mutex_get(&uart_mutex[mdev_p->port_id], OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		UART_LOG("failed to get mutex\r\n");
		return NULL;
	}

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	if (uart_data_p->mode == UART_9BIT &&
	    uart_data_p->dma == UART_DMA_ENABLE) {
		UART_LOG("Can't initialize dma with 9bit uart\r\n");
		os_mutex_put(&uart_mutex[mdev_p->port_id]);
		return NULL;
	}

	/* Setup RX ring buffer */
	/* if rx_buffer is not set by the user set it to default size*/
	if (GET_RX_BUF_SIZE(uart_data_p) == 0) {
		SET_RX_BUF_SIZE(uart_data_p, UARTDRV_SIZE_RX_BUF);
	}
	/* init ring buffer */
	if (uart_data_p->dma == true) {
		/*
		 * 1. If DMA_BLK_SIZE equals zero, do not allocate any
		 * rx_ringbuf. (TODO: Not supported yet)
		 * * 2. If DMA_BLK_SIZE equals the size of RX_BUF_SIZE, then
		 * allocate rx_ringbuf of size to DMA_BLK_SIZE.
		 * 3. DMA_BLK_SIZE > RX_BUF_SIZE, returns error.
		 * 4. DMA_BLK_SIZE < RX_BUF_SIZE, allocate rx_ringbuf of size
		 * that equals maximum possible multiple of DMA block size.
		 */
		if (GET_DMA_BLK_SIZE(uart_data_p) == 0) {
			UART_LOG("DMA_BLK_SIZE(0) is not supported\r\n");
			os_mutex_put(&uart_mutex[mdev_p->port_id]);
			return NULL;
		} else if (GET_RX_BUF_SIZE(uart_data_p) <
			GET_DMA_BLK_SIZE(uart_data_p)) {
			UART_LOG("DMA_BLK_SIZE(%d) can not be greater "
				"than RX_BUF_SIZE(%d)\r\n",
				GET_DMA_BLK_SIZE(uart_data_p),
				GET_RX_BUF_SIZE(uart_data_p));
			os_mutex_put(&uart_mutex[mdev_p->port_id]);
			return NULL;
		} else {
			buf_size = GET_RX_BUF_SIZE(uart_data_p) -
				(GET_RX_BUF_SIZE(uart_data_p) %
				 GET_DMA_BLK_SIZE(uart_data_p));
		}
	} else {
		/* Configure Software buffer size for IO mode */
		buf_size = GET_RX_BUF_SIZE(uart_data_p);
	}

	if (buf_size) {
		uart_data_p->rx_ringbuf.buf_p =	os_mem_alloc(buf_size);
		if (uart_data_p->rx_ringbuf.buf_p == NULL) {
			UART_LOG("Could not allocate fifo\r\n");
			os_mutex_put(&uart_mutex[mdev_p->port_id]);
			return NULL;
		}
	}

	/* Set rd_idx & wr_idx for IO and DMA mode */
	uart_data_p->rx_ringbuf.buf_size = buf_size;
	uart_data_p->rx_ringbuf.rd_idx = 0;
	uart_data_p->rx_ringbuf.wr_idx = 0;
	uart_data_p->num_dma_unread = 0;

	/* Initialize the hardware */
	if (baud) {
		uartcfg.baudRate = baud;
		uart_data_p->baudrate = baud;
	} else {
		uartcfg.baudRate = UARTDRV_DEFAULT_BAUDRATE;
		uart_data_p->baudrate = UARTDRV_DEFAULT_BAUDRATE;
	}
	uartcfg.dataBits = UART_DATABITS_8;
	uartcfg.highSpeedUart = ENABLE;
	uartcfg.nrzCodeing = DISABLE;

	int status = uart_data_p->stopbits ? ENABLE : DISABLE;
	UART_SetStopBits(port_id, status);

	if (uart_data_p->mode == UART_9BIT) {
		uartcfg.parity = UART_PARITY_EVEN;
		/* TODO: 9bit can also be implemented using sticky Parity */
		uartcfg.stickyParity = DISABLE;
		fifocfg.fifoEnable = DISABLE;
	} else {
		uartcfg.parity = uart_data_p->parity;
		uartcfg.stickyParity = DISABLE;
		fifocfg.fifoEnable = ENABLE;
	}
	if (uart_data_p->flowctl == FLOW_CONTROL_HW)
		fifocfg.autoFlowControl = ENABLE;
	else
		fifocfg.autoFlowControl = DISABLE;

	if (uart_data_p->dma == UART_DMA_DISABLE) {
		fifocfg.rxFifoLevel = UART_RXFIFO_BYTE_1;
		fifocfg.txFifoLevel = UART_TXFIFO_HALF_EMPTY;
		fifocfg.fifoDmaEnable = DISABLE;
		fifocfg.peripheralBusType = UART_PERIPHERAL_BITS_8;
	} else {
		fifocfg.rxFifoLevel = UART_RXFIFO_BYTE_1;
		fifocfg.txFifoLevel = UART_TXFIFO_HALF_EMPTY;
		fifocfg.fifoDmaEnable = ENABLE;
		fifocfg.peripheralBusType = UART_PERIPHERAL_BITS_8;
	}
	UART_Disable(mdev_p->port_id);
	UART_Enable(mdev_p->port_id);
	UART_Init(mdev_p->port_id, &uartcfg);
	UART_FifoConfig(mdev_p->port_id, &fifocfg);
	if (uart_data_p->flowctl == FLOW_CONTROL_HW) {
		/* As per datasheet, RTS bit should be asserted explicitly to
		 * enable RTS flow control.
		 * By default, only CTS flow control is enabled.
		 */
		UART_RTS_Assert(port_id);
	}
	if (uart_data_p->is_read_blocking == true) {
		char  sem_name[SEMAPHORE_NAME_LEN];
		snprintf(sem_name, sizeof(sem_name), "uart-block-read-sem%d",
			port_id);
		ret = os_semaphore_create(&uart_block_read_sem[mdev_p->port_id],
					  sem_name);
		if (ret) {
			UART_LOG("Failed to initialize semaphore for blocking"
				 " read\n\r");
			return NULL;
		}
		os_semaphore_get(&uart_block_read_sem[mdev_p->port_id],
					OS_WAIT_FOREVER);
	}

	if (uart_data_p->dma == UART_DMA_DISABLE) {
		install_int_callback(UART_INT_BASE + mdev_p->port_id,
				     UART_INT_RDA,
				     (uart_data_p->mode == UART_9BIT) ?
				     uart_9bit_cb_p[mdev_p->port_id] :
				     uart_8bit_cb_p[mdev_p->port_id]);

		NVIC_EnableIRQ(UART_IRQn_BASE + mdev_p->port_id);
		NVIC_SetPriority(UART_IRQn_BASE + mdev_p->port_id, 0xf);
	} else {
#ifdef USE_UART_DMA_SUPPORT
		/* initialize dma driver */
		ret = dma_drv_init();
		if (ret != WM_SUCCESS) {
			UART_LOG("Failed to initialize dma driver\r\n");
			return NULL;
		}
		ret = sys_work_queue_init();
		if (ret != WM_SUCCESS) {
			UART_LOG("Failed to initialize system workqueue\r\n");
			return NULL;
		}
		/* Enable UART to send dma request to DMAC */
		UART_DmaCmd(mdev_p->port_id, ENABLE);
		if (GET_DMA_BLK_SIZE(uart_data_p) != 0) {
			/* To enable asynchronous DMA read mode, configure and
			 * enable UART DMA receive mode. The driver ensures
			 * that, the data is stored into the internal circular
			 * software-buffer. User may read the data using the API
			 * uart_drv_read()
			 */
			ret = uart_dma_read_start(port_id,
				(uint8_t *)uart_data_p->rx_ringbuf.buf_p,
				GET_DMA_BLK_SIZE(uart_data_p));
			if (ret != WM_SUCCESS)
				return NULL;
		} else
			UART_LOG("UART DMA read is not enabled as"
					"GET_DMA_BLK_SIZE() = 0\r\n");
#endif /* USE_UART_DMA_SUPPORT */
	}

	UART_IntMask(mdev_p->port_id, UART_INT_RDA, UNMASK);

	pm_register_cb(ACTION_EXIT_PM3|ACTION_ENTER_PM2|ACTION_ENTER_PM3,
			uart_ps_cb, mdev_p);

	/* Configure the pinmux for uart pins */
	board_uart_pin_config(mdev_p->port_id);

	return mdev_p;
}

#ifdef USE_UART_DMA_SUPPORT
static void uart_dma_config_rx(mdev_t *dev, uint32_t addr, dma_config_t *dmac)
{
	ASSERT((dev != NULL) && (dmac != NULL));
	DMA_CFG_Type *dma = &dmac->dma_cfg;

	switch (dev->port_id) {
	case UART0_ID:
		dma->srcDmaAddr = UART0_BASE;
		dmac->perDmaInter = DMA_PER14_UART0_RX;
		break;
	case UART1_ID:
		dma->srcDmaAddr = UART1_BASE;
		dmac->perDmaInter = DMA_PER16_UART1_RX;
		break;
	}

	dma->destDmaAddr = addr;
	dma->transfType = DMA_PER_TO_MEM;
	dma->burstLength = DMA_ITEM_1;
	dma->srcAddrInc = DMA_ADDR_NOCHANGE;
	dma->destAddrInc = DMA_ADDR_INC;
	dma->transfWidth = DMA_TRANSF_WIDTH_8;
}

/* Work queue job function */
static int uart_dma_wq_job(void *mdev)
{
	mdev_t *dev = (mdev_t *) mdev;
	uartdev_data_t *uart_data_p = (uartdev_data_t *) (dev->private_data);
	dma_config_t dma_c;
	dma_c.dma_cfg.transfLength = GET_DMA_BLK_SIZE(uart_data_p);
	/* Configure the DMA channel with the new target address */
	uart_dma_config_rx(dev, (uint32_t)(uart_data_p->rx_ringbuf.buf_p
				+ uart_data_p->rx_ringbuf.wr_idx), &dma_c);
	/* Start DMA transfer with the modified configuration */
	dma_drv_transfer(uart_data_p->dma_rd_handle, &dma_c);

	return WM_SUCCESS;
}

/* Callback function invoked on DMA block transfer complete. The block size is
 * set using DEFAULT_DMA_BLK_SIZE.
 */
static void uart_dma_read_cb(DMA_Channel_Type i, dma_transfer_status_t status,
		void *mdev)
{
	mdev_t *dev = (mdev_t *) mdev;
	bool put_sem = false;
	uartdev_data_t *uart_data_p = (uartdev_data_t *) (dev->private_data);

	if (uart_data_p->is_read_blocking == true &&
			get_free_rx_buf_size(dev) == 0)
		put_sem = true;

	/* num_dma_unread variable keeps track
	 * of number of data items received, but not read yet */
	uart_data_p->num_dma_unread += GET_DMA_BLK_SIZE(uart_data_p);

	/* increment software buffer pointer
	 */
	incr_rx_buf_ptr(dev, GET_DMA_BLK_SIZE(uart_data_p));

	if (put_sem)
		os_semaphore_put(&uart_block_read_sem[dev->port_id]);

	/* This callback executes in ISR context.
	 * Invoking dma_drv_transfer from ISR context will give panic
	 * as it uses mutex which is prohibited in ISR context.
	 * So we enqueue this job in System Workqueue. */

	/* Work queue handle */
	wq_handle_t sys_wq_handle;
	/* Work Queue job */
	wq_job_t uart_job = {
		.job_func = uart_dma_wq_job,
		.param = mdev,
		.periodic_ms = 0,
		.initial_delay_ms = 0,
	};
	/* Get the system Work queue handle */
	sys_wq_handle = sys_work_queue_get_handle();
	/* Enqueue the task in system workqueue */
	int ret = work_enqueue(sys_wq_handle, &uart_job, NULL);
	if (ret != WM_SUCCESS) {
		UART_LOG("Failed to enqueue in  system workqueue\r\n");
		return;
	}
	return;
}

static int uart_dma_read_start(UART_ID_Type id, uint8_t *buf, uint32_t nbytes)
{
	mdev_t *dev = mdev_get_handle((mdev_uart_name[id]));
	if (dev == NULL) {
		UART_LOG("%s, Verify UART port id\r\n", __func__);
		return -WM_FAIL;
	}

	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	mdev_t *dma_dev = dma_drv_open();
	if (dma_dev == NULL) {
		UART_LOG("%s: dma_drv_open() failed\r\n", __func__);
		return -WM_FAIL;
	}

	/* For Asynchronous DMA read operation; DMA channel handle dma_dev,
	 * that is used for read operation, is stored in the private structure.
	 */
	uart_data_p->dma_rd_handle = dma_dev;

	dma_config_t dma_c;
	/* Fixme check if transfLength = nbytes / GET_DMA_BLK_SIZE */
	dma_c.dma_cfg.transfLength = GET_DMA_BLK_SIZE(uart_data_p);

	/* set DMA callback on DMA block transfer complete.
	 * */
	dma_drv_set_cb(dma_dev, uart_dma_read_cb, dev);

	uart_dma_config_rx(dev, (uint32_t)buf, &dma_c);

	int ret = dma_drv_transfer(dma_dev, &dma_c);
	if (ret != WM_SUCCESS)
		UART_LOG("%s: Failed to initiate DMA read\r\n", __func__);
	return ret;
}
#endif /* USE_UART_DMA_SUPPORT */

void uart_drv_tx_flush(mdev_t *dev)
{
	while (UART_GetLineStatus(dev->port_id, UART_LINESTATUS_TEMT)
	       != SET)
		os_thread_relinquish();
}

uint32_t uart_drv_get_portid(mdev_t *dev)
{
	return dev->port_id;
}

static uint32_t uart_9bit_read(mdev_t *dev, uint8_t *buf, uint32_t num)
{
	uint32_t len = 0;

	uint16_t data, *buf1 = (uint16_t *) buf;
	num = num / sizeof(uint16_t);

	/* Get the desired bytes from the RX ring buffer */
	while (num > 0) {
		if (get_free_rx_buf_size(dev)) {
			data = read_rx_buf(dev);
			*buf1 = (data << 8) | read_rx_buf(dev);
			--num;
			++len;
			++buf1;
		} else
			break;
	}
	return len * sizeof(uint16_t);
}

uint32_t uart_drv_read(mdev_t *dev, uint8_t *buf, uint32_t num)
{
	ASSERT(dev != NULL);

	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;
	int port_num = dev->port_id;

	if (uart_data_p->mode == UART_9BIT) {
		return uart_9bit_read(dev, buf, num);
	}

	if (num > GET_RX_BUF_SIZE(uart_data_p) && uart_data_p->is_read_blocking
			== true) {
		UART_LOG("Read data size(%d) can not be greater than the "
				"software buffer size(%d)\r\n", num,
				GET_RX_BUF_SIZE(uart_data_p));
		return -WM_FAIL;
	}

	uint32_t len = 0;

	/* If UART read is blocking; wait on semaphore till
	 * the data is received and available for reading.
	 */
	if (uart_data_p->is_read_blocking == true)
		if (get_free_rx_buf_size(dev) == 0)
			os_semaphore_get(&uart_block_read_sem[dev->port_id],
					 uart_data_p->rx_tout);
	/* Get the desired bytes from the RX ring buffer */
	while (num > 0) {
		if (get_free_rx_buf_size(dev)) {
			*buf = read_rx_buf(dev);
			--num;
			++len;
			++buf;
		} else
			break;
	}

	if (uart_data_p->flowctl == FLOW_CONTROL_HW)
		UART_IntMask(port_num, UART_INT_RDA, UNMASK);

	return len;
}

static uint32_t uart_9bit_write(mdev_t *dev, const uint8_t *buf, uint32_t num)
{

	uint32_t len = 0;

	const uint16_t *buf1 = (const uint16_t *) buf;
	num = num / sizeof(uint16_t);

	while (num > 0) {
		while (UART_GetLineStatus(dev->port_id,
					  UART_LINESTATUS_TDRQ) != SET)
			os_thread_relinquish();
		UART_Send9bits(dev->port_id, *buf1);
		buf1++;
		num--;
		len++;
	}
	/* Return number of bytes written */
	return len * sizeof(uint16_t);
}

#ifdef USE_UART_DMA_SUPPORT
static void uart_dma_config_tx(mdev_t *dev, uint32_t addr, dma_config_t *dmac)
{
	uart_entry();
	DMA_CFG_Type *dma = &dmac->dma_cfg;
	switch (dev->port_id) {
	case UART0_ID:
		dma->destDmaAddr = UART0_BASE;
		dmac->perDmaInter = DMA_PER15_UART0_TX;
		break;

	case UART1_ID:
		dma->destDmaAddr = UART1_BASE;
		dmac->perDmaInter = DMA_PER17_UART1_TX;
		break;
	}

	dma->srcDmaAddr = addr;
	dma->transfType = DMA_MEM_TO_PER;
	dma->burstLength = DMA_ITEM_8;
	dma->srcAddrInc = DMA_ADDR_INC;
	dma->destAddrInc = DMA_ADDR_NOCHANGE;
	dma->transfWidth = DMA_TRANSF_WIDTH_8;
	uart_exit();
}

static uint32_t uart_drv_dma_write(mdev_t *dev, const uint8_t *buf,
				   uint32_t num)
{
	ASSERT(dev != NULL);

	int data_len = num;
	int ret = -WM_FAIL;
	dma_config_t dma_c;
	dma_c.dma_cfg.transfLength = num < DMA_MAX_CHUNK_SIZE ?
					num : DMA_MAX_CHUNK_SIZE;

	mdev_t *dma_dev = dma_drv_open();
	if (dma_dev == NULL) {
		UART_LOG("dma_drv_open() failed\r\n");
		return -WM_FAIL;
	}

	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	while (data_len) {
		uart_dma_config_tx(dev, (uint32_t)buf, &dma_c);
		ret = dma_drv_transfer(dma_dev, &dma_c);

		if (ret == WM_SUCCESS)
			ret = dma_drv_wait_for_transfer_complete(dma_dev,
				uart_data_p->tx_tout);
		/* Check if dma transfer has failed */
		if (ret != WM_SUCCESS)
			goto dma_stop;

		data_len = data_len - dma_c.dma_cfg.transfLength;
		dma_c.dma_cfg.transfLength = data_len < DMA_MAX_CHUNK_SIZE ?
					data_len : DMA_MAX_CHUNK_SIZE;

	}

dma_stop:
	dma_drv_close(dma_dev);
	return (ret == WM_SUCCESS) ? num : ret;
}
#endif /* USE_UART_DMA_SUPPORT */

static uint32_t uart_drv_pio_write(mdev_t *dev, const uint8_t *buf,
				   uint32_t num)
{
	uint32_t len = 0, i;
	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->mode == UART_9BIT)
		return uart_9bit_write(dev, buf, num);

	while (num > 0) {
		while (UART_GetLineStatus(dev->port_id,
					  UART_LINESTATUS_TDRQ) != SET)
			os_thread_relinquish();

		for (i = 0; i < UART_FIFO_DEPTH && num > 0; i++) {
			if ((*buf == XON || *buf == XOFF || *buf == ESC)
			    && (uart_data_p->flowctl == FLOW_CONTROL_SW)) {
				UART_SendData(dev->port_id, ESC);
			}
			UART_SendData(dev->port_id, *buf);
			buf++;
			num--;
			len++;
		}
	}

	/* Return number of bytes written */
	return len;
}

uint32_t uart_drv_write(mdev_t *dev, const uint8_t *buf, uint32_t num)
{
	uartdev_data_t *uart_data_p = (uartdev_data_t *) dev->private_data;

	if (uart_data_p->dma == UART_DMA_ENABLE)
#ifdef USE_UART_DMA_SUPPORT
		return uart_drv_dma_write(dev, buf, num);
#else
		return -WM_FAIL;
#endif /* USE_UART_DMA_SUPPORT */
	else
		return uart_drv_pio_write(dev, buf, num);
}

int uart_drv_close(mdev_t *mdev_p)
{
	uartdev_data_t *uart_data_p;

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_drv_tx_flush(mdev_p);
	UART_Disable(mdev_p->port_id);

	/* de-register interrupt callbacks */
	NVIC_DisableIRQ(UART_IRQn_BASE + mdev_p->port_id);
	if (uart_data_p->dma == UART_DMA_DISABLE)
		install_int_callback(UART_INT_BASE + mdev_p->port_id,
				     UART_INT_RDA, 0);
	else {
#ifdef USE_UART_DMA_SUPPORT
		/* Disable UART requests to DMAC */
		UART_DmaCmd(mdev_p->port_id , DISABLE);

		/* Disable UART DMA read handle */
		dma_drv_close(uart_data_p->dma_rd_handle);
#endif /* USE_UART_DMA_SUPPORT */
	}

	if (uart_data_p->rx_ringbuf.buf_p) {
		os_mem_free(uart_data_p->rx_ringbuf.buf_p);
		uart_data_p->rx_ringbuf.buf_p = NULL;
		uart_data_p->rx_ringbuf.buf_size = 0;
	}
	if (uart_data_p->is_read_blocking == true) {
		os_semaphore_put(&uart_block_read_sem[mdev_p->port_id]);
		os_semaphore_delete(&uart_block_read_sem[mdev_p->port_id]);
	}

	SET_RX_BUF_SIZE(uart_data_p, 0);
	uart_data_p->parity = UART_PARITY_NONE;
	uart_data_p->stopbits = DISABLE;
	uart_data_p->flowctl = FLOW_CONTROL_NONE;
	/* DMA is default disabled */
	uart_data_p->dma = UART_DMA_DISABLE;
	uart_data_p->dma_rd_handle = NULL;
	uart_data_p->num_dma_unread = 0;
	uart_data_p->is_read_blocking = false;
	/* timeout values are initialised to OS_WAIT_FOREVER
	 */
	uart_data_p->tx_tout = OS_WAIT_FOREVER;
	uart_data_p->rx_tout = OS_WAIT_FOREVER;
	SET_DMA_BLK_SIZE(uart_data_p, DEFAULT_DMA_BLK_SIZE);

	return os_mutex_put(&uart_mutex[mdev_p->port_id]);
}

static void fill_cb_array()
{
	uart_8bit_cb_p[0] = GET_UART_CB(0, 8);
	uart_8bit_cb_p[1] = GET_UART_CB(1, 8);
	uart_9bit_cb_p[0] = GET_UART_CB(0, 9);
	uart_9bit_cb_p[1] = GET_UART_CB(1, 9);
#if (NUM_UART_PORTS > 2)
	uart_8bit_cb_p[2] = GET_UART_CB(2, 8);
	uart_9bit_cb_p[2] = GET_UART_CB(2, 9);
#if (NUM_UART_PORTS > 3)
	uart_8bit_cb_p[3] = GET_UART_CB(3, 8);
	uart_9bit_cb_p[3] = GET_UART_CB(3, 9);
#endif
#endif
}

static void uartdrv_mdev_init(int port_num, int mode)
{
	uartdev_data_t *uart_data_p;
	mdev_t *mdev_p;

	mdev_p = &mdev_uart[port_num];
	mdev_p->name = mdev_uart_name[port_num];
	mdev_p->port_id = port_num;
	mdev_p->private_data = (uint32_t) &uartdev_data[port_num];
	uart_data_p = &uartdev_data[port_num];
	uart_data_p->mode = mode;
	SET_RX_BUF_SIZE(uart_data_p, 0);
	uart_data_p->parity = UART_PARITY_NONE;
	uart_data_p->stopbits = DISABLE;
	uart_data_p->flowctl = FLOW_CONTROL_NONE;
	/* DMA is default disabled */
	uart_data_p->dma = UART_DMA_DISABLE;
	uart_data_p->dma_rd_handle = NULL;
	uart_data_p->num_dma_unread = 0;
	uart_data_p->is_read_blocking = false;
	/* timeout values are initialised to OS_WAIT_FOREVER
	 */
	uart_data_p->tx_tout = OS_WAIT_FOREVER;
	uart_data_p->rx_tout = OS_WAIT_FOREVER;
	SET_DMA_BLK_SIZE(uart_data_p, DEFAULT_DMA_BLK_SIZE);
	fill_cb_array();
}

int uart_drv_init(UART_ID_Type id, int mode)
{
	int ret;

	if (id < 0 || id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", id);
		return -WM_FAIL;
	}

	if (mdev_get_handle(mdev_uart_name[id]) != NULL)
		return WM_SUCCESS;

	ret = os_mutex_create(&uart_mutex[id], "uart", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	/* Initialize the UART port's mdev structure */
	uartdrv_mdev_init(id, mode);

	switch (id) {
	case UART0_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_0, CLK_UART_FAST);
		CLK_ModuleClkEnable(CLK_UART0);
		break;
	case UART1_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_1, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART1);
		break;
	case UART2_ID:
		CLK_SetUARTClkSrc(CLK_UART_ID_2, CLK_UART_SLOW);
		CLK_ModuleClkEnable(CLK_UART2);
		break;
	}

	/* Register the UART device driver for this port */
	mdev_register(&mdev_uart[id]);

	return WM_SUCCESS;
}

int uart_drv_rxbuf_size(UART_ID_Type uart_id, uint32_t size)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (size < 0)
		return -WM_FAIL;

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
				mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	/* threshold limit = 2 x UARTDRV_THRESHOLD_RX_BUFFER */
	if ((uart_data_p->flowctl == FLOW_CONTROL_SW) &&
			(size <= (2 * UARTDRV_THRESHOLD_RX_BUF))) {
		UART_LOG("Port %d rx buffer size should be greater than"
			 " threshold limit when software flow control is"
			 " enabled\r\n");
		return -WM_FAIL;
	}

	SET_RX_BUF_SIZE(uart_data_p, size);
	return WM_SUCCESS;
}

int uart_drv_dma_rd_blk_size(UART_ID_Type uart_id, uint32_t size)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (size < 0)
		return -WM_FAIL;

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
				mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}

	uart_data_p = (uartdev_data_t *) mdev_p->private_data;

	if (uart_data_p->dma != true) {
		UART_LOG("Can't set block size without enabling dma for %s\r\n",
			 mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	if (!IS_DMA_BLK_SIZE(size)) {
		UART_LOG("Block size should be less than or equal %d\r\n",
				DMA_MAX_CHUNK_SIZE);
		return -WM_FAIL;
	}
	SET_DMA_BLK_SIZE(uart_data_p, size);
	return WM_SUCCESS;
}

void uart_drv_timeout(UART_ID_Type uart_id, unsigned long tx_tout,
		unsigned long rx_tout)
{
	mdev_t *mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);

	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
				mdev_uart_name[uart_id]);
		return;
	}

	uartdev_data_t *uart_data_p;
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_data_p->tx_tout = tx_tout;
	uart_data_p->rx_tout = rx_tout;
}

int uart_drv_set_opts(UART_ID_Type uart_id, uint32_t parity, uint32_t stopbits,
		      flow_control_t flow_control)
{

	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	if (!IS_UART_PARITY(parity) || (stopbits != ENABLE &&
					stopbits != DISABLE)) {
		UART_LOG("Port %d parameter error\r\n", uart_id);
		return -WM_FAIL;
	}

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
				mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_data_p->parity = parity;
	uart_data_p->stopbits = stopbits;

	if ((flow_control == FLOW_CONTROL_SW) &&
			(uart_data_p->dma == UART_DMA_ENABLE)) {
		UART_LOG("Port %d software flow control cannot be enabled"
			 " when DMA is enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	uart_data_p->flowctl = flow_control;

	return WM_SUCCESS;
}

int uart_drv_blocking_read(UART_ID_Type uart_id, bool is_blocking)
{
	mdev_t *mdev_p;
	uartdev_data_t *uart_data_p;

	if (uart_id < 0 || uart_id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", uart_id);
		return -WM_FAIL;
	}

	/*check uart_id is initialized*/
	mdev_p = mdev_get_handle(mdev_uart_name[uart_id]);
	if (!mdev_p) {
		UART_LOG("Unable to open device %s\r\n",
				mdev_uart_name[uart_id]);
		return -WM_FAIL;
	}
	uart_data_p = (uartdev_data_t *) mdev_p->private_data;
	uart_data_p->is_read_blocking = is_blocking;
	return WM_SUCCESS;
}

void uart_drv_deinit(UART_ID_Type id)
{
	mdev_t *dev;
	/* Return if the port ID is invalid */
	if (id < 0 || id >= NUM_UART_PORTS) {
		UART_LOG("Port %d not enabled\r\n", id);
		return;
	}

	/* Return if the device was not registered */
	if ((dev = mdev_get_handle(mdev_uart_name[id])) == NULL)
		return;

	/* Delete mutex and dregister the device */
	os_mutex_delete(&uart_mutex[dev->port_id]);
	mdev_deregister(dev->name);
}

