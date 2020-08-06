#!/bin/bash

function setup_test()
{
    local board=$1
    
    sed -i -r "s/#define CONFIG_MQTT_DEMO_ENABLED/#define CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED/" $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_demo_config.h
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_demo_config.h CONFIG_MQTT_DEMO_ENABLED 1
    
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR0 10
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR1 14
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR2 47
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR3 68
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configTCP_ECHO_CLIENT_PORT 9001
    
    ACCEPT_MESSAGE="Received correct string from echo server."
    FAIL_MESSAGE="Echo demo failed to connect to echo server."
}