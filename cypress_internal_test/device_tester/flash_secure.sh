#!/bin/bash

SOURCE_DIR=$1
DEVICE_ID=$2
BOARD=$3

if [[ "$OSTYPE" == "linux-gnu"* || "$OSTYPE" == "darwin"* ]]; then   # linux or Mac OSX
    python_cmd="python3"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" ]]; then # Winodws
    python_cmd="python"
fi

#flush serial port to prevent UART from test to leak into another
echo "Flushing serial port"
script_dir=$(dirname "${BASH_SOURCE[0]}")
$python_cmd $script_dir/flush_serial.py $BOARD

pushd ${SOURCE_DIR}/build 

for i in 1 2 3 4 5:
do
    echo -e "programming attempt: $i"
    
    echo "Erasing chip"
    pyocd erase -c -u $DEVICE_ID
    pyocd_result=$?
    if [ $pyocd_result != 0 ]; then 
        echo "pyocd chip erasing failed with error code $pyocd_result "
        continue
    fi
    
    echo "######### Erasing eeprom from 0x14000000 to 0x14008000  #########"
    #pyocd erase -t cy8c64xA_cm0 -s 0x14000000+0x8000 -u $DEVICE_ID
    pyocd erase -s 0x14000000+0x8000 -u $DEVICE_ID
    pyocd_result=$?
    if [ $pyocd_result != 0 ]; then 
        echo "pyocd erasing EEPROM failed with error code $pyocd_result "
        continue
    fi
    
    echo "######### Erasing SST #########"
    #pyocd erase -t cy8c64xA_cm0 -s 0x101c0000+0x10000 -u $DEVICE_ID
    pyocd erase -s 0x101c0000+0x10000 -u $DEVICE_ID
    pyocd_result=$?
    if [ $pyocd_result != 0 ]; then 
        echo "pyocd erasing SST failed with error code $pyocd_result "
        continue
    fi
     
    echo "######### Flashing CM4 core #########"
    pyocd flash cm4.hex -u $DEVICE_ID
    #pyocd flash -t cy8c64xA_cm0 cm4.hex -u $DEVICE_ID
    pyocd_result=$?
    if [ $pyocd_result != 0 ]; then 
        echo "pyocd flashing cm4.hex failed with error code $pyocd_result "
        continue
    fi
    
    echo "######### Flashing CM0 core #########"
    pyocd flash cm0.hex -u $DEVICE_ID
    #pyocd flash -t cy8c64xA_cm0 cm0.hex -u $DEVICE_ID
    pyocd_result=$?
    if [ $pyocd_result != 0 ]; then 
        echo "pyocd flashing cm0.hex failed with error code $pyocd_result "
        continue
    fi
    
    if [ $pyocd_result == 0 ]; then
        break
    fi
done
popd
echo "######### Done! #########"