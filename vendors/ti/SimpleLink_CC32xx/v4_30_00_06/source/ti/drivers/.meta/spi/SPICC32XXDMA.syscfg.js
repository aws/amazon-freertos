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
 *  ======== SPICC32XXDMA.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let convertPinName = Common.cc32xxPackage2DevicePin;

let intPriority = Common.newIntPri()[0];
intPriority.name = "dmaInterruptPriority";
intPriority.displayName = "DMA Interrupt Priority";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base SPI configuration
 */
let devSpecific = {
    config: [
        {
            name: "turboMode",
            displayName: "Turbo Mode",
            description: "Enable to maximize the throughput for multiple "
                + "word SPI transfers",
            default: false
        },
        {
            name: "ssControl",
            displayName: "SS Control",
            description: "The Slave Select (SS) line can be controlled by the"
                + " driver (SW) or the hardware (HW). Only applicable in"
                + " 4 pin mode.",
            longDescription: "In HW mode the SPI HW will control the SS line"
                + " automatically to follow the frame format specified. In"
                + " some cases it may be desirable for the SS to stay asserted"
                + " for the entire transfer regardless of the format used."
                + " In SW mode the driver will assert the SS before the"
                + " transfer starts and de-assert the SS when the transfer"
                + " completes.",
            options : [
                /* Implementation note: the board template uses the names
                 * of these options directly */
                {name : "HW"},
                {name : "SW"}
            ],
            default: "HW",
            hidden: true
        },
        intPriority
    ],

    moduleStatic: {
        displayName: "SPI Driver Configuration",
        config : [{
            name: "includeNWP",
            displayName: "Include Network Processor",
            description: "A spi instance used to communicate with the NWP"
            + " will be generated.",
            default: true
        }],

        /* bring in DMA and Power modules */
        modules: Common.autoForceModules(["Board", "Power", "DMA"]),

        moduleInstances : moduleInstances
    },

    /* override generic requirements with  device-specific reqs (if any) */
    pinmuxRequirements: pinmuxRequirements,

    templates: {
        boardc: "/ti/drivers/spi/SPICC32XXDMA.Board.c.xdt",
        boardh: "/ti/drivers/spi/SPICC32XXDMA.Board.h.xdt"
    },

    uiAdd : "staticAndInstance",

    _getPinResources: _getPinResources
};

/*
 *  ======== onChangeSsOptions ========
 *  onChange callback function for mode config
 *  We selectively hide or show the ssControl option
 *
 *  param inst  - Instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onChangeSsOptions(inst, ui)
{
    if (inst.mode === "Three Pin") {
        /* Set three pin mode to SW control */
        inst.ssControl = "SW";
        ui.ssControl.hidden = true;
    }
    else {
        ui.ssControl.hidden = false;
    }
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(modStatic)
{
    let instance = [
        {
            name: "networkProcessorSPI",
            displayName: "Network Processor SPI",
            moduleName: "/ti/drivers/spi/SPICC32XXNWP",
            hidden: true,
            collapsed: true,
            args : {
                "$name" : "CONFIG_NWP_SPI"
            }
        }
    ];

    if (modStatic.includeNWP === true) {
        return (instance);
    }

    return [];
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let mosi = "Unassigned";
    let miso = "Unassigned";
    let sclk;
    let ss;

    if (inst.spi) {
        if (inst.spi.mosiPin) {
            mosi = "P" + convertPinName(inst.spi.mosiPin.$solution.packagePinName);
        }
        if (inst.spi.misoPin) {
            miso = "P" + convertPinName(inst.spi.misoPin.$solution.packagePinName);
        }

        pin = "\nMOSI: " + mosi + "\nMISO: " + miso;

        if (inst.spi.sclkPin) {
            sclk = "P" + convertPinName(inst.spi.sclkPin.$solution.packagePinName);
            pin += "\nSCLK: " + sclk;
        }
        if (inst.spi.ssPin) {
            ss = "P" + convertPinName(inst.spi.ssPin.$solution.packagePinName);
            pin += "\nSS: " + ss;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== lspiFilter ========
 *  The LSPI SPI interface is hard coded to SPI instance 0.
 *  Remove LSPI from the list of SPI choices
 */
function lspiFilter(iface, peripheralPin)
{
    /* At this moment, only SPI0 (GSPI) is available */
    if (iface.name == "SPI0") return (true);

    return (false);
}

/*
 *  ======== pinmuxRequirements ========
 *  Return peripheral pin requirements as a function of config
 */
function pinmuxRequirements(inst)
{
    let misoRequired = false;
    let mosiRequired = false;
    let txRequired   = true;
    let rxRequired   = true;

    switch (inst.duplex) {
        case "Full":
            misoRequired = true;
            mosiRequired = true;
            break;
        case "Master TX Only":
            misoRequired = false;
            mosiRequired = true;
            rxRequired   = false;
            break;
        case "Slave RX Only":
            misoRequired = false;
            mosiRequired = true;
            txRequired   = false;
            break;
        case "Master RX Only":
            misoRequired = true;
            mosiRequired = false;
            txRequired   = false;
            break;
        case "Slave TX Only":
            misoRequired = true;
            mosiRequired = false;
            rxRequired   = false;
            break;
    }

    let spi = {
        name: "spi",
        displayName: "SPI Peripheral",
        interfaceName: "SPI",
        canShareWith: "SPI",
        filter: lspiFilter,
        resources: [
            {
                name: "sclkPin",
                displayName: "SCLK Pin",
                description: "SPI Serial Clock",
                interfaceNames: ["CLK"]
            }
        ]
    };

    if (misoRequired) {
        spi.resources.push({
            name: "misoPin",
            displayName: "MISO Pin",
            description: "Master Input Slave Output pin",
            interfaceNames: ["DIN"]});
    }

    if (mosiRequired) {
        spi.resources.push({
            name: "mosiPin",
            displayName: "MOSI Pin",
            description: "Master Output Slave Input pin",
            interfaceNames: ["DOUT"]});
    }

    /* add SS pin if one of the four pin modes is selected */
    if (inst.mode != "Three Pin") {
        spi.resources.push({
                    name: "ssPin",
                    displayName: "SS Pin",
                    description: "Slave Select / Chip Select",
                    interfaceNames: ["CS"]
                });
    }

    if (rxRequired) {
        spi.resources.push({
            name: "dmaRxChannel",
            displayName: "DMA RX Channel",
            description: "DMA channel used to receive data",
            interfaceNames: ["DMA_RX"]});
    }

    if (txRequired) {
        spi.resources.push({
            name: "dmaTxChannel",
            displayName: "DMA TX Channel",
            description: "DMA channel used to send data",
            interfaceNames: ["DMA_TX"]});
    }

    spi.signalTypes = {
        sclkPin: ["SPI_SCLK"],
        mosiPin: ["SPI_MOSI"],
        misoPin: ["SPI_MISO"],
        ssPin:   ["DOUT", "SPI_SS"]
    };

    return ([spi]);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic SPI module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* display which driver implementation can be used */
    devSpecific = Common.addImplementationConfig(devSpecific, "SPI", null,
        [{name: "SPICC32XXDMA"}], null);

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    /* add our onChange function to the mode config */
    for(let i = 0; i < result.config.length; i++)
    {
        if(result.config[i].name == "mode")
        {
            result.config[i].onChange = onChangeSsOptions;
            break;
        }
    }
    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base SPI module */
    extend: extend
};
