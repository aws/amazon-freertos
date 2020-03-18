/*
 * Sample code used for reference only
 * TBD : Public URL for CY RTOS repo
 */

/******************************************************
*             Function definitions
******************************************************/

cy_rslt_t cy_rtos_create_thread(cy_thread_t *thread, cy_thread_entry_fn_t entry_function,
                                const char *name, void *stack, uint32_t stack_size, cy_thread_priority_t priority,
                                cy_thread_arg_t arg)
{
    osThreadAttr_t atr;

    atr.name = name;
    atr.attr_bits = osThreadDetached;
    atr.priority = (osPriority_t)priority;
    atr.stack_mem = stack;
    atr.stack_size = stack_size;
    atr.cb_mem = NULL;
    atr.cb_size = 0;
    atr.tz_module = 0;
    atr.reserved = 0;

    *thread = osThreadNew( (osThreadFunc_t)entry_function, (void *)arg, &atr );

    if (*thread == NULL)
    {
    }
    return (*thread == NULL) ? CY_RTOS_NO_MEMORY : CY_RSLT_SUCCESS;
}

/**
 * Exit the current thread
 *
 * @returns WHD_SUCCESS on success or otherwise
 */
cy_rslt_t cy_rtos_exit_thread(void)
{
    osThreadExit();
    return CY_RSLT_SUCCESS;
}

/**
 * Terminates the current thread
 *
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns CY_RSLT_SUCCESS on success, error otherwise
 */
cy_rslt_t cy_rtos_terminate_thread(cy_thread_t *thread)
{
    osStatus_t status;

    if (*thread == NULL)
        return CY_RTOS_BAD_PARAM;

    status = osThreadTerminate(*thread);                         // stop the thread
    *thread = NULL;

    if (status == osOK)
        return CY_RSLT_SUCCESS;
    else
        return CY_RTOS_GENERAL_ERROR;
}

/**
 * Blocks the current thread until the indicated thread is complete
 *
 * @param thread         : handle of the thread to terminate
 *
 * @returns CY_RSLT_SUCCESS on success, error otherwise
 */
cy_rslt_t cy_rtos_join_thread(cy_thread_t *thread)
{
    osStatus_t status;
    if (*thread == NULL)
        return CY_RTOS_BAD_PARAM;

    status = osThreadJoin(*thread);

    if (status == osOK)
        return CY_RSLT_SUCCESS;
    else
        return CY_RTOS_GENERAL_ERROR;
}

/**
 * Creates a semaphore
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns CY_RSLT_SUCCESS on success, error otherwise
 */
cy_rslt_t cy_rtos_init_semaphore(cy_semaphore_t *semaphore, uint32_t maxcount, uint32_t initcount)
{
    *semaphore = osSemaphoreNew(1, 0, NULL);
    if (*semaphore == NULL)
    {
        return CY_RTOS_GENERAL_ERROR;
    }
    return CY_RSLT_SUCCESS;
}

/**
 * Gets a semaphore
 *
 * If value of semaphore is larger than zero, then the semaphore is decremented and function returns
 * Else If value of semaphore is zero, then current thread is suspended until semaphore is set.
 * Value of semaphore should never be below zero
 *
 * Must not be called from interrupt context, since it could block, and since an interrupt is not a
 * normal thread, so could cause RTOS problems if it tries to suspend it.
 * If called from interrupt context time out value should be 0.
 *
 * @param semaphore   : Pointer to variable which will receive handle of created semaphore
 * @param timeout_ms  : Maximum period to block for. Can be passed CY_RTOS_NEVER_TIMEOUT to request no timeout
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 */
cy_rslt_t cy_rtos_get_semaphore(cy_semaphore_t *semaphore, uint32_t timeout_ms,
                                bool will_set_in_isr)
{
    osStatus_t result;
    will_set_in_isr = will_set_in_isr;

    if (*semaphore == NULL)
    {
        return CY_RTOS_GENERAL_ERROR;
    }

    if (timeout_ms == CY_RTOS_NEVER_TIMEOUT)
        result = osSemaphoreAcquire(*semaphore, osWaitForever);
    else
        result = osSemaphoreAcquire(*semaphore, timeout_ms * 1000 / 1000);

    if (result == osOK)
    {
        return CY_RSLT_SUCCESS;
    }
    else if (result == osErrorTimeout)
    {
        return CY_RTOS_TIMEOUT;
    }

    return CY_RTOS_GENERAL_ERROR;
}

/**
 * Sets a semaphore
 *
 * If any threads are waiting on the semaphore, the first thread is resumed
 * Else increment semaphore.
 *
 * Can be called from interrupt context, so must be able to handle resuming other
 * threads from interrupt context.
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param called_from_ISR : Value of WHD_TRUE indicates calling from interrupt context
 *                          Value of WHD_FALSE indicates calling from normal thread context
 *
 * @return cy_rslt_t : CY_RSLT_SUCCESS if semaphore was successfully set
 *                        : error if an error occurred
 *
 */
cy_rslt_t cy_rtos_set_semaphore(cy_semaphore_t *semaphore, bool called_from_ISR)
{
    osStatus_t result;
    called_from_ISR = called_from_ISR;

    if (*semaphore == NULL)
    {
        return CY_RTOS_GENERAL_ERROR;
    }

    result = osSemaphoreRelease(*semaphore);

    if ( (result == osOK) || (result == osErrorResource) )
    {
        return CY_RSLT_SUCCESS;
    }

    else if (result == osErrorParameter)
    {
        return CY_RTOS_GENERAL_ERROR;
    }
    return CY_RTOS_GENERAL_ERROR;
}

/**
 * Deletes a semaphore
 *
 * WHD uses this function to delete a semaphore.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return cy_rslt_t : CY_RSLT_SUCCESS if semaphore was successfully deleted
 *                        : error if an error occurred
 *
 */
cy_rslt_t cy_rtos_deinit_semaphore(cy_semaphore_t *semaphore)
{
    osStatus_t result;
    if (*semaphore == NULL)
    {
        return CY_RTOS_GENERAL_ERROR;
    }
    result = osSemaphoreDelete(*semaphore);

    if (result != osOK)
        return CY_RTOS_GENERAL_ERROR;

    return CY_RSLT_SUCCESS;
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
cy_rslt_t cy_rtos_get_time(cy_time_t *tval)
{
    /* Get Number of ticks per second */
    uint32_t tick_freq = osKernelGetTickFreq();

    /* Convert ticks count to time in milliseconds */
    *tval = (osKernelGetTickCount() * (1000 / tick_freq) );

    return CY_RSLT_SUCCESS;
}

/**
 * Delay for a number of milliseconds
 *
 * @return cy_rslt_t : CY_RSLT_SUCCESS if delay was successful
 *                        : error if an error occurred
 *
 */
cy_rslt_t cy_rtos_delay_milliseconds(cy_time_t num_ms)
{
    osStatus_t result;
    result = osDelay( (uint32_t )num_ms );

    if (result == osOK)
    {
        return CY_RSLT_SUCCESS;
    }
    else if (result == osErrorISR)
    {
        return CY_RTOS_GENERAL_ERROR;
    }
    else if (result == osErrorParameter)
    {
        return CY_RTOS_BAD_PARAM;
    }
    else if (result == osErrorNoMemory)
    {
        return CY_RTOS_GENERAL_ERROR;
    }
    return CY_RTOS_GENERAL_ERROR;
}

