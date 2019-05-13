/** @file mdev_sdio.c
 *
 *  @brief This file provides  mdev driver for SDIO
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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


#include <mdev_sdio.h>
#include <wm_os.h>
#include <board.h>

#define SDIO_CMD_TIMEOUT	2000

/* sdio mdev and synchronization handles */
static mdev_t mdev_sdio;
static os_mutex_t sdio_mutex;
static uint32_t i_stat;
void (*cdint_cb)(int i_stat);
void (*cdint_cb1)(int i_stat);
uint32_t boundary;

/* sdio device specific names */
static const char *mdev_sdio_name = "MDEV_SDIO";

void sdio_dumpregs()
{
	sdio_d("SYSADDR: 0x%08x\r\n", SDIO->SYSADDR.WORDVAL);
	sdio_d("BLK_CNTL: 0x%08x\r\n", SDIO->BLK_CNTL.WORDVAL);
	sdio_d("ARG: 0x%08x\r\n", SDIO->ARG.WORDVAL);
	sdio_d("CMD_XFRMD: 0x%08x\r\n", SDIO->CMD_XFRMD.WORDVAL);
	sdio_d("RESP0: 0x%08x\r\n", SDIO->RESP[0].WORDVAL);
	sdio_d("RESP1: 0x%08x\r\n", SDIO->RESP[1].WORDVAL);
	sdio_d("RESP2: 0x%08x\r\n", SDIO->RESP[2].WORDVAL);
	sdio_d("RESP3: 0x%08x\r\n", SDIO->RESP[3].WORDVAL);
	sdio_d("DP: 0x%08x\r\n", SDIO->DP.WORDVAL);
	sdio_d("STATE: 0x%08x\r\n", SDIO->STATE.WORDVAL);
	sdio_d("CNTL1: 0x%08x\r\n", SDIO->CNTL1.WORDVAL);
	sdio_d("CNTL2: 0x%08x\r\n", SDIO->CNTL2.WORDVAL);
	sdio_d("I_STAT: 0x%08x\r\n", SDIO->I_STAT.WORDVAL);
	sdio_d("I_STAT_EN: 0x%08x\r\n", SDIO->I_STAT_EN.WORDVAL);
	sdio_d("I_SIG_EN: 0x%08x\r\n", SDIO->I_SIG_EN.WORDVAL);
	sdio_d("CAP0: 0x%08x\r\n", SDIO->CAP0.WORDVAL);
	sdio_d("VER: 0x%08x\r\n", SDIO->VER.WORDVAL);

	return;
}

static void sdio_controller_reinit(void)
{
	SDIOC_CFG_Type sdioCfg;

	sdioCfg.clkEnable = ENABLE;
	sdioCfg.busWidth = SDIOC_BUS_WIDTH_4;
	sdioCfg.speedMode = SDIOC_SPEED_HIGH;
	sdioCfg.busVoltSel = SDIOC_VOLTAGE_3P3;
	sdioCfg.readWaitCtrl = DISABLE;
	sdioCfg.blkGapInterrupt = DISABLE;

	SDIOC_Init(&sdioCfg);

	/* Bus power on */
	SDIOC_PowerOn();

	/* Enable SDIO interrupt */
	SDIOC_IntMask(SDIOC_INT_ALL, UNMASK);
	SDIOC_IntSigMask(SDIOC_INT_ALL, UNMASK);
}

static void sdio_controller_init(void)
{
	SDIOC_CFG_Type sdioCfg;

	/* Init SDIO with initial settings */
	sdioCfg.clkEnable = ENABLE;
	sdioCfg.busWidth = SDIOC_BUS_WIDTH_1;
	sdioCfg.speedMode = SDIOC_SPEED_NORMAL;
	sdioCfg.busVoltSel = SDIOC_VOLTAGE_3P3;
	sdioCfg.readWaitCtrl = DISABLE;
	sdioCfg.blkGapInterrupt = DISABLE;
	SDIOC_Init(&sdioCfg);

	/* Bus power on */
	SDIOC_PowerOn();

	/* Enable SDIO interrupt */
	SDIOC_IntMask(SDIOC_INT_ALL, UNMASK);
	SDIOC_IntSigMask(SDIOC_INT_ALL, UNMASK);
}

