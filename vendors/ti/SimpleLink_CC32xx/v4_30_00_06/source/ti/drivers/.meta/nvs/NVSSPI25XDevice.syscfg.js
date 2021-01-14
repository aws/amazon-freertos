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
 *  ======== NVSSPI25XDevice.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;

let config = [
    {
        name: "manager",
        displayName: "Manager",
        description: "Manager of the SPI driver instance.",
        default: "NVS",
        options: [
            {
                name: "NVS",
                description: "NVS driver internally creates a SPI instance."
            },
            {
                name: "User",
                description: "User responsible for a SPI instance."
            }
        ],
        onChange: updateConfigs
    },
    {
        name: "slaveSelectManager",
        displayName: "Slave Select Manager",
        description: "Manager of the SPI slave select pin",
        default: "NVS",
        options: [
            {
                name: "NVS",
                description: "NVS driver asserts SPI flash slave select."
            },
            {
                name: "SPI",
                description: "SPI driver asserts SPI flash slave select."
            },
            {
                name: "User",
                description: "User application asserts SPI flash slave select."
                + " The user application must include implementations to"
                + " NVSSPI25X_initSpiCs, NVSSPI25X_deinitSpiCs,"
                + " NVSSPI25X_assertSpiCs, and NVSSPI25X_deassertSpiCs"
            }
        ],
        onChange: updateConfigs
    },
    {
        name: "handle",
        displayName: "Handle",
        hidden: true,
        description: "The symbol name of the user provided SPI Handle",
        placeholder : "Enter the SPI Handle name",
        default: ""
    },
    {
        name: "bitRate",
        displayName: "Bit Rate",
        description: "Specifies the bit rate in bits per second.",
        default: 4000000
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
 *  ======== updateConfigs ========
 */
function updateConfigs(inst, ui) {
    if (inst.manager == "User") {
        ui.handle.hidden = false;
        ui.bitRate.hidden = true;
    } else {
        ui.handle.hidden = true;
        ui.bitRate.hidden = false;
    }
}

/*
 *  ======== validate ========
 */
function validate(inst, validation) {
    if (inst.manager == "User") {
        if ((inst.handle == "NULL") || (inst.handle == "")) {
            let message = "Must be the name of a global SPI Handle.";
            logWarning(validation, inst, "handle", message);
        }
    }
    else if (inst.handle != "") {
        let message = "Must be an empty string when the NVS driver manages the SPI instance.";
        logWarning(validation, inst, "handle", message);
    }

}

/*
 *  ======== gpioInit ========
 */
function gpioInit() {
    return ({
        comment: "%l    /* SPI Flash Slave Select GPIO Instance */",
        mode: "Output",
        outputType: "Standard",
        initialOutputState: "High"
    });
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst) {
    let modules = new Array();
    /* Add GPIO instance if NVS driver asserts SS */
    if (inst.slaveSelectManager == "NVS") {
        modules.push(
            {
                name: "slaveSelectGpioInstance",
                displayName: "Slave Select GPIO Instance",
                moduleName: "/ti/drivers/GPIO",
                args: gpioInit(),
                hardware: inst.$hardware ? inst.$hardware.subComponents.SELECT : null
            }
        );
    }

    if (inst.manager == "NVS" && inst.slaveSelectManager == "SPI") {
        modules.push(
            {
                name: "spiInstance",
                displayName: "SPI Flash SPI Instance",
                moduleName: "/ti/drivers/SPI",
                hardware: inst.$hardware ? inst.$hardware : null,
                args: { mode: "Four Pin SS Active Low" }
            }
        );
    }
    else if (inst.manager == "NVS" && inst.slaveSelectManager == "User") {
        modules.push(
            {
                name: "spiInstance",
                displayName: "SPI Flash SPI Instance",
                moduleName: "/ti/drivers/SPI",
                hardware: inst.$hardware ? inst.$hardware : null,
                requiredArgs: { mode: "Three Pin" }
            }
        );
    }

    return (modules);
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui) {

    /*
     * If a SPI flash device is passed to us, we should always use the provided
     * SPI bus. Therefore, the SPI instance will be managed by NVS. Otherwise,
     * we would be "throwing away" the hardware by not using it.
     */
    if (inst.$hardware) {
        inst.manager = "NVS";
        ui.manager.readOnly = true;
    }
    else {
        ui.manager.readOnly = false;
    }
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst) {
    let sharedModules = new Array();

    /* Add SPI instance if NVS driver opens the SPI */
    if (inst.manager == "NVS" && inst.slaveSelectManager == "NVS") {
        sharedModules.push(
            {
                name: "sharedSpiInstance",
                displayName: "Shared SPI Flash Instance",
                moduleName: "/ti/drivers/SPI",
                hardware: inst.$hardware ? inst.$hardware.subComponents.SPI : null
            }
        );
    }

    return (sharedModules);
}

/*
 *  ======== exports ========
 */
exports = {
    config: config,
    defaultInstanceName: "CONFIG_NVS_SPI_",
    filterHardware: filterHardware,
    sharedModuleInstances: sharedModuleInstances,
    moduleInstances: moduleInstances,
    onHardwareChanged: onHardwareChanged,
    validate: validate
};
