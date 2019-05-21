/** @file wmassert.h
*
*  @brief This file contains Macros and function declarations for assert
*
*  (C) Copyright 2015-2018 Marvell International Ltd. All Rights Reserved
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

#ifndef _WM_ASSERT_H_
#define _WM_ASSERT_H_

#ifdef __linux__
#include <assert.h>

#ifdef CONFIG_ENABLE_ASSERTS
#define ASSERT assert
#else
#define ASSERT(...)
#endif /* CONFIG_ENABLE_ASSERTS */

#else /* ! __linux__ */

#ifdef CONFIG_ENABLE_ASSERTS
#define ASSERT(_cond_) if(!(_cond_)) \
		_wm_assert(__FILE__, __LINE__, #_cond_)
#else /* ! CONFIG_ENABLE_ASSERTS */
#define ASSERT(_cond_)
#endif /* CONFIG_ENABLE_ASSERTS */

void _wm_assert(const char *filename, int lineno, 
	    const char* fail_cond);

#endif /* __linux__ */


#endif // _WM_ASSERT_H_
