/*******************************************************************************
  WILC1000 Peripherals Application Interface

  File Name:
    m2m_periph.h

  Summary:
    WILC1000 Peripherals Application Interface

  Description:
    WILC1000 Peripherals Application Interface
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2017 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#ifndef _M2M_PERIPH_H_
#define _M2M_PERIPH_H_


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


#include "driver/wifi/wilc1000/wireless_driver_extension/common/include/nm_common.h"
#include "driver/wifi/wilc1000/wireless_driver_extension/driver/include/m2m_types.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@struct	\
	tstrPerphInitParam

@brief
	Peripheral module initialization parameters.  
*/
typedef struct {
	void * arg;
} tstrPerphInitParam;


/*!
@enum	\
	tenuGpioNum

@brief
	A list of GPIO numbers configurable through the m2m_periph module.
*/
typedef enum {
	M2M_PERIPH_GPIO15, /*!< GPIO15 pad	*/
	M2M_PERIPH_GPIO16, /*!< GPIO16 pad	*/
	M2M_PERIPH_GPIO18, /*!< GPIO18 pad	*/
	M2M_PERIPH_GPIO_MAX
} tenuGpioNum;


/*!
@enum	\
	tenuI2cMasterSclMuxOpt

@brief
	Allowed pin multiplexing options for I2C master SCL signal.
*/
typedef enum {
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_HOST_WAKEUP, /*!< I2C master SCL is avaiable on HOST_WAKEUP. */
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_SD_DAT3,     /*!< I2C master SCL is avaiable on SD_DAT3 (GPIO 7). */
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO13,      /*!< I2C master SCL is avaiable on GPIO 13. */
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_GPIO4,       /*!< I2C master SCL is avaiable on GPIO 4.*/
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_I2C_SCL,     /*!< I2C master SCL is avaiable on I2C slave SCL. */
	M2M_PERIPH_I2C_MASTER_SCL_MUX_OPT_NUM
} tenuI2cMasterSclMuxOpt;

/*!
@enum	\
	tenuI2cMasterSdaMuxOpt

@brief
	Allowed pin multiplexing options for I2C master SDA signal.
*/
typedef enum {
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_RTC_CLK , /*!< I2C master SDA is avaiable on RTC_CLK. */
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_SD_CLK,   /*!< I2C master SDA is avaiable on SD_CLK (GPIO 8). */
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO14,   /*!< I2C master SDA is avaiable on GPIO 14. */
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_GPIO6,    /*!< I2C master SDA is avaiable on GPIO 6.*/
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_I2C_SDA,  /*!< I2C master SDA is avaiable on I2C slave SDA. */
	M2M_PERIPH_I2C_MASTER_SDA_MUX_OPT_NUM
} tenuI2cMasterSdaMuxOpt;


/*!
@struct	\
	tstrI2cMasterInitParam

@brief
	I2C master configuration parameters.  
@sa
	tenuI2cMasterSclMuxOpt
	tenuI2cMasterSdaMuxOpt
*/
typedef struct {
	uint8 enuSclMuxOpt; /*!< SCL multiplexing option. Allowed value are defined in tenuI2cMasterSclMuxOpt  */
	uint8 enuSdaMuxOpt; /*!< SDA multiplexing option. Allowed value are defined in tenuI2cMasterSdaMuxOpt  */
	uint8 u8ClkSpeedKHz; /*!< I2C master clock speed in KHz. */
} tstrI2cMasterInitParam;

/*!
@enum	\
	tenuI2cMasterFlags

@brief
	Bitwise-ORed flags for use in m2m_periph_i2c_master_write and m2m_periph_i2c_master_read
@sa
	m2m_periph_i2c_master_write
	m2m_periph_i2c_master_read
*/
typedef enum  {
    I2C_MASTER_NO_FLAGS          = 0x00, 
	/*!< No flags.  */
    I2C_MASTER_NO_STOP           = 0x01, 
	/*!< No stop bit after this transaction. Useful for scattered buffer read/write operations. */
	I2C_MASTER_NO_START          = 0x02, 
	/*!< No start bit at the beginning of this transaction. Useful for scattered buffer read/write operations.*/
} tenuI2cMasterFlags;

