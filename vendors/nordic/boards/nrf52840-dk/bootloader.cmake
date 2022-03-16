#--------------------------------------------------------------------------------------------------
# This cmake file is included from the project cmakefile to build bootloader
#=-------------------------------------------------------------------------------------------------

add_executable( bootloader )

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

set(defined_symbols
   __SIZEOF_WCHAR_T=4
   __ARM_ARCH_7EM__
   __SES_ARM
   __ARM_ARCH_FPV4_SP_D16__
   __SES_VERSION=41600
   NDEBUG
   BOARD_PCA10056
   CONFIG_GPIO_AS_PINRESET
   FLOAT_ABI_HARD
   INITIALIZE_USER_SECTIONS
   NO_VTOR_CONFIG
   NRF52840_XXAA
   NRF_DFU_SETTINGS_VERSION=1
   SVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
)

target_compile_definitions(
    bootloader
    PRIVATE
    ${defined_symbols}
)

set(c_flags
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
    "-std=gnu99"
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
    bootloader
    PRIVATE
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
    bootloader
    PRIVATE
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
    "-T${CMAKE_BINARY_DIR}/bootloader.ld"
    "-Map"
    "${CMAKE_BINARY_DIR}/bootloader.map"
    "-u_vectors"
    "--emit-relocs"
)

# Linker flags
target_link_options(
    bootloader
    PRIVATE
    ${linker_flags}
)

target_link_libraries(
  bootloader
  PRIVATE
  "${nrf5_sdk}/external/nrf_cc310_bl/lib/libnrf_cc310_bl_0.9.10.a"
  "${AFR_COMPILER_DIR}/../../../lib/libm_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
  "${AFR_COMPILER_DIR}/../../../lib/libc_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
  "${AFR_COMPILER_DIR}/../../../lib/libdebugio_v7em_fpv4_sp_d16_hard_t_le_eabi.a"
)

# bootloader src
set( 
    bootloader_src
    "${board_dir}/bootloader/asn1utility.c"
    "${board_dir}/bootloader/crypto.c"
    "${board_dir}/bootloader/main.c"
    "${board_dir}/bootloader/utils.c"
    "${board_dir}/application_code/nordic_code/thumb_crt0.s"
    "${nrf5_sdk}/components/boards/boards.c"
    "${nrf5_sdk}/modules/nrfx/mdk/ses_startup_nrf_common.s"
    "${nrf5_sdk}/modules/nrfx/mdk/ses_startup_nrf52840.s"
    "${nrf5_sdk}/modules/nrfx/mdk/system_nrf52840.c"
    "${nrf5_sdk}/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c"
    "${nrf5_sdk}/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c"
    "${nrf5_sdk}/modules/nrfx/drivers/src/prs/nrfx_prs.c"
    "${nrf5_sdk}/components/libraries/util/app_error_weak.c"
    "${nrf5_sdk}/components/libraries/util/app_util_platform.c"
    "${nrf5_sdk}/components/libraries/util/nrf_assert.c"
    "${nrf5_sdk}/components/libraries/crc32/crc32.c"
    "${nrf5_sdk}/components/libraries/mem_manager/mem_manager.c"
    "${nrf5_sdk}/components/libraries/atomic/nrf_atomic.c"
    "${nrf5_sdk}/components/libraries/balloc/nrf_balloc.c"
    "${nrf5_sdk}/components/libraries/bootloader/nrf_bootloader_info.c"
    "${nrf5_sdk}/components/libraries/memobj/nrf_memobj.c"
    "${nrf5_sdk}/components/libraries/queue/nrf_queue.c"
    "${nrf5_sdk}/components/libraries/strerror/nrf_strerror.c"
    "${nrf5_sdk}/components/libraries/log/src/nrf_log_frontend.c"
    "${nrf5_sdk}/components/libraries/log/src/nrf_log_str_formatter.c"
    "${nrf5_sdk}/components/softdevice/common/nrf_sdh.c"
    "${nrf5_sdk}/modules/nrfx/hal/nrf_nvmc.c"
    "${AFR_3RDPARTY_DIR}/mbedtls/library/base64.c"
    "${AFR_3RDPARTY_DIR}/mbedtls/library/asn1parse.c"
    "${AFR_3RDPARTY_DIR}/mbedtls/library/constant_time.c"
    "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_ecc.c"
    "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_ecdsa.c"
    "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_hash.c"
    "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_init.c"
    "${nrf5_sdk}/components/libraries/crypto/nrf_crypto_shared.c"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecc.c"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_ecdsa.c"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_hash.c"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_init.c"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl/cc310_bl_backend_shared.c"
)

