#!/bin/bash

set -$-ue${DEBUG+xv}
set -e

ERRORS=0
test_error=0
board=$1
toolchain=${2:-GCC_ARM}

nightly=${NIGHTLY:-0}

# Prepare python virtual enviroment
bash --version

restore_nounset=""
test -o nounset && restore_nounset="set -u"

#********************************************************************************************
current_dir="$PWD/$(dirname "${BASH_SOURCE[0]}")"
pushd "$current_dir/.."

source setenv.sh
run_setenv

source downloadtools.sh
download_tools

popd

export CY_TOOLS_PATHS="$PWD/cypress_internal_test/tools"
mkdir -p $CY_TOOLS_PATHS/gcc-7.2.1 && cp -r $GCC_DIR/* $CY_TOOLS_PATHS/gcc-7.2.1

#********************************************************************************************
# Setup virtual environment
AFR_DIR="$PWD"

setup_python_env $AFR_DIR/env
setup_cysecuretools

#********************************************************************************************
declare -a supported_toolchains=()
KERNEL="$($(which uname) -s)"
case "$KERNEL" in
    CYGWIN*|MINGW*|MSYS*)    
    supported_toolchains=("GCC_ARM" "ARM" "IAR")
    host=win
    ;;
    Linux*)
    supported_toolchains=("GCC_ARM" "ARM")
    host=linux
    ;;
    Darwin*)
    supported_toolchains=("GCC_ARM")
    host=mac
    ;;
    *)
    echo >&2 "[ERROR]: unsupported OS: $KERNEL"
    exit 1
    ;;
esac

# Find drive letter to mount amazon-freertos on windows to work around path length issues.
win_drive_letter="A:"
if [ ${host} == win ]; then
    # Determine gitlab-runner instance ID from path
    folder_path=`pwd`
    # Assign different path for each gitlab-runner instance, to enable concurrent CI jobs
    if echo "$folder_path" | grep -q "/1/"; then
    win_drive_letter="B:" # use drive B: for 2nd instance
    elif echo "$folder_path" | grep -q "/2/"; then
    win_drive_letter="I:" # use drive I: for 3rd instance
    elif echo "$folder_path" | grep -q "/3/"; then
    win_drive_letter="J:" # use drive I: for 4th instance
    fi
fi

# Unmount virtual drive
function unmount_win_drive()
{
    # Check if the drive is mounted
    if [[ -d "$(cygpath $win_drive_letter)" ]]; then
        # Cygwin and MSYS interpret /D differently..
        KERNEL="$($(which uname) -s)"
        case "$KERNEL" in
            CYGWIN*)
                subst $win_drive_letter /D && echo "Unmounted virtual drive $win_drive_letter"
                ;;
            MINGW*|MSYS*)
                subst $win_drive_letter //D && echo "Unmounted virtual drive $win_drive_letter"
                ;;
            *)
                echo "Unsupported OS: $KERNEL"
                exit 1
                ;;
        esac
    fi
}


#********************************************************************************************

# create a tmp file to accumulate test result
result_file=$(mktemp)
echo "Tests results:" > $result_file

function compile_cmake()
{
    local board=$1
    local additional_flags=${2:-}

    pushd $AFR_DIR

    # builds the project
    case "$toolchain" in
        GCC_ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-gcc $additional_flags -DAFR_TOOLCHAIN_PATH="$GCC_DIR/bin" -DBLE_SUPPORTED=1 -S . -B build -G Ninja
            ;;
        ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-armclang $additional_flags -DAFR_TOOLCHAIN_PATH="$ARMCC_DIR/bin" -DBLE_SUPPORTED=1 -S . -B build -G Ninja
            ;;
        IAR*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-iar $additional_flags -DAFR_TOOLCHAIN_PATH="$IAR_DIR/bin" -DBLE_SUPPORTED=1 -S . -B build -G Ninja
            ;;
    esac

    $NINJA_DIR/ninja -C build
    test_error=$?

    popd
    return $test_error
}

function compile_make()
{
    local board=$1
    local additional_flags=${2:-}
    local build_dir="$AFR_DIR/build/make"

    # git set timestamp of files on clone
    # This update the timestamps files so that generated_source will not get re-generated.
    echo "find $AFR_DIR -type f -name \"*.timestamp\" -exec touch {} +"
    find $AFR_DIR -type f -name "*.timestamp" -exec touch {} +

    pushd $AFR_DIR/projects/cypress/$board/mtb/aws_demos

    # Extra quotes are added to the CY_COMPILER_PATH argument since the make build system will take off the first set of quotes
    # when the variable is initially resolved.
    # The CY_BUILD_LOCATION needs to be provided otherwise the devops machines get a wrong build location for some reason.
    case "$toolchain" in
        GCC_ARM*)
        make build $additional_flags CY_COMPILER_PATH="\"$GCC_DIR\"" CY_BUILD_LOCATION=$build_dir TOOLCHAIN=$toolchain -j
        ;;
        ARM*)
        make build $additional_flags CY_COMPILER_PATH="\"$ARMCC_DIR\"" CY_BUILD_LOCATION=$build_dir TOOLCHAIN=$toolchain -j
        ;;
        IAR*)
        make build $additional_flags CY_COMPILER_PATH="\"$IAR_DIR\"" CY_BUILD_LOCATION=$build_dir TOOLCHAIN=$toolchain -j
        ;;
    esac
    test_error=$?

    popd
    return $test_error
}

function check_build_status()
{
    local demo_name=$1
    test_error=$2

    if [ $test_error -eq 0 ]; then
        echo "$demo_name: ($toolchain) passed" >>$result_file
    elif [ $test_error -ne 0 ]; then
        echo "$demo_name: ($toolchain) failed." >>$result_file
    fi

    return $test_error
}

function clean_up()
{
    pushd $AFR_DIR

    git checkout "$AFR_DIR/vendors"
    git checkout "$AFR_DIR/demos"
    git checkout "$AFR_DIR/projects"
    rm -rf "$AFR_DIR/build"

    popd
}

function run_cmake_test()
{
    local test=$1
    local additional_flags=${2:-}

    test_error=0

    echo -e  "******************** " "Cmake build " $test " demo test started" " ********************"

    source "$current_dir/$test/setup_test.sh"

    setup_test $board
    compile_cmake $board $additional_flags
    test_error=$?
    check_build_status "cmake $test" $test_error

    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi

    clean_up

    echo -e  "******************** " "Cmake build " $test " demo test finished" " ********************"
}

function run_make_test()
{
    local test=$1
    local additional_flags=${2:-}

    test_error=0

    echo -e  "******************** " "Make build " $test " demo test started" " ********************"

    source "$current_dir/$test/setup_test.sh"

    setup_test $board
    compile_make $board $additional_flags
    test_error=$?
    check_build_status "make $test" $test_error

    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi

    clean_up

    echo -e  "******************** " "Make build " $test " demo test finished" " ********************"
}

function replace_define()
{
    # this will not handle multi line defines
    local file=$1
    local define=$2
    local value=$3

    sed -i "s/#define $define .*/#define $define $value/" $file
}

