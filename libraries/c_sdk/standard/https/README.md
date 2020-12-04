# HTTPS Compatibility Layer

HTTPS Compatibility Layer provides backward compatibility from [coreHTTP library](https://github.com/FreeRTOS/coreHTTP/blob/master/README.md) to [HTTP V1.x.x APIs](include/iot_https_client.h). **We recommend that you use the coreHTTP library for optimized memory usage and modularity, but you can use the HTTPS Compatibility Layer when you do not have the flexibility.**


## Configs for HTTPS Compatibility Layer

Configuration settings for the HTTPS Compatibility Layer, are available in addition to the [configurations of HTTPS V1.x.x library](https://docs.aws.amazon.com/freertos/latest/lib-ref/html2/https/https_config.html).

Configuration settings are C pre-processor constants. They can be set with a #define in the config file (iot_config.h) or by using a compiler option such as -D in gcc. If a configuration setting is not defined, the HTTPS Compatibility Layer will use a "sensible" default value (unless otherwise noted). Because they are compile-time constants, this HTTPS Compatibility Layer must be rebuilt if a configuration setting is changed.

1. `IOT_HTTPS_DISPATCH_QUEUE_SIZE` - The number of requests in the queue that are ready to be sent to the HTTP server.
2. `IOT_HTTPS_DISPATCH_TASK_COUNT` - The number of tasks that are responsible for sending requests from the dispatch queue.
3. `IOT_HTTPS_DISPATCH_TASK_STACK_SIZE` - The stack size of each dispatch task, sized appropriately for each board.
4. `IOT_HTTPS_DISPATCH_USE_STATIC_MEMORY` - If set to 1, the memory used by the dispatch task will be allocated statically by the library. Otherwise, memory will be allocated on the heap.
5. `IOT_HTTPS_DISPATCH_TASK_PRIORITY` - The priority of each dispatch task. This priority is deliberately chosen to match the original taskpool's priority. Doing so prevents starvation of the network-receive task and tasks potentially used by other libraries.


## Code size of HTTPS Compatibility Layer

Code sizes are calculated for the HTTPS Compatibility Layer on [FreeRTOS 202011.00 release](https://github.com/aws/amazon-freertos/releases/tag/202011.00). In order to calculate the total cost to memory of the HTTPS Compatibility Layer, the code sizes of its dependencies are also included. HTTPS Compatibility Layer and its dependencies have a code size of **30.4KB** with `-Os` compiler optimization.
Please be aware that, this code size is about **22.2KB** higher than the [code size](https://docs.aws.amazon.com/embedded-csdk/202011.00/lib-ref/libraries/standard/coreHTTP/docs/doxygen/output/html/index.html) required by the [coreHTTP library](https://github.com/FreeRTOS/coreHTTP/blob/master/README.md).

**Note** Refer to the [CODESIZE.md](CODESIZE.md) for more details about the calculation of code sizes for the HTTPS Compatibility Layer and the [coreHTTP FreeRTOS documentation](https://freertos.org/http/index.html) for coreHTTP library.


## Tasks required for HTTPS Compatibility Layer

The HTTPS Compatibility Layer has a run time dependency on tasks created by each dispatch task and Network Abstraction implementation, in addition to the system tasks. The Network Abstraction implementation creates a task to receive from the network.