set(
    bootloader_inc
    "${board_dir}/bootloader"
    "${board_dir}/bootloader/pca10056/config"
    "${board_dir}/config_files"
    "${nrf5_sdk}/components/boards"
    "${nrf5_sdk}/components/libraries/atomic"
    "${nrf5_sdk}/components/libraries/balloc"
    "${nrf5_sdk}/components/libraries/bootloader"
    "${nrf5_sdk}/components/libraries/bootloader/dfu"
    "${nrf5_sdk}/components/libraries/bootloader/serial_dfu"
    "${nrf5_sdk}/components/libraries/crc32"
    "${nrf5_sdk}/components/libraries/crypto"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310"
    "${nrf5_sdk}/components/libraries/crypto/backend/cc310_bl"
    "${nrf5_sdk}/components/libraries/crypto/backend/cifra"
    "${nrf5_sdk}/components/libraries/crypto/backend/mbedtls"
    "${nrf5_sdk}/components/libraries/crypto/backend/micro_ecc"
    "${nrf5_sdk}/components/libraries/crypto/backend/nrf_hw"
    "${nrf5_sdk}/components/libraries/crypto/backend/nrf_sw"
    "${nrf5_sdk}/components/libraries/crypto/backend/oberon"
    "${nrf5_sdk}/components/libraries/delay"
    "${nrf5_sdk}/components/libraries/experimental_section_vars"
    "${nrf5_sdk}/components/libraries/fstorage"
    "${nrf5_sdk}/components/libraries/log"
    "${nrf5_sdk}/components/libraries/log/src"
    "${nrf5_sdk}/components/libraries/mem_manager"
    "${nrf5_sdk}/components/libraries/memobj"
    "${nrf5_sdk}/components/libraries/mutex"
    "${nrf5_sdk}/components/libraries/queue"
    "${nrf5_sdk}/components/libraries/ringbuf"
    "${nrf5_sdk}/components/libraries/scheduler"
    "${nrf5_sdk}/components/libraries/slip"
    "${nrf5_sdk}/components/libraries/stack_info"
    "${nrf5_sdk}/components/libraries/strerror"
    "${nrf5_sdk}/components/libraries/util"
    "${nrf5_sdk}/components/softdevice/s140/headers"
    "${nrf5_sdk}/components/softdevice/mbr/nrf52840/headers"
    "${nrf5_sdk}/components/toolchain/cmsis/include"
    "${nrf5_sdk}/external/fprintf"
    "${nrf5_sdk}/external/nano-pb"
    "${nrf5_sdk}/external/nrf_cc310/include"
    "${nrf5_sdk}/external/nrf_cc310_bl/include"
    "${nrf5_sdk}/external/nrf_oberon"
    "${nrf5_sdk}/external/nrf_oberon/include"
    "${nrf5_sdk}/integration/nrfx"
    "${nrf5_sdk}/integration/nrfx/legacy"
    "${nrf5_sdk}/modules/nrfx"
    "${nrf5_sdk}/modules/nrfx/drivers/include"
    "${nrf5_sdk}/modules/nrfx/hal"
    "${nrf5_sdk}/modules/nrfx/mdk"
    "${AFR_3RDPARTY_DIR}/mbedtls/include"
    "${AFR_3RDPARTY_DIR}/mbedtls/library"
    #TODO: Remove depedency on "code signer cert" from crypto.c in tests.
    "$<${AFR_IS_TESTING}:${AFR_TESTS_DIR}/integration_test/ota_pal>"
)

target_sources(
    bootloader
    PRIVATE
    ${bootloader_src}
)
target_include_directories(
    bootloader
    PRIVATE
    $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${bootloader_inc}>
)

# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

set(
    bootloader_mkld_flags
    -memory-map-segments "FLASH RX 0x0 0x100000$<SEMICOLON>RAM RWX 0x20000000 0x40000$<SEMICOLON>uicr_bootloader_start_address RX 0x10001014 0x4$<SEMICOLON>mbr_params_page RX 0x000FE000 0x1000$<SEMICOLON>bootloader_settings_page RX 0x000FF000 0x1000$<SEMICOLON>uicr_mbr_params_page RX 0x10001018 0x4"
    -section-placement-file "${nrf52840_dir}/bootloader_flash_placement.xml"
    -check-segment-overflow
    -symbols "__STACKSIZE__=2048$<SEMICOLON>__STACKSIZE_PROCESS__=0$<SEMICOLON>__HEAPSIZE__=0"
    -section-placement-macros
    "FLASH_PH_START=0x0$<SEMICOLON>FLASH_PH_SIZE=0x100000$<SEMICOLON>RAM_PH_START=0x20000000$<SEMICOLON>RAM_PH_SIZE=0x40000$<SEMICOLON>FLASH_START=0xF8000$<SEMICOLON>FLASH_SIZE=0x6000$<SEMICOLON>RAM_START=0x20000008$<SEMICOLON>RAM_SIZE=0x3fff8"
)