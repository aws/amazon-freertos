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
 *  ======== UART.syscfg.js ========
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let family   = Common.device2Family(system.deviceData, "UART");
let logError = Common.logError;

/* Array of UART configurables that are common across device families */
let config = [
    {
        name        : "dataDirection",
        displayName : "Data Direction",
        default     : 'Send and Receive',
        options     : [
            { name : 'Send and Receive' },
            { name : 'Send Only' },
            { name : 'Receive Only' }
        ]
    },

    {
        name        : "errorCallbackFunction",
        displayName : "Error Callback Function",
        description : 'Optional application defined error function to be '
                      + 'called on receive errors.',
        default     : '',
        placeholder : "Enter a function name to enable"
    },

    /* baudRates is only needed by MSP432P but exists for all devs to enable
     * more portable syscfg scripts.
     */
    {
        name        : "baudRates",
        displayName : "Baud Rates",
        description : 'The set of baud rates that are used by this instance. '
                      + 'On some devices, these rates are used to pre-compute '
                      + 'a table of clock dividers necessary for each rate.',
        hidden      : true,
        default     : [115200],
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
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  @param inst       - UART instance to be validated
 *  @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    let errorFxn = inst.errorCallbackFunction;

    Common.validateNames(inst, validation);

    if ((errorFxn !== '') && !Common.isCName(inst.errorCallbackFunction)) {
        logError(validation, inst, 'errorCallbackFunction',
                "Not a valid C Indentifier.");
    }
}

/*
 *  ======== _getPinResources ========
 */
/* istanbul ignore next */
function _getPinResources(inst)
{
    return;
}

/*
 *  ======== base ========
 *  Define the base UART properties and methods
 */
let base = {
    displayName   : "UART",
    description   : "Universal Asynchronous Receiver Transmitter (UART) Driver",

    longDescription: `
The [__UART driver__][1] provides device independent APIs for reading
and writing to the UART peripherals.

* [Usage Synopsis][2]
* [Examples][3]
* [Configuration Options][4]

[1]: /drivers/doxygen/html/_u_a_r_t_8h.html#details "C API reference"
[2]: /drivers/doxygen/html/_u_a_r_t_8h.html#ti_drivers_UART_Synopsis "Basic C usage summary"
[3]: /drivers/doxygen/html/_u_a_r_t_8h.html#ti_drivers_UART_Examples "C usage examples"
[4]: /drivers/syscfg/html/ConfigDoc.html#UART_Configuration_Options "Configuration options reference"
`,
    /* instance properties and methods */
    defaultInstanceName: "CONFIG_UART_",
    config        : Common.addNameConfig(config, "/ti/drivers/UART", "CONFIG_UART_"),
    modules       : Common.autoForceModules(["Board", "Power"]),
    validate      : validate,

    _getPinResources: _getPinResources
};

/* get family-specific UART module */
let devUART = system.getScript("/ti/drivers/uart/UART" + family);

exports     = devUART.extend(base);
