The SimpleLink CC32xx SDK is needed for building FreeRTOS. Users
must download the SDK and create a symbolic link or copy the SDK to this
directory. The simplelink_sdk_dir variable in
vendors/ti/boards/CC3220SF_LAUNCHXL/CMakeLists.txt must then be updated with
the correct new directory. Alternatively, the version can be passed along with
the CMake call by setting the CC32XX_SDK_VER value.
