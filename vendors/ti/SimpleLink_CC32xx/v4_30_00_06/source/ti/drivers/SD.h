/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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

/*!***************************************************************************
 *  @file       SD.h
 *  @brief      Secure Digital (SD) Driver
 *
 *  @anchor ti_drivers_SD_Overview
 *  # Overview
 *
 *  The SD driver is designed to serve as an interface to perform basic
 *  transfers directly to the SD card.
 *
 *  <hr>
 *  @anchor ti_drivers_SD_Usage
 *  # Usage
 *  This section will cover driver usage.
 *
 *  @anchor ti_drivers_SD_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_SD_Synopsis_Code
 *  @code
 *  SD_Handle handle;
 *  uint16_t status;
 *
 *  SD_init();
 *
 *  // Open SD and initialize card
 *  handle = SD_open(CONFIG_SD0, NULL);
 *  status = SD_initialize(handle);
 *  if (handle == NULL || status != SD_STATUS_SUCCESS) {
 *      //Error opening SD driver
 *      while (1);
 *  }
 *
 *  // Write and read back the first sector
 *  status = SD_write(handle, sendBuffer, 0, 1);
 *  if (status == SD_STATUS_SUCCESS) {
 *      status = SD_read(handle, readBuffer, 0 , 1);
 *  }
 *
 *  SD_close(handle);
 *  @endcode
 *
 *  @anchor ti_drivers_SD_Examples
 *  # Examples
 *  - @ref ti_drivers_SD_Synopsis "Overview"
 *  - @ref ti_drivers_SD_Example_getCardSpace "Get SD card size"
 *
 *  Get total capacity of an SD card:
 *  @anchor ti_drivers_SD_Example_getCardSpace
 *  @code
 *  SD_Handle handle;
 *  Display_Handle display;
 *  uint_fast32_t sectorSize, sectorCount;
 *
 *  // Init, open, etc
 *  ...
 *
 *  sectorSize = SD_getSectorSize(handle);
 *  sectorCount = SD_getNumSectors(handle);
 *
 *  Display_printf(display, 0, 0,"SD card total capacity is %lu bytes.",
 *                 sectorSize * sectorCount);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_SD_Configuration
 *  # Configuration
 *  Refer to the @ref driver_configuration "Driver's Configuration"
 *  section for driver configuration information.
 *
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_SD__include
#define ti_drivers_SD__include

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup SD_CONTROL SD_control command and status codes
 *  @{
 */

/*!
 * Common SD_control() command code reservation offset.
 * SD driver implementations should offset command codes with
 * SD_CMD_RESERVED growing positively.
 *
 * Example implementation specific command codes:
 * @code
 * #define SDXYZ_CMD_COMMAND0    (SD_CMD_RESERVED + 0)
 * #define SDXYZ_CMD_COMMAND1    (SD_CMD_RESERVED + 1)
 * @endcode
 */
#define SD_CMD_RESERVED    (32)

/*!
 * Common SD_control status code reservation offset.
 * SD driver implementations should offset status codes with
 * SD_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define SDXYZ_STATUS_ERROR0    (SD_STATUS_RESERVED - 0)
 * #define SDXYZ_STATUS_ERROR1    (SD_STATUS_RESERVED - 1)
 * #define SDXYZ_STATUS_ERROR2    (SD_STATUS_RESERVED - 2)
 * @endcode
 */
#define SD_STATUS_RESERVED    (-32)

/**
 *  @defgroup SD_STATUS Status Codes
 *  SD_STATUS_* macros are general status codes returned by SD_control()
 *  @{
 *  @ingroup SD_CONTROL
 */

/*!
 * @brief Successful status code returned by SD_control().
 *
 * SD_control() returns SD_STATUS_SUCCESS if the control code was executed
 * successfully.
 */
#define SD_STATUS_SUCCESS    (0)

/*!
 * @brief Generic error status code returned by SD_control().
 *
 * SD_control() returns SD_STATUS_ERROR if the control code
 * was not executed successfully.
 */
#define SD_STATUS_ERROR    (-1)

/*!
 * @brief   An error status code returned by SD_control() for
 * undefined command codes.
 *
 * SD_control() returns SD_STATUS_UNDEFINEDCMD if the
 * control code is not recognized by the driver implementation.
 */
#define SD_STATUS_UNDEFINEDCMD    (-2)
/** @}*/

/**
 *  @defgroup SD_CMD Command Codes
 *  SD_CMD_* macros are general command codes for SD_control(). Not all SD
 *  driver implementations support these command codes.
 *  @{
 *  @ingroup SD_CONTROL
 */

/* Add SD_CMD_<commands> here */

/** @}*/

/** @}*/

/*!
 *  @brief  SD Card type inserted
 */
