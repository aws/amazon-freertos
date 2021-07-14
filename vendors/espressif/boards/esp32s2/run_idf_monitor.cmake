# or another cmake-based build runner
#
# (Needed to expand environment variables, for backwards compatibility.)
#
# It is recommended to NOT USE this CMake script if you have the option of
# running idf_monitor.py directly. This script exists only for use inside CMake builds.
#
cmake_minimum_required(VERSION 3.5)

if(NOT IDF_PATH OR NOT PROJECT_ELF OR NOT ELF_DIR)
    message(FATAL_ERROR "IDF_PATH, PROJECT_ELF and ELF_DIR must "
        "be specified on the CMake command line. For direct monitor execution, it is "
        "strongly recommended to run idf_monitor.py directly.")
endif()

# Note: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them when running monitor not at CMake runtime (so they can change
# without needing a CMake re-run)
set(ESPPORT $ENV{ESPPORT})
if(NOT ESPPORT)
    message("Note: Using default serial port /dev/ttyUSB0. To modify, set ESPPORT environment variable.")
else()
    set(port_arg "--port ${ESPPORT}")
endif()

set(MONITORBAUD $ENV{MONITORBAUD})
if(NOT MONITORBAUD)
    message("Note: Using default baud rate 115200. To modify, set MONITORBAUD environment variable.")
else()
    set(baud_arg "--baud ${MONITORBAUD}")
endif()

include("${IDF_PATH}/tools/cmake/utilities.cmake")

set(cmd "${IDF_PATH}/tools/idf_monitor.py ${port_arg} ${baud_arg} ${PROJECT_ELF}")
spaces2list(cmd)

execute_process(COMMAND ${cmd}
    WORKING_DIRECTORY "${ELF_DIR}"
    RESULT_VARIABLE result
    )

if(${result})
    # No way to have CMake silently fail, unfortunately
    message(FATAL_ERROR "idf_monitor.py failed")
endif()
