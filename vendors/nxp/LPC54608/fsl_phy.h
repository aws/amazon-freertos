/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_PHY_H_
#define _FSL_PHY_H_

#include "fsl_enet.h"

/*!
 * @addtogroup phy_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief PHY driver version */
#define FSL_PHY_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0. */

/*! @brief Defines the PHY registers. */
#define PHY_BASICCONTROL_REG 0x00U      /*!< The PHY basic control register. */
#define PHY_BASICSTATUS_REG 0x01U       /*!< The PHY basic status register. */
#define PHY_ID1_REG 0x02U               /*!< The PHY ID one register. */
#define PHY_ID2_REG 0x03U               /*!< The PHY ID two register. */
#define PHY_AUTONEG_ADVERTISE_REG 0x04U /*!< The PHY auto-negotiate advertise register. */
#define PHY_SEPCIAL_CONTROL_REG 0x1FU   /*!< The PHY control two register. */

#define PHY_CONTROL_ID1 0x07U /*!< The PHY ID1*/

/*! @brief Defines the mask flag in basic control register. */
#define PHY_BCTL_DUPLEX_MASK 0x0100U          /*!< The PHY duplex bit mask. */
#define PHY_BCTL_RESTART_AUTONEG_MASK 0x0200U /*!< The PHY restart auto negotiation mask. */
#define PHY_BCTL_AUTONEG_MASK 0x1000U         /*!< The PHY auto negotiation bit mask. */
#define PHY_BCTL_SPEED_MASK 0x2000U           /*!< The PHY speed bit mask. */
#define PHY_BCTL_LOOP_MASK 0x4000U            /*!< The PHY loop bit mask. */
#define PHY_BCTL_RESET_MASK 0x8000U           /*!< The PHY reset bit mask. */

/*!@brief Defines the mask flag of operation mode in special control register*/
#define PHY_SPECIALCTL_AUTONEGDONE_MASK 0x1000U /*!< The PHY auto-negotiation complete mask. */
#define PHY_SPECIALCTL_DUPLEX_MASK 0x0010U      /*!< The PHY duplex mask. */
#define PHY_SPECIALCTL_100SPEED_MASK 0x0008U    /*!< The PHY speed mask. */
#define PHY_SPECIALCTL_10SPEED_MASK 0x0004U     /*!< The PHY speed mask. */
#define PHY_SPECIALCTL_SPEEDUPLX_MASK 0x001cU   /*!< The PHY speed and duplex mask. */

/*! @brief Defines the mask flag in basic status register. */
#define PHY_BSTATUS_LINKSTATUS_MASK 0x0004U /*!< The PHY link status mask. */

/*! @brief Defines the mask flag in PHY auto-negotiation advertise register. */
#define PHY_ALL_CAPABLE_MASK 0x1e0U

/*! @brief Defines the PHY link speed. This is align with the speed for ENET MAC. */
typedef enum _phy_speed {
    kPHY_Speed10M = 0U, /*!< ENET PHY 10M speed. */
    kPHY_Speed100M      /*!< ENET PHY 100M speed. */
} phy_speed_t;

/*! @brief Defines the PHY link duplex. */
typedef enum _phy_duplex {
    kPHY_HalfDuplex = 0U, /*!< ENET PHY half duplex. */
    kPHY_FullDuplex       /*!< ENET PHY full duplex. */
} phy_duplex_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
  * @name PHY Driver
  * @{
  */

/*!
 * @brief Initializes PHY.
 *
 *  This function initialize the SMI interface and initialize PHY.
 *  The SMI is the MII management interface between PHY and MAC, which should be
 *  firstly initialized before any other operation for PHY.
 *
 * @param base       ENET peripheral base address.
 * @param phyAddr    The PHY address.
 * @retval kStatus_Success  PHY initialize success
 * @retval kStatus_Fail  PHY initialize fail
 */
status_t PHY_Init(ENET_Type *base, uint32_t phyAddr);

/*!
 * @brief PHY Write function. This function write data over the SMI to
 * the specified PHY register. This function is called by all PHY interfaces.
 *
 * @param base    ENET peripheral base address.
 * @param phyAddr The PHY address.
 * @param phyReg  The PHY register.
 * @param data    The data written to the PHY register.
 */
void PHY_Write(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t data);

/*!
 * @brief PHY Read function. This interface read data over the SMI from the
 * specified PHY register. This function is called by all PHY interfaces.
 *
 * @param base     ENET peripheral base address.
 * @param phyAddr  The PHY address.
 * @param phyReg   The PHY register.
 * @return The data read from the phy register.
 */
uint16_t PHY_Read(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg);

/*!
 * @brief Gets the PHY link status.
 *
 * @param base     ENET peripheral base address.
 * @param phyAddr  The PHY address.
 * @retval The link up or down status of the PHY.
 *         - true the link is up.
 *         - false the link is down.
 */
bool PHY_GetLinkStatus(ENET_Type *base, uint32_t phyAddr);

/*!
 * @brief Gets the PHY link speed and duplex.
 *
 * @param base     ENET peripheral base address.
 * @param phyAddr  The PHY address.
 * @param speed    The address of PHY link speed.
 * @param duplex   The link duplex of PHY.
 */
void PHY_GetLinkSpeedDuplex(ENET_Type *base, uint32_t phyAddr, phy_speed_t *speed, phy_duplex_t *duplex);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_PHY_H_ */
