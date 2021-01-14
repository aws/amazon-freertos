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
 *  ======== ECJPAKE.syscfg.js ========
 *  Elliptic Curve Password Authenticated Key Exchange by Juggling
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "ECDH");

let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - ECJPAKE instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base ECJPAKE properties and methods
 */
let base = {
    displayName : "ECJPAKE",
    description : "Elliptic Curve Password Authenticated Key Exchange by"
        + " Juggling Driver",
    alwaysShowLongDescription : true,
    longDescription : `
The [__ECJPAKE driver__][1] provides a key agreement
scheme that establishes a secure channel over an insecure network.
It only requires sharing a password offline and does not require
public key infrastructure or trusted third parties such as
certificate authorities.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]
[1]: /drivers/doxygen/html/_e_c_j_p_a_k_e_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_e_c_j_p_a_k_e_8h.html#ti_drivers_ECJPAKE_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_e_c_j_p_a_k_e_8h.html#ti_drivers_ECJPAKE_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#ECJPAKE_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    defaultInstanceName : "CONFIG_ECJPAKE_",
    config              : Common.addNameConfig(config, "/ti/drivers/ECJPAKE", "CONFIG_ECJPAKE_"),
    modules             : Common.autoForceModules(["Board", "Power"])
};

/* extend the base exports to include family-specific content */
let devECJPAKE = system.getScript("/ti/drivers/ecjpake/ECJPAKE" + family);
exports = devECJPAKE.extend(base);
