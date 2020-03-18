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

include("${cy_psoc6_dir}/cmake/cy_kit_utils.cmake")

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
# Provide metadata for listing on Amazon FreeRTOS console.
afr_set_board_metadata(ID "${AFR_BOARD_NAME}")
afr_set_board_metadata(IS_ACTIVE true)
afr_set_board_metadata(NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DISPLAY_NAME "${AFR_BOARD_NAME}")
afr_set_board_metadata(DESCRIPTION "PSoC 6 Wi-Fi BT Pioneer Kit")
afr_set_board_metadata(VENDOR_NAME "Cypress")
afr_set_board_metadata(FAMILY_NAME "PSoC 6")
afr_set_board_metadata(DATA_RAM_MEMORY "288KB")
afr_set_board_metadata(PROGRAM_MEMORY "1MB")
afr_set_board_metadata(CODE_SIGNER "")
afr_set_board_metadata(SUPPORTED_IDE "ModusToolbox")
afr_set_board_metadata(RECOMMENDED_IDE "ModusToolbox")
afr_set_board_metadata(IDE_ModusToolbox_NAME "ModusToolbox")
afr_set_board_metadata(IDE_ModusToolbox_COMPILER "GCC")
afr_set_board_metadata(IDE_ModusToolbox_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/cypress/${AFR_BOARD_NAME}/ModusToolbox/aws_demos")

cy_kit_generate(
    DEVICE "CY8C6247BZI-D54"
    LINKER_SCRIPT "cy8c6xx7_cm4_dual"
    COMPONENTS "4343W;CM0P_SLEEP;CAPSENSE"
    DEFINES "CYHAL_UDB_SDIO;CY_USE_LWIP"
)
