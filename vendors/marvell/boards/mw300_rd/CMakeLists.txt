set(mw320_dir "${AFR_VENDORS_DIR}/marvell/WMSDK/mw320")
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests")
if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(exe_target aws_tests)
else()
    set(board_dir "${board_demos_dir}")
    set(exe_target aws_demos)
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "mw320")
    include("${CMAKE_CURRENT_LIST_DIR}/mw320.cmake")
endif()
if("${AFR_BOARD_NAME}" STREQUAL "mw322")
    include("${CMAKE_CURRENT_LIST_DIR}/mw322.cmake")
endif()

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

if(EXISTS "${mw320_dir}/bin/libs/libwlcmgr.a")
target_link_libraries(
    AFR::compiler::mcu_port
    INTERFACE
        "${mw320_dir}/bin/libs/libwlcmgr.a"
	"${mw320_dir}/bin/libs/libwifidriver.a"
	"${mw320_dir}/bin/libs/librfget.a"
	"${mw320_dir}/bin/libs/libpsm.a"
	"${mw320_dir}/bin/libs/libpart.a"
	"${CMAKE_BINARY_DIR}/afr_kernel.a"
)
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
# ==================== Marvell ====================
set(board ${AFR_BOARD_NAME})

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
        "${mw320_dir}/sdk/external/FreeRTOS/Source/FreeRTOS-openocd.c"
)

set(
    kernel_inc_directories
    "${mw320_dir}/sdk/src"
    "${mw320_dir}/sdk/src/incl"
    "${mw320_dir}/sdk/src/incl/libc/arm_gcc"
    "${mw320_dir}/sdk/src/incl/sdk"
    "${mw320_dir}/sdk/src/incl/sdk/drivers"
    "${mw320_dir}/sdk/src/incl/sdk/drivers/wlan"
    "${mw320_dir}/sdk/src/incl/sdk/drivers/mw300"
    "${mw320_dir}/sdk/src/incl/sdk/drivers/mw300/regs"
    "${mw320_dir}/sdk/src/drivers/common/hardfault"
    "${mw320_dir}/sdk/src/boards"
    "${mw320_dir}/sdk/src/incl/platform/net/freertos-plus-tcp"
    "${AFR_KERNEL_DIR}/portable/GCC/ARM_CM4F"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls/include/"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls/include/mbedtls/"
    "${mw320_dir}/sdk/src/incl/platform/os/freertos"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include"
    "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)

if(NOT EXISTS "${mw320_dir}/bin/libs/libwlcmgr.a")
set(
    kernel_inc_lib_directories
    "${mw320_dir}/sdk/src/wlan/wifidriver"
    "${mw320_dir}/sdk/src/wlan/wifidriver/incl"
)
endif()

# Global include directories
target_include_directories(
     AFR::kernel::mcu_port
     INTERFACE ${kernel_inc_directories}
     ${kernel_inc_lib_directories}
)

# WiFi
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${afr_ports_dir}/wifi/iot_wifi.c"
)

# FreeRTOS_Plus_TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/NetworkInterface/mw300_rd/NetworkInterface.c"
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE AFR::pkcs11_mbedtls
)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE "${afr_ports_dir}/pkcs11/iot_pkcs11_pal.c"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_MODULES_ABSTRACTIONS_DIR}/secure_sockets/freertos_plus_tcp/iot_secure_sockets.c"
)

target_include_directories(
    AFR::secure_sockets::mcu_port
    INTERFACE
       "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
       "${AFR_3RDPARTY_DIR}/pkcs11"
       "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX ".axf")

set(default_modules AFR::wifi AFR::utils)
afr_glob_src(board_code_src RECURSE DIRECTORY "${board_dir}/application_code/marvell_code")
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(
        ${exe_target}
        ${board_code_src}
        ${config_files}
        "${board_dir}/application_code/main.c"
    )

    target_include_directories(
        ${exe_target}
        PUBLIC
        "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/include"
        "${AFR_3RDPARTY_DIR}/pkcs11"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/crypto/include"
    )

    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::utils
            ${default_modules}
            -T${global-linkerscript-y}
    )

    if(AFR_METADATA_MODE)
        return()
    endif()

    # Convert afx file to bin file
    if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
        set(axf2fw "${mw320_dir}/sdk/tools/bin/Linux/axf2firmware")
        execute_process(COMMAND chmod +x "${axf2fw}")  # TODO, workaround for FreeRTOS console permission issue.
    else()
        message(FATAL_ERROR "Only Linux host is supported for marvell.")
    endif()

    add_custom_command(
        TARGET ${exe_target} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E remove "${exe_target}.bin"
        COMMAND "${axf2fw}" "${exe_target}.axf" "${exe_target}.bin"
    )

    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${exe_target}.bin" "${exe_target}.map")
endif()
