# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

get_filename_component(ABS_EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../common/application_code/espressif_code" ABSOLUTE)
set(EXTRA_COMPONENT_DIRS ${ABS_EXTRA_COMPONENT_DIRS})
get_filename_component(ABS_IDF_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../lib/third_party/mcu_vendor/espressif/esp-idf" ABSOLUTE)
set(ENV{IDF_PATH} ${ABS_IDF_PATH})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(aws_tests)
