# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "Espressif-ESP-WROVER-KIT")
afr_set_board_metadata(DISPLAY_NAME "ESP-WROVER-KIT")
afr_set_board_metadata(DESCRIPTION "Development board produced by Espressif that has support for LCD, MicroSD card, and USB JTAG interface. It comes in two variants either with ESP-WROOM-32 or ESP32-WROVER module.")
afr_set_board_metadata(VENDOR_NAME "Espressif")
afr_set_board_metadata(FAMILY_NAME "ESP32")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(DATA_RAM_MEMORY "520KB")
afr_set_board_metadata(PROGRAM_MEMORY "4MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "CMakeBuildSystem;MakeBuildSystem")
afr_set_board_metadata(RECOMMENDED_IDE "CMakeBuildSystem")
afr_set_board_metadata(IDE_CMakeBuildSystem_NAME "CMakeBuildSystem")
afr_set_board_metadata(IDE_CMakeBuildSystem_COMPILER "GCC")

afr_set_board_metadata(IDE_MakeBuildSystem_PROJECT_LOCATION "${AFR_ROOT_DIR}/vendors/espressif/boards/esp32/aws_demos/make")
afr_set_board_metadata(IDE_CMakeBuildSystem_PROJECT_LOCATION "${AFR_ROOT_DIR}/vendors/espressif/boards/esp32")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${AFR_ROOT_DIR}/vendors/espressif/boards/esp32/aws_demos/common/config_files")