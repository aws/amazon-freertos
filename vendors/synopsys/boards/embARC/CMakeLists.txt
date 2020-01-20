# These Amazon FreeRTOS related global variables are available to use.
# AFR_ROOT_DIR                  Amazon FreeRTOS source root.
# AFR_KERNEL_DIR                FreeRTOS kernel root.
# AFR_MODULES_DIR               Amazon FreeRTOS modules root.
# AFR_MODULES_C_SDK_DIR         C-SDK libraries root.
# AFR_MODULES_FREERTOS_PLUS_DIR FreeRTOS-Plus libraries root.
# AFR_MODULES_ABSTRACTIONS_DIR  Abstractions layers root.
# AFR_DEMOS_DIR                 Amazon FreeRTOS demos root.
# AFR_TESTS_DIR                 Amazon FreeRTOS common tests and framework root.
# AFR_VENDORS_DIR               vendors content root.
# AFR_3RDPARTY_DIR              3rdparty libraries root.

# AFR_VENDOR_NAME           Folder name for vendor.
# AFR_BOARD_NAME            Folder name for this board.

# AFR_TOOLCHAIN             Compiler chosen by the user. Should be one of
#                           the file names under ${AFR_ROOT_DIR}/tools/cmake/toolchains
# AFR_IS_TESTING            1 if testing enabled, otherwise, 0.

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

set(embARC_src_dir ${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/embarc_bsp)
# relative directory from embarc makefile to embarc folder
set(embARC_ROOT ../../vendors/synopsys/embarc_bsp)
set(embARC_BOARD ${AFR_BOARD_NAME})

# default board version and core settings.
if(${embARC_BOARD} STREQUAL "emsdp")
    set(embARC_BOARD_VER rev2)
    set(embARC_CUR_CORE em11d_dfss)
elseif(${embARC_BOARD} STREQUAL "emsk")
    set(embARC_BOARD_VER 22)
    set(embARC_CUR_CORE arcem7d)
elseif(${embARC_BOARD} STREQUAL "hsdk")
    set(embARC_BOARD_VER 10)
    set(embARC_CUR_CORE archs38_c0)
elseif(${embARC_BOARD} STREQUAL "iotdk")
    set(embARC_BOARD_VER 10)
    set(embARC_CUR_CORE arcem9d)
else()
    message(FATAL_ERROR "embARC: Please select correct board")
endif()

# Include IDE specific cmake file.
if(${AFR_TOOLCHAIN} STREQUAL "synopsys-gcc")
    set(embARC_TOOLCHAIN gnu)
    include("${CMAKE_CURRENT_LIST_DIR}/gcc.cmake")
    message(WARNING "CMake support for Synopsys ARC using GNU is not complete yet.")
elseif(${AFR_TOOLCHAIN} STREQUAL "synopsys-mw")
    set(embARC_TOOLCHAIN mw)
    include("${CMAKE_CURRENT_LIST_DIR}/mw.cmake")
else()
    message( FATAL_ERROR "The toolchain ${AFR_TOOLCHAIN} is not supported." )
endif()

# message("AFR_VENDORS_DIR is ${AFR_VENDORS_DIR}" )
# message("embARC_src_dir is ${embARC_src_dir}" )
include(${CMAKE_CURRENT_LIST_DIR}/embARC.cmake)
# message("embARC_LIB_DIR is ${embARC_LIB_DIR}" )
# message("embARC_INC_DIR is ${embARC_INC_DIR}" )
# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
# Provide metadata for listing on Amazon FreeRTOS console.
include("${CMAKE_CURRENT_LIST_DIR}/embARC_${AFR_BOARD_NAME}.cmake")

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
    ${compiler_defined_symbols}
)

# Compiler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${c_flags}>
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>
)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

# Linker flags
target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)

# Libraries to link
target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
    ${link_dependent_libs}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Define portable layer targets with afr_mcu_port(<module_name>). We will create an CMake
# INTERFACE IMPORTED target called AFR::${module_name}::mcu_port for you. You can use it with
# standard CMake functions like target_*. To better organize your files, you can define your own
# targets and use target_link_libraries(AFR::${module_name}::mcu_port INTERFACE <your_targets>)
# to provide the public interface you want expose.

set(vendor ${AFR_VENDOR_NAME})
set(board ${AFR_BOARD_NAME})

