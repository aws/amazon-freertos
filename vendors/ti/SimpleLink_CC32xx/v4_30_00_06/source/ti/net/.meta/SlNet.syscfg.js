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
 */

/*
 *  ======== SlNet.syscfg.js ========
 *  General configuration support
 */

"use strict";

let Common = system.getScript("/ti/utils/Common.js");

/* global vars */

/* Sysconfig string specifying the current device ID */
let deviceId = system.deviceData.deviceId;
let deviceFamily = Common.device2Family(system.deviceData);

const maxPriority = 15;

/* used in the networkIfFxnList config object */
let cc32xxFunctionList = [
    {
        name: "SimpleLink WiFi"
    },
    {
        name: "Custom"
    }
];

/* used in the networkIfFxnList config object */
let e4FunctionList = [
    {
        name: "NDK"
    },
    {
        name: "Custom"
    }
];

/* hold default values for config objects */
let defval = {
    customFuncList: ""
};

/* global exports, system */

/*
 *  ======== getLibs ========
 */
function getLibs(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");

    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/net",
        deps: [],
        libs: [GenLibs.libPath("ti/net", "slnetsock_release.a")]
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

    /* NDK is only required if you are using it on this interface */
    if(inst.networkIfFxnList == "NDK") {
        modules.push({
            name: "general",
            moduleName: "/ti/ndk/General"
        });
    }

    modules.push({
        name: "getLibs",
        displayName: "GetLibs",
        moduleName: "/ti/utils/runtime/GetLibs"
    });

    return (modules);
}

/*
 *  ======== onChange_networkIfFxnList ========
 */
function onChange_networkIfFxnList(inst, ui)
{
    if(inst.networkIfFxnList == "NDK") {
        ui.ifName.readOnly = true;
        ui.ifName.hidden = true;
        ui.customFuncList.readOnly = true;
        ui.customFuncList.hidden = true;
        ui.importNIMU.hidden = false;
        ui.importNIMU.readOnly = false;
        inst.customFuncList = defval.customFuncList;
        inst.ifName = "eth0";
    }
    else if(inst.networkIfFxnList == "SimpleLink WiFi") {
        ui.ifName.readOnly = false;
        ui.ifName.hidden = false;
        ui.customFuncList.readOnly = true;
        ui.customFuncList.hidden = true;
        ui.importNIMU.hidden = true;
        ui.importNIMU.readOnly = true;
        inst.importNIMU = false;
        inst.customFuncList = defval.customFuncList;
        inst.ifName = "wlan0";
    }
    else {
        ui.ifName.readOnly = false;
        ui.ifName.hidden = false;
        ui.customFuncList.readOnly = false;
        ui.customFuncList.hidden = false;
        ui.importNIMU.hidden = true;
        ui.importNIMU.readOnly = true;
        inst.importNIMU = false;
    }
}


/*
 *  ======== validate ========
 *  Validate given instance and report conflicts
 *
 *  This function is not allowed to modify the instance state.
 */
function validate(inst, vo, getRef)
{
    /* config.id */
    let id = inst.id;
    if (id < 1 || id > 16)
    {
        vo["id"].errors.push("Invalid interface ID. Must be 1-16.");
    }

    let instances = inst.$module.$instances;
    for(let i = 0; i < instances.length; i++)
    {
        if(instances[i].id == id && instances[i].$name != inst.$name)
        {
            vo["id"].errors.push(instances[i].$name + " is already using this interface ID");
        }

        /* SlNet Ifs must have a unique NIMU inst */
        if(inst.importNIMU)
        {
            let NIMU = inst.NIMU.$name;
            if(instances[i].importNIMU &&
               instances[i].NIMU.$name == NIMU &&
               instances[i].$name != inst.$name)
            {
                let ref = system.getReference(instances[i]);
                /*
                 * This error should really be pushed to "NIMU", but in syscfg
                 * v1.4.0_1234 there is a bug with pushing errors to a
                 * sharedModuleInstance.
                 *
                 * We should change this to push to "NIMU" when the SimpleLink
                 * SDKs move to syscfg 1.5.x
                 */
                vo["networkIfFxnList"].errors.push(ref + " is already using " +
                                                  "this NDK Interface");
            }
        }

        if(inst.networkIfFxnList == "NDK" &&
           inst.importNIMU == false &&
           instances[i].networkIfFxnList == "NDK" &&
           instances[i].importNIMU == false &&
           instances[i].$name != inst.$name)
        {
            let ref = system.getReference(instances[i]);
            vo["networkIfFxnList"].errors.push(ref + " is already using NDK without Import NDK Interface");
        }
    }

    /* config.priority */
    if (inst.priority < 1 || inst.priority > maxPriority)
    {
        vo["priority"].errors.push("Invalid interface priority. Must be 1-15.");
    }

    if (inst.networkIfFxnList == "Custom" && !inst.customFuncList)
    {
        vo["customFuncList"].errors.push("A custom function list must be specified");
    }
}

/*
 *  ======== longDescription ========
 *  Intro splash on GUI
 */
let longDescription = `
The Socket Layer IP Network module (SlNet) allows you to manage
instances of IP network stacks using a consistent, cross-platform API.
Some example use cases include:

* CC3XXX (WiFi) using its network processor-based stack
* MSP432E4 (wired ethernet) using the NDK stack

Using the SlNet API enables you to create portable applications that
support all of these environments.
`;


/*
 *  ======== config_instance ========
 *  Define the config params of the module (module-wide)
 */
