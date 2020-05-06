if (AFR_ENABLE_UNIT_TESTS)

    set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Og")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS "-Wall")

    set(CMAKE_SHARED_LINKER_FLAGS "-fPIC")

    set(3rdparty_dir "${AFR_ROOT_DIR}/libraries/3rdparty")
    set(freertos_plus_dir "${AFR_ROOT_DIR}/libraries/freertos_plus")
    set(abstraction_dir "${AFR_ROOT_DIR}/libraries/abstractions")
    set(kernel_dir "${AFR_ROOT_DIR}/freertos_kernel")
    set(common_dir "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common")
    set(c_sdk_dir "${AFR_ROOT_DIR}/libraries/c_sdk")
    set(standard_dir "${AFR_ROOT_DIR}/libraries/freertos_plus/standard")

    enable_testing()
    add_subdirectory("tests/unit_test/linux")
endif ()
