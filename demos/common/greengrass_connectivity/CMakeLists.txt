# Greengrass demo
afr_demo_module(demo_greengrass_connectivity)
afr_set_demo_metadata(ID "greengrass_discovery_example")
afr_set_demo_metadata(DESCRIPTION "An example on how to discover and connect to AWS Greengrass")
afr_set_demo_metadata(DISPLAY_NAME "AWS Greengrass Discovery")

afr_module_sources(
    demo_greengrass_connectivity
    INTERFACE
        "${AFR_DEMOS_DIR}/greengrass_connectivity/aws_greengrass_discovery_demo.c"
        "${AFR_DEMOS_DIR}/include/aws_greengrass_discovery_demo.h"
        "${AFR_DEMOS_DIR}/greengrass_connectivity/README.md"
)
afr_module_dependencies(
    demo_greengrass_connectivity
    INTERFACE
        AFR::greengrass
)
