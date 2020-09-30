# Adds a post-build step to generate binaries from the project output file.

# Start with trying to locate TI Object2Bin utilities, if not successful display 
# warning as a binary will not be created, just the TI Object file. 
find_program(mkhex4bin mkhex4bin PATHS ${TI_CCS_PATH} PATH_SUFFIXES utils/tiobj2bin)

# If mkhex4bin was not found in the user specified path, check the system paths
if("${mkhex4bin}" STREQUAL "mkhex4bin-NOTFOUND")
    find_program(mkhex4bin mkhex4bin PATH_SUFFIXES utils/tiobj2bin)
endif()

# Only proceed if we found the tool path
if("${mkhex4bin}" STREQUAL "mkhex4bin-NOTFOUND")
    message(WARNING "TI \"Object2Bin\" utilities (mkhex4bin) was not found at \
            ${TI_CCS_PATH}/utils/. You can specify search path with \"TI_CCS_PATH\".")
else()

    # Locate compiler utilities paths
    find_program(armofd armofd PATHS "${AFR_COMPILER_DIR}")
    find_program(armhex armhex PATHS "${AFR_COMPILER_DIR}")

    # As find_program() does not consider .bat files as an "executable" per 
    # default, we need to specify the file ending. This means we also need to 
    # adapt based on the OS being used.
    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(tiobj2bin_script tiobj2bin)
    elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(tiobj2bin_script tiobj2bin)
    else()
        # If not Linux or macOS, assume Windows. The actual "true" variable could 
        # for example be WIN32, MINGWm MSYS or CYGWIN depending on the Windows 
        # environment used.
        set(tiobj2bin_script tiobj2bin.bat)
    endif()

    find_program(tiobj2bin ${tiobj2bin_script} PATHS ${TI_CCS_PATH} PATH_SUFFIXES utils/tiobj2bin)
    
    # If tiobj2bin was not found in the user specified path, check the system paths
    if("${tiobj2bin}" STREQUAL "tiobj2bin-NOTFOUND")
        find_program(tiobj2bin ${tiobj2bin_script} PATH_SUFFIXES utils/tiobj2bin)
    endif()

    if("${tiobj2bin}" STREQUAL "tiobj2bin-NOTFOUND")
        message(WARNING "TI \"Object2Bin\" utilities (tiobj2bin) was not found at \
            ${TI_CCS_PATH}/utils/. You can specify search path with \"TI_CCS_PATH\".")
    else()
        message(STATUS "TI \"Object2Bin\" utilities was found at: ${mkhex4bin} and ${tiobj2bin}.")

        set(output_file "$<TARGET_FILE_DIR:${exe_target}>/${exe_target}.hex")
        add_custom_command(
            TARGET ${exe_target} POST_BUILD
            COMMAND "${tiobj2bin}" "${CMAKE_BINARY_DIR}/${exe_target}.out" "${CMAKE_BINARY_DIR}/${exe_target}.bin"
                    "${armofd}" "${armhex}" "${mkhex4bin}"
        )
    endif()
endif()