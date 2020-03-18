/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_RAW_H__
#define __TFM_LOG_RAW_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Prints log messages
 *
 * \param[in]   fmt     Formatted string
 * \param[in]   ...     Viriable length argument
 *
 * \return              Number of chars printed
 *
 * \note                This function has the similar input argument format as
 *                      the 'printf' function. But it supports only some basic
 *                      formats like 'sdicpuxX' and '%'. It will output
 *                      "[Unsupported Tag]" when none of the above formats match
 *
 * \details             The following output formats are supported.
 *                      %s - string
 *                      %d - decimal signed integer (same for %i)
 *                      %u - decimal unsigned integer
 *                      %x - hex in lowercase
 *                      %X - hex in uppercase
 *                      %p - hex address of a pointer in lowercase
 *                      %c - character
 *                      %% - the '%' symbol
 */
int tfm_log_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LOG_RAW_H__ */
