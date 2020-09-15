# These FreeRTOS related global variables are available to use.
# AFR_ROOT_DIR                  FreeRTOS source root.
# AFR_KERNEL_DIR                FreeRTOS kernel root.
# AFR_MODULES_DIR               FreeRTOS modules root.
# AFR_MODULES_C_SDK_DIR         C-SDK libraries root.
# AFR_MODULES_FREERTOS_PLUS_DIR FreeRTOS-Plus libraries root.
# AFR_MODULES_ABSTRACTIONS_DIR  Abstractions layers root.
# AFR_DEMOS_DIR                 FreeRTOS demos root.
# AFR_TESTS_DIR                 FreeRTOS common tests and framework root.
# AFR_VENDORS_DIR               vendors content root.
# AFR_3RDPARTY_DIR              3rdparty libraries root.
# AFR_VENDOR_NAME               Folder name for vendor.
# AFR_BOARD_NAME                Folder name for this board
# AFR_TOOLCHAIN                 Compiler chosen by the user. Should be one of
#                               the file names under ${AFR_ROOT_DIR}/tools/cmake/toolchains
# AFR_IS_TESTING                1 if testing enabled, otherwise, 0.

# You may also use these 2 functions we defined to glob files when needed. However, we recommend
# to specify your source files explicitly to avoid unexpected behavior unless you're 100% sure.
# CMake reference link: https://cmake.org/cmake/help/latest/command/file.html#filesystem
# afr_glob_files(<out_var> [RECURSE] <DIRECTORY> <directory> [<GLOBS> <glob-expressions>...])
# afr_glob_src(<out_var> [RECURSE] <DIRECTORY> <directory> [<EXTENSIONS> <file-extensions>...])

# If you don't specify GLOBS or EXTENSIONS parameters,
# afr_glob_files: glob all files including hidden files in the specified directory.
# afr_glob_src:   glob all files ending with either .c, .h, .s or .asm

# Use RECURSE if you want to recursively search all subdirectories.

# Example usage,
# afr_glob_src(board_code DIRECTORY "${board_dir}/application_code/${vendor}_code")
# afr_glob_src(driver_code RECURSE DIRECTORY "${driver_path}")
# afr_glob_src(headers DIRECTORY "${some_path}" EXTENSIONS h)

set(simplelink_sdk_dir "${AFR_VENDORS_DIR}/ti/simplelink_cc13x2_26x2_sdk/${CC13X2_26X2_SDK_VER}")
set(simplelink_common_src_dir "${AFR_VENDORS_DIR}/ti/simplelink_common/")
set(portable_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
if (CMAKE_HOST_SYSTEM_NAME  STREQUAL "Linux")
    set(sysconfig_sh "${AFR_VENDORS_DIR}/ti/sysconfig/${SYSCONFIG_VER}/sysconfig_cli.sh")
else()
    # If not Linux, assume Windows.
    # The actual "true" variable could for example be WIN32, MINGWm MSYS or CYGWIN
    # depending on the Windows enviroment used.
    set(sysconfig_sh "${AFR_VENDORS_DIR}/ti/sysconfig/${SYSCONFIG_VER}/sysconfig_cli.bat")
endif()

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
    set(syscfg_file "${board_dir}/application_code/ti_code/aws_tests.syscfg")
    set(sysconfig_out "${CMAKE_BINARY_DIR}/sysconfig_output/aws_tests")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
    set(syscfg_file "${board_dir}/application_code/ti_code/aws_demos.syscfg")
    set(sysconfig_out "${CMAKE_BINARY_DIR}/sysconfig_output/aws_demos")
    set(exe_target aws_demos)
endif()

set(sysconfig_files_ble
    "${sysconfig_out}/ti_radio_config.c"
    "${sysconfig_out}/ti_ble_config.c"
)
set(sysconfig_files_drivers
    "${sysconfig_out}/ti_devices_config.c"
    "${sysconfig_out}/ti_drivers_config.c"
)
set(sysconfig_files_all
    "${sysconfig_files_ble}"
    "${sysconfig_files_drivers}"
)

# Create SysConfig generation folder if it does not already exist
# This to avoid CMake complaining on projects including non-existing locations
file(MAKE_DIRECTORY ${sysconfig_out})

# Include IDE specific cmake file.
include("${CMAKE_CURRENT_LIST_DIR}/gcc.cmake")
set(compiler_str "gcc")
include_directories(BEFORE "${simplelink_sdk_dir}/source/ti/posix/gcc")

# Setup SysConfig exection dependency
add_custom_command(OUTPUT ${sysconfig_files_all}
    COMMAND ${sysconfig_sh} --compiler ${compiler_str} --product
    "${simplelink_sdk_dir}/.metadata/product.json" --output ${sysconfig_out}
    ${syscfg_file}
    DEPENDS ${syscfg_file}
)
add_custom_target(gen_sysconfig_files DEPENDS ${sysconfig_files_all})

# -------------------------------------------------------------------------------------------------
# FreeRTOS disabled libraries
# -------------------------------------------------------------------------------------------------
set(AFR_MODULE_defender 0 CACHE INTERNAL "")
# HTTPS is not supported as this board does not have WiFi/Ethernet.
set(AFR_MODULE_https 0 CACHE INTERNAL "")

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
# Provide metadata for listing on FreeRTOS console.
afr_set_board_metadata(ID "CC1352R1_LAUNCHXL")
afr_set_board_metadata(DISPLAY_NAME "CC1352R1 LaunchPad")
afr_set_board_metadata(DESCRIPTION "SimpleLink Multi-Band CC1352R Wireless MCU LaunchPad Development Kit")
afr_set_board_metadata(VENDOR_NAME "Texas Instruments")
afr_set_board_metadata(FAMILY_NAME "CC13X2")
afr_set_board_metadata(DATA_RAM_MEMORY "80KB")
afr_set_board_metadata(PROGRAM_MEMORY "352KB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "CCS")
afr_set_board_metadata(IDE_CCS_NAME "Code Composer Studio")
afr_set_board_metadata(IDE_CCS_COMPILER "TI-ARM;GCC")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
# If you support multiple compilers, you can use AFR_TOOLCHAIN to conditionally define the compiler
# settings. This variable will be set to the file name of CMAKE_TOOLCHAIN_FILE. It might also be a
# good idea to put your compiler settings to different files and just include them here, e.g.,
# include(compilers/${AFR_TOOLCHAIN}.cmake)

afr_mcu_port(compiler)

# Compile definitions/macros
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_defined_symbols}>
        $<$<COMPILE_LANGUAGE:ASM>:${compiler_defined_symbols}>
)

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
        $<$<COMPILE_LANGUAGE:ASM>:${compiler_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_flags}
)

