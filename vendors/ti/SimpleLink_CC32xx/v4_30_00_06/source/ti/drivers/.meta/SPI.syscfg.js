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
 *  ======== SPI.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "SPI");

/* generic configuration parameters for SPI instances */
let config = [
    {
        name: "mode",
        displayName: "Mode",
        default: "Three Pin",
        description: "In three pin mode the SS signal is not used.",
        longDescription: "In __Three Pin__ mode, the user is responsible for"
            + " controlling the slave select line.",
        getDisabledOptions: disabledModeOptions,
        options: [
            { name: "Three Pin" },
            { name: "Four Pin SS Active Low" },
            { name: "Four Pin SS Active High" }
        ]
    },
    {
        name: "defaultTxBufferValue",
        displayName: "Default TX Buffer Value",
        description: "Specifies the default transmit buffer value.",
        longDescription: "Value sent when a TX buffer is not specified."
            + " This value must be provided in hexadecimal format;"
            + " 0 and ~0 also acceptable inputs.",
        default: "~0"
    },
    {
        name: "minDmaTransferSize",
        displayName: "Min DMA Transfer Size",
        description: "Specifies the minimum transfer size in bytes for which"
            + " the DMA will be used. Otherwise a polling transfer will occur"
            + " with some exceptions.",
        default: 10
    },
    {
        name: "duplex",
        displayName: "Duplex",
        hidden : false,
        description: "One or both device transmit/receive",
        longDescription: "In a typical SPI transfer,"
            + " both participants transmit and receive at the same time"
            + " (full duplex) but in some situations only one side will ever"
            + " transmit and the other side may only need to receive. In these"
            + " cases some pins can stay unassigned. If full duplex is not"
            + " required, the user may select the role of this SPI.",
        default: "Full",
        options: [
            { name: "Full" },
            { name: "Master TX Only" },
            { name: "Master RX Only" },
            { name: "Slave TX Only" },
            { name: "Slave RX Only" }

        ]
    }
];

/*
 *  ========= disabledModeOptions ========
 *  Selectively disables "mode" options if there is hardware. This
 *  is used instead of "onHardwareChanged()" so that we can give
 *  the user flexibility to select between the available Four Pin modes.
 */
function disabledModeOptions(inst)
{
    if (inst.$hardware) {

        /*
         * 3 Pin hardware can be used in 4 Pin mode; however, the user
         * would be responsible for manually selecting the SS pin.
         */
        if (!Common.findSignalTypes(inst.$hardware, ["SPI_SS"])) {
            return ([
                {
                    name: "Four Pin SS Active Low",
                    reason: "Disabled by " + inst.$hardware.displayName +
                    ". See: " + system.getReference(inst, "$hardware")
                },
                {
                    name: "Four Pin SS Active High",
                    reason: "Disabled by " + inst.$hardware.displayName +
                    ". See: " + system.getReference(inst, "$hardware")
                }
            ]);
        }
    }

    return ([]);
}

/*
 *  ========= onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        if (!Common.findSignalTypes(inst.$hardware, ["SPI_SS"])) {
            inst.mode = "Three Pin";
        }
    }

    /*
     * Some device specific implementations add onChange methods
     * to the mode config. Invoke them if necessary.
     */
    if (inst.$module.$configByName.mode.onChange) {
        inst.$module.$configByName.mode.onChange(inst, ui);
    }
}

/*
 *  ========= filterHardware ========
 *  param component - hardware object describing signals and
 *                    resources
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    if (Common.typeMatches(component.type, ["SPI_BUS", "SPI_DEVICE"])) {
        return (true);
    }

    return (false);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  @param inst       - SPI instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* Allow an input of 0 or ~0 */
    if (inst.defaultTxBufferValue.match( /((^0$)|(^~0$))/ )) {
        return;
    }

    if (inst.defaultTxBufferValue.match((/^0[X][\dA-F]{1,8}$/i)) === null) {
        logError(validation, inst, 'defaultTxBufferValue',
            'Must be in hex format.');
    }

    if (inst.$hardware && Common.findSignalTypes(inst.$hardware, ["SPI_SS"])) {
        if (inst.mode === "Three Pin") {
            logError(validation, inst, 'mode', "Using 'Three Pin' mode with "
                + " hardware requiring a 'Four Pin' mode.");
        }
    }
}

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
 *  Define the base properties and methods
 */
let base = {
    displayName: "SPI",
    description: "Serial Peripheral Interface (SPI) Bus Driver",
        longDescription: `
The [__SPI driver__][1] provides a portable application
interface to control onboard Serial Peripheral Interfaces (SPI).

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_s_p_i_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_s_p_i_8h.html#ti_drivers_SPI_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_s_p_i_8h.html#ti_drivers_SPI_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#SPI_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_SPI_",
    config: Common.addNameConfig(config, "/ti/drivers/SPI", "CONFIG_SPI_"),
    filterHardware: filterHardware,
    validate: validate,
    busModule: true,
    allowStaticOnly: true,
    modules: Common.autoForceModules(["Board", "Power", "DMA"]),
    onHardwareChanged: onHardwareChanged,

    _getPinResources: _getPinResources
};

/* extend the base exports to include family-specific content */
let deviceSPI = system.getScript("/ti/drivers/spi/SPI" + family + "DMA");
exports = deviceSPI.extend(base);
