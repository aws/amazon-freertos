/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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
/*!****************************************************************************
 *  @file       ADC.h
 *  @brief      Analog to Digital Conversion (ADC) Input Driver
 *
 *  @anchor ti_drivers_ADC_Overview
 *  # Overview
 *
 *  The ADC driver allows you to manage an Analog to Digital peripheral via
 *  simple and portable APIs. This driver supports sampling and converting
 *  raw values into microvolts.
 *
 *  <hr>
 *  @anchor ti_drivers_ADC_Usage
 *  # Usage
 *
 *  This documentation provides a basic @ref ti_drivers_ADC_Synopsis
 *  "usage summary" and a set of @ref ti_drivers_ADC_Examples "examples"
 *  in the form of commented code fragments. Detailed descriptions of the
 *  APIs are provided in subsequent sections.
 *
 *  @anchor ti_drivers_ADC_Synopsis
 *  ## Synopsis
 *  @anchor ti_drivers_ADC_Synopsis_Code
 *  @code
 *  // Import ADC Driver definitions
 *  #include <ti/drivers/ADC.h>
 *
 *  // Define name for ADC channel index
 *  #define THERMOCOUPLE_OUT  0
 *
 *  // One-time init of ADC driver
 *  ADC_init();
 *
 *  // initialize optional ADC parameters
 *  ADC_Params params;
 *  ADC_Params_init(&params);
 *  params.isProtected = true;
 *
 *  // Open ADC channels for usage
 *  ADC_Handle adcHandle = ADC_open(THERMOCOUPLE_OUT, &params);
 *
 *  // Sample the analog output from the Thermocouple
 *  ADC_convert(adcHandle, &result);
 *
 *  // Convert the sample to microvolts
 *  resultUv = ADC_convertToMicroVolts(adcHandle, result);
 *
 *  ADC_close(adcHandle);
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_ADC_Examples
 *  # Examples
 *
 *  @li @ref ti_drivers_ADC_Examples_open "Opening an ADC instance"
 *  @li @ref ti_drivers_ADC_Examples_convert "Taking an ADC sample"
 *  @li @ref ti_drivers_ADC_Examples_convert_microvolts "Converting a sample to microvolts"
 *
 *  @anchor ti_drivers_ADC_Examples_open
 *  ## Opening an ADC instance
 *
 *  @code
 *  ADC_Handle adc;
 *  ADC_Params params;
 *
 *  ADC_Params_init(&params);
 *
 *  adc = ADC_open(0, &params);
 *  if (adc == NULL) {
 *      // ADC_open() failed
 *      while (1) {}
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_ADC_Examples_convert
 *  ## Taking an ADC sample
 *
 *  An ADC conversion with an ADC peripheral is started by calling
 *  ADC_convert(). The result value is returned by ADC_convert()
 *  once the conversion is finished.
 *
 *  @code
 *  int_fast16_t res;
 *  uint_fast16_t adcValue;
 *
 *  res = ADC_convert(adc, &adcValue);
 *  if (res == ADC_STATUS_SUCCESS)
 *  {
 *      print(adcValue);
 *  }
 *  @endcode
 *
 *  @anchor ti_drivers_ADC_Examples_convert_microvolts
 *  ## Converting a sample to microvolts
 *
 *  The result value returned by ADC_convert() is a raw value. The
 *  following uses ADC_convertToMicroVolts() to convert the raw value
 *  into microvolts.
 *  @code
 *  int_fast16_t res;
 *  uint_fast16_t adcValue;
 *  uint32_t adcValueUv;
 *
 *  res = ADC_convert(adc, &adcValue);
 *  if (res == ADC_STATUS_SUCCESS)
 *  {
 *      adcValueUv = ADC_convertToMicroVolts(adc, adcValue);
 *  }
 *  @endcode
 *
 *  <hr>
 *  @anchor ti_drivers_ADC_Configuration
 *  # Configuration
 *
 *  Refer to the @ref driver_configuration "Driver's Configuration" section
 *  for driver configuration information.
 *  <hr>
 ******************************************************************************
 */

#ifndef ti_drivers_ADC__include
#define ti_drivers_ADC__include

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @deprecated ADC_convertRawToMicroVolts() is succeeded by
 *  ADC_convertToMicroVolts().
 */
#define ADC_convertRawToMicroVolts ADC_convertToMicroVolts

/*!
 *  @defgroup ADC_CONTROL ADC_control command and status codes
 *  These ADC macros are reservations for ADC.h
 *  @{
 */

