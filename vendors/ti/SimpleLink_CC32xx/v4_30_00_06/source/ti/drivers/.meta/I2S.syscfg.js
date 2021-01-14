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
 *  ======== I2S.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "I2S");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Hardware Interrupt Priority";
intPriority.name = "interruptPriority";

let config = [
    intPriority,
    {
        name        : "sd1DataDirection",
        displayName : "SD1 Data Direction",
        description : "Serial Data Line",
        longDescription : "May also be referred to as the AD or McAXR.",
        default     : 'Input',
        options     : [
            { name:'Input'},
            { name:'Output'},
            { name:'Disabled'}
        ]
    },
    {
        name        : "sd0DataDirection",
        displayName : "SD0 Data Direction",
        description : "Serial Data Line",
        longDescription : "May also be referred to as the AD or McAXR",
        default     : 'Output',
        options     : [
            { name:'Input'},
            { name:'Output'},
            { name:'Disabled'}
        ]
    },
    {
        name        : "masterSlaveSelection",
        displayName : "Master Slave Selection",
        description : "Select the I2S module role",
        default     : 'Master',
        options     : [
            {
                name: 'Master',
                description: "Clocks internally generated"
            },
            {
                name: 'Slave',
                description: "Clocks externally generated"
            }
        ]
    }
];

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - I2S instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* Don't allow to disable both interfaces*/
    if((inst.sd0DataDirection == "Disabled") &&
        (inst.sd1DataDirection == "Disabled")) {
            Common.logInfo(validation, inst, "sd0DataDirection",
              "Consider activating one of the two data interfaces (SD0 or SD1)");
    }

    if((inst.sd0DataDirection == "Disabled") &&
        (inst.sd1DataDirection == "Disabled")) {
            Common.logInfo(validation, inst, "sd1DataDirection",
              "Consider activating one of the two data interfaces (SD0 or SD1)");
    }
}

/*
 *  ========= filterHardware ========
 *  Check 'component' signals for compatibility with I2S
 *
 *  @param component - hardware object describing signals and
 *                     resources they're attached to
 *  @returns matching pinmuxRequirement object if I2S is supported.
 */
function filterHardware(component)
{
    if (Common.typeMatches(component.type, ["I2S"])) {
        return (true);
    }

    return (false);
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
 *  ======== base ========
 *  Define the base I2S properties and methods
 */
let base = {
    displayName: "I2S",
    description: "Inter-Integrated Circuit Sound (I2S) Bus Driver",
    longDescription:`
The [__I2S driver__][1] provides a simplified application interface to access
peripherals on an I2S bus.

* [Examples][2]
* [Configuration Options][3]

[1]: /drivers/doxygen/html/_i2_s_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_i2_s_8h.html#ti_drivers_I2S_Examples "C usage examples"
[3]: /drivers/syscfg/html/ConfigDoc.html#I2S_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_I2S_",
    config: Common.addNameConfig(config, "/ti/drivers/I2S", "CONFIG_I2S_"),
    validate: validate,
    busModule: true,
    filterHardware: filterHardware,

    _getPinResources: _getPinResources
};

/* extend the base exports to include family-specific content */
let deviceI2S = system.getScript("/ti/drivers/i2s/I2S" + family);
exports = deviceI2S.extend(base);