# Library search path for linker
target_link_directories(
    AFR::compiler::mcu_port
    INTERFACE
        "${simplelink_sdk_dir}"
)

# Libraries to link
target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Define portable layer targets with afr_mcu_port(<module_name>). We will create an CMake
# INTERFACE IMPORTED target called AFR::${module_name}::mcu_port for you. You can use it with
# standard CMake functions like target_*. To better organize your files, you can define your own
# targets and use target_link_libraries(AFR::${module_name}::mcu_port INTERFACE <your_targets>)
# to provide the public interface you want expose.

# Kernel
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_sdk_dir}/kernel/freertos/dpl/ClockPCC26X2_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/DebugP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/HwiPCC26X2_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/MutexP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/PowerCC26X2_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/QueueP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SemaphoreP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/StaticAllocs_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SwiP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/SystemP_freertos.c"
        "${simplelink_sdk_dir}/kernel/freertos/dpl/TimerPCC26XX_freertos.c"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        ${compiler_specific_src}
        # Include POSIX with kernel as in CoreSDK projects
        # POSIX is used by the BLE HAL port
        "${simplelink_sdk_dir}/source/ti/posix/freertos/clock.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/memory.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/mqueue.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_barrier.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_cond.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_mutex.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread_rwlock.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/pthread.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/sched.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/semaphore.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/sleep.c"
        "${simplelink_sdk_dir}/source/ti/posix/freertos/timer.c"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_sdk_dir}/source"
        "${board_dir}/config_files"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE ${other_targets}
)

# If you defined the driver and freertos portable target separately, you can use afr_mcu_port with
# DEPENDS keyword, e.g.,
# afr_mcu_port(kernel DEPENDS my_board_driver freertos_port)

# Common I/O, currently only fully supported for GCC
afr_mcu_port(common_io)
target_sources(
  AFR::common_io::mcu_port
  INTERFACE
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_uart.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_spi.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_i2c.c"
      "${simplelink_common_src_dir}/ports/common_io/src/cc13x2_26x2/iot_perfcounter.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_gpio.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_timer.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_flash.c"
      "${simplelink_common_src_dir}/ports/common_io/src/cc13x2_26x2/iot_adc.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_pwm.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_rtc.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_tsensor.c"
      "${simplelink_common_src_dir}/ports/common_io/src/simplelink/iot_watchdog.c"
      "${simplelink_common_src_dir}/ports/common_io/src/cc13x2_26x2/iot_hw.c"
      "${simplelink_common_src_dir}/ports/common_io/src/cc13x2_26x2/iot_reset.c"
      "$<IF:${AFR_IS_TESTING},${simplelink_common_src_dir}/ports/common_io/iot_test_common_io_internal.c,>"
)

target_include_directories(
    AFR::common_io::mcu_port
    INTERFACE
      "${sysconfig_out}"
      "${AFR_MODULES_ABSTRACTIONS_DIR}/common_io/include"
)

