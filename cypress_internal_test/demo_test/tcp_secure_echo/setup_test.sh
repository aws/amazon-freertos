#!/bin/bash

function setup_test()
{
    local board=$1
    
    cp -f $AFR_DIR/cypress_internal_test/demo_test/tcp_secure_echo/aws_tcp_echo_client_single_task.c $AFR_DIR/demos/tcp/aws_tcp_echo_client_single_task.c
    
    sed -i -r "s/#define CONFIG_MQTT_DEMO_ENABLED/#define CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED/" $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_demo_config.h
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_demo_config.h CONFIG_MQTT_DEMO_ENABLED 1
    
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR0 10
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR1 198
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR2 85
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configECHO_SERVER_ADDR3 78
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/FreeRTOSConfig.h configTCP_ECHO_CLIENT_PORT 9000
    
    ACCEPT_MESSAGE="Demo completed successfully."
    #ACCEPT_MESSAGE="Received correct string from echo server."
    FAIL_MESSAGE="Echo demo failed to connect to echo server."
}


