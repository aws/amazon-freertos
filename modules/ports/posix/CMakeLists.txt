afr_module(KERNEL)

afr_glob_src(posix_headers DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include" RECURSE)
afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PUBLIC ${posix_headers}
)

# If you need to make sure the this POSIX include dir is put in front of other include directories,
# you can use generator expression and target_include_directories with BEFORE, e.g.,
# target_include_directories(
#     <target> BEFORE
#     PUBLIC $<TARGET_PROPERTY:AFR::posix,INTERFACE_INCLUDE_DIRECTORIES>
# )
afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_CURRENT_LIST_DIR}/include/FreeRTOS_POSIX"
        $<${AFR_IS_TESTING}:$<TARGET_PROPERTY:AFR::${AFR_CURRENT_MODULE}::mcu_port,INTERFACE_INCLUDE_DIRECTORIES>>
    PRIVATE
        $<TARGET_PROPERTY:AFR::kernel,INTERFACE_INCLUDE_DIRECTORIES>
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::${AFR_CURRENT_MODULE}::mcu_port
        AFR::compiler::mcu_port
)
