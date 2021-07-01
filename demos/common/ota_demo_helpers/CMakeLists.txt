afr_module( INTERNAL NAME ota_demo_version )

# Add this CMakeLists.txt file to the metadata. This is required because the
# name of the CMake target does not match the name of the folder.
afr_module_cmake_files(${AFR_CURRENT_MODULE}
    ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ota_application_version.c"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::ota
)