/*******************************************************************************
  I2C Driver Functions for Static Single Instance Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_static_buffer_model.c

  Summary:
    I2C driver implementation for the static single instance driver.

  Description:
    The I2C device driver provides a simple interface to manage the I2C
    modules on Microchip microcontrollers. This file contains interface
    functions for the I2C driver.

  Remarks:
    Static interfaces incorporate the driver instance number within the names
    of the routines, eliminating the need for an object ID or object handle.

    Static single-open interfaces also eliminate the need for the open handle.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
#include "driver/i2c/src/drv_i2c_local.h"

<#if CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
static uint32_t ReadCoreTimer(void);

static uint32_t ReadCoreTimer()
{
    volatile uint32_t timer;

    // get the count reg
    asm volatile("mfc0   %0, $9" : "=r"(timer));

    return(timer);
}

</#if>

// *****************************************************************************
/*
 Upper address limit for a 7-bit address
 */

#define ADDRESS_7BIT_UPPER_LIMIT                    0xFF

typedef struct _DRV_STATIC_I2C_OBJ
{

    DRV_I2C_MODE            i2cMode;

    uint8_t                 slaveaddresshighbyte;

    uint8_t                 slaveaddresslowbyte;

    uint8_t *               txBuffer;

    uint8_t *               rxBuffer;

    DRV_I2C_OPERATIONS      i2coperation;

    uint8_t                 transfersize;

    uint8_t                 readtransfersize;

    uint32_t                actualtransfersize;

    bool                    transmitForced;

    DRV_I2C_BUS_ERROR_EVENT errorEvent;

    DRV_I2C_BUFFER_EVENT    bufferstatus;

    DRV_I2C_CallBack        operationStarting;

}DRV_STATIC_I2C_OBJECT;


typedef struct _DRV_STATIC_I2C_CLIENT_OBJ
{

    /* Save the context, will be passed back with the call back */
    uintptr_t                             context;

    /* I2C callback */
    DRV_I2C_BUFFER_EVENT_HANDLER          callback;

}DRV_STATIC_I2C_CLIENT_OBJ;

<#macro DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE I2C_INSTANCE MODE SLEW_RATE BRG_CLOCK BAUD SMBus_SPEC
ADDRESS_WIDTH STRICT_ADDRESS_RULE GENERAL_ADDRESS CLOCK_STRETCH ADDRESS_MASK SLAVE_ADDRESS MASTER_INT_SRC
SLAVE_CALLBACK SLAVE_INT_SRC ERR_INT_SRC INT_VECTOR ISR_VECTOR INT_PRIORITY INT_SUB_PRIORITY MASTER_INT_VECTOR
MASTER_ISR_VECTOR MASTER_INT_PRIORITY MASTER_INT_SUB_PRIORITY SLAVE_INT_VECTOR SLAVE_ISR_VECTOR
SLAVE_INT_PRIORITY SLAVE_INT_SUB_PRIORITY ERR_INT_VECTOR ERR_INT_PRIORITY ERR_INT_SUB_PRIORITY PWR_MODE>

<#if MODE == "DRV_I2C_MODE_SLAVE">
/* Callback for Slave */
/* Function should be implemented in application */

void ${SLAVE_CALLBACK}(DRV_I2C_BUFFER_EVENT event, void * context);
</#if>

<#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >

bool _DRV_I2C${DRV_INSTANCE}_MasterStop(void);

#define BAUD_RATE_${DRV_INSTANCE}         ${BAUD}
#define BRG_1_TIME_${DRV_INSTANCE}        SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2)/(2*BAUD_RATE_${DRV_INSTANCE})
</#if>

/* Master Object variables*/
DRV_I2C_DATA_OBJECT_TASK                    i2c${DRV_INSTANCE}State;
DRV_STATIC_I2C_OBJECT                       i2c${DRV_INSTANCE}Obj;
DRV_STATIC_I2C_CLIENT_OBJ                   i2c${DRV_INSTANCE}ClientObj;

/* Variables to maintain Queue State */
volatile uint16_t QueueIn_${DRV_INSTANCE}     = 0;
volatile uint16_t QueueOut_${DRV_INSTANCE}    = 0;
volatile uint16_t QueueDelta_${DRV_INSTANCE}  = 0;

static DRV_STATIC_I2C_OBJECT sDrvI2CBufferObj_${DRV_INSTANCE}[DRV_I2C_NUM_OF_BUFFER_OBJECTS];

DRV_STATIC_I2C_OBJECT* lBufferObj${DRV_INSTANCE};

// *****************************************************************************
// Section: Queue ${DRV_INSTANCE} Initialize Queue index variables
// *****************************************************************************
void QueueInitialize_${DRV_INSTANCE}(void)
{
    QueueIn_0 =  0;
    QueueOut_0 = 0;
}

// *****************************************************************************
// Section: Queue ${DRV_INSTANCE} Find a slot in queue
// *****************************************************************************

DRV_STATIC_I2C_OBJECT*  _DRV_I2C_QueueSlotGet_${DRV_INSTANCE}(void)
{

    uint8_t numberOfSlotsAvailable;

    DRV_STATIC_I2C_OBJECT*   lQueueObj;


    if (QueueIn_${DRV_INSTANCE} >= QueueOut_${DRV_INSTANCE})
    {
        numberOfSlotsAvailable = (DRV_I2C_NUM_OF_BUFFER_OBJECTS-(QueueIn_${DRV_INSTANCE} - QueueOut_${DRV_INSTANCE}) -1);
    }
    else
    {
        numberOfSlotsAvailable = ((QueueOut_${DRV_INSTANCE} - QueueIn_${DRV_INSTANCE}) -1);
    }

    if (numberOfSlotsAvailable > 0)
    {
        lQueueObj = &sDrvI2CBufferObj_${DRV_INSTANCE}[QueueIn_${DRV_INSTANCE}];

        QueueIn_${DRV_INSTANCE}++;

        if (QueueIn_${DRV_INSTANCE} >= (DRV_I2C_NUM_OF_BUFFER_OBJECTS))
        {
            QueueIn_${DRV_INSTANCE} = 0;
        }

        return lQueueObj;
    }

    return (DRV_STATIC_I2C_OBJECT*)NULL;

}

// *****************************************************************************
/* Checks if Queue is empty, returns true if Queue is empty else returns false*/
// *****************************************************************************

bool IsQueueEmpty_${DRV_INSTANCE}(void)
{
    if ( (QueueOut_${DRV_INSTANCE} == QueueIn_${DRV_INSTANCE}) )
        return true;

    return false;
}

// **************************************************
/* returns the next item from the queue             */
// **************************************************

