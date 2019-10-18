/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MZ2048
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "atca_hal.h"
#include "atca_device.h"
#include "hal/hal_pic32mz2048efm_i2c.h"
#include "driver/i2c/drv_i2c.h"
#include "system_definitions.h"
#include "driver/i2c/src/drv_i2c_local.h"


DRV_HANDLE drvI2CMasterHandle;         //Returned from DRV_I2C_Open for I2C Master
DRV_HANDLE drvI2CMasterHandle1;        //Returned from DRV_I2C_Open for I2C Master
DRV_I2C_BUFFER_HANDLE write_bufHandle; //Returned from calling a Data Transfer function
DRV_I2C_BUFFER_HANDLE read_bufHandle;  //Returned from calling a Data Transfer function
static DRV_I2C_BUFFER_EVENT  bufferEvent;
static volatile bool isBufferCompleteEvent = true;
extern const DRV_I2C_INIT drvI2C0InitData;

DRV_I2C_Object cryptoObj = 
{
    .i2cDriverInstanceIndex = 0,
    .i2cDriverInit = &drvI2C0InitData
};


/**
 * \brief
 * Logical to physical bus mapping structure
 */
ATCAI2CMaster_t *i2c_hal_data[MAX_I2C_BUSES];   // map logical, 0-based bus number to index
int i2c_bus_ref_ct = 0;                         // total in-use count across buses
//twi_options_t opt_twi_master;

