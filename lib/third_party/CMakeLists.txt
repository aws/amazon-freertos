# -------------------------------------------------------------------------------------------------
# 3rdparty libraries
# -------------------------------------------------------------------------------------------------
# jsmn - JSON parser
afr_3rdparty_module(jsmn)
target_sources(
    3rdparty::jsmn INTERFACE
    "${AFR_3RDPARTY_DIR}/jsmn/jsmn.h"
    "${AFR_3RDPARTY_DIR}/jsmn/jsmn.c"
)
target_include_directories(
    3rdparty::jsmn INTERFACE
    "${AFR_3RDPARTY_DIR}/jsmn"
)

# mbedtls - TLS protocol implementation
afr_glob_src(mbedtls_src DIRECTORY "${AFR_3RDPARTY_DIR}/mbedtls" RECURSE)
# Build to static library to save some time.
add_library(
    afr_3rdparty_mbedtls STATIC
    ${mbedtls_src}
)
target_include_directories(
    afr_3rdparty_mbedtls
    PUBLIC
        "${AFR_3RDPARTY_DIR}/mbedtls/include"
)
target_link_libraries(
    afr_3rdparty_mbedtls
    PRIVATE AFR::kernel
)
add_library(3rdparty::mbedtls ALIAS afr_3rdparty_mbedtls)


# pkcs11 standard header
afr_3rdparty_module(pkcs11)
target_include_directories(
    3rdparty::pkcs11 INTERFACE
    "${AFR_3RDPARTY_DIR}/pkcs11"
)

# tinycbor - CBOR implementation
afr_3rdparty_module(tinycbor)
afr_glob_src(tinycbor_src DIRECTORY "${AFR_3RDPARTY_DIR}/tinycbor" RECURSE)
target_sources(
    3rdparty::tinycbor INTERFACE
    ${tinycbor_src}
)
target_include_directories(
    3rdparty::tinycbor INTERFACE
    "${AFR_3RDPARTY_DIR}/tinycbor"
)

# tracealyzer - Trace record library from FreeRTOS
afr_3rdparty_module(tracealyzer)
target_sources(
    3rdparty::tracealyzer INTERFACE
    "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/trcKernelPort.c"
    "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/trcSnapshotRecorder.c"
)
target_include_directories(
    3rdparty::tracealyzer INTERFACE
    "${AFR_3RDPARTY_DIR}/tracealyzer_recorder/include"
)

# WinPcap - pcap port for windows
afr_3rdparty_module(winpcap)
target_include_directories(3rdparty::winpcap INTERFACE "${AFR_3RDPARTY_DIR}/win_pcap")
if(MSVC)
    target_link_libraries(3rdparty::winpcap INTERFACE "${AFR_3RDPARTY_DIR}/win_pcap/wpcap.lib")
elseif(MINGW)
    target_link_libraries(3rdparty::winpcap INTERFACE "${AFR_3RDPARTY_DIR}/win_pcap/libwpcap.a")
endif()

# unity - test framework
afr_3rdparty_module(unity)
target_sources(
    3rdparty::unity INTERFACE
    "${AFR_3RDPARTY_DIR}/unity/src/unity.c"
    "${AFR_3RDPARTY_DIR}/unity/src/unity.h"
    "${AFR_3RDPARTY_DIR}/unity/src/unity_internals.h"
    "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src/unity_fixture.c"
    "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src/unity_fixture.h"
    "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src/unity_fixture_internals.h"
    "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src/unity_fixture_malloc_overrides.h"
)
target_include_directories(
    3rdparty::unity INTERFACE
    "${AFR_3RDPARTY_DIR}/unity/src"
    "${AFR_3RDPARTY_DIR}/unity/extras/fixture/src"
)
target_compile_definitions(3rdparty::unity INTERFACE UNITY_INCLUDE_CONFIG_H)

# cmock - mock framework
afr_3rdparty_module(cmock)
target_sources(
    3rdparty::cmock INTERFACE
    "${AFR_3RDPARTY_DIR}/cmock/src/cmock.c"
    "${AFR_3RDPARTY_DIR}/cmock/src/cmock.h"
    "${AFR_3RDPARTY_DIR}/cmock/src/cmock_internals.h"
)
target_include_directories(
    3rdparty::cmock INTERFACE
    "${AFR_3RDPARTY_DIR}/cmock/src"
)
