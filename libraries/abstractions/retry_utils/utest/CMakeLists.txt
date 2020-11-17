# Include file path configuration for retry utils library.
include(../retryUtilsFilePaths.cmake)

project ("retry utils unit test")
cmake_minimum_required (VERSION 3.2.0)

# ====================  Define your project name (edit) ========================
set(project_name "retry_utils")

# =====================  Create your mock here  (edit)  ========================

# list the files to mock here
list(APPEND mock_list
                ""
        )

# list the directories your mocks need
list(APPEND mock_include_list
                ""
        )
#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
                ""
        )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
                ${RETRY_UTILS_SOURCES}
        )
# list the directories the module under test includes
list(APPEND real_include_directories
                ${RETRY_UTILS_INCLUDE_PUBLIC_DIRS}
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
                ${RETRY_UTILS_INCLUDE_PUBLIC_DIRS}
        )

# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    ""
        )

list(APPEND utest_link_list
            lib${real_name}.a
        )

list(APPEND utest_dep_list
            ${real_name}
        )

set(utest_name "${project_name}_utest")
set(utest_source "${project_name}_utest.c")
create_test(${utest_name}
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
        )
