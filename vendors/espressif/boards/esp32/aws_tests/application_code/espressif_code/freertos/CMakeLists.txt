set(AMAZON_FREERTOS_KERNEL_DIR "${AFR_KERNEL_DIR}")
set(AMAZON_FREERTOS_FREERTOS_PLUS_DIR "${AFR_MODULES_FREERTOS_PLUS_DIR}")
set(AMAZON_FREERTOS_3RDPARTY_DIR "${AFR_3RDPARTY_DIR}")

set(
    COMPONENT_ADD_INCLUDEDIRS
    include
    ../../../config_files
    ${AMAZON_FREERTOS_KERNEL_DIR}/include
    ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32/include
    ${AMAZON_FREERTOS_FREERTOS_PLUS_DIR}/standard/freertos_plus_posix/include
    ${AMAZON_FREERTOS_3RDPARTY_DIR}/unity/src
)


# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS . ${AMAZON_FREERTOS_KERNEL_DIR}/portable/ThirdParty/GCC/Xtensa_ESP32 ${AMAZON_FREERTOS_KERNEL_DIR})

register_component()