void SDIO_IRQHandler(void)
{
	uint32_t mask;

	/* Read the interrupt status */
	mask = SDIO->I_STAT.WORDVAL;

	/* Clear the interrupts */
	SDIO->I_STAT.WORDVAL |= mask;

	i_stat |= mask;

	if (((struct I_STAT *)&mask)->DMAINT) {
		SDIO->SYSADDR.WORDVAL = boundary;
		boundary += DMA_BOUNDARY_SIZE;
	}

	if (((struct I_STAT *)&mask)->CDINT) {
		SDIOC_IntMask(SDIOC_INT_CDINT, MASK);
		SDIOC_IntSigMask(SDIOC_INT_CDINT, MASK);
		if (cdint_cb)
			cdint_cb(0);
		if (cdint_cb1)
			cdint_cb1(0);
	}
}

static int sdio_check_status(int cnt, int dat)
{
	int sta;
	int cmd = 1;

	while (cmd || dat) {
		sta = os_enter_critical_section();
		if (((struct I_STAT *)&i_stat)->ERRINT) {
			os_exit_critical_section(sta);
			sdio_d("Command/Data Transfer error cmd = %d dat = %d"
					" i_stat = %x", cmd, dat, i_stat);
			return false;
		}
		if (((struct I_STAT *)&i_stat)->CMDCOMP)
			cmd = 0;
		if (((struct I_STAT *)&i_stat)->XFRCOMP)
			dat = 0;
		os_exit_critical_section(sta);

		if (!cnt--) {
			sdio_d("Timeout waiting for Command/Data to complete "
			"cmd = %d dat = %d i_stat %x", cmd, dat, i_stat);
			return false;
		}
	}
	return true;
}

static int sdio_send_cmd(int cmdno, uint32_t arg, uint32_t *resp)
{
	int sta;
	SDIOC_CmdCfg_Type cmd;
	uint32_t cnt;

	sta = os_enter_critical_section();
	i_stat = 0;
	os_exit_critical_section(sta);

	cmd.cmdIndex = cmdno;
	cmd.arg = arg;
	cmd.resp = SDIOC_RESP_SHORT;
	cmd.cmdType = SDIOC_CMD_NORMAL;
	cmd.dataPreSel = SDIOC_DATA_PRESENT_NO;
	cmd.cmdIndexCheck = ENABLE;
	cmd.cmdCrcCheck = ENABLE;

	/* Only volt cmd needs this disabled */
	if (cmd.cmdIndex == SDIO_SDIO_SEN_OP_COND) {
		cmd.cmdIndexCheck = DISABLE;
		cmd.cmdCrcCheck = DISABLE;
	}

	cnt = SDIO_CMD_TIMEOUT;
	while (--cnt && SDIOC_GetStatus(SDIOC_STATUS_CCMDINHBT))
		;
	if (!cnt) {
		sdio_e("Timeout waiting for CMD ready", resp);
		return false;
	}

	SDIOC_SendCmd(&cmd);

	if (sdio_check_status(SDIO_CMD_TIMEOUT, 0) == false)
		return false;

	*resp = SDIOC_GetResponse(SDIOC_RESP_REG_0);

	return true;
}

int sdio_drv_creg_read(mdev_t *dev, int addr, int fn, uint32_t *resp)
{
	uint32_t arg;
	int ret;

	ret = os_mutex_get(&sdio_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		sdio_e("failed to get mutex\r\n");
		return false;
	}

	arg = (fn << 28) | (addr << 9);
	ret = sdio_send_cmd(SDIO_RW_DIRECT, arg, resp);

	os_mutex_put(&sdio_mutex);
	return ret;
}

