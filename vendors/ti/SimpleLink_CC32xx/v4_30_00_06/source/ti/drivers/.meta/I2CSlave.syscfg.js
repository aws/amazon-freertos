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
 *  ======== I2CSlave.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "I2CSlave");

let config = [];

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let i2cSlave = {
        name: "i2c",
        displayName: "I2C Peripheral",
        interfaceName: "I2C",
        resources: [
            {
                name: "sdaPin",
                displayName: "SDA Pin",
                interfaceNames: ["SDA"]
            },
            {
                name: "sclPin",
                displayName: "SCL Pin",
                interfaceNames: ["SCL"]
            }
        ]
    };

    return ([i2cSlave]);
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with I2CSlave
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinRequirement object if I2CSlave is supported.
 */
function filterHardware(component)
{
    let sda, scl;

    for (let sig in component.signals) {
        let type = component.signals[sig].type;
        if (Common.typeMatches(type, ["I2C_SCL"])) scl = sig;
        if (Common.typeMatches(type, ["I2C_SDA"])) sda = sig;
    }

    if (scl && sda) {
        return ({
            i2c: {
                sdaPin: [sda],
                sclPin: [scl]
            }
        });
    }

    return null;
}

/*
 *  ======== _getPinResources ========
 */
/* istanbul ignore next */
function _getPinResources(inst)
{
    return;
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - I2CSlave instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "I2C Slave",
    description: "Inter-Integrated Circuit (I2C) Slave Driver",
    longDescription:`
 The [__I2C Slave driver__][1] allows you to send and recieve I2C transfers
 from an I2C master.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_i2_c_slave_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_i2_c_slave_8h.html#ti_drivers_I2CSlave_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_i2_c_slave_8h.html#ti_drivers_I2CSlave_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#I2CSlave_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_I2CSLAVE_",
    config: Common.addNameConfig(config, "/ti/drivers/I2CSlave", "CONFIG_I2CSLAVE_"),
    validate: validate,
    modules: Common.autoForceModules(["Board", "Power"]),
    busModule: true, /* true => instances of this module can be shared */
    filterHardware: filterHardware,
    pinmuxRequirements: pinmuxRequirements,

    _getPinResources: _getPinResources
};

/* extend the base exports to include family-specific content */
let deviceI2CSlave = system.getScript("/ti/drivers/i2cslave/I2CSlave" + family);
exports = deviceI2CSlave.extend(base);
