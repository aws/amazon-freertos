# This function checks if the .timestamp file is up to date
# Arguments:
# 1 - path to the .timestamp file
# 2 - the threshold in seconds to decide if the timestamp is new enough
#     default threshold: 86400 seconds = 60*60*24 = 1 day
# Returns:
# 0 - the timestamp is up to date
# 1 - the timestamp is out of date
#
function check_timestamp()
{
    local timestamp_file=$1
    local threshold_sec=${2:-86400}
    # when the timestamp file doesn't exist, always download the artifacts
    [[ -f "$timestamp_file" ]] || return 1
    # read the timestamp from file
    local timestamp_date=$(< "$timestamp_file")
    # convert timestamp to UNIX time in seconds.
    # GNU date: use "date -d", BSD date: use "date -j -f"
    local timestamp_sec=$(date -d "$timestamp_date" "+%s" 2>/dev/null || \
    date -j -f "%Y-%m-%d %H:%M:%S" "$timestamp_date" "+%s" || echo 0)
    # determine the current UNIX time in seconds
    local current_sec=$(date "+%s")
    # check if the current time exceeds timestamp by threshold
    return $(( $current_sec - $timestamp_sec >= $threshold_sec ))
}

# This function saves the timestamp to file in the following format:
# 2019-04-03 14:09:53
function save_timestamp()
{
    local timestamp_file=$1
    local current_date=$(date "+%Y-%m-%d %H:%M:%S")
    echo "$current_date" > "$timestamp_file"
}

function run_setenv()
{
    local IDT_VER=${1:-'3.1.0'}

    # Set OS-specific directories
    KERNEL="$($(which uname) -s)"
    case "$KERNEL" in
        CYGWIN*|MINGW*|MSYS*)
            # Cygwin, MinGW, Git Bash, MSYS2 provide cygpath command
            command -v cygpath >/dev/null 2>&1 || { echo >&2 "ERROR: cygpath tool not found."; exit 1; }

            # Download Windows dependencies
            export CY_DEP_DIR="${CY_DEP_DIR:-$LOCALAPPDATA/cydep}"
            export CY_DEP_MANIFEST="dependencies-windows.txt"
            export PATH="$(cygpath --unix "$CY_DEP_DIR/cmake-3.15.3-win64-x64/bin"):$PATH"
            export PATH="$(cygpath --unix "$CY_DEP_DIR/ninja-1.9.0-win"):$PATH"
            export CMAKE_DIR="$CY_DEP_DIR/cmake-3.15.3-win64-x64/bin"
            export NINJA_DIR="$CY_DEP_DIR/ninja-1.9.0-win"


            # Set path to GCC toolchain root directory
            export GCC_DIR="$(cygpath --mixed "${GCC_DIR:-$CY_DEP_DIR/gcc-7.2.1-1.0.0.1-windows}")"
            export ARMCC_DIR="${ARMCC_DIR:-$CY_DEP_DIR/arm-compiler-6.12-windows-x86_64}"
            export PATH="${ARMCC_DIR}/bin:$PATH"
            export PATH="$(cygpath -u -a "${GCC_DIR}/bin"):$PATH"
            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_win_$IDT_VER/devicetester_freertos_win}"
            if [[ -z "${HOST_IAR_PATH_8504+x}" ]]; then
                export IAR_DIR="$(cygpath --mixed "${IAR_DIR:-C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.2/arm}")"
            else
                export IAR_DIR="$(cygpath --mixed "${HOST_IAR_PATH_8504}")"
            fi
            # Assume "python" points to Python3 executable
            export PYTHON="${PYTHON:-"python3"}"
            ;;
        Linux*)
            # Download Linux dependencies
            XDG_CACHE_HOME="${XDG_CACHE_HOME:-$HOME/.cache/cydep}"
            export CY_DEP_DIR="${CY_DEP_DIR:-$XDG_CACHE_HOME}"
            export CY_DEP_MANIFEST="dependencies-linux.txt"
            export PATH="$CY_DEP_DIR/cmake-3.15.3-Linux-x86_64/bin:$PATH"
            export PATH="$CY_DEP_DIR/ninja-1.9.0-linux:$PATH"
            export PATH="$CY_DEP_DIR/openocd-4.0.0.820-linux/openocd/bin:$PATH"
            export CMAKE_DIR="$CY_DEP_DIR/cmake-3.15.3-Linux-x86_64/bin"
            export NINJA_DIR="$CY_DEP_DIR/ninja-1.9.0-linux"
            export OPENOCD_DIR="$CY_DEP_DIR/openocd-4.0.0.820-linux/openocd"

            # Set path to GCC toolchain root directory
            export GCC_DIR="${GCC_DIR:-$CY_DEP_DIR/gcc-7.2.1-1.0.0.1-linux}"
            export ARMCC_DIR="${ARMCC_DIR:-$CY_DEP_DIR/arm-compiler-6.12-linux-x86_64}"
            export PATH="${ARMCC_DIR}/bin:$PATH"
            export PATH="${GCC_DIR}/bin:$PATH"

            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_linux_$IDT_VER/devicetester_freertos_linux}"

            # Use "python3" to find Python3 executable ("python" is Python2)
            export PYTHON="${PYTHON:-"python3"}"
            ;;
        Darwin*)
            # Download MacOS dependencies
            XDG_CACHE_HOME="${XDG_CACHE_HOME:-$HOME/.cache/cydep}"
            export CY_DEP_DIR="${CY_DEP_DIR:-$XDG_CACHE_HOME}"
            export CY_DEP_MANIFEST="dependencies-osx.txt"
            export PATH="$CY_DEP_DIR/cmake-3.15.3-Darwin-x86_64/CMake.app/Contents/bin:$PATH"
            export PATH="$CY_DEP_DIR/ninja-1.9.0-mac:$PATH"
            export PATH="$CY_DEP_DIR/openocd-4.0.0.820-macos/openocd/bin:$PATH"
            export CMAKE_DIR="$CY_DEP_DIR/cmake-3.15.3-Darwin-x86_64/CMake.app/Contents/bin"
            export NINJA_DIR="$CY_DEP_DIR/ninja-1.9.0-mac"
            export OPENOCD_DIR="$CY_DEP_DIR/openocd-4.0.0.820-macos/openocd"

            # Set path to GCC toolchain root directory
            export GCC_DIR="${GCC_DIR:-$CY_DEP_DIR/gcc-7.2.1-1.0.0.1-macos}"
            export PATH="${GCC_DIR}/bin:$PATH"

            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_mac_$IDT_VER/devicetester_freertos_mac}"

            # Use "python3" to find Python3 executable ("python" is Python2)
            export PYTHON="${PYTHON:-"python3"}"
            ;;
        *)
            echo >&2 "[ERROR]: unsupported OS: $KERNEL"
            exit 1
            ;;
    esac

    export ARMLMD_LICENSE_FILE="${ARMLMD_LICENSE_FILE:-"8224@armlmd-aus-prod-wan.licenses.cypress.com"}"

    # Download third-party dependencies
    # Avoid calling downloaddeps.sh on each shell script invocation
    # by saving the timestamp to CY_DEP_DIR
    deps_file="$CY_DEP_MANIFEST"
    mkdir -p "$CY_DEP_DIR"
    timestamp_file="$CY_DEP_DIR/.timestamp_afr"
    echo "$timestamp_file"
    # Always download dependencies when deps_file is newer
    if [[ "$deps_file" -nt "$timestamp_file" ]] || ! check_timestamp "$timestamp_file"; then
        sh "downloaddeps.sh" "$deps_file"
        save_timestamp "$timestamp_file"
    fi
}

