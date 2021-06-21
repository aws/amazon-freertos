# MQTT Agent Interface
afr_module(NAME mqtt_agent_interface INTERNAL)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}")

set(include_dir "${CMAKE_CURRENT_LIST_DIR}/include")

# Add cmake files of module to metadata.
afr_module_cmake_files(${AFR_CURRENT_MODULE} 
    ${CMAKE_CURRENT_LIST_DIR}/mqtt_agent_interface.cmake
)

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        "${src_dir}/freertos_agent_message.c"
        "${src_dir}/freertos_command_pool.c"
        # Header files added to the target so that these are available in code
        # downloaded from the FreeRTOS console.
        "${include_dir}/freertos_agent_message.h"
        "${include_dir}/freertos_command_pool.h"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        AFR::core_mqtt_agent
        AFR::core_mqtt
)

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC
       "${include_dir}"
       "${src_dir}"
)

