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
 *  ======== Timer.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "Timer");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "Timer Interrupt Priority";


let timerConfig = [
    {
        name        : "timerType",
        displayName : "Timer Type",
        default     : "16 Bits",
        options     : [
            { name: "16 Bits" },
            { name: "32 Bits" }
        ]
    },

    intPriority
];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - Timer instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base Timer properties and methods
 */
let base = {
    displayName         : "Timer",
    description         : "Timer Driver",
    longDescription     : `
The [__Timer driver__][1] allows you to manage a Timer peripheral via simple 
and portable APIs.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_timer_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_timer_8h.html#ti_drivers_Timer_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_timer_8h.html#ti_drivers_Timer_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Timer_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName : "CONFIG_TIMER_",
    config              : Common.addNameConfig(timerConfig, "/ti/drivers/Timer", "CONFIG_TIMER_"),
    modules             : Common.autoForceModules(["Board", "Power"]),
    validate            : validate
};

/* get family-specific Timer module */
let devTimer = system.getScript("/ti/drivers/timer/Timer" + family);

exports = devTimer.extend(base);
