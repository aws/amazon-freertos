/*
 * Copyright (c) 2017-2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== SimpleLinkWifi.syscfg.js ========
 */
 
"use strict";

let Common     = system.getScript("/ti/drivers/Common.js");
let logError   = Common.logError;
 

/*
 *  ======== getLibs ========
 */
function getLibs(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");
    let rtos = system.modules["/ti/drivers/RTOS"];
    var libs = [];
    var deps = [];
    
    if(rtos != null)
    {
        if(rtos.$static.name == "NoRTOS")
        {
            libs.push("ti/drivers/net/wifi/" + GenLibs.getToolchainDir() + "/nortos/simplelink.a");  
            deps.push("/ti/drivers");
        }
        else
        {
            libs.push("ti/drivers/net/wifi/slnetif/" + GenLibs.getToolchainDir() + "/Release/slnetifwifi.a");
            libs.push("ti/drivers/net/wifi/" + GenLibs.getToolchainDir() + "/rtos/simplelink.a");            
        }
    }
    else
    {
        libs.push("ti/drivers/net/wifi/slnetif/" + GenLibs.getToolchainDir() + "/Release/slnetifwifi.a");
        libs.push("ti/drivers/net/wifi/" + GenLibs.getToolchainDir() + "/rtos/simplelink.a");
    }
    
    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/drivers/net/wifi/SimpleLinkWifi",
        deps: deps, 
        libs: libs       
    };  
    
    return (linkOpts);
}  
 
let config = [
    {
        name        : "startRole",
        displayName : "Start Role",
        description : "Specifies NWP start role",
        onChange    : onStartRoleChange,
        default     : 'STA',
        options     : [
            { name : "AP" , displayName: "Access Point" },
            //{ name : "P2P", displayName: "Peer to Peer" },
            { name : "STA", displayName: "Station" },
            { name : "AS_CONFIGURED" , displayName: "AS CONFIGURED" },
        ]
    },

    {
        name        : "connectionPolicy",
        displayName : "Connection Policy",
        description : "Configure Connection Policy",
        onChange    : onConnectionPolicyChange,
        default     : ['(1,0,0,0)'],
        minSelections: 0,
        options     : [
            { name : "(1,0,0,0)" , displayName: "Auto Connect" },
            { name : "(0,1,0,0)" , displayName: "Fast Connect" },
            // { name : "(0,0,1,0)" , displayName: "P2P" },
            { name : "(0,0,0,1)" , displayName: "Auto Provisioning" },
            { name : "AS_CONFIGURED" , displayName: "AS CONFIGURED" },   
        ]
    },
    
    {
        name        : "powerManagementPolicy",
        displayName : "Power Management Policy",
        description : "Configure Power Management Policy",
        onChange    : onPowerManagementPolicyChange,
        default     : 'NORMAL',
        options     : [
            { name : "NORMAL" , displayName: "Normal" },
            { name : "LOW_LATENCY" , displayName: "Low latency" },
            { name : "LOW_POWER" , displayName: "Low power" },
            { name : "ALWAYS_ON" , displayName: "Always on" },
            { name : "LONG_SLEEP_INTERVAL" , displayName: "Long Sleep Interval" },     
            { name : "AS_CONFIGURED" , displayName: "AS CONFIGURED" },        
        ]
    },

    {
        name        : "maxSleepTime",
        displayName : "Max Sleep Time (Milliseconds)",
        description : "Configure Max Sleep Time in milliseconds for Long Sleep Interval "
                + "power management policy. Min - 100ms, Max - 2000ms",
        hidden      : true,
        default     : 300,
    },

    {
        name        : "scanPolicy",
        displayName : "Scan Policy",
        description : "Configure Scan Policy ",
        onChange    : onScanPolicyChange,
        default     : '(0,0)',
        options     : [
            { name : "(0,0)" , displayName: "Disable" },
            { name : "(1,1)" , displayName: "With hidden SSID" },
            { name : "(1,0)" , displayName: "No hidden SSID" },
            { name : "AS_CONFIGURED" , displayName: "AS CONFIGURED" },        
        ]
    },

    {
        name        : "scanIntervalInSeconds",
        displayName : "Scan Interval (Seconds)",
        description : "Configure Scan interval in seconds for scan policy "
                +" Min - 10sec for AP/ 1sec for STA",
        hidden      : true,
        default     : 600,
    },

    {
        name        : "ipv4Config",
        displayName : "IPv4 Config",
        onChange    : onIPv4ConfigChange,
        default     : 'DHCP',
        options     : [
            { name : "DHCP" , displayName: "DHCP" },
            { name : "STATIC", displayName: "Static" },   
            { name : "AS_CONFIGURED" , displayName: "AS CONFIGURED" },   
    ]
    },

    {
        name        : "ipv4Address",
        displayName : "IPv4 Address",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "10.1.1.201",
    },

    {
        name        : "subnetMask",
        displayName : "Subnet Mask",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "255.255.255.0",
    },

    {
        name        : "defaultGateway",
        displayName : "Default Gateway",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "10.1.1.1",
    },

    {
        name        : "dnsServer",
        displayName : "DNS Server",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "8.16.32.64",
    },

    {
        name        : "dhcpServer",
        displayName : "DHCP Server",
        onChange    : onDHCPServerChange,
        readOnly    : false,
        default     : false
    },

    {
        name        : "startAddress",
        displayName : "Start Address",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "10.1.1.2",
    },

    {
        name        : "lastAddress",
        displayName : "Last Address",
        placeholder : "000.000.000.000",
        textType    : "ipv4_address",
        hidden      : true,
        default     : "10.1.1.200",
    },

    {
        name        : "leaseTime",
        displayName : "Lease Time",
        description : "Configure Lease Time for DHCP Server in seconds",
        hidden      : true,
        default     : 86400
    },

    {
        name        : "provisioningStop",
        displayName : "Provisioning Stop",
        readOnly    : false,
        default     : true
    },

    {
        name        : "deleteAllProfile",
        displayName : "Delete All Profiles",
        default     : false
    },
];


   


