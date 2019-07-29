/*******************************************************************************
  I2C Device Driver Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_i2c_bb_a.c

  Summary:
    I2C Device Driver Dynamic Multiple Client Implementation

  Description:
    The I2C device driver provides a simple interface to manage the I2C
    modules on Microchip microcontrollers.  This file Implements the core
    interface routines for the I2C driver.

    While building the driver from source, ALWAYS use this file in the build.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include "driver/i2c/drv_i2c_bb.h"
#include "driver/i2c/src/drv_i2c_local_bb.h"
#include "osal/osal.h"

        static uint32_t ReadCoreTimer(void);

        static uint32_t ReadCoreTimer()
        {
            volatile uint32_t timer;

            // get the count reg
            asm volatile("mfc0   %0, $9" : "=r"(timer));

            return(timer);
        }

         uint32_t    starttime;

        #define BAUD_RATE_0         50000
        #define BRG_1_TIME_0        SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_2)/(2*BAUD_RATE_0)
// *****************************************************************************
// *****************************************************************************
// Section: File Scope Variables
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Driver Hardware instance objects.

  Summary:
    Defines the hardware instances objects that are available on the part

  Description:
    This data type defines the hardware instance objects that are available on
    the part, so as to capture the hardware state of the instance.

  Remarks:
    Not all modes are available on all micro-controllers.
*/

static DRV_I2C_OBJ             gDrvI2CObj[DRV_I2C_INSTANCES_NUMBER] ;

// *****************************************************************************
/* Driver Client instance objects.

  Summary:
    Defines the Client instances objects that are available on the part

  Description:
    This data type defines the Client instance objects that are available on
    the part, so as to capture the Client state of the instance.

  Remarks:
    None
*/

static DRV_I2C_CLIENT_OBJ      gDrvI2CClientObj [ DRV_I2C_CLIENTS_NUMBER ] ;


// *****************************************************************************
/* Driver data objects.

  Summary:
    Defines the data object.

  Description:
    This data type defines the data objects. This is used to queue the user
    requests for different operations.

  Remarks:
    None
*/

static DRV_I2C_BUFFER_OBJECT	gDrvI2CBufferObj [ DRV_I2C_INSTANCES_NUMBER ][ DRV_I2C_NUM_OF_BUFFER_OBJECTS ];


/* This object maintains data that is required by all I2C
   driver instances. */
DRV_I2C_COMMON_DATA_OBJ gDrvI2CCommonDataObj;

// *****************************************************************************
/* Driver Shared Object protection
 */
static uint32_t i2cNumInited = 0;

//static OSAL_MUTEX_DECLARE(i2cClientMutex);

// *****************************************************************************
/* Macro: _DRV_I2C_CLIENT_OBJ(obj,mem)

  Summary:
    Returns the appropriate client member

  Description:
    Either return the static object or returns the indexed dynamic object.
    This macro has variations for dynamic or static driver.
*/

#define _DRV_I2C_CLIENT_OBJ(obj,mem)    obj->mem


// *****************************************************************************
/* Macro: _DRV_I2C_CLIENT_OBJ_GET(obj)

  Summary:
    Returns the appropriate client instance

  Description:
    Either return the static object or returns the indexed dynamic object.
    This macro has variations for dynamic or static driver.
*/

#define _DRV_I2C_CLIENT_OBJ_GET(obj)    &gDrvI2CClientObj[obj]

// *****************************************************************************
/* Macro: _DRV_I2C_DATA_OBJ(obj,mem)

  Summary:
    Returns the appropriate client member

  Description:
    Either return the static object or returns the indexed dynamic object.
    This macro has variations for dynamic or static driver.
*/

#define _DRV_I2C_DATA_OBJ(obj,mem)    ((DRV_I2C_BUFFER_OBJECT*)(obj))->mem

// *****************************************************************************
// *****************************************************************************
// Section: File Scope Functions
// *****************************************************************************
// *****************************************************************************

static void _DRV_I2C_LockMutex(DRV_I2C_OBJ *dObj);
static void _DRV_I2C_UnlockMutex(DRV_I2C_OBJ *dObj);
static void _DRV_I2C_SetupHardware ( const I2C_MODULE_ID plibId,
                                     DRV_I2C_OBJ *dObj,
                                     DRV_I2C_INIT * i2cInit );

void _DRV_I2C_Advance_Queue( DRV_I2C_OBJ *dObj );

// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_OBJ* _DRV_I2C_QueueSlotGet ( DRV_I2C_OBJ *dObj )

  Summary:
    Adds an element to the queue.

  Description:
    This API adds an element to the queue.

  Parameters:
    i2cDataObj   - Pointer to the structure which holds the data which is to be
    				added to the queue.

  Returns:
    DRV_I2C_BUFFER_HANDLE - Handle, a pointer to the allocated element in the
    						queue.
*/
DRV_I2C_BUFFER_OBJECT* _DRV_I2C_QueueSlotGet ( DRV_I2C_OBJ *dObj )
{
//    int index;
    uint8_t numOfFreeQueueSlots;
    DRV_I2C_BUFFER_OBJECT *lQueueObj;
    bool interruptEnableState = false;

    SYS_MODULE_INDEX drvIndex = dObj->drvIndex;

        if (dObj->interruptNestingCount == 0)
    {
        Nop();
        if (OSAL_MUTEX_Lock(&(dObj->mutexDriverInstance), OSAL_WAIT_FOREVER)  == OSAL_RESULT_TRUE)
        {
            /* We will disable interrupts so that the queue
               status does not get updated asynchronously.
               This code will always execute. */

            if (dObj->i2cMode == DRV_I2C_MODE_MASTER)
            {
                interruptEnableState = _DRV_I2C_InterruptSourceDisable(dObj->mstrInterruptSource);
            }
            else
            {
                interruptEnableState = _DRV_I2C_InterruptSourceDisable(dObj->slaveInterruptSource);
            }
        }
        else
        {
            /* The mutex acquisition timed out. Return with an
               invalid handle. This code will not execute
               if there is no RTOS. */
            return (DRV_I2C_BUFFER_OBJECT*)NULL;;

        }
    }

//    _DRV_I2C_LockMutex(dObj);



    /* if position of item populated in the queue (Queue-In pointer) is greater
     * than the position where the item is to be taken out of the queue,
     * (Queue-Out pointer) then the number of free slots is the remainder of the
     * slots excluding the slot index where Queue Out pointer resides to the
     * slot index where Queue-In pointer resides.
     * Ex: # of Queue Slots = 6, Queue-In = 5 and Queue-Out = 2
     * Number of free queue available  = 6 -(5-2) - 1 = 2 (free slots - 6 & 1)
     *
     * if Queue-Out pointer is greater than Queue-In pointer but the Queue Out
     * pointer is adjacent to Queue-In pointer (Queue-Out - Queue-In == 1)
     * then buffer is full. Return 0 slots available if it's the case.
     * If Queue-Out pointer is greater than Queue-In pointer then slots starting
     * from Queue-In pointer to Queue-Out pointer is number of available queue
     * slots
     * Ex: # of Queue Slots = 6, Queue-Out = 4 and Queue-In = 2
     * Number of free queue slots available  = 4-2-1 (free slots - 3)
     */

    if (dObj->queueIn >= dObj->queueOut)
    {
        numOfFreeQueueSlots =  (DRV_I2C_NUM_OF_BUFFER_OBJECTS - (dObj->queueIn - dObj->queueOut) -1);
    }
    else
    {
        numOfFreeQueueSlots = ((dObj->queueOut - dObj->queueIn) -1);
    }

    if (numOfFreeQueueSlots > 0)
    {
        lQueueObj = &gDrvI2CBufferObj [ drvIndex ][ dObj->queueIn ];

        dObj->queueIn++;

        if ( dObj->queueIn >= DRV_I2C_NUM_OF_BUFFER_OBJECTS )
        {
            dObj->queueIn = 0;
        }

        /* We are done. Restore the interrupt enable status
           and return. */

        if(interruptEnableState)
        {
            if (dObj->i2cMode == DRV_I2C_MODE_MASTER)
            {
                _DRV_I2C_InterruptSourceEnable(dObj->mstrInterruptSource);
            }
            else
            {
                 _DRV_I2C_InterruptSourceEnable(dObj->slaveInterruptSource);
            }
        }

        if(dObj->interruptNestingCount == 0)
        {
            /* Release mutex */
            OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
        }

//        _DRV_I2C_UnlockMutex(dObj);
        return lQueueObj;
    }

    if(interruptEnableState)
    {
        if (dObj->i2cMode == DRV_I2C_MODE_MASTER)
        {
            _DRV_I2C_InterruptSourceEnable(dObj->mstrInterruptSource);
        }
        else
        {
             _DRV_I2C_InterruptSourceEnable(dObj->slaveInterruptSource);
        }
    }

    if(dObj->interruptNestingCount == 0)
    {
        /* Release mutex */
        OSAL_MUTEX_Unlock(&(dObj->mutexDriverInstance));
    }

   return (DRV_I2C_BUFFER_OBJECT*)NULL;
}


// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_OBJ* _DRV_I2C_QueuePop ( DRV_I2C_OBJ *dObj )

  Summary:
    Removes a element from the queue

  Description:
    This API removes an element from the queue

  Parameters:
    i2cDataObj   - Pointer to the data structure to be removed from the queue

  Returns:
    DRV_I2C_BUFFER_HANDLE - Handle, a pointer to the next allocated element in the
    						queue.
*/

DRV_I2C_BUFFER_OBJECT * _DRV_I2C_QueuePop( DRV_I2C_OBJ *dObj)
{
    SYS_MODULE_INDEX drvIndex = dObj->drvIndex;

    // Make sure that the pointer is valid inside out allocated space.
    if (dObj < &gDrvI2CObj[0] || dObj > &gDrvI2CObj[DRV_I2C_INSTANCES_NUMBER-1])
    {
            return NULL;
    }
    // Make sure that this driver instance is actually in use.
    if (dObj->inUse != true)
    {
            return NULL;
    }
//    DRV_I2C_BUFFER_OBJECT * ret = dObj->queueHead;
//    if (ret == NULL)
//    {
//            return NULL;
//    }
//    dObj->queueHead = dObj->queueHead->next;
//    ret->next = NULL;

    DRV_I2C_BUFFER_OBJECT * ret = &gDrvI2CBufferObj [ drvIndex ][ dObj->queueOut ];

    if (dObj->i2cMode == DRV_I2C_MODE_SLAVE)
    {
        if ( dObj->queueOut >= (DRV_I2C_NUM_OF_BUFFER_OBJECTS-1) )
        {
            dObj->queueOut = 0;
        }
        else
        {
            dObj->queueOut++;
        }
    }

    return ret;
}

