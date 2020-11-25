set(AMAZON_FREERTOS_KERNEL_DIR "${AFR_KERNEL_DIR}")
set(AMAZON_FREERTOS_FREERTOS_PLUS_DIR "${AFR_MODULES_FREERTOS_PLUS_DIR}")
set(AMAZON_FREERTOS_3RDPARTY_DIR "${AFR_3RDPARTY_DIR}")

set(
    include_dirs
    include
    ${AMAZON_FREERTOS_KERNEL_DIR}/include
    ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32/include
)

if(AFR_ENABLE_TESTS)
    list(APPEND include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/../../aws_tests/config_files"
        "${AMAZON_FREERTOS_FREERTOS_PLUS_DIR}/standard/freertos_plus_posix/include"
        "${AMAZON_FREERTOS_3RDPARTY_DIR}/unity/src"
        )
else()
    list(APPEND include_dirs
        "${CMAKE_CURRENT_LIST_DIR}/../../aws_demos/config_files")
endif()

set(COMPONENT_ADD_INCLUDEDIRS "${include_dirs}")

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS . ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32 ${AMAZON_FREERTOS_KERNEL_DIR})

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()