DRV_STATIC_I2C_OBJECT* _DRV_I2C_QueuePop_${DRV_INSTANCE}(void)
{
    uint8_t temp;

    DRV_STATIC_I2C_OBJECT* lQueuePop;

    lQueuePop = NULL;

    /* if queue is empty then return NULL */
    if (IsQueueEmpty_${DRV_INSTANCE}())
    {
        Nop();
        return (DRV_STATIC_I2C_OBJECT*)NULL;
    }

    temp = QueueOut_${DRV_INSTANCE};

    lQueuePop = &sDrvI2CBufferObj_${DRV_INSTANCE}[temp];

    <#if MODE == "DRV_I2C_MODE_SLAVE">

    if (i2c${DRV_INSTANCE}Obj.i2cMode == DRV_I2C_MODE_SLAVE)
    {
        if ( QueueOut_${DRV_INSTANCE} >= (DRV_I2C_NUM_OF_BUFFER_OBJECTS-1) )
            QueueOut_${DRV_INSTANCE} = 0;
        else
            QueueOut_${DRV_INSTANCE}++;
    }

    </#if>

    return lQueuePop;

}

<#if MODE == "DRV_I2C_MODE_MASTER">
void _DRV_I2C_Advance_Queue_${DRV_INSTANCE}(void)
{

    if ( QueueOut_${DRV_INSTANCE} >= (DRV_I2C_NUM_OF_BUFFER_OBJECTS-1) )
        QueueOut_${DRV_INSTANCE} = 0;
    else
        QueueOut_${DRV_INSTANCE}++;

}
</#if>

// *****************************************************************************
// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} static driver functions
// *****************************************************************************
// *****************************************************************************
void DRV_I2C${DRV_INSTANCE}_Initialize(void)
{
    /* Initialize I2C${DRV_INSTANCE} */
<#if MODE == "DRV_I2C_MODE_MASTER">
<#if CONFIG_PIC32MX == true>
    PLIB_I2C_BaudRateSet(${I2C_INSTANCE}, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), ${BAUD});
<#else>
    PLIB_I2C_BaudRateSet(${I2C_INSTANCE}, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2), ${BAUD});
</#if>
<#if PWR_MODE == "SYS_MODULE_POWER_IDLE_STOP">
    PLIB_I2C_StopInIdleEnable(${I2C_INSTANCE});
<#else>
    PLIB_I2C_StopInIdleDisable(${I2C_INSTANCE});
</#if>
<#if SLEW_RATE == true>

    /* High frequency is enabled (**NOTE** PLIB function logic inverted) */
    PLIB_I2C_HighFrequencyDisable(${I2C_INSTANCE});
<#else>

    /* Low frequency is enabled (**NOTE** PLIB function logic reverted) */
    PLIB_I2C_HighFrequencyEnable(${I2C_INSTANCE});
</#if>
    i2c${DRV_INSTANCE}Obj.i2cMode         = DRV_I2C_MODE_MASTER;
    i2c${DRV_INSTANCE}Obj.transfersize    = 0;
    i2c${DRV_INSTANCE}State               = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
<#else>
<#if ADDRESS_WIDTH == "DRV_I2C_7BIT_SLAVE">
    /* The I2C module shifts left internally, so shifting right to account for that */
    PLIB_I2C_SlaveAddress7BitSet(${I2C_INSTANCE}, ${SLAVE_ADDRESS}>>1);
    PLIB_I2C_SlaveMask7BitSet(${I2C_INSTANCE}, ${ADDRESS_MASK});
<#else>
    PLIB_I2C_SlaveAddress10BitSet(${I2C_INSTANCE}, ${SLAVE_ADDRESS}>>1);
    PLIB_I2C_SlaveMask10BitSet(${I2C_INSTANCE}, ${ADDRESS_MASK});
</#if>
<#if GENERAL_ADDRESS == true>
    PLIB_I2C_GeneralCallEnable(${I2C_INSTANCE});
<#else>
    PLIB_I2C_GeneralCallDisable(${I2C_INSTANCE});
</#if>
<#if STRICT_ADDRESS_RULE == true>
    PLIB_I2C_ReservedAddressProtectEnable(${I2C_INSTANCE});
<#else>
    PLIB_I2C_ReservedAddressProtectDisable(${I2C_INSTANCE});
</#if>
<#if CLOCK_STRETCH == true>
    PLIB_I2C_SlaveClockStretchingEnable(${I2C_INSTANCE});
<#else>
    PLIB_I2C_SlaveClockStretchingEnable(${I2C_INSTANCE});
</#if>

    /*include callback function, see commented out line below for example */
    /* refer at top of the file for example   */
    i2c${DRV_INSTANCE}Obj.operationStarting = &${SLAVE_CALLBACK};

    i2c${DRV_INSTANCE}Obj.i2cMode         = DRV_I2C_MODE_SLAVE;
    i2c${DRV_INSTANCE}Obj.transfersize    = 0;
    i2c${DRV_INSTANCE}State               = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
</#if>

    QueueInitialize_${DRV_INSTANCE}();

<#if CONFIG_DRV_I2C_INTERRUPT_MODE == true>
<#if CONFIG_PIC32MX == true>
    /* Initialize interrupts */
<#if MODE == "DRV_I2C_MODE_MASTER">
    PLIB_INT_SourceFlagClear(INT_ID_0, ${MASTER_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${MASTER_INT_SRC});
<#else>
    PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
</#if>
    PLIB_INT_SourceFlagClear(INT_ID_0, ${ERR_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${ERR_INT_SRC});
    PLIB_INT_VectorPrioritySet(INT_ID_0, ${INT_VECTOR}, ${INT_PRIORITY});
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, ${INT_VECTOR}, ${INT_SUB_PRIORITY});
</#if>

<#if CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true>
<#if MODE == "DRV_I2C_MODE_MASTER">
    /* Initialize master interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${MASTER_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${MASTER_INT_SRC});
    PLIB_INT_VectorPrioritySet(INT_ID_0, ${MASTER_INT_VECTOR}, ${MASTER_INT_PRIORITY});
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, ${MASTER_INT_VECTOR}, ${MASTER_INT_SUB_PRIORITY});
<#else>
    /* Initialize slave interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
    PLIB_INT_VectorPrioritySet(INT_ID_0, ${SLAVE_INT_VECTOR}, ${SLAVE_INT_PRIORITY});
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, ${SLAVE_INT_VECTOR}, ${SLAVE_INT_SUB_PRIORITY});
</#if>

    /* Initialize fault interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, ${ERR_INT_SRC});
    PLIB_INT_SourceEnable(INT_ID_0, ${ERR_INT_SRC});
    PLIB_INT_VectorPrioritySet(INT_ID_0, ${ERR_INT_VECTOR}, ${ERR_INT_PRIORITY});
    PLIB_INT_VectorSubPrioritySet(INT_ID_0, ${ERR_INT_VECTOR}, ${ERR_INT_SUB_PRIORITY});

</#if>
</#if>

    /* Enable I2C${DRV_INSTANCE} */
    PLIB_I2C_Enable(${I2C_INSTANCE});

<#if CONFIG_DEVICE?contains("EC")>
<#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true>
<#if I2C_INSTANCE == "I2C_ID_1">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
<#else>
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_2">
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2);
</#if>
<#if I2C_INSTANCE == "I2C_ID_3">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
<#else>
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_4">
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
</#if>
<#if I2C_INSTANCE == "I2C_ID_5">
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_4);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_5);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_4);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_5);
</#if>
</#if>
</#if>

