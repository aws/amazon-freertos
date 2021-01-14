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

"use strict";

let Common = system.getScript("/ti/utils/Common.js");

/* Sysconfig string specifying the current device ID */
let deviceId = system.deviceData.deviceId;
let deviceFamily = Common.device2Family(system.deviceData);

/*
 *  ======== onChange_dataInputMethod ========
 */
function onChange_dataInputMethod(inst, ui)
{
    if(inst.dataInputMethod == "Reference to Secure Object Variable") {
        ui.secObjText.hidden = true;
        ui.secObjFile.hidden = true;
        ui.secObjVariable.hidden = false;
        ui.secObjVariableSize.hidden = false;
    }
    else if(inst.dataInputMethod == "secObjFile") {
        ui.secObjText.hidden = true;
        ui.secObjFile.hidden = false;
        ui.secObjVariable.hidden = true;
        ui.secObjVariableSize.hidden = true;
    }
    else {
        ui.secObjText.hidden = false;
        ui.secObjFile.hidden = true;
        ui.secObjVariable.hidden = true;
        ui.secObjVariableSize.hidden = true;
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
    /* config.secObjVariable */
    if (inst.dataInputMethod == "Reference to Secure Object Variable" && !inst.secObjVariable) {
        vo["secObjVariable"].errors.push("A variable name must be specified");
    }

    /* config.secObjVariableSize */
    if (inst.dataInputMethod == "Reference to Secure Object Variable" && !inst.secObjVariableSize) {
        vo["secObjVariableSize"].errors.push("A variable name must be specified");
    }

    /* config.secObjText */
    if (inst.dataInputMethod == "Secure Object Text" && !inst.secObjText) {
        vo["secObjText"].errors.push("Secure object text must be provided");
    }
}

/*
 *  ======== config_instance ========
 *  Define the config params of the module (module-wide)
 */
let config_instance = [
    {
        name: "$name",
        displayName: "Name",
        description: "Secure Object Name",
        hidden: false,
        longDescription: `
You will use this name whenever you need to refer to this secure object inside
your application source code. For instance, the **SlNetSock_secAttribSet()**
function takes this name as an argument.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_$name)`,
        documentation: `
This setting is equivalent to setting the \`objName\` argument in
[**SlNetIf_loadSecObj()**](/ns/html/group__SlNetIf.html#ga72f32689df9255833f05af282ae3639b).
`
    },
    {
        name: "secObjType",
        displayName: "Type",
        default: "SLNETIF_SEC_OBJ_TYPE_CERTIFICATE",
        description: "Secure Object Type",
        options: [
            {
                name: "SLNETIF_SEC_OBJ_TYPE_RSA_PRIVATE_KEY",
                displayName: "RSA Private Key",
                description: "RSA Private Key"
            },
            {
                name: "SLNETIF_SEC_OBJ_TYPE_CERTIFICATE",
                displayName: "Certificate",
                description: "Certificate"
            },
            {
                name: "SLNETIF_SEC_OBJ_TYPE_DH_KEY",
                displayName: "DH Key",
                description: "Diffie-Hellman Key"
            },
        ],
        longDescription: `
Indicate the Secure Object type.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_secObjType)`,
        documentation: `
This setting is equivalent to setting the \`objType\` argument in
[**SlNetIf_loadSecObj()**](/ns/html/group__SlNetIf.html#ga72f32689df9255833f05af282ae3639b).
`
    },
    {
        name: "dataInputMethod",
        displayName: "Data Input Method",
        description: "Method of adding the secure object",
        longDescription: `
Specify how to add the secure object in SysConfig.

__Reference to Secure Object Variables__ - Create variables containing the Secure Object and the
Secure Object size in a *.c file compiled with your project.

__Secure Object Text__ - Provide the raw text describing the Secure Object

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_dataInputMethod)
`,
        options: [
            {
                name: "Reference to Secure Object Variable",
                description: "Secure Object variables in the application"
            },
            {
                name: "Secure Object Text",
                description: "Raw text string containing the Secure Object"
            }
            /*
             * The following menu item is commented out because syscfg does
             * not yet have support for opening files in templates. It is
             * blocked on PMUX-2030.
             */
            // {
            //     name: "secObjFile",
            //     displayName: "Reference to secObj File"
            // }
        ],
        default: "Reference to Secure Object Variable",
        onChange: onChange_dataInputMethod
    },
    {
        name: "secObjVariable",
        displayName: "Secure Object Variable",
        default: "",
        description: "Secure Object Variable",
        longDescription: `
Name of the uint8_t array containing the Secure Object text

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_secObjVariable)`,
        documentation: `
This setting is equivalent to setting the \`objBuff\` argument in
[**SlNetIf_loadSecObj()**](/ns/html/group__SlNetIf.html#ga72f32689df9255833f05af282ae3639b).
`
    },
    {
        name: "secObjVariableSize",
        displayName: "Secure Object Variable Size",
        default: "",
        description: "Secure Object Variable Size",
        longDescription: `
Name of the int16_t variable containing the secure object size

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_secObjVariableSize)`,
        documentation: `
This setting is equivalent to setting the \`objBuffLen\` argument in
[**SlNetIf_loadSecObj()**](/ns/html/group__SlNetIf.html#ga72f32689df9255833f05af282ae3639b).
`
    },
    {
        name: "secObjText",
        description: "Secure Object Text",
        displayName: "Secure Object Text",
        default: "",
        hidden: true,
        /*
         * The property below was added with PMUX-2010, but is not on the
         * syscfg shipped with CCSv9.1. Until CCSv9.2 builds are released
         * you can only use this property while launching syscfg
         * from ti-pinmux-devtools or at least syscfg version 1.1.850
         * (instructions here: https://confluence.itg.ti.com/display/CCS/Getting+Started+Guide)
         * With multiline commented out syscfg will function but secObjText
         * will not be generated correctly in the template file.
         */
        multiline: true,
        longDescription: `
Paste the raw text of your secure object into this field.

[More ...](/ns/configdocs/${deviceFamily}/ConfigDoc.html#ti_net_SecObj_secObjText)`,
        documentation: `
This setting will generate the \`objBuff\` and \`objBuffLen\` arguments for
[**SlNetIf_loadSecObj()**](/ns/html/group__SlNetIf.html#ga72f32689df9255833f05af282ae3639b).
`

    },
    {
        name: "secObjFile",
// hide from docs
//        displayName: "Secure Object File",
        default: "",
        hidden: true,
        fileFilter: "*.*",
        placeholder: "Secure Object File",
        description: "Secure Object File"
    }
];

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "Secure Object",
    description: "Secure Object configuration",
    defaultInstanceName: "CONFIG_SECOBJ_",
//    longDescription: longDescription,
    config: config_instance,
    validate: validate
};

/* export the module */
exports = base;
