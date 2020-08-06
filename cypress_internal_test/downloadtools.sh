#!/bin/sh
download_tools()
{
    rm -rf tools
    mkdir tools
    pushd tools
    curl http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/bsp_csp/develop/Latest/deploy/tools-make.zip >tools-make.zip
    unzip tools-make.zip
    popd
}