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
 * ======== PowerCC32XXPins.syscfg.js ========
 * CC32XX Pins module (owned by PowerCC32XX)
 */

"use strict";

let pinTable = [1,2,3,4,5,6,7,8,13,15,16,17,18,19,20,21,29,30,
                45,50,52,53,55,57,58,59,60,61,62,63,64];
let pinOptions  = [
    {
        displayName: 'Default',
        name: "default",
        description: 'All pins configured with pull-down resistors with the '
           +  'exception of Pin 55 and Pin 57 configured with pull-up '
           +  'resistors.'
    },
    {
        name: 'NO_PULL_HIZ',
        description: 'No pull resistor, leave pin in a high impedance state.'
    },
    {
        name: 'WEAK_PULL_UP_STD',
        description: 'Pull-up resistor for standard pin type'
    },
    {
        name: 'WEAK_PULL_DOWN_STD',
        description: 'Pull-down resistor for standard pin type'
    },
    {
        name: 'WEAK_PULL_UP_OPENDRAIN',
        description: 'Pull-up resistor for open drain pin type'
    },
    {
        name: 'WEAK_PULL_DOWN_OPENDRAIN',
        description: 'Pull-down resistor for open drain pin type'
    },
    {
        name: 'DRIVE_LOW',
        description: 'Drive pin to a low logic state'
    },
    {
        name: 'DRIVE_HIGH',
        description: 'Drive pin to a high logic state'
    },
    {
        name: 'DONT_PARK',
        description: 'Take no action; do not park the pin'
    }
];

function padStart(x)
{
    if (x < 10) {
        x = "0" + x;
    }
    return x;
}

function resources()
{
    let resources = [
        {
            name          : 'changeAllPins',
            displayName   : 'Change All Pins',
            description   : "Use this configurable to update all pin"
                + " park states concurrently",
            onChange      : onchangeAllPins,
            default       : "default",
            options       : pinOptions
        }
    ];

    for (let idx = 0; idx < pinTable.length; idx++) {
        let pinNum = padStart(pinTable[idx].toString());
        let defOpt = 'WEAK_PULL_DOWN_STD';

        if (pinNum == '55' || pinNum == '57') {
            defOpt = 'WEAK_PULL_UP_STD';
        }

        let description = "PIN " + pinNum;
        if (system.deviceData.devicePins[pinTable[idx]]) {
            description = system.deviceData.devicePins[pinTable[idx]].designSignalName;
        }
        resources.push({
            name          : 'PIN' + pinNum,
            displayName   : 'PIN' + pinNum,
            description   : description,
            longDescription : description,
            default       : defOpt,
            options       : pinOptions
        });
    }
    return resources;
}

let config = resources();

function onchangeAllPins(inst, ui)
{
    let initVal = inst.changeAllPins;

    if (initVal === "default") {
        initVal = "WEAK_PULL_DOWN_STD";
    }

    for (let idx = 0; idx < pinTable.length; idx++) {
        let pinName = 'PIN' + padStart(pinTable[idx].toString());
        if (inst.changeAllPins === "default") {
            if (pinName == "PIN55" || pinName == "PIN57") {
                inst[pinName] = "WEAK_PULL_UP_STD";
                continue;
            }
        }

        inst[pinName] = initVal;
    }
}

/* The device specific exports for the power module */
exports = {
    name         : 'parkPins',
    displayName  : 'Park Pins',
    description  : "Pin Park States",
    maxInstances : 1,
    config       : config,
    templates    : {
        boardc      : "/ti/drivers/power/PowerCC32XXPins.Board.c.xdt"
    }
};
