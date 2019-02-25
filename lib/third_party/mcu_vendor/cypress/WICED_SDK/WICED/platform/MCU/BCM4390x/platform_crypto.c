/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#include <string.h>
#include "platform_crypto.h"
#include "RTOS/wwd_rtos_interface.h"

/* is_buf_aligned macro works only for "align" values those are power of 2 */
#define is_buf_aligned(buf, align)  ((((unsigned long) (buf) & (align-1) ) == 0) ? 1 : 0)

static unsigned int is_crypto_inited = WICED_FALSE;

void wiced_hwcrypto_init( void )
{
    if ( is_crypto_inited == WICED_FALSE )
    {
        platform_hwcrypto_init( );
        is_crypto_inited = WICED_TRUE;
    }
}

#if defined(MBEDTLS_AES_ALT)

void wiced_hw_aes_init( wiced_hw_aes_context *ctx )
{
    memset( ctx, 0, sizeof(wiced_hw_aes_context) );
    return;
}

void wiced_hw_aes_free( wiced_hw_aes_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }

    memset( ctx, 0, sizeof(wiced_hw_aes_context) );

    return;
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */

int wiced_hw_aes_setkey_enc( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits )
{
    /* AES supported key lengths are 128bits, 192 bits and 256 bits. return Invalid key length error for
       key length other than the supported lengths */
    if ( keybits != AES128_KEYSIZE_BITS && keybits != AES192_KEYSIZE_BITS && keybits != AES256_KEYSIZE_BITS )
    {
        return -0x0020;
    }

    hw_aes_setkey_enc( ctx, (unsigned char *) key, keybits );

    return PLATFORM_SUCCESS;
}

/*
 * AES key schedule (same for encryption or decryption, as hardware handles schedule)
 *
 */

int wiced_hw_aes_setkey_dec( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits )
{
    /* AES supported key lengths are 128bits, 192 bits and 256 bits. return Invalid key length error for
       key length other than the supported lengths */
    if ( keybits != AES128_KEYSIZE_BITS && keybits != AES192_KEYSIZE_BITS && keybits != AES256_KEYSIZE_BITS )
    {
        return -0x0020;
    }

    hw_aes_setkey_dec( ctx, (unsigned char *) key, keybits );

    return PLATFORM_SUCCESS;
}

/*
 * AES-ECB block encryption
 */

void wiced_hw_aes_encrypt( wiced_hw_aes_context *ctx, const unsigned char input[ 16 ], unsigned char output[ 16 ] )
{
    hw_aes_crypt_ecb( ctx, 1, input, output );
    return;
}

/*
 * AES-ECB block decryption
 */

void wiced_hw_aes_decrypt( wiced_hw_aes_context *ctx,
        const unsigned char input[16],
        unsigned char output[16] )
{
    hw_aes_crypt_ecb( ctx, 0, input, output );
    return;
}

/*
 * AES-ECB block encryption/decryption
 */

int wiced_hw_aes_crypt_ecb( wiced_hw_aes_context *ctx, int mode, const unsigned char input[ 16 ], unsigned char output[ 16 ] )
{
    unsigned char aligned_inbuf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;

    if ( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf,input,16);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_aes_crypt_ecb( ctx, mode, hwcrypto_inbuf, hwcrypto_outbuf );

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, 16);
    }

    return PLATFORM_SUCCESS;
}

/*
 * AES-CBC buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cbc( wiced_hw_aes_context *ctx, int mode, size_t length, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    unsigned char aligned_inbuf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[16] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;

    /* AES input length should be multiple of 16, if input length is not
       multiple of 16, return Invalid data input length error */
    if ( length % AES_BLOCK_SIZE )
    {
        return ( -0x0022 );
    }

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    while ( length != 0 )
    {
        if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( aligned_inbuf, input, 16);
            hwcrypto_inbuf = aligned_inbuf;
        }
        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            hwcrypto_outbuf = aligned_outbuf;
        }

        hw_aes_crypt_cbc( ctx, mode, 16, iv, hwcrypto_inbuf, hwcrypto_outbuf );

        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( output, hwcrypto_outbuf, 16);
        }

        input += 16;
        output += 16;
        length -= 16;

        hwcrypto_inbuf = input;
        hwcrypto_outbuf = output;
    }

    return PLATFORM_SUCCESS;
}

