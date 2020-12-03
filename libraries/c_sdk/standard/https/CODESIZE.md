# Code size of HTTPS Compatibility Layer

Code size for the HTTPS Compatibility Layer is calculated using GCC for ARM Cortex-M4 on [FreeRTOS 202012.00 release](https://github.com/aws/amazon-freertos/releases/tag/202012.00). Build for calculating code size is done using build configuration for [Cypress CY8CKIT-064S0S2-4343W](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_cypress_psoc64.html) by using the Release configuration. All logs were disabled in this build. Refer to the table below for calculated code sizes.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_https_client.c | 21.0K | 19.6K |
| iot_https_utils.c | 0.5K | 0.5K |
| **Total estimate** | **21.5K** | **21.1K** |

The HTTPS Compatibility Layer is implemented using the [coreHTTP library](https://github.com/FreeRTOS/coreHTTP/blob/master/README.md). In addition, the HTTPS Compatibility Layer maintains the dependency of the network abstraction and linear containers from the HTTP V1.x.x library. The task pool dependency is removed in order to allow statically-allocated tasks. To account for the total code size of the HTTPS Compatibility Layer, code sizes for all dependencies have been considered (see below).

1. **The coreHTTP library** : The HTTPS Compatibility Layer is implemented using [coreHTTP library](https://github.com/FreeRTOS/coreHTTP/blob/master/README.md). Memory estimates for [coreHTTP library](https://github.com/FreeRTOS/coreHTTP/blob/master/README.md) can be found in the documentation [here](https://freertos.org/mqtt/index.html).

2. **Platform Abstraction** :  This HTTPS Compatibility Layer depends on the platform-specific implementation of network abstraction. The code size is calculated for the [FreeRTOS implementation of Abstraction Layer](../../../abstractions/platform/freertos) and is added in the table below.

| File | With -O1 Optimization | With -Os Optimization |
| :-: | :-: | :-: |
| iot_network_freertos.c | 1.2K | 1.1K |
| **Total estimate** | **1.2K** | **1.1K** |

Note that the implementation for network abstraction may not be exclusively used by the HTTPS Compatibility Layer, but may be shared by other libraries. However, this network abstraction is not used by any of the [redesigned libraries for LTS](https://www.freertos.org/ltsroadmap.html).

3. **Linear Containers** : This HTTPS Compatibility Layer depends on [Linear Containers](https://docs.aws.amazon.com/freertos/latest/lib-ref/embedded-csdk/v4.0_beta_deprecated/lib-ref/c-sdk/linear_containers/index.html). However, this implementation can be found in a [c header file](../common/include/iot_linear_containers.h), and its code sizes are already accounted for in the calculated sizes of this HTTPS Compatibility Layer.
