#!/bin/bash

set -$-ue${DEBUG+xv}




current_dir="$PWD/$(dirname "${BASH_SOURCE[0]}")"
pushd "$current_dir/.."

source setenv.sh
run_setenv

popd

ERRORS=0
test_err=0
board=$1
toolchain=$2

ota_only=${OTA_ONLY:-0}
ext_flash=${OTA_USE_EXTERNAL_FLASH:-1}
    
if [[ $ota_only == 1 ]]; then
    demo_thing=$(echo demo-$board | sed s/_/-/g)
else
    demo_thing='demo-tester-thing'
fi

board_name=${board}

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
    python_cmd="python3.7"
    # Configure python venv 
    python3.7 -m venv env
    source env/bin/activate
    pip3 --no-cache-dir install --upgrade pip
    pip3 install --upgrade --force-reinstall git+http://git-ore.aus.cypress.com/repo/cysecuretools.git@CYSECURETOOLS_3.1.0_CYBOOTLOADER_SWAP_SCRATCH_TC3
    pip3 install -r cypress_internal_test/demo_test/ota/scripts/requirements.txt
    AFR_DIR="$PWD"
    if [[ ${board}  == "CY8CKIT_064S0S2_4343W" ]]; then
        board_name="CY8CKIT064S0S2_4343W"
    fi
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws
    python_cmd="python"
    AFR_DIR="$(cygpath -w "$PWD")"
fi

GET_DEVICE_LIST=$($python_cmd cypress_internal_test/demo_test/get_device_id_port.py $board_name)
GET_DEVICE_LIST="${GET_DEVICE_LIST/$'\r\n'/}"
IFS=' ;' read -ra DEVICE_LIST <<< "$GET_DEVICE_LIST"
DEVICE_ID="${DEVICE_LIST[0]}"
COM_PORT="${DEVICE_LIST[1]}"
echo "DEVICE_ID: [$DEVICE_ID]"
echo "COM_PORT: [$COM_PORT]"

# create a tmp file to accumulate test result
result_file=$(mktemp)
echo "Tests results:" > $result_file

function compile_cmake()
{
    local ota_en=${1:-0}
    pushd $AFR_DIR

    # builds the project    
    case "$toolchain" in 
        GCC_ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-gcc -DOTA_SUPPORT=$ota_en -DAFR_TOOLCHAIN_PATH="$GCC_DIR/bin" -DOTA_USE_EXTERNAL_FLASH=$ext_flash -S . -B build -G Ninja
            ;;
        ARM*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-armclang -DOTA_SUPPORT=$ota_en -DAFR_TOOLCHAIN_PATH="$ARMCC_DIR/bin" -DOTA_USE_EXTERNAL_FLASH=$ext_flash -S . -B build -G Ninja
            ;;
        IAR*)
            $CMAKE_DIR/cmake -DVENDOR=cypress -DBOARD=$board -DCOMPILER=arm-iar -DOTA_SUPPORT=$ota_en -DAFR_TOOLCHAIN_PATH="$IAR_DIR/bin" -DOTA_USE_EXTERNAL_FLASH=$ext_flash -S . -B build -G Ninja
            ;;
    esac
       
    $NINJA_DIR/ninja -C build
    test_error=$?
    popd
    return $test_error
}

function compile_make()
{
    local ota_en=${1:-0}
    pushd $AFR_DIR/projects/cypress/$board/mtb/aws_demos
        
    case "$toolchain" in 
        GCC_ARM*)
        make build OTA_SUPPORT=$ota_en CY_COMPILER_PATH="\"$GCC_DIR\"" TOOLCHAIN=$toolchain OTA_USE_EXTERNAL_FLASH=$ext_flash -j8
        ;;
        ARM*)
        make build OTA_SUPPORT=$ota_en CY_COMPILER_PATH="\"$ARMCC_DIR\"" TOOLCHAIN=$toolchain OTA_USE_EXTERNAL_FLASH=$ext_flash -j8
        ;;
        IAR*)
        make build OTA_SUPPORT=$ota_en CY_COMPILER_PATH="\"$IAR_DIR\"" TOOLCHAIN=$toolchain OTA_USE_EXTERNAL_FLASH=$ext_flash -j8
        ;;
    esac
    
    test_error=$?
    popd
    return $test_error
}

