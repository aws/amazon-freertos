set(AMAZON_FREERTOS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../lib")

set(COMPONENT_ADD_INCLUDEDIRS include ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/include
 ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC
)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS .)

register_component()