/*
 *  ======== base ========
 *  Define the base SimpleLink Wifi properties and methods
 */
let base = {
    name: "simpleLinkWifi",
    displayName: "General",
    description: "SimpleLink Wifi Host Driver configuration",
    longDescription: 'The SimpleLink Wifi module allows '
        + 'to configure the device to a pre-configured state. \n' 
        + 'The configuration of the SimpleLink Wifi is usually persistent, ' 
        + 'and can be reconfigured at runtime. \n' 
        + 'Reconfiguration should be preformed only when needed since '
        + 'the process involves flash writes and might impact system lifetime (flash write endurance) and power consumption. ',
    documentation: "/wifi_host_driver_api/index.html",
    maxInstances: 1,
    moduleStatic: {
        config,
        validate        : validate
    },
    templates : {
        "/ti/utils/build/GenLibs.cmd.xdt"   : {modName: "/ti/drivers/net/wifi/SimpleLinkWifi", getLibs: getLibs},
        "/ti/drivers/net/wifi/Config.c.xdt" : "/ti/drivers/net/wifi/Config.c.xdt"
    }
};    

/*
 *  ======== onStartRoleChange ========
 *  onChange callback function for the Start Role config
 *
 *  param inst  - Start Role instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onStartRoleChange(inst, ui)
{
    if (inst.startRole == "AP" ) {
        inst.ipv4Config = "STATIC";
        ui.ipv4Address.hidden = false;
        ui.subnetMask.hidden = false;
        ui.defaultGateway.hidden = false;
        ui.dnsServer.hidden = false;
    }

}

/*
 *  ======== onScanPolicyChange ========
 *  onChange callback function for the scanPolicy config
 *
 *  param inst  - Scan Policy instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onScanPolicyChange(inst, ui)
{
    if (inst.scanPolicy == '(0,0)' ||  inst.scanPolicy == "AS_CONFIGURED") {
        ui.scanIntervalInSeconds.hidden = true;
    }
    else {
        ui.scanIntervalInSeconds.hidden = false;
    }
}

/*
 *  ======== onIPv4ConfigChange ========
 *  onChange callback function for the IPv4Config config
 *
 *  param inst  - IPv4 Config instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onIPv4ConfigChange(inst, ui)
{
    if (inst.ipv4Config == "STATIC") {
        ui.ipv4Address.hidden = false;
        ui.subnetMask.hidden = false;
        ui.defaultGateway.hidden = false;
        ui.dnsServer.hidden = false;
    }
    else {
        ui.ipv4Address.hidden = true;
        ui.subnetMask.hidden = true;
        ui.defaultGateway.hidden = true;
        ui.dnsServer.hidden = true;
    }
}

/*
 *  ======== onConnectionPolicyChange ========
 *  onChange callback function for the ConnectionPolicy config
 *
 *  param inst  - Connection Policy Config instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onConnectionPolicyChange(inst, ui)
{
   let i;
   ui.provisioningStop.readOnly = false;
   for (i = 0; i < inst.connectionPolicy.length; i++) {
        if (inst.connectionPolicy[i] == "(0,0,0,1)") {
            inst.provisioningStop = false;
            ui.provisioningStop.readOnly = true;
            }
    }
}

/*
 *  ======== onPowerManagementPolicyChange ========
 *  onChange callback function for the powerManagementPolicy config
 *
 *  param inst  - Power Management instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onPowerManagementPolicyChange(inst, ui)
{
    if (inst.powerManagementPolicy != "LONG_SLEEP_INTERVAL") {
        ui.maxSleepTime.hidden = true;
    }
    else {
        ui.maxSleepTime.hidden = false;
    }
}

/*
 *  ======== onDHCPServerChange ========
 *  onChange callback function for the DHCPServer config
 *
 *  param inst  - DHCP Server instance containing the config that changed
 *  param ui    - The User Interface object
 *
 */
