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
set(cbmc_proof_names "${cbmc_proof_name};${cbmc_proof_names}" CACHE INTERNAL "")


# ______________________________________________________________________________
#     Build up variables out of lists that the proof might have set
# ``````````````````````````````````````````````````````````````````````````````

list(JOIN cbmc_unwindset "," _cbmc_unwindset)

list(APPEND cbmc_flags
    --object-bits ${cbmc_objectid_bits}
    --unwind ${cbmc_unwind}
    --unwindset ${_cbmc_unwindset}
)

list(JOIN cbmc_flags " " _cbmc_flags)

list(APPEND cbmc_compile_definitions
    CBMC_OBJECTID_BITS=${cbmc_objectid_bits}
    CBMC_MAX_OBJECT_SIZE=${cbmc_max_object_size}
)

# ______________________________________________________________________________
#     Common recipes for building a single proof
# ``````````````````````````````````````````````````````````````````````````````

# Separately build an object file from the proof harness, and an object file
# from all project source files that the harness will link against.  This is
# because we sometimes want to remove particular functions out of the project,
# and particular functions out of the harness. So we remove the functions from
# each object file and then link them together.

set_target_properties(
    ${cbmc_proof_name}_0010_harness.goto
    PROPERTIES
    INCLUDE_DIRECTORIES "${cbmc_include_directories}"
    COMPILE_DEFINITIONS "${cbmc_compile_definitions}"
    COMPILE_OPTIONS     "${cbmc_compile_options}"
)

set_target_properties(
    ${cbmc_proof_name}_0010_project.goto
    PROPERTIES
    INCLUDE_DIRECTORIES "${cbmc_include_directories}"
    COMPILE_DEFINITIONS "${cbmc_compile_definitions}"
    COMPILE_OPTIONS     "${cbmc_compile_options}"
)

# Function removal

list(PREPEND cbmc_harness_remove goto-instrument)
list(APPEND  cbmc_harness_remove ${cbmc_proof_name}_0010_harness.goto)
list(APPEND  cbmc_harness_remove ${cbmc_proof_name}_0020_harness_functions_removed.goto)

list(PREPEND cbmc_project_remove goto-instrument)
list(APPEND  cbmc_project_remove ${cbmc_proof_name}_0010_project.goto)
list(APPEND  cbmc_project_remove ${cbmc_proof_name}_0020_project_functions_removed.goto)

# TODO: break this up into a sequence of custom commands, for easier debugging
add_custom_command(
    DEPENDS ${cbmc_proof_name}_0010_harness.goto ${cbmc_proof_name}_0010_project.goto
    OUTPUT
        ${cbmc_proof_name}_0020_harness_functions_removed.goto
        ${cbmc_proof_name}_0020_project_functions_removed.goto
        ${cbmc_proof_name}_0030_library_added.goto
        ${cbmc_proof_name}_0040_unused_functions_dropped.goto
        ${cbmc_proof_name}_0050_global_inits_sliced.goto
        ${cbmc_proof_name}.goto
    COMMENT "Linking"
    COMMAND ${cbmc_harness_remove}
    COMMAND ${cbmc_project_remove}
    COMMAND
      goto-cc --function harness
          ${cbmc_proof_name}_0020_harness_functions_removed.goto
          ${cbmc_proof_name}_0020_project_functions_removed.goto
          -o ${cbmc_proof_name}_0030_library_added.goto
    COMMAND
      goto-instrument --add-library
      ${cbmc_proof_name}_0030_library_added.goto
      ${cbmc_proof_name}_0040_unused_functions_dropped.goto
    COMMAND
      goto-instrument --drop-unused-functions
      ${cbmc_proof_name}_0040_unused_functions_dropped.goto
      ${cbmc_proof_name}_0050_global_inits_sliced.goto
    COMMAND
      goto-instrument --slice-global-inits
      ${cbmc_proof_name}_0050_global_inits_sliced.goto
      ${cbmc_proof_name}.goto
)

add_custom_target(${cbmc_proof_name}-proof DEPENDS ${cbmc_proof_name}.goto)


# ______________________________________________________________________________
#     Common recipe for running a single proof using CTest.
# ``````````````````````````````````````````````````````````````````````````````

# Actually check the proof
# TODO this obviously won't work on windows.
add_test(
  NAME ${cbmc_proof_name}-cbmc
  COMMAND
    /bin/sh -c
    "cbmc --unwinding-assertions --trace ${_cbmc_flags} ${cbmc_proof_name}.goto > cbmc.txt 2>&1 "
)
set_tests_properties(
    ${cbmc_proof_name}-cbmc
    PROPERTIES
    LABELS "${cbmc_test_labels}"
    SKIP_RETURN_CODE 10
)

# Show the properties that were checked
# TODO this obviously won't work on windows.
add_test(
  NAME ${cbmc_proof_name}-property
  COMMAND
    /bin/sh -c
    "cbmc --unwinding-assertions --show-properties ${_cbmc_flags} ${cbmc_proof_name}.goto --xml-ui > property.xml 2>&1"
)
set_tests_properties(
    ${cbmc_proof_name}-property
    PROPERTIES
    LABELS "${cbmc_test_labels}"
)

# Compute which lines of code were covered by the proof
# TODO this obviously won't work on windows.
add_test(
  NAME ${cbmc_proof_name}-coverage
  COMMAND
    /bin/sh -c
    "cbmc --cover location ${_cbmc_flags} ${cbmc_proof_name}.goto --xml-ui > coverage.xml 2>&1"
)
set_tests_properties(
    ${cbmc_proof_name}-coverage
    PROPERTIES
    LABELS "${cbmc_test_labels}"
)

# Accumulate the results of the proof runs above and generate a report
set(_report_command
    cbmc-viewer
    --goto ${cbmc_proof_name}.goto
    --srcdir ${CMAKE_SOURCE_DIR}
    --blddir ${CMAKE_SOURCE_DIR}
    --htmldir ${CMAKE_BINARY_DIR}/cbmc-reports/${cbmc_proof_name}
    --srcexclude "\"(./doc|./tests|./vendors)\""
    --result cbmc.txt
    --property property.xml
    --block coverage.xml
)
list(JOIN _report_command " " report_command)

# TODO this obviously won't work on windows. But why do we even need a shell here?
add_test(
  NAME ${cbmc_proof_name}-report
  COMMAND
    /bin/sh -c "${report_command}"
)
set_tests_properties(
    ${cbmc_proof_name}-report
    PROPERTIES
    LABELS "${cbmc_test_labels}"
    DEPENDS "${cbmc_proof_name}-cbmc;${cbmc_proof_name}-property;${cbmc_proof_name}-coverage"
)
