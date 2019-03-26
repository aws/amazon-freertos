# -------------------------------------------------------------------------------------------------
# Amazon FreeRTOS modules
# -------------------------------------------------------------------------------------------------
# First we need to clean defined CACHE variables on previous CMake run.
foreach(__module IN LISTS AFR_MODULES)
    foreach(__prop IN ITEMS SOURCES INCLUDES DEPENDS)
        unset(AFR_MODULE_${__module}_${__prop}_PRIVATE CACHE)
        unset(AFR_MODULE_${__module}_${__prop}_PUBLIC CACHE)
        unset(AFR_MODULE_${__module}_${__prop}_INTERFACE CACHE)
        unset(AFR_MODULE_${__module}_${__prop}_ALL CACHE)
    endforeach()
endforeach()

# Global variables
set(AFR_MODULES               "" CACHE INTERNAL "List of Amazon FreeRTOS modules.")
set(AFR_MODULES_PUBLIC        "" CACHE INTERNAL "List of public Amazon FreeRTOS modules.")
set(AFR_MODULES_OPTIONAL      "" CACHE INTERNAL "List of optional internal Amazon FreeRTOS modules.")
set(AFR_MODULES_ENABLED       "" CACHE INTERNAL "List of enabled Amazon FreeRTOS modules.")
set(AFR_MODULES_BUILD         "" CACHE INTERNAL "List of Amazon FreeRTOS modules to build.")
set(AFR_MODULES_DISABLED_USER "" CACHE INTERNAL "List of Amazon FreeRTOS modules disabled by user.")
set(AFR_MODULES_DISABLED_DEPS "" CACHE INTERNAL "List of Amazon FreeRTOS modules disabled due to dependencies.")
set(AFR_DEMOS_ENABLED         "" CACHE INTERNAL "List of supported demos for Amazon FreeRTOS.")
set(AFR_TESTS_ENABLED         "" CACHE INTERNAL "List of supported tests for Amazon FreeRTOS.")

# Define an Amazon FreeRTOS module, will add module name to global variable AFR_MODULES.
# Use INTERNAL to indicate the module is for internal use only and should not provide an option for user to disable it.
# Use INTERFACE to define the module as an INTERFACE target instead of a static library, implies INTERNAL.
# Use OPTIONAL to indicate the target should only be enabled if it's required by non-internal modules, implies INTERNAL.
# Use KERNEL to indicate the module is part of the kernel. In this case it will not implicitly depends on kernel.
function(afr_module module_name)
    cmake_parse_arguments(
        PARSE_ARGV 1
        "ARG"                                   # Prefix of parsed results.
        "INTERNAL;INTERFACE;OPTIONAL;KERNEL"    # Option arguments.
        ""                                      # One value arguments.
        ""                                      # Multi value arguments.
    )

    afr_cache_append(AFR_MODULES ${module_name})

    if(ARG_INTERFACE)
        set(AFR_MODULE_${module_name}_IS_INTERFACE TRUE CACHE INTERNAL "")
    else()
        set(AFR_MODULE_${module_name}_IS_INTERFACE FALSE CACHE INTERNAL "")
    endif()

    if(ARG_OPTIONAL)
        afr_cache_append(AFR_MODULES_OPTIONAL ${module_name})
    endif()

    if(NOT (ARG_INTERNAL OR ARG_INTERFACE OR ARG_OPTIONAL))
        afr_cache_append(AFR_MODULES_PUBLIC ${module_name})
        option(AFR_MODULE_${module_name} "Enable ${module_name} module." ON)
    endif()

    # All modules implicitly depends on kernel unless INTERFACE or KERNEL is provided.
    if(NOT (ARG_INTERFACE OR ARG_KERNEL))
        afr_module_dependencies(
            ${module_name}
            PUBLIC AFR::kernel
        )
    endif()

    # Test dependencies.
    if(AFR_IS_TESTING AND NOT AFR_MODULE_${module_name}_IS_INTERFACE)
        afr_module_include_dirs(${module_name} PRIVATE "${AFR_TESTS_DIR}/include")
    endif()
endfunction()

# Define a demo module.
function(afr_demo_module demo_name)
    afr_module(${demo_name} INTERFACE)

    # Demo implicitly depends on AFR::demo_base.
    if(NOT "${demo_name}" STREQUAL "demo_base")
        afr_module_dependencies(
            ${demo_name}
            INTERFACE
                AFR::demo_base
        )
    endif()
