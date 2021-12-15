<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>core_pkcs11.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/</path>
    <filename>core__pkcs11_8c.html</filename>
    <includes id="core__pkcs11_8h" name="core_pkcs11.h" local="yes" imported="no">core_pkcs11.h</includes>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvOpenSession</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a823383b0a2c3c3a7f7fad292307a3317</anchor>
      <arglist>(CK_SESSION_HANDLE *pxSession, CK_SLOT_ID xSlotId)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xGetSlotList</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a46c41cd94b0ad5eed22d17a59aef73da</anchor>
      <arglist>(CK_SLOT_ID **ppxSlotId, CK_ULONG *pxSlotCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePKCS11</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>af9674309915268c89934f5b4ba9dba4e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePkcs11Token</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a839e3d1d640a1a13b25a49c93d1a05fa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePkcs11Session</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a54d499d52094e28bdcfb59839e381675</anchor>
      <arglist>(CK_SESSION_HANDLE *pxSession)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xFindObjectWithLabelAndClass</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a6f97ebff17fdfd0f2dc404b2c61010ae</anchor>
      <arglist>(CK_SESSION_HANDLE xSession, char *pcLabelName, CK_ULONG ulLabelNameLen, CK_OBJECT_CLASS xClass, CK_OBJECT_HANDLE_PTR pxHandle)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>vAppendSHA256AlgorithmIdentifierSequence</name>
      <anchorfile>core__pkcs11_8c.html</anchorfile>
      <anchor>a2e58242913103993041305e349913c88</anchor>
      <arglist>(const uint8_t *puc32ByteHashedMessage, uint8_t *puc51ByteHashOidBuffer)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_pkcs11.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/include/</path>
    <filename>core__pkcs11_8h.html</filename>
    <class kind="struct">PKCS11_CertificateTemplate_t</class>
    <member kind="define">
      <type>#define</type>
      <name>CK_PTR</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a423401496b51f5c72a74e5502b47fd7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NULL_PTR</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a530f11a96e508d171d28564c8dc20942</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CK_DEFINE_FUNCTION</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>aa21d2a59f7de7ecc92a13e2958bb60b8</anchor>
      <arglist>(returnType, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CK_DECLARE_FUNCTION</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a30315d302108bcfb354196f37b16a492</anchor>
      <arglist>(returnType, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CK_DECLARE_FUNCTION_POINTER</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>aad472a68fb8e3eb9ba40169f5180b3b7</anchor>
      <arglist>(returnType, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CK_CALLBACK_FUNCTION</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a5235e6437759c93b8189b124c8c807cf</anchor>
      <arglist>(returnType, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11SHA256_DIGEST_LENGTH</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>ac1020688685b585cda7b9c4f2b78744f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11ECDSA_P256_SIGNATURE_LENGTH</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>afb0ec19370a0ef13c3ba9e16f2d812c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11ECDSA_P256_KEY_BITS</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a7b079f0b4d00704a7528468024f3375a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11RSA_PUBLIC_EXPONENT</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a88b87e958fdc824b82e7970feed64759</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11RSA_2048_MODULUS_BITS</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a53ad7f65ddb35590bc1393411e285127</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11RSA_2048_SIGNATURE_LENGTH</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a7bbb5c3318097a212658f3eaa92c06bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11RSA_SIGNATURE_INPUT_LENGTH</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>aaa89dfd906480c38a78efa5e854f3076</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11ELLIPTIC_CURVE_NISTP256</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a2f217cc01f929b02e1a789cc9e908427</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11MAX_LABEL_LENGTH</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>ab94d23efa5d9de9342e45d357153e54e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11DER_ENCODED_OID_P256</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a61aab65f40f1865cfcdbb66aab3c9ffa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a6a4e66ae660556550058473322f5db51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11STUFF_APPENDED_TO_RSA_SIG</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a3ec42bafe82e299acf76e00da9a54168</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePKCS11</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>af9674309915268c89934f5b4ba9dba4e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xGetSlotList</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a46c41cd94b0ad5eed22d17a59aef73da</anchor>
      <arglist>(CK_SLOT_ID **ppxSlotId, CK_ULONG *pxSlotCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePkcs11Session</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a54d499d52094e28bdcfb59839e381675</anchor>
      <arglist>(CK_SESSION_HANDLE *pxSession)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xInitializePkcs11Token</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a839e3d1d640a1a13b25a49c93d1a05fa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>xFindObjectWithLabelAndClass</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a6f97ebff17fdfd0f2dc404b2c61010ae</anchor>
      <arglist>(CK_SESSION_HANDLE xSession, char *pcLabelName, CK_ULONG ulLabelNameLen, CK_OBJECT_CLASS xClass, CK_OBJECT_HANDLE_PTR pxHandle)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>vAppendSHA256AlgorithmIdentifierSequence</name>
      <anchorfile>core__pkcs11_8h.html</anchorfile>
      <anchor>a2e58242913103993041305e349913c88</anchor>
      <arglist>(const uint8_t *puc32ByteHashedMessage, uint8_t *puc51ByteHashOidBuffer)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_pkcs11_mbedtls.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/portable/mbedtls/</path>
    <filename>core__pkcs11__mbedtls_8c.html</filename>
    <includes id="core__pkcs11_8h" name="core_pkcs11.h" local="yes" imported="no">core_pkcs11.h</includes>
    <includes id="core__pkcs11__pal_8h" name="core_pkcs11_pal.h" local="yes" imported="no">core_pkcs11_pal.h</includes>
    <includes id="core__pki__utils_8h" name="core_pki_utils.h" local="yes" imported="no">core_pki_utils.h</includes>
    <class kind="struct">P11Object_t</class>
    <class kind="struct">P11ObjectList_t</class>
    <class kind="struct">P11Struct_t</class>
    <class kind="struct">P11Session_t</class>
    <member kind="define">
      <type>#define</type>
      <name>mbedtlsHighLevelCodeOrDefault</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a92427274c3174ef89fbf82a0ad25a252</anchor>
      <arglist>(mbedTlsCode)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>mbedtlsLowLevelCodeOrDefault</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a1936e3a07702240561149805e8f0bfbb</anchor>
      <arglist>(mbedTlsCode)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11MUTEX_WAIT_MS</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga3c3cb4f818434c9c5805eff93be9d26d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11NO_OPERATION</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga72e760ed32f2b6f0594b8dd809a7a419</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PRIVATE_EC_PRIME_256_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad1708045ca3bc09c5a21749c96ef5c2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PUBLIC_EC_PRIME_256_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga4f8ea6875e04ee62fe1ca1075140d700</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PUBLIC_RSA_2048_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga90f9bd9cec08398aec0ddbb1e444636c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PRIVATE_RSA_2048_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaca48610021fedfe070a799710c789078</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_EC_PUBLIC_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gab2529e98c1b474814147f833376ac084</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_EC_PRIVATE_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga1c4abec3a04c71a88e5200b01cb86da8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11EC_POINT_LENGTH</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaed3393d271431bcd27e5017304b03003</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_PUBLIC_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad4af7c6b1be242776d1a84da0573de96</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_PRIVATE_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga40b4e383d07c501a52e671afea745303</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11KEY_GEN_MAX_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad7d5c1bfb7405cf466597145a634b7d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11SLOT_ID</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gadada8e23f6001eedf0b726604495c611</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LABEL_IN_TEMPLATE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga772e2b44059eb5224eb4900f628f6182</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PRIVATE_IN_TEMPLATE</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a1ef7157051f0e594ce92ff341e33ba9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIGN_IN_TEMPLATE</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a242475cf0d847d052b57456ed5df99a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>EC_PARAMS_IN_TEMPLATE</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a781de9e6cf56f8a0ad83e8f6dab3c818</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>VERIFY_IN_TEMPLATE</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a612b5efde52700aca5c9ff37d7e00ad1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PKCS11_INVALID_KEY_TYPE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaeae7ec1a599c2f7304f883c90379db36</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCheckValidSessionAndModule</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ab93c9263b9b89774e57eee9e8ac708a8</anchor>
      <arglist>(const P11Session_t *pxSession)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static P11Session_t *</type>
      <name>prvSessionPointerFromHandle</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a8a3079aa3ce5d51bb787a3e661714ef0</anchor>
      <arglist>(CK_SESSION_HANDLE xSession)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_BBOOL</type>
      <name>prvOperationActive</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a2ec465960e108e808263b43e589fbd58</anchor>
      <arglist>(const P11Session_t *pxSession)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvMbedTLS_Initialize</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ad7e3c1e337d187b04f516cb1fb15353d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvGetObjectClass</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aea749c88944158ec1b78218887ae956a</anchor>
      <arglist>(const CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount, CK_OBJECT_CLASS *pxClass)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCertAttParse</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac3c56007ca946891c275921e943186ba</anchor>
      <arglist>(CK_ATTRIBUTE *pxAttribute, CK_CERTIFICATE_TYPE *pxCertificateType, CK_BYTE_PTR *ppxCertificateValue, CK_ULONG *pxCertificateLength, CK_ATTRIBUTE **ppxLabel)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvRsaPrivKeyAttParse</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a3462432962ae2df4b3012761ac9aa264</anchor>
      <arglist>(const CK_ATTRIBUTE *pxAttribute)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvRsaPubKeyAttParse</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a54ead3a2ed216f399173b9a5286c0d02</anchor>
      <arglist>(const CK_ATTRIBUTE *pxAttribute)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvRsaContextParse</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a017f9faccb8b2be0504b3777d6bd9797</anchor>
      <arglist>(const CK_ATTRIBUTE *pxAttribute, mbedtls_rsa_context *pxRsaContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvRsaKeyAttParse</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a22f5c778444a3f181c1cefb8ce0cd61c</anchor>
      <arglist>(const CK_ATTRIBUTE *pxAttribute, mbedtls_rsa_context *pxRsaContext, CK_BBOOL xIsPrivate)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvFindObjectInListByLabel</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ada19dbe74aaa7877055858c819c60ad5</anchor>
      <arglist>(const CK_BYTE *pcLabel, CK_ULONG xLabelLength, CK_OBJECT_HANDLE_PTR pxPalHandle, CK_OBJECT_HANDLE_PTR pxAppHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvFindObjectInListByHandle</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aadf1ee4b99279af65de44082fbd4de52</anchor>
      <arglist>(CK_OBJECT_HANDLE xAppHandle, CK_OBJECT_HANDLE_PTR pxPalHandle, CK_BYTE_PTR *ppcLabel, CK_ULONG_PTR pxLabelLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvDeleteObjectFromList</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>acbd6c592f8060853f5e2eda2af208059</anchor>
      <arglist>(CK_OBJECT_HANDLE xPalHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvAddObjectToList</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac07ee599e28a77e94ef82db1346157d3</anchor>
      <arglist>(CK_OBJECT_HANDLE xPalHandle, CK_OBJECT_HANDLE_PTR pxAppHandle, const CK_BYTE *pcLabel, CK_ULONG xLabelLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvAppendEmptyECDerKey</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aab7928cf39dc526a8f8e2ee1a1e3d828</anchor>
      <arglist>(uint8_t *pusECPrivateKey, uint32_t ulDerBufSize, int32_t lDerKeyLength, uint32_t *pulActualKeyLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvSaveDerKeyToPal</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a9fc8ed202af3975cfcf3e65593b20330</anchor>
      <arglist>(mbedtls_pk_context *pxMbedContext, CK_OBJECT_HANDLE_PTR pxObject, CK_ATTRIBUTE *pxLabel, CK_KEY_TYPE xKeyType, CK_BBOOL xIsPrivate)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_Initialize</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a2e45ad4ce8c84712c646dddaa99544e3</anchor>
      <arglist>(CK_VOID_PTR pInitArgs)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_Finalize</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a945318df98e940ab331931023133ed3d</anchor>
      <arglist>(CK_VOID_PTR pReserved)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GetFunctionList</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ab4cf13775c4e92b36156ee92dd52bef5</anchor>
      <arglist>(CK_FUNCTION_LIST_PTR_PTR ppFunctionList)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GetSlotList</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac6ce96c3f4a3b7f24d9420763df121fc</anchor>
      <arglist>(CK_BBOOL tokenPresent, CK_SLOT_ID_PTR pSlotList, CK_ULONG_PTR pulCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GetTokenInfo</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac933291718eb6aa218db1c677e07ab02</anchor>
      <arglist>(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR pInfo)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GetMechanismInfo</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>abe5aeb6b794d4ca28cb2534fe0a4a159</anchor>
      <arglist>(CK_SLOT_ID slotID, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_InitToken</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a3bac1f2e5a7bc50d4d27fb2360eaadcb</anchor>
      <arglist>(CK_SLOT_ID slotID, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen, CK_UTF8CHAR_PTR pLabel)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_OpenSession</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ad28ad989a94c345875dff446ad6c6481</anchor>
      <arglist>(CK_SLOT_ID slotID, CK_FLAGS flags, CK_VOID_PTR pApplication, CK_NOTIFY Notify, CK_SESSION_HANDLE_PTR phSession)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_CloseSession</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ae2ce0be368072fa83e0b1f5136b7b753</anchor>
      <arglist>(CK_SESSION_HANDLE hSession)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_Login</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aca7aa4ae8d9592f464dc1b545fead02e</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_USER_TYPE userType, CK_UTF8CHAR_PTR pPin, CK_ULONG ulPinLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCreateCertificate</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ae79b4b6a2d8cd5db13cc41241f2b9f33</anchor>
      <arglist>(CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR pxObject)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvGetKeyType</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aa0e580aa2af212cfd2468cea733368ae</anchor>
      <arglist>(CK_KEY_TYPE *pxKeyType, const CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvGetLabel</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>af5e3d88c14dcc4e522555febdfc1ad9c</anchor>
      <arglist>(CK_ATTRIBUTE **ppxLabel, CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCreateRsaKey</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a3a0c57a64c5e4015e4db38268aa9e3ac</anchor>
      <arglist>(CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR pxObject, CK_BBOOL xIsPrivate)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCreatePrivateKey</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a0fc54608ace5726a722929c07e8dbbba</anchor>
      <arglist>(CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR pxObject)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCreatePublicKey</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a591739d3757d8746043b26a6e4c9e2bd</anchor>
      <arglist>(CK_ATTRIBUTE *pxTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR pxObject)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_CreateObject</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a01b36384bff05a4dc4476c0764cdc248</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR phObject)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_DestroyObject</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a1b991b6fd8b20f130f9d87b98e590fdb</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GetAttributeValue</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aaad61dc7b2313286bdd049676ef0fd70</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_FindObjectsInit</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>af4d4d071deb07e7dcfa489a2a835756d</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_FindObjects</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a42635cbaa2ba78482ae053bea477246a</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE_PTR phObject, CK_ULONG ulMaxObjectCount, CK_ULONG_PTR pulObjectCount)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_FindObjectsFinal</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a46347f830269bfbcbece15951f0c0fa4</anchor>
      <arglist>(CK_SESSION_HANDLE hSession)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_DigestInit</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a3f6bcb9f9be8a7822d9d8f7c44c33403</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_DigestUpdate</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a7a8189a6963fbcfdfe2bebe20a8bf3b6</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pPart, CK_ULONG ulPartLen)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_DigestFinal</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a1f3dc3758726bb4737db956e4cd71e91</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pDigest, CK_ULONG_PTR pulDigestLen)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_SignInit</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a37d93326915dc9b74f2476238f35037c</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_Sign</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a5f587605d68a7a56012ac337ddaec5e1</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG_PTR pulSignatureLen)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_VerifyInit</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>aeae51e6092fa8c8a58e022fcd8366c45</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_OBJECT_HANDLE hKey)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_Verify</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a4e5f5ad0a6c7c6083ad3d780cc6d5529</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_BYTE_PTR pData, CK_ULONG ulDataLen, CK_BYTE_PTR pSignature, CK_ULONG ulSignatureLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCheckGenerateKeyPairPrivateTemplate</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>af6ce5a8dfa0d3de7a17807a079f6bf88</anchor>
      <arglist>(CK_ATTRIBUTE **ppxLabel, CK_ATTRIBUTE *pxAttribute, uint32_t *pulAttributeMap)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CK_RV</type>
      <name>prvCheckGenerateKeyPairPublicTemplate</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ab49218b2f2a55fe7a50f8f472d42058f</anchor>
      <arglist>(CK_ATTRIBUTE **ppxLabel, CK_ATTRIBUTE *pxAttribute, uint32_t *pulAttributeMap)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GenerateKeyPair</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a802a061224966a5523beb1ea0a9d9c27</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_MECHANISM_PTR pMechanism, CK_ATTRIBUTE_PTR pPublicKeyTemplate, CK_ULONG ulPublicKeyAttributeCount, CK_ATTRIBUTE_PTR pPrivateKeyTemplate, CK_ULONG ulPrivateKeyAttributeCount, CK_OBJECT_HANDLE_PTR phPublicKey, CK_OBJECT_HANDLE_PTR phPrivateKey)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>C_GenerateRandom</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a54a883eb1608be800f7fbd204665d50d</anchor>
      <arglist>(CK_SESSION_HANDLE hSession, CK_BYTE_PTR RandomData, CK_ULONG ulRandomLen)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pNoHighLevelMbedTlsCodeStr</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac80d2c725e8e5238f649a2ed75e67c30</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pNoLowLevelMbedTlsCodeStr</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>ac3215d33b1ac379cd57b6458609de535</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static P11Struct_t</type>
      <name>xP11Context</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a61d2aa023bf1b179d662d7a959348c0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static P11Session_t</type>
      <name>pxP11Sessions</name>
      <anchorfile>core__pkcs11__mbedtls_8c.html</anchorfile>
      <anchor>a97e74ce80ee33111b27c11c3644c9cc7</anchor>
      <arglist>[pkcs11configMAX_SESSIONS]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_pkcs11_pal.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/include/</path>
    <filename>core__pkcs11__pal_8h.html</filename>
    <member kind="function">
      <type>CK_RV</type>
      <name>PKCS11_PAL_Initialize</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>ad78bacbf90a03e49a93b73fc7f4287ec</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>CK_OBJECT_HANDLE</type>
      <name>PKCS11_PAL_SaveObject</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>aa426d170f01eec0268938577546c2683</anchor>
      <arglist>(CK_ATTRIBUTE_PTR pxLabel, CK_BYTE_PTR pucData, CK_ULONG ulDataSize)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>PKCS11_PAL_DestroyObject</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>a3e5df5f3f2fe4ab8b5b703e90973603f</anchor>
      <arglist>(CK_OBJECT_HANDLE xHandle)</arglist>
    </member>
    <member kind="function">
      <type>CK_OBJECT_HANDLE</type>
      <name>PKCS11_PAL_FindObject</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>aa40ed819a4db63bf1f82ec3a83f7671d</anchor>
      <arglist>(CK_BYTE_PTR pxLabel, CK_ULONG usLength)</arglist>
    </member>
    <member kind="function">
      <type>CK_RV</type>
      <name>PKCS11_PAL_GetObjectValue</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>a5f9deac2fdf2dc0ac52e87f95d3d6fb6</anchor>
      <arglist>(CK_OBJECT_HANDLE xHandle, CK_BYTE_PTR *ppucData, CK_ULONG_PTR pulDataSize, CK_BBOOL *pIsPrivate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>PKCS11_PAL_GetObjectValueCleanup</name>
      <anchorfile>core__pkcs11__pal_8h.html</anchorfile>
      <anchor>a62b94691f4534e5328eb88362244e308</anchor>
      <arglist>(CK_BYTE_PTR pucData, CK_ULONG ulDataSize)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_pki_utils.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/</path>
    <filename>core__pki__utils_8c.html</filename>
    <includes id="core__pki__utils_8h" name="core_pki_utils.h" local="yes" imported="no">core_pki_utils.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>FAILURE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga6d58f9ac447476b4e084d7ca383f5183</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int8_t</type>
      <name>PKI_mbedTLSSignatureToPkcs11Signature</name>
      <anchorfile>core__pki__utils_8c.html</anchorfile>
      <anchor>a7d2768d5e160165af1294e46e21787c4</anchor>
      <arglist>(uint8_t *pxSignaturePKCS, const uint8_t *pxMbedSignature)</arglist>
    </member>
    <member kind="function">
      <type>int8_t</type>
      <name>PKI_pkcs11SignatureTombedTLSSignature</name>
      <anchorfile>core__pki__utils_8c.html</anchorfile>
      <anchor>a089abc57156366d6e5ab137d51e5e60d</anchor>
      <arglist>(uint8_t *pucSig, size_t *pxSigLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_pki_utils.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/corePKCS11/source/include/</path>
    <filename>core__pki__utils_8h.html</filename>
    <member kind="function">
      <type>int8_t</type>
      <name>PKI_mbedTLSSignatureToPkcs11Signature</name>
      <anchorfile>core__pki__utils_8h.html</anchorfile>
      <anchor>a7d2768d5e160165af1294e46e21787c4</anchor>
      <arglist>(uint8_t *pxSignaturePKCS, const uint8_t *pxMbedSignature)</arglist>
    </member>
    <member kind="function">
      <type>int8_t</type>
      <name>PKI_pkcs11SignatureTombedTLSSignature</name>
      <anchorfile>core__pki__utils_8h.html</anchorfile>
      <anchor>a089abc57156366d6e5ab137d51e5e60d</anchor>
      <arglist>(uint8_t *pucSig, size_t *pxSigLen)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>P11Object_t</name>
    <filename>struct_p11_object__t.html</filename>
    <member kind="variable">
      <type>CK_OBJECT_HANDLE</type>
      <name>xHandle</name>
      <anchorfile>struct_p11_object__t.html</anchorfile>
      <anchor>a57fcfdc589dfcbd844b43f98a7decaab</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ULONG</type>
      <name>xLabelSize</name>
      <anchorfile>struct_p11_object__t.html</anchorfile>
      <anchor>a17b64668ea51878819866f1aff94797c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_BYTE</type>
      <name>xLabel</name>
      <anchorfile>struct_p11_object__t.html</anchorfile>
      <anchor>aaf579a29d826ac56799a36ca6c6a7be7</anchor>
      <arglist>[pkcs11configMAX_LABEL_LENGTH+1]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>P11ObjectList_t</name>
    <filename>struct_p11_object_list__t.html</filename>
    <member kind="variable">
      <type>mbedtls_threading_mutex_t</type>
      <name>xMutex</name>
      <anchorfile>struct_p11_object_list__t.html</anchorfile>
      <anchor>ac27b74ff013d1d035cea4129e52900a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>P11Object_t</type>
      <name>xObjects</name>
      <anchorfile>struct_p11_object_list__t.html</anchorfile>
      <anchor>a28728d92aa4778ef78a414b40596998d</anchor>
      <arglist>[pkcs11configMAX_NUM_OBJECTS]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>P11Session_t</name>
    <filename>struct_p11_session__t.html</filename>
    <member kind="variable">
      <type>CK_ULONG</type>
      <name>ulState</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>ab65cd70e10d4221b1c811814745ecf04</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_BBOOL</type>
      <name>xOpened</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a789e8a76ef3043347f3185678595c8e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_MECHANISM_TYPE</type>
      <name>xOperationDigestMechanism</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>ae7e701806cc6cc8bf7a08e702faa1365</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_BYTE *</type>
      <name>pxFindObjectLabel</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a1b3bec8cf6fa188a2aca9709c2400188</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ULONG</type>
      <name>xFindObjectLabelLen</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>ad7ba8e3179ffb7d43b037c6ad843735c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_MECHANISM_TYPE</type>
      <name>xOperationVerifyMechanism</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a5cd70536a7b732572dcabccf726ed01e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_threading_mutex_t</type>
      <name>xVerifyMutex</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a12aac1e5f178a7b71a88bd83a6316d5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_OBJECT_HANDLE</type>
      <name>xVerifyKeyHandle</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>ab740704e3cd0a5a4e9e5ef756f9ff384</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_pk_context</type>
      <name>xVerifyKey</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a75880c73f99a62ce084a5d2ecc05421b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_MECHANISM_TYPE</type>
      <name>xOperationSignMechanism</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a946bd1288a08460e4b00085f8f08d5ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_threading_mutex_t</type>
      <name>xSignMutex</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>abc6d27471e92686faee54a9a4be9d770</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_OBJECT_HANDLE</type>
      <name>xSignKeyHandle</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a01a1d13faae5fee4e8c8b76badb7fabf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_pk_context</type>
      <name>xSignKey</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>ae954bd4ce22253542065d19e5187dda9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_sha256_context</type>
      <name>xSHA256Context</name>
      <anchorfile>struct_p11_session__t.html</anchorfile>
      <anchor>a335416e156facf8dc065d2b1550047da</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>P11Struct_t</name>
    <filename>struct_p11_struct__t.html</filename>
    <member kind="variable">
      <type>CK_BBOOL</type>
      <name>xIsInitialized</name>
      <anchorfile>struct_p11_struct__t.html</anchorfile>
      <anchor>adadf5bdd5a81f36f7ffaad5ccdaf220e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_ctr_drbg_context</type>
      <name>xMbedDrbgCtx</name>
      <anchorfile>struct_p11_struct__t.html</anchorfile>
      <anchor>afe68c63069ef3833e2954cfb364fdc51</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_entropy_context</type>
      <name>xMbedEntropyContext</name>
      <anchorfile>struct_p11_struct__t.html</anchorfile>
      <anchor>ad51766f220f2dfcd6da2a370d69345c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>mbedtls_threading_mutex_t</type>
      <name>xSessionMutex</name>
      <anchorfile>struct_p11_struct__t.html</anchorfile>
      <anchor>adb2b36855d05652f07bb6e42b8ee73dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>P11ObjectList_t</type>
      <name>xObjectList</name>
      <anchorfile>struct_p11_struct__t.html</anchorfile>
      <anchor>aaa3b2f04cc57c3538c638cc97747e1bf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PKCS11_CertificateTemplate_t</name>
    <filename>struct_p_k_c_s11___certificate_template__t.html</filename>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xObjectClass</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>ab47a88d3c053dc9800fe4f03b9f675b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xSubject</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>a7239b6cff4ffa2b63d88b149d52e3d8a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xCertificateType</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>a94d315d61728f981137572ea4a0c9f4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xValue</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>a028ec980b8dca55b842ae0bf1657907f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xLabel</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>ae4fa330e581803d900d1d2b05cbbe52b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CK_ATTRIBUTE</type>
      <name>xTokenObject</name>
      <anchorfile>struct_p_k_c_s11___certificate_template__t.html</anchorfile>
      <anchor>aaf1210b30b9b00d4ad9d8165b7e7bc38</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>pkcs11_macros</name>
    <title>PKCS #11 Implementation Macros</title>
    <filename>group__pkcs11__macros.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>FAILURE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga6d58f9ac447476b4e084d7ca383f5183</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11MUTEX_WAIT_MS</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga3c3cb4f818434c9c5805eff93be9d26d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11NO_OPERATION</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga72e760ed32f2b6f0594b8dd809a7a419</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PRIVATE_EC_PRIME_256_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad1708045ca3bc09c5a21749c96ef5c2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PUBLIC_EC_PRIME_256_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga4f8ea6875e04ee62fe1ca1075140d700</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PUBLIC_RSA_2048_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga90f9bd9cec08398aec0ddbb1e444636c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_PRIVATE_RSA_2048_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaca48610021fedfe070a799710c789078</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_EC_PUBLIC_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gab2529e98c1b474814147f833376ac084</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_EC_PRIVATE_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga1c4abec3a04c71a88e5200b01cb86da8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11EC_POINT_LENGTH</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaed3393d271431bcd27e5017304b03003</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_PUBLIC_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad4af7c6b1be242776d1a84da0573de96</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11_MAX_PRIVATE_KEY_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga40b4e383d07c501a52e671afea745303</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11KEY_GEN_MAX_DER_SIZE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gad7d5c1bfb7405cf466597145a634b7d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>pkcs11SLOT_ID</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gadada8e23f6001eedf0b726604495c611</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LABEL_IN_TEMPLATE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>ga772e2b44059eb5224eb4900f628f6182</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PKCS11_INVALID_KEY_TYPE</name>
      <anchorfile>group__pkcs11__macros.html</anchorfile>
      <anchor>gaeae7ec1a599c2f7304f883c90379db36</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>pkcs11_datatypes</name>
    <title>PKCS #11 Datatypes</title>
    <filename>group__pkcs11__datatypes.html</filename>
    <class kind="struct">PKCS11_CertificateTemplate_t</class>
    <class kind="struct">P11Object_t</class>
    <class kind="struct">P11ObjectList_t</class>
    <class kind="struct">P11Struct_t</class>
    <class kind="struct">P11Session_t</class>
  </compound>
  <compound kind="page">
    <name>pkcs11_design</name>
    <title>Design</title>
    <filename>pkcs11_design.html</filename>
    <docanchor file="pkcs11_design.html" title="Memory Requirements">pkcs11_memory_requirements</docanchor>
    <docanchor file="pkcs11_design.html" title="PKCS #11 Wrapper Dependencies">PKCS11_Wrapper</docanchor>
    <docanchor file="pkcs11_design.html" title="PKCS #11 Software Implementation Dependencies">PKCS11_implementation</docanchor>
    <docanchor file="pkcs11_design.html" title="PKCS #11 Utilities Dependencies">PKCS11_utilities</docanchor>
  </compound>
  <compound kind="page">
    <name>pkcs11_seq</name>
    <title>PKCS #11 Sequence Diagrams</title>
    <filename>pkcs11_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_rng_seq</name>
    <title>PKCS #11 RNG Sequence Diagram</title>
    <filename>pkcs11_rng_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_dig_seq</name>
    <title>PKCS #11 Digest Sequence Diagram</title>
    <filename>pkcs11_dig_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_obj_imp_seq</name>
    <title>PKCS #11 Object Import Sequence Diagram</title>
    <filename>pkcs11_obj_imp_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_obj_gen_seq</name>
    <title>PKCS #11 Generate Key Pair Sequence Diagram</title>
    <filename>pkcs11_obj_gen_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_sign_verify_seq</name>
    <title>PKCS #11 Sign and Verify Sequence Diagram</title>
    <filename>pkcs11_sign_verify_seq.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_config</name>
    <title>PKCS #11 Configuration Macros</title>
    <filename>pkcs11_config.html</filename>
    <docanchor file="pkcs11_config.html">configPKCS11_DEFAULT_USER_PIN</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configMAX_LABEL_LENGTH</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configMAX_NUM_OBJECTS</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configMAX_SESSIONS</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11testIMPORT_PRIVATE_KEY_SUPPORT</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11testGENERATE_KEYPAIR_SUPPORT</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11testPREPROVISIONED_SUPPORT</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS</docanchor>
    <docanchor file="pkcs11_config.html">pkcs11configLABEL_ROOT_CERTIFICATE</docanchor>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_mbedtls_function</name>
    <title>PKCS #11 mbed TLS Implementation Functions</title>
    <filename>pkcs11_core_mbedtls_function.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_initialize</name>
    <title>C_Initialize</title>
    <filename>pkcs11_mbedtls_function_c_initialize.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_finalize</name>
    <title>C_Finalize</title>
    <filename>pkcs11_mbedtls_function_c_finalize.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_getfunctionlist</name>
    <title>C_GetFunctionList</title>
    <filename>pkcs11_mbedtls_function_c_getfunctionlist.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_getslotlist</name>
    <title>C_GetSlotList</title>
    <filename>pkcs11_mbedtls_function_c_getslotlist.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_gettokeninfo</name>
    <title>C_GetTokenInfo</title>
    <filename>pkcs11_mbedtls_function_c_gettokeninfo.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_getmechanisminfo</name>
    <title>C_GetMechanismInfo</title>
    <filename>pkcs11_mbedtls_function_c_getmechanisminfo.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_inittoken</name>
    <title>C_InitToken</title>
    <filename>pkcs11_mbedtls_function_c_inittoken.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_opensession</name>
    <title>C_OpenSession</title>
    <filename>pkcs11_mbedtls_function_c_opensession.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_closesession</name>
    <title>C_CloseSession</title>
    <filename>pkcs11_mbedtls_function_c_closesession.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_login</name>
    <title>C_Login</title>
    <filename>pkcs11_mbedtls_function_c_login.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_createobject</name>
    <title>C_CreateObject</title>
    <filename>pkcs11_mbedtls_function_c_createobject.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_destroyobject</name>
    <title>C_DestroyObject</title>
    <filename>pkcs11_mbedtls_function_c_destroyobject.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_getattributevalue</name>
    <title>C_GetAttributeValue</title>
    <filename>pkcs11_mbedtls_function_c_getattributevalue.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_findobjectsinit</name>
    <title>C_FindObjectsInit</title>
    <filename>pkcs11_mbedtls_function_c_findobjectsinit.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_findobjects</name>
    <title>C_FindObjects</title>
    <filename>pkcs11_mbedtls_function_c_findobjects.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_findobjectsfinal</name>
    <title>C_FindObjectsFinal</title>
    <filename>pkcs11_mbedtls_function_c_findobjectsfinal.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_digestinit</name>
    <title>C_DigestInit</title>
    <filename>pkcs11_mbedtls_function_c_digestinit.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_digestupdate</name>
    <title>C_DigestUpdate</title>
    <filename>pkcs11_mbedtls_function_c_digestupdate.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_digestfinal</name>
    <title>C_DigestFinal</title>
    <filename>pkcs11_mbedtls_function_c_digestfinal.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_signinit</name>
    <title>C_SignInit</title>
    <filename>pkcs11_mbedtls_function_c_signinit.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_verifyinit</name>
    <title>C_VerifyInit</title>
    <filename>pkcs11_mbedtls_function_c_verifyinit.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_verify</name>
    <title>C_Verify</title>
    <filename>pkcs11_mbedtls_function_c_verify.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_generatekeypair</name>
    <title>C_GenerateKeyPair</title>
    <filename>pkcs11_mbedtls_function_c_generatekeypair.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_mbedtls_function_c_generate_random</name>
    <title>C_GenerateRandom</title>
    <filename>pkcs11_mbedtls_function_c_generate_random.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_wrapper_function</name>
    <title>PKCS #11 Wrapper Functions</title>
    <filename>pkcs11_core_wrapper_function.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_xinitializepkcs11</name>
    <title>xInitializePKCS11</title>
    <filename>pkcs11_core_xinitializepkcs11.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_xgetslotlist</name>
    <title>xGetSlotList</title>
    <filename>pkcs11_core_xgetslotlist.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_xinitializepkcs11token</name>
    <title>xInitializePkcs11Token</title>
    <filename>pkcs11_core_xinitializepkcs11token.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_xinitializepkcs11session</name>
    <title>xInitializePkcs11Session</title>
    <filename>pkcs11_core_xinitializepkcs11session.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_xfindobjectwithlabelandclass</name>
    <title>xFindObjectWithLabelAndClass</title>
    <filename>pkcs11_core_xfindobjectwithlabelandclass.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_vappendsha256algorithmidentifiersequence</name>
    <title>vAppendSHA256AlgorithmIdentifierSequence</title>
    <filename>pkcs11_core_vappendsha256algorithmidentifiersequence.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_pal_function</name>
    <title>PKCS #11 PAL Functions</title>
    <filename>pkcs11_core_pal_function.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_initialize</name>
    <title>PKCS11_PAL_Initialize</title>
    <filename>pkcs11_pal_initialize.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_saveobject</name>
    <title>PKCS11_PAL_SaveObject</title>
    <filename>pkcs11_pal_saveobject.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_destroyobject</name>
    <title>PKCS11_PAL_DestroyObject</title>
    <filename>pkcs11_pal_destroyobject.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_findobject</name>
    <title>PKCS11_PAL_FindObject</title>
    <filename>pkcs11_pal_findobject.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_getobjectvalue</name>
    <title>PKCS11_PAL_GetObjectValue</title>
    <filename>pkcs11_pal_getobjectvalue.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_pal_getobjectvaluecleanup</name>
    <title>PKCS11_PAL_GetObjectValueCleanup</title>
    <filename>pkcs11_pal_getobjectvaluecleanup.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_core_utils_function</name>
    <title>PKCS #11 Utils Functions</title>
    <filename>pkcs11_core_utils_function.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_utils_pkipkcs11signaturetombedtlssignature</name>
    <title>PKI_mbedTLSSignatureToPkcs11Signature</title>
    <filename>pkcs11_utils_pkipkcs11signaturetombedtlssignature.html</filename>
  </compound>
  <compound kind="page">
    <name>pkcs11_utils_pkimbedtlssignaturetopkcs11signature</name>
    <title>PKI_pkcs11SignatureTombedTLSSignature</title>
    <filename>pkcs11_utils_pkimbedtlssignaturetopkcs11signature.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
    <docanchor file="index.html">core_pkcs11</docanchor>
  </compound>
</tagfile>
