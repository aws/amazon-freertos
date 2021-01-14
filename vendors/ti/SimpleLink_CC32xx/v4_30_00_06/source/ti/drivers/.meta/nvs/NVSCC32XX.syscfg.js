/*
 * Copyright (c) 2018-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== NVSCC32XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

let config = [
    {
        name         : "regionBase",
        displayName  : "Region Base",
        description  : "Base address of the region. Must be aligned an"
            + " integer multiple of sector size boundary.",
        displayFormat: "hex",
        default      : 0x0
    },
    {
        name         : "regionSize",
        displayName  : "Region Size",
        description  : "Base address of the region. Must be aligned on an"
            + " integer multiple of the sector size.",
        longDescription : "NOTE: This setting has no effect when using the GCC"
            + " toolchain. The linker script must be manually modified if"
            + " the region must be at a specific location. See the nvsinternal"
            + " example README file for details.",
        displayFormat: "hex",
        default      : 0x1000
    },
    {
        name         : "sectorSize",
        displayName  : "Sector Size",
        description  : "Size of the erase sector in bytes",
        displayFormat: "hex",
        hidden       : false,
        readOnly     : true,
        default      : 0x1000
    }
];

/*
 *  ======== validate ========
 */
function validate(inst, validation)
{
    let regionBase = inst.regionBase;
    let regionSize = inst.regionSize;
    let sectorSize = inst.sectorSize;

    if (sectorSize == 0) {
        let message = "Invalid sectorSize value: " + inst.sectorSize + ".";
        logError(validation, inst, "sectorSize", message);
    }
    else if ((regionSize == 0) || (regionSize % sectorSize != 0)) {
        let message = "Invalid regionSize value: " + inst.regionSize + "." +
            "\nMust be a non-zero multiple of " + sectorSize + " bytes.";
        logError(validation, inst, "regionSize", message);
    }
    if (regionBase % sectorSize) {
        let message = "Region Base address must be aligned on a " + sectorSize
            + " page boundary.";
        logError(validation, inst, "regionBase", message);
    }

    /* verify that this region does not overlap within any other region */
    for (let i = 0; i < inst.$module.$instances.length; i++) {
        let tinst = inst.$module.$instances[i];
        if ((inst == tinst)) continue;
        let tRegionBase = tinst.regionBase;
        let tRegionSize = tinst.regionSize;
        if ((regionBase >= tRegionBase) &&
            (regionBase < (tRegionBase + tRegionSize))) {
            let message = "Region Base overlaps with NVS region: " + tinst.$ownedBy.$name + ".";
            logWarning(validation, inst, "regionBase", message);
            break;
        }
        if (((regionBase + regionSize) > tRegionBase) &&
            ((regionBase + regionSize) <= (tRegionBase + tRegionSize))) {
            let message = "Region Base + Region Size overlaps with NVS region: " + tinst.$ownedBy.$name + ".";
            logWarning(validation, inst, "regionBase", message);
            break;
        }
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This should only modify the base config
 */
 function extend(base)
 {
    let config = base.config;

    let typeLongDescription =`
* [__External__][2] allows you to configure a flash
region accessible via SPI.
* [__RAM__][3] allows you to configure in a CPU addressable internal RAM
region.

[2]: /drivers/syscfg/html/ConfigDoc.html#NVSSPI25X_Configuration_Options
[3]: /drivers/syscfg/html/ConfigDoc.html#NVSRAM_Configuration_Options
`;

    /*
     * Find and remove "Internal" from "nvsType". Make "External" default.
     * Find and remove "NVSCC32XX" from "nvsImplementation". Make "NVSSPI25X"
     * the default.
     */
    for (let i = 0; i < config.length; i++) {
        if (config[i].name === "nvsType") {
            for (let k = 0; k < config[i].options.length; k++) {
                if (config[i].options[k].name === "Internal") {
                    base.config[i].options.splice(k, 1);
                    base.config[i].default = "External";
                    break;
                }
            }
            base.config[i].longDescription = typeLongDescription;
        }
        if (config[i].name === "nvsImplementation") {
            for (let k = 0; k < config[i].options.length; k++) {
                if (config[i].options[k].name === "NVSCC32XX") {
                    base.config[i].options.splice(k, 1);
                    base.config[i].default = "NVSSPI25X";
                    break;
                }
            }
        }
    }

    return (base);
 }

exports = {
    config: config,
    validate: validate,
    extend: extend
};
