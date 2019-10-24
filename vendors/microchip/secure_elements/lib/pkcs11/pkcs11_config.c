/**
 * \file
 * \brief PKCS11 Library Configuration
 *
 * \copyright (c) 2017 Microchip Technology Inc. and its subsidiaries.
 *            You may use this software and any derivatives exclusively with
 *            Microchip products.
 *
 * \page License
 *
 * (c) 2017 Microchip Technology Inc. and its subsidiaries. You may use this
 * software and any derivatives exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIPS TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
 * TERMS.
 */

#include "pkcs11_config.h"
#include "pkcs11_debug.h"
#include "cryptoauthlib.h"
#include "pkcs11_slot.h"
#include "pkcs11_object.h"
#include "pkcs11_key.h"
#include "pkcs11_cert.h"
#include "pkcs11_os.h"


/**
 * \defgroup pkcs11 Configuration (pkcs11_config_)
   @{ */

void pkcs11_config_init_private(pkcs11_object_ptr pObject, char * label, size_t len)
{
    if (len >= PKCS11_MAX_LABEL_SIZE)
    {
        len = PKCS11_MAX_LABEL_SIZE - 1;
    }
    memcpy(pObject->name, label, len);
    pObject->name[len] = '\0';
    pObject->class_id = CKO_PRIVATE_KEY;
    pObject->class_type = CKK_EC;
    pObject->attributes = pkcs11_key_private_attributes;
    pObject->count = pkcs11_key_private_attributes_count;
    pObject->data = NULL;
    pObject->size = 16;
}

void pkcs11_config_init_public(pkcs11_object_ptr pObject, char * label, size_t len)
{
    if (len >= PKCS11_MAX_LABEL_SIZE)
    {
        len = PKCS11_MAX_LABEL_SIZE - 1;
    }
    memcpy(pObject->name, label, len);
    pObject->name[len] = '\0';
    pObject->class_id = CKO_PUBLIC_KEY;
    pObject->class_type = CKK_EC;
    pObject->attributes = pkcs11_key_public_attributes;
    pObject->count = pkcs11_key_public_attributes_count;
    pObject->data = NULL;
    pObject->size = 64;
}

void pkcs11_config_init_cert(pkcs11_object_ptr pObject, char * label, size_t len)
{
    if (len >= PKCS11_MAX_LABEL_SIZE)
    {
        len = PKCS11_MAX_LABEL_SIZE - 1;
    }
    memcpy(pObject->name, label, len);
    pObject->name[len] = '\0';
    pObject->class_id = CKO_CERTIFICATE;
    pObject->class_type = 0;
    pObject->attributes = pkcs11_cert_x509public_attributes;
    pObject->count = pkcs11_cert_x509public_attributes_count;
    pObject->data = NULL;
    pObject->size = 0;
}

#if !PKCS11_USE_STATIC_CONFIG

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static size_t pkcs11_config_load_file(FILE* fp, char ** buffer)
{
    size_t size = 0;

    if (buffer)
    {
        /* Get file size */
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);

        *buffer = (char*)pkcs11_os_malloc(size);
        memset(*buffer, 0, size);
        if (*buffer)
        {
            if (size != fread(*buffer, 1, size, fp))
            {
                pkcs11_os_free(*buffer);
                *buffer = NULL;
                size = 0;
            }
        }
    }
    return size;
}

/* Parse a buffer into key value pairs. Return value should be an even value */
static int pkcs11_config_parse_buffer(char* buffer, size_t len, int argc, char* argv[])
{
    char* s;
    int args = 0;
    bool comment = FALSE;
    bool arg = FALSE;
    bool v = FALSE;

    if (!buffer || !len || !argc || !argv)
    {
        return 0;
    }

    for (s = buffer; s < (buffer + len) && args < argc; s++)
    {
        switch (*s)
        {
        case '\n':
        case '\r':
            /* End the line*/
            if (arg && !v && !comment)
            {
                /* Everything must be a key value pair */
                return 0;
            }
            else
            {
                comment = FALSE;
                v = FALSE;
                arg = FALSE;
                *s = '\0';
            }
            break;
        case '=':
            v = TRUE;
        case ' ':
        case '\t':
            *s = '\0';
            arg = 0;
            break;
        default:
            if (!comment)
            {
                if (*s == '#')
                {
                    comment = 1;
                }
                else if (!arg)
                {
                    argv[args++] = s;
                    arg = TRUE;
                }
            }
            break;
        }
    }

    if (args & 0x1)
    {
        /* Parsing error occured */
        args = 0;
    }

    return args;
}

