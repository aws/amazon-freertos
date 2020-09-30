#include <stdint.h>

#include "iot_test_common_io_internal.h"

/*------------------------I2C-------------------------------*/

#define I2C_TEST_SET                                         1

/* 0x18 is the slave address for the on-board ACC sensor. */
const uint8_t i2cTestSlaveAddr[ I2C_TEST_SET ] = { 0x18 };
const uint8_t i2cTestDeviceRegister[ I2C_TEST_SET ] = { 0x30 };
const uint8_t i2cTestWriteVal[ I2C_TEST_SET ] = { 0x3 };
const uint8_t i2cTestInstanceIdx[ I2C_TEST_SET ] = { 0 };
const uint8_t i2cTestInstanceNum[ I2C_TEST_SET ] = { 1 };

/* Not used by tests in this code base. */
IotI2CHandle_t gIotI2cHandle[ 4 ] = { NULL, NULL, NULL, NULL };

#if defined( IOT_TEST_COMMON_IO_I2C_SUPPORTED ) && ( IOT_TEST_COMMON_IO_I2C_SUPPORTED >= 1 )
void SET_TEST_IOT_I2C_CONFIG(int testSet)
{
    uctestIotI2CSlaveAddr = i2cTestSlaveAddr[testSet];
    uctestIotI2CDeviceRegister = i2cTestDeviceRegister[testSet];
    uctestIotI2CWriteVal = i2cTestWriteVal[testSet];
    uctestIotI2CInstanceIdx = i2cTestInstanceIdx[testSet];
    uctestIotI2CInstanceNum = i2cTestInstanceNum[testSet];
}
#endif

/*------------------------UART-------------------------------*/

#define UART_TEST_SET                                       1

const uint8_t uartTestInstanceIdx [ UART_TEST_SET ] = { 1 };

#if defined( IOT_TEST_COMMON_IO_UART_SUPPORTED ) && ( IOT_TEST_COMMON_IO_UART_SUPPORTED >= 1 )
void SET_TEST_IOT_UART_CONFIG(int testSet)
{
    uctestIotUartPort = uartTestInstanceIdx[testSet]; 
}
#endif

/*------------------------SPI-------------------------------*/

#define SPI_TEST_SET                                       1

#if defined( IOT_TEST_COMMON_IO_SPI_SUPPORTED ) && ( IOT_TEST_COMMON_IO_SPI_SUPPORTED >= 1 )
void SET_TEST_IOT_SPI_CONFIG(int testSet)
{
    (void)testSet;
}
#endif


/*------------------------GPIO-------------------------------*/

#define GPIO_TEST_SET                                       1

// DIO17 and DIO18 based on GPIO driver index
const int32_t gpioIndexPortA = 0; 
const int32_t gpioIndexPortB = 1; 

#if defined( IOT_TEST_COMMON_IO_GPIO_SUPPORTED ) && ( IOT_TEST_COMMON_IO_GPIO_SUPPORTED >= 1 )
void SET_TEST_IOT_GPIO_CONFIG(int testSet)
{
    if ( testSet == 0 ) {
        ltestIotGpioPortA = gpioIndexPortA; 
	    ltestIotGpioPortB = gpioIndexPortB;
    } else {
        ltestIotGpioPortA = gpioIndexPortB; 
	    ltestIotGpioPortB = gpioIndexPortA;
    }
}
#endif

/*------------------------TIMER-------------------------------*/

#if defined( IOT_TEST_COMMON_IO_TIMER_SUPPORTED ) && ( IOT_TEST_COMMON_IO_TIMER_SUPPORTED >= 1 )
void SET_TEST_IOT_TIMER_CONFIG(int testSet) 
{
    (void)testSet;
}
#endif

/*------------------------FLASH-------------------------------*/

#define FLASH_TEST_SET                                       1

const uint8_t iotFlashInstance = 0;
extern uint8_t ltestIotFlashInstance;

#if defined( IOT_TEST_COMMON_IO_FLASH_SUPPORTED ) && ( IOT_TEST_COMMON_IO_FLASH_SUPPORTED >= 1 )
void SET_TEST_IOT_FLASH_CONFIG(int testSet)
{
    ltestIotFlashInstance = iotFlashInstance;
}
#endif

/*------------------------PWM-------------------------------*/

const uint32_t pwmTestInstanceIdx = 0;
const int32_t gpioPwmInputPin = 2;

#if defined( IOT_TEST_COMMON_IO_PWM_SUPPORTED ) && ( IOT_TEST_COMMON_IO_PWM_SUPPORTED >= 1 )
void SET_TEST_IOT_PWM_CONFIG(int testSet)
{   
    ultestIotPwmInstance = pwmTestInstanceIdx;
    ultestIotPwmGpioInputPin = gpioPwmInputPin;
}
#endif

/*------------------------RTC-------------------------------*/

#if defined( IOT_TEST_COMMON_IO_RTC_SUPPORTED ) && ( IOT_TEST_COMMON_IO_RTC_SUPPORTED >= 1 )
void SET_TEST_IOT_RTC_CONFIG(int testSet)
{
    (void)testSet;
}
#endif

/*------------------------WATCHDOG-------------------------------*/

#if defined( IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED ) && ( IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED >= 1 )
void SET_TEST_IOT_WATCHDOG_CONFIG(int testSet)
{
    (void)testSet;
}
#endif
