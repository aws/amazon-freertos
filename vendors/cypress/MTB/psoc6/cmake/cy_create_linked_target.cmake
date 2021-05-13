
function(cy_cfg_env)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "TARGET;DEVICE;TOOLCHAIN"
    ""
    )

    if(NOT DEFINED ARG_TARGET)
    message(FATAL_ERROR "Missing TARGET argument.")
    endif()

    if(NOT DEFINED ARG_DEVICE)
    message(FATAL_ERROR "Missing DEVICE argument.")
    endif()

    if(NOT DEFINED ARG_TOOLCHAIN)
    message(FATAL_ERROR "Missing TOOLCHAIN argument.")
    endif()

    #####################################
    # set CY_TARGET
    set(ENV{CY_TARGET} "${ARG_TARGET}")

    #####################################
    # set CY_DEVICE
    set(ENV{CY_DEVICE} "${ARG_DEVICE}")

    #####################################
    # set CY_CORE
    if ("${CORE}" STREQUAL "")
    set(ENV{CY_CORE} "CM4")
    else()
    set(ENV{CY_CORE} "${CORE}")
    endif()

    #####################################
    # map AFR toolchain names to cypress toolchain names
    # TODO make this support multiple toolchain and flash memory sizes
    if ("${ARG_TOOLCHAIN}" STREQUAL "arm-gcc")
      set(ARG_TOOLCHAIN GCC_ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-armclang")
      set(ARG_TOOLCHAIN ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-iar")
      set(ARG_TOOLCHAIN IAR)
    endif()
    set(ENV{CY_TOOLCHAIN} "${ARG_TOOLCHAIN}")

    cy_cfg_toolchain()
endfunction()

function(cy_add_toolchain_arg_to_target target mode)
    cmake_parse_arguments(
    PARSE_ARGV 2
    "ARG"
    ""
    "OPTIMIZATION;DEBUG_FLAG;DEFINE_FLAGS;COMMON_FLAGS;VFP_FLAGS;CORE_FLAGS;CFLAGS;CXXFLAGS;ASFLAGS;LDFLAGS"
    ""
    )

    set(_all_cflags ${ARG_CORE_FLAGS} ${ARG_OPTIMIZATION} ${ARG_VFP_FLAGS} ${ARG_COMMON_FLAGS})
    target_compile_options(
        ${target}
        ${mode}
        $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:CXX>>:${_all_cflags}>
    )

    target_compile_options(
        ${target}
        ${mode}
        $<$<COMPILE_LANGUAGE:ASM>:${ARG_ASFLAGS}>
    )

    if("${CMAKE_ASM_COMPILER}" STREQUAL "${CMAKE_C_COMPILER}")
        # If using compiler frontend for assembler then pass cflags when assembling
        target_compile_options(
            ${target}
            ${mode}
            $<$<COMPILE_LANGUAGE:ASM>:${_all_cflags}>
        )
    endif()

    target_compile_definitions(
        ${target}
        ${mode}
        $<$<CONFIG:Debug>:${ARG_DEBUG_FLAG}>
        ${ARG_DEFINE_FLAGS}
    )

    if("$ENV{CY_TOOLCHAIN}" STREQUAL "IAR")
        # IAR has a separate linker command that does not use the same options as the compiler.
        target_link_options(
            ${target}
            ${mode}
            ${ARG_CORE_FLAGS}
            ${ARG_VFP_FLAGS}
            ${ARG_LDFLAGS}
        )
    elseif("$ENV{CY_TOOLCHAIN}" STREQUAL "ARM")
        # ARM has a separate linker command that does not use the same options as the compiler.
        target_link_options(
            ${target}
            ${mode}
            ${ARG_LDFLAGS}
        )
    else()
        # GNU based toolchain
        target_link_options(
            ${target}
            ${mode}
            ${ARG_CORE_FLAGS}
            ${ARG_VFP_FLAGS}
            ${ARG_COMMON_FLAGS}
            ${ARG_LDFLAGS}
        )
    endif()
endfunction()


function(cy_add_link_libraries)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "DEVICE;PORT_DIR;BSP_DIR;CONFIG_DIRS;INCLUDE_DIRS;COMPONENTS;DEFINES"
    ""
    )
    string(FIND "${ARG_DEFINES}" "CYHAL_UDB_SDIO" check_sdio)
    if (NOT ("${check_sdio}" STREQUAL "-1"))
        set(CYHAL_UDB_SDIO "1")
    endif()

    #--------------------------------------------------------------------
    # Utilities
    #--------------------------------------------------------------------
    if(EXISTS "${cy_psoc6_cmake_dir}/toolchains/${AFR_TOOLCHAIN}.cmake")
        include("${cy_psoc6_cmake_dir}/toolchains/${AFR_TOOLCHAIN}.cmake")
    elseif(AFR_METADATA_MODE)
        function(cy_cfg_toolchain)
        endfunction()
        set(ENV{CY_FREERTOS_TOOLCHAIN} GCC)
    else()
        message(FATAL_ERROR "Unsupported toolchain: ${AFR_TOOLCHAIN}")
    endif()

    # For now, the "cy_code" directory is under <board>/<app_name>/application_code/cy_code
    # Eventually we want ARG_BSP_DIR to be under the board directory, not the application
    # Allow over-rides of the linker script location

    cy_cfg_env(
        TARGET        "${AFR_BOARD_NAME}"
        DEVICE        "${ARG_DEVICE}"
        TOOLCHAIN     "${AFR_TOOLCHAIN}"
    )

    set(ENV{CY_COMPONENTS} "${AFR_BOARD_NAME};${ARG_COMPONENTS};SOFTFP;BSP_DESIGN_MODUS;PSOC6HAL;FREERTOS;$ENV{CY_CORE}")
    set(CY_ARCH_DIR
        "${cy_libraries_clib_dir}"
        "${cy_psoc6_dir}"
        "${cy_libraries_whd_dir}/WiFi_Host_Driver"
        "${cy_libraries_capsense_dir}"
        "${cy_libraries_dir}/abstraction-rtos"
        "${cy_libraries_dir}/core-lib"
        "${cy_libraries_dir}/retarget-io"
        "${cy_libraries_dir}/rgb-led"
        "${cy_libraries_dir}/serial-flash"
        "${cy_libraries_dir}/whd-bsp-integration")

    if(COMPONENTS)
        set(ENV{CY_COMPONENTS} "$ENV{CY_COMPONENTS};${COMPONENTS}")
    endif()

    if(BLE_SUPPORTED)
        set(ENV{CY_COMPONENTS} "$ENV{CY_COMPONENTS};WICED_BLE")
        list(APPEND CY_ARCH_DIR ${cy_libararies_bt_dir})
    endif()

    # Find MTB files
    add_library(psoc6_core INTERFACE)
    cy_find_files(mtb_files DIRECTORY "${CY_ARCH_DIR}")
    cy_get_includes(mtb_inc ITEMS "${mtb_files}" ROOT "${CY_ARCH_DIR}")
    list(APPEND mtb_inc ${ARG_CONFIG_DIRS})
    target_include_directories(psoc6_core INTERFACE "${mtb_inc}")
    cy_get_src(mtb_src ITEMS "${mtb_files}")
    target_sources(psoc6_core INTERFACE "${mtb_src}")
    cy_get_libs(mtb_libs ITEMS "${mtb_files}")
    target_link_libraries(psoc6_core INTERFACE "${mtb_libs}")
    target_sources(psoc6_core INTERFACE "${cy_port_support_dir}/FreeRTOS-openocd.c")
    
    # Add the project makefiles
    afr_files_to_console_metadata(
        "${AFR_ROOT_DIR}/projects/cypress/make_support/afr.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_afr_source.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_cypress_source.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_feature_ble.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_feature_ota.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_global_settings.mk"
        "${AFR_ROOT_DIR}/projects/cypress/make_support/mtb_secure_sign.mk"
    )

    # -------------------------------------------------------------------------------------------------
    # Compiler settings
    # -------------------------------------------------------------------------------------------------
    # If you support multiple compilers, you can use AFR_TOOLCHAIN to conditionally define the compiler
    # settings. This variable will be set to the file name of CMAKE_TOOLCHAIN_FILE. It might also be a
    # good idea to put your compiler settings to different files and just include them here, e.g.,
    # include(compilers/${AFR_TOOLCHAIN}.cmake)

    afr_mcu_port(compiler)
    cy_add_toolchain_arg_to_target(
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

    string(REGEX REPLACE - _ tgt_mpn $ENV{CY_DEVICE})
    string(REGEX REPLACE - _ tgt_def $ENV{CY_TARGET})

    set(component_defines "")
    set(CY_COMPONENT_LIST $ENV{CY_COMPONENTS})
    foreach(comp IN LISTS CY_COMPONENT_LIST)
        list(APPEND component_defines "COMPONENT_${comp}")
    endforeach()

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
        ${component_defines}
        ${tgt_mpn}
        TARGET_${tgt_def}
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
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_3.c"
    )

    # is CY_BOOT_USE_EXTERNAL_FLASH supported?
    if ("$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" STREQUAL "1")
        target_compile_definitions(AFR::kernel::mcu_port INTERFACE "-DCY_BOOT_USE_EXTERNAL_FLASH=$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" )
    endif()

    target_include_directories(AFR::kernel::mcu_port INTERFACE
        ${app_inc}
        ${mtb_inc}
        "${AFR_KERNEL_DIR}/include"
        "${AFR_KERNEL_DIR}/portable/$ENV{CY_FREERTOS_TOOLCHAIN}/ARM_CM4F"	# for portmacro.h
        "${AFR_3RDPARTY_DIR}/tinycrypt/lib/include"
        "${ARG_BSP_DIR}/GeneratedSource"
        "${ARG_BSP_DIR}"
    )

    target_link_libraries(
        AFR::kernel::mcu_port
        INTERFACE
            3rdparty::tinycrypt
    )

    add_library(KVStore INTERFACE)
    target_sources(KVStore INTERFACE
        "${cy_port_support_dir}/kvstore/kvstore.c"
        "${cy_libraries_dir}/kv-store/mtb_kvstore.c"
    )
    target_include_directories(KVStore INTERFACE
        "${cy_libraries_dir}/abstraction-rtos/include/COMPONENT_FREERTOS"
        "${cy_libraries_dir}/kv-store"
        "${cy_port_support_dir}/kvstore"
    )

    # WiFi
    afr_mcu_port(wifi)
    target_sources(
        AFR::wifi::mcu_port
        INTERFACE
        "${afr_ports_dir}/wifi/iot_wifi.c"
        "${afr_ports_dir}/wifi/iot_wifi_lwip.c"
        "${cy_port_support_dir}/wifi/cy_wifi_notify.c"
        "${afr_ports_dir}/wifi/emac_eapol.h"
        "${afr_ports_dir}/wifi/iot_wifi_common.h"
    )
    target_include_directories(AFR::wifi::mcu_port INTERFACE
        "${afr_ports_dir}/wifi"
        "${cy_libraries_dir}/abstraction-rtos/include"
        "${cy_libraries_dir}/kv-store"
        "${cy_port_support_dir}/kvstore"
        "${cy_libraries_whd_dir}"
        "${cy_libraries_dir}/whd-bsp-integration"
        "${cy_libraries_whd_dir}/WiFi_Host_Driver/src/include"
        "${cy_port_support_dir}/wifi"
    )
    target_link_libraries(
        AFR::wifi::mcu_port
        INTERFACE
        3rdparty::lwip
        3rdparty::lwip_osal
    )

    # BLE
    if(BLE_SUPPORTED)
        set(BLE_SUPPORTED 1 CACHE INTERNAL "BLE is supported on this platform.")
        afr_mcu_port(ble_hal DEPENDS KVStore)
        target_sources(
            AFR::ble_hal::mcu_port
            INTERFACE
            "${afr_ports_dir}/ble/iot_ble_hal_manager.c"
            "${afr_ports_dir}/ble/iot_ble_hal_manager_adapter_ble.c"
            "${afr_ports_dir}/ble/iot_ble_hal_gatt_server.c"
            "${afr_ports_dir}/ble/wiced_bt_cfg.c"
            "${cy_port_support_dir}/bluetooth/platform_bt_nvram.c"
            "${cy_port_support_dir}/bluetooth/wiced_bt_dynamic_gattdb.c"
            "${afr_ports_dir}/ble/bt_hal_internal.h"
        )

        target_include_directories(AFR::ble_hal::mcu_port INTERFACE
            "${afr_ports_dir}/ble"
            "${cy_libararies_bt_dir}/platform/include"
            "${cy_libararies_bt_dir}/platform/common"
            "${cy_libararies_bt_dir}/wiced_include"
            "${cy_port_support_dir}/bluetooth"
            "${cy_libraries_dir}/lpa/include"
        )

        target_link_libraries(
            AFR::ble_hal::mcu_port
            INTERFACE
        )

        target_compile_definitions(
            AFR::ble_hal::mcu_port
            INTERFACE
            BLE_SUPPORTED=1
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
    afr_mcu_port(pkcs11_implementation DEPENDS KVStore)

    target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
        "${cy_libraries_dir}/abstraction-rtos/include"
    )

    if(CY_TFM_PSA_SUPPORTED)
        target_link_libraries(AFR::pkcs11_implementation::mcu_port INTERFACE
            AFR::pkcs11_psa)
        target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
            "${iot_common_include}"
        )
    else()
        # Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
        target_link_libraries(
            AFR::pkcs11_implementation::mcu_port
            INTERFACE
            AFR::pkcs11_mbedtls
        )
        
        target_sources(
            AFR::pkcs11_implementation::mcu_port
            INTERFACE
            "${afr_ports_dir}/pkcs11/core_pkcs11_pal.c"
        )

    endif()

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
    "${cy_libraries_dir}/abstraction-rtos/include")

    target_include_directories(afr_3rdparty_mbedtls INTERFACE
        "${cy_psoc6_dir}/psoc6hal/include"
        "${cy_libraries_dir}/core-lib/include"
        "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
        "${cy_psoc6_dir}/psoc6pdl/devices/include"
        "${cy_psoc6_dir}/psoc6pdl/drivers/include"
        "${cy_libraries_dir}/abstraction-rtos/include"
        "${ARG_BSP_DIR}"
    )

    if(CY_TFM_PSA_SUPPORTED)
        target_include_directories(afr_3rdparty_mbedtls INTERFACE
            "${cy_psoc6_dir}/psoc64tfm/COMPONENT_TFM_NS_INTERFACE/include"
        )
    endif()

    target_include_directories(afr_3rdparty_lwip PUBLIC
        "${cy_psoc6_dir}/psoc6hal/include"
        "${cy_libraries_dir}/core-lib/include"
        "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
        "${cy_psoc6_dir}/psoc6pdl/devices/include"
        "${cy_psoc6_dir}/psoc6pdl/drivers/include"
        "${cy_libraries_whd_dir}/WiFi_Host_Driver/inc"
        "${ARG_BSP_DIR}"
        "${cy_libraries_dir}/abstraction-rtos/include"
    )

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

    #----------------------------------------------------------------
    # OTA SUPPORT
    #----------------------------------------------------------------
    if(OTA_SUPPORT)
        #----------------------------------------------------------------
        # Add AWS OTA Library
        afr_mcu_port(ota)

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
            target_compile_definitions(AFR::ota::mcu_port INTERFACE
                "-DAPP_VERSION_MAJOR=$ENV{APP_VERSION_MAJOR}"
                "-DAPP_VERSION_MINOR=$ENV{APP_VERSION_MINOR}"
                "-DAPP_VERSION_BUILD=$ENV{APP_VERSION_BUILD}"
            )
        endif()

        if ("$ENV{MCUBOOT_IMAGE_NUMBER}" STREQUAL "")
            set(ENV{MCUBOOT_IMAGE_NUMBER} "1" )
        endif()

        # need these here for the mcu_port
        target_compile_definitions(AFR::ota::mcu_port INTERFACE
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
            "-DCY_BOOT_SECONDARY_2_START=$ENV{CY_BOOT_SECONDARY_2_START}"
            "-DCY_BOOT_SECONDARY_2_SIZE=$ENV{CY_BOOT_PRIMARY_2_SIZE}"
            "-DMCUBOOT_MAX_IMG_SECTORS=$ENV{MCUBOOT_MAX_IMG_SECTORS}"
            "-DCY_RETARGET_IO_CONVERT_LF_TO_CRLF=1"
        )

        # is CY_BOOT_USE_EXTERNAL_FLASH supported?
        if ("$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" STREQUAL "1")
            target_compile_definitions(AFR::ota::mcu_port INTERFACE "-DCY_BOOT_USE_EXTERNAL_FLASH=$ENV{CY_BOOT_USE_EXTERNAL_FLASH}" )
        endif()

        if("$ENV{MCUBOOT_IMAGE_NUMBER}" STREQUAL "2" )
            target_compile_definitions(AFR::ota::mcu_port INTERFACE
                "-DCY_BOOT_PRIMARY_2_START=$ENV{CY_BOOT_PRIMARY_2_START}"
                "-DCCY_BOOT_PRIMARY_2_SIZE=$ENV{CY_BOOT_PRIMARY_2_SIZE}"
                "-DCCY_BOOT_SECONDARY_2_SIZE=$ENV{CY_BOOT_PRIMARY_2_SIZE}"
                "-DCCY_BOOT_SECONDARY_2_START=$ENV{CY_BOOT_SECONDARY_2_START}"
            )
        endif()

        # common ota sources
        target_sources(AFR::ota::mcu_port INTERFACE
            "${AFR_DEMOS_DIR}/ota/aws_iot_ota_update_demo.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_flash_map.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_flash_psoc6.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi/flash_qspi.c"
            "${MCUBOOT_CYFLASH_PAL_DIR}/cy_smif_psoc6.c"
            "${MCUBOOT_DIR}/bootutil/src/bootutil_misc.c"
            "${cy_libraries_dir}/connectivity-utilities/JSON_parser/cy_json_parser.c"
            "${cy_port_support_dir}/untar/untar.c"
        )

        # The ports for the secure board lives in the OTA directory since this is the only board accepted in the upstream amazon-freertos repo
        #( not the cypress port) and since that board is not OTA qualified amazon did not allow putting the ota port in the ports folder with
        # the board. This should be removed when the board gets qualified and at OTA port is moved with the other ports.
        if(CY_TFM_PSA_SUPPORTED)
            target_sources(AFR::ota::mcu_port INTERFACE "${cy_port_support_dir}/ota/ports/${AFR_BOARD_NAME}/aws_ota_pal.c")
        else()
            target_sources(AFR::ota::mcu_port INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c")
        endif()

        target_include_directories(AFR::ota::mcu_port INTERFACE
            "${AFR_DEMOS_DIR}/network_manager"
            "${MCUBOOT_DIR}"
            "${MCUBOOT_DIR}/bootutil/include"
            "${MCUBOOT_DIR}/mcuboot_header"
            "${MCUBOOT_DIR}/sysflash"
            "${MCUBOOT_CYFLASH_PAL_DIR}"
            "${MCUBOOT_CYFLASH_PAL_DIR}/include"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi"
            "${cy_libraries_dir}/connectivity-utilities"
            "${cy_libraries_dir}/connectivity-utilities/JSON_parser"
            "${cy_port_support_dir}/untar"
            "${cy_libraries_dir}/abstraction-rtos/include"
            "${cy_psoc6_dir}/psoc6pdl/cmsis/include"
            "${cy_psoc6_dir}/psoc6pdl/devices/include"
            "${cy_psoc6_dir}/psoc6pdl/drivers/include"
            "${iot_common_include}"
            "${ARG_BSP_DIR}"
            "${ARG_BSP_DIR}/GeneratedSource"
        )

        if(AFR_IS_TESTING)
            # For aws_tests build for OTA and OTA PAL
            # add files to build for testing
            target_include_directories(AFR::ota::mcu_port INTERFACE
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
            # add extra includes
            target_include_directories(AFR::ota::mcu_port INTERFACE
                "${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/test"
                "${cy_board_dir}/aws_demos/config_files"
            )
        endif()

        target_link_libraries(AFR::ota::mcu_port INTERFACE
            AFR::ota_mqtt
            AFR::ota_http
            AFR::crypto
            AFR::wifi
        )

        target_include_directories(psoc6_core INTERFACE
            "${MCUBOOT_CYFLASH_PAL_DIR}/include"
            "${MCUBOOT_CYFLASH_PAL_DIR}/flash_qspi"
            "${MCUBOOT_DIR}/sysflash"
        )
        target_link_libraries(psoc6_core INTERFACE
            AFR::ota
            afr_dev_mode_key_provisioning
            AFR::https
        )

        target_include_directories(AFR::pkcs11_implementation::mcu_port INTERFACE
            "${MCUBOOT_DIR}/mcuboot_header"
            )
        target_include_directories(AFR::kernel::mcu_port INTERFACE
            "${MCUBOOT_DIR}/mcuboot_header"
            )

    endif(OTA_SUPPORT)
endfunction(cy_add_link_libraries)
