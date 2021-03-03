# This file is to add source files and include directories
# into variables so that it can be reused from different repositories
# in their Cmake based build system by including this file.
#
# Files specific to the repository such as test runner, platform tests
# are not added to the variables.

# MQTT agent library source files.
set( MQTT_AGENT_SOURCES
     ${CMAKE_CURRENT_LIST_DIR}/source/freertos_mqtt_agent.c
     ${CMAKE_CURRENT_LIST_DIR}/source/agent_command_pool.c
     ${CMAKE_CURRENT_LIST_DIR}/source/agent_message.c )

# MQTT Agent library public include directories.
set( MQTT_AGENT_INCLUDE_PUBLIC_DIRS
     ${CMAKE_CURRENT_LIST_DIR}/source/include )