flash_hex_secure()
{
    hexs_folder=$1
            
    # workaround for BSP-2213
    local board_name=""
    if [[ ${board}  == "CY8CKIT_064S0S2_4343W" ]]; then
        board_name="CY8CKIT064S0S2_4343W"
    fi
    
    for i in 1 2 3 4 5:
    do
        echo -e "programming attempt: $i"
    
        echo "Erasing chip"
        pyocd erase -c -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd chip erasing failed with error code $pyocd_result"
            continue
        fi
        
        echo "######### Erasing eeprom from 0x14000000 to 0x14008000  #########"
        pyocd erase -s 0x14000000+0x8000 -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd erasing EEPROM failed with error code $pyocd_result?"
            continue
        fi
        
        echo "######### Erasing SST #########"
        pyocd erase -s 0x101c0000+0x10000 -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd erasing SST failed with error code $pyocd_result"
            continue
        fi
         
        echo "######### Flashing CM4 core #########"
        pyocd flash $hexs_folder/cm4.hex -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd flashing $hexs_folder/cm4.hex failed with error code $pyocd_result"
            continue
        fi
        
        echo "######### Flashing CM0 core #########"
        pyocd flash $hexs_folder/cm0.hex -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd flashing $hexs_folder/cm0.hex failed with error code $pyocd_result"
            continue
        fi
        
        if [ $pyocd_result == 0 ]; then
            break
        fi
    done

    test_error=$?
    return $test_error
}

flash_hex_062()
{
    hexs_folder=$1

    if [ $ext_flash == 1 ]; then 
        mcuboot_image=MCUBootApp1_6.hex
    else
        mcuboot_image=MCUBootApp1_6_internal_flash.hex
    fi
    hex_name=aws_demos.hex

    # flash mcuboot
    for i in 1 2 3 4 5:
    do
        echo -e "programming mcuboot: $i"

        echo "Erasing chip"
        pyocd erase -c -u $DEVICE_ID
        pyocd_result=$?
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd chip erasing failed with error code $pyocd_result"
            continue
        fi

        echo "program $AFR_DIR/cypress_internal_test/device_tester/$mcuboot_image"
        pyocd flash $AFR_DIR/cypress_internal_test/device_tester/$mcuboot_image -u $DEVICE_ID
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd flashing /$mcuboot_image failed with error code $pyocd_result"
            continue
        fi

        if [ $pyocd_result == 0 ]; then
            break
        fi

    done
    
    for i in 1 2 3 4 5:
    do
        echo -e "programming attempt: $i"
        
        echo "pyocd flash $hexs_folder/$hex_name -u $DEVICE_ID"
        pyocd flash $hexs_folder/$hex_name -u $DEVICE_ID
        if [ $pyocd_result != 0 ]; then 
            echo "pyocd flashing $hex_name failed with error code $pyocd_result"
            continue
        fi
        if [ $pyocd_result == 0 ]; then
            break
        fi

    done

    test_error=$?
    return $test_error
}

reset_mcu()
{
    local board_name=""
    if [[ ${board}  == "CY8CKIT_064S0S2_4343W" ]]; then
        board_name="CY8CKIT064S0S2_4343W"
    else
        board_name=$board
    fi

    echo "sw reset $DEVICE_ID"
    pyocd reset -m 'sw' -u "$DEVICE_ID"
}

function clean_up()
{
    pushd $AFR_DIR

    git checkout "vendors"
    git checkout "demos"
    git checkout "projects"
    rm -rf "$AFR_DIR/build"
    
    popd
}