# This function configures Python environment
function setup_python_env()
{
    # Set path to the python environment
    local env_dir=${1:-env}

    # Check if 'pip install' is needed
    # Skip on consecutive invocations to speedup local build
    # Can be overridden by FORCE_PIP_INSTALL=1 !!
    local need_pip_install=${FORCE_PIP_INSTALL:=0}

    # Create and activate the environment
    # Note: under Windows use virtualenv, under UNIX - venv
    case "$KERNEL" in
        CYGWIN*|MINGW*|MSYS*)
            if ! [[ -d "${env_dir}" ]]; then
                echo "Creating python virtual environment in ${env_dir}"
                "$PYTHON" -m virtualenv "${env_dir}"
                need_pip_install=1
            fi
            source "${env_dir}/Scripts/activate"
            ;;
        Linux*|Darwin*)
            if ! [[ -d "${env_dir}" ]]; then
                echo "Creating python virtual environment in ${env_dir}"
                "$PYTHON" -m venv "${env_dir}"
                need_pip_install=1
            fi
            set +u # activate script uses unreferenced variables
            source "${env_dir}/bin/activate"
            set -u
            ;;
        *)
            echo >&2 "ERROR: unsupported OS: $KERNEL"
            exit 1
            ;;
    esac

    if [[ ${need_pip_install} -eq 1 ]]; then
        # Populate pip install arguments
        local pip_install_args=

        # Install required python modules.
        # Pip version 20.0.1 doesn't include general "py3-none-platform" tags when evaluating wheel validity,
        # which prevents pip from installing cmsis-pack-manager.
        # https://github.com/pypa/pip/issues/7629
        python -m pip install $pip_install_args --upgrade pip
        #python -m pip install $pip_install_args --upgrade -r "$SCRIPTS_DIR/requirements.txt"
    fi

    # Once the environment is activated, the 'python' binary always points to env_dir
    export PYTHON=python
    "$PYTHON" --version
}

# This function installs cysecuretools into the python environment
function setup_cysecuretools()
{
    # Check the python virtual environment is loaded to avoid system-wide install
    [[ -n ${VIRTUAL_ENV} ]] || { echo "ERROR: setup_python_env was never called"; return 1; }

    # Define the package version as seen reported by "pip freeze"
    local cysecuretools_version=2.0.0

    # Install cysecuretools module, in case the correct version is not yet installed
    # Checking the package version by grepping "pip show" output
    if ! pip show cysecuretools 2>/dev/null | grep "^Version:" | grep -q "$cysecuretools_version"; then
        # true: install official package from pypi.org
        # false: install directly from gitlab development repo
        local use_pypi=false

        if [[ $use_pypi == true ]]; then
            echo "Installing cysecuretools-$cysecuretools_version from pypi.org"
            pip install --upgrade "cysecuretools==$cysecuretools_version"
        else
            # Use fixed tag to avoid sudden breakage of bsp_csp CI due to incompatible cysecuretools changes
            # Lookup the recent tags: http://git-ore.aus.cypress.com/repo/cysecuretools/-/tags
            local cysecuretools_url=http://git-ore.aus.cypress.com/repo/cysecuretools.git
            local cysecuretools_tag=CYSECURETOOLS_3.1.0_CYBOOTLOADER_SWAP_SCRATCH_TC3

            # cysecuretools dev package require new setuptools/pip
            python -m pip install --upgrade setuptools pip

            echo "Installing cysecuretools#$cysecuretools_tag from $cysecuretools_url"
            pip install --upgrade git+$cysecuretools_url@$cysecuretools_tag
        fi
    fi
}
