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
 *  ======== RTOS.syscfg.js ========
 *  RTOS configuration support
 *
 *  This module is used to specify which RTOS is being used in the
 *  application.  This removes any "out of band" peripherals directly managed
 *  by the specified RTOS (i.e., not through TI-DRIVERS) from being accessed
 *  by any module in this configuration.
 *
 *  This module is a singleton module:
 *     o never need to add more than one RTOS
 *     o must support modules that need to reflect on which RTOS
 *     o want "one liner" in all config scripts:
 *          system.addModule("ti/driver/RTOS").name = "TI-RTOS";
 */
"use strict";

/* get /ti/drivers family name from device object */
let Common = system.getScript("/ti/drivers/Common.js");
let family = Common.device2Family(system.deviceData, "RTOS");

/*
 *  ======== rtosConfig ========
 *  RTOS specific configs
 */
let rtosConfig = [
    {
        name: "name",
        displayName: "Name",
        default: "TI-RTOS",
        options: [
            {name: "NoRTOS"},
            {name: "TI-RTOS"},
            {name: "FreeRTOS"}
        ],
        description: "Select the RTOS being used",
        onChange: updateConfigs
    },
    {
        name: "timerID",
        displayName: "Timer ID",
        default: 0,
        description: "Timer's numeric ID",
        hidden: !_timerConflict("TI-RTOS")
    }
];

/*
 *  ======== modules ========
 *  Declare that we need Reservation when an RTOS is specified
 */
function modules(inst)
{
    let reqs = [
        {name  : "Reservation", moduleName: "/ti/drivers/Reservation",
         hidden: true
        }
    ];

    /* issue: PMUX-1104 triggers an exception for Reservation */
    if (reqs.length != 0 && reqs[0].name == "Reservation") {
        reqs = [];
    }

    if (inst.name == "FreeRTOS") {
        reqs.push({
            name: "FreeRTOS",
            moduleName: "/ti/drivers/FreeRTOS",
            hidden: true
        });
    }

    return (reqs);
}

/*
 *  ======== moduleInstances ========
 *  Instances of other modules that must be created to support the
 *  specified RTOS
 *
 *  @returns array of ModuleInstanceRequirement objects
 */
function moduleInstances(inst)
{
    // issue: PMUX-1104?, should be:
    // let Reservation  = system.modules["/ti/drivers/Reservation"];
    let Reservation  = system.getScript("/ti/drivers/Reservation");
    let reservations = [];

    if (Reservation == null) { // issue: protect against PMUX-1104
        return (reservations);
    }

    if (_timerConflict(inst.name)) {
        var timer = Reservation.getTimerInstParams(inst.timerID);
        timer.requiredArgs.owner = inst.name;
        timer.requiredArgs.purpose = "Needed to support function timeouts";
        timer.requiredArgs.$name = "RTOS_Timer0";
        timer.hidden = true;
        timer.collapsed = true; /* don't auto expand resource properties */
        reservations.push(timer);
    }

    return (reservations);
}

/*
 *  ======== updateConfigs ========
 *  Adjust UI properties of configs based on current config settings
 *
 *  Called on every change to our configuration settings
 */
function updateConfigs(inst, ui)
{
    ui.timerID.hidden = _timerConflict(inst.name) ? false : true;
}

/*
 *  ======== _timerConflict ========
 *  Returns true iff the specified RTOS has a timer conflict
 *  with TI-Drivers
 *
 *  The following table documents the current HW conflicts
 *  between the three RTOSs and TI-Drivers; (*) => conflict
 *
 *  family   BIOS        NoRTOS     FreeRTOS  TI-Drivers
 *  ------   ----        ------     --------  ----------
 *  LPRF     RTC         RTC           -      GP Timers
 *  CC32XX   SysTick     SysTick    SysTick   GP Timers
 *  MSP432E  GP Timer(*) Systick    SysTick   GP Timers
 */
function _timerConflict(rtosName)
{
    /* if MPS432E */
    if (family == "MSP432E4") {
        if (rtosName == "TI-RTOS") {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== exports ========
 */
exports = {
    staticOnly: true,

    moduleStatic: {

        /* extend basic /ti/drivers configs with RTOS specifics */
        config: rtosConfig,

        modules: modules,

        /* create HW reservations as a function of the specified RTOS */
        moduleInstances: moduleInstances
    },

    /* -------- GUI Control -------- */
    /* GUI module name */
    displayName: "RTOS",

    /* tooltip in GUI */
    description: "RTOS Resource Manager",

    alwaysShowLongDescription : true,

    /* Intro splash in GUI */
    longDescription: "The RTOS module reserves device resources that"
        + " are required by low-level RTOS layers that must directly"
        + " access device resources.  It also provides a means for"
        + " other system software to determine the low-level RTOS"
        + " used by TI-Drivers to provide timeout and mutex services."
};
