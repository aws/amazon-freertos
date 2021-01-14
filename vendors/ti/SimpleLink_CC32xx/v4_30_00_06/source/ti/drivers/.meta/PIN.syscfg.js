/*
 * Copyright (c) 2018-2019, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== PIN.syscfg.js ========
 */

"use strict";

/* issue: what's difference between system.getScript(), system.modules[] ...?
 *     system.getScript()      - == xdc.loadCapsule()
 *     system.modules[)        - == xdc.useModule(],
 *     scripting.modules.addInstance() - create instance of module:
 */

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* compute /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "PIN");

let logError = Common.logError;

let intPriority = Common.newIntPri()[0];
intPriority.displayName = "Interrupt Priority";
intPriority.name = "interruptPriority";

let swiPriority = Common.newSwiPri();
swiPriority.displayName = "Software Interrupt Priority";
swiPriority.name = "softwareInterruptPriority";

/* Intro splash on GUI */
let longDescription =
    "  The PIN driver allows clients (applications or other drivers) to "
    + "allocate and control the I/O pins on the device. The pins can "
    + "either be software controlled general-purpose I/O (GPIO) or "
    + "connected to hardware peripherals.  Furthermore, the PIN driver "
    + "allows clients to configure interrupt functionality on the pins to "
    + "receive callbacks (and potentially wake up from the standby or idle "
    + "power modes) on configurable signal edges.\n"
    + "  Most other drivers rely on functionality in the PIN driver.\n"
    + "  The PIN module provides the following functionality:"
    + "  - Initialize I/O pins upon boot to a default configuration (possibly"
    + "   user-generated)";

/* generic configuration parameters for PIN instances */
let pinConfig = [
    {
        name: "mode",
        displayName: "Mode",
        default: "Input",
        options: [
            { name: "Input" },
            { name: "Output" }
        ],
        onChange: updateConfigs
    },
    {
        name: "invert",
        displayName: "Invert",
        description: "Invert Input/Output",
        default: false,
        onChange: updateConfigs
    },
    /* Output Configs */
    {
        name: "outputState",
        displayName: "Output State",
        description: "Initial Output State",
        hidden: true,
        default: "Low",
        options: [
            { name: "High" },
            { name: "Low" }
        ]
    },
    {
        name: "outputType",
        displayName: "Output Type",
        hidden: true,
        default: "Push/Pull",
        options: [
            { name: "Push/Pull" },
            { name: "Open Drain" },
            { name: "Open Source" }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputStrength",
        displayName: "Output Strength",
        hidden: true,
        default: "Medium",
        options: [
            { name: "Maximum",
              displayName: "Maximum (8mA)" },
            { name: "Medium",
              displayName: "Medium  (4mA)" },
            { name: "Minimum",
              displayName: "Minimum (2mA)" }
        ],
        onChange: updateConfigs
    },
    {
        name: "outputSlew",
        displayName: "Output Slew",
        description: "Enable Output Slew",
        hidden: true,
        default: false,
        onChange: updateConfigs
    },
    /* Input Configs */
    {
        name: "hysteresis",
        displayName: "Hysteresis",
        description: "Enable Input Hysteresis",
        default: false,
        onChange: updateConfigs
    },
    {
        name: "pull",
        displayName: "Pull",
        description: "Pull up/down",
        hidden: false,
        default: "None",
        options: [
            { name: "None" },
            { name: "Pull Up" },
            { name: "Pull Down" }
        ],
        onChange: updateConfigs
    },
    {
        name: "irq",
        displayName: "IRQ",
        description: "Interrupt",
        hidden: false,
        default: "Disabled",
        options: [
            { name: "Disabled" },
            { name: "Falling Edge" },
            { name: "Rising Edge" },
            { name: "Both Edges" }
        ]
    }
];

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 *
 *  issue: must modify isr config to prevent false readonly display of
 *         interrupt trigger
 */
function updateConfigs(inst, ui)
{
    switch (inst.mode) {
        case "Output": {
            ui.pull.hidden = true;
            ui.outputType.hidden = false;
            ui.outputState.hidden = false;
            ui.outputStrength.hidden = false;
            ui.hysteresis.hidden = true;
            ui.irq.hidden = true;
            break;
        }
        case "Input": {
            ui.pull.hidden = false;
            ui.outputType.hidden = true;
            ui.outputStrength.hidden = true;
            ui.outputState.hidden = true;
            ui.hysteresis.hidden = false;
            ui.irq.hidden = false;
            break;
        }
    }
}

/*
 *  ======== validate ========
 */
function validate(inst, validation)
{
    if (inst.$ownedBy && inst.$hardware) {
        logError(validation, inst, "", "Can't use hardware on " + inst.$name
            + " while its owned by " + system.getReference(inst.$ownedBy)
            + ".");
    }

    Common.validateNames(inst, validation);
}

/*
 *  ======== getAttrs ========
 *  Return a symbolic PIN bit mask corresponding to inst's configs
 */
function getAttrs(inst)
{
    let input = "PIN_INPUT_EN";
    let output = "PIN_GPIO_OUTPUT_EN";

    if (inst.mode == "Output") {
        if (inst.outputState == "High") {
            output += " | PIN_GPIO_HIGH";
        }
        else {
            output += " | PIN_GPIO_LOW";
        }
        switch (inst.outputType) {
            case "Push/Pull": {
                output += " | PIN_PUSHPULL";
                break;
            }
            case "Open Drain": {
                output += " | PIN_OPENDRAIN";
                break;
            }
            case "Open Source": {
                output += " | PIN_OPENSOURCE";
                break;
            }
        }
        switch (inst.outputStrength) {
            case "Maximum": {
                output += " | PIN_DRVSTR_MAX";
                break;
            }
            case "Medium": {
                output += " | PIN_DRVSTR_MED";
                break;
            }
            case "Minimim":
            default: {
                output += " | PIN_DRVSTR_MIN";
                break;
            }
        }
        return (output);
    }
    else {
        switch (inst.pull) {
            case "Pull Up": {
                input += " | PIN_PULLUP";
                break;
            }
            case "Pull Down": {
                input += " | PIN_PULLDOWN";
                break;
            }
            case "Push/Pull": {
                input += " | PIN_PUSHPULL";
                break;
            }
            case "None":
            default: {
                input += " | PIN_NOPULL";
                break;
            }
        }

        switch (inst.irq) {
            case "Disabled": {
                input += " | PIN_IRQ_DIS";
                break;
            }
            case "Falling Edge": {
                input += " | PIN_IRQ_NEGEDGE";
                break;
            }
            case "Rising Edge": {
                input += " | PIN_IRQ_POSEDGE";
                break;
            }
            case "Both Edges": {
                input += " | PIN_IRQ_BOTHEDGES";
                break;
            }
        }

        if (inst.hysteresis) {
            input += " | PIN_HYSTERESIS";
        }

        return (input);
    }
}

/*
 *  ======== base ========
 *  Define the base/common PIN property and method exports
 */
let base = {
    displayName: "PIN",
    defaultInstanceName: "CONFIG_PIN_",
    description: "General Purpose PIN Driver",
    longDescription: longDescription,
    validate: validate,
    config: [],
    pinConfig: pinConfig,
    moduleStatic: {
        displayName: "PIN Global",
        config: [
            intPriority,
            swiPriority
        ],
        getAttrs: getAttrs,
        modules: Common.autoForceModules(["Board", "Power"])
    }
};

/* get the family-specific PIN module */
let devicePIN = system.getScript("/ti/drivers/pin/PIN" + family);

/* extend our common exports to include the family-specific content */
exports = devicePIN.extend(base);
