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
 * ===== Comparator.syscfg.js ====
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "Comparator");

let config = [
    {
        name: "outputEnable",
        displayName: "Output Enable",
        default: true,
        description: "Request pin for Comparator output"
    }
];

/*
 *  =========== filterHardware ============
 *  @param component - hardware object describing signals
 *                    and resources
 *  @returns Boolean indicating whether or not to allow the component
 *          be asign to an instance's $hardware config
 */
function filterHardware(component)
{
    return (false);
}

/*
 *  =========== validate ==========
 *  Validate this instance's configuration
 *
 *  @param inst         - Comparator instance to be validated
 *  @param validation   - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

function _getPinResources()
{
    return;
}

/*
 *  ====== base ======
 *  Define the base Comparator properties and methods
 */
let base = {
    displayName: "Comparator",
    description: "Analog Comparator Driver",
    longDescription: `
The [__Comparator driver__][1] provides device independent APIs for reading
and writing to the Comparator peripherals.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_comparator_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_comparator_8h.html#ti_drivers_Comparator_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_comparator_8h.html#ti_drivers_Comparator_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Comparator_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_COMPARATOR_",
    config: Common.addNameConfig(config, "/ti/drivers/Comparator", "CONFIG_COMPARATOR_"),
    validate: validate,
    filterHardware: filterHardware,
    modules: Common.autoForceModules(["Board", "Power"]),
    _getPinResources: _getPinResources
};


/* extend the base exports to include family-specific content */
let deviceComparator = system.getScript("/ti/drivers/comparator/Comparator" + family);
exports = deviceComparator.extend(base);
