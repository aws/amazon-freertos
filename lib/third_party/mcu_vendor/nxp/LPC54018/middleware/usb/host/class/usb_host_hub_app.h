/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_HOST_HUB_APP_H_
#define _USB_HOST_HUB_APP_H_

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief HUB reset times*/
#define USB_HOST_HUB_PORT_RESET_TIMES (1)

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
/*! @brief HUB Control transcation retry times for remote wakeup*/
#define USB_HOST_HUB_REMOTE_WAKEUP_TIMES (3U)
#endif

/*! @brief HUB application global structure */
typedef struct _usb_host_hub_global
{
    usb_host_handle hostHandle;          /*!< This HUB list belong to this host*/
    usb_host_hub_instance_t *hubProcess; /*!< HUB in processing*/
    usb_host_hub_instance_t *hubList;    /*!< host's HUB list*/
    usb_osa_mutex_handle hubMutex;       /*!< HUB mutex*/
} usb_host_hub_global_t;

/*! @brief HUB application status */
typedef enum _usb_host_hub_app_status
{
    kHubRunIdle = 0,         /*!< Idle */
    kHubRunInvalid,          /*!< Invalid state */
    kHubRunWaitSetInterface, /*!< Wait callback of set interface */
    kHubRunGetDescriptor7,   /*!< Get 7 bytes HUB descriptor */
    kHubRunGetDescriptor,    /*!< Get all HUB descriptor */
    kHubRunSetPortPower,     /*!< Set HUB's port power */
    kHubRunGetStatusDone,    /*!< HUB status changed */
    kHubRunClearDone,        /*!< clear HUB feature callback */
} usb_host_hub_app_status_t;

/*! @brief HUB port application status */
typedef enum _usb_host_port_app_status
{
    kPortRunIdle = 0,             /*!< Idle */
    kPortRunInvalid,              /*!< Invalid state */
    kPortRunWaitPortChange,       /*!< Wait port status change */
    kPortRunCheckCPortConnection, /*!< Check C_PORT_CONNECTION */
    kPortRunGetPortConnection,    /*!< Get port status data */
    kPortRunCheckPortConnection,  /*!< Check PORT_CONNECTION   */
    kPortRunWaitPortResetDone,    /*!< Wait port reset transfer done */
    kPortRunWaitCPortReset,       /*!< Wait C_PORT_RESET */
    KPortRunCheckCPortReset,      /*!< Check C_PORT_RESET */
    kPortRunResetAgain,           /*!< Reset port again */
    kPortRunPortAttached,         /*!< Device is attached on the port */
    kPortRunCheckPortDetach,      /*!< Check port is detached */
    kPortRunGetConnectionBit,     /*!< Get the port status data */
    kPortRunCheckConnectionBit,   /*!< Check PORT_CONNECTION */
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    kPortRunClearCPortSuspend, /*!< Clear C_PORT_SUSPEND */
    kPortRunCheckPortSuspend,  /*!< Check PORT_SUSPEND */
    kPortRunPortSuspended,     /*!< Port is suspended */
#endif
} usb_host_port_app_status_t;

/*! @brief HUB data prime status */
typedef enum _usb_host_hub_prime_status
{
    kPrimeNone = 0,    /*!< Don't prime data*/
    kPrimeHubControl,  /*!< Prime HUB control transfer*/
    kPrimePortControl, /*!< Prime port control transfer*/
    kPrimeInterrupt,   /*!< Prime interrupt transfer*/
} usb_host_hub_prime_status_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* USB_HOST_CONFIG_HUB */

#endif /* _USB_HOST_HUB_APP_H_ */
