#!/bin/bash

if [ -z $1 ]; then
    SOURCE_DIR="."
else
    SOURCE_DIR=$1
fi

device_id='122317F302098400'
CM0_IMG=${SOURCE_DIR}/build/cm0.hex
CM4_IMG=${SOURCE_DIR}/build/cm4.hex

source $SOURCE_DIR/cypress_internal_test/setenv.sh
run_setenv

echo " "
echo "######### Erasing eeprom from 0x14000000 to 0x14008000  #########"
echo " "

${OPENOCD_DIR}/bin/openocd \
        -s ${OPENOCD_DIR}/scripts \
        -f interface/kitprog3.cfg \
        -c "set TARGET_AP cm4_ap" \
        -f target/psoc6_2m_secure.cfg \
        -c "cmsis_dap_serial ${device_id}" \
        -c "init; reset init" \
        -c "flash erase_address 0x14000000 0x8000" \
        -c "exit"

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
        -c "flash erase_address 0x101c0000 0x10000" \
        -c "flash write_image erase ${CM0_IMG}" \
        -c "flash write_image erase ${CM4_IMG}" \
        -c "resume; reset; exit"

echo "######### Done! #########"