// *****************************************************************************
/* Function:
    void _DRV_I2C_Advance_Queue ( DRV_I2C_OBJ *dObj )

  Summary:
    Moves the Queue-Out pointer

  Description:
    Advances the Queue-Out pointer to the next element that needs to be taken
    out from the queue

  Parameters:
    dObj   - driver object

  Returns:
    None
*/

void _DRV_I2C_Advance_Queue( DRV_I2C_OBJ *dObj )
{

    if ((dObj->i2cMode == DRV_I2C_MODE_MASTER) &&
        (dObj->queueOut != dObj->queueIn))
    {
        if ( dObj->queueOut >= (DRV_I2C_NUM_OF_BUFFER_OBJECTS-1) )
        {
            dObj->queueOut = 0;
        }
        else
        {
            dObj->queueOut++;
        }
    }
}

// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_OBJ* _DRV_I2C_IsQueueEmpty (DRV_I2C_OBJ *dObj)

  Summary:
    Checks if the queue is empty

  Description:
    This API checks if the end of queue has been reached or if the tail pointer
    is at the same location as the head pointer. If the tail pointer and head
    pointer coincide, it implies that the queue is empty

  Parameters:
    i2cDataObj   - Pointer to the data structure to be removed from the queue
    i2cbufferObj - Pointer to buffer where i2c objects are stored
  Returns:
    true, if queue is empty
    false, if is queue is not empty
*/

bool _DRV_I2C_IsQueueEmpty(DRV_I2C_OBJ *dObj)
{

    // Make sure that the pointer is valid inside out allocated space.
    if (dObj < &gDrvI2CObj[0] || dObj > &gDrvI2CObj[DRV_I2C_INSTANCES_NUMBER-1])
    {
        return false;
    }
    // Make sure that this driver instance is actually in use.
    if (dObj->inUse != true)
    {
        return false;
    }

    if (dObj->queueOut == dObj->queueIn)
        return true;

    return false;


}

// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_OBJ* _DRV_I2C_QueuePush (DRV_I2C_OBJ *dObj, DRV_I2C_BUFFER_OBJECT *buf)

  Summary:
 Adds a DRV_I2C_OBJ into a I2C Buffer Object

  Description:
    This API adds an DRV_I2C_OBJ into a buffer containing I2C objects

  Parameters:
    i2cDataObj   - Pointer to the data structure to be removed from the queue
    i2cbufferObj - Pointer to buffer where i2c objects are stored
  Returns:
    None
*/

void _DRV_I2C_QueuePush(DRV_I2C_OBJ *dObj, DRV_I2C_BUFFER_OBJECT *buf)
{
    SYS_MODULE_INDEX drvIndex = NULL;

    // Make sure that the pointer is valid inside out allocated space.
    if (dObj < &gDrvI2CObj[0] || dObj > &gDrvI2CObj[DRV_I2C_INSTANCES_NUMBER-1])
    {
	return;
    }
    // Make sure that this driver instance is actually in use.
    if (dObj->inUse != true)
    {
    	return;
    }

    drvIndex = dObj->drvIndex;
    if (buf < &gDrvI2CBufferObj[drvIndex][0] || buf > &gDrvI2CBufferObj[drvIndex][DRV_I2C_NUM_OF_BUFFER_OBJECTS])
    {
    	return;
    }

//    _DRV_I2C_LockMutex(dObj);
//    if (dObj->queueHead == NULL)
//    {
//    	Nop();
//        dObj->queueHead = buf;
//        _DRV_I2C_UnlockMutex(dObj);
//		return;
//    }
//
//    DRV_I2C_BUFFER_OBJECT * ptr = dObj->queueHead;
//    while (ptr->next != NULL)
//    {
//        ptr = ptr->next;
//        Nop();
//    }
//    ptr->next = buf;
//    _DRV_I2C_UnlockMutex(dObj);

    dObj->queueHead = &gDrvI2CBufferObj[drvIndex][dObj->queueIn];

    return;
}


//******************************************************************************
/* Function:
    static void _DRV_I2C_SetupHardware ( const I2C_MODULE_ID   plibId,
                                        DRV_I2C_OBJ_HANDLE     dObj,
                                        DRV_I2C_INIT         * i2cInit )

  Summary:
    Sets up the hardware from the initialization structure

  Description:
    This routine sets up the hardware from the initialization structure.

  Remarks:
    None.
*/

static void _DRV_I2C_SetupHardware ( const I2C_MODULE_ID plibId,
                                     DRV_I2C_OBJ *dObj,
                                     DRV_I2C_INIT * i2cInit )
{
    if ( DRV_I2C_PERIPHERAL == _DRV_I2C_OPERATION_MODE_GET(i2cInit->i2cPerph))
	{
		/* Initialize the Interrupt Sources */
		dObj->mstrInterruptSource   = i2cInit->mstrInterruptSource;
		dObj->slaveInterruptSource  = i2cInit->slaveInterruptSource;
		dObj->errInterruptSource    = i2cInit->errInterruptSource;

		/* Power state initialization */
		if( _DRV_I2C_POWER_STATE_GET( i2cInit->moduleInit.value ) == SYS_MODULE_POWER_IDLE_STOP )
		{
			PLIB_I2C_StopInIdleEnable( plibId  );
		}
		else if( ( _DRV_I2C_POWER_STATE_GET( i2cInit->moduleInit.value ) == SYS_MODULE_POWER_IDLE_RUN ) ||
					( _DRV_I2C_POWER_STATE_GET( i2cInit->moduleInit.value ) == SYS_MODULE_POWER_RUN_FULL ) )
		{
			PLIB_I2C_StopInIdleDisable( plibId  );
		}
		else
		{
			if( _DRV_I2C_POWER_STATE_GET( i2cInit->moduleInit.sys.powerState ) == SYS_MODULE_POWER_IDLE_STOP )
			{
				PLIB_I2C_StopInIdleEnable( plibId  );
			}
		}

		 /* Set I2C operational mode -- Master or Slave */
		 dObj->i2cMode = _DRV_I2C_OPERATION_MODE_GET(i2cInit->i2cMode);

		 /* Set Baud Rate */
		 if ( DRV_I2C_MODE_MASTER == _DRV_I2C_OPERATION_MODE_GET(i2cInit->i2cMode))
		 {
			PLIB_I2C_BaudRateSet ( plibId, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1),
									_DRV_I2C_BAUD_RATE_VALUE_GET(i2cInit->baudRate) );
			dObj->modulemainstate = DRV_I2C_MODULE_IDLE;
		 }

		 /* Set SLEW rate based on baud-rate; if baud-rate is either <= 100k
			OR baud-rate = 1M; I2C2xCON.DISSLW = 1  */
		 if (_DRV_I2C_BAUD_RATE_VALUE_GET(i2cInit->baudRate) <= 100000 ||
			   _DRV_I2C_BAUD_RATE_VALUE_GET(i2cInit->baudRate) == 1000000 )
		 {
			 PLIB_I2C_HighFrequencyEnable(plibId);
		 }
		 else
		 {
			 PLIB_I2C_HighFrequencyDisable(plibId);
		 }

		/* SMBus Input Level */
		dObj->buslevel = _DRV_I2C_SMBus_LEVEL_GET(i2cInit->buslevel);

		/* Slave Address width select */
		if ( DRV_I2C_MODE_SLAVE == _DRV_I2C_OPERATION_MODE_GET(i2cInit->i2cMode))
		{
			dObj->modulemainstate = DRV_I2C_MODULE_IDLE;

			if (DRV_I2C_7BIT_SLAVE == _DRV_I2C_ADDRESS_WIDTH_GET(i2cInit->addWidth))
				PLIB_I2C_SlaveAddress7BitSet(plibId, (I2C_SLAVE_ADDRESS_7bit)(i2cInit->slaveaddvalue>>1));
			else
				PLIB_I2C_SlaveAddress10BitSet (plibId,(i2cInit->slaveaddvalue>>1) );

			PLIB_I2C_SlaveClockStretchingEnable(plibId);
			PLIB_I2C_ReservedAddressProtectEnable(plibId);
			PLIB_I2C_GeneralCallDisable ( plibId );
			PLIB_I2C_ReservedAddressProtectEnable(plibId);
			PLIB_I2C_SlaveMask7BitSet ( plibId, i2cInit->maskslaveaddress );
			_DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;
			SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
		}

	   /* Allow reserved slave address */
		dObj->reservedaddenable = _DRV_I2C_RESERVED_ADDRESS_EN_GET(i2cInit->reservedaddenable);
	}

} /* _DRV_I2C_SetupHardware */

//******************************************************************************
/* Function:
    static void _DRV_I2C_LockMutex(DRV_I2C_OBJ *dObj)

 Summary:
    Invokes OSAL call

 Parameters:
    i2cDataObj   - Pointer to the structure which holds the data which is to be
    				added to the queue.

  Remarks:
    None.
*/

void _DRV_I2C_LockMutex(DRV_I2C_OBJ *dObj)
{
    bool tmp = false;
    tmp = tmp; // Remove compile warning
    if (OSAL_MUTEX_Lock(&dObj->mutexDriverInstance, OSAL_WAIT_FOREVER)  != OSAL_RESULT_TRUE) {/* Report error*/}

} /*    _DRV_I2C_LockMutex  */

//******************************************************************************
/* Function:
    static void _DRV_I2C_UnlockMutex(DRV_I2C_OBJ *dObj)

 Summary:
    Invokes OSAL call

 Parameters:
    i2cDataObj   - Pointer to the structure which holds the data which is to be
    				added to the queue.

  Remarks:
    None.
*/

