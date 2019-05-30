set(AMAZON_FREERTOS_FREERTOS_PLUS_DIR "${AFR_MODULES_FREERTOS_PLUS_DIR}")

set(COMPONENT_ADD_INCLUDEDIRS include ${AMAZON_FREERTOS_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/include
 ${AMAZON_FREERTOS_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/source/portable/Compiler/GCC
)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS .)

register_component()
