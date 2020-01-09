if (AFR_ENABLE_UNIT_TESTS)
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    message("building linux specific Unit Test port")
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tools/cmock)
    include(create_test)
    message("cmake module path  ${CMAKE_MODULE_PATH}")

    include_directories("${CMAKE_CURRENT_LIST_DIR}/unit_test/config_files"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
            "${AFR_ROOT_DIR}/freertos_kernel/include"
            "${AFR_ROOT_DIR}/libraries/3rdparty/unity/src"
            "${AFR_ROOT_DIR}/libraries/3rdparty/tracealyzer_recorder/Include"
            "${AFR_ROOT_DIR}/freertos_kernel/portable/GCC/Linux/"
            "${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src"
        )
    link_directories("${CMAKE_BINARY_DIR}/lib")
    add_library(libcmock  STATIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src/cmock.c
        )
    set_target_properties(libcmock PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
        )

    add_library(libunity STATIC 
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/vendor/unity/src/unity.c
        )
    set_target_properties(libunity PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
        )

    target_include_directories(libcmock PUBLIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src
        )
    target_link_libraries(libcmock libunity.a)

    # add unit test subdirectories here
    add_subdirectory("libraries")

    add_custom_target(genhtml
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/tools/cmock/coverage.cmake
        )
endif ()
