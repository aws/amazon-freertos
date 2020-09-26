function(cy_config_ota_exe_target)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "EXE_APP_NAME"
    ""
    )

    # Define these environment variables to override default settings
    # "export CY_TEST_APP_VERSION_IN_TAR=1"
    # "export APP_VERSION_MAJOR=x"
    # "export APP_VERSION_MINOR=y"
    # "export APP_VERSION_BUILD=z"
    if(NOT "${CY_TEST_APP_VERSION_IN_TAR}" STREQUAL "")
        if ( ("$ENV{APP_VERSION_MAJOR}" STREQUAL "") OR
             ("$ENV{APP_VERSION_MINOR}" STREQUAL "") OR
             ("$ENV{APP_VERSION_BUILD}" STREQUAL "") )
            message(FATAL "Define APP_VERSION_MAJOR, APP_VERSION_MINOR, and APP_VERSION_BUILD when using CY_TEST_APP_VERSION_IN_TAR.")
        endif()
        target_compile_definitions(${ARG_EXE_APP_NAME} PUBLIC
            "-DAPP_VERSION_MAJOR=$ENV{APP_VERSION_MAJOR}"
            "-DAPP_VERSION_MINOR=$ENV{APP_VERSION_MINOR}"
            "-DAPP_VERSION_BUILD=$ENV{APP_VERSION_BUILD}"
        )
        set(CY_BUILD_VERSION "$ENV{APP_VERSION_MAJOR}.$ENV{APP_VERSION_MINOR}.$ENV{APP_VERSION_BUILD}")
    endif()

    if ("$ENV{MCUBOOT_IMAGE_NUMBER}" STREQUAL "")
        set(ENV{MCUBOOT_IMAGE_NUMBER} "1" )
    endif()

    # Add OTA defines
    target_compile_definitions(${ARG_EXE_APP_NAME} PUBLIC
        "-DOTA_SUPPORT=1"
        "-DMCUBOOT_KEY_FILE=${MCUBOOT_KEY_FILE}"
        "-DCY_FLASH_ERASE_VALUE=$ENV{CY_FLASH_ERASE_VALUE}"
        "-DMCUBOOT_HEADER_SIZE=$ENV{MCUBOOT_HEADER_SIZE}"
        "-DMCUBOOT_IMAGE_NUMBER=$ENV{MCUBOOT_IMAGE_NUMBER}"
        "-DCY_BOOT_SCRATCH_SIZE=$ENV{CY_BOOT_SCRATCH_SIZE}"
        "-DCY_BOOT_BOOTLOADER_SIZE=$ENV{MCUBOOT_BOOTLOADER_SIZE}"
        "-DMCUBOOT_BOOTLOADER_SIZE=$ENV{MCUBOOT_BOOTLOADER_SIZE}"
        "-DCY_BOOT_PRIMARY_1_START=$ENV{CY_BOOT_PRIMARY_1_START}"
        "-DCY_BOOT_PRIMARY_1_SIZE=$ENV{CY_BOOT_PRIMARY_1_SIZE}"
        "-DCY_BOOT_SECONDARY_1_SIZE=$ENV{CY_BOOT_PRIMARY_1_SIZE}"
        "-DMCUBOOT_MAX_IMG_SECTORS=$ENV{MCUBOOT_MAX_IMG_SECTORS}"
        "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF=1"
        )

    # is CY_BOOT_USE_EXTERNAL_FLASH supported?
    if(("${CY_BOOT_USE_EXTERNAL_FLASH}" STREQUAL "1" ) OR ("$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" STREQUAL "1"))
        target_compile_definitions(${ARG_EXE_APP_NAME} PUBLIC "-DCY_BOOT_USE_EXTERNAL_FLASH=$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" )
    endif()

    # are using Multi-image mcuboot?
    if("$ENV{MCUBOOT_IMAGE_NUMBER}" STREQUAL "2" )
        target_compile_definitions(${ARG_EXE_APP_NAME} PUBLIC
            "-DCY_BOOT_PRIMARY_2_START=$ENV{CY_BOOT_PRIMARY_2_START}"
            "-DCCY_BOOT_PRIMARY_2_SIZE=$ENV{CY_BOOT_PRIMARY_2_SIZE}"
            "-DCCY_BOOT_SECONDARY_2_SIZE=$ENV{CY_BOOT_PRIMARY_2_SIZE}"
            "-DCCY_BOOT_SECONDARY_2_START=$ENV{CY_BOOT_SECONDARY_2_START}"
        )
    endif()
    #----------------------------------------------------------------
    # Add Linker options
    #
    if($ENV{MCUBOOT_HEADER_SIZE})
        if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "-Wl,--defsym,MCUBOOT_HEADER_SIZE=$ENV{MCUBOOT_HEADER_SIZE}")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--pd=\"-DMCUBOOT_HEADER_SIZE=$ENV{MCUBOOT_HEADER_SIZE}\"")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "SHELL:--config_def MCUBOOT_HEADER_SIZE=$ENV{MCUBOOT_HEADER_SIZE}")
        endif()
    endif()
    if($ENV{MCUBOOT_BOOTLOADER_SIZE})
        if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "-Wl,--defsym,MCUBOOT_BOOTLOADER_SIZE=$ENV{MCUBOOT_BOOTLOADER_SIZE}")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--pd=\"-DMCUBOOT_BOOTLOADER_SIZE=$ENV{MCUBOOT_BOOTLOADER_SIZE}\"")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "SHELL: --config_def MCUBOOT_BOOTLOADER_SIZE=$ENV{MCUBOOT_BOOTLOADER_SIZE}")
        endif()
    endif()
    if($ENV{CY_BOOT_PRIMARY_1_SIZE})
        if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "-Wl,--defsym,CY_BOOT_PRIMARY_1_SIZE=$ENV{CY_BOOT_PRIMARY_1_SIZE}")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--pd=\"-DCY_BOOT_PRIMARY_1_SIZE=$ENV{CY_BOOT_PRIMARY_1_SIZE}\"")
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
            target_link_options(${ARG_EXE_APP_NAME} PUBLIC "SHELL: --config_def CY_BOOT_PRIMARY_1_SIZE=$ENV{CY_BOOT_PRIMARY_1_SIZE}")
        endif()
    endif()
