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
 *  ======== Common.js ========
 */

exports = {
    /* function to configure device-specific implementation */
    addImplementationConfig: addImplementationConfig,

    boardName: boardName,  /* get /ti/boards name */

    /* CC32XX specific function to convert package pin to device pin */
    cc32xxPackage2DevicePin : cc32xxPackage2DevicePin,

    device2Family: device2Family,  /* get /ti/drivers device family name */
    device2DeviceFamily: device2DeviceFamily, /* get driverlib DeviceFamily_xxx name */
    device2DevicesDir: device2DevicesDir, /* get driverlib 'devices' dir */

    getCompName: getCompName,      /* get fully qualified component name */
    getConfigs: getConfigs,        /* get all of a module's config params */

    getName: getName,              /* get C name for instance */
    getSymbolicName: getSymbolicName,
    getPort: getPort,              /* get pin port name: Px_y */
    getInstanceIndex: getInstanceIndex, /* returns mod.$instances array index of inst */
    getLibSuffix: getLibSuffix, /* returns library file suffixes specific to drivers tree */
    isCName: isCName,              /* validate name is C identifier */
    genAliases: genAliases,        /* generate list of instance name aliases */
    pinToName: pinToName,          /* convert pin number to pin name */
    intPriority2Hex: intPriority2Hex, /* translate logical priority into a
                                       * valid hex value
                                       */
    swiPriority2Int: swiPriority2Int,
    logError: logError,           /* push error onto into validation obj */
    logWarning: logWarning,       /* push warning onto into validation obj */
    logInfo: logInfo,             /* push remark onto into validation obj */

    newConfig: newConfig,         /* create /ti/drivers config structure */

    newIntPri: newIntPri,         /* create a common 'intPriority' config */

    newSwiPri: newSwiPri,         /* create a common 'swiPriority' config */

    print: print,                 /* debug: print specified object */
    printModule: printModule,     /* debug: print module configs and methods */
    printPins: printPins,         /* debug: print pinmux solution */

    useModule: useModule,

    typeMatches: typeMatches,
    setDefaults: setDefaults,

    validateNames: validateNames, /* validate inst names are unique C names */

    addNameConfig: addNameConfig, /* add driver-specific $name config */

    autoForceModules: autoForceModules,
    genBoardHeader: genBoardHeader,
    genBoardDeclarations : genBoardDeclarations,
    genResourceComment: genResourceComment,
    findSignalTypes : findSignalTypes,

    init: init
};

/*
 *  ======== IModule ========
 *  Names and types defined by _all_ sysconfig modules
 */
let IModule = {
    config               : [],
    defaultInstanceName  : "",
    description          : "",
    displayName          : "",
    filterHardware       : function (component){return false;},
    longDescription      : "",
    groups               : [], /* unused?? */
    layout               : [], /* unused?? */
    maxInstances         : 0,
    moduleInstances      : function (inst){return [];},
    moduleStatic         : {/* IModule declaration */},
    modules              : function (inst){return [];},
    onHardwareChanged    : function (inst, ui){},
    pinmuxRequirements   : function (inst){return [];},
    sharedModuleInstances: function (inst){return [];},
    templates            : {},
    validate             : function (inst, validation){}
};

let deferred = {
    errs: [],
    warn: [],
    info: [],
    logError:   function (inst, field, msg) {this.errs.push({inst: inst, field: field, msg: msg});},
    logWarning: function (inst, field, msg) {this.warn.push({inst: inst, field: field, msg: msg});},
    logInfo:    function (inst, field, msg) {this.info.push({inst: inst, field: field, msg: msg});}
};

/*!
 *  ======== addImplementationConfig ========
 *  Displays device-specific driver impelementation. This function is intended
 *  to be called in the extend function of the <driver><device>.syscfg.js file.
 *
 *  @param[in] base: The base exports object needing to be modified
 *  @param[in] driver: A string formatted how the driver name is intended to
 *  be displayed in the UI
 *  @param[in] defaultOption: The default driver implementation. If there is
 *  only one implementation, this param can be passed in as null since the
 *  information will be extracted from the options param
 *  @param[in] options: an array in the format of {name: "driverImplementation"}
 *  for each available implementation. Separate each implementation with a comma
 *  @param[in] longDescription: String with a driver-specific description that
 *  will to be added to the base long description. The value of this param is
 *  null if no extra description is desired.
 *  @return The modified base exports object
 */
function addImplementationConfig(base, driver, defaultOption=null, options,
    longDescription=null)
{
    if ((options.length > 1) && (defaultOption == null)) {
        throw new Error("Common.js, addImplementationConfig(): Invalid params" +
            " passed in!");
    }

    let description = "Displays " + driver + " delegates available for the " +
        system.deviceData.deviceId + " device.";

    let implementationCountDescription = `
Since there is only one delegate, it is a read-only value that cannot be changed.
`;

    let readOnly = false;
    if (options.length == 1) {
        readOnly = true;

        /* If there's only one option, set that option to be the default */
        defaultOption = options[0].name;
    }
    else if (options.length > 1) {
        implementationCountDescription = `
Since there is more than one delegate for this module, this option allows you to
choose which driver you would like to use.
`;
    }

    let baseDescription = description + "\n\n" + implementationCountDescription +
`Please refer to the [__TI-Drivers implementation matrix__][0] for all
available drivers and documentation.

[0]: /drivers/doxygen/html/index.html#drivers
`;

    /* Add driver-specific description onto the base description */
    if (longDescription != null) {
        baseDescription = baseDescription.concat(longDescription);
    }

    let config = [{
        name: driver.toLowerCase() + "Implementation",
        displayName: driver + " Implementation",
        default: defaultOption,
        options: options,
        readOnly: readOnly,
        description: description,
        longDescription : baseDescription
    }];

    /* Determine if base contains a module static */
    if (base.moduleStatic && base.moduleStatic.config) {
        base.moduleStatic.config = config.concat(base.moduleStatic.config);
    }
    else {
        let globalName = base.displayName.toLowerCase() + "Global";
        let globalDisplayName = base.displayName + " Global";

        base.moduleStatic = {
            name: globalName,
            displayName: globalDisplayName,
            config: config
        };
    }

    return (base);
}

