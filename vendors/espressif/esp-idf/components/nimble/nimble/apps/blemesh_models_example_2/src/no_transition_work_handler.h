/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NO_TRANSITION_WORK_HANDLER_H
#define _NO_TRANSITION_WORK_HANDLER_H

extern struct ble_npl_callout no_transition_work;

void no_transition_work_init(void);

#endif
