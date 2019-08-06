/*
 * Amazon FreeRTOS Defender V2.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_defender_schema_v1.h
 * @brief User-facing Device Defender report schema definition. version 1.x.
 *
 * Schema version format: <major>.<minor>.
 * Include this file, if the report schema is on version 1.x.
 * When new metrics is added minor version is bumped.
 * When metrics is deleted major version is bumped.
 * Long tags are for readability and not recommended to be used.
 */

#ifndef AWS_IOT_DEFENDER_SCHEMA_V1_H_
#define AWS_IOT_DEFENDER_SCHEMA_V1_H_


/*
 * A sample report could look like below.
 * {
 * "hed": {
 *  "rid": 1,
 *  "v": "1.1"
 * },
 * "met": {
 *  "km": {
 *    "hp": 48264,
 *    "hplo": 48264,
 *    ...
 *    "tno": 8,
 *    "tdet": [...],
 *    ...
 *    "mupt": "114146063",
 *    "mpct": "99"
 *    },
 *  "tc": {
 *    ...
 *  }
 * }
 * }
 */
#define  HEADER_TAG                     AwsIotDefenderInternal_SelectTag( "header", "hed" )
#define  REPORTID_TAG                   AwsIotDefenderInternal_SelectTag( "report_id", "rid" )
#define  VERSION_TAG                    AwsIotDefenderInternal_SelectTag( "version", "v" )
#define  METRICS_TAG                    AwsIotDefenderInternal_SelectTag( "metrics", "met" )

/* In version 1.0, below metrics is supported. */
#define  VERSION_1_0                    "1.0"

/* Network metrics -- TCP connections. */
#define TCP_CONN_TAG                    AwsIotDefenderInternal_SelectTag( "tcp_connections", "tc" )
#define EST_CONN_TAG                    AwsIotDefenderInternal_SelectTag( "established_connections", "ec" )
#define TOTAL_TAG                       AwsIotDefenderInternal_SelectTag( "total", "t" )
#define CONN_TAG                        AwsIotDefenderInternal_SelectTag( "connections", "cs" )
#define REMOTE_ADDR_TAG                 AwsIotDefenderInternal_SelectTag( "remote_addr", "rad" )

/* In version 1.1, below metrics is supported. */
#define  VERSION_1_1                    "1.1"

/* Kernel metrics.
 * Prefixed with k (kernel), m (mcu), t (task), hp (hp), st (stack). */
#define KERNEL_METRICS                  AwsIotDefenderInternal_SelectTag( "kernel_metrics", "km" )
#define KERNEL_MCU_UPTIME               AwsIotDefenderInternal_SelectTag( "mcu_uptime", "mupt" )
#define KERNEL_MCU_UTILIZATION          AwsIotDefenderInternal_SelectTag( "mcu_utilization", "mpct" )

#define KERNEL_HEAP_FREE_SIZE           AwsIotDefenderInternal_SelectTag( "heap_free_size", "hp" )
#define KERNEL_HEAP_LARGEST_FREE        AwsIotDefenderInternal_SelectTag( "heap_largest_free_block", "hplg" )
#define KERNEL_HEAP_SMALLEST_FREE       AwsIotDefenderInternal_SelectTag( "heap_smallest_free_block", "hpsm" )
#define KERNEL_HEAP_NUM_OF_FREE         AwsIotDefenderInternal_SelectTag( "heap_free_blocks", "hpno" )
#define KERNEL_HEAP_LOW_WATERMARK       AwsIotDefenderInternal_SelectTag( "heap_low_watermark", "hplo" )
#define KERNEL_HEAP_SUCCESSFUL_ALLOC    AwsIotDefenderInternal_SelectTag( "heap_succ_alloc", "hpal" )
#define KERNEL_HEAP_SUCCESSFUL_FREE     AwsIotDefenderInternal_SelectTag( "heap_succ_free", "hpfr" )

#define KERNEL_NUM_OF_TASKS             AwsIotDefenderInternal_SelectTag( "num_of_tasks", "tno" )
#define KERNEL_TASK_DETAILS             AwsIotDefenderInternal_SelectTag( "task_details", "tdet" )

#define KERNEL_TASK_ID                  AwsIotDefenderInternal_SelectTag( "task_id", "tid" )
#define KERNEL_TASK_NAME                AwsIotDefenderInternal_SelectTag( "task_name", "tn" )
#define KERNEL_TASK_STATUS              AwsIotDefenderInternal_SelectTag( "task_status", "ts" )
#define KERNEL_TASK_PRIORITY            AwsIotDefenderInternal_SelectTag( "task_priority", "tp" )
#define KERNEL_TASK_ABS_CYCLES          AwsIotDefenderInternal_SelectTag( "task_abs_cycles", "tcyc" )
#define KERNEL_TASK_PERCENTAGE          AwsIotDefenderInternal_SelectTag( "task_percentage", "tpct" )
#define KERNEL_STACK_HIGH_WATERMARK     AwsIotDefenderInternal_SelectTag( "stack_high_watermark", "sthi" )

/* Device type.
 * Prefixed with d (device). */
#define DEVICE_TYPE                     AwsIotDefenderInternal_SelectTag( "device_type", "dtp" )



#endif /* AWS_IOT_DEFENDER_SCHEMA_V1_H_ */
