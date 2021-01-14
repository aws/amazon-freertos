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
 *  ======== LED.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* generic configuration parameters for LED instances */
let config = [
    {
        name: "dimmable",
        displayName: "Dimmable",
        description: "Determines whether control of brightness is required",
        longDescription:`The user may configure an LED to be driven by
a [__GPIO__][1] instance or a [__PWM__][2] instance, however, brightness
control requires a PWM.

[1]: /drivers/doxygen/html/_g_p_i_o_8h.html#details "GPIO"
[2]: /drivers/doxygen/html/_p_w_m_8h.html#details "PWM"
`,
        default: false
    }
];

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let mod;
    let pin;

    if (inst.pwmPin) {
        mod = system.getScript("/ti/drivers/PWM.syscfg.js");
        pin = mod._getPinResources(inst.pwmPin);
    }
    else if (inst.gpioPin) {
        mod = system.getScript("/ti/drivers/GPIO.syscfg.js");
        pin = mod._getPinResources(inst.gpioPin);
    }

    return (pin);
}

/*
 *  ========= filterHardware ========
 *  param component - hardware object describing signals and
 *                    resources
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    if (component.type) {
        if (Common.typeMatches(component.type, ["LED"])) {
            return (true);
        }
    }
    return (false);
}

/*
 *  ======== supportsPWM ========
 */
function supportsPWM(comp)
{
    let PWMDef = system.getScript("/ti/drivers/PWM");
    return PWMDef.filterHardware(comp);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - LED instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);

    if (inst.$hardware) {
        if (inst.dimmable && !supportsPWM(inst.$hardware)) {
            var name = inst.$hardware.displayName
                ? inst.$hardware.displayName : inst.$hardware.name;
            Common.logError(validation, inst, "dimmable",
                "The hardware selected, " + name + ", doesn't support dimming");
        }
    }
    else {
        Common.logInfo(validation, inst,
            "$hardware", "No hardware is selected, please ensure a suitable"
            + " pin is selected.");
    }
}

/*
 *  ======== moduleInstances ========
 *  Add a pwm or gpio instance to the module depending on if the LED is
 *  dimmable or not.
 */
function moduleInstances(inst)
{
    let devInstance;
    if (inst.dimmable) {
        if (inst.$hardware && !supportsPWM(inst.$hardware)) {
            return([]); /* dimmable requires a PWM */
        }
        devInstance = {
            displayName: "PWM Pin",
            name: "pwmPin",
            description: "This LED is driven by a PWM",
            moduleName: "/ti/drivers/PWM",
            args: {
                $name: inst.$name + "_PWM"
            }
        };
    }
    else {
        devInstance = {
            displayName: "GPIO Pin",
            name: "gpioPin",
            description: "This LED is driven by a GPIO",
            moduleName: "/ti/drivers/GPIO",
            args: {
                $name: inst.$name + "_GPIO"
            },
            requiredArgs: {
                mode : "Output"
            }
        };
    }

    devInstance.hardware = inst.$hardware;

    return ([devInstance]);
}

/*
 *  ======== onHardwareChanged ========
 *  Handle new hardware selection
 *
 *  @param inst - LED instance to be validated
 *  @param ui   - object with properties for each configurable whose value
 *                is an object that allows us to control the GUI display of
 *                the configurable (hidden, readonly, ...)
 */
function onHardwareChanged(inst, ui)
{
    /* issue: filterHardware can't prevent selection of invalid components,
     *        so if a component is not valid we must enable/disable dimmable
     *        PMUX-661 and PMUX-1649
     */
    if (inst.$hardware) {
        if (supportsPWM(inst.$hardware) != true) {
            /* this LED can't be dimmed */
            if (inst.dimmable) {
                ui.dimmable.readOnly = false; /* allow it to be fixed */
            }
            else {
                var name = inst.$hardware.displayName
                    ? inst.$hardware.displayName : inst.$hardware.name;
                ui.dimmable.readOnly = "The selected hardware, "
                    + name + ", does not support dimming";
            }
        }
        else {
            ui.dimmable.readOnly = false;
        }
    }
}

/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "LED",
    description: "LED Driver",
    longDescription: `
The [__LED driver__][1] provides a simple interface to control LEDs.
* [Usage Synopsis][2]
* [Examples][3]
* [Configuration][4]
[1]: /drivers/doxygen/html/_l_e_d_8h.html#details "C API reference"
[2]:
/drivers/doxygen/html/_l_e_d_8h.html#ti_drivers_LED_Synopsis "Synopsis"
[3]: /drivers/doxygen/html/_l_e_d_8h.html#ti_drivers_LED_Examples
"C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#LED_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_LED_",

    config: Common.addNameConfig(config, "/ti/drivers/apps/LED", "CONFIG_LED_"),

    validate: validate,
    modules: Common.autoForceModules(["Board"]),
    moduleInstances: moduleInstances,
    filterHardware: filterHardware,

    /* make GUI changes in response to HW model changes */
    onHardwareChanged: onHardwareChanged,

    templates: {
        boardc: "/ti/drivers/apps/led/LED.Board.c.xdt",
        boardh: "/ti/drivers/apps/led/LED.Board.h.xdt"
    },

    _getPinResources: _getPinResources
};

exports = base;