/*
 * AES-CFB128 buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cfb128( wiced_hw_aes_context *ctx, int mode, size_t length, size_t *iv_off, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *inbuf_base = NULL;
    unsigned char *outbuf_base = NULL;

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        inbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) inbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy( aligned_inbuf, input, length);
        hwcrypto_inbuf = aligned_inbuf;
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        outbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        hwcrypto_outbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) outbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
    }

    hw_aes_crypt_cfb( ctx, mode, length, (uint32_t*) iv_off, iv, hwcrypto_inbuf, hwcrypto_outbuf );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, length);
    }

    free( inbuf_base );
    free( outbuf_base );

    return PLATFORM_SUCCESS;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */

int wiced_hw_aes_crypt_cfb8( wiced_hw_aes_context *ctx, int mode, size_t length, unsigned char iv[ 16 ], const unsigned char *input, unsigned char *output )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *inbuf_base = NULL;
    unsigned char *outbuf_base = NULL;

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        inbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) inbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy( aligned_inbuf, input, length);
        hwcrypto_inbuf = aligned_inbuf;
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        outbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        hwcrypto_outbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) outbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
    }

    hw_aes_crypt_cfb( ctx, mode, length, 0, iv, hwcrypto_inbuf, hwcrypto_outbuf );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, length);
    }

    free( inbuf_base );
    free( outbuf_base );

    return PLATFORM_SUCCESS;
}

/*
 * AES-CTR buffer encryption/decryption
 */
int wiced_hw_aes_crypt_ctr( wiced_hw_aes_context *ctx, size_t length, size_t *nc_off, unsigned char nonce_counter[ 16 ], unsigned char stream_block[ 16 ], const unsigned char *input, unsigned char *output )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *inbuf_base = NULL;
    unsigned char *outbuf_base = NULL;

    UNUSED_PARAMETER( nc_off );
    UNUSED_PARAMETER( stream_block );

    if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        inbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) inbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy( aligned_inbuf, input, length);
        hwcrypto_inbuf = aligned_inbuf;
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        outbuf_base = (unsigned char *)calloc( length + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        hwcrypto_outbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) outbuf_base, HWCRYPTO_ALIGNMENT_BYTES );
    }

    hw_aes_crypt_ctr(ctx, HW_AES_ENCRYPT, length, nonce_counter, hwcrypto_inbuf, hwcrypto_outbuf);

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, length);
    }

    free( inbuf_base );
    free( outbuf_base );

    return PLATFORM_SUCCESS;
}
#endif /* defined(MBEDTLS_AES_ALT) */

#if defined(MBEDTLS_DES_ALT)
/********************DES START*************************/
void wiced_hw_des_init( wiced_hw_des_context *ctx )
{
    memset( ctx, 0, sizeof(wiced_hw_des_context) );
    return;
}

void wiced_hw_des_free( wiced_hw_des_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }

    memset( ctx, 0, sizeof(wiced_hw_des_context) );

    return;
}
void wiced_hw_des3_init( wiced_hw_des_context *ctx )
{
    memset( ctx, 0, sizeof(wiced_hw_des_context) );
    return;
}

void wiced_hw_des3_free( wiced_hw_des_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }

    memset( ctx, 0, sizeof(wiced_hw_des_context) );

    return;
}

int wiced_hw_des_setkey( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE ] )
{
    hw_des_setkey( ctx, (unsigned char*) key );
    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_set2key( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 2 ] )
{
    hw_des3_set2key( ctx, (unsigned char*)key );
    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_set3key( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 3 ] )
{
    hw_des3_set3key( ctx, (unsigned char*)key );
    return PLATFORM_SUCCESS;
}

