/**
 * \file mbedtls_user_config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef MBEDTLS_USER_CONFIG_H
#define MBEDTLS_USER_CONFIG_H

/* MPI / BIGNUM options. */
/*
 *  This value is sufficient for handling 2048 bit RSA keys.
 *
 *  Set this value higher to enable handling larger keys, but be aware that this
 *  will increase the stack usage.
 */
#define MBEDTLS_MPI_WINDOW_SIZE    1    /**< Maximum windows size used. */
#define MBEDTLS_MPI_MAX_SIZE       512  /**< Maximum number of bytes for usable MPIs. */

/* SSL options. */
/* Below configuration could be enabled as while Server & client support TLS ext. */
#if defined( MBEDTLS_SSL_MAX_CONTENT_LEN )
    #undef MBEDTLS_SSL_MAX_CONTENT_LEN
#endif
#define MBEDTLS_SSL_MAX_CONTENT_LEN    ( 5345 )          /**< Maximum fragment length in bytes, determines the size of each of the two internal I/O buffers. */

#define MBEDTLS_ECP_WINDOW_SIZE        2

#endif /* MBEDTLS_USER_CONFIG_H */
