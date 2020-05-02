# These Amazon FreeRTOS related global variables are available to use.
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
include("${AFR_VENDORS_DIR}/cypress/psoc6/cmake/cy_defines.cmake")

#--------------------------------------------------------------------
# Default linker script for this board
#  (modified for OTA builds)
#
set(LINKER_SCRIPT_NAME "cyb06xxa_cm4_dual")

#--------------------------------------------------------------------
# Board defines
#

set(CY_TFM_HEX "${cy_psoc6_dir}/psoc64tfm/COMPONENT_TFM_S_FW/tfm_s_unsigned.hex")
set(CY_TFM_POLICY_FILE "${cy_psoc6_dir}/psoc64tfm/security/policy_multi_img_CM0p_CM4_debug_2M.json")
set(CY_TFM_SIGN_SCRIPT "${cy_psoc6_dir}/psoc64tfm/security/sign.py")
set(CY_DEVICE_NAME "cy8ckit-064s0s2-4343w")

# Python path definition
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
find_program(CY_PYTHON_PATH "python")
else()
find_program(CY_PYTHON_PATH "python3")
endif()

target_compile_definitions(${AFR_TARGET_APP_NAME} PUBLIC
    "-DCOMPONENT_CAPSENSE"
    )

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
    # TODO: new 064 memory map (note: No secure secondary yet...)
    # 0x10000000 - 0x10040000 Secure Primary
    # 0x10040000 - 0x10100000 Nonsecure Primary
    # 0x10100000 - 0x101C0000 Nonsecure Secondary
    # 0x101C0000 - 0x10200000 Reserved
    #
    set(MCUBOOT_HEADER_SIZE         "0x400"      )       # Must be a multiple of 1024
    set(MCUBOOT_MAX_IMG_SECTORS     "2000"       )
    set(CY_BOOT_SCRATCH_SIZE        "0x00001000" )
    set(MCUBOOT_BOOTLOADER_SIZE     "0x00040000" )
    set(CY_BOOT_PRIMARY_1_START     "0x10040000" )
    set(CY_BOOT_PRIMARY_1_SIZE      "0x00080000" )

    #set(CY_BOOT_PRIMARY_1_SIZE      "0x00080000" ) # Moved WIFI FW out of AFR app so slots are smaller

    # When we use external FLASH (TODO)
    #
    # if(SLOT_1_EXTERNAL)
    #   TODO: linker script for external?
    #
    #   set(CY_USE_EXTERNAL_FLASH  "1" )
    #   set(CY_BOOT_PRIMARY_2_START  "0x????" )
    #   set(CY_BOOT_PRIMARY_2_SIZE   "0x????" )
    # endif()

    # Default to internal FLASH value for MCUBoot header signing
    #
    set(CY_FLASH_ERASE_VALUE        "0x00" )

    # Linker script for OTA Internal Flash!
    #
    # Don't change linker script for 064 TFM builds
    #     set(LINKER_SCRIPT_NAME "ota/${LINKER_SCRIPT_NAME}_ota_int")

endif(OTA_SUPPORT)

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
afr_set_board_metadata(SUPPORTED_IDE "CMakeBuildSystem")
afr_set_board_metadata(RECOMMENDED_IDE "CMakeBuildSystem")
afr_set_board_metadata(IDE_CMakeBuildSystem_NAME "CMakeBuildSystem")
afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "GCC")
if("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
    afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "ARM")
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "IAR")
endif()
afr_set_board_metadata(IDE_CMakeBuildSystem_PROJECT_LOCATION "null")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

#--------------------------------------------------------------------
# Include general kit functions
#
include("${cy_psoc6_dir}/cmake/cy_kit_utils.cmake")

cy_kit_generate(
    DEVICE "CYB0644ABZI-S2D44"
    LINKER_SCRIPT "${LINKER_SCRIPT_NAME}"
    COMPONENTS "4343W;CAPSENSE;TFM_NS_FREERTOS_WRAPPER;TFM_NS_INTERFACE"
    DEFINES "CY_BLE_SUPPORTED;CY_USE_LWIP;CY_TFM_PSA_SUPPORTED"
)
