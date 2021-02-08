## Sample Implementation of Logging Interface

This sample provides reference implementations of the [logging interface](#using-your-custom-implementation-of-logging-interface) that is used by FreeRTOS libraries and demos to generate log messages. This sample provides
implementations of the logging interface for both the ISO C90 and ISO C99 (with GNU extension) standards.

The sample implementations add metadata information of **task name**, **FreeRTOS timer tick count** and **incrementing counter value** as prefixes to each log message. Additionally, the ISO C99 implementation adds metadata information of the **library name** and **source file location** to each log message.

**Note**: Users can provide their own implementations of the logging interface to enable logging, if they choose to not use this implementation. Please refer [here](#using-your-custom-implementation-of-logging-interface) for more information.

### Logging Task 
The sample implementation serializes logging messages from different tasks using a FreeRTOS queue that is serviced by a daemon logging task. The daemon task reads log message strings from the queue and forwards them to the vendor-specific function through the `configPRINT` macro.

Each of the implementations (ISO C90 and ISO C99 with GNU extension) route the logging interface macros to a logging function (defined in [`iot_logging_task.h`](./include/iot_logging_task.h)) that pushes the message to the FreeRTOS queue, thereby serializing messages logged through the logging interfaces.

### Using the Sample Implementation

To enable logging for a FreeRTOS library and/or demo using the sample implementation, 
* Configure logging level by defining the `LIBRARY_LOG_LEVEL` macro to one of `LOG_NONE` (this disables logging), `LOG_ERROR`, `LOG_WARN`, `LOG_INFO` or `LOG_DEBUG` constants. 
* By default, the ISO C90 implementation is enabled. If your compiler supports ISO C99 (with GNU extension), you can enable the ISO C99 implementation by setting the `LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION` macro to `1`. When using the ISO C99 implementation, define the `LIBRARY_LOG_NAME` macro to configure the library name which is prefixed in
the log messages.

The above macro definitions should occur in the FreeRTOS library-specific config file (like `core_mqtt_config.h` file for the coreMQTT library) for which logging is being configured. 

The logging file include and macro definitions must follow the following order in the config file:
* Include [`logging_levels.h`](./include/logging_levels.h).
* Define the `LIBRARY_LOG_LEVEL`, `LIBRARY_LOG_NAME`, and `LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION` macros as mentioned above.
* Include [`logging_stack.h`](./include/logging_stack.h).

Here is a sample logging configuration for the coreMQTT library which is defined in
`core_mqtt_config.h` file:

```
#include "logging_levels.h"

#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "MQTT"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

#include "logging_stack.h"
```

When using the ISO C90 implementation, the above configuration generates the following log messages for the coreMQTT library. 

```
78 1950 [iot_thread] [INFO] Packet received. ReceivedBytes=2.
79 1950 [iot_thread] [INFO] Ack packet deserialized with result: MQTTSuccess.
80 1951 [iot_thread] [INFO] State record updated. New state=MQTTPublishDone.
```

If using the ISO C99 implementation (i.e. when `LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION` macro is set to `1`), the log messages from coreMQTT library look like the following.  
Note that the library name (`[MQTT]`) and source file location (`[core_mqtt.c:<line-number>]`)
are present in each log message.

```
78 1950 [iot_thread] [INFO] [MQTT] [core_mqtt.c:886] Packet received. ReceivedBytes=2.
79 1950 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1162] Ack packet deserialized with result: MQTTSuccess.
80 1951 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1175] State record updated. New state=MQTTPublishDone.
```

### Using your custom implementation of Logging Interface
The logging interface comprises of the following 4 logging macros, listed in increasing order of verbosity:

1. LogError
2. LogWarn
3. LogInfo
4. LogDebug

`LogError` is only called when there is an error, so is the least verbose, whereas `LogDebug` is called more frequently to provide debug level information and is therefore, the most verbose.

An example call of the logging macros (in FreeRTOS libraries) is the following:

```
LogInfo( ( “This prints an integer %d”, 100 ) );
```

Your custom implementation of the logging interface needs to define these macros to
map them to your logging implementation, depending on the logging levels you want to enable.  
Logging macros use format specifier and variable number of arguments, just like standard function, `printf`, but they use double parenthesis to support ISO C90 standard which should be taken care while defining them.

If you have a thread safe printf function, LogInfo should be defined like
the following. Notice how one set of parentheses are removed because X
already contains a set of parentheses.

```
#define LogInfo( X )  printf X
```
