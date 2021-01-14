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
 *  ======== AESCCM.syscfg.js ========
 *  AES (Advanced Encryption Standard) Counter with CBC-MAC
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "AESCCM");

let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - AESCCM instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base AESCCM properties and methods
 */
let base = {
    displayName         : "AESCCM",
    description         : "AES (Advanced Encryption Standard) Counter with"
        + " Cipher Block Chaining Message Authentication Code (CBC-MAC) Driver",
    alwaysShowLongDescription : true,
    longDescription     : `
The [__AESCCM driver__][1] combines CBC-MAC with an AES block cipher
in CTR mode of operation. This combination of block cipher modes
enables CCM to encrypt messages of any length and not only
multiples of the block cipher block size.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]
[1]: /drivers/doxygen/html/_a_e_s_c_c_m_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_a_e_s_c_c_m_8h.html#ti_drivers_AESCCM_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_a_e_s_c_c_m_8h.html#ti_drivers_AESCCM_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#AESCCM_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    defaultInstanceName : "CONFIG_AESCCM_",
    config              : Common.addNameConfig(config, "/ti/drivers/AESCCM", "CONFIG_AESCCM_"),
    modules: Common.autoForceModules(["Board", "Power", "DMA"])
};

/* extend the base exports to include family-specific content */
let devAESCCM = system.getScript("/ti/drivers/aesccm/AESCCM" + family);
exports = devAESCCM.extend(base);
