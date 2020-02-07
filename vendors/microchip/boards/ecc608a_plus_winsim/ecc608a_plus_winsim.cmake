# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "MCHP-ATECC608A-With-Windows-Simulator")
afr_set_board_metadata(DISPLAY_NAME "ATECC608A with Windows Simulator")
afr_set_board_metadata(DESCRIPTION " Microchip ATECC608A secure element with Windows Simulator ")
afr_set_board_metadata(VENDOR_NAME "Microchip")
afr_set_board_metadata(FAMILY_NAME "ATECC608A")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "VisualStudio")
afr_set_board_metadata(RECOMMENDED_IDE "VisualStudio")
afr_set_board_metadata(IDE_VisualStudio_NAME "Visual Studio")
afr_set_board_metadata(IDE_VisualStudio_COMPILER "MSVC")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "FALSE")

afr_set_board_metadata(IDE_VisualStudio_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/microchip/ecc608a_plus_winsim/visual_studio/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")
