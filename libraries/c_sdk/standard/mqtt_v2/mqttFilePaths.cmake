# This file is to add source files and include directories
# into variables so that it can be reused from different repositories
# in their Cmake based build system by including this file.
#
# Files specific to the repository such as test runner, platform tests
# are not added to the variables.

# MQTT library source files.
set( MQTT_SOURCES
     "${MODULES_DIR}/standard/mqtt/src/mqtt.c"
     "${MODULES_DIR}/standard/mqtt/src/mqtt_state.c"
     "${MODULES_DIR}/standard/mqtt/src/mqtt_lightweight.c" )

# MQTT library Public Include directories.
set( MQTT_INCLUDE_PUBLIC_DIRS
     "${MODULES_DIR}/standard/mqtt/include"
     "${MODULES_DIR}/standard/utilities/include" )

# MQTT library Private Include directories.
set( MQTT_INCLUDE_PRIVATE_DIRS
     "${MODULES_DIR}/standard/mqtt/src" )