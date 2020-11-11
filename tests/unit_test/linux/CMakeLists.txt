    message("building linux specific Unit Test port")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tools/cmock)

    include(create_test)
    include_directories(
            "${CMAKE_CURRENT_LIST_DIR}/config_files"
            "${CMAKE_CURRENT_LIST_DIR}/utils"
            "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
            "${AFR_ROOT_DIR}/freertos_kernel/include"
            "${AFR_ROOT_DIR}/libraries/3rdparty/tracealyzer_recorder/Include"
            "${AFR_ROOT_DIR}/libraries/3rdparty/CMock/vendor/unity/src/"
            "${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src"
            "${AFR_ROOT_DIR}/libraries/coreMQTT/source/interface"
            )
    link_directories("${CMAKE_BINARY_DIR}/lib"
            )

    add_library(cmock  STATIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src/cmock.c
            )

    set_target_properties(cmock PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
            COMPILE_FLAGS "-Og"
            )

    add_library(unity STATIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/vendor/unity/src/unity.c
            )
    set_target_properties(unity PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            POSITION_INDEPENDENT_CODE ON
            )

    target_include_directories(cmock PUBLIC
            ${AFR_ROOT_DIR}/libraries/3rdparty/CMock/src
            )
    target_link_libraries(cmock libunity.a)

    add_library(utils SHARED
                ${CMAKE_CURRENT_LIST_DIR}/utils/wait_for_event.c
                ${CMAKE_CURRENT_LIST_DIR}/utils/task_control.c
            )
    set_target_properties(utils PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            LINK_FLAGS " -pthread"
            )

    # add unit test subdirectories here
    add_subdirectory(../../../libraries libraries)

    add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/tools/cmock/coverage.cmake
            DEPENDS transport_secure_sockets_utest secure_sockets_utest cmock unity
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )
