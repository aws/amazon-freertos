/*
 * Amazon FreeRTOS Common IO V0.1.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*******************************************************************************
 * IOT On-Target Unit Test
 * @File: test_iot_internal.h
 * @Brief: File for IOT HAL test board specific configuration setup
 ******************************************************************************/

#include "test_iot_config.h"

#pragma once


/*
 * Test iteration count macros that platforms should set
 * in their repective "test_iot_config.h" file. If not set
 * there, the default value of 1 is set here.
 */

#ifndef IOT_TEST_COMMON_IO_UART_SUPPORTED
 #define IOT_TEST_COMMON_IO_UART_SUPPORTED  1
#endif

#ifndef IOT_TEST_COMMON_IO_FLASH_SUPPORTED
 #define IOT_TEST_COMMON_IO_FLASH_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED
 #define IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_GPIO_SUPPORTED
 #define IOT_TEST_COMMON_IO_GPIO_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_RTC_SUPPORTED
 #define IOT_TEST_COMMON_IO_RTC_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_TIMER_SUPPORTED
 #define IOT_TEST_COMMON_IO_TIMER_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_ADC_SUPPORTED
 #define IOT_TEST_COMMON_IO_ADC_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_RESET_SUPPORTED
 #define IOT_TEST_COMMON_IO_RESET_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED
 #define IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_PWM_SUPPORTED
 #define IOT_TEST_COMMON_IO_PWM_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_I2C_SUPPORTED
 #define IOT_TEST_COMMON_IO_I2C_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED
 #define IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_POWER_SUPPORTED
 #define IOT_TEST_COMMON_IO_POWER_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_BATTERY_SUPPORTED
 #define IOT_TEST_COMMON_IO_BATTERY_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_EFUSE_SUPPORTED
 #define IOT_TEST_COMMON_IO_EFUSE_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_SPI_SUPPORTED
 #define IOT_TEST_COMMON_IO_SPI_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_USB_DEVICE_SUPPORTED
 #define IOT_TEST_COMMON_IO_USB_DEVICE_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_USB_HOST_SUPPORTED
 #define IOT_TEST_COMMON_IO_USB_HOST_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_SDIO_SUPPORTED
 #define IOT_TEST_COMMON_IO_SDIO_SUPPORTED 1
#endif

#ifndef IOT_TEST_COMMON_IO_I2S_SUPPORTED
 #define IOT_TEST_COMMON_IO_I2S_SUPPORTED 1
#endif


/*
 * The variables below are used in the tests. Generally. they should be
 * set in SET_TEST_IOT_RESET_CONFIG() defined in the respective platform
 * directory. If not set, the default value set at the variable
 * definition is used.
 */

/* Uart */
extern uint8_t uctestIotUartPort;                       /* The index of the UART that will be tested */

/* Flash */
extern uint32_t ultestIotFlashStartOffset;              /* The Flash offset at which the flash operations in the test will take place */

/* GPIO */
extern int32_t ltestIotGpioPortA;                       /* The 1st GPIO port used in the loop back test */
extern int32_t ltestIotGpioPortB;                       /* The 2nd GPIO port used in the loop back test */
extern uint16_t ustestIotGpioConfig;                    /* The configuration of GPIO in the test (port, direction, irq, write value)
                                                         * Check test_iot_gpio.c for its bit assignment */
extern uint32_t ultestIotGpioWaitTime;                  /* Wait time for GPIO port A to receive GPIO interrupt from port B during the test
                                                         * This is needed to avoid indefinite wait during the test */
extern uint32_t ultestIotGpioSlowSpeed;                 /* Based on the underlying HW, set the slow speed setting of GPIO */
extern uint32_t ultestIotGpioFastSpeed;                 /* Based on the underlying HW, set the high speed setting of GPIO */
extern uint32_t ultestIotGpioFunction;                  /* Alt Function for GPIO for the pin */


/* Timer */
extern int32_t ltestIotTimerInstance;                   /* HW timer instance to use */

/* SPI */
extern uint32_t ultestIotSpiInstance;                   /* SPI instance that we plan to test */
extern uint32_t ulAssistedTestIotSpiInstance;           /* SPI assisted tests */
extern uint32_t ultestIotSpiSlave;                      /* SPI assisted tests */
extern uint32_t ulAssistedTestIotSpiSlave;              /* SPI assisted tests */

/* ADC */
extern uint8_t uctestIotAdcChListLen;                   /* Length of ADC chains used in the test. The chains are ADC channels
                                                         * that would be sampled with a single trigger */
extern uint8_t * puctestIotAdcChList;                   /* The ADC chains used in the tests */
extern uint8_t ucAssistedTestIotAdcChannel;             /* Assisted Tests: ADC Channel list */

/* PWM */
extern uint32_t ultestIotPwmGpioInputPin;               /* GPIO used to measure PWM accuracy */
extern uint32_t ultestIotPwmInstance;                   /* PWM instance */
extern uint32_t ultestIotPwmFrequency;                  /* PWM freq */
extern uint32_t ultestIotPwmDutyCycle;                  /* PWM duty cycle */
extern uint32_t ultestIotPwmChannel;                    /* PWM Channel */
extern uint32_t ulAssistedTestIotPwmGpioInputPin;       /* PWM assisted tests only */

