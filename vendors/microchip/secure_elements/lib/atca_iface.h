/**
 * \file
 *
 * \brief  Microchip Crypto Auth hardware interface object
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

#ifndef ATCA_IFACE_H
#define ATCA_IFACE_H
/*lint +flb */

/** \defgroup interface ATCAIface (atca_)
 *  \brief Abstract interface to all CryptoAuth device types.  This interface
 *  connects to the HAL implementation and abstracts the physical details of the
 *  device communication from all the upper layers of CryptoAuthLib
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

#include "atca_devtypes.h"
#include <stdint.h>
#include "atca_status.h"


typedef enum
{
    ATCA_I2C_IFACE = 0,         /**< Native I2C Driver */
    ATCA_SWI_IFACE = 1,         /**< SWI or 1-Wire over UART/USART */
    ATCA_UART_IFACE = 2,        /**< Kit v1 over UART/USART */
    ATCA_SPI_IFACE = 3,         /**< Native SPI Driver */
    ATCA_HID_IFACE = 4,         /**< Kit v1 over HID */
    ATCA_KIT_IFACE = 5,         /**< Kit v2 (Binary/Bridging) */
    ATCA_CUSTOM_IFACE = 6,      /**< Custom HAL functions provided during interface init */
    ATCA_I2C_GPIO_IFACE = 7,    /**< I2C "Bitbang" Driver */
    ATCA_SWI_GPIO_IFACE = 8,    /**< SWI or 1-Wire using a GPIO */
    ATCA_SPI_GPIO_IFACE = 9,    /**< SWI or 1-Wire using a GPIO */

    // additional physical interface types here
    ATCA_UNKNOWN_IFACE = 0xFE
} ATCAIfaceType;


/*The types are used within the kit protocol to identify the correct interface*/
typedef enum
{   ATCA_KIT_AUTO_IFACE,        //Selects the first device if the Kit interface is not defined
    ATCA_KIT_I2C_IFACE,
    ATCA_KIT_SWI_IFACE,
    ATCA_KIT_SPI_IFACE,
    ATCA_KIT_UNKNOWN_IFACE } ATCAKitType;


/* ATCAIfaceCfg is the configuration object for a device
 */

typedef struct
{

    ATCAIfaceType  iface_type;      // active iface - how to interpret the union below
    ATCADeviceType devtype;         // explicit device type

    union                           // each instance of an iface cfg defines a single type of interface
    {
        struct
        {
#ifdef ATCA_ENABLE_DEPRECATED
            uint8_t slave_address;  // 8-bit slave address
#else
            uint8_t address;        /**< Device address - the upper 7 bits are the I2c address bits */
#endif
            uint8_t  bus;           // logical i2c bus number, 0-based - HAL will map this to a pin pair for SDA SCL
            uint32_t baud;          // typically 400000
        } atcai2c;

        struct
        {
            uint8_t address;        // 7-bit device address
            uint8_t bus;            // logical SWI bus - HAL will map this to a pin	or uart port
        } atcaswi;

        struct
        {
            uint8_t  bus;           // logical i2c bus number, 0-based - HAL will map this to a spi pheripheral
            uint8_t  select_pin;    // CS pin line typically 0
            uint32_t baud;          // typically 16000000
        } atcaspi;

        struct
        {
            int      port;      // logic port number
            uint32_t baud;      // typically 115200
            uint8_t  wordsize;  // usually 8
            uint8_t  parity;    // 0 == even, 1 == odd, 2 == none
            uint8_t  stopbits;  // 0,1,2
        } atcauart;

        struct
        {
            int         idx;           // HID enumeration index
            ATCAKitType dev_interface; // Kit interface type
            uint8_t     dev_identity;  // I2C address for the I2C interface device or the bus number for the SWI interface device.
            uint32_t    vid;           // Vendor ID of kit (0x03EB for CK101)
            uint32_t    pid;           // Product ID of kit (0x2312 for CK101)
            uint32_t    packetsize;    // Size of the USB packet
        } atcahid;

        struct
        {
            ATCAKitType dev_interface; // Target Bus Type
            uint8_t     dev_identity;  // Target device identity
            uint32_t    flags;
        } atcakit;

        struct
        {
            ATCA_STATUS (*halinit)(void *hal, void *cfg);
            ATCA_STATUS (*halpostinit)(void *iface);
            ATCA_STATUS (*halsend)(void *iface, uint8_t word_address, uint8_t *txdata, int txlength);
            ATCA_STATUS (*halreceive)(void *iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxlength);
            ATCA_STATUS (*halwake)(void *iface);
            ATCA_STATUS (*halidle)(void *iface);
            ATCA_STATUS (*halsleep)(void *iface);
            ATCA_STATUS (*halrelease)(void* hal_data);
        } atcacustom;

    };

    uint16_t wake_delay;    // microseconds of tWHI + tWLO which varies based on chip type
    int      rx_retries;    // the number of retries to attempt for receiving bytes
    void *   cfg_data;      // opaque data used by HAL in device discovery
} ATCAIfaceCfg;



typedef struct atca_iface * ATCAIface;

/** \brief HAL Driver Structure
 */
typedef struct
{
    ATCA_STATUS (*halinit)(ATCAIface iface, ATCAIfaceCfg* cfg);
    ATCA_STATUS (*halpostinit)(ATCAIface iface);
    ATCA_STATUS (*halsend)(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength);
    ATCA_STATUS (*halreceive)(ATCAIface iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxlength);
    ATCA_STATUS (*halcontrol)(ATCAIface iface, uint8_t option, void* param, size_t paramlen);
    ATCA_STATUS (*halrelease)(void* hal_data);
} ATCAHAL_t;

/** \brief atca_iface is the context structure for a configured interface
 */
typedef struct atca_iface
{
    ATCAIfaceCfg* mIfaceCFG;    /**< Points to previous defined/given Cfg object, the caller manages this */
    ATCAHAL_t*    hal;          /**< The configured HAL for the interface */
    ATCAHAL_t*    phy;          /**< When a HAL is not a "native" hal it needs a physical layer to be associated with it */
    void*         hal_data;     /**< Pointer to HAL specific context/data */
} atca_iface_t;

ATCA_STATUS initATCAIface(ATCAIfaceCfg *cfg, ATCAIface ca_iface);
ATCAIface newATCAIface(ATCAIfaceCfg *cfg);
ATCA_STATUS releaseATCAIface(ATCAIface ca_iface);
void deleteATCAIface(ATCAIface *ca_iface);

// IFace methods
ATCA_STATUS atinit(ATCAIface ca_iface);
ATCA_STATUS atsend(ATCAIface ca_iface, uint8_t word_address, uint8_t *txdata, int txlength);
ATCA_STATUS atreceive(ATCAIface ca_iface, uint8_t word_address, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS atcontrol(ATCAIface ca_iface, uint8_t option, void* param, size_t paramlen);
ATCA_STATUS atwake(ATCAIface ca_iface);
ATCA_STATUS atidle(ATCAIface ca_iface);
ATCA_STATUS atsleep(ATCAIface ca_iface);

// accessors
ATCAIfaceCfg * atgetifacecfg(ATCAIface ca_iface);
void* atgetifacehaldat(ATCAIface ca_iface);

/* Utilities */
bool atca_iface_is_kit(ATCAIface ca_iface);
int atca_iface_get_retries(ATCAIface ca_iface);
uint16_t atca_iface_get_wake_delay(ATCAIface ca_iface);

#ifdef __cplusplus
}
#endif
/*lint -flb*/
/** @} */
#endif



