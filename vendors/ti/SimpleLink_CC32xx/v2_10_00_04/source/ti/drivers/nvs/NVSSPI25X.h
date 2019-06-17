/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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

/*!*****************************************************************************
 *  @file       NVSSPI25X.h
 *  @brief      Non-Volatile Storage driver implementation
 *              for SPI flash peripherals
 *
 *  # Overview #
 *
 *  The NVSSPI25X module allows you to manage SPI flash memory.
 *  This driver works with most 256 byte/page SPI flash memory devices
 *  such as:
 *
 *      Winbond     W25xx   family
 *      Macronics   MX25Rxx family
 *      Micron      N25Qxx  family
 *
 *  The SPI flash commands used by this driver are as follows:
 *
 *  @code
 *  #define SPIFLASH_PAGE_WRITE       0x02 // Page Program (up to 256 bytes)
 *  #define SPIFLASH_READ             0x03 // Read Data
 *  #define SPIFLASH_READ_STATUS      0x05 // Read Status Register
 *  #define SPIFLASH_WRITE_ENABLE     0x06 // Write Enable
 *  #define SPIFLASH_SUBSECTOR_ERASE  0x20 // SubSector (4K bytes) Erase
 *  #define SPIFLASH_SECTOR_ERASE     0xD8 // Sector (usually 64K bytes) Erase
 *  #define SPIFLASH_RDP              0xAB // Release from Deep Power Down
 *  #define SPIFLASH_DP               0xB9 // Deep Power Down
 *  #define SPIFLASH_MASS_ERASE       0xC7 // Erase entire flash.
 *  @endcode
 *
 *  It is assumed that the SPI flash device used by this driver supports
 *  the byte programmability of the SPIFLASH_PAGE_WRITE command and that
 *  write page size is 256 bytes.
 *
 *  The NVS_erase() command assumes that regions with sectorSize = 4096 bytes
 *  are erased using the SPIFLASH_SUBSECTOR_ERASE command (0x20). Otherwise the
 *  SPIFLASH_SECTOR_ERASE command (0xD8) is used to erase the flash sector.
 *  It is up to the user to ensure that each region's sectorSize matches these
 *  sector erase rules.
 *
 * ## SPI Interface Management ##
 *
 *  For each managed flash region, a corresponding SPI instance must be
 *  provided to the NVSSPI25X driver.
 *
 *  The SPI instance can be opened and closed
 *  internally by the NVSSPI25X driver, or alternatively, a SPI handle can be
 *  provided to the NVSSPI25X driver, indicating that the SPI instance is being
 *  opened and closed elsewhere within the application. This mode is useful
 *  when the SPI bus is share by more than just the SPI flash device.
 *
 *  If the SPI instance is to be managed internally by the NVSSPI25X driver, a SPI
 *  instance index and bit rate must be configured in the region's HWAttrs.
 *  If the same SPI instance is referenced by multiple flash regions
 *  the driver will ensure that SPI_open() is invoked only once, and that
 *  SPI_close() will only be invoked when all flash regions using the SPI
 *  instance have been closed.
 *
 *  If the SPI bus that the SPI flash device is on is shared with other
 *  devices accessed by an application, then the SPI handle used to manage
 *  a SPI flash region can be provided in the region's HWAttrs "spiHandle"
 *  field. Keep in mind that the "spiHandle" field is a POINTER to a
 *  SPI Handle, NOT a SPI Handle. This allows the user to simply initialize
 *  this field with the name of the global variable used for the SPI handle.
 *  In this mode, the user MUST open the SPI instance prior to opening the NVS
 *  region instance so that the referenced spiHandle is valid.
 *
 *  By default, the "spiHandle" field is set to NULL, indicating that the user
 *  expects the NVS driver to open and close the SPI instance internally using
 *  the 'spiIndex' and 'spiBitRate' provided in the HWAttrs.
 *
 *  ## @anchor SPI_CS_MGMT SPI Flash Chip Select Management ##
 *
 *  ### Option 1: NVSSPI25X Driver Manages Chip Select ###
 *  By default, the NVSSPI25X driver will assert and de-assert a GPIO
 *  driver managed pin to select the SPI flash device before and after
 *  each SPI transfer to and from the device.
 *
 *  To enable this behavior, a valid GPIO driver instance index must be
 *  provided in the NVS region's [spiCsnGpioIndex]
 *  (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex) field of the
 *  NVSSPI25X_HWAttrs structure. The corresponding GPIO pin will be
 *  configured at runtime by the NVSSPI25X driver as "GPIO_CFG_OUT_STD"
 *  and assertion of this pin is assumed to be active LOW.
 *
 *  ### Option 2: SPI Driver Manages Chip Select ###
 *  Some SPI peripherals can be configured to manage their own chip
 *  select. Setting the [spiCsnGpioIndex]
 *   (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex) field of the NVSSPI25X_HWAttrs
 *  structure to #NVSSPI25X_SPI_MANAGES_CS informs the NVSSPI25X driver
 *  that the SPI peripheral used by the NVS driver has been configured
 *  that way.
 *
 *  ### Option 3: User Manages Chip Select ###
 *  Alternatively, the user can manage the assertion and de-assertion of
 *  the SPI flash chip select entirely themselves by providing implementations
 *  of the following 4 APIs in their application code:
 *
 *  @code
 *  void NVSSPI25X_initSpiCs(NVS_Handle nvsHandle, uint16_t csId);
 *  @endcode
 *  - This function is invoked within the NVS_open() API and is where the
 *    user should do whatever is required to initialize the hardware
 *    used for asserting and de-assering the SPI chip select signal.
 *  - The 'nvsHandle` argument is the NVS handle associated with the
 *    corresponding NVS region.
 *  - The 'csId' argument passed to this API is a copy of the [spiCsnGpioIndex]
 *    (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex)
 *    field of the corresponding NVS region's NVSSPI25X_HWAttrs structure.
 *
 *  @code
 *  void NVSSPI25X_deinitSpiCs(NVS_Handle nvsHandle, uint16_t csId);
 *  @endcode
 *  - This function is invoked within the NVS_close() API and is where the
 *    user should do whatever is required to de-initialize the hardware
 *    used for asserting and de-assering the SPI chip select signal.
 *  - The 'nvsHandle` argument is the NVS handle associated with the
 *    corresponding NVS region.
 *  - The 'csId' argument passed to this API is a copy of the [spiCsnGpioIndex]
 *    (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex)
 *    field of the corresponding NVS region's NVSSPI25X_HWAttrs structure.
 *
 *  @code
 *  void NVSSPI25X_assertSpiCs(NVS_Handle nvsHandle, uint16_t csId);
 *  @endcode
 *  - This function is called PRIOR to every SPI transfer to and from the SPI
 *    flash device performed by the NVSSPI25X driver. The user code should
 *    perform the corresponding action required to select the SPI flash
 *    device to prepare for the SPI transfer.
 *  - The 'nvsHandle` argument is the NVS handle associated with the
 *    corresponding NVS region.
 *  - The 'csId' argument passed to this API is a copy of the [spiCsnGpioIndex]
 *    (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex)
 *    field of the corresponding NVS region's NVSSPI25X_HWAttrs structure.
 *
 *  @code
 *  void NVSSPI25X_deassertSpiCs(NVS_Handle nvsHandle, uint16_t csId);
 *  @endcode
 *  - This function is called AFTER every SPI transfer to and from the SPI
 *    flash device performed by the NVSSPI25X driver. The user code should
 *    perform the corresponding action required to de-select the SPI flash
 *    device.
 *    following the SPI transfer.
 *  - The 'nvsHandle` argument is the NVS handle associated with the
 *    corresponding NVS region.
 *  - The 'csId' argument passed to this API is a copy of the [spiCsnGpioIndex]
 *    (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex)
 *    field of the corresponding NVS region's NVSSPI25X_HWAttrs structure.
 *
 *  @warning  All 4 of the above APIs must be provided by the user if this
 *  option is used, otherwise default internal implementations of the APIs
 *  will be called that will likely lead to application failure.
 */

