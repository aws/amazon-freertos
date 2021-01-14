/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== CAN.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "CAN");

/* generic configuration parameters for CAN instances */
let config = [
    {
        name: "baudRate",
        displayName: "Baudrate",
        description: "Specifies the baudrate.",
        default: "125000"
    },
    {
        name: "errorFxn",
        displayName: "Error Function",
        description: "Error Function",
        longDescription: "Application error function to be called on receive "
            + "errors",
        default: "NULL"
    }
];

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - CAN instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* callbackFunction must be valid C identifier */
    if (!Common.isCName(inst.errorFxn)) {
        let message = "'" + inst.errorFxn
            + "' is not a valid a C identifier";
        Common.logError(validation, inst, "errorFxn", message);
    }
}

/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "CAN",
    description: "Controller Area Network(CAN) Bus Driver",
        longDescription: `
The [__CAN driver__][1] provides a portable application
interface to control onboard Controller Area Network (CAN).

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_c_a_n_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_c_a_n_8h.html#ti_drivers_CAN_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_c_a_n_8h.html#ti_drivers_CAN_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#CAN_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_CAN",
    config: Common.addNameConfig(config, "/ti/drivers/CAN", "CONFIG_CAN"),
    validate: validate,
    busModule: true,
    allowStaticOnly: true,
    modules: Common.autoForceModules(["Board", "Power"])
};

/* extend the base exports to include family-specific content */
let deviceCAN = system.getScript("/ti/drivers/can/CAN" + family);
exports = deviceCAN.extend(base);
