#!/bin/bash

board="$1"
pushd "$(dirname "${BASH_SOURCE[0]}")/.."

source setenv.sh
run_setenv
source downloadtools.sh
download_tools

source "cmake-test/test.sh"

pushd "cmake-test/file_type_test"

BUILD_DIR="build"
LOG_FILE="$BUILD_DIR/rslt.txt"
rm -rf $BUILD_DIR
mkdir $BUILD_DIR

cmake -S . -B build -G "Unix Makefiles" 2>$LOG_FILE

set +e

check_not_exist "a.lib" $LOG_FILE
check_exist "a.c" $LOG_FILE
check_exist "a.h" $LOG_FILE

popd
popd
