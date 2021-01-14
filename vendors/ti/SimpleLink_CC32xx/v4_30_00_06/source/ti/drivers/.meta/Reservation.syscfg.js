/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Reservation ========
 *  Reservation configuration support
 */
"use strict";

/* get Common /ti/drivers utility functions */
let Common     = system.getScript("/ti/drivers/Common.js");
let logWarning = Common.logWarning;

/*
 *  ======== _getPeripheralOptions ========
 */
function _getPeripheralOptions()
{
    let all = [{name: "", displayName: "<select peripheral type>"}];
    for (var name in system.deviceData.interfaces) {
        all.push({name: name});
    }

    return (all);
}

/*
 *  ======== resConfig ========
 *  Reservation specific configs
 */
let resConfig = [
    {
        name: "owner",
        displayName: "Owner",
        default: "",
        placeholder: "Name used to group related reservations",
        onChange: updateConfigs
    },
    {
        name: "purpose",
        displayName: "Purpose",
        default: "",
        placeholder: "Reason for reserving this resource",
        onChange: updateConfigs
    },
    {
        name: "resourceType",
        displayName: "Resource Type",
        default: "",
        options: _getPeripheralOptions(),
        description: "The type of the resource to reserve",
        onChange: updateConfigs
    },

    /* resourceName is solely used by other modules to create reservations */
    {
        name: "resourceName",
        hidden: true,
        description: "The specific resource to reserve",
        default: "",
        onChange: updateConfigs
    }
];

/*
 *  ======== getTimerInstParams ========
 *  Device-independent API for modules to reserve a timer peripheral
 *
 *  @param timerId - numeric id for the timer to allocate
 *
 *  @returns mutable object suitable for return within the array returned
 *           from moduleInstances().
 */
function getTimerInstParams(timerId)
{
    let family = Common.device2Family(system.deviceData);

    let timerArgs = { /* issue: should be timerArgs = {}: PMUX-1361 & 1357 */
        owner   : undefined,
        purpose : undefined,
        $name   : undefined
    };

    switch (family) {
        case "MSP432E4":
        case "CC32XX": {
            timerArgs.resourceType = "Timer";
            timerArgs.resourceName = "Timer";
            break;
        }
        case "CC26XX":
        case "CC26X2": {
            timerArgs.resourceType = "GPTM";
            timerArgs.resourceName = "GPTM";
            break;
        }
        default: {
            throw new Error("unsupported device family: " + family);
        }
    }
    timerArgs.resourceName += String(timerId);

    let timerRes = {
        name         : "timer" + timerId,
        moduleName   : "/ti/drivers/Reservation",
        requiredArgs : timerArgs,
        hidden       : true /* hides the configuration in the client's view */
    };

    return (timerRes);
}

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 *
 *  Called on every change to our configuration settings
 */
function updateConfigs(inst, ui)
{
    //console.log("Reservation.updateConfigs(" + inst.$name + ", ...)");

    /* resourceName is _only_ empty when inst was created by an other module */
    if (inst.resourceName != "") {
        /* make all fields readOnly to prevent serialization & modification */
        //ui.$name.readOnly = true;        /* issue: PMUX-1356 */
        ui.resource.readOnly = true;     /* issue: PMUX-1356 */
        ui.resourceName.readOnly = true;
        ui.resourceType.readOnly = true;
        ui.purpose.readOnly = true;
        ui.owner.readOnly = true;
    }
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - Reservation instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if (inst.$hardware == null && inst.resourceType == "") {
        logWarning(validation, inst,
            "resourceType", "no hardware peripheral or component specified");
    }
}

/*
 *  ========= filterHardware ========
 *  Filter to identify compatible Reservation components
 *
 *  @param component - hardware object describing signals and
 *                     pins to which they're attached
 *
 *  @returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    return (false);
}

/*
 * ======== pinmuxRequirements ========
 *
 * Return array of interface requirements
 *
 * @param inst - instance
 *
 * @returns array of InterfaceRequirement or GPIOInterfaceRequirement objs
 */
function pinmuxRequirements(inst)
{
    /* if the peripheral interface is not specified, there's nothing to do */
    if (inst.resourceType == "") {
        return ([]);
    }

    /* compose single requirement from inst configs */
    function filterByName(pobj) {
        var ok = inst.resourceName == "" || pobj.name == inst.resourceName;
        return (ok);
    }

    let req = {
        name          : "resource", /* name injected into mod/inst obj */
        displayName   : "Resource", /* name shown in GUI for user control */
        description   : "Resource to exclude from SysConfig modules",
        interfaceName : inst.resourceType, /* selects "type" of peripheral */
        filter        : filterByName       /* selects specific peripheral(s) */
    };

    /* GPIOInterfaceRequirement does not have a resources property(!) */
    if (inst.resourceType != "GPIO") {
        req.resources = []; /* required to avoid pin conflicts */
    }

    return ([req]);
}

/*
 *  ======== exports ========
 */
exports = {
    /* prefix of name given to new instances */
    defaultInstanceName: "CONFIG_Reservation_", /* we add our names to ti_drivers_config.h */

    /* instance validation */
    validate: validate,

    /* extend basic /ti/drivers configs with Reservation specifics */
    config: resConfig,

    /* Reservation contributions to ti_drivers_config.[ch] */
    templates: {
//        boardh: "/ti/drivers/Reservation.Board.h.xdt",
//        boardc: "/ti/drivers/Reservation.Board.c.xdt"
    },

    /* -------- Client Methods -------- */
    /* simple device-independent API for reserving a timer peripheral */
    getTimerInstParams: getTimerInstParams,

    /* -------- GUI Control -------- */
    /* GUI module name */
    displayName: "Reservation",

    /* tooltip in GUI */
    description: "Resource Reservation Manager",

    /* Intro splash in GUI */
    longDescription: "The Reservation module enables users to exclude "
                   + "hardware resources from being used by other SysConfig "
                   + "modules.  This makes it possible to reliably mix "
                   + "SysConfig modules with existing software that has "
                   + "its own hardware requirements.",

    /* add Reservation to GUI selection lists related to recognized signals  */
    filterHardware: filterHardware,

    pinmuxRequirements: pinmuxRequirements
};