int wiced_hw_des_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] )
{
    uint32_t mode = ctx->direction;
    unsigned char aligned_inbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;

    if ( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf,input,8);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_des_crypt_ecb( ctx, mode, hwcrypto_inbuf, hwcrypto_outbuf );

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, 8);
    }

    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] )
{
    uint32_t mode = ctx->direction;
    unsigned char aligned_inbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;

    if ( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf,input,8);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    mode = ( mode == HW_AES_DECRYPT ) ? HW_AES_ENCRYPT : HW_AES_DECRYPT;

    hw_des3_crypt_ecb( ctx, mode, hwcrypto_inbuf, hwcrypto_outbuf );

    if ( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, 8);
    }

    return PLATFORM_SUCCESS;
}

int wiced_hw_des_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output )
{
    unsigned char aligned_inbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;

    if( length % 8 )
    {
        return PLATFORM_ERROR;
    }
    mode = ( mode == HW_DES_DECRYPT ) ? HW_DES_ENCRYPT : HW_DES_DECRYPT;

    while(length != 0 )
    {
        if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( aligned_inbuf, input, 8);
            hwcrypto_inbuf = aligned_inbuf;
        }
        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            hwcrypto_outbuf = aligned_outbuf;
        }

        hw_des_crypt_cbc( ctx, mode, 8, iv, hwcrypto_inbuf, hwcrypto_outbuf );

        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( output, hwcrypto_outbuf, 8);
        }

        input += 8;
        output += 8;
        length -= 8;

        hwcrypto_inbuf = input;
        hwcrypto_outbuf = output;
    }

    return PLATFORM_SUCCESS;
}

int wiced_hw_des3_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output )
{
    unsigned char aligned_inbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[8] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_outbuf = output;
    const unsigned char *hwcrypto_inbuf = input;

    if( length % 8 )
    {
        return PLATFORM_ERROR;
    }

    mode = ( mode == HW_DES_DECRYPT ) ? HW_DES_ENCRYPT : HW_DES_DECRYPT;

    while(length != 0 )
    {
        if( !is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( aligned_inbuf, input, 8);
            hwcrypto_inbuf = aligned_inbuf;
        }
        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            hwcrypto_outbuf = aligned_outbuf;
        }

        hw_des3_crypt_cbc( ctx, mode, 8, iv, hwcrypto_inbuf, hwcrypto_outbuf );

        if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            memcpy( output, hwcrypto_outbuf, 8);
        }

        input += 8;
        output += 8;
        length -= 8;

        hwcrypto_inbuf = input;
        hwcrypto_outbuf = output;
    }

    return PLATFORM_SUCCESS;
}
#endif /* #if defined(MBEDTLS_DES_ALT) */
/********************DES END*************************/

/********************MD5 STARTS*************************/

#if defined(MBEDTLS_MD5_ALT)
void wiced_hw_md5_init( wiced_hw_md5_context *ctx )
{
    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    ctx->hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );

    return;
}

void wiced_hw_md5_free( wiced_hw_md5_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    return;
}

