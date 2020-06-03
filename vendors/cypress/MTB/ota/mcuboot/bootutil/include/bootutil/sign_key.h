/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __BOOTUTIL_SIGN_KEY_H_
#define __BOOTUTIL_SIGN_KEY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bootutil_key {
    const uint8_t *key;
    const unsigned int *len;
};

extern const struct bootutil_key bootutil_keys[];
extern const int bootutil_key_cnt;

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_SIGN_KEY_H_ */
