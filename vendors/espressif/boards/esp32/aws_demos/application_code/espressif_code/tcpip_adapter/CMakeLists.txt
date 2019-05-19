set(AMAZON_FREERTOS_LIB_DIR "${AFR_MODULES_STANDARD_DIR}")

set(COMPONENT_ADD_INCLUDEDIRS include ${AMAZON_FREERTOS_LIB_DIR}/freertos_plus_tcp/include
 ${AMAZON_FREERTOS_LIB_DIR}/freertos_plus_tcp/source/portable/Compiler/GCC
)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS .)

register_component()
