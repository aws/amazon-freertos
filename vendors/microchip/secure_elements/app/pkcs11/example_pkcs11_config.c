
#include "FreeRTOS.h"
#include "pkcs11_config.h"
#include "pkcs11/pkcs11_object.h"
#include "pkcs11/pkcs11_slot.h"
#include "example_cert_chain.h"


/** Standard Configuration Structure for ATECC608A devices */
const uint8_t atecc608_config[] = {
    0x01, 0x23, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x01, 0x01, 0x00,
    0xC0, 0x00, 0x00, 0x01, 0x8F, 0x20, 0xC4, 0x44, 0x87, 0x20, 0x87, 0x20, 0x8F, 0x0F, 0xC4, 0x36,
    0x9F, 0x0F, 0x82, 0x20, 0x0F, 0x0F, 0xC4, 0x44, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x1C, 0x00, 0x13, 0x00, 0x13, 0x00, 0x7C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x33, 0x00,
    0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00,
};

/* Helper function to assign the proper fields to an certificate object from a cert def */
CK_RV pkcs11_config_cert(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    CK_RV rv = CKR_OK;
	size_t len;
    
    (void)pLibCtx;
    (void)pSlot;

    if (!pObject || !pLabel)
    {
        return CKR_ARGUMENTS_BAD;
    }

	if (pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE)
	{
		return CKR_ARGUMENTS_BAD;
	}

	if (!strncmp(pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, (char*)pLabel->pValue, pLabel->ulValueLen))
	{
    	/* Slot 10 - Device Cert for Slot 0*/
        pkcs11_config_init_cert(pObject, pLabel->pValue, pLabel->ulValueLen);
    	pObject->slot = 10;
    	pObject->class_type = CK_CERTIFICATE_CATEGORY_TOKEN_USER;
    	pObject->size = g_cert_def_2_device.cert_template_size;
    	pObject->data = &g_cert_def_2_device;
	}
	else if (!strncmp(pkcs11configLABEL_JITP_CERTIFICATE, (char*)pLabel->pValue, pLabel->ulValueLen))
	{
    	/* Slot 12 - Signer Cert for Slot 10 */
        pkcs11_config_init_cert(pObject, pLabel->pValue, pLabel->ulValueLen);
    	pObject->slot = 12;
    	pObject->class_type = CK_CERTIFICATE_CATEGORY_AUTHORITY;
    	pObject->size = g_cert_def_1_signer.cert_template_size;
    	pObject->data = &g_cert_def_1_signer;
	}
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

    return rv;
}

/* Helper function to assign the proper fields to a key object */
CK_RV pkcs11_config_key(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    CK_RV rv = CKR_OK;
	size_t len;
    
    (void)pLibCtx;

    if (!pObject || !pLabel || !pSlot)
    {
        return CKR_ARGUMENTS_BAD;
    }

	if (pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE)
	{
		return CKR_ARGUMENTS_BAD;
	}

    if (!strncmp(pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
		/* Slot 0 - Device Private Key */
        pkcs11_config_init_private(pObject, pLabel->pValue, pLabel->ulValueLen);
		pObject->slot = 0;
        pObject->config = &pSlot->cfg_zone;
    }
    else if (!strncmp(pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
		/* Slot 0 - Device Private Key */
        pkcs11_config_init_public(pObject, pLabel->pValue, pLabel->ulValueLen);
		pObject->slot = 0;
        pObject->config = &pSlot->cfg_zone;
    }
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

    return rv;
}

CK_RV pkcs11_config_load_objects(pkcs11_slot_ctx_ptr pSlot)
{
	pkcs11_object_ptr pObject;
	CK_RV rv = CKR_OK;
    CK_ATTRIBUTE xLabel;

	if (CKR_OK == rv)
	{
		rv = pkcs11_object_alloc(&pObject);
		if (pObject)
		{
			/* Slot 0 - Device Private Key */
			xLabel.pValue = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
            xLabel.ulValueLen = strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
			pkcs11_config_key(NULL, pSlot, pObject, &xLabel );
		}
	}
    
	if (CKR_OK == rv)
	{
		rv = pkcs11_object_alloc(&pObject);
		if (pObject)
		{
			/* Slot 0 - Device Public Key */
			xLabel.pValue = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
            xLabel.ulValueLen = strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
			pkcs11_config_key(NULL, pSlot, pObject, &xLabel );
		}
	}

	if (CKR_OK == rv)
	{
		rv = pkcs11_object_alloc(&pObject);
		if (pObject)
		{
			/* Slot 0 - Device Public Key */
			xLabel.pValue = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
            xLabel.ulValueLen = strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
			pkcs11_config_cert(NULL, pSlot, pObject, &xLabel );
		}
	}
    
	if (CKR_OK == rv)
	{
		rv = pkcs11_object_alloc(&pObject);
		if (pObject)
		{
			/* Slot 0 - Device Public Key */
			xLabel.pValue = pkcs11configLABEL_JITP_CERTIFICATE;
            xLabel.ulValueLen = strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
			pkcs11_config_cert(NULL, pSlot, pObject, &xLabel );
		}
	}

	return rv;
}