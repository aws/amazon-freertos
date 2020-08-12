# Cellular common
afr_module(NAME cellular_module_port PUBLIC)

afr_module_sources(
    cellular_module_port
    PRIVATE
        "${AFR_VENDORS_DIR}/vendor/modules/module/ports/cellular/cellular.c"
)

afr_module_dependencies(
    cellular_module_port
    PRIVATE
        AFR::cellular
)
