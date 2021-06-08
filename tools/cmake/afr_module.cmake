# -------------------------------------------------------------------------------------------------
# FreeRTOS modules
# -------------------------------------------------------------------------------------------------
# First we need to clean defined CACHE variables on previous CMake run.
foreach(module IN LISTS AFR_MODULES)
    foreach(prop IN ITEMS SOURCES INCLUDES DEPENDS)
        unset(AFR_MODULE_${module}_CMAKE_FILES CACHE)
        unset(AFR_MODULE_${module}_${prop}_PRIVATE CACHE)
        unset(AFR_MODULE_${module}_${prop}_PUBLIC CACHE)
        unset(AFR_MODULE_${module}_${prop}_INTERFACE CACHE)
        unset(AFR_MODULE_${module}_${prop}_ALL CACHE)
    endforeach()
endforeach()
unset(AFR_FILES_TO_CONSOLE_METADATA CACHE)

# Global variables.
set(AFR_MODULES                 "" CACHE INTERNAL "List of FreeRTOS modules.")
set(AFR_MODULES_PORT            "" CACHE INTERNAL "List of porting layer targets defined from vendors.")
set(AFR_MODULES_PUBLIC          "" CACHE INTERNAL "List of public FreeRTOS modules.")
set(AFR_MODULES_BUILD           "" CACHE INTERNAL "List of FreeRTOS modules to build.")
set(AFR_MODULES_ENABLED         "" CACHE INTERNAL "List of enabled FreeRTOS modules.")
set(AFR_MODULES_ENABLED_USER    "" CACHE INTERNAL "List of FreeRTOS modules enabled by user.")
set(AFR_MODULES_ENABLED_DEPS    "" CACHE INTERNAL "List of FreeRTOS modules enabled due to dependencies.")
set(AFR_DEMOS_ENABLED           "" CACHE INTERNAL "List of supported demos for FreeRTOS.")
set(AFR_TESTS_ENABLED           "" CACHE INTERNAL "List of supported tests for FreeRTOS.")
set(3RDPARTY_MODULES_ENABLED    "" CACHE INTERNAL "List of 3rdparty libraries enabled due to dependencies.")

# Global setting for whether enable all modules by default or not.
if(NOT AFR_ENABLE_ALL_MODULES)
    set(AFR_ENABLE_ALL_MODULES 1 CACHE INTERNAL "")
endif()

# Define an FreeRTOS module, the module name will be added to the global variable AFR_MODULES.
# Use NAME to provide a name for the module, if not use, the name will be inferred from the folder name.
# Use INTERNAL to indicate the module is for internal use and will only be enabled if it's required by a public module.
# Use INTERFACE to define the module as an INTERFACE target instead of a static library, implies INTERNAL.
# Use KERNEL to indicate the module is part of the kernel. In this case it will not implicitly depends on kernel.
function(afr_module)
    cmake_parse_arguments(
        PARSE_ARGV 0
        "ARG"                          # Prefix of parsed results.
        "INTERNAL;INTERFACE;KERNEL"    # Option arguments.
        "NAME"                         # One value arguments.
        ""                             # Multi value arguments.
    )

    if(DEFINED ARG_NAME)
        set(module_name ${ARG_NAME})
    else()
        get_filename_component(module_name "${CMAKE_CURRENT_LIST_DIR}" NAME)
    endif()

    afr_cache_append(AFR_MODULES ${module_name})

    if(ARG_INTERFACE)
        set(AFR_MODULE_${module_name}_IS_INTERFACE TRUE CACHE INTERNAL "")
    else()
        set(AFR_MODULE_${module_name}_IS_INTERFACE FALSE CACHE INTERNAL "")
    endif()

    if(NOT (ARG_INTERNAL OR ARG_INTERFACE))
        afr_cache_append(AFR_MODULES_PUBLIC ${module_name})
    endif()

    if(NOT (ARG_INTERFACE OR ARG_KERNEL))
        afr_module_dependencies(
            ${module_name}
            PUBLIC AFR::kernel
        )
    endif()

    # Set current module name.
    set(AFR_CURRENT_MODULE ${module_name} PARENT_SCOPE)
endfunction()

