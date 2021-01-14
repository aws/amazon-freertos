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
 *  ======== PowerCC32XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let isCName  = Common.isCName;

/* Array of power configurables for this device family */
let config = [
    {
        name        : "enablePolicy",
        displayName : "Enable Policy",
        description : "Enable the power policy to run when the CPU is idle.",
        longDescription:`
If enabled, the policy function will be invoked once for each pass of the
idle loop.

In addition to this static setting, the Power Policy can be dynamically
enabled and disabled at runtime, via the [Power_enablePolicy()][1] and
[Power_disablePolicy()][2] functions, respectively.

[1]: /drivers/doxygen/html/_power_8h.html#ti_drivers_Power_Examples_enable
"Enabling power policy"
[2]: /drivers/doxygen/html/_power_8h.html#ti_drivers_Power_Examples_disable
"Disabling power policy"
`,
        onChange    : onChangeEnablePolicy,
        default     : false
    },
    {
        name        : "policyFunction",
        displayName : "Policy Function",
        description : "Power policy function called from the idle loop.",
        longDescription:`
When enabled, this function is invoked in the idle loop, to opportunistically
select and activate sleep states.

One reference policy is provided:

* __PowerCC32XX_sleepPolicy()__

In addition to this static selection, the Power Policy can be
dynamically changed at runtime, via the Power_setPolicy() API.
`,
        onChange    : onChangePolicyFxn,
        default     : "PowerCC32XX_sleepPolicy",
        options     :
        [
            {
                name: "PowerCC32XX_sleepPolicy",
                description: "A reference power policy is provided which can"
                    + " transition the MCU from the active state to one of two"
                    + " sleep states: Low-Power Deep Sleep (LPDS) or Sleep.",
                longDescription:`The policy looks at the estimated idle time
remaining, and the active constraints, and determine which sleep state to
transition to. The policy will give first preference to choosing LPDS, but if
that is not appropriate (e.g., not enough idle time), it will choose Sleep.`
            },
            {
                name: "Custom",
                description: "Custom policies can be written, and specified"
                    + " via the Policy Custom Function configuration."
            }
        ]
    },
    {
        name        : "policyCustomFunction",
        displayName : "Policy Custom Function",
        description : "User provided Power policy function. Usage not typical.",
        default     : "customPolicyFxn",
        hidden      : true
    },

    {
        name        : "policyInitFunction",
        displayName : "Policy Init Function",
        description : "The initialization function for the Power policy.",
        longDescription:`
This is an optional policy initialization function that will be called during
application startup, during Power Manager initialization.
`,
        default     : "PowerCC32XX_initPolicy",
        onChange    : onChangePolicyInitFxn,
        options     :
        [
            { name: "PowerCC32XX_initPolicy" },
            {
                name: "Custom",
                description: "Custom initialization policies can be written"
                    + "and specified via the Policy Init Custom Function"
                    + " configuration."
            }
        ]
    },
    {
        name        : "policyInitCustomFunction",
        displayName : "Policy Init Custom Function",
        description : "User provided Power policy initialization function. " +
                      "Usage not typical.",
        default     : "customPolicyInitFxn",
        hidden      : true
    },
    {
        name        : "enterLPDSHookFunction",
        displayName : "Enter LPDS Hook Function",
        description : "Optional hook function to call on entry into Low-Power"
            +" Deep Sleep (LPDS)",
        longDescription:`This function is called after any notifications are
complete, and before any pins are parked, just before entry to LPDS.`,
        placeholder : "Enter a function name to enable",
        default     : ""
    },
    {
        name        : "resumeLPDSHookFunction",
        displayName : "Resume LPDS Hook Function",
        description : "Optional hook function to call on resuming from"
            + " Low-Power Deep Sleep (LPDS)",
        longDescription:`This function is called early in the wake sequence,
before any notification functions are run.`,
        placeholder : "Enter a function name to enable",
        default     : ""
    },
    {
        name        : "enableNetworkWakeupLPDS",
        displayName : "Enable Network Wakeup LPDS",
        description : "Enable Network Activity as a wakeup source for"
            + " Low-Power Deep Sleep (LPDS)",
        default     : true
    },
    {
        name        : "keepDebugActiveDuringLPDS",
        displayName : "Keep Debug Active During LPDS",
        description : "Keep debug interface active during Low-Power Deep Sleep"
            + " (LPDS)",
        longDescription:`
This controls whether the debug interface is active when LPDS is
entered. For better power savings this should be disabled. Setting this flag
to true will prevent full LPDS and result in increased power consumption.
`,
        default     : false
    },
    {
        name        : "enableGPIOWakeupLPDS",
        displayName : "Enable GPIO Wakeup LPDS",
        description : "Enable GPIO as a Wakeup Source for Low-Power Deep Sleep"
            + " (LPDS).",
        onChange    : onChangeEnableGPIOWakeupLPDS,
        default     : true
    },
    {
        name        : "wakeupGPIOSourceLPDS",
        displayName : "Wakeup GPIO Source LPDS",
        description : "GPIO Source for wakeup from Low-Power Deep Sleep"
            + " (LPDS)",
        longDescription: "Only one of the following GPIOs can be specified as"
            + " a wake source from LPDS.",
        default     : "GPIO13",
        hidden      : false,  /* because enableGPIOWakeupLPDS default is true */
        options     :
        [
            { name: "GPIO2" },
            { name: "GPIO4" },
            { name: "GPIO11" },
            { name: "GPIO13" },
            { name: "GPIO17" },
            { name: "GPIO24" },
            { name: "GPIO26" }
        ]
    },
    {
        name        : "wakeupGPIOTypeLPDS",
        displayName : "Wakeup GPIO Type LPDS",
        description : "GPIO Trigger Type for wakeup from LPDS",
        hidden      : false,  /* because enableGPIOWakeupLPDS default is true */
        default     : "FALL_EDGE",
        options     :
        [
            {
                name: "LOW_LEVEL",
                description: "Wake up is active low"
            },
            {
                name: "HIGH_LEVEL",
                description: "Wake up is active high"
            },
            {
                name: "FALL_EDGE",
                description: "Wake up is active on a falling edge"
            },
            {
                name: "RISE_EDGE",
                description: "Wake up is active on a rising edge"
            }
        ]
    },
    {
        name        : "wakeupGPIOLPDSFunction",
        displayName : "Wakeup GPIO LPDS Function",
        description : "Optional hook function to call on Low-Power Deep Sleep"
            + " (LPDS) wakeup triggered by GPIO",
        longDescription:`
An argument for this wakeup function can be specified via the
__Wakeup GPIO LPDS Function Arg__ configurable.

During LPDS the internal GPIO module is powered off. No GPIO interrupt service
routine (ISR) will be triggered in this case (even if an ISR is configured,
and used normally to detect GPIO interrupts when not in LPDS). This function
can be used in lieu of a GPIO ISR, to take specific action upon LPDS wakeup.

This wakeup function will be called as one of the last steps in Power_sleep(),
after all notifications have been sent out, and after pins have been restored
to their previous (non-parked) states.
`,
        onChange    : onChangeWakeupGPIOFxnLPDS,
        placeholder : "Enter a function name to enable",
        hidden      : false,  /* because enableGPIOWakeupLPDS is true */
        default     : ""
    },
    {
        name        : "wakeupGPIOLPDSFunctionArg",
        displayName : "Wakeup GPIO LPDS Function Arg",
        description : "Byte argument to the function called on a GPIO"
                      + " triggered wakeup event from Low-Power Deep Sleep"
                      + " (LPDS).",
        hidden      : true,
        default     : 0
    },
    {
        name        : "enableGPIOWakeupShutdown",
        displayName : "Enable GPIO Wakeup Shutdown",
        description : "Enable GPIO as a Wakeup Source for Shutdown",
        onChange    : onChangeEnableGPIOWakeupShutdown,
        default     : true
    },
    {
        name        : "wakeupGPIOSourceShutdown",
        displayName : "Wakeup GPIO Source Shutdown",
        description : "GPIO Source for wakeup from Shutdown",
        longDescription: "Only one of the following GPIOs can be specified as"
            + " a wake source from Shutdown.",
        hidden      : false,  /* because enableGPIOWakeupShutdown is true */
        default     : "GPIO13",
        options     :
        [
            { name: "GPIO2" },
            { name: "GPIO4" },
            { name: "GPIO11" },
            { name: "GPIO13" },
            { name: "GPIO17" },
            { name: "GPIO24" },
            { name: "GPIO26" }
        ]
    },
    {
        name        : "wakeupGPIOTypeShutdown",
        displayName : "Wakeup GPIO Type Shutdown",
        description : "GPIO Trigger Type for wakeup from Shutdown",
        hidden      : false,  /* because enableGPIOWakeupShutdown is true */
        default     : "RISE_EDGE",
        options     :
        [
            {
                name: "LOW_LEVEL",
                description: "Wake up is active low"
            },
            {
                name: "HIGH_LEVEL",
                description: "Wake up is active high"
            },
            {
                name: "FALL_EDGE",
                description: "Wake up is active on a falling edge"
            },
            {
                name: "RISE_EDGE",
                description: "Wake up is active on a rising edge"
            }
        ]
    },
    {
        name         : "ramRetentionMaskLPDS",
        displayName  : "RAM Retention Mask LPDS",
        description  : "SRAM retention mask for Low-Power Deep Sleep (LPDS)",
        default      : ["SRAM_COL_1", "SRAM_COL_2", "SRAM_COL_3", "SRAM_COL_4"],
        minSelections: 0,
        options      :
        [
            { name: "SRAM_COL_1" },
            { name: "SRAM_COL_2" },
            { name: "SRAM_COL_3" },
            { name: "SRAM_COL_4" }
        ]
    },
    {
        name        : "latencyForLPDS",
        displayName : "Latency for LPDS",
        description : "The latency to reserve for entry to and exit from LPDS"
          + " (in units of microseconds).",
        longDescription:`
There is a latency for the device to transtion into the LPDS sleep state, and to
wake from LPDS to resume activity. The Power policy will check the available time
before next scheduled work, versus the latency reserved for the LPDS transition,
to determine if LPDS can be entered. The actual transtion latency will depend upon
overall device state (e.g. if the NWP is active or not), as well as execution of
notification functions that are registered with the Power driver.  The default
value of 20 milliseconds is chosen as a safe default for all device variants, with
margin for execution of software notification functions.  This latency value can
be changed (tuned) to meet specific application requirements.
`,
        default     : 20000
    },
    {
        name         : "ioRetentionShutdown",
        displayName  : "IO Retention Shutdown",
        description  : "IO groups to be retained during Shutdown",
        default      : [ "GRP_0", "GRP_1", "GRP_2", "GRP_3" ],
        minSelections: 0,
        options      :
        [
            {
                name: "GRP_0",
                description: "All pins except sFlash and JTAG interface"
            },
            {
                name: "GRP_1",
                description: "sFlash interface pins 11, 12, 13, 14"
            },
            {
                name: "GRP_2",
                description: "JTAG TDI and TDO interface pins 16, 17"
            },
            {
                name: "GRP_3",
                description: "JTAG TCK and TMS interface pins 19, 20"
            }
        ]
    }
];

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Power configuration
 */
