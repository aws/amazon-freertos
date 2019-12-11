# ``````````````````````````````````````````````````````````````````````````````
#               Common recipes for building and running CBMC proofs
#               ===================================================
#
# This recipes file should be included at the bottom of every CMakeLists.txt
# that is used for defining a CBMC proof. The CMakeLists.txt is responsible for
# setting the options for that particular proof, while this recipes file uses
# those options to actually generate the builds for that proof. This file uses
# the variable definitions in tools/cmake/toolchains/cmake.cbmc, which can be
# changed by the CMakeLists.txt of each proof.
#
# This file builds the object files for each proof (consisting of the proof
# harness plus the code that is being proven); does some processing on those
# object files (removing unneeded functions, adding in function stubs, etc); and
# then generates 'tests' over those object files that actually run CBMC and
# generate a HTML proof report.
# ______________________________________________________________________________


# ______________________________________________________________________________
#     Sanity checks: ensure that the CMakeLists.txt for a single proof is
#     correctly written
# ``````````````````````````````````````````````````````````````````````````````

if(NOT DEFINED cbmc_proof_name)
    message(FATAL_ERROR "\
The CMakeLists.txt for the CBMC proof in directory
${CMAKE_CURRENT_SOURCE_DIR}
should set the variable 'cbmc_proof_name' to the name of the proof.")
endif()

if(${cbmc_proof_name} IN_LIST cbmc_proof_names)
    message(FATAL_ERROR "\
The CMakeLists.txt for the CBMC proof in directory
${CMAKE_CURRENT_SOURCE_DIR}
sets the proof name to '${cbmc_proof_name}', but that name is already used by
another proof. Ensure that you set the variable 'cbmc_proof_name' to a unique
value for each proof.")
endif()

list(LENGTH cbmc_nondet_static_exclude cnse_length)
if(${cnse_length} AND ${cbmc_nondet_static})
    message(FATAL_ERROR "\
The CMakeLists.txt for the CBMC proof in directory
${CMAKE_CURRENT_SOURCE_DIR}
sets both `cbmc_nondet_static` to true, and `cbmc_nondet_static_exclude` to a
nonempty list. You should only do one or the other. Value of
cbmc_nondet_static_exclude: [${cbmc_nondet_static_exclude}].")
endif()

set(cbmc_proof_names "${cbmc_proof_name};${cbmc_proof_names}" CACHE INTERNAL "")
list(APPEND cbmc_test_labels ${cbmc_proof_name})


# ______________________________________________________________________________
#     Common recipes for building a single proof
# ``````````````````````````````````````````````````````````````````````````````

# Separately build an object file from the proof harness, and an object file
# from all project source files that the harness will link against.  This is
# because we sometimes want to remove particular functions out of the project,
# and particular functions out of the harness. So we remove the functions from
# each object file and then link them together.
#
# After the project and harness have been linked together, we perform a series
# of transformations on the resulting object file using goto-instrument. The
# result of each transformation is the input to the next one. We leave behind
# all intermediate files, with sequence numbers, to aid debugging.

list(APPEND cbmc_compile_definitions
    CBMC_OBJECT_BITS=${cbmc_object_bits}
    CBMC_MAX_OBJECT_SIZE=${cbmc_max_object_size}
)

add_executable(
    ${cbmc_proof_name}_0010_harness.goto
    ${cbmc_harness_source}
)

set_target_properties(
    ${cbmc_proof_name}_0010_harness.goto
    PROPERTIES
    INCLUDE_DIRECTORIES "${cbmc_compile_includes}"
    COMPILE_DEFINITIONS "${cbmc_compile_definitions}"
    COMPILE_OPTIONS     "${cbmc_compile_options}"
)

# We may want to export functions marked as `static` so that we can write
# harnesses for them.
if(cbmc_export_static)
    list(APPEND cbmc_compile_options "--export-function-local-symbols")
endif()

add_executable(
    ${cbmc_proof_name}_0010_project.goto
    ${cbmc_project_source}
)

set_target_properties(
    ${cbmc_proof_name}_0010_project.goto
    PROPERTIES
    INCLUDE_DIRECTORIES "${cbmc_compile_includes}"
    COMPILE_DEFINITIONS "${cbmc_compile_definitions}"
    COMPILE_OPTIONS     "${cbmc_compile_options}"
)

# Function removal
#
# If funs_to_remove is a non-empty list of functions, return a goto-instrument
# command that strips those functions out of the binary.  If the list is empty,
# return a command to copy the binary without changing it.
function(cbmc_remove_functions funs_to_remove binary)
    list(LENGTH ${funs_to_remove} n_funs_to_remove)

    set(in_file ${cbmc_proof_name}_0010_${binary}.goto)
    set(out_file ${cbmc_proof_name}_0020_${binary}_functions_removed.goto)

    if(${n_funs_to_remove})
      list(JOIN ${funs_to_remove} ";--remove-function-body;" fun_remove_flags)
      add_custom_command(
          COMMENT "Removing ${n_funs_to_remove} functions from ${cbmc_proof_name}"
          DEPENDS ${in_file}
          OUTPUT  ${out_file}
          COMMAND
              goto-instrument
              --verbosity ${CBMC_VERBOSITY}
              --remove-function-body ${fun_remove_flags}
              ${in_file} ${out_file}
      )
    else()
      add_custom_command(
          COMMENT "Not removing any functions from ${cbmc_proof_name}"
          DEPENDS ${in_file}
          OUTPUT  ${out_file}
          COMMAND cmake -E copy ${in_file} ${out_file}
      )
    endif()
endfunction()

cbmc_remove_functions(cbmc_harness_remove harness)
cbmc_remove_functions(cbmc_project_remove project)

add_custom_command(
    COMMENT "Linking ${cbmc_proof_name}"
    DEPENDS
        ${cbmc_proof_name}_0020_harness_functions_removed.goto
        ${cbmc_proof_name}_0020_project_functions_removed.goto
    OUTPUT ${cbmc_proof_name}_0030_linked.goto
    COMMAND
        goto-cc --function harness
        ${cbmc_proof_name}_0020_harness_functions_removed.goto
        ${cbmc_proof_name}_0020_project_functions_removed.goto
        -o ${cbmc_proof_name}_0030_linked.goto
)

add_custom_command(
    COMMENT "Adding standard library to ${cbmc_proof_name}"
    DEPENDS ${cbmc_proof_name}_0030_linked.goto
    OUTPUT ${cbmc_proof_name}_0040_library_added.goto
    COMMAND
        goto-instrument --add-library --verbosity ${CBMC_VERBOSITY}
        ${cbmc_proof_name}_0030_linked.goto
        ${cbmc_proof_name}_0040_library_added.goto
)

add_custom_command(
    COMMENT "Dropping unused functions from ${cbmc_proof_name}"
    DEPENDS ${cbmc_proof_name}_0040_library_added.goto
    OUTPUT ${cbmc_proof_name}_0050_unused_functions_dropped.goto
    COMMAND
        goto-instrument --drop-unused-functions --verbosity ${CBMC_VERBOSITY}
        ${cbmc_proof_name}_0040_library_added.goto
        ${cbmc_proof_name}_0050_unused_functions_dropped.goto
)

list(LENGTH cbmc_nondet_static_exclude cnse_length)

# Nondeterministically initialize static variables
set(in_file ${cbmc_proof_name}_0050_unused_functions_dropped.goto)
set(out_file ${cbmc_proof_name}_0060_nondet_static.goto)
# At most one of these disjuncts will be true, given the sanity check at the top
# of the file
if(${cbmc_nondet_static} OR ${cnse_length})
    if(${cbmc_nondet_static})
        add_custom_command(
            COMMENT "Havocking all statics in ${cbmc_proof_name}"
            DEPENDS ${in_file}
            OUTPUT ${out_file}
            COMMAND
                goto-instrument --nondet-static
                --verbosity ${CBMC_VERBOSITY}
                ${in_file} ${out_file}
        )
    else()
        list(JOIN
            cbmc_nondet_static_exclude
            ";--nondet-static-exclude;"
            nondet_static_flags
        )
        add_custom_command(
            COMMENT "Havocking all except ${cnse_length} statics in ${cbmc_proof_name}"
            DEPENDS ${in_file}
            OUTPUT ${out_file}
            COMMAND
                goto-instrument
                --nondet-static-exclude ${nondet_static_flags}
                --verbosity ${CBMC_VERBOSITY}
                ${in_file} ${out_file}
        )
    endif()
else()
    add_custom_command(
        COMMENT "Not havocking statics in ${cbmc_proof_name}"
        DEPENDS ${in_file}
        OUTPUT ${out_file}
        COMMAND cmake -E copy ${in_file} ${out_file}
    )
endif()

add_custom_command(
    COMMENT "Removing unused globals initialization for ${cbmc_proof_name}"
    DEPENDS ${cbmc_proof_name}_0060_nondet_static.goto
    OUTPUT ${cbmc_proof_name}.goto
    COMMAND
        goto-instrument --slice-global-inits --verbosity ${CBMC_VERBOSITY}
        ${cbmc_proof_name}_0060_nondet_static.goto
        ${cbmc_proof_name}.goto
)

add_custom_target(${cbmc_proof_name}-goto DEPENDS ${cbmc_proof_name}.goto)


# ______________________________________________________________________________
#     Common recipe for running a single proof using CTest.
# ``````````````````````````````````````````````````````````````````````````````

list(APPEND cbmc_flags
    --object-bits ${cbmc_object_bits}
    --unwind ${cbmc_unwind}
)

list(LENGTH cbmc_unwindset cbmc_unwindset_length)
if(${cbmc_unwindset_length} GREATER 0)
    list(JOIN cbmc_unwindset "," _cbmc_unwindset)
    list(APPEND cbmc_flags --unwindset ${_cbmc_unwindset})
endif()

add_test(
    NAME ${cbmc_proof_name}-cbmc
    COMMAND
        ${CMAKE_COMMAND}
        "-Dproof_name=${cbmc_proof_name}"
        "-Dcbmc_flags=${cbmc_flags}"
        "-Dcbmc_verbosity=${cbmc_verbosity}"
        "-Dgoto_binary=${cbmc_proof_name}.goto"
        "-Dout_file=cbmc.txt"
        -P ${cbmc_dir}/cmake/model-check.cmake
)
set_tests_properties(
    ${cbmc_proof_name}-cbmc
    PROPERTIES
    LABELS "${cbmc_test_labels}"
)

add_test(
    NAME ${cbmc_proof_name}-property
    COMMAND
        ${CMAKE_COMMAND}
        "-Dproof_name=${cbmc_proof_name}"
        "-Dcbmc_flags=${cbmc_flags}"
        "-Dcbmc_verbosity=${cbmc_verbosity}"
        "-Dgoto_binary=${cbmc_proof_name}.goto"
        "-Dout_file=property.xml"
        -P ${cbmc_dir}/cmake/compute-property.cmake
)
set_tests_properties(
    ${cbmc_proof_name}-property
    PROPERTIES
    LABELS "${cbmc_test_labels}"
)

add_test(
    NAME ${cbmc_proof_name}-coverage
    COMMAND
        ${CMAKE_COMMAND}
        "-Dproof_name=${cbmc_proof_name}"
        "-Dcbmc_flags=${cbmc_flags}"
        "-Dcbmc_verbosity=${cbmc_verbosity}"
        "-Dgoto_binary=${cbmc_proof_name}.goto"
        "-Dout_file=coverage.xml"
        -P ${cbmc_dir}/cmake/compute-coverage.cmake
)
set_tests_properties(
    ${cbmc_proof_name}-coverage
    PROPERTIES
    LABELS "${cbmc_test_labels}"
)

add_test(
    NAME ${cbmc_proof_name}-report
    COMMAND
        cbmc-viewer
        --goto ${cbmc_proof_name}.goto
        --srcdir ${CMAKE_SOURCE_DIR}
        --blddir ${CMAKE_SOURCE_DIR}
        --htmldir ${CMAKE_BINARY_DIR}/cbmc-reports/${cbmc_proof_name}
        --srcexclude ${cbmc_viewer_src_exclude}
        --result cbmc.txt
        --property property.xml
        --block coverage.xml
)
set_tests_properties(
    ${cbmc_proof_name}-report
    PROPERTIES
    LABELS "${cbmc_test_labels}"
    DEPENDS
        ${cbmc_proof_name}-cbmc
        ${cbmc_proof_name}-property
        ${cbmc_proof_name}-coverage
)
