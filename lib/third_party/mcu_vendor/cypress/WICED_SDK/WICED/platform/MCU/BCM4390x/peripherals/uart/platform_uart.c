/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
 * BCM43909 supports 3 UART ports as follows:
 * 1) Slow UART (ChipCommon offset 0x300-0x327), 2-wire (TX/RX), 16550-compatible.
 * 2) Fast UART (ChipCommon offset 0x130), 4-wire (TX/RX/RTS/CTS), SECI.
 * 3) GCI UART (GCI core offset 0x1D0), 2-wire (TX/RX), SECI.
 *
 * ChipCommon Fast UART has dedicated pins.
 * ChipCommon Slow UART and GCI UART share pins with RF_SW_CTRL_6-9.
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "typedefs.h"
#include "platform_constants.h"
#include "RTOS/wwd_rtos_interface.h"
#include "platform_peripheral.h"
#include "platform_map.h"
#include "wwd_rtos.h"
#include "wwd_assert.h"
#include "wiced_osl.h"
#include "wiced_utilities.h"
#include "hndsoc.h"
#ifdef CONS_LOG_BUFFER_SUPPORT
#include "platform_cache.h"
#endif  /* CONS_LOG_BUFFER_SUPPORT */
#include "platform_pinmux.h"
#include "wiced_low_power.h"
#include "wiced_power_logger.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* Some of the bits in slow UART status and control registers */
#define UART_SLOW_LSR_THRE   (0x20)    /* Transmit-hold-register empty */
#define UART_SLOW_LSR_TDHR   (0x40)    /* Data-hold-register empty */
#define UART_SLOW_LSR_RXRDY  (0x01)    /* Receiver ready */
#define UART_SLOW_LCR_DLAB   (0x80)    /* Divisor latch access bit */
#define UART_SLOW_LCR_WLEN8  (0x03)    /* Word length: 8 bits */

/* Some of the bits in slow UART Interrupt Enable Register */
#define UART_SLOW_IER_THRE   (0x02)     /* Transmit-hold-register empty */
#define UART_SLOW_IER_RXRDY  (0x01)     /* Receiver ready */

/* Some of the bits in slow UART Interrupt Identification Register */
#define UART_SLOW_IIR_INT_ID_MASK   (0xF)
#define UART_SLOW_IIR_INT_ID_THRE   (0x2)   /* Transmit-hold-register empty */
#define UART_SLOW_IIR_INT_ID_RXRDY  (0x4)   /* Receiver ready */

/* Some of the bits in slow UART Modem Control Register */
#define UART_SLOW_MCR_OUT2          (0x08)

/* Define maximum attempts to clean pending data kept from before driver initialized */
#define UART_SLOW_CLEAN_ATTEMPTS    (64)

/* FIFO Enable bit in slow UART FCR Register */
#define UART_SLOW_FCR_FIFO_ENABLE   (0x01)

/* Parity bits in slow UART LCR register */
#define UART_EPS_BIT    (4)
#define UART_PEN_BIT    (3)

#define UART_SLOW_REGBASE               (PLATFORM_CHIPCOMMON_REGBASE(0x300))
#define UART_FAST_REGBASE               (PLATFORM_CHIPCOMMON_REGBASE(0x1C0))
#define UART_GCI_REGBASE                (PLATFORM_GCI_REGBASE(0x1DC))

/* The below register defines can be moved to structure-pointer format after
 * ChipCommon and GCI structure layouts are fully defined in platform_appscr4.h */
#define CHIPCOMMON_CORE_CAP_REG         *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x004)))
#define CHIPCOMMON_CORE_CTRL_REG        *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x008)))
#define CHIPCOMMON_INT_STATUS_REG       *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x020)))
#define CHIPCOMMON_INT_MASK_REG         *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x024)))
#define CHIPCOMMON_CORE_CLK_DIV         *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x0A4)))
#define CHIPCOMMON_CORE_CAP_EXT_REG     *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x0AC)))
#define CHIPCOMMON_SECI_CONFIG_REG      *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x130)))
#define CHIPCOMMON_SECI_STATUS_REG      *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x134)))
#define CHIPCOMMON_SECI_STATUS_MASK_REG *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x138)))
#define CHIPCOMMON_SECI_FIFO_LEVEL_REG  *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x18C)))
#define CHIPCOMMON_SECI_UART_FCR_REG    *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x1C8)))
#define CHIPCOMMON_SECI_UART_LCR_REG    *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x1CC)))
#define CHIPCOMMON_SECI_UART_MCR_REG    *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x1D0)))
#define CHIPCOMMON_CLK_CTL_STATUS_REG   *((volatile uint32_t*)(PLATFORM_CHIPCOMMON_REGBASE(0x1E0)))

#define GCI_CORE_CTRL_REG                *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x00C)))
#define GCI_CORE_STATUS_REG              *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x010)))
#define GCI_INT_STATUS_REG               *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x014)))
#define GCI_INT_MASK_REG                 *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x018)))
#define GCI_INDIRECT_ADDRESS_REG         *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x040)))
#define GCI_GPIO_CTRL_REG                *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x044)))
#define GCI_GCI_RX_FIFO_PER_IP_CTRL_REG  *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x1C8)))
#define GCI_SECI_FIFO_LEVEL_REG          *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x1D8)))
#define GCI_SECI_IN_CTRL_REG             *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x218)))
#define GCI_SECI_OUT_CTRL_REG            *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x21C)))
#define GCI_SECI_IN_AUX_FIFO_RX_ENAB_REG *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x220)))
#define GCI_SECI_OUT_TX_ENAB_TX_BRK_REG  *((volatile uint32_t*)(PLATFORM_GCI_REGBASE(0x224)))

/* ChipCommon Capabilities Extension */
#define CC_CAP_EXT_SECI_PRESENT                  (0x00000001)    /* SECI present */
#define CC_CAP_EXT_SECI_PLAIN_UART_PRESENT       (0x00000008)    /* SECI Plain UART present */

/* ChipCommon ClockCtlStatus bits */
#define CC_CLK_CTL_ST_SECI_CLK_REQ                  (1 << 8)
#define CC_CLK_CTL_ST_SECI_CLK_AVAIL                (1 << 24)
#define CC_CLK_CTL_ST_BACKPLANE_ALP                 (1 << 18)
#define CC_CLK_CTL_ST_BACKPLANE_HT                  (1 << 19)

/* ChipCommon Slow UART IntStatus and IntMask register bit */
#define UART_SLOW_CC_INT_STATUS_MASK                (1 << 6)

/* ChipCommon FAST UART IntStatus and IntMask register bit */
#define UART_FAST_CC_INT_STATUS_MASK                (1 << 4)

/* ChipCommon GCI UART IntStatus and IntMask register bit */
#define UART_GCI_CC_INT_STATUS_MASK                 (1 << 4)

/* ChipCommon SECI Config register bits */
#define CC_SECI_CONFIG_HT_CLOCK                     (1 << 13)

/* ChipCommon SECI Status register bits */
#define CC_SECI_STATUS_RX_IDLE_TIMER_INT            (1 << 8)
#define CC_SECI_STATUS_TX_FIFO_FULL                 (1 << 9)
#define CC_SECI_STATUS_TX_FIFO_ALMOST_EMPTY         (1 << 10)
#define CC_SECI_STATUS_RX_FIFO_EMPTY                (1 << 11)
#define CC_SECI_STATUS_RX_FIFO_ALMOST_FULL          (1 << 12)

/* GCI CoreCtrl register bits */
#define GCI_CORE_CTRL_SECI_RESET                    (1 << 0)
#define GCI_CORE_CTRL_RESET_SECI_LOGIC              (1 << 1)
#define GCI_CORE_CTRL_ENABLE_SECI                   (1 << 2)
#define GCI_CORE_CTRL_FORCE_SECI_OUT_LOW            (1 << 3)
#define GCI_CORE_CTRL_UPDATE_SECI                   (1 << 7)
#define GCI_CORE_CTRL_BREAK_ON_SLEEP                (1 << 8)
#define GCI_CORE_CTRL_SECI_IN_LOW_TIMEOUT_SHIFT     (9)
#define GCI_CORE_CTRL_SECI_IN_LOW_TIMEOUT_MASK      (0x3)
#define GCI_CORE_CTRL_RESET_OFF_CHIP_COEX           (1 << 11)
#define GCI_CORE_CTRL_AUTO_BT_SIG_RESEND            (1 << 12)
#define GCI_CORE_CTRL_FORCE_GCI_CLK_REQ             (1 << 16)
#define GCI_CORE_CTRL_FORCE_HW_CLK_REQ_OFF          (1 << 17)
#define GCI_CORE_CTRL_FORCE_REG_CLK                 (1 << 18)
#define GCI_CORE_CTRL_FORCE_SECI_CLK                (1 << 19)
#define GCI_CORE_CTRL_FORCE_GCI_CLK_AVAIL           (1 << 20)
#define GCI_CORE_CTRL_FORCE_GCI_CLK_AVAIL_VALUE     (1 << 21)
#define GCI_CORE_CTRL_SECI_CLK_STRETCH_SHIFT        (24)
#define GCI_CORE_CTRL_SECI_CLK_STRETCH_MASK         (0xFF)

/* GCI CoreStatus register bits */
#define GCI_CORE_STATUS_BREAK_IN                    (1 << 0)
#define GCI_CORE_STATUS_BREAK_OUT                   (1 << 1)
#define GCI_CORE_STATUS_GCI_CLK_AVAIL               (1 << 16)
#define GCI_CORE_STATUS_GCI_CLK_AVAIL_PRE           (1 << 17)

/* GCI IntStatus and IntMask register bits */
#define GCI_INT_ST_MASK_RX_BREAK_EVENT_INT          (1 << 0)
#define GCI_INT_ST_MASK_UART_BREAK_INT              (1 << 1)
#define GCI_INT_ST_MASK_SECI_PARITY_ERROR           (1 << 2)
#define GCI_INT_ST_MASK_SECI_FRAMING_ERROR          (1 << 3)
#define GCI_INT_ST_MASK_SECI_DATA_UPDATED           (1 << 4)
#define GCI_INT_ST_MASK_SECI_AUX_DATA_UPDATED       (1 << 5)
#define GCI_INT_ST_MASK_SECI_TX_UPDATED_DONE        (1 << 6)
#define GCI_INT_ST_MASK_SECI_RX_IDLE_TIMER_INT      (1 << 9)
#define GCI_INT_ST_MASK_SECI_TX_FIFO_FULL           (1 << 10)
#define GCI_INT_ST_MASK_SECI_TX_FIFO_ALMOST_EMPTY   (1 << 11)
#define GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL    (1 << 12)
#define GCI_INT_ST_MASK_SECI_FLOW_CONTROL_EVENT     (1 << 13)
#define GCI_INT_ST_MASK_SECI_RX_FIFO_NOT_EMPTY      (1 << 14)
#define GCI_INT_ST_MASK_SECI_RX_FIFO_OVERFLOW       (1 << 15)
#define GCI_INT_ST_MASK_GCI_LEVEL_INT               (1 << 20)
#define GCI_INT_ST_MASK_GCI_EVENT_INT               (1 << 21)
#define GCI_INT_ST_MASK_GCI_WAKE_LEVEL_INT          (1 << 22)
#define GCI_INT_ST_MASK_GCI_WAKE_EVENT_INT          (1 << 23)
#define GCI_INT_ST_MASK_SEMAPHORE_INT               (1 << 24)
#define GCI_INT_ST_MASK_GCI_GPIO_INT                (1 << 25)
#define GCI_INT_ST_MASK_GCI_GPIO_WAKE               (1 << 26)
#define GCI_INT_ST_MASK_BATTERY_INT                 (1 << 27)

/* GCI GPIO Control register bits */
#define GCI_GPIO_CTRL_BITS_PER_GPIO                     (8)
#define GCI_GPIO_CTRL_REG_INDEX(gpio_num)               ((gpio_num * GCI_GPIO_CTRL_BITS_PER_GPIO) / 32)
#define GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num)             ((gpio_num * GCI_GPIO_CTRL_BITS_PER_GPIO) % 32)
#define GCI_GPIO_CTRL_GPIO_MASK(gpio_num)               (0xFF << GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num))
#define GCI_GPIO_CTRL_INPUT_ENAB(gpio_num)              (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 0))
#define GCI_GPIO_CTRL_OUTPUT_ENAB(gpio_num)             (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 1))
#define GCI_GPIO_CTRL_INVERT(gpio_num)                  (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 2))
#define GCI_GPIO_CTRL_PUP(gpio_num)                     (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 3))
#define GCI_GPIO_CTRL_PDN(gpio_num)                     (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 4))
#define GCI_GPIO_CTRL_BT_SIG_ENAB(gpio_num)             (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 5))
#define GCI_GPIO_CTRL_OPEN_DRAIN_OUTPUT_ENAB(gpio_num)  (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 6))
#define GCI_GPIO_CTRL_EXTRA_GPIO_ENAB(gpio_num)         (1 << (GCI_GPIO_CTRL_GPIO_OFFSET(gpio_num) + 7))