let devSpecific = {
    getClockFrequencies : getClockFrequencies,

    moduleStatic : {
        config          : config,
        moduleInstances : modulesInstances,
        validate        : validate,
        modules: Common.autoForceModules(["Board"])
    },

    templates           : {
        boardc      : "/ti/drivers/power/PowerCC32XX.Board.c.xdt",
        board_initc : "/ti/drivers/power/PowerCC32XX.Board_init.c.xdt"
    }
};

/*
 *  ======== onChangeWakeupGPIOFxnLPDS ========
 *  onChange callback function for the wakeupGPIOLPDSFunction config
 */
function onChangeWakeupGPIOFxnLPDS(inst, ui)
{
    let subState = (inst.wakeupGPIOLPDSFunction == '');
    ui.wakeupGPIOLPDSFunctionArg.hidden = subState;
}

/*
 *  ======== onChangeEnableGPIOWakeupLPDS ========
 *  onChange callback function for the enableGPIOWakeupLPDS config
 */
function onChangeEnableGPIOWakeupLPDS(inst, ui)
{
    let subState = (!inst.enableGPIOWakeupLPDS);
    ui.wakeupGPIOTypeLPDS.hidden     = subState;
    ui.wakeupGPIOSourceLPDS.hidden   = subState;
    ui.wakeupGPIOLPDSFunction.hidden = subState;

    if (inst.enableGPIOWakeupLPDS &&
            inst.wakeupGPIOLPDSFunction != '') {
        ui.wakeupGPIOLPDSFunctionArg.hidden = false;
    }
    else {
        ui.wakeupGPIOLPDSFunctionArg.hidden = true;
    }
}

