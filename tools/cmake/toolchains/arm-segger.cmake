include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

# Find GCC for ARM.
afr_find_compiler(AFR_COMPILER_CC cc1)
afr_find_compiler(AFR_COMPILER_CXX cc1plus)
afr_find_compiler(AFR_COMPILER_ASM as)

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

# Default flags and include paths.
include_directories(${CMAKE_FIND_ROOT_PATH}/../../include)

# Override the command to compile c files
set(CMAKE_C_COMPILE_OBJECT
	"<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE> -o <OBJECT>.asm"
	"<CMAKE_ASM_COMPILER> --traditional-format -mcpu=cortex-m4 -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb <OBJECT>.asm -o <OBJECT>"
)

# Override the command to compile asm files
set(CMAKE_ASM_COMPILE_OBJECT
	"<CMAKE_C_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE> -o <OBJECT>.asm"
	"<CMAKE_ASM_COMPILER> --traditional-format -mcpu=cortex-m4 -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb <OBJECT>.asm -o <OBJECT>"
)

# Override the linker command.
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
