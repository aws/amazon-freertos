# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "MCHP-Curiosity-PIC32MZEF")
afr_set_board_metadata(DISPLAY_NAME "Curiosity PIC32MZEF")
afr_set_board_metadata(DESCRIPTION "Development board for the PIC32MZ EF Series MCU")
afr_set_board_metadata(VENDOR_NAME "Microchip")
afr_set_board_metadata(FAMILY_NAME "PIC32MZ")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(DATA_RAM_MEMORY "512KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "MPLABX")
afr_set_board_metadata(RECOMMENDED_IDE "MPLABX")
afr_set_board_metadata(IDE_MPLABX_NAME "MPLAB X")
afr_set_board_metadata(IDE_MPLABX_COMPILER "XC32")

afr_set_board_metadata(IDE_MPLABX_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/microchip/curiosity_pic32mzef/mplab/aws_demos")
afr_set_board_metadata(IDE_MPLABX_ADDITIONAL_FILES "${AFR_ROOT_DIR}/projects/microchip/curiosity_pic32mzef/mplab/bootloader")

afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")
