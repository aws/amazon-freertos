/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _STATE_BINDING_H
#define _STATE_BINDING_H

enum state_binding {
	ONPOWERUP = 0x01,
	ONOFF,
	LEVEL,
	DELTA_LEVEL,
	ACTUAL,
	LINEAR,
	CTL,
	IGNORE,

	ONOFF_TEMP,
	LEVEL_TEMP,
	CTL_TEMP,
	IGNORE_TEMP
};

extern u16_t lightness, target_lightness;
extern s16_t temperature, target_temperature;

void state_binding(u8_t lightness, u8_t temperature);
void calculate_lightness_target_values(u8_t type);
void calculate_temp_target_values(u8_t type);

#endif
