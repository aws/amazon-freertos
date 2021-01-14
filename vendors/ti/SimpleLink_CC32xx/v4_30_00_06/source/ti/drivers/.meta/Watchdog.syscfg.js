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
 *  ======== Watchdog.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "Watchdog");
let logError = Common.logError;

/* Array of Watchdog configurables that are common across device families */
let config = [
    {
        name        : "period",
        displayName : "Period",
        description : "Watchdog timer period in milliseconds",
        default     : 1000
    }
];

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    return ([
        {
            name          : "watchdog",
            displayName   : "Watchdog Peripheral",
            interfaceName : "WATCHDOG",
            resources     : []
        }
    ]);
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - Watchdog instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    let period = parseInt(inst.period);
    let message;

    if (period < 0) {
        message = 'Period value: ' + period + ' can not be negative';
        logError(validation, inst, "period", message);
    }

    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base Watchdog properties and methods
 */
let base = {
    displayName         : "Watchdog",
    description         : "Watchdog Timer Driver",
    defaultInstanceName : "CONFIG_WATCHDOG_",
    longDescription     : `
The [__Watchdog driver__][1] allows you to configure the system watchdog.
A watchdog timer is used to reset the system if it has become unresponsive.
Parameters such as reset behavior and the callback function can be
[configured][2] at runtime.
* [Usage Synopsis][3]
* [Examples][4]
* [Configuration][5]
[1]: /drivers/doxygen/html/_watchdog_8h.html#details "C API reference"
[2]:
/drivers/doxygen/html/_watchdog_8h.html#ti_drivers_Watchdog_example_callback
"Example: Configure the watchdog"
[3]: /drivers/doxygen/html/_watchdog_8h.html#ti_drivers_Watchdog_Synopsis
"Basic C usage summary"
[4]: /drivers/doxygen/html/_watchdog_8h.html#ti_drivers_Watchdog_Examples
"C usage examples"
[5]: /drivers/syscfg/html/ConfigDoc.html#Watchdog_Configuration_Options "Configuration options reference"
`,
    modules             : Common.autoForceModules(["Board", "Power"]),
    config              : Common.addNameConfig(config, "/ti/drivers/Watchdog", "CONFIG_WATCHDOG_"),
    pinmuxRequirements  : pinmuxRequirements,
    validate            : validate
};

/* get family-specific Watchdog module */
let deviceWatchdog = system.getScript("/ti/drivers/watchdog/Watchdog" + family);
exports = deviceWatchdog.extend(base);
