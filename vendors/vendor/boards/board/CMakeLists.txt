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

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
# Provide metadata for listing on Amazon FreeRTOS console.
afr_set_board_metadata(NAME "board")
afr_set_board_metadata(DISPLAY_NAME "Board")
afr_set_board_metadata(DESCRIPTION "Template Board for AmazonFreeRTOS")
afr_set_board_metadata(VENDOR_NAME "Vendor")
afr_set_board_metadata(FAMILY_NAME "Family")
afr_set_board_metadata(DATA_RAM_MEMORY "0")
afr_set_board_metadata(PROGRAM_MEMORY "0")
afr_set_board_metadata(CODE_SIGNER "")
afr_set_board_metadata(SUPPORTED_IDE "")
afr_set_board_metadata(IDE_<IDE_ID>_NAME "")
afr_set_board_metadata(IDE_<IDE_ID>_COMPILERS "")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
# If you support multiple compilers, you can use AFR_TOOLCHAIN to conditionally define the compiler
# settings. This variable will be set to the file name of CMAKE_TOOLCHAIN_FILE. It might also be a
# good idea to put your compiler settings to different files and just include them here, e.g.,
# include(compilers/${AFR_TOOLCHAIN}.cmake)

# ==================== Example ====================

# afr_mcu_port(compiler)

# Compile definitions/macros
# target_compile_definitions(
#     AFR::compiler::mcu_port
#     INTERFACE
#         AmazonFreeRTOS
# )

# Compiler flags
# target_compile_options(
#     AFR::compiler::mcu_port
#     INTERFACE
#         -std=c11
# )

# Global include directories
# target_include_directories(
#     AFR::compiler::mcu_port
#     INTERFACE
#         "<include_path_0>"
#         "<include_path_1>"
# )

# Linker flags
# target_link_options(
#     AFR::compiler::mcu_port
#     INTERFACE
#         -mcpu=cortex-m4
# )

# Library search path for linker
# target_link_directories(
#     AFR::compiler::mcu_port
#     INTERFACE
#         "<lib_path_0>"
#         "<lib_path_1>"
# )

# Libraries to link
# target_link_libraries(
#     AFR::compiler::mcu_port
#     INTERFACE
#         "foo.a"
#         "/full/path/bar.a"
# )

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# Define portable layer targets with afr_mcu_port(<module_name>). We will create an CMake
# INTERFACE IMPORTED target called AFR::${module_name}::mcu_port for you. You can use it with
# standard CMake functions like target_*. To better organize your files, you can define your own
# targets and use target_link_libraries(AFR::${module_name}::mcu_port INTERFACE <your_targets>)
# to provide the public interface you want expose.

# ==================== Example ====================
# set(vendor ${AFR_VENDOR_NAME})
# set(board ${AFR_BOARD_NAME})

# set(portable_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
# set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
# set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")

# if(AFR_IS_TESTING)
#     set(board_dir "${board_tests_dir}")
#     set(aws_credentials_include "${AFR_TESTS_DIR}/include")
# else()
#     set(board_dir "${board_demos_dir}")
#     set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
# endif()

# Kernel
# afr_mcu_port(kernel)
# target_sources(
#     AFR::kernel::mcu_port
#     INTERFACE
#         ${board_src}              # Normally in ${board_dir}/application_code/${vendor}_code
#         ${driver_src}             # Normally in ${AFR_VENDORS_DIR}/${AFR_VENDOR_NAME}/<board specific code>
#         ${freertos_portable_src}  # Normally in ${AFR_KERNEL_DIR}/portable
# )
# target_include_directories(
#     AFR::kernel::mcu_port
#     INTERFACE
#         "${board_specific_include}"       # Normally ${board_dir}/application_code/${vendor}_code
#         "${board_configs_include}"        # Normally ${board_dir}/application_code/config_files
#         "${driver_public_include}"        # Normally in ${driver_dir}
#         "${freertos_portable_include}"    # Normally in ${AFR_KERNEL_DIR}/portable
#         "${aws_credentials_include}"      # Optional, but in case you need it.
# )
# target_link_libraries(
#     AFR::kernel::mcu_port
#     INTERFACE ${other_targets}
# )

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

# PKCS11
# afr_mcu_port(pkcs11_implementation)
# target_sources(
#     AFR::pkcs11_implementation::mcu_port
#     INTERFACE "${portable_dir}/pkcs11/iot_pkcs11_pal.c"
# )
# Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
# target_link_libraries(
#     AFR::pkcs11_implementation::mcu_port
#     INTERFACE AFR::pkcs11_mbedtls
# )

# Secure sockets
# afr_mcu_port(secure_sockets)
# Link to AFR::secure_sockets_freertos_plus_tcp if you want use default implementation based on
# FreeRTOS-Plus-TCP.
# target_link_libraries(
#     AFR::secure_sockets::mcu_port
#     INTERFACE AFR::secure_sockets_freertos_plus_tcp
# )
# Or provide your own implementation.
# target_sources(
#     AFR::secure_sockets::mcu_port
#     INTERFACE "${portable_dir}/secure_sockets/iot_secure_sockets.c"
# )

# OTA
# afr_mcu_port(ota)
# target_sources(
#     AFR::ota::mcu_port
#     INTERFACE "${portable_dir}/ota/aws_ota_pal.c"
# )

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
# We require you to define at least demos and tests executable targets. Available demos and tests
# will be automatically enabled by us. You need to provide other project settings such as linker
# scripts and post build commands.

# ==================== Example ====================
# set(CMAKE_EXECUTABLE_SUFFIX ".out")

# set(default_modules AFR::wifi AFR::utils)

# if(AFR_IS_TESTING)
#     set(exe_target aws_tests)
# else()
#     set(exe_target aws_demos)
# endif()

# add_executable(${exe_target} "${board_dir}/application_code/main.c")
# target_link_libraries(
#     ${exe_target}
#     PRIVATE
#         ${default_modules}
#         ${additional_linker_file_and_flags}
# )
# add_custom_command(
#     TARGET ${exe_target} POST_BUILD
#     COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${exe_target}>" "${CMAKE_BINARY_DIR}"
# )