static void pkcs11_config_split_string(char* s, char splitter, int * argc, char* argv[])
{
    char * e;
    int args = 1;

    if (!s || !argc || !argv)
    {
        return;
    }

    e = s + strlen(s);
    argv[0] = s;

    for (; s < e && args < *argc; s++)
    {
        if (*s == splitter)
        {
            *s = '\0';
            argv[args++] = ++s;
        }
    }
    *argc = args;
}

static CK_RV pkcs11_config_parse_interface(pkcs11_slot_ctx_ptr slot_ctx, char* cfgstr)
{
    int argc = 4;
    char * argv[4];
    CK_RV rv = CKR_GENERAL_ERROR;

    pkcs11_config_split_string(cfgstr, ',', &argc, argv);

    if (!strcmp(argv[0], "i2c"))
    {
        slot_ctx->interface_config = &cfg_ateccx08a_i2c_default;
        if (argc > 1)
        {
            cfg_ateccx08a_i2c_default.atcai2c.slave_address = (uint8_t)strtol(argv[1], NULL, 16);
        }
        if (argc > 2)
        {
            cfg_ateccx08a_i2c_default.atcai2c.bus = (uint8_t)strtol(argv[2], NULL, 16);
        }
        if (argc > 3)
        {
            cfg_ateccx08a_i2c_default.atcai2c.baud = (uint8_t)strtol(argv[3], NULL, 10);
        }
        rv = CKR_OK;
    }
    else if (!strcmp(argv[0], "hid"))
    {
        slot_ctx->interface_config = &cfg_ateccx08a_kithid_default;
        rv = CKR_OK;
    }
    else
    {
        PKCS11_DEBUG("Unrecognized interface: %s", argv[0]);
    }
    return rv;
}

static CK_RV pkcs11_config_parse_freeslots(pkcs11_slot_ctx_ptr slot_ctx, char* cfgstr)
{
    int argc = 16;
    char * argv[16];
    int i;

    pkcs11_config_split_string(cfgstr, ',', &argc, argv);

    for (i = 0; i < argc; i++)
    {
        uint32_t slot = strtol(argv[i], NULL, 10);
        if (slot < 16)
        {
            slot_ctx->flags |= (1 << slot);
        }
    }

    return CKR_OK;
}

static CK_RV pkcs11_config_parse_object(pkcs11_slot_ctx_ptr slot_ctx, char* cfgstr)
{
    int argc = 3;
    char * argv[3];
    CK_RV rv = CKR_GENERAL_ERROR;
    pkcs11_object_ptr pObject;

    pkcs11_config_split_string(cfgstr, ',', &argc, argv);

    if (!strcmp(argv[0], "private") && argc == 3)
    {
        pkcs11_object_ptr pPubkey = NULL;
        CK_BYTE slot = (CK_BYTE)strtol(argv[2], NULL, 10);

        rv = pkcs11_object_alloc(&pObject);
        if (!rv && pObject)
        {
            pkcs11_config_init_private(pObject, argv[1], strlen(argv[1]));
            pObject->slot = slot;
            pObject->flags = 0;
            pObject->config = &slot_ctx->cfg_zone;
        }

        /* Every private key object needs a cooresponding public key object */
        if (!rv)
        {
            rv = pkcs11_object_alloc(&pPubkey);
        }
        if (!rv)
        {
            pkcs11_config_init_public(pPubkey, argv[1], strlen(argv[1]));
            pPubkey->slot = slot;
            pPubkey->flags = 0;
            pPubkey->config = &slot_ctx->cfg_zone;
        }
        else
        {
            pkcs11_object_free(pObject);
        }
    }
    else if (!strcmp(argv[0], "public") && argc == 3)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (!rv && pObject)
        {
            pkcs11_config_init_public(pObject, argv[1], strlen(argv[1]));
            pObject->slot = (CK_BYTE)strtol(argv[2], NULL, 10);
            pObject->flags = 0;
            pObject->config = &slot_ctx->cfg_zone;
        }
    }
#if !PKCS11_USE_STATIC_MEMORY
    else if (!strcmp(argv[0], "certificate") && argc > 3)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (!rv && pObject)
        {
            memmove(pObject->name, argv[1], strlen(argv[1]));
            pObject->slot = (CK_BYTE)strtol(argv[2], NULL, 10);
            pObject->class_id = CKO_CERTIFICATE;
            pObject->class_type = CK_CERTIFICATE_CATEGORY_TOKEN_USER;
            pObject->attributes = pkcs11_cert_x509public_attributes;
            pObject->count = pkcs11_cert_x509public_attributes_count;

            /* Load certificate data from the file system */
//            pObject->size = g_cert_def_2_device.cert_template_size;
//            pObject->data = &g_cert_def_2_device;
            pObject->flags = 0;
            pObject->config = &slot_ctx->cfg_zone;
        }
    }
