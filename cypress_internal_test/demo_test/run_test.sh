#!/bin/bash

current_dir="$PWD/$(dirname "${BASH_SOURCE[0]}")"
pushd "$current_dir/.."

source setenv.sh
run_setenv

popd

AFR_DIR="$current_dir/../.."

ERRORS=0
test_error=0
board=$1

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    python_cmd="python3.7"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws
    python_cmd="python"
fi

# create a tmp file to accumulate test result
result_file=$(mktemp)
echo "Tests results:" > $result_file

function compile_cmake()
{
    pushd $AFR_DIR

    # builds the project
    $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-gcc -DBLE_SUPPORTED=1 -DAFR_TOOLCHAIN_PATH="$GCC_DIR/bin" -S . -B build -G Ninja
    $NINJA_DIR/ninja -C build
    test_error=$?
    popd
    return $test_error
}

function compile_make()
{
    pushd $AFR_DIR/projects/cypress/$board/mtb/aws_demos

    make build -j4 BLE_SUPPORT=1
    test_error=$?
    popd
    return $test_error
}

function run_obj_copy()
{
    local file_name=$1

    $GCC_DIR/bin/arm-none-eabi-objcopy -O ihex $file_name.elf $file_name.hex
    test_error=$?
    return $test_error
}

flash_hex()
{
    file_name=$1
    device_id=$($python_cmd get_device_id.py $board)
    pyocd erase -c -u $device_id
    pyocd flash -e chip -u $device_id $file_name

    test_error=$?
    return $test_error
}

function clean_up()
{
    pushd $AFR_DIR

    git checkout "$AFR_DIR/vendors"
    git checkout "$AFR_DIR/demos"
    rm -rf "$AFR_DIR/build"

    popd
}

function check_uart_result()
{
    local demo_name=$1

    TEST_TIMEOUT="${TEST_TIMEOUT:-300}"
    ACCEPT_MESSAGE="${ACCEPT_MESSAGE:-Demo completed successfully.}"
    FAIL_MESSAGE="${FAIL_MESSAGE:-Error running demo.}"

    pushd $current_dir
    $python_cmd parse_serial.py $board "$TEST_TIMEOUT" "$ACCEPT_MESSAGE" "$FAIL_MESSAGE"

    test_error=$?
    if [ $test_error -eq 0 ]; then
        echo "$demo_name: passed" >>$result_file
    elif [ $test_error -eq 1 ]; then
        echo "$demo_name: failed." >>$result_file
    elif [ $test_error -eq 2 ]; then
        echo "$demo_name: timed out." >>$result_file
    fi
    return $test_error

    popd
}

function run_cmake_test()
{
    local test=$1
    test_error=0

    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes

    compile_cmake $board
 
    if [ $test_error -eq 0 ]; then
        run_obj_copy "$AFR_DIR/build/aws_demos"
    fi
 
    if [ $test_error -eq 0 ]; then
        flash_hex "$AFR_DIR/build/aws_demos.hex"
    fi

    fi
    if [ $test_error -eq 0 ]
    then
        check_uart_result "cmake $test"
    fi
    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi

    clean_up

    popd
}

function run_make_test()
{
    local test=$1
    test_error=0

    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes

    compile_make

    if [ $test_error -eq 0 ]; then
        board_name=$(echo $board | sed s/_/-/g)
        flash_hex "$AFR_DIR/build/cy/aws_demos/$board_name/Debug/aws_demos.hex"
    fi
    
    if [ $test_error -eq 0 ]
    then
        check_uart_result "make $test"
    fi
    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi

    clean_up

    popd
}

function replace_define()
{
    # this will not handle multi line defines
    local file=$1
    local define=$2
    local value=$3

    sed -i "s/#define $define .*/#define $define $value/" $file
}

function apply_common_changes()
{
    # TODO
    # The credidential_key file and the endpoint need to be filled in when we have a corporate aws account
    cp -f aws_clientcredential_keys.h $AFR_DIR/demos/include/aws_clientcredential_keys.h
    cp -f aws_clientcredential.h $AFR_DIR/demos/include/aws_clientcredential.h

    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_SSID \"$DEVICE_TESTER_WIFI_SSID\"
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_PASSWORD \"$DEVICE_TESTER_WIFI_PASSWORD\"
}

set +e

rm -rf $AFR_DIR/build

#run_cmake_test tcp_echo
run_cmake_test mqtt
run_cmake_test shadow

#run_make_test tcp_echo
run_make_test mqtt
run_make_test shadow

echo "----------------"
cat $result_file
rm -f $result_file

exit $ERRORS
