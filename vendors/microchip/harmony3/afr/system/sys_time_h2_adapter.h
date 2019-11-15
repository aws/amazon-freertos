/*******************************************************************************
  Adapter Time System Service Library Interface Header File

  Company
    Microchip Technology Inc.

  File Name
    sys_time_h2_adapter.h

  Summary
    Time system service library interface.

  Description
    This is the adapter file defines the interface to the Time system service 
    library from H2. The System Time service APIs are used to provide an adapter 
    routines which are part of H2.

  Remarks:
    This interface will be extended in the future to utilize in TCPIP module
    to provide time in milliseconds and seconds.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software 
and any derivatives exclusively with Microchip products. It is your 
responsibility to comply with third party license terms applicable to your 
use of third party software (including open source software) that may 
accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*****************************************************************************/

// DOM-IGNORE-END

#ifndef SYS_TIME_ADAPTER_H    // Guards against multiple inclusion
#define SYS_TIME_ADAPTER_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section lists the other files that are included in this file.
*/

#include <stdint.h>
#include <stdbool.h>
#include "system/system.h"
#include "system/time/sys_time.h"
#include "system/time/sys_time_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/*  The following data type definitions are used by the functions in this
    interface and should be considered part it.
*/
	//Timer COUNT to TICK conversion for tickless implementation : 150,000,000 for 1sec. Configure tick conversion for 1 msec.
#define SYS_COUNT_TICK_CONV         150000

   typedef uintptr_t SYS_TMR_HANDLE;
   typedef void ( * SYS_TMR_CALLBACK ) ( uintptr_t context, uint32_t currTick );
   #define SYS_TMR_HANDLE_INVALID          ( ( uintptr_t ) -1 )

   typedef struct{
	   SYS_TMR_CALLBACK   callback;
	}SYS_TIME_H2_ADAPTER_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: System Interface Functions
// *****************************************************************************
// *****************************************************************************
/*  System interface functions are called by system code to initialize the
    module and maintain proper operation of it.
*/

// *****************************************************************************
/* Function:
   SYS_STATUS SYS_TMR_Status ( SYS_MODULE_OBJ object )

Summary:
Returns status of the specific module instance of the Timer module.

Description:
This function returns the status of the specific module instance disabling its
operation (and any hardware for driver modules).

PreCondition:
The SYS_TMR_Initialize function should have been called before calling
this function.

Parameters:
object          - SYS TMR object handle, returned from SYS_TMR_Initialize

Returns:
SYS_STATUS_READY    Indicates that any previous module operation for the
specified module has completed

SYS_STATUS_BUSY     Indicates that a previous module operation for the
specified module has not yet completed

SYS_STATUS_ERROR    Indicates that the specified module is in an error state
*/

static __inline__ SYS_STATUS __attribute__((always_inline)) SYS_TMR_Status ( SYS_MODULE_OBJ object )
{
   return SYS_TIME_Status(object);
}
// *****************************************************************************
/* Function:
    SYS_STATUS SYS_TMR_ModuleStatusGet ( const SYS_MODULE_INDEX index )

  Summary:
    Returns status of the specific instance of the Timer module.

  Description:
    This function returns the status of the specific module instance.
    It uses the index of the instance rather than an object handle.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    object          - SYS TMR object handle, returned from SYS_TMR_Initialize

  Returns:
    - SYS_STATUS_READY          - Indicates that the service initialization is
    							  complete and the timer is ready to be used.
    - SYS_STATUS_ERROR          - Indicates that the timer service is in an error state
                                  Note: Any value less than SYS_STATUS_ERROR is
                                  also an error state.
    - SYS_MODULE_UNINITIALIZED  - Indicates that the timer service has been deinitialized

  Example:
    <code>

    SYS_STATUS tmrStatus = SYS_TMR_ModuleStatusGet (SYS_TMR_INDEX_0);
	
    if ( SYS_STATUS_ERROR <= tmrStatus )
    {
        // Handle error
    }

    </code>

  Remarks:
    None.
*/

static __inline__ SYS_STATUS __attribute__((always_inline))  SYS_TMR_ModuleStatusGet ( const SYS_MODULE_INDEX index )
{
	return SYS_STATUS_READY; // blindly returning status Ready!
}
// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_TickCountGet( void )

  Summary:
    Provides the current counter value.

  Description:
    This function provides the current tick counter value.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer tick count value.

  Example:
    <code>
    uint32_t count;
    count = SYS_TMR_TickCountGet ();
    </code>

  Remarks:
    None.
*/

static __inline__ uint32_t __attribute__((always_inline)) SYS_TMR_TickCountGet(void)
{
	return (uint32_t)(SYS_TIME_Counter64Get()/SYS_COUNT_TICK_CONV);
}

// *****************************************************************************
/* Function:
    uint64_t SYS_TMR_TickCountGetLong( void )

  Summary:
    Provides the current counter value with 64-bit resolution.

  Description:
    This function provides the current tick counter value as a 64-bit value.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer tick count value.

  Example:
    <code>
    uint64_t lCount;
    lCount = SYS_TMR_TickCountGetLong ();
    </code>

  Remarks:
    None.
*/
static __inline__ uint64_t __attribute__((always_inline)) SYS_TMR_TickCountGetLong(void)
{
	return SYS_TIME_Counter64Get()/SYS_COUNT_TICK_CONV;
}
// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_TickCounterFrequencyGet ( void )

  Summary:
    Provides the number of ticks per second

  Description:
    This function provides the system tick resolution,
    the number of ticks per second

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer ticks per second.

  Example:
    <code>
    uint32_t tickFrequencyHz;
 
    tickFrequencyHz = SYS_TMR_TickCounterFrequencyGet ( );
    </code>

  Remarks:
    None.
