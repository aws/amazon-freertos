#ifndef AWS_PKCS11_MBEDTLS
#define AWS_PKCS11_MBEDTLS

/**
 * @brief Structure for saving Find Object template information
 *
 * This structure is malloc'ed in FindObjectsInit, used to find a
 * match in FindObjects, and freed in FindObjectsFinal.
 */
typedef struct SearchableAttributes_t
{
    char cLabel[ pkcs11MAX_LABEL_LENGTH ];
    CK_BBOOL xLabelIsValid;
    CK_OBJECT_CLASS xClass;
    CK_BBOOL xClassIsValid;
} SearchableAttributes_t;


void PKI_mbedTLSSignatureToPkcs11Signature( CK_BYTE * pxSignaturePKCS,
                                         uint8_t * pxMbedSignature );

#endif /* AWS_PKCS11_MBEDTLS include guard. */