endfunction()

# Define a test module.
function(afr_test_module test_name)
    afr_module(${test_name} INTERFACE)

    # Test implicitly depends on AFR::test_base.
    if(NOT "${test_name}" STREQUAL "test_base")
        afr_module_dependencies(
            ${test_name}
            INTERFACE
                AFR::test_base
        )
    endif()
endfunction()

# Define a 3rdparty module.
function(afr_3rdparty_module module_name)
    add_library(3rdparty::${module_name} INTERFACE IMPORTED GLOBAL)
endfunction()

# Add properties to a module, will set these global variables accordingly:
# AFR_MODULE_${module_name}_${prop_type}_PRIVATE
# AFR_MODULE_${module_name}_${prop_type}_PUBLIC
# AFR_MODULE_${module_name}_${prop_type}_INTERFACE
# AFR_MODULE_${module_name}_${prop_type}_ALL
# ${prop_type} can be SOURCES, INCLUDES or DEPENDS.
function(__afr_module_prop module_name prop_type)
    cmake_parse_arguments(
        PARSE_ARGV 2
        "ARG"                       # Prefix of parsed results.
        ""                          # Option arguments.
        ""                          # One value arguments.
        "PRIVATE;PUBLIC;INTERFACE"  # Multi value arguments.
    )

    set(__properties SOURCES INCLUDES DEPENDS)
    if(NOT prop_type IN_LIST __properties)
        message(FATAL_ERROR "Specified module property ${prop_type} is invalid.")
    endif()

    set(__prop_all_var AFR_MODULE_${module_name}_${prop_type}_ALL)
    if(NOT DEFINED ${__prop_all_var})
        set(${__prop_all_var} "" CACHE INTERNAL "")
    endif()

    foreach(__scope IN ITEMS PRIVATE PUBLIC INTERFACE)
        if(DEFINED ARG_${__scope})
            set(__prop_var AFR_MODULE_${module_name}_${prop_type}_${__scope})
            if(NOT DEFINED ${__prop_var})
                set(${__prop_var} "" CACHE INTERNAL "")
            endif()
            afr_cache_append(${__prop_var} ${ARG_${__scope}})
            afr_cache_append(${__prop_all_var} ${ARG_${__scope}})
        endif()
    endforeach()
endfunction()

# Add source files to a module.
function(afr_module_sources module_name)
    __afr_module_prop(${module_name} SOURCES ${ARGN})
endfunction()

# Add include directories to a module.
function(afr_module_include_dirs module_name)
    __afr_module_prop(${module_name} INCLUDES ${ARGN})
endfunction()

# Specify dependencies of a module.
function(afr_module_dependencies module_name)
    __afr_module_prop(${module_name} DEPENDS ${ARGN})
endfunction()

