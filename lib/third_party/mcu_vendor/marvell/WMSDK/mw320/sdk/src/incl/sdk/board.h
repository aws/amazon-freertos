/*
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

/** @file board.h
*
*  @brief This file contains Board Specific APIs
*
*  The board specific APIs are provided to help
*  with porting the SDK onto new boards. These
*  APIs are board specific and they need to be
*  modified/implemented according to the actual
*  board specifications.
*
*  Please verify that {board_file}.c file found under sdk/src/boards/
*  reflects the correct hardware configuration for the project.
*/


#ifndef __BOARD_H__
#define __BOARD_H__

#include <wmtypes.h>
#include <lowlevel_drivers.h>
#include <generic_io.h>

typedef enum {
	/** US FCC */
	BOARD_COUNTRY_US = 1,
	/** IC Canada */
	BOARD_COUNTRY_CA,
	/** Singapore */
	BOARD_COUNTRY_SG,
	/** ETSI */
	BOARD_COUNTRY_EU,
	/** Australia */
	BOARD_COUNTRY_AU,
	/** Republic Of Korea */
	BOARD_COUNTRY_KR,
	/** France */
	BOARD_COUNTRY_FR,
	/** Japan */
	BOARD_COUNTRY_JP,
	/** China */
	BOARD_COUNTRY_CN,
} board_country_code_t;

struct pwr_table {
	const board_country_code_t country;
	const uint8_t num_of_channels;
	const uint8_t **table;
};

/** Frequency of Main Crystal
 *
 * The MC200/MW300 processor can be clocked through
 * either of the following 3 sources:<br>
 * 1) Externally connected crystal.<br>
 * 2) Externally connected oscillator.<br>
 * 3) Internal RC oscillator.<br>
 *
 * If the board has an external crystal for
 * main cpu clocking, mention it's frequency
 * here.
 *
 *  \return Frequency of external crystal if it is
 * present, -WM_FAIL otherwise.
 */
int board_main_xtal();

/** Frequency of Main Oscillator
 *
 * The MC200/MW300 processor can be clocked through
 * either of the following 3 sources:<br>
 * 1) Externally connected crystal.<br>
 * 2) Externally connected oscillator.<br>
 * 3) Internal RC oscillator.<br>
 *
 * If the board has an external oscillator for
 * main cpu clocking, mention it's frequency
 * here.
 *
 *  \return Frequency of external oscillator if it is
 * present, -WM_FAIL otherwise.
 */
int board_main_osc();

/** Frequency at which CPU should run
 *
 * Mention the frequency at which the CPU should
 * run here. If this frequency is greater than the
 * source clock, internal PLL will be configured.<br>
 *
 * NOTE: Valid frequencies depend upon the source clock.
 * Please refer to the datasheet for further details.
 *
 *  \return Frequency at which CPU should operate.
 */
int board_cpu_freq();

/** 32 KHz crystal
 *
 * If 32 KHz crystal is present on the board and
 * is functional, return true here.
 *
 *  \return true if 32 KHz crystal is present and
 *  functional, false otherwise.
 */
int board_32k_xtal();

/** 32 KHz Oscillator
 *
 * If 32 KHz oscillator is present on the board and
 * is functional, return true here.
 *
 *  \return true if 32 KHz oscillator is present and
 *  functional, false otherwise.
 */
int board_32k_osc();

/** 32 KHz Internal RC Calibration
 *
 * If 32 KHz crystal or oscillator is not present. The
 * internal rc32k can be calibrated using GPT.
 * NOTE1: On MW300 this needs the GPIO24 and GPIO25 to
 * be connected.
 * NOTE2: On MC200 this will work only if 32MHz crystal
 * is present on the board.
 *
 *  \return true if RC32K should be calibrated using GPT
 */
int board_rc32k_calib();

/** Detect the SDIO card (only for 88MC200)
 *
 * If there is a mechanism to detect the SDIO card on
 * the board, use it here, or else, always return the
 * default condition.
 *
 *  \return TRUE if card is present, FALSE otherwise
 */
int board_card_detect();

/** Powerdown the SDIO card (only for 88MC200)
 *
 * If there is a GPIO connected to PDn
 * of the SDIO card, ASSERT it in this function.
 *
 * Leave blank if not connected.
 */
void board_sdio_pdn();

/** Powerup the SDIO card (only for 88MC200)
 *
 * If there is a GPIO connected to PDn
 * of the SDIO card, DE-ASSERT it in this function.
 *
 * Leave blank if not connected.
 */
void board_sdio_pwr();

/** Reset the SDIO card (only for 88MC200)
 *
 * If there is a GPIO connected to RST
 * of the SDIO card, ASSERT it for some
 * duration and DE-ASSERT it again.
 *
 * Leave blank if not connected.
 *
 */
void board_sdio_reset();

/** SDIO pin config (only for 88MC200)
 *
 * Select which pins will be used for SDIO controller
 *
 * Leave Blank if SDIO is not connected on the board.
 */
void board_sdio_pin_config();

/** Power On GPIO settings
 *
 * Do board specific power GPIO settings
 * here.
 */
void board_gpio_power_on();

/** UART pin config
 *
 * Select which pins will be used for different UART ports
 *
 * Leave Blank if UART is not implemented on the board.
 *
 * \param[in] id UART port number
 */
void board_uart_pin_config(int id);

/** I2C pin config
 *
 * Select which pins will be used for different I2C ports
 *
 * Leave Blank if I2C is not implemented on the board.
 *
 * \param[in] id I2C port number
 */
void board_i2c_pin_config(int id);

