# These Amazon FreeRTOS related global variables are available to use.
#
# AFR_ROOT_DIR                  Amazon FreeRTOS source root.
# AFR_KERNEL_DIR                FreeRTOS kernel root.
# AFR_MODULES_DIR               Amazon FreeRTOS modules root.
# AFR_MODULES_C_SDK_DIR         C-SDK libraries root.
# AFR_MODULES_FREERTOS_PLUS_DIR FreeRTOS-Plus libraries root.
# AFR_MODULES_ABSTRACTIONS_DIR  Abstractions layers root.
# AFR_DEMOS_DIR                 Amazon FreeRTOS demos root.
# AFR_TESTS_DIR                 Amazon FreeRTOS common tests and framework root.
# AFR_VENDORS_DIR               vendors content root.
# AFR_3RDPARTY_DIR              3rdparty libraries root.

# AFR_VENDOR_NAME           Folder name for vendor.
# AFR_BOARD_NAME            Folder name for this board.

# AFR_TOOLCHAIN             Compiler chosen by the user. Should be one of
#                           the file names under ${AFR_ROOT_DIR}/tools/cmake/toolchains
# AFR_IS_TESTING            1 if testing enabled, otherwise, 0.

# You may also use these 2 functions we defined to glob files when needed. However, we recommend
# to specify your source files explicitly to avoid unexpected behavior unless you're 100% sure.
# CMake reference link: https://cmake.org/cmake/help/latest/command/file.html#filesystem
# afr_glob_files(<out_var> [RECURSE] <DIRECTORY> <directory> [<GLOBS> <glob-expressions>...])
# afr_glob_src(<out_var> [RECURSE] <DIRECTORY> <directory> [<EXTENSIONS> <file-extensions>...])

# If you don't specify GLOBS or EXTENSIONS parameters,
# afr_glob_files: glob all files including hidden files in the specified directory.
# afr_glob_src:   glob all files ending with either .c, .h, .s or .asm

#--------------------------------------------------------------------
# Cypress Defines
#
include("${AFR_VENDORS_DIR}/cypress/MTB/psoc6/cmake/cy_defines.cmake")
include("${cy_psoc6_dir}/cmake/cy_create_linked_target.cmake")
include("${cy_psoc6_dir}/cmake/cy_create_exe_target.cmake")

#--------------------------------------------------------------------
# Default linker script for this board
#  (modified for OTA builds)
#
set(ENV{LINKER_SCRIPT_NAME} "cyb06xxa_cm4_dual")

#--------------------------------------------------------------------
# Board defines
#

# These must be in parent scope so the rest of the cmake flow sees them
set(ENV{CY_TFM_HEX} "${cy_psoc6_dir}/psoc64tfm/COMPONENT_TFM_S_FW/tfm_s_unsigned.hex")
if (CY_TFM_POLICY_VER_ES100)
set(ENV{CY_TFM_POLICY_FILE} "${cy_psoc6_dir}/psoc64tfm/security/policy/policy_multi_CM0_CM4_jitp_es100.json")
else()
set(ENV{CY_TFM_POLICY_FILE} "${cy_psoc6_dir}/psoc64tfm/security/policy/policy_multi_CM0_CM4_jitp.json")
endif()
set(ENV{CY_DEVICE_NAME} "cy8ckit-064s0s2-4343w")

#Features
# CY_TFM_PSA_SUPPORTED feature is required on this board.
set(CY_TFM_PSA_SUPPORTED        "1" )
# Enable BLE
if (NOT DEFINED BLE_SUPPORTED)
set(BLE_SUPPORTED               "1" )
endif()

# Python path definition
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
find_program(CY_PYTHON_PATH "python")
else()
find_program(CY_PYTHON_PATH "python3")
endif()

#--------------------------------------------------------------------
# PKCS11 library defines
# Only build pkcs11_psa library for this platform
set(AFR_PKCS11_PSA TRUE)

#--------------------------------------------------------------------
# Defender metrics collection using lwIP is supported on this platform
set(DEFENDER_METRICS_LWIP_SUPPORTED TRUE CACHE INTERNAL "LWIP Defender Metrics supported")

#--------------------------------------------------------------------
# OTA SUPPORT is always used for this platform
#
# NOTES:
#  - These are board-specific values based on Internal / External FLASH sizes
#  - These defines MUST match the MCUBootAPP build ***EXACTLY***
#
#   MCUBOOT_BOOTLOADER_SIZE
#   CY_BOOT_PRIMARY_1_START
#   CY_BOOT_PRIMARY_1_SIZE
#   CY_BOOT_SCRATCH_SIZE
#   MCUBOOT_MAX_IMG_SECTORS
#
# TODO: new 064 memory map (note: No secure secondary yet...)
# 0x10000000 - 0x10040000 Secure Primary
# 0x10040000 - 0x10100000 Nonsecure Primary
# 0x10100000 - 0x101C0000 Nonsecure Secondary
# 0x101C0000 - 0x10200000 Reserved
#

