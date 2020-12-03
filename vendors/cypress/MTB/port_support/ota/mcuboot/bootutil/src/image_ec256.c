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

#ifdef MCUBOOT_SIGN_EC256
#include "bootutil/sign_key.h"

#include "mbedtls/oid.h"
#include "mbedtls/asn1.h"

#ifdef MCUBOOT_USE_TINYCRYPT
#include "tinycrypt/ecc_dsa.h"
#endif
#ifdef MCUBOOT_USE_CC310
#include "cc310_glue.h"
#define NUM_ECC_BYTES (4*8)
#endif
#include "bootutil_priv.h"

/*
 * Declaring these like this adds NULL termination.
 */
static const uint8_t ec_pubkey_oid[] = MBEDTLS_OID_EC_ALG_UNRESTRICTED;
static const uint8_t ec_secp256r1_oid[] = MBEDTLS_OID_EC_GRP_SECP256R1;

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

    /* ECParameters (RFC5480) */
    if (mbedtls_asn1_get_alg(cp, end, &alg, &param)) {
        return -2;
    }
    /* id-ecPublicKey (RFC5480) */
    if (alg.len != sizeof(ec_pubkey_oid) - 1 ||
        memcmp(alg.p, ec_pubkey_oid, sizeof(ec_pubkey_oid) - 1)) {
        return -3;
    }
    /* namedCurve (RFC5480) */
    if (param.len != sizeof(ec_secp256r1_oid) - 1 ||
        memcmp(param.p, ec_secp256r1_oid, sizeof(ec_secp256r1_oid) - 1)) {
        return -4;
    }
    /* ECPoint (RFC5480) */
    if (mbedtls_asn1_get_bitstring_null(cp, end, &len)) {
        return -6;
    }
    if (*cp + len != end) {
        return -7;
    }

    if (len != 2 * NUM_ECC_BYTES + 1) {
        return -8;
    }
    /* Is uncompressed? */
    if (*cp[0] != 0x04) {
        return -9;
    }

    (*cp)++;
    return 0;
}

/*
 * cp points to ASN1 string containing an integer.
 * Verify the tag, and that the length is 32 bytes.
 */
static int
bootutil_read_bigint(uint8_t i[NUM_ECC_BYTES], uint8_t **cp, uint8_t *end)
{
    size_t len;

    if (mbedtls_asn1_get_tag(cp, end, &len, MBEDTLS_ASN1_INTEGER)) {
        return -3;
    }

    if (len >= NUM_ECC_BYTES) {
        memcpy(i, *cp + len - NUM_ECC_BYTES, NUM_ECC_BYTES);
    } else {
        memset(i, 0, NUM_ECC_BYTES - len);
        memcpy(i + NUM_ECC_BYTES - len, *cp, len);
    }
    *cp += len;
    return 0;
}

/*
 * Read in signature. Signature has r and s encoded as integers.
 */
static int
bootutil_decode_sig(uint8_t signature[NUM_ECC_BYTES * 2], uint8_t *cp, uint8_t *end)
{
    int rc;
    size_t len;

    rc = mbedtls_asn1_get_tag(&cp, end, &len,
                              MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return -1;
    }
    if (cp + len > end) {
        return -2;
    }

    rc = bootutil_read_bigint(signature, &cp, end);
    if (rc) {
        return -3;
    }
    rc = bootutil_read_bigint(signature + NUM_ECC_BYTES, &cp, end);
    if (rc) {
        return -4;
    }
    return 0;
}

#ifdef MCUBOOT_USE_TINYCRYPT
int
bootutil_verify_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig, size_t slen,
  uint8_t key_id)
{
    int rc;
    uint8_t *pubkey;
    uint8_t *end;

    uint8_t signature[2 * NUM_ECC_BYTES];

    pubkey = (uint8_t *)bootutil_keys[key_id].key;
    end = pubkey + *bootutil_keys[key_id].len;

    rc = bootutil_import_key(&pubkey, end);
    if (rc) {
        return -1;
    }

    rc = bootutil_decode_sig(signature, sig, sig + slen);
    if (rc) {
        return -1;
    }

    /*
     * This is simplified, as the hash length is also 32 bytes.
     */
    if (hlen != NUM_ECC_BYTES) {
        return -1;
    }

    rc = uECC_verify(pubkey, hash, NUM_ECC_BYTES, signature, uECC_secp256r1());
    if (rc == 1) {
        return 0;
    } else {
        return -2;
    }
}
#endif /* MCUBOOT_USE_TINYCRYPT */
#ifdef MCUBOOT_USE_CC310
int
bootutil_verify_sig(uint8_t *hash,
                    uint32_t hlen,
                    uint8_t *sig,
                    size_t slen,
                    uint8_t key_id)
{
    int rc;
    uint8_t *pubkey;
    uint8_t *end;
    uint8_t signature[2 * NUM_ECC_BYTES];

    pubkey = (uint8_t *)bootutil_keys[key_id].key;
    end = pubkey + *bootutil_keys[key_id].len;

    rc = bootutil_import_key(&pubkey, end);
    if (rc) {
        return -1;
    }

    /* Decode signature */
    rc = bootutil_decode_sig(signature, sig, sig + slen);
    if (rc) {
        return -1;
    }

    /*
     * This is simplified, as the hash length is also 32 bytes.
     */
    if (hlen != NUM_ECC_BYTES) {
        return -1;
    }

    /* Initialize and verify in one go */
    rc = cc310_ecdsa_verify_secp256r1(hash, pubkey, signature, hlen);

    if (rc != 0) {
        return -2;
    }

    return rc;
}
#endif /* MCUBOOT_USE_CC310 */
#endif /* MCUBOOT_SIGN_EC256 */