<#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32WK == true>
<#if CONFIG_DEVICE == "PIC32WK2057GPD132" || CONFIG_DEVICE == "PIC32WK2057GPB132" >
<#if I2C_INSTANCE == "I2C_ID_1">
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_7);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_7);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_6);
</#if>
<#if I2C_INSTANCE == "I2C_ID_2">
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_5);
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_5);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_4);
</#if>
</#if>
</#if>
}

void DRV_I2C${DRV_INSTANCE}_DeInitialize(void)
{
    /* Disable I2C${DRV_INSTANCE} */
    PLIB_I2C_Disable(${I2C_INSTANCE});
}

// *****************************************************************************
// Section: I2C Instance ${DRV_INSTANCE} Function to read a buffer (Master/Slave)
// *****************************************************************************

DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_Receive (uint16_t deviceaddress,
                                    void *rxBuffer,
                                    size_t size,
                                    void * context)
{
       DRV_STATIC_I2C_OBJECT* i2cBufferData;

    i2cBufferData = _DRV_I2C_QueueSlotGet_${DRV_INSTANCE}();


    if (i2cBufferData != NULL)
    {
        <#if MODE == "DRV_I2C_MODE_MASTER">
        i2cBufferData->i2cMode          = DRV_I2C_MODE_MASTER;
        <#else>
        i2cBufferData->i2cMode          = DRV_I2C_MODE_SLAVE;
        </#if>
        if (deviceaddress > ADDRESS_7BIT_UPPER_LIMIT )
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)((deviceaddress & 0xFF00)>>8);
            i2cBufferData->slaveaddresslowbyte  = (uint8_t)(deviceaddress & 0x00FF);
        }
        else
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cBufferData->slaveaddresslowbyte  = 0;
        }
        i2cBufferData->transfersize     = size;
        i2cBufferData->i2coperation     = DRV_I2C_OP_READ;
        i2cBufferData->rxBuffer         = rxBuffer;
        i2cBufferData->bufferstatus     = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cBufferData->transmitForced   = false;
        <#if MODE == "DRV_I2C_MODE_MASTER">
        if (i2cBufferData->i2cMode == DRV_I2C_MODE_MASTER)
        {
            /*  if either START and STOP were not detected which is true the
            first time OR if STOP was detected, then it assumed the
            transaction on the bus is complete */

            if ( ((!(PLIB_I2C_StartWasDetected(${I2C_INSTANCE}))) && (!(PLIB_I2C_StopWasDetected(${I2C_INSTANCE})))) || (PLIB_I2C_StopWasDetected(${I2C_INSTANCE})) )
            {
                if ( (PLIB_I2C_BusIsIdle(${I2C_INSTANCE})) && (!(PLIB_INT_SourceFlagGet(INT_ID_0, ${MASTER_INT_SRC}))) )
                {
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                        PLIB_I2C_MasterStart(${I2C_INSTANCE});
                }
            }
        }
        <#else>
        PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
        PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
        </#if>
    }

    return (DRV_I2C_BUFFER_HANDLE)i2cBufferData;
}

// *****************************************************************************
// Section: I2C Instance ${DRV_INSTANCE} Function to write a buffer (Master/Slave)
// *****************************************************************************

DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_Transmit (uint16_t deviceaddress,
                                    void *txBuffer,
                                    size_t size,
                                    void * context)
{
    DRV_STATIC_I2C_OBJECT* i2cBufferData;

    i2cBufferData = _DRV_I2C_QueueSlotGet_${DRV_INSTANCE}();

    if (i2cBufferData != NULL)
    {
        <#if MODE == "DRV_I2C_MODE_MASTER">
        i2cBufferData->i2cMode          = DRV_I2C_MODE_MASTER;
        <#else>
        i2cBufferData->i2cMode          = DRV_I2C_MODE_SLAVE;
        </#if>
        if (deviceaddress > ADDRESS_7BIT_UPPER_LIMIT )
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)((deviceaddress & 0xFF00)>>8);
            i2cBufferData->slaveaddresslowbyte  = (uint8_t)(deviceaddress & 0x00FF);
        }
        else
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cBufferData->slaveaddresslowbyte  = 0;
        }
        i2cBufferData->i2coperation     = DRV_I2C_OP_WRITE;
        i2cBufferData->transfersize     = size;
        i2cBufferData->txBuffer         = txBuffer;
        i2cBufferData->bufferstatus     = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cBufferData->transmitForced   = false;
        <#if MODE == "DRV_I2C_MODE_MASTER">
        if (i2cBufferData->i2cMode == DRV_I2C_MODE_MASTER)
        {
            /*  if either START and STOP were not detected which is true the
            first time OR if STOP was detected, then it assumed the
            transaction on the bus is complete */

            if ( ((!(PLIB_I2C_StartWasDetected(${I2C_INSTANCE}))) && (!(PLIB_I2C_StopWasDetected(${I2C_INSTANCE})))) || (PLIB_I2C_StopWasDetected(${I2C_INSTANCE})) )
            {
                if ( (PLIB_I2C_BusIsIdle(${I2C_INSTANCE})) && (!(PLIB_INT_SourceFlagGet(INT_ID_0, ${MASTER_INT_SRC}))) )
                {
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                        PLIB_I2C_MasterStart(${I2C_INSTANCE});
                }
            }
        }
        <#else>
        PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
        PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
        </#if>
    }
    return (DRV_I2C_BUFFER_HANDLE)i2cBufferData;
}


DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_TransmitForced (uint16_t deviceaddress,
                                                             void *txBuffer,
                                                             size_t size,
                                                             void * context)
{
    DRV_STATIC_I2C_OBJECT* i2cBufferData;

    i2cBufferData = _DRV_I2C_QueueSlotGet_${DRV_INSTANCE}();

    if (i2cBufferData != NULL)
    {
        <#if MODE == "DRV_I2C_MODE_MASTER">
        i2cBufferData->i2cMode          = DRV_I2C_MODE_MASTER;
        <#else>
        i2cBufferData->i2cMode          = DRV_I2C_MODE_SLAVE;
        </#if>
        if (deviceaddress > ADDRESS_7BIT_UPPER_LIMIT )
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)((deviceaddress & 0xFF00)>>8);
            i2cBufferData->slaveaddresslowbyte  = (uint8_t)(deviceaddress & 0x00FF);
        }
        else
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cBufferData->slaveaddresslowbyte  = 0;
        }
        i2cBufferData->i2coperation     = DRV_I2C_OP_WRITE;
        i2cBufferData->transfersize     = size;
        i2cBufferData->txBuffer         = txBuffer;
        i2cBufferData->bufferstatus     = DRV_I2C_BUFFER_EVENT_PENDING;
        i2cBufferData->errorEvent       = DRV_I2C_HALT_ON_ERROR;
        i2cBufferData->transmitForced   = true;
        <#if MODE == "DRV_I2C_MODE_MASTER">
        if (i2cBufferData->i2cMode == DRV_I2C_MODE_MASTER)
        {
            /*  if either START and STOP were not detected which is true the
            first time OR if STOP was detected, then it assumed the
            transaction on the bus is complete */

            if ( ((!(PLIB_I2C_StartWasDetected(${I2C_INSTANCE}))) && (!(PLIB_I2C_StopWasDetected(${I2C_INSTANCE})))) || (PLIB_I2C_StopWasDetected(${I2C_INSTANCE})) )
            {
                if ( (PLIB_I2C_BusIsIdle(${I2C_INSTANCE})) && (!(PLIB_INT_SourceFlagGet(INT_ID_0, ${MASTER_INT_SRC}))) )
                {
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                        PLIB_I2C_MasterStart(${I2C_INSTANCE});
                }
            }
        }
        <#else>
        PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
        PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
        </#if>
    }
    return (DRV_I2C_BUFFER_HANDLE)i2cBufferData;
}

DRV_I2C_BUFFER_HANDLE DRV_I2C${DRV_INSTANCE}_TransmitThenReceive ( uint16_t deviceaddress,
                                    void *txBuffer,
                                    size_t wsize,
                                    void *rxBuffer,
                                    size_t rsize,
                                    void * context)
{
    DRV_STATIC_I2C_OBJECT* i2cBufferData;

    i2cBufferData = _DRV_I2C_QueueSlotGet_${DRV_INSTANCE}();

    if (i2cBufferData != NULL)
    {
        <#if MODE == "DRV_I2C_MODE_MASTER">
        i2cBufferData->i2cMode             = DRV_I2C_MODE_MASTER;
        <#else>
        i2cBufferData->i2cMode             = DRV_I2C_MODE_SLAVE;
        </#if>
        if (deviceaddress > ADDRESS_7BIT_UPPER_LIMIT )
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)((deviceaddress & 0xFF00)>>8);
            i2cBufferData->slaveaddresslowbyte  = (uint8_t)(deviceaddress & 0x00FF);
        }
        else
        {
            i2cBufferData->slaveaddresshighbyte = (uint8_t)(deviceaddress & 0x00FF);
            i2cBufferData->slaveaddresslowbyte  = 0;
        }
        i2cBufferData->i2coperation        = DRV_I2C_OP_WRITE_READ;
        i2cBufferData->txBuffer            = txBuffer;
        i2cBufferData->transfersize        = wsize;
        i2cBufferData->rxBuffer            = rxBuffer;
        i2cBufferData->readtransfersize    = rsize;
        i2cBufferData->actualtransfersize  = 0;
        i2cBufferData->transmitForced      = false;
        i2cBufferData->bufferstatus        = DRV_I2C_BUFFER_EVENT_PENDING;


        <#if MODE == "DRV_I2C_MODE_MASTER">
        if (i2c${DRV_INSTANCE}Obj.i2cMode == DRV_I2C_MODE_MASTER)
        {
            /*  if either START and STOP were not detected which is true the
            first time OR if STOP was detected, then it assumed the
            transaction on the bus is complete */

            if ( ((!(PLIB_I2C_StartWasDetected(${I2C_INSTANCE}))) && (!(PLIB_I2C_StopWasDetected(${I2C_INSTANCE})))) || (PLIB_I2C_StopWasDetected(${I2C_INSTANCE})) )
            {
                if ( (PLIB_I2C_BusIsIdle(${I2C_INSTANCE})) && (!(PLIB_INT_SourceFlagGet(INT_ID_0, ${MASTER_INT_SRC}))) )
                {
                    i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(${I2C_INSTANCE});
                }
            }
        }
        <#else>
        PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
        PLIB_INT_SourceEnable(INT_ID_0, ${SLAVE_INT_SRC});
        </#if>
    }

    return (DRV_I2C_BUFFER_HANDLE)i2cBufferData;

} /* DRV_I2C_BufferAddWriteRead */

