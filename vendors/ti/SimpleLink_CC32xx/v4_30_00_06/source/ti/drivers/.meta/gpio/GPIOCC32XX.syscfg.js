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
 *  ======== GPIOCC32XX.syscfg.js ========
 */

"use strict";

/* get ti/drivers common utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let convertPinName = Common.cc32xxPackage2DevicePin;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base GPIO configuration
 */
let devSpecific = {
    config : [
        {
            name: "enableStaticParking",
            displayName: "Enable Static Parking",
            description: "Enabling this option will allow the GPIO pin to"
                + " retain a static parking state when the device enters a low"
                + " power mode.",
            default: false
        }
    ],

    /* override device-specific templates */
    templates:
    {
        boardc : "/ti/drivers/gpio/GPIOCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/gpio/GPIO.Board.h.xdt"
    },

    _getPinResources: _getPinResources

};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;

    if (inst.gpioPin) {
        pin = "P" + convertPinName(inst.gpioPin.$solution.packagePinName);

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += ", " + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* display which driver implementation can be used */ 
    base = Common.addImplementationConfig(base, "GPIO", null,
        [{name: "GPIOCC32XX"}], null);

    /* overwrite base module attributes */
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