/* Default maximum wait times in ms for Slow UART TX and RX */
#define UART_SLOW_MAX_TRANSMIT_WAIT_TIME         (10)
#define UART_SLOW_MAX_READ_WAIT_TIME             (200)
#define UART_SLOW_CLKDIV_MASK                    (0x000000FF)

/* Default maximum wait times in ms for Fast UART TX and RX */
#define UART_FAST_MAX_TRANSMIT_WAIT_TIME         (10)
#define UART_FAST_MAX_READ_WAIT_TIME             (200)

/* Default maximum wait times in ms for GCI UART TX and RX */
#define UART_GCI_MAX_TRANSMIT_WAIT_TIME          (10)
#define UART_GCI_MAX_READ_WAIT_TIME              (200)

/* SECI configuration */
#define SECI_MODE_UART                           (0x0)
#define SECI_MODE_SECI                           (0x1)
#define SECI_MODE_LEGACY_3WIRE_BT                (0x2)
#define SECI_MODE_LEGACY_3WIRE_WLAN              (0x3)
#define SECI_MODE_HALF_SECI                      (0x4)

#define CC_SECI_RESET                            (1 << 0)
#define CC_SECI_RESET_BAR_UART                   (1 << 1)
#define CC_SECI_ENAB_SECI_ECI                    (1 << 2)
#define CC_SECI_ENAB_SECIOUT_DIS                 (1 << 3)
#define CC_SECI_MODE_MASK                        (0x7)
#define CC_SECI_MODE_SHIFT                       (4)
#define CC_SECI_UPD_SECI                         (1 << 7)

#define SECI_SLIP_ESC_CHAR                       (0xDB)
#define SECI_SIGNOFF_0                           SECI_SLIP_ESC_CHAR
#define SECI_SIGNOFF_1                           (0)
#define SECI_REFRESH_REQ                         (0xDA)

#define UART_SECI_MSR_CTS_STATE                  (1 << 0)
#define UART_SECI_MSR_RTS_STATE                  (1 << 1)
#define UART_SECI_SECI_IN_STATE                  (1 << 2)
#define UART_SECI_SECI_IN2_STATE                 (1 << 3)

/* ChipCommon SECI FIFO Level Register Offsets */
#define CC_SECI_RX_FIFO_LVL_MASK                 (0x3F)
#define CC_SECI_RX_FIFO_LVL_SHIFT                (0)
#define CC_SECI_TX_FIFO_LVL_MASK                 (0x3F)
#define CC_SECI_TX_FIFO_LVL_SHIFT                (8)
#define CC_SECI_RX_FIFO_LVL_FLOW_CTL_MASK        (0x3F)
#define CC_SECI_RX_FIFO_LVL_FLOW_CTL_SHIFT       (16)

/* GCI SECI RX FIFO Level Register Offsets */
#define GCI_SECI_RX_FIFO_LVL_MASK                (0x3F)
#define GCI_SECI_RX_FIFO_LVL_SHIFT               (0)

/* GCI SECI TX FIFO Level Register Offsets */
#define GCI_SECI_TX_FIFO_LVL_MASK                (0x3F)
#define GCI_SECI_TX_FIFO_LVL_SHIFT               (8)

/* GCI UART SECI_IN port operating mode */
#define GCI_SECI_IN_OP_MODE_MASK                 (0x7)
#define GCI_SECI_IN_OP_MODE_SHIFT                (0)

/* GCI UART SECI_OUT port operating mode */
#define GCI_SECI_OUT_OP_MODE_MASK                (0x7)
#define GCI_SECI_OUT_OP_MODE_SHIFT               (0)

/* GCI UART SECI_IN GCI_GPIO mapping */
#define GCI_SECI_IN_GPIO_NUM_MASK                (0xF)
#define GCI_SECI_IN_GPIO_NUM_SHIFT               (4)

/* GCI UART SECI_OUT GCI_GPIO mapping */
#define GCI_SECI_OUT_GPIO_NUM_MASK               (0xF)
#define GCI_SECI_OUT_GPIO_NUM_SHIFT              (4)

/* Default GCI_GPIO mappings for SECI_IN and SECI_OUT */
#define GCI_SECI_IN_GPIO_NUM_DEFAULT             (6)
#define GCI_SECI_OUT_GPIO_NUM_DEFAULT            (7)

/* GCI UART SECI RX FIFO Enable bit */
#define GCI_SECI_FIFO_RX_ENAB                    (1 << 16)

/* GCI UART SECI TX Enable bit */
#define GCI_SECI_TX_ENAB                         (1 << 0)

/* Default FIFO levels for ChipCommon SECI RX, TX and host flow control */
#define CC_SECI_RX_FIFO_LVL_DEFAULT                 (0x20)
#define CC_SECI_TX_FIFO_LVL_DEFAULT                 (0x20)
#define CC_SECI_RX_FIFO_LVL_FLOW_CTL_DEFAULT        (0x20)

/* Default FIFO levels for GCI SECI RX and TX */
#define GCI_SECI_RX_FIFO_LVL_DEFAULT                (0x8)
#define GCI_SECI_TX_FIFO_LVL_DEFAULT                (0x8)

/* SECI UART FCR bit definitions */
#define UART_SECI_FCR_RFR                        (1 << 0)
#define UART_SECI_FCR_TFR                        (1 << 1)
#define UART_SECI_FCR_SR                         (1 << 2)
#define UART_SECI_FCR_THP                        (1 << 3)
#define UART_SECI_FCR_AB                         (1 << 4)
#define UART_SECI_FCR_ATOE                       (1 << 5)
#define UART_SECI_FCR_ARTSOE                     (1 << 6)
#define UART_SECI_FCR_ABV                        (1 << 7)
#define UART_SECI_FCR_ALM                        (1 << 8)

/* SECI UART LCR bit definitions */
#define UART_SECI_LCR_STOP_BITS                  (1 << 0) /* 0 - 1bit, 1 - 2bits */
#define UART_SECI_LCR_PARITY_EN                  (1 << 1)
#define UART_SECI_LCR_PARITY                     (1 << 2) /* 0 - odd, 1 - even */
#define UART_SECI_LCR_RX_EN                      (1 << 3)
#define UART_SECI_LCR_LBRK_CTRL                  (1 << 4) /* 1 => SECI_OUT held low */
#define UART_SECI_LCR_TXO_EN                     (1 << 5)
#define UART_SECI_LCR_RTSO_EN                    (1 << 6)
#define UART_SECI_LCR_SLIPMODE_EN                (1 << 7)
#define UART_SECI_LCR_RXCRC_CHK                  (1 << 8)
#define UART_SECI_LCR_TXCRC_INV                  (1 << 9)
#define UART_SECI_LCR_TXCRC_LSBF                 (1 << 10)
#define UART_SECI_LCR_TXCRC_EN                   (1 << 11)
#define UART_SECI_LCR_RXSYNC_EN                  (1 << 12)

/* SECI UART MCR bit definitions */
#define UART_SECI_MCR_TX_EN                      (1 << 0)
#define UART_SECI_MCR_PRTS                       (1 << 1)
#define UART_SECI_MCR_SWFLCTRL_EN                (1 << 2)
#define UART_SECI_MCR_HIGHRATE_EN                (1 << 3)
#define UART_SECI_MCR_LOOPBK_EN                  (1 << 4)
#define UART_SECI_MCR_AUTO_RTS                   (1 << 5)
#define UART_SECI_MCR_AUTO_TX_DIS                (1 << 6)
#define UART_SECI_MCR_BAUD_ADJ_EN                (1 << 7)
#define UART_SECI_MCR_XONOFF_RPT                 (1 << 9)

/* SECI UART LSR bit definitions */
#define UART_SECI_LSR_RXOVR_MASK                 (1 << 0)
#define UART_SECI_LSR_RFF_MASK                   (1 << 1)
#define UART_SECI_LSR_TFNE_MASK                  (1 << 2)
#define UART_SECI_LSR_TI_MASK                    (1 << 3)
#define UART_SECI_LSR_TPR_MASK                   (1 << 4)
#define UART_SECI_LSR_TXHALT_MASK                (1 << 5)

/* SECI UART MSR bit definitions */
#define UART_SECI_MSR_CTSS_MASK                  (1 << 0)
#define UART_SECI_MSR_RTSS_MASK                  (1 << 1)
#define UART_SECI_MSR_SIS_MASK                   (1 << 2)
#define UART_SECI_MSR_SIS2_MASK                  (1 << 3)

/* SECI UART Data bit definitions */
#define UART_SECI_DATA_RF_NOT_EMPTY_BIT          (1 << 12)
#define UART_SECI_DATA_RF_FULL_BIT               (1 << 13)
#define UART_SECI_DATA_RF_OVRFLOW_BIT            (1 << 14)
#define UART_SECI_DATA_FIFO_PTR_MASK             (0xFF)
#define UART_SECI_DATA_RF_RD_PTR_SHIFT           (16)
#define UART_SECI_DATA_RF_WR_PTR_SHIFT           (24)

/* Range for High rate SeciUARTBaudRateDivisor is 0xF1 - 0xF8 */
#define UART_SECI_HIGH_RATE_THRESHOLD_LOW        (0xF1)
#define UART_SECI_HIGH_RATE_THRESHOLD_HIGH       (0xF8)

#define UART_SECI_BAUD_RATE_THRESHOLD_LOW        (9600)
#define UART_SECI_ALP_CLOCK_DEFAULT              (37400000)
#define UART_SECI_BACKPLANE_CLOCK_DEFAULT        (160000000)
#define UART_SECI_BAUD_RATE_DIVISOR_MAX          (255)
#define UART_SECI_BAUD_RATE_DIVISOR_RANGE        (UART_SECI_BAUD_RATE_DIVISOR_MAX + 1)
#define UART_SECI_BAUD_RATE_ADJUSTMENT_MAX       (15)
#define UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE     (UART_SECI_BAUD_RATE_ADJUSTMENT_MAX + 1)

/*
* Maximum delay for the PMU state transition in us.
* This is an upper bound intended for spinwaits etc.
*/
#define PMU_MAX_TRANSITION_DLY  15000

/* Slow UART RX ring buffer size */
#define UART_SLOW_RX_BUFFER_SIZE  64

/* Fast UART RX ring buffer size */
#define UART_FAST_RX_BUFFER_SIZE  128

/* GCI UART RX ring buffer size */
#define UART_GCI_RX_BUFFER_SIZE  128

#ifdef CONS_LOG_BUFFER_SUPPORT
#define CONS_LOG_BUFFER_SIZE      (16*1024)
#endif
/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    volatile uint8_t rx_tx_dll;
    volatile uint8_t ier_dlm;
    volatile uint8_t iir_fcr;
    volatile uint8_t lcr;
    volatile uint8_t mcr;
    volatile uint8_t lsr;
    volatile uint8_t msr;
    volatile uint8_t scr;
} uart_slow_t;

typedef struct
{
    volatile uint32_t data;
    volatile uint32_t bauddiv;
    volatile uint32_t fcr;
    volatile uint32_t lcr;
    volatile uint32_t mcr;
    volatile uint32_t lsr;
    volatile uint32_t msr;
    volatile uint32_t baudadj;
} uart_seci_t;

typedef struct
{
    uint32_t clock;                 /* Clock value in HZ */
    uint32_t desired_baud_rate;     /* Desired baud-rate in BPS */
    uint32_t target_baud_rate;      /* Target baud-rate in BPS */
} uart_seci_baud_rate_config_t;

#ifdef CONS_LOG_BUFFER_SUPPORT
typedef struct
{
    uint8_t *buf;
    uint buf_size;
    uint idx;
    uint out_idx;
} hndrte_log_t;

typedef struct
{
    volatile uint vcons_in;
    volatile uint vcons_out;
    hndrte_log_t log;
    uint reserved[2];
} hndrte_cons_t;
#endif  /* CONS_LOG_BUFFER_SUPPORT */

/******************************************************
 *               Function Declarations
 ******************************************************/

static wiced_bool_t uart_slow_transmit_fifo_empty( void );
static wiced_bool_t uart_fast_transmit_fifo_empty( void );

/******************************************************
 *               Variables Definitions
 ******************************************************/

/*
 * Slow and Fast UART reside in ChipCommon core,
 * GCI UART resides in GCI core (Always On domain),
 * and accessed from Apps core via AXI backplane.
 */
uart_slow_t* const uart_slow_base = ( uart_slow_t* )( UART_SLOW_REGBASE );
uart_seci_t* const uart_fast_base = ( uart_seci_t* )( UART_FAST_REGBASE );
uart_seci_t* const uart_gci_base  = ( uart_seci_t* )( UART_GCI_REGBASE );

/* Slow UART RX ring buffer */
wiced_ring_buffer_t uart_slow_rx_buffer;
uint8_t             uart_slow_rx_data[UART_SLOW_RX_BUFFER_SIZE];

/* Fast UART RX ring buffer */
wiced_ring_buffer_t uart_fast_rx_buffer;
uint8_t             uart_fast_rx_data[UART_FAST_RX_BUFFER_SIZE];

