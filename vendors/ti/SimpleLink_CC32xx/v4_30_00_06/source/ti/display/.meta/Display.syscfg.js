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
 *  ======== Display.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let UART     = system.getScript("/ti/drivers/UART");
let logError = Common.logError;

let config = [
    {
        name        : "displayType",
        displayName : "Display Type",
        default     : "UART",
        onChange    : onChange,
        options : [
            {name: "UART"},
            {name: "LCD"},
            {name: "Host"}
        ]
    },
    {
        name: "displayImplementation",
        displayName: "Display Implementation",
        default: "DisplayUart",
        options: [
            {name: "DisplayUart"},
            {name: "DisplayUart2"},
            {name: "DisplayDogm1286"},
            {name: "DisplayHost"}
        ],
        readOnly: true,
        description: "Displays Display delegates available for the " +
            system.deviceData.deviceId + " device and Display Type.",
        longDescription: "Displays Display delegates available for the " +
            system.deviceData.deviceId + " device and the " +
            "__Display Type__.\n\n" + `
Since there is only one delegate for each Display Type, it is a read-only
value that cannot be changed. Please refer to the
[__Display driver table__][0] for further documentation.

[0]: /drivers/doxygen/html/index.html#display
`
    },
    {
        name        : "lcdSize",
        displayName : "LCD Size",
        description : "Size of the LCD in pixels.",
        default     : 128,
        hidden      : true
    },
    {
        name        : "uartBufferSize",
        displayName : "UART Buffer Size",
        description : "UART display buffer size in bytes",
        default     : 1024
    },
    {
        name        : "useUART2",
        displayName : "Use UART2",
        description : "Use UART2 for underlying UART driver",
        default     : false,
        onChange    : onChange
    },
    {
        name        : "enableANSI",
        displayName : "Enable ANSI",
        description: "Enables or disables ANSI cursor support.",
        default     : false
    },
    {
        name        : "maxPrintLength",
        displayName : "Maximum Print Length",
        default     : 256,
        hidden      : true
    },
    {
        name        : "mutexTimeout",
        displayName : "Mutex Timeout",
        description : "Timeout to acquire mutex for synchronizing access to UART.",
        default     : "Never Timeout",
        onChange    : onChangeMutexTimeout,
        options     : [
            { name : "Never Timeout" },
            { name : "Never Wait" },
            { name : "Custom", description: "Custom number of RTOS ticks" }
        ]
    },
    {
        name        : "mutexTimeoutValue",
        displayName : "Mutex Timeout Value",
        hidden      : true,
        default     : 1
    },
    {
        name        : "baudRate",
        displayName : "Baud Rate",
        description : "UART baud rate",
        default     : 115200,
        options     : [
            { name:   1200 },
            { name:   2400 },
            { name:   4800 },
            { name:   9600 },
            { name:  19200 },
            { name:  38400 },
            { name:  57600 },
            { name: 115200 },
            { name: 230400 },
            { name: 460800 },
            { name: 921600 }
        ]
    }
];

