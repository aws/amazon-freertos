include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)



# Find GCC for ARM.
afr_find_compiler(NORDIC_ARM_GCC_C cc1)
afr_find_compiler(NORDIC_ARM_GCC_CXX cc1plus)
afr_find_compiler(NORDIC_ARM_GCC_ASM as)
afr_find_compiler(NORDIC_ARM_GCC_LD ld)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${NORDIC_ARM_GCC_C} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${NORDIC_ARM_GCC_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${NORDIC_ARM_GCC_ASM} CACHE FILEPATH "ASM compiler")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(NORDIC_ARM_BIN "${NORDIC_ARM_GCC_C}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${NORDIC_ARM_BIN}" DIRECTORY)

# Don't look for executable in target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

include_directories(${CMAKE_FIND_ROOT_PATH}/../../include)
set(CMAKE_C_COMPILE_OBJECT  "<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE> -o <OBJECT>")