/*!
@enum	\
	tenuPullupMask

@brief
	Bitwise-ORed flags for use in m2m_perph_pullup_ctrl.
@sa
	m2m_periph_pullup_ctrl
	
*/
typedef enum {
	M2M_PERIPH_PULLUP_HOST_WAKEUP     = (1ul << 0),
	M2M_PERIPH_PULLUP_RTC_CLK         = (1ul << 1),
	M2M_PERIPH_PULLUP_IRQN            = (1ul << 2),
	M2M_PERIPH_PULLUP_GPIO_3          = (1ul << 3), 
	M2M_PERIPH_PULLUP_GPIO_4          = (1ul << 4),
	M2M_PERIPH_PULLUP_GPIO_5          = (1ul << 5),
	M2M_PERIPH_PULLUP_SD_DAT3         = (1ul << 6),
	M2M_PERIPH_PULLUP_SD_DAT2_SPI_RXD = (1ul << 7),
	M2M_PERIPH_PULLUP_SD_DAT1_SPI_SSN = (1ul << 8),
	M2M_PERIPH_PULLUP_SD_CMD_SPI_SCK  = (1ul << 9),
	M2M_PERIPH_PULLUP_SD_DAT0_SPI_TXD = (1ul << 10),
	M2M_PERIPH_PULLUP_GPIO_6          = (1ul << 11),
	M2M_PERIPH_PULLUP_SD_CLK          = (1ul << 12),
	M2M_PERIPH_PULLUP_I2C_SCL         = (1ul << 13),
	M2M_PERIPH_PULLUP_I2C_SDA         = (1ul << 14),
	M2M_PERIPH_PULLUP_GPIO_11         = (1ul << 15),
	M2M_PERIPH_PULLUP_GPIO_12         = (1ul << 16),
	M2M_PERIPH_PULLUP_GPIO_13         = (1ul << 17),
	M2M_PERIPH_PULLUP_GPIO_14         = (1ul << 18),
	M2M_PERIPH_PULLUP_GPIO_15         = (1ul << 19),
	M2M_PERIPH_PULLUP_GPIO_16         = (1ul << 20),
	M2M_PERIPH_PULLUP_GPIO_17         = (1ul << 21),
	M2M_PERIPH_PULLUP_GPIO_18         = (1ul << 22),
	M2M_PERIPH_PULLUP_GPIO_19         = (1ul << 23),
	M2M_PERIPH_PULLUP_GPIO_20         = (1ul << 24),
	M2M_PERIPH_PULLUP_GPIO_21         = (1ul << 25),
	M2M_PERIPH_PULLUP_GPIO_22         = (1ul << 26),
	M2M_PERIPH_PULLUP_GPIO_23         = (1ul << 27),
	M2M_PERIPH_PULLUP_GPIO_24         = (1ul << 28),
} tenuPullupMask;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


