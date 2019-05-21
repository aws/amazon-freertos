include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find Microchip XC32.
afr_find_compiler(XC32_C xc32-gcc)
afr_find_compiler(XC32_CXX xc32-g++)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${XC32_C} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${XC32_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${XC32_C} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(XC32_BIN "${XC32_C}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${XC32_BIN}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
