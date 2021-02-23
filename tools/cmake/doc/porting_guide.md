# FreeRTOS CMake porting guide

A CMake listfile is required to list a newly qualified board on the FreeRTOS console. Please
use the [`CMakeLists.txt`](../vendors/vendor/board/CMakeLists.txt) template to create a new CMake
listfile file in the `<AFR_ROOT>/cmake/vendors/<vendor_name>/<board_name>` directory (`<AFR_ROOT>`
is the path to the root directory of FreeRTOS source code).

**Note:** You need to understand how to write CMake build files for a project that has at least
one executable target depending on one library target to follow this document.

## Table of contents

- [CMake listfile Template](#cmake-listfile-template)
  - [FreeRTOS Console Metadata](#amazon-freertos-console-metadata)
  - [Compiler Settings](#compiler-settings)
  - [FreeRTOS Portable Layers](#amazon-freertos-portable-layers)
    - [Kernel porting target](#kernel-porting-target)
    - [Other modules](#other-modules)
  - [FreeRTOS Demos and Tests](#amazon-freertos-demos-and-tests)
- [Build and Test FreeRTOS with CMake](#build-and-test-amazon-freertos-with-cmake)
  - [Setting up the toolchain file](#setting-up-the-toolchain-file)
  - [Build FreeRTOS](#build-amazon-freertos)
- [Glossary](#glossary)

## CMake listfile Template

The [`CMakeLists.txt`](../vendors/vendor/board/CMakeLists.txt) template file is divided into four
sections:

- [FreeRTOS Console Metadata](#amazon-freertos-console-metadata)
- [Compiler Settings](#compiler-settings)
- [FreeRTOS Portable Layers](#amazon-freertos-portable-layers)
- [FreeRTOS Demos and Tests](#amazon-freertos-demos-and-tests)

The `*.cmake` files located at `<AFR_ROOT>/cmake` provide 2 functions you need to use in your CMake
listfile to define the required information for FreeRTOS:

```cmake
# This function is used to define FreeRTOS Console metadata.
afr_set_board_metadata(<name> <value>)

# This function is used to define the portable layer target associated with an
# FreeRTOS module, e.g., secure sockets or WiFi. It will create a CMake
# global INTERFACE IMPORTED target with a name AFR:<module_name>::mcu_port.
# If `DEPENDS` is used, additional targets will be linked via target_link_libraries.
afr_mcu_port(<module_name> [<DEPENDS> [targets...]])
```

### FreeRTOS Console Metadata

The first section of the template file defines the metadata that is required to display a board's
data in the FreeRTOS console. Use the function `afr_set_board_metadata(<name> <value>)` to
define each field listed in the template:

```cmake
afr_set_board_metadata(ID "board")
afr_set_board_metadata(DISPLAY_NAME "Board")
afr_set_board_metadata(DESCRIPTION "Template Board for AmazonFreeRTOS")
afr_set_board_metadata(VENDOR_NAME "Vendor")
afr_set_board_metadata(FAMILY_NAME "Family")
afr_set_board_metadata(DATA_RAM_MEMORY "0")
afr_set_board_metadata(PROGRAM_MEMORY "0")
afr_set_board_metadata(CODE_SIGNER "")
afr_set_board_metadata(SUPPORTED_IDE "")
afr_set_board_metadata(IDE_<ID>_NAME "")
afr_set_board_metadata(IDE_<ID>_COMPILERS "")
```

Define each field according to the following descriptions:

- `ID` - ID for the board to uniquely identify it.
- `DISPLAY_NAME` - Name of the board displayed on the FreeRTOS Console.
- `DESCRIPTION` - Short description of the board on the FreeRTOS Console.
- `VENDOR_NAME` - Name of the vendor for the board.
- `FAMILY_NAME` - Family of the board.
- `DATA_RAM_MEMORY` - RAM size in suffixed with units (for example: KB).
- `PROGRAM_MEMORY` - Program memory size suffixed with units (for example: MB).
- `CODE_SIGNER` - Code signing platform used for OTA updates. Please Use
    AmazonFreeRTOS-Default for SHA256 Hash Algorithm and ECDSA encryption algorithm. For any other
    code signing please contact us.
- `SUPPORTED_IDE` - Semicolon separated IDs of IDEs supported.
- `IDE_<ID>_NAME` - Name of the IDE. Please replace <ID> with IDE ID described above.
- `IDE_<ID>_COMPILERS` - Names of compilers supported for the corresponding IDE. Please replace
    <ID> with IDE ID described above.

### Compiler Settings

The second section of the template file defines the compiler settings. We require you to use
`afr_mcu_port` with a special name `compiler` to create a target `AFR::compiler::mcu_port` which
holds the compiler settings information. Under the hood, the kernel will publicly link to this
`INTERFACE` target so that the compiler settings are transitively populated to all modules. Please
use standard CMake built-in functions to provide compiler settings. You can find an example for the
TI CC3220 Launchpad in `<AFR_ROOT>/cmake/vendors/ti/cc3220_launchpad/CMakeLists.txt`.

Here are some best practices to follow as you define the compiler settings:

- Use `target_compile_definitions` to provide compile definitions/macros.
- Use `target_compile_options` to provide compiler flags.
- Use `target_include_directories` to provide include directories.
- Use `target_link_options` to provide linker flags.
- Use `target_link_directories` to provide linker search directories.
- Use `target_link_libraries` to provide libraries to link against.

**Note:** If you have defined the compiler settings information somewhere else (for example,
if you're using CMake for your own code), then you don't need to duplicate the information in the
compiler settings section of this CMake file. Simply call `afr_mcu_port` with `DEPENDS` to bring in
the target definition from another location:

```cmake
# <your_target> is defined somewhere else, does not have to be in the same file.
afr_mcu_port(compiler DEPENDS <your_target>)
```

When you call `afr_mcu_port` with `DEPENDS`, it will call
`target_link_libraries(AFR::<module_name>::mcu_port INTERFACE <your_targets>)` for you so that your
compiler settings are populated to the required `AFR::compiler::mcu_port` target.

If you support multiple compilers, you can use the `AFR_TOOLCHAIN` variable to dynamically select
the compiler settings. This variable is set to the name of the compiler that you are using, which
should be same as the toolchain file name found under `<AFR_ROOT>/cmake/toolchains`). For example,

```cmake
if("${AFR_TOOLCHAIN}" STREQUAL "arm-gcc")
    afr_mcu_port(compiler DEPENDS my_gcc_settings).
elseif("${AFR_TOOLCHAIN}" STREQUAL "arm-iar")
    afr_mcu_port(compiler DEPENDS my_iar_settings).
else()
    message(FATAL_ERROR "Compiler ${AFR_TOOLCHAIN} not supported.")
endif()
```

If you need more advanced control over your compiler settings to do things like set compiler
flags based on programming language, or to change settings for release and debug configurations,
you can use [CMake generator expressions](
https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html). These expressions
are not evaluated at the configuration stage when CMake is reading all the listfiles, but at the
generation stage when CMake finishes reading listfiles and is generating build files for the target
build system. Here's an example,

```cmake
set(common_flags "-foo")
set(c_flags "-foo-c")
set(asm_flags "-foo-asm")
target_compile_options(
    my_compiler_settings INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${common_flags} ${c_flags}> # This only have effect on C files.
    $<$<COMPILE_LANGUAGE:ASM>:${common_flags} ${asm_flags}> # This only have effect on ASM files.
)
```

### FreeRTOS Portable Layers

The third section of the template file defines all of the portable layer targets for FreeRTOS
modules. **The only requirement is to use the `afr_mcu_port(<module_name>)` function to define
portable layer target for every FreeRTOS module you're planning to implement.** You're free
to use any CMake functions as long as the target `AFR::<module_name>::mcu_port` created from
`afr_mcu_port` provides the information we need to build the corresponding FreeRTOS module.

 The `afr_mcu_port` function creates a [GLOBAL INTERFACE IMPORTED library target](
https://cmake.org/cmake/help/latest/command/add_library.html?#interface-libraries) with the name
`AFR::<module_name>::mcu_port`. `GLOBAL` allows us to reference the target in our CMake
listfiles. `INTERFACE` specifies that it should not be built as a standalone target/library, and
will be compiled into the corresponding FreeRTOS module. And `IMPORTED` allows us to use
namespace (double colon) in the target name like `AFR::kernel::mcu_port`.

All modules without corresponding portable layer targets are disabled by default. If you run CMake
to configure FreeRTOS without defining any portable layer target, you should see the
following output:

```txt
FreeRTOS modules:
  Modules to build:
  Disabled by user:
  Disabled by dependency:  kernel, posix, pkcs11, secure_sockets, mqtt, ...

  Available demos:
  Available tests:
```

As you update the `CMakeLists.txt` file with porting layer targets, corresponding FreeRTOS
modules wll be enabled. You should also be able to build any FreeRTOS module whose dependency
requirements are satisfied. For example, if MQTT is enabled, Shadow will also be enabled since it
only depends on MQTT.

**Note:** The kernel dependency is a minimum requirement. The CMake configuration will fail if the
kernel dependency is not satisfied.

#### Kernel porting target

The kernel target depends on FreeRTOS, your driver code and compiler settings. Compiler settings are
already covered in the second section. We require you to use `afr_mcu_port` with the module name
`kernel` to create the required portable layer target `AFR::kernel::mcu_port`, and provides the rest
dependency information, FreeRTOS portable layer and driver code information, to this target. Here's
a simple example,

**Create a `STATIC` library target for the driver code:**

```cmake
add_library(my_board_driver STATIC ${driver_sources})

# Use your compiler settings
target_link_libraries(
    my_board_driver
    PRIVATE AFR::compiler::mcu_port
# Or use your own target if you already have it.
#   PRIVATE ${compiler_settings_target}
)

target_include_directories(
    my_board_driver
    PRIVATE "<include_dirs_for_private_usage>"
    PUBLIC "<include_dirs_for_public_interface>"
)
```

Or an `INTERFACE` library target:

```cmake
# No need to specify compiler settings since kernel target has them.
add_library(my_board_driver INTERFACE ${driver_sources})
```

**Note:** `INTERFACE` library target does not have a build output, in this case the driver code
will be compiled into the kernel library.

**Configure the FreeRTOS portable layer:**

```cmake
add_library(freertos_port INTERFACE)
target_sources(
    freertos_port
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/port.c"
        "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F/portmacro.h"
        "${AFR_MODULES_DIR}/FreeRTOS/portable/MemMang/heap_4.c"
)
target_include_directories(
    freertos_port
    INTERFACE
        "${AFR_MODULES_DIR}/FreeRTOS/portable/GCC/ARM_CM4F"
        "${include_path_to_FreeRTOSConfig_h}
)
```

**Note:** This FreeRTOS portable layer target is an example and not required. You can optionally
specify these source files and include directories directly in `AFR::kernel::mcu_port`.

**Create the kernel portable layer target:**

```cmake
# Bring in driver code and freertos portable layer dependency.
afr_mcu_port(kernel DEPENDS my_board_driver freertos_port)

# If you need to specify additional configurations, use standard CMake functions with
# AFR::kernel::mcu_port as the target name.
target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE
        "${additional_includes}" # e.g. board configuration files
)
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE
        "${additional_dependencies}"
)
```

After the kernel portable layer is configured, you can compile the kernel as a static library. To
test the configuration, you can write a simple hello world application to consume it and make the
LED lights on your board blink. More details on how to build and test are covered in
[Build and Test FreeRTOS with CMake](#build-and-test-amazon-freertos-with-cmake).

```cmake
add_executable(
    my_led_demo
    main.c
)
target_link_libraries(
    my_led_demo
    PRIVATE AFR::kernel
)
```

#### Other modules

After you add the portable layer target for the kernel, you can add the portable layer for other
FreeRTOS modules. Here's a simple example for Wi-Fi:

```cmake
afr_mcu_port(wifi)
target_sources(
    AFR::wifi::mcu_port
    INTERFACE "${AFR_MODULES_DIR}/wifi/portable/<vendor>/<board>/iot_wifi.c"
)
```

This example portable layer only has one implementation file that is based on your driver code.

**Note:** All non-kernel FreeRTOS modules implicitly depend on the kernel. Their porting
layers don't require you to explicitly specify the kernel as a dependency. POSIX, on the other
hand, is defined as an optional kernel module, you need to explicitly include it in your kernel
portable layer if you want to use it. For example:

```cmake
# By default, AFR::posix target does not expose standard POSIX headers in its public
# interface, i.e., you need to use "FreeRTOS_POSIX/pthread.h"  instead of "pthread.h".
# Link to AFR::use_posix instead if you need to use those headers directly.
target_link_libraries(
    AFR::kernel::mcu_port
    INTERFACE AFR::use_posix
)
```

Take Secure Sockets module as another example. This module depends on TLS, making the portable layer
target for this module slightly more complicated than Wi-Fi. FreeRTOS provides a default
TLS implementation based on mbedtls:

```cmake
afr_mcu_port(secure_sockets)
target_sources(
    AFR::secure_sockets::mcu_port
    INTERFACE ${portable_layer_sources}
)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    AFR::tls
)
```

If your hardware is able to handle TLS by itself, you can just use your driver code directly. And
since all FreeRTOS modules implicitly depend on kernel which includes your driver code, you
don't need to call `target_link_libraries` in this case.

Note that we use the standard CMake function `target_link_libraries` to state that the portable
layer depends on `AFR::tls` in the example above. You can reference all FreeRTOS modules by
using their target name `AFR::<module_name>`. For example, you can use the same syntax to also
depend on FreeRTOS-Plus-TCP:

```cmake
target_link_libraries(
    AFR::secure_sockets::mcu_port
    AFR::freertos_plus_tcp
    AFR::tls
)
```

### FreeRTOS Demos and Tests

This section defines the demo and test targets for FreeRTOS. CMake targets will be created
automatically for each demo and test that satisfy the dependency requirements. You need to define
an executable target with `add_executable` and use `aws_tests` or `aws_demos` as the target name
depending on whether you're compiling tests or not. You may also need to provide additional project
settings such as linker scripts and post build commands. We will call `target_link_libraries` to
link available CMake demo or test targets to your executable target.

**Note:** To enable certain demos and tests you still need to modify
`demos/common/demo_runner/aws_demo_runner.c` and `tests/common/test_runner/aws_test_runner.c`.

```cmake
if(AFR_IS_TESTING)
    set(exe_target aws_tests)
else()
    set(exe_target aws_demos)
endif()

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
add_executable(${exe_target} "${board_dir}/application_code/main.c")
```

For information about how to run post build commands, please check [add_custom_command](
https://cmake.org/cmake/help/latest/command/add_custom_command.html). You need to use the second
signature, for example:

```cmake
# This should run an external command "command --arg1 --arg2".
add_custom_command(
    TARGET ${exe_target} POST_BUILD COMMAND "command" "--arg1" "--arg2"
)
```

**Note:** CMake itself supports many common operations in a platform independent way, like creating
directories, copying files, etc. Check the reference [here](
https://cmake.org/cmake/help/latest/manual/cmake.1.html#command-line-tool-mode) for more
information. You can reference CMake itself with the built-in variable `${CMAKE_COMMAND}`.

## Build and Test FreeRTOS with CMake

### Setting up the toolchain file

By default, CMake targets your host OS as the target system. To use CMake for cross-compiling
instead, you need to provide a toolchain file that specifies the compiler you want to use.
Some examples can be found in `<AFR_ROOT>/cmake/toolchains`. If you're using a different compiler
than what we already provide, you need to write this toolchain file first.

### Build FreeRTOS

Follow this [README.md](../README.md) under the cmake folder. Make sure your directory structure is
correct, it is required that you put the `CMakeLists.txt` file and the toolchain file under the
expected location:

```txt
ROOT_DIR
|-- cmake
|   |-- vendors
|   |   `-- <vendor_name>
|   |       `-- <board_name>
|   |           `-- CMakeLists.txt [REQUIRED]
|   `-- toolchains
|       `-- <toolchain_name>.cmake [REQUIRED]
|
|-- lib
|-- demos
|-- tests
`-- CMakeLists.txt
```

## Glossary

- `FreeRTOS module`: A library from FreeRTOS, e.g., Secure Sockets, MQTT and OTA.
  Note that FreeRTOS itself is also an independent module and it's part of the kernel module. The
  configuration of each module still has to be done through their config files as of now, e.g.,
  `FreeRTOSConfig.h`, `iot_mqtt_config.h`.
- `Kernel`: A special FreeRTOS module that consist of FreeRTOS, portable layer and the
  optional POSIX modules. Modules within the kernel can depends on each other but not from modules
  outside the kernel. Regular modules outside kernel implicitly depend on the kernel module.
- `Portable layer`: An implementation of an FreeRTOS module that requires porting from
  vendors. For example, OTA have a portable layer that vendor needs to implement while MQTT does
  not.
- `Target`: CMake target, usually created via `add_executable` and `add_library`.
