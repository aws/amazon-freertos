/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== UART2CC32XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;

let intPriority = Common.newIntPri()[0];
intPriority.name = "interruptPriority";
intPriority.displayName = "Interrupt Priority";
intPriority.description = "UART peripheral interrupt priority";

let convertPinName = Common.cc32xxPackage2DevicePin;
/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base UART configuration
 */
let devSpecific = {
    config: [
        {
            name        : "flowControl",
            displayName : "Flow Control",
            default     : false,
            description : "Enable hardware flow control",
            longDescription : "Hardware flow control between two devices "
                + "is accomplished by connecting the UART Request-To-Send "
                + "(RTS) pin to the Clear-To-Send (CTS) input on the "
                + "receiving device, and connecting the RTS output of the "
                + "receiving device to the UART CTS pin"
        },
        intPriority,

        /* RX and TX ring buffer sizes */
        {
            name        : "rxRingBufferSize",
            displayName : "RX Ring Buffer Size",
            description : "Number of bytes in the RX ring buffer",
            longDescription : "The RX ring buffer serves as an extension "
                + "of the RX FIFO. If data is received when UART2_read() "
                + "is not called, data will be stored in the RX ring "
                + "buffer. The size can be changed to suit the "
                + "application.",
            default     : 32
        },
        {
            name        : "txRingBufferSize",
            displayName : "TX Ring Buffer Size",
            description : "Number of bytes in the TX ring buffer",
            longDescription : "The TX ring buffer serves as an extension "
                + "of the TX FIFO. Data is written to the TX ring buffer "
                + "when UART_write() is called. Data in the TX ring "
                + "buffer is then copied by the DMA to the TX FIFO. The "
                + "size can be changed to suit the application.",
            default     : 32
        }
    ],

    /* override generic pin requirements */
    pinmuxRequirements    : pinmuxRequirements,

    onHardwareChanged: onHardwareChanged,

    /* override device-specific templates */
    templates: {
        boardc : "/ti/drivers/uart2/UART2CC32XX.Board.c.xdt",
        boardh : "/ti/drivers/uart2/UART2.Board.h.xdt"
    },

    /* override generic filterHardware with ours */
    filterHardware        : filterHardware,

    _getPinResources: _getPinResources
};

/*
 *  ======== _getPinResources ========
 */
function _getPinResources(inst)
{
    let pin;
    let rxPin = "Unassigned";
    let txPin = "Unassigned";
    let ctsPin;
    let rtsPin;

    if (inst.uart) {
        if (inst.uart.rxPin) {
            rxPin = "P" + convertPinName(inst.uart.rxPin.$solution.packagePinName);
        }
        if (inst.uart.txPin) {
            txPin = "P" + convertPinName(inst.uart.txPin.$solution.packagePinName);
        }

        pin = "\nTX: " + txPin + "\nRX: " + rxPin;

        if (inst.uart.ctsPin) {
            ctsPin = "P" + convertPinName(inst.uart.ctsPin.$solution.packagePinName);
            pin += "\nCTS: " + ctsPin;
        }
        if (inst.uart.rtsPin) {
            rtsPin = "P" + convertPinName(inst.uart.rtsPin.$solution.packagePinName);
            pin += "\nRTS: " + rtsPin;
        }

        if (inst.$hardware && inst.$hardware.displayName) {
            pin += "\n" + inst.$hardware.displayName;
        }
    }

    return (pin);
}

/*
 *  ======== onHardwareChanged ========
 */
function onHardwareChanged(inst, ui)
{
    if (inst.$hardware) {
        let component = inst.$hardware;

        /* Determine if hardware supports flow control */
        if (Common.findSignalTypes(component, ["UART_CTS", "UART_RTS"])) {
            inst.flowControl = true;
        }
        else {
            inst.flowControl = false;
        }
        ui.flowControl.readOnly = true;
    }
    else {
        inst.flowControl = false;
        ui.flowControl.readOnly = false;
    }
}

