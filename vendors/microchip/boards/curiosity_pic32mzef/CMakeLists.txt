# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------
if("${AFR_BOARD_NAME}" STREQUAL "curiosity_pic32mzef")
    include("curiosity_pic32mzef.cmake")
endif()

# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

# Compiler flags
set(c_flags -g -x c -DIOT_CONFIG_FILE=\\\"iot_demo_config.h\\\" -ffunction-sections -O1 -mprocessor=32MZ2048EFM100 -mnewlib-libc -std=gnu99 -fgnu89-inline)
set(asm_flags -mprocessor=32MZ2048EFM100 -MMD -MF -DXPRJ_pic32mz_ef_curiosity=pic32mz_ef_curiosity -no-legacy-libc -Wa$<COMMA>--gdwarf-2)
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
    $<IF:$<COMPILE_LANGUAGE:C>,${c_flags},${asm_flags}>
)

# Linker flags
set(linker_flags -Wl,--script=\"/Users/ejacobus/gitroot/amazon-freertos/vendors/microchip/boards/curiosity_pic32mzef/aws_demos/common/application_code/microchip_code/app_mz.ld\",--defsym=_min_heap_size=170000,--defsym=_min_stack_size=10000,--gc-sections,--no-code-in-dinit,--no-dinit-in-serial-mem,--allow-multiple-definition)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE ${linker_flags}
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------
set(afr_ports_dir "${CMAKE_CURRENT_LIST_DIR}/ports")
set(board_demos_dir "${CMAKE_CURRENT_LIST_DIR}/aws_demos/common")
set(board_tests_dir "${CMAKE_CURRENT_LIST_DIR}/aws_tests/common")

if(AFR_IS_TESTING)
    set(board_dir "${board_tests_dir}")
    set(aws_credentials_include "${AFR_TESTS_DIR}/include")
else()
    set(board_dir "${board_demos_dir}")
    set(aws_credentials_include "${AFR_DEMOS_DIR}/include")
endif()

# Kernel
afr_mcu_port(kernel)
afr_glob_src(driver_src RECURSE DIRECTORY "${AFR_ROOT_DIR}/lib/third_party/mcu_vendor/microchip/harmony/v2.05/framework") 
target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/port.c"
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/port_asm.S"
        "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ/portmacro.h"
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
)

set(
    kernel_inc_dirs
    "${AFR_MODULES_STANDARD_DIR}/common/include"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/jsmn"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/pkcs11"
    "${AFR_ROOT_DIR}/modules/libraries/3rdparty/tinycbor"  
    "${AFR_MODULES_STANDARD_DIR}/crypto/include"     
    "${AFR_MODULES_STANDARD_DIR}/tls/include"
    "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/include"
    "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/Compiler/GCC"
    "${AFR_ROOT_DIR}/modules/ports/wifi/include"
    "${AFR_ROOT_DIR}/modules/ports/pkcs11/include"
    "${board_dir}/application_code/microchip_code"
    "${AFR_KERNEL_DIR}/portable/MPLAB/PIC32MZ"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/bsp"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/system/common"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/include"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/common/include"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/include"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver_extension/driver/source"
    "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework/driver/wifi/wilc1000/wireless_driver/include"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        ${kernel_inc_dirs}
        "${aws_credentials_include}"
        "${board_dir}/config_files"
#        "${CMAKE_BINARY_DIR}/config"
)


# WiFi
afr_mcu_port(wifi)
target_link_libraries(
    AFR::wifi::mcu_port
    INTERFACE
    AFR::freertos_plus_tcp
)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE 
        "${afr_ports_dir}/wifi/aws_wifi.c"
        "${afr_ports_dir}/wifi/aws_wifi_assert.c"
)

target_include_directories(
    AFR::wifi::mcu_port
    INTERFACE
        "${AFR_ROOT_DIR}/modules/ports/secure_sockets/include"
        "${AFR_MODULES_STANDARD_DIR}/common/include/private"
)

# PKCS11
afr_mcu_port(pkcs11)
target_sources(
    AFR::pkcs11::mcu_port
    INTERFACE 
        "${afr_ports_dir}/pkcs11/aws_pkcs11_pal.c"
        "${afr_ports_dir}/pkcs11/pkcs11_nvm.c"
)


# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/BufferAllocation_2.c"
        "${AFR_MODULES_STANDARD_DIR}/freertos_plus_tcp/source/portable/NetworkInterface/pic32mzef/NetworkInterface_wifi.c"
)


# Secure sockets
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE "${AFR_ROOT_DIR}/modules/ports/secure_sockets/freertos_plus_tcp/aws_secure_sockets.c"
)


# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${afr_ports_dir}/ota/aws_ota_pal.c"
    INTERFACE "${afr_ports_dir}/ota/aws_nvm.c"
)

# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------

if(AFR_IS_TESTING)
    set(exe_target aws_tests)
    set(extra_exe_sources "${AFR_TESTS_DIR}/src/iot_tests_network.c")
else()
    set(exe_target aws_demos)
    set(
        extra_exe_sources
        ${NETWORK_MANAGER_SOURCES}
    )
endif()

#afr_glob_src(driver_code RECURSE DIRECTORY "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/framework") 
afr_glob_src(config_files DIRECTORY "${board_dir}/config_files")
add_executable(
    ${exe_target}
    ${config_files}
    "${board_dir}/application_code/main.c"
#    ${driver_code}
)

target_link_libraries(
    ${exe_target}
    PRIVATE
        AFR::wifi
        AFR::utils
        "${AFR_ROOT_DIR}/vendors/microchip/harmony/v2.05/bin/framework/peripheral/PIC32MZ2048EFM100_peripherals.a"
)

target_include_directories(
    ${exe_target}
    INTERFACE
        "${board_dir}/config_files"
)


if(AFR_NON_BUILD_MODE)
    return()
endif()


# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

set_source_files_properties(${AFR_MODULES_DIR}/greengrass/aws_greengrass_discovery.c
    ${AFR_DEMOS_DIR}/tcp/aws_tcp_echo_client_single_task.c
    ${AFR_DEMOS_DIR}/secure_sockets/aws_test_tcp.c
    ${AFR_DEMOS_DIR}/wifi/aws_test_wifi.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)

set_source_files_properties(${AFR_DEMOS_DIR}/logging/aws_logging_task_dynamic_buffers.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-format -Wno-uninitialized"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_pal.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign -Wno-sizeof-pointer-memaccess"
)

set_source_files_properties(${AFR_DEMOS_DIR}/ota/aws_test_ota_agent.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-pointer-sign"
)

set_source_files_properties(${AFR_DEMOS_DIR}/posix/aws_test_posix_pthread.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-int-conversion"
)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

#add_custom_command(
#    TARGET ${exe_target} POST_BUILD
#    COMMAND python ../../../common/ota/bootloader/utility/binary_image_generator.py -d ${MP_CC_DIR} -b xc32-objcopy -p "-I ihex ${ImagePath} -O binary ${ImageDir}/mplab.${IMAGE_TYPE}.bin" && python ../../../common/ota/bootloader/utility/ota_image_generator.py -b ${ImageDir}/mplab.${IMAGE_TYPE}.bin -p MCHP-Curiosity-PIC32MZEF
#)
