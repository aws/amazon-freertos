typedef struct tskTaskControlBlock 			/* The old naming convention is used to prevent breaking kernel aware debuggers. */
{
	volatile StackType_t	*pxTopOfStack;	/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

	#if ( portUSING_MPU_WRAPPERS == 1 )
		xMPU_SETTINGS	xMPUSettings;		/*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
	#endif

	ListItem_t			xStateListItem;	/*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
	ListItem_t			xEventListItem;		/*< Used to reference a task from an event list. */
	UBaseType_t			uxPriority;			/*< The priority of the task.  0 is the lowest priority. */
	StackType_t			*pxStack;			/*< Points to the start of the stack. */
	char				pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

	#if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
		StackType_t		*pxEndOfStack;		/*< Points to the highest valid address for the stack. */
	#endif

	#if ( portCRITICAL_NESTING_IN_TCB == 1 )
		UBaseType_t		uxCriticalNesting;	/*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
	#endif

	#if ( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t		uxTCBNumber;		/*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
		UBaseType_t		uxTaskNumber;		/*< Stores a number specifically for use by third party trace code. */
	#endif

	#if ( configUSE_MUTEXES == 1 )
		UBaseType_t		uxBasePriority;		/*< The priority last assigned to the task - used by the priority inheritance mechanism. */
		UBaseType_t		uxMutexesHeld;
	#endif

	#if ( configUSE_APPLICATION_TASK_TAG == 1 )
		TaskHookFunction_t pxTaskTag;
	#endif

	#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
		void			*pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
	#endif

	#if( configGENERATE_RUN_TIME_STATS == 1 )
		uint32_t		ulRunTimeCounter;	/*< Stores the amount of time the task has spent in the Running state. */
	#endif

	#if ( configUSE_NEWLIB_REENTRANT == 1 )
		/* Allocate a Newlib reent structure that is specific to this task.
		Note Newlib support has been included by popular demand, but is not
		used by the FreeRTOS maintainers themselves.  FreeRTOS is not
		responsible for resulting newlib operation.  User must be familiar with
		newlib and must provide system-wide implementations of the necessary
		stubs. Be warned that (at the time of writing) the current newlib design
		implements a system-wide malloc() that must be provided with locks. */
		struct	_reent xNewLib_reent;
	#endif

	#if( configUSE_TASK_NOTIFICATIONS == 1 )
		volatile uint32_t ulNotifiedValue;
		volatile uint8_t ucNotifyState;
	#endif

	/* See the comments in FreeRTOS.h with the definition of
	tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
	#if( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 ) /*lint !e731 !e9029 Macro has been consolidated for readability reasons. */
		uint8_t	ucStaticallyAllocated; 		/*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
	#endif

	#if( INCLUDE_xTaskAbortDelay == 1 )
		uint8_t ucDelayAborted;
	#endif

	#if( configUSE_POSIX_ERRNO == 1 )
		int iTaskErrno;
	#endif

} tskTCB;

typedef tskTCB TCB_t;

extern void prvInitialiseTaskLists( void );

extern PRIVILEGED_DATA TCB_t * pxCurrentTCB;

/* Lists for ready and blocked tasks. --------------------
xDelayedTaskList1 and xDelayedTaskList2 could be move to function scople but
doing so breaks some kernel aware debuggers and debuggers that rely on removing
the static qualifier. */
extern PRIVILEGED_DATA List_t pxReadyTasksLists[ configMAX_PRIORITIES ];/*< Prioritised ready tasks. */
extern PRIVILEGED_DATA List_t xDelayedTaskList1;						/*< Delayed tasks. */
extern PRIVILEGED_DATA List_t xDelayedTaskList2;						/*< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
extern PRIVILEGED_DATA List_t * pxDelayedTaskList;				/*< Points to the delayed task list currently being used. */
extern PRIVILEGED_DATA List_t * pxOverflowDelayedTaskList;		/*< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
extern PRIVILEGED_DATA List_t xPendingReadyList;						/*< Tasks that have been readied while the scheduler was suspended.  They will be moved to the ready list when the scheduler is resumed. */

#if( INCLUDE_vTaskDelete == 1 )
	extern PRIVILEGED_DATA List_t xTasksWaitingTermination;				/*< Tasks that have been deleted - but their memory not yet freed. */
	extern PRIVILEGED_DATA volatile UBaseType_t uxDeletedTasksWaitingCleanUp;
#endif

#if ( INCLUDE_vTaskSuspend == 1 )
	extern PRIVILEGED_DATA List_t xSuspendedTaskList;					/*< Tasks that are currently suspended. */
#endif

/* Global POSIX errno. Its value is changed upon context switching to match
the errno of the currently running task. */
#if ( configUSE_POSIX_ERRNO == 1 )
	extern int FreeRTOS_errno;
#endif

/* Other file private variables. --------------------------------*/
extern PRIVILEGED_DATA volatile UBaseType_t uxCurrentNumberOfTasks;
extern PRIVILEGED_DATA volatile TickType_t xTickCount;
extern PRIVILEGED_DATA volatile UBaseType_t uxTopReadyPriority;
extern PRIVILEGED_DATA volatile BaseType_t xSchedulerRunning;
extern PRIVILEGED_DATA volatile UBaseType_t uxPendedTicks;
extern PRIVILEGED_DATA volatile BaseType_t xYieldPending;
extern PRIVILEGED_DATA volatile BaseType_t xNumOfOverflows;
extern PRIVILEGED_DATA UBaseType_t uxTaskNumber;
extern PRIVILEGED_DATA volatile TickType_t xNextTaskUnblockTime; /* Initialised to portMAX_DELAY before the scheduler starts. */
extern PRIVILEGED_DATA TaskHandle_t xIdleTaskHandle;			/*< Holds the handle of the idle task.  The idle task is created automatically when the scheduler is started. */

/* Context switches are held pending while the scheduler is suspended.  Also,
interrupts must not manipulate the xStateListItem of a TCB, or any of the
lists the xStateListItem can be referenced from, if the scheduler is suspended.
If an interrupt needs to unblock a task while the scheduler is suspended then it
moves the task's event list item into the xPendingReadyList, ready for the
kernel to move the task from the pending ready list into the real ready list
when the scheduler is unsuspended.  The pending ready list itself can only be
accessed from a critical section. */
extern PRIVILEGED_DATA volatile UBaseType_t uxSchedulerSuspended;