/*
 *  ======== onChangeEnableGPIOWakeupShutdown ========
 *  onChange callback function for the enableGPIOWakeupShutdown config
 */
function onChangeEnableGPIOWakeupShutdown(inst, ui)
{
    let subState = (!inst.enableGPIOWakeupShutdown);
    ui.wakeupGPIOTypeShutdown.hidden   = subState;
    ui.wakeupGPIOSourceShutdown.hidden = subState;
}

/*
 *  ======== onChangeEnablePolicy ========
 *  onChange callback function for the enablePolicy config
 */
function onChangeEnablePolicy(inst, ui)
{
    onChangePolicyInitFxn(inst,ui);
    onChangePolicyFxn(inst,ui);
}

/*
 *  ======== onChangePolicyInitFxn ========
 *  onChange callback function for the policyInitFunction config
 */
function onChangePolicyInitFxn(inst, ui)
{
    let subState = (inst.policyInitFunction !== 'Custom');
    ui.policyInitCustomFunction.hidden = subState;
}

/*
 *  ======== onChangePolicyFxn ========
 *  onChange callback function for the policyFunction config
 */
function onChangePolicyFxn(inst, ui)
{
    let subState = inst.policyFunction !== 'Custom';

    ui.policyCustomFunction.hidden = subState;
}