#endif
    else
    {
        PKCS11_DEBUG("Unrecognized object type: %s", argv[0]);
    }
    return rv;
}

static CK_RV pkcs11_config_parse_slot_file(pkcs11_slot_ctx_ptr slot_ctx, int argc, char * argv[])
{
    CK_RV rv = CKR_GENERAL_ERROR;
    int i;

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp(argv[i], "interface"))
        {
            rv = pkcs11_config_parse_interface(slot_ctx, argv[i + 1]);
        }
        else if (!strcmp(argv[i], "freeslots"))
        {
            rv = pkcs11_config_parse_freeslots(slot_ctx, argv[i + 1]);
        }
        else if (!strcmp(argv[i], "object"))
        {
            rv = pkcs11_config_parse_object(slot_ctx, argv[i + 1]);
        }
    }
    return rv;
}

static CK_RV pkcs11_config_parse_object_file(pkcs11_slot_ctx_ptr slot_ctx, CK_BYTE slot, int argc, char * argv[])
{
    CK_RV rv;
    int i;
    pkcs11_object_ptr pObject = NULL;
    bool privkey = FALSE;

    rv = pkcs11_object_alloc(&pObject);
    if (!rv && pObject)
    {
        pObject->slot = slot;
        pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        pObject->config = &slot_ctx->cfg_zone;
        memset(pObject->name, 0, sizeof(pObject->name));
    }

    for (i = 0; i < argc; i += 2)
    {
        if (!strcmp(argv[i], "type"))
        {
            if (!strcmp(argv[i + 1], "private"))
            {
                privkey = TRUE;
                pkcs11_config_init_private(pObject, "", 0);
            }
            else if (!strcmp(argv[i + 1], "public"))
            {
                pkcs11_config_init_public(pObject, "", 0);
            }
            //if (!strcmp(argv[i + 1], "certificate"))
            //{
            //}
        }
        else if (!strcmp(argv[i], "label"))
        {
            strncpy(pObject->name, argv[i + 1], sizeof(pObject->name));
        }
    }

    if (!rv && privkey)
    {
        /* Have to create a public copy of private keys */
        pkcs11_object_ptr pPubkey = NULL;
        rv = pkcs11_object_alloc(&pPubkey);
        if (!rv && pPubkey)
        {
            pPubkey->slot = slot;
            pPubkey->flags = pObject->flags;
            pPubkey->config = &slot_ctx->cfg_zone;
            pkcs11_config_init_public(pPubkey, pObject->name, strlen(pObject->name));
        }
        else
        {
            pkcs11_object_free(pObject);
        }
    }


    return rv;
}

CK_RV pkcs11_config_cert(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    return CKR_OK;
}

CK_RV pkcs11_config_key(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    FILE* fp;
    char *objtype = "";
    char filename[200];
    int i;

    /* Find a free slot that matches the object type */

    for (i = 0; i < 16; i++)
    {
        if (pSlot->flags & (1 << i))
        {
            uint8_t keytype = (ATCA_KEY_CONFIG_KEY_TYPE_MASK & pSlot->cfg_zone.KeyConfig[i]) >> ATCA_KEY_CONFIG_KEY_TYPE_SHIFT;
            bool privkey = (ATCA_KEY_CONFIG_PRIVATE_MASK & pSlot->cfg_zone.KeyConfig[i]) ? TRUE : FALSE;

            if (CKO_PRIVATE_KEY == pObject->class_id)
            {
                if ((4 == keytype) && privkey)
                {
                    pObject->slot = i;
                    pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
                    pkcs11_config_init_private(pObject, pLabel->pValue, pLabel->ulValueLen);
                    objtype = "private";
                    break;
                }
            }
            else if (CKO_PUBLIC_KEY == pObject->class_id)
            {
                if ((4 == keytype) && !privkey)
                {
                    pObject->slot = i;
                    pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
                    pkcs11_config_init_public(pObject, pLabel->pValue, pLabel->ulValueLen);
                    objtype = "public";
                    break;
                }
            }
        }
    }

    if (i < 16)
    {
        (void)snprintf(filename, sizeof(filename), "%s%d.%d.conf", pLibCtx->config_path, pSlot->slot_id, i);
        fp = fopen(filename, "wb");
        if (fp)
        {
            fprintf(fp, "type = %s\n", objtype);
            fprintf(fp, "label = %s\n", pObject->name);
            fclose(fp);
        }
    }
    return CKR_OK;
}

