set(nrf52840_dir "${AFR_VENDORS_DIR}/nordic/boards/nrf52840-dk")
set(afr_ports_dir "${nrf52840_dir}/ports")
set(nrf5_sdk "${AFR_VENDORS_DIR}/nordic/nRF5_SDK_15.2.0")
set(board_demos_dir "${nrf52840_dir}/aws_demos")
set(board_tests_dir "${nrf52840_dir}/aws_tests")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
else()
    set(board_dir "${board_demos_dir}")
endif()


# -------------------------------------------------------------------------------------------------
# FreeRTOS disabled libraries
# -------------------------------------------------------------------------------------------------
set(AFR_MODULE_defender 0 CACHE INTERNAL "")
# HTTPS is not supported for Nordic as this board does not have WiFi/Ethernet.
set(AFR_MODULE_https 0 CACHE INTERNAL "")


# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "nrf52840-dk")
afr_set_board_metadata(DISPLAY_NAME "nRF52840 DK")
afr_set_board_metadata(DESCRIPTION "Nordic nrf52840-dk Development Kit")
afr_set_board_metadata(VENDOR_NAME "Nordic")
afr_set_board_metadata(FAMILY_NAME "NRF52")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "SeggerStudio")
afr_set_board_metadata(RECOMMENDED_IDE "SeggerStudio")
afr_set_board_metadata(IDE_SeggerStudio_NAME "SEGGER Embedded Studio")
afr_set_board_metadata(IDE_SeggerStudio_COMPILER "segger-arm-gcc")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_SeggerStudio_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/nordic/nrf52840-dk/ses/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()


# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

afr_mcu_port(compiler)
set(
    defined_symbols
    __SIZEOF_WCHAR_T=4
    __ARM_ARCH_7EM__
    __SES_ARM
    __ARM_ARCH_FPV4_SP_D16__
    __SES_VERSION=41600
    DEBUG
    DEBUG_NRF
    NRF52840_XXAA
    BOARD_PCA10056
    CONFIG_GPIO_AS_PINRESET
    ENABLE_DEBUG_LOG_SUPPORT
    FLOAT_ABI_HARD
    INITIALIZE_USER_SECTIONS
    NO_VTOR_CONFIG
    NRF_SD_BLE_API_VERSION=6
    RETARGET_ENABLED=1
    S140
    SVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
    SDK_MUTEX_ENABLE
    SOFTDEVICE_PRESENT
    SWI_DISABLE0
    __FREERTOS
    DEBUG_GAP=1
)

target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE
    ${defined_symbols}
)

set(c_flags
    "-std=gnu99"
    "-O0"
    "-g"
    "-fmessage-length=0"
    "-fno-diagnostics-show-caret"
    "-mcpu=cortex-m4"
    "-mlittle-endian"
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
    "-mthumb"
    "-mtp=soft"
    "-munaligned-access"
    "-nostdinc"
    "-quiet"
    "-gdwarf-4"
    "-g3"
    "-gpubnames"
    "-fomit-frame-pointer"
    "-fno-dwarf2-cfi-asm"
    "-fno-builtin"
    "-ffunction-sections"
    "-fdata-sections"
    "-fshort-enums"
    "-fno-common"
 )

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
    $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${c_flags}>
)


set(
    asm_flags
    "-fmessage-length=0"
    "-fno-diagnostics-show-caret"
    "-E"
    "-mcpu=cortex-m4"
    "-mlittle-endian"
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
    "-mthumb"
    "-nostdinc"
    "-quiet"
    "-lang-asm"
)


target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${asm_flags}>
)

set(linker_flags
    "-X"
    "--omagic"
    "-eReset_Handler"
    "--defsym=__vfprintf=__vfprintf_long_long"
    "--defsym=__vfscanf=__vfscanf_long_long"
    "--gc-sections"
    "-EL"
    "-T${CMAKE_BINARY_DIR}/${exe_target}.ld"
    "-Map"
    "${CMAKE_BINARY_DIR}/${exe_target}.map"
    "-u_vectors"
    "--emit-relocs"
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
    ${linker_flags}
)

target_link_libraries(
  AFR::compiler::mcu_port
  INTERFACE

)


# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Kernel
afr_mcu_port(kernel)

