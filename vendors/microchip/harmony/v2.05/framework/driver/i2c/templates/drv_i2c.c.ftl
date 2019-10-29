<#--
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
 -->
// <editor-fold defaultstate="collapsed" desc="DRV_I2C Initialization Data">
<#if CONFIG_DRV_I2C_DRIVER_MODE == "DYNAMIC">
// *****************************************************************************
/* I2C Driver Initialization Data
*/
<#-- Instance 0 -->
<#if CONFIG_DRV_I2C_INST_IDX0 == true>

const DRV_I2C_INIT drvI2C0InitData =
{
<#if CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0?has_content>
    .i2cId = DRV_I2C_PERIPHERAL_ID_IDX0,
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX0?has_content && CONFIG_DRV_I2C_BB_USED == true >
    .i2cPerph = DRV_I2C_BIT_BANG_IDX0,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX0?has_content>
    .i2cMode = DRV_I2C_OPERATION_MODE_IDX0,
</#if>
<#-- port and pin definitions included so that a bus reset routine can manually clear the I2C Bus -->
    .portSCL = DRV_SCL_PORT_IDX0,
	.pinSCL  = DRV_SCL_PIN_POSITION_IDX0,
	.portSDA = DRV_SDA_PORT_IDX0,
	.pinSDA  = DRV_SDA_PIN_POSITION_IDX0,
<#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == true >
    .baudRate = DRV_I2C_BIT_BANG_BAUD_RATE_IDX0,
	.tmrSource = DRV_I2C_BIT_BANG_TMR_MODULE_IDX0,
	.tmrInterruptSource = DRV_I2C_BIT_BANG_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_MASTER">
<#if CONFIG_DRV_I2C_BAUD_RATE_IDX0?has_content>
    .baudRate = DRV_I2C_BAUD_RATE_IDX0,
</#if>
<#if CONFIG_DRV_I2C_SLEW_RATE_CONTROL_IDX0?has_content>
    .busspeed = DRV_I2C_SLEW_RATE_CONTROL_IDX0,
</#if>
</#if>
<#if CONFIG_DRV_I2C_SMBus_SPECIFICATION_IDX0?has_content>
    .buslevel = DRV_I2C_SMBus_SPECIFICATION_IDX0,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX0 == "DRV_I2C_MODE_SLAVE">
<#if CONFIG_DRV_I2C_ADDRESS_WIDTH_IDX0?has_content>
    .addWidth = DRV_I2C_ADDRESS_WIDTH_IDX0,
</#if>
<#if CONFIG_DRV_I2C_STRICT_ADDRESS_RULE_IDX0?has_content>
    .reservedaddenable = DRV_I2C_STRICT_ADDRESS_RULE_IDX0,
</#if>
<#if CONFIG_DRV_I2C_GENERAL_CALL_ADDRESS_IDX0?has_content>
    .generalcalladdress = DRV_I2C_GENERAL_CALL_ADDRESS_IDX0,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_VALUE_IDX0?has_content>
    .slaveaddvalue = DRV_I2C_SLAVE_ADDRESS_VALUE_IDX0,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_MASK_IDX0?has_content>
    .maskslaveaddress = DRV_I2C_SLAVE_ADDRESS_MASK_IDX0,
</#if>	
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX0 == false>
<#if CONFIG_DRV_I2C_MASTER_INT_SRC_IDX0?has_content>
    .mstrInterruptSource = DRV_I2C_MASTER_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX0?has_content>
    .slaveInterruptSource = DRV_I2C_SLAVE_INT_SRC_IDX0,
</#if>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX0?has_content>
    .errInterruptSource = DRV_I2C_ERR_MX_INT_SRC_IDX0,
</#if>
<#else>
<#if CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX0?has_content>
    .errInterruptSource = DRV_I2C_ERR_MZ_INT_SRC_IDX0,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX0?has_content>
    .operationStarting = DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX0,
</#if>

</#if>
</#if>
};
</#if>

<#-- Instance 1 -->
<#if CONFIG_DRV_I2C_INST_IDX1 == true>