CK_RV pkcs11_config_remove_object(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject)
{
    char filename[200];

    (void)snprintf(filename, sizeof(filename), "%s%d.%d.conf", pLibCtx->config_path, pSlot->slot_id, pObject->slot);

    remove(filename);

    return CKR_OK;
}

/* Load configuration from the filesystem */
CK_RV pkcs11_config_load_objects(pkcs11_slot_ctx_ptr slot_ctx)
{
    FILE* fp;
    char* buffer;
    size_t buflen;
    char* argv[PKCS11_MAX_OBJECTS_ALLOWED + 1];
    int argc = 0;
    char filename[200];
    int i;
    int j;
    pkcs11_lib_ctx_ptr pLibCtx = pkcs11_get_context();
    CK_RV rv;

    /* Open the general library configuration */
    fp = fopen(ATCA_LIBRARY_CONF, "rb");
    if (fp)
    {
        buflen = pkcs11_config_load_file(fp, &buffer);
        fclose(fp);
        fp = NULL;

        if (0 < buflen)
        {
            if (0 < (argc = pkcs11_config_parse_buffer(buffer, buflen, sizeof(argv) / sizeof(argv[0]), argv)))
            {
                if (strcmp("filestore", argv[0]) == 0)
                {
                    buflen = strlen(argv[1]);
                    memcpy(pLibCtx->config_path, argv[1], buflen);

                    if (pLibCtx->config_path[buflen - 1] != '/')
                    {
                        pLibCtx->config_path[buflen++] = '/';
                    }
                    pLibCtx->config_path[buflen] = '\0';
                }
            }
            else
            {
                PKCS11_DEBUG("Failed to parse the configuration file: %s", ATCA_LIBRARY_CONF);
            }
            pkcs11_os_free(buffer);
        }
    }

    rv = 0;
    for (i = 0; i < PKCS11_MAX_SLOTS_ALLOWED && !rv; i++)
    {
        (void)snprintf(filename, sizeof(filename), "%s%d.conf", pLibCtx->config_path, i);
        fp = fopen(filename, "rb");

        if (fp)
        {
            buflen = pkcs11_config_load_file(fp, &buffer);
            fclose(fp);
            fp = NULL;

            if (0 < buflen)
            {
                if (0 < (argc = pkcs11_config_parse_buffer(buffer, buflen, sizeof(argv) / sizeof(argv[0]), argv)))
                {
                    rv = pkcs11_config_parse_slot_file(slot_ctx, argc, argv);
                }
                else
                {
                    PKCS11_DEBUG("Failed to parse the slot configuration file");
                }
                pkcs11_os_free(buffer);
            }
        }

        for (j = 0; j < 16; j++)
        {
            (void)snprintf(filename, sizeof(filename), "%s%d.%d.conf", pLibCtx->config_path, i, j);
            fp = fopen(filename, "rb");
            if (fp)
            {
                buflen = pkcs11_config_load_file(fp, &buffer);
                fclose(fp);
                fp = NULL;

                /* Remove the slot from the free list*/
                slot_ctx->flags &= ~(1 << j);

                if (0 < buflen)
                {
                    if (0 < (argc = pkcs11_config_parse_buffer(buffer, buflen, sizeof(argv) / sizeof(argv[0]), argv)))
                    {
                        rv = pkcs11_config_parse_object_file(slot_ctx, j, argc, argv);
                    }
                    else
                    {
                        PKCS11_DEBUG("Failed to parse the slot configuration file");
                    }
                    pkcs11_os_free(buffer);
                }
            }
        }
    }


    return rv;
}

#endif

/* Function to load/specify device configurations depending on platform */
CK_RV pkcs11_config_load(pkcs11_slot_ctx_ptr slot_ctx)
{
    pkcs11_object_ptr pObject;
    CK_RV rv = CKR_OK;

#if PKCS11_MONOTONIC_ENABLE
    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (pObject)
        {
            /* Hardware Feature */
            pObject->slot = 0;
            pObject->name = "counter";
            pObject->class_id = CKO_HW_FEATURE;
            pObject->class_type = CKH_MONOTONIC_COUNTER;
            pObject->attributes = pkcs11_object_monotonic_attributes;
            pObject->count = pkcs11_object_monotonic_attributes_count;
            pObject->size = 4;
            pObject->config = &slot_ctx->cfg_zone;
        }
    }
#endif

    if (CKR_OK == rv)
    {
        rv = pkcs11_config_load_objects(slot_ctx);
    }

    return rv;
}


/** @} */
