
# -------------------------------------------------------------------------------------------------
# For OTA_SUPPORT, we need to sign the Hex output for use with cy_mcuboot
# This is used in a POST BUILD Step (see bottom of function(cy_kit_generate) )
# -------------------------------------------------------------------------------------------------
# These can be defined before calling to over-ride
#      - define in <application>/CMakeLists.txt )
#
#   CMake Variable                  Default
#   --------------                  -------
# MCUBOOT_KEY_FILE           "cypress-test-ec-p256.pem"
#
function(config_cy_mcuboot_sign_script)
    # Python script for the image signing
    set(IMGTOOL_SCRIPT_NAME     "./imgtool.py")
    set(IMGTOOL_SCRIPT_PATH     "${MCUBOOT_SCRIPT_FILE_DIR}/imgtool.py")

    # cy_mcuboot key file
    if((NOT MCUBOOT_KEY_FILE) OR ("${MCUBOOT_KEY_FILE}" STREQUAL ""))
        set(MCUBOOT_KEY_FILE  "cypress-test-ec-p256.pem")
    endif()
    set(SIGNING_KEY_PATH         "${MCUBOOT_KEY_DIR}/${MCUBOOT_KEY_FILE}")

    # Is flash erase value defined ?
    # NOTE: Do not define anything for erase value 0xff
    if((NOT CY_FLASH_ERASE_VALUE) OR ("${CY_FLASH_ERASE_VALUE}" STREQUAL "0") OR ("${CY_FLASH_ERASE_VALUE}" STREQUAL "0x00"))
        set(FLASH_ERASE_VALUE "-R 0")
    else()
        set(FLASH_ERASE_VALUE "")
    endif()

    # Slot Start
    if(NOT CY_BOOT_PRIMARY_1_START)
        message(FATAL_ERROR "You must define CY_BOOT_PRIMARY_1_START in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT CY_BOOT_PRIMARY_1_SIZE)
        message(FATAL_ERROR "You must define CY_BOOT_PRIMARY_1_SIZE in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT MCUBOOT_HEADER_SIZE)
        message(FATAL_ERROR "You must define MCUBOOT_HEADER_SIZE in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    if(NOT MCUBOOT_MAX_IMG_SECTORS)
        message(FATAL_ERROR "You must define MCUBOOT_MAX_IMG_SECTORS in your board CMakeLists.txt for OTA_SUPPORT")
    endif()

    # Create version for sign_script.sh
    set(CY_BUILD_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_BUILD}")

    if("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
        find_program(GCC_OBJCOPY arm-none-eabi-objcopy)
        if(NOT GCC_OBJCOPY )
            message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
        endif()

        # Generate HEX file
        add_custom_command(
            TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
            COMMAND "${GCC_OBJCOPY}" -O ihex "${CY_OUTPUT_FILE_PATH_ELF}" "${CY_OUTPUT_FILE_PATH_HEX}"
        )
    elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
        find_program(FROMELF_TOOL fromelf)
        if(NOT FROMELF_TOOL )
            message(FATAL_ERROR "Cannot find fromelf tool")
        endif()

        # Generate HEX file
        add_custom_command(
            TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
            COMMAND ${FROMELF_TOOL} --i32 --output="${CY_OUTPUT_FILE_PATH_HEX}" "${CY_OUTPUT_FILE_PATH_ELF}"
        )
    elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
        find_program(FROMELF_TOOL ielftool)
        if(NOT FROMELF_TOOL )
            message(FATAL_ERROR "Cannot find ielftool tool")
        endif()

        # Generate HEX file
        add_custom_command(
            TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
            COMMAND ${FROMELF_TOOL} --ihex "${CY_OUTPUT_FILE_PATH_ELF}" "${CY_OUTPUT_FILE_PATH_HEX}"
        )
    elseif(NOT AFR_METADATA_MODE)
        message(FATAL_ERROR "Toolchain ${AFR_TOOLCHAIN} is not supported ")
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

    # create the script to call imgtool.py to sign the image
    configure_file("${cy_psoc6_dir}/cmake/sign_script.sh.in" "${SIGN_SCRIPT_FILE_PATH_TMP}" @ONLY NEWLINE_STYLE LF)
    # and make sure it is executable on all platforms
    file( COPY ${SIGN_SCRIPT_FILE_PATH_TMP} DESTINATION ${CMAKE_BINARY_DIR}
          FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
endfunction(config_cy_mcuboot_sign_script)

# -------------------------------------------------------------------------------------------------
# Configure ModusToolbox cmake environment
# -------------------------------------------------------------------------------------------------
function(cy_kit_generate)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "DEVICE;LINKER_SCRIPT;COMPONENTS;DEFINES"
    ""
    )

    # is SDIO supported?
    string(FIND "${ARG_DEFINES}" "CYHAL_UDB_SDIO" check_sdio)
    if (NOT ("${check_sdio}" STREQUAL "-1"))
        set(CYHAL_UDB_SDIO "1")
    endif()

    # is TF-M PSA supported?
    string(FIND "${ARG_DEFINES}" "CY_TFM_PSA_SUPPORTED" check_tfm_psa)
    if (NOT ("${check_tfm_psa}" STREQUAL "-1"))
       set(CY_TFM_PSA "1")
    endif()

    # is BLE supported?
    string(FIND "${ARG_DEFINES}" "CY_BLE_SUPPORTED" check_ble_supported)
    if (NOT ("${check_ble_supported}" STREQUAL "-1"))
       # Add BLE when generating metadata for Build Combination and OCW.
       if ("${AFR_METADATA_MODE}" STREQUAL "1")
          set(CY_BLE_SUPPORTED 1)           
       endif()
       # BLE is only supported for arm-gcc
       if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
          set(CY_BLE_SUPPORTED 1)
       endif()
    endif()

    # is CY_BOOT_USE_EXTERNAL_FLASH supported?
    string(FIND "${ARG_DEFINES}" "CY_BOOT_USE_EXTERNAL_FLASH" check_cy_boot_external_flash)
    if (NOT ("${check_cy_boot_external_flash}" STREQUAL "-1"))
       set(CY_BOOT_USE_EXTERNAL_FLASH "1")
    endif()

    #--------------------------------------------------------------------
    # Utilities
    #
    include("${cy_psoc6_dir}/cmake/cy_utils.cmake")
    if(EXISTS "${cy_psoc6_dir}/cmake/toolchains/${AFR_TOOLCHAIN}.cmake")
        include("${cy_psoc6_dir}/cmake/toolchains/${AFR_TOOLCHAIN}.cmake")
    elseif(AFR_METADATA_MODE)
        function(cy_cfg_toolchain)
        endfunction()
        set(ENV{CY_FREERTOS_TOOLCHAIN} GCC)
    else()
        message(FATAL_ERROR "Unsupported toolchain: ${AFR_TOOLCHAIN}")
    endif()

    # Set dirs for use below:
    #
    # app_dir: base application directory
    #        |
    #        |-- config_files           (AWS configuration files)
    #        |-- include                (AWS IOT config files)
    #        \-- main.c
    #
    # aws_credentials_incude: Location of aws_clientcredential_keys.h and aws_clientcredential.h
    # iot_common_include: Location of other iot_xxx.h files
    # cy_code_dir: used to locate the linker script files
    #
    if(CY_ALTERNATE_APP)
        set(app_dir "${AFR_ROOT_DIR}/${AFR_TARGET_APP_DIR}")
        if(NOT "${CY_APP_CONFIG_DIR}" STREQUAL "")
            set(aws_config_dir "${CY_APP_CONFIG_DIR}")
        else()
            set(aws_config_dir "${app_dir}/config_files")
        endif()
        if(NOT "${CY_APP_IOT_CONFIG_DIR}" STREQUAL "")
            set(iot_common_include "${CY_APP_IOT_CONFIG_DIR}")
        else()
            set(iot_common_include "${app_dir}/include")
        endif()
        set(cy_code_dir "${cy_board_dir}/aws_demos/application_code/cy_code")
    elseif(AFR_IS_TESTING)
        set(app_dir "${cy_board_dir}/aws_tests")
        set(aws_config_dir "${app_dir}/config_files")
        set(iot_common_include "${AFR_TESTS_DIR}/include")
        set(cy_code_dir "${app_dir}/application_code/cy_code")
    else()
        set(app_dir "${cy_board_dir}/aws_demos")
        set(aws_config_dir "${app_dir}/config_files")
        set(iot_common_include "${AFR_DEMOS_DIR}/include")
        set(cy_code_dir "${app_dir}/application_code/cy_code")
    endif()

    # verify we have an AWS config files dir
    if(NOT (EXISTS "${aws_config_dir}"))
        message( FATAL_ERROR "No config directory found: '${aws_config_dir}' (ex: <application>/config_files)")
    endif()

    # verify we have an AWS IOT config files dir
    if(NOT (EXISTS "${iot_common_include}"))
        message( FATAL_ERROR "No IOT common config directory found: '${iot_common_include}' (ex: <application>/include)")
    endif()

    # For now, the "cy_code" directory is under <board>/<app_name>/application_code/cy_code
    # Eventually we want cy_code_dir to be under the board directory, not the application
    # Allow over-rides of the linker script location

    set(CY_LINKER_PATH  "${cy_code_dir}")
    set(CY_ARCH_DIR     "${cy_clib_dir};${cy_psoc6_dir};${cy_whd_dir};${cy_capsense_dir}")

    cy_cfg_env(
        TARGET        "${AFR_BOARD_NAME}"
        DEVICE        "${ARG_DEVICE}"
        TOOLCHAIN     "${AFR_TOOLCHAIN}"
        LINKER_PATH   "${CY_LINKER_PATH}"
        LINKER_SCRIPT "${ARG_LINKER_SCRIPT}"
        COMPONENTS    "${AFR_BOARD_NAME};SOFTFP;BSP_DESIGN_MODUS;PSOC6HAL;FREERTOS;${ARG_COMPONENTS}"
        ARCH_DIR      "${CY_ARCH_DIR}"
    )

    # -------------------------------------------------------------------------------------------------
    # Configure ModusToolbox target (used to build ModusToolbox firmware)
    # -------------------------------------------------------------------------------------------------

    # Find Application-specific files
    cy_find_files(app_exe_files DIRECTORY "${app_dir}")
    if(NOT ("${app_exe_files}" STREQUAL ""))
        cy_get_includes(app_inc ITEMS "${app_exe_files}" ROOT "${app_dir}")
        target_include_directories(${AFR_TARGET_APP_NAME} BEFORE PUBLIC "${app_dir}")
        cy_get_src(app_src ITEMS "${app_exe_files}")
        if(NOT ("${app_src}" STREQUAL ""))
            target_sources(${AFR_TARGET_APP_NAME} PUBLIC             "${app_src}" )
        endif()
        cy_get_libs(app_libs ITEMS "${app_exe_files}")
        if(NOT ("${app_libs}" STREQUAL ""))
            target_link_libraries(${AFR_TARGET_APP_NAME} PUBLIC        "${app_libs}")
        endif()
    endif()

    # Find Config-specific files
    target_include_directories(${AFR_TARGET_APP_NAME} BEFORE PUBLIC "${aws_config_dir}")
    cy_find_files(cfg_files DIRECTORY "${aws_config_dir}")
    if(NOT ("${cfg_files}" STREQUAL ""))
        cy_get_src(cfg_src ITEMS "${cfg_files}" ROOT "${aws_config_dir}")
        target_sources(${AFR_TARGET_APP_NAME} PUBLIC            "${cfg_src}")
    endif()

    # Find Board-specific files
    cy_find_files(board_exe_files DIRECTORY "${cy_code_dir}")
    cy_get_includes(board_inc ITEMS "${board_exe_files}" ROOT "${cy_code_dir}")
    target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC "${board_inc}")
    cy_get_src(board_src ITEMS "${board_exe_files}")
    target_sources(${AFR_TARGET_APP_NAME} PUBLIC            "${board_src}")
    cy_get_libs(board_libs ITEMS "${board_exe_files}")
    target_link_libraries(${AFR_TARGET_APP_NAME} PUBLIC        "${board_libs}")

    # Find MTB files
    cy_find_files(mtb_files DIRECTORY "$ENV{CY_ARCH_DIR}")
    cy_get_includes(mtb_inc ITEMS "${mtb_files}" ROOT "$ENV{CY_ARCH_DIR}")
    target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC "${mtb_inc}")
    cy_get_src(mtb_src ITEMS "${mtb_files}")
    target_sources(${AFR_TARGET_APP_NAME} PUBLIC             "${mtb_src}")
    cy_get_libs(mtb_libs ITEMS "${mtb_files}")
    target_link_libraries(${AFR_TARGET_APP_NAME} PUBLIC        "${mtb_libs}")

    # -------------------------------------------------------------------------------------------------
    # Compiler settings
    # -------------------------------------------------------------------------------------------------
    # If you support multiple compilers, you can use AFR_TOOLCHAIN to conditionally define the compiler
    # settings. This variable will be set to the file name of CMAKE_TOOLCHAIN_FILE. It might also be a
    # good idea to put your compiler settings to different files and just include them here, e.g.,
    # include(compilers/${AFR_TOOLCHAIN}.cmake)

    afr_mcu_port(compiler)
    cy_add_args_to_target(
        AFR::compiler::mcu_port INTERFACE
        OPTIMIZATION "$ENV{OPTIMIZATION}"
        DEBUG_FLAG "$ENV{DEBUG_FLAG}"
        DEFINE_FLAGS "$ENV{DEFINE_FLAGS}"
        COMMON_FLAGS "$ENV{COMMON_FLAGS}"
        VFP_FLAGS "$ENV{VFP_FLAGS}"
        CORE_FLAGS "$ENV{CORE_FLAGS}"
        ASFLAGS "$ENV{ASFLAGS}"
        LDFLAGS "$ENV{LDFLAGS}"
    )
    cy_add_std_defines(AFR::compiler::mcu_port INTERFACE)
    target_compile_definitions(
        AFR::compiler::mcu_port INTERFACE
        CYBSP_WIFI_CAPABLE
        CY_RTOS_AWARE
        CY_USING_HAL
        $<$<C_COMPILER_ID:GNU>:LWIP_ERRNO_STDINCLUDE>
        $<$<C_COMPILER_ID:ARMCC>:LWIP_ERRNO_INCLUDE="cy_lwip_errno_armcc.h">
        $<$<C_COMPILER_ID:ARMClang>:LWIP_ERRNO_INCLUDE="cy_lwip_errno_armcc.h">
        $<$<C_COMPILER_ID:IAR>:LWIP_ERRNO_INCLUDE="cy_lwip_errno_iar.h">
        ${ARG_DEFINES}
    )
    target_include_directories(
        AFR::compiler::mcu_port INTERFACE
        "${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/lwip"
    )

    # -------------------------------------------------------------------------------------------------
    # Amazon FreeRTOS portable layers
    # -------------------------------------------------------------------------------------------------
    # Define portable layer targets with afr_mcu_port(<module_name>). We will create an CMake
    # INTERFACE IMPORTED target called AFR::${module_name}::mcu_port for you. You can use it with
    # standard CMake functions like target_*. To better organize your files, you can define your own
    # targets and use target_link_libraries(AFR::${module_name}::mcu_port INTERFACE <your_targets>)
    # to provide the public interface you want expose.

    # Kernel
    afr_mcu_port(kernel)
    file(GLOB cy_freertos_port_src ${AFR_KERNEL_DIR}/portable/$ENV{CY_FREERTOS_TOOLCHAIN}/ARM_CM4F/*.[chsS])
    target_sources(
        AFR::kernel::mcu_port
        INTERFACE
        ${cy_freertos_port_src}
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
    )

    target_include_directories(AFR::kernel::mcu_port INTERFACE
        ${app_inc}
        ${mtb_inc}
        "${AFR_KERNEL_DIR}/include"
        "${AFR_KERNEL_DIR}/portable/$ENV{CY_FREERTOS_TOOLCHAIN}/ARM_CM4F"	# for portmacro.h
        "${aws_config_dir}"                                                 # for FreeRTOSconfig.h
        "${iot_common_include}"                                             # for iot_config_common.h
    )

    target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        3rdparty::tinycrypt
    )

    add_library(CyObjStore INTERFACE)
    target_sources(CyObjStore INTERFACE
        "${cy_psoc6_dir}/mw/objstore/cyobjstore.c"
        "${cy_psoc6_dir}/mw/emeeprom/cy_em_eeprom.c"
    )
    target_include_directories(CyObjStore INTERFACE
        "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include/COMPONENT_FREERTOS"
        "${cy_psoc6_dir}/mw/emeeprom"
        "${cy_psoc6_dir}/mw/objstore"
    )

    # WiFi
    afr_mcu_port(wifi)
    target_sources(
        AFR::wifi::mcu_port
        INTERFACE
        "${afr_ports_dir}/wifi/iot_wifi.c"
        "${afr_ports_dir}/wifi/iot_wifi_lwip.c"
        "${afr_ports_dir}/wifi/emac_eapol.h"
        "${afr_ports_dir}/wifi/iot_wifi_common.h"
    )
    target_include_directories(AFR::wifi::mcu_port INTERFACE
        "${afr_ports_dir}/wifi"
        "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include/COMPONENT_FREERTOS"
        "${cy_psoc6_dir}/common"
        "${cy_psoc6_dir}/mw/objstore"
        "${cy_whd_dir}/src/include"
    )
    target_link_libraries(
        AFR::wifi::mcu_port
        INTERFACE
        3rdparty::lwip
        3rdparty::lwip_osal
    )

    # BLE 
    if(CY_BLE_SUPPORTED)
        set(BLE_SUPPORTED 1 CACHE INTERNAL "BLE is supported on this platform.")

        afr_mcu_port(ble_hal DEPENDS CyObjStore)
        target_sources(
            AFR::ble_hal::mcu_port
            INTERFACE
            "${afr_ports_dir}/ble/iot_ble_hal_manager.c"
            "${afr_ports_dir}/ble/iot_ble_hal_manager_adapter_ble.c"
            "${afr_ports_dir}/ble/iot_ble_hal_gatt_server.c"
            "${afr_ports_dir}/ble/wiced_bt_cfg.c"
            "${afr_ports_dir}/ble/bt_hal_internal.h"
            "${cy_bluetooth_dir}/psoc6/cyosal/src/cybt_osal_amzn_freertos.c"
            "${cy_bluetooth_dir}/psoc6/cyosal/src/wiced_time_common.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform_gpio.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform_clock.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform_uart.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform_bluetooth.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/platform_bt_nvram.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/ring_buffer.c"
            "${cy_bluetooth_dir}/psoc6/cyhal/src/bt_firmware_controller.c"
        )

        target_include_directories(AFR::ble_hal::mcu_port INTERFACE
            "${afr_ports_dir}/ble"
            "${cy_bluetooth_dir}/psoc6/cyosal/include"
            "${cy_bluetooth_dir}/psoc6/cyhal/include"
            "${cy_bluetooth_dir}/include"
            "${cy_bluetooth_dir}/include/stackHeaders"
        )

        target_link_libraries(
            AFR::ble_hal::mcu_port
            INTERFACE
            "${cy_bluetooth_dir}/bluetooth.FreeRTOS.ARM_CM4.release.a"
            "${cy_bluetooth_dir}/shim.FreeRTOS.ARM_CM4.release.a"
        )

    endif()

    # Secure sockets

    afr_mcu_port(secure_sockets)

    target_link_libraries(
        AFR::secure_sockets::mcu_port
        INTERFACE
        AFR::tls
        AFR::secure_sockets_lwip
        3rdparty::lwip_osal
    )

    # PKCS11
    afr_mcu_port(pkcs11_implementation DEPENDS CyObjStore)

    target_sources(
        afr_3rdparty_mbedtls
        INTERFACE
        "${afr_ports_dir}/pkcs11/hw_poll.c"
    )

    target_sources(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE
        "${afr_ports_dir}/pkcs11/hw_poll.c"
    )

    target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
        "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include"
    )

    if(CY_TFM_PSA)
        # Link to AFR::pkcs11_psa use implementation based on TF-M PSA.
        target_sources(
            AFR::pkcs11_implementation::mcu_port
            INTERFACE
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa.c"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_input_format.c"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_input_format.h"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_jitp_status.c"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_jitp_status.h"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_object_management.c"
            "${cy_board_dir}/ports/pkcs11/psa/iot_pkcs11_psa_object_management.h"
        )
        target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
            "${iot_common_include}"
        )

    else()
        # Link to AFR::pkcs11_mbedtls if you want to use default implemenƒtation based on mbedtls.
        target_link_libraries(
            AFR::pkcs11_implementation::mcu_port
            INTERFACE
            AFR::pkcs11_mbedtls
        )

        target_sources(
            AFR::pkcs11_implementation::mcu_port
            INTERFACE
            "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
        )
    endif()

    target_include_directories(afr_3rdparty_mbedtls INTERFACE
        "${cy_psoc6_dir}/psoc6csp/hal/include"
        "${cy_psoc6_dir}/psoc6csp/core_lib/include"
        "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
        "${cy_psoc6_dir}/psoc6pdl/devices/include"
        "${cy_psoc6_dir}/psoc6pdl/drivers/include"
        "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include"
        "${cy_code_dir}"
    )

    if(CY_TFM_PSA)
    target_include_directories(afr_3rdparty_mbedtls INTERFACE
        "${cy_psoc6_dir}/psoc64tfm/COMPONENT_TFM_NS_INTERFACE/include"
    )
    endif()

    target_include_directories(afr_3rdparty_lwip PUBLIC
        "${cy_psoc6_dir}/psoc6csp/hal/include"
        "${cy_psoc6_dir}/psoc6csp/core_lib/include"
        "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
        "${cy_psoc6_dir}/psoc6pdl/devices/include"
        "${cy_psoc6_dir}/psoc6pdl/drivers/include"
        "${cy_whd_dir}/inc"
        "${cy_code_dir}"
        "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include"
    )


    #----------------------------------------------------------------
    # is SDIO supported?
    if(CYHAL_UDB_SDIO)
        target_include_directories(AFR::kernel::mcu_port INTERFACE
            "${cy_code_dir}/SDIO_HOST"
        )

        target_include_directories( AFR::secure_sockets::mcu_port INTERFACE
            "${cy_code_dir}/SDIO_HOST"
            )

        target_include_directories(afr_3rdparty_mbedtls PUBLIC
            "${cy_code_dir}/SDIO_HOST"
            )
    endif()

    target_link_libraries(${AFR_TARGET_APP_NAME}  PUBLIC
        AFR::utils
        AFR::wifi
        AFR::wifi::mcu_port
        )

    #----------------------------------------------------------------
    if(CY_BLE_SUPPORTED)
        target_link_libraries(${AFR_TARGET_APP_NAME}  PUBLIC
            AFR::ble
            AFR::ble_hal::mcu_port
            )
    endif()

    set(CMAKE_EXECUTABLE_SUFFIX ".elf" PARENT_SCOPE)

    # If we are using our own app, remove aws_demos and aws_tests directories
    # We want to keep the cy_code directory, save and restore those files to the list
    if(CY_ALTERNATE_APP)
        # Save the cy_code
        set(save_cy_code "${exe_src}")
        list(FILTER save_cy_code INCLUDE REGEX "cy_code")

        # filter out the default build app sources
        # Do both in aws_demos and aws_tests
        list(FILTER exe_src EXCLUDE REGEX "aws_demos")
        list(FILTER exe_src EXCLUDE REGEX "aws_tests")

        # add the saved cy_code back into the build
        set(exe_src "${exe_src}" "${save_cy_code}")

        #----------------------------------------------------------------
        # Remove unwanted modules from the build
        # Create a list in your <application_dir>/CMakeLists.txt file
        #
        # ex: set(CY_APP_DISABLE_AFR_MODULES
        #           "defender"
        #           "mqtt"
        #           "greengrass"
        #           )
        #
        foreach(module IN LISTS CY_APP_DISABLE_AFR_MODULES)
           # message("cy_kit_utils.cmake: disable module ${module}")
           afr_module_dependencies(${module} INTERFACE 3rdparty::does_not_exist)
        endforeach()
    endif(CY_ALTERNATE_APP)

    #----------------------------------------------------------------
    # OTA SUPPORT
    #----------------------------------------------------------------
    if(OTA_SUPPORT)
        target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC
            "${MCUBOOT_CYFLASH_PAL_DIR}/include"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi"
            "${MCUBOOT_DIR}/sysflash"
            )

        #----------------------------------------------------------------
        # Add Linker options
        #
        if(MCUBOOT_HEADER_SIZE)
            if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "-Wl,--defsym,MCUBOOT_HEADER_SIZE=${MCUBOOT_HEADER_SIZE}")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "--pd=\"-DMCUBOOT_HEADER_SIZE=${MCUBOOT_HEADER_SIZE}\"")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "SHELL:--define_symbol MCUBOOT_HEADER_SIZE=${MCUBOOT_HEADER_SIZE}")
            endif()
        endif()
        if(MCUBOOT_BOOTLOADER_SIZE)
            if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "-Wl,--defsym,MCUBOOT_BOOTLOADER_SIZE=${MCUBOOT_BOOTLOADER_SIZE}")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "--pd=\"-DMCUBOOT_BOOTLOADER_SIZE=${MCUBOOT_BOOTLOADER_SIZE}\"")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "SHELL: --define_symbol MCUBOOT_BOOTLOADER_SIZE=${MCUBOOT_BOOTLOADER_SIZE}")
            endif()
        endif()
        if(CY_BOOT_PRIMARY_1_SIZE)
            if ("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "-Wl,--defsym,CY_BOOT_PRIMARY_1_SIZE=${CY_BOOT_PRIMARY_1_SIZE}")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "--pd=\"-DCY_BOOT_PRIMARY_1_SIZE=${CY_BOOT_PRIMARY_1_SIZE}\"")
            elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
                target_link_options(${AFR_TARGET_APP_NAME} PUBLIC "SHELL: --define_symbol CY_BOOT_PRIMARY_1_SIZE=${CY_BOOT_PRIMARY_1_SIZE}")
            endif()
        endif()

        #----------------------------------------------------------------
        # Add AWS OTA Library mcu_port

        afr_mcu_port(ota)
        
        # need these here for the mcu_port
        target_compile_definitions(AFR::ota::mcu_port INTERFACE
            "-DOTA_SUPPORT=1"
            "-DMCUBOOT_KEY_FILE=${MCUBOOT_KEY_FILE}"
            "-DCY_FLASH_ERASE_VALUE=${CY_FLASH_ERASE_VALUE}"
            "-DMCUBOOT_HEADER_SIZE=${MCUBOOT_HEADER_SIZE}"
            "-DCY_BOOT_SCRATCH_SIZE=${CY_BOOT_SCRATCH_SIZE}"
            "-DCY_BOOT_BOOTLOADER_SIZE=${MCUBOOT_BOOTLOADER_SIZE}"
            "-DMCUBOOT_BOOTLOADER_SIZE=${MCUBOOT_BOOTLOADER_SIZE}"
            "-DCY_BOOT_PRIMARY_1_START=${CY_BOOT_PRIMARY_1_START}"
            "-DCY_BOOT_PRIMARY_1_SIZE=${CY_BOOT_PRIMARY_1_SIZE}"
            "-DCY_BOOT_SECONDARY_1_SIZE=${CY_BOOT_PRIMARY_1_SIZE}"
            "-DCY_BOOT_SECONDARY_2_START=${CY_BOOT_SECONDARY_2_START}"
            "-DCY_BOOT_SECONDARY_2_SIZE=${CY_BOOT_PRIMARY_2_SIZE}"
            "-DMCUBOOT_MAX_IMG_SECTORS=${MCUBOOT_MAX_IMG_SECTORS}"
        )
        # common ota sources
        target_sources(AFR::ota::mcu_port INTERFACE
            "${cy_ota_dir}/ports/${AFR_BOARD_NAME}/aws_ota_pal.c"
            "${AFR_DEMOS_DIR}/ota/aws_iot_ota_update_demo.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_flash_map.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_flash_psoc6.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi/flash_qspi.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_smif_psoc6.c"
            "${MCUBOOT_DIR}/bootutil/src/bootutil_misc.c"
            "${cy_common_dir}/utilities/untar/untar.c"
            "${cy_common_dir}/utilities/JSON_parser/JSON.c"
        )

        # common ota includes
        target_include_directories(AFR::ota::mcu_port INTERFACE
            "${AFR_DEMOS_DIR}/network_manager"
            "${MCUBOOT_DIR}"
            "${MCUBOOT_DIR}/bootutil/include"
            "${MCUBOOT_DIR}/mcuboot_header"
            "${MCUBOOT_DIR}/sysflash"
            "${MCUBOOT_CYFLASH_PAL_DIR}"
            "${MCUBOOT_CYFLASH_PAL_DIR}/include"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi"
            "${cy_common_dir}/utilities/untar"
            "${cy_common_dir}/utilities/JSON_parser"
            "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include"
            "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
            "${cy_psoc6_dir}/psoc6pdl/devices/include"
            "${cy_psoc6_dir}/psoc6pdl/drivers/include"
            "${aws_config_dir}"                                                 # for FreeRTOSconfig.h
            "${iot_common_include}"                                             # for iot_config_common.h
            "${cy_code_dir}"                                                    # for system_psoc6.h
            "${cy_code_dir}/GeneratedSource"
        )

        if(CY_ALTERNATE_APP)
            # Normal OTA build
            # Add extra sources for our port
            target_sources(AFR::ota::mcu_port INTERFACE
                "${AFR_DEMOS_DIR}/demo_runner/aws_demo_version.c"
                "${AFR_DEMOS_DIR}/demo_runner/iot_demo_freertos.c"
                "${AFR_DEMOS_DIR}/demo_runner/iot_demo_runner.c"
                "${AFR_DEMOS_DIR}/network_manager/aws_iot_demo_network.c"
                "${AFR_DEMOS_DIR}/network_manager/aws_iot_network_manager.c"
            )

            # add extra includes
            target_include_directories(AFR::ota::mcu_port INTERFACE
                "${cy_psoc6_dir}/psoc6csp/abstraction/rtos/include"
                "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
                "${cy_psoc6_dir}/psoc6pdl/devices/include"
                "${cy_psoc6_dir}/psoc6pdl/drivers/include"
                "${aws_config_dir}"                                                 # for FreeRTOSconfig.h
                "${iot_common_include}"                                             # for iot_config_common.h
                "${cy_code_dir}"                                                    # for system_psoc6.h
            )

        elseif(AFR_IS_TESTING)
            # For aws_tests build for OTA and OTA PAL
            # Defines so OTA part of build will not have errors
            set(APP_VERSION_MAJOR 0)
            set(APP_VERSION_MINOR 9)
            set(APP_VERSION_BUILD 0)
            # add files to build for testing
            target_include_directories( AFR::ota::mcu_port INTERFACE
                "${AFR_ROOT_DIR}/tests/include"
                "${cy_board_dir}/aws_tests/config_files"
                "${AFR_MODULES_C_SDK_DIR}/standard/https/include"
                "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src"
                "${AFR_ROOT_DIR}/demos/include"
                "${AFR_ROOT_DIR}/demos/dev_mode_key_provisioning/include"
                "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/pkcs11/include"
                "${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/src/mqtt"
                "${AFR_3RDPARTY_DIR}/pkcs11"
            )
     
        else()
            # For aws_demos builds for OTA demos
            # Defines so OTA part of build will not have errors
            set(APP_VERSION_MAJOR 0)
            set(APP_VERSION_MINOR 9)
            set(APP_VERSION_BUILD 0)
            # add extra includes
            target_include_directories(AFR::ota::mcu_port INTERFACE
                "${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/test"
                "${cy_board_dir}/aws_demos/config_files"
            )
        endif()

        # Add version defines
        target_compile_definitions(AFR::ota::mcu_port INTERFACE
            "-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR}"
            "-DAPP_VERSION_MINOR=${APP_VERSION_MINOR}"
            "-DAPP_VERSION_BUILD=${APP_VERSION_BUILD}"
        )

        target_link_libraries(AFR::ota::mcu_port INTERFACE
            AFR::ota_mqtt
            AFR::ota_http
            AFR::crypto
            AFR::wifi
        )
            
        # link libs to our app
        target_link_libraries(${AFR_TARGET_APP_NAME} PUBLIC
            "AFR::ota"
            "afr_dev_mode_key_provisioning"
            "AFR::https"
        )

        # extra OTA includes for pkcs11 and the kernel 
        target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
            "${MCUBOOT_DIR}/mcuboot_header"
        )
        target_include_directories(AFR::kernel::mcu_port INTERFACE
            "${MCUBOOT_DIR}/mcuboot_header"
        )
        
        if(NOT AFR_METADATA_MODE)
            if(CY_TFM_PSA)
                # TFM uses a different linker script and signing tool
                # see next if() section after endif(CY_ALTERNATE_APP)
            else()
                # non-TFM signing

                #------------------------------------------------------------
                # Create our script filename in this scope
                set(SIGN_SCRIPT_FILE_NAME           "sign_${AFR_TARGET_APP_NAME}.sh")
                set(SIGN_SCRIPT_FILE_PATH           "${CMAKE_BINARY_DIR}/${SIGN_SCRIPT_FILE_NAME}")
                set(SIGN_SCRIPT_FILE_PATH_TMP       "${CMAKE_BINARY_DIR}/tmp/${SIGN_SCRIPT_FILE_NAME}")
                set(CY_OUTPUT_FILE_PATH             "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}")
                set(CY_OUTPUT_FILE_PATH_ELF         "${CY_OUTPUT_FILE_PATH}.elf")
                set(CY_OUTPUT_FILE_PATH_HEX         "${CY_OUTPUT_FILE_PATH}.hex")
                set(CY_OUTPUT_FILE_PATH_SIGNED_HEX  "${CY_OUTPUT_FILE_PATH}.signed.hex")
                set(CY_OUTPUT_FILE_NAME_BIN         "${AFR_TARGET_APP_NAME}.bin")
                set(CY_OUTPUT_FILE_PATH_BIN         "${CY_OUTPUT_FILE_PATH}.bin")
                set(CY_OUTPUT_FILE_PATH_TAR         "${CY_OUTPUT_FILE_PATH}.tar")
                set(CY_OUTPUT_FILE_PATH_WILD        "${CY_OUTPUT_FILE_PATH}.*")
                set(CY_COMPONENTS_JSON_NAME         "components.json")
                set(CY_OUTPUT_FILE_NAME_TAR         "${AFR_TARGET_APP_NAME}.tar")

                # creates the script to call imgtool.py to sign the image
                config_cy_mcuboot_sign_script("${CMAKE_BINARY_DIR}")

                add_custom_command(
                    TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                    COMMAND "${SIGN_SCRIPT_FILE_PATH}"
                    )
            endif()
        endif()
    else()
        target_include_directories(${AFR_TARGET_APP_NAME} PUBLIC
        "${MCUBOOT_DIR}/sysflash"
        )
    endif(OTA_SUPPORT)
        

    if(CY_TFM_PSA AND (NOT AFR_METADATA_MODE))
        set(CY_AWS_ELF  "${CMAKE_BINARY_DIR}/aws.elf")
        set(CY_CM0_IMG "${CMAKE_BINARY_DIR}/cm0.hex")
        set(CY_CM4_IMG "${CMAKE_BINARY_DIR}/cm4.hex")
        set(CY_CM0_UNSIGNED_IMG "${CMAKE_BINARY_DIR}/cm0_unsigned.hex")
        set(CY_CM4_UNSIGNED_IMG "${CMAKE_BINARY_DIR}/cm4_unsigned.hex")
        # for file suitable for uploading to AWS
        set(CY_APP_CM0_BIN "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}_cm0.bin")
        set(CY_APP_CM4_BIN "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}_cm4.bin")
        set(CY_CM0_UPGRADE_IMG "${CMAKE_BINARY_DIR}/cm0_upgrade.hex")
        set(CY_CM4_UPGRADE_IMG "${CMAKE_BINARY_DIR}/cm4_upgrade.hex")
        set(CY_APP_CM0_UPGRADE_BIN "${CMAKE_BINARY_DIR}/cm0_upgrade.bin")
        set(CY_APP_CM4_UPGRADE_BIN "${CMAKE_BINARY_DIR}/cm4_upgrade.bin")

        if(NOT CY_TFM_HEX)
            message(FATAL_ERROR "You must define CY_TFM_HEX in your board CMakeLists.txt for CY_TFM_PSA")
        endif()
        if(NOT CY_TFM_POLICY_FILE)
            message(FATAL_ERROR "You must define CY_TFM_POLICY_FILE in your board CMakeLists.txt for CY_TFM_PSA")
        endif()
        if(NOT CY_DEVICE_NAME)
            message(FATAL_ERROR "You must define CY_DEVICE_NAME in your board CMakeLists.txt for CY_TFM_PSA")
        endif()

        if("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
            find_program(GCC_OBJCOPY arm-none-eabi-objcopy)
            if(NOT GCC_OBJCOPY )
                message(FATAL_ERROR "Cannot find arm-none-eabi-objcopy.")
            endif()

            # Workaround the signing issue by removing the sFlash sections
            add_custom_command(
            TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
            COMMAND "${GCC_OBJCOPY}" -R .cy_sflash_user_data -R .cy_toc_part2 "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}.elf" "${CY_AWS_ELF}"
            )
            # Generate HEX file
            add_custom_command(
                TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
                COMMAND "${GCC_OBJCOPY}" -O ihex "${CY_AWS_ELF}" "${CY_CM4_IMG}"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-armclang")
            find_program(FROMELF_TOOL fromelf)
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find fromelf tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --i32 --output="${CY_CM4_IMG}" "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}.elf"
            )
        elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
        find_program(FROMELF_TOOL ielftool)
            if(NOT FROMELF_TOOL )
                message(FATAL_ERROR "Cannot find ielftool tool")
            endif()

            # Generate HEX file
            add_custom_command(
                TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
                COMMAND ${FROMELF_TOOL} --ihex "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}.elf" "${CY_CM4_IMG}"
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
            TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${CY_TFM_HEX}"    "${CY_CM0_IMG}"
            #The output of cysecuretools for signing upgrade image is <name>_upgrade.hex. The original file is rename as <name>_unsigned.hex
            COMMAND "${CY_SIGN_SCRIPT}" --policy "${CY_TFM_POLICY_FILE}" --target "${CY_DEVICE_NAME}" sign-image --hex "${CY_CM0_IMG}" --image-type UPGRADE --image-id 1
            COMMAND "${CY_SIGN_SCRIPT}" --policy "${CY_TFM_POLICY_FILE}" --target "${CY_DEVICE_NAME}" sign-image --hex "${CY_CM4_IMG}" --image-type UPGRADE --image-id 16
            COMMAND "${CMAKE_COMMAND}" -E copy "${CY_CM0_UNSIGNED_IMG}"  "${CY_CM0_IMG}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${CY_CM4_UNSIGNED_IMG}"  "${CY_CM4_IMG}"
            #For signing boot image, cysecuretools over-writes the original file.
            COMMAND "${CY_SIGN_SCRIPT}" --policy "${CY_TFM_POLICY_FILE}" --target "${CY_DEVICE_NAME}" sign-image --hex "${CY_CM0_IMG}" --image-type BOOT --image-id 1
            COMMAND "${CY_SIGN_SCRIPT}" --policy "${CY_TFM_POLICY_FILE}" --target "${CY_DEVICE_NAME}" sign-image --hex "${CY_CM4_IMG}" --image-type BOOT --image-id 16

        )
        #convert signed hex files to binary format
        #CLANG and IAR do not provide a tool, search for a generic objcopy
        if(NOT GCC_OBJCOPY )
            find_program(GCC_OBJCOPY objcopy arm-none-eabi-objcopy)
            if(NOT GCC_OBJCOPY )
                message(FATAL_ERROR "Cannot find objcopy.")
            endif()
        endif()

        if(OTA_SUPPORT)
            #------------------------------------------------------------
            # Create our build_tar script
            set(TAR_SCRIPT_FILE_NAME            "tar_${AFR_TARGET_APP_NAME}.sh")
            set(TAR_SCRIPT_FILE_PATH            "${CMAKE_BINARY_DIR}/${TAR_SCRIPT_FILE_NAME}")
            set(TAR_SCRIPT_FILE_PATH_TMP        "${CMAKE_BINARY_DIR}/tmp/${TAR_SCRIPT_FILE_NAME}")
            set(CY_OUTPUT_FILE_PATH             "${CMAKE_BINARY_DIR}/${AFR_TARGET_APP_NAME}")
            get_filename_component(CY_OUTPUT_FILE_CM0_NAME_BIN ${CY_APP_CM0_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM4_NAME_BIN ${CY_APP_CM4_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM0_UPGRADE_BIN ${CY_APP_CM0_UPGRADE_BIN} NAME)
            get_filename_component(CY_OUTPUT_FILE_CM4_UPGRADE_BIN ${CY_APP_CM4_UPGRADE_BIN} NAME)
    
            set(CY_OUTPUT_FILE_PATH_BIN         "${CY_OUTPUT_FILE_PATH}")
           
            set(CY_COMPONENTS_JSON_NAME         "components.json")
    
            # Create version for tar_XXXX.sh
            set(CY_BUILD_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_BUILD}")
    
            set(CY_CM4_ONLY_TAR   "cm4_only_signed.tar")
            set(CY_CM4_CM0_TAR    "cm4_cm0_signed.tar")
    
            set(CY_CM4_ONLY_UPGRADE_TAR "cm4_only_upgrade.tar")
            set(CY_CM4_CM0_UPGRADE_TAR  "cm4_cm0_upgrade.tar")
        
            # create the script to call imgtool.py to sign the image
            configure_file("${cy_psoc6_dir}/cmake/tar.sh.in" "${TAR_SCRIPT_FILE_PATH_TMP}" @ONLY NEWLINE_STYLE LF)
            # and then make sure it is executable on all platforms
            file( COPY "${TAR_SCRIPT_FILE_PATH_TMP}" DESTINATION "${CMAKE_BINARY_DIR}"
                  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            )
    
            add_custom_command(
                TARGET "${AFR_TARGET_APP_NAME}" POST_BUILD
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                COMMAND "${CMAKE_COMMAND}" -E remove -f "${CY_APP_CM0_BIN}" "${CY_APP_CM4_BIN}" "${CY_APP_CM0_UPGRADE_BIN}" "${CY_APP_CM4_UPGRADE_BIN}"
                COMMAND "${TAR_SCRIPT_FILE_PATH}"
            )
        endif(OTA_SUPPORT)

    endif(CY_TFM_PSA AND (NOT AFR_METADATA_MODE))

endfunction(cy_kit_generate)
