/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/*! \file work-queue.h
 * \brief Work Queue
 *
 * Work queues are software entities provided as a service to the
 * application layer to simplify the task of programming new
 * features. Whenever simple to moderate tasks are do be done in thread
 * context either once or periodically, work queues are a good
 * solution. They free up the application code for the tasks of creating
 * new thread and managing synchronization. In many use-cases, the new
 * application thread will be active for small portion of the time slice
 * and be idle otherwise. However, it still blocks up valuable RAM and
 * other system resources. Work queue can help here by sharing the thread
 * context including its stack among many jobs.
 *
 * The basic concept is to have a queue of jobs. This queue elements
 * i.e. jobs are executed one after another by a single thread. Additional
 * feature provided by work-queue is to have the options of automatically
 * running a job periodically. This feature is of immense benefit
 * w.r.t. optimum use of resources. One example will prove the
 * point. Consider the task is to blink an LED periodically every 500
 * mS. Without work-queue you will have two options. First, you create a
 * new thread and toggle the GPIO every 500 mS. The thread sleeps in between
 * for 500 mS. Second, you add a timer to call you back every 500 mS and
 * you toggle the LED in this callback. The first option works but for the
 * trivial task of changing the state of a GPIO, a separate thread is
 * present while sleeps for most of the time. Thus, the resources like
 * stack memory blocked by the thread are used inefficiently. The second
 * option will blink the LED well but may be harmful for other timer
 * objects. A timer thread runs has very high priority and the LED toggle
 * logic which involves access to the slow speed GPIO peripheral, starves
 * off other critical threads from access to CPU.
 *
 * A work queue can help here by doing this task of toggling the GPIO in a
 * normal priority thread (if so configured), periodically. The 500 mS
 * periodic execution is also handled automatically.
 */
#ifndef __WORK_QUEUE_H__
#define __WORK_QUEUE_H__

#include <wmtypes.h>
#include <wmerrno.h>

/** Worker thread priority default value */
#define DEFAULT_WORKER_PRIO OS_PRIO_3
/** Worker thread stack size default value */
#define DEFAULT_STACK_SIZE 1024
/** Default ISR jobs reserve */
#define DEFAULT_ISR_JOBS_RESERVE 2

/**
 * Work Queue Configuration data structure.
 * It holds priority of the thread created
 * and stack size fo the thread which serves queued jobs.
 */
typedef struct {
	/**  Thread priority, if not sure set \ref DEFAULT_WORKER_PRIO*/
	int worker_priority;
	/**  Thread stack size, if not sure set \ref
	     DEFAULT_STACK_SIZE. This size should be decided based stack
	     usage characteristics of jobs */
	int worker_stack_bytes;
	/** Number of simultaneous ISR jobs expected. ISR jobs are jobs
	    which are enqueued from interrupt context. The standard API
	    work_enqueue() automatically handles this scenario. If not sure
	    set to DEFAULT_ISR_JOBS_RESERVE. */
	int worker_isr_jobs_reserve;
} wq_cfg_t;

typedef int wq_handle_t;
typedef int job_handle_t;

/**
 * A callback function executed as part of a job. This function is the
 * application function containing the application logic. The return value
 * of this function can be passed on to the user when the user uses the
 * work_enqueue_and_wait() instead of work_enqueue().
 */
typedef int (*job_func_t) (void *param);

/** Length of owner string (including NULL) passed in \ref wq_job_t */
#define MAX_OWNER_NAME_LEN 5

/**
 * Data structure for Job queued in work queue. Work queue keeps its own
 * copy of this structure. Caller may re-use/destroy its copy after
 * work_enqueue() returns.
 */
typedef struct {
	/**
	 * Name of the job owner. Empty string can be passed but note that
	 * caller will not be able to the API use
	 * work_dequeue_owner_all(). If passed ensure that its length
	 * including NULL terminator does not exceed MAX_OWNER_NAME_LEN.
	 */
	char owner[MAX_OWNER_NAME_LEN];
	/** Function called when job is scheduled */
	job_func_t job_func;
	/** Private user pointer. This will be returned back as your job
	 * parameter */
	void *param;
	/**
	 * If this has a non-zero value the job will run periodically at every
	 * 'periodic_ms' milliseconds until work_dequeue() is called.
	 */
	uint32_t periodic_ms;
	/** This is initial delay before job is run for first time*/
	uint32_t initial_delay_ms;
} wq_job_t;

/**
 * Initializes a work queue.
 *
 * A work queue is a queue of jobs assigned to a single worker thread of
 * execution. The thread associated with the queue will service any jobs
 * pending on the queue.
 *
 * The worker thread by default is created during queue init and stays
 * alive till the queue is de-init'ed. It sleeps when no jobs are present.
 *
 * A job is basically just a function (and its callees) with execute in the
 * worker thread context. Being a thread context a job can use mutexes,
 * semaphores and all remaining activities that a normal thread can do. The
 * only thing to keep in mind while designing jobs is to be aware that
 * other jobs also run in the same context and they will be starved of the
 * CPU if a job hogs the CPU or sleeps for a long time.
 *
 * @param[in] wq_cfg Work queue configuration parameters \ref wq_cfg_t. If
 * NULL is passed default values are used. The default values are
 * \ref DEFAULT_WORKER_PRIO and \ref DEFAULT_STACK_SIZE.
 * @param[out] wq_handle Handle returned after the work queue is initialized
 * successfully.
 *
 * @return WM_SUCCESS Work queue initialization successfully.
 * @return -WM_E_INVAL if invalid arguments given.
 * @return -WM_E_NOMEM if heap allocation failed.
 * @return -WM_FAIL for any other failure.
 */