/*
 *  ======== init ========
 */
function init()
{
}

/*
 *  ======== useModule ========
 */
function useModule(name)
{
    let mod = {};
    mod.$mod = scripting.addModule(name);

    /* hoist module statics to mod */
    let spec = system.getScript(mod.$mod.$name.split('$')[0]);
    if (spec.moduleStatic != null && spec.moduleStatic.config != null) {
        let configs = spec.moduleStatic.config;
        for (var i = 0; i < configs.length; i++) {
            let p = configs[i].name;
            let setter = function(x) { this.$mod[p] = x; };
            Object.defineProperty(mod, p, {set: setter});
        }
    }

    /* add create method */
    mod.create = function (name, params) {
        if (params == null) {
            params = {};
        }
        let inst = mod.$mod.addInstance();
        if (name) {
            inst.$name = name;
        }
        for (let p in params) {
            if ("$hardware" == p && params[p] === undefined) {
                console.log("warning: " + name
                            + ".$hardware was set to undefined");
            }
            if (params[p] !== undefined) {
                inst[p] = params[p];
            }
        }
        return (inst);
    };
    return (mod);
}


/*!
 *  ======== boardName ========
 *  Get the name of the board (or device)
 *
 *  @returns String - Name of the board with prefix /ti/boards and
 *                    suffix .syscfg.json stripped off.  If no board
 *                    was specified, the device name is returned.
 */
