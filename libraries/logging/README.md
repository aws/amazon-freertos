## Sample Implementation of Logging Interface

This library provides reference implementations of the [logging interface](#the-logging-interface) that is used by FreeRTOS libraries and demos to generate log messages. This library provides
implementations for both ISO C90 standard and ISO C99 standard (with GNU extension) of the logging interface.

**Note**: Users can provide their own implementations of the logging interface to enable logging, if they choose to not use this implementation.

### Logging Task 
Logging messages from multiple tasks are serialized with a FreeRTOS queue that is serviced by a daemon logging task. The daemon task reads log message strings from the queue and forwards them to the vendor-specific function through the `configPRINT` macro.

Each of the implementations (C90 and C99 with GNU extension) route the logging interface macros to a logging function (defined in [`iot_logging_task.h`](./include/iot_logging_task.h)) that pushes the message to the FreeRTOS queue, thereby serializing messages logged through the logging interfaces.

### Setting the Verbosity Level
To configure the verbosity level of log messages from a FreeRTOS library, define the `LIBRARY_LOG_LEVEL` macro to one of the following values in the library-specific configuration file (like `core_mqtt_config.h` for the coreMQTT library). Valid values for LIBRARY_LOG_LEVEL are:

`LOG_NONE (turns logging off)`  
`LOG_ERROR`  
`LOG_WARN`  
`LOG_INFO` 
`LOG_DEBUG`

For example:
```
#define LIBRARY_LOG_LEVEL  LOG_NONE
```

### Log Message Output

The sample implementation adds some metadata information for debugging aid as prefix to the log message strings passed by FreeRTOS libraries and demos. Both the C90 and C99 implementations
add the **task name**, **FreeRTOS timer tick count** and **incrementing counter value** to each log message. 

#### ISO C90 Implementation

This is the default enabled implementation of the logging interface in the library.

If your compiler only supports the ISO C90 standard, the C90 implementation can be used by setting the `LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION` configuration to `0`. (This is default implementation enabled in the library.)
With the ISO C90 implementation, here is an example output of log messages for the MQTT Connection Sharing Demo:

```
83 2128 [SyncPublisher] [INFO] Publish operation complete. Sleeping for 100 ms.
84 2140 [SyncPublisher] [INFO] Adding publish operation for message Hello World! Sync: 3
on topic filter/sync/3
85 2142 [SyncPublisher] [INFO] Waiting for publish 3 to complete.
86 2168 [iot_thread] [INFO] Packet received. ReceivedBytes=2.
87 2168 [iot_thread] [INFO] Ack packet deserialized with result: MQTTSuccess.
88 2168 [iot_thread] [INFO] State record updated. New state=MQTTPublishDone.
89 2170 [SyncPublisher] [INFO] Publish operation complete. Sleeping for 100 ms.
90 2182 [SyncPublisher] [INFO] Adding publish operation for message Hello World! Sync: 4
on topic filter/sync/4
91 2184 [SyncPublisher] [INFO] Waiting for publish 4 to complete.
92 2209 [iot_thread] [INFO] Packet received. ReceivedBytes=2.
93 2209 [iot_thread] [INFO] Ack packet deserialized with result: MQTTSuccess.
94 2209 [iot_thread] [INFO] State record updated. New state=MQTTPublishDone.
95 2210 [SyncPublisher] [INFO] Publish operation complete. Sleeping for 100 ms.
```


#### ISO C99 Implementation (with GNU extension)

This implementation uses variadic macros specified by the ISO C99 standard macros and [the GNU extension feature of comma elision](https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Variadic-Macros.html) with the `##` operator.
If your toolchain supports both ISO C99 and GNU extension, you can enabled this implementation by setting the `LOGGING_ENABLE_METADATA_WITH_C99_AND_GNU_EXTENSION` configuration.

With the use of variadic macro feature of ISO C99, this implementation adds extra metadata information of the source library and file location of the log messages.
**Note**: If you want to add/remove/change the metadata added, you can directly make changes in the [`logging_stack.h`](./include/logging_stack.h) file.

#### Setting the Text Name
To set the text name define the LIBRARY_LOG_NAME macro to a string within the same configuration file used to define SdkLog(). Each log message prints the name, so it is normal to set the name to the name of the library. For example:
```
#define LIBRARY_LOG_NAME “MQTT”
```
Setting the name to an empty string will save program space.

Using this implementation. here is an example output of log messages for the MQTT Connection Sharing Demo:

```
78 1950 [iot_thread] [INFO] [MQTT] [core_mqtt.c:886] Packet received. ReceivedBytes=2.
79 1950 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1162] Ack packet deserialized with result: MQTTSuccess.
80 1951 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1175] State record updated. New state=MQTTPublishDone.
81 1953 [SyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1846] Publish operation complete. Sleeping for 100 ms.

82 1953 [AsyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1970] Deleting Async Publisher task.
83 1965 [SyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1820] Adding publish operation for message Hello World! Sync: 3
on topic filter/sync/3
84 1967 [SyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1832] Waiting for publish 3 to complete.
85 1981 [iot_thread] [INFO] [MQTT] [core_mqtt.c:886] Packet received. ReceivedBytes=2.
86 1981 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1162] Ack packet deserialized with result: MQTTSuccess.
87 1982 [iot_thread] [INFO] [MQTT] [core_mqtt.c:1175] State record updated. New state=MQTTPublishDone.
88 1983 [SyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1846] Publish operation complete. Sleeping for 100 ms.

89 1995 [SyncPublisher] [INFO] [MQTTDemo] [mqtt_demo_connection_sharing.c:1820] Adding publish operation for message Hello World! Sync: 4
```


### The Logging Interface
The FreeRTOS libraries use the following 4 logging macros, listed in increasing order of verbosity. For example, LogError() is only called when there is an error so is the least verbose, whereas LogDebug() is called more frequently to provide debug level information and is therefore the most verbose.

LogError
LogWarn
LogInfo
LogDebug
Logging macros are used with a variable number of arguments, just like printf() (with the exception that they use double parenthesis). For example, the libraries call the logging macros in the following way:

```
LogInfo( ( “This prints an integer %d”, 100 ) );
```