/* GCI UART RX ring buffer */
wiced_ring_buffer_t uart_gci_rx_buffer;
uint8_t             uart_gci_rx_data[UART_GCI_RX_BUFFER_SIZE];

/* SECI UART baud-rate mapping table for fast lookup of target baud-rate */
static const uart_seci_baud_rate_config_t uart_seci_baud_rate_mapping[] =
{
    {UART_SECI_ALP_CLOCK_DEFAULT,       9600,    9599},
    {UART_SECI_ALP_CLOCK_DEFAULT,       14400,   14401},
    {UART_SECI_ALP_CLOCK_DEFAULT,       19200,   19199},
    {UART_SECI_ALP_CLOCK_DEFAULT,       38400,   38398},
    {UART_SECI_ALP_CLOCK_DEFAULT,       57600,   57627},
    {UART_SECI_ALP_CLOCK_DEFAULT,       115200,  115076},
    {UART_SECI_ALP_CLOCK_DEFAULT,       230400,  230864},
    {UART_SECI_ALP_CLOCK_DEFAULT,       460800,  461728},
    {UART_SECI_ALP_CLOCK_DEFAULT,       921600,  912195},
    {UART_SECI_ALP_CLOCK_DEFAULT,       1000000, 1010810},
    {UART_SECI_ALP_CLOCK_DEFAULT,       1500000, 1496000},
    {UART_SECI_ALP_CLOCK_DEFAULT,       2000000, 1968421},
    {UART_SECI_ALP_CLOCK_DEFAULT,       2500000, 2493333},
    {UART_SECI_ALP_CLOCK_DEFAULT,       3000000, 3116666},
    {UART_SECI_ALP_CLOCK_DEFAULT,       3500000, 3400000},
    {UART_SECI_ALP_CLOCK_DEFAULT,       4000000, 4155555},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 57600,   57595},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 115200,  115190},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 230400,  230547},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 460800,  461095},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 921600,  919540},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 1000000, 1000000},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 1500000, 1495327},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 2000000, 2000000},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 2500000, 2500000},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 3000000, 3018867},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 3500000, 3478260},
    {UART_SECI_BACKPLANE_CLOCK_DEFAULT, 4000000, 4000000}
};

#ifdef CONS_LOG_BUFFER_SUPPORT
#define CONS_BUFFER_SIZE        (sizeof(hndrte_cons_t) + CONS_LOG_BUFFER_SIZE)
static uint8_t             cons_buffer[sizeof(hndrte_cons_t) + CONS_LOG_BUFFER_SIZE + 2*PLATFORM_L1_CACHE_BYTES];
static hndrte_cons_t       *cons0;
#endif  /*  CONS_LOG_BUFFER_SUPPORT  */

/******************************************************
 *               Function Definitions
 ******************************************************/

#ifdef CONS_LOG_BUFFER_SUPPORT
static platform_result_t cons_init(void)
{
    uint8* buf_startp;

    buf_startp = (uint8 *) (((uint)&cons_buffer[0] + PLATFORM_L1_CACHE_BYTES-1) & ~(PLATFORM_L1_CACHE_BYTES-1));

    /* prevent any write backs */
    platform_dcache_inv_range(buf_startp, CONS_BUFFER_SIZE);
    cons0 = (hndrte_cons_t *) platform_addr_cached_to_uncached(buf_startp);

    cons0->vcons_in = 0;
    cons0->vcons_out = 0;
    cons0->log.buf = (uint8 *) (cons0 + 1);
    cons0->log.buf_size = CONS_LOG_BUFFER_SIZE;
    cons0->log.idx = 0;
    cons0->log.out_idx = 0;

    return PLATFORM_SUCCESS;
}

platform_result_t cons_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    uint remain_len;

    if (size > cons0->log.buf_size)
    {
        /*  Message is larger than the log buffer. Only hold the last part  */
        data_out += size - cons0->log.buf_size;
        size = cons0->log.buf_size;
    }

    remain_len = cons0->log.buf_size - cons0->log.idx;
    if (size > remain_len)
    {
        memcpy(&cons0->log.buf[cons0->log.idx], data_out, remain_len);
        cons0->log.idx = 0;
        data_out += remain_len;
        size -= remain_len;
    }

    memcpy(&cons0->log.buf[cons0->log.idx], data_out, size);
    cons0->log.idx += size;
    if (cons0->log.idx >= cons0->log.buf_size)
        cons0->log.idx = 0;
    cons0->log.out_idx = cons0->log.idx;

    return PLATFORM_SUCCESS;
}

#endif /* CONS_LOG_BUFFER_SUPPORT */

static void uart_chipcommon_interrupt_mask( uint32_t clear_mask, uint32_t set_mask )
{
    uint32_t cc_int_mask;
    uint32_t flags;

    WICED_SAVE_INTERRUPTS(flags);
    cc_int_mask = PLATFORM_CHIPCOMMON->interrupt.mask.raw;

    cc_int_mask = (cc_int_mask & ~clear_mask) | set_mask;

    PLATFORM_CHIPCOMMON->interrupt.mask.raw = cc_int_mask;
    WICED_RESTORE_INTERRUPTS(flags);
}

static void uart_slow_disable_interrupts( void )
{
    /* Disable slow UART interrupts for RX data */
    uart_slow_base->mcr &= ~UART_SLOW_MCR_OUT2;

    /* Disable slow UART interrupts */
    uart_slow_base->ier_dlm = 0x0;

    /* Disable slow UART interrupt in ChipCommon interrupt mask */
    uart_chipcommon_interrupt_mask(UART_SLOW_CC_INT_STATUS_MASK, 0x0);
}

static void uart_fast_disable_interrupts( void )
{
    /* Disable fast UART interrupts */
    CHIPCOMMON_SECI_STATUS_MASK_REG = 0x0;
}

static void uart_gci_disable_interrupts( void )
{
    /* Disable GCI UART interrupts */
    GCI_INT_MASK_REG = 0x0;
}

static void platform_uart_disable_interrupts( platform_uart_port_t uart_port )
{
    if (uart_port == UART_SLOW)
    {
        uart_slow_disable_interrupts();
    }
    else if (uart_port == UART_FAST)
    {
        uart_fast_disable_interrupts();
    }
    else if (uart_port == UART_GCI)
    {
        uart_gci_disable_interrupts();
    }
}

#if !defined(BCM4390X_UART_SLOW_POLL_MODE) || !defined(BCM4390X_UART_FAST_POLL_MODE) || !defined(BCM4390X_UART_GCI_POLL_MODE)
static void uart_slow_enable_interrupts( void )
{
#ifndef BCM4390X_UART_SLOW_POLL_MODE
    /* Enable slow UART interrupt in ChipCommon interrupt mask */
    uart_chipcommon_interrupt_mask(0x0, UART_SLOW_CC_INT_STATUS_MASK);

    /* Enable RX Data Available interrupt in slow UART interrupt mask */
    uart_slow_base->ier_dlm = UART_SLOW_IER_RXRDY;

    /* Enable slow UART interrupts for RX data */
    uart_slow_base->mcr |= UART_SLOW_MCR_OUT2;
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */
}

/* Enable/Disable tx/rx interrupts on Slow Uart */
static void uart_slow_toggle_txrx_interrupt( wiced_bool_t tx, wiced_bool_t enable )
{
#ifndef BCM4390X_UART_SLOW_POLL_MODE
    uint8_t interrupt_mask_bits = tx ? UART_SLOW_IER_THRE : UART_SLOW_IER_RXRDY;

    if ( enable == WICED_TRUE )
    {
        uart_slow_base->ier_dlm |= interrupt_mask_bits;
    }
    else
    {
        uart_slow_base->ier_dlm &= ~interrupt_mask_bits;
    }
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */
}

static void uart_fast_enable_interrupts( void )
{
#ifndef BCM4390X_UART_FAST_POLL_MODE
    /* Enable fast UART interrupt in ChipCommon interrupt mask */
    uart_chipcommon_interrupt_mask(0x0, UART_FAST_CC_INT_STATUS_MASK);

    /* Enable SRFAF (SECI RX FIFO Almost Full) Interrupt and SRITI
     * (SECI RX Idle Timer Interrupt) in fast UART interrupt mask */
    CHIPCOMMON_SECI_STATUS_MASK_REG = CC_SECI_STATUS_RX_FIFO_ALMOST_FULL | CC_SECI_STATUS_RX_IDLE_TIMER_INT;
#endif /* !BCM4390X_UART_FAST_POLL_MODE */
}

static void uart_fast_toggle_txrx_interrupt( wiced_bool_t tx, wiced_bool_t enable )
{
#ifndef BCM4390X_UART_FAST_POLL_MODE
    uint32_t interrupt_mask_bits = tx ? CC_SECI_STATUS_TX_FIFO_ALMOST_EMPTY : (CC_SECI_STATUS_RX_FIFO_ALMOST_FULL | CC_SECI_STATUS_RX_IDLE_TIMER_INT);

    if ( enable == WICED_TRUE )
    {
        CHIPCOMMON_SECI_STATUS_MASK_REG |= interrupt_mask_bits;
    }
    else
    {
        CHIPCOMMON_SECI_STATUS_MASK_REG &= ~interrupt_mask_bits;
    }
#endif /* !BCM4390X_UART_FAST_POLL_MODE */
}

static void uart_gci_enable_interrupts( void )
{
#ifndef BCM4390X_UART_GCI_POLL_MODE
    /* Enable GCI UART interrupt in ChipCommon interrupt mask */
    uart_chipcommon_interrupt_mask(0x0, UART_GCI_CC_INT_STATUS_MASK);

    /* Enable SRFAF (SECI RX FIFO Almost Full) Interrupt and SRITI
     * (SECI RX Idle Timer Interrupt) in GCI UART interrupt mask */
    GCI_INT_MASK_REG = GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL | GCI_INT_ST_MASK_SECI_RX_IDLE_TIMER_INT;
#endif /* !BCM4390X_UART_GCI_POLL_MODE */
}

static void uart_gci_toggle_txrx_interrupt( wiced_bool_t tx, wiced_bool_t enable )
{
#ifndef BCM4390X_UART_GCI_POLL_MODE
    uint32_t interrupt_mask_bits = tx ? GCI_INT_ST_MASK_SECI_TX_FIFO_ALMOST_EMPTY : (GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL | GCI_INT_ST_MASK_SECI_RX_IDLE_TIMER_INT);

    if ( enable == WICED_TRUE )
    {
        GCI_INT_MASK_REG |= interrupt_mask_bits;
    }
    else
    {
        GCI_INT_MASK_REG &= ~interrupt_mask_bits;
    }
#endif /* !BCM4390X_UART_GCI_POLL_MODE */
}

static void platform_uart_enable_interrupts( platform_uart_port_t uart_port )
{
    if (uart_port == UART_SLOW)
    {
        uart_slow_enable_interrupts();
    }
    else if (uart_port == UART_FAST)
    {
        uart_fast_enable_interrupts();
    }
    else if (uart_port == UART_GCI)
    {
        uart_gci_enable_interrupts();
    }
}

void platform_uart_toggle_txrx_interrupt( platform_uart_port_t uart_port, wiced_bool_t tx, wiced_bool_t enable )
{
    if ( uart_port == UART_SLOW )
    {
        uart_slow_toggle_txrx_interrupt( tx, enable );
    }
    else if ( uart_port == UART_FAST )
    {
        uart_fast_toggle_txrx_interrupt( tx, enable );
    }
    else if ( uart_port == UART_GCI )
    {
        uart_gci_toggle_txrx_interrupt( tx, enable );
    }
}
#endif /* !BCM4390X_UART_SLOW_POLL_MODE || !BCM4390X_UART_FAST_POLL_MODE || !BCM4390X_UART_GCI_POLL_MODE */

/*
 * This algorithm converges on the optimal target baud-rate that will
 * achieve the least rate error with respect to the desired baud-rate.
 */