int sdio_drv_creg_write(mdev_t *dev, int addr, int fn, uint8_t data,
			uint32_t *resp)
{
	uint32_t arg;
	int ret;

	ret = os_mutex_get(&sdio_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		sdio_e("failed to get mutex\r\n");
		return false;
	}

	arg = (1 << 31) | (fn << 28) | (1 << 27) | (addr << 9) | data;
	ret = sdio_send_cmd(SDIO_RW_DIRECT, arg, resp);

	os_mutex_put(&sdio_mutex);
	return ret;
}

static int sdio_xfer_data(SDIOC_Transfer_Type *ttype, uint32_t arg,
			  uint32_t *resp)
{
	int sta;
	SDIOC_CmdCfg_Type cmd;
	uint32_t cnt;

	sta = os_enter_critical_section();
	i_stat = 0;
	os_exit_critical_section(sta);

	cmd.cmdIndex = SDIO_RW_DIRECT_EXTENDED;
	cmd.arg = arg;
	cmd.resp = SDIOC_RESP_SHORT;
	cmd.cmdType = SDIOC_CMD_NORMAL;
	cmd.dataPreSel = SDIOC_DATA_PRESENT_YES;
	cmd.cmdIndexCheck = ENABLE;
	cmd.cmdCrcCheck = ENABLE;

	cnt = SDIO_CMD_TIMEOUT;
	while (--cnt && (SDIOC_GetStatus(SDIOC_STATUS_CCMDINHBT)
			 || SDIOC_GetStatus(SDIOC_STATUS_DCMDINHBT)))
		;
	if (!cnt) {
		sdio_e("Timeout waiting for CMD/DAT ready", resp);
		return false;
	}

	SDIOC_SetTransferMode(ttype);
	SDIOC_SendCmd(&cmd);

	if (sdio_check_status(SDIO_CMD_TIMEOUT, 1) == false)
		return false;

	*resp = SDIOC_GetResponse(SDIOC_RESP_REG_0);

	return true;
}

int sdio_drv_read(mdev_t *dev, uint32_t addr, uint32_t fn, uint32_t bcnt,
		  uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
	SDIOC_DmaCfg_Type dmacfg;
	SDIOC_Transfer_Type ttype;
	uint32_t arg;
	int ret;

	ret = os_mutex_get(&sdio_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		sdio_e("failed to get mutex\r\n");
		return false;
	}

	dmacfg.dmaBufSize = 0x7;
	dmacfg.dmaSystemAddr = (uint32_t) buf;
	/* XXX Need to clean up this implementation */
	boundary = DMA_BOUNDARY_SIZE +
	    ((uint32_t) buf & ~(DMA_BOUNDARY_SIZE - 1));
	/* DMA configuration */
	SDIOC_DmaConfig(&dmacfg);

	ttype.dmaEnabe = ENABLE;
	ttype.dataTimeoutCnt = 0xe;
	ttype.transferDir = SDIOC_TRANSDIR_READ;
	ttype.blkCntEnable = ENABLE;
	ttype.blkCnt = bcnt;
	ttype.blkSize = bsize;
	ttype.blkSel = (bcnt > 1) ? SDIOC_BLOCK_MULTIPLE : SDIOC_BLOCK_SINGLE;
	arg = (fn << 28) | (addr << 9);

	/* Block/Byte mode */
	if (bcnt == 1 && bsize <= 512)
		arg |= bsize & 0x1ff;
	else
		arg |= (1 << 27) | bcnt;

	ret = sdio_xfer_data(&ttype, arg, resp);

	os_mutex_put(&sdio_mutex);
	return ret;
}

