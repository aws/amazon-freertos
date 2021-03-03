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
include("${cy_psoc6_cmake_dir}/cy_create_linked_target.cmake")
include("${cy_psoc6_cmake_dir}/cy_create_exe_target.cmake")

#--------------------------------------------------------------------
# Default linker script for this board
#  (modified for OTA builds)
#
set(ENV{LINKER_SCRIPT_NAME} "cy8c6xxa_cm4_dual")

#--------------------------------------------------------------------
# Features
# Enable BLE
if (NOT DEFINED BLE_SUPPORTED)
set(BLE_SUPPORTED               "1" )
endif()

if(OTA_SUPPORT)
    #--------------------------------------------------------------------
    # OTA SUPPORT
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
    set(ENV{MCUBOOT_HEADER_SIZE}        "0x400"      )       # Must be a multiple of 1024

    # Set to external flash if not set by the environment
    if(NOT DEFINED ENV{OTA_USE_EXTERNAL_FLASH})
        set(ENV{OTA_USE_EXTERNAL_FLASH}     "1")
    endif()

    if ("$ENV{OTA_USE_EXTERNAL_FLASH}" STREQUAL "0")
        message("Using internal flash for Secondary Slot")
        set(ENV{CY_FLASH_ERASE_VALUE}       "0x00" )
        set(ENV{MCUBOOT_MAX_IMG_SECTORS}    "2000"       )       # 1MB app max
        set(ENV{CY_BOOT_SCRATCH_SIZE}       "0x00010000" )
        set(ENV{MCUBOOT_BOOTLOADER_SIZE}    "0x00018000" )
        set(ENV{CY_BOOT_PRIMARY_1_START}    "0x00018000" )  # Currently follows Bootloader sequentially
        set(ENV{CY_BOOT_PRIMARY_1_SIZE}     "0x000EE000" )
        set(ENV{CY_BOOT_PRIMARY_2_SIZE}     "0x01000" )
        set(ENV{CY_BOOT_SECONDARY_2_START}  "0x001E0000" )
    else()
        message("Using external flash for Secondary Slot")
        set(ENV{CY_BOOT_USE_EXTERNAL_FLASH}  "1")
        set(ENV{CY_FLASH_ERASE_VALUE}       "0xFF" )
        set(ENV{MCUBOOT_MAX_IMG_SECTORS}    "3584"       )       # 1.75MB app max
        set(ENV{CY_BOOT_SCRATCH_SIZE}       "0x00040000" )
        set(ENV{MCUBOOT_BOOTLOADER_SIZE}    "0x00018000" )
        set(ENV{CY_BOOT_PRIMARY_1_START}    "0x00018000" )  # Currently follows Bootloader sequentially
        set(ENV{CY_BOOT_PRIMARY_1_SIZE}     "0x001C0000" )
        set(ENV{CY_BOOT_SECONDARY_1_SIZE}   "0x001C0000" )
    endif()

    # Linker script for OTA
    #
    set(ENV{LINKER_SCRIPT_NAME} "ota/$ENV{LINKER_SCRIPT_NAME}_ota_int")

    # secure boot not supported for PSoC 062 boards
    # DO NOT USE: set(CY_BOOT_SECURE_BOOT         "1")

endif(OTA_SUPPORT)

# -------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------
# Provide metadata for listing on Amazon FreeRTOS console.
afr_set_board_metadata(ID "${AFR_BOARD_NAME}")
afr_set_board_metadata(IS_ACTIVE true)
afr_set_board_metadata(NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DISPLAY_NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DESCRIPTION "PSoC 6 Wi-Fi BT Prototyping Kit")
afr_set_board_metadata(VENDOR_NAME "Cypress")
afr_set_board_metadata(FAMILY_NAME "PSoC 6")
afr_set_board_metadata(DATA_RAM_MEMORY "1024KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(CODE_SIGNER "")
afr_set_board_metadata(SUPPORTED_IDE "ModusToolbox")
afr_set_board_metadata(RECOMMENDED_IDE "ModusToolbox")
afr_set_board_metadata(IDE_ModusToolbox_NAME "ModusToolbox")
if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
    afr_set_board_metadata(IDE_ModusToolbox_COMPILER "GCC")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
    afr_set_board_metadata(IDE_ModusToolbox_COMPILER "ARM")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    afr_set_board_metadata(IDE_ModusToolbox_COMPILER "IAR")
endif()
afr_set_board_metadata(IDE_ModusToolbox_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/cypress/${AFR_BOARD_NAME}/ModusToolbox/aws_demos")

cy_add_link_libraries(
    DEVICE "CY8C624ABZI-D44"
    PORT_DIR "${cy_board_dir}/ports"
    BSP_DIR "${app_dir}/application_code/cy_code"
    CONFIG_DIRS "${include_dirs}"
    COMPONENTS "4343W;CM0P_SLEEP;CAPSENSE"
    DEFINES "CY_USE_LWIP"
)

if (NOT DEFINED LIB_MODE)
    cy_create_exe_target(
        EXE_APP_NAME ""
        SOURCE_DIRS "${app_dir}/application_code"
    )
endif()
