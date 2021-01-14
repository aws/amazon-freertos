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
 *  ======== I2CCC32XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Interrupt Priority";
intPriority.name = "interruptPriority";

let convertPinName = Common.cc32xxPackage2DevicePin;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base I2C configuration
 */
let devSpecific = {
    config : [
        {
            name: "sclTimeout",
            displayName: "SCL Timeout",
            displayFormat: "hex",
            default: 0x00,
            description: "Sets the SCL low clock timeout",
            longDescription:`
An I2C slave can extend a I2C transaction by periodically pulling the
clock low to create a slow bit transfer rate. The __SCL Timeout__ is used to
force a timeout if an I2C slave holds the clock line low for longer than the
timeout duration. A value less than 0x1 indicates the timeout is disabled.`
        },

        intPriority
    ],

    templates: {
        boardc : "/ti/drivers/i2c/I2CCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/i2c/I2C.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.i2c) {
        let sclPin = "P" + convertPinName(inst.i2c.sclPin.$solution.packagePinName);
        let sdaPin = "P" + convertPinName(inst.i2c.sdaPin.$solution.packagePinName);
        pin = "\nSCL: " + sclPin + "\nSDA: " + sdaPin;

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic I2C module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "I2C", null,
        [{name: "I2CCC32XX"}], null);
    
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    extend: extend
};
