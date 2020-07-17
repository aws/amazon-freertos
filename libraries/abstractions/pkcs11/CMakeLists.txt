if(AFR_ENABLE_UNIT_TESTS)
    add_subdirectory(utest)
    return()
endif()
afr_module(NAME pkcs11_implementation)

set(inc_dir "${CMAKE_CURRENT_LIST_DIR}/include")
set(test_dir "${CMAKE_CURRENT_LIST_DIR}/test")

afr_module_include_dirs(
    ${AFR_CURRENT_MODULE}
    PUBLIC "${inc_dir}"
    # Requires standard/common/include/private/aws_default_root_certificates.h
    PRIVATE "${AFR_MODULES_C_SDK_DIR}/standard/common/include/private"
    # Requires aws_ota_codesigner_certificate.h
    "$<IF:${AFR_IS_TESTING},${AFR_MODULES_FREERTOS_PLUS_DIR}/aws/ota/test,${AFR_DEMOS_DIR}/include>"
)

afr_module_dependencies(
    ${AFR_CURRENT_MODULE}
    PUBLIC
        3rdparty::pkcs11 
        AFR::pkcs11
    PRIVATE
        AFR::${AFR_CURRENT_MODULE}::mcu_port
	    AFR::utils
)


if(ECC608_IN_USE)
    set(mchp_dir "${AFR_VENDORS_DIR}/microchip")
    set(ecc608a_dir "${mchp_dir}/secure_elements")
    set(atca_config_dir "${CMAKE_CURRENT_LIST_DIR}/ecc608a")
    afr_module(NAME pkcs11_ecc608a INTERFACE)

    afr_glob_src(ecc608a_atacert_src RECURSE DIRECTORY ${ecc608a_dir}/lib/atcacert)
    afr_glob_src(ecc608a_basic_src RECURSE DIRECTORY ${ecc608a_dir}/lib/basic)
    afr_glob_src(ecc608a_crypto_src RECURSE DIRECTORY ${ecc608a_dir}/lib/crypto)
    afr_glob_src(ecc608a_host_src RECURSE DIRECTORY ${ecc608a_dir}/lib/host)
    afr_glob_src(ecc608a_pkcs11_src RECURSE DIRECTORY ${ecc608a_dir}/lib/pkcs11)


    afr_glob_src(ecc608a_src DIRECTORY ${ecc608a_dir}/lib)
    list(REMOVE_ITEM ecc608a_src "${ecc608a_dir}/lib/atca_cfgs.c")

    set(ecc608_include "${ecc608a_dir}/lib")
    set(ecc608_src_list
        "${ecc608a_src}"
        "${ecc608a_atacert_src}"
        "${ecc608a_basic_src}"
        "${ecc608a_crypto_src}"
        "${ecc608a_host_src}"
        "${ecc608a_pkcs11_src}"
    )

    afr_module_sources(
        pkcs11_ecc608a 
        INTERFACE
            "${atca_config_dir}/atca_cert_chain.h"
            "${atca_config_dir}/atca_cert_chain.c"
            "${atca_config_dir}/iot_pkcs11_secure_element.c"
            "${ecc608_src_list}"
    )

    afr_module_include_dirs(
        pkcs11_ecc608a
        INTERFACE
            "${inc_dir}"
            "${ecc608_include}"
    )

    afr_module_dependencies(
        pkcs11_ecc608a
        INTERFACE
            AFR::crypto
            AFR::pkcs11_implementation
            AFR::utils
    )
else()
    afr_module(NAME pkcs11_mbedtls INTERFACE)
    set(src_dir "${CMAKE_CURRENT_LIST_DIR}/mbedtls")

    afr_module_sources(
        pkcs11_mbedtls 
        INTERFACE
            "${src_dir}/iot_pkcs11_mbedtls.c"
            "${src_dir}/threading_alt.h"
            "${inc_dir}/iot_pkcs11_pal.h"
    )

    afr_module_dependencies(
        pkcs11_mbedtls
        INTERFACE
            AFR::crypto
            AFR::pkcs11_implementation
            AFR::utils
            3rdparty::mbedtls
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
)