# Define a demo module.
function(afr_demo_module)
    if(${ARGC} EQUAL 1)
        set(module_name ${ARGV0})
    else()
        get_filename_component(module_name "${CMAKE_CURRENT_LIST_DIR}" NAME)
    endif()
    set(module_name demo_${module_name})
    afr_module(NAME ${module_name} INTERFACE)

    # Demo implicitly depends on AFR::demo_base.
    if(NOT "${module_name}" STREQUAL "demo_base")
        afr_module_dependencies(
            ${module_name}
            INTERFACE
                AFR::demo_base
        )
    else()
        # Make demo_base depends on this dummy target when configuring tests so all demos
        # will be disabled by this non-existing dependency.
        if(AFR_IS_TESTING)
            afr_module_dependencies(demo_base INTERFACE 3rdparty::dummy)
        endif()
    endif()

    # Set current module name.
    set(AFR_CURRENT_MODULE ${module_name} PARENT_SCOPE)
endfunction()

# Define a test module.
function(afr_test_module)
    if(${ARGC} EQUAL 1)
        set(module_name ${ARGV0})
    else()
        get_filename_component(module_name "${CMAKE_CURRENT_LIST_DIR}" NAME)
    endif()
    set(module_name test_${module_name})
    afr_module(NAME ${module_name} INTERFACE)

    # Test implicitly depends on AFR::test_base.
    if(NOT "${module_name}" STREQUAL "test_base")
        afr_module_dependencies(
            ${module_name}
            INTERFACE
                AFR::test_base
        )
    else()
        # Make test_base depends on this dummy target when configuring demos so all tests
        # will be disabled by this non-existing dependency.
        if(NOT AFR_IS_TESTING)
            afr_module_dependencies(test_base INTERFACE 3rdparty::dummy)
        endif()
    endif()

    # Set current module name.
    set(AFR_CURRENT_MODULE ${module_name} PARENT_SCOPE)
endfunction()

# Define a 3rdparty module.
function(afr_3rdparty_module arg_name)
    add_library(3rdparty::${arg_name} INTERFACE IMPORTED GLOBAL)
endfunction()

# Add properties to a module, will set these global variables accordingly:
# AFR_MODULE_${arg_module}_${arg_prop_type}_PRIVATE
# AFR_MODULE_${arg_module}_${arg_prop_type}_PUBLIC
# AFR_MODULE_${arg_module}_${arg_prop_type}_INTERFACE
# AFR_MODULE_${arg_module}_${arg_prop_type}_ALL
# ${arg_prop_type} can be SOURCES, INCLUDES or DEPENDS.
function(__afr_module_prop arg_module arg_prop_type)
    cmake_parse_arguments(
        PARSE_ARGV 2
        "ARG"                       # Prefix of parsed results.
        ""                          # Option arguments.
        ""                          # One value arguments.
        "PRIVATE;PUBLIC;INTERFACE"  # Multi value arguments.
    )

    set(properties SOURCES INCLUDES DEPENDS)
    if(NOT "${arg_prop_type}" IN_LIST properties)
        message(FATAL_ERROR "Specified module property ${arg_prop_type} is invalid.")
    endif()

    set(prop_all_var AFR_MODULE_${arg_module}_${arg_prop_type}_ALL)
    if(NOT DEFINED ${prop_all_var})
        set(${prop_all_var} "" CACHE INTERNAL "")
    endif()

    foreach(scope IN ITEMS PRIVATE PUBLIC INTERFACE)
        if(DEFINED ARG_${scope})
            set(prop_var AFR_MODULE_${arg_module}_${arg_prop_type}_${scope})
            if(NOT DEFINED ${prop_var})
                set(${prop_var} "" CACHE INTERNAL "")
            endif()
            afr_cache_append(${prop_var} ${ARG_${scope}})
            afr_cache_append(${prop_all_var} ${ARG_${scope}})
        endif()
    endforeach()
endfunction()

# Add source files to a module.
function(afr_module_sources arg_module)
    __afr_module_prop(${arg_module} SOURCES ${ARGN})
endfunction()

# Add include directories to a module.
function(afr_module_include_dirs arg_module)
    __afr_module_prop(${arg_module} INCLUDES ${ARGN})
endfunction()

# Specify dependencies of a module.
function(afr_module_dependencies arg_module)
    __afr_module_prop(${arg_module} DEPENDS ${ARGN})
endfunction()

# Function to add module-specific CMake files to metadata.
# This function should be used to add cmake files when:
# 1. The module name does not match its parent folder name
#                 OR/AND
# 2. A non-CMakeLists.txt (like "core_json.cmake") file needs to be added to metadata.
# Thif function sets the AFR_MODULE_${module_name}_CMAKE_FILES cache
# variable.
function(afr_module_cmake_files module_name)
    set(prop_var AFR_MODULE_${module_name}_CMAKE_FILES)
    set(${prop_var} "" CACHE INTERNAL "")
    afr_cache_append(${prop_var} ${ARGN})
