#!/bin/bash
#
# This file is used by Make to create a tarball for OTA multi-image update
#
# Modify at your peril !
#
# The output files will be in the same directory as the .elf, .hex, etc files
#
(set -o igncr) 2>/dev/null && set -o igncr; # this comment is required
set -e

# Arguments
# We have a lot
#
CY_OUTPUT_PATH=$1
shift
CY_OUTPUT_NAME=$1
shift
CY_OBJ_COPY=$1
shift
MCUBOOT_SCRIPT_FILE_DIR=$1
shift
IMGTOOL_SCRIPT_NAME=$1
shift
IMGTOOL_COMMAND_ARG=$1
shift
FLASH_ERASE_VALUE=$1
shift
MCUBOOT_HEADER_SIZE=$1
shift
MCUBOOT_MAX_IMG_SECTORS=$1
shift
CY_BUILD_VERSION=$1
shift
CY_BOOT_PRIMARY_1_START=$1
shift
CY_BOOT_PRIMARY_1_SIZE=$1
shift
SIGNING_KEY_PATH=$1
shift
CY_SIGNING_KEY_ARG=$1

# Export these values for python3 click module
export LC_ALL=C.UTF-8
export LANG=C.UTF-8

#CY_OUTPUT_BIN=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.bin
#CY_OUTPUT_ELF=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.elf
#CY_OUTPUT_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.hex
#CY_OUTPUT_FILE_NAME_BIN=$CY_OUTPUT_NAME.bin
#CY_OUTPUT_FILE_NAME_TAR=$CY_OUTPUT_NAME.tar
#CY_OUTPUT_SIGNED_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.hex
CY_OUTPUT_FILE_PATH_WILD=$CY_OUTPUT_PATH/c*.*

CY_OUTPUT_FILE_ELF=$CY_OUTPUT_NAME.elf

CY_OUTPUT_FILE_CM0_NAME_HEX=cm0.hex
CY_OUTPUT_FILE_CM0_NAME_BIN=cm0.bin
CY_OUTPUT_FILE_CM4_NAME_HEX=cm4.hex
CY_OUTPUT_FILE_CM4_NAME_BIN=cm4.bin
CY_OUTPUT_FILE_CM0_UPGRADE_HEX=cm0_upgrade.hex
CY_OUTPUT_FILE_CM0_UPGRADE_BIN=cm0_upgrade.bin
CY_OUTPUT_FILE_CM4_UPGRADE_HEX=cm4_upgrade.hex
CY_OUTPUT_FILE_CM4_UPGRADE_BIN=cm4_upgrade.bin

CY_CM4_ONLY_TAR=cm4_only_signed.tar
CY_CM4_CM0_TAR=cm4_cm0_signed.tar

CY_CM4_ONLY_UPGRADE_TAR=cm4_only_upgrade.tar
CY_CM4_CM0_UPGRADE_TAR=cm4_cm0_upgrade.tar

CY_COMPONENTS_JSON_NAME=components.json

#
# Leave here for debugging
#echo " CY_OUTPUT_NAME           $CY_OUTPUT_NAME"
#echo " CY_OUTPUT_FILE_CM0_NAME_BIN      $CY_OUTPUT_FILE_CM0_NAME_BIN"
#echo " CY_OUTPUT_FILE_CM4_NAME_BIN      $CY_OUTPUT_FILE_CM4_NAME_BIN"
#echo " CY_OUTPUT_FILE_CM0_UPGRADE_BIN   $CY_OUTPUT_FILE_CM0_UPGRADE_BIN"
#echo " CY_OUTPUT_FILE_CM4_UPGRADE_BIN   $CY_OUTPUT_FILE_CM4_UPGRADE_BIN"
#echo " CY_CM4_ONLY_TAR          $CY_CM4_ONLY_TAR"
#echo " CY_CM4_CM0_TAR           $CY_CM4_CM0_TAR"
#echo " CY_CM4_ONLY_UPGRADE_TAR  $CY_CM4_ONLY_UPGRADE_TAR"
#echo " CY_CM4_CM0_UPGRADE_TAR   $CY_CM4_CM0_UPGRADE_TAR"
#echo " MCUBOOT_SCRIPT_FILE_DIR  $MCUBOOT_SCRIPT_FILE_DIR"
#echo " IMGTOOL_SCRIPT_NAME      $IMGTOOL_SCRIPT_NAME"
#echo " FLASH_ERASE_VALUE        $FLASH_ERASE_VALUE"
#echo " MCUBOOT_HEADER_SIZE      $MCUBOOT_HEADER_SIZE"
#echo " MCUBOOT_MAX_IMG_SECTORS  $MCUBOOT_MAX_IMG_SECTORS"
#echo " CY_BUILD_VERSION         $CY_BUILD_VERSION"
#echo " CY_BOOT_PRIMARY_1_START  $CY_BOOT_PRIMARY_1_START"
#echo " CY_BOOT_PRIMARY_1_SIZE   $CY_BOOT_PRIMARY_1_SIZE"
#echo " SIGNING_KEY_PATH         $SIGNING_KEY_PATH"
#
# For FLASH_ERASE_VALUE
# If value is 0x00, we need to specify "-R 0"
# If value is 0xFF, we do not specify anything!
#
FLASH_ERASE_ARG=
if [ $FLASH_ERASE_VALUE -eq 0 ]
then 
FLASH_ERASE_ARG="-R 0"
fi

