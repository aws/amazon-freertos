# ``````````````````````````````````````````````````````````````````````````````
#               Toolchain for building and running CBMC proofs
#               ==============================================
#
# CBMC proofs require include paths, compile definitions, and other variables
# that are common to all proofs. At the same time, individual proofs may wish to
# extend these options---for example, including headers only needed by that
# proof, or proving more than just memory safety.
#
# This file sets up variables that can be appended to in the CMakeLists.txt of
# each proof. Most of the recipes for building and running proofs are written in
# the cbmc_recipes.cmake file. cbmc_recipes.cmake gets included at the bottom of
# each proof, and uses the variables defined in this toolchains file. This file
# is only processed once when using the CBMC toolchain, while cbmc_recipes.cmake
# gets included separately by each proof.
# ______________________________________________________________________________

set(CBMC 1)

#  0 no output
#  1 errors
#  2 warnings
#  4 results
#  6 status information
#  8 statistical information
#  9 progress information
# 10 debug info
set(CBMC_VERBOSITY 6 CACHE STRING "Verbosity of CBMC and related tools")

# ______________________________________________________________________________
#     Variables that are not intended to be directly changed by proof-writers
# ``````````````````````````````````````````````````````````````````````````````

# The list of all proof names (used to check that proof names are unique)
set(cbmc_proof_names "" CACHE INTERNAL "")

# The cbmc root
set(cbmc_dir ${CMAKE_SOURCE_DIR}/tools/cbmc)

# The cbmc proof root
set(cbmc_proofs_dir ${cbmc_dir}/proofs)

# Directories that cbmc-viewer should not bother to scan. This saves a lot of
# time when generating reports.
set(cbmc_viewer_src_exclude "(./doc|./tests|./vendors)")

# ______________________________________________________________________________
#     Strings that proof-writers can change for a particular proof in each
#     proof's CMakeLists.txt
# ``````````````````````````````````````````````````````````````````````````````

# Argument to cbmc --unwind flag. See also cbmc_unwindset below.
set(cbmc_unwind 1)

# Setting this to TRUE for a particular proof builds that proof with the
# --nondet-static flag. See also cbmc_nondet_static_except below.
set(cbmc_nondet_static FALSE)

# Setting this to TRUE for a particular proof exports all of the static
# functions of that proof, enabling you to directly write harnesses for
# those functions.
set(cbmc_export_static FALSE)

# Argument to cbmc --object-bits flag
set(cbmc_object_bits 8)

# Maximum object size that can be modeled by cbmc.
# Must use +1 until https://github.com/diffblue/cbmc/issues/5096 is resolved.
set(cbmc_max_object_size "(SIZE_MAX>>(CBMC_OBJECT_BITS+1))")

# ______________________________________________________________________________
#     Lists that proof-writers can append to in each proof's CMakeLists.txt
# ``````````````````````````````````````````````````````````````````````````````

# Each CBMC proof has a list of labels; CTest can run all proofs that have a
# particular label. All CBMC proofs have the 'cbmc' label. Each FreeRTOS module
# should get its own label, and each individual proof should also get its own
# label.
set(cbmc_test_labels "cbmc")

# Argument to cbmc --unwindset flag
set(cbmc_unwindset)

# Argument to goto-instrument --nondet-static-except flag.
set(cbmc_nondet_static_except)

set(cbmc_compile_options)
set(cbmc_compile_definitions)
set(cbmc_compile_includes)
set(cbmc_flags)

# ______________________________________________________________________________
#     Use goto-cc (the CBMC compiler) and do other FreeRTOS-specific setup
# ``````````````````````````````````````````````````````````````````````````````

include("${CMAKE_CURRENT_LIST_DIR}/find_compiler.cmake")

set(CMAKE_SYSTEM_NAME Generic)

afr_find_compiler(AFR_COMPILER_CC goto-cc)
afr_find_compiler(AFR_COMPILER_CXX goto-cc)
set(AFR_COMPILER_ASM "${AFR_COMPILER_CC}" CACHE INTERNAL "")

# Specify the cross compiler.
set(CMAKE_C_COMPILER ${AFR_COMPILER_CC} CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER ${AFR_COMPILER_CXX} CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER ${AFR_COMPILER_ASM} CACHE FILEPATH "ASM compiler")

# Disable compiler checks.
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Look for includes and libraries only in the target system prefix.
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

# CBMC proofs are run as tests using CTest, so unconditionally enable this
enable_testing()