#ifdef __cplusplus
     extern "C" {
#endif

/*!
@fn	\
	NMI_API sint8 m2m_periph_init(tstrPerphInitParam * param);

@brief
	Initialize the NMC1500 peripheral driver module. 

@param [in]	param
				Peripheral module initialization structure. See members of tstrPerphInitParam.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tstrPerphInitParam
*/
NMI_API sint8 m2m_periph_init(tstrPerphInitParam * param);

/*!
@fn	\
	NMI_API sint8 m2m_periph_gpio_set_dir(uint8 u8GpioNum, uint8 u8GpioDir);

@brief
	Configure a specific NMC1500 pad as a GPIO and sets its direction (input or output).  

@param [in]	u8GpioNum
				GPIO number. Allowed values are defined in tenuGpioNum.

@param [in]	u8GpioDir
				GPIO direction: Zero = input. Non-zero = output.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuGpioNum
*/
NMI_API sint8 m2m_periph_gpio_set_dir(uint8 u8GpioNum, uint8 u8GpioDir);

/*!
@fn	\
	NMI_API sint8 m2m_periph_gpio_set_val(uint8 u8GpioNum, uint8 u8GpioVal);

@brief
	Set an NMC1500 GPIO output level high or low.  

@param [in]	u8GpioNum
				GPIO number. Allowed values are defined in tenuGpioNum.

@param [in]	u8GpioVal
				GPIO output value. Zero = low, non-zero = high.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuGpioNum
*/
NMI_API sint8 m2m_periph_gpio_set_val(uint8 u8GpioNum, uint8 u8GpioVal);

/*!
@fn	\
	NMI_API sint8 m2m_periph_gpio_get_val(uint8 u8GpioNum, uint8 * pu8GpioVal);

@brief
	Read an NMC1500 GPIO input level.  

@param [in]	u8GpioNum
				GPIO number. Allowed values are defined in tenuGpioNum.

@param [out] pu8GpioVal
				GPIO input value. Zero = low, non-zero = high.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuGpioNum
*/
NMI_API sint8 m2m_periph_gpio_get_val(uint8 u8GpioNum, uint8 * pu8GpioVal);

/*!
@fn	\
	NMI_API sint8 m2m_periph_gpio_pullup_ctrl(uint8 u8GpioNum, uint8 u8PullupEn);

@brief
	Set an NMC1500 GPIO pullup resisitor enable or disable.  

@param [in]	u8GpioNum
				GPIO number. Allowed values are defined in tenuGpioNum.

@param [in] u8PullupEn
				Zero: pullup disabled. Non-zero: pullup enabled. 

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuGpioNum
*/
NMI_API sint8 m2m_periph_gpio_pullup_ctrl(uint8 u8GpioNum, uint8 u8PullupEn);

/*!
@fn	\
	NMI_API sint8 m2m_periph_i2c_master_init(tstrI2cMasterInitParam * param);

@brief
	Initialize and configure the NMC1500 I2C master peripheral.

@param [in]	param
				I2C master initialization structure. See members of tstrI2cMasterInitParam.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tstrI2cMasterInitParam
*/
NMI_API sint8 m2m_periph_i2c_master_init(tstrI2cMasterInitParam * param);

/*!
@fn	\
	NMI_API sint8 m2m_periph_i2c_master_write(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint8 flags);

@brief
	Write a stream of bytes to the I2C slave device.

@param [in]	u8SlaveAddr
				7-bit I2C slave address.
@param [in]	pu8Buf
				A pointer to an input buffer which contains a stream of bytes.
@param [in]	u16BufLen
				Input buffer length in bytes. 
@param [in]	flags
				Write operation bitwise-ORed flags. See tenuI2cMasterFlags.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuI2cMasterFlags
*/
NMI_API sint8 m2m_periph_i2c_master_write(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint8 flags);


/*!
@fn	\
	NMI_API sint8 m2m_periph_i2c_master_read(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint16 * pu16ReadLen, uint8 flags);

@brief
	Write a stream of bytes to the I2C slave device.

@param [in]	u8SlaveAddr
				7-bit I2C slave address.
@param [out] pu8Buf
				A pointer to an output buffer in which a stream of bytes are received.
@param [in]	u16BufLen
				Max output buffer length in bytes. 
@param [out] pu16ReadLen
				Actual number of bytes received.
@param [in]	flags
				Write operation bitwise-ORed flags. See tenuI2cMasterFlags.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuI2cMasterFlags
*/
NMI_API sint8 m2m_periph_i2c_master_read(uint8 u8SlaveAddr, uint8 * pu8Buf, uint16 u16BufLen, uint16 * pu16ReadLen, uint8 flags);


/*!
@fn	\
	NMI_API sint8 m2m_periph_pullup_ctrl(uint32 pinmask, uint8 enable);

@brief
	Control the programmable pull-up resistor on the chip pads .


@param [in]	pinmask
				Write operation bitwise-ORed mask for which pads to control. Allowed values are defined in tenuPullupMask.

@param [in]	enable
				Set to 0 to disable pull-up resistor. Non-zero will enable the pull-up.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuPullupMask
*/
NMI_API sint8 m2m_periph_pullup_ctrl(uint32 pinmask, uint8 enable);

#ifdef __cplusplus
}
#endif


#endif /* _M2M_PERIPH_H_ */