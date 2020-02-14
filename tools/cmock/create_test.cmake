
#function to create the test executable
function(create_test test_name test_src link_list dep_list)
    set(mocks_dir "${CMAKE_CURRENT_BINARY_DIR}/mocks")
    include (CTest)
    get_filename_component(test_src_absolute ${test_src} ABSOLUTE)
    add_custom_command(OUTPUT ${test_name}_runner.c
                  COMMAND ruby
                    ${CMAKE_SOURCE_DIR}/libraries/3rdparty/unity/auto/generate_test_runner.rb
                    ${CMAKE_SOURCE_DIR}/tools/cmock/project.yml
                    ${test_src_absolute}
                    ${test_name}_runner.c
                  DEPENDS ${test_src}
        )
    add_executable(${test_name} ${test_src} ${test_name}_runner.c)
    set_target_properties(${test_name} PROPERTIES
            COMPILE_FLAGS "-ggdb3 -Og -Wall -pthread"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
            INSTALL_RPATH_USE_LINK_PATH TRUE
            LINK_FLAGS "-Og -ggdb3 -pthread \
                -Wl,-rpath,${CMAKE_CURRENT_BINARY_DIR}/lib"
        )
    target_include_directories(${test_name} PUBLIC
                               ${mocks_dir}
        )
    target_link_directories(${test_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# link all libraries sent through parameters
    foreach(link IN LISTS link_list)
        target_link_libraries(${test_name} ${link})
    endforeach()

# add dependency to all the dep_list parameter
    foreach(dependency IN LISTS dep_list)
        add_dependencies(${test_name} ${dependency})
    endforeach()
    target_link_libraries(${test_name} -lgcov)

    target_link_directories(secure_sockets_utest  PUBLIC
                            ${CMAKE_CURRENT_BINARY_DIR}/lib
            )
    add_test(NAME ${test_name}
             COMMAND ${CMAKE_BINARY_DIR}/bin/${test_name}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
#add_test(${test_name} ${CMAKE_BINARY_DIR}/bin/${test_name})
endfunction()

# Generates a mock library based on a module's header file
# places the generated source file in the build directory
function(create_mock_list mock_name mock_list)
    set(mocks_dir "${CMAKE_CURRENT_BINARY_DIR}/mocks")
    add_library(${mock_name} SHARED)
    foreach (mock_file IN LISTS mock_list)
        get_filename_component(mock_file_abs
                               ${mock_file}
                               ABSOLUTE
                )
        get_filename_component(mock_file_name
                               ${mock_file}
                               NAME_WLE
                )
        get_filename_component(mock_file_dir
                               ${mock_file}
                               DIRECTORY
                )
        add_custom_command (
                  OUTPUT ${mocks_dir}/mock_${mock_file_name}.c
                  COMMAND ruby
                  ${CMAKE_SOURCE_DIR}/libraries/3rdparty/CMock/lib/cmock.rb
                  -o${CMAKE_SOURCE_DIR}/tools/cmock/project.yml ${mock_file_abs}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                )
        target_sources(${mock_name} PUBLIC
                       ${mocks_dir}/mock_${mock_file_name}.c
                )

        target_include_directories(${mock_name} PUBLIC
                                   ${mock_file_dir}
                )
    endforeach()
    target_include_directories(${mock_name} PUBLIC
                               ${mocks_dir}
            )
    set_target_properties(${mock_name} PROPERTIES
                        COMPILE_FLAGS "-ggdb3 -Og -fPIC"
                        LINK_FLAGS "-ggdb3 -fPIC -Og"
                        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
                        POSITION_INDEPENDENT_CODE ON
            )
    target_link_libraries(${mock_name} libcmock.a libunity.a)
endfunction()
