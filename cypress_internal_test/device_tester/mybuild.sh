#!/bin/bash
set -ue

AFR_ROOT_DIR=$1
RUN_TESTS=$2
BOARD=$3
COMPILER=$4
OTA=$5

BUILD_DIR=$AFR_ROOT_DIR/build

source $AFR_ROOT_DIR/cypress_internal_test/setenv.sh
run_setenv

[ -d $BUILD_DIR ] && rm -r $BUILD_DIR
mkdir -p $BUILD_DIR

support_args=""
if [ $OTA = "OTA_YES" ]; then
    support_args="-DOTA_SUPPORT=1"
fi

$CMAKE_DIR/cmake -G Ninja -DVENDOR=cypress -DBOARD=$BOARD -DCOMPILER=$COMPILER -DAFR_ENABLE_TESTS=$RUN_TESTS -S $AFR_ROOT_DIR -B $BUILD_DIR -DAFR_TOOLCHAIN_PATH=$GCC_DIR -DBLE_SUPPORTED=1 $support_args
$NINJA_DIR/ninja -C $BUILD_DIR