void _DRV_I2C_UnlockMutex(DRV_I2C_OBJ *dObj)
{
    bool tmp = false;
    tmp = tmp; // Remove compile warning
    if (OSAL_MUTEX_Unlock(&dObj->mutexDriverInstance)  != OSAL_RESULT_TRUE) {/* Report error*/}
} /* _DRV_I2C_UnlockMutex */


// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

//******************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_I2C_Initialize ( const SYS_MODULE_INDEX  index,
                                       const SYS_MODULE_INIT * const init )

  Summary:
    Initializes hardware and data for the given instance of the I2C module

  Description:
    This routine initializes hardware for the instance of the I2C module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    index           - Identifies the driver instance to be initialized

    init            - Pointer to the data structure containing all data
                      necessary to initialize the hardware. This pointer may
                      be null if no data is required and static initialization
                      values are to be used.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID.
*/

SYS_MODULE_OBJ DRV_I2C_Initialize ( const SYS_MODULE_INDEX   drvIndex,
                                   const SYS_MODULE_INIT    * const init )
{

    DRV_I2C_INIT * i2cInit;
    I2C_MODULE_ID i2cId;

    /* Validate the driver index */
    if ( drvIndex >= DRV_I2C_INSTANCES_NUMBER )
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    DRV_I2C_OBJ *dObj = _DRV_I2C_INSTANCE_GET ( drvIndex );

    /* Cheap dead man's mutex during initialization to make sure two different
       tasks don't try to initialize the same driver at the same time.*/

    if (dObj->inUse)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    if (i2cNumInited == 0)
    {
        memset(gDrvI2CClientObj, 0, sizeof(gDrvI2CClientObj));
    }

    i2cNumInited++;

    /* Assign to the local pointer the init data passed */
    i2cInit = ( DRV_I2C_INIT * ) init;

    /* Object is valid, set it in use */
    dObj->inUse = true;

    /* Save the index of the driver. Important to know this
    as we are using reference based accessing */
    dObj->drvIndex = drvIndex;

    /* Update the I2C Module Index */
    dObj->i2cId = i2cInit->i2cId;

    /* Update the I2C Module Index */
    dObj->i2cPerph = i2cInit->i2cPerph;

    /* assign SCL and SDA ports for bit banging purposes*/
    dObj->portSCL   = i2cInit->portSCL;
    dObj->pinSCL    = i2cInit->pinSCL;
    dObj->portSDA   = i2cInit->portSDA;
    dObj->pinSDA    = i2cInit->pinSDA;

        /* Enable the I2C module */
    if ( DRV_I2C_BIT_BANG == i2cInit->i2cPerph)
    {

        dObj->tmrSource = i2cInit->tmrSource;
        dObj->tmrInterruptSource = i2cInit->tmrInterruptSource;

//GJV
#ifdef  BB_ENABLED
        DRV_I2C_BB_Initialize(dObj, i2cInit);
#endif
    }

    /* Speed up accessing, take it to a local variable */
    i2cId = dObj->i2cId;

    /* Setup the Hardware */
    _DRV_I2C_SetupHardware ( i2cId, dObj, i2cInit );

    /* Reset the number of clients */
    dObj->numClients = 0;

    /* Reset the locally used variables */
    dObj->lastClientHandle  = DRV_I2C_CLIENTS_NUMBER+1;

    dObj->operationStarting = i2cInit->operationStarting;

    /* Interrupt flag cleared on the safer side */

    _DRV_I2C_InterruptSourceClear( dObj->mstrInterruptSource );
    _DRV_I2C_InterruptSourceClear( dObj->slaveInterruptSource );
    _DRV_I2C_InterruptSourceClear( dObj->errInterruptSource );

    /* Set the current driver state */
    dObj->status = SYS_STATUS_READY;

    dObj->interruptNestingCount = 0;

    /* Create the hardware instance mutex. */
    if(OSAL_MUTEX_Create(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

            /* Check if the global mutexes have been created. If not
       then create these. */

    if(!gDrvI2CCommonDataObj.membersAreInitialized)
    {
        /* This means that mutexes where not created. Create them. */
        if(OSAL_MUTEX_Create(&(gDrvI2CCommonDataObj.mutexClientObjects)) != OSAL_RESULT_TRUE)
        {
                 return SYS_MODULE_OBJ_INVALID;
        }
        if(OSAL_MUTEX_Create(&(gDrvI2CCommonDataObj.mutexBufferQueueObjects)) != OSAL_RESULT_TRUE)
        {
                 return SYS_MODULE_OBJ_INVALID;
        }
        /* Set this flag so that global mutexes get allocated only once */
        gDrvI2CCommonDataObj.membersAreInitialized = true;
    }

    #if defined  __PIC32MZ &&  (__PIC32_FEATURE_SET0 == 'E') && (__PIC32_FEATURE_SET1 == 'C')
    if ( DRV_I2C_PERIPHERAL == i2cInit->i2cPerph)
    {
        PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);           //SDA low
        PLIB_PORTS_PinSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);             //SCL high
        PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);  //SDA output
        PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);   //SCL input
    }
    #endif

    /* Enable the I2C module */
    if ( DRV_I2C_PERIPHERAL == i2cInit->i2cPerph)
	{
		PLIB_I2C_Enable( i2cId ) ;
	} else
    {
		PLIB_I2C_Disable( i2cId );
	}
    /* Return the driver handle */
    return( (SYS_MODULE_OBJ) dObj );
} /* DRV_I2C_Initialize */


//******************************************************************************
/* Function:
    void DRV_I2C_Deinitialize ( SYS_MODULE_OBJ object )

  Summary:
    De-initializes the specific module instance of the I2C module

  Description:
    De-initializes the specific module instance disabling its operation (and any
    hardware for driver modules).  Resets all the internal data structures and
    fields for the specified instance to the default settings.

  Parameters:
    object          - Identifies the Driver Object returned by the Initialize
                      interface

  Returns:
    None
*/

void DRV_I2C_Deinitialize ( SYS_MODULE_OBJ object )
{
	DRV_I2C_OBJ *dObj = (DRV_I2C_OBJ*) object;
	I2C_MODULE_ID i2cId = dObj->i2cId;
	size_t iClient;
	uint8_t iDriver;
	if (dObj->inUse == false)
	{
		return;
	}

	/* Interrupt De-Registration */
#if defined (DRV_I2C_INTERRUPT_MODE) && (DRV_I2C_INTERRUPT_MODE == true)
     if (dObj->mstrInterruptEnabled) _DRV_I2C_InterruptSourceDisable(dObj->mstrInterruptSource);
     if (dObj->slaveInterruptEnabled) _DRV_I2C_InterruptSourceDisable(dObj->slaveInterruptSource);
     if (dObj->errInterruptEnabled) _DRV_I2C_InterruptSourceDisable(dObj->errInterruptSource);
#endif

        PLIB_I2C_Disable(i2cId);
    if(OSAL_MUTEX_Delete(&(dObj->mutexDriverInstance)) != OSAL_RESULT_TRUE)
    {
       return;
    }

    /* Check if the global mutexes have been created. If so
       then delete these. */
    if(gDrvI2CCommonDataObj.membersAreInitialized)
    {
        /* This means that mutexes where created. Delete them. */
        if(OSAL_MUTEX_Delete(&(gDrvI2CCommonDataObj.mutexClientObjects)) != OSAL_RESULT_TRUE)
        {
                 return;
        }
        if(OSAL_MUTEX_Delete(&(gDrvI2CCommonDataObj.mutexBufferQueueObjects)) != OSAL_RESULT_TRUE)
        {
                 return;
        }
        /* Set this flag so that global mutexes get allocated only once */
        gDrvI2CCommonDataObj.membersAreInitialized = false;
    }

	for (iClient = 0; iClient < DRV_I2C_CLIENTS_NUMBER; iClient++)
	{
            if (gDrvI2CClientObj[iClient].driverObject == dObj)
            {
                    gDrvI2CClientObj[iClient].driverObject = NULL;
            }
	}

	dObj->numClients = 0;
	dObj->isExclusive = false;
	/* Clear all the pending requests */
//        while (_DRV_I2C_QueuePop(dObj) != NULL);
    while ((_DRV_I2C_IsQueueEmpty(dObj) == false))
    {
        _DRV_I2C_Advance_Queue(dObj);
    }
	dObj->queueHead = NULL;
	/* Set the Device Status */
	dObj->status = SYS_STATUS_UNINITIALIZED;

	/* Remove the driver usage */
	dObj->inUse = false;

	for (iDriver = 0; iDriver < DRV_I2C_INSTANCES_NUMBER; iDriver++)
	{
            DRV_I2C_OBJ * obj = _DRV_I2C_INSTANCE_GET ( iDriver );

            if (obj->inUse == true)
            {
                return;
            }
	}
    i2cNumInited--;

    return;

} /* DRV_I2C_Deinitialize */


//******************************************************************************
/* Function:
    SYS_STATUS DRV_I2C_Status ( SYS_MODULE_OBJ object )

  Summary:
    Provides the current status of the hardware instance of the I2C module

  Description:
    This routine Provides the current status of the hardware instance of the
    I2C module.

  Parameters:
    object          - Identifies the Driver Object returned by the Initialize
                      interface

  Returns:
    SYS_STATUS_READY    Indicates that any previous module operation for the
                        specified module has completed

    SYS_STATUS_BUSY     Indicates that a previous module operation for the
                        specified module has not yet completed

    SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

SYS_STATUS DRV_I2C_Status ( SYS_MODULE_OBJ object )
{
    if ( object == SYS_MODULE_OBJ_INVALID )
    {
        //SYS_ASSERT( " Handle is invalid " );
        return SYS_MODULE_OBJ_INVALID;
    }
    DRV_I2C_OBJ *dObj = (DRV_I2C_OBJ*) object;

    /* Return the status associated with the driver handle */
    return ( dObj->status );
} /* DRV_I2C_Status */


//******************************************************************************
/* Function:
    void DRV_I2C_Tasks ( SYS_MODULE_OBJ object )

  Summary:
    Used to maintain the driver's state machine and implement its ISR

  Description:
    This routine is used to maintain the driver's internal state machine and
    implement its ISR for interrupt-driven implementations.

  Parameters:
    object          - Identifies the Driver Object returned by the Initialize
                      interface

  Returns:
    None.
*/

