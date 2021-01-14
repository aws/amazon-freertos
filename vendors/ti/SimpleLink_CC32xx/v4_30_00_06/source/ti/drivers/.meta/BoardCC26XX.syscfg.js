/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== BoardCC26XX.syscfg.js ========
 */

"use strict";

/* get ti/drivers common utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let logInfo = Common.logInfo;

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Board configuration
 */
let devSpecific = {
    moduleStatic: {
        config: [
            {
                name: "generateInitializationFunctions",
                displayName: "Generate Initialization Functions",
                description: "Generates board-specific initialization functions.",
                default: true
            }
        ],
        validate : validate
    }
};

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  param inst - Instance to be validated
 *  param validation - object to hold detected validation issues
 *
 */
function validate(inst, validation)
{
    if (inst.generateInitializationFunctions == true && system.deviceData.board == undefined) {
        logInfo(validation, inst, "generateInitializationFunctions", "The initialization" +
            " functions are still being generated after switching to a device.");
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic Board module to
 *  allow us to augment and override as needed.
 */
function extend(base)
{
    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* merge moduleStatic objects */
    result.moduleStatic = Object.assign({}, base.moduleStatic, devSpecific.moduleStatic);

    /* merge moduleStatic configs */
    result.moduleStatic.config = base.moduleStatic.config.concat(devSpecific.moduleStatic.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base Board module */
    extend: extend
};