/** \brief discover any CryptoAuth devices on a given logical bus number
 * \param[in]  busNum  logical bus number on which to look for CryptoAuth devices
 * \param[out] cfg     pointer to head of an array of interface config structures which get filled in by this method
 * \param[out] found   number of devices found on this bus
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_discover_devices(int busNum, ATCAIfaceCfg cfg[], int *found)
{
    return ATCA_UNIMPLEMENTED;
}

/** \brief discover i2c buses available for this hardware
 * this maintains a list of logical to physical bus mappings freeing the application
 * of the a-priori knowledge
 * \param[in] i2c_buses  an array of logical bus numbers
 * \param[in] max_buses  maximum number of buses the app wants to attempt to discover
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_discover_buses(int i2c_buses[], int max_buses)
{
    i2c_buses[0] = 0;
    return ATCA_SUCCESS;
}

/** \brief initialize an I2C interface using given config
 * \param[in] hal  opaque ptr to HAL data
 * \param[in] cfg  interface configuration
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
    DRV_I2C_Object *tempI2cObj;
    int bus = cfg->atcai2c.bus;   // 0-based logical bus number
    int i;
    ATCAHAL_t *phal = (ATCAHAL_t*)hal;

    if (i2c_bus_ref_ct == 0)       // power up state, no i2c buses will have been used
    {
        for (i = 0; i < MAX_I2C_BUSES; i++)
        {
            i2c_hal_data[i] = NULL;
        }
    }

    i2c_bus_ref_ct++;  // total across buses

    if (bus >= 0 && bus < MAX_I2C_BUSES)
    {
        // if this is the first time this bus and interface has been created, do the physical work of enabling it
        if (i2c_hal_data[bus] == NULL)
        {
            i2c_hal_data[bus] = hal_malloc(sizeof(ATCAI2CMaster_t) );
            i2c_hal_data[bus]->ref_ct = 1;  // buses are shared, this is the first instance

            tempI2cObj = (DRV_I2C_Object*)(cfg->cfg_data);
            drvI2CMasterHandle = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
            if (drvI2CMasterHandle == DRV_HANDLE_INVALID)
            {
                return ATCA_COMM_FAIL;

            }

            // store this for use during the release phase
            i2c_hal_data[bus]->bus_index = bus;
            //   i2c_master_enable(&(i2c_hal_data[bus]->i2c_master_instance));
        }
        else
        {
            // otherwise, another interface already initialized the bus, so this interface will share it and any different
            // cfg parameters will be ignored...first one to initialize this sets the configuration
            i2c_hal_data[bus]->ref_ct++;
        }

        phal->hal_data = i2c_hal_data[bus];

        return ATCA_SUCCESS;
    }
    return ATCA_COMM_FAIL;
}

/** \brief HAL implementation of I2C post init
 * \param[in] iface  instance
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of I2C send over ASF
 * \param[in] iface     instance
 * \param[in] txdata    pointer to space to bytes to send
 * \param[in] txlength  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
uint32_t Debug_count = 0;
uint32_t bytes_transferred = 0;
ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    DRV_I2C_BUFFER_EVENT Transaction;

    // for this implementation of I2C with CryptoAuth chips, txdata is assumed to have ATCAPacket format

    // other device types that don't require i/o tokens on the front end of a command need a different hal_i2c_send and wire it up instead of this one
    // this covers devices such as ATSHA204A and ATECCx08A that require a word address value pre-pended to the packet
    // txdata[0] is using _reserved byte of the ATCAPacket

    txdata[0] = 0x03;   // insert the Word Address Value, Command token
    txlength++;         // account for word address value byte.
    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, txdata, txlength, NULL);
    if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
    {
        while (1)
        {
            ;
        }
    }
    do
    {
        Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, write_bufHandle);

        if (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE)
        {
            Debug_count++;
            bytes_transferred = DRV_I2C_BytesTransferred(drvI2CMasterHandle, write_bufHandle);
        }

        if (Transaction == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            return ATCA_COMM_FAIL;
        }
    }
    while (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE);

    return ATCA_SUCCESS;


}

/** \brief HAL implementation of I2C receive function for ASF I2C
 * \param[in] iface     instance
 * \param[in] rxdata    pointer to space to receive the data
 * \param[in] rxlength  ptr to expected number of receive bytes to request
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{

    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    DRV_I2C_BUFFER_EVENT Transaction = DRV_I2C_BUFFER_EVENT_ERROR;

    //int bus = cfg->atcai2c.bus;
    //int retries = cfg->rx_retries;

    read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle, cfg->atcai2c.slave_address, rxdata, *rxlength, NULL);
    do
    {
        Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, read_bufHandle);
        if (Transaction == DRV_I2C_BUFFER_EVENT_ERROR)
        {
            return ATCA_COMM_FAIL;
        }
    }
    while (Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Transaction != DRV_I2C_BUFFER_EVENT_ERROR);


    if (atCheckCrc(rxdata) != ATCA_SUCCESS)
    {
        return ATCA_COMM_FAIL;
    }

    return ATCA_SUCCESS;

}

/** \brief wake up CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to wakeup
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
    DRV_I2C_Object *tempI2cObj;
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    //int bus = cfg->atcai2c.bus;
    //int retries = cfg->rx_retries;
    uint32_t bdrt = cfg->atcai2c.baud;
    uint8_t data[4];
    uint8_t expected[4] = { 0x04, 0x11, 0x33, 0x43 };
    DRV_I2C_BUFFER_EVENT Rx_Transaction = DRV_I2C_BUFFER_EVENT_ERROR;

    data[0] = 0x00;

    if (bdrt != 100000)   // if not already at 100KHz, change it
    {
        uint32_t backupBaudRate;
        DRV_I2C_INIT *tempi2cDriverInit;

        DRV_I2C_Close(drvI2CMasterHandle);

        /* DeInitialize Drivers */
        tempI2cObj = (DRV_I2C_Object*)(cfg->cfg_data);
        DRV_I2C_Deinitialize(tempI2cObj->i2cDriverInstance);

        /* Re-Initialize Drivers for ECC508A sleep configuration - I2C speed at 100KHZ */
        tempi2cDriverInit = (DRV_I2C_INIT*)tempI2cObj->i2cDriverInit;
        backupBaudRate = tempi2cDriverInit->baudRate;
        tempi2cDriverInit->baudRate = 100000;

        tempI2cObj->i2cDriverInstance = DRV_I2C_Initialize(tempI2cObj->i2cDriverInstanceIndex, (SYS_MODULE_INIT*)tempi2cDriverInit);
        drvI2CMasterHandle1 = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
        write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle1, cfg->atcai2c.slave_address, &data, 1, NULL);
        if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
        {
            while (1)
            {
                ;
            }
        }

        atca_delay_us(cfg->wake_delay); // wait tWHI + tWLO which is configured based on device type and configuration structure
        read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle1, cfg->atcai2c.slave_address, &data, 4, NULL);
        do
        {
            Rx_Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle1, read_bufHandle);
        }
        while (Rx_Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Rx_Transaction != DRV_I2C_BUFFER_EVENT_ERROR);

        DRV_I2C_Close(drvI2CMasterHandle1);

        /* DeInitialize Drivers */
        DRV_I2C_Deinitialize(tempI2cObj->i2cDriverInstance);

        /* Re-Initialize Drivers back for ECC508A default configuration*/
        tempi2cDriverInit->baudRate = backupBaudRate;
        tempI2cObj->i2cDriverInstance = DRV_I2C_Initialize(tempI2cObj->i2cDriverInstanceIndex, (SYS_MODULE_INIT*)tempi2cDriverInit);
        drvI2CMasterHandle = DRV_I2C_Open(tempI2cObj->i2cDriverInstanceIndex, DRV_IO_INTENT_NONBLOCKING | DRV_IO_INTENT_READWRITE);
    }
    else
    {
        // Send the wake by writing to an address of 0x00
        write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, 0x00, &data, 1, NULL);
        if ( ((DRV_I2C_BUFFER_HANDLE)(-1)) == write_bufHandle)
        {
            while (1)
            {
                ;
            }
        }
        atca_delay_us(cfg->wake_delay); // wait tWHI + tWLO which is configured based on device type and configuration structure
        read_bufHandle = DRV_I2C_Receive(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 4, NULL);
        do
        {
            Rx_Transaction = DRV_I2C_TransferStatusGet(drvI2CMasterHandle, read_bufHandle);
        }
        while (Rx_Transaction != DRV_I2C_BUFFER_EVENT_COMPLETE && Rx_Transaction != DRV_I2C_BUFFER_EVENT_ERROR);
    }

    if (memcmp(data, expected, 4) == 0)
    {
        return ATCA_SUCCESS;
    }
    return ATCA_COMM_FAIL;
}

/** \brief idle CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to idle
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[1];

    data[0] = 0x02;  // idle word address value
    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 1, NULL);

    return ATCA_SUCCESS;
}

/** \brief sleep CryptoAuth device using I2C bus
 * \param[in] iface  interface to logical device to sleep
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t data[4];

    data[0] = 0x01;  // sleep word address value

    write_bufHandle = DRV_I2C_Transmit(drvI2CMasterHandle, cfg->atcai2c.slave_address, &data, 1, NULL);

    return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data  opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{

    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    i2c_bus_ref_ct--;  // track total i2c bus interface instances for consistency checking and debugging

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0 && i2c_hal_data[hal->bus_index] != NULL)
    {
        DRV_I2C_Close(drvI2CMasterHandle);
        hal_free(i2c_hal_data[hal->bus_index]);
        i2c_hal_data[hal->bus_index] = NULL;
    }

    return ATCA_SUCCESS;

}
