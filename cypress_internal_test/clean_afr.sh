#!/bin/bash

# This script removes all internal test files and 3rd party vendor files that we don't want to include
# in our vendor qualified release package.

set -$-ue${DEBUG+xv}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rm -rf "${DIR}/../.gitlab-ci.yml"
rm -rf "${DIR}/../projects/espressif"
rm -rf "${DIR}/../projects/marvell"
rm -rf "${DIR}/../projects/mediatek"
rm -rf "${DIR}/../projects/microchip"
rm -rf "${DIR}/../projects/nordic"
rm -rf "${DIR}/../projects/nuvoton"
rm -rf "${DIR}/../projects/nxp"
rm -rf "${DIR}/../projects/pc"
rm -rf "${DIR}/../projects/renesas"
rm -rf "${DIR}/../projects/st"
rm -rf "${DIR}/../projects/ti"
rm -rf "${DIR}/../projects/xilinx"
rm -rf "${DIR}/../vendors/espressif"
rm -rf "${DIR}/../vendors/marvell"
rm -rf "${DIR}/../vendors/mediatek"
rm -rf "${DIR}/../vendors/microchip"
rm -rf "${DIR}/../vendors/nordic"
rm -rf "${DIR}/../vendors/nuvoton"
rm -rf "${DIR}/../vendors/nxp"
rm -rf "${DIR}/../vendors/pc"
rm -rf "${DIR}/../vendors/renesas"
rm -rf "${DIR}/../vendors/st"
rm -rf "${DIR}/../vendors/ti"
rm -rf "${DIR}/../vendors/xilinx"
rm -rf "${DIR}/../cypress_internal_test"
