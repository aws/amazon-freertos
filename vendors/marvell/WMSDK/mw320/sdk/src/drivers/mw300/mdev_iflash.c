/** @file mdev_iflash.c
 *
 *  @brief This file provides  mdev driver for internal flash
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


/*
 * mdev_iflash.c: mdev driver for internal flash
 */

#include <mdev_iflash.h>
#include <lowlevel_drivers.h>
#include <flash.h>
#include <wmstdio.h>
#include <wm_os.h>
#include <compat_attribute.h>

static mdev_t MDEV_iflash;
static const char *MDEV_NAME_iflash = "iflash";
static flash_cfg fl_cfg;
static os_mutex_t iflash_mutex;
static uint32_t flashc_cmd_type;
extern unsigned long _flashc_mem_start;

/*
 * Define following to keep IRQs (above certain threshold priority) enabled
 * during flash controller outage. If code is executing from flash and we need
 * to perform flash erase/program operations, then flash controller needs to
 * be disabled and switch to normal QSPI mode. Some applications with strict
 * timing constraints may need to keep special IRQs enabled (with
 * corresponding code brought to RAM) during this period, hence the option.
 */
/* #define CONFIG_IRQS_ALWAYS_ENABLED */
#if defined(CONFIG_IRQS_ALWAYS_ENABLED)

/*
 * Interrupts with priority less than following value will be masked
 * Hence:
 * 1. Define required IRQ priority higher than following (less value high
 *    priority)
 * 2. Move its IRQ handler in to RAM, along with required execution code
 * 3. Do not use any OS specific call in this IRQ handler, as flash controller
 *    is in disabled mode.
 */
#define MIN_INT_DISABLE_PRIO (0x8)

#define FLASHC_MAX_IRQ_PRIO (MIN_INT_DISABLE_PRIO << 4)

inline __attribute__((always_inline))
static void Disable_IRQs()
{
	uint32_t reg;

	__asm volatile
		(
		 "mov %0, %1\n"
		 "msr basepri, %0\n"
		 "isb\n"
		 "dsb\n"
		 :"=r" (reg) : "i" (FLASHC_MAX_IRQ_PRIO)
		);
}

inline __attribute__((always_inline))
static void Enable_IRQs()
{
	__asm volatile
		(
		 "msr basepri, %0 " :: "r" (0)
		);
}
#else

#define Disable_IRQs	 __disable_irq
#define Enable_IRQs	 __enable_irq

#endif /* CONFIG_IRQS_ALWAYS_ENABLED */

static FLASH_Interface_Type FLASH_GetInterface(void)
{
	return FLASHC->FCCR.BF.FLASHC_PAD_EN;
}

mdev_t *iflash_drv_open(const char *name, uint32_t flags)
{
	mdev_t *mdev_p = mdev_get_handle(name);

	if (mdev_p == NULL) {
		ifl_d("driver open called without reg dev (%s)", name);
		return NULL;
	}

	return mdev_p;
}

int iflash_drv_close(mdev_t *dev)
{
	return 0;
}

int iflash_drv_get_id(mdev_t *dev, uint64_t *id)
{
	flash_cfg *temp;
	temp = (flash_cfg *) dev->private_data;
	*id = temp->fl_id;
	return WM_SUCCESS;
}

#define FLASHC_Disable() { \
	FLASHC->FCCR.BF.CACHE_EN = DISABLE; \
	flashc_cmd_type = FLASHC->FCCR.BF.CMD_TYPE; \
	if (FLASHC->FCCR.BF.CMD_TYPE == FLASHC_HW_CMD_FRQIOC) { \
		FLASHC->FCCR.BF.CMD_TYPE = FLASHC_HW_CMD_ECRQ;	\
		while (FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE == 0) \
			; \
		FLASHC->FCSR.BF.CONT_RD_MD_EXIT_DONE = 1; \
	} \
	FLASHC->FCCR.BF.FLASHC_PAD_EN = DISABLE; \
}

#define FLASHC_Flush_Cache() { \
	FLASHC->FCCR.BF.CACHE_LINE_FLUSH = 1; \
	volatile uint32_t __cnt = 0; \
	while (__cnt < 0x200000) { \
		if (FLASHC->FCCR.BF.CACHE_LINE_FLUSH == 0) \
			break; \
		__cnt++; \
	} \
}

/* Enable cache mode and flash controller */
#define FLASHC_Enable() { \
	FLASHC->FCCR.BF.CACHE_EN = ENABLE; \
	FLASHC->FCCR.BF.CMD_TYPE = flashc_cmd_type; \
	FLASHC->FCCR.BF.FLASHC_PAD_EN = ENABLE; \
}

