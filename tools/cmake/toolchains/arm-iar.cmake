include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find IAR for ARM.
afr_find_compiler(ARM_IAR_CXX iccarm)
afr_find_compiler(ARM_IAR_ASM iasmarm)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${ARM_IAR_CXX} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${ARM_IAR_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${ARM_IAR_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(ARM_IAR_BIN "${ARM_IAR_CXX}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${ARM_IAR_BIN}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
