/**/
#include "cryptoauthlib.h"
#include "atca_hal.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

typedef struct atca_spi_host_s
{
    char spi_file[20];
    int  f_spi;
} atca_spi_host_t;

/** \brief Open and configure the SPI device
 * \param[in]  dev_name  File name in the form /dev/spidevX.Y
 * \param[in]  speed     Clock speed in Hz
 * \param[out] fd        resulting file descriptor
 */
ATCA_STATUS hal_spi_open_file(const char * dev_name, uint32_t speed, int * fd)
{
    ATCA_STATUS status = ATCA_BAD_PARAM;
    int ret;

    if (dev_name && fd)
    {
        uint8_t mode = SPI_MODE_3;
        uint8_t bits = 8;

        ret = open(dev_name, O_RDWR);

        if (0 < ret)
        {
            *fd = ret;
            ret = ioctl(*fd, SPI_IOC_WR_MODE, &mode);
        }

        if (0 <= ret)
        {
            ret = ioctl(*fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        }

        if (0 <= ret)
        {
            ret = ioctl(*fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        }

        if (*fd && (0 <= ret))
        {
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_COMM_FAIL;
        }
    }

    return status;
}


/** \brief HAL implementation of SPI init
 *
 * this implementation assumes SPI peripheral has been enabled by user . It only initialize an
 * SPI interface using given config.
 *
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_spi_init(ATCAIface iface, ATCAIfaceCfg *cfg)
{

    int f_spi;
    ATCA_STATUS status = ATCA_BAD_PARAM;

    if (iface && cfg)
    {
        if (!iface->hal_data)
        {
            atca_spi_host_t * hal_data = malloc(sizeof(atca_spi_host_t));
            if (hal_data)
            {
                (void)snprintf(hal_data->spi_file, sizeof(hal_data->spi_file) - 1,
                               "/dev/spidev%d.%d", (uint8_t)cfg->atcaspi.bus, (uint8_t)cfg->atcaspi.select_pin);

                iface->hal_data = hal_data;
                status = ATCA_SUCCESS;
            }
            else
            {
                status = ATCA_ALLOC_FAILURE;
            }
        }
        else
        {
            status = ATCA_SUCCESS;
        }

    }
    return status;
}

ATCA_STATUS hal_spi_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}


/** \brief HAL implementation to assert the device chip select
 * \param[in]    iface          Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_spi_select(ATCAIface iface)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    atca_spi_host_t * hal_data = (atca_spi_host_t*)atgetifacehaldat(iface);

    if (hal_data && cfg)
    {
        return hal_spi_open_file(hal_data->spi_file, cfg->atcaspi.baud, &hal_data->f_spi);
    }
    else
    {
        return ATCA_BAD_PARAM;
    }
}


/** \brief HAL implementation to deassert the device chip select
 * \param[in]    iface          Device to interact with.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_spi_deselect(ATCAIface iface)
{
    atca_spi_host_t * hal_data = (atca_spi_host_t*)atgetifacehaldat(iface);

    if (hal_data)
    {
        struct spi_ioc_transfer spi_xfer = { 0 };

        (void)ioctl(hal_data->f_spi, SPI_IOC_MESSAGE(1), &spi_xfer);

        return close(hal_data->f_spi);
    }
    else
    {
        return ATCA_BAD_PARAM;
    }
}


/** \brief HAL implementation of SPI receive function
 * \param[in]    iface          Device to interact with.
 * \param[in]    word_address   device transaction type
 * \param[out]   rxdata         Data received will be returned here.
 * \param[in,out] len           As input, the size of the rxdata buffer.
 *                              As output, the number of bytes received.
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_spi_receive(ATCAIface iface, uint8_t flags, uint8_t *rxdata, uint16_t *rxlength)
{
    int f_spi;
    ATCA_STATUS status = ATCA_BAD_PARAM;
    atca_spi_host_t * hal_data = (atca_spi_host_t*)atgetifacehaldat(iface);

    if (hal_data)
    {
        struct spi_ioc_transfer spi_xfer = { 0 };
        spi_xfer.rx_buf = (unsigned long)rxdata;
        spi_xfer.len = *rxlength;
        spi_xfer.cs_change = true;

        if (*rxlength == ioctl(hal_data->f_spi, SPI_IOC_MESSAGE(1), &spi_xfer))
        {
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_COMM_FAIL;
        }
    }
    return status;
}


/** \brief HAL implementation of SPI send
 * \param[in] iface           instance
 * \param[in] word_address    transaction type
 * \param[in] txdata          data to be send to device
 * \param[in] txdata          pointer to space to bytes to send
 * \param[in] len  number of bytes to send
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */

ATCA_STATUS hal_spi_send(ATCAIface iface, uint8_t flags, uint8_t *txdata, int txlen)
{
    int f_spi;
    ATCA_STATUS status = ATCA_SUCCESS;
    atca_spi_host_t * hal_data = (atca_spi_host_t*)atgetifacehaldat(iface);
    int ret;

    if (hal_data)
    {
        struct spi_ioc_transfer spi_xfer = { 0 };
        spi_xfer.tx_buf = (unsigned long)txdata;
        spi_xfer.len = txlen;
        spi_xfer.cs_change = true;

        if (txlen == ioctl(hal_data->f_spi, SPI_IOC_MESSAGE(1), &spi_xfer))
        {
            status = ATCA_SUCCESS;
        }
        else
        {
            status = ATCA_COMM_FAIL;
        }
    }

    return status;
}


/** \brief Perform control operations for the kit protocol
 * \param[in]     iface          Interface to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_spi_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        switch (option)
        {
        case ATCA_HAL_CONTROL_SELECT:
            return hal_spi_select(iface);
        case ATCA_HAL_CONTROL_DESELECT:
            return hal_spi_deselect(iface);
        default:
            return ATCA_UNIMPLEMENTED;
        }
    }
    return ATCA_BAD_PARAM;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_spi_release(void *hal_data)
{
    atca_spi_host_t *hal = (atca_spi_host_t*)hal_data;

    if (hal)
    {
        close(hal->f_spi);
        free(hal);
    }

    return ATCA_SUCCESS;
}