static uint32_t uart_seci_target_baud_rate( uint32_t clock, const platform_uart_config_t* config )
{
    int desired_baud_rate = config->baud_rate;
    int baud_rate_divisor = 0;
    int baud_rate_adjustment = 0;
    int optimal_baud_rate = 0;
    int target_baud_rate_lrm = 0;    /* lrm - low rate mode */
    int optimal_baud_rate_lrm = 0;
    int target_baud_rate_hrm = 0;    /* hrm - high rate mode */
    int optimal_baud_rate_hrm = 0;

    /* Fast lookup of the desired baud-rate in the baud-rate mapping table */
    for (int i = 0 ; i < ARRAY_SIZE(uart_seci_baud_rate_mapping) ; i++)
    {
        if ((uart_seci_baud_rate_mapping[i].clock == clock) && (uart_seci_baud_rate_mapping[i].desired_baud_rate == desired_baud_rate))
        {
            return uart_seci_baud_rate_mapping[i].target_baud_rate;
        }
    }

    for (baud_rate_divisor = 0 ; baud_rate_divisor <= UART_SECI_BAUD_RATE_DIVISOR_MAX ; baud_rate_divisor++)
    {
        if ((baud_rate_divisor >= UART_SECI_HIGH_RATE_THRESHOLD_LOW) && (baud_rate_divisor <= UART_SECI_HIGH_RATE_THRESHOLD_HIGH))
        {
            /* Compute optimal target baud-rate in high rate mode for this divisor value */
            target_baud_rate_hrm = (int)clock / (UART_SECI_BAUD_RATE_DIVISOR_RANGE - baud_rate_divisor);

            /* Update the optimal target baud-rate achievable in high rate mode */
            if (optimal_baud_rate_hrm == 0)
            {
                optimal_baud_rate_hrm = target_baud_rate_hrm;
            }
            else if (abs(desired_baud_rate - target_baud_rate_hrm) < abs(desired_baud_rate - optimal_baud_rate_hrm))
            {
                optimal_baud_rate_hrm = target_baud_rate_hrm;
            }
        }

        /* Compute optimal target baud-rate in low rate mode for this divisor value */
        for (baud_rate_adjustment = UART_SECI_BAUD_RATE_ADJUSTMENT_MAX ; baud_rate_adjustment >= 0 ; baud_rate_adjustment--)
        {
            target_baud_rate_lrm = (int)clock / (((UART_SECI_BAUD_RATE_DIVISOR_RANGE - baud_rate_divisor) * UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE) + baud_rate_adjustment);

            /* Update the optimal target baud-rate achievable in low rate mode */
            if (optimal_baud_rate_lrm == 0)
            {
                optimal_baud_rate_lrm = target_baud_rate_lrm;
            }
            else if (abs(desired_baud_rate - target_baud_rate_lrm) < abs(desired_baud_rate - optimal_baud_rate_lrm))
            {
                optimal_baud_rate_lrm = target_baud_rate_lrm;
            }
        }
    }

    if (abs(desired_baud_rate - optimal_baud_rate_lrm) < abs(desired_baud_rate - optimal_baud_rate_hrm))
    {
        optimal_baud_rate = optimal_baud_rate_lrm;
    }
    else
    {
        optimal_baud_rate = optimal_baud_rate_hrm;
    }

    return (uint32_t)optimal_baud_rate;
}