#ifndef ti_drivers_nvs_NVSSPI25X__include
#define ti_drivers_nvs_NVSSPI25X__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/SPI.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*!
 *  @brief Command to perform mass erase of entire flash
 *
 *  As this command can erase flash memory outside the region associated
 *  with the NVS_Handle passed to the control command, the user must
 *  carefully orchestrate the use of the command.
 *
 *  Mass Erase is the only control command supported.
 */
#define NVSSPI25X_CMD_MASS_ERASE        (NVS_CMD_RESERVED + 0)

/*!
 *  @brief Disable internal management of SPI chip select
 *
 *  Some SPI peripherals can be configured to manage their own chip
 *  select. Setting the [spiCsnGpioIndex]
 *  (@ref NVSSPI25X_HWAttrs.spiCsnGpioIndex) field of the NVSSPI25X_HWAttrs
 *  structure to #NVSSPI25X_SPI_MANAGES_CS informs the NVSSPI25X driver
 *  that the SPI peripheral used by the NVS driver is configured
 *  to manage its own chip select signal.
 */
#define NVSSPI25X_SPI_MANAGES_CS        ((uint16_t)(~0))

/*!
 *  @internal @brief NVS function pointer table
 *
 *  'NVSSPI25X_fxnTable' is a fully populated function pointer table
 *  that can be referenced in the NVS_config[] array entries.
 *
 *  Users can minimize their application code size by providing their
 *  own custom NVS function pointer table that contains only those APIs
 *  used by the application.
 */
extern const NVS_FxnTable NVSSPI25X_fxnTable;