let config_instance = [
    {
        name: "networkIfFxnList",
        displayName: "Network Interface Function List",
        default: deviceId.match(/CC32.*/) ? "SimpleLink WiFi" : "NDK",
        options: deviceId.match(/CC32.*/) ? cc32xxFunctionList : e4FunctionList,
        onChange: onChange_networkIfFxnList,
        description: 'Choose the desired interface function list',
        longDescription: `
Choose from a list of supported function lists on your device or use a custom
one

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_networkIfFxnList)`,
        documentation: `
This setting is used to determine the \`ifConf\` argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
`
    },
    {
        name: "importNIMU",
        displayName: "Import NDK Interface",
        hidden: deviceId.match(/CC32.*/) ? true : false,
        readOnly: deviceId.match(/CC32.*/) ? true : false,
        default: false,
        description: "Select this if you want a NDK Interface to be " +
                     "brought in for you",
        longDescription: `
Selecting this option will automatically pull in a NDK Interface for you. Other
wise you will be responsible for adding a NDK Interface. Furthermore, if this
option is not selected you will be limited to only one SlNet instance using the
NDK as its Function List.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_importNIMU)`,
        documentation: `
[documentation here]
`
    },
    {
        name: "ndkInterface",
        displayName: "",
        config: []
    },
    {
        name: "customFuncList",
        displayName: "Custom Network Interface Symbol",
        default: defval.customFuncList,
        hidden: true,
        readOnly: true,
        description: 'User symbol specifying a SlNetIf-compliant network interface',
        longDescription: `
A SlNetIf-compliant network interface symbol for this interface instance.

This config param is only available if the "Network Interface Function List" is set to
"Custom".

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_customFuncList)`,
        documentation: `
This setting is equivalent to setting the \`ifConf\` argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
`
    },
    {
        name: "id",
        displayName: "ID",
        default: 1,
        description: "SLNETIF_ID_? value",
        longDescription: `
Specifies the interface ID for this interface instance.

The value of the interface identifier is defined with the prefix \`SLNETIF_ID_\`
which is defined in **slnetif.h**.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_id)`,
        documentation: `
This setting is equivalent to setting the \`ifID\` argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
`
    },
    {
        name: "ifName",
        displayName: "Interface Name",
        default: deviceId.match(/CC32.*/) ? "wlan0" : "eth0",
        hidden: deviceId.match(/CC32.*/) ? false : true,
        readOnly: deviceId.match(/CC32.*/) ? false : true,
        description: 'Specifies the name for this interface"',
        longDescription: `
Specifies the name of the interface. Note: Can be set to \`NULL\`, but when set
to \`NULL\` cannot be used with **SlNetIf_getIDByName()**.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_ifName)`,
        documentation: `
This setting is equivalent to setting the \`ifName\` argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
`
    },
    {
        name: "priority",
        displayName: "Priority",
        default: 5,
        description: 'Specifies the priority of the interface.',
        longDescription: `
Specifies the priority of the interface (In ascending order). Note: maximum
priority is ${maxPriority}.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_priority)`,
        documentation: `
This setting is equivalent to setting the \`priority\` argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
`
    },
    {
        name: "enableSecureSocks",
        displayName: "Enable Secure Sockets",
        description: 'Enable secure sockets',
        default: true,
        hidden: deviceId.match(/CC32.*/) ? true : false,
        readOnly: deviceId.match(/CC32.*/) ? true : false,
        longDescription: `
Enable secure sockets on this interface. You cannot add secure objects without
this option being selected.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SlNet_enableSecureSocks)`,
    documentation: `
Selecting this option will display the Secure Object adder interface in the
SysConfig GUI. If you are using the NDK it will also change the \`ifConf\`
argument in
[**SlNetIf_add()**](/ns/html/group__SlNetIf.html#gae09651b941726526788a932498d2d250).
to the NDK's secure function list.
`
    }
];

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let modules = [];

    if(inst.enableSecureSocks == true)
    {
        modules.push(
            {
                name: "SECOBJ",
                displayName: "Secure Objects",
                moduleName: "/ti/net/SecObj",
                collapsed: false,
                /*
                 * The following options allow the useArray feature to be
                 * backwards compatible with older *.syscfg scripts based on
                 * versions of NS that did not use the useArray feature.
                 */
                legacyMigrationOptions: {
                    lengthConfigName: "secObjs",
                    instanceNameMapping: "CONFIG_SECOBJ_`index`"
                },
                useArray: true
            }
        );
    }

    return modules;
}

/*
 *  ======== sharedModuleInstances ========
 */
function sharedModuleInstances(inst)
{
    let modules = [];

    if(inst.networkIfFxnList == "NDK" && inst.importNIMU == true)
    {
        modules.push(
            {
                name: "NIMU",
                displayName: "NDK Interface",
                moduleName: "/ti/ndk/NIMU",
                description: "The NDK Interface to use for this SlNet Interface",
                collapsed: false,
                hidden: false,
                group: "ndkInterface"
            }
        );
    }

    return modules;
}


/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "SlNet",
    description: "Socket Layer IP Network System Configuration",
    defaultInstanceName: "CONFIG_SLNET_",
    longDescription: longDescription,
    config: config_instance,
    moduleInstances: moduleInstances,
    sharedModuleInstances: sharedModuleInstances,
    modules: modules,
    validate: validate,
    maxInstances: 16,
    templates: {
        /* contribute NS libraries to linker command file */
        "/ti/utils/build/GenLibs.cmd.xdt"   :
            {modName: "/ti/net/SlNet", getLibs: getLibs},

        /* trigger generation of ti_ndk_config.c */
        "/ti/net/Config.c.xdt": "/ti/net/SlNet.Config.c.xdt"
    }
};

/* export the module */
exports = base;
