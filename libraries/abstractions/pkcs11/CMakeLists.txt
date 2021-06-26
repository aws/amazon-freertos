######################################################
# PKCS #11 Utils Module
######################################################

afr_module(NAME pkcs11_utils)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source/include")

afr_module_sources(
    pkcs11_utils
    PRIVATE
        "${src_dir}/core_pki_utils.c"
        "${inc_dir}/core_pki_utils.h"
)

afr_module_include_dirs(
    pkcs11_utils
    PUBLIC "${inc_dir}"
)


######################################################
# PKCS #11 Wrapper Module
######################################################
afr_module(NAME pkcs11 )

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source")
set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source/include")

afr_module_sources(
    pkcs11
    PRIVATE
        "${inc_dir}/core_pkcs11.h"
        "${src_dir}/core_pkcs11.c"
)

afr_module_include_dirs(
    pkcs11
    PUBLIC
        "${inc_dir}"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PRIVATE
        AFR::pkcs11_implementation
    PUBLIC
        3rdparty::pkcs11
        AFR::common
)
######################################################

######################################################
# PKCS #11 Implementation Module
######################################################
afr_module(NAME pkcs11_implementation)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_include_dirs(
    pkcs11_implementation
    PUBLIC "${inc_dir}"
    # Requires standard/common/include/private/aws_default_root_certificates.h
    PRIVATE "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
)

afr_module_dependencies(
    pkcs11_implementation
    PUBLIC
        3rdparty::pkcs11
    PRIVATE
        AFR::pkcs11_implementation::mcu_port
        AFR::pkcs11_utils
        AFR::pkcs11
)

if(ECC608_IN_USE)
    set(mchp_dir "${AFR_VENDORS_DIR}/microchip")
    set(cryptoauthlib_dir "${mchp_dir}/cryptoauthlib")
    set(pkcs11_cryptoauthlib_dir "${CMAKE_CURRENT_LIST_DIR}/cryptoauthlib")
    afr_module(NAME pkcs11_ecc608a INTERFACE)

    set(cryptoauthlib_include_dirs
        "${cryptoauthlib_dir}/lib"
        "${cryptoauthlib_dir}/lib/pkcs11"
        "${cryptoauthlib_dir}/lib/atcacert"
        "${cryptoauthlib_dir}/lib/hal"
        "${cryptoauthlib_dir}/lib/crypto"
        "${cryptoauthlib_dir}/lib/calib"
        "${pkcs11_cryptoauthlib_dir}"
        )

    set(ecc608_src_list
        ${cryptoauthlib_dir}/lib/atca_basic.c
        ${cryptoauthlib_dir}/lib/atca_debug.c
        ${cryptoauthlib_dir}/lib/atca_device.c
        ${cryptoauthlib_dir}/lib/atca_helpers.c
        ${cryptoauthlib_dir}/lib/atca_iface.c
        ${cryptoauthlib_dir}/lib/atca_utils_sizes.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_client.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_date.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_def.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_der.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_host_hw.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_host_sw.c
        ${cryptoauthlib_dir}/lib/atcacert/atcacert_pem.c
        ${cryptoauthlib_dir}/lib/calib/calib_aes.c
        ${cryptoauthlib_dir}/lib/calib/calib_aes_gcm.c
        ${cryptoauthlib_dir}/lib/calib/calib_basic.c
        ${cryptoauthlib_dir}/lib/calib/calib_checkmac.c
        ${cryptoauthlib_dir}/lib/calib/calib_command.c
        ${cryptoauthlib_dir}/lib/calib/calib_counter.c
        ${cryptoauthlib_dir}/lib/calib/calib_derivekey.c
        ${cryptoauthlib_dir}/lib/calib/calib_ecdh.c
        ${cryptoauthlib_dir}/lib/calib/calib_execution.c
        ${cryptoauthlib_dir}/lib/calib/calib_gendig.c
        ${cryptoauthlib_dir}/lib/calib/calib_genkey.c
        ${cryptoauthlib_dir}/lib/calib/calib_hmac.c
        ${cryptoauthlib_dir}/lib/calib/calib_info.c
        ${cryptoauthlib_dir}/lib/calib/calib_helpers.c
        ${cryptoauthlib_dir}/lib/calib/calib_kdf.c
        ${cryptoauthlib_dir}/lib/calib/calib_lock.c
        ${cryptoauthlib_dir}/lib/calib/calib_mac.c
        ${cryptoauthlib_dir}/lib/calib/calib_nonce.c
        ${cryptoauthlib_dir}/lib/calib/calib_privwrite.c
        ${cryptoauthlib_dir}/lib/calib/calib_random.c
        ${cryptoauthlib_dir}/lib/calib/calib_read.c
        ${cryptoauthlib_dir}/lib/calib/calib_secureboot.c
        ${cryptoauthlib_dir}/lib/calib/calib_selftest.c
        ${cryptoauthlib_dir}/lib/calib/calib_sha.c
        ${cryptoauthlib_dir}/lib/calib/calib_sign.c
        ${cryptoauthlib_dir}/lib/calib/calib_updateextra.c
        ${cryptoauthlib_dir}/lib/calib/calib_verify.c
        ${cryptoauthlib_dir}/lib/calib/calib_write.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_hw_aes_cbc.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_hw_aes_cbcmac.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_hw_aes_ccm.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_hw_aes_cmac.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_hw_aes_ctr.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_sw_ecdsa.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_sw_rand.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_sw_sha1.c
        ${cryptoauthlib_dir}/lib/crypto/atca_crypto_sw_sha2.c
        ${cryptoauthlib_dir}/lib/crypto/hashes/sha2_routines.c
        ${cryptoauthlib_dir}/lib/crypto/hashes/sha1_routines.c
        ${cryptoauthlib_dir}/lib/host/atca_host.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_attrib.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_cert.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_config.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_debug.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_digest.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_find.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_info.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_init.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_key.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_main.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_mech.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_object.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_os.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_session.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_signature.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_slot.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_token.c
        ${cryptoauthlib_dir}/lib/pkcs11/pkcs11_util.c
    )

    afr_module_sources(
        pkcs11_ecc608a
        INTERFACE
            "${pkcs11_cryptoauthlib_dir}/atca_cert_chain.h"
            "${pkcs11_cryptoauthlib_dir}/atca_cert_chain.c"
            "${pkcs11_cryptoauthlib_dir}/core_pkcs11_secure_element.c"
            "${ecc608_src_list}"
    )

    afr_module_include_dirs(
        pkcs11_ecc608a
        INTERFACE
            "${inc_dir}"
            "${cryptoauthlib_include_dirs}"
            "${AFR_MODULES_ABSTRACTIONS_DIR}/pkcs11/cryptoauthlib"
    )

    afr_module_dependencies(
        pkcs11_ecc608a
        INTERFACE
            AFR::crypto
            AFR::pkcs11_implementation
    )