static platform_result_t uart_seci_check_config( uint32_t clock, const platform_uart_config_t* config )
{
    int baud_rate_div_factor = 0;

    if ( config->baud_rate < UART_SECI_BAUD_RATE_THRESHOLD_LOW )
    {
        return PLATFORM_ERROR;
    }

    /* Check if the desired baud-rate and clock are configurable */
    baud_rate_div_factor = UART_SECI_BAUD_RATE_DIVISOR_RANGE - (int)(clock/(UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE * config->baud_rate));

    if ( baud_rate_div_factor < 0 )
    {
        return PLATFORM_ERROR;
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t uart_seci_setup_internal( uart_seci_t* uart_seci_base, uint32_t clock, const platform_uart_config_t* config )
{
    uint32_t uart_seci_baud_rate      = 0;
    uint32_t baud_rate_div_high_rate  = 0;
    uint32_t baud_rate_div_low_rate   = 0;
    uint32_t baud_rate_temp_value     = 0;
    uint32_t baud_rate_adjustment     = 0;
    uint32_t baud_rate_adjust_low     = 0;
    uint32_t baud_rate_adjust_high    = 0;
    uint32_t uart_high_rate_mode      = 0;

    /* init code should not cause clock to be 0 if all is working */
    wiced_assert( "Clock is zero\n", clock != 0 );

    /* Setup SECI UART baud rate */
    uart_seci_baud_rate = uart_seci_target_baud_rate( clock, config );

    /* from inspection, the function above can never return a zero baud rate unless clock is 0.  Catch it here if it does */
    if ( 0 == uart_seci_baud_rate )
    {
        WPRINT_PLATFORM_ERROR(("Unexpected zero baud rate for seci; clock is %lu\n", clock));
        wiced_assert( "uart_seci_baud_rate is zero\n", uart_seci_baud_rate != 0 );
        return PLATFORM_ERROR;
    }

    baud_rate_temp_value = clock/uart_seci_baud_rate;
    if ( baud_rate_temp_value > UART_SECI_BAUD_RATE_DIVISOR_RANGE )
    {
        uart_high_rate_mode = 0;
    }
    else
    {
        baud_rate_div_high_rate = UART_SECI_BAUD_RATE_DIVISOR_RANGE - baud_rate_temp_value;
        if ( ( baud_rate_div_high_rate < UART_SECI_HIGH_RATE_THRESHOLD_LOW ) || ( baud_rate_div_high_rate > UART_SECI_HIGH_RATE_THRESHOLD_HIGH ) )
        {
            uart_high_rate_mode = 0;
        }
        else
        {
            uart_high_rate_mode = 1;
        }
    }

    if ( uart_high_rate_mode == 1 )
    {
        /* Setup in high rate mode, disable baudrate adjustment */
        baud_rate_div_high_rate  = UART_SECI_BAUD_RATE_DIVISOR_RANGE - (clock/uart_seci_baud_rate);
        baud_rate_adjustment = 0x0;

        uart_seci_base->bauddiv = baud_rate_div_high_rate;
        uart_seci_base->baudadj = baud_rate_adjustment;
        uart_seci_base->mcr |= UART_SECI_MCR_HIGHRATE_EN;
        uart_seci_base->mcr &= ~(UART_SECI_MCR_BAUD_ADJ_EN);
    }
    else
    {
        /* Setup in low rate mode, enable baudrate adjustment */
        baud_rate_div_low_rate  = UART_SECI_BAUD_RATE_DIVISOR_RANGE - (clock/(UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE * uart_seci_baud_rate));
        baud_rate_adjust_low  = ((clock/uart_seci_baud_rate) % UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE) / 2;
        baud_rate_adjust_high = ((clock/uart_seci_baud_rate) % UART_SECI_BAUD_RATE_ADJUSTMENT_RANGE) - baud_rate_adjust_low;
        baud_rate_adjustment  = (baud_rate_adjust_high << 4) | baud_rate_adjust_low;

        uart_seci_base->bauddiv = baud_rate_div_low_rate;
        uart_seci_base->baudadj = baud_rate_adjustment;
        uart_seci_base->mcr &= ~(UART_SECI_MCR_HIGHRATE_EN);
        uart_seci_base->mcr |= UART_SECI_MCR_BAUD_ADJ_EN;
    }

    wiced_assert(" Currently supported data width is 8bits ", config->data_width == DATA_WIDTH_8BIT );

    /* Configure parity */
    if ( config->parity == ODD_PARITY )
    {
        uart_seci_base->lcr |= UART_SECI_LCR_PARITY_EN;
        uart_seci_base->lcr &= ~(UART_SECI_LCR_PARITY);
    }
    else if ( config->parity == EVEN_PARITY )
    {
        uart_seci_base->lcr |= UART_SECI_LCR_PARITY_EN;
        uart_seci_base->lcr |= UART_SECI_LCR_PARITY;
    }
    else
    {
        /* Default NO_PARITY */
        uart_seci_base->lcr &= ~(UART_SECI_LCR_PARITY_EN);
    }

    /* Configure stop bits */
    if ( config->stop_bits == STOP_BITS_1 )
    {
        uart_seci_base->lcr &= ~(UART_SECI_LCR_STOP_BITS);
    }
    else if ( config->stop_bits == STOP_BITS_2 )
    {
        uart_seci_base->lcr |= UART_SECI_LCR_STOP_BITS;
    }
    else
    {
        /* Default STOP_BITS_1 */
        uart_seci_base->lcr &= ~(UART_SECI_LCR_STOP_BITS);
    }

    /* Configure Flow Control */
    if ( config->flow_control == FLOW_CONTROL_DISABLED )
    {
        /* No Flow Control */
        uart_seci_base->mcr &= ~(UART_SECI_MCR_AUTO_RTS);
        uart_seci_base->mcr &= ~(UART_SECI_MCR_AUTO_TX_DIS);
    }
    else
    {
        /* RTS+CTS Flow Control */

    /* DO not enable AUTO_RTS now, (uart_seci_base->mcr |= UART_SECI_MCR_AUTO_RTS)
         * this asserts RTS Low, indicating readiness to accept data.
         * When waking up from Deep-Sleep, this causes problems, as UART is not fully
         * initialized at this time, and interrupts are disabled, so any incoming data
         * might be lost.
         */

         uart_seci_base->mcr |= UART_SECI_MCR_AUTO_TX_DIS;
    }

    /* Setup LCR and MCR registers for TX, RX and HW flow control */
    /* Do not enable RX, interrupts are still disabled */
    uart_seci_base->lcr |= UART_SECI_LCR_TXO_EN;
    uart_seci_base->mcr |= UART_SECI_MCR_TX_EN;

    return PLATFORM_SUCCESS;
}

static platform_result_t uart_slow_deinit_internal( platform_uart_driver_t* driver )
{
    int i;

    /* Disable FIFO */
    uart_slow_base->iir_fcr &= ~UART_SLOW_FCR_FIFO_ENABLE;

    /* Disable interrupts from slow UART during initialization */
    platform_uart_disable_interrupts( driver->interface->port );

#ifndef BCM4390X_UART_SLOW_POLL_MODE
    /* Make sure ChipCommon Core external IRQ to APPS core is enabled */
    platform_chipcommon_disable_irq( );
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */

    /* Deinit semaphores */
    host_rtos_deinit_semaphore( &driver->rx_complete );
    host_rtos_deinit_semaphore( &driver->tx_complete );

    /* Deinit ring buffers */
    ring_buffer_deinit( &uart_slow_rx_buffer );

    /* Clean pending data. */
    for ( i = 0; i < UART_SLOW_CLEAN_ATTEMPTS; ++i )
    {
        if ( ( uart_slow_base->lsr & UART_SLOW_LSR_RXRDY ) == 0 )
        {
            break;
        }
        (void)uart_slow_base->rx_tx_dll; /* read and discard */
    }

    return PLATFORM_SUCCESS;
}

static platform_result_t uart_slow_init_internal( platform_uart_driver_t* driver, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    uint32_t            alp_clock_value = 0;
    uint16_t            baud_divider;
    int                 i;

    /* Disable interrupts from slow UART during initialization */
    platform_uart_disable_interrupts(driver->interface->port);

    /* Slow UART is driven by ALP clock */
    if ( driver->interface->src_clk != CLOCK_ALP )
    {
        wiced_assert(" Slow UART is driven by ALP clock ", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    /* Currently supported data width is 8 bits */
    if ( config->data_width != DATA_WIDTH_8BIT )
    {
        wiced_assert(" Currently supported data width is 8 bits ", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    /* Slow UART does not have HW flow control */
    if ( config->flow_control != FLOW_CONTROL_DISABLED )
    {
        wiced_assert(" Slow UART does not have HW flow control ", 0 );
        return PLATFORM_UNSUPPORTED;
    }
    driver->hw_flow_control_is_on = WICED_FALSE;

    /* Make sure ChipCommon core is enabled */
    osl_core_enable(CC_CORE_ID);

    /* Enable Slow UART clocking */
    /* Turn on ALP clock for UART, when we are not using a divider
     * Set the override bit so we don't divide it
     */
    CHIPCOMMON_CORE_CTRL_REG &= ~( 1 << 3 );
    CHIPCOMMON_CORE_CTRL_REG |= 0x01;
    CHIPCOMMON_CORE_CTRL_REG |= ( 1 << 3 );

    /* Get current ALP clock value */
    alp_clock_value = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ALP );

    wiced_assert("ALP clock value can not be identified properly", alp_clock_value != 0);

    /* Calculate the necessary divider value for a given baud rate */
    /* divisor = (serial clock frequency/16) / (baud rate)
    *  The baud_rate / 2 is added to reduce error to + / - half of baud rate.
    */
    baud_divider = ( (alp_clock_value / 16 ) + (config->baud_rate / 2) ) / config->baud_rate;

    /* Setup the baudrate */
    uart_slow_base->lcr      |= UART_SLOW_LCR_DLAB;
    uart_slow_base->rx_tx_dll = baud_divider & 0xff;
    uart_slow_base->ier_dlm   = baud_divider >> 8;
    uart_slow_base->lcr      |= UART_SLOW_LCR_WLEN8;

    /* Clear DLAB bit. This bit must be cleared after initial baud rate setup in order to access other registers. */
    uart_slow_base->lcr &= ~UART_SLOW_LCR_DLAB;
    wiced_assert(" Currently we do not support data width different from 8bits ", config->data_width == DATA_WIDTH_8BIT );

    /* Enable FIFO */
    uart_slow_base->iir_fcr |= UART_SLOW_FCR_FIFO_ENABLE;

    /* Configure stop bits and a parity */
    if ( config->parity == ODD_PARITY )
    {
        uart_slow_base->lcr |= ( 1 << UART_PEN_BIT );
        uart_slow_base->lcr &= ~( 1 << UART_EPS_BIT );
    }
    else if ( config->parity == EVEN_PARITY )
    {
        uart_slow_base->lcr |= ( 1 << UART_PEN_BIT );
        uart_slow_base->lcr |= ( 1 << UART_EPS_BIT );
    }

    /* Strap the appropriate platform pins for Slow UART RX/TX functions */
    platform_pinmux_init(driver->interface->rx_pin->pin, PIN_FUNCTION_UART_DBG_RX);
    platform_pinmux_init(driver->interface->tx_pin->pin, PIN_FUNCTION_UART_DBG_TX);

    /* Setup ring buffer and related parameters */
    if ( optional_ring_buffer != NULL )
    {
        driver->rx_buffer = optional_ring_buffer;
    }
    else
    {
        ring_buffer_init( &uart_slow_rx_buffer, uart_slow_rx_data, UART_SLOW_RX_BUFFER_SIZE );
        driver->rx_buffer = &uart_slow_rx_buffer;
    }

    host_rtos_init_semaphore(&driver->rx_complete);
    host_rtos_init_semaphore(&driver->tx_complete);

    /* Reset any error counters */
    driver->rx_overflow = 0;

#ifndef BCM4390X_UART_SLOW_POLL_MODE
    /* Initialization complete, enable interrupts from slow UART */
    platform_uart_enable_interrupts(driver->interface->port);

    /* Make sure ChipCommon Core external IRQ to APPS core is enabled */
    platform_chipcommon_enable_irq();
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */

    /* Clean pending data. */
    for ( i = 0; i < UART_SLOW_CLEAN_ATTEMPTS; ++i )
    {
        if ( ( uart_slow_base->lsr & UART_SLOW_LSR_RXRDY ) == 0 )
        {
            break;
        }
        (void)uart_slow_base->rx_tx_dll; /* read and discard */
    }
#ifdef WICED_POWER_LOGGER_ENABLE
    host_rtos_set_semaphore( &driver->tx_complete, WICED_FALSE);
#endif
    return PLATFORM_SUCCESS;
}

static platform_result_t uart_fast_deinit_internal( platform_uart_driver_t* driver )
{
    /* Disable receive-enable here at first */
    uart_fast_base->lcr &= ~UART_SECI_LCR_RX_EN;
    uart_fast_base->mcr &= ~UART_SECI_MCR_AUTO_RTS;

    /* Set linebreak to set SECI is not ready to communicate */
    CHIPCOMMON_SECI_UART_LCR_REG |= 0x10;

    /* Set force-low bit: we're not ready for data as we are deinitting */
    CHIPCOMMON_SECI_CONFIG_REG |= CC_SECI_ENAB_SECIOUT_DIS;

    /* Disable interrupts as we are deinitting; we won't re-enable until the next init call */
    platform_uart_disable_interrupts( driver->interface->port );

#ifndef BCM4390X_UART_FAST_POLL_MODE
    /* Make sure ChipCommon Core external IRQ to APPS core is disabled */
    platform_chipcommon_disable_irq();
#endif /* !BCM4390X_UART_FAST_POLL_MODE */

    /* Deinit semaphores used for data transfer completion */
    host_rtos_deinit_semaphore(&driver->rx_complete);
    host_rtos_deinit_semaphore(&driver->tx_complete);

    /* Clear out the ring buffer as we're deinitting */
    ring_buffer_deinit( &uart_fast_rx_buffer );

    return PLATFORM_SUCCESS;
}


static platform_result_t uart_fast_init_internal( platform_uart_driver_t* driver, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    uint32_t cc_cap_ext = 0;
    uint32_t clk_value  = 0;
    uint32_t clk_timeout = 100000000;
    uint32_t cc_seci_fifo_level = 0;
    uint32_t flags;

    /* Disable interrupts from fast UART during initialization */
    platform_uart_disable_interrupts(driver->interface->port);

    /* Currently supported data width is 8 bits */
    if ( config->data_width != DATA_WIDTH_8BIT )
    {
        wiced_assert(" Currently supported data width is 8 bits ", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    if ( config->flow_control == FLOW_CONTROL_DISABLED )
    {
        /* No Flow Control */
        driver->hw_flow_control_is_on = WICED_FALSE;
    }
    else
    {
        /* RTS+CTS Flow Control */
        driver->hw_flow_control_is_on = WICED_TRUE;
    }

    /* Make sure ChipCommon core is enabled */
    osl_core_enable(CC_CORE_ID);

    /* 43909 Fast UART has dedicated pins, so pin-mux selection not required */

    /* Make sure SECI Fast UART is available on the chip */
    cc_cap_ext = CHIPCOMMON_CORE_CAP_EXT_REG;
    if ( !(cc_cap_ext & CC_CAP_EXT_SECI_PRESENT) && !(cc_cap_ext & CC_CAP_EXT_SECI_PLAIN_UART_PRESENT) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Configure SECI Fast UART clock */
    if ( driver->interface->src_clk == CLOCK_HT )
    {
        CHIPCOMMON_SECI_CONFIG_REG |= (CC_SECI_CONFIG_HT_CLOCK);
        clk_value = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_FAST_UART );
    }
    else if ( driver->interface->src_clk == CLOCK_ALP )
    {
        CHIPCOMMON_SECI_CONFIG_REG &= ~(CC_SECI_CONFIG_HT_CLOCK);
        clk_value = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ALP );
    }
    else
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Check if the desired configuration and clock are supported */
    if ( uart_seci_check_config( clk_value, config ) != PLATFORM_SUCCESS )
    {
        return PLATFORM_UNSUPPORTED;
    }

    WICED_SAVE_INTERRUPTS(flags);

    /* Enable SECI clock */
    CHIPCOMMON_CLK_CTL_STATUS_REG |= CC_CLK_CTL_ST_SECI_CLK_REQ;

    WICED_RESTORE_INTERRUPTS(flags);

    /* Wait for the transition to complete */
    while ( ( ( CHIPCOMMON_CLK_CTL_STATUS_REG & CC_CLK_CTL_ST_SECI_CLK_AVAIL ) == 0 ) && ( clk_timeout > 0 ) )
    {
        /* spin wait for clock transition */
        clk_timeout--;
    }

    /* Put SECI block into reset */
    CHIPCOMMON_SECI_CONFIG_REG &= ~(CC_SECI_ENAB_SECI_ECI);
    CHIPCOMMON_SECI_CONFIG_REG |= CC_SECI_RESET;

    /* set force-low, and set EN_SECI for all non-legacy modes */
    CHIPCOMMON_SECI_CONFIG_REG |= CC_SECI_ENAB_SECIOUT_DIS;
    CHIPCOMMON_SECI_CONFIG_REG |= CC_SECI_ENAB_SECI_ECI;

    /* Take SECI block out of reset */
    CHIPCOMMON_SECI_CONFIG_REG &= ~(CC_SECI_RESET);

    /* Set linebreak to set SECI is not ready to communicate */
    CHIPCOMMON_SECI_UART_LCR_REG |= 0x10;

    /* Setup and configure SECI UART */
    uart_seci_setup_internal( uart_fast_base, clk_value, config );

    /* Setup SECI block operation mode */
    CHIPCOMMON_SECI_CONFIG_REG &= ~(CC_SECI_MODE_MASK << CC_SECI_MODE_SHIFT);
    CHIPCOMMON_SECI_CONFIG_REG |= (SECI_MODE_UART << CC_SECI_MODE_SHIFT);

    /* Setup default FIFO levels for TX, RX and flow control */
    cc_seci_fifo_level = CHIPCOMMON_SECI_FIFO_LEVEL_REG;
    cc_seci_fifo_level &= ~(CC_SECI_RX_FIFO_LVL_MASK << CC_SECI_RX_FIFO_LVL_SHIFT);
    cc_seci_fifo_level |= (CC_SECI_RX_FIFO_LVL_DEFAULT << CC_SECI_RX_FIFO_LVL_SHIFT);
    cc_seci_fifo_level &= ~(CC_SECI_TX_FIFO_LVL_MASK << CC_SECI_TX_FIFO_LVL_SHIFT);
    cc_seci_fifo_level |= (CC_SECI_TX_FIFO_LVL_DEFAULT << CC_SECI_TX_FIFO_LVL_SHIFT);
    cc_seci_fifo_level &= ~(CC_SECI_RX_FIFO_LVL_FLOW_CTL_MASK << CC_SECI_RX_FIFO_LVL_FLOW_CTL_SHIFT);
    cc_seci_fifo_level |= (CC_SECI_RX_FIFO_LVL_FLOW_CTL_DEFAULT << CC_SECI_RX_FIFO_LVL_FLOW_CTL_SHIFT);
    CHIPCOMMON_SECI_FIFO_LEVEL_REG = cc_seci_fifo_level;

    /* Strap the appropriate platform pins for Fast UART RX/TX/CTS/RTS functions */
    platform_pinmux_init(driver->interface->rx_pin->pin,  PIN_FUNCTION_FAST_UART_RX);
    platform_pinmux_init(driver->interface->tx_pin->pin,  PIN_FUNCTION_FAST_UART_TX);
    platform_pinmux_init(driver->interface->cts_pin->pin, PIN_FUNCTION_FAST_UART_CTS_IN);
    platform_pinmux_init(driver->interface->rts_pin->pin, PIN_FUNCTION_FAST_UART_RTS_OUT);

    /* Setup ring buffer and related parameters */
    if ( optional_ring_buffer != NULL )
    {
        driver->rx_buffer = optional_ring_buffer;
    }
    else
    {
        ring_buffer_init(&uart_fast_rx_buffer, uart_fast_rx_data, UART_FAST_RX_BUFFER_SIZE);
        driver->rx_buffer = &uart_fast_rx_buffer;
    }

    host_rtos_init_semaphore(&driver->rx_complete);
    host_rtos_init_semaphore(&driver->tx_complete);

    /* Reset any error counters */
    driver->rx_overflow = 0;

    /* Clear force-low bit */
    CHIPCOMMON_SECI_CONFIG_REG &= ~CC_SECI_ENAB_SECIOUT_DIS;

    /* Wait 10us for enabled SECI block serial output to stabilize */
    OSL_DELAY(10);

#ifndef BCM4390X_UART_FAST_POLL_MODE
    /* Initialization complete, enable interrupts from fast UART */
    platform_uart_enable_interrupts(driver->interface->port);

    /* Make sure ChipCommon Core external IRQ to APPS core is enabled */
    platform_chipcommon_enable_irq();
#endif /* !BCM4390X_UART_FAST_POLL_MODE */

    /* Disable linebreak to indicate SECI is ready to communicate */
    CHIPCOMMON_SECI_UART_LCR_REG &= ~(0x10);

    /* Enable receive-enable at the end, when everything is set-up */
    uart_fast_base->lcr |= UART_SECI_LCR_RX_EN;
    uart_fast_base->mcr |= UART_SECI_MCR_AUTO_RTS;

    return PLATFORM_SUCCESS;
}

static platform_result_t uart_gci_deinit_internal( platform_uart_driver_t* driver )
{
    /* Set SECI output force-low */
    GCI_CORE_CTRL_REG |= (GCI_CORE_CTRL_FORCE_SECI_OUT_LOW);

    /* Disable the SECI UART TX state machine */
    GCI_SECI_OUT_TX_ENAB_TX_BRK_REG &= ~(GCI_SECI_TX_ENAB);

    /* Disable UART bytes reception from SECI_IN port to RX FIFO */
    GCI_SECI_IN_AUX_FIFO_RX_ENAB_REG &= ~(GCI_SECI_FIFO_RX_ENAB);

    /*Disable SECI data communication */
    GCI_CORE_CTRL_REG &= ~(GCI_CORE_CTRL_ENABLE_SECI);

    /* Disable interrupts from GCI UART during deinitialization */
    platform_uart_disable_interrupts(driver->interface->port);

#ifndef BCM4390X_UART_GCI_POLL_MODE
    /* Make sure ChipCommon Core external IRQ to APPS core is enabled */
    platform_chipcommon_enable_irq( );
#endif /* !BCM4390X_UART_GCI_POLL_MODE */

    host_rtos_deinit_semaphore( &driver->rx_complete );
    host_rtos_deinit_semaphore( &driver->tx_complete );

    ring_buffer_deinit( &uart_gci_rx_buffer );

    return PLATFORM_UNSUPPORTED;
}

static platform_result_t uart_gci_init_internal( platform_uart_driver_t* driver, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    uint32_t clk_value  = 0;
    uint32_t clk_timeout = 100000000;
    uint32_t reg_val;
    uint32_t gci_seci_fifo_level = 0;
    uint32_t flags;

    /* Disable interrupts from GCI UART during initialization */
    platform_uart_disable_interrupts(driver->interface->port);

    /* GCI UART is driven by ALP clock */
    if ( driver->interface->src_clk != CLOCK_ALP )
    {
        wiced_assert(" GCI UART is driven by ALP clock ", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    /* Currently supported data width is 8 bits */
    if ( config->data_width != DATA_WIDTH_8BIT )
    {
        wiced_assert(" Currently supported data width is 8 bits ", 0 );
        return PLATFORM_UNSUPPORTED;
    }

    /* GCI UART does not have HW flow control */
    if ( config->flow_control != FLOW_CONTROL_DISABLED )
    {
        wiced_assert(" GCI UART does not have HW flow control ", 0 );
        return PLATFORM_UNSUPPORTED;
    }
    driver->hw_flow_control_is_on = WICED_FALSE;

    /* Configure GCI UART clock */
    clk_value = platform_reference_clock_get_freq( PLATFORM_REFERENCE_CLOCK_ALP );

    /* Check if the desired configuration and clock are supported */
    if ( uart_seci_check_config(clk_value, config) != PLATFORM_SUCCESS )
    {
        return PLATFORM_UNSUPPORTED;
    }

    /* Set ForceSeciClk and ForceRegClk */
    GCI_CORE_CTRL_REG |= (GCI_CORE_CTRL_FORCE_SECI_CLK | GCI_CORE_CTRL_FORCE_REG_CLK);

    /* Wait for SECI Clock Available */
    while ( ( ( GCI_CORE_STATUS_REG & GCI_CORE_STATUS_GCI_CLK_AVAIL ) == 0 ) && ( clk_timeout > 0 ) )
    {
        /* spin wait for clock transition */
        clk_timeout--;
    }

    /* Put SECI block and OffChipCoex into reset */
    GCI_CORE_CTRL_REG |= (GCI_CORE_CTRL_SECI_RESET | GCI_CORE_CTRL_RESET_SECI_LOGIC | GCI_CORE_CTRL_RESET_OFF_CHIP_COEX);

    /* Set SECI output force-low */
    GCI_CORE_CTRL_REG |= (GCI_CORE_CTRL_FORCE_SECI_OUT_LOW);

    /* Take SECI block and OffChipCoex out of reset */
    GCI_CORE_CTRL_REG &= ~(GCI_CORE_CTRL_SECI_RESET | GCI_CORE_CTRL_RESET_SECI_LOGIC | GCI_CORE_CTRL_RESET_OFF_CHIP_COEX);

    /* Clear ForceSeciClk and ForceRegClk */
    GCI_CORE_CTRL_REG &= ~(GCI_CORE_CTRL_FORCE_SECI_CLK | GCI_CORE_CTRL_FORCE_REG_CLK);

    WICED_SAVE_INTERRUPTS(flags);

    /* Configure SECI_IN Control */
    GCI_INDIRECT_ADDRESS_REG = 0x0;
    reg_val = GCI_SECI_IN_CTRL_REG;
    /* Setup SECI_IN operation mode */
    reg_val &= ~(GCI_SECI_IN_OP_MODE_MASK << GCI_SECI_IN_OP_MODE_SHIFT);
    reg_val |= (SECI_MODE_UART << GCI_SECI_IN_OP_MODE_SHIFT);
    /* Setup GCI_GPIO to SECI_IN (UART_RX) mapping */
    reg_val &= ~(GCI_SECI_IN_GPIO_NUM_MASK << GCI_SECI_IN_GPIO_NUM_SHIFT);
    reg_val |= (GCI_SECI_IN_GPIO_NUM_DEFAULT << GCI_SECI_IN_GPIO_NUM_SHIFT);
    GCI_SECI_IN_CTRL_REG = reg_val;

    /* Configure SECI_OUT Control */
    GCI_INDIRECT_ADDRESS_REG = 0x0;
    reg_val = GCI_SECI_OUT_CTRL_REG;
    /* Setup SECI_OUT operation mode */
    reg_val &= ~(GCI_SECI_OUT_OP_MODE_MASK << GCI_SECI_OUT_OP_MODE_SHIFT);
    reg_val |= (SECI_MODE_UART << GCI_SECI_OUT_OP_MODE_SHIFT);
    /* Setup SECI_OUT (UART_TX) to GCI_GPIO mapping */
    reg_val &= ~(GCI_SECI_OUT_GPIO_NUM_MASK << GCI_SECI_OUT_GPIO_NUM_SHIFT);
    reg_val |= (GCI_SECI_OUT_GPIO_NUM_DEFAULT << GCI_SECI_OUT_GPIO_NUM_SHIFT);
    GCI_SECI_OUT_CTRL_REG = reg_val;

    /* Initialize the SECI_IN GCI_GPIO */
    GCI_INDIRECT_ADDRESS_REG = GCI_GPIO_CTRL_REG_INDEX(GCI_SECI_IN_GPIO_NUM_DEFAULT);
    reg_val = GCI_GPIO_CTRL_REG;
    reg_val &= ~(GCI_GPIO_CTRL_GPIO_MASK(GCI_SECI_IN_GPIO_NUM_DEFAULT));
    reg_val |= (GCI_GPIO_CTRL_INPUT_ENAB(GCI_SECI_IN_GPIO_NUM_DEFAULT));
    reg_val |= (GCI_GPIO_CTRL_PDN(GCI_SECI_IN_GPIO_NUM_DEFAULT));
    GCI_GPIO_CTRL_REG = reg_val;

    /* Initialize the SECI_OUT GCI_GPIO */
    GCI_INDIRECT_ADDRESS_REG = GCI_GPIO_CTRL_REG_INDEX(GCI_SECI_OUT_GPIO_NUM_DEFAULT);
    reg_val = GCI_GPIO_CTRL_REG;
    reg_val &= ~(GCI_GPIO_CTRL_GPIO_MASK(GCI_SECI_OUT_GPIO_NUM_DEFAULT));
    reg_val |= (GCI_GPIO_CTRL_OUTPUT_ENAB(GCI_SECI_OUT_GPIO_NUM_DEFAULT));
    reg_val |= (GCI_GPIO_CTRL_PDN(GCI_SECI_OUT_GPIO_NUM_DEFAULT));
    GCI_GPIO_CTRL_REG = reg_val;

    WICED_RESTORE_INTERRUPTS(flags);

    /* Enable SECI data communication */
    GCI_CORE_CTRL_REG |= (GCI_CORE_CTRL_ENABLE_SECI);

    /* Setup and configure SECI UART */
    uart_seci_setup_internal(uart_gci_base, clk_value, config);

    /* Setup default FIFO level for RX */
    gci_seci_fifo_level = GCI_GCI_RX_FIFO_PER_IP_CTRL_REG;
    gci_seci_fifo_level &= ~(GCI_SECI_RX_FIFO_LVL_MASK << GCI_SECI_RX_FIFO_LVL_SHIFT);
    gci_seci_fifo_level |= (GCI_SECI_RX_FIFO_LVL_DEFAULT << GCI_SECI_RX_FIFO_LVL_SHIFT);
    GCI_GCI_RX_FIFO_PER_IP_CTRL_REG = gci_seci_fifo_level;

    /* Setup default FIFO level for TX */
    gci_seci_fifo_level = GCI_SECI_FIFO_LEVEL_REG;
    gci_seci_fifo_level &= ~(GCI_SECI_TX_FIFO_LVL_MASK << GCI_SECI_TX_FIFO_LVL_SHIFT);
    gci_seci_fifo_level |= (GCI_SECI_TX_FIFO_LVL_DEFAULT << GCI_SECI_TX_FIFO_LVL_SHIFT);
    GCI_SECI_FIFO_LEVEL_REG = gci_seci_fifo_level;

    /* Setup ring buffer and related parameters */
    if ( optional_ring_buffer != NULL )
    {
        driver->rx_buffer = optional_ring_buffer;
    }
    else
    {
        ring_buffer_init(&uart_gci_rx_buffer, uart_gci_rx_data, UART_GCI_RX_BUFFER_SIZE);
        driver->rx_buffer = &uart_gci_rx_buffer;
    }

    host_rtos_init_semaphore(&driver->rx_complete);
    host_rtos_init_semaphore(&driver->tx_complete);

    /* Reset any error counters */
    driver->rx_overflow = 0;

    /* Strap the appropriate platform pins for GCI UART RX/TX functions */
    platform_pinmux_init(driver->interface->rx_pin->pin, PIN_FUNCTION_SECI_IN);
    platform_pinmux_init(driver->interface->tx_pin->pin, PIN_FUNCTION_SECI_OUT);

    /* Enable UART bytes reception from SECI_IN port to RX FIFO */
    GCI_SECI_IN_AUX_FIFO_RX_ENAB_REG |= (GCI_SECI_FIFO_RX_ENAB);

    /* Enable the SECI UART TX state machine */
    GCI_SECI_OUT_TX_ENAB_TX_BRK_REG |= (GCI_SECI_TX_ENAB);

    /* Clear SECI output force-low */
    GCI_CORE_CTRL_REG &= ~(GCI_CORE_CTRL_FORCE_SECI_OUT_LOW);

    /* Wait 10us for enabled SECI block serial output to stabilize */
    OSL_DELAY(10);

#ifndef BCM4390X_UART_GCI_POLL_MODE
    /* Initialization complete, enable interrupts from GCI UART */
    platform_uart_enable_interrupts(driver->interface->port);

    /* Make sure ChipCommon Core external IRQ to APPS core is enabled */
    platform_chipcommon_enable_irq();
#endif /* !BCM4390X_UART_GCI_POLL_MODE */

    return PLATFORM_SUCCESS;
}

platform_result_t platform_uart_init( platform_uart_driver_t* driver, const platform_uart_t* interface, const platform_uart_config_t* config, wiced_ring_buffer_t* optional_ring_buffer )
{
    if ( (driver == NULL) || (interface == NULL) || (config == NULL) )
    {
        wiced_assert( "Bad argument", 0 );
        return PLATFORM_ERROR;
    }

    if ( optional_ring_buffer != NULL )
    {
        if ( (optional_ring_buffer->buffer == NULL) || (optional_ring_buffer->size == 0) )
        {
            wiced_assert("Ring buffer is not initialized", 0 );
            return PLATFORM_ERROR;
        }
    }

#ifdef CONS_LOG_BUFFER_SUPPORT
#ifdef CONS_LOG_BUFFER_ONLY
    driver->interface = (platform_uart_t*) interface;
    return cons_init();
#else
    cons_init();
#endif  /* CONS_LOG_BUFFER_ONLY */
#endif  /* CONS_LOG_BUFFER_SUPPORT */
    driver->interface = (platform_uart_t*) interface;

    if ( interface->port == UART_SLOW )
    {
        return uart_slow_init_internal( driver, config, optional_ring_buffer );
    }
    else if ( interface->port == UART_FAST )
    {
        return uart_fast_init_internal( driver, config, optional_ring_buffer );
    }
    else if ( interface->port == UART_GCI )
    {
        return uart_gci_init_internal( driver, config, optional_ring_buffer );
    }
    else
    {
        driver->interface = NULL;
        return PLATFORM_UNSUPPORTED;
    }
}

platform_result_t platform_uart_deinit( platform_uart_driver_t* driver )
{
    if ( NULL != driver->interface )
    {
        if ( driver->interface->port == UART_SLOW )
        {
            return uart_slow_deinit_internal( driver );
        }
        else if ( driver->interface->port == UART_FAST )
        {
            return uart_fast_deinit_internal( driver );
        }
        else if ( driver->interface->port == UART_GCI )
        {
            return uart_gci_deinit_internal( driver );
        }
    }
    return PLATFORM_UNSUPPORTED;
}

#if !defined(BCM4390X_UART_SLOW_POLL_MODE) || !defined(BCM4390X_UART_FAST_POLL_MODE) || !defined(BCM4390X_UART_GCI_POLL_MODE)
static platform_result_t uart_receive_bytes_irq( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* data_size_left_to_read, uint32_t timeout_ms )
{
    platform_result_t result = PLATFORM_ERROR;

    wiced_assert( "bad argument", ( driver != NULL ) && ( data_in != NULL ) && ( data_size_left_to_read != NULL ) && ( *data_size_left_to_read != 0 ) );
    wiced_assert( "not inited", ( driver->rx_buffer != NULL ) );

    if ( driver->rx_buffer != NULL )
    {
        uint32_t bytes_read = 0;
        uint32_t read_index = 0;
        uint32_t data_size  = 0;
        uint32_t ring_size  = 0;

        result = PLATFORM_SUCCESS;

        while ( *data_size_left_to_read > 0 )
        {
            wwd_result_t sem_result;
            uint32_t     flags;

            /* Get the semaphore whenever data needs to be consumed from the ring buffer */
            sem_result = host_rtos_get_semaphore( &driver->rx_complete, timeout_ms, WICED_TRUE );
            if ( sem_result != WWD_SUCCESS )
            {
                /* Can't get the semaphore */
                result = ( sem_result == WWD_TIMEOUT ) ? PLATFORM_TIMEOUT : PLATFORM_ERROR;
                break;
            }

            /* Disable interrupts */
            WICED_SAVE_INTERRUPTS(flags);

            /* Read the data from the ring buffer */
            ring_size = ring_buffer_used_space(driver->rx_buffer);
            data_size = MIN( *data_size_left_to_read, ring_size );
            ring_buffer_read( driver->rx_buffer, &data_in[read_index], data_size, &bytes_read );
            read_index              += bytes_read;
            *data_size_left_to_read -= bytes_read;
            ring_size = ring_buffer_used_space(driver->rx_buffer);

            /* Make sure the UART interrupts are re-enabled, they could have
             * been disabled by the ISR if ring buffer was about to overflow */
            platform_uart_enable_interrupts(driver->interface->port);

            /* Enable interrupts */
            WICED_RESTORE_INTERRUPTS(flags);

            if ( ring_size > 0 )
            {
                /* Set the semaphore to indicate the ring buffer is not empty */
                host_rtos_set_semaphore( &driver->rx_complete, WICED_TRUE );
            }
        }
    }

    return result;
}
#endif /* !BCM4390X_UART_SLOW_POLL_MODE || !BCM4390X_UART_FAST_POLL_MODE || !BCM4390X_UART_GCI_POLL_MODE */

static wiced_bool_t uart_slow_txrx_ready( wiced_bool_t tx )
{
    if ( tx == WICED_TRUE )
    {
        /* Check if TX interface is ready with space available to accept TX data */
        const uint8_t empty_mask = UART_SLOW_LSR_THRE;
        return ( ( uart_slow_base->lsr & empty_mask ) != empty_mask ) ? WICED_FALSE : WICED_TRUE;
    }
    else
    {
        /* Check if RX interface is ready with RX data available to be read */
        return ( ( uart_slow_base->lsr & UART_SLOW_LSR_RXRDY ) == 0 ) ? WICED_FALSE : WICED_TRUE;
    }
}

static wiced_bool_t uart_fast_txrx_ready( wiced_bool_t tx )
{
    if ( tx == WICED_TRUE )
    {
        /* Check if TX interface is ready with space available to accept TX data */
        return ( CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_TX_FIFO_FULL ) ? WICED_FALSE : WICED_TRUE;
    }
    else
    {
        /* Check if RX interface is ready with RX data available to be read */
        return ( CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_RX_FIFO_EMPTY ) ? WICED_FALSE : WICED_TRUE;
    }
}

static wiced_bool_t uart_gci_txrx_ready( wiced_bool_t tx )
{
    if ( tx == WICED_TRUE )
    {
        /* Check if TX interface is ready with space available to accept TX data */
        return ( GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_TX_FIFO_FULL ) ? WICED_FALSE : WICED_TRUE;
    }
    else
    {
        /* Check if RX interface is ready with RX data available to be read */
        return ( GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_RX_FIFO_NOT_EMPTY ) ? WICED_TRUE : WICED_FALSE;
    }
}

wiced_bool_t platform_uart_txrx_ready( platform_uart_port_t port, wiced_bool_t tx )
{
    if ( port == UART_SLOW )
    {
        return uart_slow_txrx_ready( tx );
    }
    else if ( port == UART_FAST)
    {
        return uart_fast_txrx_ready( tx );
    }
    else if ( port == UART_GCI )
    {
        return uart_gci_txrx_ready( tx );
    }

    return WICED_FALSE;
}

#ifdef BCM4390X_UART_SLOW_POLL_MODE
platform_result_t uart_slow_receive_bytes_poll( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* data_size_left_to_read, uint32_t timeout_ms )
{
    wwd_time_t   total_start_time   = host_rtos_get_time( );
    wwd_time_t   total_elapsed_time = 0;
    wiced_bool_t first_read         = WICED_TRUE;

    do
    {
        wwd_time_t read_start_time   = host_rtos_get_time( );
        wwd_time_t read_elapsed_time = 0;
        wwd_time_t read_timeout_ms   = ( first_read == WICED_TRUE ) ? timeout_ms : UART_SLOW_MAX_READ_WAIT_TIME;

        /* Wait till there is something in the RX register of the UART */
        while ( ( ( uart_slow_base->lsr & UART_SLOW_LSR_RXRDY ) == 0 ) && ( read_elapsed_time < read_timeout_ms ) )
        {
            read_elapsed_time = host_rtos_get_time() - read_start_time;

            host_rtos_delay_milliseconds(1);
        }

        if ( read_elapsed_time >= read_timeout_ms )
        {
            break;
        }

        *data_in++ = uart_slow_base->rx_tx_dll;
        (*data_size_left_to_read)--;
        total_elapsed_time = host_rtos_get_time() - total_start_time;

    } while ( ( *data_size_left_to_read != 0 ) && ( total_elapsed_time < timeout_ms ) );

    return ( *data_size_left_to_read == 0 ) ? PLATFORM_SUCCESS : PLATFORM_TIMEOUT;
}
#endif /* BCM4390X_UART_SLOW_POLL_MODE */

#ifdef BCM4390X_UART_FAST_POLL_MODE
platform_result_t uart_fast_receive_bytes_poll( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* data_size_left_to_read, uint32_t timeout_ms )
{
    wwd_time_t   total_start_time   = host_rtos_get_time( );
    wwd_time_t   total_elapsed_time = 0;
    wiced_bool_t first_read         = WICED_TRUE;

    do
    {
        wwd_time_t read_start_time   = host_rtos_get_time( );
        wwd_time_t read_elapsed_time = 0;
        wwd_time_t read_timeout_ms   = ( first_read == WICED_TRUE ) ? timeout_ms : UART_FAST_MAX_READ_WAIT_TIME;

        /* Wait till there is something in the RX FIFO of the Fast UART */
        while ( (CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_RX_FIFO_EMPTY) && (read_elapsed_time < read_timeout_ms) )
        {
            read_elapsed_time = host_rtos_get_time() - read_start_time;

            host_rtos_delay_milliseconds(1);
        }

        if ( read_elapsed_time >= read_timeout_ms )
        {
            break;
        }

        *data_in++ = uart_fast_base->data;
        (*data_size_left_to_read)--;
        total_elapsed_time = host_rtos_get_time() - total_start_time;

    } while ( ( *data_size_left_to_read != 0 ) && ( total_elapsed_time < timeout_ms ) );

    return ( *data_size_left_to_read == 0 ) ? PLATFORM_SUCCESS : PLATFORM_TIMEOUT;
}
#endif /* BCM4390X_UART_FAST_POLL_MODE */

#ifdef BCM4390X_UART_GCI_POLL_MODE
platform_result_t uart_gci_receive_bytes_poll( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* data_size_left_to_read, uint32_t timeout_ms )
{
    wwd_time_t   total_start_time   = host_rtos_get_time( );
    wwd_time_t   total_elapsed_time = 0;
    wiced_bool_t first_read         = WICED_TRUE;

    do
    {
        wwd_time_t read_start_time   = host_rtos_get_time( );
        wwd_time_t read_elapsed_time = 0;
        wwd_time_t read_timeout_ms   = ( first_read == WICED_TRUE ) ? timeout_ms : UART_GCI_MAX_READ_WAIT_TIME;

        /* Wait till there is something in the RX FIFO of the GCI UART */
        while ( !(GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_RX_FIFO_NOT_EMPTY) && (read_elapsed_time < read_timeout_ms) )
        {
            read_elapsed_time = host_rtos_get_time() - read_start_time;

            host_rtos_delay_milliseconds(1);
        }

        if ( read_elapsed_time >= read_timeout_ms )
        {
            break;
        }

        *data_in++ = uart_gci_base->data;
        (*data_size_left_to_read)--;
        total_elapsed_time = host_rtos_get_time() - total_start_time;

    } while ( ( *data_size_left_to_read != 0 ) && ( total_elapsed_time < timeout_ms ) );

    return ( *data_size_left_to_read == 0 ) ? PLATFORM_SUCCESS : PLATFORM_TIMEOUT;
}
#endif /* BCM4390X_UART_GCI_POLL_MODE */

wiced_bool_t platform_uart_transmit_fifo_empty( platform_uart_port_t port )
{
    switch ( port )
    {
        case UART_FAST:
            return uart_fast_transmit_fifo_empty( );

        case UART_SLOW:
            return uart_slow_transmit_fifo_empty( );

        default:
            return WICED_TRUE;
    }
}

static wiced_bool_t uart_slow_transmit_fifo_empty( void )
{
    const uint8_t empty_mask = UART_SLOW_LSR_THRE;
    return ( ( uart_slow_base->lsr & empty_mask ) != empty_mask ) ? WICED_FALSE : WICED_TRUE;
}

static wiced_bool_t uart_slow_wait_transmit_fifo_empty( void )
{
    const wwd_time_t start_time = host_rtos_get_time();
#if PLATFORM_ALP_CLOCK_RES_FIXUP
    const uint8_t    empty_mask = UART_SLOW_LSR_TDHR;
#else
    const uint8_t    empty_mask = UART_SLOW_LSR_THRE;
#endif

    while ( ( uart_slow_base->lsr & empty_mask ) != empty_mask )
    {
        const wwd_time_t elapsed_time = host_rtos_get_time( ) - start_time;

        if ( elapsed_time >= UART_SLOW_MAX_TRANSMIT_WAIT_TIME )
        {
            break;
        }
    }

    if ( ( uart_slow_base->lsr & empty_mask ) != empty_mask )
    {
        return WICED_FALSE;
    }
    else
    {
        return WICED_TRUE;
    }
}

platform_result_t uart_slow_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    platform_result_t result = PLATFORM_SUCCESS;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );

    PLATFORM_ALP_CLOCK_RES_UP();

    do
    {
        if ( uart_slow_wait_transmit_fifo_empty() != WICED_TRUE )
        {
            result = PLATFORM_TIMEOUT;
            break;
        }

        uart_slow_base->rx_tx_dll = *data_out++;
        size--;

    } while ( size != 0 );

    PLATFORM_ALP_CLOCK_RES_DOWN( uart_slow_wait_transmit_fifo_empty, WICED_FALSE );

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    return result;
}

static wiced_bool_t uart_fast_transmit_fifo_empty( void )
{
    return ( CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_TX_FIFO_ALMOST_EMPTY ) ? WICED_TRUE : WICED_FALSE;
}

platform_result_t uart_fast_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );

    do
    {
        wwd_time_t   start_time   = host_rtos_get_time();
        wwd_time_t   elapsed_time = 0;

        /* Wait till there is space in the TX FIFO of the Fast UART */
        while ( (CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_TX_FIFO_FULL) && (elapsed_time < UART_FAST_MAX_TRANSMIT_WAIT_TIME) )
        {
            elapsed_time = host_rtos_get_time( ) - start_time;
        }

        if ( CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_TX_FIFO_FULL )
        {
            return PLATFORM_TIMEOUT;
        }

        uart_fast_base->data = *data_out++;
        size--;

    } while ( size != 0 );

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    return PLATFORM_SUCCESS;
}

platform_result_t uart_gci_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_TX );
    do
    {
        wwd_time_t   start_time   = host_rtos_get_time();
        wwd_time_t   elapsed_time = 0;

        /* Wait till there is space in the TX FIFO of the Fast UART */
        while ( (GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_TX_FIFO_FULL) && (elapsed_time < UART_GCI_MAX_TRANSMIT_WAIT_TIME) )
        {
            elapsed_time = host_rtos_get_time( ) - start_time;
        }

        if ( GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_TX_FIFO_FULL )
        {
            return PLATFORM_TIMEOUT;
        }

        uart_gci_base->data = *data_out++;
        size--;

    } while ( size != 0 );

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    return PLATFORM_SUCCESS;
}

static platform_result_t platform_uart_transmit_bytes_common( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    if ( driver->interface->port == UART_SLOW )
    {
        return uart_slow_transmit_bytes( driver, data_out, size );
    }
    else if ( driver->interface->port == UART_FAST )
    {
        return uart_fast_transmit_bytes( driver, data_out, size );
    }
    else if ( driver->interface->port == UART_GCI )
    {
        return uart_gci_transmit_bytes( driver, data_out, size );
    }

    return PLATFORM_UNSUPPORTED;
}

platform_result_t platform_uart_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    platform_result_t p_result;
    wiced_assert( "bad argument", ( driver != NULL ) && ( data_out != NULL ) && ( size != 0 ) );

#ifdef CONS_LOG_BUFFER_SUPPORT
#ifdef CONS_LOG_BUFFER_ONLY
    return cons_transmit_bytes(driver, data_out, size);
#else
    cons_transmit_bytes(driver, data_out, size);
#endif  /* CONS_LOG_BUFFER_ONLY */
#endif  /* CONS_LOG_BUFFER_SUPPORT */

#ifdef WICED_POWER_LOGGER_ENABLE
    /* Wait for 10 secs to get Tx semaphore. Timed wait would avoid blocking in error scenarios */
    p_result = (platform_result_t) host_rtos_get_semaphore( &driver->tx_complete, 10000, WICED_FALSE );
    if ( p_result != PLATFORM_SUCCESS )
    {
        WPRINT_PLATFORM_ERROR( ("platform_uart_transmit_bytes: Not able to transmit on UART, error: %d\n", result) );
        return PLATFORM_ERROR;
    }
#endif

    p_result = platform_uart_transmit_bytes_common( driver, data_out, size );
#ifdef WICED_POWER_LOGGER_ENABLE
    host_rtos_set_semaphore( &driver->tx_complete, WICED_FALSE);
#endif
    return p_result;
}

platform_result_t platform_uart_exception_transmit_bytes( platform_uart_driver_t* driver, const uint8_t* data_out, uint32_t size )
{
    /* Called in exception context and must not use interrupts. */

    if ( ( driver == NULL ) || ( data_out == NULL ) || ( size == 0 ) )
    {
        return PLATFORM_UNSUPPORTED;
    }

    return platform_uart_transmit_bytes_common( driver, data_out, size );
}

platform_result_t platform_uart_receive_bytes( platform_uart_driver_t* driver, uint8_t* data_in, uint32_t* expected_data_size, uint32_t timeout_ms )
{
    platform_result_t result = PLATFORM_UNSUPPORTED;
    uint32_t          bytes_left;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_RX );

    wiced_assert( "bad argument", ( driver != NULL ) && ( data_in != NULL ) && ( expected_data_size != NULL )  && ( *expected_data_size != 0 ) );

    bytes_left = *expected_data_size;

    if ( driver->interface->port == UART_SLOW )
    {
#ifndef BCM4390X_UART_SLOW_POLL_MODE
        result = uart_receive_bytes_irq( driver, data_in, &bytes_left, timeout_ms );
#else
        result = uart_slow_receive_bytes_poll( driver, data_in, &bytes_left, timeout_ms );
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */
    }
    else if ( driver->interface->port == UART_FAST )
    {
#ifndef BCM4390X_UART_FAST_POLL_MODE
        result = uart_receive_bytes_irq( driver, data_in, &bytes_left, timeout_ms );
#else
        result = uart_fast_receive_bytes_poll( driver, data_in, &bytes_left, timeout_ms );
#endif /* !BCM4390X_UART_FAST_POLL_MODE */
    }
    else if ( driver->interface->port == UART_GCI )
    {
#ifndef BCM4390X_UART_GCI_POLL_MODE
        result = uart_receive_bytes_irq( driver, data_in, &bytes_left, timeout_ms );
#else
        result = uart_gci_receive_bytes_poll( driver, data_in, &bytes_left, timeout_ms );
#endif /* !BCM4390X_UART_GCI_POLL_MODE */
    }

    /* Return read bytes count */
    *expected_data_size -= bytes_left;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );

    return result;
}

