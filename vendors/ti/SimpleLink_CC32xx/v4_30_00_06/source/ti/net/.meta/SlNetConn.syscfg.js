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
 */

/*
 *  ======== SlNetConn.syscfg.js ========
 *  Connection Manager configuration support
 */

 "use strict";

let Common = system.getScript("/ti/utils/Common.js");

/*
 *  ======== onChange_generateEventThread ========
 */
function onChange_generateEventThread(inst, ui)
{
    if (inst.generateEventThread == true) {
        ui.eventThreadPriority.hidden = false;
        ui.eventThreadStackSize.hidden = false;
    } else {
        ui.eventThreadPriority.hidden = true;
        ui.eventThreadStackSize.hidden = true;
    }
}

/*
 *  ======== validate ========
 *  Validate given module and report conflicts
 *
 *  This function is not allowed to modify the instance state.
 */
function validate(inst, vo, getRef)
{
    let SLNET = system.modules["/ti/net/SlNet"];
    if (SLNET.$instances.length == 0) {
        vo.logWarning("An SlNet interface must be present for connection manager to be included.", inst);
    }

    if (inst.eventThreadPriority <= 0) {
        vo["eventThreadPriority"].errors.push("Thread priority must be greater than 0.");
    }

    if (inst.eventThreadStackSize < 0) {
        vo["eventThreadStackSize"].errors.push("Stack size must be greater than 0.");
    }
}

/*
 *  ======== config ========
 *  Define the config params of the module
 */
let config = [
    {
        name: "generateEventThread",
        displayName: "Create Thread To Process CM Events",
        default: false,
        onChange: onChange_generateEventThread,
        description: "Select this to generate a pthread that will handle event responses.",
        longDescription: `
Selecting this option will create a pthread that will call SlNetConn_process().
If not selected, the application code must provide a task/thread that calls
SlNetConn_process().
`
    },
    {
        name: "eventThreadPriority",
        displayName: "Event Thread Priority",
        default: "1",
        hidden: true,
        description: "The priority of the event thread.",
        longDescription: `The priority of the SlNetConn_process() pthread`
    },
    {
        name: "eventThreadStackSize",
        displayName: "Event Thread Stack Size",
        default: 2048,
        hidden: true,
        description: "The stack size of the event thread.",
        longDescription: `The stack size of the SlNetConn_process() pthread.`
    }
]

/*
 *  ======== modules ========
 *  Express dependencies for other modules
 *
 *  Invoked on any configuration change to the given instance.
 */
function modules(mod)
{
    let modules = new Array();

    modules.push({
        name: "slnet",
        moduleName: "/ti/net/SlNet"
    });

    return (modules);
}

/*
 * ======== longDescription ========
 * Intro splash on GUI
 */
let longDescription = `
The Connection Manager can manage and monitor connections at various levels of the network stack (i.e. MAC, IP, and Internet).
`;

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "Connection Manager",
    description: "Connection Manager configuration",
    defaultInstanceName: "CONFIG_SLNETCONN_MANAGER_",
    longDescription: longDescription,
    moduleStatic: {
        config: config,
        validate: validate,
        modules: modules,
    }
};

/* export the module */
exports = base;
