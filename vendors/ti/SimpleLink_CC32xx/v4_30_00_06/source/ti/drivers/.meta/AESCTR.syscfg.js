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
 *  ======== AESCTR.syscfg.js ========
 *  AES (Advanced Encryption Standard) Counter Mode of Operation
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "AESCTR");

let config = [];

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - AESCTR instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== base ========
 *  Define the base AESCTR properties and methods
 */
let base = {
    displayName         : "AESCTR",
    description         : "AES (Advanced Encryption Standard) Counter Mode Driver",
    alwaysShowLongDescription : true,
    longDescription     : `
The [__AESCTR driver__][1] encrypts messages of any practical
length. Unlike AESECB, it guarantees confidentiality of the
entire message when the message is larger than one block.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]
[1]: /drivers/doxygen/html/_a_e_s_c_t_r_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_a_e_s_c_t_r_8h.html#ti_drivers_AESCTR_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_a_e_s_c_t_r_8h.html#ti_drivers_AESCTR_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#AESCTR_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    defaultInstanceName : "CONFIG_AESCTR_",
    config              : Common.addNameConfig(config, "/ti/drivers/AESCTR", "CONFIG_AESCTR_"),
    modules: Common.autoForceModules(["Board", "Power", "DMA"])
};

/* extend the base exports to include family-specific content */
let devAESCTR = system.getScript("/ti/drivers/aesctr/AESCTR" + family);
exports = devAESCTR.extend(base);
