/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_ASSERT_H__
#define __TFM_ASSERT_H__

#include "tfm_log_raw.h"

/* Functions and macros in this file is for 'thread mode' usage. */

#define TFM_ASSERT(cond)                                                    \
            do {                                                            \
                if (!(cond)) {                                              \
                    tfm_log_printf("Assert:%s:%d", __FUNCTION__, __LINE__); \
                    while (1)                                               \
                        ;                                                   \
                }                                                           \
            } while (0)

#endif /* __TFM_ASSERT_H__ */