# change dir to output dir
cd $CY_OUTPUT_PATH

#
# Do final hex -> bin conversion for use in tarballs

# signed.hex -> signed.bin
"$CY_OBJ_COPY" --input-target=ihex --output-target=binary $CY_OUTPUT_FILE_CM0_NAME_HEX $CY_OUTPUT_FILE_CM0_NAME_BIN
"$CY_OBJ_COPY" --input-target=ihex --output-target=binary $CY_OUTPUT_FILE_CM4_NAME_HEX $CY_OUTPUT_FILE_CM4_NAME_BIN

# upgrade.hex -> upgrade.bin
"$CY_OBJ_COPY" --input-target=ihex --output-target=binary $CY_OUTPUT_FILE_CM0_UPGRADE_HEX $CY_OUTPUT_FILE_CM0_UPGRADE_BIN
"$CY_OBJ_COPY" --input-target=ihex --output-target=binary $CY_OUTPUT_FILE_CM4_UPGRADE_HEX $CY_OUTPUT_FILE_CM4_UPGRADE_BIN

#
# Create a few different tarballs for testing different update types
#

#==============================================================================
# "Normal" OTA tarballs (binaries DO NOT include cybootloader magic, etc)
# get size of binary file for components.json
BIN_CM0_SIZE=$(ls -l $CY_OUTPUT_FILE_CM0_NAME_BIN | awk '{printf $6}')
BIN_CM4_SIZE=$(ls -l $CY_OUTPUT_FILE_CM4_NAME_BIN | awk '{printf $6}')

#---------------------------------------
# CM4-only "normal" OTA Image tarball
#
# create "components.json" file for cm4-only OTA tarball
echo "{\"numberOfComponents\":\"2\",\"version\":\"$CY_BUILD_VERSION\",\"files\":["                             >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                                    >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM4_NAME_BIN\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_CM4_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME
#
# create tarball for OTA-cm4-only
tar -cf $CY_CM4_ONLY_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_FILE_CM4_NAME_BIN


#---------------------------------------
# CM0 and CM4 "normal" OTA Image tarball
#
# create "components.json" file for cm4-only OTA tarball
echo "{\"numberOfComponents\":\"3\",\"version\":\"$CY_BUILD_VERSION\",\"files\":["                             >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                                    >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM0_NAME_BIN\",\"fileType\": \"SPE\",\"fileSize\":\"$BIN_CM0_SIZE\"},"   >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM4_NAME_BIN\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_CM4_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME
#
# create tarball for CM0 & CM4 OTA
tar -cf $CY_CM4_CM0_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_FILE_CM0_NAME_BIN $CY_OUTPUT_FILE_CM4_NAME_BIN

#==============================================================================
# "upgrade" OTA tarballs (includes cybootloader magic, etc)
# get size of binary file for components.json for "upgrade" files
BIN_CM0_UPGRADE_SIZE=$(ls -l "$CY_OUTPUT_FILE_CM0_UPGRADE_BIN" | awk '{printf $6}')
BIN_CM4_UPGRADE_SIZE=$(ls -l "$CY_OUTPUT_FILE_CM4_UPGRADE_BIN" | awk '{printf $6}')

#---------------------------------------
# CM4-only "upgrade" OTA Image tarball
#
# create "components.json" file for cm4-only OTA tarball
echo "{\"numberOfComponents\":\"2\",\"version\":\"$CY_BUILD_VERSION\",\"files\":["                             >  $CY_COMPONENTS_JSON_NAME@
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                                     >> $CY_COMPONENTS_JSON_NAME@
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM4_UPGRADE_BIN\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_CM4_UPGRADE_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME@

# create tarball for "update" OTA-cm4-only tarball
tar -cf $CY_CM4_ONLY_UPGRADE_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_FILE_CM4_UPGRADE_BIN

#---------------------------------------
# CM0 and CM4 "upgrade" OTA Image tarball
#
# create "components.json" file for cm4-only OTA tarball
echo "{\"numberOfComponents\":\"3\",\"version\":\"$CY_BUILD_VERSION\",\"files\":["                             >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                                     >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM0_UPGRADE_BIN\",\"fileType\": \"SPE\",\"fileSize\":\"$BIN_CM0_UPGRADE_SIZE\"}," >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_CM4_UPGRADE_BIN\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_CM4_UPGRADE_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME
#
# create tarball for CM0 & CM4 OTA
tar -cf $CY_CM4_CM0_UPGRADE_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_FILE_CM0_UPGRADE_BIN $CY_OUTPUT_FILE_CM4_UPGRADE_BIN

echo ""

ls -l *.bin

echo ""

ls -l *.tar

echo "Done"
