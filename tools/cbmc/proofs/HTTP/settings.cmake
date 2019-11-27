include(${CMAKE_CURRENT_LIST_DIR}/../settings.cmake)

list(PREPEND cbmc_compile_includes
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/include
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/src/private
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/http_parser
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/common/include
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/include
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/freertos/include
    ${cbmc_dir}/proofs/HTTP
)