typedef enum {
    SD_NOCARD = 0, /*!< Unrecognized Card */
    SD_MMC = 1,    /*!< Multi-media Memory Card (MMC) */
    SD_SDSC = 2,   /*!< Standard SDCard (SDSC) */
    SD_SDHC = 3    /*!< High Capacity SDCard (SDHC) */
} SD_CardType;

/*!
 *  @brief      A handle that is returned from a SD_open() call.
 */
typedef struct SD_Config_ *SD_Handle;

/*!
 *  @brief SD Parameters
 *
 *  SD Parameters are used to with the SD_open() call.
 *  Default values for these parameters are set using SD_Params_init().
 *
 *  @sa SD_Params_init()
 */

/* SD Parameters */
typedef struct {
    void   *custom;  /*!< Custom argument used by driver implementation */
} SD_Params;

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_CloseFxn().
 */
typedef void (*SD_CloseFxn) (SD_Handle handle);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_controlFxn().
 */
typedef int_fast16_t (*SD_ControlFxn) (SD_Handle handle,
    uint_fast16_t cmd, void *arg);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_getNumSectorsFxn().
 */
typedef uint_fast32_t (*SD_getNumSectorsFxn) (SD_Handle handle);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_getSectorSizeFxn().
 */
typedef uint_fast32_t (*SD_getSectorSizeFxn) (SD_Handle handle);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_InitFxn().
 */
typedef void (*SD_InitFxn) (SD_Handle handle);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_initializeFxn().
 */
typedef int_fast16_t (*SD_InitializeFxn) (SD_Handle handle);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_OpenFxn().
 */
typedef SD_Handle (*SD_OpenFxn) (SD_Handle handle, SD_Params *params);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_readFxn().
 */
typedef int_fast16_t (*SD_ReadFxn) (SD_Handle handle, void *buf,
    int_fast32_t sector, uint_fast32_t secCount);

/*!
 *  @brief A function pointer to a driver specific implementation of
 *         SD_writeFxn().
 */
typedef int_fast16_t (*SD_WriteFxn) (SD_Handle handle, const void *buf,
    int_fast32_t sector, uint_fast32_t secCount);

/*!
 *  @brief The definition of a SD function table that contains the
 *         required set of functions to control a specific SD driver
 *         implementation.
 */
typedef struct {
    /*! Function to close the specified peripheral */
    SD_CloseFxn             closeFxn;
    /*! Function to implementation specific control function */
    SD_ControlFxn           controlFxn;
    /*! Function to return the total number of sectors on the SD card */
    SD_getNumSectorsFxn     getNumSectorsFxn;
    /*! Function to return the sector size used to address the SD card */
    SD_getSectorSizeFxn     getSectorSizeFxn;
    /*! Function to initialize the given data object */
    SD_InitFxn              initFxn;
    /*! Function to initialize the SD card */
    SD_InitializeFxn        initializeFxn;
    /*! Function to open the specified peripheral */
    SD_OpenFxn              openFxn;
    /*! Function to read from the SD card */
    SD_ReadFxn              readFxn;
    /*! Function to write to the SD card */
    SD_WriteFxn             writeFxn;
} SD_FxnTable;

/*!
 *  @brief SD Global configuration
 *
 *  The SD_Config structure contains a set of pointers used
 *  to characterize the SD driver implementation.
 *
 *  This structure needs to be defined before calling SD_init() and it must
 *  not be changed thereafter.
 *
 *  @sa SD_init()
 */
typedef struct SD_Config_ {
    /*! Pointer to a table of driver-specific implementations of SD APIs */
    SD_FxnTable const    *fxnTablePtr;

    /*! Pointer to a driver specific data object */
    void                 *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void const           *hwAttrs;
} SD_Config;

/*!
 *  @brief Function to close a SD peripheral specified by the SD handle.
 *
 *  @pre SD_open() had to be called first.
 *
 *  @param handle A #SD_Handle returned from SD_open()
 *
 *  @sa SD_open()
 */
