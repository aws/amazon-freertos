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
            export PATH="$(cygpath --unix "$CY_DEP_DIR/openocd-4.0.0.820-windows/openocd/bin"):$PATH"  
            export CMAKE_DIR="$CY_DEP_DIR/cmake-3.15.3-win64-x64/bin"
            export NINJA_DIR="$CY_DEP_DIR/ninja-1.9.0-win"
            export OPENOCD_DIR="$CY_DEP_DIR/openocd-4.0.0.820-windows/openocd"
            
            # Set path to GCC toolchain root directory
            export GCC_DIR="$(cygpath --mixed "${GCC_DIR:-$CY_DEP_DIR/gcc-7.2.1-1.0.0.1-windows}")"
            export IAR_DIR="$(cygpath --mixed "${HOST_IAR_PATH_8504}")"
            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_win_$IDT_VER/devicetester_freertos_win}"
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

            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_linux_$IDT_VER/devicetester_freertos_linux}"
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
            
            export AFR_DEVICE_TESTER_DIR="${AFR_DEVICE_TESTER_DIR:-$CY_DEP_DIR/devicetester_freertos_mac_$IDT_VER/devicetester_freertos_mac}"
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
    
    case "$KERNEL" in
        CYGWIN*|MINGW*|MSYS*)
            pushd $CY_DEP_DIR
            _arm_compiler=Win64
            git clone git@git-ore.aus.cypress.com:devops/devops_scripts.git
            devops_scripts/install_tool.sh git@git-ore.aus.cypress.com:devops/tools/ARM_Compiler.git arm_compiler_612 ${_arm_compiler}
            
            export ARMCC_DIR=$(pwd)/ARM_Compiler/${_arm_compiler}
            export PATH=${ARMCC_DIR}/bin:$PATH
            popd 
        ;;
    esac
    
}
