#!/usr/bin/env bash
# This script is intended to execute on any "devops-linux" runner

# Enable strict error checking
set -$-e${DEBUG+xv}

board="$1"

pushd "$(dirname "${BASH_SOURCE[0]}")/.."

source setenv.sh
run_setenv
source downloadtools.sh
download_tools
cd ..

# Set path to the root directory of the git repository
ROOT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ROOT_DIR="$ROOT_PATH"

# Set path to the output directory for generated assets
#OUT_DIR="cypress_internal_test"
OUT_DIR="output"

# rm -rf "$OUT_DIR"
# mkdir -p "$OUT_DIR"
cd "$OUT_DIR" 
echo "OUT_DIR= $OUT_DIR"

#TODO temporary, need a better way to specify compiler.
export AFR_TOOLCHAIN_PATH="$GCC_DIR/bin"
export CY_COMPILER_GCC_ARM_DIR="$GCC_DIR"

echo "pwd: $(pwd)"
echo "cmake -G ... "
cmake -G "Unix Makefiles" -S .. -B . -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=$board -DAPP=projects/cypress/$board/mtb/ota -DAPP_VERSION_MAJOR=1
# clean sign sign_ota.sh
:> sign_ota.sh
chmod 777 sign_ota.sh
echo "make -j4"
make -j4