set(
    compiler_src
    "${nrf5_sdk}/external/freertos/portable/GCC/nrf52/port.c"
    "${nrf5_sdk}/external/freertos/portable/GCC/nrf52/portmacro.h"
    "${nrf5_sdk}/external/freertos/portable/CMSIS/nrf52/port_cmsis.c"
    "${nrf5_sdk}/external/freertos/portable/CMSIS/nrf52/port_cmsis_systick.c"
    "${nrf5_sdk}/external/freertos/portable/CMSIS/nrf52/portmacro_cmsis.h"
    "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
 )

 set(
     nrf5_sdk_src
     "${nrf5_sdk}/components/ble/ble_advertising/ble_advertising.c"
     "${nrf5_sdk}/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c"
     "${nrf5_sdk}/components/ble/ble_services/ble_ipsp/ble_ipsp.c"
     "${nrf5_sdk}/components/ble/common/ble_advdata.c"
     "${nrf5_sdk}/components/ble/common/ble_conn_params.c"
     "${nrf5_sdk}/components/ble/common/ble_conn_state.c"
     "${nrf5_sdk}/components/ble/common/ble_srv_common.c"
     "${nrf5_sdk}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c"
     "${nrf5_sdk}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c"
     "${nrf5_sdk}/components/ble/peer_manager/gatt_cache_manager.c"
     "${nrf5_sdk}/components/ble/peer_manager/gatts_cache_manager.c"
     "${nrf5_sdk}/components/ble/peer_manager/id_manager.c"
     "${nrf5_sdk}/components/ble/peer_manager/nrf_ble_lesc.c"
     "${nrf5_sdk}/components/ble/peer_manager/peer_data_storage.c"
     "${nrf5_sdk}/components/ble/peer_manager/peer_database.c"
     "${nrf5_sdk}/components/ble/peer_manager/peer_id.c"
     "${nrf5_sdk}/components/ble/peer_manager/peer_manager.c"
     "${nrf5_sdk}/components/ble/peer_manager/peer_manager_handler.c"
     "${nrf5_sdk}/components/ble/peer_manager/pm_buffer.c"
     "${nrf5_sdk}/components/ble/peer_manager/security_dispatcher.c"
     "${nrf5_sdk}/components/ble/peer_manager/security_manager.c"
     "${nrf5_sdk}/components/boards/boards.c"
     "${nrf5_sdk}/components/libraries/atomic/nrf_atomic.c"
     "${nrf5_sdk}/components/libraries/atomic_fifo/nrf_atfifo.c"
     "${nrf5_sdk}/components/libraries/atomic_flags/nrf_atflags.c"
     "${nrf5_sdk}/components/libraries/balloc/nrf_balloc.c"
     "${nrf5_sdk}/components/libraries/bsp/bsp.c"
     "${nrf5_sdk}/components/libraries/bsp/bsp_btn_ble.c"
     "${nrf5_sdk}/components/libraries/bsp/bsp_nfc.c"
     "${nrf5_sdk}/components/libraries/button/app_button.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_aead.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_aes.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_aes_shared.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_ecc.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_ecdh.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_ecdsa.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_eddsa.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_error.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_hash.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_hkdf.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_hmac.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_init.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_rng.c"
     "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_shared.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_aes.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_aes_aead.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_chacha_poly_aead.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_ecdh.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_ecdsa.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_eddsa.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_hash.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_hmac.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_init.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_rng.c"
     "${nrf5_sdk}/components/libraries/crypto/backend/cc310/cc310_backend_shared.c"
     "${nrf5_sdk}/components/libraries/experimental_section_vars/nrf_section_iter.c"
     "${nrf5_sdk}/components/libraries/fds/fds.c"
     "${nrf5_sdk}/components/libraries/fifo/app_fifo.c"
     "${nrf5_sdk}/components/libraries/fstorage/nrf_fstorage.c"
     "${nrf5_sdk}/components/libraries/fstorage/nrf_fstorage_sd.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_backend_rtt.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_backend_serial.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_backend_uart.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_default_backends.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_frontend.c"
     "${nrf5_sdk}/components/libraries/log/src/nrf_log_str_formatter.c"
     "${nrf5_sdk}/components/libraries/mem_manager/mem_manager.c"
     "${nrf5_sdk}/components/libraries/memobj/nrf_memobj.c"
     "${nrf5_sdk}/components/libraries/queue/nrf_queue.c"
     "${nrf5_sdk}/components/libraries/ringbuf/nrf_ringbuf.c"
     "${nrf5_sdk}/components/libraries/scheduler/app_scheduler.c"
     "${nrf5_sdk}/components/libraries/sensorsim/sensorsim.c"
     "${nrf5_sdk}/components/libraries/strerror/nrf_strerror.c"
     "${nrf5_sdk}/components/libraries/timer/app_timer_freertos.c"
     "${nrf5_sdk}/components/libraries/uart/app_uart_fifo.c"
     "${nrf5_sdk}/components/libraries/hardfault/hardfault_implementation.c"
     "${nrf5_sdk}/components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c"
     "${nrf5_sdk}/components/libraries/util/app_error.c"
     "${nrf5_sdk}/components/libraries/util/app_error_weak.c"
     "${nrf5_sdk}/components/libraries/util/app_util_platform.c"
     "${nrf5_sdk}/components/libraries/util/nrf_assert.c"
     "${nrf5_sdk}/components/softdevice/common/nrf_sdh.c"
     "${nrf5_sdk}/components/softdevice/common/nrf_sdh_ble.c"
     "${nrf5_sdk}/components/softdevice/common/nrf_sdh_freertos.c"
     "${nrf5_sdk}/components/softdevice/common/nrf_sdh_soc.c"
     "${nrf5_sdk}/external/fprintf/nrf_fprintf.c"
     "${nrf5_sdk}/external/fprintf/nrf_fprintf_format.c"
     "${nrf5_sdk}/integration/nrfx/legacy/nrf_drv_clock.c"
     "${nrf5_sdk}/integration/nrfx/legacy/nrf_drv_rng.c"
     "${nrf5_sdk}/integration/nrfx/legacy/nrf_drv_uart.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_clock.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_gpiote.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_rng.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_saadc.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_timer.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_uart.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_wdt.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/prs/nrfx_prs.c"
     "${nrf5_sdk}/modules/nrfx/drivers/src/nrfx_uarte.c"
     "${nrf5_sdk}/modules/nrfx/hal/nrf_ecb.c"
     "${nrf5_sdk}/modules/nrfx/hal/nrf_nvmc.c"
     "${nrf5_sdk}/modules/nrfx/mdk/system_nrf52.c"
     "${nrf5_sdk}/modules/nrfx/mdk/ses_startup_nrf_common.s"
     "${nrf5_sdk}/modules/nrfx/mdk/ses_startup_nrf52840.s"
 )

 set(
     nrf5_sdk_include
    "${nrf5_sdk}/components/libraries/util"
    "${nrf5_sdk}/components/libraries/svc"
    "${nrf5_sdk}/components/libraries/fifo"
    "${nrf5_sdk}/components/libraries/uart"
    "${nrf5_sdk}/components/libraries/stack_info"
    "${nrf5_sdk}/components/libraries/queue"
    "${nrf5_sdk}/components/libraries/crypto/backend/nrf_hw"
    "${nrf5_sdk}/components/libraries/crypto/backend/nrf_sw"
    "${nrf5_sdk}/components/libraries/atomic_flags"
    "${nrf5_sdk}/components/libraries/mutex"
    "${nrf5_sdk}/components/softdevice/s140/headers"
    "${nrf5_sdk}/components/libraries/sensorsim"
    "${nrf5_sdk}/components/libraries/fds"
    "${nrf5_sdk}/components/libraries/fstorage"
    "${nrf5_sdk}/components/libraries/atomic_fifo"
    "${nrf5_sdk}/components/libraries/util"
    "${nrf5_sdk}/components/libraries/crypto"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310"
    "${nrf5_sdk}/components/ble/ble_advertising"
    "${nrf5_sdk}/components/toolchain/cmsis/include"
    "${nrf5_sdk}/modules/nrfx/mdk"
    "${nrf5_sdk}/components"
    "${nrf5_sdk}/components/ble/common"
    "${nrf5_sdk}/components/boards"
    "${nrf5_sdk}/components/ble/nrf_ble_gatt"
    "${nrf5_sdk}/components/ble/ble_services/ble_nus"
    "${nrf5_sdk}/components/ble/nrf_ble_qwr"
    "${nrf5_sdk}/components/libraries/atomic"
    "${nrf5_sdk}/components/libraries/balloc"
    "${nrf5_sdk}/components/libraries/bsp"
    "${nrf5_sdk}/components/libraries/button"
    "${nrf5_sdk}/components/libraries/delay"
    "${nrf5_sdk}/components/libraries/experimental_section_vars"
    "${nrf5_sdk}/components/libraries/hardfault"
    "${nrf5_sdk}/components/libraries/log"
    "${nrf5_sdk}/components/libraries/log/src"
    "${nrf5_sdk}/components/libraries/mem_manager"
    "${nrf5_sdk}/components/libraries/memobj"
    "${nrf5_sdk}/components/libraries/ringbuf"
    "${nrf5_sdk}/components/libraries/strerror"
    "${nrf5_sdk}/components/libraries/timer"
    "${nrf5_sdk}/components/libraries/util"
    "${nrf5_sdk}/components/libraries/crypto/backend/cifra"
    "${nrf5_sdk}/components/ble/peer_manager"
    "${nrf5_sdk}/components/softdevice/common"
    "${nrf5_sdk}/components/softdevice/s140/headers"
    "${nrf5_sdk}/components/softdevice/mbr/nrf52840/headers"
    "${nrf5_sdk}/components/toolchain"
    "${nrf5_sdk}/components/libraries/crypto/backend/micro_ecc"
    "${nrf5_sdk}/components/libraries/crypto/backend/mbedtls"
    "${nrf5_sdk}/components/toolchain/cmsis/include"
    "${nrf5_sdk}/external/fprintf"
    "${nrf5_sdk}/external/nrf_cc310/include"
    "${nrf5_sdk}/external/segger_rtt"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl"
    "${nrf5_sdk}/components/libraries/crypto/backend/oberon"
    "${nrf5_sdk}/integration/nrfx"
    "${nrf5_sdk}/integration/nrfx/legacy"
    "${nrf5_sdk}/modules/nrfx"
    "${nrf5_sdk}/modules/nrfx/drivers/include"
    "${nrf5_sdk}/modules/nrfx/hal"
    "${nrf5_sdk}/modules/nrfx/mdk"
 )

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
    ${compiler_src}
)
set(
    kernel_inc_dirs
    "${nrf5_sdk}/external/freertos/portable/GCC/nrf52"
    "${nrf5_sdk}/external/freertos/portable/CMSIS/nrf52"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/include"
    "${AFR_3RDPARTY_DIR}/jsmn"
    "${AFR_3RDPARTY_DIR}/pkcs11"
    "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
    ${nrf5_sdk_include}
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
        "${board_dir}/config_files"
)