void DRV_I2C_Tasks ( SYS_MODULE_OBJ object )
{
    DRV_I2C_OBJ             *dObj           = (DRV_I2C_OBJ*)object;
    DRV_I2C_BUFFER_OBJECT   *lBufferObj     = dObj->taskLObj;
//    uint8_t                 dummy           = 0xFF;
    I2C_MODULE_ID           i2cId           = dObj->i2cId;
    DRV_I2C_CLIENT_OBJ      *lClientObj     = NULL;

//    static unsigned char bytecounter = 0;


    if ( object == SYS_MODULE_OBJ_INVALID )
    {
        //SYS_ASSERT( " Handle is invalid " );
        return;
    }

        switch ( dObj->task )
        {
            case DRV_I2C_TASK_SEND_DEVICE_ADDRESS:

                /* Pop the first element from the queue */
                if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
                {
                    if ( ( dObj->queueHead != NULL ) )
                    {
                        /* This should be a global variable since the control could
                        go out of this function */

                        dObj->taskLObj = _DRV_I2C_QueuePop(dObj);

                        /* Take it to a local variable to avoid multiple referencing and to speed up */
                        lBufferObj   = dObj->taskLObj;

                        lClientObj = (DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle;

                        /*  We need not do this all the time but when the request from a
                            new client  */
                        _DRV_I2C_CLIENT_SWITCH_CLIENT();

                        /* Update the task state as per the operation */
                        dObj->task = lBufferObj->operation + DRV_I2C_TASK_PROCESS_WRITE_ONLY;

                        if ( lBufferObj->operation == DRV_I2C_OP_READ )
                        {
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                 DRV_I2C_BUFFER_SLAVE_READ_REQUESTED;
                            dObj->modulemainstate = DRV_I2C_MASTER_RX_FROM_SLAVE;
                            PLIB_I2C_TransmitterByteSend( i2cId,(DRV_I2C_OP_READ | lBufferObj->slaveaddresshighbyte) );
                            dObj->task = DRV_I2C_TASK_SET_RCEN_ONLY;
                        }
                        else
                        {
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                            DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED;
                            PLIB_I2C_TransmitterByteSend ( i2cId, lBufferObj->slaveaddresshighbyte ) ;
                        }
                    }
                }
                else if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_SLAVE)
                {
                        /* master writing data to slave when R/W = 0 and Address is detected */
                        dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;

                        /* Slave is accepting data from Master */
                        if ( (!PLIB_I2C_SlaveReadIsRequested(i2cId)) && (PLIB_I2C_SlaveAddressIsDetected(i2cId)) )
                        {
                                /* slave is set to accept data from master */
                                dObj->modulemainstate = DRV_I2C_SLAVE_READY_TO_RX_FROM_MASTER;

                                if (lBufferObj)
                                {
                                    lBufferObj->inUse = false;
                                }

                                if (dObj->operationStarting)
                                {
                                    dObj->operationStarting(DRV_I2C_BUFFER_SLAVE_READ_REQUESTED, 0);
                                }

                                /* This should be a global variable since the control could
                                    go out of this function */
                                dObj->taskLObj = _DRV_I2C_QueuePop(dObj);

                                /* Take it to a local variable to avoid multiple referencing and to speed up */
                                lBufferObj   = dObj->taskLObj;

                                lBufferObj->actualtransfersize = 0;

                                Nop();

                                lClientObj = (DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle;

                                Nop();

                                _DRV_I2C_CLIENT_SWITCH_CLIENT();

                                /* do a dummy read so that I2CxRCV is cleared and not clobbered with data */
                                PLIB_I2C_ReceivedByteGet ( i2cId );
                                PLIB_I2C_SlaveClockRelease ( i2cId );
                        }
                        /*  Slave is sending data back to the Master    */
                        else if ( (PLIB_I2C_SlaveReadIsRequested(i2cId)) && (PLIB_I2C_SlaveAddressIsDetected(i2cId)) )
                        {

                            PLIB_I2C_ReceivedByteGet ( i2cId );

                            PLIB_I2C_SlaveClockHold (i2cId);

                            dObj->modulemainstate = DRV_I2C_SLAVE_READY_TO_TX_TO_MASTER;
                            if (lBufferObj)
                            {
                                lBufferObj->inUse = false;
                            }

                            if (dObj->operationStarting)
                            {
                                dObj->operationStarting(DRV_I2C_BUFFER_SLAVE_WRITE_REQUESTED, 0);
                            }
                            lBufferObj = dObj->taskLObj = _DRV_I2C_QueuePop(dObj);

                            lBufferObj->actualtransfersize = 0;

                            lClientObj = (DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle;

                            _DRV_I2C_CLIENT_SWITCH_CLIENT();

                            // transmit first byte
                            PLIB_I2C_TransmitterByteSend(_DRV_I2C_PERIPHERAL_ID_GET(i2cId), *lBufferObj->txBuffer++);
                            lBufferObj->actualtransfersize++;
                            PLIB_I2C_SlaveClockRelease ( i2cId );
                            PLIB_PORTS_PinSet(PORTS_ID_0, lClientObj->irqSelectPort, lClientObj->irqSelectBitPos);   //Pull-High IRQ
                        }
                        else if ( (!PLIB_I2C_SlaveReadIsRequested(i2cId)) && PLIB_I2C_SlaveDataIsDetected( i2cId ))
                        {
                            /*        Master sends data to the slave        */
                            if (dObj->modulemainstate == DRV_I2C_SLAVE_READY_TO_RX_FROM_MASTER)
                            {
                                PLIB_I2C_SlaveClockRelease ( i2cId );

                                if (lBufferObj->actualtransfersize < lBufferObj->transferSize)
                                {
                                    *lBufferObj->rxBuffer++ = PLIB_I2C_ReceivedByteGet ( i2cId );
                                     lBufferObj->actualtransfersize++;
                                     _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                                     _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                             DRV_I2C_BUFFER_SLAVE_READ_BYTE;

                                    /*  Call the event handler. Increment the interrupt nesting
                                        count which lets the driver functions that are called
                                        from the event handler know that an interrupt context
                                        is active.
                                    */

                                    dObj->interruptNestingCount++;

                                    if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                    {
                                           _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle),
                                                       callback)( DRV_I2C_BUFFER_SLAVE_READ_BYTE, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                    }

                                    dObj->interruptNestingCount--;
                                }
                                else
                                {
                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                             DRV_I2C_BUFFER_EVENT_ERROR;

                                    PLIB_I2C_ReceivedByteGet ( i2cId );

                                    if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                    {
                                        _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle),
                                                    callback)( DRV_I2C_BUFFER_EVENT_ERROR, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                    }
                                }
                            }
                        }
                        else if (PLIB_I2C_SlaveReadIsRequested(i2cId) && PLIB_I2C_SlaveDataIsDetected( i2cId ))
                        {
                            PLIB_I2C_SlaveClockHold (i2cId);

                            if ( PLIB_I2C_TransmitterByteWasAcknowledged(i2cId) )
                            {
                                if ( lBufferObj->actualtransfersize < lBufferObj->transferSize )
                                {
                                    PLIB_I2C_SlaveClockRelease(i2cId);
                                    PLIB_I2C_TransmitterByteSend ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ), *lBufferObj->txBuffer++ );
                                    lBufferObj->actualtransfersize++;
                                    lBufferObj->inUse = false;

                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                             DRV_I2C_BUFFER_SLAVE_WRITE_BYTE;

                                    /*  Call the event handler. Increment the interrupt nesting
                                        count which lets the driver functions that are called
                                        from the event handler know that an interrupt context
                                        is active.
                                    */

                                    dObj->interruptNestingCount++;

                                    if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                    {
                                        _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle),
                                                    callback)( DRV_I2C_BUFFER_SLAVE_WRITE_BYTE, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                    }

                                    dObj->interruptNestingCount--;
                                }
                                else
                                {
                                   _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                                   _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                        DRV_I2C_BUFFER_EVENT_ERROR;

                                     /* Have a check here because DRV_I2C_ClientSetup function call is optional */
                                     if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                     {
                                      /* Give an indication to the higher layer upon successful transmission */
                                     _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                                              ( DRV_I2C_BUFFER_EVENT_ERROR, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                     }
                                     /* dummy write - pad with zeros - cases where master requests more data than what is in buffer */
                                     PLIB_I2C_TransmitterByteSend(_DRV_I2C_PERIPHERAL_ID_GET(i2cId), 0);
                                }
                            }
                            else if ( !PLIB_I2C_TransmitterByteWasAcknowledged(i2cId) )
                            {

                               _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;
                               _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                            DRV_I2C_BUFFER_EVENT_COMPLETE;

                                dObj->interruptNestingCount++;

                                 /* Have a check here because DRV_I2C_ClientSetup function call is optional */
                                if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                {
                                    /* Give an indication to the higher layer upon successful transmission */
                                    _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                                          ( DRV_I2C_BUFFER_EVENT_COMPLETE, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                }

                                dObj->interruptNestingCount--;
                            }
                        }
                    }
                break;
            case DRV_I2C_SEND_RANDOM_READ_DEVICE_ADDRESS:
                    dObj->modulemainstate = DRV_I2C_MASTER_RX_FROM_SLAVE;
                    PLIB_I2C_TransmitterByteSend( i2cId,(DRV_I2C_OP_READ | lBufferObj->slaveaddresshighbyte) );
                    dObj->task = DRV_I2C_TASK_SET_RCEN_ONLY;
                break;
            case DRV_I2C_TASK_PROCESS_READ_ONLY:
                /* Read I2CxRCV register for received data */
                if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
                {
                    if (lBufferObj->transferSize)
                    {
                        dObj->modulemainstate = DRV_I2C_MASTER_RX_FROM_SLAVE;

                        if ( PLIB_I2C_ReceivedByteIsAvailable (i2cId) )
                        {
                            /* If all but one reception is complete */
                            if ( lBufferObj->transferSize > 1 )
                            {
                                *lBufferObj->rxBuffer++ = PLIB_I2C_ReceivedByteGet (i2cId);
                                lBufferObj->transferSize--;
                                dObj->task = DRV_I2C_TASK_SET_RCEN_ONLY;
                                if (PLIB_I2C_MasterReceiverReadyToAcknowledge(i2cId))
                                {
                                    PLIB_I2C_ReceivedByteAcknowledge ( i2cId, true );
                                }
                            }
                            else
                            {
                                lBufferObj->transferSize--;

//                                lBufferObj->inUse = false;

                                *lBufferObj->rxBuffer++ = PLIB_I2C_ReceivedByteGet ( i2cId );
                                if (PLIB_I2C_MasterReceiverReadyToAcknowledge(i2cId))
                                {
                                    PLIB_I2C_ReceivedByteAcknowledge ( i2cId, false  );
                                }

                                dObj->task = DRV_I2C_BUS_SILENT;
                            }
                        }
                        else
                        {
                            // Do not block in any case
                            break;
                        }
                    }
                }
                break;
            case DRV_I2C_TASK_PROCESS_WRITE_ONLY:
            case DRV_I2C_TASK_PROCESS_WRITE_READ:

                if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
                {
                    /* Loop till the transmit size, do not block though */
                    if( lBufferObj->transferSize )
                    {
                        if ( PLIB_I2C_TransmitterByteHasCompleted ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ) ) )
                        {
                            if (PLIB_I2C_TransmitterByteWasAcknowledged ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ) )   )
                            {

//                                lBufferObj->inUse = false;

                                /* Handle the overflow */
                                if ( lBufferObj->transferSize > 1 )
                                {
                                    PLIB_I2C_TransmitterByteSend ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ), *lBufferObj->txBuffer++ );
                                    lBufferObj->transferSize--;
                                }
                                else
                                {
                                    lBufferObj->transferSize--;

//                                    /* Hold the buffer till the completion of the operation */
//                                    lBufferObj->inUse = false;

                                    PLIB_I2C_TransmitterByteSend ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ), *lBufferObj->txBuffer++ );

                                    dObj->task = DRV_I2C_BUS_SILENT;

                                }
                            }
                            else
                            {

                                #if defined  __PIC32MZ &&  (__PIC32_FEATURE_SET0 == 'E') && (__PIC32_FEATURE_SET1 == 'C')
                                /*   Errata for PIC32MZ which requires reset of I2C module */

                                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;

                                    /* alternate method to do STOP (previous errata note) */

                                    #ifdef MZ_EC_ERRATA_25_BB_STOP               //GJV
                                        /* Bit bang procedure for STOP */

                                        starttime = ReadCoreTimer();
                                        while(ReadCoreTimer()-starttime < BRG_1_TIME_0);

                                        /* Disable I2C */
                                        PLIB_I2C_Disable(i2cId);

                                        /* Wait 2 BRG */
                                        starttime = ReadCoreTimer();
                                        while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

                                        /* Set SDA as I/P */
                                        PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);

                                        /* Wait 3 BRG */
                                        starttime = ReadCoreTimer();
                                        while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

                                        PLIB_I2C_Enable(i2cId);
                                        PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                                        PLIB_PORTS_PinSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                                        PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                                        PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);

                                        /* End-of-Bit-Bang procedure for STOP */
                                    #else                                                   //GJV
                                      PLIB_I2C_Disable(i2cId);
                                      PLIB_I2C_Enable(i2cId);
                                    #endif                                                  //GJV

                                    if (lBufferObj)
                                    {
                                        lBufferObj->inUse = false;
                                    }

                                    /*clear any previous status flags */
                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;

                                    /* set status to buffer event complete */
                                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                        DRV_I2C_BUFFER_EVENT_COMPLETE;

                                    /* Have a check here because DRV_I2C_ClientSetup function call is optional */                       //Issue Callback for STOP event
                                    if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                                    {
                                        /* Give an indication to the higher layer upon successful transmission */
                                        _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                                                ( DRV_I2C_SEND_STOP_EVENT, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                                    }
                                #else
                                    dObj->task = DRV_I2C_TASK_PROCESS_STOP;
                                    PLIB_I2C_MasterStop (i2cId);
                                #endif

                            }
                        }
                        else
                        {
                            /* Do not block in any case */
                            break;
                        }
                    }
                }
                break;
            case DRV_I2C_BUS_SILENT:

                /*  The Bus is Silent/Idle when the last byte is either ACK'ed  OR
                    in the event of slave unexpectedly aborting operation, check
                    if transmission is complete and NACK is received   */

                if ( PLIB_I2C_TransmitterByteWasAcknowledged ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ) ) ||
                      ( PLIB_I2C_TransmitterByteHasCompleted ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ) )  &&
                            (!PLIB_I2C_TransmitterByteWasAcknowledged ( _DRV_I2C_PERIPHERAL_ID_GET ( i2cId ) )) ) )
                {
                    if ( lBufferObj->operation == DRV_I2C_OP_WRITE_READ )
                    {

                        SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);

//                        dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;          //Go to START condition
                        dObj->task  = DRV_I2C_SEND_RANDOM_READ_DEVICE_ADDRESS;

                        lBufferObj->operation = DRV_I2C_OP_READ;

                        lBufferObj->transferSize = lBufferObj->readtransferSize;    //Assign # of bytes to be read into transfer size

//                        _DRV_I2C_QueuePush( dObj, lBufferObj);

                        _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;

                        _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                DRV_I2C_SEND_RESTART_EVENT;

                        PLIB_I2C_MasterStartRepeat (dObj->i2cId);
                    }
                    else
                    {
                        #if defined  __PIC32MZ &&  (__PIC32_FEATURE_SET0 == 'E') && (__PIC32_FEATURE_SET1 == 'C')
                                 _DRV_I2C_InterruptSourceDisable (_DRV_I2C_INT_SRC_GET(dObj->mstrInterruptSource));
                                 dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                        #else
                                dObj->task = DRV_I2C_TASK_PROCESS_STOP;
                        #endif

                        #if defined  __PIC32MZ &&  (__PIC32_FEATURE_SET0 == 'E') && (__PIC32_FEATURE_SET1 == 'C')
                        /*   Errata for PIC32MZ which requires reset of I2C module */

                            #ifdef MZ_EC_ERRATA_25_BB_STOP               //GJV
                                /* Bit bang procedure for STOP */

                                starttime = ReadCoreTimer();
                                while(ReadCoreTimer()-starttime < BRG_1_TIME_0);

                                /* Disable I2C */
                                PLIB_I2C_Disable(i2cId);

                                /* Wait 2 BRG */
                                starttime = ReadCoreTimer();
                                while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

                                /* Set SDA as I/P */
                                PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);

                                /* Wait 3 BRG */
                                starttime = ReadCoreTimer();
                                while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

                                PLIB_I2C_Enable(i2cId);
                                PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                                PLIB_PORTS_PinSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                                PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
                                PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
                            #else
                                /* alternate method to do STOP (previous errata note) */
                                PLIB_I2C_Disable(i2cId);
                                PLIB_I2C_Enable(i2cId);
                            #endif

                            /* End-of-Bit-Bang procedure for STOP */

                            if (lBufferObj)
                            {
                                lBufferObj->inUse = false;
                            }

                            /* set buffer event complete status and callback here instead of STOP condition
                            because STOP condition is PIC32MZ-EC is bit-banged and would not generate an interrupt  */
                            dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;

                            /*clear any previous status flags */
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;

                            /* set status to buffer event complete */
                            _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                                    DRV_I2C_BUFFER_EVENT_COMPLETE;

                            if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                            {
                                    /* Give an indication to the higher layer upon successful transmission */
                                    _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                                    ( DRV_I2C_BUFFER_EVENT_COMPLETE, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                            }
                        #else
                            PLIB_I2C_MasterStop (i2cId);
                        #endif
                    }
                }
                break;
            case DRV_I2C_TASK_SET_RCEN_ONLY:
                if ( (PLIB_I2C_TransmitterByteWasAcknowledged (i2cId)) ||
                        ((PLIB_I2C_ReceiverByteAcknowledgeHasCompleted(i2cId) == true)) )
                {
                    PLIB_I2C_MasterReceiverClock1Byte (i2cId);
                    dObj->task = DRV_I2C_TASK_PROCESS_READ_ONLY;
                }
                else
                {
                    dObj->task = DRV_I2C_TASK_PROCESS_STOP;
                    _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                        DRV_I2C_SEND_STOP_EVENT;
                    /* Have a check here because DRV_I2C_ClientSetup function call is optional */
                    if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                    {
                        /* Give an indication to the higher layer upon successful transmission */
                        _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                                ( DRV_I2C_SEND_STOP_EVENT, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                    }
                }
                break;
            case DRV_I2C_TASK_PROCESS_STOP:

#if defined (DRV_I2C_INTERRUPT_MODE) && (DRV_I2C_INTERRUPT_MODE == true)
                 _DRV_I2C_InterruptSourceDisable (_DRV_I2C_INT_SRC_GET(dObj->mstrInterruptSource));
#endif
                if (lBufferObj)
                {
                    lBufferObj->inUse = false;
                }

                 dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                _DRV_I2C_CLIENT_OBJ(lBufferObj, status) = 0;


                _DRV_I2C_CLIENT_OBJ(lBufferObj, status) |=
                    DRV_I2C_BUFFER_EVENT_COMPLETE;

               /* Have a check here because DRV_I2C_ClientSetup function call is optional */
                if ( _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback) != NULL )
                {
                    /* Give an indication to the higher layer upon successful transmission */
                    _DRV_I2C_CLIENT_OBJ(((DRV_I2C_CLIENT_OBJ *)lBufferObj->clientHandle), callback)
                    ( DRV_I2C_BUFFER_EVENT_COMPLETE, (DRV_I2C_BUFFER_HANDLE)lBufferObj, 0x00 );
                }

                break;
            default:
                break;

        }

        /* This state is encountered when an error interrupt has occurred.
           or an error has occurred during read */

        if ( true == _DRV_I2C_InterruptSourceStatusGet ( dObj->errInterruptSource ) )
        {
            /* Check for the overflow error */
            if ( PLIB_I2C_ReceiverOverflowHasOccurred ( i2cId ) )
            {
                if ( PLIB_I2C_ExistsReceiverOverflow ( i2cId ) )
                {
                    PLIB_I2C_ReceiverOverflowClear ( i2cId );
                }
            }

            _DRV_I2C_InterruptSourceClear ( dObj->errInterruptSource );
        }

        if (SYS_INT_SourceStatusGet(dObj->mstrInterruptSource))
        {
            SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
        }
        if (SYS_INT_SourceStatusGet(dObj->slaveInterruptSource))
        {
            SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
            PLIB_I2C_SlaveClockRelease ( i2cId );
        }
        if (SYS_INT_SourceStatusGet(dObj->errInterruptSource))
        {
            SYS_INT_SourceStatusClear(dObj->errInterruptSource);
        }

        if ( (dObj->queueHead != NULL) &&  (_DRV_I2C_CLIENT_OBJ(lBufferObj, status) == DRV_I2C_BUFFER_EVENT_COMPLETE) )
        {
            _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;

            if (PLIB_I2C_BusIsIdle(dObj->i2cId))
            {
                PLIB_I2C_MasterStart(dObj->i2cId);
                Nop();
            }
        }

}   /* DRV_I2C_TASKS */


