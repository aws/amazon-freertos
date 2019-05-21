/** @file mw300_rd.c
*
*  @brief Board File
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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
 * This is a board specific configuration file for
 * the AB-88MW300 baseboard based on schematic dated 19th June 2014.
 *
 */

#include <wmtypes.h>
#include <wmerrno.h>
#include <board.h>
#include <lowlevel_drivers.h>

/* Source module specific board functions for RD-88MW300 module */
#include <modules/gti-mw300.c>

int board_cpu_freq()
{
	return 200000000;
}

int board_32k_xtal()
{
	return false;
}

int board_32k_osc()
{
	return true;
}

int board_rc32k_calib()
{
	return false;
}

void board_gpio_power_on()
{
	/* Wakeup push buttons are active low */
	PMU_ConfigWakeupPin(PMU_GPIO22_INT, PMU_WAKEUP_LEVEL_LOW);
	PMU_ConfigWakeupPin(PMU_GPIO23_INT, PMU_WAKEUP_LEVEL_LOW);
}

void board_uart_pin_config(int id)
{
	switch (id) {
	case UART0_ID:
		GPIO_PinMuxFun(GPIO_2, GPIO2_UART0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_UART0_RXD);
		break;
	case UART1_ID:
		GPIO_PinMuxFun(GPIO_13, GPIO13_UART1_TXD);
		GPIO_PinMuxFun(GPIO_14, GPIO14_UART1_RXD);
		break;
	case UART2_ID:
		/* Not implemented yet */
		break;
	}
}

void board_i2c_pin_config(int id)
{
	switch (id) {
	case I2C0_PORT:
		GPIO_PinMuxFun(GPIO_4, GPIO4_I2C0_SDA);
		GPIO_PinMuxFun(GPIO_5, GPIO5_I2C0_SCL);
		break;
	case I2C1_PORT:
		GPIO_PinMuxFun(GPIO_17, GPIO17_I2C1_SCL);
		GPIO_PinMuxFun(GPIO_18, GPIO18_I2C1_SDA);
		break;
	}
}

I2C_ID_Type board_acodec_i2c_id(void)
{
	return I2C0_PORT;
}

void board_usb_pin_config()
{
	GPIO_PinMuxFun(GPIO_27, GPIO27_DRVVBUS);
}

void board_ssp_pin_config(int id, int cs)
{
	/* To do */
	switch (id) {
	case SSP0_ID:
		GPIO_PinMuxFun(GPIO_0, GPIO0_SSP0_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_1, GPIO1_SSP0_FRM);
		GPIO_PinMuxFun(GPIO_2, GPIO2_SSP0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_SSP0_RXD);
		break;
	case SSP1_ID:
		GPIO_PinMuxFun(GPIO_11, GPIO11_SSP1_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_12, GPIO12_SSP1_FRM);
		else {
			GPIO_PinMuxFun(GPIO_12, GPIO12_GPIO12);
			GPIO_SetPinDir(GPIO_12, GPIO_INPUT);
		}
		GPIO_PinMuxFun(GPIO_13, GPIO13_SSP1_TXD);
		GPIO_PinMuxFun(GPIO_14, GPIO14_SSP1_RXD);
		break;
	case SSP2_ID:
		GPIO_PinMuxFun(GPIO_46, GPIO46_SSP2_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_47, GPIO47_SSP2_FRM);
		GPIO_PinMuxFun(GPIO_48, GPIO48_SSP2_TXD);
		GPIO_PinMuxFun(GPIO_49, GPIO49_SSP2_RXD);
		break;
	}
}

SSP_ID_Type board_acodec_ssp_id(void)
{
	return SSP2_ID;
}

int board_adc_pin_config(int adc_id, int channel)
{
	/* Channel 2 and channel 3 need GPIO 44
	 * and GPIO 45 which are used for
	 * RF control and not available for ADC
	 */
	if (channel == ADC_CH2 || channel == ADC_CH3) {
		return -WM_FAIL;
	}
	GPIO_PinMuxFun((GPIO_42 + channel),
			 PINMUX_FUNCTION_1);
	return WM_SUCCESS;
}

void board_dac_pin_config(int channel)
{
	switch (channel) {
	case DAC_CH_A:
		/* For this channel GPIO 44 is needed
		 * GPIO 44 is reserved for  RF control
		 * on this module so channel DAC_CH_A
		 * should not be used.
		 */
		break;
	case DAC_CH_B:
		GPIO_PinMuxFun(GPIO_43, GPIO43_DACB);
		break;
	}
}

output_gpio_cfg_t board_led_1()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_40,
		.type = GPIO_ACTIVE_LOW,
	};
	return gcfg;
}

output_gpio_cfg_t board_led_2()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_41,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_3()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_4()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

int board_button_1()
{
	GPIO_PinMuxFun(GPIO_26, GPIO26_GPIO26);
	return GPIO_26;
}

int board_button_2()
{
	GPIO_PinMuxFun(GPIO_24, GPIO24_GPIO24);
	return GPIO_24;
}

int board_button_3()
{
	return -WM_FAIL;
}

int board_button_pressed(int pin)
{
	if (pin < 0)
		return false;

	GPIO_SetPinDir(pin, GPIO_INPUT);
	if (GPIO_ReadPinLevel(pin) == GPIO_IO_LOW)
		return true;

	return false;
}

int board_wakeup0_functional()
{
	return true;
}

int board_wakeup1_functional()
{
	return true;
}

unsigned int board_antenna_select()
{
	return 1;
}

