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

/*! \file critical_error.h
 * \brief Critical Error handler module
 *
 * This is used for handling critical error conditions.
 * The default action can be overridden by applications.
 */

#ifndef _CRITICAL_ERROR_H_
#define _CRITICAL_ERROR_H_
#include <wmerrno.h>

/** Types of critical errors. A negation of the below values is passed
 * to critical_error() handler.
 */
typedef enum {
	/** WLAN initialization failed. */
	CRIT_ERR_WLAN_INIT_FAILED = MOD_ERROR_START(MOD_CRIT_ERR),
	/** Application Framework initialization failed. */
	CRIT_ERR_APP_FRAMEWORK_INIT_FAILED,
	/** Critical error occurred in Application Framework. */
	CRIT_ERR_APP_FRAMEWORK,
	/** Hard fault occurred	*/
	CRIT_ERR_HARD_FAULT,
	/** Last entry. Applications can define their own error numbers
	 * after this.
	 */
	CRIT_ERR_LAST,
} critical_errno_t;

/** Critical error handler
 *
 * This function handles critical errors. Applications can define
 * their own function with the below name and signature to override
 * the default behavior.
 * Default behaviour is to just print the error message (if console
 * is enabled) and stall.
 *
 * \param[in] crit_errno The error number of type \ref critical_errno_t.
 * Note that a negation of the error number is passed here.
 * \param[in] data Pointer to any data associated with the error.
 * This will be valid only if it is explicitly mentioned in the
 * documentation of the particular error.
 */
void critical_error(int crit_errno, void *data);

/** Get critical error message
 *
 * This is a convenience API that can be used inside a critical error
 * handler. The critical error number in itself is not very user friendly.
 * This API converts in into a user friendly message.
 *
 * \param[in] crit_errno The critical error number. This should be the same
 * value as received by critical_error()
 *
 * \return A user friendly string associated with the critical error.
 */
char *critical_error_msg(int crit_errno);
#endif /* _CRITICAL_ERROR_H_ */
