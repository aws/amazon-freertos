#!/bin/bash

set -$-ue${DEBUG+xv}

# Configure python venv 
python3.7 -m venv env
source env/bin/activate
pip3 --no-cache-dir install --upgrade pip
pip3 install cysecuretools==2.0.0 
pip3 install pyserial
pip3 install pyopenssl

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

CAcertificateId='c165108be9702f777f2c87faf0d1b68b9a2d63a2c171b91cca6ace18c9c8b215'

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    python_cmd="python3.7"
    serial_port="/dev/ttyACM2"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Windows
    python_cmd="python"
    serial_port="COM3"
fi

# create a tmp file to accumulate test result
result_file=$(mktemp)
echo "Tests results:" > $result_file


function get_iot_things_name_and_dev_certificate()
{
    # erease fimware image
    echo " "
    echo "######### Erasing FW image #########"
    echo " "
    ${OPENOCD_DIR}/bin/openocd \
        -s ${OPENOCD_DIR}/scripts \
        -f interface/kitprog3.cfg \
        -f target/psoc6_2m_secure.cfg \
        -c "cmsis_dap_serial ${device_id}" \
        -c "adapter speed 1000" \
        -c "init; reset; flash erase_address 0x10000000 0xc0000" \
        -c "init; reset run; exit"
    
    echo $(PWD)
    # read device certificate
    $python_cmd jitp/get_device_certificate.py ${device_id}

    #read things name
    IOT_THING_NAME=$(openssl x509 -noout -subject -in device_cert.json | awk -F"CN = " '/CN = /{print $2}')
    echo -ne "IoT things name: $IOT_THING_NAME \\n"

    certificateId=$(openssl x509 -outform der -in device_cert.json | openssl dgst --sha256 | cut -c10-74)
    echo -ne "Device certificate: $certificateId \\n"
    
    test_error=$?
    return $test_error
}

function enable_auto_registration()
{
    aws iot update-ca-certificate --certificate-id ${CAcertificateId}  --new-auto-registration-status ENABLE --registration-config file://jitp/psoc64JITP.json

    # confirm CA status
    if (aws iot describe-ca-certificate --certificate-id ${CAcertificateId} | grep ENABLE); then
        test_error=0
    else
        test_error=1
    fi
    
    test_error=$?
    return $test_error
}



function remove_device_from_aws()
{
    # Use AWS CLI command to delete registered certificate :

    # 1. De-active the certificate
    aws iot update-certificate --certificate-id ${certificateId} --new-status INACTIVE

    # 2. Detach certificate with thing
    aws iot detach-thing-principal --thing-name ${IOT_THING_NAME} --principal "arn:aws:iot:us-east-2:969910087511:cert/${certificateId}"

    # 3. Delete the certificate
    aws iot delete-certificate --force-delete --certificate-id ${certificateId}

    # 4. Delete the thing
    aws iot delete-thing --thing-name ${IOT_THING_NAME}

    echo "Device was removed from AWS"
    
    test_error=$?
    return $test_error
}

function check_if_device_added_to_aws()
{
    if (aws iot list-things | grep ${IOT_THING_NAME} ) && (aws iot list-certificates | grep ${certificateId}) ; then
        test_error=0
    else
        test_error=1
    fi

    return $test_error
}

function compile_cmake()
{   
    mkdir -p $AFR_DIR/build/cy
    pushd $AFR_DIR/build/cy

    # builds the project    
    case "$toolchain" in 
        GCC_ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DAPP=projects/cypress/$board/mtb/jitp -DCOMPILER=arm-gcc -DAFR_TOOLCHAIN_PATH="$GCC_DIR/bin" -S ../.. -B . -G Ninja
            ;;
        ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DAPP=projects/cypress/$board/mtb/jitp -DCOMPILER=arm-armclang -DAFR_TOOLCHAIN_PATH="$ARMCC_DIR/bin" -S ../.. -B . -G Ninja
            ;;
        IAR*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DAPP=projects/cypress/$board/mtb/jitp -DCOMPILER=arm-iar -DAFR_TOOLCHAIN_PATH="$IAR_DIR/bin" -S ../.. -B . -G Ninja
            ;;
    esac
       
    $NINJA_DIR/ninja -C .
    test_error=$?
    popd
    return $test_error
}

function compile_make()
{
    pushd $AFR_DIR/projects/cypress/$board/mtb/jitp
    echo $toolchain
    
    case "$toolchain" in 
        GCC_ARM*)
        make build CY_COMPILER_PATH=$GCC_DIR TOOLCHAIN=$toolchain -j8
        ;;
        ARM*)
        make build CY_COMPILER_PATH=$ARMCC_DIR TOOLCHAIN=$toolchain -j8
        ;;
        IAR*)
        make build CY_COMPILER_PATH=$IAR_DIR TOOLCHAIN=$toolchain -j8
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

function run_cmake_test()
{
    local test=$1
    test_error=0
    
    echo -e  "******************** " "Cmake " $test " demo test started" " ********************"

    pushd $current_dir

    get_iot_things_name_and_dev_certificate
    
    enable_auto_registration
    if [ $test_error -eq 1 ]
    then
        ((ERRORS++))
        echo "error during auto registration"
    fi

    apply_common_changes
    
    compile_cmake $board
    
    if [ $test_error -eq 0 ]
    then        
          flash_hex_secure "$AFR_DIR/build/cy"     
    fi

    if [ $test_error -eq 0 ]
    then
        check_uart_result "cmake $test"
    fi

    if [ $test_error -eq 0 ]
    then
       check_if_device_added_to_aws
    fi

    if [ $test_error -eq 0 ]
    then
       remove_device_from_aws
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
    test_error=0
    
    echo -e  "******************** " "Make " $test " demo test started" " ********************"
    
    pushd $current_dir

    get_iot_things_name_and_dev_certificate

    enable_auto_registration
    if [ $test_error -eq 1 ]
    then
        ((ERRORS++))
        echo "error during auto registration"
    fi

    apply_common_changes

    compile_make
    
    if [ $test_error -eq 0 ]
    then
        flash_hex_secure "$AFR_DIR/build/cy/jitp/$board/Debug"
    fi

    if [ $test_error -eq 0 ]
    then
        check_uart_result "make $test"
    fi

    if [ $test_error -eq 0 ]
    then
       check_if_device_added_to_aws
    fi

    if [ $test_error -eq 0 ]
    then
       remove_device_from_aws
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
    replace_define $AFR_DIR/projects/cypress/CY8CKIT_064S0S2_4343W/mtb/jitp/include/aws_clientcredential.h clientcredentialMQTT_BROKER_ENDPOINT \"a33jl9z28enc1q.iot.us-east-2.amazonaws.com\"
    replace_define $AFR_DIR/projects/cypress/CY8CKIT_064S0S2_4343W/mtb/jitp/include/aws_clientcredential.h clientcredentialIOT_THING_NAME \"$IOT_THING_NAME\"
    replace_define $AFR_DIR/projects/cypress/CY8CKIT_064S0S2_4343W/mtb/jitp/include/aws_clientcredential.h clientcredentialWIFI_SSID \"$DEVICE_TESTER_WIFI_SSID\"
    replace_define $AFR_DIR/projects/cypress/CY8CKIT_064S0S2_4343W/mtb/jitp/include/aws_clientcredential.h clientcredentialWIFI_PASSWORD \"$DEVICE_TESTER_WIFI_PASSWORD\"
}

set +e

rm -rf $AFR_DIR/build

run_cmake_test jitp
run_make_test jitp

echo "----------------"
cat $result_file
rm -f $result_file

deactivate

exit $ERRORS