target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        3rdparty::jsmn
        3rdparty::pkcs11
        # Require this for bootloader
        3rdparty::tinycrypt
)

# BLE
set(BLE_SUPPORTED 1 CACHE INTERNAL "BLE is supported on this platform.")

afr_mcu_port(ble_hal)
target_sources(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${afr_ports_dir}/ble/iot_ble_gap_config.h"
        "${afr_ports_dir}/ble/iot_ble_hal_common_gap.c"
        "${afr_ports_dir}/ble/iot_ble_hal_dis.c"
        "${afr_ports_dir}/ble/iot_ble_hal_dis.h"
        "${afr_ports_dir}/ble/iot_ble_hal_gap.c"
        "${afr_ports_dir}/ble/iot_ble_hal_gatt_server.c"
        "${afr_ports_dir}/ble/iot_ble_hal_internals.h"
)
target_include_directories(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${afr_ports_dir}/ble"
)

# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE
        "${afr_ports_dir}/ota/aws_ota_pal.c"
        "${afr_ports_dir}/ota/asn1utility.c"
        "${afr_ports_dir}/ota/asn1utility.h"
        "${afr_ports_dir}/ota/aws_ota_pal_settings.h"
)

target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::crypto
        AFR::ota_mqtt
)

# POSIX
afr_mcu_port(posix)
target_sources(
    AFR::posix::mcu_port
    INTERFACE "${afr_ports_dir}/posix/FreeRTOS_POSIX_portable.h"
)
target_include_directories(
    AFR::posix::mcu_port
    INTERFACE "${afr_ports_dir}/posix"
)
target_link_libraries(
    AFR::posix::mcu_port
    INTERFACE AFR::freertos_plus_posix
)
# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".hex")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/bootloader.cmake")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)

    set( application_src
        "${board_dir}/application_code/main.c"
        "${board_dir}/application_code/nordic_code/SEGGER_HardFaultHandler.c"
        "${board_dir}/application_code/nordic_code/SEGGER_RTT.c"
        "${board_dir}/application_code/nordic_code/SEGGER_RTT_Syscalls_SES.c"
        "${board_dir}/application_code/nordic_code/thumb_crt0.s"
    )
    add_executable(
        ${exe_target}
        ${application_src}
        ${nrf5_sdk_src}
    )

    target_link_libraries(
       ${exe_target}
       PRIVATE
       "${nrf5_sdk}/external/nrf_cc310/lib/libnrf_cc310_0.9.10.a"
       "${AFR_COMPILER_DIR}/../../../lib/libc_v7em_fpv5_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libdebugio_mempoll_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libm_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libc_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libcpp_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libdebugio_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
       "${AFR_COMPILER_DIR}/../../../lib/libvfprintf_v7em_fpv4_sp_d16_hard_t_le_eabi.o"
       "${AFR_COMPILER_DIR}/../../../lib/libvfscanf_v7em_fpv4_sp_d16_hard_t_le_eabi.o"
    )

    if(AFR_METADATA_MODE)
        return()
    endif()

    # -------------------------------------------------------------------------------------------------
    # Additional build configurations
    # -------------------------------------------------------------------------------------------------

    set( mkld_flags
         -memory-map-segments "FLASH RX 0x0 0x100000$<SEMICOLON>RAM RWX 0x20000000 0x40000"
         -section-placement-file "${nrf52840_dir}/flash_placement.xml"
         -check-segment-overflow
         -symbols "__STACKSIZE__=8192$<SEMICOLON>__STACKSIZE_PROCESS__=0$<SEMICOLON>__HEAPSIZE__=8192"
         -section-placement-macros
         "FLASH_PH_START=0x0$<SEMICOLON>FLASH_PH_SIZE=0x100000$<SEMICOLON>RAM_PH_START=0x20000000$<SEMICOLON>RAM_PH_SIZE=0x40000$<SEMICOLON>FLASH_START=0x27000$<SEMICOLON>FLASH_SIZE=0xda000$<SEMICOLON>RAM_START=0x200046F8$<SEMICOLON>RAM_SIZE=0x3B908"
    )

    find_program(gcc_objectcopy objcopy HINTS "${AFR_COMPILER_DIR}")
    find_program(gcc_size arm-none-eabi-size HINTS "${AFR_COMPILER_DIR}")

    if(NOT gcc_objectcopy)
        message(FATAL_ERROR "Cannot find objcopy.")
    endif()

    if(NOT gcc_size)
        message(FATAL_ERROR "Cannot find arm-none-eabi-size.")
    endif()

    function(nrf52840_build)
        set( build_target ${ARGV0} )
        set( build_mkld_flags ${${ARGV1}} )
        add_custom_command(
            TARGET ${build_target}
            PRE_LINK
            COMMAND VERBATIM "${AFR_COMPILER_DIR}/../../../bin/mkld" ${build_mkld_flags} "${CMAKE_BINARY_DIR}/${build_target}.ld"
        )

        set(output_file "$<TARGET_FILE_DIR:${build_target}>/${build_target}.hex")

        add_custom_command(
            TARGET ${build_target} POST_BUILD
            COMMAND "${gcc_objectcopy}" -O ihex "$<TARGET_FILE:${build_target}>" "${output_file}"
            COMMAND "${gcc_size}" "$<TARGET_FILE:${build_target}>"
        )

        add_custom_command(
            TARGET ${build_target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${output_file}" "${CMAKE_BINARY_DIR}"
        )

    endfunction()

    nrf52840_build(
        bootloader
        bootloader_mkld_flags
    )

    nrf52840_build(
        ${exe_target}
        mkld_flags
    )

endif()
