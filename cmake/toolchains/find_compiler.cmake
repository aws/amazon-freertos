# Toolchain file is processed multiple times, however, it cannot access CMake cache on some runs.
# We store the search path in an environment variable so that we can always access it.
if(NOT "${AFR_TOOLCHAIN_PATH}" STREQUAL "")
    set(ENV{AFR_TOOLCHAIN_PATH} "${AFR_TOOLCHAIN_PATH}")
endif()

# Find the compiler executable and store its path in a cache entry ${compiler_path}.
# If not found, issue a fatal message and stop processing. AFR_TOOLCHAIN_PATH can be provided from
# commandline as additional search path.
function(afr_find_compiler compiler_path compiler_exe)
    set(__search_path $ENV{AFR_TOOLCHAIN_PATH} "$ENV{AFR_TOOLCHAIN_PATH}/bin")
    find_program(${compiler_path} ${compiler_exe} PATHS ${__search_path})
    if("${${compiler_path}}" STREQUAL "${compiler_path}-NOTFOUND")
        set(AFR_TOOLCHAIN_PATH "" CACHE PATH "Path to search for compiler.")
        message(FATAL_ERROR "Compiler not found, you can specify search path with\
            \"AFR_TOOLCHAIN_PATH\".")
    endif()
endfunction()