__SECTION__(.ram)
uint32_t iflash_flashc_to_qspi_read_jedecid()
{
	uint32_t id;

	ifl_d("Enter %s", __func__);

	Disable_IRQs();
	FLASHC_Disable();
	id = FLASH_GetJEDECID();
	FLASHC_Enable();
	Enable_IRQs();

	ifl_d("Exit %s", __func__);
	return id;
}

__SECTION__(.ram)
uint64_t iflash_flashc_to_qspi_read_uniqid()
{
	uint64_t id;

	ifl_d("Enter %s", __func__);

	Disable_IRQs();
	FLASHC_Disable();
	id = FLASH_GetUniqueID();
	FLASHC_Enable();
	Enable_IRQs();

	ifl_d("Exit %s", __func__);
	return id;
}

__SECTION__(.ram)
int iflash_flashc_to_qspi_read(uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	ifl_d("%s: len: %d, addr: %x", __func__, len, addr);

	if ((((uint32_t) buf) & ((uint32_t) &_flashc_mem_start))
			== (uint32_t) &_flashc_mem_start) {
		fl_e("read buf from mmapped flashc range when"
		     " flashc is disabled");
		return -WM_E_FAULT;
	}

	Disable_IRQs();
	FLASHC_Disable();
	ret = FLASH_Read(FLASH_FAST_READ_QUAD_IO,
			addr, buf, len);
	FLASHC_Enable();
	Enable_IRQs();

	ifl_d("Exit %s", __func__);
	return ret;
}

__SECTION__(.ram)
int iflash_flashc_to_qspi_write(uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	ifl_d("Enter %s: len: %d, addr: %x", __func__, len, addr);

	if ((((uint32_t) buf) & ((uint32_t) &_flashc_mem_start))
			== (uint32_t) &_flashc_mem_start) {
		fl_d("write buffer from mmapped flashc range"
		     " when flashc is disabled");
		return -WM_E_FAULT;
	}

	Disable_IRQs();
	FLASHC_Disable();
	ret = FLASH_Write(FLASH_PROGRAM_QUAD, addr, buf, len);
	FLASHC_Flush_Cache();
	FLASHC_Enable();
	Enable_IRQs();

	ifl_d("Exit %s", __func__);
	return ret;
}

__SECTION__(.ram)
int iflash_flashc_to_qspi_erase(unsigned long start, unsigned long size)
{
	int ret;

	ifl_d("Enter %s: start: %d, size: %d", __func__, start, size);
	Disable_IRQs();
	FLASHC_Disable();
	ret = FLASH_Erase(start, (start + size - 1));
	FLASHC_Flush_Cache();
	FLASHC_Enable();
	Enable_IRQs();
	ifl_d("Exit %s", __func__);
	return ret;
}

__SECTION__(.ram)
int iflash_flashc_to_qspi_eraseall()
{
	int ret;

	ifl_d("Enter %s", __func__);
	Disable_IRQs();
	FLASHC_Disable();
	ret = FLASH_EraseAll();
	FLASHC_Flush_Cache();
	FLASHC_Enable();
	Enable_IRQs();
	ifl_d("Exit %s", __func__);
	return ret;
}

int iflash_drv_erase(mdev_t *dev, unsigned long start, unsigned long size)
{
	int ret = 0;

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		/*
		 * During flash erase/write operation in XiP mode, flash
		 * controller is disabled and hence all interrupts stay
		 * disabled. If erase size is larger, then it may create
		 * issues with respect to system responsiveness and hence
		 * erase in smaller chunks to allow pending interrupts to
		 * clear.
		 */
		uint32_t chunk_size = 0x80000; /* 512 KB */
		uint32_t erased_size = 0;
		uint32_t addr = start;
		while (erased_size < size) {
			if ((size - erased_size) < chunk_size)
				chunk_size = size - erased_size;
			ret = iflash_flashc_to_qspi_erase(addr, chunk_size);
			if (!ret)
				break;
			erased_size += chunk_size;
			addr += chunk_size;
		}
	} else {
		ret = FLASH_Erase(start, (start + size - 1));
	}

	os_mutex_put(&iflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int iflash_drv_eraseall(mdev_t *dev)
{
	int ret = 0;

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		ret = iflash_flashc_to_qspi_eraseall();
	} else {
		ret = FLASH_EraseAll();
	}

	os_mutex_put(&iflash_mutex);

	return ret ? 0 : -WM_FAIL;
}