endfunction()

# -------------------------------------------------------------------------------------------------
# Dependency resolver.
# -------------------------------------------------------------------------------------------------
# Helper variables for traversing dependency graph:
# __dg_path: current search path.
# __dg_visited: already visited nodes (modules).
# __dg_disabled: nodes (modules) that are disabled due to missing dependencies.
set(__dg_path     "" CACHE INTERNAL "")
set(__dg_visited  "" CACHE INTERNAL "")
set(__dg_disabled "" CACHE INTERNAL "")

# A helper macro to handle circular dependency.
macro(__check_circular_dependency node)
    list(FIND __dg_path ${node} idx)
    if(NOT idx EQUAL -1)
        if(AFR_DEBUG_CMAKE)
            list(SUBLIST __dg_path ${idx} -1 circle)
            list(JOIN circle "->" circle)
            message(WARNING "Circular dependency detected: ${circle}->${node}")
        endif()
        return()
    else()
        afr_cache_append(__dg_path ${node})
    endif()
endmacro()

# Recursively search dependencies for a target, return all AFR and 3rdparty dependencies found. This
# is to process dependency information added by vendors' CMake files and generate a complete
# dependency graph.
macro(__search_afr_dependencies arg_target arg_dependencies)
    set(__dg_path "" CACHE INTERNAL "")
    set(__dependencies "" CACHE INTERNAL "")
    __search_afr_dependencies_impl(${arg_target})
    set(${arg_dependencies} ${__dependencies})
    unset(__dependencies CACHE)
    set(__dg_visited "" CACHE INTERNAL "")
endmacro()

function(__search_afr_dependencies_impl arg_target)
    __check_circular_dependency(${arg_target})

    if(NOT ${arg_target} IN_LIST __dg_visited)
        afr_cache_append(__dg_visited ${arg_target})
        afr_get_target_dependencies(${arg_target} dependencies)
        foreach(dep IN LISTS dependencies)
            string(FIND "${dep}" "AFR::" idx_a)
            string(FIND "${dep}" "3rdparty::" idx_b)
            if((idx_a EQUAL 0 OR idx_b EQUAL 0) AND NOT ${dep} IN_LIST __dependencies)
                afr_cache_append(__dependencies ${dep})
            endif()
            if(TARGET ${dep})
                __search_afr_dependencies_impl(${dep})
            endif()
        endforeach()
    endif()

    afr_cache_remove(__dg_path ${arg_target})
endfunction()

# Recursively search dependencies for a given module, store modules with missing dependencies in
# __dg_disabled.
macro(__resolve_dependencies arg_module)
    if("${ARGV1}" STREQUAL "QUIET")
        set(__verbose 0 CACHE INTERNAL "")
    else()
        set(__verbose 1 CACHE INTERNAL "")
    endif()
    set(__dg_path "" CACHE INTERNAL "")
    __resolve_dependencies_impl(${arg_module})
endmacro()

macro(__print_dependency_details reason)
    if(__verbose)
        list(JOIN __dg_path "->" curr_path)
        afr_status("module disabled: " "${arg_module}")
        afr_status("reason:          " "${reason}")
        afr_status("dependency path: " "${curr_path}->${dep}")
        afr_status("")
    endif()
endmacro()

function(__resolve_dependencies_impl arg_module)
    __check_circular_dependency(${arg_module})

    set(skip ${__dg_visited} ${__dg_disabled})
    if(NOT ${arg_module} IN_LIST skip)
        afr_cache_append(__dg_visited ${arg_module})
        foreach(dep IN LISTS AFR_MODULE_${arg_module}_DEPENDS_ALL)
            # If the dependency starts with 3rdparty::, check if it exists.
            string(FIND "${dep}" "3rdparty::" idx)
            if(idx EQUAL 0)
                if(NOT TARGET ${dep})
                    __print_dependency_details("${dep} does not exist.")
                    afr_cache_append(__dg_disabled ${arg_module})
                    break()
                else()
                    afr_cache_append(__dg_visited ${dep})
                    continue()  # No need to search dependencies of 3rdparty libraries.
                endif()
            endif()

            # Otherwise it should be an AFR module and start with AFR::.
            # If it's a portable layer target, check if it's defined by vendor.
            string(REPLACE "AFR::" "" dep "${dep}")
            string(FIND "${dep}" "::mcu_port" idx REVERSE)
            if(NOT ${idx} EQUAL -1 AND NOT ${dep} IN_LIST AFR_MODULES_PORT)
                __print_dependency_details("${dep} is not defined by vendor.")
                afr_cache_append(__dg_disabled ${arg_module})
                break()
            endif()

            # Verify this AFR module exists.
            if(NOT "${dep}" IN_LIST AFR_MODULES)
                message(FATAL_ERROR "Module ${arg_module} has an nonexistent AFR dependency ${dep}.")
            endif()

            __resolve_dependencies_impl(${dep})

            # Add the module name to __dg_disabled if its dependency is missing.
            if(NOT ${arg_module} IN_LIST __dg_disabled  # Skip if already disabled.
                AND ${dep} IN_LIST __dg_disabled)
                afr_cache_append(__dg_disabled ${arg_module})
            endif()
        endforeach()
    endif()

    afr_cache_remove(__dg_path ${arg_module})
