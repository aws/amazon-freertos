#!/bin/bash

set -$-ue${DEBUG+xv}
set -e

ERRORS=0
test_error=0
host=$1
board=$2

# Prepare python virtual enviromental 
bash --version
python --version

restore_nounset=""
test -o nounset && restore_nounset="set -u"

# install virtualenv
[[ ${host} = mac ]]     && python3 -m venv env
[[ ${host} = ubuntu ]]  && python3 -m venv env
[[ ${host} = win ]]     && python3 -m venv env

# activate virtualenv
set +u
[[ ${host} = mac ]]     && source env/bin/activate
[[ ${host} = ubuntu ]]  && source env/bin/activate
[[ ${host} = win ]]     && source env/Scripts/activate
${restore_nounset}

if [[ ${host} != win ]]; then
    pip --no-cache-dir install --upgrade pip
else
    python -m pip --no-cache-dir install --upgrade pip
fi

pip install --upgrade --force-reinstall cysecuretools==2.0.0 

current_dir="$PWD/$(dirname "${BASH_SOURCE[0]}")"
pushd "$current_dir/.."

source setenv.sh
run_setenv
popd

declare -a supported_toolchains=()

#********************************************************************************************
echo "MTB Tools tests install start"

mtb_gcc_url="http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/sdk_toolchains/develop/1/deploy"
mt_gcg_ver=gcc-7.2.1-1.0.0.1
KERNEL="$($(which uname) -s)"
case "$KERNEL" in
    CYGWIN*|MINGW*|MSYS*)
    export IAR_DIR="$(cygpath -u -a "${HOST_IAR_PATH_8504}")"
    export PATH="${IAR_DIR}/bin:$PATH"
    mtb_gcc_link=$mtb_gcc_url/$mt_gcg_ver-windows.zip
    mtb_gcc_arhive=$mt_gcg_ver-windows.zip
    supported_toolchains=("GCC_ARM" "ARM" "IAR")    
    ;;
    Linux*)
    # change this link linux
    mtb_gcc_link=$mtb_gcc_url/$mt_gcg_ver-linux.zip
    mtb_gcc_arhive=$mt_gcg_ver-linux.zip
    supported_toolchains=("GCC_ARM" "ARM")
    ;;
    Darwin*)
    mtb_gcc_link=$mtb_gcc_url/$mt_gcg_ver-macos.zip
    mtb_gcc_arhive=$mt_gcg_ver-macos.zip
    supported_toolchains=("GCC_ARM")
    ;;
    *)
    echo >&2 "[ERROR]: unsupported OS: $KERNEL"
    exit 1
    ;;
esac

mtb_make_link="http://iot-webserver.aus.cypress.com/projects/iot_release/ASSETS/repo/bsp_csp/develop/7927/deploy/tools-make-1.0.0.7927.zip"
mtb_tools_folder=mt_tools_2.1

mkdir -p $CY_DEP_DIR/$mtb_tools_folder
pushd $CY_DEP_DIR/$mtb_tools_folder

curl -O $mtb_make_link 
curl -O $mtb_gcc_link

unzip $mtb_gcc_arhive 
unzip tools-make-1.0.0.7927.zip

export CY_TOOLS_PATHS="$CY_DEP_DIR/$mtb_tools_folder"

popd

echo "MTB Tools tests install finish"
#********************************************************************************************

AFR_DIR="$PWD"
             
# create a tmp file to accumulate test result
result_file=$(mktemp)
echo "Tests results:" > $result_file

function compile_cmake()
{
    pushd $AFR_DIR
    
    # builds the project    
    case "$toolchain" in 
        GCC_ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-gcc -DOTA_SUPPORT=1 -DAFR_TOOLCHAIN_PATH="$GCC_DIR/bin" -S . -B build -G Ninja
            ;;
        ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-armclang -DOTA_SUPPORT=1 -DAFR_TOOLCHAIN_PATH="$ARMCC_DIR/bin" -S . -B build -G Ninja
            ;;
        IAR*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-iar -DOTA_SUPPORT=1 -DAFR_TOOLCHAIN_PATH="$IAR_DIR/bin" -S . -B build -G Ninja
            ;;
    esac
       
    $NINJA_DIR/ninja -C build
    test_error=$?
    popd
    return $test_error
}

function compile_make()
{
    pushd $AFR_DIR/projects/cypress/$board/mtb/aws_demos
    
    case "$toolchain" in 
        GCC_ARM*)
        make build OTA_SUPPORT=1 CY_COMPILER_PATH=$GCC_DIR TOOLCHAIN=$toolchain -j8
        ;;
        ARM*)
        make build OTA_SUPPORT=1 CY_COMPILER_PATH=$ARMCC_DIR TOOLCHAIN=$toolchain -j8
        ;;
        IAR*)
        make build OTA_SUPPORT=1 CY_COMPILER_PATH=$IAR_DIR TOOLCHAIN=$toolchain -j8
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
    test_error=0
    
    echo -e  "******************** " "Cmake build " $test " demo test started" " ********************"
    
    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    
    compile_cmake $board
    
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
    test_error=0
    
    echo -e  "******************** " "Make build " $test " demo test started" " ********************"
    
    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    echo $test_error
    compile_make
    test_error=$?
    echo $test_error
    
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

for val in ${supported_toolchains[@]}; do
    toolchain=$val
        
    run_cmake_test tcp_secure_echo
    run_cmake_test mqtt
    run_cmake_test shadow
    run_cmake_test defender
    run_cmake_test ota
    run_cmake_test green_grass    
    
    # There is issue with cygwin, cysecuretools and make
    if [[ ${host} != win ]]; then
        run_make_test tcp_secure_echo
        run_make_test mqtt
        run_make_test shadow
        run_make_test defender
        run_make_test ota 
        run_make_test green_grass
    fi
done

echo "----------------"
cat $result_file
rm -f $result_file

# uninstall virtualenv 
set +u
[[ ${host} = mac ]]     && deactivate
[[ ${host} = ubuntu ]]  && deactivate
[[ ${host} = win ]]     && deactivate
${restore_nounset}

exit $ERRORS
