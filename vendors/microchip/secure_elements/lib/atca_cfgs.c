/**
 * \file
 * \brief a set of default configurations for various ATCA devices and interfaces
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

#include <stddef.h>
#include "cryptoauthlib.h"
#include "atca_cfgs.h"
#include "atca_iface.h"
#include "atca_device.h"

/** \defgroup config Configuration (cfg_)
 * \brief Logical device configurations describe the CryptoAuth device type and logical interface.
   @{ */

/* if the number of these configurations grows large, we can #ifdef them based on required device support */

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_I2C)
/** \brief default configuration for an ECCx08A device */
ATCAIfaceCfg cfg_ateccx08a_i2c_default = {
    .iface_type                 = ATCA_I2C_IFACE,
    .devtype                    = ATECC608,
    {
#ifdef ATCA_ENABLE_DEPRECATED
        .atcai2c.slave_address  = 0xC0,
#else
        .atcai2c.address        = 0xC0,
#endif
        .atcai2c.bus            = 2,
        .atcai2c.baud           = 400000,
        //.atcai2c.baud = 100000,
    },
    .wake_delay                 = 1500,
    .rx_retries                 = 20
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_SWI)
/** \brief default configuration for an ECCx08A device on the logical SWI bus over UART*/
ATCAIfaceCfg cfg_ateccx08a_swi_default = {
    .iface_type         = ATCA_SWI_IFACE,
    .devtype            = ATECC608,
    {
        .atcaswi.bus    = 4,
    },
    .wake_delay         = 1500,
    .rx_retries         = 10
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_KIT_CDC)
/** \brief default configuration for Kit protocol over the device's async interface */
ATCAIfaceCfg cfg_ateccx08a_kitcdc_default = {
    .iface_type             = ATCA_UART_IFACE,
    .devtype                = ATECC608,
    {
        .atcauart.port      = 0,
        .atcauart.baud      = 115200,
        .atcauart.wordsize  = 8,
        .atcauart.parity    = 2,
        .atcauart.stopbits  = 1,
    },
    .rx_retries             = 1,
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_KIT_HID)
/** \brief default configuration for Kit protocol over the device's async interface */
ATCAIfaceCfg cfg_ateccx08a_kithid_default = {
    .iface_type                  = ATCA_HID_IFACE,
    .devtype                     = ATECC608,
    {
        .atcahid.dev_interface   = ATCA_KIT_AUTO_IFACE,
        .atcahid.dev_identity    = 0,
        .atcahid.idx             = 0,
        .atcahid.vid             = 0x03EB,
        .atcahid.pid             = 0x2312,
        .atcahid.packetsize      = 64,
    }
};
#endif

#if defined(ATCA_SHA_SUPPORT) && defined(ATCA_HAL_I2C)
/** \brief default configuration for a SHA204A device on the first logical I2C bus */
ATCAIfaceCfg cfg_atsha20xa_i2c_default = {
    .iface_type                 = ATCA_I2C_IFACE,
    .devtype                    = ATSHA204A,
    {
#ifdef ATCA_ENABLE_DEPRECATED
        .atcai2c.slave_address  = 0xC8,
#else
        .atcai2c.address        = 0xC8,
#endif
        .atcai2c.bus            = 2,
        .atcai2c.baud           = 400000,
    },
    .wake_delay                 = 2560,
    .rx_retries                 = 20
};
#endif

#if defined(ATCA_SHA_SUPPORT) && defined(ATCA_HAL_SWI)
/** \brief default configuration for an SHA20xA device on the logical SWI bus over UART*/
ATCAIfaceCfg cfg_atsha20xa_swi_default = {
    .iface_type       = ATCA_SWI_IFACE,
    .devtype          = ATSHA204A,
    {
        .atcaswi.bus  = 4,
    },
    .wake_delay       = 2560,
    .rx_retries       = 10
};
#endif

#if defined(ATCA_SHA_SUPPORT) && defined(ATCA_HAL_KIT_CDC)
/** \brief default configuration for Kit protocol over the device's async interface */
ATCAIfaceCfg cfg_atsha20xa_kitcdc_default = {
    .iface_type            = ATCA_UART_IFACE,
    .devtype               = ATSHA204A,
    {
        .atcauart.port     = 0,
        .atcauart.baud     = 115200,
        .atcauart.wordsize = 8,
        .atcauart.parity   = 2,
        .atcauart.stopbits = 1,
    },
    .rx_retries            = 1,
};
#endif

#if defined(ATCA_SHA_SUPPORT) && defined(ATCA_HAL_KIT_HID)
/** \brief default configuration for Kit protocol over the device's async interface */
ATCAIfaceCfg cfg_atsha20xa_kithid_default = {
    .iface_type            = ATCA_HID_IFACE,
    .devtype               = ATSHA204A,
    .atcahid.dev_interface = ATCA_KIT_AUTO_IFACE,
    .atcahid.dev_identity  = 0,
    .atcahid.idx           = 0,
    .atcahid.vid           = 0x03EB,
    .atcahid.pid           = 0x2312,
    .atcahid.packetsize    = 64,
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_I2C)
/** \brief default configuration for an ECC204 device on the first logical I2C bus */
ATCAIfaceCfg cfg_ecc204_i2c_default = {
    .iface_type                 = ATCA_I2C_IFACE,
    .devtype                    = ECC204,
    {
#ifdef ATCA_ENABLE_DEPRECATED
        .atcai2c.slave_address  = 0x33,
#else
        .atcai2c.address        = 0x33,
#endif
        .atcai2c.bus            = 2,
        .atcai2c.baud           = 400000,
        //.atcai2c.baud = 100000,
    },
    .wake_delay                 = 1500,
    .rx_retries                 = 20
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_SWI)
/** \brief default configuration for an ECC204 device on the logical SWI over GPIO*/
ATCAIfaceCfg cfg_ecc204_swi_default = {
    .iface_type         = ATCA_SWI_IFACE,
    .devtype            = ECC204,
    {
        .atcaswi.bus    = 4,
    },
    .wake_delay         = 1500,
    .rx_retries         = 10
};
#endif

#if defined(ATCA_ECC_SUPPORT) && defined(ATCA_HAL_KIT_HID)
/** \brief default configuration for Kit protocol over the device's async interface */
ATCAIfaceCfg cfg_ecc204_kithid_default = {
    .iface_type                  = ATCA_HID_IFACE,
    .devtype                     = ECC204,
    {
        .atcahid.dev_interface   = ATCA_KIT_AUTO_IFACE,
        .atcahid.dev_identity    = 0,
        .atcahid.idx             = 0,
        .atcahid.vid             = 0x03EB,
        .atcahid.pid             = 0x2312,
        .atcahid.packetsize      = 64,
    }
};
#endif

/** @} */
