#!/usr/bin/env bash
# This script is intended to execute on any "devops-linux" runner

# Enable strict error checking
set -$-e${DEBUG+xv}
BOARD="$1"
host=$2
branch=${CI_COMMIT_REF_NAME}

restore_nounset=""
test -o nounset && restore_nounset="set -u"

_arm_gnu=""
[[ ${host} = mac ]]     && _arm_gnu=OSX
[[ ${host} = ubuntu ]]  && _arm_gnu=Linux64
[[ ${host} = win ]]     && _arm_gnu=Win32

# install virtualenv
[[ ${host} = mac ]]     && python3 -m venv env
[[ ${host} = ubuntu ]]  && python3 -m venv env
[[ ${host} = win ]]     && python3 -m venv env

# activate virtualenv
set +u
[[ ${host} = mac ]]     && source env/bin/activate
[[ ${host} = ubuntu ]]  && source env/bin/activate
[[ ${host} = win ]]     && source env/bin/activate
${restore_nounset}

# Install Python modules in the virtualenv
pip3 --no-cache-dir install --upgrade pip
# cysecuretools RC branch
pip3 install --upgrade --force-reinstall git+http://git-ore.aus.cypress.com/repo/cysecuretools.git@CYSECURETOOLS_3.1.0_CYBOOTLOADER_SWAP_SCRATCH_TC3

# workaround for pyocd installation issue
pip3 install setuptools_scm==3.5.0

# Download DevOps-provided tools and scripts
git clone git@git-ore.aus.cypress.com:devops/devops_scripts.git
git clone git@git-ore.aus.cypress.com:devops/cy-checker.git

pushd "$(dirname "${BASH_SOURCE[0]}")/.."

# Download tools
source setenv.sh
run_setenv
source downloadtools.sh
download_tools
cd ..

# Set path to the root directory of the git repository
ROOT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ROOT_DIR="$ROOT_PATH"

# Set path to the output directory for generated assets
OUT_DIR="output"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"
cd "$OUT_DIR" 
echo "OUT_DIR= $OUT_DIR"

# Compilers path
export AFR_TOOLCHAIN_PATH="$GCC_DIR/bin"
export CY_COMPILER_GCC_ARM_DIR="$GCC_DIR"

echo "cmake -G 'Unix Makefiles' -S .. -B . -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=$BOARD -DAPP=projects/cypress/$BOARD/mtb/ota -DAPP_VERSION_MAJOR=1"
cmake -G "Unix Makefiles" -S .. -B . -DVENDOR=cypress -DCOMPILER=arm-gcc -DBOARD=$BOARD -DAPP=projects/cypress/$BOARD/mtb/ota -DAPP_VERSION_MAJOR=1

# clean sign sign_ota.sh
:> sign_ota.sh
chmod 777 sign_ota.sh
cd "$ROOT_DIR"

PROJECT=afr_ota
STREAM=afr_ota_$BOARD

