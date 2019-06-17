afr_module()

afr_glob_src(posix_headers DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include" RECURSE)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC ${posix_headers}
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        $<TARGET_PROPERTY:AFR::${AFR_CURRENT_MODULE}::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::${AFR_CURRENT_MODULE}::mcu_port
)

# By default we don't expose the standard posix headers to the include paths. Users have to access
# it by "namespace", e.g., #include <FreeRTOS_POSIX/pthread.h>. Only link to this target if you need
# direct access to the standard posix headers. If you need to make sure the this POSIX include dir
# is put in front of other include directories, you can use generator expression and
# target_include_directories with BEFORE, e.g.,
# target_include_directories(
#     <target> BEFORE
#     PUBLIC $<TARGET_PROPERTY:AFR::posix,INTERFACE_INCLUDE_DIRECTORIES>
# )
afr_module(NAME posix_headers INTERFACE)
afr_module_include_dirs(
    posix_headers
    INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/include/FreeRTOS_POSIX"
)
