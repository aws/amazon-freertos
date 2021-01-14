/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Button.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* generic configuration parameters for Button instances */
let config = [
    {
        name: "autoHwConfig",
        displayName: "Use Provided Button Configuration",
        description: "Use the button configurations provided by the board file"
        + " for this button.",
        longDescription:"When this setting is enabled, the board specific"
        + " settings will be used to determine the button's hardware"
        + " configuration. This will provide the correct configuration most of"
        + " the time. If the button hardware has been modified, this setting"
        + " should be disabled and the user should manually set the correct"
        + " hardware configuration.",
        hidden : true,
        default : true,
        onChange : onAutoHwConfigChanged
    },
    {
        name: "hwConfig",
        displayName: "Button Configuration",
        description: "Specify the hardware configuration of the physical"
        + " button.",
        longDescription:"Specify whether the button is connected to ground or"
        + " Vcc when pushed, and whether there is an external pull up/down"
        + " network attached. If a 'no pull' setting is selected, internal pull"
        + " settings will be used. e.g. 'Active High Pull Down' should be set"
        + " for a button that has a pull down resistor connected and is"
        + " connected to Vcc when pushed. 'Active Low No Pull' should be set"
        + " for a button that has no pull up/down and is connected to ground"
        + " when pushed.",
        options: [
            {name : "Active Low Pull Up"},
            {name : "Active Low No Pull"},
            {name : "Active High Pull Down"},
            {name : "Active High No Pull"},
            {name : "None"}
        ],
        default : "Active Low No Pull"
    }
];

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui) {
    /*
     * Select the correct button hwConfig if autoHwConfig is true and
     * a config is provided in available in components settings.
     */
    let trigger = "NA";
    let pull = "NA";

    if (inst.$hardware) {
        ui.autoHwConfig.hidden = false;
    }
    else {
        ui.autoHwConfig.hidden = true;
    }

    if (inst.autoHwConfig) {
        if (inst.$hardware &&
            inst.$hardware.settings &&
            inst.$hardware.settings.DIN) {

            if (inst.$hardware.settings.DIN.pull) {
                /* Read GPIO internal pull if one exists */
                pull = inst.$hardware.settings.DIN.pull;
            }

            if (inst.$hardware.settings.DIN.interruptTrigger) {
                /* Read GPIO interrupt trigger if one exists */
                trigger = inst.$hardware.settings.DIN.interruptTrigger;
            }

            if (pull == "None") {
                /* External pull up/down, infer from edge */
                if (trigger == "Rising Edge") {
                    inst.hwConfig = "Active High Pull Down";
                    ui.hwConfig.readOnly = true;
                }
                else if (trigger == "Falling Edge") {
                    inst.hwConfig = "Active Low Pull Up";
                    ui.hwConfig.readOnly = true;
                }
            }
            else if (pull == "Pull Up" && trigger == "Falling Edge") {
                inst.hwConfig = "Active Low No Pull";
                ui.hwConfig.readOnly = true;
            }
            else if (pull == "Pull Down" && trigger == "Rising Edge") {
                inst.hwConfig = "Active High No Pull";
                ui.hwConfig.readOnly = true;
            }
            else {
                /* Invalid GPIO settings specified */
                inst.hwConfig = "None";
                ui.hwConfig.readOnly = false;
            }
        }
        else {
            /* HW config not provided */
            inst.hwConfig = "None";
            ui.hwConfig.readOnly = false;
        }
    }
    else {
        ui.hwConfig.readOnly = false;
    }
}

/*
 *  ======== onAutoHwConfigChanged ========
 */
function onAutoHwConfigChanged(inst, ui) {
    /* Set hwConfig options as read only if using autoHwConfig */
    onHardwareChanged(inst, ui);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - Button instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    /* A hwConfig of "None" means autoHwConfig can't be used */
    if (inst.hwConfig == "None") {
        let msg = "No button config was found, please set manually.";
        Common.logWarning(validation, inst, "hwConfig", msg);
    }

    /* If no hardware is selected, user must select their own GPIO */
    if (!inst.$hardware) {
        let msg = "No board hardware selected. Please select the GPIO pin"
        +  " with hardware attached.";
        Common.logInfo(validation, inst, "$hardware", msg);
    }
}

/*
 *  ======== moduleInstances ========
 *  Add a gpio pin instance to this button module
 */
function moduleInstances(inst)
{
    /* Update GPIO instance with hwConfig */
    let args;
    switch(inst.hwConfig) {
        case "Active Low No Pull":
            args = {
                mode : "Input",
                pull : "Pull Up",
                interruptTrigger : "Falling Edge"
            };
            break;
        case "Active Low Pull Up":
            args = {
                mode : "Input",
                pull : "None",
                interruptTrigger : "Falling Edge"
            };
            break;
        case "Active High No Pull":
            args = {
                mode : "Input",
                pull : "Pull Down",
                interruptTrigger : "Rising Edge"
            };
            break;
        case "Active High Pull Down":
            args = {
                mode : "Input",
                pull : "None",
                interruptTrigger : "Rising Edge"
            };
            break;
    }

    let gpio = [{
        name: "gpioPin",
        displayName: "GPIO Pin",
        description: "This Button is driven by a GPIO",
        moduleName: "/ti/drivers/GPIO",
        hardware: inst.$hardware,
        requiredArgs: args
    }];

    return(gpio);
}

/*
 *  ========= filterHardware ========
 *  param component - hardware object describing signals and
 *                    resources
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component) {
    if (component.type) {
        if (Common.typeMatches(component.type, ["BUTTON"])) {
            return (true);
        }
    }
    return (false);
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let mod = system.getScript("/ti/drivers/GPIO.syscfg.js");
    let pin;

    if (inst.gpioPin) {
        pin = mod._getPinResources(inst.gpioPin);
    }

    return (pin);
}


/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "Button",
    description: "Button Driver",
    longDescription: `
The [__Button driver__][1] provides a simple interface to control Buttons.
* [Usage Synopsis][2]
* [Examples][3]
* [Configuration][4]
[1]: /drivers/doxygen/html/_button_8h.html#details "C API reference"
[2]:
/drivers/doxygen/html/_button_8h.html#ti_drivers_Button_Synopsis "Synopsis"
[3]: /drivers/doxygen/html/_button_8h.html#ti_drivers_Button_Examples
"C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Button_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_BUTTON_",
    config: Common.addNameConfig(config, "/ti/drivers/apps/Button", "CONFIG_BUTTON_"),
    validate: validate,
    modules: Common.autoForceModules(["Board"]),
    moduleInstances: moduleInstances,
    filterHardware: filterHardware,
    onHardwareChanged : onHardwareChanged,
    templates: {
        boardc: "/ti/drivers/apps/button/Button.Board.c.xdt",
        boardh: "/ti/drivers/apps/button/Button.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

exports = base;