int work_queue_init(const wq_cfg_t *wq_cfg, wq_handle_t *wq_handle);

/**
 * Add a job to the given work queue.
 *
 * A job is a data structure containing the callback function pointer and some
 * other relevant parameters. When a job is enqueued the callback function
 * passed will be executed some time in near future by the worker thread
 * associated with the queue.
 *
 * @note This function can also be invoked from ISR context.
 * @note If the job is not periodic it will be run only once.
 * @note This function can be invoked from callback function i.e. a job can
 * enque itself or any other job.
 *
 * @param[in] wq_handle The handle returned by work_queue_init().
 * @param[in] wq_job The structure containing the callback function and
 * relevant parameters. A copy of this structure is maintained internally
 * by work queue and this is not necessary to keep it after call to
 * work_enqueue() returns.
 * @param[out] job_handle Will be populated by work_enqueue(). This handle is
 * needed as input parameter to other work queue functions like
 * work_dequeue(). Pass NULL if handle is not needed.
 *
 * @return WM_SUCCESS if enque success.
 * @return -WM_E_INVAL if invalid params are passed.
 * @return -WM_E_NOMEM if memory allocation failure.
 * @return -WM_E_NOSPC If job could not be enqueued from ISR as no
 * preallocated object was found.
 * @return -WM_FAIL if any other error is seen.
 */
int work_enqueue(wq_handle_t wq_handle, const wq_job_t *wq_job,
	       job_handle_t *job_handle);

/**
 * Add a job to the given work queue and wait till it is complete
 *
 * A job is a data structure containing the callback function pointer and
 * some other relevant parameters. When a job is enqueued the callback
 * function passed will be executed some time in near future by the worker
 * thread associated with the queue. Till the job is executed and it
 * returns the caller thread will be blocked.
 *
 * The main difference between work_enqueue() and work_enqueue_and_wait()
 * is that the latter enables the user to wait till a job is complete and
 * additionally also provides the status value returned by the job. This is
 * same as making a function call except that the said function runs in
 * worker thread context.
 *
 * @note This function should not be invoked from ISR context.
 * @note The job added through this function call should not be
 * periodic. It should be a single execution job.
 * @note Ensure that this function is not invoked from a job running in the
 * same worker thread context which will cause a deadlock.
 *
 * @param[in] wq_handle The handle returned by work_queue_init().
 * @param[in] wq_job The structure containing the callback function and
 * relevant parameters.
 * @param[in] timeout_ms This function will block for the given millisecs
 * before giving up waiting for the job to complete. Keep this high enough
 * to cover for time spent by the job in waiting and also time spent by CPU
 * in doing other tasks. Setting it to OS_WAIT_FOREVER will give infinite
 * timeout. Setting it to OS_NO_WAIT will give zero timeout which is same
 * as calling work_enqueue() instead of work_enqueue_and_wait().
 * @param[out] ret_status Pass this if you need the return value of the
 * job. Pass NULL if the return value is not needed.
 *
 * @return WM_SUCCESS if enque success.
 * @return -WM_E_INVAL if invalid params are passed.
 * @return -WM_E_NOMEM if memory allocation failure.
 * @return -WM_FAIL if any other error is seen.
 */
int work_enqueue_and_wait(wq_handle_t wq_handle, const wq_job_t *wq_job,
			  unsigned long timeout_ms, int *ret_status);

/** Query if any job is queued for a particular owner
 *
 * This API can be used to check if any job has been queued for any
 * particular owner
 *
 * \param[in] wq_handle The handle returned by work_queue_init().
 * \param[in] owner Pointer to the owner string
 *
 * \return true If a job for the owner is in queue
 * \return false If no job for the owner is queued
 */
bool is_owner_job_queued(wq_handle_t wq_handle, const char *owner);

/**
 * Remove a job from the queue.
 *
 * A job currently in the queue is removed by this function. If the job is
 * already executing it is marked for deletion and removed once it finishes
 * execution. If there are more than one jobs with the same callback
 * address all of them are removed.
 *
 * @param[in] wq_handle The handle returned by work_queue_init().
 * @param[in] job_handle The handle populated by work_enqueue().
 *
 * @return WM_SUCCESS if enqueue success.
 * @return -WM_E_NOENT if no job is found associated with given
 * handle. This could be if job is already completed.
 * @return -WM_FAIL if any other error is seen.
 * @return -WM_E_INVAL if invalid arguments given.
 */
int work_dequeue(wq_handle_t wq_handle, job_handle_t *job_handle);

/**
 * Deque all jobs belonging to an owner.
 *
 * All jobs enqueued by a user (owner) can be de-queued in one go.
 *
 * @param[in] wq_handle The handle returned by work_queue_init().
 * @param[in] owner The owner name originally passed in owner field of \ref
 * wq_job_t
 *
 * @return WM_SUCCESS if enqueue success.
 * @return -WM_E_INVAL if invalid arguments given.
 */
int work_dequeue_owner_all(wq_handle_t wq_handle, const char *owner);

/**
 * De-initialize a work queue.
 *
 * After finishing the current job if any, the worker thread and the queue
 * is deleted.
 *
 * @param[in] wq_handle Pointer to the handle returned by
 * work_queue_init(). This will be set to NULL by this function.
 */
void work_queue_deinit(wq_handle_t *wq_handle);


#endif /* __WORK_QUEUE_H__ */
