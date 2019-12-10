set(cbmc_proof_name IotHttpsClient_ReadResponseStatus)
list(APPEND cbmc_test_labels ${cbmc_proof_name})

list(APPEND cbmc_flags
  --div-by-zero-check
  --float-overflow-check
  --nan-check
  --nondet-static
  --pointer-overflow-check
  --signed-overflow-check
  --undefined-shift-check
  --unsigned-overflow-check
)

list(PREPEND cbmc_include_directories
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/include
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/src/private
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/http_parser
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/common/include
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/include
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/freertos/include
    ${cbmc_dir}/proofs/HTTP
)

add_executable(${cbmc_proof_name}_0010_project.goto
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/src/iot_https_client.c
)

add_executable(${cbmc_proof_name}_0010_harness.goto
    ${CMAKE_CURRENT_SOURCE_DIR}/${cbmc_proof_name}_harness.c
)

include(${CMAKE_SOURCE_DIR}/tools/cmake/cbmc_recipies.cmake)
