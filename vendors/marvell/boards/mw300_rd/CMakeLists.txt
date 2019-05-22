set(mw320_dir "${AFR_3RDPARTY_DIR}/mcu_vendor/marvell/WMSDK/mw320")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(exe_target aws_demos)
endif()

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
afr_set_board_metadata(ID "MW320")
afr_set_board_metadata(DISPLAY_NAME "mw300_rd")
afr_set_board_metadata(DESCRIPTION "Marvell Board for AmazonFreeRTOS")
afr_set_board_metadata(VENDOR_NAME "Marvell")
afr_set_board_metadata(FAMILY_NAME "Wireless Microcontroller")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(DATA_RAM_MEMORY "512KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(SUPPORTED_IDE "Eclipse")
afr_set_board_metadata(RECOMMENDED_IDE "Eclipse")
afr_set_board_metadata(IDE_SystemWorkbench_NAME "Eclipse")
afr_set_board_metadata(IDE_SystemWorkbench_COMPILER "GCC")
afr_set_board_metadata(IDE_SystemWorkbench_RECOMMENDED "TRUE")

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

afr_mcu_port(compiler)
set(
    defined_symbols
    MW320
)

# Compile definitions/macros
target_compile_definitions(
    AFR::compiler::mcu_port
    INTERFACE MW320
)

# Compiler flags
# cflags
set(SDK_VERSION_INTERNAL "\"v1.2.r6.p1\"")
set(SDK_VERSION ${SDK_VERSION_INTERNAL})

set(global-cflags-y
	"-mcpu=cortex-m4"
	"-mthumb"
	"-g"
	"-Os"
	"-fdata-sections"
	"-ffunction-sections"
	"-ffreestanding"
	"-MMD"
	"-Wall"
	"-fno-strict-aliasing"
	"-fno-common"
	"-Wextra"
	"-Wno-unused-parameter"
	"-Wno-empty-body"
	"-Wno-missing-field-initializers"
	"-Wno-sign-compare"
	"-Wno-type-limits"
	"-std=c99"
	"-DSDK_VERSION=${SDK_VERSION}"
)

# ARM Cortex M4 flags
set(tc-lflags-tc-cortex-m4-y
	"-mcpu=cortex-m4"
	"-mthumb"
	"-g"
	"-Os"
	"-fdata-sections"
	"-ffunction-sections"
	"-ffreestanding"
	"-MMD"
	"-Wall"
	"-fno-strict-aliasing"
	"-fno-common"
	"-mfloat-abi=softfp"
	"-mfpu=fpv4-sp-d16"
	"-D__FPU_PRESENT"
)

target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE ${global-cflags-y} ${tc-lflags-tc-cortex-m4-y}
    	-include ${mw320_dir}/sdk/src/incl/autoconf.h
)

# Global include directories
target_include_directories(
     AFR::compiler::mcu_port
     INTERFACE
         "${mw320_dir}/sdk/src"
	 "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/"
	 "${board_dir}/config_files"
)

# Linker flags
# gcc specific extra linker flags
set(tc-lflags-y
	"-nostartfiles"
	"-Wl,--wrap,malloc"
	"-Wl,--wrap,free"
	"-Wl,--wrap,calloc"
	"-Wl,--wrap,realloc"
	"-Wl,--wrap,printf"
	"-Wl,--wrap,fprintf"
)

# Xlinker flags
set (CMAKE_C_LINKER_WRAPPER_FLAG "-Xlinker" " ")
add_link_options("LINKER:--undefined")
add_link_options("LINKER:uxTopUsedPriority")
add_link_options("LINKER:--cref")
add_link_options("LINKER:--gc-sections")
add_link_options("LINKER:--defsym=_rom_data=64")
add_link_options("LINKER:-M")
add_link_options("LINKER:-Map")
add_link_options("LINKER:${exe_target}.map")

# Linker Script
set(global-linkerscript-y "${mw320_dir}/build/toolchains/arm_gcc/mw300.ld")

target_link_options(
     AFR::compiler::mcu_port
     INTERFACE ${tc-lflags-y} ${tc-lflags-tc-cortex-m4-y}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# ==================== Marvell ====================
set(vendor ${AFR_VENDOR_NAME})
set(board ${AFR_BOARD_NAME})
set(portable_dir "portable/${vendor}/${board}")

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src RECURSE DIRECTORY "${mw320_dir}/sdk/src")
afr_glob_src(driver_headers RECURSE DIRECTORY "${mw320_dir}/sdk/src" EXTENSIONS h)

list(REMOVE_ITEM driver_src "${mw320_dir}/sdk/src/libc/src/atox.c")
list(REMOVE_ITEM driver_src "${mw320_dir}/sdk/src/libc/src/strtox.c")

target_sources(
     AFR::kernel::mcu_port
     INTERFACE
          ${driver_src}
	  #${driver_headers}
	  "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/port.c"
	  "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F/portmacro.h"
	  "${mw320_dir}/sdk/external/FreeRTOS/portable/MemMang/heap_marvell.c"
	  "${mw320_dir}/sdk/external/FreeRTOS//Source/FreeRTOS-openocd.c"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${simplelink_dir}/source"
        "${board_dir}/config_files"
        "${board_dir}/application_code/ti_code"
        # Need aws_clientcredential.h
        "$<IF:${AFR_IS_TESTING},${AFR_TEST_DIR},${AFR_DEMOS_DIR}>/include"
        ${compiler_specific_include}
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        AFR::posix
        AFR::posix_headers
)

# POSIX
# afr_mcu_port(posix)
# target_include_directories(
#     AFR::posix::mcu_port
#     INTERFACE "${AFR_MODULES_DIR}/FreeRTOS-Plus-POSIX/include/${portable_dir}"
# )

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/aws_wifi.c"
)

# FreeRTOS_Plus_TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/mw300_rd/NetworkInterface.c"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
)

# Link to AFR::pkcs11_mbedtls if you want to use default implementation based on mbedtls.
target_link_libraries(
    AFR::pkcs11::mcu_port
    INTERFACE
        AFR::pkcs11_mbedtls
        AFR::wifi
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE
        AFR::tls
        AFR::wifi
)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${afr_ports_dir}/secure_sockets/aws_secure_sockets.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

set(default_modules AFR::wifi AFR::utils)
afr_glob_src(board_code_src RECURSE DIRECTORY "${board_dir}/application_code/marvell_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

add_executable(
    ${exe_target}
    ${board_code_src}
    ${config_files}
    "${board_dir}/application_code/main.c"
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        ${default_modules}
        -T${global-linkerscript-y}
)

# Convert afx file to bin file
set(axf2fw "${mw320_dir}/sdk/tools/bin/Linux/axf2firmware")
add_custom_command(
     TARGET ${exe_target} POST_BUILD
     COMMAND "${CMAKE_COMMAND}" -E remove "${exe_target}.bin"
     COMMAND "${axf2fw}" "${exe_target}.axf" "${exe_target}.bin"
)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${exe_target}.bin" "${exe_target}.map")