endfunction()

# Traverse dependency graph and enable selected public modules with their dependencies.
function(afr_resolve_dependencies)
    # Process dependencies information from portable layer targets.
    foreach(mcu_port IN LISTS AFR_MODULES_PORT)
        __search_afr_dependencies(AFR::${mcu_port} dependencies)
        afr_module_dependencies(${mcu_port} INTERFACE ${dependencies})
    endforeach()

    # Process dependencies information from aws_demos/aws_tests.
    if(AFR_IS_TESTING)
        set(exe_target aws_tests)
        set(exe_base test_base)
    else()
        set(exe_target aws_demos)
        set(exe_base demo_base)
    endif()
    # If neither demos nor tests are enabled, then don't search the aws_demos/aws_tests targets.
    if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
        __search_afr_dependencies(${exe_target} dependencies)
	afr_module_dependencies(${exe_base} INTERFACE ${dependencies})
    endif()

    if (AFR_ENABLE_UNIT_TESTS)
	return()
    endif()

    # Make sure kernel can be enabled first.
    __resolve_dependencies(kernel)
    if("kernel" IN_LIST __dg_disabled)
        message(FATAL_ERROR "Unable to build kernel due to missing dependencies.")
    endif()
    afr_cache_append(AFR_MODULES_ENABLED ${__dg_visited})

    # Check if demo_base/test_base can be enabled.
    __resolve_dependencies(${exe_base})
    if("${exe_base}" IN_LIST __dg_disabled)
        message(FATAL_ERROR "Unable to build ${exe_target} due to missing dependencies.")
    else()
        afr_cache_append(AFR_MODULES_ENABLED ${__dg_visited})
    endif()

    # Calculate all public modules that can be enabled.
    foreach(module IN LISTS AFR_MODULES_PUBLIC)
        __resolve_dependencies(${module})
        # Initialize the option.
        if(NOT ${module} IN_LIST __dg_disabled)
            option(AFR_MODULE_${module} "Enable ${module_name} module." ${AFR_ENABLE_ALL_MODULES})
        else()
            # If this module is disabled but user enabled it, throw FATAL_ERROR.
            if(AFR_MODULE_${module})
                message(FATAL_ERROR "Module ${module} cannot be enabled due to missing dependencies.")
            endif()
        endif()
    endforeach()
    if(NOT __dg_disabled)
        message("                          All modules available                          ")
    endif()

    # Enable all required modules, i.e., user selected + their dependencies.
    set(__dg_visited "" CACHE INTERNAL "")
    foreach(module IN LISTS AFR_MODULES_PUBLIC)
        if(AFR_MODULE_${module})
            __resolve_dependencies(${module})
            afr_cache_append(AFR_MODULES_ENABLED_USER ${module})
            afr_cache_append(AFR_MODULES_ENABLED ${__dg_visited})
        endif()
    endforeach()

    # Cleanup result.
    afr_cache_remove_duplicates(AFR_MODULES_ENABLED)
    set(3rdparty_libs ${AFR_MODULES_ENABLED})
    list(FILTER 3rdparty_libs INCLUDE REGEX "3rdparty::")
    afr_cache_remove(AFR_MODULES_ENABLED ${3rdparty_libs})
    list(TRANSFORM 3rdparty_libs REPLACE "3rdparty::" "")
    afr_cache_append(3RDPARTY_MODULES_ENABLED ${3rdparty_libs})
    afr_cache_append(AFR_MODULES_ENABLED_DEPS ${AFR_MODULES_ENABLED})
    afr_cache_remove(AFR_MODULES_ENABLED_DEPS ${AFR_MODULES_ENABLED_USER} ${AFR_MODULES_PORT})

    # Disable all other modules that are not required.
    set(__dg_disabled "" CACHE INTERNAL "")
    foreach(module IN LISTS AFR_MODULES)
        string(REGEX MATCH "^(demo_|test_)" match "${module}")
        if("${match}" STREQUAL "" AND NOT ${module} IN_LIST AFR_MODULES_ENABLED)
            afr_cache_append(__dg_disabled ${module})
        endif()
    endforeach()

    # Enable available demos/tests.
    foreach(module IN LISTS AFR_MODULES)
        string(REGEX MATCH "^(demo_|test_)" match "${module}")
        if(NOT "${match}" STREQUAL "")
            __resolve_dependencies(${module} QUIET)
            if(NOT ${module} IN_LIST __dg_disabled)
                afr_cache_append(AFR_MODULES_ENABLED ${module})
            endif()
        endif()
    endforeach()

    # Define all targets and populate results.
    foreach(module IN LISTS AFR_MODULES_ENABLED)
        afr_module_define_target(${module})

        if(NOT AFR_MODULE_${module}_IS_INTERFACE)
            afr_cache_append(AFR_MODULES_BUILD ${module})
        endif()

        string(FIND ${module} "_base" is_base)
        string(FIND ${module} "demo_" idx)
        if(is_base EQUAL -1 AND idx EQUAL 0)
            afr_cache_append(AFR_DEMOS_ENABLED ${module})
        endif()

        string(FIND ${module} "test_" idx)
        if(is_base EQUAL -1 AND idx EQUAL 0)
            afr_cache_append(AFR_TESTS_ENABLED ${module})
        endif()
    endforeach()
