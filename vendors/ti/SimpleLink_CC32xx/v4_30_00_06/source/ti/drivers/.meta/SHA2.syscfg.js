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
 *  ======== SHA2.syscfg.js ========
 *  Secure Hash Algorithm
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "SHA2");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Crypto peripheral interrupt priority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Crypto interrupt Swi handler priority";

let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - SHA2 instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base SHA2 properties and methods
 */
let base = {
    displayName         : "SHA2",
    description         : "Secure Hash Algorithm (SHA2) Driver",
    longDescription     : `
The [__SHA2 driver__][1] provides access to the
SHA2 family of cryptographic hashes that map an
input of arbitrary length to a fixed-length output
with negligible probability of collision.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]
[1]: /drivers/doxygen/html/_s_h_a2_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_s_h_a2_8h.html#ti_drivers_SHA2_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_s_h_a2_8h.html#ti_drivers_SHA2_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#SHA2_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    defaultInstanceName : "CONFIG_SHA2_",
    config              : Common.addNameConfig(config, "/ti/drivers/SHA2", "CONFIG_SHA2_"),
    modules: Common.autoForceModules(["Board", "Power"])
};

/* get family-specific SHA2 module */
let devSHA2 = system.getScript("/ti/drivers/sha2/SHA2" + family);
exports = devSHA2.extend(base);