//******************************************************************************
/* Function:
    DRV_HANDLE DRV_I2C_Open ( const SYS_MODULE_INDEX    index,
                             const DRV_IO_INTENT       intent )

  Summary:
    Opens the specific module instance and returns a handle

  Description:
    This routine opens a driver for use by any client module and provides a
    handle that must be provided to any of the other driver operations to
    identify the caller and the instance of the driver/hardware module.

  Parameters:
    index           - Identifier for the instance to be initialized
    ioIntent        - Possible values from the enumeration DRV_IO_INTENT

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance)
    If an error occurs, the return value is DRV_HANDLE_INVALID
*/

DRV_HANDLE DRV_I2C_Open ( const SYS_MODULE_INDEX   drvIndex,
                         const DRV_IO_INTENT      ioIntent )
{
    /* Multi client variables are removed from single client builds. */
    DRV_I2C_OBJ         *dObj;
    size_t              iClient;

    /* Validate the driver index */
    if( drvIndex >= DRV_I2C_INSTANCES_NUMBER )
    {
        return DRV_HANDLE_INVALID;
    }
    dObj = _DRV_I2C_INSTANCE_GET(drvIndex);

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//        I2C_Initialize ( 50000, 0, 0 );
        Nop();
    }
    else
    {

        /* Check for exclusive access */
        _DRV_I2C_LockMutex(dObj);

        /* Check if max number of clients open */
        if ( ( dObj->isExclusive == true ) ||
             ( dObj->inUse != true ) ||
             (( dObj->numClients > 0 ) &&
                DRV_IO_ISEXCLUSIVE( ioIntent )) ||
             (dObj->numClients >= DRV_I2C_CLIENTS_NUMBER))
        {
            /* Set that the hardware instance is opened in exclusive mode */
            _DRV_I2C_UnlockMutex(dObj);
             return DRV_HANDLE_INVALID;
        }
        _DRV_I2C_UnlockMutex(dObj);
    }

    if(OSAL_MUTEX_Lock(&(gDrvI2CCommonDataObj.mutexClientObjects), OSAL_WAIT_FOREVER) == OSAL_RESULT_TRUE)
    {
        /* Setup client operations */
        /* Find available slot in array of client objects */
        for ( iClient = 0; iClient < DRV_I2C_CLIENTS_NUMBER ; iClient++ )
        {
            DRV_I2C_CLIENT_OBJ  *clientObj      =
                            ( DRV_I2C_CLIENT_OBJ* ) _DRV_I2C_CLIENT_OBJ_GET(iClient);
            if ( !clientObj->inUse )
            {
                /* Increment the client in case of Multi client support, otherwise remove
                the below statement */

                dObj->numClients++;

                /* Update that, the client is opened in exclusive access mode */
                if( DRV_IO_ISEXCLUSIVE( ioIntent ) )
                {
                    dObj->isExclusive = true;
                }
                clientObj->inUse  = true;

                /* We have found a client object. Release the mutex */

                OSAL_MUTEX_Unlock(&(gDrvI2CCommonDataObj.mutexClientObjects));

    //            _DRV_I2C_UnlockMutex(dObj);
                clientObj->driverObject = dObj;
                clientObj->intent = ioIntent;

                /* Return the client object */
                return ( DRV_HANDLE ) clientObj;
            }
        }

        /* Could not find a client object. Release the mutex and
           return with an invalid handle. */
        OSAL_MUTEX_Unlock(&(gDrvI2CCommonDataObj.mutexClientObjects));
    }
