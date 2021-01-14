/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 *  ======== Power.syscfg.js ========
 */

"use strict";

let Common = system.getScript("/ti/drivers/Common.js");
let family = Common.device2Family(system.deviceData, "Power");

/*
 *  ======== base ========
 *  Define the base Power properties and methods
 */
let base = {
    displayName         : "Power",
    description         : "Power Driver",
    longDescription: `
The [__Power driver__][1] facilitates the transition of the MCU from active
states to one of the sleep states and vice
versa. The Power driver enables the other driver modules to minimize power
consumption. In addition, applications can set or release power
constraints to prevent the MCU from transitioning into a particular
sleep state.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_power_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_power_8h.html#ti_drivers_Power_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_power_8h.html#ti_drivers_Power_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Power_Configuration_Options "Configuration options reference"
`,
    maxInstances        : 1,
    initPriority        : -1 /* lower numbers init earlier */
};

/* get family-specific Power module */
let devicePower = system.getScript("/ti/drivers/power/Power" + family);
exports = devicePower.extend(base);