/******************************************************
 *            IRQ Handlers Definition
 ******************************************************/

#ifndef BCM4390X_UART_SLOW_POLL_MODE

/* Read one byte from Slow Uart */
void platform_uart_receive_byte( platform_uart_port_t port, uint8_t* dest )
{
    switch ( port )
    {
        case UART_SLOW:
            *dest = uart_slow_base->rx_tx_dll;
            break;

        case UART_FAST:
            *dest = uart_fast_base->data;
            break;

        default:
            break;
    }
}

/*
 * Slow UART interrupt service routine
 */
static void uart_slow_irq( platform_uart_driver_t* driver )
{
    uint32_t int_status;
    uint32_t int_mask;

    int_mask = uart_slow_base->ier_dlm;
    int_status = uart_slow_base->iir_fcr & UART_SLOW_IIR_INT_ID_MASK;

    /* Turn off slow UART interrupts */
    platform_uart_disable_interrupts(driver->interface->port);

    if ( (int_status == UART_SLOW_IIR_INT_ID_RXRDY) && (int_mask & UART_SLOW_IER_RXRDY) )
    {
        if (driver->rx_buffer != NULL)
        {
            /*
             * Check whether the ring buffer is already about to overflow.
             * This condition cannot happen during correct operation of the driver, but checked here only as precaution
             * to protect against ring buffer overflows for e.g. if UART interrupts got inadvertently enabled elsewhere.
             */
            if ( ( ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size ) == driver->rx_buffer->head )
            {
                /* RX overflow error counter */
                driver->rx_overflow++;

                /* Slow UART interrupts remain turned off */
                return;
            }

            /* Transfer data from the slow UART RX Buffer Register into the ring buffer */
            driver->rx_buffer->buffer[driver->rx_buffer->tail] = uart_slow_base->rx_tx_dll;
            driver->rx_buffer->tail = ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size;

            /* Set the semaphore whenever a byte is produced into the ring buffer */
            host_rtos_set_semaphore( &driver->rx_complete, WICED_TRUE );

            /* Check whether the ring buffer is about to overflow */
            if ( ( ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size ) == driver->rx_buffer->head )
            {
                /* Slow UART interrupts remain turned off */
                return;
            }
        }
        else
        {
            /* Slow UART interrupts remain turned off */
            return;
        }
    }

    /* Turn on slow UART interrupts */
    platform_uart_enable_interrupts(driver->interface->port);
}
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */

