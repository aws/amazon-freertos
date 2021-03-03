compiler=${CONFIG_COMPILER:-GCC}
[ "$TESTRAIL_REPORT" -eq 0 ] && echo "To enable TestRail reporting set TESTRAIL_REPORT=1" && exit 0;
set -e

PYTHON=python3
PIP=pip3

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    # install virtualenv
    ${PYTHON} -m venv env

    # activate virtualenv
    source env/bin/activate

    # Install Python modules in the virtualenv 
    ${PIP} --no-cache-dir install --upgrade pip
    
    ${PIP} install -r  ${DIR}/requirements.txt
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws
    DIR="$(cygpath -w "$DIR")"
fi

# test results are located on level up
LOG_PATH="$(dirname "$DIR")"/test_results

LOG_TYPE="idt_xml"
CONFIGURATIONS="{\"Compiler\":\"$compiler\"}"

${PYTHON} ${DIR}/testrail_uploader.py ${LOG_PATH} ${LOG_TYPE} ${VERSION} ${BUILD_ID} -p ${TESTRAIL_PROJECT} -u ${TESTRAIL_USERNAME} -k ${TESTRAIL_API_KEY} -c ${CONFIGURATIONS}

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    # uninstall virtualenv
    deactivate
fi

${restore_nounset}
