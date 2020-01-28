cmake_minimum_required(VERSION 3.13)

    message("base binary dir is ${CMAKE_BINARY_DIR}")
    execute_process(
                COMMAND lcov --directory ${CMAKE_BINARY_DIR}
                             -b ${CMAKE_BINARY_DIR}
                             --zerocounters

                COMMAND mkdir -p  ${CMAKE_BINARY_DIR}/coverage
            )
    execute_process( COMMAND lcov --directory ${CMAKE_BINARY_DIR}
                             -b ${CMAKE_BINARY_DIR}
                             --initial
                             --capture
                             --output-file=${CMAKE_BINARY_DIR}/base_coverage.info
            )
    message("output base_Coverage .info")
    file(GLOB files "${CMAKE_BINARY_DIR}/bin/*")

    foreach(testname ${files})

        get_filename_component(test
                               ${testname}
                               NAME_WLE
                        )
        message("Running ${testname}")
        execute_process(COMMAND ${testname} OUTPUT_FILE ${CMAKE_BINARY_DIR}/${test}_out.txt)
        execute_process(COMMAND ruby
                ${CMAKE_SOURCE_DIR}/../libraries/3rdparty/CMock/vendor/unity/auto/parse_output.rb -xml ${CMAKE_BINARY_DIR}/${test}_out.txt
                OUTPUT_FILE ${CMAKE_BINARY_DIR}/${test}_out.xml
                )
    endforeach()

    execute_process(
                COMMAND lcov --capture
                             -b ${CMAKE_BINARY_DIR}
                             --directory ${CMAKE_BINARY_DIR}
                             --output-file ${CMAKE_BINARY_DIR}/second_coverage.info
            )
    execute_process(
                COMMAND lcov -b ${CMAKE_BINARY_DIR}
                             --directory ${CMAKE_BINARY_DIR}
                             --add-tracefile ${CMAKE_BINARY_DIR}/base_coverage.info
                             --add-tracefile ${CMAKE_BINARY_DIR}/second_coverage.info
                             --output-file ${CMAKE_BINARY_DIR}/coverage.info
                             --no-external
            )
    execute_process(
                COMMAND genhtml -o ${CMAKE_BINARY_DIR}/coverage
                                    ${CMAKE_BINARY_DIR}/coverage.info  
            )

