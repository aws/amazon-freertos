#!/bin/bash

function setup_test()
{
    local board=$1
    
    replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/iot_pkcs11_config.h pkcs11configVENDOR_DEVICE_CERTIFICATE_SUPPORTED 1

    ACCEPT_MESSAGE="Demo completed successfully."
    FAIL_MESSAGE="Error running demo."
}