int sdio_drv_write(mdev_t *dev, uint32_t addr, uint32_t fn, uint32_t bcnt,
		   uint32_t bsize, uint8_t *buf, uint32_t *resp)
{
	SDIOC_DmaCfg_Type dmacfg;
	SDIOC_Transfer_Type ttype;
	uint32_t arg;
	int ret;

	ret = os_mutex_get(&sdio_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL) {
		sdio_e("failed to get mutex\r\n");
		return false;
	}

	dmacfg.dmaBufSize = 0x7;
	dmacfg.dmaSystemAddr = (uint32_t) buf;
	/* XXX Need to cleanpup this implementation */
	boundary = DMA_BOUNDARY_SIZE +
	    ((uint32_t) buf & ~(DMA_BOUNDARY_SIZE - 1));
	/* DMA configuration */
	SDIOC_DmaConfig(&dmacfg);

	ttype.dmaEnabe = ENABLE;
	ttype.dataTimeoutCnt = 0xe;
	ttype.transferDir = SDIOC_TRANSDIR_WRITE;
	ttype.blkCntEnable = ENABLE;
	ttype.blkCnt = bcnt;
	ttype.blkSize = bsize;
	ttype.blkSel = (bcnt > 1) ? SDIOC_BLOCK_MULTIPLE : SDIOC_BLOCK_SINGLE;
	arg = (1 << 31) | (fn << 28) | (addr << 9);

	/* Block/Byte mode */
	if (bcnt == 1 && bsize <= 512)
		arg |= bsize & 0x1ff;
	else
		arg |= (1 << 27) | bcnt;

	ret = sdio_xfer_data(&ttype, arg, resp);

	os_mutex_put(&sdio_mutex);
	return ret;
}

static int sdio_card_reset(void)
{
	uint32_t resp;
	int ret;

	/* Soft Reset card */
	ret = sdio_drv_creg_write(&mdev_sdio, 0x6, 0, 0x8, &resp);
	if (ret != true)
		return ret;

	SDIOC_CmdLineReset();
	SDIOC_DataxLineReset();

	return true;
}

static int sdio_card_init()
{
	uint32_t arg, resp;
	uint8_t hs;
	int ret;

	/* Get supported voltages. */
	arg = 0;
	ret = sdio_send_cmd(SDIO_SDIO_SEN_OP_COND, arg, &resp);
	if (ret != true) {
		sdio_e("Get SDIO_SEN_OP_COND volt 0x%08x", resp);
		return ret;
	}

	/* Set supported voltages. */
	arg = resp;
	ret = sdio_send_cmd(SDIO_SDIO_SEN_OP_COND, arg, &resp);
	if (ret != true) {
		sdio_e("Set SDIO_SEN_OP_COND volt 0x%08x", resp);
		return ret;
	}

	/* Get Relative Card Address (RCA) */
	arg = 0;
	ret = sdio_send_cmd(SDIO_SET_REL_ADDR, arg, &resp);
	if (ret != true) {
		sdio_e("Get SDIO_SET_REL_ADDR rca 0x%08x", resp >> 16);
		return ret;
	}

	/* Select/Deselect Card with correct RCA */
	arg = resp & (0xffff << 16);
	ret = sdio_send_cmd(SDIO_SEL_DESEL_CARD, arg, &resp);
	if (ret != true) {
		sdio_e("Set SDIO_SEL_DESEL_CARD resp 0x%08x", resp);
		return ret;
	}

	sdio_drv_creg_read(&mdev_sdio, 0x13, 0, &resp);
	hs = resp & 0xff;
	if (hs & (1 << 0)) {
		uint8_t bus_width;

		sdio_drv_creg_read(&mdev_sdio, 0x7, 0, &resp);
		bus_width = resp & 0xfc;
		bus_width |= (1 << 1);
		/* HighSpeed capable card, switch to 4 bit 50MHz. */
		sdio_drv_creg_write(&mdev_sdio, 0x7, 0, bus_width, &resp);
		sdio_drv_creg_read(&mdev_sdio, 0x7, 0, &resp);
		if ((resp & 0x3) != 0x2) {
			sdio_e("4 bit mode failed!");
			return false;
		} else {
			SDIOC_CFG_Type sdioCfg;

			hs |= (1 << 1);
			sdio_drv_creg_write(&mdev_sdio, 0x13, 0, hs, &resp);
			sdioCfg.clkEnable = ENABLE;
			sdioCfg.busWidth = SDIOC_BUS_WIDTH_4;
			sdioCfg.speedMode = SDIOC_SPEED_HIGH;
			sdioCfg.busVoltSel = SDIOC_VOLTAGE_3P3;
			sdioCfg.readWaitCtrl = DISABLE;
			sdioCfg.blkGapInterrupt = DISABLE;
			SDIOC_Init(&sdioCfg);
		}
	}

	sdio_drv_creg_read(&mdev_sdio, 0x0, 0, &resp);
	sdio_d("Card Version - (0x%x)", resp & 0xff);

	/* Mask interrupts in card */
	sdio_drv_creg_write(&mdev_sdio, 0x4, 0, 0x3, &resp);
	/* Enable IO in card */
	sdio_drv_creg_write(&mdev_sdio, 0x2, 0, 0x2, &resp);

	return true;
}

