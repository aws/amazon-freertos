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
 *  ======== NVSSPI25X.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logWarning = Common.logWarning;

let config = [
    {
        name: "regionBase",
        displayName: "Region Base",
        description : "Base address of the region. Must be aligned an"
            + " integer multiple of sector size boundary.",
        displayFormat: "hex",
        default: 0x0
    },
    {
        name: "regionSize",
        displayName: "Region Size",
        description : "Size of the region in bytes. Must be a non-zero"
            + " multiple of the Sector Size",
        displayFormat: "hex",
        default: 0x1000
    },
    {
        name: "sectorSize",
        displayName: "Sector Size",
        description : "Size of the erase sector in bytes",
        displayFormat: "hex",
        default: 0x1000
    },
    {
        name: "verifyBufferSize",
        displayName: "Verify Buffer Size",
        description: "Size of the write verification buffer in bytes.",
        default: 0
    },
    {
        name: "statusPollDelay",
        displayName: "Status Poll Delay",
        description: "If the external flash responds with a busy status,"
            + " the driver waits this many microseconds before polling again.",
        default: 100
    }
];

/*
 *  ======== filterHardware ========
 */
function filterHardware(component) {
    if (component.type) {
        if (Common.typeMatches(component.type, ["SPI25X_FLASH"])) {
            return (true);
        }
    }
    return (false);
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui) {
    if (inst.$hardware) {
        /* Default to max size of spi flash if specified */
        if (inst.$hardware.settings && inst.$hardware.settings.capacity) {
            inst.regionSize = inst.$hardware.settings.capacity;
        }
    }
    else {
        /* restore to default regionSize */
        inst.regionSize = inst.$module.$configByName.regionSize.default;
    }
}

/*
 *  ======== validate ========
 */
function validate(inst, validation) {
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
    if (inst.statusPollDelay < 0 || inst.statusPollDelay > 0xFFFFFFFF) {
        let message = "Status Polling Delay must be a positive integer that"
            + " can be expressed as a 32 bit value.";
        logError(validation, inst, "statusPollDelay", message);
    }

    /*
     * verify that this region does not overlap within any other region
     * on the same SPI flash device
     */
    for (let i = 0; i < inst.$module.$instances.length; i++) {
        let tinst = inst.$module.$instances[i];
        if (inst == tinst) continue;

        /* only compare regions on the same SPI flash device */
        if (inst.spiFlashDevice != tinst.spiFlashDevice) continue;

        let tRegionBaseOffset = tinst.regionBase;
        let tRegionSize = tinst.regionSize;
        if ((regionBase >= tRegionBaseOffset) &&
            (regionBase < (tRegionBaseOffset + tRegionSize))) {
            let message = "Region Base overlaps with NVS region: " + tinst.$name + ".";
            logWarning(validation, inst, "regionBase", message);
            break;
        }
        if (((regionBase + regionSize) > tRegionBaseOffset) &&
            ((regionBase + regionSize) <= (tRegionBaseOffset + tRegionSize))) {
            let message = "Region Base + Region Size overlaps with NVS region: " + tinst.$name + ".";
            logWarning(validation, inst, "regionBase", message);
            break;
        }
    }
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst) {

    let sharedModules = [{
        name: "spiFlashDevice",
        displayName: "SPI Flash Device",
        moduleName: "/ti/drivers/nvs/NVSSPI25XDevice",
        hardware: inst.$hardware
    }];

    return (sharedModules);
}

/*
 *  ======== exports ========
 */
exports = {
    config: config,
    filterHardware: filterHardware,
    onHardwareChanged : onHardwareChanged,
    sharedModuleInstances: sharedModuleInstances,
    validate: validate
};
