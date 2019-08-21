include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find TI for ARM.
afr_find_compiler(AFR_COMPILER_CC armcl)
set(AFR_COMPILER_CXX "${AFR_COMPILER_CC}" CACHE INTERNAL "")
set(AFR_COMPILER_ASM "${AFR_COMPILER_CC}" CACHE INTERNAL "")

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

# Helper variables to abstracts some common compiler flags.
set(AFR_COMPILER_NO_WARNINGS "--no_warnings" CACHE INTERNAL "")

# Set up TI compiler default flags and lib settings.
set(CMAKE_C_FLAGS_INIT "--c99")
set(CMAKE_C_FLAGS_DEBUG_INIT "-g")
include_directories(${CMAKE_FIND_ROOT_PATH}/include)
link_directories(${CMAKE_FIND_ROOT_PATH}/lib)
link_libraries(-llibc.a)

# Overwrite CMake archiver command for TI's compiler.
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> a <TARGET> <LINK_FLAGS> <OBJECTS>")

# Default response file flag is "@".  TI's compiler doesn't require that.
set(CMAKE_C_RESPONSE_FILE_LINK_FLAG "")
