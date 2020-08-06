##
set -$-ue${DEBUG+xv}

# Syntax example: bash cypress_internal_test/test_afr_cysecure.sh ubuntu CY8CKIT_064S0S2_4343W GCC_ARM --no-p6make 
# Argument 1 is destination_machine type, can be one of mac, ubuntu or win
# Argument 2 is the board to build
# Argument 3 is the toolchain
# Argument 4 is the makeoption (example: --no-p6make, --no-cmake)
host=$1
board=$2
toolchain=$3
makeopt=$4

restore_nounset=""
test -o nounset && restore_nounset="set -u"

_arm_gnu=""
[[ ${host} = mac ]]     && _arm_gnu=OSX
[[ ${host} = ubuntu ]]  && _arm_gnu=Linux64
[[ ${host} = win ]]     && _arm_gnu=Win32

# install virtualenv
[[ ${host} = mac ]]     && python3 -m venv env
[[ ${host} = ubuntu ]]  && python3 -m venv env
[[ ${host} = win ]]     && python -m venv env

# activate virtualenv
set +u
[[ ${host} = mac ]]     && source env/bin/activate
[[ ${host} = ubuntu ]]  && source env/bin/activate
[[ ${host} = win ]]     && source env/bin/activate
${restore_nounset}

# Install Python modules in the virtualenv
pip3 --no-cache-dir install --upgrade pip

# cysecuretools RC branch
pip3 install --upgrade --force-reinstall git+http://git-ore.aus.cypress.com/repo/cysecuretools.git@P64_2M_ES100_RC2
# pyocd RC branch

# workaround for pyocd installation issue
pip3 install setuptools_scm==3.5.0

pip3 install --upgrade --force-reinstall git+http://git-ore.aus.cypress.com/repo/pyocd.git@P64_2M_ES100_RC2

# Apply patch
PATCH_FILE="cypress_internal_test/enable_demo.patch"
[[ -f ${PATCH_FILE} ]] && git apply ${PATCH_FILE}

echo "source cypress_internal_test/test_afr.sh ${board} ${toolchain} ${makeopt}"
source cypress_internal_test/test_afr.sh ${board} ${toolchain} ${makeopt}

set -x
if [ ${makeopt} = "--no-p6make" ]; then
    pushd build/cmake
    find . \( -name '*.hex' -o -name '*.map' -o -name '*.bin' -o -name '*.elf' \) -print0 | xargs -0 tar cvzf aws_demos.tgz
    popd
fi

# uninstall virtualenv
set +u
[[ ${host} = mac ]]     && deactivate
[[ ${host} = ubuntu ]]  && deactivate
[[ ${host} = win ]]     && deactivate
${restore_nounset}
