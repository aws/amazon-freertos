// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _TEST_IOT_CONFIG_H_
#define _TEST_IOT_CONFIG_H_

/* I2C includes */
#include "iot_i2c.h"

/* SPI includes */
#include "iot_spi.h"


/*------------------------UART-------------------------------*/

/* UART is supported. */
#define IOT_TEST_COMMON_IO_UART_SUPPORTED    1

/* How many UART ports are being test. */
#define UART_TEST_SET                        1

/* Instance ID 3 corresponds to UART4. */
extern const uint8_t uartTestPort[ UART_TEST_SET ];
/* Following configurations are not being used for now. */
extern const uint32_t uartIotUartFlowControl[ UART_TEST_SET ];
extern const uint32_t uartIotUartParity[ UART_TEST_SET ];
extern const uint32_t uartIotUartWordLength[ UART_TEST_SET ];
extern const uint32_t uartIotUartStopBits[ UART_TEST_SET ];

/* How many I2C ports are being test. */
#define I2C_TEST_SET                        1

#define IOT_TEST_COMMON_IO_I2C_SUPPORTED                     1

#if ( IOT_TEST_COMMON_IO_I2C_SUPPORTED == 1 )
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_SEND_NO_STOP    1
    #define IOT_TEST_COMMON_IO_I2C_SUPPORTED_CANCEL          0
#endif

extern uint16_t uctestIotI2CSlaveAddr;         /* Address of Slave I2C (7-bit address) connected to the bus */
extern uint8_t uctestIotI2CDeviceRegister;     /* Slave I2C register address used in read/write tests */
extern uint8_t uctestIotI2CWriteVal;           /* Write value that will be used in the register write test */
extern uint8_t uctestIotI2CInstanceIdx;        /* I2C instance used in the test */
extern uint8_t uctestIotI2CInstanceNum;        /* The total number of I2C instances on the device */
extern uint16_t ucAssistedTestIotI2CSlaveAddr; /* The slave address to be set for the assisted test. */

/*------------------------SPI-------------------------------*/
#define IOT_TEST_COMMON_IO_SPI_SUPPORTED                     1

#define SPI_TEST_SET                                         1

extern uint32_t ultestIotSpiInstance;
extern IotSPIMode_t xtestIotSPIDefaultConfigMode;
extern IotSPIBitOrder_t xtestIotSPIDefaultconfigBitOrder;
extern uint32_t ultestIotSPIFrequency;
extern uint32_t ultestIotSPIDummyValue;

#endif /* ifndef _TEST_IOT_CONFIG_H_ */
