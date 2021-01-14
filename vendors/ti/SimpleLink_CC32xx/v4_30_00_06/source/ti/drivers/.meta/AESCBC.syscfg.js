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
 *  ======== AESCBC.syscfg.js ========
 *  AES (Advanced Encryption Standard) Cipher Block Chaining
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "AESCBC");

let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - AESCBC instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base AESCBC properties and methods
 */
let base = {
    displayName         : "AESCBC",
    description         : "AES (Advanced Encryption Standard) Cipher"
        + " Block Chaining Driver",
    alwaysShowLongDescription : true,
    longDescription     : `
The [__AESCBC driver__][1] encrypts messages of any practical
length that have a length evenly divisibly by the block size.
Unlike ECB, it guarantees confidentiality of the entire message.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_a_e_s_c_b_c_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_a_e_s_c_b_c_8h.html#ti_drivers_AESCBC_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_a_e_s_c_b_c_8h.html#ti_drivers_AESCBC_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#AESCBC_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    defaultInstanceName : "CONFIG_AESCBC_",
    config              : Common.addNameConfig(config, "/ti/drivers/AESCBC", "CONFIG_AESCBC_"),
    modules             : Common.autoForceModules(["Board", "DMA", "Power"])
};

/* extend the base exports to include family-specific content */
let devAESCBC = system.getScript("/ti/drivers/aescbc/AESCBC" + family);
exports = devAESCBC.extend(base);
