/** \file
 *  \brief Timer Utility Functions for Linux
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "atca_hal.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


/** \brief This function delays for a number of microseconds.
 *
 * \param[in] delay number of microseconds to delay
 */
void hal_delay_us(uint32_t delay)
{
    usleep(delay);
}

/** \brief This function delays for a number of tens of microseconds.
 *
 * \param[in] delay number of 0.01 milliseconds to delay
 */
void hal_delay_10us(uint32_t delay)
{
    hal_delay_us(delay * 10);
}


/** \brief This function delays for a number of milliseconds.
 *
 *         You can override this function if you like to do
 *         something else in your system while delaying.
 * \param[in] delay number of milliseconds to delay
 */

/* ASF already has delay_ms - see delay.h */
void hal_delay_ms(uint32_t delay)
{
    hal_delay_us(delay * 1000);
}

#ifndef ATCA_USE_RTOS_TIMER
#if ATCA_USE_SHARED_MUTEX

typedef struct
{
    pthread_mutex_t mutex;      /** Must be first*/
    bool            shared;
} hal_mutex_t;

/**
 * \brief Application callback for creating a mutex object
 * \param[IN/OUT] ppMutex location to receive ptr to mutex
 * \param[IN/OUT] name String used to identify the mutex
 */
ATCA_STATUS hal_create_mutex(void ** ppMutex, char* pName)
{
    int fd;
    bool created = false;

    if (!ppMutex)
    {
        return ATCA_BAD_PARAM;
    }

    if (pName)
    {
        /* Set up a shared memory region */
        fd = shm_open(pName, O_RDWR | O_CREAT | O_EXCL, 0666);
        if (0 > fd)
        {
            if (EEXIST == errno)
            {
                fd = shm_open(pName, O_RDWR, 0666);
            }
        }
        else
        {
            if (0 > ftruncate(fd, sizeof(hal_mutex_t)))
            {
                close(fd);
                fd = -1;
            }
            created = true;
        }

        if (0 > fd)
        {
            return ATCA_GEN_FAIL;
        }
        else
        {
            *ppMutex = mmap(NULL, sizeof(hal_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
        }
    }
    else
    {
        *ppMutex = malloc(sizeof(hal_mutex_t));
        created = true;
    }

    if (created && *ppMutex)
    {
        pthread_mutexattr_t muattr;
        pthread_mutexattr_init(&muattr);
        pthread_mutexattr_settype(&muattr, PTHREAD_MUTEX_ERRORCHECK);
#ifdef PTHREAD_PRIO_INHERIT
        pthread_mutexattr_setprotocol(&muattr, PTHREAD_PRIO_INHERIT);
#endif
        pthread_mutexattr_setrobust(&muattr, PTHREAD_MUTEX_ROBUST);
        if (pName)
        {
            pthread_mutexattr_setpshared(&muattr, PTHREAD_PROCESS_SHARED);
            ((hal_mutex_t*)*ppMutex)->shared = 1;
        }

        if (pthread_mutex_init(*ppMutex, &muattr))
        {
            if (pName)
            {
                munmap(*ppMutex, sizeof(hal_mutex_t));
            }
            else
            {
                free(*ppMutex);
            }
            *ppMutex = NULL;
            return ATCA_GEN_FAIL;
        }
    }

    if (!*ppMutex)
    {
        return ATCA_GEN_FAIL;
    }

    return ATCA_SUCCESS;
}

/*
 * \brief Application callback for destroying a mutex object
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_destroy_mutex(void * pMutex)
{
    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    if (((hal_mutex_t*)pMutex)->shared)
    {
        return munmap(pMutex, sizeof(hal_mutex_t)) ? ATCA_GEN_FAIL : ATCA_SUCCESS;
    }
    else
    {
        pthread_mutex_destroy(pMutex);
        free(pMutex);
        return ATCA_SUCCESS;
    }
}


/*
 * \brief Application callback for locking a mutex
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_lock_mutex(void * pMutex)
{
    int rv;

    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    rv = pthread_mutex_lock(pMutex);

    if (!rv || EDEADLK == rv)
    {
        return ATCA_SUCCESS;
    }
    else if (EOWNERDEAD == rv)
    {
        /* Lock was obtained but its because another process terminated so the
           state is indeterminate and will probably need to be fixed */
        pthread_mutex_consistent(pMutex);
        return ATCA_SUCCESS;
    }
    else
    {
        return ATCA_GEN_FAIL;
    }
}

/*
 * \brief Application callback for unlocking a mutex
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_unlock_mutex(void * pMutex)
{
    if (!pMutex)
    {
        return ATCA_BAD_PARAM;
    }

    return pthread_mutex_unlock(pMutex) ? ATCA_GEN_FAIL : ATCA_SUCCESS;
}

#else

#include <semaphore.h>

/**
 * \brief Application callback for creating a mutex object
 * \param[in,out] ppMutex location to receive ptr to mutex
 * \param[in,out] pName String used to identify the mutex
 */
ATCA_STATUS hal_create_mutex(void ** ppMutex, char* pName)
{
    sem_t * sem;

    if (!ppMutex)
    {
        return ATCA_BAD_PARAM;
    }

    if (!pName)
    {
        pName = "atca_mutex";
    }

    sem = sem_open(pName, (O_CREAT | O_RDWR), (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 0);
    if (SEM_FAILED == sem)
    {
        return ATCA_GEN_FAIL;
    }

    *ppMutex = sem;

    return ATCA_SUCCESS;
}

/*
 * \brief Application callback for destroying a mutex object
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_destroy_mutex(void * pMutex)
{
    sem_t * sem = (sem_t*)pMutex;

    if (!sem)
    {
        return ATCA_BAD_PARAM;
    }

    if (-1 == sem_close(sem))
    {
        return ATCA_GEN_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}


/*
 * \brief Application callback for locking a mutex
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_lock_mutex(void * pMutex)
{
    sem_t * sem = (sem_t*)pMutex;

    if (!sem)
    {
        return ATCA_BAD_PARAM;
    }

    if (-1 == sem_wait(sem))
    {
        return ATCA_GEN_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}

/*
 * \brief Application callback for unlocking a mutex
 * \param[IN] pMutex pointer to mutex
 */
ATCA_STATUS hal_unlock_mutex(void * pMutex)
{
    sem_t * sem = (sem_t*)pMutex;

    if (!sem)
    {
        return ATCA_BAD_PARAM;
    }

    if (-1 == sem_post(sem))
    {
        return ATCA_GEN_FAIL;
    }
    else
    {
        return ATCA_SUCCESS;
    }
}
#endif
#endif
/** @} */
