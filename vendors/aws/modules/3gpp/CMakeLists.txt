# Cellular common
afr_module(NAME cellular_common PRIVATE)

afr_module_sources(
    cellular_common
    PRIVATE
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_3gpp_api.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_3gpp_urc_handler.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_common.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_common_api.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_pkthandler.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_at_core.c"
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/src/cellular_pktio.c"
)

afr_module_include_dirs(
    cellular_common
    PUBLIC
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/include"
    PRIVATE
        "${AFR_VENDORS_DIR}/aws/modules/3gpp/ports/cellular/include/private"
)

afr_module_dependencies(
    cellular_common
    PRIVATE
        AFR::cellular
)