function check_uart_result()
{
    local demo_name=$1

    TEST_TIMEOUT="${TEST_TIMEOUT:-1200}"
    ACCEPT_MESSAGE="${ACCEPT_MESSAGE:-Demo completed successfully.}"
    FAIL_MESSAGE="${FAIL_MESSAGE:-Error running demo.}"

    pushd $current_dir
    $python_cmd parse_serial_port.py $COM_PORT "$TEST_TIMEOUT" "$ACCEPT_MESSAGE" "$FAIL_MESSAGE"

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

function create_ota_job_with_id()
{
    hexs_folder=$1
    ota_img_name=$2
    protocol=$3
    jobid=${4:-'AFR_OTA_update'}

    echo $hexs_folder
    
    pushd $current_dir/ota/scripts

    $python_cmd start_ota.py --profile default \
                                         --name $demo_thing \
                                         --role lava-role \
                                         --s3bucket lava-bucket \
                                         --otasigningprofile lavasigningprofile \
                                         --certarn ../signer_cert/certarn.json \
                                         --updateimage $hexs_folder/${ota_img_name} \
                                         --protocol ${protocol}\
                                         --jobid ${jobid}

    test_error=$?
    popd

    return $test_error
}

function aws_delete_jobid()
{
    local jobid=$1
    echo "command: aws iot delete-job --force --job-id AFR_OTA-$jobid"
    if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then   # linux or Mac OSX
        aws iot delete-job --force --job-id AFR_OTA-$jobid &
    elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws

        cmd /C aws iot delete-job --force --job-id AFR_OTA-$jobid &
    fi
}

function update_img_ver()
{
    echo "updating image version..."
    sed -i -r "s/#define APP_VERSION_BUILD    2/#define APP_VERSION_BUILD    3/" $AFR_DIR/demos/include/aws_application_version.h
    if [[ $board == "CY8CKIT_064S0S2_4343W" ]];    then
        sed -i -r "s/#define APP_VERSION_MAJOR    0/#define APP_VERSION_MAJOR    1/" $AFR_DIR/projects/cypress/$board/mtb/aws_demos/include/aws_application_version.h
    fi
}

function run_cmake_test()
{
    local test=$1
    local ota_protocol=${2:-'MQTT'}
    local ota_image_name=${3:-'aws_demos.bin'}
    local ota_en=0 
    test_error=0
    
    if [ "$test" == "ota" ]; then
        echo -e  "***** " "CMake  " $test $ota_protocol $ota_image_name " demo test started " " *****"
    else
        echo -e  "******************** " "CMake " $test " demo test started" " ********************"
    fi

    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes $test $ota_protocol
        
    if [[ ${test}  == "ota" ]]; then
        ota_en=1
    fi
    
    compile_cmake $ota_en

    if [ $test_error -eq 0 ]
    then
        if [[ $board == "CY8CKIT_064S0S2_4343W" ]];
        then
            flash_hex_secure "$AFR_DIR/build"
        else
            flash_hex_062 "$AFR_DIR/build"
        fi  
    fi

    if [ "$test" == "ota" ]; then
        jobid=$board-update-$((1 + RANDOM % 10000000))
        if [ $test_error -eq 0 ]
        then 
            echo "OTA protocol: " $ota_protocol
            echo "prepare image for OTA"
            update_img_ver

            rm -rf $AFR_DIR/build
            compile_cmake $ota_en

            image_name=$(echo $board"_"$ota_image_name)
            cp $AFR_DIR/build/$ota_image_name $AFR_DIR/build/$image_name
            create_ota_job_with_id "$AFR_DIR/build" $image_name $ota_protocol $jobid
        fi
        
        echo "reset the $board after the 2nd build"
        reset_mcu
        sleep 1
    fi

    if [ $test_error -eq 0 ]
    then    
        if [[ ("$test" == "ota") && ("$ota_protocol" == "MQTT" || "$ota_protocol" == "HTTP") ]]; then
            check_uart_result "cmake $test $ota_protocol $ota_image_name"       
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
    
    if [ "$test" == "ota" ]; then
        aws_delete_jobid $jobid
        echo -e  "***** " "CMake  " $test $ota_protocol $ota_image_name " demo test finished " " *****"
    else
        echo -e  "******************** " "CMake " $test " demo test finished" " ********************"
    fi
}

function run_make_test()
{
    local test=$1
    local ota_protocol=${2:-'MQTT'}
    local ota_image_name=${3:-'aws_demos.bin'}
    local ota_en=0
    test_error=0
    
    if [ "$test" == "ota" ]; then
        echo -e  "***** " "Make  " $test $ota_protocol $ota_image_name " demo test started " " *****"
    else
        echo -e  "******************** " "Make " $test " demo test started" " ********************"
    fi
    
    source "$current_dir/$test/setup_test.sh"

    pushd $current_dir

    setup_test $board
    apply_common_changes $test $ota_protocol
    
    if [[ ${test}  == "ota" ]]; then
        ota_en=1
    fi
    compile_make $ota_en

    if [ $test_error -eq 0 ]
    then
        if [[ $board == "CY8CKIT_064S0S2_4343W" ]];
        then
            board_name=$board
            flash_hex_secure "$AFR_DIR/build/cy/aws_demos/$board_name/Debug"
        else
            board_name=$(echo $board | sed s/_/-/g)
            flash_hex_062 "$AFR_DIR/build/cy/aws_demos/$board_name/Debug"
        fi
    fi
    
    if [ "$test" == "ota" ]; then
        jobid=$board-update-$((1 + RANDOM % 10000000))
        
        if [ $test_error -eq 0 ]
        then 
            echo "OTA protocol: " $ota_protocol
            echo "prepare image for OTA"
            update_img_ver
            
            rm -rf $AFR_DIR/build
            compile_make $ota_en

            image_name=$(echo $board"_"$ota_image_name)
            cp $AFR_DIR/build/cy/aws_demos/$board_name/Debug/$ota_image_name  $AFR_DIR/build/cy/aws_demos/$board_name/Debug/$image_name
            create_ota_job_with_id "$AFR_DIR/build/cy/aws_demos/$board_name/Debug" $image_name $ota_protocol $jobid

            echo "create job: $jobid" 
        fi
        
        echo "reset the $board after the 2nd build"
        reset_mcu
        sleep 1
    fi

    if [ $test_error -eq 0 ]
    then
        if [[ ("$test" == "ota") && ("$ota_protocol" == "MQTT" || "$ota_protocol" == "HTTP") ]]; then
            check_uart_result "make $test $ota_protocol $ota_image_name"
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
    
    if [ "$test" == "ota" ]; then
        aws_delete_jobid $jobid
        echo -e  "***** " "Make  " $test $ota_protocol $ota_image_name " demo test finished " " *****"
    else
        echo -e  "******************** " "Make " $test " demo test finishe" " ********************"
    fi
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
    local test=$1
    local ota_protocol=${2:-'MQTT'}
    
    if [ "$test" == "ota" ]; then
        cp -f $AFR_DIR/cypress_internal_test/demo_test/ota/aws_ota_codesigner_certificate.h $AFR_DIR/demos/include/aws_ota_codesigner_certificate.h
    fi
    
    if [ "$test" != "mar" ]; then
        cp -f aws_clientcredential_keys.h $AFR_DIR/demos/include/aws_clientcredential_keys.h
    fi
    
    cp -f aws_clientcredential.h $AFR_DIR/demos/include/aws_clientcredential.h
    
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialIOT_THING_NAME \"$demo_thing\"
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_SSID \"$DEVICE_TESTER_WIFI_SSID\"
    replace_define $AFR_DIR/demos/include/aws_clientcredential.h clientcredentialWIFI_PASSWORD \"$DEVICE_TESTER_WIFI_PASSWORD\"

    if [[ $ota_protocol == *HTTP* ]]; then
        echo "HTTP as primary data protocol" 
        replace_define $AFR_DIR/vendors/cypress/boards/$board/aws_demos/config_files/aws_ota_agent_config.h configOTA_PRIMARY_DATA_PROTOCOL "( OTA_DATA_OVER_HTTP )"
    fi
}

set +e

rm -rf $AFR_DIR/build

if [[ $ota_only == 1 ]]; 
then
    if [[ $board == "CY8CKIT_064S0S2_4343W" ]];
    then
        # Cmake build flow test cases
        run_cmake_test ota MQTT aws_demos_cm4.bin
        run_cmake_test ota HTTP aws_demos_cm4.bin
        run_cmake_test ota HTTP cm4_only_signed.tar
        run_cmake_test ota HTTP cm4_cm0_signed.tar

        # Make build flow test cases
        run_make_test ota MQTT cm4.bin
        run_make_test ota HTTP cm4.bin
        run_make_test ota HTTP cm4_only_signed.tar
        run_make_test ota HTTP cm4_cm0_signed.tar
    else
        # Make build flow test cases on 062 boards
        run_make_test ota MQTT aws_demos.bin
        run_make_test ota HTTP aws_demos.bin
        run_make_test ota HTTP aws_demos.tar

        # Cmake build flow test cases on 062 boards
        run_cmake_test ota MQTT aws_demos.bin
        run_cmake_test ota HTTP aws_demos.bin
        run_cmake_test ota HTTP aws_demos.tar
    fi
else
    # Run other demos 
    run_cmake_test tcp_secure_echo
    run_cmake_test mqtt
    run_cmake_test shadow
    run_cmake_test defender
    run_cmake_test ota MQTT cm4_upgrade.bin
    run_cmake_test ota HTTP cm4_upgrade.bin
    if [[ $board == "CY8CKIT_064S0S2_4343W" ]]; then
        run_cmake_test mar
    fi

    run_make_test tcp_secure_echo
    run_make_test mqtt
    run_make_test shadow
    run_make_test defender
    run_make_test ota MQTT cm4.bin
    run_make_test ota HTTP cm4.bin
    if [[ $board == "CY8CKIT_064S0S2_4343W" ]]; then
        run_make_test mar
    fi
fi

echo "---------------------------------------------------------------------"
cat $result_file
cat $result_file >$AFR_DIR/cypress_internal_test/demo_result_file.txt 
rm -f $result_file

$python_cmd $AFR_DIR/cypress_internal_test/demo_test/convert_results.py $board $AFR_DIR/cypress_internal_test/demo_result_file.txt

if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin" ]]; then
    deactivate
fi

exit $ERRORS
