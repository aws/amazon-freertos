/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       I2CCC32XX.h
 *
 *  @brief      I2C driver implementation for a CC32XX I2C controller.
 *
 *  The I2C header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/I2C.h>
 *  #include <ti/drivers/i2c/I2CCC32XX.h>
 *  @endcode
 *
 *  Refer to @ref I2C.h for a complete description of APIs and usage.
 *
 *  ## Supported Bit Rates ##
 *    - #I2C_100kHz
 *    - #I2C_400kHz
 *
 ******************************************************************************
 */

#ifndef ti_drivers_i2c_I2CCC32XX__include
#define ti_drivers_i2c_I2CCC32XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/I2C.h>
#include <ti/drivers/Power.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Macros defining possible I2C signal pin mux options
 *
 *  The bits in the pin mode macros are as follows:
 *  The lower 8 bits of the macro refer to the pin, offset by 1, to match
 *  driverlib pin defines.  For example, I2CCC32XX_PIN_01_I2C_SCL & 0xff = 0,
 *  which equals PIN_01 in driverlib pin.h.  By matching the PIN_xx defines in
 *  driverlib pin.h, we can pass the pin directly to the driverlib functions.
 *  The upper 8 bits of the macro correspond to the pin mux confg mode
 *  value for the pin to operate in the I2C mode.  For example, pin 1 is
 *  configured with mode 1 to operate as I2C_SCL.
 */
#define I2CCC32XX_PIN_01_I2C_SCL  0x100 /*!< PIN 1 is used for I2C_SCL */
#define I2CCC32XX_PIN_02_I2C_SDA  0x101 /*!< PIN 2 is used for I2C_SDA */
#define I2CCC32XX_PIN_03_I2C_SCL  0x502 /*!< PIN 3 is used for I2C_SCL */
#define I2CCC32XX_PIN_04_I2C_SDA  0x503 /*!< PIN 4 is used for I2C_SDA */
#define I2CCC32XX_PIN_05_I2C_SCL  0x504 /*!< PIN 5 is used for I2C_SCL */
#define I2CCC32XX_PIN_06_I2C_SDA  0x505 /*!< PIN 6 is used for I2C_SDA */
#define I2CCC32XX_PIN_16_I2C_SCL  0x90F /*!< PIN 16 is used for I2C_SCL */
#define I2CCC32XX_PIN_17_I2C_SDA  0x910 /*!< PIN 17 is used for I2C_SDA */

/**
 *  @addtogroup I2C_STATUS
 *  I2CCC32XX_STATUS_* macros are command codes only defined in the
 *  I2CCC32XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2c/I2CCC32XX.h>
 *  @endcode
 *  @{
 */

/* Add I2CCC32XX_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup I2C_CMD
 *  I2CCC32XX_CMD_* macros are command codes only defined in the
 *  I2CCC32XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/i2c/I2CCC32XX.h>
 *  @endcode
 *  @{
 */

/* Add I2CCC32XX_CMD_* macros here */

/** @}*/

/*!
 *  @brief  I2CCC32XX Hardware attributes
 *
 *  The baseAddr and intNum fields define the base address and interrupt number
 *  of the I2C peripheral.  These values are used by driverlib APIs and
 *  therefore must be populated by driverlib macro definitions. These
 *  definitions are found in the header files:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *
 *  intPriority is the I2C peripheral's interrupt priority.
 *  This driver uses the ti.dpl interface instead of making OS calls directly,
 *  and the corresponding HwiP port handles the interrupt priority in an
 *  OS-specific way.  For example, in the case of the TI-RTOS kernel port,
 *  the intPriority is passed unmodified to Hwi_create() provided by the
 *  ti.sysbios.family.arm.m3.Hwi module; so the documentation for the
 *  ti.sysbios.family.arm.m3.Hwi module should be referenced for a description
 *  of usage of priority.
 *
 *  clkPin and dataPin define the pin multiplexing to be used for the SCL and
 *  SDA pins, respectively.  Macro values defined in this header file should
 *  be used for these fields.
 *
 *  A sample structure is shown below:
 *  @code
 *  const I2CCC32XX_HWAttrsV1 i2cCC32XXHWAttrs[] = {
 *      {
 *          .baseAddr = I2CA0_BASE,
 *          .intNum = INT_I2CA0,
 *          .intPriority = (~0),
 *          .clkPin = I2CCC32XX_PIN_01_I2C_SCL,
 *          .dataPin = I2CCC32XX_PIN_02_I2C_SDA,
 *      }
 *  };
 *  @endcode
 */
typedef struct {
    I2C_BASE_HWATTRS

    /*! SCL low clock timeout specified in I2C clocks */
    uint32_t sclTimeout;
    /* I2C clock pin configuration */
    uint16_t clkPin;
    /* I2C data pin configuration */
    uint16_t dataPin;
} I2CCC32XX_HWAttrsV1;

/*!
 *  @brief      I2CCC32XX Object.
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    I2C_BASE_OBJECT

    /* Burst counters */
    uint16_t           burstCount;
    bool               burstStarted;

    /* Enumerated bit rate */
    I2C_BitRate        bitRate;

    /* For notification of wake from LPDS */
    Power_NotifyObj    notifyObj;
} I2CCC32XX_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2c_I2CCC32XX__include */
