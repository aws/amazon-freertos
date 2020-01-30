/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file pal_os_lock.c
*
* \brief   This file implements the platform abstraction layer APIs for os locks (e.g. semaphore).
*
* \ingroup  grPAL
* @{
*/

#include "optiga/pal/pal_os_lock.h"

/**
 * @brief PAL OS lock structure. Might be extended if needed
 */
typedef struct pal_os_lock
{
    uint8_t lock;
} pal_os_lock_t;

volatile static pal_os_lock_t pal_os_lock = {.lock = 0};

pal_status_t pal_os_lock_acquire(void)
{
    pal_status_t return_status = PAL_STATUS_FAILURE;

    if(!(pal_os_lock.lock))
    {
    	pal_os_lock.lock++;
        if(pal_os_lock.lock != 1)
        {
        	pal_os_lock.lock--;
        }
        return_status = PAL_STATUS_SUCCESS;
    }
    return return_status;
}

void pal_os_lock_release(void)
{
    if(pal_os_lock.lock)
    {
    	pal_os_lock.lock--;
    }
}

/**
* @}
*/