const DRV_I2C_INIT drvI2C1InitData =
{
<#if CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1?has_content>
    .i2cId = DRV_I2C_PERIPHERAL_ID_IDX1,
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX1?has_content  && CONFIG_DRV_I2C_BB_USED == true >
    .i2cPerph = DRV_I2C_BIT_BANG_IDX1,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX1?has_content>
    .i2cMode = DRV_I2C_OPERATION_MODE_IDX1,
</#if>
<#-- port and pin definitions included so that a bus reset routine can manually clear the I2C Bus -->
    .portSCL = DRV_SCL_PORT_IDX1,
	.pinSCL  = DRV_SCL_PIN_POSITION_IDX1,
	.portSDA = DRV_SDA_PORT_IDX1,
	.pinSDA  = DRV_SDA_PIN_POSITION_IDX1,
<#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == true>
    .baudRate = DRV_I2C_BIT_BANG_BAUD_RATE_IDX1,
	.tmrSource = DRV_I2C_BIT_BANG_TMR_MODULE_IDX1,
	.tmrInterruptSource = DRV_I2C_BIT_BANG_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SMBus_SPECIFICATION_IDX1?has_content>
    .buslevel = DRV_I2C_SMBus_SPECIFICATION_IDX1,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_MASTER">
<#if CONFIG_DRV_I2C_BAUD_RATE_IDX1?has_content>
    .baudRate = DRV_I2C_BAUD_RATE_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SLEW_RATE_CONTROL_IDX1?has_content>
    .busspeed = DRV_I2C_SLEW_RATE_CONTROL_IDX1,
</#if>
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX1 == "DRV_I2C_MODE_SLAVE">
<#if CONFIG_DRV_I2C_ADDRESS_WIDTH_IDX1?has_content>
    .addWidth = DRV_I2C_ADDRESS_WIDTH_IDX1,
</#if>
<#if CONFIG_DRV_I2C_STRICT_ADDRESS_RULE_IDX1?has_content>
    .reservedaddenable = DRV_I2C_STRICT_ADDRESS_RULE_IDX1,
</#if>
<#if CONFIG_DRV_I2C_GENERAL_CALL_ADDRESS_IDX1?has_content>
    .generalcalladdress = DRV_I2C_GENERAL_CALL_ADDRESS_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_VALUE_IDX1?has_content>
    .slaveaddvalue = DRV_I2C_SLAVE_ADDRESS_VALUE_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_MASK_IDX1?has_content>
    .maskslaveaddress = DRV_I2C_SLAVE_ADDRESS_MASK_IDX1,
</#if>	
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX1 == false>
<#if CONFIG_DRV_I2C_MASTER_INT_SRC_IDX1?has_content>
    .mstrInterruptSource = DRV_I2C_MASTER_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX1?has_content>
    .slaveInterruptSource = DRV_I2C_SLAVE_INT_SRC_IDX1,
</#if>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX1?has_content>
    .errInterruptSource = DRV_I2C_ERR_MX_INT_SRC_IDX1,
</#if>
<#else>
<#if CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX1?has_content>
    .errInterruptSource = DRV_I2C_ERR_MZ_INT_SRC_IDX1,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX1?has_content>
    .operationStarting = DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX1,
</#if>

</#if>
</#if>
};
</#if>

<#-- Instance 2 -->
<#if CONFIG_DRV_I2C_INST_IDX2 == true>

const DRV_I2C_INIT drvI2C2InitData =
{
<#if CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2?has_content>
    .i2cId = DRV_I2C_PERIPHERAL_ID_IDX2,
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX2?has_content && CONFIG_DRV_I2C_BB_USED == true >
    .i2cPerph = DRV_I2C_BIT_BANG_IDX2,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX2?has_content>
    .i2cMode = DRV_I2C_OPERATION_MODE_IDX2,
</#if>
<#-- port and pin definitions included so that a bus reset routine can manually clear the I2C Bus -->
    .portSCL = DRV_SCL_PORT_IDX2,
	.pinSCL  = DRV_SCL_PIN_POSITION_IDX2,
	.portSDA = DRV_SDA_PORT_IDX2,
	.pinSDA  = DRV_SDA_PIN_POSITION_IDX2,
<#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == true>
    .baudRate = DRV_I2C_BIT_BANG_BAUD_RATE_IDX2,
	.tmrSource = DRV_I2C_BIT_BANG_TMR_MODULE_IDX2,
	.tmrInterruptSource = DRV_I2C_BIT_BANG_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_MASTER">
<#if CONFIG_DRV_I2C_BAUD_RATE_IDX2?has_content>
    .baudRate = DRV_I2C_BAUD_RATE_IDX2,
</#if>
<#if CONFIG_DRV_I2C_SLEW_RATE_CONTROL_IDX2?has_content>
    .busspeed = DRV_I2C_SLEW_RATE_CONTROL_IDX2,
</#if>
</#if>
<#if CONFIG_DRV_I2C_SMBus_SPECIFICATION_IDX2?has_content>
    .buslevel = DRV_I2C_SMBus_SPECIFICATION_IDX2,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX2 == "DRV_I2C_MODE_SLAVE">
<#if CONFIG_DRV_I2C_STRICT_ADDRESS_RULE_IDX2?has_content>
    .reservedaddenable = DRV_I2C_STRICT_ADDRESS_RULE_IDX2,
</#if>
<#if CONFIG_DRV_I2C_GENERAL_CALL_ADDRESS_IDX2?has_content>
    .generalcalladdress = DRV_I2C_GENERAL_CALL_ADDRESS_IDX2,
</#if>
<#if CONFIG_DRV_I2C_ADDRESS_WIDTH_IDX2?has_content>
    .addWidth = DRV_I2C_ADDRESS_WIDTH_IDX2,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_VALUE_IDX2?has_content>
    .slaveaddvalue = DRV_I2C_SLAVE_ADDRESS_VALUE_IDX2,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_MASK_IDX2?has_content>
    .maskslaveaddress = DRV_I2C_SLAVE_ADDRESS_MASK_IDX2,
</#if>
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX2 == false>
<#if CONFIG_DRV_I2C_MASTER_INT_SRC_IDX2?has_content>
    .mstrInterruptSource = DRV_I2C_MASTER_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX2?has_content>
    .slaveInterruptSource = DRV_I2C_SLAVE_INT_SRC_IDX2,
</#if>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX2?has_content>
    .errInterruptSource = DRV_I2C_ERR_MX_INT_SRC_IDX2,
</#if>
<#else>
<#if CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX2?has_content>
    .errInterruptSource = DRV_I2C_ERR_MZ_INT_SRC_IDX2,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX2?has_content>
    .operationStarting = DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX2,
</#if>

</#if>
</#if>
};
</#if>

