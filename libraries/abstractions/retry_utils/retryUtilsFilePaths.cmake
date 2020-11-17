# This file is to add source files and include directories
# into variables so that it can be reused from different repositories
# in their Cmake based build system by including this file.
#
# Files specific to the repository such as test runner, platform tests
# are not added to the variables.

# Retry Utils library source files.
set( RETRY_UTILS_SOURCES
     "${CMAKE_CURRENT_LIST_DIR}/source/retry_utils.c" )

# Retry Utils library Public Include directories.
set( RETRY_UTILS_INCLUDE_PUBLIC_DIRS
     "${CMAKE_CURRENT_LIST_DIR}/source/include" )
