include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find Keil for ARM.
afr_find_compiler(ARM_KEIL_C armcc.exe)
afr_find_compiler(ARM_KEIL_CXX armcc.exe)
afr_find_compiler(ARM_KEIL_ASM armasm.exe)


# Specify the cross compiler.
set(CMAKE_C_COMPILER ${ARM_KEIL_C} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${ARM_KEIL_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${ARM_KEIL_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(ARM_KEIL_BIN "${ARM_KEIL_C}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${ARM_KEIL_BIN}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
