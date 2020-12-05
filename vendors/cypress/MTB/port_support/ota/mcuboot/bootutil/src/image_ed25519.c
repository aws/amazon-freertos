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

#include <string.h>

#include "mcuboot_config/mcuboot_config.h"

#ifdef MCUBOOT_SIGN_ED25519
#include "bootutil/sign_key.h"

#include "mbedtls/oid.h"
#include "mbedtls/asn1.h"

#include "bootutil_priv.h"

static const uint8_t ed25519_pubkey_oid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG "\x65\x70";
#define NUM_ED25519_BYTES 32

extern int ED25519_verify(const uint8_t *message, size_t message_len,
                          const uint8_t signature[64],
                          const uint8_t public_key[32]);

/*
 * Parse the public key used for signing.
 */
static int
bootutil_import_key(uint8_t **cp, uint8_t *end)
{
    size_t len;
    mbedtls_asn1_buf alg;
    mbedtls_asn1_buf param;

    if (mbedtls_asn1_get_tag(cp, end, &len,
        MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) {
        return -1;
    }
    end = *cp + len;

    if (mbedtls_asn1_get_alg(cp, end, &alg, &param)) {
        return -2;
    }

    if (alg.len != sizeof(ed25519_pubkey_oid) - 1 ||
        memcmp(alg.p, ed25519_pubkey_oid, sizeof(ed25519_pubkey_oid) - 1)) {
        return -3;
    }

    if (mbedtls_asn1_get_bitstring_null(cp, end, &len)) {
        return -4;
    }
    if (*cp + len != end) {
        return -5;
    }

    if (len != NUM_ED25519_BYTES) {
        return -6;
    }

    return 0;
}

int
bootutil_verify_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig, size_t slen,
  uint8_t key_id)
{
    int rc;
    uint8_t *pubkey;
    uint8_t *end;

    if (hlen != 32 || slen != 64) {
        return -1;
    }

    pubkey = (uint8_t *)bootutil_keys[key_id].key;
    end = pubkey + *bootutil_keys[key_id].len;

    rc = bootutil_import_key(&pubkey, end);
    if (rc) {
        return -1;
    }

    rc = ED25519_verify(hash, 32, sig, pubkey);
    if (rc == 0) {
        return -2;
    }

    return 0;
}

#endif /* MCUBOOT_SIGN_ED25519 */