void DRV_I2C${DRV_INSTANCE}_Tasks(void)
{
    uint8_t tempDeviceAddress;

    switch (i2c${DRV_INSTANCE}State)
    {
        case DRV_I2C_TASK_SEND_DEVICE_ADDRESS:
        {
            if (i2c${DRV_INSTANCE}Obj.i2cMode == DRV_I2C_MODE_MASTER)
            {
                lBufferObj${DRV_INSTANCE} = _DRV_I2C_QueuePop_${DRV_INSTANCE}();

                if (PLIB_I2C_StartWasDetected(${I2C_INSTANCE}))
                {
                    if ( (lBufferObj${DRV_INSTANCE}->i2coperation == DRV_I2C_OP_WRITE) || (lBufferObj${DRV_INSTANCE}->i2coperation == DRV_I2C_OP_WRITE_READ) )
                    {
                        if (lBufferObj${DRV_INSTANCE}->slaveaddresslowbyte)
                        {
                            i2c${DRV_INSTANCE}State = DRV_I2C_SEND_DEVICE_ADDRESS_BYTE_2;
                        }
                        else
                        {
                            lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED;
                            i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_WRITE_ONLY;
                        }

                        PLIB_I2C_TransmitterByteSend( ${I2C_INSTANCE}, lBufferObj${DRV_INSTANCE}->slaveaddresshighbyte );
                    }
                    else
                    {
                        lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_SLAVE_READ_REQUESTED;
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SET_RCEN_ONLY;
                        PLIB_I2C_TransmitterByteSend(${I2C_INSTANCE}, lBufferObj${DRV_INSTANCE}->slaveaddresshighbyte | DRV_I2C_OP_READ );
                    }
                }
            }
            else if (i2c${DRV_INSTANCE}Obj.i2cMode == DRV_I2C_MODE_SLAVE)
            {
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                /* Slave is accepting data from Master */
                if ( (!PLIB_I2C_SlaveReadIsRequested(${I2C_INSTANCE})) && (PLIB_I2C_SlaveAddressIsDetected(${I2C_INSTANCE})) )
                {
                    i2c${DRV_INSTANCE}Obj.operationStarting(DRV_I2C_BUFFER_SLAVE_READ_REQUESTED,0x00);

                    lBufferObj${DRV_INSTANCE} = _DRV_I2C_QueuePop_${DRV_INSTANCE}();

                    /* do a dummy read so that I2CxRCV is cleared and not clobbered with data */
                    PLIB_I2C_ReceivedByteGet (${I2C_INSTANCE});
                    PLIB_I2C_SlaveClockRelease (${I2C_INSTANCE});
                }
                /*  Slave is sending data back to the Master    */
                else if ( (PLIB_I2C_SlaveReadIsRequested(${I2C_INSTANCE})) && (PLIB_I2C_SlaveAddressIsDetected(${I2C_INSTANCE})) )
                {

                    PLIB_I2C_ReceivedByteGet (${I2C_INSTANCE});
                    PLIB_I2C_SlaveClockHold (${I2C_INSTANCE});

                    i2c${DRV_INSTANCE}Obj.operationStarting(DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED,0x00);

                    lBufferObj${DRV_INSTANCE} = _DRV_I2C_QueuePop_${DRV_INSTANCE}();

                    lBufferObj${DRV_INSTANCE}->transfersize = 0;

                    // transmit first byte
                    PLIB_I2C_TransmitterByteSend(${I2C_INSTANCE}, *lBufferObj${DRV_INSTANCE}->txBuffer++);
                    lBufferObj${DRV_INSTANCE}->transfersize++;
                    PLIB_I2C_SlaveClockRelease (${I2C_INSTANCE});
                }
                else if ( (!PLIB_I2C_SlaveReadIsRequested(${I2C_INSTANCE})) && PLIB_I2C_SlaveDataIsDetected(${I2C_INSTANCE}))
                {
                    /*        Master sends data to the slave        */
                    PLIB_I2C_SlaveClockRelease (${I2C_INSTANCE});

                    if (lBufferObj${DRV_INSTANCE}->transfersize < 255)
                    {
                        *lBufferObj${DRV_INSTANCE}->rxBuffer++ = PLIB_I2C_ReceivedByteGet(${I2C_INSTANCE});
                        lBufferObj${DRV_INSTANCE}->transfersize++;
                        lBufferObj${DRV_INSTANCE}->bufferstatus = DRV_I2C_BUFFER_SLAVE_READ_BYTE;
                        if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                        {
                            /* Give an indication to the higher layer upon successful operation */
                            i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_SLAVE_READ_BYTE,
                                        (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                        }
                    }
                    else
                    {
                        lBufferObj${DRV_INSTANCE}->bufferstatus = DRV_I2C_BUFFER_EVENT_ERROR;
                        PLIB_I2C_ReceivedByteGet(${I2C_INSTANCE});
                        if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                        {
                            /* Give an indication to the higher layer upon when error happens */
                            i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_ERROR,
                                        (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                        }
                    }
                }
                else if (PLIB_I2C_SlaveReadIsRequested(${I2C_INSTANCE}) && PLIB_I2C_SlaveDataIsDetected(${I2C_INSTANCE}))
                {
                    PLIB_I2C_SlaveClockHold (${I2C_INSTANCE});

                    if ( PLIB_I2C_TransmitterByteWasAcknowledged(${I2C_INSTANCE}) )
                    {
                        if ( lBufferObj${DRV_INSTANCE}->transfersize < 255 )
                        {
                            PLIB_I2C_SlaveClockRelease(${I2C_INSTANCE});
                            PLIB_I2C_TransmitterByteSend (${I2C_INSTANCE}, *lBufferObj${DRV_INSTANCE}->txBuffer++ );
                            lBufferObj${DRV_INSTANCE}->transfersize++;
                            lBufferObj${DRV_INSTANCE}->bufferstatus = DRV_I2C_BUFFER_SLAVE_WRITE_BYTE;
                            if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                            {
                                /* Give an indication to the higher layer upon successful transmission */
                                i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_SLAVE_WRITE_BYTE,
                                            (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                    }
                        }
                        else
                        {
                            lBufferObj${DRV_INSTANCE}->bufferstatus = DRV_I2C_BUFFER_EVENT_ERROR;
                            PLIB_I2C_TransmitterByteSend(${I2C_INSTANCE}, 0);
                            if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                            {
                                /* Give an indication to the higher layer upon when error happens */
                                i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_ERROR,
                                        (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                            }
                        }
                    }
                    else if ( !PLIB_I2C_TransmitterByteWasAcknowledged(${I2C_INSTANCE}) )
                    {
                        PLIB_I2C_SlaveClockRelease (${I2C_INSTANCE});
                        //TODO callback implementation
                        lBufferObj${DRV_INSTANCE}->bufferstatus = DRV_I2C_BUFFER_EVENT_COMPLETE;
                    }
                }
            }
            break;
        }

        case DRV_I2C_SEND_DEVICE_ADDRESS_BYTE_2:
        {
            if ( lBufferObj${DRV_INSTANCE}->i2coperation == DRV_I2C_OP_READ )
            {
                PLIB_I2C_TransmitterByteSend(${I2C_INSTANCE},lBufferObj${DRV_INSTANCE}->slaveaddresslowbyte>>1) ;
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SET_RCEN_ONLY;
            }
            else
            {
                PLIB_I2C_TransmitterByteSend (${I2C_INSTANCE}, lBufferObj${DRV_INSTANCE}->slaveaddresslowbyte>>1) ;
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_WRITE_ONLY;
            }
            break;
        }

        case DRV_I2C_SEND_RANDOM_READ_DEVICE_ADDRESS:
        {
            tempDeviceAddress = (lBufferObj${DRV_INSTANCE}->slaveaddresshighbyte);
            tempDeviceAddress |= 0x01;
            PLIB_I2C_TransmitterByteSend( ${I2C_INSTANCE},tempDeviceAddress );
            i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SET_RCEN_ONLY;
            break;
        }

        case DRV_I2C_TASK_PROCESS_READ_ONLY:
        {
            /* Read I2CxRCV register for received data */
            if (lBufferObj${DRV_INSTANCE}->transfersize)
            {
                if ( PLIB_I2C_ReceivedByteIsAvailable (${I2C_INSTANCE}) )
                {
                    /* If all but one reception is complete */
                    if ( lBufferObj${DRV_INSTANCE}->transfersize > 1 )
                    {
                        *lBufferObj${DRV_INSTANCE}->rxBuffer++ = PLIB_I2C_ReceivedByteGet (${I2C_INSTANCE});
                        lBufferObj${DRV_INSTANCE}->transfersize--;
                        lBufferObj${DRV_INSTANCE}->actualtransfersize++;
                        i2c${DRV_INSTANCE}State  = DRV_I2C_TASK_SET_RCEN_ONLY;
                        if (PLIB_I2C_MasterReceiverReadyToAcknowledge(${I2C_INSTANCE}))
                        {
                            PLIB_I2C_ReceivedByteAcknowledge (${I2C_INSTANCE}, true);
                        }
                    }
                    else
                    {
                        lBufferObj${DRV_INSTANCE}->transfersize--;
                        lBufferObj${DRV_INSTANCE}->actualtransfersize++;

                        *lBufferObj${DRV_INSTANCE}->rxBuffer++ = PLIB_I2C_ReceivedByteGet (${I2C_INSTANCE});
                        if (PLIB_I2C_MasterReceiverReadyToAcknowledge(${I2C_INSTANCE}))
                        {
                            PLIB_I2C_ReceivedByteAcknowledge ( ${I2C_INSTANCE}, false  );
                        }

                        i2c${DRV_INSTANCE}State = DRV_I2C_BUS_SILENT;
                    }
                }
                else
                {
                    // Do not block in any case
                    break;
                }
            }
            break;
        }

        case    DRV_I2C_TASK_PROCESS_WRITE_ONLY:
        {
            if (lBufferObj${DRV_INSTANCE}->transfersize)
            {
                if ( PLIB_I2C_TransmitterByteHasCompleted (${I2C_INSTANCE}) )
                {
                    if ( PLIB_I2C_TransmitterByteWasAcknowledged (${I2C_INSTANCE}) ||
                        (lBufferObj${DRV_INSTANCE}->transmitForced)  )
                    {
                        if (lBufferObj${DRV_INSTANCE}->transfersize > 1)
                            i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_WRITE_ONLY;
                        else
                            i2c${DRV_INSTANCE}State = DRV_I2C_BUS_SILENT;
                        PLIB_I2C_TransmitterByteSend ( ${I2C_INSTANCE}, *lBufferObj${DRV_INSTANCE}->txBuffer++ );
                        lBufferObj${DRV_INSTANCE}->transfersize--;
                        lBufferObj${DRV_INSTANCE}->actualtransfersize++;
                    }
                    else
                    {
                        <#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
                        _DRV_I2C${DRV_INSTANCE}_MasterStop();
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                        lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_ERROR;
                        if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                        {
                            /* Give an indication to the higher layer upon successful transmission */
                            i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_ERROR,
                                                                (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                        }
                        <#else>
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_STOP;
                        PLIB_I2C_MasterStop (${I2C_INSTANCE});
                        </#if>
                    }
                }
                else
                {
                    /* do not block but exit*/
                    break;
                }
            }
            else
            {
                <#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
                _DRV_I2C${DRV_INSTANCE}_MasterStop();
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_ERROR;
                if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                {
                    /* Give an indication to the higher layer upon successful transmission */
                    i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_ERROR,
                                                        (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                }
                <#else>
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_STOP;
                PLIB_I2C_MasterStop (${I2C_INSTANCE});
                </#if>
            }
            break;
        }

        case    DRV_I2C_BUS_SILENT:
        {
            /* The bus is silent/idle when when the last byte is either ACKed  OR
            in the event of slave unexpectedly aborting operation, check
            if transmission is complete and NACK is received
            ACK received OR (transmit complete & NACKed)
            */

            if ( (PLIB_I2C_TransmitterByteWasAcknowledged (${I2C_INSTANCE})) ||
                    ( PLIB_I2C_TransmitterByteHasCompleted (${I2C_INSTANCE})  &&
                        (!PLIB_I2C_TransmitterByteWasAcknowledged (${I2C_INSTANCE})) ) )
            {
                if (lBufferObj${DRV_INSTANCE}->i2coperation == DRV_I2C_OP_WRITE_READ)
                {
                    lBufferObj${DRV_INSTANCE}->transfersize   = lBufferObj${DRV_INSTANCE}->readtransfersize;
                    i2c${DRV_INSTANCE}State                   = DRV_I2C_SEND_RANDOM_READ_DEVICE_ADDRESS;
                    lBufferObj${DRV_INSTANCE}->i2coperation   = DRV_I2C_OP_READ;
                    PLIB_I2C_MasterStartRepeat (${I2C_INSTANCE});
                }
                else
                {
                    <#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
                    _DRV_I2C${DRV_INSTANCE}_MasterStop();
                    i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_COMPLETE;
                    if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                    {
                        /* Give an indication to the higher layer upon successful transmission */
                        i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_COMPLETE,
                                                            (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                    }
                    <#else>
                    i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_STOP;
                    PLIB_I2C_MasterStop (${I2C_INSTANCE});
                    </#if>
                }
            }
            break;
        }

        case DRV_I2C_TASK_SET_RCEN_ONLY:
		{
            if ( PLIB_I2C_TransmitterByteHasCompleted(${I2C_INSTANCE}) )
            {
                if ( PLIB_I2C_TransmitterByteWasAcknowledged(${I2C_INSTANCE}) )
                {
                    /* if I2CxSTAT.ACKEN bit is 0, the Acknowledge sequence has been completed */
                    if (PLIB_I2C_ReceiverByteAcknowledgeHasCompleted(${I2C_INSTANCE}))
                    {
                        PLIB_I2C_MasterReceiverClock1Byte (${I2C_INSTANCE});
                        i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_READ_ONLY;
                    }
                }
                else
                {
                    <#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
                    _DRV_I2C${DRV_INSTANCE}_MasterStop();
                    i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_COMPLETE;
                    if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                    {
                        /* Give an indication to the higher layer upon successful transmission */
                        i2c${DRV_INSTANCE}ClientObj.callback(DRV_I2C_BUFFER_EVENT_ERROR,
                                                             (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                    }
                    <#else>
                    i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_STOP;
                    PLIB_I2C_MasterStop (${I2C_INSTANCE});
                    </#if>
                }
            }
            break;
        }

        case    DRV_I2C_TASK_PROCESS_STOP:
        {
            if (PLIB_I2C_StopWasDetected(${I2C_INSTANCE}))
            {
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                if (lBufferObj${DRV_INSTANCE}->transfersize)
                {
                    lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_ERROR;
                }
                else
                {
                    lBufferObj${DRV_INSTANCE}->bufferstatus    = DRV_I2C_BUFFER_EVENT_COMPLETE;
                }
                /* Have a check here because DRV_I2C_BufferEvent_Handler function call is optional */
                if ( i2c${DRV_INSTANCE}ClientObj.callback != NULL )
                {
                    /* Give an indication to the higher layer upon successful transmission */
                    i2c${DRV_INSTANCE}ClientObj.callback(lBufferObj${DRV_INSTANCE}->bufferstatus,
                                                        (DRV_I2C_BUFFER_HANDLE) lBufferObj${DRV_INSTANCE}, 0x00);
                }
            }
            else
            {
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_PROCESS_STOP;
            }
            break;
        }

        default:
            break;
    }

    /* Clear pending interrupt */
<#if MODE == "DRV_I2C_MODE_MASTER">
    PLIB_INT_SourceFlagClear(INT_ID_0, ${MASTER_INT_SRC});
<#else>
    PLIB_INT_SourceFlagClear(INT_ID_0, ${SLAVE_INT_SRC});
</#if>
    PLIB_INT_SourceFlagClear(INT_ID_0, ${ERR_INT_SRC});

    if (i2c${DRV_INSTANCE}Obj.i2cMode == DRV_I2C_MODE_MASTER)
    {
        if ( (lBufferObj${DRV_INSTANCE}->bufferstatus == DRV_I2C_BUFFER_EVENT_COMPLETE) || (lBufferObj${DRV_INSTANCE}->bufferstatus == DRV_I2C_BUFFER_EVENT_ERROR) )
        {
            <#if MODE == "DRV_I2C_MODE_MASTER">
            _DRV_I2C_Advance_Queue_${DRV_INSTANCE}();
            </#if>
            if (IsQueueEmpty_${DRV_INSTANCE}() == false)
            {
                i2c${DRV_INSTANCE}State = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                PLIB_I2C_MasterStart(${I2C_INSTANCE});
                Nop();
            }
        }
    }
}


// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Byte Transfer Functions (Master/Slave)
// *****************************************************************************

uint32_t DRV_I2C${DRV_INSTANCE}_BytesTransferred ( DRV_I2C_BUFFER_HANDLE bufferHandle )
{
    /* returns the number of bytes transferred in an I2C transaction */
    return ((DRV_STATIC_I2C_OBJECT*)bufferHandle)->actualtransfersize;
}

// *****************************************************************************
// Section: Function that will initiate a callback to the main
//          application on status of buffer operation
// *****************************************************************************

void DRV_I2C${DRV_INSTANCE}_BufferEventHandlerSet (const DRV_I2C_BUFFER_EVENT_HANDLER eventHandler,
                                                   const uintptr_t context )
{
    i2c${DRV_INSTANCE}ClientObj.callback = eventHandler;

    i2c${DRV_INSTANCE}ClientObj.context = context;
}   /* DRV_I2C_BufferEventHandlerSet */

<#if MODE == "DRV_I2C_MODE_MASTER">

// *****************************************************************************
// Section: Instance ${DRV_INSTANCE} Function to check status of a buffer
//          transfer operation
// *****************************************************************************

DRV_I2C_BUFFER_EVENT DRV_I2C${DRV_INSTANCE}_TransferStatusGet ( DRV_I2C_BUFFER_HANDLE bufferHandle )
{
    /* return status of Buffer Transfer */
    return ((DRV_STATIC_I2C_OBJECT*)bufferHandle)->bufferstatus;
} /* DRV_I2C_TransferStatus */

<#if MODE == "DRV_I2C_MODE_MASTER" && CONFIG_PIC32MZ == true && CONFIG_DEVICE?contains("EC") >
bool _DRV_I2C${DRV_INSTANCE}_MasterStop(void)
{
    uint32_t starttime;

    /* Wait 1 BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()-starttime < BRG_1_TIME_${DRV_INSTANCE});

    /* Disable I2C */
    PLIB_I2C_Disable(${I2C_INSTANCE});

    /* Wait 2 BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_${DRV_INSTANCE});

    /* Set SDA as I/P */
<#if I2C_INSTANCE == "I2C_ID_1">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);
<#else>
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_2">
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
</#if>
<#if I2C_INSTANCE == "I2C_ID_3">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
<#else>
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_4">
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7);
</#if>
<#if I2C_INSTANCE == "I2C_ID_5">
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_4);
</#if>
    /* Wait 3 BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_${DRV_INSTANCE});

    PLIB_I2C_Enable(${I2C_INSTANCE});
<#if I2C_INSTANCE == "I2C_ID_1">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_9);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
<#else>
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_15);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_14);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_2">
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2);
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_3);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_A, PORTS_BIT_POS_2);
</#if>
<#if I2C_INSTANCE == "I2C_ID_3">
<#if CONFIG_DEVICE == "PIC32MZ0512ECE064" || CONFIG_DEVICE == "PIC32MZ0512ECF064" || CONFIG_DEVICE == "PIC32MZ0512ECK064"
|| CONFIG_DEVICE == "PIC32MZ1024ECG064" || CONFIG_DEVICE == "PIC32MZ1024ECH064" || CONFIG_DEVICE == "PIC32MZ1024ECM064"
|| CONFIG_DEVICE == "PIC32MZ2048ECG064" || CONFIG_DEVICE == "PIC32MZ2048ECH064" || CONFIG_DEVICE == "PIC32MZ2048ECM064"
|| CONFIG_DEVICE == "PIC32MZ1024ECE064" || CONFIG_DEVICE == "PIC32MZ1024ECF064" || CONFIG_DEVICE == "PIC32MZ1024ECK064" >
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_3);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_2);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_D, PORTS_BIT_POS_10);
<#else>
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_2);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_8);
</#if>
</#if>
<#if I2C_INSTANCE == "I2C_ID_4">
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_7);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_8);
</#if>
<#if I2C_INSTANCE == "I2C_ID_5">
              /* set SDA to 0 */
    PLIB_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_4);
              /* set SCL to 1 */
    PLIB_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_5);
              /* set SDA to O/P */
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_4);
              /* set SCL to I/P */
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, PORT_CHANNEL_F, PORTS_BIT_POS_5);
</#if>

    return true;
}
</#if>
</#if>

</#macro>
<#if CONFIG_DRV_I2C_INST_IDX0 == true && CONFIG_PIC32MX == true>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="0"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX0
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX0
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX0
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX0
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX0
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX0
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX0
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX0
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX0
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX0
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX0
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX0
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX0
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX0
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX0
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX0
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX0
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX0
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX0
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX0
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX0
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX0
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX0
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX0
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX0
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX0
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX0
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX0
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX0
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX0/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX0 == true && (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true)>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="0"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX0
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX0
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX0
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX0
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX0
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX0
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX0
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX0
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX0
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX0
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX0
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX0
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX0
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX0
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX0
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX0
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX0
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX0
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX0
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX0
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX0
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX0
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX0
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX0
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX0
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX0
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX0
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX0
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX0
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX0
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX0
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX0/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX1 == true && CONFIG_PIC32MX == true>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="1"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX1
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX1
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX1
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX1
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX1
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX1
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX1
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX1
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX1
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX1
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX1
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX1
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX1
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX1
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX1
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX1
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX1
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX1
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX1
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX1
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX1
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX1
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX1
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX1
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX1
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX1
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX1
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX1
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX1
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX1/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX1 == true && (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true)>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="1"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX1
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX1
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX1
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX1
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX1
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX1
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX1
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX1
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX1
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX1
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX1
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX1
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX1
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX1
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX1
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX1
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX1
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX1
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX1
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX1
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX1
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX1
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX1
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX1
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX1
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX1
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX1
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX1
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX1
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX1
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX1
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX1/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX2 == true && CONFIG_PIC32MX == true>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="2"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX2
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX2
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX2
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX2
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX2
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX2
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX2
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX2
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX2
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX2
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX2
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX2
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX2
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX2
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX2
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX2
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX2
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX2
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX2
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX2
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX2
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX2
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX2
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX2
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX2
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX2
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX2
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX2
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX2
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX2/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX2 == true && (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true)>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="2"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX2
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX2
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX2
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX2
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX2
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX2
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX2
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX2
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX2
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX2
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX2
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX2
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX2
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX2
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX2
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX2
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX2
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX2
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX2
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX2
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX2
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX2
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX2
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX2
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX2
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX2
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX2
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX2
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX2
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX2
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX2
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX2/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX3 == true && CONFIG_PIC32MX == true>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="3"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX3
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX3
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX3
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX3
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX3
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX3
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX3
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX3
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX3
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX3
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX3
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX3
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX3
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX3
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX3
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX3
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX3
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX3
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX3
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX3
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX3
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX3
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX3
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX3
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX3
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX3
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX3
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX3
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX3
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX3/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX3 == true && (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true || CONFIG_PIC32MK == true)>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="3"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX3
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX3
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX3
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX3
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX3
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX3
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX3
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX3
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX3
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX3
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX3
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX3
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX3
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX3
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX3
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX3
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX3
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX3
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX3
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX3
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX3
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX3
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX3
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX3
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX3
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX3
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX3
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX3
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX3
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX3
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX3
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX3/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX4 == true && CONFIG_PIC32MX == true>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="4"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX4
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX4
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX4
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX4
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX4
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX4
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX4
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX4
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX4
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX4
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX4
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX4
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX4
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MX_INT_SRC_IDX4
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX4
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX4
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX4
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX4
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX4
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX4
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX4
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX4
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX4
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX4
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX4
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX4
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX4
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX4
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX4
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX4/>
</#if>
<#if CONFIG_DRV_I2C_INST_IDX4 == true && (CONFIG_PIC32MZ == true || CONFIG_PIC32WK == true)>
<@DRV_I2C_STATIC_FUNCTIONS DRV_INSTANCE="4"
I2C_INSTANCE=CONFIG_DRV_I2C_PERIPHERAL_ID_IDX4
MODE=CONFIG_DRV_I2C_STATIC_OPERATION_MODE_IDX4
SLEW_RATE=CONFIG_DRV_I2C_STATIC_SLEW_RATE_CONTROL_IDX4
BRG_CLOCK=CONFIG_DRV_I2C_STATIC_BRG_CLOCK_IDX4
BAUD=CONFIG_DRV_I2C_STATIC_BAUD_RATE_IDX4
SMBus_SPEC=CONFIG_DRV_I2C_STATIC_SMBus_SPECIFICATION_IDX4
ADDRESS_WIDTH=CONFIG_DRV_I2C_STATIC_ADDRESS_WIDTH_IDX4
STRICT_ADDRESS_RULE=CONFIG_DRV_I2C_STATIC_STRICT_ADDRESS_RULE_IDX4
GENERAL_ADDRESS=CONFIG_DRV_I2C_STATIC_GENERAL_CALL_ADDRESS_IDX4
CLOCK_STRETCH=CONFIG_DRV_I2C_STATIC_CLOCK_STRETCH_IDX4
ADDRESS_MASK=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_MASK_IDX4
SLAVE_ADDRESS=CONFIG_DRV_I2C_STATIC_SLAVE_ADDRESS_VALUE_IDX4
SLAVE_CALLBACK=CONFIG_DRV_I2C_STATIC_SLAVE_CALLBACK_FUNCTION_IDX4
MASTER_INT_SRC=CONFIG_DRV_I2C_MASTER_INT_SRC_IDX4
SLAVE_INT_SRC=CONFIG_DRV_I2C_SLAVE_INT_SRC_IDX4
ERR_INT_SRC = CONFIG_DRV_I2C_ERR_MZ_INT_SRC_IDX4
INT_VECTOR=CONFIG_DRV_I2C_INT_VECTOR_IDX4
ISR_VECTOR=CONFIG_DRV_I2C_ISR_VECTOR_IDX4
INT_PRIORITY=CONFIG_DRV_I2C_INT_PRIORITY_IDX4
INT_SUB_PRIORITY=CONFIG_DRV_I2C_INT_SUB_PRIORITY_IDX4
MASTER_INT_VECTOR=CONFIG_DRV_I2C_MASTER_INT_VECTOR_IDX4
MASTER_ISR_VECTOR=CONFIG_DRV_I2C_MASTER_ISR_VECTOR_IDX4
MASTER_INT_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_PRIORITY_IDX4
MASTER_INT_SUB_PRIORITY=CONFIG_DRV_I2C_MASTER_INT_SUB_PRIORITY_IDX4
SLAVE_INT_VECTOR=CONFIG_DRV_I2C_SLAVE_INT_VECTOR_IDX4
SLAVE_ISR_VECTOR=CONFIG_DRV_I2C_SLAVE_ISR_VECTOR_IDX4
SLAVE_INT_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_PRIORITY_IDX4
SLAVE_INT_SUB_PRIORITY=CONFIG_DRV_I2C_SLAVE_INT_SUB_PRIORITY_IDX4
ERR_INT_VECTOR=CONFIG_DRV_I2C_ERR_INT_VECTOR_IDX4
ERR_INT_PRIORITY=CONFIG_DRV_I2C_ERR_INT_PRIORITY_IDX4
ERR_INT_SUB_PRIORITY=CONFIG_DRV_I2C_ERR_INT_SUB_PRIORITY_IDX4
PWR_MODE=CONFIG_DRV_I2C_POWER_STATE_IDX4/>
</#if>

/*******************************************************************************
 End of File
*/