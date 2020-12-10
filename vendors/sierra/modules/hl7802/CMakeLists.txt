# This port uses cellular HAL common
include("${AFR_VENDORS_DIR}/aws/modules/3gpp/CMakeLists.txt")

# Cellular common
afr_module(NAME cellular_module_port PUBLIC)

afr_module_sources(
    cellular_module_port
    PRIVATE
        "${AFR_VENDORS_DIR}/sierra/modules/hl7802/ports/cellular/cellular_hl7802.c"
        "${AFR_VENDORS_DIR}/sierra/modules/hl7802/ports/cellular/cellular_hl7802_api.c"
        "${AFR_VENDORS_DIR}/sierra/modules/hl7802/ports/cellular/cellular_hl7802_urc_handler.c"
        "${AFR_VENDORS_DIR}/sierra/modules/hl7802/ports/cellular/cellular_hl7802_wrapper.c"
)

afr_module_dependencies(
    cellular_module_port
    PRIVATE
        AFR::cellular
        AFR::cellular_common
)

if( ${AFR_BOARD} STREQUAL "sierra.sensorhub" )
set_source_files_properties(
    "${AFR_VENDORS_DIR}/sierra/modules/hl7802/ports/cellular/cellular_hl7802_api.c"
    PROPERTIES COMPILE_FLAGS
    "-Wno-format"
)
endif()