//    _DRV_I2C_UnlockMutex(dObj);

    return  DRV_HANDLE_INVALID ;
} /* DRV_I2C_Open */


//******************************************************************************
/* Function:
    void DRV_I2C_Close ( DRV_HANDLE handle )

  Summary:
    Closes an opened-instance of a driver

  Description:
    This routine closes an opened-instance of a driver, invalidating the given
    handle.

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

  Returns:
    None
*/

void DRV_I2C_Close ( DRV_HANDLE handle )
{
    /* Multi client variables are removed from single client builds. */
    DRV_I2C_CLIENT_OBJ *clientObj = (DRV_I2C_CLIENT_OBJ*)handle;

    if (clientObj->inUse == false)
    {
        //_DRV_I2C_UnlockMutex(clientObj->driverObject);
        return;
    }

    /* Free the Client Instance */
	if (clientObj->driverObject != NULL)
	{
		clientObj->driverObject->numClients --;
		clientObj->driverObject->isExclusive = false;
	}
    clientObj->inUse = false ;

} /* DRV_I2C_Close */


// *****************************************************************************
/* Function:
    void DRV_I2C_ClientSetup ( DRV_HANDLE handle,
                                 const DRV_I2C_CLIENT_SETUP * const config )

  Summary:
    Sets up the device communication parameters

  Description:
    This function sets up the device communication parameters

  Parameters:
    handle       - A valid open-instance handle, returned from the driver's
                   open routine

    config       - Port parameters tied to IRQ line

  Returns:
    None
*/

void DRV_I2C_ClientSetup ( DRV_HANDLE handle,
                                 const DRV_I2C_CLIENT_SETUP * const config )
{
//    bool temp;
    DRV_I2C_CLIENT_OBJ *clientObj = (DRV_I2C_CLIENT_OBJ*) handle;


    _DRV_I2C_SAVE_LAST_CLIENT();
//    temp = config->irqSetupLogicLevel;

    clientObj->irqSelectLogicLevel  = config->irqSetupLogicLevel;
    clientObj->irqSelectPort        = config->irqSetupPort;
    clientObj->irqSelectBitPos      = config->irqSetupBitPos;

    SYS_PORTS_PinDirectionSelect( PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT,
            clientObj->irqSelectPort, clientObj->irqSelectBitPos);
    if (clientObj->irqSelectLogicLevel == true)
        SYS_PORTS_PinSet (PORTS_ID_0, clientObj->irqSelectPort, clientObj->irqSelectBitPos );
    else
        SYS_PORTS_PinClear (PORTS_ID_0, clientObj->irqSelectPort, clientObj->irqSelectBitPos );

}  /* DRV_I2C_ClientSetup */

//******************************************************************************
/* Function:
    void DRV_I2C_BufferEventHandlerSet ( const DRV_HANDLE handle,
                    const DRV_I2C_BUFFER_EVENT_HANDLER eventHandler,
                    const uintptr_t context )

  Summary:
	Adds a callback function for a client.

  Description:
    This routine adds a callback function for a client.

  Parameters:
        handle       	- A valid open-instance handle, returned from the driver's
						open routine.

	eventHandler    - Call back function.

	context		 	-  context.

  Returns:
    None.
*/

void DRV_I2C_BufferEventHandlerSet (const DRV_HANDLE handle,
                    const DRV_I2C_BUFFER_EVENT_HANDLER eventHandler,
                    const uintptr_t context )
{
	( (DRV_I2C_CLIENT_OBJ *) handle)->callback = eventHandler;

	( (DRV_I2C_CLIENT_OBJ *) handle)->context = context;
}   /* DRV_I2C_BufferEventHandlerSet */


//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_Receive (     DRV_HANDLE handle,
                                                uint8_t* address,
                                                void *rxBuffer,
                                                size_t size,
                                                void * context);

  Summary:
    Adds a buffer to queue with a read request. Driver will process this
    request in the task routine.

  Description:
    This routine adds a buffer to queue with a read request. Driver will process
    this request in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    rxBuffer    - This buffer holds data is received
    size        - The number of bytes that the Master expects to read from Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the READ operation
                  is terminated.
    context     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/


DRV_I2C_BUFFER_HANDLE DRV_I2C_Receive ( DRV_HANDLE handle,
                                        uint16_t address,
                                        void *buffer,
                                        size_t size,
                                        void * callbackContext )
{
    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE)DRV_I2C_MasterBufferReadXE( (DRV_HANDLE) handle, address, (uint8_t*)buffer, (uint16_t) size);
#endif
    }
    else
    {

        /* Get a slot in the queue */
       i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );


        if ( i2cDataObj != NULL )
        {
            /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->i2cPerph                = DRV_I2C_PERIPHERAL;
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = address;
            i2cDataObj->slaveaddresslowbyte     = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_READ;
            i2cDataObj->txBuffer                = NULL;
            i2cDataObj->rxBuffer                = buffer;
            i2cDataObj->transferSize            = size;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = callbackContext;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
               if (PLIB_I2C_BusIsIdle(dObj->i2cId))
               {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                   _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                   dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                   PLIB_I2C_MasterStart(dObj->i2cId);
               }
//               else
//                    return (DRV_I2C_BUFFER_HANDLE)NULL;
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;
            }
            return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
        }
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}   /* DRV_I2C_Receive */

//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddRead (   DRV_HANDLE handle,
                                                    uint8_t* slaveaddress,
                                                    void *rxBuffer,
                                                    size_t size,
                                                    void * context);

  Summary:
    Adds a buffer to queue with a read request. Driver will process this
    request in the task routine.

  Description:
    This routine adds a buffer to queue with a read request. Driver will process
    this request in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    slaveaddress- Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    rxBuffer    - This buffer holds data is received
    size        - The number of bytes that the Master expects to read from Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the READ operation
                  is terminated.
    context     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/


DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddRead ( DRV_HANDLE handle, uint8_t* deviceaddress,
                                    void *rxBuffer,
                                    size_t size,
                                    void * context )
{
    DRV_I2C_BUFFER_OBJECT   *i2cDataObj;
    DRV_I2C_OBJ             *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE)DRV_I2C_MasterBufferRead( (DRV_HANDLE) handle, deviceaddress, (uint8_t*)rxBuffer, (uint16_t) size);
#endif
    }
    else
    {
        /* Get a slot in the queue */
       i2cDataObj = _DRV_I2C_QueueSlotGet ( dObj );


        if ( i2cDataObj != NULL )
        {
            /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->i2cPerph                = DRV_I2C_PERIPHERAL;
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
            i2cDataObj->slaveaddresslowbyte = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_READ;
            i2cDataObj->txBuffer                = NULL;
            i2cDataObj->rxBuffer                = rxBuffer;
            i2cDataObj->transferSize            = size;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = context;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
               if (PLIB_I2C_BusIsIdle(dObj->i2cId))
               {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                   _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                   dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                   PLIB_I2C_MasterStart(dObj->i2cId);
               }
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;
            }
            return (DRV_I2C_BUFFER_HANDLE)i2cDataObj;
        }
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;
}   /* DRV_I2C_BufferAddRead */

//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_Transmit  (   DRV_HANDLE handle,
                                                uint8_t* address,
                                                void *txBuffer,
                                                size_t size,
                                                void * callbackContext);
  Summary:
    Adds a buffer to queue with a write request. Driver will process this
    request in the task routine.

  Description:
    This routine adds a buffer to queue with a read request. Driver will process
    this request in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    buffer      - Contains data to be transmitted
    size        - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    callbackContext     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_Transmit ( DRV_HANDLE handle, uint16_t address,
                                    void *buffer,
                                    size_t size,
                                    void * callbackContext)
{
    DRV_I2C_BUFFER_OBJECT *i2cDataObj;
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE)DRV_I2C_MasterBufferWriteXE( (DRV_HANDLE) handle, address, (uint8_t*)buffer, (uint16_t) size);
#endif
    }
    else
    {

        /* Get a slot in the queue */
        i2cDataObj = _DRV_I2C_QueueSlotGet (dObj);

        if ( i2cDataObj != NULL )
        {
           /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = address;
            i2cDataObj->slaveaddresslowbyte     = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_WRITE;
            i2cDataObj->txBuffer                = buffer;
            i2cDataObj->rxBuffer                = NULL;
            i2cDataObj->transferSize            = size;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = callbackContext;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
                if (PLIB_I2C_BusIsIdle(dObj->i2cId))
                {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                    _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(dObj->i2cId);
                }
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;

            }

            return (DRV_I2C_BUFFER_HANDLE) i2cDataObj;
        }

        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;
} /* DRV_I2C_Transmit */

//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWrite (  DRV_HANDLE handle,
                                                    uint8_t* slaveaddress,
                                                    void *txBuffer,
                                                    size_t size,
                                                    void * context);
  Summary:
    Adds a buffer to queue with a write request. Driver will process this
    request in the task routine.

  Description:
    This routine adds a buffer to queue with a read request. Driver will process
    this request in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    slaveaddress- Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    txBuffer    - Contains data to be transffered
    size        - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    context     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWrite ( DRV_HANDLE handle, uint8_t* deviceaddress,
                                    void *txBuffer,
                                    size_t size,
                                    void * context)
{
    DRV_I2C_BUFFER_OBJECT *i2cDataObj;
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE)DRV_I2C_MasterBufferWrite( (DRV_HANDLE) handle, deviceaddress, (uint8_t*)txBuffer, (uint16_t) size);
#endif
    }
    else
    {
         /* Get a slot in the queue */
        i2cDataObj = _DRV_I2C_QueueSlotGet (dObj);

        if ( i2cDataObj != NULL )
        {
           /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
            i2cDataObj->slaveaddresslowbyte     = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_WRITE;
            i2cDataObj->txBuffer                = txBuffer;
            i2cDataObj->rxBuffer                = NULL;
            i2cDataObj->transferSize            = size;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = context;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
                if (PLIB_I2C_BusIsIdle(dObj->i2cId))
                {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                    _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(dObj->i2cId);
                }
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;

            }

            return (DRV_I2C_BUFFER_HANDLE) i2cDataObj;
        }
        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;     //GJV
} /* DRV_I2C_BufferAddWrite */

//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitThenReceive   (   DRV_HANDLE handle,
                                                            uint8_t* address,
                                                            void *txBuffer,
                                                            size_t writeSize,
                                                            void *rxBuffer,
                                                            size_t readSize,
                                                            void * callbackContext);

  Summary:
    This function writes data to Slave, inserts restart and requests read from
    slave.

  Description:
    Master calls this function to send a register address value to the slave and
    then queries the slave with a read request to read the contents indexed by
    the register location. The Master sends a restart condition after the
    initial write before sending the device address with R/W = 1. The restart
    condition prevents the Master from relinquishing the control of the bus. The
    slave should not use this function. Driver will process this request
    in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    address     - Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    txBuffer    - Contains data to be transferred
    writeSize   - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    rxBuffer    - This buffer holds data that is send back from slave after
                  read operation.
    readSize    - The number of bytes the Master expects to be read from the
                  slave
    callbackContext     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_TransmitThenReceive ( DRV_HANDLE handle,
                                                    uint16_t address,
                                                    void *writeBuffer,
                                                    size_t writeSize,
                                                    void *readBuffer,
                                                    size_t readSize,
                                                    void * callbackContext)
{
    DRV_I2C_BUFFER_OBJECT *i2cDataObj;
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE) DRV_I2C_MasterBufferWriteReadXE( (DRV_HANDLE) handle, address, (uint8_t*)writeBuffer, (uint16_t) writeSize, (uint8_t*) readBuffer, (uint16_t) readSize);
#endif
    }
    else
    {
        /* Get a slot in the queue */
        i2cDataObj = _DRV_I2C_QueueSlotGet (dObj);

        if ( i2cDataObj != NULL )
        {
            /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = address;
            i2cDataObj->slaveaddresslowbyte     = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_WRITE_READ;
            i2cDataObj->txBuffer                = writeBuffer;
            i2cDataObj->transferSize            = writeSize;
            i2cDataObj->rxBuffer                = readBuffer;
            i2cDataObj->readtransferSize        = readSize;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = callbackContext;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
                if (PLIB_I2C_BusIsIdle(dObj->i2cId))
                {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                    _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(dObj->i2cId);
                }
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;

            }
            return (DRV_I2C_BUFFER_HANDLE) i2cDataObj;
        }

        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;
} /* DRV_I2C_TransmitThenReceive */


//******************************************************************************
/* Function:
    DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWriteRead ( DRV_HANDLE handle,
                                                uint8_t* slaveaddress,
                                                void *txBuffer,
                                                size_t wsize,
                                                void *rxBuffer,
                                                size_t rsize,
                                                void * context);

  Summary:
    This function writes data to Slave, inserts restart and requests read from
    slave.

  Description:
    Master calls this function to send a register address value to the slave and
    then queries the slave with a read request to read the contents indexed by
    the register location. The Master sends a restart condition after the
    initial write before sending the device address with R/W = 1. The restart
    condition prevents the Master from relinquishing the control of the bus. The
    slave should not use this function. Driver will process this request
    in the task routine.

  Parameters:
    drvHandle   - A valid open-instance handle, returned from the driver's open
                  routine
    slaveaddress- Device address of slave. If this API is used in Slave mode,
                  then a dummy value can be used
    txBuffer    - Contains data to be transffered
    wsize       - The number of bytes that the Master expects to write to Slave.
                  This value can be kept as the MAX BUFFER SIZE for slave.
                  This is because the Master controls when the WRITE operation
                  is terminated.
    rxBuffer    - This buffer holds data that is send back from slave after
                  read operation.
    rsize       - The number of bytes the Master expects to be read from the
                  slave
    context     - Not implemented, future expansion

  Returns:
    DRV_I2C_BUFFER_HANDLE using which application can track the current status of
    the buffer.
*/

DRV_I2C_BUFFER_HANDLE DRV_I2C_BufferAddWriteRead ( DRV_HANDLE handle, uint8_t* deviceaddress,
                                    void *txBuffer,
                                    size_t wsize,
                                    void *rxBuffer,
                                    size_t rsize,
                                    void * context)
{
    DRV_I2C_BUFFER_OBJECT *i2cDataObj;
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( DRV_I2C_BIT_BANG == dObj->i2cPerph)
    {
//GJV
#ifdef  BB_ENABLED
        return (DRV_I2C_BUFFER_HANDLE) DRV_I2C_MasterBufferWriteRead( (DRV_HANDLE) handle, deviceaddress, (uint8_t*)txBuffer, (uint16_t) wsize, (uint8_t*) rxBuffer, (uint16_t) rsize);
#endif
    }
    else
    {
        /* Get a slot in the queue */
        i2cDataObj = _DRV_I2C_QueueSlotGet (dObj);

        if ( i2cDataObj != NULL )
        {
            /* Fill the data directly to the queue. Set the inUse flag only at the end */
            i2cDataObj->clientHandle            = handle;
            i2cDataObj->slaveaddresshighbyte    = *(deviceaddress);
            i2cDataObj->slaveaddresslowbyte     = 0x00;
            i2cDataObj->operation               = DRV_I2C_OP_WRITE_READ;
            i2cDataObj->txBuffer                = txBuffer;
            i2cDataObj->transferSize            = wsize;
            i2cDataObj->rxBuffer                = rxBuffer;
            i2cDataObj->readtransferSize        = rsize;
            i2cDataObj->actualtransfersize      = 0;
            i2cDataObj->status                  = DRV_I2C_BUFFER_EVENT_PENDING;
            i2cDataObj->context                 = context;

            _DRV_I2C_QueuePush( dObj, i2cDataObj);

            if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
            {
                if (PLIB_I2C_BusIsIdle(dObj->i2cId))
                {
                    SYS_INT_SourceStatusClear(dObj->mstrInterruptSource);
                    _DRV_I2C_InterruptSourceEnable( dObj->mstrInterruptSource ) ;
                    dObj->task = DRV_I2C_TASK_SEND_DEVICE_ADDRESS;
                    PLIB_I2C_MasterStart(dObj->i2cId);
                }
            }
            else
            {
                SYS_INT_SourceStatusClear(dObj->slaveInterruptSource);
                _DRV_I2C_InterruptSourceEnable( dObj->slaveInterruptSource ) ;

            }
            return (DRV_I2C_BUFFER_HANDLE) i2cDataObj;
        }

        return DRV_I2C_BUFFER_HANDLE_INVALID;
    }
    return DRV_I2C_BUFFER_HANDLE_INVALID;     //GJV
} /* DRV_I2C_BufferAddWrite */

// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_EVENT DRV_I2C_BufferStatus ( DRV_I2C_BUFFER_HANDLE bufferHandle )

  Summary:
    Returns the transmitter and receiver transfer status

  Description:
    This returns the transmitter and receiver transfer status.

  Parameters:
    handle          -   A valid open-instance handle, returned from the
                        driver's open routine
    bufferHandle    -   A valid buffer handle, returned when
                        calling the BufferAddRead/BufferAddWrite/
                        BufferAddReadWrite function

  Returns:
    A DRV_I2C_TRANSFER_STATUS value describing the current status of the
    transfer.
*/

DRV_I2C_BUFFER_EVENT DRV_I2C_TransferStatusGet ( DRV_HANDLE handle,
                                            DRV_I2C_BUFFER_HANDLE bufferHandle)
{
        /* return the transfer status. This doesn't have any protection */
        return _DRV_I2C_DATA_OBJ(bufferHandle, status);

} /* DRV_I2C_TransferStatusGet */

// *****************************************************************************
/* Function:
    DRV_I2C_BUFFER_EVENT DRV_I2C_BufferStatus ( DRV_I2C_BUFFER_HANDLE bufferHandle )

  Summary:
    Returns the transmitter and receiver transfer status

  Description:
    This returns the transmitter and receiver transfer status.

  Parameters:
    bufferHandle    -   A valid buffer handle, returned when
                        calling the BufferAddRead/BufferAddWrite/
                        BufferAddReadWrite function

  Returns:
    A DRV_I2C_TRANSFER_STATUS value describing the current status of the
    transfer.
*/

DRV_I2C_BUFFER_EVENT DRV_I2C_BufferStatus ( DRV_I2C_BUFFER_HANDLE bufferHandle )
{
        /* return the transfer status. This doesn't have any protection */
        return _DRV_I2C_DATA_OBJ(bufferHandle, status);

} /* DRV_I2C_TransferStatus */

// *****************************************************************************
/* Function:
    uint32_t DRV_I2C_BytesTransferred ( DRV_HANDLE handle,
                                            DRV_I2C_BUFFER_HANDLE bufferHandle )

  Summary:
    Returns the number of bytes transmitted or received in a particular I2C
    transaction. The transaction is identified by the handle.

  Description:
    This returns the transmitter and receiver transfer status.

  Parameters:

    handle      - A valid open-instance handle, returned from the driver's open
                  routine
    bufferHandle  -  A valid open-instance handle, returned when calling the
                     BufferAddRead/BufferAddWrite/BufferAddReadWrite function

  Returns:
    The number of bytes transferred in a particular I2C transaction.
*/

uint32_t DRV_I2C_BytesTransferred ( DRV_HANDLE handle,
                                    DRV_I2C_BUFFER_HANDLE bufferHandle )
{
    /* returns the number of bytes in an I2C transaction */

    return _DRV_I2C_DATA_OBJ(bufferHandle, actualtransfersize);

} /* DRV_I2C_BytesTransferred */




//******************************************************************************
/* Function:
    unsigned int DRV_I2C_VersionGet( const SYS_MODULE_INDEX drvIndex )

  Summary:
    Gets I2C driver version in numerical format.

  Description:
    This routine gets the I2C driver version. The version is encoded as
    major * 10000 + minor * 100 + patch. The stringized version can be obtained
    using DRV_I2C_VersionStrGet()

  Parameters:
    None.

  Returns:
    Current driver version in numerical format.
*/

unsigned int DRV_I2C_VersionGet( const SYS_MODULE_INDEX drvIndex )
{
    return( ( _DRV_I2C_VERSION_MAJOR * 10000 ) +
            ( _DRV_I2C_VERSION_MINOR * 100 ) +
            ( _DRV_I2C_VERSION_PATCH ) );

} /* DRV_I2C_VersionGet */


// *****************************************************************************
/* Function:
    char * DRV_I2C_VersionStrGet ( const SYS_MODULE_INDEX drvIndex )

  Summary:
    Gets I2C driver version in string format.

  Description:
    This routine gets the I2C driver version. The version is returned as
    major.minor.path[type], where type is optional. The numerical version can
    be obtained using DRV_I2C_VersionGet()

  Parameters:
    None.

  Returns:
    Current I2C driver version in the string format.

  Remarks:
    None.
*/

char * DRV_I2C_VersionStrGet( const SYS_MODULE_INDEX drvIndex )
{
    return _DRV_I2C_VERSION_STR;

} /* DRV_I2C_VersionStrGet */


// *****************************************************************************
/* Function:
    void DRV_I2C_Bus_Reset ( SYS_MODULE_OBJ object )

  Summary:
    Resets any slaves on the I2C bus which might be stuck in thier internal state
    machines.

  Description:
    This returns the transmitter and receiver transfer status.  From the I2C
    Specification:
    "If the data line (SDA) is stuck LOW, the master should send nine clock pulses.
    The device that held the bus LOW should release it sometime within those nine
    clocks. If not, then use the HW reset or cycle power to clear the bus."
    This function sends a Start signal, 9 clocks, and then a Stop singal.

  Parameters:
    object          - Identifies the Driver Object returned by the Initialize
                      interface

  Returns:

  Remarks:
    NOTE:   This is a blocking function.  It is NOT multi client friendly.  It
            should be used with caution.  It is meant to be used in a single
            Master environment where the PIC using it is the sole Master device
            on the I2C bus.  Under normal conditions, it should only be
            necessary during development.  Reseting the PIC32 without resetting
            slave devices on the I2C bus can sometimes cause them to be stuck
            in the middle of a reception or transmission.  This function can
            clear the bus in those cases.
*/
void DRV_I2C_Bus_Reset ( SYS_MODULE_OBJ object )
{
    uint32_t    starttime;
    DRV_I2C_OBJ *dObj = (DRV_I2C_OBJ*) object;
    if ( DRV_I2C_BIT_BANG != dObj->i2cPerph)
    {
        PLIB_I2C_Disable (dObj->i2cId);
    }

    /* Wait 50k BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

    // I2C Bus reset consists of an empty byte transfer.
    // That is,
    // a Start signal, Data Goes Low while Clock is High.
    // sequence of 9 clocks so that any slave state machine completes.
    // A Stop signal, Data Goes High while Clock is High.
    // Set Both pins High
    PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
    PLIB_PORTS_PinClear(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
    /* Wait 50k BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

    // Change SDA  to LOW  --  Start Signal.
    PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
    /* Wait 50k BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

    // Change SDA  to HIGH  --  Allows SDA to float so no data is sent from master.
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
    /* Wait 50k BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_0);

    int i;
    // sequence of 9 clocks so that any slave state machine completes.
    for(i=0;i<10;i++)
    {
        // Change SCL to LOW
        PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
        if(i == 9)
        {
            /* Wait 50k BRG */
            starttime = ReadCoreTimer();
            while(ReadCoreTimer()- starttime < BRG_1_TIME_0);
            PLIB_PORTS_PinDirectionOutputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
        }
        /* Wait 50k BRG */
        starttime = ReadCoreTimer();
        while(ReadCoreTimer()- starttime < BRG_1_TIME_0);
        // Change SCL to HIGH
        PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSCL, dObj->pinSCL);
        /* Wait 50k BRG */
        starttime = ReadCoreTimer();
        while(ReadCoreTimer()- starttime < BRG_1_TIME_0);
    }
    // After this sequence, the clock is high, and the data is low.

    // A Stop signal, Data Goes High while Clock is High.
    PLIB_PORTS_PinDirectionInputSet(PORTS_ID_0, dObj->portSDA, dObj->pinSDA);
    /* Wait 50k BRG */
    starttime = ReadCoreTimer();
    while(ReadCoreTimer()- starttime < BRG_1_TIME_0);
    // At this point both lines are high, and the Bus is idle.
    if ( DRV_I2C_BIT_BANG != dObj->i2cPerph)
    {
        PLIB_I2C_Enable (dObj->i2cId);
    }
}


// *****************************************************************************
/*
  Remarks:
    The functions DRV_I2C_StopEventSend, DRV_I2C_RestartEventSend and
    and DRV_I2C_IRQEventSend are deprecated but maintained for backward
    compatibility
*/

void DRV_I2C_StopEventSend(DRV_HANDLE handle)
{
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;
    if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
    {
        PLIB_I2C_MasterStop (dObj->i2cId);
    }
}

void DRV_I2C_RestartEventSend(DRV_HANDLE handle)
{
    DRV_I2C_OBJ *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_MASTER)
    {
        PLIB_I2C_MasterStartRepeat (dObj->i2cId);
    }
}

void DRV_I2C_IRQEventSend(DRV_HANDLE handle)
{
    DRV_I2C_CLIENT_OBJ *clientObj = (DRV_I2C_CLIENT_OBJ*) handle;

    DRV_I2C_OBJ  *dObj = ((DRV_I2C_CLIENT_OBJ *) handle)->driverObject;

    if ( (_DRV_I2C_OPERATION_MODE_GET(dObj->i2cMode)) == DRV_I2C_MODE_SLAVE)
    {
        PLIB_PORTS_PinClear(PORTS_ID_0, clientObj->irqSelectPort, clientObj->irqSelectBitPos);
        dObj->modulemainstate = DRV_I2C_SLAVE_READY_TO_TX_TO_MASTER;
    }
}


/*******************************************************************************
End of File
*/



