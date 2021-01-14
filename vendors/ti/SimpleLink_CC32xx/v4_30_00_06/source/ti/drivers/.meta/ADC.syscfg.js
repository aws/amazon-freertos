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
 *  ======== ADC.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ADC");

let config = [];

/*
 *  ========= filterHardware ========
 *  param component - hardware object describing signals and
 *                    resources
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    for (let sig in component.signals) {
        if (component.signals[sig].type == "AIN") {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - ADC instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
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
 *  Define the base ADC properties and methods
 */
let base = {
    displayName: "ADC",
    description: "Analog to Digital Conversion (ADC) Input Driver",

    longDescription: `
The [__ADC driver__][1] allows you to manage an Analog to Digital peripheral
via simple and portable APIs.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_a_d_c_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_a_d_c_8h.html#ti_drivers_ADC_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_a_d_c_8h.html#ti_drivers_ADC_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#ADC_Configuration_Options "Configuration options reference"
`,

    defaultInstanceName: "CONFIG_ADC_",
    config: Common.addNameConfig(config, "/ti/drivers/ADC", "CONFIG_ADC_"),
    validate: validate,
    filterHardware: filterHardware,
    modules: Common.autoForceModules(["Board"]),

    _getPinResources: _getPinResources
};

/* extend the base exports to include family-specific content */
let deviceADC = system.getScript("/ti/drivers/adc/ADC" + family);
exports = deviceADC.extend(base);
