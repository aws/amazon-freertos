/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       UART2CC32XX.h
 *
 *  @brief      UART driver implementation for a CC32XX UART controller
 *
 *  The UART header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/UART2.h>
 *  #include <ti/drivers/uart2/UART2CC32XX.h>
 *  @endcode
 *
 *  Refer to @ref UART2.h for a complete description of APIs and examples
 *  of use.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_uart2_UART2CC32XX__include
#define ti_drivers_uart2_UART2CC32XX__include

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/dma/UDMACC32XX.h>
#include <ti/drivers/Power.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Indicates a pin is not being used
 *
 *  If hardware flow control is not being used, the UART CTS and RTS
 *  pins should be set to UART2CC32XX_PIN_UNASSIGNED.
 */
#define UART2CC32XX_PIN_UNASSIGNED   0xFFF

/*!
 * @brief Indicates a DMA channel is not being used
 *
 *  If DMA is not being used, the UART rxDmaChannel and txDmaChannel
 *  should be set to UART2CC32XX_DMACH_UNASSIGNED.
 */
#define UART2CC32XX_DMACH_UNASSIGNED 0xFF

/*
 *  The bits in the pin mode macros are as follows:
 *  The lower 8 bits of the macro refer to the pin, offset by 1, to match
 *  driverlib pin defines.  For example, UART2CC32XX_PIN_01_UART1_TX & 0xff = 0,
 *  which equals PIN_01 in driverlib pin.h.  By matching the PIN_xx defines in
 *  driverlib pin.h, we can pass the pin directly to the driverlib functions.
 *  The upper 8 bits of the macro correspond to the pin mux confg mode
 *  value for the pin to operate in the UART mode.  For example, pin 1 is
 *  configured with mode 7 to operate as UART1 TX.
 */
#define UART2CC32XX_PIN_01_UART1_TX  0x700 /*!< PIN 1 is used for UART1 TX */
#define UART2CC32XX_PIN_02_UART1_RX  0x701 /*!< PIN 2 is used for UART1 RX */
#define UART2CC32XX_PIN_03_UART0_TX  0x702 /*!< PIN 3 is used for UART0 TX */
#define UART2CC32XX_PIN_04_UART0_RX  0x703 /*!< PIN 4 is used for UART0 RX */
#define UART2CC32XX_PIN_07_UART1_TX  0x506 /*!< PIN 7 is used for UART1 TX */
#define UART2CC32XX_PIN_08_UART1_RX  0x507 /*!< PIN 8 is used for UART1 RX */
#define UART2CC32XX_PIN_16_UART1_TX  0x20F /*!< PIN 16 is used for UART1 TX */
#define UART2CC32XX_PIN_17_UART1_RX  0x210 /*!< PIN 17 is used for UART1 RX */
#define UART2CC32XX_PIN_45_UART0_RX  0x92C /*!< PIN 45 is used for UART0 RX */
#define UART2CC32XX_PIN_45_UART1_RX  0x22C /*!< PIN 45 is used for UART1 RX */
#define UART2CC32XX_PIN_53_UART0_TX  0x934 /*!< PIN 53 is used for UART0 TX */
#define UART2CC32XX_PIN_55_UART0_TX  0x336 /*!< PIN 55 is used for UART0 TX */
#define UART2CC32XX_PIN_55_UART1_TX  0x636 /*!< PIN 55 is used for UART1 TX */
#define UART2CC32XX_PIN_57_UART0_RX  0x338 /*!< PIN 57 is used for UART0 RX */
#define UART2CC32XX_PIN_57_UART1_RX  0x638 /*!< PIN 57 is used for UART1 RX */
#define UART2CC32XX_PIN_58_UART1_TX  0x639 /*!< PIN 58 is used for UART1 TX */
#define UART2CC32XX_PIN_59_UART1_RX  0x63A /*!< PIN 59 is used for UART1 RX */
#define UART2CC32XX_PIN_62_UART0_TX  0xB3D /*!< PIN 62 is used for UART0 TX */

/*
 *  Flow control pins.
 */
#define UART2CC32XX_PIN_50_UART0_CTS  0xC31 /*!< PIN 50 is used for UART0 CTS */
#define UART2CC32XX_PIN_50_UART0_RTS  0x331 /*!< PIN 50 is used for UART0 RTS */
#define UART2CC32XX_PIN_50_UART1_RTS  0xA31 /*!< PIN 50 is used for UART1 RTS */
#define UART2CC32XX_PIN_52_UART0_RTS  0x633 /*!< PIN 52 is used for UART0 RTS */
#define UART2CC32XX_PIN_61_UART0_RTS  0x53C /*!< PIN 61 is used for UART0 RTS */
#define UART2CC32XX_PIN_61_UART0_CTS  0x63C /*!< PIN 61 is used for UART0 CTS */
#define UART2CC32XX_PIN_61_UART1_CTS  0x33C /*!< PIN 61 is used for UART1 CTS */
#define UART2CC32XX_PIN_62_UART0_RTS  0xA3D /*!< PIN 62 is used for UART0 RTS */
#define UART2CC32XX_PIN_62_UART1_RTS  0x33D /*!< PIN 62 is used for UART1 RTS */

/*!
 *  @brief    UART TX/RX interrupt FIFO threshold select
 *
 *  Defined FIFO thresholds for generation of both TX interrupt and RX
 *  interrupt.  If the RX and TX FIFO and thresholds are not set in the
 *  HwAttrs, the RX interrupt FIFO threshold is set to 1/8 full, and the
 *  TX interrupt FIFO threshold is set to 1/8 full.
 */