/*!
 * @hideinitializer
 * Common ADC_control command code reservation offset.
 * ADC driver implementations should offset command codes with ADC_CMD_RESERVED
 * growing positively
 *
 * Example implementation specific command codes:
 * @code
 * #define ADCXYZ_CMD_COMMAND0      ADC_CMD_RESERVED + 0
 * #define ADCXYZ_CMD_COMMAND1      ADC_CMD_RESERVED + 1
 * @endcode
 */
#define ADC_CMD_RESERVED           (32)

/*!
 * @hideinitializer
 * Common ADC_control status code reservation offset.
 * ADC driver implementations should offset status codes with
 * ADC_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define ADCXYZ_STATUS_ERROR0     ADC_STATUS_RESERVED - 0
 * #define ADCXYZ_STATUS_ERROR1     ADC_STATUS_RESERVED - 1
 * #define ADCXYZ_STATUS_ERROR2     ADC_STATUS_RESERVED - 2
 * @endcode
 */
#define ADC_STATUS_RESERVED        (-32)

/*!
 * @brief   Successful status code returned by ADC_control().
 *
 * ADC_control() returns ADC_STATUS_SUCCESS if the control code was executed
 * successfully.
 *  @{
 *  @ingroup ADC_CONTROL
 */
#define ADC_STATUS_SUCCESS         (0)

/*!
 * @brief   Generic error status code returned by ADC_control().
 *
 * ADC_control() returns ADC_STATUS_ERROR if the control code was not executed
 * successfully.
 */
#define ADC_STATUS_ERROR           (-1)

/*!
 * @brief   An error status code returned by ADC_control() for undefined
 * command codes.
 *
 * ADC_control() returns ADC_STATUS_UNDEFINEDCMD if the control code is not
 * recognized by the driver implementation.
 */
#define ADC_STATUS_UNDEFINEDCMD    (-2)
/** @}*/

/**
 *  @defgroup ADC_CMD Command Codes
 *  ADC_CMD_* macros are general command codes for ADC_control(). Not all ADC
 *  driver implementations support these command codes.
 *  @{
 *  @ingroup ADC_CONTROL
 */

/* Add ADC_CMD_<commands> here */

/** @}*/

/** @}*/

/*!
 *  @brief      A handle that is returned from an ADC_open() call.
 */
typedef struct ADC_Config_ *ADC_Handle;

/*!
 *  @brief  ADC Parameters used with ADC_open().
 *
 *  ADC_Params_init() must be called prior to setting fields in
 *  this structure.
 *
 *  @sa     ADC_Params_init()
 */
typedef struct {
    void    *custom;        /*!< Custom argument used by driver
                                implementation */
    bool    isProtected;    /*!< By default ADC uses a semaphore
                                to guarantee thread safety. Setting
                                this parameter to 'false' will eliminate
                                the usage of a semaphore for thread
                                safety. The user is then responsible
                                for ensuring that parallel invocations
                                of ADC_convert() are thread safe. */
} ADC_Params;

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_close().
 */