set(portable_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# Kernel
afr_mcu_port(kernel)
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/port.c"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/arc_freertos_exceptions.c"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/arc_freertos_exceptions.h"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/freertos_tls.c"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC/arc_support.s"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        # "${board_specific_include}"       # Normally ${board_dir}/application_code/${vendor}_code
        # "${board_configs_include}"        # Normally ${board_dir}/application_code/config_files
        # "${driver_public_include}"        # Normally in ${driver_dir}
        ${aws_credentials_include}
        ${embARC_INC_DIR}
        "${board_dir}/config_files"
        "${AFR_KERNEL_DIR}/portable/Synopsys/ARC"
        "${AFR_MODULES_C_SDK_DIR}/standard/common/include"
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
    ${other_targets}
)

# If you defined the driver and freertos portable target separately, you can use afr_mcu_port with
# DEPENDS keyword, e.g.,
# afr_mcu_port(kernel DEPENDS my_board_driver freertos_port)

# POSIX
# afr_mcu_port(posix)
# target_include_directories(
#     AFR::posix::mcu_port
#     INTERFACE "${portable_dir}/posix"
# )

# WiFi
# afr_mcu_port(wifi)
# target_sources(
#     AFR::wifi::mcu_port
#     INTERFACE "${portable_dir}/wifi/iot_wifi.c"
# )

#currently the following modules are not ported
if(AFR_IS_TESTING)
    # PKCS11
    afr_mcu_port(pkcs11_implementation)
    target_sources(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE "${portable_dir}/pkcs11/iot_pkcs11_pal.c"
    )
    # Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
    target_link_libraries(
        AFR::pkcs11_implementation::mcu_port
        INTERFACE AFR::pkcs11_mbedtls
    )

    # FreeRTOS Plus TCP
    afr_mcu_port(freertos_plus_tcp)
    target_sources(
        AFR::freertos_plus_tcp::mcu_port
        INTERFACE
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c"
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/embARC/NetworkInterface.c"
    )
    target_include_directories(
        AFR::freertos_plus_tcp::mcu_port
        INTERFACE
            "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/MetaWare"
    )

    # Secure sockets
    afr_mcu_port(secure_sockets)
    # Link to AFR::secure_sockets_freertos_plus_tcp if you want use default implementation based on
    # FreeRTOS-Plus-TCP.
    target_link_libraries(
        AFR::secure_sockets::mcu_port
        INTERFACE AFR::secure_sockets_freertos_plus_tcp
    )
endif()
# OTA
# afr_mcu_port(ota)
# target_sources(
#     AFR::ota::mcu_port
#     INTERFACE "${portable_dir}/ota/aws_ota_pal.c"
# )
# Choose which backend to enable. Link to AFR::ota_mqtt to enable MQTT, link to AFR:ota_http to enable HTTP.
# target_link_libraries(
#     AFR::ota::mcu_port
#     INTERFACE
#         AFR::ota_mqtt
#         AFR::ota_http
# )

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
# We require you to define at least demos and tests executable targets. Available demos and tests
# will be automatically enabled by us. You need to provide other project settings such as linker
# scripts and post build commands.

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)

    if(AFR_IS_TESTING)
        set(exe_target aws_tests)
    else()
        set(exe_target aws_demos)
    endif()
    add_executable(
        ${exe_target}
        "${board_dir}/application_code/main.c"
    )
    target_include_directories(
        ${exe_target}
        PUBLIC
            ${embARC_INC_DIR}
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::kernel
            # AFR::common
            AFR::platform
            # ${embARC_LIB_DIR}/libembarc.a
            # AFR::wifi
            # AFR::utils
            # AFR::ble
    )
    target_compile_definitions(
        ${exe_target}
        PUBLIC
            ${compiler_defined_symbols}
    )

    if(${AFR_TOOLCHAIN} STREQUAL "synopsys-gcc")
    # TODO: add support of openocd
    add_custom_target(run
        COMMAND arc-elf32-gdb ${exe_target}.elf
        DEPENDS ${exe_target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(debug
        COMMAND arc-elf32-gdb ${exe_target}.elf
        DEPENDS ${exe_target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(debug_mdb
        COMMAND mdb -source_path=${embARC_src_dir}/arc/startup -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent ${exe_target}.elf
        DEPENDS ${exe_target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    elseif(${AFR_TOOLCHAIN} STREQUAL "synopsys-mw")
    add_custom_target(run
        COMMAND mdb -source_path=${embARC_src_dir}/arc/startup -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent -run ${exe_target}.elf
        DEPENDS ${exe_target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(debug
        COMMAND mdb -source_path=${embARC_src_dir}/arc/startup -nooptions -nogoifmain -toggle=include_local_symbols=1 -hard -digilent -OS=FreeRTOS ${exe_target}.elf
        DEPENDS ${exe_target}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    endif()
endif()