/*
 *  ======== pinmuxRequirements ========
 *  Control RX, TX pin usage by the user specified dataDirection.
 *  Add CTS, RTS Pins if use specified flowControl is selected.
 *
 *  param inst      - UART instance
 *
 *  returns req[] - array of requirements needed by inst
 */
function pinmuxRequirements(inst)
{
    let cts = {
        name           : "ctsPin",    /* config script name */
        displayName    : "CTS Pin",   /* GUI name */
        interfaceNames : ["CTS"]      /* pinmux tool name */
    };

    let rts = {
        name           : "rtsPin",
        displayName    : "RTS Pin",
        interfaceNames : ["RTS"]
    };

    let tx = {
        name              : "txPin",  /* config script name */
        displayName       : "TX Pin", /* GUI name */
        interfaceNames    : ["TX"]    /* pinmux tool name */
    };

    let rx = {
        name              : "rxPin",
        displayName       : "RX Pin",
        interfaceNames    : ["RX"]
    };

    let dma_tx = {
        name           : "txDmaChannel",
        displayName    : "TX DMA Channel",
        interfaceNames : ["DMA_CHTX"]
    };

    let dma_rx = {
        name           : "rxDmaChannel",
        displayName    : "RX DMA Channel",
        interfaceNames : ["DMA_CHRX"]
    };

    let resources = [];

    if (inst.dataDirection != 'Receive Only') {
        resources.push(tx);
        resources.push(dma_tx);

        if (inst.flowControl === true) {
            resources.push(cts);
        }
    }

    if (inst.dataDirection != 'Send Only') {
        resources.push(rx);
        resources.push(dma_rx);

        if (inst.flowControl === true) {
            resources.push(rts);
        }
    }

    let uart = {
        name          : "uart",
        displayName   : "UART Peripheral",
        interfaceName : "UART",
        resources     : resources,
        signalTypes   : {
            txPin     : ['UART_TXD'],
            rxPin     : ['UART_RXD'],
            ctsPin    : ['UART_CTS'],
            rtsPin    : ['UART_RTS']
        }
    };

    return [uart];
}

/*
 *  ======== filterHardware ========
 *  Check 'component' signals for compatibility with UART
 *
 *  param component - hardware object describing signals and
 *                     resources they're attached to
 *
 *  returns Boolean indicating whether or not to allow the component to
 *           be assigned to an instance's $hardware config
 */
function filterHardware(component)
{
    return (Common.typeMatches(component.type, ["UART"]));
}

/*
 *  ======== validate ========
 *  Validate this instance's configuration
 *
 *  param inst       - UART instance to be validated
 *  param validation - object to hold detected validation issues
 *
 *  @param $super    - needed to call the generic module's functions
 */
function validate(inst, validation, $super)
{
    let minRingBufferSize = 16;
    let rxRingBufferSize = inst.rxRingBufferSize;
    let txRingBufferSize = inst.txRingBufferSize;
    let message;

    if ($super.validate) {
        $super.validate(inst, validation);
    }

    if (rxRingBufferSize < minRingBufferSize) {
        message = 'RX ring buffer size must be at least ' +
            minRingBufferSize + ' bytes';
        logError(validation, inst, "rxRingBufferSize", message);
    }
    if (txRingBufferSize < minRingBufferSize) {
        message = 'TX ring buffer size must be at least ' +
            minRingBufferSize + ' bytes';
        logError(validation, inst, "rxRingBufferSize", message);
    }
}

/*
 *  ======== extend ========
 */
function extend(base)
{
    /* display which driver implementation can be used */
    base = Common.addImplementationConfig(base, "UART2", null,
        [{name: "UART2CC32XX"}], null);

    /* override base validate */
    devSpecific.validate = function (inst, validation) {
        return validate(inst, validation, base);
    };

    /* merge and overwrite base module attributes */
    let result = Object.assign({}, base, devSpecific);

    /* concatenate device-specific configs */
    result.config = base.config.concat(devSpecific.config);

    return (result);
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base UART module */
    extend: extend
};
