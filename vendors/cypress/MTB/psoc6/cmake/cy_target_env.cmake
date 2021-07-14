# Set up enviroment variable for target device and toolchains so that the correct compile and linker arguments can be generated.
function(cy_cfg_env)
    cmake_parse_arguments(
    PARSE_ARGV 0
    "ARG"
    ""
    "TARGET;DEVICE;TOOLCHAIN"
    ""
    )

    if(NOT DEFINED ARG_TARGET)
    message(FATAL_ERROR "Missing TARGET argument.")
    endif()

    if(NOT DEFINED ARG_DEVICE)
    message(FATAL_ERROR "Missing DEVICE argument.")
    endif()

    if(NOT DEFINED ARG_TOOLCHAIN)
    message(FATAL_ERROR "Missing TOOLCHAIN argument.")
    endif()

    #####################################
    # set CY_TARGET
    set(ENV{CY_TARGET} "${ARG_TARGET}")

    #####################################
    # set CY_DEVICE
    set(ENV{CY_DEVICE} "${ARG_DEVICE}")

    #####################################
    # set CY_CORE
    if ("${CORE}" STREQUAL "")
    set(ENV{CY_CORE} "CM4")
    else()
    set(ENV{CY_CORE} "${CORE}")
    endif()

    #####################################
    # map AFR toolchain names to cypress toolchain names
    # TODO make this support multiple toolchain and flash memory sizes
    if ("${ARG_TOOLCHAIN}" STREQUAL "arm-gcc")
    set(ARG_TOOLCHAIN GCC_ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-armclang")
    set(ARG_TOOLCHAIN ARM)
    elseif("${ARG_TOOLCHAIN}" STREQUAL "arm-iar")
    set(ARG_TOOLCHAIN IAR)
    endif()
    set(ENV{CY_TOOLCHAIN} "${ARG_TOOLCHAIN}")

    cy_cfg_toolchain()
endfunction()
