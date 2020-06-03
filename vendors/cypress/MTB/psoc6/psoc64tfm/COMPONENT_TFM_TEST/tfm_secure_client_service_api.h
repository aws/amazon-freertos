/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SECURE_CLIENT_SERVICE_API_H__
#define __TFM_SECURE_CLIENT_SERVICE_API_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Runs the secure integration tests.
 *
 * \return Returns 0 on success
 */
int32_t tfm_secure_client_run_tests(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SECURE_CLIENT_SERVICE_API_H__ */
