#!/bin/bash

set -u

DEVICE_ID=$1
FILE_PATH=$2
BOARD=$3
OTA=$4

# Convert to hex first; pyocd misinterprets vaddr/paddr
if [ $OTA = "OTA_NO" ]; then
    echo "Convert $FILE_PATH.elf to $FILE_PATH.hex"
    arm-none-eabi-objcopy -O ihex $FILE_PATH.elf $FILE_PATH.hex
fi

echo "Erasing chip"
pyocd erase -c -u $DEVICE_ID

#flush serial port to prevent UART from test to leak into another
if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "darwin"* ]]; then   # linux or Mac OSX
    python_cmd="python3"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws
    python_cmd="python"
fi

echo "Flushing serial port"
script_dir=$(dirname "${BASH_SOURCE[0]}")
$python_cmd $script_dir/flush_serial.py $BOARD

if [ $OTA = "OTA_YES" ]; then
    file_ext="signed.hex"       # for the CM4 app
    if [ "$OTA_USE_EXTERNAL_FLASH" = "0" ]; then
        mcuboot_image=MCUBootApp1_6_internal_flash.hex
    else
        mcuboot_image=MCUBootApp1_6.hex
    fi    
else
    file_ext="hex"
fi

for i in 1 2 3 4 5
do
    if [ $OTA = "OTA_YES" ]; then
        echo "Program mcuboot image: pyocd flash -u $DEVICE_ID $script_dir/$mcuboot_image"
        pyocd flash -u $DEVICE_ID $script_dir/$mcuboot_image
        pyocd_result=$?
        if [ $pyocd_result != 0 ]
        then
            echo "Failed to program MCU_boot $?"
            continue
        fi
    fi

    echo "Program test image: pyocd flash -u $DEVICE_ID $FILE_PATH.$file_ext"
    pyocd flash -u $DEVICE_ID $FILE_PATH.$file_ext
    pyocd_result=$?
    if [ $pyocd_result == 0 ]
    then
        break
    else
        echo "pyocd failed with error code $?"
    fi
done