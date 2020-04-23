if (AFR_ENABLE_UNIT_TESTS)

    set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -Og")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    set(CMAKE_C_FLAGS "-Wall")

    set(CMAKE_SHARED_LINKER_FLAGS "-fPIC")

    enable_testing()
    add_subdirectory("tests/unit_test/linux")
endif ()