endfunction(cy_config_ota_exe_target)

# -------------------------------------------------------------------------------------------------
# For OTA_SUPPORT, we need to sign the Hex output for use with cy_mcuboot
# This is used in a POST BUILD Step (see bottom of function(cy_kit_generate) )
# -------------------------------------------------------------------------------------------------
# These can be defined before calling to over-ride
#      - define in <application>/CMakeLists.txt )
#
#   CMake Variable                  Default
#   --------------                  -------
# IMGTOOL_SCRIPT_NAME       "./imgtool.py"
# MCUBOOT_SCRIPT_FILE_DIR   "${cy_port_support_dir}/ota/scripts"
# MCUBOOT_KEY_DIR           "${cy_port_support_dir}/ota/mcuboot/keys"
# MCUBOOT_KEY_FILE          "cypress-test-ec-p256.pem"
#
function(config_cy_mcuboot_sign_script)
    # Python script for the image signing

    # signing scripts and keys from MCUBoot
    if((NOT IMGTOOL_SCRIPT_NAME) OR ("${IMGTOOL_SCRIPT_NAME}" STREQUAL ""))
        set(IMGTOOL_SCRIPT_NAME     "./imgtool.py")
    endif()
    if((NOT MCUBOOT_SCRIPT_FILE_DIR) OR ("${MCUBOOT_SCRIPT_FILE_DIR}" STREQUAL ""))
        set(MCUBOOT_SCRIPT_FILE_DIR     "${cy_port_support_dir}/ota/scripts")
    endif()
    if((NOT MCUBOOT_KEY_DIR) OR ("${MCUBOOT_KEY_DIR}" STREQUAL ""))
        set(MCUBOOT_KEY_DIR             "${cy_port_support_dir}/ota/mcuboot/keys")
    endif()
    if((NOT MCUBOOT_KEY_FILE) OR ("${MCUBOOT_KEY_FILE}" STREQUAL ""))
        set(MCUBOOT_KEY_FILE  "cypress-test-ec-p256.pem")
    endif()

    set(IMGTOOL_SCRIPT_PATH     "${MCUBOOT_SCRIPT_FILE_DIR}/imgtool.py")

    # cy_mcuboot key file
    set(SIGNING_KEY_PATH         "${MCUBOOT_KEY_DIR}/${MCUBOOT_KEY_FILE}")

    # Is flash erase value defined ?
    # NOTE: For usage in imgtool.py, no value defaults to an erase value of 0xff
    # NOTE: Default for internal FLASH is 0x00
    if((NOT $ENV{CY_FLASH_ERASE_VALUE}) OR ("${CY_FLASH_ERASE_VALUE}" STREQUAL "0") OR ("${CY_FLASH_ERASE_VALUE}" STREQUAL "0x00"))
        set(FLASH_ERASE_VALUE "-R 0")
    else()
        set(FLASH_ERASE_VALUE "")
    endif()

    # Slot Start
    if(NOT $ENV{CY_BOOT_PRIMARY_1_START})
        message(FATAL_ERROR "You must define CY_BOOT_PRIMARY_1_START in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT $ENV{CY_BOOT_PRIMARY_1_SIZE})
        message(FATAL_ERROR "You must define CY_BOOT_PRIMARY_1_SIZE in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT $ENV{MCUBOOT_HEADER_SIZE})
        message(FATAL_ERROR "You must define MCUBOOT_HEADER_SIZE in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT $ENV{MCUBOOT_MAX_IMG_SECTORS})
        message(FATAL_ERROR "You must define MCUBOOT_MAX_IMG_SECTORS in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    # Default version for sign_script.sh
    if( "${CY_BUILD_VERSION}" STREQUAL "")
        set(CY_BUILD_VERSION "0.9.0")
    endif()


    # set env variables as local for the configure_file() call
    set(CY_ELF_TO_HEX "$ENV{CY_ELF_TO_HEX}")
    set(CY_ELF_TO_HEX_OPTIONS "$ENV{CY_ELF_TO_HEX_OPTIONS}")
    if("$ENV{CY_ELF_TO_HEX_FILE_ORDER}" STREQUAL "elf_first")
        set(CY_ELF_TO_HEX_FILE_1 "${CY_OUTPUT_FILE_PATH_ELF}")
        set(CY_ELF_TO_HEX_FILE_2 "${CY_OUTPUT_FILE_PATH_UNSIGNED_HEX}")
    else()
        set(CY_ELF_TO_HEX_FILE_1 "${CY_OUTPUT_FILE_PATH_UNSIGNED_HEX}")
        set(CY_ELF_TO_HEX_FILE_2 "${CY_OUTPUT_FILE_PATH_ELF}")
    endif()

    # If PSoC 062 board, use "create" instead of "sign"; do not pass in CY_SIGNING_KEY_ARG
    # MCUBoot must also be modified to skip checking the signature
    #   Comment out and re-build MCUBootApp
    #   <mcuboot>/boot/cypress/MCUBootApp/config/mcuboot_config/mcuboot_config.h
    #   line 37, 38, 77
    # 37: //#define MCUBOOT_SIGN_EC256
    # 38: //#define NUM_ECC_BYTES (256 / 8)   // P-256 curve size in bytes, rnok: to make compilable
    # 77: //#define MCUBOOT_VALIDATE_PRIMARY_SLOT
    if("${CY_BOOT_SECURE_BOOT}" STREQUAL "sign")
        set(IMGTOOL_SCRIPT_COMMAND "sign")
        set(CY_SIGNING_KEY_ARG   "-k ${SIGNING_KEY_PATH}")
    else()
        set(IMGTOOL_SCRIPT_COMMAND "create")
        set(CY_SIGNING_KEY_ARG   " ")
        set(SIGNING_KEY_PATH     "")
    endif()

    # set these ENV vars locally for configure_file
    set(AFR_TARGET_APP_NAME ${ARG_EXE_APP_NAME})
    set(MCUBOOT_HEADER_SIZE $ENV{MCUBOOT_HEADER_SIZE})
    set(MCUBOOT_MAX_IMG_SECTORS $ENV{MCUBOOT_MAX_IMG_SECTORS})
    set(CY_BOOT_PRIMARY_1_START $ENV{CY_BOOT_PRIMARY_1_START})
    set(CY_BOOT_PRIMARY_1_SIZE $ENV{CY_BOOT_PRIMARY_1_SIZE})
    set(CY_APP_DIRECTORY ${CMAKE_SOURCE_DIR})

    configure_file("${cy_psoc6_cmake_dir}/sign_script.sh.in" "${SIGN_SCRIPT_FILE_PATH}" @ONLY NEWLINE_STYLE LF)

endfunction(config_cy_mcuboot_sign_script)

function(cy_sign_boot_image)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "EXE_APP_NAME"
    ""
    )
    if(NOT(CY_TFM_PSA_SUPPORTED) AND OTA_SUPPORT)
        # non-TFM signing
        #------------------------------------------------------------
        # Create our script filename in this scope
        set(SIGN_SCRIPT_FILE_NAME             "sign_${ARG_EXE_APP_NAME}.sh")
        set(SIGN_SCRIPT_FILE_PATH             "${CMAKE_BINARY_DIR}/${SIGN_SCRIPT_FILE_NAME}")
        set(SIGN_SCRIPT_FILE_PATH_TMP         "${CMAKE_BINARY_DIR}/tmp/${SIGN_SCRIPT_FILE_NAME}")
        set(CY_OUTPUT_FILE_PATH               "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}")
        set(CY_OUTPUT_FILE_PATH_ELF           "${CY_OUTPUT_FILE_PATH}.elf")
        set(CY_OUTPUT_FILE_PATH_HEX           "${CY_OUTPUT_FILE_PATH}.hex")
        set(CY_OUTPUT_FILE_NAME_UNSIGNED_HEX  "${ARG_EXE_APP_NAME}.unsigned.hex")
        set(CY_OUTPUT_FILE_PATH_UNSIGNED_HEX  "${CY_OUTPUT_FILE_PATH}.unsigned.hex")
        set(CY_OUTPUT_FILE_NAME_BIN           "${ARG_EXE_APP_NAME}.bin")
        set(CY_OUTPUT_FILE_PATH_BIN           "${CY_OUTPUT_FILE_PATH}.bin")
        set(CY_OUTPUT_FILE_PATH_TAR           "${CY_OUTPUT_FILE_PATH}.tar")
        set(CY_OUTPUT_FILE_PATH_WILD          "${CY_OUTPUT_FILE_PATH}.*")
        set(CY_COMPONENTS_JSON_NAME           "components.json")
        set(CY_OUTPUT_FILE_NAME_TAR           "${ARG_EXE_APP_NAME}.tar")

        # We can use objcopy for .hex to .bin for all toolchains
        find_program(GCC_OBJCOPY arm-none-eabi-objcopy HINT "${AFR_TOOLCHAIN_PATH}")
        if(NOT GCC_OBJCOPY )
            message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
        endif()

        if("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND "${GCC_OBJCOPY}" -O ihex "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf" "${CMAKE_BINARY_DIR}/${CY_OUTPUT_FILE_NAME_UNSIGNED_HEX}"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            find_program(FROMELF_TOOL fromelf HINT "${AFR_TOOLCHAIN_PATH}")
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find fromelf tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --i32 --output="${CMAKE_BINARY_DIR}/${CY_OUTPUT_FILE_NAME_UNSIGNED_HEX}" "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
        find_program(FROMELF_TOOL ielftool HINT "${AFR_TOOLCHAIN_PATH}")
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find ielftool tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --ihex "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf" "${CMAKE_BINARY_DIR}/${CY_OUTPUT_FILE_NAME_UNSIGNED_HEX}"
            )
        elseif(NOT AFR_METADATA_MODE)
            message(FATAL_ERROR "Toolchain ${AFR_TOOLCHAIN} is not supported ")
        endif()

        # creates the script to call imgtool.py to sign the image
        config_cy_mcuboot_sign_script("${CMAKE_BINARY_DIR}")

        add_custom_command(
            TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMAND "bash" "${SIGN_SCRIPT_FILE_PATH}"
        )
    endif()

    if(CY_TFM_PSA_SUPPORTED)
        set(CY_AWS_ELF  "${CMAKE_BINARY_DIR}/aws.elf")
        set(CY_CM0_IMG "${CMAKE_BINARY_DIR}/cm0.hex")
        set(CY_CM4_IMG "${CMAKE_BINARY_DIR}/cm4.hex")
        set(CY_CM0_UNSIGNED_IMG "${CMAKE_BINARY_DIR}/cm0_unsigned.hex")
        set(CY_CM4_UNSIGNED_IMG "${CMAKE_BINARY_DIR}/cm4_unsigned.hex")
	    # for file suitable for uploading to AWS
        set(CY_APP_CM0_BIN "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}_cm0.bin")
        set(CY_APP_CM4_BIN "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}_cm4.bin")
        set(CY_CM0_UPGRADE_IMG "${CMAKE_BINARY_DIR}/cm0_upgrade.hex")
        set(CY_CM4_UPGRADE_IMG "${CMAKE_BINARY_DIR}/cm4_upgrade.hex")
        set(CY_APP_CM0_UPGRADE_BIN "${CMAKE_BINARY_DIR}/cm0_upgrade.bin")
        set(CY_APP_CM4_UPGRADE_BIN "${CMAKE_BINARY_DIR}/cm4_upgrade.bin")

        if("$ENV{CY_TFM_HEX}" STREQUAL "")
            message(FATAL_ERROR "You must define CY_TFM_HEX in your board CMakeLists.txt for CY_TFM_PSA_SUPPORTED")
        endif()
        if("$ENV{CY_TFM_HEX}" STREQUAL "")
            message(FATAL_ERROR "You must define CY_TFM_POLICY_FILE in your board CMakeLists.txt for CY_TFM_PSA_SUPPORTED")
        endif()
        if("$ENV{CY_DEVICE_NAME}" STREQUAL "")
            message(FATAL_ERROR "You must define CY_DEVICE_NAME in your board CMakeLists.txt for CY_TFM_PSA_SUPPORTED")
        endif()

        # We can use objcopy for .hex to .bin for all toolchains
        find_program(GCC_OBJCOPY arm-none-eabi-objcopy HINT "${AFR_TOOLCHAIN_PATH}")
        if(NOT GCC_OBJCOPY )
            message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
        endif()

        if("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            # Workaround the signing issue by removing the sFlash sections
            add_custom_command(
            TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
            COMMAND "${GCC_OBJCOPY}" -R .cy_sflash_user_data -R .cy_toc_part2 "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf" "${CY_AWS_ELF}"
            )
            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND "${GCC_OBJCOPY}" -O ihex "${CY_AWS_ELF}" "${CY_CM4_IMG}"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            find_program(FROMELF_TOOL fromelf HINT "${AFR_TOOLCHAIN_PATH}")
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find fromelf tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --i32 --output="${CY_CM4_IMG}" "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
        find_program(FROMELF_TOOL ielftool HINT "${AFR_TOOLCHAIN_PATH}")
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find ielftool tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --ihex "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}.elf" "${CY_CM4_IMG}"
            )
        elseif(NOT AFR_METADATA_MODE)
            message(FATAL_ERROR "Toolchain ${AFR_TOOLCHAIN} is not supported ")
        endif()

        # Sign both TFM and AFR images
        find_program(CY_SIGN_SCRIPT cysecuretools)
        if(NOT CY_SIGN_SCRIPT )
            message(FATAL_ERROR "Cannot find cysecuretools.")
        endif()

	add_custom_command(
            TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "$ENV{CY_TFM_HEX}"    "${CY_CM0_IMG}"
            #The output of cysecuretools for signing upgrade image is <name>_upgrade.hex. The original file is rename as <name>_unsigned.hex
            COMMAND "${CY_SIGN_SCRIPT}" --policy "$ENV{CY_TFM_POLICY_FILE}" --target "$ENV{CY_DEVICE_NAME}" sign-image --hex "${CY_CM0_IMG}" --image-type UPGRADE --image-id 1
            COMMAND "${CY_SIGN_SCRIPT}" --policy "$ENV{CY_TFM_POLICY_FILE}" --target "$ENV{CY_DEVICE_NAME}" sign-image --hex "${CY_CM4_IMG}" --image-type UPGRADE --image-id 16
            COMMAND "${CMAKE_COMMAND}" -E copy "${CY_CM0_UNSIGNED_IMG}"  "${CY_CM0_IMG}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CY_CM4_UNSIGNED_IMG}"  "${CY_CM4_IMG}"
            #For signing boot image, cysecuretools over-writes the original file.
            COMMAND "${CY_SIGN_SCRIPT}" --policy "$ENV{CY_TFM_POLICY_FILE}" --target "$ENV{CY_DEVICE_NAME}" sign-image --hex "${CY_CM0_IMG}" --image-type BOOT --image-id 1
            COMMAND "${CY_SIGN_SCRIPT}" --policy "$ENV{CY_TFM_POLICY_FILE}" --target "$ENV{CY_DEVICE_NAME}" sign-image --hex "${CY_CM4_IMG}" --image-type BOOT --image-id 16
        )

        # Prepare vars for script to create TAR archives
        if(OTA_SUPPORT)
            #------------------------------------------------------------
            # Create our build_tar script
            set(TAR_SCRIPT_FILE_NAME            "tar_${ARG_EXE_APP_NAME}.sh")
            set(TAR_SCRIPT_FILE_PATH            "${CMAKE_BINARY_DIR}/${TAR_SCRIPT_FILE_NAME}")
            set(TAR_SCRIPT_FILE_PATH_TMP        "${CMAKE_BINARY_DIR}/tmp/${TAR_SCRIPT_FILE_NAME}")
            set(CY_OUTPUT_FILE_PATH             "${CMAKE_BINARY_DIR}/${ARG_EXE_APP_NAME}")
            get_filename_component(CY_OUTPUT_FILE_CM0_NAME_BIN ${CY_APP_CM0_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM4_NAME_BIN ${CY_APP_CM4_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM0_UPGRADE_BIN ${CY_APP_CM0_UPGRADE_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM4_UPGRADE_BIN ${CY_APP_CM4_UPGRADE_BIN} NAME)

            set(CY_OUTPUT_FILE_PATH_BIN         "${CY_OUTPUT_FILE_PATH}")

            set(CY_COMPONENTS_JSON_NAME         "components.json")

            # Default version for sign_script.sh
            if( "${CY_BUILD_VERSION}" STREQUAL "")
                set(CY_BUILD_VERSION "0.9.0")
            endif()

            set(CY_CM4_ONLY_TAR   "cm4_only_signed.tar")
            set(CY_CM4_CM0_TAR    "cm4_cm0_signed.tar")

            set(CY_CM4_ONLY_UPGRADE_TAR "cm4_only_upgrade.tar")
            set(CY_CM4_CM0_UPGRADE_TAR  "cm4_cm0_upgrade.tar")

            set(AFR_TARGET_APP_NAME ${ARG_EXE_APP_NAME})
            set(MCUBOOT_MAX_IMG_SECTORS $ENV{MCUBOOT_MAX_IMG_SECTORS})
            set(CY_BOOT_PRIMARY_1_START $ENV{CY_BOOT_PRIMARY_1_START})
            set(CY_BOOT_PRIMARY_1_SIZE $ENV{CY_BOOT_PRIMARY_1_SIZE})
            set(CY_BOOT_PRIMARY_2_SIZE $ENV{CY_BOOT_PRIMARY_2_SIZE})
            set(CY_BOOT_SECONDARY_2_START $ENV{CY_BOOT_SECONDARY_2_START})
            set(CY_APP_DIRECTORY ${CMAKE_SOURCE_DIR})

            # create the script to call imgtool.py to sign the image
            configure_file("${cy_psoc6_cmake_dir}/tar.sh.in" "${TAR_SCRIPT_FILE_PATH_TMP}" @ONLY NEWLINE_STYLE LF)
            # and then make sure it is executable on all platforms
            file( COPY "${TAR_SCRIPT_FILE_PATH_TMP}" DESTINATION "${CMAKE_BINARY_DIR}"
                  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            )

            add_custom_command(
                TARGET "${ARG_EXE_APP_NAME}" POST_BUILD
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                COMMAND "${CMAKE_COMMAND}" -E remove -f "${CY_APP_CM0_BIN}" "${CY_APP_CM4_BIN}" "${CY_APP_CM0_UPGRADE_BIN}" "${CY_APP_CM4_UPGRADE_BIN}"
                COMMAND "bash" "${TAR_SCRIPT_FILE_PATH_TMP}"
            )
        endif(OTA_SUPPORT)
    endif(CY_TFM_PSA_SUPPORTED)
endfunction(cy_sign_boot_image)

function(cy_add_linker_options)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "EXE_APP_NAME"
    ""
    )
    set(linker_script "${app_dir}/application_code/cy_code/COMPONENT_$ENV{CY_CORE}/TOOLCHAIN_$ENV{CY_TOOLCHAIN}/$ENV{LINKER_SCRIPT_NAME}$ENV{CY_LINKERSCRIPT_EXT}")

    if("$ENV{CY_TOOLCHAIN}" STREQUAL "IAR")
        # IAR has a separate linker command that does not use the same options as the compiler.
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--config" "${linker_script}")
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--map=${ARG_EXE_APP_NAME}.map")
    elseif("$ENV{CY_TOOLCHAIN}" STREQUAL "ARM")
        # ARM has a separate linker command that does not use the same options as the compiler.
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--scatter=${linker_script}")
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC --info sizes)
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC --callgraph)
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "--map")
    elseif("$ENV{CY_TOOLCHAIN}" STREQUAL "GCC_ARM")
        # GNU based toolchain
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "-T${linker_script}")
        target_link_options(${ARG_EXE_APP_NAME} PUBLIC "-Wl,--Map=${ARG_EXE_APP_NAME}.map")
    endif()
    set(CMAKE_EXECUTABLE_SUFFIX ".elf" PARENT_SCOPE)
endfunction(cy_add_linker_options)

function(cy_create_exe_target)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "EXE_APP_NAME;SOURCE_DIRS"
    ""
    )
    #--------------------------------------------------------------------
    # Set initial info for the application
    #
    # Pass argument to cmake "-DAPP=<path_to_application_directory>"
    #      Application directory needs to have a CMakeLists.txt file
    # We include the file here to get some app-specific sources and settings
    #
    # Define these environment variables to override default settings
    # "export CY_TEST_APP_VERSION_IN_TAR=1"
    # "export APP_VERSION_MAJOR=x"
    # "export APP_VERSION_MINOR=y"
    # "export APP_VERSION_BUILD=z"

    # -------------------------------------------------------------------------------------------------
    # Amazon FreeRTOS demos and tests
    # -------------------------------------------------------------------------------------------------
    # We require you to define at least demos and tests executable targets. Available demos and tests
    # will be automatically enabled by us. You need to provide other project settings such as linker
    # scripts and post build commands.
    # ==================== Example ====================

    # Set dirs for use below:
    #
    # app_dir: base application directory
    #        |
    #        |-- include                (AWS IOT config files)
    #        \-- main.c
    #
    # aws_credentials_incude: Location of aws_clientcredential_keys.h and aws_clientcredential.h
    # ARG_SOURCE_DIRS: used to locate the linker script files
    #

    set(cy_exe_target_include "")
    if(AFR_IS_TESTING)
        if ("${ARG_EXE_APP_NAME}" STREQUAL "")
            set(ARG_EXE_APP_NAME aws_tests)
        endif()
        list(APPEND cy_exe_target_include "${AFR_TESTS_DIR}/include")
    else()
        if ("${ARG_EXE_APP_NAME}" STREQUAL "")
            set(ARG_EXE_APP_NAME aws_demos)
        endif()
        list(APPEND cy_exe_target_include "${AFR_DEMOS_DIR}/include")
    endif()
    # prepend the <board>/<app>/config_files before any other config files
    list(APPEND cy_exe_target_include "${app_dir}/config_files")

    add_executable(${ARG_EXE_APP_NAME})


    # -------------------------------------------------------------------------------------------------
    # Configure ModusToolbox target (used to build ModusToolbox firmware)
    # -------------------------------------------------------------------------------------------------

    # Find Board-specific files
    if (NOT ("${ARG_SOURCE_DIRS}" STREQUAL ""))
        cy_find_files(board_exe_files DIRECTORY "${ARG_SOURCE_DIRS}")
        cy_get_includes(app_inc ITEMS "${board_exe_files}" ROOT "${ARG_SOURCE_DIRS}")
        target_include_directories(${ARG_EXE_APP_NAME} BEFORE PUBLIC "${app_inc}")
        cy_get_src(board_src ITEMS "${board_exe_files}")
        target_sources(${ARG_EXE_APP_NAME} PUBLIC "${board_src}")
        cy_get_libs(board_libs ITEMS "${board_exe_files}")
        target_link_libraries(${ARG_EXE_APP_NAME} PUBLIC "${board_libs}")
    endif()

    #--------------------------------------------------------------------
    # Common libraries we need for all apps
    #
    target_link_libraries(${ARG_EXE_APP_NAME}  PUBLIC
        AFR::common
        AFR::utils
        AFR::wifi
        AFR::wifi::mcu_port
        psoc6_core
    )

    if(BLE_SUPPORTED)
        target_link_libraries(${ARG_EXE_APP_NAME}  PUBLIC
            AFR::ble
            AFR::ble_hal::mcu_port
        )
    endif()

    #--------------------------------------------------------------------
    # linker script and map files
    #--------------------------------------------------------------------
    cy_add_linker_options(EXE_APP_NAME ${ARG_EXE_APP_NAME})
    set(CMAKE_EXECUTABLE_SUFFIX ".elf" PARENT_SCOPE)

    if (OTA_SUPPORT)
        cy_config_ota_exe_target(EXE_APP_NAME ${ARG_EXE_APP_NAME})
    endif()

    cy_sign_boot_image(EXE_APP_NAME ${ARG_EXE_APP_NAME})

endfunction()

