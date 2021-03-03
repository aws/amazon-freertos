cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0025 NEW)

include("../../../vendors/cypress/MTB/psoc6/cmake/cy_glob_utils.cmake")
include("../../../vendors/cypress/MTB/psoc6/cmake/toolchains/arm-gcc.cmake")

project(test)

cy_find_files(found_files DIRECTORY ".")

cy_get_includes(include_dirs ITEMS "${found_files}" ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

foreach(itr ${include_dirs})
    message(${itr})
endforeach()