#if !defined(BCM4390X_UART_FAST_POLL_MODE) || !defined(BCM4390X_UART_GCI_POLL_MODE)
static wiced_bool_t uart_seci_process_irq( platform_uart_driver_t* driver, uart_seci_t* uart_seci_base )
{
    if (driver->rx_buffer == NULL)
    {
        /* SECI UART interrupts remain turned off */
        return WICED_FALSE;
    }

    /*
     * Check whether the ring buffer is already about to overflow.
     * This condition cannot happen during correct operation of the driver, but checked here only as precaution
     * to protect against ring buffer overflows for e.g. if UART interrupts got inadvertently enabled elsewhere.
     */
    if ( ( ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size ) == driver->rx_buffer->head )
    {
        /* RX overflow error counter */
        driver->rx_overflow++;

        /* SECI UART interrupts remain turned off */
        return WICED_FALSE;
    }

    /* Transfer data from the SECI UART Data Register into the ring buffer */
    driver->rx_buffer->buffer[driver->rx_buffer->tail] = uart_seci_base->data;
    driver->rx_buffer->tail = ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size;

    if ( ring_buffer_used_space(driver->rx_buffer) == 1 )
    {
        /* Set the semaphore to indicate the ring buffer is not empty */
        host_rtos_set_semaphore( &driver->rx_complete, WICED_TRUE );
    }

    /* Check whether the ring buffer is about to overflow */
    if ( ( ( driver->rx_buffer->tail + 1 ) % driver->rx_buffer->size ) == driver->rx_buffer->head )
    {
        /* SECI UART interrupts remain turned off */
        return WICED_FALSE;
    }

    return WICED_TRUE;
}
#endif /* !BCM4390X_UART_FAST_POLL_MODE || !BCM4390X_UART_GCI_POLL_MODE */

