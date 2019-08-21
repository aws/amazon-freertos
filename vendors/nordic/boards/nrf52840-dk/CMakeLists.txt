set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(nrf5_sdk "${AFR_VENDORS_DIR}/nordic/nRF5_SDK_15.2.0")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
else()
    set(board_dir "${board_demos_dir}")
endif()


set(AFR_MODULE_defender 0 CACHE INTERNAL "")

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS disabled libraries
# -------------------------------------------------------------------------------------------------

# HTTPS is not supported for Nordic as this board does not have WiFi/Ethernet.
afr_disable_public_modules(
    https
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
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

afr_set_board_metadata(IDE_SeggerStudio_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/nordic/nrf52840-dk/ses/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")


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
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${defined_symbols}>
)
#"$<TARGET_FILE_DIR:${exe_target}>  Debug/Obj/${exe_target}.ld"
set(c_flags "-fmessage-length=0" "-fno-diagnostics-show-caret" "-mcpu=cortex-m4" "-mlittle-endian" "-mfloat-abi=hard" "-mfpu=fpv4-sp-d16" "-mthumb" "-mtp=soft" "-munaligned-access" "-nostdinc" "-quiet" "-std=gnu99" "-gdwarf-4" "-g3" "-gpubnames" "-fomit-frame-pointer" "-fno-dwarf2-cfi-asm" "-fno-builtin" "-ffunction-sections" "-fdata-sections" "-fshort-enums" "-fno-common")
set(linker_flags "-X" "--omagic" "-eReset_Handler" "--defsym=__vfprintf=__vfprintf_long_long" "--defsym=__vfscanf=__vfscanf_long_long" "-EL"
"-T${CMAKE_BINARY_DIR}/aws_tests.ld"
"-Map Output/Debug/Exe/aws_tests.map"
"-u_vectors"
"-o Output/Debug/Exe/aws_tests.elf"
"--emit-relocs"
--start-group "@/home/ANT.AMAZON.COM/hbouvier/Desktop/afr-dev/amazon-freertos/vendors/nordic/boards/nrf52840-dk/aws_tests/ses/Output/aws_tests Debug/Obj/aws_tests.ind" --end-group
)


# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${c_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_flags}
)


# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Kernel
afr_mcu_port(kernel)

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk/port.c"
        "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk/port_cmsis.c"
        "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk/port_cmsis_systick.c"
        "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk/portmacro_cmsis.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)
set(
    kernel_inc_dirs
    "${AFR_KERNEL_DIR}/portable/GCC/nrf52840-dk"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/include"
    "${AFR_3RDPARTY_DIR}/jsmn"
    "${AFR_3RDPARTY_DIR}/pkcs11"
    "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
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
    "${nrf5_sdk}/components/ble/ble_link_ctx_manager"
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
    "${nrf5_sdk}/components/device"
    "${nrf5_sdk}/components/drivers_nrf/clock"
    "${nrf5_sdk}/components/drivers_nrf/common"
    "${nrf5_sdk}/components/drivers_nrf/delay"
    "${nrf5_sdk}/components/drivers_nrf/gpiote"
    "${nrf5_sdk}/components/ble/nrf_ble_gatt"
    "${nrf5_sdk}/components/ble/ble_services/ble_nus"
    "${nrf5_sdk}/components/ble/nrf_ble_qwr"
    "${nrf5_sdk}/components/drivers_nrf/hal"
    "${nrf5_sdk}/components/drivers_nrf/rng"
    "${nrf5_sdk}/components/drivers_nrf/timer"
    "${nrf5_sdk}/components/drivers_nrf/wdt"
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
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".hex")

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()


if(AFR_IS_TESTING)
add_executable(
    ${exe_target}
    "${board_dir}/application_code/main.c"
)
else()
add_executable(
    ${exe_target}
    "${board_dir}/application_code/main.c"
)
endif()

if(AFR_METADATA_MODE)
    return()
else()
    message(FATAL_ERROR "CMake support for Nordic is not complete yet.")
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

set(
    mkld_flags -memory-map-segments "FLASH RX 0x0 0x100000$<SEMICOLON>RAM RWX 0x20000000 0x40000"
    -section-placement-file "${AFR_ROOT_DIR}/projects/nordic/nrf52840-dk/ses/aws_tests/flash_placement.xml"
    -check-segment-overflow -symbols "__STACKSIZE__=8192$<SEMICOLON>__STACKSIZE_PROCESS__=0$<SEMICOLON>__HEAPSIZE__=8192" -section-placement-macros "FLASH_PH_START=0x0$<SEMICOLON>FLASH_PH_SIZE=0x100000$<SEMICOLON>RAM_PH_START=0x20000000$<SEMICOLON>RAM_PH_SIZE=0x40000$<SEMICOLON>FLASH_START=0x27000$<SEMICOLON>FLASH_SIZE=0xda000$<SEMICOLON>RAM_START=0x200046F8$<SEMICOLON>RAM_SIZE=0x3B908"
)

add_custom_command(
    TARGET ${exe_target} PRE_LINK
    COMMAND VERBATIM "/usr/share/segger_embedded_studio_for_arm_4.16/bin/mkld" ${mkld_flags} "${CMAKE_BINARY_DIR}/aws_tests.ld"
)
