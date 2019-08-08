/*******************************************************************************
 * @file iot_perfcounter.h
 *
 * @breif HAL APIs for Performance Counter
 *
 * Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *******************************************************************************
 */

/**
 * @file    iot_perfcounter.h
 * @brief   This file contains all the Performance counters HAL API definitions
 */

#ifndef _IOT_PERFCOUNTER_H
#define _IOT_PERFCOUNTER_H

/**
 * @defgroup iot_perfcounter PerfCounter HAL APIs
 * @{
 */

/**
 * @brief   iot_perfcounter_open is used to Initialize the performance counter.
 *
 */
void iot_perfcounter_open( void );

/**
 * @brief   iot_perfcounter_get_value is used to get the current performance counter
 *          value.
 *
 * @return  returns performance counter value as uint64 value.
 */
uint64_t iot_perfcounter_get_value( void );

/**
 * @brief   iot_perfcounter_get_frequency is used to get the current frequency
 *          performance counters are running at. This can be used to determine the
 *          time delta between two perfcounter values returned by valling iot_perfcounter_get_value()
 *
 * @return  returns the frequency of the performance counter.
 *          This can be used to deterime the period between perfcounter
 *          increments.
 */
uint32_t iot_perfcounter_get_frequency( void );


/**
 * @brief   iot_perfcounter_close is used to de-initialize the perfcounter.
 *          It may reset the counter value in perfcounter.
 *
 */
void iot_perfcounter_close( void );

/**
 * @}
 */

#endif /* ifndef _IOT_PERFCOUNTER_H */
