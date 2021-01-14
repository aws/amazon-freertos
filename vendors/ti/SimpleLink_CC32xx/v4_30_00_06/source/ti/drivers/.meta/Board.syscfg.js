/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Board.syscfg.js ========
 */

/* Module used to invoke Board.c and Board.h templates */

"use strict";

/* get ti/drivers common utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

/* get /ti/drivers family name from device object */
let family = Common.device2Family(system.deviceData, "Board");

let config = [];

/*
 *  ======== getLibs ========
 */
function getLibs(mod)
{
    /* get device ID to select appropriate libs */
    let devId = system.deviceData.deviceId;

    /* get device information from DriverLib */
    var DriverLib = system.getScript("/ti/devices/DriverLib");
    let family = DriverLib.getAttrs(devId).deviceDefine;

    /* Get current RTOS configuration information */
    var RTOS = system.modules["/ti/drivers/RTOS"];
    let rtos = "TI-RTOS";
    if (RTOS != undefined) {
        rtos = RTOS.$static.name;
    }

    /* Get toolchain specific information from GenLibs */
    let GenLibs = system.getScript("/ti/utils/build/GenLibs");
    let libPath = GenLibs.libPath;
    let getToolchainDir = GenLibs.getToolchainDir;
    let getDeviceIsa = GenLibs.getDeviceIsa;
    let libs = [];

    if (family != "") {
        family = family.replace(/^DeviceFamily_/, "").toLowerCase();
        if (family.indexOf("msp432e") == 0) {
            family = "msp432e4";
        }
        else if (family.indexOf("cc32") == 0) {
            family = "cc32xx";
        }

        libs.push(libPath("ti/drivers","drivers_" + family + ".a"));
        libs.push(libPath("ti/grlib", "grlib.a"));

        if (rtos == "TI-RTOS") {
            libs.push(libPath("ti/dpl","dpl_" + family + ".a"));
        }
        else if (rtos == "NoRTOS") {
            libs.push("lib/" + getToolchainDir() + "/" + getDeviceIsa() +
                "/nortos_" + family + ".a");
        }
    }

    if (libs == null) {
        throw Error("device2LinkCmd: unknown device family ('"
            + family + "') for deviceId '" + devId + "'");
    }

    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/drivers",
        vers: "1.0.0.0",
        deps: [],
        libs: libs
    };

    /* add dependency on useFatFS configuration (if needed) */
    if (system.modules["/ti/drivers/SD"]) {
        let sdModule = system.modules["/ti/drivers/SD"];
        for (let i = 0; i < sdModule.$instances.length; i++) {
            let inst = sdModule.$instances[i];
            if (inst.useFatFS === true) {
                linkOpts.deps.push("/third_party/fatfs");
                break;
            }
        }
    }

    return (linkOpts);
}

/*
 *  ======== modules ========
 *  Reflect on HW components to "autoload" any support modules required
 */
function modules(mod)
{
    let reqs = [];

    reqs.push({
        name      : "Driverlib",
        moduleName: "/ti/devices/DriverLib",
        hidden    : true
    });

    if (system.deviceData.board && system.deviceData.board.components) {

        /* accumulate all modules required by the board's components */
        let mods = {};
        let comps = system.deviceData.board.components;
        for (let cname in comps) {
            let comp = comps[cname];
            if (comp.settings && comp.settings.requiredModule) {
                mods[comp.settings.requiredModule] = comp;
            }
        }

        /* create module requirements */
        for (let mname in mods) {
            reqs.push({
                name: mname.replace(/\//g, '_'), // private property name
                moduleName: mname,               // module to implicitly add
                hidden: true                     // don't show it as dependency
            });
        }
    }

    return (reqs);
}

/*
 *  ======== base ========
 */
let base = {
    displayName  : "Board",
    staticOnly   : true,
    maxInstances : 1,
    description  : "Board Support Module",
    alwaysShowLongDescription : true,
    longDescription: `
This module supports users utilizing Texas Instruments boards by generating
board-specific support functions. For more details, please visit the
[__Configuration Options__][1] and the [__Board Description__][2].

[1]: /drivers/syscfg/html/ConfigDoc.html#Board_Configuration_Options "Configuration options reference"
[2]: /drivers/syscfg/html/ConfigDoc.html#Board_Description "Board description reference"
`,

    documentation: `
This module provides basic validation required by any Hardware
Component that has been added to the board.

Hardware Components that require validation simply declare the name of
a module that must be added to a configuration.  The declaration is
made by adding a "requiredModule" field to the settings structure of the
Hardware Component.

The module named by a requiredModule field implements a validation
function that can reflect on the state of the configuration and plant
errors, warnings, and/or info messages throughout the system as
needed.

The example below is a fragment of the TMP116 component from the
BOOSTXL-BASSENSORS.syscfg.json:

        "TMP116": {
            "type": "TMP116",
            "displayName": "TMP116 Temperature Sensor",
            "description": "Accurate Digital Temperature Sensor ...",
            "settings": {
                "requiredModule": "/ti/boards/boosterpacks/bas/BAS",
                :
            },
            :
        }

In this case, whenever this TMP116 component is added to the board's
Hardware Components, the /ti/boards/boosterpacks/bas/BAS module will
be implicitly added to the application's configuration.  This, in
turn, will ensure that any validation functions defined by the BAS
module will execute.
`,

    templates    : {
        /* contribute TI-DRIVERS libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt"   :
            {modName: "/ti/drivers/Board", getLibs: getLibs},

        /* trigger generation of ti_drivers_config.[ch] */
        "/ti/drivers/templates/Board.c.xdt" : true,
        "/ti/drivers/templates/Board.h.xdt" : true
    },

    moduleStatic : {
        /* ensure something supplies appropriate DriverLib library */
        modules  : modules,
        config   : config
    }
};

/* extend the base exports to include family-specific content */
let deviceBoard = system.getScript("/ti/drivers/Board" + family);
exports = deviceBoard.extend(base);
