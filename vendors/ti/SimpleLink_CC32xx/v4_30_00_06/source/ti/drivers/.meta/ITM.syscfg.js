/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ITM.syscfg.js ========
 *  Instrumentation Trace Macrocell
 */

"use strict";

let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let logInfo = Common.logInfo;
let family   = Common.device2Family(system.deviceData, "ITM");
let Power = system.getScript("/ti/drivers/power/Power" + family);
let clockSpeed = Power.getClockFrequencies();
const MAXBAUDRATE = 12e6; /* Maximum Baud rate */
const DEFAULTBAUDRATE = MAXBAUDRATE;
const BITSPERPACKET = 32; /* Number of bits in a word */
const TPIU_ACPR_MASK = 0x00001FFF; /* Mask for the prescaler field in ACPR */
const BAUDRATE_ERROR_MARGIN = 0.1; /* Percentage error before print */

/* Array of ITM configurables that are common across device families */
let config = [
    {
        name        : "tpiuSerialFormat",
        displayName : "TPIU Serial Format",
        description : 'The serial format to be used by the Trace Port Interface Unit (TPIU)',
        default     : 'ITM_TPIU_SWO_UART',
        options     : [
            { displayName: 'Manchester' , name:   'ITM_TPIU_SWO_MANCHESTER' },
            { displayName: 'UART' , name:   'ITM_TPIU_SWO_UART' }
        ]
    },
    {
        name        : "baudRate",
        displayName : "Baud Rate",
        description : "Baud rate for TPIU",
        default     : DEFAULTBAUDRATE,
        onChange    : onBaudRateChanged
    },
    {
        name        : "traceEnable",
        displayName : "Trace Enable",
        description : `Bitmask of stimulus ports to enable. Setting a bit
enables the port in that bit position`,
        displayFormat : "hex",
        default     : 0xFFFFFFFF
    },
    {
        name        : "fullPacketInCycles",
        displayName : "Full Packet In Cycles",
        description : `The number of clock cycles used by a full packet`,
        displayFormat : "hex",
        hidden: true,
        default     : calculateCyclesInPacket(clockSpeed, DEFAULTBAUDRATE)
    },
    {
        name        : "tpiuPrescaler",
        displayName : "TPIU Prescaler Register Value",
        description : `This is the value to be programmed in CPU_TPIU_O_ACPR.
In order to save size of the driver this is calculated via syscfg`,
        displayFormat : "hex",
        hidden: true,
        default     : calculateTPIUPrescaler(clockSpeed, DEFAULTBAUDRATE)
    }
];

/*
 *  ======== calculateCyclesInPacket ========
 *  Calculates the number of cycles in an ITM packet based on clock speed
 *
 *  @param sysClkSpeed - Processor's clock speed
 *  @param baudRate - User's selected baudRate
 */
function calculateCyclesInPacket(sysClkSpeed, baudRate)
{
    return (BITSPERPACKET * Math.floor(sysClkSpeed / baudRate));
}

/*
 *  ======== calculateTPIUPrescaler ========
 *  Calculates the value of CPU_TPIU_O_ACPR based on baudrate and clock speed
 *
 *  @param sysClkSpeed - Processor's clock speed
 *  @param baudRate - User's selected baudRate
 */
function calculateTPIUPrescaler(sysClkSpeed, baudRate)
{
    /* Setup Baud rate */
    let prescaler = Math.floor(sysClkSpeed / baudRate);

    /* Offset with current prescaler value */
    let diff1 = sysClkSpeed - (prescaler * baudRate);
    /* Offset with prescaler+1 value */
    let diff2 = ((prescaler+1) * baudRate) - sysClkSpeed;

    if (diff2 < diff1)
    {
        prescaler++;
    }

    return(((prescaler-1) & TPIU_ACPR_MASK));
}

/*
 *  ======== onBaudRateChanged ========
 *  Called when config baudRate changes.
 *
 *  @param inst - Module instance object containing config that changed
 *  @param ui   - User Interface state object
 */
function onBaudRateChanged(inst, ui)
{
    if(inst.baudRate && (inst.baudRate <= 12e6 && inst.baudRate > 0))
    {
        /* First calculate the length of a full packet in cycles */
        inst.fullPacketInCycles = calculateCyclesInPacket(clockSpeed, inst.baudRate);
        /* Then, calculate the TPIU prescaler */
        inst.tpiuPrescaler = calculateTPIUPrescaler(clockSpeed, inst.baudRate);
    }
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - ITM instance to be validated
 *  param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    Common.validateNames(inst, validation);
    let baudRateActual = Math.floor(clockSpeed/(inst.tpiuPrescaler + 1));
    let baudRateRangeH = inst.baudRate + (inst.baudRate*BAUDRATE_ERROR_MARGIN);
    let baudRateRangeL = inst.baudRate - (inst.baudRate*BAUDRATE_ERROR_MARGIN);

    if (inst.baudRate > 12e6 || inst.baudRate <= 0) {

        logError(validation, inst, "baudRate", "Baud rate must be greater than"
                + " 0 and less than " + MAXBAUDRATE);
    }
    else if ((baudRateActual > baudRateRangeH) ||
             (baudRateActual < baudRateRangeL)) {
        logInfo(validation, inst, "baudRate", "Expected baudrate: " +
                inst.baudRate + " Actual baudrate: " + baudRateActual);
    }
}

/*
 *  ======== base ========
 *  Define the base ITM properties and methods
 */
let base = {
    displayName         : "ITM",
    description         : "Instrumentation Trace Macrocell (ITM)",
    longDescription     : `
The [__ITM driver__][1] interfaces to the hardware module with the same name.
ITM provides a memory-mapped register interface that applications can use
to write logging or event words to a trace sink such as the TPIU.

* [Setup][2]
* [Pin Muxing][3]
* [Software Messages][4]
* [ARMv7-M Architecture Reference Manual][5]

[1]: /drivers/doxygen/html/_i_t_m_8h.html#ti_drivers_ITM_Overview "C API reference"
[2]: /drivers/doxygen/html/_i_t_m_8h.html#ti_drivers_ITM_Setup "C Setup"
[3]: /drivers/doxygen/html/_i_t_m_8h.html#ti_drivers_ITM_PinMux "Pin Muxing"
[4]: /drivers/doxygen/html/_i_t_m_8h.html#ti_drivers_ITM_SwMessages "Software Messages"
[5]: https://static.docs.arm.com/ddi0403/e/DDI0403E_d_armv7m_arm.pdf
`,
    maxInstances        : 1,    /* NB! Only one instance of ITM is supported */
    moduleStatic        : {
        config   : config,
        validate : validate,
        modules: Common.autoForceModules(["Board"])
    }
};


/* get family-specific ITM module */
let devITM;
if(family === "CC26XX" || family === "CC26X2") {
    /* The CC26XX family uses extended metadata, so it has a special module */
    devITM = system.getScript("/ti/drivers/itm/ITM" + family);
}
else{
    /* Other devices can use the generic ITM module */
    devITM = system.getScript("/ti/drivers/itm/ITMXX");
}
exports = devITM.extend(base);
