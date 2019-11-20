include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)
if(NOT CMAKE_SYSTEM_PROCESSOR)
  set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
endif()

# Find Keil for ARM.
afr_find_compiler(AFR_COMPILER_CC armclang)
afr_find_compiler(AFR_COMPILER_CXX armclang)
afr_find_compiler(AFR_COMPILER_ASM armasm)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${AFR_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${AFR_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${AFR_COMPILER_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(AFR_COMPILER_DIR "${AFR_COMPILER_CC}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${AFR_COMPILER_DIR}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
