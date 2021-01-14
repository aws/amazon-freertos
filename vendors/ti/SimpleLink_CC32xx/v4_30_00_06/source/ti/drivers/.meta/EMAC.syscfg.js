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
 *  ======== EMAC.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "EMAC");

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
 *  Define the base EMAC properties and methods
 */
let base = {
    displayName         : "EMAC",
    description         : "Ethernet Media Access Control (EMAC) Driver"
        +" Configuration",
    alwaysShowLongDescription : true,
    defaultInstanceName : "CONFIG_EMAC_",
    longDescription     :
`The [__EMAC driver__][1] is used by the
[__TI Network Developer's Kit (NDK)__][2] to provide networking capabilities.

* [Configuration Options][3]

[1]: /drivers/doxygen/html/_e_m_a_c_m_s_p432_e4_8h.html#details "C API Reference"
[2]: /ndk/NDK_Users_Guide.html#overview "NDK User's Guide"
[3]: /drivers/syscfg/html/ConfigDoc.html#EMAC_Configuration_Options "Configuration options reference"
`,
    validate            : validate,
    modules: Common.autoForceModules(["Board", "Power", "DMA"]),

    _getPinResources: _getPinResources
};

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  @param inst       - EMAC instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/* extend the base exports to include family-specific content */
let devEMAC = system.getScript("/ti/drivers/emac/EMAC" + family);
exports = devEMAC.extend(base);