function onDHCPServerChange(inst, ui)
{
    if (inst.dhcpServer == true) {
        ui.startAddress.hidden = false;
        ui.lastAddress.hidden = false;
        ui.leaseTime.hidden = false;
    }
    else {
        ui.startAddress.hidden = true;
        ui.lastAddress.hidden = true;
        ui.leaseTime.hidden = true; 
    }
}

/*
 *  ======== SplitIP ========
 *  SplitIP function get ip with "." and return the ip with ","
 *
 *  param ip  - ip address instance
 *
 */
function SplitIP(ip)
{
      let dots = ip.split('.');
      return dots;
}

/*
 *  ======== SL_IPV4_VAL ========
 *  SL_IPV4_VAL function get ip with "," and return the ip in Hex
 *
 *  param add_3  - first field of the ip
 *  param add_2  - second field of the ip
 *  param add_1  - third field of the ip
 *  param add_0  - fourth field of the ip
 *
 */
function SL_IPV4_VAL(add_3,add_2,add_1,add_0)
{
    return ((parseInt(add_3) << 24 & 0xFF000000) | ((parseInt(add_2) << 16) & 0xFF0000) | ((parseInt(add_1) << 8) & 0xFF00) | (parseInt(add_0) & 0xFF));
}

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  param inst - Simplelink Wifi instance to be validated
 *  param vo   - Issue reporting object
 *
 */

function validate(inst, vo)
{
    let split, mask, ip, gateway;
    let check, check_gateway;
    let start, last, check_start, check_last;
	
	
    if (inst.connectionPolicy.length > 1) {
        if (inst.connectionPolicy[inst.connectionPolicy.length-1] == "AS_CONFIGURED") {
                logError(vo, inst, "connectionPolicy",
                    "AS CONFIGURED must be selected without other configurations");
            }
        }
    if (inst.powerManagementPolicy == "LONG_SLEEP_INTERVAL") {
        if (inst.maxSleepTime < 100 || inst.maxSleepTime > 2000) {
            logError(vo, inst, "maxSleepTime",
                "Value range is between 100ms-2000ms");
            }
    }
    if (inst.startRole == "AP"){
        if (inst.ipv4Config == "DHCP") {
        logError(vo, inst, "ipv4Config",
                    "Access Point Mode and DHCP IPv4 configuration can't be selected together");
            }
        if (inst.scanIntervalInSeconds < 10) {
            logError(vo, inst, "scanIntervalInSeconds",
                "Must be greater than 10 seconds");
            }
    }
    else if (inst.startRole == "STA") {
        if (inst.scanIntervalInSeconds < 1) {
            logError(vo, inst, "scanIntervalInSeconds",
                "Must be greater than 1 second");
            }
    }
    if (inst.leaseTime < 60){
            logError(vo, inst, "leaseTime",
                "Must be greater than 60 seconds");
        }
    if (inst.leaseTime > 0xFFFFFFFF){
            logError(vo, inst, "leaseTime",
                "Value is out of range");
        }
    if (inst.leaseTime % 1 != 0 ){
            logError(vo, inst, "leaseTime",
                "Value must be a whole number");
        }
    split = SplitIP(inst.subnetMask);
    mask = parseInt(SL_IPV4_VAL(split[0],split[1],split[2],split[3]));

    split = SplitIP(inst.ipv4Address);
    ip = parseInt(SL_IPV4_VAL(split[0],split[1],split[2],split[3]));

    split = SplitIP(inst.defaultGateway);
    gateway = parseInt(SL_IPV4_VAL(split[0],split[1],split[2],split[3]));

    check = parseInt(mask) & parseInt(ip);
    check_gateway = parseInt(mask) & parseInt(gateway);

    if (parseInt(check) != parseInt(check_gateway)){
        logError(vo, inst, "defaultGateway", "IP address is out of range");
        }

    if (inst.dhcpServer == true) {
        if (inst.startRole == "AP") {
            split = SplitIP(inst.startAddress);
            start = parseInt(SL_IPV4_VAL(split[0],split[1],split[2],split[3]));

            split = SplitIP(inst.lastAddress);
            last = parseInt(SL_IPV4_VAL(split[0],split[1],split[2],split[3]));

            check = parseInt(ip);
            check_start = parseInt(start);
            check_last = parseInt(last);

            if ((parseInt(check) >= parseInt(check_start)) && (parseInt(check) <= parseInt(check_last))) {
                logError(vo, inst, "ipv4Address", "IP address is between the DHCP range");
                }
        }
        else {
        logError(vo, inst, "dhcpServer",
                    "Station Mode and DHCP Server can't be selected together");
        }
    }

}

exports = base;