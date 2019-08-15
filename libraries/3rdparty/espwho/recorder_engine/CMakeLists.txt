set(COMPONENT_SRCS a.c)
set(COMPONENT_ADD_INCLUDEDIRS include)

register_component()

target_link_libraries(recorder_engine "-L ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(recorder_engine
    esp_wakenet
    nn_model
    )