extern void SD_close(SD_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a given
 *          #SD_Handle.
 *
 *  Commands for SD_control can originate from SD.h or from implementation
 *  specific SD*.h files.
 *  While commands from SD.h are API portable across driver implementations,
 *  not all implementations may support all these commands.
 *  Conversely, commands from driver implementation specific SD*.h files add
 *  unique driver capabilities but are not API portable across all SD driver
 *  implementations.
 *
 *  Commands supported by SD.h follow a SD*_CMD naming
 *  convention.
 *
 *  Commands supported by SD*.h follow a SD*_CMD naming
 *  convention.
 *  Each control command defines arg differently. The types of arg are
 *  documented with each command.
 *
 *  See @ref SD_CMD "SD_control command codes" for command codes.
 *
 *  See @ref SD_STATUS "SD_control return status codes" for status codes.
 *
 *  @pre SD_open() has to be called first.
 *
 *  @param handle A #SD_Handle returned from SD_open().
 *
 *  @param cmd SD.h or SD*.h commands.
 *
 *  @param arg An optional R/W (read/write) command argument
 *              accompanied with cmd.
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @sa SD_open()
 */
extern int_fast16_t SD_control(SD_Handle handle, uint_fast16_t cmd, void *arg);

/*!
 *  @brief Function to obtain the total number of sectors on the SD card.
 *         Note: Total Card capacity is the (NumberOfSectors * SectorSize).
 *
 *  @pre SD Card has been initialized using SD_initialize().
 *
 *  @param  handle A #SD_Handle returned from SD_open().
 *
 *  @return The total number of sectors on the SD card,
 *          or 0 if an error occurred.
 *
 *  @sa SD_initialize()
 */
extern uint_fast32_t SD_getNumSectors(SD_Handle handle);

/*!
 *  @brief Function to obtain the sector size used to access the SD card.
 *
 *  @pre SD Card has been initialized using SD_initialize().
 *
 *  @param handle A #SD_Handle returned from SD_open().
 *
 *  @return The sector size set for use during SD card read/write operations.
 *
 *  @sa SD_initialize()
 */
extern uint_fast32_t SD_getSectorSize(SD_Handle handle);

/*!
 *  @brief This function initializes the SD driver.
 *
 *  @pre The SD_config[] array must exist and be persistent before this
 *       function can be called. This function must also be called before
 *       any other SD driver APIs. This function call does not modify any
 *       peripheral registers.
 */
extern void SD_init(void);

/*!
 *  @brief Function to initialize the #SD_Params struct to its defaults.
 *
 *  @param params A pointer to #SD_Params structure for initialization.
 */
extern void SD_Params_init(SD_Params *params);

 /*!
 *  @brief  Function to initialize the SD card.
 *
 *  @pre    SD controller has been opened by calling SD_open().
 *
 *  @param  handle A #SD_Handle returned from SD_open().
 *
 *  @return #SD_STATUS_SUCCESS if no errors occurred during the initialization,
 *          #SD_STATUS_ERROR otherwise.
 */
extern int_fast16_t SD_initialize(SD_Handle handle);

/*!
 *  @brief Function to open the SD peripheral with the index and parameters
 *         specified.
 *
 *  @pre SD controller has been initialized using SD_init().
 *
 *  @param index  Logical peripheral number for the SD indexed into
 *                the SD_config[] table.
 *
 *  @param params Pointer to a parameter block, if NULL it will use
 *                default values. All the fields in this structure are
 *                RO (read-only).
 *
 *  @return A #SD_Handle on success or a NULL on an error or if it has been
 *          opened already.
 *
 *  @sa SD_init()
 *  @sa SD_close()
 */
extern SD_Handle SD_open(uint_least8_t index, SD_Params *params);

/*!
 *  @brief Function that reads the specified sectors from the SD card.
 *         The destination is specified by \a buf. The starting sector
 *         is specified by \a sector and the total number of sectors to
 *         read is provided by \a secCount.
 *
 *  @pre SD controller has been opened and initialized by calling SD_open()
 *       followed by SD_initialize().
 *
 *  @param handle A #SD_Handle returned from SD_open().
 *
 *  @param buf Pointer to a buffer to read data into.
 *
 *  @param sector Starting sector on the disk to read from.
 *
 *  @param secCount Number of sectors to be read.
 *
 *  @return #SD_STATUS_SUCCESS if no errors occurred during the write,
 *          #SD_STATUS_ERROR otherwise.
 *
 *  @sa SD_initialize()
 */
extern int_fast16_t SD_read(SD_Handle handle, void *buf,
    int_fast32_t sector, uint_fast32_t secCount);

/*!
 *  @brief Function that writes data to the specified sectors of the SD card.
 *         The source is specified by \a buf. The starting sector to write
 *         is specified by \a sector and the total number of sectors to write
 *         is provided by \a secCount.
 *
 *  @pre SD controller has been opened and initialized by calling SD_open()
 *       followed by SD_initialize().
 *
 *  @param  handle A #SD_Handle returned from SD_open().
 *
 *  @param  buf Pointer to a buffer containing data to write to disk.
 *
 *  @param  sector Starting sector on the disk to write to.
 *
 *  @param  secCount Number of sectors to be written.
 *
 *  @return #SD_STATUS_SUCCESS if no errors occurred during the write,
 *          #SD_STATUS_ERROR otherwise.
 *
 *  @sa     SD_initialize()
 */
extern int_fast16_t SD_write(SD_Handle handle, const void *buf,
    int_fast32_t sector, uint_fast32_t secCount);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_SD__include */
