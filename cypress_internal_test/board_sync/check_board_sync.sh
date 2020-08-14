#!/bin/bash

# This script checks to make sure that all the common files are in sync for all the boards.
# Usage:
# bash cypress_internal_test/check_board_sync.sh

source "$(dirname "${BASH_SOURCE[0]}")/common_data.sh"

sync_script_msg="Please use the script \"cypress_internal_test/sync_boards.sh\" by passing it the board that was updated to sync the common files."
retval=0

ref_board=${board_list[0]}

for project_name in ${project_list[@]}; do
    for board in "${board_list[@]}"; do
        if [[ $board != $ref_board ]]; then
            for i in "${!sync_list[@]}"; do
                if [[ $board ==  CY8CKIT_064S0S2_4343W ]]; then
                    exclude=${exclude_list[i]}
                fi
                eval "file_path=${sync_list[i]}"
                ref_path=$boards_dir/$ref_board/$file_path
                path=$boards_dir/$board/$file_path
                if ! diff -r $ref_path $path $exclude; then
                    echo "Contents of $file_path did not match between $board and $ref_board. $sync_script_msg"
                    retval=1
                fi
            done
        fi
    done
done

ref_path=$boards_dir/$ref_board/ports/ota
path=$secure_ota_port_dir
if ! diff -r $ref_path $path; then
    echo "Contents of the OTA port folder did not match between and $ref_board. $sync_script_msg"
    retval=1
fi

exit $retval