if(OTA_SUPPORT)
    # Set for new Cy Bootloader SWAP_WITH_STATUS support
    set(CY_MCUBOOT_SWAP_USING_STATUS             "1")
    set(ENV{CY_MCUBOOT_SWAP_USING_STATUS}        "1")

    set(ENV{MCUBOOT_HEADER_SIZE}         "0x400"      )       # Must be a multiple of 1024
    set(ENV{MCUBOOT_MAX_IMG_SECTORS}     "2000"       )
    set(ENV{CY_BOOT_SCRATCH_SIZE}        "0x00001000" )

    set(ENV{CY_BOOT_PRIMARY_2_START}     "0x00000000" )
    set(ENV{CY_BOOT_PRIMARY_2_SIZE}      "0x00050000" )

    if(CY_MCUBOOT_SWAP_USING_STATUS)
        # New SWAP_USING_STATUS bootloader
        set(ENV{MCUBOOT_BOOTLOADER_SIZE}     "0x00050000" )
        set(ENV{CY_BOOT_PRIMARY_1_START}     "0x00050000" )
        set(ENV{CY_BOOT_PRIMARY_1_SIZE}      "0x0011C000" )
        set(ENV{CY_BOOT_SECONDARY_1_START}   "0x00024400" ) # offset from start of external FLASH 18000000
        set(ENV{CY_BOOT_SECONDARY_2_START}   "0x0016C000" ) # offset from CY_FLASH_BASE
    else()
        set(ENV{MCUBOOT_BOOTLOADER_SIZE}     "0x00040000" )
        set(ENV{CY_BOOT_PRIMARY_1_START}     "0x00050000" )
        set(ENV{CY_BOOT_PRIMARY_1_SIZE}      "0x00120000" )
        set(ENV{CY_BOOT_SECONDARY_1_START}   "0x18000000" ) # start of external FLASH
        set(ENV{CY_BOOT_SECONDARY_2_START}   "0x00170000" )
    endif()

    set(ENV{MCUBOOT_IMAGE_NUMBER}        "2")            # 2 images (primary_1 and primary_2)

    # Default to internal FLASH value for MCUBoot header signing
    # Note that after cy_bootloader changes to support 0xFF erase values
    # for different FLASH, this may	 need to change
    set(ENV{CY_BOOT_USE_EXTERNAL_FLASH}     "1" )
    set(ENV{CY_FLASH_ERASE_VALUE}           "1" )

endif()

# -------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
# Provide metadata for listing on Amazon FreeRTOS console.
afr_set_board_metadata(ID "${AFR_BOARD_NAME}")
afr_set_board_metadata(IS_ACTIVE true)
afr_set_board_metadata(NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DISPLAY_NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DESCRIPTION "PSoC 64 Wi-Fi BT Pioneer Kit")
afr_set_board_metadata(VENDOR_NAME "Cypress")
afr_set_board_metadata(FAMILY_NAME "PSoC 6")
afr_set_board_metadata(DATA_RAM_MEMORY "1024KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(CODE_SIGNER "null")
afr_set_board_metadata(SUPPORTED_IDE "CMakeBuildSystem;ModusToolbox")
afr_set_board_metadata(RECOMMENDED_IDE "CMakeBuildSystem")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")
# settings for CmakeBuildSystem as IDE
afr_set_board_metadata(IDE_CMakeBuildSystem_NAME "CMakeBuildSystem")
afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "GCC")
if("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
    afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "ARM")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "IAR")
endif()
afr_set_board_metadata(IDE_CMakeBuildSystem_PROJECT_LOCATION "null")
# settings for ModusToolbox as IDE
afr_set_board_metadata(IDE_ModusToolbox_NAME "Eclipse IDE for ModusToolbox")
afr_set_board_metadata(IDE_ModusToolbox_COMPILER "GCC")
if("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
    afr_set_board_metadata(IDE_ModusToolbox_COMPILER "ARM")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    afr_set_board_metadata(IDE_ModusToolbox_COMPILER "IAR")
endif()
# demo project directory location
afr_set_board_metadata(IDE_ModusToolbox_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/cypress/${AFR_BOARD_NAME}/mtb/aws_demos")
# demo project configuration file location
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

cy_add_link_libraries(
    DEVICE "CYB0644ABZI-S2D44"
    PORT_DIR "${cy_board_dir}/ports"
    BSP_DIR "${app_dir}/application_code/cy_code"
    CONFIG_DIRS "${include_dirs}"
    COMPONENTS "4343W;CAPSENSE;TFM_NS_FREERTOS_WRAPPER;TFM_NS_INTERFACE"
    DEFINES "CY_USE_LWIP;CY_TFM_PSA_SUPPORTED;TFM_MULTI_CORE_MULTI_CLIENT_CALL"
)

# TODO: remove this once whd is updated to new version
target_include_directories(psoc6_core INTERFACE "${cy_libraries_whd_dir}/WiFi_Host_Driver/resources/nvram/TARGET_CY8CKIT_064B0S2_4343W")

if (NOT DEFINED LIB_MODE)
    cy_create_exe_target(
        EXE_APP_NAME ""
        SOURCE_DIRS "${app_dir}/application_code"
    )
endif()
