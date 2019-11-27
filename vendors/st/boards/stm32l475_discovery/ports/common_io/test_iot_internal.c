/*******************************************************************************
 * IOT On-Target Unit Test Internal - LPC54018
 *
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *******************************************************************************
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "test_iot_internal.h"

/* UART */
extern uint8_t ustestIotUartPort;
extern uint32_t ultestIotUartFlowControl;
extern uint32_t ultestIotUartParity;
extern uint32_t ultestIotUartWordLength;
extern uint32_t ultestIotUartStopBits;

void SET_TEST_IOT_UART_CONFIG( int testSet )
{
    ustestIotUartPort = uartTestPort[ testSet ];
    ultestIotUartFlowControl = uartIotUartFlowControl[ testSet ];
    ultestIotUartParity = uartIotUartParity[ testSet ];
    ultestIotUartWordLength = uartIotUartWordLength[ testSet ];
    ultestIotUartStopBits = uartIotUartStopBits[ testSet ];
}

/* I2C */
extern uint8_t uctestIotI2CSlaveAddr;
extern uint8_t xtestIotI2CDeviceRegister;
extern uint8_t uctestIotI2CWriteVal;
extern uint8_t uctestIotI2CInstanceIdx;
extern uint8_t uctestIotI2CInstanceNum;

void SET_TEST_IOT_I2C_CONFIG( int testSet )
{
    uctestIotI2CSlaveAddr = i2cTestSlaveAddr[ testSet ];
    xtestIotI2CDeviceRegister = i2cTestDeviceRegister[ testSet ];
    uctestIotI2CWriteVal = i2cTestWriteVal[ testSet ];
    uctestIotI2CInstanceIdx = i2cTestInstanceIdx[ testSet ];
    uctestIotI2CInstanceNum = i2cTestInstanceNum[ testSet ];
}

/* SPI */
extern uint8_t ultestIotSpiInstance;
extern IotSPIMode_t xtestIotSPIDefaultConfigMode;
extern IotSPIBitOrder_t xtestIotSPIDefaultconfigBitOrder;
extern uint32_t ultestIotSPIFrequency;
extern uint32_t ultestIotSPIDummyValue;
void SET_TEST_IOT_SPI_CONFIG(int testSet)
{
    ultestIotSpiInstance = spiTestPort[ testSet ] ;
    xtestIotSPIDefaultConfigMode = spiIotMode[ testSet ];
    xtestIotSPIDefaultconfigBitOrder = spiIotSpitBitOrder[ testSet ];
    ultestIotSPIFrequency = spiIotFrequency[ testSet ];
    ultestIotSPIDummyValue = spiIotDummyValue[ testSet ];
}
