#!/bin/bash

function setup_test()
{
    local board=$1

    sed -i -r "s/#define CONFIG_MQTT_DEMO_ENABLED/#define CONFIG_DEFENDER_DEMO_ENABLED/" $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_demo_config.h

    ACCEPT_MESSAGE="Demo completed successfully."
    FAIL_MESSAGE="Error running demo."
}