/*
 *  ======== moduleInstances ========
 *  Return a array of dependent modules for PowerCC32XX
 */
function modulesInstances(inst)
{
    return ([{
        name : 'parkPins',
        displayName: "Park Pins",
        description: "Pin Park States",
        moduleName : '/ti/drivers/power/PowerCC32XXPins',
        collapsed : true
    }]);
}

/*
 *  ======== getClockFrequencies ========
 *  Return the value of the CC32XX main CPU clock frequency
 */
function getClockFrequencies(clock)
{
    return [ 80000000 ];
}

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  param inst - Power instance to be validated
 *  param vo   - Issue reporting object
 *
 */
function validate(inst, vo)
{
    if (inst.enablePolicy) {
        if (inst.policyInitFunction === 'Custom') {
            if (!isCName(inst.policyInitCustomFunction)) {
                logError(vo, inst, "policyInitCustomFunction",
                    "Not a valid C identifier");
            }
            if (inst.policyInitCustomFunction === '') {
                logError(vo, inst, "policyInitCustomFunction",
                    "Must contain a valid function name if the " +
                    "Policy Init Function field == 'Custom'");
            }
        }
        if (inst.policyFunction === 'Custom') {
            if (!isCName(inst.policyCustomFunction)) {
                logError(vo, inst, "policyCustomFunction",
                    "is not a valid C identifier");
            }
            if (inst.policyCustomFunction === '') {
                logError(vo, inst, "policyCustomFunction",
                    "Must contain a valid function name if the " +
                    "Policy Function field == 'Custom'");
            }
        }
    }

    if (!isCName(inst.enterLPDSHookFunction)) {
        logError(vo, inst, 'enterLPDSHookFunction', 'Not a valid C identifier');
    }

    if (!isCName(inst.resumeLPDSHookFunction)) {
        logError(vo, inst, 'resumeLPDSHookFunction', 'Not a valid C identifier');
    }

    if (!isCName(inst.wakeupGPIOLPDSFunction)) {
        logError(vo, inst, 'wakeupGPIOLPDSFunction', 'Not a valid C identifier');
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic Power module to
 *  allow us to augment/override as needed for the CC32XX
 */
function extend(base)
{
    /* display which driver implementation can be used */
    devSpecific = Common.addImplementationConfig(devSpecific, "Power", null,
        [{name: "PowerCC32XX"}], null);

    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADCBuf module */
    extend: extend,
    getClockFrequencies: getClockFrequencies
};
