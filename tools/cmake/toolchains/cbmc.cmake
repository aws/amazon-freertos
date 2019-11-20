# ``````````````````````````````````````````````````````````````````````````````
#               Toolchain for building and running CBMC proofs
#               ==============================================
#
# CBMC proofs require include paths, compile definitions, and other variables
# that are common to all proofs. At the same time, individual proofs may wish to
# extend these options---for example, including headers only needed by that
# proof, or proving more than just memory safety.
#
# This file sets up variables that can be appended to in the CMakeLists.txt of
# each proof. Most of the recipes for building and running proofs are written in
# the cbmc_recipes.cmake file. cbmc_recipes.cmake gets included at the bottom of
# each proof, and uses the variables defined in this toolchains file. This file
# is only processed once when using the CBMC toolchain, while cbmc_recipes.cmake
# gets included separately by each proof.
# ______________________________________________________________________________

set(CBMC 1)

# ______________________________________________________________________________
#     Variables that are not intended to be directly changed by proof-writers
# ``````````````````````````````````````````````````````````````````````````````

# The list of all proofs so far, used to check that all proof names are unique
set(cbmc_proof_names "" CACHE INTERNAL "")
set(cbmc_dir ${CMAKE_SOURCE_DIR}/tools/cbmc)


# ______________________________________________________________________________
#     Strings that proof-writers can change for a particular proof in each
#     proof's CMakeLists.txt
# ``````````````````````````````````````````````````````````````````````````````

# How many times CBMC should unwind all loops in the program. It's usually best
# to keep this set to 1, and to increase the loop bound for individual loops
# using cbmc_unwindset if necessary.
set(cbmc_unwind 1)

set(cbmc_objectid_bits 7)
set(cbmc_max_object_size "(UINT32_MAX>>CBMC_OBJECTID_BITS)")


# ______________________________________________________________________________
#     Lists that proof-writers can append to in each proof's CMakeLists.txt
# ``````````````````````````````````````````````````````````````````````````````

# Override the unwind limit for particular loops in the program. Append to this
# list strings of the form LOOP_NAME:UNWIND_LIMIT
set(cbmc_unwindset BOGUS_LOOP.0:1)

set(cbmc_compile_options
    -m32
)

set(cbmc_compile_definitions
    CBMC
    WINVER=0x400
    _CONSOLE
    _CRT_SECURE_NO_WARNINGS
    _DEBUG
    _WIN32_WINNT=0x0500
    __PRETTY_FUNCTION__=__FUNCTION__
    __free_rtos__
)

# Proof-writers can append additional CBMC checks, like --signed-overflow-check.
# The checks below are for memory safety.
set(cbmc_flags
    --32
    --bounds-check
    --pointer-check
)

set(cbmc_include_directories
    ${CMAKE_SOURCE_DIR}/demos/include
    ${CMAKE_SOURCE_DIR}/freertos_kernel/include
    ${CMAKE_SOURCE_DIR}/freertos_kernel/portable/MSVC-MingW
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/http_parser
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/jsmn
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/mbedtls/include/mbedtls
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/pkcs11
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/tinycbor
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/tracealyzer_recorder/Include
    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/win_pcap
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/freertos/include/
    ${CMAKE_SOURCE_DIR}/libraries/abstractions/platform/include/
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/aws/defender/include
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/common/include/
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/include
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/https/src/private
    ${CMAKE_SOURCE_DIR}/libraries/c_sdk/standard/serializer/src/cbor
    ${CMAKE_SOURCE_DIR}/libraries/freertos_plus/aws/ota/include
    ${CMAKE_SOURCE_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/include
    ${CMAKE_SOURCE_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/BufferManagement
    ${CMAKE_SOURCE_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/source/portable/Compiler/MSVC
    ${CMAKE_SOURCE_DIR}/vendors/pc/boards/windows/aws_demos/application_code
    ${CMAKE_SOURCE_DIR}/vendors/pc/boards/windows/aws_demos/config_files
    ${cbmc_dir}/include
    ${cbmc_dir}/windows
)


# Each CBMC proof has a list of labels; CTest can run all proofs that have a
# particular label. All CBMC proofs have the 'cbmc' label. Each FreeRTOS module
# should get its own label, and each individual proof should also get its own
# label.
set(cbmc_test_labels
    "cbmc"
)


# ______________________________________________________________________________
#     Use goto-cc (the CBMC compiler) and do other FreeRTOS-specific setup
# ``````````````````````````````````````````````````````````````````````````````

include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

afr_find_compiler(AFR_COMPILER_CC goto-cc)
afr_find_compiler(AFR_COMPILER_CXX goto-cc)
set(AFR_COMPILER_ASM "${AFR_COMPILER_CC}" CACHE INTERNAL "")

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${AFR_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${AFR_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${AFR_COMPILER_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# CBMC proofs are run as tests using CTest, so unconditionally enable this
enable_testing()