*/

static __inline__ uint32_t __attribute__((always_inline)) SYS_TMR_TickCounterFrequencyGet ( void )
{
	return SYS_TIME_FrequencyGet()/SYS_COUNT_TICK_CONV;
}


// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_SystemCountFrequencyGet( void )

  Summary:
    Provides the current system count frequency.

  Description:
    This function provides the current system count frequency in Hz.
    The tick count measurement is based on this value.
    This is the resolution of the system time base.
   
  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system frequency value in Hertz.

  Example:
    <code>
    uint32_t sysCountFreq;
    sysCountFreq = SYS_TMR_SystemCountFrequencyGet ();
    </code>

  Remarks:
    None.
*/

static __inline__ uint32_t __attribute__((always_inline)) SYS_TMR_SystemCountFrequencyGet( void )
{
	return SYS_TIME_FrequencyGet();
}


// *****************************************************************************
/* Function:
    uint64_t SYS_TMR_SystemCountGet( void )

  Summary:
    Provides the current system time count value.

  Description:
    This function provides the current system time count value with the
    greatest possible resolution.
    It is the number of counts from the system start up as counted
    with the system count frequency (SYS_TMR_SystemCountFrequencyGet).

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system count value.

  Example:
    <code>
    uint64_t sysCount;
    sysCount = SYS_TMR_SystemCountGet ();
    // the system has been running for sysCount/SYS_TMR_SystemCountFrequencyGet seconds
    </code>

  Remarks:
    None.
*/

static __inline__ uint64_t __attribute__((always_inline)) SYS_TMR_SystemCountGet(void)
{
	return SYS_TIME_Counter64Get();
}
// *****************************************************************************
/* Function:
    SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, 
	                                          SYS_TMR_CALLBACK   callback )

  Summary:
    Creates a permanent timer object that calls back to the client periodically.

  Description:
    This function creates a timer object that will continuously notify the client
    once the time-out occurs.
    The timer object will be deleted by a call to SYS_TMR_CallbackStop
   
  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    periodMs    - Periodic delay in milliseconds
	context     - A client parameter that's passed in the callback function.
                  This will help to identify the callback.
    callback    - Pointer to a callback routine that will be called periodically
                  once the time-out occurs.


  Returns:
    A valid timer object handle of type SYS_TMR_HANDLE if the call succeeds.
    SYS_TMR_HANDLE_INVALID otherwise.
 
  Example:
    <code>
    SYS_TMR_HANDLE handle20ms, handle30ms;
   
    void Test_Callback ( uintptr_t context, uint32_t currTick );

    handle20ms = SYS_TMR_CallbackPeriodic ( 20, 1, Test_Callback );
	handle30ms = SYS_TMR_CallbackPeriodic ( 20, 2, Test_Callback );
	
	void Test_Callback ( uintptr_t context, uint32_t currTick )
	{
		if ( context == 1 )
		{
			//20 ms
		}
		else
		{
			//30 ms
		}
	}
    </code>

  Remarks:
    The created timer object is persistent and needs to be deleted with
    SYS_TMR_CallbackStop when no longer needed.

    This function is equivalent to
    SYS_TMR_ObjectCreate(periodMs, context, callback, SYS_TMR_FLAG_PERIODIC);

*/
/*
static __inline__ SYS_TIME_HANDLE __attribute__((always_inline)) SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, void * callback )
{
	return SYS_TIME_CallbackRegisterMS((SYS_TIME_CALLBACK)sy_time_h2_adapter_callback, context, periodMs, SYS_TIME_PERIODIC );
}*/
SYS_TMR_HANDLE SYS_TMR_CallbackPeriodic ( uint32_t periodMs, uintptr_t context, 
                                          SYS_TMR_CALLBACK   callback );
// *****************************************************************************
/* Function:
    void SYS_TMR_CallbackStop ( SYS_TMR_HANDLE handle )

  Summary:
    Stops a periodic timer object.

  Description:
    This function deletes a previously created periodic timer object.
    All the associated resources are released.

  Precondition:
    The SYS_TMR_CallbackPeriodic function should have been called to obtain a valid 
	timer handle.

  Parameters:
    handle - A valid periodic timer handle, returned by a SYS_TMR_CallbackPeriodic call.

  Returns:
    None.

  Example:
    <code>
	int count, state;
	void Test_Callback ( uintptr_t context, uint32_t currTick )
	{
		count++;
	}

	void DelayTask ( )
	{
		SYS_TMR_HANDLE handle;
		switch (state)
		{
			case 1:
				handle = SYS_TMR_CallbackPeriodic ( 20, 1, Test_Callback );
				state++;
				break;
			case 2:
				if ( count > 100 )
				{
					SYS_TMR_CallbackStop ( handle );
					state++;
				}
				count++;
				break;
		}
	}
    </code>

  Remarks:
    This function is equivalent to SYS_TMR_ObjectDelete(handle);.
    
*/

static __inline__ void  __attribute__((always_inline)) SYS_TMR_CallbackStop ( SYS_TMR_HANDLE handle )
{
	SYS_TIME_TimerDestroy(handle);
}

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END

#endif //SYS_TIME_ADAPTER_H