afr_mcu_port(ble_hal)
target_sources(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${sysconfig_files_all}"
        # BLE_HAL implementation files
        "${simplelink_common_src_dir}/ports/ble/iot_ble_hal_manager.c"
        "${simplelink_common_src_dir}/ports/ble/iot_ble_hal_manager_adapter_ble.c"
        "${simplelink_common_src_dir}/ports/ble/iot_ble_hal_mq_task.c"
        "${simplelink_common_src_dir}/ports/ble/iot_ble_hal_types.c"
        "${simplelink_common_src_dir}/ports/ble/iot_ble_hal_gatt_server.c"
        "${simplelink_common_src_dir}/ports/ble/osal_icall_ble.c"
        # ICall application side files
        "${simplelink_sdk_dir}/source/ti/ble5stack/common/cc26xx/freertos/icall_POSIX.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/src/icall_cc2650.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/src/icall_user_config.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/app/ble_user_config.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/stack/ble_user_config_stack.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/app/icall_api_lite.c"
        # BLE-Stack application middleware
        "${simplelink_sdk_dir}/source/ti/ble5stack/profiles/dev_info/cc26xx/devinfoservice.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/host/gattservapp_util.c"
        "${simplelink_sdk_dir}/source/ti/ble5stack/host/gatt_uuid.c"
        # EEPROM emulation/NV drivers, used by stack for key storage
        "${simplelink_sdk_dir}/source/ti/common/nv/crc.c"
        "${simplelink_sdk_dir}/source/ti/common/nv/nvocmp.c"
        # Secondary heap providing ISR alloc support
        "${simplelink_sdk_dir}/source/ti/ble5stack/common/cc26xx/freertos/bget.c"
        # ROM initialization code
        "${simplelink_sdk_dir}/source/ti/ble5stack/rom/agama_r1/rom_init.c"
)
target_include_directories(
    AFR::ble_hal::mcu_port
    INTERFACE
        "${sysconfig_out}"
        "${simplelink_sdk_dir}/source"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
        "${simplelink_common_src_dir}/ports/ble"
        "${simplelink_sdk_dir}/source/ti/ble5stack/controller/cc26xx/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/rom"
        "${simplelink_sdk_dir}/source/ti/ble5stack/common/cc26xx"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/hal/src/target/_common"
        "${simplelink_sdk_dir}/source/ti/ble5stack/hal/src/target/_common/cc26xx"
        "${simplelink_sdk_dir}/source/ti/ble5stack/hal/src/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/heapmgr"
        "${simplelink_sdk_dir}/source/ti/ble5stack/profiles/dev_info"
        "${simplelink_sdk_dir}/source/ti/ble5stack/profiles/simple_profile"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/src/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/icall/src"
        "${simplelink_sdk_dir}/source/ti/ble5stack/osal/src/inc"
        "${simplelink_sdk_dir}/source/ti/ble5stack/services/src/saddr"
        "${simplelink_sdk_dir}/source/ti/ble5stack/services/src/sdata"
        "${simplelink_sdk_dir}/source/ti/common/nv"
        "${simplelink_sdk_dir}/source/ti/common/cc26xx"
        "${simplelink_sdk_dir}/source/ti/ble5stack/common/cc26xx/freertos"
        "${simplelink_sdk_dir}/source/ti/devices/cc13x2_cc26x2"
)
target_compile_definitions(
    AFR::ble_hal::mcu_port
    INTERFACE
        FREERTOS
        MR_POSIX
        NVOCMP_NWSAMEITEM=1
        FLASH_ROM_BUILD
        NVOCMP_POSIX_MUTEX
)
# Include IDE specific cmake file.
target_compile_options(
    AFR::ble_hal::mcu_port
    INTERFACE
        "@${sysconfig_out}/ti_ble_app_config.opt"
        "@${sysconfig_out}/ti_build_config.opt"
        "@${simplelink_sdk_dir}/source/ti/ble5stack/config/build_components.opt"
        "@${simplelink_sdk_dir}/source/ti/ble5stack/config/factory_config.opt"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
# We require you to define at least demos and tests executable targets. Available demos and tests
# will be automatically enabled by us. You need to provide other project settings such as linker
# scripts and post build commands.
set(CMAKE_EXECUTABLE_SUFFIX ".out")

afr_glob_src(board_code_src DIRECTORY "${board_dir}/application_code/ti_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    message(${board_code_src})
    add_executable(
        ${exe_target}
        ${sysconfig_files_drivers}
        ${config_files}
        ${board_code_src}
        "${simplelink_common_src_dir}/utils/uart_term/uart_term.c"
        "${board_dir}/application_code/main.c"
    )
    target_include_directories(
        ${exe_target}
        PRIVATE
            "${sysconfig_out}"
            "${AFR_MODULES_C_SDK_DIR}/standard/mqtt/include"
            "${simplelink_common_src_dir}/utils/uart_term"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::common_io
            AFR::ble_hal
            AFR::ble
            -l":source/ti/ble5stack/libraries/cc13x2r1/OneLib.a"
            -l":source/ti/ble5stack/libraries/cc13x2r1/StackWrapper.a"
            -l":source/ti/ble5stack/libraries/cc13x2r1/ble_r2.symbols"
            ${link_extra_flags}
            -T"${board_dir}/application_code/ti_code/cc13x2_cc26x2_freertos.lds"
    )
endif()