void wiced_hw_md5_clone( wiced_hw_md5_context *dst, const wiced_hw_md5_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_md5_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_md5_starts( wiced_hw_md5_context *ctx )
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;

    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(md5_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_md5_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;

    return;
}

void wiced_hw_md5_update( wiced_hw_md5_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }

        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_md5_starts( &ctx->hw_ctx, input, 64 );
        }
        else
        {
            memcpy(buf + ctx->last, input, fill);
            platform_hwcrypto_md5_starts( &ctx->hw_ctx, buf, 64 );
        }

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    /* md5_update hwcrypto api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( ctx->flags == HASH_UPDATE &&
            (ctx->last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->last + ilen;

        if( ctx->last )
        {
            memcpy( buf, ctx->left_over_bytes, ctx->last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_md5_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_md5_update( &ctx->hw_ctx, input, process_bytes );
        }
        else
        {
            memcpy( buf + ctx->last, input, process_bytes - ctx->last );
            platform_hwcrypto_md5_update( &ctx->hw_ctx, buf, process_bytes );
        }
        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->last;
        ilen  = ilen - process_bytes + ctx->last;
        total_bytes_to_process -= process_bytes;
        ctx->last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_md5_update( &ctx->hw_ctx, input, process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_md5_update( &ctx->hw_ctx, buf, process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_md5_finish( wiced_hw_md5_context *ctx, unsigned char output[ MD5_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[MD5_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    unsigned char *hwcrypto_inbuf = ctx->left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;

    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;


    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    if ( ctx->total < 64 )
    {

        payloadbuf = (unsigned char *)calloc( ctx->total + MD5_HASH_SIZE + MD5_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

        platform_hwcrypto_md5( hwcrypto_inbuf, ctx->total, hwcrypto_outbuf, payloadbuf_aligned );

        free( payloadbuf );
    }
    else
    {
        platform_hwcrypto_md5_finish( &ctx->hw_ctx, hwcrypto_inbuf, ctx->last, hwcrypto_outbuf );
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, MD5_HASH_SIZE);
    }

    return;
}

void wiced_md5_hmac_starts( wiced_hw_md5_hmac_context * ctx, const unsigned char *key, uint32_t keylen )
{
    memset( &ctx->ctx.hw_ctx, 0, sizeof( md5_context_t ) );
    memset( ctx, 0, sizeof( wiced_hw_md5_hmac_context ) );

    ctx->ctx.hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->ctx.payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_md5_hmac_starts( &ctx->ctx.hw_ctx, key, keylen );

    return;
}

void wiced_md5_hmac_update( wiced_hw_md5_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    ctx->ctx.total += ilen;

    /* md5_hmac_update api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( (ctx->ctx.last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->ctx.last + ilen;

        if( ctx->ctx.last )
        {
            memcpy( buf, ctx->ctx.left_over_bytes, ctx->ctx.last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_md5_hmac_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        /* even if input buffer is aligned, if residue bytes left from previous update call, we can not avoid memcpy */
        if( ctx->ctx.last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
        }
        else
        {
            memcpy( buf + ctx->ctx.last, input, process_bytes - ctx->ctx.last );
            platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
        }

        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->ctx.last;
        ilen  = ilen - process_bytes + ctx->ctx.last;
        total_bytes_to_process -= process_bytes;
        ctx->ctx.last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_md5_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_md5_hmac_finish( wiced_hw_md5_hmac_context * ctx, unsigned char output[ MD5_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[MD5_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_inbuf = ctx->ctx.left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->ctx.last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    platform_hwcrypto_md5_hmac_finish( &ctx->ctx.hw_ctx, hwcrypto_inbuf, (int32_t) ctx->ctx.last, hwcrypto_outbuf );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, MD5_HASH_SIZE);
    }

    return;
}

void wiced_md5_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    wiced_hw_md5_hmac_context ctx;
    /* platform HW Crypto full hmac API "platform_hwcrypto_md5_hmac"
       is supporting key size up to 16Bytes only. But HMAC key size can be
       upto 64-Bytes. So using incremental method. */
    wiced_md5_hmac_starts( &ctx, key, (uint32_t) keylen);
    wiced_md5_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_md5_hmac_finish( &ctx, output );

    return;
}

void wiced_hw_md5_process( wiced_hw_md5_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_md5( const unsigned char *input, size_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char aligned_outbuf[MD5_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;
    unsigned char *buf = NULL;
    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        buf = (unsigned char *)calloc( ilen + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) buf, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy(aligned_inbuf, input, ilen);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    payloadbuf = (unsigned char *)calloc( ilen + MD5_HASH_SIZE + MD5_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
    payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_md5( hwcrypto_inbuf, ilen, hwcrypto_outbuf, payloadbuf_aligned );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, MD5_HASH_SIZE);
    }
    free( payloadbuf );
    free( buf );

    return;
}
#endif /* defined(MBEDTLS_MD5_ALT) */

/********************MD5 END*************************/

/********************SHA256 STARTS*************************/

#if defined(MBEDTLS_SHA256_ALT)
void wiced_hw_sha256_init( wiced_hw_sha256_context *ctx )
{
    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    ctx->hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );

    return;
}

void wiced_hw_sha256_free( wiced_hw_sha256_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    return;
}