# Traverse dependency graph and disable modules with missing dependencies.
function(afr_resolve_dependencies)
    function(__check_circular_dependency node)
        list(FIND __path ${node} __idx)
        if(NOT __idx EQUAL -1)
            list(SUBLIST __path ${__idx} -1 __circle)
            list(JOIN __circle "->" __circle)
            message(FATAL_ERROR "Circular dependency detected: ${__circle}->${node}")
        else()
            afr_cache_append(__path ${node})
        endif()
    endfunction()

    function(__search_afr_dependencies mcu_port target)
        __check_circular_dependency(${target})

        if(NOT ${target} IN_LIST __visited)
            afr_cache_append(__visited ${target})
            get_target_property(__dependencies ${target} INTERFACE_LINK_LIBRARIES)
            foreach(__depend IN LISTS __dependencies)
                string(FIND "${__depend}" "AFR::" __idx)
                if(__idx EQUAL 0 AND NOT ${__depend} IN_LIST AFR_MODULE_${mcu_port}_DEPENDS_ALL)
                    afr_module_dependencies(${mcu_port} INTERFACE ${__depend})
                endif()
                if(TARGET ${__depend})
                    __search_afr_dependencies(${mcu_port} ${__depend})
                endif()
            endforeach()
        endif()

        afr_cache_remove(__path ${target})
    endfunction()

    function(__resolve_dependencies module_name)
        __check_circular_dependency(${module_name})

        set(__skip ${__visited} ${AFR_MODULES_DISABLED_USER} ${AFR_MODULES_DISABLED_DEPS})
        if(NOT ${module_name} IN_LIST __skip)
            afr_cache_append(__visited ${module_name})
            foreach(__dep IN LISTS AFR_MODULE_${module_name}_DEPENDS_ALL)
                # If the dependency starts with 3rdparty::, check if it exists.
                string(FIND "${__dep}" "3rdparty::" __idx)
                if(__idx EQUAL 0)
                    if(NOT TARGET ${__dep})
                        afr_cache_append(AFR_MODULES_DISABLED_DEPS ${module_name})
                    endif()
                    continue()
                endif()

                # Otherwise it should be an AFR module and start with AFR::, verify it exists.
                string(REPLACE "AFR::" "" __dep "${__dep}")
                if(NOT "${__dep}" IN_LIST AFR_MODULES)
                    message(FATAL_ERROR "Module ${module_name} has an nonexistent AFR dependency ${__dep}.")
                endif()

                __resolve_dependencies(${__dep})

                # Add the module name to AFR_MODULES_DISABLED_DEPS if the dependency is not satisfied.
                if(NOT ${module_name} IN_LIST AFR_MODULES_DISABLED_USER     # Skip if it's disabled by user.
                AND NOT ${module_name} IN_LIST AFR_MODULES_DISABLED_DEPS # Skip if already disabled.
                AND ${__dep} IN_LIST AFR_MODULES_DISABLED_DEPS
                OR ${__dep} IN_LIST AFR_MODULES_DISABLED_USER)
                    afr_cache_append(AFR_MODULES_DISABLED_DEPS ${module_name})
                endif()
            endforeach()
        endif()

        afr_cache_remove(__path ${module_name})
    endfunction()

    # Initialize dependencies for portable layer targets first.
    set(__visited "" CACHE INTERNAL "")
    foreach(__mcu_port IN LISTS AFR_MODULES)
        string(FIND "${__mcu_port}" "::mcu_port" __idx REVERSE)
        if(NOT __idx EQUAL -1)
            set(__path "" CACHE INTERNAL "")
            __search_afr_dependencies(${__mcu_port} AFR::${__mcu_port})
        endif()
    endforeach()

    # Initialize enabled and disabled modules.
    foreach(__module IN LISTS AFR_MODULES)
        string(FIND "${__module}" "::mcu_port" __idx REVERSE)
        set(__port_target AFR::${__module}::mcu_port)
        # Disable the module if it has a portable layer and it's not defined by the vendor.
        if(__idx EQUAL -1       # Make sure the ${__module} itself is not a portable layer.
           AND ${__port_target} IN_LIST AFR_MODULE_${__module}_DEPENDS_ALL # And it has a portable layer.
           AND NOT TARGET ${__port_target})     # And the vendor didn't define the portable layer.
            afr_cache_append(AFR_MODULES_DISABLED_DEPS ${__module})
            if(DEFINED AFR_MODULE_${__module})
                unset(AFR_MODULE_${__module} CACHE)
            endif()
        # Check if the module is explicitly disabled by the user.
        elseif(DEFINED AFR_MODULE_${__module} AND NOT AFR_MODULE_${__module})
            afr_cache_append(AFR_MODULES_DISABLED_USER ${__module})
        # Otherwise, add it to the initial enabled modules list.
        else()
            afr_cache_append(AFR_MODULES_ENABLED ${__module})
        endif()
    endforeach()

    # Resolve dependencies with public modules first.
    set(__visited "" CACHE INTERNAL "")
    foreach(__module IN LISTS AFR_MODULES_PUBLIC)
        set(__path "" CACHE INTERNAL "")
        __resolve_dependencies(${__module})
    endforeach()

    # Disable optional modules that are not required by others.
    set(__visited_or_disabled ${__visited} ${AFR_MODULES_DISABLED_DEPS})
    foreach(__module IN LISTS AFR_MODULES_OPTIONAL)
        if(NOT ${__module} IN_LIST __visited_or_disabled)
            afr_cache_append(AFR_MODULES_DISABLED_DEPS ${__module})
        endif()
    endforeach()

    # Resolve dependencies with reset modules.
    foreach(__module IN LISTS AFR_MODULES)
        set(__path "" CACHE INTERNAL "")
        __resolve_dependencies(${__module})
    endforeach()

    afr_cache_remove(AFR_MODULES_ENABLED ${AFR_MODULES_DISABLED_DEPS})

    unset(__visited CACHE)
    unset(__path CACHE)

    # Define all targets and populate results.
    foreach(__module IN LISTS AFR_MODULES_ENABLED)
        afr_module_define_target(${__module})

        if(NOT AFR_MODULE_${__module}_IS_INTERFACE)
            afr_cache_append(AFR_MODULES_BUILD ${__module})
        endif()

        string(FIND ${__module} "_base" __is_base)
        string(FIND ${__module} "demo_" __idx)
        if(__is_base EQUAL -1 AND __idx EQUAL 0)
            afr_cache_append(AFR_DEMOS_ENABLED ${__module})
        endif()

        string(FIND ${__module} "test_" __idx)
        if(__is_base EQUAL -1 AND __idx EQUAL 0)
            afr_cache_append(AFR_TESTS_ENABLED ${__module})
        endif()
    endforeach()

    # No need to show disabled INTERFACE targets to user unless debug mode is enabled.
    if(NOT AFR_DEBUG_CMAKE)
        set(__filter ${AFR_MODULES_DISABLED_DEPS})
        foreach(__module IN LISTS __filter)
            if(AFR_MODULE_${__module}_IS_INTERFACE)
                afr_cache_remove(AFR_MODULES_DISABLED_DEPS ${__module})
            endif()
        endforeach()
    endif()
