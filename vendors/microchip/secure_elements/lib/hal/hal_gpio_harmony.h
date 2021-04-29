/**
 * \file
 * \brief ATCA Hardware abstraction layer for SWI over GPIO drivers.
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef HAL_GPIO_HARMONY
#define HAL_GPIO_HARMONY

#include <stdlib.h>
#include "cryptoauthlib.h"
#include "atca_status.h"
#include "atca_hal.h"
#include "atca_config.h"

/**
 * \name Macros for Bit-Banged 1WIRE Timing
 *
 * Times to drive bits at 230.4 kbps.
   @{ */

#define tPUP                0
#define tDSCHG              150 //min spec = 150us
#define tRESET              96  //min spec = 480us (STD Speed)
#define tRRT                1   //min spec = 1ms
#define tDRR                1   //min spec = 1us; max spec = 2us
#define tMSDR               2   //min spec = 2us; max spec = 6us
#define tHTSS               150 //min spec = 150us
#define tDACK               2   //min spec = 2us; max spec = 6

#define tDACK_DLY           atca_delay_us(tDACK)
#define tRRT_DLY            atca_delay_ms(tRRT)
#define tDRR_DLY            atca_delay_us(tDRR)
#define tMSDR_DLY           atca_delay_us(tMSDR)
#define tDSCHG_DLY          atca_delay_us(tDSCHG)
#define tRESET_DLY          atca_delay_us(tRESET)
#define tHTSS_DLY           atca_delay_us(tHTSS)

#define tLOW0_MIN           6
#define tLOW0_MAX           16
#define tLOW1_MIN           1
#define tLOW1_MAX           2

#define tRCV_MIN            4
#define tRCV_MAX            6

#define tBIT_MIN            (tLOW0_MIN + tPUP + tRCV_MIN)
#define tBIT_MAX            75
#define tWAKEUP             1                                           //min spec = 1ms

#define tLOW0_TYPICAL       (tLOW0_MIN + ((tLOW0_MAX - tLOW0_MIN) / 2)) //creates typical data timing (AVG of Min and Max)
#define tLOW1_TYPICAL       (tLOW1_MIN + ((tLOW1_MAX - tLOW1_MIN) / 2)) //creates typical data timing (AVG of Min and Max)
#define tBIT_TYPICAL        (tBIT_MIN + ((tBIT_MAX - tBIT_MIN) / 2 ))   //creates typical data timing (AVG of Min and Max)

#define tLOW0_HDLY          atca_delay_us(11)                           //min spec = 6us; max spec = 16us
#define tRD_HDLY            atca_delay_us(1)                            //min spec = 1us; max spec = 2us
#define tLOW1_HDLY          atca_delay_us(1)                            //min spec = 1us; max spec = 2us
#define tRCV0_HDLY          atca_delay_us(11)
#define tRCV1_HDLY          atca_delay_us(14)

#define tRD_DLY             atca_delay_us(1)  //min spec = 1us; max spec = 2us
#define tHIGH_SPEED_DLY     atca_delay_us(1)
#define tSWIN_DLY           atca_delay_us(1)  //delay to put master strobe within sample window

#define tLOW0_DLY           atca_delay_us(tLOW0_TYPICAL)
#define tLOW1_DLY           atca_delay_us(tLOW1_TYPICAL)

#define tBIT_DLY            atca_delay_us(tBIT_TYPICAL)
#define tRCV0_DLY           atca_delay_us(tBIT_TYPICAL - tLOW0_TYPICAL)
#define tRCV1_DLY           atca_delay_us(tBIT_TYPICAL - tLOW1_TYPICAL)

#define send_logic0_1wire(...)      send_logic_bit(__VA_ARGS__, ATCA_GPIO_LOGIC_BIT0)
#define send_logic1_1wire(...)      send_logic_bit(__VA_ARGS__, ATCA_GPIO_LOGIC_BIT1)
#define send_ACK_1wire(...)         send_logic0_1wire(__VA_ARGS__)
#define send_NACK_1wire(...)        send_logic1_1wire(__VA_ARGS__)

#define ATCA_1WIRE_RESET_WORD_ADDR              0x00
#define ATCA_1WIRE_SLEEP_WORD_ADDR              0x01
#define ATCA_1WIRE_SLEEP_WORD_ADDR_ALTERNATE    0x02
#define ATCA_1WIRE_COMMAND_WORD_ADDR            0x03

#define ATCA_1WIRE_RESPONSE_LENGTH_SIZE         0x01
#define ATCA_1WIRE_BIT_MASK                     0x80

#define ATCA_GPIO_WRITE                         0
#define ATCA_GPIO_READ                          1
#define ATCA_GPIO_INPUT_DIR                     0
#define ATCA_GPIO_OUTPUT_DIR                    1
#define ATCA_GPIO_LOGIC_BIT0                    0
#define ATCA_GPIO_LOGIC_BIT1                    1
#define ATCA_GPIO_ACK                           ATCA_GPIO_LOGIC_BIT0
#define ATCA_GPIO_CLEAR                         0
#define ATCA_GPIO_SET                           1
#define ATCA_MIN_RESPONSE_LENGTH                4


/**
 * \name Macros for Bit-Banged SWI Timing
 *
 * Times to drive bits at 230.4 kbps.
   @{ */

//! delay macro for width of one pulse (start pulse or zero pulse)
//! should be 4.34 us, is 4.05 us

#define BIT_DELAY_1L        atca_delay_us(4)
//! should be 4.34 us, is 4.05us
#define BIT_DELAY_1H        atca_delay_us(4)

//! time to keep pin high for five pulses plus stop bit (used to bit-bang CryptoAuth 'zero' bit)
//! should be 26.04 us, is 26.92 us
#define BIT_DELAY_5        atca_delay_us(26)    // considering pin set delay

//! time to keep pin high for seven bits plus stop bit (used to bit-bang CryptoAuth 'one' bit)
//! should be 34.72 us, is 35.13 us
#define BIT_DELAY_7        atca_delay_us(34)    // considering pin set delay

//! turn around time when switching from receive to transmit
//! should be 93 us (Setting little less value as there would be other process before these steps)
#define RX_TX_DELAY        atca_delay_us(65)


/** SWI WORD Address */
#define ATCA_SWI_WAKE_WORD_ADDR     ((uint8_t)0x00)
#define ATCA_SWI_CMD_WORD_ADDR      ((uint8_t)0x77)
#define ATCA_SWI_TX_WORD_ADDR       ((uint8_t)0x88)
#define ATCA_SWI_IDLE_WORD_ADDR     ((uint8_t)0xBB)
#define ATCA_SWI_SLEEP_WORD_ADDR    ((uint8_t)0xCC)
#define ATCA_SWI_BIT_MASK           0x01

typedef enum
{
    ATCA_PROTOCOL_1WIRE,
    ATCA_PROTOCOL_SWI,
    NO_OF_PROTOCOL
} protocol_type;

typedef enum
{
    LOGIC0_1,
    LOGIC0_2,
    LOGIC0_3,
    LOGIC0_4,
    LOGIC1_1,
    LOGIC1_2,
    NO_OF_DELAYS
} delay_type;

#endif
