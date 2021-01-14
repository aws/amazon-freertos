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
 */

/*
 *  ======== MQTT.syscfg.js ========
 *  MQTT configuration support
 */

 "use strict";

/*
 *  ======== getLibs ========
 */
function getLibs(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");

    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/net/mqtt",
        deps: ["/ti/net"],
        libs: [GenLibs.libPath("ti/net/mqtt", "mqtt_release.a")]
    };

    return (linkOpts);
}

/*
 *  ======== modules ========
 *  Express dependencies for other modules
 *
 *  Invoked on any configuration change to the given instance.
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "slnet",
        moduleName: "/ti/net/SlNet"
    });

    modules.push({
        name: "getLibs",
        displayName: "GetLibs",
        moduleName: "/ti/utils/runtime/GetLibs"
    });

    return (modules);
}

/*
 * ======== longDescription ========
 * Intro splash on GUI
 */
let longDescription = `
The MQTT module allows management of MQTT clients and brokers. The
MQTTClient module provides an easy-to-use API that allows interactions
between cloud-based or local MQTT servers.

The library supports the MQTT 3.1 protocol.
`;

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "MQTT",
    description: "MQTT Client and Server configuration",
    defaultInstanceName: "CONFIG_MQTT_",
    longDescription: longDescription,
    modules: modules,
    templates: {
        /* contribute NS libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt"   :
            {modName: "/ti/net/MQTT", getLibs: getLibs}
    }
};

/* export the module */
exports = base;
