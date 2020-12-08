#!/bin/bash 
(set -o igncr) 2>/dev/null && set -o igncr; # this comment is required
set -$-ue${DEBUG+x}

#######################################################################################################################
# This script is designed to process resource files that are necessary for a single component. It converts each
# resource file into a binary object and then stores that into an array in a .c file. The .c file can then be compiled
# with and linked into an application image.
#
# Once all resources files have been converted it will generate a header file that references each of them.
#
# usage:
#	genresources.bash <RECIPE_DIR> <SCRIPTS_DIR> <RESOURCE_FILE> <PROJECT_DIR> <TARGET_DIR> <FILESYSTEM|MEM> [s]
#
#######################################################################################################################

RECIPE_DIR=$1				#eg: ./tools
RESOURCE_FILE=$2		    #eg: ./generated/temp.cyrsc
PROJECT_DIR=$3				#eg: ./project_mainapp
TARGET_DIR=$4				#eg: ./project_resources
RESOURCE_TYPE=$5			#eg: FILESYSTEM or MEM

echo Script: genresources.bash
echo "    1: Recipe Dir     : '"$RECIPE_DIR"'"
echo "    3: Resource Files : '"$RESOURCE_FILE"'"
echo "    4: Project Dir    : '"$PROJECT_DIR"'"
echo "    5: Target Dir     : '"$TARGET_DIR"'"
echo "    6: Resource Type  : '"$RESOURCE_TYPE"'"

#
# File in the target directory
#
RES_FILE="$TARGET_DIR/cy_resources.h"

# array of c source files parsed for declarations to generate resources.h
declare SOURCE_ARRAY=()

#
# Print nice error messages
#
function error() {
    echo "ERROR: $1"
    shift

    while (( $# > 0 )); do
        echo "     : $1:"
        shift
    done

    echo "—ABORTING--"
    exit 1
}

#
# Checks if the value $1 is in the array $element
#
array_contains () {
    local seeking=$1; shift
    local in=0
    for element; do
        if [[ $element == $seeking ]]; then
            in=1
            break
        fi
    done
    echo $in
}

#
# Prepares the resource file for outputing as c-file
#
convert_resource_name() {
    local input=$1
    local result=${input//\//_DIR_} #replace '/' with '_DIR_'
    result=${result//./_} #replace '.' with '_'
    result=${result//-/_} #replace '-' with '_'
    result=${result//resources_DIR/resources} #replace 'resources_DIR' with 'resources'
    echo $result
}

#
# Process the resources listed in the .cyrsc file by converting them to .c and creating
# a list of files for the resource header script
#
processResources() {
    local TEXT_FILTERS=(html htm txt eml js css dat cer pem json xml py key)
    local BINARY_FILTERS=(jpg jpeg png ico gif bin flac wav clm_blob gz mp3 wmfw)
    local TEXT_TO_RES="$RECIPE_DIR/text_to_resource_c.pl"
    local BIN_TO_RES="$RECIPE_DIR/bin_to_resource_c.pl"
    local resourceList=($(<$1))

    # Parse through each element in the .cyrsc file
    for resourceFile in "${resourceList[@]}"; do
        local filename="${resourceFile##*/}"
        local extension="${filename##*.}"

        if [ -f "$resourceFile" ]; then
            local resourceName=$(convert_resource_name "$resourceFile")
            local outputFile="$TARGET_DIR/$(convert_resource_name $filename).c"
            SOURCE_ARRAY+=("$outputFile")
            
            if [ "$resourceFile" -nt "$outputFile" ]; then
                if [ "1" == "$(array_contains $extension "${TEXT_FILTERS[@]}")" ]; then
                    local script=$TEXT_TO_RES
                elif [ "1" == "$(array_contains $extension "${BINARY_FILTERS[@]}")" ]; then
                    local script=$BIN_TO_RES
                else
                    error  "Unsupported resource type $resourceFile with extension $extension"
                fi
                echo "Generating file $outputFile"
                perl "$script" "$RESOURCE_TYPE" "$resourceName" "$resourceFile" > "$outputFile"
            fi
        else
            error  "Listed resource $resourceFile does not exist"
        fi
    done
}

#
# Call the perl script that creates resources.h
#
generateResourceHeader() {
    perl "$RECIPE_DIR/resources_header.pl" ${SOURCE_ARRAY[*]} > "$RES_FILE"
}

#######################################################################################################################

#
# Process all the resources in the cyrsc file
#
processResources $RESOURCE_FILE

#
# Create the resource header
#
generateResourceHeader