/** SSP pin config
 *
 * Select which pins will be used for different SSP ports
 *
 * Leave Blank if SSP is not implemented on the board.
 *
 * \param[in] id SSP port number
 * \param[in] cs If internal SSPx_FRM (cs=1) should be activated or not.
 */

void board_ssp_pin_config(int id, int cs);

/**
 * Select pin configuration for ADC
 * \param [in] adc_id
 * \param [in] channel number
 */
int board_adc_pin_config(int adc_id, int channel);

/**
 * Select pin configuration for DAC
 * \param [in] channel number
 */
void board_dac_pin_config(int channel);
/**
 * Select pin configuration for USB
 * */
void board_usb_pin_config();

/** Pin number and configuration of
 *  GPIO connected to LED 1
 *
 * \note If LED is not present on the board
 * gpio member in \ref output_gpio_cfg_t is set to -1 by this API.
 * \return Output GPIO Configuration type \ref output_gpio_cfg_t
 */
output_gpio_cfg_t board_led_1();

/** Pin number and configuration of
 *  GPIO connected to LED 2
 *
 * \note If LED is not present on the board
 * gpio member in \ref output_gpio_cfg_t is set to -1 by this API.
 * \return Output GPIO Configuration type \ref output_gpio_cfg_t
 */
output_gpio_cfg_t board_led_2();

/** Pin number and configuration of
 *  GPIO connected to LED 3
 *
 * \note If LED is not present on the board
 * gpio member in \ref output_gpio_cfg_t is set to -1 by this API.
 * \return Output GPIO Configuration type \ref output_gpio_cfg_t
 */
output_gpio_cfg_t board_led_3();

/** Pin number and configuration of
 *  GPIO connected to LED 4
 *
 * \note If LED is not present on the board
 * gpio member in \ref output_gpio_cfg_t is set to -1 by this API.
 * \return Output GPIO Configuration type \ref output_gpio_cfg_t
 */
output_gpio_cfg_t board_led_4();

/** WPS Pushbutton
 *
 * \return GPIO connected to the pushbutton
 * to be used for WPS functionality or -WM_FAIL
 * if such pushbutton is not implemented on the board.
 */
int board_button_1();

/** Reset to Provisioning Pushbutton
 *
 * \return GPIO connected to the pushbutton
 * to be used for Reset to Provisioning mode
 * functionality or -WM_FAIL if such pushbutton
 * is not implemented on the board.
 */
int board_button_2();

/** Boot Override Pushbutton
 *
 * \return GPIO pin number connected to the
 * pushbutton to be used for Boot override mode
 * functionality or -WM_FAIL if such pushbutton
 * is not implemented on the board.
 */
int board_button_3();

/** Push Button Pressed
 *
 * Depending upon how the push button is
 * connected, corresponding GPIO needs to
 * be checked for either set or reset.
 *
 * If no pushbuttons are implemented on the board,
 * always return FALSE (i.e. button not pressed).
 *
 * \param[in] pin GPIO pin to be used
 * \return TRUE if pressed, FALSE otherwise
 */
int board_button_pressed(int pin);

/**
 *  This function  indicates whether  wakeup 0 pin is functional
 *   or not on this board
 *   @return   True or False
 */
int board_wakeup0_functional();

/**
 *  This function  indicates whether  wakeup 1 pin is functional
 *   or not on this board
 *   @return   True or False
 */
int board_wakeup1_functional();

/*
 * Returns  GPIO pin of WIFI card that is used for
 * host wake up on WLAN feature. (only for 88MC200)
 * @return pin_no  used for wakeup
 *        -WM_FAIL in case no pin available
 */
int board_wifi_host_wakeup();

/**
 *  This function indicates whether wakeup 0 pin is connected
 *  to the WiFi chip's host wakeup pin. (only for 88MC200)
 *   @return   True or False
 */
int board_wakeup0_wifi();

/**
 *  This function indicates whether wakeup 1 pin is connected
 *  to the WiFi chip's host wakeup pin. (only for 88MC200)
 *   @return   True or False
 */
int board_wakeup1_wifi();

/**
 * This function indicates whether board has an antenna control
 * switch or not.
 * @return True or False
 * @note If this is set to false
 *	a. Antenna diversity will be unavailable
 *	b. GPIO pins used for RF Ctrl should not be
 *	programmed in board_gpio_power_on()
 */
int board_antenna_switch_ctrl();

/**
 * This function indicates whether antenna 1 is used or
 * antenna 2 is used or both are used with diversity.
 *   @return   1 or 2 or 65535
 */
unsigned int board_antenna_select();

/**
 * This function indicates which UART port is used to interface BLE SoC
 *   @return   UART_ID_Type
 */
UART_ID_Type board_ble_uart_id();

/**
 * This function indicates which I2C port is used by audio codec peripheral
 *   @return   I2C_ID_Type
 */
I2C_ID_Type board_acodec_i2c_id(void);

/**
 * This function indicates which SSP port is used by audio codec peripheral
 *   @return   SSP_ID_Type
 */
SSP_ID_Type board_acodec_ssp_id(void);

/* This macro defines the SFLL frequency
 * for the board
 */
#define CHIP_SFLL_FREQ() (board_cpu_freq())

/**
 * This function gets the power table as per the
 * country specified.
 *
 * \param[in] country Country for which power table requested.
 *
 * \return power table specific to the country, null if not available.
 */
struct pwr_table *board_region_pwr_tbl(board_country_code_t country);

/**
 * This function indicates whether COEX interface is active or not
 *   @return   TRUE if active or FALSE otherwise
 */
int board_coex_interface();

/**
 * This function configures coex interface pins
 */
void board_coex_pin_config();

#endif /* __BOARD_H__ */
