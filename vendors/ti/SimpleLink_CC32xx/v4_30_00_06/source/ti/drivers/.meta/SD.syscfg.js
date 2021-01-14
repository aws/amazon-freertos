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
 *  ======== SD.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");

let family = Common.device2Family(system.deviceData, "SD");

/* generic configuration parameters for SD instances */
let config = [
    {
        name: "useFatFS",
        displayName: "Use FatFS",
        description: "Enables the driver to use the SDFatFS driver interface",
        longDescription:`When enabled, the [__SDFatFS__][1] driver interface
will be accessible by the application.

[1]: /drivers/doxygen/html/_s_d_fat_f_s_8h.html#details "C API reference"
`,
        default: false
    }
];

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
        if (Common.typeMatches(component.type, ["SD_SPI_FLASH"])) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== validate ========
 *  Validate this inst's configuration
 *
 *  param inst       - SD instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
}

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let mod = system.getScript("/ti/drivers/SPI.syscfg.js");

    if (inst.spiInstance) {
        pin = mod._getPinResources(inst.spiInstance);
    }

    if (inst.slaveSelect) {
        let mod = system.getScript("/ti/drivers/GPIO.syscfg.js");
        pin += "\nSS: " + mod._getPinResources(inst.slaveSelect);
    }

    return (pin);
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst)
{
    /* Some devices support non SPI modes */
    if (inst.interfaceType && inst.interfaceType !== "SD SPI") {
        return ([]);
    }

    let spiName = "SD SPI";
    let spiHardware = null;

    /* Speculatively get hardware and displayName */
    if (inst.$hardware && inst.$hardware.subComponents) {
        let components = inst.$hardware.subComponents;
        spiHardware = components.SPI;
        if (spiHardware && spiHardware.displayName) {
            spiName = spiHardware.displayName;
        }
    }

    return ([
        {
            name: "spiInstance",
            displayName: spiName,
            moduleName: "/ti/drivers/SPI",
            hardware: spiHardware
        }
    ]);
}

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    /* Some devices support non SPI modes */
    if (inst.interfaceType && inst.interfaceType !== "SD SPI") {
        return ([]);
    }

    let selectHardware = null;
    let selectName = "SD Slave Select";

    /* Speculatively get hardware and displayName */
    if (inst.$hardware && inst.$hardware.subComponents) {
        let components = inst.$hardware.subComponents;

        selectHardware = components.SELECT;
        if (selectHardware && selectHardware.displayName) {
            selectName = selectHardware.displayName;
        }
    }

    return ([{
        name: "slaveSelect",
        displayName: selectName,
        moduleName: "/ti/drivers/GPIO",
        hardware: selectHardware,
        args: {
            mode: "Output",
            outputType: "Standard",
            initialOutputState:"High"
        }
    }]);
}

/*
 *  ======== getLibs ========
 *  Argument to the /ti/utils/build/GenLibs.cmd.xdt template
 */
function getLibs(mod)
{
    /* Get device information from GenLibs */
    let GenLibs = system.getScript("/ti/utils/build/GenLibs");

    let libGroup = {
        name: "/third_party/fatfs",
        deps: [],
        libs: []
    };

    /* add dependency on useFatFS configuration (if needed) */
    for (let i = 0; i < mod.$instances.length; i++) {
        let inst =  mod.$instances[i];
        if (inst.useFatFS === true) {
            libGroup.libs.push(GenLibs.libPath("third_party/fatfs", "fatfs.a"));
            break;
        }
    }

    return (libGroup);
}

/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName: "SD",
    description: "Secure Digital (SD) Driver",
    longDescription: `
The [__SD driver__][1] provides a simple interface to perform basic
operations on SD cards.
* [Usage Synopsis][2]
* [Examples][3]
* [Configuration][4]
[1]: /drivers/doxygen/html/_s_d_8h.html#details "C API reference"
[2]:
/drivers/doxygen/html/_s_d_8h.html#ti_drivers_SD_Synopsis "Synopsis"
[3]: /drivers/doxygen/html/_s_d_8h.html#ti_drivers_SD_Examples
"C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#SD_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName: "CONFIG_SD_",
    config: Common.addNameConfig(config, "/ti/drivers/SD", "CONFIG_SD_"),
    validate: validate,
    modules: Common.autoForceModules(["Board", "Power"]),
    sharedModuleInstances: sharedModuleInstances,
    moduleInstances: moduleInstances,
    filterHardware: filterHardware,
    _getPinResources: _getPinResources,
    templates             : {
        /* contribute libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt":
            {modName: "/ti/drivers/SD", getLibs: getLibs}
    }
};

/* extend the base exports to include family-specific content */
let deviceSD = system.getScript("/ti/drivers/sd/SD" + family);
exports = deviceSD.extend(base);
