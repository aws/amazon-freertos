#!/bin/bash
#
# This file is used by make to create the build commands to sign an OTA image
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
CY_ELF_TO_HEX=$1
shift
CY_ELF_TO_HEX_OPTIONS=$1
shift
CY_ELF_TO_HEX_FILE_ORDER=$1
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
CY_SIGNING_KEY_ARG=$1


# Export these values for python3 click module
export LC_ALL=C.UTF-8
export LANG=C.UTF-8
    
CY_OUTPUT_BIN=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.bin
CY_OUTPUT_ELF=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.elf
CY_OUTPUT_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.hex
CY_OUTPUT_FILE_NAME_BIN=$CY_OUTPUT_NAME.bin
CY_OUTPUT_FILE_NAME_TAR=$CY_OUTPUT_NAME.tar
CY_OUTPUT_SIGNED_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.hex
CY_OUTPUT_FILE_PATH_WILD=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.*

CY_COMPONENTS_JSON_NAME=components.json

#
# For elf -> hex conversion
#
if [ "$CY_ELF_TO_HEX_FILE_ORDER" == "elf_first" ]
then
    CY_ELF_TO_HEX_FILE_1=$CY_OUTPUT_ELF
    CY_ELF_TO_HEX_FILE_2=$CY_OUTPUT_HEX
else
    CY_ELF_TO_HEX_FILE_1=$CY_OUTPUT_HEX
    CY_ELF_TO_HEX_FILE_2=$CY_OUTPUT_ELF
fi
#
# Leave here for debugging
#echo " CY_OUTPUT_NAME           $CY_OUTPUT_NAME"
#echo " CY_OUTPUT_BIN            $CY_OUTPUT_BIN"
#echo " CY_OUTPUT_ELF            $CY_OUTPUT_ELF"
#echo " CY_OUTPUT_HEX            $CY_OUTPUT_HEX"
#echo " CY_OUTPUT_SIGNED_HEX     $CY_OUTPUT_SIGNED_HEX"
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

echo "Create  $CY_OUTPUT_HEX"
"$CY_ELF_TO_HEX" $CY_ELF_TO_HEX_OPTIONS $CY_ELF_TO_HEX_FILE_1 $CY_ELF_TO_HEX_FILE_2

echo  "$IMGTOOL_COMMAND_ARG Hex, creating bin."
cd $MCUBOOT_SCRIPT_FILE_DIR
echo "$IMGTOOL_SCRIPT_NAME $IMGTOOL_COMMAND_ARG $FLASH_ERASE_ARG -e little --pad-header --align 8 -H $MCUBOOT_HEADER_SIZE -M $MCUBOOT_MAX_IMG_SECTORS -v $CY_BUILD_VERSION -L $CY_BOOT_PRIMARY_1_START -S $CY_BOOT_PRIMARY_1_SIZE $CY_SIGNING_KEY_ARG $CY_OUTPUT_HEX $CY_OUTPUT_SIGNED_HEX"
python3 $IMGTOOL_SCRIPT_NAME $IMGTOOL_COMMAND_ARG $FLASH_ERASE_ARG -e little --pad-header --align 8 -H $MCUBOOT_HEADER_SIZE -M $MCUBOOT_MAX_IMG_SECTORS -v $CY_BUILD_VERSION -L $CY_BOOT_PRIMARY_1_START -S $CY_BOOT_PRIMARY_1_SIZE $CY_SIGNING_KEY_ARG $CY_OUTPUT_HEX $CY_OUTPUT_SIGNED_HEX

# back to our build directory
cd $CY_OUTPUT_PATH

#
# Convert signed hex file to Binary for AWS uploading
"$CY_ELF_TO_HEX" $CY_ELF_TO_HEX_OPTIONS $CY_OUTPUT_SIGNED_HEX $CY_OUTPUT_BIN
echo  " Done."

# get size of binary file for components.json
BIN_SIZE=$(ls -l $CY_OUTPUT_BIN | awk '{printf $6}')

# create "components.json" file
echo "{\"numberOfComponents\":\"2\",\"version\":\"$CY_BUILD_VERSION\",\"files\":["                    >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                             >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_FILE_NAME_BIN\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME

# create tarball for OTA
tar -cf $CY_OUTPUT_FILE_NAME_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_FILE_NAME_BIN


echo ""
echo "Application Name                         : $CY_OUTPUT_NAME"
echo "Application Version                      : $CY_BUILD_VERSION"
echo "Primary 1 Slot Start                     : $CY_BOOT_PRIMARY_1_START"
echo "Primary 1 Slot Size                      : $CY_BOOT_PRIMARY_1_SIZE"
echo "FLASH ERASE Value (NOTE: Empty for 0xff) : $FLASH_ERASE_VALUE"
echo "Cypress MCUBoot Header size              : $MCUBOOT_HEADER_SIZE"
echo "Max 512 bytes sectors for Application    : $MCUBOOT_MAX_IMG_SECTORS"
if [ "$SIGNING_KEY_PATH" != "" ]
then
    echo "Signing key: $SIGNING_KEY_PATH"
fi
echo ""
#
ls -l $CY_OUTPUT_FILE_PATH_WILD
echo ""

echo "$CY_OUTPUT_FILE_NAME_TAR File List"
# print tar file list
tar -t -f $CY_OUTPUT_FILE_NAME_TAR

echo ""
