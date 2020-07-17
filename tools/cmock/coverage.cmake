    cmake_minimum_required(VERSION 3.13)
    set(BINARY_DIR ${CMAKE_BINARY_DIR})
# reset coverage counters
    execute_process(
                COMMAND lcov --directory ${CMAKE_BINARY_DIR}
                             --base-directory ${CMAKE_BINARY_DIR}
                             --zerocounters

                COMMAND mkdir -p  ${CMAKE_BINARY_DIR}/coverage
            )
# make the initial/baseline capture a zeroed out files
    execute_process( COMMAND lcov --directory ${CMAKE_BINARY_DIR}
                             --base-directory ${CMAKE_BINARY_DIR}
                             --initial
                             --capture
                             --rc lcov_branch_coverage=1
                             --rc genhtml_branch_coverage=1
                             --output-file=${CMAKE_BINARY_DIR}/base_coverage.info
            )
    file(GLOB files "${CMAKE_BINARY_DIR}/bin/*")

    set(REPORT_FILE ${CMAKE_BINARY_DIR}/utest_report.txt)
# execute all files in bin directory, gathering the output to show it in CI
    foreach(testname ${files})
        get_filename_component(test
                               ${testname}
                               NAME_WLE
                )
        message("Running ${testname}")
        execute_process(COMMAND ${testname} OUTPUT_FILE ${CMAKE_BINARY_DIR}/${test}_out.txt)

        file(READ ${CMAKE_BINARY_DIR}/${test}_out.txt CONTENTS)
        file(APPEND ${REPORT_FILE} "${CONTENTS}")
    endforeach()

# generage Junit style xml output
    execute_process(COMMAND ruby
        ${CMAKE_SOURCE_DIR}/../libraries/3rdparty/CMock/vendor/unity/auto/parse_output.rb
                        -xml ${REPORT_FILE}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )

# capture data after running the tests
    execute_process(
                COMMAND lcov --capture
                             --rc lcov_branch_coverage=1
                             --rc genhtml_branch_coverage=1
                             --base-directory ${CMAKE_BINARY_DIR}
                             --directory ${CMAKE_BINARY_DIR}
                             --output-file ${CMAKE_BINARY_DIR}/second_coverage.info
            )
# combile baseline results (zeros) with the one after running the tests
    execute_process(
                COMMAND lcov --base-directory ${CMAKE_BINARY_DIR}
                             --directory ${CMAKE_BINARY_DIR}
                             --add-tracefile ${CMAKE_BINARY_DIR}/base_coverage.info
                             --add-tracefile ${CMAKE_BINARY_DIR}/second_coverage.info
                             --output-file ${CMAKE_BINARY_DIR}/coverage.info
                             --no-external
                             --rc lcov_branch_coverage=1
            )
    execute_process(
                COMMAND genhtml --rc lcov_branch_coverage=1
                                --branch-coverage
                                --output-directory ${CMAKE_BINARY_DIR}/coverage
                                    ${CMAKE_BINARY_DIR}/coverage.info
            )

