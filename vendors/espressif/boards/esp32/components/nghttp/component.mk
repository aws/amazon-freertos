#
# Component Makefile
#

AMAZON_FREERTOS_LIB_DIR := ../../../../../../libraries

# Keep everything the same, EXCEPT prefer coreHTTP http_parser  
COMPONENT_ADD_INCLUDEDIRS := ${AMAZON_FREERTOS_LIB_DIR}/libraries/coreHTTP/source/dependency/3rdparty/http_parser port/include nghttp2/lib/includes
COMPONENT_SRCDIRS := ${AMAZON_FREERTOS_LIB_DIR}/libraries/coreHTTP/source/dependency/3rdparty/http_parser nghttp2/lib port