elseif(AFR_PKCS11_PSA)
    afr_module(NAME pkcs11_psa INTERFACE)
    set(src_dir "${CMAKE_CURRENT_LIST_DIR}/psa")

    afr_module_sources(
        pkcs11_psa
        INTERFACE
            "${src_dir}/iot_pkcs11_psa.c"
            "${src_dir}/iot_pkcs11_psa_input_format.c"
            "${src_dir}/iot_pkcs11_psa_input_format.h"
            "${src_dir}/iot_pkcs11_psa_object_management.c"
            "${src_dir}/iot_pkcs11_psa_object_management.h"
    )

    afr_module_include_dirs(
        pkcs11_psa
        INTERFACE
            "${scr_dir}"
    )

    afr_module_dependencies(
        pkcs11_psa
        INTERFACE
            AFR::pkcs11_implementation
            AFR::crypto
            3rdparty::mbedtls
    )
else()
    afr_module(NAME pkcs11_mbedtls INTERFACE)
    set(src_dir "${CMAKE_CURRENT_LIST_DIR}/corePKCS11/source/portable/mbedtls")

    afr_module_sources(
        pkcs11_mbedtls
        INTERFACE
            "${src_dir}/core_pkcs11_mbedtls.c"
            "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls_config/threading_alt.h"
            "${inc_dir}/core_pkcs11_pal.h"
    )

    afr_module_dependencies(
        pkcs11_mbedtls
        INTERFACE
            AFR::pkcs11_implementation
            3rdparty::mbedtls
            AFR::crypto
    )
endif()

# PKCS11 test
afr_test_module()
if(${AFR_BOARD} STREQUAL "pc.windows")
    set(mbt_tests
        "${test_dir}/iot_test_pkcs11_globals.h"
        "${test_dir}/MBT_C_CloseSession.c"
        "${test_dir}/MBT_C_DigestFinal.c"
        "${test_dir}/MBT_C_DigestInit.c"
        "${test_dir}/MBT_C_DigestUpdate.c"
        "${test_dir}/MBT_C_Finalize.c"
        "${test_dir}/MBT_C_FindObjects.c"
        "${test_dir}/MBT_C_FindObjectsFinal.c"
        "${test_dir}/MBT_C_FindObjectsInit.c"
        "${test_dir}/MBT_C_GenerateKeyPair.c"
        "${test_dir}/MBT_C_GenerateRandom.c"
        "${test_dir}/MBT_C_GetAttributeValue.c"
        "${test_dir}/MBT_C_Initialize.c"
        "${test_dir}/MBT_C_OpenSession.c"
        "${test_dir}/MBT_C_Sign.c"
        "${test_dir}/MBT_C_SignInit.c"
        "${test_dir}/MBT_C_Verify.c"
        "${test_dir}/MBT_C_VerifyInit.c"
        "${test_dir}/MBT_DigestMachine.c"
        "${test_dir}/MBT_GenerationMachine.c"
        "${test_dir}/MBT_ObjectMachine.c"
        "${test_dir}/MBT_SessionMachine.c"
        "${test_dir}/MBT_SignMachine.c"
        "${test_dir}/MBT_VerifyMachine.c"
    )
endif()

afr_module_sources(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        "${test_dir}/iot_test_pkcs11.c"
        "${mbt_tests}"
)
afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    INTERFACE
        AFR::pkcs11
        AFR::crypto
        AFR::pkcs11_utils
)