int iflash_drv_read(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		uint32_t flash_offset;
		/* See whether flash controller offset register is enabled */
		if (FLASHC->FCACR.BF.OFFSET_EN == 1)
			flash_offset = FLASHC->FAOFFR.BF.OFFSET_VAL;
		else
			flash_offset = 0;

		if (addr < flash_offset) {
			/* We have address out of range of memory mapped
			 * flash controller range. Hence disable flash
			 * controller read through QSPI interface.
			 */
			ret = iflash_flashc_to_qspi_read(buf, len, addr);
		} else {
			memcpy(buf,
			(uint8_t *)addr +
			((uint32_t) &_flashc_mem_start - flash_offset), len);
			ret = len;
		}
	} else {
		ret = FLASH_Read(FLASH_FAST_READ_QUAD_IO, addr, buf, len);
	}
	os_mutex_put(&iflash_mutex);

	if (ret == len)
		return WM_SUCCESS;
	else if (ret == ERROR)
		return -WM_FAIL;
	else
		return ret;
}

int iflash_drv_write(mdev_t *dev, uint8_t *buf, uint32_t len, uint32_t addr)
{
	int ret;

	if (len == 0)
		return 0;
	if (!buf)
		return -WM_FAIL;

	ret = os_mutex_get(&iflash_mutex, OS_WAIT_FOREVER);
	if (ret == -WM_FAIL)
		return ret;

	if (FLASH_GetInterface() == FLASH_INTERFACE_FLASHC) {
		ret = iflash_flashc_to_qspi_write(buf, len, addr);
	} else {
		ret = FLASH_Write(FLASH_PROGRAM_QUAD, addr, buf, len);
	}
	os_mutex_put(&iflash_mutex);

	if (ret == DSUCCESS)
		return WM_SUCCESS;
	else if (ret == ERROR)
		return -WM_FAIL;
	else
		return ret;
}

int iflash_drv_init(void)
{
	int ret;
	uint32_t jedec_id;

	if (mdev_get_handle(MDEV_NAME_iflash) != NULL)
		return WM_SUCCESS;

	MDEV_iflash.name = MDEV_NAME_iflash;

	ret = os_mutex_create(&iflash_mutex, "iflash", OS_MUTEX_INHERIT);
	if (ret == -WM_FAIL)
		return -WM_FAIL;

	PMU_PowerOnVDDIO(PMU_VDDIO_2);
	GPIO_PinMuxFun(GPIO_28, GPIO28_QSPI_SSn);
	GPIO_PinMuxFun(GPIO_29, GPIO29_QSPI_CLK);
	GPIO_PinMuxFun(GPIO_30, GPIO30_QSPI_D0);
	GPIO_PinMuxFun(GPIO_31, GPIO31_QSPI_D1);
	GPIO_PinMuxFun(GPIO_32, GPIO32_QSPI_D2);
	GPIO_PinMuxFun(GPIO_33, GPIO33_QSPI_D3);

	/* Max QSPI0 Clock Frequency is 50MHz */
	if (CLK_GetSystemClk() > 50000000)
		CLK_ModuleClkDivider(CLK_QSPI0, 4);

	if (FLASH_GetInterface() == FLASH_INTERFACE_QSPI) {
		QSPI->CONF.BF.CLK_PRESCALE = 0;
		/* Sample data on the falling edge.
		 * Sampling on rising edge does not
		 * meet timing requirements at 50MHz
		 */
		QSPI->TIMING.BF.CLK_CAPT_EDGE = 1;
		FLASH_ResetFastReadQuad();
	}

	/* Read the JEDEC id of the primary flash that is connected */
	if (FLASH_GetInterface() == FLASH_INTERFACE_QSPI)
		jedec_id = FLASH_GetJEDECID();
	else
		jedec_id = iflash_flashc_to_qspi_read_jedecid();

	/* Set the flash configuration as per the JEDEC id */
	ret = FLASH_SetConfig(jedec_id);
	/* In case of an error, print error message and continue */
	if (ret != DSUCCESS)
		ifl_d("Flash JEDEC ID 0x%x not present in supported flash list,"
		      "using default config for W25Q32BV", jedec_id);
	__UNUSED__ const struct flash_device_config *fl_conf
		= FLASH_GetConfig();
	ifl_d("Flash configuration:");
	ifl_d("Name: %s, JEDEC ID: 0x%x\r\n"
	      "Chip Size: 0x%x, Sector Size: 0x%x\r\n"
	      "Block Size: 0x%x, Page Size: 0x%x", fl_conf->name,
	      fl_conf->jedec_id, fl_conf->chip_size,
	      fl_conf->sector_size, fl_conf->block_size,
	      fl_conf->page_size);

	/* Store the Unique ID for internal flash into private data */
	if (FLASH_GetInterface() == FLASH_INTERFACE_QSPI)
		fl_cfg.fl_id = FLASH_GetUniqueID();
	else
		fl_cfg.fl_id = iflash_flashc_to_qspi_read_uniqid();

	ifl_d("Flash Unique ID is %llx", fl_cfg.fl_id);

	fl_cfg.fl_dev = FL_INT;
	MDEV_iflash.private_data = (uint32_t)&fl_cfg;

	mdev_register(&MDEV_iflash);

	return WM_SUCCESS;
}
