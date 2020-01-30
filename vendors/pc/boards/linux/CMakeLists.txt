if (AFR_ENABLE_UNIT_TESTS)
    message("building linux specific Unit Test port")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tools/cmock)

    include(create_test)
    include_directories("${CMAKE_CURRENT_LIST_DIR}/unit_test/config_files"
            "${CMAKE_CURRENT_LIST_DIR}/unit_test/utils"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
            "${AFR_ROOT_DIR}/freertos_kernel/include"
            "${AFR_ROOT_DIR}/libraries/3rdparty/unity/src"
            "${AFR_ROOT_DIR}/libraries/3rdparty/tracealyzer_recorder/Include"
            "${AFR_ROOT_DIR}/freertos_kernel/portable/GCC/Linux/"
            "${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src"
        )
    link_directories("${CMAKE_BINARY_DIR}/lib"
        )

    add_library(libcmock  STATIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src/cmock.c
        )

    set_target_properties(libcmock PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
            COMPILE_FLAGS "-O0"
        )

    add_library(libunity STATIC 
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/vendor/unity/src/unity.c
        )
    set_target_properties(libunity PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
            COMPILE_FLAGS "-O0"
        )

    target_include_directories(libcmock PUBLIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src
        )
    target_link_libraries(libcmock libunity.a)

    add_library(libutils STATIC
                ${CMAKE_CURRENT_LIST_DIR}/unit_test/utils/wait_for_event.c)
    set_target_properties(libutils PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            COMPILE_FLAGS "-Wall -O0"
        )

    # add unit test subdirectories here
    add_subdirectory("libraries")

    add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/tools/cmock/coverage.cmake
            DEPENDS secure_sockets_utest libcmock libunity
        )
endif ()
