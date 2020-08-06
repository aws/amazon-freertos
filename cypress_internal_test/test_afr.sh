#!/bin/bash

# Parameters:
# For running for a specific compiler
#  - bash afr/test_afr.sh arm-gcc CY8CKIT_062_WIFI_BT
# For running for all compilers
#  - bash afr/test_afr.sh CY8CKIT_062_WIFI_BT

set -$-ue${DEBUG+xv}

board="$1"
toolchain="$2"
enable_p6make=1
enable_cmake=1
shift 2
while [ $# -gt 0 ]; do
  case "$1" in
    --no-p6make)
      enable_p6make=0
      ;;
    --no-cmake)
      enable_cmake=0
      ;;
    *)
      echo "$0: Unrecognized argument: $1"
      exit 1
      ;;
  esac
  shift
done

pushd "$(dirname "${BASH_SOURCE[0]}")"

source setenv.sh
run_setenv

source downloadtools.sh
download_tools

popd

# git set timestamp of files on clone
# This update the timestamps files so that generated_source will not get re-generated.
find  -type f  -name "*.timestamp" -exec touch {} +

function afr_compile_psoc6()
{
    echo "afr compile $@"
    local build_dir="build/cmake"
    rm -rf $build_dir
    echo "cmake -DVENDOR=cypress -S . -DCOMPILER="$1" ""-DBOARD="$2" -B $build_dir -G "Ninja""
    # For now, always assume that BLE is supported. This might not be true for all boards.
    if [ "$1" = arm-armclang ]; then
        BLE_SUPPORTED=0 # BLE SDK does not currently compile with armclang
    else
        # Temporarily disabled because BLE lib has not been updated for 201912.00
        BLE_SUPPORTED=0
        #BLE_SUPPORTED=1
    fi
    cmake -DVENDOR=cypress -S . -DCOMPILER="$1" -DBOARD="$2" -DBLE_SUPPORTED=$BLE_SUPPORTED -B "$build_dir" -G "Ninja"
    ninja -C $build_dir
}

function afr_compile_psoc6make()
{
    local toolchain="$1"
    local board="$2"
    local build_dir="$PWD/build/make"
    
    do_not_convert_list=("CY8CKIT_064S0S2_4343W")

    if [[ " ${do_not_convert_list[@]} " =~ " ${board} " ]]; then
        local makeboard=${board}
    else
        local makeboard="${board//_/-}"
    fi

    echo "afr compile psoc6make $makeboard"
    
    make -C projects/cypress/$board/mtb/aws_demos clean -j4 TOOLCHAIN=$toolchain TARGET=$makeboard \
        CY_TOOLS_PATHS=$PWD/cypress_internal_test/tools CY_BUILD_LOCATION=$build_dir CY_RECIPE_POSTBUILD=
    make -C projects/cypress/$board/mtb/aws_demos app -j4 TOOLCHAIN=$toolchain TARGET=$makeboard \
        CY_TOOLS_PATHS=$PWD/cypress_internal_test/tools CY_BUILD_LOCATION=$build_dir CY_RECIPE_POSTBUILD=
}

export CY_COMPILER_GCC_ARM_DIR="$GCC_DIR"
export CY_COMPILER_IAR_DIR="\"${IAR_DIR:-}\""
export CY_COMPILER_ARM_DIR="${ARMCC_DIR:-}"

case "$toolchain" in
  GCC_ARM)
    export AFR_TOOLCHAIN_PATH="$GCC_DIR/bin"
    cmake_toolchain=arm-gcc
  ;;
  IAR)
    export AFR_TOOLCHAIN_PATH="$IAR_DIR/bin"
    cmake_toolchain=arm-iar
  ;;
  ARM|ARMC6)
    export AFR_TOOLCHAIN_PATH="$ARMCC_DIR/bin"
    cmake_toolchain=arm-armclang
  ;;
  *)
    echo "Unsupported toolchain: $toolchain"
    exit 1
  ;;
esac

[ $enable_p6make -ne 0 ] && afr_compile_psoc6make $toolchain $board
[ $enable_cmake -ne 0 ] && afr_compile_psoc6 $cmake_toolchain $board

echo "[INFO] $0: SUCCESS"
