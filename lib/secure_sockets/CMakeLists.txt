afr_module(secure_sockets)

afr_module_sources(
    secure_sockets
    PRIVATE
        "${AFR_MODULES_DIR}/include/aws_secure_sockets.h"
        "${AFR_MODULES_DIR}/include/private/aws_secure_sockets_config_defaults.h"
)

afr_module_include_dirs(
    secure_sockets
    PUBLIC
        "${AFR_MODULES_DIR}/include"
        "${AFR_MODULES_DIR}/include/private"
)

afr_module_dependencies(
    secure_sockets
    PRIVATE AFR::secure_sockets::mcu_port
)

# Link to this INTERFACE target to use the default implementation based on FreeRTOS-Plus-TCP.
afr_module(secure_sockets_freertos_tcp INTERFACE)
afr_module_sources(
    secure_sockets_freertos_tcp INTERFACE
    "${AFR_MODULES_DIR}/portable/freertos_plus_tcp/aws_secure_sockets.c"
)
afr_module_dependencies(
    secure_sockets_freertos_tcp INTERFACE
    AFR::tls
    AFR::crypto
    AFR::pkcs11
)