# Execute Coverity analysis per template defined in asset-template:
# http://git-ore.aus.cypress.com/devops/asset-template/blob/master/.gitlab-ci.yml
# http://git-ore.aus.cypress.com/devops/asset-template/blob/master/coverity/readme.md
ANALYSIS_CONFIG=.coverity_analyze.config
source cypress_internal_test/coverity/coverity_environment.sh $(pwd)/cypress_internal_test > $ANALYSIS_CONFIG
#source cypress_internal_test/coverity/coverity_environment.sh $(pwd)/output > $ANALYSIS_CONFIG
echo "PROJECT: $PROJECT" >> $ANALYSIS_CONFIG
echo "STREAM: $STREAM" >> $ANALYSIS_CONFIG
COVERITY_SOURCE_ROOT=$(pwd)/vendors/cypress/boards/$BOARD/ports/ota
echo "SOURCE_ROOT: $COVERITY_SOURCE_ROOT" >> $ANALYSIS_CONFIG
# MISRA C 2004 coding standards in Coverity
#export SUPPLIED_CODING_STANDARD=misrac2004/misrac2004-advisory-only.config
#export SUPPLIED_CODING_STANDARD=misrac2004/misrac2004-required-only.config
#export SUPPLIED_CODING_STANDARD=misrac2004/misrac2004-all.config
# MISRA C++ 2008 coding standards in Coverity
#export SUPPLIED_CODING_STANDARD=misracpp2008/misracpp2008-advisory-only.config
#export SUPPLIED_CODING_STANDARD=misracpp2008/misracpp2008-required-only.config
#export SUPPLIED_CODING_STANDARD=misracpp2008/misracpp2008-all.config
# MISRA C 2012 coding standards in Coverity
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-advisory-only.config
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-required-only.config
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-required-advisory.config
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-mandatory-only.config
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-mandatory-advisory.config
#export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-mandatory-required.config
export SUPPLIED_CODING_STANDARD=misrac2012/misrac2012-all.config
# COVERITY_CODING_STANDARD=$COVERITY_ROOT/config/coding-standards/$SUPPLIED_CODING_STANDARD
# Using customized MISRA config with excluded deviations
COVERITY_CODING_STANDARD=$COVERITY_TOOLS/misrac2012-custom.config
cat $COVERITY_CODING_STANDARD
echo "CODING_STANDARD: $COVERITY_CODING_STANDARD" >> $ANALYSIS_CONFIG
#echo "CUSTOM_CHECKER: cy-checker/001-60227/checker-all.cxm" >> $ANALYSIS_CONFIG
echo "ENABLE_CHECKERS:" >> $ANALYSIS_CONFIG
echo "  - ENUM_AS_BOOLEAN" >> $ANALYSIS_CONFIG
echo "  - HFA" >> $ANALYSIS_CONFIG
echo "DISABLE_CHECKERS:" >> $ANALYSIS_CONFIG
echo "  - SECURE_CODING" >> $ANALYSIS_CONFIG
echo "  - MISRA_CAST" >> $ANALYSIS_CONFIG
echo "ANALYZE_OPTS:" >> $ANALYSIS_CONFIG
echo "  - --all" >> $ANALYSIS_CONFIG
echo "SOURCE_FILTER:" >> $ANALYSIS_CONFIG
# Skip checking of sources
echo "  - \"! file('demos')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('libraries')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('tests')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('tools')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('mcuboot')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('tools')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('freertos_kernel')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('vendors/cypress/psoc6')\"" >> $ANALYSIS_CONFIG
echo "  - \"! file('vendors/cypress/whd')\"" >> $ANALYSIS_CONFIG
echo "  - success()" >> $ANALYSIS_CONFIG
cat $ANALYSIS_CONFIG
echo "# ANALYSIS_CONFIG: $ANALYSIS_CONFIG"

set | egrep "^(COVERITY_|RUN_COV_)"
echo "COVERITY_CONFIG: $COVERITY_CONFIG"
cov-configure --config $COVERITY_CONFIG --template --comptype gcc --compiler arm-none-eabi-gcc
cov-configure --config $COVERITY_CONFIG --list-configured-compilers text

echo "- $RUN_COV_BUILD  make $OUT_DIR -j4"
$RUN_COV_BUILD  make -C $OUT_DIR -j4

echo "- $RUN_COV_ANALYZE $ANALYSIS_CONFIG"
$RUN_COV_ANALYZE $ANALYSIS_CONFIG

if [[ ${branch} = "cypress-202002.00" ]]; then
    echo "- $RUN_COV_POST_RESULTS $ANALYSIS_CONFIG"
    $RUN_COV_POST_RESULTS $ANALYSIS_CONFIG
    echo "To analyze the code analysis results at the coverity server, follow the URL:"
    echo "http://iot-coverity.aus.cypress.com:8080/reports.htm#v11626/p10113"
fi

echo "Coverity logs"
mkdir -p deploy/coverity
cp -v cov-*log*.txt deploy/coverity

# Filter out headers of external components
errorlist=cov-analyze-errorlist-$STREAM.txt
echo "errorlist: $errorlist"
awk -v RS= -v FS="\n" '!(/^ARM_GNU/){printf("%s\n\n", $0)}' $errorlist > deploy/coverity/$errorlist
errorlist_board=cov-analyze-errorlist-$STREAM-$BOARD.txt
awk "/aws_ota_pal.c/{i=3;}{if(i){i--; print;}}" deploy/coverity/$errorlist > deploy/coverity/$errorlist_board
echo "Coverity MISRA analysis results:"
cat deploy/coverity/$errorlist_board

# uninstall virtualenv
set +u
[[ ${host} = mac ]]     && deactivate
[[ ${host} = ubuntu ]]  && deactivate
[[ ${host} = win ]]     && deactivate
${restore_nounset}