function boardName()
{
    let boardName = system.deviceData.deviceId;

    if (system.deviceData.board != null) {
        boardName = system.deviceData.board.source;

        /* Strip off everything up to and including the last '/' */
        boardName = boardName.replace(/.*\//, '');

        /* Strip off everything after and including the first '.' */
        boardName = boardName.replace(/\..*/, '');
    }
    return (boardName);
}

/*!
 *  ======== cc32xxPackage2DevicePin ========
 *  Converts a CC32XX package pin number to the device pin number
 *
 *  @param packagePinName  The packagePinName returned to a $solution
 *
 *  @returns String  The device pin number as an integer.
 */
function cc32xxPackage2DevicePin(packagePinName)
{
    let pin = "";
    let key = String(packagePinName);
    let devicePins = system.deviceData.devicePins;

    if (devicePins[key]) {
        pin = devicePins[key].controlRegisterOffset;
        pin = pin.padStart(2, "0");
    }

    return (pin);
}

/*!
 *  ======== device2Family ========
 *  Map a pimux device object to a TI-driver device family string
 *
 *  @param device   - a pinmux device object (system.deviceData)
 *  @param mod      - module name ("ADC", "PWM", etc)
 *
 *  @returns String - the name of a device family that's used to
 *                    create family specifc-implementation module
 *                    names.  Returns null, in the event of an
 *                    unsupported device.
 */
function device2Family(device, mod)
{
    /* device.deviceId prefix -> /ti/drivers family name */
    let DEV2FAMILY = [
        {prefix: "CC13.2",   family: "CC26X2"},
        {prefix: "CC26.2",   family: "CC26X2"},
        {prefix: "CC13.1",   family: "CC26X1"},
        {prefix: "CC26.1",   family: "CC26X1"},
        {prefix: "CC13",     family: "CC26XX"},
        {prefix: "CC26",     family: "CC26XX"},
        {prefix: "CC32",     family: "CC32XX"},
        {prefix: "MSP432E",  family: "MSP432E4"}
    ];

    /* Agama Classic (CC26X2 and CC26X2F6) specific module delegates */
    let cc26x2Mods = {
        "ECDH" :        "CC26X2",
        "ECDSA" :       "CC26X2",
        "ECJPAKE" :     "CC26X2",
        "SHA2" :        "CC26X2",
        "Temperature" : "CC26X2"
    };

    /* Agama Lite (CC26X1) specific module delegates */
    let cc26x1Mods = {
        "ECDH" :        "CC26X1",
        "ECDSA" :       "CC26X1",
        "SHA2" :        "CC26X1",
        "Temperature" : "CC26X2"
    };

    /* deviceId is the directory name within the pinmux/deviceData */
    let deviceId = device.deviceId;

    for (let i = 0; i < DEV2FAMILY.length; i++) {
        let d2f = DEV2FAMILY[i];

        if (deviceId.match(d2f.prefix)) {
            /* trap Agama specific mods */
            if (d2f.family == "CC26X2") {
                if (mod in cc26x2Mods) {
                    return (cc26x2Mods[mod]);
                }
                else {
                    return ("CC26XX");
                }
            }
            else if (d2f.family == "CC26X1") {
                if (mod in cc26x1Mods) {
                    return (cc26x1Mods[mod]);
                }
                else {
                    return ("CC26XX");
                }
            }
            else {
                return (d2f.family);
            }
        }
    }
    return (null);
}

/*!
 *  ======== device2DeviceFamily ========
 *  Map a pimux deviceID to a TI-driver device family string
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding "DeviceFamily_xxxx" string
 *                    used by driverlib header files.
 */
function device2DeviceFamily(deviceId, part)
{
    var DriverLib = system.getScript("/ti/devices/DriverLib");

    let attrs = DriverLib.getAttrs(deviceId, part);

    return (attrs.deviceDefine);
}

/*!
 *  ======== device2DevicesDir ========
 *  Map a pimux deviceId to a driverlib 'devices' dir
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns String - the corresponding ti/devices/--- string
 */
function device2DevicesDir(deviceId)
{
    var DriverLib = system.getScript("/ti/devices/DriverLib");

    let attrs = DriverLib.getAttrs(deviceId);

    return (attrs.deviceDir);
}

/*
 *  ======== getCompName ========
 *  Get fully qualified name of a (sub)component
 */
function getCompName(component)
{
    var name = component.name;
    if (component.$parents && component.$parents.length > 0) {
        name = getCompName(component.$parents[0]) + "." + name;
    }
    return (name);
}

/*
 *  ======== getInstanceIndex ========
 *  returns module.$instances array index of this inst
 */
function getInstanceIndex(inst)
{
    let instances = inst.$module.$instances;
    for (let i = 0; i < instances.length; i++) {
        if (inst == instances[i]) {
            return (i);
        }
    }
    return (-1);
}

/*
 *  ======== getLibSuffix ========
 *  Returns the filename extension (suffix) appended to library names
 *
 *  This function assumes valid inputs
 *
 *  The drivers repository currently does not follow the SimpleLink
 *  library naming convention. When updated to do so, calls to this
 *  function may be replaced by:
 *      /ti/utils/build/.meta/GenLibs.syscfg.js::libPath()
 *
 *  @param isa  -  The isa string. For example "m3" or "m4", "m4f"
 *
 *  @param toolchain  -  The toolchain string. For example "ticlang" or "gcc"
 */
function getLibSuffix(isa, toolchain)
{
    let tc2ext = {
        ccs: {
            prefix: "ae",
            suffix: ""
        },
        gcc: {
            prefix: "a",
            suffix: "g"
        },
        iar: {
            prefix: "ar",
            suffix: ""
        },
        ticlang: {
            prefix: "a",
            suffix: ""
        }
    };

    let suffix = tc2ext[toolchain].prefix + isa + tc2ext[toolchain].suffix;
    suffix = "." + suffix;

    return (suffix);
}


/*
 *  ======== getConfigs ========
 *  Return a description of all the configs of the specified module
 *
 *  The description is an object of the form:
 *   {
 *      modName: "",  // full name of the module
 *      baseName: "", // only the last component of the full name
 *      family: ""    // the TI-DRIVERS device family used
 *      mod:  {
 *          portable: {}, // map of all portable module configs
 *          device: {},   // map of all device-specific module configs
 *      },
 *      inst: {
 *          portable: {}, // map of all portable instance configs
 *          device: {},   // map of all device-specific instance configs
 *      }
 *   }
 *  Each config object is of the form:
 *   {
 *      name: "",                         // name of the confg param
 *      default: "",                      // default value
 *      options: [{name: "", ...}, ...],  // array of "named" options
 *   }
 */
function getConfigs(modName)
{
    /* flatten GUI grouping within a config array */
    function flattenGroups(cfgArray) {
        let result = [];
        cfgArray = (cfgArray == null) ? [] : cfgArray;
        for (var i = 0; i < cfgArray.length; i++) {
            var elem = cfgArray[i];
            if (elem.name == null) {
                result = result.concat(flattenGroups(elem.config));
            }
            else {
                result.push(elem);
            }
        }
        return (result);
    }

    /* deep clone needed to prevent inadvertent model corruption */
    function clone(obj) {
        return (JSON.parse(JSON.stringify(obj)));
    }

    /* get current device family */
    let baseName = modName.substring(modName.lastIndexOf("/") + 1);
    let family = device2Family(system.deviceData, baseName);
    let result = {
        modName: modName, baseName: baseName, family: family,
        mod:  {portable: {}, device: {}},
        inst: {portable: {}, device: {}}
    };

    /* get all instance configs */
    let Mod = undefined;
    let allConfigs = [];
    try {
        Mod = system.getScript(modName);
        if (Mod.config) {
            allConfigs = Mod.config;
        }
    }
    catch (x) {
        //console.log("can't find script: '" + modName + "': " + x);
        return result;
    }
    allConfigs = flattenGroups(allConfigs);

    /* determine device-specific instance configs */
    let ModDev = undefined;
    let deviceConfigs = {};
    let portableConfigs = {};
    let base = {moduleStatic: {config:[]}, config: []};
    let devModName =
        modName.substring(0, modName.lastIndexOf('/'))
        + '/' + baseName.toLowerCase() + '/'
        + baseName + family + ".syscfg.js";
    try {
        ModDev = system.getScript(devModName);
        base = ModDev.extend(base);
        if (base.config) {
            base.config = flattenGroups(base.config);
            for (let i = 0; i < base.config.length; i++) {
                let cfg = base.config[i];
                deviceConfigs[cfg.name] = cfg;
            }
        }
    }
    catch (x) {
        //console.log("can't find script: '" + devModName + "': " + x);
    }

    /* determine portable instance configs */
    for (let i = 0; i < allConfigs.length; i++) {
        let cfg = allConfigs[i];
        if (deviceConfigs[cfg.name] == null) {
            portableConfigs[cfg.name] = cfg;
        }
    }
    result.inst.portable = portableConfigs;
    result.inst.device = deviceConfigs;

    /* get all module configs */
    allConfigs = [];
    if (Mod.moduleStatic && Mod.moduleStatic.config) {
        allConfigs = Mod.moduleStatic.config;
    }
    allConfigs = flattenGroups(allConfigs);

    /* determine device-specific module configs */
    deviceConfigs = {};
    portableConfigs = {};
    if (base.moduleStatic && base.moduleStatic.config) {
        let modCfgs = flattenGroups(base.moduleStatic.config);
        for (let i = 0; i < modCfgs.length; i++) {
            let cfg = modCfgs[i];
            deviceConfigs[cfg.name] = cfg;
        }
    }

    /* determine portable module configs */
    for (let i = 0; i < allConfigs.length; i++) {
        let cfg = allConfigs[i];
        if (deviceConfigs[cfg.name] == null) {
            portableConfigs[cfg.name] = cfg;
        }
    }
    result.mod.portable = portableConfigs;
    result.mod.device = deviceConfigs;

    /* make $name appear to be a regular config parameter */
    var nameCfg = result.inst.portable["$name"];
    if (nameCfg != null) {
        /* clone nameCfg so we can add default without breaking
         * addModule/addInstance
         */
        nameCfg = clone(nameCfg);
        nameCfg.displayName = "Name";
        nameCfg.default = Mod.defaultInstanceName + "{num}";
        result.inst.portable["$name"] = nameCfg;
    }

    return (result);
}

/*
 *  ======== getName ========
 *  Compute C name for specified instance
 */
function getName(inst, index)
{
    return (inst.$name);
}

/*
 *  ======== getSymbolicName ========
 *  Compute private name for an instance.
 *
 *  This name is used exclusively inside of the ti_drivers_config.c
 *  source file.
 *
 *  The name returned _may_ not be a C identifier.
 */
function getSymbolicName(inst)
{
    return (inst.$name + "_CONST");
}

/*
 *  ======== getPort ========
 *  Return pin port name or ""
 *
 *  @param pin - pin object representing the
 *               solution found by pinmux
 *  @returns string of the form P<port>_<index>
 */
function getPort(pin)
{
    let port = "";
    if (pin != null && pin.$solution != null) {
        let pname = pin.$solution.devicePinName;
        port = pname.match(/P\d+\.\d+/);
        port = port ? port[0] : "";

    }
    return port.replace(".", "_");
}

/*
 *  ======== isCName ========
 *  Determine if specified id is either empty or a valid C identifier
 *
 *  @param id  - String that may/may not be a valid C identifier
 *
 *  @returns true if id is a valid C identifier OR is the empty
 *           string; otherwise false.
 */
function isCName(id)
{
    if ((id != null && id.match(/^[a-zA-Z_][0-9a-zA-Z_]*$/) != null)
            || id == '') { /* '' is a special value that means "default" */
        return true;
    }
    return false;
}

/*
 *  ======== genAliases ========
 */
function genAliases(inst, name)
{
    let lines = [];
    if (inst.cAliases != null && inst.cAliases.length > 0) {
        let aliases = inst.cAliases.split(/[,;\s]+/);
        for (let i = 0; i < aliases.length; i++) {
            let alias = aliases[i];
            lines.push("    " + alias + " = " + name + ",");
        }
        lines.push('');
    }
    return (lines.join('\n'));
}

/*
 *  ======== logError ========
 *  Log a new error
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this error is associated
 */
/* global global */
function logError(vo, inst, field, msg)
{
    if (typeof global != "undefined" && global.__coverage__) {
        vo = deferred;
    }
    if (typeof(field) === 'string') {
        vo.logError(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo.logError(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== logInfo ========
 *  Log a new remark
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this remark is associated
 */
function logInfo(vo, inst, field, msg)
{
    if (typeof global != "undefined" && global.__coverage__) {
        vo = deferred;
    }
    if (typeof(field) === 'string') {
        vo.logInfo(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo.logInfo(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== logWarning ========
 *  Log a new warning
 *
 *  @param vo     - a validation object passed to the validate() method
 *  @param inst   - module instance object
 *  @param field  - instance property name, or array of property names, with
 *                  which this warning is associated
 */
function logWarning(vo, inst, field, msg)
{
    if (typeof global != "undefined" && global.__coverage__) {
        vo = deferred;
    }
    if (typeof(field) === 'string') {
        vo.logWarning(msg, inst, field);
    }
    else {
        for (let i = 0; i < field.length; i++) {
            vo.logWarning(msg, inst, field[i]);
        }
    }
}

/*
 *  ======== newConfig ========
 */
function newConfig()
{
    let config = [
        {
            name: "cAliases",
            displayName: "Alternate Names",
            description: "A comma separated list of valid C identifiers",
            default: ""
        }
    ];

    return (config);
}

/*
 *  ======== newIntPri ========
 *  Create a new intPriority config parameter
 *
 *  Presents a constrained set of logical NVIC interrupt priorities,
 *  excluding priority 0, which is the "Zero Latency"
 *  interrupt priority.
 */
function newIntPri()
{
    let intPri = [{
        name: "interruptPriority",
        displayName: "Hardware Interrupt Priority",
        description: "Hardware interrupt priority",
        longDescription:`This configuration allows you to configure the
hardware interrupt priority.
`,
        default: "7",
        options: [
            { name: "7", displayName: "7 - Lowest Priority" },
            { name: "6" },
            { name: "5" },
            { name: "4" },
            { name: "3" },
            { name: "2" },
            { name: "1", displayName: "1 - Highest Priority" }
        ]
    }];

    return (intPri);
}

/*
 * ======== intPriority2Hex ========
 * translate user readable priority into NVIC priority value
 */
function intPriority2Hex(intPri)
{

    /*
     *(~0) is always interpreted as the lowest priority.
     * NoRTOS DPL does not support "7" as a HwiP priority.
     */
    if (intPri == "7") {

        return ("(~0)");
    }

    return ("0x" + (intPri << 5).toString(16));
}

/*
 *  ======== newSwiPri ========
 *  Create a new swiPriority config parameter
 */
function newSwiPri()
{
    let swiPri = {
        name: "softwareInterruptPriority",
        displayName: "Software Interrupt Priority",
        description: "Software interrupt priority",
        longDescription:`This configuration allows you to configure the
software interrupt priority.
`,
        default: "0",
        options: [
            { name: "0", displayName: "0 - Lowest Priority" },
            { name: "1" },
            { name: "2" },
            { name: "3" },
            { name: "4" },
            { name: "5" },
            { name: "6" },
            { name: "7" },
            { name: "8" },
            { name: "9" },
            { name: "10" },
            { name: "11" },
            { name: "12" },
            { name: "13" },
            { name: "14" },
            { name: "15", displayName: "15 - Highest Priority" }
        ]
    };

    return (swiPri);
}

/*
 * ======== swiPriority2Int ========
 * translate user priority to integer
 */
function swiPriority2Int(swiPri)
{
    return (swiPri);
}

/*
 *  ======== pinToName ========
 * Translate device pin number into pin name
 */
function pinToName(pinNum)
{
    for (let x in system.deviceData.devicePins) {
        if (system.deviceData.devicePins[x].ball == pinNum) {
            let desc = String(system.deviceData.devicePins[x].description);
            return (desc);
        }
    }

    return ("");
}

/*
 *  ======== print ========
 *  Print specified obj
 */
function print(obj, header, depth, indent)
{
    if (indent == null) indent = "";
    if (header == null) header = "";
    if (depth == null) depth = 4;
    if (indent.length > 2 * depth) return;

    if (obj == null) {
        console.log(header + (obj === null ? "null" : "undefined"));
        return;
    }
    if (indent == "") {
        if (obj.$name != null) {
            console.log(header + obj.$name + ":");
        }
        else if (obj.name != null) {
            console.log(header + obj.name + ":");
        }
        else {
            console.log(header + obj + " (nameless):");
        }
    }

    for (let p in obj) {
        /* print the enumerable properties of obj and their value */
        let line = indent + "  " + p + ": ";

        let value = obj[p];

        let fxn = null;
        if (typeof value == "function") {
            try {
                var src = String(value);
                var k = src.indexOf('{');
                if (k <= 0) {
                    k = src.indexOf('\n');
                }
                fxn = src.substring(0, k);
            } catch (x){/* ignore any exception */}
        }
        if (fxn != null) {
            /* only print declaration of functions (not their entire source) */
            console.log(line + fxn);
        }
        else if (value != null && (typeof value == "object")) {
            console.log(line
                + (("$name" in value) ? (" (" + value.$name + ")") : ""));

            if (p[0] != '$') {
                /* recursively print (non-tool) object properties */
                print(value, "", depth - 1, indent + "  ");
            }
        }
        else {
            /* print non-object values */
            console.log(line + value);
        }
    }
}

/*
 *  ======== printModule ========
 */
function printModule(mod, header, indent)
{
    if (indent == null) indent = "";
    if (header == null) header = "";

    function printConfigs(configs, indent) {

        /* recursively find config arrays and accumulate into flat array */
        function flatten(cfgs, result) {
            if (cfgs != null) {
                for (var i = 0; i < cfgs.length; i++) {
                    var cfg = cfgs[i];
                    if (cfg.name == null) { /* must be a config group */
                        flatten(cfg.config, result);
                    }
                    else {
                        result.push(cfg);
                    }
                }
            }
        }

        if (configs != null && configs.length != 0) {
            /* accumulate all configs */
            var flatCfgs = [];
            flatten(configs, flatCfgs);
            configs = flatCfgs;

            /* maxPad is the maximum field width */
            let maxPad = "                           ";

            /* compute max config name field width */
            let maxName = 1;
            for (let i = 0; i < configs.length; i++) {
                let len = configs[i].name.length;
                if (len > maxName) {
                    maxName = len;
                }
            }
            if (maxName > maxPad.length) {
                maxName = maxPad.length;
            }

            /* compute max config default value field width */
            let maxValue = 1;
            for (let i = 0; i < configs.length; i++) {
                let len = String(configs[i].default).length;
                if (len > maxValue) {
                    maxValue = len;
                }
            }
            if (maxValue > maxPad.length) {
                maxValue = maxPad.length;
            }

            /* print all configs */
            for (let i = 0; i < configs.length; i++) {
                let nLen = configs[i].name.length;
                let vLen = String(configs[i].default).length;
                var npad = maxPad.substring(0, maxName - nLen);
                var vpad = maxPad.substring(0, maxValue - vLen);
                printCfg(configs[i], indent + "  ", npad, vpad);
            }
        }
        else {
            console.log(indent + "  <none>");
        }
    }

    function printCfg(cfg, indent, npad, vpad) {
        npad = npad == null ? "" : npad;
        vpad = vpad == null ? "" : vpad;
        let quote = " ";
        if (typeof cfg.default == "string" || cfg.default instanceof String) {
            quote = "'";
        }
        let desc = cfg.description != null ? (" - " + cfg.description) : "";
        console.log(indent + cfg.name + npad
                    + ": " + quote + cfg.default + quote + vpad
                    + " <" + cfg.displayName + desc + ">"
        );
        printCfgOptions(cfg, indent, quote);
    }

    function printCfgOptions(cfg, indent, quote) {
        /* maxPad is the maximum field width */
        let maxPad = "                           ";

        if (cfg.options != null) {
            let maxName = 0;
            for (let i = 0; i < cfg.options.length; i++) {
                let name = cfg.options[i].name;
                if (maxName < name.length) {
                    maxName = name.length;
                }
            }

            let prefix = indent + "   " + quote;
            for (let i = 0; i < cfg.options.length; i++) {
                let opt = cfg.options[i];
                let desc = opt.description != null ? (" - " + opt.description) : "";
                let displayName = opt.displayName == null ? opt.name : opt.displayName;
                let suffix = ((opt.displayName == opt.name || opt.displayName == null) && desc.length == 0) ? "" : (" <" + displayName + desc +  ">");
                let namePad = maxPad.substring(0, maxName - opt.name.length + 1);
                console.log(prefix + opt.name + quote + namePad + suffix );
            }
        }
    }

    function printLine(line, indent, maxLen) {
        let curline = line;
        while (curline.length > 0) {
            let lastSpace;
            let i;

            /* trim leading space */
            for (i = 0; (curline[i] == ' ') && (i < curline.length);) {
                i++;
            }
            curline = curline.substring(i);
            if (curline.length == 0) return;

            /* find last white space within maxLen characters */
            lastSpace = 0;
            for (i = 0; (i < curline.length) && (i < maxLen); i++) {
                if (curline[i] == ' ') {
                    lastSpace = i;
                }
            }
            if (i == curline.length) {
                lastSpace = i;
            }
            else if (i == maxLen && lastSpace == 0) {
                /* advance i to first space or end of string */
                while ((i < curline.length) && (curline[i] != ' ')) {
                    i++;
                }
                lastSpace = i;
            }

            /* print up to lastSpace and reset curline */
            console.log(indent + curline.substring(0, lastSpace));
            curline = curline.substring(lastSpace);
        }
    }

    /* get the real module object (gack!) */
    mod = system.getScript(mod.$name.split('$')[0]);
    console.log(header + mod.$name + " - " + mod.description);

    indent += "  ";

    console.log(indent + "Module Description");
    if  (mod.longDescription) {
        let lines = mod.longDescription.split('\n');
        for (let i = 0; i < lines.length; i++) {
            printLine(lines[i], indent + "  ", 60);
        }
    }
    else {
        console.log(indent + "  <none>");
    }

    console.log("\n" + indent + "Module Configuration Options");
    if (mod.moduleStatic != null) {
        printConfigs(mod.moduleStatic.config, indent);
    }
    else {
        console.log(indent + "  <none>");
    }

    console.log("\n" + indent + "Instance Configuration Options");
    printConfigs(mod.config, indent);

    console.log("\n" + indent + "Module Methods");
    let methods = [];
    for (let m in mod) {
        if (typeof mod[m] == "function") {
            let mname = String(mod[m]).match(/function [^)(]+(\([^)]+\))/);
            if (mname == null) {
                mname = "function ()";
            }
            else {
                mname = "function " + mname[1];
            }
            let prefix = "";
            if (m in IModule && typeof IModule[m] == "function") {
                prefix = "IModule::";
            }
            methods.push(indent + "  " + prefix + m + ": " + mname);
        }
    }
    console.log(methods.sort().join('\n'));
}

/*
 *  ======== printPins ========
 *  Print specified pinmux solution
 */
function printPins(obj, indent)
{
    if (indent == null) {
        indent = "    ";
    }
    if (obj == null) {
        console.log(obj === null ? "null" : "udefined");
        return;
    }
    let solution = obj.$solution;
    let resources = obj.$resources;
    let peripheral = system.deviceData.peripherals[solution.peripheralName];
    console.log(indent + "peripheral.name = " + peripheral.name);
    for (let p in resources) {
        console.log(indent + "    resources." + p
                    + " = " + resources[p].$solution.devicePinName);
        //            for (let q in resources[p].$solution) {
        //                console.log("          " + q + ": "
        //                    + resources[p].$solution[q]);
        //            }
    }
}

/*
 *  ======== typeMatches ========
 *  Check that HW signal type matches a specified array of types
 *
 *  Example: within a module's filterHardware(component) method:
 *      for (sig in component.signals) {
 *          let type = component.signals[sig].type;
 *          if (Common.typeMatches(type, ["PWM", "DOUT"])) {
 *              :
 *          }
 *      }
 *
 *  type      - a string or array of strings that are valid signal types
 *  nameArray - array of signal name types that must match one of the signal
 *              types named by type
 *
 *  Returns true iff nameArray contains at least one type name that's
 *          specified the type parameter.
 */
function typeMatches(type, nameArray)
{
    let options = {};

    if (type instanceof Array || typeof type == "object") {
        for (let i = 0; i < type.length; i++) {
            options[type[i]] = 1;
        }
    }
    else if (typeof type == "string" || type instanceof String) {
        options[type] = 1;
    }

    for (let i = 0; i < nameArray.length; i++) {
        let name = nameArray[i];
        if (name in options) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== setDefaults ========
 *  Copy properties from a signal's settings into inst
 *
 *  inst   - the instance whose properties need to be modified by
 *           an inst.$hardware assignment
 *  signal - a signal object defined by a component
 *  type   - a specific type for the signal (DOUT, PWM, ...)
 *
 *  If type is undefined or null, no properties of inst are modified.
 */
function setDefaults(inst, signal, type)
{
    let settings = {};

    let comp = inst.$hardware;

    /* populate settings hash from the specified signal settings */
    if (signal.settings != null) {
        if (signal.type instanceof Array && signal.type.length > 1) {
            settings = signal.settings[type];
            /* if settings == null, there aren't settings for this type */
        }
        else {
            settings = signal.settings;
        }
    }

    /* allow component settings to override signal settings */
    if (comp.settings != null) {
        let csettings = null;
        if (comp.type instanceof Array && comp.type.length > 1) {
            csettings = comp.settings[type];
            /* if settings == null, there aren't settings for this type */
        }
        else {
            csettings = comp.settings;
        }
        for (let cfg in csettings) {
            settings[cfg] = csettings[cfg];
        }
    }

    /* apply any settings to the instance */
    for (let cfg in settings) {
        /* ensure cfg is in inst */
        if (inst.$uiState[cfg] != null) {
            /* assign it as specified by the HW (unless cfg is readonly) */
            if (!inst.$uiState[cfg].readOnly) {
                inst[cfg] = settings[cfg];
            }
        }
        else {
            /* if comp references a bogus cfg setting throw "bad board file" */
            let cname = comp.name;
            let parents = comp.$parents;
            for (; parents.length > 0; parents = parents[0]) {
                cname = parents[0].name + "." + cname;
            }
            let msg = "invalid board data: signal '" + signal.name
                + "' of component '" + cname + "' "
                + "specified an unknown setting (" + cfg + " = " + settings[cfg]
                + ") for " + inst.$name;
            throw new Error(msg); /* needed to detect bogus board data */
        }
    }
}

/*
 *  ======== validateNames ========
 *  Validate that all names defined by inst are globally unique and
 *  valid C identifiers.
 */
function validateNames(inst, validation)
{
    let myNames = {}; /* all C identifiers defined by inst) */

    /* check that $name is a C identifier */
    if (inst.$name != "") {
        let token = inst.$name;
        if (!isCName(token)) {
            logError(validation, inst, "$name",
                "'" + token + "' is not a valid a C identifier");
        }
        myNames[token] = 1;
    }

    /* check that cAliases are all C identifiers and there are no dups */
    let tokens = [];
    if ("cAliases" in inst && inst.cAliases != "") {
        tokens = inst.cAliases.split(/[,;\s]+/);
    }

    for (let i = 0; i < tokens.length; i++) {
        let token = tokens[i];
        if (!isCName(token)) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is not a valid a C identifier");
        }
        if (myNames[token] != null) {
            logError(validation, inst, "cAliases",
                "'" + token + "' is defined twice");
        }
        myNames[token] = 1;
    }

    /* ensure all inst C identifiers are globally unique */
    let mods = system.modules;
    for (let i in mods) {
        /* for all instances in all modules in /ti/drivers ... */
        let instances = mods[i].$instances;
        for (let j = 0; j < instances.length; j++) {
            let other = instances[j];

            /* skip self */
            if (inst.$name == other.$name) {
                continue;
            }

            /* compute all other names */
            let name = other.$name;
            if (name != "" && name in myNames) {
                logError(validation, inst, "cAliases",
                    "multiple instances with the same name: '"
                         + name + "': " + inst.$name + " and " + other.$name);
                break;
            }
            if (other.cAliases != null && other.cAliases != "") {
                let tokens = other.cAliases.split(/[,;\s]+/);
                for (let k = 0; k < tokens.length; k++) {
                    name = tokens[k];
                    if (name != "" && name in myNames) {
                        logError(validation, inst, "cAliases",
                            "multiple instances with the same name: '" + name
                                 + "': " + inst.$name + " and " + other.$name);
                        break;
                    }
                }
            }
        }
    }
}

/*
 *  ======== addNameConfig ========
 *  Add $name config for context-sensitive and reference documentation
 */
function addNameConfig(config, modName, prefix)
{
    let baseName = modName.split('/').pop();                 // GPIO
    let fullName = modName.replace(/\//g, '_').substring(1); // ti_drivers_GPIO
    //let docsDir =  modName.split('/').slice(0, -1).join(""); // drivers
    let docsDir = "drivers"; // Since this function is drivers specific

    let nameCfg = {
        name: "$name",
        description: "The C/C++ identifier used in applications as the index"
                   + " parameter passed to " + baseName + " runtime APIs",

        /* TODO: The name should be declared as an extern const in ti_drivers_config.h
         * and defined in ti_drivers_config.c. Using an extern const
         * allows libraries to define symbolic names for GPIO
         * signals they require for their use _without_ requiring
         * editing or rebuilding of the library source files.
         */
        longDescription: "This name is declared in the generated ti_drivers_config.h"
                   + " file so applications can reference this instance"
                   + " symbolically. Additionally, this name is used to declare"
                   + " an 'extern const' which allows libraries to define symbolic"
                   + " names for required driver configurations _without_ needing to"
                   + " rebuild library source files. The 'const' identifier is"
                   + " declared as the same name with a ___CONST__ suffix. The name can"
                   + " be set to any globally unique name that is also"
                   + " a valid C/C++ identifier.\n"
                   + "[More ...](/" + docsDir + "/syscfg/html/ConfigDoc.html#"
                   + fullName + "_$name \"" + baseName
                   + " Name reference documentation\")",

        documentation: "\n\n"
                   + "The SysConfig tooling ensures that _all_ names defined"
                   + " in a configuration are unique.  When instances are"
                   + " first created, SysConfig gives them a default name,"
                   + " `" + prefix + "`, that's made unique by appending a"
                   + " numeric id.  If you provide a name, it's checked"
                   + " against all other instance names in the configuration"
                   + " and, if another instance has the same name, an error is"
                   + " triggered. The additional 'const' declaration is assumed"
                   + " to also be globally unique."
                   + "\n\n"
                   + "Note: since not all names are added to ti_drivers_config.h,"
                   + " it's possible that some names will not be allowed even"
                   + " though they do not actually collide in the generated"
                   + " files."
    };

    return ([nameCfg].concat(config));
}


/*
 *  ======== autoForceModules ========
 *  Returns an implementation of a module's modules method that just
 *  forces the addition of the specified modules
 *
 *  @param kwargs An array of module name strings.
 *
 *  @return An array with module instance objects
 *
 *  Example:
 *     modules: Common.autoForceModules(["Board", "DMA"])
 */
function autoForceModules(kwargs)
{
    return (function(){

        let modArray = [];

        if (kwargs == undefined || kwargs == null || !Array.isArray(kwargs)) {
            console.log("Common.js:autoForceModules('kwargs'): 'kwargs' invalid!");
            return (modArray);
        }

        for (let args = kwargs.length - 1; args >= 0; args--) {
            let modPath = kwargs[args];
            if (modPath.indexOf('/') == -1) {
                modPath = "/ti/drivers/" + modPath;
            }
            modArray.push({
                name      : modPath.substring(modPath.lastIndexOf('/') + 1),
                moduleName: modPath,
                hidden    : true
            });
        }

        return modArray;
    });
}

/*
 *  ======== genBoardHeader ========
 *  Common Board.h.xdt function to generate standard module header
 *  including instance externs.
 *
 *  instances = array of module instances
 *  mod       = module object
 */
function genBoardHeader(instances, mod, includeBanner=true)
{
    let padding = Array(80).join(' ');
    let maxLineLength = 30;
    let line;
    let lines = [];
    let banner = [];
    let pinResources = [];

    banner.push("/*");
    banner.push(" *  ======== " + mod.displayName + " ========");
    banner.push(" */");
    banner.push("");

    /* Construct each line and determine max line length */
    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];

        /* Does the module have a '_getPinResources()' method */
        if (mod._getPinResources) {

            /* Construct pin resources string to be used as a comment */
            pinResources[i] = mod._getPinResources(inst);

            /* Check if anything was returned */
            if (pinResources[i] != null && pinResources[i] != undefined) {

                /* Construct a comment string */
                line = genResourceComment(pinResources[i]);
                lines.push(line);
            }
        }

        line = "extern const uint_least8_t ";
        lines.push(line);

        /* Is this line length greater than the previous max */
        if (line.length > maxLineLength) {
            maxLineLength = line.length;
        }

        line = "#define " + inst.$name ;
        lines.push(line);

        /* Is this line length greater than the previous max */
        if (line.length > maxLineLength) {
            maxLineLength = line.length;
        }
    }

    /* Add configuration count define */
    let displayName = instances[0].$module.displayName;
    line = "#define CONFIG_TI_DRIVERS_" + displayName.toUpperCase() + "_COUNT";
    lines.push(line);

    /* Is this line length greater than the previous max */
    if (line.length > maxLineLength) {
        maxLineLength = line.length;
    }

    /*
     * No comment was included in this original implementation. Not sure
     * what wizardry is going on here.
     */
    maxLineLength = ((maxLineLength + 3) & 0xfffc) + 4;

    /*
     * Modulate lines based on max line length and append instance number.
     * Occurs for all lines except the last one (config count define) as
     * it is formatted differently.
     */
    let instanceNum = 0;
    let lastLine = lines.length - 1;
    for (let i = 0; i < lastLine; i++) {
        let inst = instances[instanceNum];

        /* If this instance has a comment, assume 1 comment per instance */
        if (pinResources[instanceNum] && pinResources[instanceNum] != null) {
            i++;
        }

        /* Pad and add symbolic name. ie `extern const uint8_t CONFIG_INDEX` */
        lines[i] += padding.substring(0, maxLineLength - lines[i].length);
        lines[i++] += getSymbolicName(inst) + ";";

        /* Pad and add instance number to config define. ie `#define CONFIG_ADC_0 0` */
        lines[i] += padding.substring(0, maxLineLength - lines[i].length);
        lines[i] += instanceNum++;
    }

    /* Pad and add the number of instances to the config count define */
    lines[lastLine] += padding.substring(0, maxLineLength - lines[lastLine].length);
    lines[lastLine] += instanceNum;

    if (includeBanner === true) {
        lines = banner.concat(lines);
    }

    return ((lines).join("\n"));
}

/*
 *  ======== genBoardDeclarations ========
 *  Common Board.c.xdt function to generate instance declarations
 *
 *  instances = array of module instances
 */
function genBoardDeclarations(instances)
{
    let lines = [];

    /* Construct each line */
    for (let i = 0; i < instances.length; i++) {
        let inst = instances[i];

        let line = "const uint_least8_t " + getSymbolicName(inst) + " = "
            + inst.$name + ";";
        lines.push(line);
    }

    return ((lines).join("\n"));
}

/*
 *  ======== genResourceComment ========
 * Construct a comment string from (potentially) multi-line text
 *
 * Assumes 'text' is a string containing one or more
 * lines delimited by new lines (\n)
 */
function genResourceComment(text)
{
    /* split into separate lines and trim trailing white space */
    let lines = text.split(/[ \t]*\n/);

    /* if it's a multi-line comment ... */
    if (lines.length > 1) {

        /* Prevent trailing space after digraph */
        lines[0] = "/*" + lines[0];

        for (let i = 1; i < lines.length; i++) {
            /* prefix intermediate lines with asterisks and spaces */
            lines[i] = " *  " + lines[i];
        }
        lines.push(""); /* add new line for comment end */
    }
    else {
        /* If single line comment, add a space after initial digraph */
        lines[0] = "/* " + lines[0];
    }

    /* end the comment */
    lines[lines.length - 1] += " */";

    /* return a single string with embedded \n's */
    return (lines.join("\n"));
}

/*
 *  ======== findSignalTypes ========
 *  Function to recursively parse a hardware component for a signal type.
 *
 *  hardware    - an object representing a hardware component
 *  signalTypes - an array of signal type strings
 *
 * Example:
 *    findSignalTypes(hardware, ["SPI_SS", "DOUT", "I2S_SCL"])
 *
 *  Returns true if any signal in 'signalTypes' is found in the hardware
 *  component. This function returns as soon as any signal in 'signalTypes' is
 *  found.
 */
function findSignalTypes(hardware, signalTypes)
{
    /* 'signalTypes' should be an array of signal names in a string */
    if (signalTypes == undefined || signalTypes == null || !Array.isArray(signalTypes)) {
        console.log("Common.js:findSignalTypes(hardware, signalTypes):"
            + "'signalTypes' invalid!");
        return (false);
    }

    /* Ensure hardware is not null or undefined */
    if (hardware == undefined || hardware == null) {
        console.log("Common.js:findSignalTypes(hardware, signalTypes):"
            + "'hardware' undefined!");
        return (false);
    }

    /* Evaluate if this hardware component has any signals */
    if (hardware.signals) {
        for (let sig in hardware.signals) {
            let signal = hardware.signals[sig];
            for (let i in signal.type) {
                let type = signal.type[i];
                if (signalTypes.includes(type)) {
                    return (true);
                }
            }
        }
    }

    /* Evaluate if this hardware component has any subcomponents */
    if (hardware.subComponents || hardware.components) {
        let result;

        /* Ensure we don't pass an empty object */
        if (hardware.subComponents != undefined && hardware.subComponents != null) {

            /* Iterate over all subComponents of this hardware component */
            for (let components in hardware.subComponents) {

                /* Recursively pass each component */
                result = findSignalTypes(hardware.subComponents[components], signalTypes);
                if (result == true) {
                    return (result);
                }
            }
        }

        /* Repeat steps above for "components" instead of "subComponents" */
        if (hardware.components != undefined && hardware.components != null) {
            for (let components in hardware.components) {
                result = findSignalTypes(hardware.components[components], signalTypes);
                if (result == true) {
                    return (result);
                }
            }
        }
    }

    return (false);
}
