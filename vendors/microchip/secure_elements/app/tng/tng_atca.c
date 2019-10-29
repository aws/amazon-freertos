#include "tng_atca.h"


ATCA_STATUS tng_get_type(tng_type_t* type)
{
    ATCA_STATUS ret;
    uint8_t sn[ATCA_SERIAL_NUM_SIZE];

    if (type == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    ret = atcab_read_serial_number(sn);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    if (sn[0] == 0x01 && sn[1] == 0x23 && sn[8] == 0x27)
    {
        *type = TNGTYPE_TN;
    }
    else
    {
        *type = TNGTYPE_22;
    }

    return ATCA_SUCCESS;
}

ATCA_STATUS tng_get_device_pubkey(uint8_t *public_key)
{
    ATCA_STATUS ret;
    tng_type_t type;

    ret = tng_get_type(&type);
    if (ret != ATCA_SUCCESS)
    {
        return ret;
    }

    if (type == TNGTYPE_TN)
    {
        return atcab_get_pubkey(TNGTN_PRIMARY_KEY_SLOT, public_key);
    }
    else
    {
        return atcab_get_pubkey(TNG22_PRIMARY_KEY_SLOT, public_key);
    }
}