/*
 * ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    if (inst.displayType === "UART") {

        let displayName = "UART";
        let moduleName = "/ti/drivers/UART";
        if (inst.$hardware && inst.$hardware.displayName) {
            displayName = inst.$hardware.displayName;
        }

        if (inst.useUART2) {
            moduleName = "/ti/drivers/UART2";
        }

        return ([
            {
                name       : "uart",
                displayName: displayName,
                moduleName : moduleName,
                hardware   : inst.$hardware
            }
        ]);
    }

    if (inst.displayType === "LCD") {

        let enableName = "LCD Enable";
        let enableHardware = null;
        let powerName = "LCD Power";
        let powerHardware = null;
        let selectName = "LCD Slave Select";
        let selectHardware = null;

        /* Speculatively get hardware and displayName */
        if (inst.$hardware && inst.$hardware.subComponents) {
            let components = inst.$hardware.subComponents;

            enableHardware = components.ENABLE;
            powerHardware = components.POWER;
            selectHardware = components.SELECT;

            if (enableHardware && enableHardware.displayName) {
                enableName = enableHardware.displayName;
            }
            if (powerHardware && powerHardware.displayName) {
                powerName = powerHardware.displayName;
            }
            if (selectHardware && selectHardware.displayName) {
                selectName = selectHardware.displayName;
            }
        }

        return ([
            {
                name       : "lcdEnable",
                displayName: enableName,
                moduleName : "/ti/drivers/GPIO",
                hardware   : enableHardware,
                args       : { mode: "Dynamic" }
            },
            {
                name       : "lcdPower",
                displayName: powerName,
                moduleName : "/ti/drivers/GPIO",
                hardware   : powerHardware,
                args       : { mode: "Dynamic" }
            },
            {
                name       : "lcdSS",
                displayName: selectName,
                moduleName : "/ti/drivers/GPIO",
                hardware   : selectHardware,
                args       : { mode: "Dynamic" }
            }
        ]);
    }

    return ([]);
}

/*
 * ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst)
{
    if (inst.displayType == "LCD") {

        let spiName = "LCD SPI";
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
                name       : "spi",
                displayName: spiName,
                moduleName : "/ti/drivers/SPI",
                hardware   : spiHardware
            }
        ]);
    }

    return ([]);
}

/*
 * ======== validate ========
 */
function validate(inst, validation)
{
    if (inst.mutexTimeoutValue <= 0) {
        logError(validation, inst, 'mutexTimeoutValue',
                 'Must be a positive integer.');
    }

    if (inst.uartBufferSize < 32) {
        logError(validation, inst, 'uartBufferSize',
                 'Must be greater than 32 bytes.');
    }

    if (inst.maxPrintLength <= 0) {
        logError(validation, inst, 'maxPrintLength',
                 'Must be a positive integer.');
    }

    if (inst.lcdSize <= 0) {
        logError(validation, inst, 'lcdSize', 'Must be a positive integer.');
    }

    /* Ensure display baud rate is supported by the uart's baud rate list */
    if (inst.displayType === "UART" && inst.uart.hasOwnProperty('baudRates')) {
        if (! inst.uart.baudRates.includes(inst.baudRate)) {
            let suffix = "";
            if (inst.uart.$hardware) {
                suffix = "(" + inst.uart.$hardware.name + ")";
            }
            logError(validation, inst, 'baudRate',
                inst.baudRate + " baud is not supported by the UART "
                + inst.uart.$name + suffix
            );
        }
    }
}

/*
 * ======== onChangeMutexTimeout ========
 *
 */
function onChangeMutexTimeout(inst, ui)
{
    if (inst.mutexTimeout === "Custom") {
        ui.mutexTimeoutValue.hidden = false;
    }
    else {
        ui.mutexTimeoutValue.hidden = true;
        inst.mutexTimeoutValue = 1;
    }
}

/*
 *  ======== onChange ========
 *  Show/hide appropriate config options for each type of display
 *  Update Display Implementation if useUART2 config is invoked
 */
