set(COMPONENT_SRCS
    face_detection/fd_forward.c
    face_recognition/fr_forward.c
    face_recognition/fr_flash.c
    image_util/image_util.c
    )

set(COMPONENT_ADD_INCLUDEDIRS
    face_detection/include
    face_recognition/include
    image_util/include
    lib/include
    )

set(COMPONENT_REQUIRES spi_flash)

register_component()

target_link_libraries(esp-face "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(esp-face
    dl_lib
    fd
    fr
    )

