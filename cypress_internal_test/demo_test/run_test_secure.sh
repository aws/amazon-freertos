#!/bin/bash

set -$-ue${DEBUG+xv}

# Configure python venv 
python3.7 -m venv env
source env/bin/activate
pip3 --no-cache-dir install --upgrade pip
pip3 install cysecuretools==2.0.0 
pip3 install -r cypress_internal_test/demo_test/ota/scripts/requirements.txt

current_dir="$PWD/$(dirname "${BASH_SOURCE[0]}")"
pushd "$current_dir/.."

source setenv.sh
run_setenv

popd


AFR_DIR="$PWD"
             
ERRORS=0
test_err=0
board=$1
toolchain=$2
device_id=$3

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    python_cmd="python3.7"
    #serial_port="/dev/ttyACM2"
    serial_port="/dev/serial/by-id/usb-Cypress_Semiconductor_KitProg3_CMSIS-DAP_122317F302098400-if02"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Windows
    python_cmd="python"
    serial_port="COM3"
fi


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

function run_obj_copy()
{
    local file_name=$1

    $GCC_DIR/bin/arm-none-eabi-objcopy -O ihex aws_demos.elf aws_demos.hex
    test_error=$?
    return $test_error
}

flash_hex_secure()
{
    hexs_folder=$1
    
    pushd $hexs_folder

    echo " "
    echo "######### Erasing eeprom from 0x14000000 to 0x14008000  #########"
    echo "######### Erasing SST are and flashing CM0 and CM4 cores #########"
    echo " "
    ${OPENOCD_DIR}/bin/openocd \
        -s ${OPENOCD_DIR}/scripts \
        -f interface/kitprog3.cfg \
        -f target/psoc6_2m_secure.cfg \
        -c "cmsis_dap_serial ${device_id}" \
        -c "adapter speed 1000" \
        -c "init; reset init" \
        -c "flash erase_address 0x14000000 0x8000" \
        -c "flash erase_address 0x101c0000 0x10000" \
        -c "flash write_image erase cm0.hex" \
        -c "flash write_image erase cm4.hex" \
        -c "resume; reset; exit"
    
    popd
    
    test_error=$?
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

function check_uart_result()
{
    local demo_name=$1

    TEST_TIMEOUT="${TEST_TIMEOUT:-400}"
    ACCEPT_MESSAGE="${ACCEPT_MESSAGE:-Demo completed successfully.}"
    FAIL_MESSAGE="${FAIL_MESSAGE:-Error running demo.}"
            
    pushd $current_dir
    $python_cmd parse_serial_secure.py $serial_port "$TEST_TIMEOUT" "$ACCEPT_MESSAGE" "$FAIL_MESSAGE"
    
    test_error=$?
    if [ $test_error -eq 0 ]; then
        echo "$demo_name: passed" >>$result_file
    elif [ $test_error -eq 1 ]; then
        echo "$demo_name: failed." >>$result_file
    elif [ $test_error -eq 2 ]; then
        echo "$demo_name: timed out." >>$result_file
    fi

    popd

    return $test_error
    
}

function create_ota_job()
{
    hexs_folder=$1
    ota_img_name=$2
    protocol=$3
        
    echo $hexs_folder
    
    pushd $current_dir/ota/scripts
            
    $python_cmd start_ota.py --profile default \
                                         --name demo-tester-thing \
                                         --role lava-role \
                                         --s3bucket lava-bucket \
                                         --otasigningprofile lavasigningprofile \
                                         --certarn ../signer_cert/certarn.json \
                                         --updateimage $hexs_folder/${ota_img_name} \
                                         --protocol ${protocol}
                                
    test_error=$?
    popd

    return $test_error
}

function update_img_ver()
{
    echo "updating image version..."
    sed -i -r "s/#define APP_VERSION_MAJOR    0/#define APP_VERSION_MAJOR    1/" $AFR_DIR/demos/include/aws_application_version.h

    sed -i -r "s/#define APP_VERSION_MAJOR    0/#define APP_VERSION_MAJOR    1/" $AFR_DIR/projects/cypress/$board/mtb/aws_demos/include/aws_application_version.h
}

function run_cmake_test()
{
    local test=$1
    local ota_protocol=${2:-'MQTT'}

    test_error=0
    
    echo -e  "******************** " "Cmake " $test " demo test started" " ********************"

    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes
    
    compile_cmake $board
    
    if [ $test_error -eq 0 ]
    then        
        flash_hex_secure "$AFR_DIR/build"      
    fi
    
    if [ "$test" == "ota" ]; then
        if [ $test_error -eq 0 ]
        then 
            echo "OTA protocol: " $ota_protocol
            echo "prepare image for OTA"
            update_img_ver
            
            rm -rf $AFR_DIR/build
            compile_cmake $board
            create_ota_job "$AFR_DIR/build" "cm4_upgrade.bin" $ota_protocol
        fi
    fi

    if [ $test_error -eq 0 ]
    then    
        if [[ ("$test" == "ota") && ("$ota_protocol" == "MQTT" || "$ota_protocol" == "HTTP") ]]; then
            check_uart_result "cmake $test $ota_protocol"       
        else
            check_uart_result "cmake $test"
        fi
    fi
    
    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi
        
    clean_up
    
    echo -e  "******************** " "Cmake " $test " demo test finished" " ********************"
}

function run_make_test()
{
    local test=$1
    local ota_protocol=${2:-'MQTT'}
    test_error=0
    
    echo -e  "******************** " "Make " $test " demo test started" " ********************"
    
    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes
    
    compile_make
    
    if [ $test_error -eq 0 ]
    then
        flash_hex_secure "$AFR_DIR/build/cy/aws_demos/$board/Debug"
    fi
    
    if [ "$test" == "ota" ]; then
        if [ $test_error -eq 0 ]
        then 
            echo "OTA protocol: " $ota_protocol
            echo "prepare image for OTA"
            update_img_ver
            
            rm -rf $AFR_DIR/build
            compile_make $board
            create_ota_job "$AFR_DIR/build/cy/aws_demos/$board/Debug" "cm4.bin" $ota_protocol
        fi
    fi

    if [ $test_error -eq 0 ]
    then
        if [[ ("$test" == "ota") && ("$ota_protocol" == "MQTT" || "$ota_protocol" == "HTTP") ]]; then
            check_uart_result "make $test $ota_protocol"
        else
            check_uart_result "make $test"
        fi
    fi
    
    if [ $test_error -ne 0 ]
    then
        ((ERRORS++))
        echo "Test $test failed."
    fi
        
    clean_up
    
    echo -e  "******************** " "Make " $test " demo test finished" " ********************"
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
    cp -f aws_clientcredential_keys.h $AFR_DIR/demos/include/aws_clientcredential_keys.h
    cp -f aws_clientcredential.h $AFR_DIR/demos/include/aws_clientcredential.h 
 
    cp -f $AFR_DIR/cypress_internal_test/demo_test/ota/aws_ota_codesigner_certificate.h $AFR_DIR/demos/include/aws_ota_codesigner_certificate.h
    
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_SSID \"$DEVICE_TESTER_WIFI_SSID\"
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_PASSWORD \"$DEVICE_TESTER_WIFI_PASSWORD\"
}

set +e

rm -rf $AFR_DIR/build

run_cmake_test tcp_secure_echo
run_cmake_test mqtt
run_cmake_test shadow
run_cmake_test defender
run_cmake_test ota MQTT
run_cmake_test ota HTTP

run_make_test tcp_secure_echo
run_make_test mqtt
run_make_test shadow
run_make_test defender
run_make_test ota MQTT
run_make_test ota HTTP

echo "----------------"
cat $result_file
rm -f $result_file

deactivate

exit $ERRORS
