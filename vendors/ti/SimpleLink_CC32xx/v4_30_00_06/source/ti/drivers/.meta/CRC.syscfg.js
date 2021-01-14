/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== CRC.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "CRC");

/* generic configuration parameters for PWM instances */
let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - CRC instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base CRC properties and methods
 */
let base = {
    displayName         : "CRC",
    description         : "Cyclic Redundancy Check (CRC) Driver",
    longDescription     : `
The [__CRC driver__][1] is a peripheral for calculating cyclic checksums from blocks of data.
CRC checks entire or partial blocks of bytes, and a number of common polynomials are
supported on each platform. Common CRC pre- and post-processing options are also supported.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_c_r_c_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_c_r_c_8h.html#ti_drivers_CRC_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_c_r_c_8h.html#ti_drivers_CRC_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#CRC_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName : "CONFIG_CRC_",
    config: Common.addNameConfig(config, "/ti/drivers/CRC", "CONFIG_CRC_"),
    validate : validate,
    modules: Common.autoForceModules(["Board", "Power"])
};


/* get family-specific CRC module */
let devCRC = system.getScript("/ti/drivers/crc/CRC" + family);
exports = devCRC.extend(base);