typedef enum {
    UART2CC32XX_FIFO_THRESHOLD_1_8 = 0,     /*!< FIFO threshold of 1/8 full */
    UART2CC32XX_FIFO_THRESHOLD_2_8 = 1,     /*!< FIFO threshold of 2/8 full */
    UART2CC32XX_FIFO_THRESHOLD_4_8 = 2,     /*!< FIFO threshold of 4/8 full */
    UART2CC32XX_FIFO_THRESHOLD_6_8 = 3,     /*!< FIFO threshold of 6/8 full */
    UART2CC32XX_FIFO_THRESHOLD_7_8 = 4      /*!< FIFO threshold of 7/8 full */
} UART2CC32XX_FifoThreshold;

/*!
 *  @brief      UART2CC32XX Hardware attributes
 *
 *  The fields, baseAddr and intNum are used by driverlib
 *  APIs and therefore must be populated by
 *  driverlib macro definitions. These definitions are found under the
 *  device family in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *      - driverlib/uart.h
 *
 *  intPriority is the UART peripheral's interrupt priority, as defined by the
 *  underlying OS.  It is passed unmodified to the underlying OS's interrupt
 *  handler creation code, so you need to refer to the OS documentation
 *  for usage.  For example, for SYS/BIOS applications, refer to the
 *  ti.sysbios.family.arm.m3.Hwi documentation for SYS/BIOS usage of
 *  interrupt priorities.  If the driver uses the ti.dpl interface
 *  instead of making OS calls directly, then the HwiP port handles the
 *  interrupt priority in an OS specific way.  In the case of the SYS/BIOS
 *  port, intPriority is passed unmodified to Hwi_create().
 *  The CC32XX uses three of the priority bits, meaning ~0 has the same
 *  effect as (7 << 5).
 *
 *        (7 << 5) will apply the lowest priority.
 *        (1 << 5) will apply the highest priority.
 *
 *  Setting the priority to 0 is not supported by this driver.  HWI's with
 *  priority 0 ignore the HWI dispatcher to support zero-latency interrupts,
 *  thus invalidating the critical sections in this driver.
 *
 *  A sample structure is shown below:
 *  @code
 *  const UART2CC32XX_HWAttrs UART2CC32XXHWAttrs[] = {
 *      {
 *          .baseAddr     = UARTA0_BASE,
 *          .intNum       = INT_UARTA0,
 *          .intPriority  = (~0),
 *          .flowControl  = UART2_FLOWCTRL_NONE,
 *          .rxPin        = UART2CC32XX_PIN_57_UART0_RX,
 *          .txPin        = UART2CC32XX_PIN_55_UART0_TX,
 *          .ctsPin       = UART2CC32XX_PIN_UNASSIGNED,
 *          .rtsPin       = UART2CC32XX_PIN_UNASSIGNED,
 *          .txIntFifoThr = UART2CC32XX_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr = UART2CC32XX_FIFO_THRESHOLD_4_8,
 *          .rxDmaChannel = UDMA_CH8_UARTA0_RX,
 *          .txDmaChannel = UDMA_CH9_UARTA0_TX,
 *      },
 *      {
 *          .baseAddr     = UARTA1_BASE,
 *          .intNum       = INT_UARTA1,
 *          .intPriority  = (~0),
 *          .flowControl  = UART2_FLOWCTRL_NONE,
 *          .rxPin        = UART2CC32XX_PIN_08_UART1_RX,
 *          .txPin        = UART2CC32XX_PIN_07_UART1_TX,
 *          .ctsPin       = UART2CC32XX_PIN_UNASSIGNED,
 *          .rtsPin       = UART2CC32XX_PIN_UNASSIGNED,
 *          .txIntFifoThr = UART2CC32XX_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr = UART2CC32XX_FIFO_THRESHOLD_4_8,
 *          .rxDmaChannel = UDMA_CH10_UARTA1_RX,
 *          .txDmaChannel = UDMA_CH11_UARTA1_TX,
 *      },
 *  };
 *  @endcode
 *
 *  To enable flow control, the .ctsPin and/or .rtsPin must be assigned.
 *  In addition, .flowControl must be set to UART2_FLOWCTRL_HARDWARE.
 */
typedef struct {
    UART2_BASE_HWATTRS

    /*! UART TX interrupt FIFO threshold select */
    UART2CC32XX_FifoThreshold txIntFifoThr;
    /*! UART RX interrupt FIFO threshold select */
    UART2CC32XX_FifoThreshold rxIntFifoThr;
    /*! uDMA channel for RX data */
    uint32_t  rxDmaChannel;
    /*! uDMA channel for TX data */
    uint32_t  txDmaChannel;
} UART2CC32XX_HWAttrs;

/*!
 *  @brief      UART2CC32XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    UART2_BASE_OBJECT

    UDMACC32XX_Handle    udmaHandle;       /* For setting power dependency */

    /* For Power management */
    Power_NotifyObj       postNotify;
    PowerCC32XX_ParkState prevParkTX;      /* Previous park state TX pin */
    uint16_t              txPin;           /* TX pin ID */
    PowerCC32XX_ParkState prevParkRTS;     /* Previous park state of RTS pin */
    uint16_t              rtsPin;          /* RTS pin ID */
} UART2CC32XX_Object, *UART2CC32XX_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_uart2_UART2CC32XX__include */
