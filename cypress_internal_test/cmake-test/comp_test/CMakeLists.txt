cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0025 NEW)

include("../../../vendors/cypress/MTB/psoc6/cmake/cy_glob_utils.cmake")
include("../../../vendors/cypress/MTB/psoc6/cmake/toolchains/arm-gcc.cmake")

project(test)

set(ENV{CY_COMPONENTS} "b;d")

cy_find_files(found_files DIRECTORY ".")

foreach(itr ${found_files})
    message(${itr})
endforeach()