endfunction()

# Define the module target.
function(afr_module_define_target module_name)
    if(NOT ${module_name} IN_LIST AFR_MODULES)
        message(FATAL_ERROR "Specified module ${module_name} does not exist.")
    endif()

    if(TARGET AFR::${module_name})
        return()
    endif()

    set(__target afr_${module_name})
    if(AFR_MODULE_${module_name}_IS_INTERFACE)
        add_library(${__target} INTERFACE)
    else()
        add_library(${__target} STATIC)
    endif()
    add_library(AFR::${module_name} ALIAS ${__target})

    set(__scopes PRIVATE PUBLIC INTERFACE)
    if(DEFINED AFR_MODULE_${module_name}_SOURCES_ALL)
        foreach(__scope IN LISTS __scopes)
            set(__prop "${AFR_MODULE_${module_name}_SOURCES_${__scope}}")
            if(__prop)
                target_sources(${__target} ${__scope} ${__prop})
            endif()
        endforeach()
    endif()

    if(DEFINED AFR_MODULE_${module_name}_INCLUDES_ALL)
        foreach(__scope IN LISTS __scopes)
            set(__prop "${AFR_MODULE_${module_name}_INCLUDES_${__scope}}")
            if(__prop)
                target_include_directories(${__target} ${__scope} ${__prop})
            endif()
        endforeach()
    endif()

    if(DEFINED AFR_MODULE_${module_name}_DEPENDS_ALL)
        foreach(__scope IN LISTS __scopes)
            set(__prop "${AFR_MODULE_${module_name}_DEPENDS_${__scope}}")
            if(__prop)
                target_link_libraries(${__target} ${__scope} ${__prop})
            endif()
        endforeach()
    endif()
endfunction()

# -------------------------------------------------------------------------------------------------
# Interface for MCU vendors
# -------------------------------------------------------------------------------------------------
# Define an INTERFACE IMPORTED target for the portable layer of an Amazon FreeRTOS module, the
# target name is added to the global variable AFR_MODULES. Additional dependencies can be provided
# with DEPENDS parameter, or you can also use the target name AFR::${module_name}::mcu_port directly
# with any CMake built-in functions.
function(afr_mcu_port module_name)
    cmake_parse_arguments(
        PARSE_ARGV 1
        "ARG"       # Prefix of parsed results.
        ""          # Option arguments.
        ""          # One value arguments.
        "DEPENDS"   # Multi value arguments.
    )

    set(__port_name ${module_name}::mcu_port)
    set(AFR_MODULE_${__port_name}_IS_INTERFACE TRUE CACHE INTERNAL "")
    afr_cache_append(AFR_MODULES ${__port_name})

    add_library(AFR::${__port_name} INTERFACE IMPORTED GLOBAL)
    target_link_libraries(
        AFR::${__port_name}
        INTERFACE ${ARG_DEPENDS}
    )
endfunction()
