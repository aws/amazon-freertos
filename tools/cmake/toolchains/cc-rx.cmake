# Note, Cmake doesn't support cc-rx compiler.
# Even it finds the executable, it doesn't know its identificatioin.
# TODO: create a patch to Cmake

include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find Renesas CC-RX.
afr_find_compiler(CCRX_CXX ccrx)
afr_find_compiler(CCRX_ASM asrx)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${CCRX_CXX} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${CCRX_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${CCRX_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(CCRX_BIN "${CCRX_CXX}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${CCRX_BIN}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
