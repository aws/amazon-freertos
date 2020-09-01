[ "$TESTRAIL_REPORT" -eq 0 ] && echo "To enable TestRail reporting set TESTRAIL_REPORT=1" && exit 0;
set -e

PYTHON=python3
PIP=pip3

# install virtualenv
${PYTHON} -m venv env

# activate virtualenv
source env/bin/activate

# Install Python modules in the virtualenv 
${PIP} --no-cache-dir install --upgrade pip

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# test results are located on level up
LOG_PATH="$(dirname "$DIR")"/test_results

LOG_TYPE="idt_xml"
CONFIGURATIONS="{\"Compiler\":\"GCC\"}"

${PIP} install -r  ${DIR}/requirements.txt
${PYTHON} ${DIR}/testrail_uploader.py ${LOG_PATH} ${LOG_TYPE} ${VERSION} ${BUILD_ID} -p ${TESTRAIL_PROJECT} -u ${TESTRAIL_USERNAME} -k ${TESTRAIL_API_KEY} -c ${CONFIGURATIONS}

# uninstall virtualenv
deactivate

${restore_nounset}