void wiced_hw_sha256_clone( wiced_hw_sha256_context *dst, const wiced_hw_sha256_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_sha256_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_sha256_starts( wiced_hw_sha256_context *ctx, int is224 )
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;

    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;
    ctx->hw_ctx.is224 = is224;
}

void wiced_hw_sha256_update( wiced_hw_sha256_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }

        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha2_starts( &ctx->hw_ctx, input, 64, ctx->hw_ctx.is224 );
        }
        else
        {
            memcpy(buf + ctx->last, input, fill);
            platform_hwcrypto_sha2_starts( &ctx->hw_ctx, buf, 64, ctx->hw_ctx.is224 );
        }

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    /* sha2_update api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( ctx->flags == HASH_UPDATE &&
            (ctx->last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->last + ilen;

        if( ctx->last )
        {
            memcpy( buf, ctx->left_over_bytes, ctx->last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_sha2_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        /* even if input buffer is aligned, if residue bytes left from previous update call, we can not avoid memcpy */
        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha2_update( &ctx->hw_ctx, input, process_bytes );
        }
        else
        {
            memcpy( buf + ctx->last, input, process_bytes - ctx->last );
            platform_hwcrypto_sha2_update( &ctx->hw_ctx, buf, process_bytes );
        }
        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->last;
        ilen  = ilen - process_bytes + ctx->last;
        total_bytes_to_process -= process_bytes;
        ctx->last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_sha2_update( &ctx->hw_ctx, input, process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_sha2_update( &ctx->hw_ctx, buf, process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_sha256_finish( wiced_hw_sha256_context *ctx, unsigned char output[ SHA256_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[SHA256_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    unsigned char *hwcrypto_inbuf = ctx->left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;

    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    if ( ctx->total < 64 )
    {
        payloadbuf = (unsigned char *)calloc( ctx->total + SHA256_HASH_SIZE + SHA_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

        platform_hwcrypto_sha2( hwcrypto_inbuf, ctx->total, hwcrypto_outbuf, payloadbuf_aligned, ctx->hw_ctx.is224 );

        free( payloadbuf );
    }
    else
    {
        platform_hwcrypto_sha2_finish( &ctx->hw_ctx, hwcrypto_inbuf, ctx->last, hwcrypto_outbuf );
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, SHA256_HASH_SIZE);
    }

    return;
}

void wiced_sha256_hmac_starts( wiced_hw_sha256_hmac_context * ctx, const unsigned char *key, uint32_t keylen , int32_t is224)
{
    memset( &ctx->ctx.hw_ctx, 0, sizeof(sha256_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha256_hmac_context) );

    ctx->ctx.hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->ctx.payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_sha2_hmac_starts( &ctx->ctx.hw_ctx, key, keylen, is224 );

    return;
}

void wiced_sha256_hmac_update( wiced_hw_sha256_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    ctx->ctx.total += ilen;

    /* sha256_hmac_update api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( (ctx->ctx.last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->ctx.last + ilen;

        if( ctx->ctx.last )
        {
            memcpy( buf, ctx->ctx.left_over_bytes, ctx->ctx.last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_sha2_hmac_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        /* even if input buffer is aligned, if residue bytes left from previous update call, we can not avoid memcpy */
        if( ctx->ctx.last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
        }
        else
        {
            memcpy( buf + ctx->ctx.last, input, process_bytes - ctx->ctx.last );
            platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
        }

        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->ctx.last;
        ilen  = ilen - process_bytes + ctx->ctx.last;
        total_bytes_to_process -= process_bytes;
        ctx->ctx.last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_sha2_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_sha256_hmac_finish( wiced_hw_sha256_hmac_context * ctx, unsigned char output[ SHA256_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[SHA256_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_inbuf = ctx->ctx.left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->ctx.last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    platform_hwcrypto_sha2_hmac_finish( &ctx->ctx.hw_ctx, hwcrypto_inbuf, (int32_t) ctx->ctx.last, hwcrypto_outbuf );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, SHA256_HASH_SIZE);
    }

    return;
}

void wiced_sha256_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], int32_t is224 )
{
    wiced_hw_sha256_hmac_context ctx;
    /* platform HW Crypto full hmac API "platform_hwcrypto_sha2_hmac"
       is supporting key size up to 32Bytes only. But HMAC key size can be
       upto 64-Bytes. So using incremental method. */
    wiced_sha256_hmac_starts( &ctx, key, (uint32_t) keylen , is224);
    wiced_sha256_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_sha256_hmac_finish( &ctx, output );
    return;
}

void wiced_hw_sha256_process( wiced_hw_sha256_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_sha256( const unsigned char *input, size_t ilen,unsigned char output[32], int is224 )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char aligned_outbuf[SHA256_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;
    unsigned char *buf = NULL;
    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        buf = (unsigned char *)calloc( ilen + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) buf, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy(aligned_inbuf, input, ilen);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    payloadbuf = (unsigned char *)calloc( ilen + SHA256_HASH_SIZE + SHA_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
    payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_sha2( hwcrypto_inbuf, ilen, hwcrypto_outbuf, payloadbuf_aligned, is224 );


    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, SHA256_HASH_SIZE);
    }
    free( payloadbuf );
    free( buf );

    return;
}
#endif /* defined(MBEDTLS_SHA256_ALT) */
/********************SHA256 END*************************/

/********************SHA1 STARTS*************************/

#if defined(MBEDTLS_SHA1_ALT)

void wiced_hw_sha1_init( wiced_hw_sha1_context *ctx )
{
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    ctx->hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );
    return;
}

void wiced_hw_sha1_free( wiced_hw_sha1_context *ctx )
{
    if ( ctx == NULL )
    {
        return;
    }
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    return;
}

void wiced_hw_sha1_clone( wiced_hw_sha1_context *dst, const wiced_hw_sha1_context *src )
{
    uint8_t *payload_buffer = dst->hw_ctx.payload_buffer;

    memcpy(dst,src,sizeof(wiced_hw_sha1_context));
    dst->hw_ctx.payload_buffer = payload_buffer;
}

void wiced_hw_sha1_starts( wiced_hw_sha1_context *ctx)
{
    uint8_t *payload_buffer = ctx->hw_ctx.payload_buffer;
    /* reinitializing the context structures in _starts also
       to handle the sequence of calls init->starts->update->finish
       and after this again starts->update->finish->free
       ( w/o invoking free for first sequence, second set is called)
       storing aligned payload_buffer and restoring it after memset */
    memset( &ctx->hw_ctx, 0, sizeof(sha1_context_t) );
    memset( ctx, 0, sizeof(wiced_hw_sha1_context) );

    ctx->hw_ctx.payload_buffer = payload_buffer;
    ctx->flags = HASH_STARTS;

    return;
}

void wiced_hw_sha1_update( wiced_hw_sha1_context *ctx, const unsigned char *input, size_t ilen )
{
    uint32_t fill = 0;
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    fill = 64 - ctx->last;
    ctx->total += ilen;

    if ( ctx->flags == HASH_STARTS && ctx->total >= 64)
    {
        /* first 64-bytes available, we can call starts crypto
           API */
        if ( ctx->last > 0)
        {
            /* first copy the previously stored bytes  to
               aligned buf and then from input buffer*/
            memcpy(buf, ctx->left_over_bytes, ctx->last);
        }

        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha1_starts( &ctx->hw_ctx, input, 64 );
        }
        else
        {
            memcpy(buf + ctx->last, input, fill);
            platform_hwcrypto_sha1_starts( &ctx->hw_ctx, buf, 64 );
        }

        input += fill;
        ilen  -= fill;

        ctx->last = 0;
        ctx->flags = HASH_UPDATE;
    }

    /* sha1_update hwcrypto api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( ctx->flags == HASH_UPDATE &&
            (ctx->last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->last + ilen;

        if( ctx->last )
        {
            memcpy( buf, ctx->left_over_bytes, ctx->last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_sha1_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        /* even if input buffer is aligned, if residue bytes left from previous update call, we can not avoid memcpy */
        if( ctx->last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha1_update( &ctx->hw_ctx, input, process_bytes );
        }
        else
        {
            memcpy( buf + ctx->last, input, process_bytes - ctx->last );
            platform_hwcrypto_sha1_update( &ctx->hw_ctx, buf, process_bytes );
        }

        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->last;
        ilen  = ilen - process_bytes + ctx->last;
        total_bytes_to_process -= process_bytes;
        ctx->last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_sha1_update( &ctx->hw_ctx, input, process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_sha1_update( &ctx->hw_ctx, buf, process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->left_over_bytes + ctx->last, input, ilen );
        ctx->last = ctx->last + ilen;
    }

    return;
}

void wiced_hw_sha1_finish( wiced_hw_sha1_context *ctx, unsigned char output[ SHA1_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[SHA1_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_inbuf = ctx->left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;
    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    if ( ctx->total < 64 )
    {
        payloadbuf = (unsigned char *)calloc( ctx->total + SHA1_HASH_SIZE + SHA_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

        platform_hwcrypto_sha1( hwcrypto_inbuf, ctx->total, hwcrypto_outbuf, payloadbuf_aligned );

        free( payloadbuf );
    }
    else
    {
        platform_hwcrypto_sha1_finish( &ctx->hw_ctx, hwcrypto_inbuf, ctx->last, hwcrypto_outbuf );
    }
    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, SHA1_HASH_SIZE);
    }

    return;
}

void wiced_sha1_hmac_starts( wiced_hw_sha1_hmac_context * ctx, const unsigned char *key, uint32_t keylen )
{
    memset( &ctx->ctx.hw_ctx, 0, sizeof( sha1_context_t ) );
    memset( ctx, 0, sizeof( wiced_hw_sha1_hmac_context ) );

    ctx->ctx.hw_ctx.payload_buffer = (unsigned char*) ROUND_UP( ( unsigned long ) ctx->ctx.payload_buffer, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_sha1_hmac_starts( &ctx->ctx.hw_ctx, key, keylen );

    return;
}

void wiced_sha1_hmac_update( wiced_hw_sha1_hmac_context * ctx, const unsigned char *input, uint32_t ilen )
{
    unsigned char buf[HWCRYPTO_BLOCK_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned long total_bytes_to_process = 0;
    unsigned int residue = 0;
    unsigned int process_bytes = 0;

    ctx->ctx.total += ilen;

    /* sha1_hmac_update api needs atleast 64-bytes and should be multiple of 64Bytes */
    if( (ctx->ctx.last + ilen) >= 64 )
    {
        total_bytes_to_process = ctx->ctx.last + ilen;

        if( ctx->ctx.last )
        {
            memcpy( buf, ctx->ctx.left_over_bytes, ctx->ctx.last );
        }

        /* upto 64 bytes multiple, data will be processed in this update call, remaining
         * bytes (that is lesser than 64Bytes) will be processed in next update or finish call */
        residue = total_bytes_to_process % 64;
        total_bytes_to_process -= residue;

        /* at a time, maximum of HWCRYPTO_BLOCK_SIZE bytes can be sent to platform_hwcrypto_sha1_hmac_update */
        process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

        /* even if input buffer is aligned, if residue bytes left from previous update call, we can not avoid memcpy */
        if( ctx->ctx.last == 0 && is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
        {
            platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
        }
        else
        {
            memcpy( buf + ctx->ctx.last, input, process_bytes - ctx->ctx.last );
            platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
        }

        /* process_bytes may include left over bytes of previous update call also
         * So move input pointer by "process_bytes - ctx->ctx.last", similarly ilen is decremented */
        input = input + process_bytes - ctx->ctx.last;
        ilen  = ilen - process_bytes + ctx->ctx.last;
        total_bytes_to_process -= process_bytes;
        ctx->ctx.last = 0;

        while( total_bytes_to_process > 0 )
        {
            process_bytes = total_bytes_to_process < HWCRYPTO_BLOCK_SIZE ? total_bytes_to_process : HWCRYPTO_BLOCK_SIZE;

            if( is_buf_aligned( input, HWCRYPTO_ALIGNMENT_BYTES ) )
            {
                platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, input, (int32_t)process_bytes );
            }
            else
            {
                memcpy( buf, input, process_bytes );
                platform_hwcrypto_sha1_hmac_update( &ctx->ctx.hw_ctx, buf, (int32_t)process_bytes );
            }
            input = input + process_bytes;
            ilen  = ilen - process_bytes;
            total_bytes_to_process -= process_bytes;
        }
    }

    if ( ilen > 0 )
    {
        /* Store the residue bytes ( remaining bytes after processing 64 byte chunks).
           and these bytes will be processed in next update or in finish call */
        memcpy( ctx->ctx.left_over_bytes + ctx->ctx.last, input, ilen );
        ctx->ctx.last = ctx->ctx.last + ilen;
    }

    return;
}

void wiced_sha1_hmac_finish( wiced_hw_sha1_hmac_context * ctx, unsigned char output[ SHA1_HASH_SIZE ] )
{
    unsigned char aligned_inbuf[64] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char aligned_outbuf[SHA1_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    unsigned char *hwcrypto_inbuf = ctx->ctx.left_over_bytes ;
    unsigned char *hwcrypto_outbuf = output;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(aligned_inbuf, hwcrypto_inbuf, ctx->ctx.last);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    platform_hwcrypto_sha1_hmac_finish( &ctx->ctx.hw_ctx, hwcrypto_inbuf, (int32_t) ctx->ctx.last, hwcrypto_outbuf );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy( output, hwcrypto_outbuf, SHA1_HASH_SIZE);
    }

    return;
}

void wiced_sha1_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] )
{
    wiced_hw_sha1_hmac_context ctx;

    /* platform HW Crypto full hmac API "platform_hwcrypto_sha1_hmac"
       is supporting key size up to 20Bytes only. But HMAC key size can be
       upto 64-Bytes. So using incremental method. */
    wiced_sha1_hmac_starts( &ctx, key, (uint32_t) keylen);
    wiced_sha1_hmac_update( &ctx, input, (uint32_t) ilen );
    wiced_sha1_hmac_finish( &ctx, output );

    return;

}

void wiced_hw_sha1_process( wiced_hw_sha1_context *ctx, const unsigned char data[ 64 ] )
{
    UNUSED_PARAMETER( ctx );
    UNUSED_PARAMETER( data );

    return;
}

void wiced_hw_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] )
{
    unsigned char *aligned_inbuf = NULL;
    unsigned char aligned_outbuf[SHA1_HASH_SIZE] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    const unsigned char *hwcrypto_inbuf = input;
    unsigned char *hwcrypto_outbuf = output;
    unsigned char *buf = NULL;
    unsigned char *payloadbuf = NULL;
    unsigned char *payloadbuf_aligned = NULL;

    if( !is_buf_aligned( hwcrypto_inbuf, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        buf = (unsigned char *)calloc( ilen + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
        aligned_inbuf =  (unsigned char*) ROUND_UP( ( unsigned long ) buf, HWCRYPTO_ALIGNMENT_BYTES );
        memcpy(aligned_inbuf, input, ilen);
        hwcrypto_inbuf = aligned_inbuf;
    }

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        hwcrypto_outbuf = aligned_outbuf;
    }

    payloadbuf = (unsigned char *)calloc( ilen + SHA1_HASH_SIZE + SHA_PADDING_MAX + HWCRYPTO_ALIGNMENT_BYTES - 1, 1 );
    payloadbuf_aligned = (unsigned char*) ROUND_UP( ( unsigned long ) payloadbuf, HWCRYPTO_ALIGNMENT_BYTES );

    platform_hwcrypto_sha1( hwcrypto_inbuf, ilen, hwcrypto_outbuf, payloadbuf_aligned );

    if( !is_buf_aligned( output, HWCRYPTO_ALIGNMENT_BYTES ) )
    {
        memcpy(output, hwcrypto_outbuf, SHA1_HASH_SIZE);
    }
    free( payloadbuf );
    free( buf );

    return;
}
#endif /* #if defined(MBEDTLS_SHA256_ALT) */
/********************SHA1 END*************************/