endfunction()

# Define the module target.
function(afr_module_define_target arg_module)
    if(NOT ${arg_module} IN_LIST AFR_MODULES)
        message(FATAL_ERROR "Specified module ${arg_module} does not exist.")
    endif()

    if(TARGET AFR::${arg_module})
        return()
    endif()

    set(target afr_${arg_module})
    if(AFR_MODULE_${arg_module}_IS_INTERFACE)
        add_library(${target} INTERFACE)
    else()
        add_library(${target} STATIC)
    endif()
    add_library(AFR::${arg_module} ALIAS ${target})

    set(scopes PRIVATE PUBLIC INTERFACE)
    if(DEFINED AFR_MODULE_${arg_module}_SOURCES_ALL)
        foreach(scope IN LISTS scopes)
            set(prop "${AFR_MODULE_${arg_module}_SOURCES_${scope}}")
            if(prop)
                target_sources(${target} ${scope} ${prop})
            endif()
        endforeach()
    endif()

    if(DEFINED AFR_MODULE_${arg_module}_INCLUDES_ALL)
        foreach(scope IN LISTS scopes)
            set(prop "${AFR_MODULE_${arg_module}_INCLUDES_${scope}}")
            if(prop)
                target_include_directories(${target} ${scope} ${prop})
            endif()
        endforeach()
    endif()

    if(DEFINED AFR_MODULE_${arg_module}_DEPENDS_ALL)
        foreach(scope IN LISTS scopes)
            set(prop "${AFR_MODULE_${arg_module}_DEPENDS_${scope}}")
            if(prop)
                target_link_libraries(${target} ${scope} ${prop})
            endif()
        endforeach()
    endif()
endfunction()

# -------------------------------------------------------------------------------------------------
# Interface for MCU vendors
# -------------------------------------------------------------------------------------------------
# Define an INTERFACE IMPORTED target for the portable layer of an FreeRTOS module, the
# target name is added to the global variables AFR_MODULES and AFR_MODULES_PORT. Additional
# dependencies can be provided with DEPENDS parameter, or you can also use the target name
# AFR::${arg_module}::mcu_port directly with any CMake built-in functions.
function(afr_mcu_port arg_module)
    cmake_parse_arguments(
        PARSE_ARGV 1
        "ARG"       # Prefix of parsed results.
        ""          # Option arguments.
        ""          # One value arguments.
        "DEPENDS"   # Multi value arguments.
    )

    set(port_name ${arg_module}::mcu_port)
    set(AFR_MODULE_${port_name}_IS_INTERFACE TRUE CACHE INTERNAL "")
    afr_cache_append(AFR_MODULES ${port_name})
    afr_cache_append(AFR_MODULES_PORT ${port_name})

    add_library(AFR::${port_name} INTERFACE IMPORTED GLOBAL)
    target_link_libraries(
        AFR::${port_name}
        INTERFACE ${ARG_DEPENDS}
    )
endfunction()
