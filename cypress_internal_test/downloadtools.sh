#!/bin/sh
download_tools()
{
    rm -rf tools
    mkdir tools
    pushd tools
    curl -O http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/bsp_csp/develop/13016/deploy/tools-make-1.1.0.13016.zip
    unzip -o tools-make-1.1.0.13016.zip
    popd
}