#!/bin/bash

# This script is a convience utility to synchronize boards. It applies the changes from the common files of the board you are working on to
# the other boards.
#
# Usage:
# bash cypress_internal_test/sync_boards.sh <board>
# <board> is the board you are currently working on with the latest changes.
# Example:
# bash cypress_internal_test/board_sync/sync_boards.sh CY8CKIT_064S0S2_4343W

source "$(dirname "${BASH_SOURCE[0]}")/common_data.sh"

board="$1"

function sync_boards()
{
    local updated_board=$1
    for project_name in ${project_list[@]}; do
        for stale_board in "${board_list[@]}"; do
            if [[ $stale_board != $updated_board ]]; then
                for item in "${sync_list[@]}"; do
                    eval "file_path=$item"
                    cp -fr $boards_dir/$updated_board/$file_path* $boards_dir/$stale_board/$file_path
                done
            fi
        done
    done
}

function handle_064_differences()
{
    local updated_board=$1
    [[ "$updated_board" == "CY8CKIT_064S0S2_4343W" ]]; secure=$?;

    # When copying
    if [[ "$secure" -eq 0 ]]; then
        for stale_board in "${board_list[@]}"; do
            if [[ "$stale_board" != "$updated_board" ]]; then
                # Remove the PSA port from other boards. It only applies to the secure board
                git clean -fd "$boards_dir/$stale_board/ports/pkcs11/psa/"

                # Sync OTA port from 064 board to other boards
                cp -r $secure_ota_port_dir/* $boards_dir/$stale_board/ports/ota/

                # Checkout FreeRTOSConfig.h in the secure board
                echo "The FreeRTOSConfig.h config file changes on the secure board will not be synced over since this board has Tickless mode disabled temporariliy due to TFM-435."
                git checkout $boards_dir/$stale_board/aws_demos/config_files/FreeRTOSConfig.h
                git checkout $boards_dir/$stale_board/aws_tests/config_files/FreeRTOSConfig.h
            fi
        done
    else
        # Clean pkcs pal implementation, the secure board does not use the pal implementation.
        git clean -fd $boards_dir/CY8CKIT_064S0S2_4343W/ports/pkcs11/iot_pkcs11_pal.c

        # Move OTA port to port_support/ota/ports
        mv $boards_dir/CY8CKIT_064S0S2_4343W/ports/ota/* $secure_ota_port_dir/

        # Checkout FreeRTOSConfig.h in the secure board
        echo "The FreeRTOSConfig.h config file changes on the secure board will not be synced over since this board has Tickless mode disabled temporariliy due to TFM-435."
        git checkout $boards_dir/CY8CKIT_064S0S2_4343W/aws_demos/config_files/FreeRTOSConfig.h
        git checkout $boards_dir/CY8CKIT_064S0S2_4343W/aws_tests/config_files/FreeRTOSConfig.h
    fi
}

sync_boards $board
handle_064_differences $board
