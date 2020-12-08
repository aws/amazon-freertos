/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __INTEG_TEST_H__
#define __INTEG_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Executes integration tests. To be called from appropriate
 *        secure/non-secure service client.
 */
void start_integ_test(void);

/**
 * \brief Service stand-in shim for non secure tests. To be called from a
 *        non-secure client.
 */
void tfm_non_secure_client_run_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __INTEG_TEST_H__ */