#ifndef BCM4390X_UART_FAST_POLL_MODE
/*
 * Fast UART interrupt service routine
 */
static void uart_fast_irq( platform_uart_driver_t* driver )
{
    uint32_t int_status;
    uint32_t int_mask;

    int_mask = CHIPCOMMON_SECI_STATUS_MASK_REG;
    int_status = CHIPCOMMON_SECI_STATUS_REG;

    /* Turn off fast UART interrupts */
    platform_uart_disable_interrupts(driver->interface->port);

    if ( ((int_status & CC_SECI_STATUS_RX_FIFO_ALMOST_FULL) && (int_mask & CC_SECI_STATUS_RX_FIFO_ALMOST_FULL)) ||
         ((int_status & CC_SECI_STATUS_RX_IDLE_TIMER_INT) && (int_mask & CC_SECI_STATUS_RX_IDLE_TIMER_INT)) )
    {
        /* Drain the fast UART RX FIFO */
        while ( !(CHIPCOMMON_SECI_STATUS_REG & CC_SECI_STATUS_RX_FIFO_EMPTY) )
        {
            if ( uart_seci_process_irq(driver, uart_fast_base) != WICED_TRUE )
            {
                /* Fast UART interrupts remain turned off */
                return;
            }
        }
    }

    /* Turn on fast UART interrupts */
    platform_uart_enable_interrupts(driver->interface->port);
}
#endif /* !BCM4390X_UART_FAST_POLL_MODE */

#ifndef BCM4390X_UART_GCI_POLL_MODE
/*
 * GCI UART interrupt service routine
 */
static void uart_gci_irq( platform_uart_driver_t* driver )
{
    uint32_t int_status;
    uint32_t int_mask;

    int_mask = GCI_INT_MASK_REG;
    int_status = GCI_INT_STATUS_REG;

    /* Turn off GCI UART interrupts */
    platform_uart_disable_interrupts(driver->interface->port);

    if ( ((int_status & GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL) && (int_mask & GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL)) ||
         ((int_status & GCI_INT_ST_MASK_SECI_RX_IDLE_TIMER_INT) && (int_mask & GCI_INT_ST_MASK_SECI_RX_IDLE_TIMER_INT)) )
    {
        /* Drain the GCI UART RX FIFO */
        while ( (GCI_INT_STATUS_REG & GCI_INT_ST_MASK_SECI_RX_FIFO_NOT_EMPTY) )
        {
            if ( uart_seci_process_irq(driver, uart_gci_base) != WICED_TRUE )
            {
                /* GCI UART interrupts remain turned off */
                return;
            }
        }
    }

    /* Turn on GCI UART interrupts */
    platform_uart_enable_interrupts(driver->interface->port);
}
#endif /* !BCM4390X_UART_GCI_POLL_MODE */

static uint32_t uart_slow_irq_txrx_ready( void )
{
    uint32_t int_recvd = UART_NO_INTERRUPT;

#ifndef BCM4390X_UART_SLOW_POLL_MODE
    uint32_t int_status = uart_slow_base->iir_fcr & UART_SLOW_IIR_INT_ID_MASK;

    if ( int_status & UART_SLOW_IIR_INT_ID_RXRDY )
    {
        int_recvd |= UART_RX_READY;
    }
    if ( int_status & UART_SLOW_IIR_INT_ID_THRE )
    {
        int_recvd |= UART_TX_READY;
    }
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */

    return int_recvd;
}

static uint32_t uart_fast_irq_txrx_ready( void )
{
    uint32_t int_recvd = UART_NO_INTERRUPT;

#ifndef BCM4390X_UART_FAST_POLL_MODE
    uint32_t int_status = CHIPCOMMON_SECI_STATUS_REG;

    if ( int_status & CC_SECI_STATUS_RX_FIFO_ALMOST_FULL )
    {
        int_recvd |= UART_RX_READY;
    }
    if ( int_status & CC_SECI_STATUS_TX_FIFO_ALMOST_EMPTY )
    {
        int_recvd |= UART_TX_READY;
    }
#endif /* !BCM4390X_UART_FAST_POLL_MODE */

    return int_recvd;
}

static uint32_t uart_gci_irq_txrx_ready( void )
{
    uint32_t int_recvd = UART_NO_INTERRUPT;

#ifndef BCM4390X_UART_GCI_POLL_MODE
    uint32_t int_status = GCI_INT_STATUS_REG;

    if ( int_status & GCI_INT_ST_MASK_SECI_RX_FIFO_ALMOST_FULL )
    {
        int_recvd |= UART_RX_READY;
    }
    if ( int_status & GCI_INT_ST_MASK_SECI_TX_FIFO_ALMOST_EMPTY )
    {
        int_recvd |= UART_TX_READY;
    }
#endif /* !BCM4390X_UART_GCI_POLL_MODE */

    return int_recvd;
}

uint32_t platform_uart_irq_txrx_ready( platform_uart_port_t port )
{
    if ( port == UART_SLOW )
    {
        return uart_slow_irq_txrx_ready( );
    }
    else if ( port == UART_FAST )
    {
        return uart_fast_irq_txrx_ready( );
    }
    else if ( port == UART_GCI )
    {
        return uart_gci_irq_txrx_ready( );
    }

    return UART_NO_INTERRUPT;
}

/*
 * Platform UART interrupt service routine
 */
WEAK NEVER_INLINE void platform_uart_irq( platform_uart_driver_t* driver )
{
    if (driver->interface->port == UART_SLOW)
    {
#ifndef BCM4390X_UART_SLOW_POLL_MODE
        uart_slow_irq(driver);
#endif /* !BCM4390X_UART_SLOW_POLL_MODE */
    }
    else if (driver->interface->port == UART_FAST)
    {
#ifndef BCM4390X_UART_FAST_POLL_MODE
        uart_fast_irq(driver);
#endif /* !BCM4390X_UART_FAST_POLL_MODE */
    }
    else if (driver->interface->port == UART_GCI)
    {
#ifndef BCM4390X_UART_GCI_POLL_MODE
        uart_gci_irq(driver);
#endif /* !BCM4390X_UART_GCI_POLL_MODE */
    }
}

WICED_SLEEP_EVENT_HANDLER( platform_uart_sleep_handler )
{
    switch ( event )
    {
        case WICED_SLEEP_EVENT_ENTER:
            /* In case of Deep-Sleep, Set RTS to high to indicate APPS is not ready to
             * receive data
             */
            if ( platform_mcu_powersave_get_mode( ) == PLATFORM_MCU_POWERSAVE_MODE_DEEP_SLEEP )
            {
                /* Drain RX and TX FIFO's */
                CHIPCOMMON_SECI_UART_FCR_REG |= 0x3 ;

                /* Disable HW Flow control to assert RTS high */
                CHIPCOMMON_SECI_UART_MCR_REG &= ~0x20;
            }
            WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_UART, EVENT_DESC_UART_IDLE );
            break;

        case WICED_SLEEP_EVENT_CANCEL:
            break;

        case WICED_SLEEP_EVENT_LEAVE:
            break;

        default:
            break;
    }
}