/*!
 *  @brief      NVSSPI25X attributes
 *
 *  The 'regionBaseOffset' is the offset, in bytes, from the base of the
 *  SPI flash, of the flash region to be managed.
 *
 *  The 'regionSize' must be an integer multiple of the flash sector size.
 *
 *  The 'sectorSize' is SPI flash device specific. This parameter should
 *  correspond to the number of bytes erased when the
 *  'SPIFLASH_SUBSECTOR_ERASE' (0x20) command is issued to the device.
 *
 *  The 'verifyBuf' and 'verifyBufSize' parameters are used by the
 *  NVS_write() command when either 'NVS_WRITE_PRE_VERIFY' or
 *  'NVS_WRITE_POST_VERIFY' functions are requested in the 'flags'
 *  argument. The 'verifyBuf' is used to successively read back portions
 *  of the flash to compare with the data being written to it.
 *
 *  @code
 *  //
 *  // Only one region write operation is performed at a time
 *  // so a single verifyBuf can be shared by all the regions.
 *  //
 *  uint8_t verifyBuf[256];
 *
 *  NVSSPI25X_HWAttrs nvsSPIHWAttrs[2] = {
 *      //
 *      // region 0 is 1 flash sector in length.
 *      //
 *      {
 *          .regionBaseOffset = 0,
 *          .regionSize = 4096,
 *          .sectorSize = 4096,
 *          .verifyBuf = verifyBuf;
 *          .verifyBufSize = 256;
 *          .spiHandle = NULL,
 *          .spiIndex = 0,
 *          .spiBitRate = 40000000,
 *          .spiCsnGpioIndex = 12,
 *      },
 *      //
 *      // region 1 is 3 flash sectors in length.
 *      //
 *      {
 *          .regionBaseOffset = 4096,
 *          .regionSize = 4096 * 3,
 *          .sectorSize = 4096,
 *          .verifyBuf = verifyBuf;     // use shared verifyBuf
 *          .verifyBufSize = 256;
 *          .spiHandle = NULL,
 *          .spiIndex = 0,
 *          .spiBitRate = 40000000,
 *          .spiCsnGpioIndex = 12,
 *      }
 *  };
 *  @endcode
 */
typedef struct NVSSPI25X_HWAttrs {
    size_t      regionBaseOffset;   /*!< Offset from base of SPI flash */
    size_t      regionSize;         /*!< The size of the region in bytes */
    size_t      sectorSize;         /*!< Erase sector size */
    uint8_t     *verifyBuf;         /*!< Write Pre/Post verify buffer */
    size_t      verifyBufSize;      /*!< Write Pre/Post verify buffer size */
    SPI_Handle  *spiHandle;         /*!< ptr to SPI handle if provided by user. */
    uint16_t    spiIndex;           /*!< SPI instance index from Board file */
    uint32_t    spiBitRate;         /*!< SPI bit rate in Hz */
    /*! @brief SPI Flash Chip Select GPIO index

        This field should be set to either an index within the
        GPIO driver's GPIO_Config table, or to #NVSSPI25X_SPI_MANAGES_CS.
        see [SPI Flash Chip Select Management] (@ref SPI_CS_MGMT) for more
        details.
    */
    uint16_t    spiCsnGpioIndex;
} NVSSPI25X_HWAttrs;

/*
 *  @brief      NVSSPI25X Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct NVSSPI25X_Object {
    bool        opened;             /* Has this region been opened */
    SPI_Handle  spiHandle;
    size_t      sectorBaseMask;
} NVSSPI25X_Object;

/*
 *  @cond NODOC
 *  NVSSPI25X driver public APIs
 */

extern void         NVSSPI25X_close(NVS_Handle handle);
extern int_fast16_t NVSSPI25X_control(NVS_Handle handle, uint_fast16_t cmd,
                        uintptr_t arg);
extern int_fast16_t NVSSPI25X_erase(NVS_Handle handle, size_t offset,
                        size_t size);
extern void         NVSSPI25X_getAttrs(NVS_Handle handle, NVS_Attrs *attrs);
extern void         NVSSPI25X_init();
extern int_fast16_t NVSSPI25X_lock(NVS_Handle handle, uint32_t timeout);
extern NVS_Handle   NVSSPI25X_open(uint_least8_t index, NVS_Params *params);
extern int_fast16_t NVSSPI25X_read(NVS_Handle handle, size_t offset,
                        void *buffer, size_t bufferSize);
extern void         NVSSPI25X_unlock(NVS_Handle handle);
extern int_fast16_t NVSSPI25X_write(NVS_Handle handle, size_t offset,
                        void *buffer, size_t bufferSize, uint_fast16_t flags);
/*
 *  Weakly defined APIs that can be overridden by the user
 */
extern void         NVSSPI25X_initSpiCs(NVS_Handle spiHandle, uint16_t csId);
extern void         NVSSPI25X_deinitSpiCs(NVS_Handle spiHandle, uint16_t csId);
extern void         NVSSPI25X_assertSpiCs(NVS_Handle spiHandle, uint16_t csId);
extern void         NVSSPI25X_deassertSpiCs(NVS_Handle spiHandle, uint16_t csId);

/*! @endcond */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

/** @}*/
#endif /* ti_drivers_nvs_NVSSPI25X__include */
