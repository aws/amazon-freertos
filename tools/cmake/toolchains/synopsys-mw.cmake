include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find Metaware for SYNOPSYS-ARC
afr_find_compiler(AFR_COMPILER_CC ccac)
afr_find_compiler(AFR_COMPILER_CXX ccac)
afr_find_compiler(AFR_COMPILER_ASM ccac)
afr_find_compiler(AFR_COMPILER_LD ccac)
afr_find_compiler(AFR_COMPILER_AR arac)
afr_find_compiler(AFR_COMPILER_DMP elfdumpac)
afr_find_compiler(AFR_COMPILER_NM nmac)
afr_find_compiler(AFR_COMPILER_OBJCOPY elf2bin)

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${AFR_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${AFR_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${AFR_COMPILER_ASM} CACHE FILEPATH "ASM compiler")
set(CMAKE_LINKER ${AFR_COMPILER_LD} CACHE FILEPATH "Linker")
set(CMAKE_AR ${AFR_COMPILER_AR} CACHE FILEPATH "Archiver")
set(CMAKE_OBJDUMP ${AFR_COMPILER_DMP} CACHE FILEPATH "object dump")
set(CMAKE_NM ${AFR_COMPILER_NM} CACHE FILEPATH "nm archiver")
set(CMAKE_OBJCOPY ${AFR_COMPILER_OBJCOPY} CACHE FILEPATH "opject copy")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Add target system root to cmake find path.
get_filename_component(AFR_COMPILER_DIR "${AFR_COMPILER_CC}" DIRECTORY)
get_filename_component(CMAKE_FIND_ROOT_PATH "${AFR_COMPILER_DIR}" DIRECTORY)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