mdev_t *sdio_drv_open(const char *name)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		sdio_e("driver open called without registering device"
		       " (%s)", name);
		return NULL;
	}
	return mdev_p;
}

void sdio_drv_set_cb1(void (*cd_int)(int))
{
	cdint_cb1 = cd_int;
}

void sdio_drv_deinit(void)
{
	if (mdev_get_handle(mdev_sdio_name) == NULL)
		return;

	os_mutex_delete(&sdio_mutex);
	CLK_ModuleClkDisable(CLK_SDIO);
	NVIC_DisableIRQ(SDIO_IRQn);
	cdint_cb = NULL;
	cdint_cb1 = NULL;
	mdev_deregister(mdev_sdio_name);
}

int sdio_drv_reinit(void (*cd_int)(int))
{

	mdev_sdio.name = mdev_sdio_name;
	if (sdio_mutex == NULL) {
		int ret = os_mutex_create(&sdio_mutex, "sdio-mutex",
					  OS_MUTEX_INHERIT);
		if (ret == -WM_FAIL) {
			sdio_e("Failed to create mutex\r\n");
			return -WM_FAIL;
		}
	}
	/* Max SDIO Clock Frequency is 50MHz */
	if (CLK_GetSystemClk() > 50000000)
		CLK_ModuleClkDivider(CLK_SDIO, 4);

	/* Enable SDIO Clock */
	CLK_ModuleClkEnable(CLK_SDIO);

	/* Open the SDIO interrupt control of Cortex-M3 */
	NVIC_EnableIRQ(SDIO_IRQn);
	/* Set the SDIO interrupt priority */
	NVIC_SetPriority(SDIO_IRQn, 0xa);

	sdio_controller_reinit();
	cdint_cb = cd_int;
	mdev_register(&mdev_sdio);
	return WM_SUCCESS;
}

int sdio_drv_init(void (*cd_int)(int))
{
	int ret;

	if (mdev_get_handle(mdev_sdio_name) != NULL)
		return WM_SUCCESS;

	mdev_sdio.name = mdev_sdio_name;

	ret = os_mutex_create(&sdio_mutex, "sdio-mutex", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL) {
		sdio_e("Failed to create mutex\r\n");
		return -WM_FAIL;
	}

	/* Max SDIO Clock Frequency is 50MHz */
	if (CLK_GetSystemClk() > 50000000)
		CLK_ModuleClkDivider(CLK_SDIO, 4);

	/* Enable SDIO Clock */
	CLK_ModuleClkEnable(CLK_SDIO);

	/* Open the SDIO interrupt control of Cortex-M3 */
	NVIC_EnableIRQ(SDIO_IRQn);
	/* Set the SDIO interrupt priority */
	NVIC_SetPriority(SDIO_IRQn, 0xa);

	sdio_controller_init();

	if (sdio_card_reset() == false) {
		sdio_e("Card reset failed");
		return -WM_FAIL;
	} else
		sdio_d("Card reset successful");

	cdint_cb = cd_int;
	if (sdio_card_init() == false) {
		sdio_e("Card initialization failed");
		return -WM_FAIL;
	} else
		sdio_d("Card initialization successful");

	mdev_register(&mdev_sdio);

	return WM_SUCCESS;
}
