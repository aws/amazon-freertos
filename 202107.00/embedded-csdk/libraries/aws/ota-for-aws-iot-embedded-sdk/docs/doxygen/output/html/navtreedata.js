/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "AWS IoT Over-the-air Update", "index.html", [
    [ "OTA Dependencies", "index.html#ota_dependencies", null ],
    [ "Memory Requirements", "index.html#ota_memory_requirements", null ],
    [ "Design", "ota_design.html", [
      [ "OTA Library Configurations", "ota_design.html#ota_design_library_configs", null ],
      [ "OTA Library", "ota_design.html#ota_design_library", [
        [ "OTA Agent API’s", "ota_design.html#ota_design_agent_api", null ],
        [ "OTA Platform Abstraction Layer", "ota_design.html#ota_design_pal", null ],
        [ "OTA OS Functional Abstraction", "ota_design.html#ota_design_os", null ],
        [ "OTA MQTT interface", "ota_design.html#ota_design_mqtt", null ],
        [ "OTA HTTP interface", "ota_design.html#ota_design_http", null ],
        [ "OTA Memory Allocation", "ota_design.html#ota_design_memory", null ],
        [ "OTA Agent Task", "ota_design.html#ota_design_agent_task", null ]
      ] ],
      [ "Job Parser", "ota_design.html#ota_design_job_parser", null ],
      [ "OTA Control and Data Protocols", "ota_design.html#ota_design_control_data_protocols", null ]
    ] ],
    [ "Functions", "ota_functions.html", "ota_functions" ],
    [ "Configurations", "ota_config.html", [
      [ "OTA_DO_NOT_USE_CUSTOM_CONFIG", "ota_config.html#OTA_DO_NOT_USE_CUSTOM_CONFIG", null ],
      [ "otaconfigSTACK_SIZE", "ota_config.html#otaconfigSTACK_SIZE", null ],
      [ "otaconfigAGENT_PRIORITY", "ota_config.html#otaconfigAGENT_PRIORITY", null ],
      [ "otaconfigLOG2_FILE_BLOCK_SIZE", "ota_config.html#otaconfigLOG2_FILE_BLOCK_SIZE", null ],
      [ "otaconfigMAX_NUM_BLOCKS_REQUEST", "ota_config.html#otaconfigMAX_NUM_BLOCKS_REQUEST", null ],
      [ "otaconfigSELF_TEST_RESPONSE_WAIT_MS", "ota_config.html#otaconfigSELF_TEST_RESPONSE_WAIT_MS", null ],
      [ "otaconfigFILE_REQUEST_WAIT_MS", "ota_config.html#otaconfigFILE_REQUEST_WAIT_MS", null ],
      [ "otaconfigMAX_THINGNAME_LEN", "ota_config.html#otaconfigMAX_THINGNAME_LEN", null ],
      [ "otaconfigMAX_NUM_REQUEST_MOMENTUM", "ota_config.html#otaconfigMAX_NUM_REQUEST_MOMENTUM", null ],
      [ "otaconfigMAX_NUM_OTA_DATA_BUFFERS", "ota_config.html#otaconfigMAX_NUM_OTA_DATA_BUFFERS", null ],
      [ "otaconfigOTA_UPDATE_STATUS_FREQUENCY", "ota_config.html#otaconfigOTA_UPDATE_STATUS_FREQUENCY", null ],
      [ "otaconfigAllowDowngrade", "ota_config.html#otaconfigAllowDowngrade", null ],
      [ "configENABLED_CONTROL_PROTOCOL", "ota_config.html#configENABLED_CONTROL_PROTOCOL", null ],
      [ "configENABLED_DATA_PROTOCOLS", "ota_config.html#configENABLED_DATA_PROTOCOLS", null ],
      [ "configOTA_PRIMARY_DATA_PROTOCOL", "ota_config.html#configOTA_PRIMARY_DATA_PROTOCOL", null ],
      [ "LogError", "ota_config.html#ota_logerror", null ],
      [ "LogWarn", "ota_config.html#ota_logwarn", null ],
      [ "LogInfo", "ota_config.html#ota_loginfo", null ],
      [ "LogDebug", "ota_config.html#ota_logdebug", null ]
    ] ],
    [ "Porting Guide", "ota_porting.html", [
      [ "Configuration Macros", "ota_porting.html#ota_porting_config", null ],
      [ "OTA PAL requirements:", "ota_porting.html#ota_porting_pal", null ],
      [ "OTA OS Functional Interface requirements:", "ota_porting.html#ota_porting_os", null ],
      [ "OTA MQTT requirements:", "ota_porting.html#ota_porting_mqtt", null ],
      [ "OTA HTTP requirements:", "ota_porting.html#ota_porting_http", null ]
    ] ],
    [ "OTA HTTP Interface", "ota_http_interface.html", [
      [ "OTA HTTP Overview", "ota_http_interface.html#ota_http_interface_overview", null ]
    ] ],
    [ "OTA MQTT Interface", "ota_mqtt_interface.html", [
      [ "OTA MQTT Overview", "ota_mqtt_interface.html#ota_mqtt_interface_overview", null ]
    ] ],
    [ "OTA OS Functional Interface", "ota_os_functional_interface.html", [
      [ "OTA OS Functional Interface", "ota_os_functional_interface.html#ota_os_functional_interface_overview", null ]
    ] ],
    [ "OTA PAL Interface", "ota_pal_interface.html", [
      [ "OTA PAL Overview", "ota_pal_interface.html#ota_pal_interface_overview", null ]
    ] ],
    [ "Constants", "ota_constants.html", [
      [ "OTA Error Code Helper constants", "ota_constants.html#ota_constants_err_code_helpers", null ]
    ] ],
    [ "Data types and Constants", "modules.html", "modules" ],
    [ "Files", "files.html", "files" ]
  ] ]
];

var NAVTREEINDEX =
[
"files.html",
"ota_8h.html#ab5997df93a33e5f1260e7a16655a5cca",
"ota__private_8h.html#a9d3eb1ee8e8a129c990fe9effd93f91dafbb2d9ebcabfdb39b1dbe1d2976d82f6"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';