typedef void (*ADC_CloseFxn) (ADC_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_control().
 */
typedef int_fast16_t (*ADC_ControlFxn) (ADC_Handle handle, uint_fast16_t cmd,
    void *arg);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_ConvertFxn().
 */
typedef int_fast16_t (*ADC_ConvertFxn) (ADC_Handle handle, uint16_t *value);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_convertToMicroVolts().
 */
typedef uint32_t (*ADC_ConvertToMicroVoltsFxn) (ADC_Handle handle,
    uint16_t adcValue);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_init().
 */
typedef void (*ADC_InitFxn) (ADC_Handle handle);

/*!
 *  @private
 *  @brief      A function pointer to a driver specific implementation of
 *              ADC_open().
 */
typedef ADC_Handle (*ADC_OpenFxn) (ADC_Handle handle, ADC_Params *params);

/*!
 *  @brief      The definition of an ADC function table that contains the
 *              required set of functions to control a specific ADC driver
 *              implementation.
 */
typedef struct {
    /*! Function to close the specified peripheral */
    ADC_CloseFxn      closeFxn;

    /*! Function to perform implementation specific features */
    ADC_ControlFxn    controlFxn;

    /*! Function to initiate an ADC single channel conversion */
    ADC_ConvertFxn    convertFxn;

    /*! Function to convert ADC result to microvolts */
    ADC_ConvertToMicroVoltsFxn convertToMicroVolts;

    /*! Function to initialize the given data object */
    ADC_InitFxn       initFxn;

    /*! Function to open the specified peripheral */
    ADC_OpenFxn       openFxn;
} ADC_FxnTable;

/*!
 *  @brief ADC driver's custom @ref driver_configuration "configuration"
 *  structure.
 *
 *  @sa     ADC_init()
 *  @sa     ADC_open()
 */
typedef struct ADC_Config_ {
    /*! Pointer to a @ref driver_function_table "function pointer table"
     *  with driver-specific implementations of ADC APIs */
    ADC_FxnTable const *fxnTablePtr;

    /*! Pointer to a driver specific @ref driver_objects "data object". */
    void               *object;

    /*! Pointer to a driver specific @ref driver_hardware_attributes
     *  "hardware attributes structure". */
    void         const *hwAttrs;
} ADC_Config;

/*!
 *  @brief  Function to close an ADC driver instance
 *
 *  @pre        ADC_open() has to be called first.
 *
 *  @param[in]  handle An #ADC_Handle returned from ADC_open()
 */
extern void ADC_close(ADC_Handle handle);

/*!
 *  @brief  Function performs implementation specific features on a
 *          driver instance.
 *
 *  @pre    ADC_open() has to be called first.
 *
 *  @param[in]  handle   An #ADC_Handle returned from ADC_open()
 *
 *  @param[in]  cmd     A command value defined by the device specific
 *                      implementation
 *
 *  @param[in]  arg     An optional R/W (read/write) argument that is
 *                      accompanied with @p cmd
 *
 *  @return Implementation specific return codes. Negative values indicate
 *          unsuccessful operations.
 *
 *  @retval #ADC_STATUS_SUCCESS The call was successful.
 *  @retval #ADC_STATUS_UNDEFINEDCMD The @p cmd value is not supported by
 *                                   the device specific implementation.
 */
extern int_fast16_t ADC_control(ADC_Handle handle, uint_fast16_t cmd,
    void *arg);

/*!
 *  @brief  Function to perform an ADC conversion
 *
 *  Function to perform a single channel sample conversion.
 *
 *  @pre    ADC_open() has been called
 *
 *  @param[in]      handle    An #ADC_Handle returned from ADC_open()
 *  @param[in,out]  value     A pointer to a uint16_t to store the conversion
 *                            result
 *
 *  @retval #ADC_STATUS_SUCCESS  The conversion was successful.
 *  @retval #ADC_STATUS_ERROR    The conversion failed and @p value is
 *                               invalid.
 *
 *  @sa     ADC_convertToMicroVolts()
 */
extern int_fast16_t ADC_convert(ADC_Handle handle, uint16_t *value);

/*!
 *  @brief  Function to convert a raw ADC sample into microvolts.
 *
 *  @pre    ADC_convert() has to be called first.
 *
 *  @param[in]  handle      An #ADC_Handle returned from ADC_open()
 *
 *  @param[in]  adcValue    A sampling result return from ADC_convert()
 *
 *  @return @p adcValue converted into microvolts
 *
 *  @sa     ADC_convert()
 */
extern uint32_t ADC_convertToMicroVolts(ADC_Handle handle,
    uint16_t adcValue);

/*!
 *  @brief  Function to initialize the ADC driver.
 *
 *  This function must also be called before any other ADC driver APIs.
 */
extern void ADC_init(void);

/*!
 *  @brief  Function to initialize the ADC peripheral
 *
 *  Function to initialize the ADC peripheral specified by the
 *  particular index value.
 *
 *  @pre    ADC_init() has been called
 *
 *  @param[in]  index     Index in the @p ADC_Config[] array.
 *  @param[in]  params    Pointer to an initialized #ADC_Params structure.
 *                        If NULL, the default #ADC_Params values are used.
 *
 *  @return An #ADC_Handle on success or NULL on error.
 *
 *  @sa     ADC_init()
 *  @sa     ADC_close()
 */
extern ADC_Handle ADC_open(uint_least8_t index, ADC_Params *params);

/*!
 *  @brief  Initialize an #ADC_Params structure to its default values.
 *
 *  @param[in]  params  A pointer to an #ADC_Params structure.
 *
 *  Default values are:
 *  @arg #ADC_Params.custom = NULL
 *  @arg #ADC_Params.isProtected = true
 */
extern void ADC_Params_init(ADC_Params *params);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ADC__include */
