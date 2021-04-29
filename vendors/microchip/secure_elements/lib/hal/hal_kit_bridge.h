/**
 * \file
 * \brief Kit Bridging HAL for cryptoauthlib. This is not intended to be a zero
 * copy driver. It should work with any interface that confirms to a few basic
 * requirements: a) will accept an arbitrary number of bytes and packetize it if
 * necessary for transmission, b) will block for the duration of the transmit.
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

#ifndef HAL_KIT_BRIDGE_H
#define HAL_KIT_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_KIT_COMMAND_SEND    0x01
#define HAL_KIT_COMMAND_RECV    0x02
#define HAL_KIT_COMMAND_WAKE    0x03
#define HAL_KIT_COMMAND_IDLE    0x04
#define HAL_KIT_COMMAND_SLEEP   0x05

#define HAL_KIT_HEADER_LEN      (3)

/* Kit Protocol Header defintion
 * Byte    Bits    Definition
 * ----    ----    -----------------
 * 0       0..4    Protocol Version
 * 0       5..8    Protocol Options
 * 1       0..4    Kit Command
 * 1       5..8    Interface Type
 * 2               Device Identity
 */

ATCA_STATUS hal_kit_attach_phy(ATCAIfaceCfg* cfg, atca_hal_kit_phy_t* phy);

#ifdef __cplusplus
}
#endif

#endif /* HAL_KIT_BRIDGE_H */
