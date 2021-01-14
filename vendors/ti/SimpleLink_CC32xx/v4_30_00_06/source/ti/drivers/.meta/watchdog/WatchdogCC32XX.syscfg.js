/*
 * Copyright (c) 2018-2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== WatchdogCC32XX.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.default = "1";

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Watchdog configuration
 */
let devSpecific = {
    config : [
        intPriority
    ],

    templates : {
        boardc : "/ti/drivers/watchdog/WatchdogCC32XX.Board.c.xdt",
        boardh : "/ti/drivers/watchdog/Watchdog.Board.h.xdt"
    }
};

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - Watchdog timer instance to be validated
 *  @param validation - object to hold detected validation issues
 *
 *  @param $super     - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    let maxPeriod = 53687;
    let period    = parseInt(inst.period);
    let message;

    if ($super.validate) {
        $super.validate(inst, validation); // Defer to the base validation
    }

    if (period > maxPeriod) {
        message = 'Period value: ' + period
            + ' exceeds maximum allowed period: ' + maxPeriod;
        logError(validation, inst, "period", message);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "Watchdog", null,
        [{name: "WatchdogCC32XX"}], null);

    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

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
    /* required function, called by base Watchdog module */
    extend : extend
};