<#-- Instance 3 -->
<#if CONFIG_DRV_I2C_INST_IDX3 == true>

const DRV_I2C_INIT drvI2C3InitData =
{
<#if CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3?has_content>
    .i2cId = DRV_I2C_PERIPHERAL_ID_IDX3,
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX3?has_content && CONFIG_DRV_I2C_BB_USED == true >
    .i2cPerph = DRV_I2C_BIT_BANG_IDX3,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX3?has_content>
    .i2cMode = DRV_I2C_OPERATION_MODE_IDX3,
</#if>
<#-- port and pin definitions included so that a bus reset routine can manually clear the I2C Bus -->
    .portSCL = DRV_SCL_PORT_IDX3,
	.pinSCL  = DRV_SCL_PIN_POSITION_IDX3,
	.portSDA = DRV_SDA_PORT_IDX3,
	.pinSDA  = DRV_SDA_PIN_POSITION_IDX3,
<#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == true>
    .baudRate = DRV_I2C_BIT_BANG_BAUD_RATE_IDX3,
	.tmrSource = DRV_I2C_BIT_BANG_TMR_MODULE_IDX3,
	.tmrInterruptSource = DRV_I2C_BIT_BANG_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_MASTER">
<#if CONFIG_DRV_I2C_BAUD_RATE_IDX3?has_content>
    .baudRate = DRV_I2C_BAUD_RATE_IDX3,
</#if>
<#if CONFIG_DRV_I2C_SLEW_RATE_CONTROL_IDX3?has_content>
    .busspeed = DRV_I2C_SLEW_RATE_CONTROL_IDX3,
</#if>
</#if>
<#if CONFIG_DRV_I2C_SMBus_SPECIFICATION_IDX3?has_content>
    .buslevel = DRV_I2C_SMBus_SPECIFICATION_IDX3,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX3 == "DRV_I2C_MODE_SLAVE">
<#if CONFIG_DRV_I2C_ADDRESS_WIDTH_IDX3?has_content>
    .addWidth = DRV_I2C_ADDRESS_WIDTH_IDX3,
</#if>
<#if CONFIG_DRV_I2C_STRICT_ADDRESS_RULE_IDX3?has_content>
    .reservedaddenable = DRV_I2C_STRICT_ADDRESS_RULE_IDX3,
</#if>
<#if CONFIG_DRV_I2C_GENERAL_CALL_ADDRESS_IDX3?has_content>
    .generalcalladdress = DRV_I2C_GENERAL_CALL_ADDRESS_IDX3,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_VALUE_IDX3?has_content>
    .slaveaddvalue = DRV_I2C_SLAVE_ADDRESS_VALUE_IDX3,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_MASK_IDX3?has_content>
    .maskslaveaddress = DRV_I2C_SLAVE_ADDRESS_MASK_IDX3,
</#if>
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX3 == false>
<#if CONFIG_DRV_I2C_MASTER_INT_SRC_IDX3?has_content>
    .mstrInterruptSource = DRV_I2C_MASTER_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX3?has_content>
    .slaveInterruptSource = DRV_I2C_SLAVE_INT_SRC_IDX3,
</#if>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX3?has_content>
    .errInterruptSource = DRV_I2C_ERR_MX_INT_SRC_IDX3,
</#if>
<#else>
<#if CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX3?has_content>
    .errInterruptSource = DRV_I2C_ERR_MZ_INT_SRC_IDX3,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX3?has_content>
    .operationStarting = DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX3,
</#if>

</#if>
</#if>
};
</#if>

