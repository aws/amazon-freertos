set(AMAZON_FREERTOS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../lib") 
set(AMAZON_FREERTOS_DEMOS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../demos")
set(AMAZON_FREERTOS_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../tests")

set(COMPONENT_ADD_INCLUDEDIRS include  ../../../config_files ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS-Plus-POSIX/include ${AMAZON_FREERTOS_LIB_DIR}/include ${AMAZON_FREERTOS_LIB_DIR}/include/private ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32/include ${AMAZON_FREERTOS_LIB_DIR}/third_party/unity/src)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCDIRS . ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32 ${AMAZON_FREERTOS_LIB_DIR}/FreeRTOS)

register_component()
