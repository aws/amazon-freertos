/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== I2SCC32XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let convertPinName = Common.cc32xxPackage2DevicePin;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2S configuration
 */
let devSpecific = {
    config: [
        {
            name        : "enableDMA",
            displayName : "Enable DMA",
            description : "Enable DMA use to handle I2S data in order to"
                + " reduce CPU overhead",
            default     : true
        },

        {
            name        : "enableSCKX",
            displayName : "Enable SCKX",
            description : "Enable a second bit-clock line",
            default     : false
        }
    ],

    templates: {
        boardc : "/ti/drivers/i2s/I2SCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/i2s/I2S.Board.h.xdt"
    },

    modules: modules,
    pinmuxRequirements: pinmuxRequirements,

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let sckPin;
    let sckxPin;
    let wsPin;
    let sd0Pin;
    let sd1Pin;

    if (inst.i2s) {

        if (inst.i2s.SCKPin) {
            sckPin = "P" + convertPinName(inst.i2s.SCKPin.$solution.packagePinName);
            pin = "\nSCK: " + sckPin;
        }

        if (inst.i2s.SCKXPin) {
            sckxPin = "P" + convertPinName(inst.i2s.SCKXPin.$solution.packagePinName);
            pin += "\nSCKX: " + sckxPin;
        }

        if (inst.i2s.WSPin) {
            wsPin = "P" + convertPinName(inst.i2s.WSPin.$solution.packagePinName);
            pin += "\nWS: " + wsPin;
        }

        if (inst.i2s.SD0Pin) {
            sd0Pin = "P" + convertPinName(inst.i2s.SD0Pin.$solution.packagePinName);
            pin += "\nSD0: " + sd0Pin;
        }

        if (inst.i2s.SD1Pin) {
            sd1Pin = "P" + convertPinName(inst.i2s.SD1Pin.$solution.packagePinName);
            pin += "\nSD1: " + sd1Pin;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== modules ========
 */
function modules(inst)
{
    /* bring in Board, Power, and (optionally DMA) modules */
    if (inst.enableDMA) {
        return (Common.autoForceModules(["Board", "Power", "DMA"])());
    }
    else {
        return (Common.autoForceModules(["Board", "Power"])());
    }
}

/*
 *  ======== pinmuxRequirements ========
 */
function pinmuxRequirements(inst)
{
    let sck = {
        name: "SCKPin",
        displayName: "SCK Pin",
        description: "Audio port clock output (McACLK)",
        interfaceNames: ["McACLK"]
    };

    let sckx  = {
        name: "SCKXPin",
        displayName: "SCKX Pin",
        description: "Audio port clock (McACLKX)",
        interfaceNames: ["McACLKX"]
    };

    let sd0 = {
        name: "SD0Pin",
        displayName: "SD0 Pin",
        description: "Audio port data 0 pin (McAXR0)",
        interfaceNames: ["McAXR0"]
    };

    let sd1 = {
        name: "SD1Pin",
        displayName: "SD1 Pin",
        description: "Audio port data 1 pin (McAXR1)",
        interfaceNames: ["McAXR1"]
    };

    let ws = {
        name: "WSPin",
        displayName: "WS Pin",
        description: "Audio port frame sync pin (McAFSX)",
        interfaceNames: ["McAFSX"]
    };

    let rxChannel = {
        name: "dmaRxChannel",
        displayName: "DMA RX Channel",
        interfaceNames: ["DMA_RX"]
    };

    let txChannel = {
        name: "dmaTxChannel",
        displayName: "DMA TX Channel",
        interfaceNames: ["DMA_TX"]
    };

    let resources = [];

    if (inst.sd0DataDirection != "Disabled") {
        resources.push(sd0);
    }

    if (inst.sd1DataDirection != "Disabled") {
        resources.push(sd1);
    }

    resources.push(sck);

    if (inst.enableSCKX == true) {
        resources.push(sckx);
    }

    resources.push(ws);

    if (inst.enableDMA == true &&
       ((inst.sd0DataDirection == "Input")||
       (inst.sd1DataDirection == "Input"))) {
        resources.push(rxChannel);
    }

    if (inst.enableDMA == true &&
       ((inst.sd0DataDirection == "Output") ||
       (inst.sd1DataDirection == "Output"))) {
        resources.push(txChannel);
    }

    let i2s = {
        name: "i2s",
        displayName: "I2S Peripheral",
        description: "I2S Multichannel Audio Serial Port",
        interfaceName: "McASP",
        resources: resources,
        signalTypes : {
            SCKPin:   ["I2S_SCK"],
            SCKXPin:  ["I2S_SCKX"],
            SD0Pin:   ["I2S_SD0"],
            SD1Pin:   ["I2S_SD1"],
            WSPin:    ["I2S_WS"]
        }
    };

    return ([i2s]);
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "I2S", null,
        [{name: "I2SCC32XX"}], null);

    /* overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 */
exports = {
    extend: extend
};