function onChange(inst, ui)
{
    if (inst.displayType == "LCD") {
        ui.enableANSI.hidden = true;
        ui.maxPrintLength.hidden = true;
        ui.uartBufferSize.hidden = true;
        ui.useUART2.hidden = true;
        ui.baudRate.hidden = true;
        ui.lcdSize.hidden = false;
        ui.mutexTimeout.hidden = true;
        ui.mutexTimeoutValue.hidden = true;
        inst.displayImplementation = "DisplayDogm1286";
    }
    else if (inst.displayType == "Host") {
        ui.enableANSI.hidden = true;
        ui.maxPrintLength.hidden = false;
        ui.uartBufferSize.hidden = true;
        ui.useUART2.hidden = true;
        ui.baudRate.hidden = true;
        ui.lcdSize.hidden = true;
        ui.mutexTimeout.hidden = true;
        ui.mutexTimeoutValue.hidden = true;
        inst.displayImplementation = "DisplayHost";
    }
    else {
        ui.enableANSI.hidden = false;
        ui.maxPrintLength.hidden = true;
        ui.uartBufferSize.hidden = false;
        ui.useUART2.hidden = false;
        ui.baudRate.hidden = false;
        ui.lcdSize.hidden = true;
        ui.mutexTimeout.hidden = false;
        onChangeMutexTimeout(inst, ui);

        if (inst.useUART2 == false) {
            inst.displayImplementation =
                inst.$module.$configByName.displayImplementation.default;
        }
        else {
            inst.displayImplementation = "DisplayUart2";
        }
    }

    if (inst.$hardware) {
        ui.displayType.readOnly = true;
    }
    else {
        ui.displayType.readOnly = false;
    }
}

/*
 *  ========= filterHardware ========
 */
function filterHardware(component)
{
    let ret = false;

    if (component.type) {
        /* Check for known component types */
        if (Common.typeMatches(component.type, ["SHARP_LCD", "SPI_LCD"])) {
            return (true);
        }
    }

    ret = UART.filterHardware(component);

    return (ret);
}

/*
 * ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {

        if (Common.typeMatches(inst.$hardware.type, ["SHARP_LCD", "SPI_LCD"])) {
            inst.displayType = "LCD";
            inst.lcdSize = inst.$hardware.settings.Display.size;
        } else if (Common.typeMatches(inst.$hardware.type, ["UART"])) {
            inst.displayType = "UART";
        }
    }
    else {
        /* Set defaults */
        inst.displayType = "UART";
        inst.mutexTimeout = "Never Timeout";
    }

    onChange(inst, ui);
}

/*
 *  ======== getLibs ========
 *  Argument to the /ti/utils/build/GenLibs.cmd.xdt template
 */
function getLibs(mod)
{
    let libGroup = {
        name: "/ti/display",
        vers: "1.0.0.0",
        deps: [],
        libs: []
    };

    /* Get device information from GenLibs */
    let GenLibs = system.getScript("/ti/utils/build/GenLibs");
    let libPath = GenLibs.libPath;

    /* add the display library to libGroup's libs */
    libGroup.libs.push(libPath("ti/display", "display.a"));

    /* add dependency on /ti/drivers (if needed) */
    let needDrivers = false;
    for (let i = 0; i < mod.$instances.length; i++) {
        let inst =  mod.$instances[i];
        if (inst.displayType != "HOST") {
            needDrivers = true;
            break;
        }
    }
    libGroup.deps = needDrivers ? ["/ti/drivers"] : [];

    return (libGroup);
}

/*
 *  ======== base ========
 *  Define the base properties and methods
 */
let base = {
    displayName           : "Display",
    description           : "Display Driver",
    longDescription: `
The [__Display driver__][1] allows you to manage Display instances via simple
and portable APIs.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_display_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_display_8h.html#ti_drivers_Display_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_display_8h.html#ti_drivers_Display_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#Display_Configuration_Options "Configuration options reference"
`,
    defaultInstanceName   : "CONFIG_Display_",
    config                : Common.addNameConfig(config, "/ti/display/Display","CONFIG_Display_"),
    validate              : validate,
    maxInstances          : 3,
    filterHardware        : filterHardware,
    onHardwareChanged     : onHardwareChanged,
    modules               : Common.autoForceModules(["Board"]),
    moduleInstances       : moduleInstances,
    sharedModuleInstances : sharedModuleInstances,
    templates             : {
        /* contribute to TI-DRIVERS configuration file */
        boardc: "/ti/display/Display.Board.c.xdt",

        /* contribute libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt":
            {modName: "/ti/display/Display", getLibs: getLibs}
    }
};

exports = base;