set +e

rm -rf $AFR_DIR/build

if [[ $nightly -eq 0 ]]; then
    # Acceptance pipeline
    echo "Running acceptance job: MQTT + OTA demo CMake and all demos Make"

    ############################### CMAKE TESTS ####################################
    run_cmake_test mqtt
    # OTA feature is not supported on the WIFI_BT board.
    # Remove toolchain GCC_ARM condition after MIDDLEWARE-3718 and MIDDLEWARE-3719 are resolved.
    if [[ ($board != "CY8CKIT_062_WIFI_BT") ]]; then
        run_cmake_test ota -DOTA_SUPPORT=1
    fi

    # Test compile with BLE module disabled. (Only GCC should be sufficient)
    if [[ $toolchain == GCC_ARM ]]; then
        run_cmake_test mqtt -DBLE_SUPPORTED=0
    fi

    ############################### MAKE TESTS ####################################
    if [ ${host} == win ]; then
        unmount_win_drive
        WIN_AFR_DIR="$(cygpath --windows "${AFR_DIR}")"
        subst $win_drive_letter $WIN_AFR_DIR
        echo "Mounted $AFR_DIR as virtual drive $win_drive_letter"
        trap unmount_win_drive EXIT
        pushd $win_drive_letter >/dev/null
        # Update AFR_DIR with the substituted drive letter.
        AFR_DIR="$PWD"
    fi

    run_make_test mqtt
    if [[ ($board != "CY8CKIT_062_WIFI_BT") ]]; then
        run_make_test ota OTA_SUPPORT=1
    fi

    # All demos are only run on GCC_ARM since the source changes between demos are not owned by
    # cypress hence is unlikely to be changed by use and cause failures. The source files inclusion
    # for these demos is in a common file shared accross all demos so any changes to should caught in
    # tests for GCC.
    if [[ $toolchain == GCC_ARM ]]; then
        run_make_test mqtt  BLE_SUPPORTED=0
        run_make_test tcp_secure_echo
        run_make_test shadow
        run_make_test defender
        run_make_test green_grass
    fi
else
    # Nightly pipeline
    echo "Running nightly job: All demos build make and CMake"
    for val in ${supported_toolchains[@]}; do
        toolchain=$val

        run_cmake_test mqtt
        run_cmake_test mqtt -DBLE_SUPPORTED=0
        if [[ $board != "CY8CKIT_062_WIFI_BT" ]]; then
            run_cmake_test ota -DOTA_SUPPORT=1
        fi
        run_cmake_test tcp_secure_echo
        run_cmake_test shadow
        run_cmake_test defender
        run_cmake_test green_grass

        run_make_test mqtt
        run_make_test mqtt  BLE_SUPPORTED=0
        if [[ $board != "CY8CKIT_062_WIFI_BT" ]]; then
            run_make_test ota OTA_SUPPORT=1
        fi
        run_make_test tcp_secure_echo
        run_make_test shadow
        run_make_test defender
        run_make_test green_grass
    done
fi

echo "----------------"
cat $result_file
rm -f $result_file

# uninstall virtualenv
set +u
deactivate
rm -rf $AFR_DIR/env
${restore_nounset}

exit $ERRORS