/* I2C */
extern uint8_t uctestIotI2CSlaveAddr;                   /* Address of Slave I2C (7-bit address) connected to the bus */
extern uint8_t uctestIotI2CDeviceRegister;              /* Slave I2C register address used in read/write tests */
extern uint8_t uctestIotI2CWriteVal;                    /* Write value that will be used in the register write test */
extern uint8_t uctestIotI2CInstanceIdx;                 /* I2C instance used in the test */
extern uint8_t uctestIotI2CInstanceNum;                 /* The total number of I2C instances on the device */
extern uint8_t ucAssistedTestIotI2CSlaveAddr;           /* The slave address to be set for the assisted test. */

/* I2S */
extern int32_t ltestIotI2sInputInstance;                /* used in assisted tests */
extern int32_t ltestIotI2sOutputInstance;               /* used in assisted tests */
extern int32_t ltestIotI2SReadSize;                     /* used in assisted tests */

/* EFUSE */
extern uint32_t ultestIotEfuse16BitWordValidIdx;        /* Efuse index for valid 16-bit R/W operations */
extern uint32_t ultestIotEfuse16BitWordInvalidIdx;      /* Efuse index that is not valid for 16-bit R/W operations */
extern uint32_t ustestIotEfuse16BitWordWriteVal;        /* 16-bit value to write to eFuse at index uctestIotEfuse16BitWordValidIdx */
extern uint32_t ultestIotEfuse32BitWordValidIdx;        /* Efuse index for valid 32-bit R/W operations */
extern uint32_t ultestIotEfuse32BitWordInvalidIdx;      /* Efuse index that is not valid for 32-bit R/W operations */
extern uint32_t ultestIotEfuse32BitWordWriteVal;        /* 32-bit value to write to eFuse at index uctestIotEfuse32BitWordValidIdx */

/* TSensor */
extern uint8_t uctestIotTsensorInstance;                /* I2C instance used in the test */

/* Power */
extern uint32_t ultestIotPowerDelay;                    /* Delay used to wait for the DUT enter idle */

extern uint32_t ultestIotPowerPcWakeThreshold;          /* Threshold (minimum idle Time required to enter PC Idle Mode */
extern uint32_t ultestIotPowerClkSrcOffThreshold;       /* Threshold (minimum idle Time required to enter ClkSrc Off Idle Mode */
extern uint32_t ultestIotPowerVddOffThreshold;          /* Threshold (minimum idle Time required to enter Vdd Off Idle Mode */

extern uint32_t ultestIotPowerInterruptConfig1;         /* ARM-only: NVIC interrupt mask to disable, for interrupts 0-31 */
extern uint32_t ultestIotPowerInterruptConfig2;         /* ARM-only: NVIC interrupt mask to disable, for interrupts 32-63 */

extern uint16_t ustestIotPowerWakeupSourcesLength;      /* Number of wakeup source listed in puctestIotPowerWakeupSources */
extern uint8_t * puctestIotPowerWakeupSources;          /* Array of wakeup sources, HW-dependent */

/* USB Device */
extern uint8_t uctestIotUsbDeviceControllerId;          /* USB Device controller ID used */


/**
 * Board specific UART config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_UART_CONFIG( int testSet );

/**
 * Board specific Flash config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_FLASH_CONFIG( int testSet );

/**
 * Board specific Watchdog config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_WATCHDOG_CONFIG( int testSet );

/**
 * Board specific RTC config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_RTC_CONFIG( int testSet );

/**
 * Board specific GPIO config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_GPIO_CONFIG( int testSet );

/**
 * Board specific Timer config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_TIMER_CONFIG( int testSet );

/**
 * Board specific ADC config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_ADC_CONFIG( int testSet );

/**
 * Board specific Reset config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_RESET_CONFIG( int testSet );

/**
 * Board specific Performance Counter config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_PERFCOUNTER_CONFIG( int testSet );

/**
 * Board specific PWM config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_PWM_CONFIG( int testSet );

/**
 * Board specific I2C config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_I2C_CONFIG( int testSet );

/**
 * Board specific SPI config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_SPI_CONFIG( int testSet );

/**
 * Board specific Power config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_POWER_CONFIG( int testSet );

/**
 * Board specific SDIO config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_SDIO_CONFIG( int testSet );

/**
 * Board specific Temp Sensor config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_TEMP_SENSOR_CONFIG( int testSet );

/**
 * Board specific efuse config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_EFUSE_CONFIG(int testSet);

/**
 * Board specific I2S config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_I2S_CONFIG( int testSet );

/**
 * Board specific USB Device config set
 *
 * @param: testSet: number of config set to be test
 * @return None
 */
void SET_TEST_IOT_USB_DEVICE_CONFIG(int testSet);
