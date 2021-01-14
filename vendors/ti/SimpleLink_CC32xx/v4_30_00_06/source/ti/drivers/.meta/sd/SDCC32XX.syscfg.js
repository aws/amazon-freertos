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
 *  ======== SDCC32XX.syscfg.js ========
 */

"use strict";

let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "SDHost Interrupt Priority";

let convertPinName = Common.cc32xxPackage2DevicePin;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SD configuration
 */
let devSpecific = {
    config: [
        {
            name: "clockRate",
            displayName: "Clock Rate",
            description: "The SD interface clock rate",
            default: 8000000
        },
        {
            name: "interfaceType",
            displayName: "Interface Type",
            description: "Select the SD driver interface",
            default: "SD Host",
            options: [
                { name: "SD SPI" },
                { name: "SD Host" }
            ],
            onChange: interfaceChange
        },
        {
            name: "sdImplementation",
            displayName: "SD Implementation",
            default: "SDHostCC32XX",
            options: [{name: "SDHostCC32XX"}, {name: "SDSPI"}],
            readOnly: true,
            description: "Displays SD delegates available for the " +
                system.deviceData.deviceId + " device and the Interface Type.",
            longDescription: "Displays SD delegates available for the " +
                system.deviceData.deviceId + " device and the " +
                "__Interface Type__.\n\n" + `
Since there is only one delegate for each Interface Type, it is a read-only
value that cannot be changed. Please refer to the
[__TI-Drivers implementation matrix__][0] for all available drivers and
documentation.

[0]: /drivers/doxygen/html/index.html#drivers
`
        },
        intPriority
    ],

    onHardwareChanged: onHardwareChanged,

    pinmuxRequirements: pinmuxRequirements,

    modules: Common.autoForceModules(["Board", "Power", "DMA"]),

    filterHardware: filterHardware,

    templates: {
        boardc: "/ti/drivers/sd/SDCC32XX.Board.c.xdt",
        boardh: "/ti/drivers/sd/SD.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.interfaceType === "SD SPI") {
        if (inst.spiInstance) {
            let mod = system.getScript("/ti/drivers/SPI.syscfg.js");
            pin = mod._getPinResources(inst.spiInstance);
        }

        if (inst.slaveSelect) {
            let mod = system.getScript("/ti/drivers/GPIO.syscfg.js");
            pin += "SS: " + mod._getPinResources(inst.slaveSelect);
        }
    }
    else {
        if (inst.sdHost) {
            let clkPin = "P" + convertPinName(inst.sdHost.clkPin.$solution.packagePinName);
            let cmdPin = "P" + convertPinName(inst.sdHost.cmdPin.$solution.packagePinName);
            let dataPin = "P" + convertPinName(inst.sdHost.dataPin.$solution.packagePinName);

            pin = "\nDATA: " + dataPin + " \nCMD: "
                + cmdPin + "\nCLK: " + clkPin;

            if (inst.$hardware && inst.$hardware.displayName) {
                pin += "\n" + inst.$hardware.displayName;
            }
        }
    }

    return (pin);
}

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    if (inst.interfaceType === "SD Host") {
        let sdHost = {
            name: "sdHost",
            displayName: "SD Host Peripheral",
            interfaceName: "SDHost",
            hidden: true,
            resources: [
                {
                    name: "clkPin",
                    displayName: "CLK Pin",
                    interfaceNames: ["CLK"]
                },
                {
                    name: "cmdPin",
                    displayName: "CMD Pin",
                    interfaceNames: ["CMD"]
                },
                {
                    name: "dataPin",
                    displayName: "DATA Pin",
                    interfaceNames: ["DATA"]
                },
                {
                    name: "dmaTxChannel",
                    displayName: "DMA TX Channel",
                    interfaceNames: ["DMA_TX"]
                },
                {
                    name: "dmaRxChannel",
                    displayName: "DMA RX Channel",
                    interfaceNames: ["DMA_RX"]
                }
            ],

            signalTypes: {
                clkPin: ["SDHost_CLK"],
                cmdPin: ["SDHost_CMD"],
                dataPin: ["SDHost_DATA"]
            }
        };

        return ([sdHost]);
    }

    return ([]);
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component)
{

    if (component.type) {
        /* Check for know component types */
        if (Common.typeMatches(component.type, ["SD_SPI_FLASH", "SD_HOST"])) {
            return (true);
        }
    }

    /* Check for other types with know SD signals */
    if (Common.findSignalTypes(component,
        ["SDHost_CLK", "SDHost_CMD", "SDHost_DATA"])) {
        return (true);
    }

    return (false);
}

/*
 *  ======== interfaceChange ========
 */
function interfaceChange(inst, ui)
{
    if (inst.interfaceType === "SD Host") {
        ui.interruptPriority.hidden = false;
        ui.clockRate.hidden = false;
        inst.sdImplementation = inst.$module.$configByName.sdImplementation.default;
    }
    else {
        ui.interruptPriority.hidden = true;
        ui.clockRate.hidden = true;
        inst.sdImplementation = "SDSPI";
    }

    if (inst.$hardware) {
        ui.interfaceType.readOnly = true;
    }
    else {
        ui.interfaceType.readOnly = false;
    }
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {

        let component = inst.$hardware;

        if (Common.typeMatches(component.type, ["SD_HOST"])) {
            inst.interfaceType = "SD Host";
        } else if (Common.typeMatches(component.type, ["SD_SPI_FLASH"])) {
            inst.interfaceType = "SD SPI";
        }
    }
    else {
        /* Set defaults */
        inst.interfaceType = "SD Host";
        inst.clockRate = 8000000;
    }

    interfaceChange(inst, ui);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic SD module to
 *  allow us to augment/override as needed for the CC32XX.
 */
function extend(base)
{
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);
    result.templates = Object.assign({}, base.templates, devSpecific.templates);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base SD module */
    extend: extend
};
