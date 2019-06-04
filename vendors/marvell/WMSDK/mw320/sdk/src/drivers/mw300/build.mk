# Copyright (C) 2008-2017, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdrv

libdrv-objs-y := \
	lowlevel/mw300_clock.c  	\
	lowlevel/mw300_driver.c 	\
	lowlevel/mw300_pinmux.c 	\
	lowlevel/mw300_gpio.c 		\
	lowlevel/mw300_pmu.c 		\
	lowlevel/mw300_sdio.c 		\
	lowlevel/mw300_flashc.c 	\
	lowlevel/mw300_flash.c 		\
	lowlevel/mw300_crc.c 		\
	lowlevel/mw300_aes.c 		\
	lowlevel/mw300_uart.c 		\
	lowlevel/mw300_sdio.c 		\
	lowlevel/mw300_ssp.c 		\
	lowlevel/mw300_i2c.c 		\
	lowlevel/mw300_adc.c 		\
	lowlevel/mw300_crc.c 		\
	lowlevel/mw300_acomp.c 		\
	lowlevel/mw300_dac.c 		\
	lowlevel/mw300_dma.c 		\
	lowlevel/mw300_qspi.c 		\
	lowlevel/mw300_gpt.c 		\
	lowlevel/mw300_wdt.c 		\
	lowlevel/mw300_rtc.c 		\
	lowlevel/mw300_bg.c 		\
	lowlevel/mw300_spi_flash.c

libdrv-objs-y += \
	../common/mdev_crc.c 		\
	../common/mdev_dac.c

libdrv-objs-y += \
	mdev_sdio.c 			\
	mdev_rtc.c			\
	mdev_uart.c 			\
	mdev_iflash.c 			\
	mdev_startup.c 			\
	mdev_rfctrl.c			\
	mdev_pm.c			\
	mdev_dma.c			\
	mdev_adc.c

libdrv-supported-toolchain-y := arm_gcc iar