<#-- Instance 4 -->
<#if CONFIG_DRV_I2C_INST_IDX4 == true>

const DRV_I2C_INIT drvI2C4InitData =
{
<#if CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4?has_content>
    .i2cId = DRV_I2C_PERIPHERAL_ID_IDX4,
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX4?has_content  && CONFIG_DRV_I2C_BB_USED == true >
    .i2cPerph = DRV_I2C_BIT_BANG_IDX4,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX4?has_content>
    .i2cMode = DRV_I2C_OPERATION_MODE_IDX4,
</#if>
<#-- port and pin definitions included so that a bus reset routine can manually clear the I2C Bus -->
    .portSCL = DRV_SCL_PORT_IDX4,
	.pinSCL  = DRV_SCL_PIN_POSITION_IDX4,
	.portSDA = DRV_SDA_PORT_IDX4,
	.pinSDA  = DRV_SDA_PIN_POSITION_IDX4,
<#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == true>
    .baudRate = DRV_I2C_BIT_BANG_BAUD_RATE_IDX4,
	.tmrSource = DRV_I2C_BIT_BANG_TMR_MODULE_IDX4,
	.tmrInterruptSource = DRV_I2C_BIT_BANG_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_MASTER">
<#if CONFIG_DRV_I2C_SMBus_SPECIFICATION_IDX4?has_content>
    .buslevel = DRV_I2C_SMBus_SPECIFICATION_IDX4,
</#if>
<#if CONFIG_DRV_I2C_BAUD_RATE_IDX4?has_content>
    .baudRate = DRV_I2C_BAUD_RATE_IDX4,
</#if>
<#if CONFIG_DRV_I2C_SLEW_RATE_CONTROL_IDX4?has_content>
    .busspeed = DRV_I2C_SLEW_RATE_CONTROL_IDX4,
</#if>
</#if>
<#if CONFIG_DRV_I2C_ADDRESS_WIDTH_IDX4?has_content>
    .addWidth = DRV_I2C_ADDRESS_WIDTH_IDX4,
</#if>
<#if CONFIG_DRV_I2C_OPERATION_MODE_IDX4 == "DRV_I2C_MODE_SLAVE">
<#if CONFIG_DRV_I2C_STRICT_ADDRESS_RULE_IDX4?has_content>
    .reservedaddenable = DRV_I2C_STRICT_ADDRESS_RULE_IDX4,
</#if>
<#if CONFIG_DRV_I2C_GENERAL_CALL_ADDRESS_IDX4?has_content>
    .generalcalladdress = DRV_I2C_GENERAL_CALL_ADDRESS_IDX4,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_VALUE_IDX4?has_content>
    .slaveaddvalue = DRV_I2C_SLAVE_ADDRESS_VALUE_IDX4,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_ADDRESS_MASK_IDX4?has_content>
    .maskslaveaddress = DRV_I2C_SLAVE_ADDRESS_MASK_IDX4,
</#if>
</#if>
<#if CONFIG_DRV_I2C_BIT_BANG_IDX4 == false>
<#if CONFIG_DRV_I2C_MASTER_INT_SRC_IDX4?has_content>
    .mstrInterruptSource = DRV_I2C_MASTER_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX4?has_content>
    .slaveInterruptSource = DRV_I2C_SLAVE_INT_SRC_IDX4,
</#if>
<#if CONFIG_PIC32MX == true>
<#if CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX4?has_content>
    .errInterruptSource = DRV_I2C_ERR_MX_INT_SRC_IDX4,
</#if>
<#else>
<#if CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX4?has_content>
    .errInterruptSource = DRV_I2C_ERR_MZ_INT_SRC_IDX4,
</#if>
<#if CONFIG_DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX4?has_content>
    .operationStarting = DRV_I2C_SLAVE_CALLBACK_FUNCTION_IDX4,
</#if>

</#if>
</#if>
};
</#if>
</#if>
// </editor-fold>
<#--
/*******************************************************************************
 End of File
*/
-->
