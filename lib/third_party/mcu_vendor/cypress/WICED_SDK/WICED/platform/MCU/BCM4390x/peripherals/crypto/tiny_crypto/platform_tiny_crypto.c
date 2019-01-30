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

/** @file
 * Broadcom SPU-M Interface
 */

#include "typedefs.h"
#include <siutils.h>
#include <platform_peripheral.h>
#include "wiced_osl.h"
#include <crypto_core.h>
#include <crypto_api.h>
#include "cr4.h"
#include "platform_cache.h"
#include "../platform_map.h"
#include "hndsoc.h"
#include <platform_toolchain.h>
#include "RTOS/wwd_rtos_interface.h"

#define PLATFORM_CRYPTO_MUTEX_GET()       host_rtos_lock_mutex( &crypto_mutex )
#define PLATFORM_CRYPTO_MUTEX_RELEASE()   host_rtos_unlock_mutex( &crypto_mutex )
#define PLATFORM_CRYPTO_MUTEX_INIT()   host_rtos_init_mutex( &crypto_mutex )

/******************************************************
 *                      Macros
 ******************************************************/

#define initialize_sctx2(sctx2, inbound, order, crypt_algo, crypt_mode, crypt_optype, hash_algo, hash_mode, hash_optype) \
( \
    sctx2   =   (unsigned) ( inbound  << SCTX2_INBOUND_SHIFT ) | \
                ( order << SCTX2_ORDER_SHIFT) | \
                ( crypt_mode << SCTX2_CRYPT_MODE_SHIFT ) | \
                ( crypt_algo << SCTX2_CRYPT_ALGO_SHIFT ) | \
                ( crypt_optype << SCTX2_CRYPT_OPTYPE_SHIFT ) | \
                ( hash_algo << SCTX2_HASH_ALGO_SHIFT ) | \
                ( hash_mode << SCTX2_HASH_MODE_SHIFT ) | \
                ( hash_optype << SCTX2_HASH_OPTYPE_SHIFT ) \
)

/******************************************************
 *                    Constants
 ******************************************************/

#define BYTES_IN_WORD                   4
#define BITS_IN_BYTE                    8
#define WORD_ALIGNMENT_BYTES_MASK       3U
#define CRYPTO_REG_ADDR                 PLATFORM_CRYPTO_REGBASE(0)
#define MAX_DMA_BUFFER_SIZE             (D64_CTRL2_BC_USABLE_MASK + 1)
#define CRYPTO_STATUS_SIZE              8
/* 1 (header) +  4 (payload) + 1 (padded hash input) 1 (status) */
#define MAX_TX_DMADESCRIPTOS            ( 7 + 1 )
/* 1 (header) +  4 (payload) + 2 (aligned payload buffers) + 1 (hashoutput) + 1 (status) */
#define MAX_RX_DMADESCRIPTOS            ( 9 + 1 )
#define SPUM_STATUS_SIZE                BYTES_IN_WORD
#define OUTPUT_HDR_SIZE                 ( 12 + CRYPTO_STATUS_SIZE )
#define OUTPUT_HDR_CACHE_ALIGNED_SIZE   PLATFORM_L1_CACHE_ROUND_UP( OUTPUT_HDR_SIZE )
#define HWCRYPTO_UNUSED                 0


/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
    MH_IDX = 0,
    EH_IDX,
    SCTX1_IDX,
    SCTX2_IDX,
    SCTX3_IDX,
    HASH_KEY0_IDX,
    HASH_KEY1_IDX,
    HASH_KEY2_IDX,
    HASH_KEY3_IDX,
    HASH_KEY4_IDX,
    HASH_KEY5_IDX,
    HASH_KEY6_IDX,
    HASH_KEY7_IDX,
    CRYPT_KEY0_IDX,
    CRYPT_KEY1_IDX,
    CRYPT_KEY2_IDX,
    CRYPT_KEY3_IDX,
    CRYPT_KEY4_IDX,
    CRYPT_KEY5_IDX,
    CRYPT_KEY6_IDX,
    CRYPT_KEY7_IDX,
    CRYPT_IV0_IDX,
    CRYPT_IV1_IDX,
    CRYPT_IV2_IDX,
    CRYPT_IV3_IDX,
    BDESC_MAC_IDX,
    BDESC_CRYPTO_IDX,
    BDESC_IV_IDX,
    BD_SIZE_IDX,
    SPUM_HDR_SIZE,
} spu_combo_hdr_index_t;

typedef enum
{
    HDR=0,
    PAYLOAD,
    STATUS,
} dmadesc_index_t;

typedef struct bdesc_header
{
    uint32_t mac;
    uint32_t crypt;
    uint32_t iv;

} bdesc_header_t;

typedef struct bd_header
{
    uint32_t size;
} bd_header_t;

typedef struct dma_list
{
    uint32_t addr;
    uint32_t len;
} dma_list_t;

typedef struct
{
    uint8_t     start_aligned_buffer[ HWCRYPTO_ALIGNMENT_BYTES ];
    uint8_t     end_aligned_buffer[ HWCRYPTO_ALIGNMENT_BYTES ];
    uint8_t     aligned_hash_output_buffer[ HMAC256_OUTPUT_SIZE ];
    uint32_t    start_copy_byte_count;
    uint32_t    end_copy_byte_count;
    uint32_t    hash_output_byte_count;
} aligned_buffers_t;


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static void hwcrypto_compute_hmac_inner_outer_hashcontext( uint8_t* key, uint32_t keylen, uint8_t i_key_pad[ ], uint8_t o_key_pad[ ] );
static void hwcrypto_unprotected_blocking_dma_transfer( int txendindex, int rxendindex, dma64dd_t input_descriptors[ ], dma64dd_t output_descriptors[ ] );
static uint32_t hwcrypto_split_dma_data( uint32_t size, uint8_t* src, dma64dd_t* dma_descriptor );
static void hwcrypto_dcache_clean_dma_input( int txendindex, int rxendindex, dma64dd_t input_desc[ ], dma64dd_t output_desc[ ] );
static void hwcrypto_dcache_invalidate_dma_output( int rxendindex, dma64dd_t output_desc[ ] );
static platform_result_t populate_spum_header( crypto_cmd_t* cmd, uint32_t total_size, uint32_t* spum_header_size );
static void populate_input_descriptors( uint32_t* total_size, crypto_cmd_t* cmd, uint32_t* num_input_descriptors );
static void populate_hash_payload_padding(uint32_t* num_payload_descriptors, uint32_t* total_size);
static void populate_output_descriptors( crypto_cmd_t *cmd, uint32_t output_size, aligned_buffers_t* aligned_buffer, uint32_t* num_output_descriptors, uint8_t* output_header);
static void hw_aes_crypt(hw_aes_context_t* ctx, uint32_t length, unsigned char iv[16], const unsigned char* input, unsigned char* output);
static void hw_des_crypt(hw_des_context_t* ctx, uint32_t length, unsigned char iv[ DES_IV_LEN ], const unsigned char* input, unsigned char* output);
static platform_result_t platform_hwcrypto_sha2_internal( sha256_context_t* ctx, uint8_t* source, uint32_t length);
static platform_result_t platform_hwcrypto_sha1_internal( sha1_context_t* ctx, uint8_t* source, uint32_t length );
static platform_result_t platform_hwcrypto_md5_internal( md5_context_t* ctx, uint8_t* source, uint32_t length );

/******************************************************
 *               Variables Definitions
 ******************************************************/
static host_mutex_type_t crypto_mutex;

static volatile crypto_regs_t* cryptoreg;

static dma64dd_t PLATFORM_DMA_DESCRIPTORS_SECTION( input_descriptors  )[ MAX_TX_DMADESCRIPTOS ] ALIGNED(HWCRYPTO_DESCRIPTOR_ALIGNMENT_BYTES);
static dma64dd_t PLATFORM_DMA_DESCRIPTORS_SECTION( output_descriptors )[ MAX_RX_DMADESCRIPTOS ] ALIGNED(HWCRYPTO_DESCRIPTOR_ALIGNMENT_BYTES);
static uint32_t  spum_header[ PLATFORM_L1_CACHE_ROUND_UP( SPUM_HDR_SIZE ) ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
static uint8_t   status[ PLATFORM_L1_CACHE_ROUND_UP( SPUM_STATUS_SIZE ) ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
static uint8_t   padding_buffer[ SHA_BLOCK_SIZE + SHA_BLOCK_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

/******************************************************
 *               Function Definitions
 ******************************************************/

static void hwcrypto_core_enable( void )
{
    osl_wrapper_enable( (void*) PLATFORM_CRYPTO_MASTER_WRAPPER_REGBASE(0x0) );
}

/* Initialize the Crypto Core */
void platform_hwcrypto_init( void )
{

    PLATFORM_CRYPTO_MUTEX_INIT();
    PLATFORM_CRYPTO_MUTEX_GET();
    hwcrypto_core_enable( );
    cryptoreg = (crypto_regs_t*) CRYPTO_REG_ADDR;

    /* We are using 32 bit integers(To program The SPU-M packet) and
     * Char Array (Input payload), this is Mixed mode .
     * See Appendix H on Endianness Support for details
     */
    cryptoreg->spum_ctrl.raw = ( SPUM_BIGENDIAN << SPUM_OUTPUT_FIFO_SHIFT ) | ( SPUM_BIGENDIAN << SPUM_INPUT_FIFO_SHIFT );
    PLATFORM_CRYPTO_MUTEX_RELEASE();
}

/** form the SPU-M header consisting of
     +--------------------------+
     | Header                   |
     | (MH + EH + SCTX0 + SCTX1 |
     |  SCTX2 + SCTX3)          |
     +--------------------------+
     | BufferDescriptor (BDESC) |
     +--------------------------+
     | Buffer Data (BD)         |
     +--------------------------+

 * @param[in]   cmd                 hwcrypto command
 * @param[in]   total_size          size (in bytes)of input payload
 * @param[out]  spum_header_size    size (in bytes)of SPUM header formed by this function
 */
static platform_result_t populate_spum_header( crypto_cmd_t* cmd, uint32_t total_size, uint32_t* spum_header_size  )
{

    uint32_t        sctx_size;
    uint8_t*        spum_header_ptr;
    bd_header_t*    bd;
    bdesc_header_t* bdesc;

    if ( total_size > HWCRYPTO_MAX_PAYLOAD_SIZE )
        return PLATFORM_ERROR;

    spum_header[ MH_IDX ]    = MH_SCTX | MH_BDESC | MH_BD;
    spum_header[ EH_IDX ]    = HWCRYPTO_UNUSED;
    spum_header[ SCTX2_IDX ] = cmd->msg.sctx2.raw;

    sctx_size = SCTX1_SIZE + SCTX2_SIZE + SCTX3_SIZE;
    spum_header_ptr = ( uint8_t* )spum_header + ( ( SCTX3_IDX + 1 ) * BYTES_IN_WORD );

    if ( 0 != cmd->msg.sctx2.bits.hash_algo )
    {
        if ( 0 != cmd->hash_key_len )
        {
            /* Copy hash_key/hash_state to SCTX */
            memcpy( spum_header_ptr, cmd->hash_key, cmd->hash_key_len );
            sctx_size       += cmd->hash_key_len;
            spum_header_ptr += cmd->hash_key_len;
        }

        spum_header[ SCTX3_IDX ] |= (uint32_t) ( cmd->msg.sctx3.bits.icv_size << SCTX3_ICV_SHIFT );
    }

    if ( 0 != cmd->msg.sctx2.bits.crypt_algo )
    {
        if ( 0 != cmd->crypt_key_len )
        {
            /* Copy crypt key to SCTX */
            memcpy( spum_header_ptr, cmd->crypt_key, cmd->crypt_key_len );
            spum_header_ptr += cmd->crypt_key_len;
            sctx_size       += cmd->crypt_key_len;
        }

        if ( 0 != cmd->crypt_iv_len )
        {
            /* Copy crypt IV to SCTX */
            memcpy( spum_header_ptr, cmd->crypt_iv, cmd->crypt_iv_len );
            spum_header_ptr += cmd->crypt_iv_len;
            sctx_size       += cmd->crypt_iv_len;
            spum_header[ SCTX3_IDX ] |= ( SCTX_IV << SCTX3_IV_FLAGS_SHIFT );
        }
    }

    /* Form BDESC */
    bdesc           = ( bdesc_header_t* ) spum_header_ptr;
    bdesc->mac      = cmd->msg.bdesc.mac.length_mac;
    bdesc->crypt    = cmd->msg.bdesc.crypto.length_crypto;
    bdesc->iv       = HWCRYPTO_UNUSED;
    spum_header_ptr += sizeof( bdesc_header_t );

    /* Form BD */
    bd = ( bd_header_t* ) spum_header_ptr;
    bd->size = ( ( unsigned ) ( total_size << 16 ) & 0xffff0000 ) | ( cmd->msg.bd.prev_length & 0xffff );

    *spum_header_size = MH_SIZE + ECH_SIZE + sctx_size + sizeof( bdesc_header_t ) + sizeof( bd_header_t );
    spum_header[ SCTX1_IDX ] = ( sctx_size ) / BYTES_IN_WORD;

    return PLATFORM_SUCCESS;

}

/** Form The input descriptors for DMA
 * @param[in]   spum_header_size      size of spum_header
 * @param[in]   total_size            size in bytes of the input payload
 * @param[in]   cmd                   hwcrypto cmd
 * @param[out]  num_input_descriptors Total number of input descriptors created by this function
 *
 */
static void populate_input_descriptors( uint32_t* total_size, crypto_cmd_t* cmd, uint32_t* num_input_descriptors)
{
    uint32_t    num_payload_descriptors;
    uint32_t    status_index;

    input_descriptors[ HDR ].ctrl1      = D64_CTRL1_SOF;
    input_descriptors[ HDR ].ctrl2      = MH_SIZE + ECH_SIZE + SCTX1_SIZE + SCTX2_SIZE + SCTX3_SIZE + sizeof( bdesc_header_t ) + sizeof( bd_header_t )
                                            + cmd->crypt_iv_len + cmd->crypt_key_len + cmd->hash_key_len;
    input_descriptors[ HDR ].addrlow    = (uint32_t) spum_header;
    input_descriptors[ HDR ].addrhigh   = HWCRYPTO_UNUSED;

    /* Split total payload into chunks of HWCRYPTO_MAX_PAYLOAD_SIZE buffers */
    num_payload_descriptors             = hwcrypto_split_dma_data( *total_size, ( uint8_t* ) cmd->source, ( dma64dd_t* ) ( &input_descriptors[ PAYLOAD ] ) );

    /* For SHA1/2 Hash , the input has to be padded with 10* to make it
     * multiple of 512 bits refer to
     * From RFC4634: http://tools.ietf.org/html/rfc4634#section-4.1 ,Section 4.1 SHA-224 and SHA-256 and
     * SPU-M User manual Section 4.4.1: SHA-224 and SHA256
     */
    if ( cmd->msg.sctx2.bits.hash_algo == MD5 || cmd->msg.sctx2.bits.hash_algo == SHA1 || cmd->msg.sctx2.bits.hash_algo == SHA224 || cmd->msg.sctx2.bits.hash_algo == SHA256 )
    {
        if ( *total_size != 0 && ( ( input_descriptors[num_payload_descriptors].ctrl2 ) & SHA_BLOCK_SIZE_MASK ) != 0 )
        {
            populate_hash_payload_padding( &num_payload_descriptors, total_size );
        }
    }

    status_index                        = PAYLOAD + num_payload_descriptors;
    *num_input_descriptors               = status_index + 1;

    input_descriptors[ status_index ].ctrl1     = D64_CTRL1_EOF;
    input_descriptors[ status_index ].ctrl2     = SPUM_STATUS_SIZE;
    input_descriptors[ status_index ].addrlow   = (uint32_t) &status;
    input_descriptors[ status_index ].addrhigh  = HWCRYPTO_UNUSED;

    /* Set the descriptor next to the Last descriptor to HWCRYPTO_UNUSED */
    input_descriptors[ status_index + 1 ].ctrl1    = HWCRYPTO_UNUSED;
    input_descriptors[ status_index + 1 ].ctrl2    = HWCRYPTO_UNUSED;
    input_descriptors[ status_index + 1 ].addrlow  = HWCRYPTO_UNUSED;
    input_descriptors[ status_index + 1 ].addrhigh = HWCRYPTO_UNUSED;

}

/* Populate the SHA1/SHA2 Hash payload if not multiple of 64 ( SHA_BLOCK_SIZE )bytes
 * by padding it with 10* and length .
 * From RFC4634: http://tools.ietf.org/html/rfc4634#section-4.1 ,Section 4.1 SHA-224 and SHA-256
 *
 * @param[in]   num_payload_descriptors     Number of TX payload descriptors already used
 *                                          increased by one if additional descriptor is needed
 * @param[in]   total_size                  total_size of input payload, increased by number of
 *                                          bytes of padding needed.
 */
static void populate_hash_payload_padding(uint32_t* num_payload_descriptors, uint32_t* total_size)
{

    uint32_t num_padding_bits;
    uint32_t last_descriptor_index      = *num_payload_descriptors;
    uint32_t next_descriptor_index      = last_descriptor_index + 1;
    uint8_t  *buffer                    = padding_buffer;
    uint32_t msglen_bits                = ( uint32_t )input_descriptors[ last_descriptor_index ].ctrl2 * BITS_IN_BYTE;
    uint32_t unaligned_bits;
    uint32_t unaligned_bytes;

    memset( padding_buffer, 0, sizeof( padding_buffer ) );

    /* From RFC4634: Section 4.1 SHA-224 and SHA-256
     * http://tools.ietf.org/html/rfc4634#section-4.1

       Suppose a message has length L < 2^64.  Before it is input to the
       hash function, the message is padded on the right as follows:

       a.  "1" is appended.  Example: if the original message is
           "01010000", this is padded to "010100001".

       b.  K "0"s are appended where K is the smallest, non-negative
           solution to the equation

                L + 1 + K = 448 (mod 512)

       c.  Then append the 64-bit block that is L in binary representation.
           After appending this block, the length of the message will be a
           multiple of 512 bits.
     */
    unaligned_bits  = ( msglen_bits % 512 );
    unaligned_bytes = unaligned_bits / BITS_IN_BYTE;

    if ( unaligned_bits < 448 )
    {
        num_padding_bits = 448 - unaligned_bits;
    }
    else
    {
        num_padding_bits = 960 - unaligned_bits;
    }

    /* Copy the unaligned bytes from the last descriptor to buffer */
    memcpy( buffer, ( uint8_t* )( input_descriptors[ last_descriptor_index ].addrlow ) +
            ( input_descriptors[ last_descriptor_index ].ctrl2 - unaligned_bytes ),
            unaligned_bytes );

    buffer += unaligned_bytes;

    /* Reduce the size of the last descriptor to reflect the bytes copied */
    input_descriptors[ last_descriptor_index ].ctrl2 -= unaligned_bytes;

    /* If the last descriptor is small enough to fit into the padded buffer
     * replace the last descriptor with the padded buffer
     */
    if ( input_descriptors[ last_descriptor_index ].ctrl2 == 0 )
    {
        next_descriptor_index = last_descriptor_index;
    }

    /* 0x80 = 0b1000 0000
     * Other bits of the buffer are zero as the buffer is zero initialized
     */
    memset( buffer, 0x80, 1 );

    /* Copy length in the last 64 bits
     * actually 32 bits as HWCRYPTO_MAX_PAYLOAD_SIZE < 2^32.
     */
    {
        uint32_t size_in_bits = (uint32_t)( *total_size * BITS_IN_BYTE);
        buffer[ ( num_padding_bits / BITS_IN_BYTE ) + 4 ] = (uint8_t)( ( ( size_in_bits & 0xff000000 ) >> 24 ) & 0xff );
        buffer[ ( num_padding_bits / BITS_IN_BYTE ) + 5 ] = (uint8_t)( ( ( size_in_bits & 0x00ff0000 ) >> 16 ) & 0xff );
        buffer[ ( num_padding_bits / BITS_IN_BYTE ) + 6 ] = (uint8_t)( ( ( size_in_bits & 0x0000ff00 ) >> 8  ) & 0xff );
        buffer[ ( num_padding_bits / BITS_IN_BYTE ) + 7 ] = (uint8_t)( ( ( size_in_bits & 0x000000ff ) >> 0 )  & 0xff );
    }
    input_descriptors[ next_descriptor_index ].ctrl1    = HWCRYPTO_UNUSED;
    /* Unaligned bytes +
     * 8 (0b1000 0000) +
     * num_padding_bytes
     */
    input_descriptors[ next_descriptor_index ].ctrl2    = unaligned_bytes + ( num_padding_bits / BITS_IN_BYTE ) + 8;
    input_descriptors[ next_descriptor_index ].addrlow  = (uint32_t)padding_buffer;
    input_descriptors[ next_descriptor_index ].addrhigh = HWCRYPTO_UNUSED;
    *num_payload_descriptors                            = next_descriptor_index;

    /*
     * 8 (0b1000 0000) +
     * num_padding_bytes
     */
    *total_size                                         += ( num_padding_bits / BITS_IN_BYTE ) + 8;
}

/** Form the output descriptors for DMA
 * @param[in]   cmd                     hwcrypto command
 * @param[in]   output_size             size of output payload in bytes
 * @param[in]   start_aligned_buffer    buffer used to store unaligned bytes from start of output
 * @param[out]  start_copy_byte_count   number of bytes from the start of output that are unaligned to PLATFORM_L1_CACHE_SIZE
 * @param[in]   end_aligned_buffer      buffer used to store unaligned bytes at end of output
 * @param[out]  end_copy_byte_count     number of bytes at the end of output that make the size unaligned to PLATFORM_L1_CACHE_SIZE
 * @param[out]  num_output_descriptors  number of output descriptors created by this function
 * @param[in]   output_header           buffer used to store the header of output message
 */
static void populate_output_descriptors( crypto_cmd_t* cmd, uint32_t output_size, aligned_buffers_t* aligned_buffer,
        uint32_t* num_output_descriptors, uint8_t* output_header)
{
    uint32_t i;
    uint32_t start_offset;
    uint32_t end_offset;
    uint32_t num_payload_descriptors;


    output_descriptors[ HDR ].ctrl1     = D64_CTRL1_SOF;
    output_descriptors[ HDR ].ctrl2     = OUTPUT_HDR_SIZE;
    output_descriptors[ HDR ].addrlow   = ( uint32_t ) output_header;
    output_descriptors[ HDR ].addrhigh  = HWCRYPTO_UNUSED;

    aligned_buffer->start_copy_byte_count   = 0;
    aligned_buffer->end_copy_byte_count     = 0;
    aligned_buffer->hash_output_byte_count  = 0;

    /* If cmd.output is not cache line aligned , copy unaligned bytes from cmd.output to start_aligned_buffer */
    i = HDR + 1;

    start_offset = PLATFORM_L1_CACHE_LINE_OFFSET( cmd->output );

    /* if cmd->output is not cache line aligned */
    if ( start_offset != 0 )
    {
        /* If output_size is <= PLATFORM_L1_CACHE_BYTES, start aligned buffer can contain all of output */
        if ( output_size <= PLATFORM_L1_CACHE_BYTES )
        {
            aligned_buffer->start_copy_byte_count  = output_size;
        }
        /* if output_size is > PLATFORM_L1_CACHE_BYTES, store unaligned bytes from start in start aligned buffer */
        else
        {
            aligned_buffer->start_copy_byte_count  = PLATFORM_L1_CACHE_BYTES - start_offset;
        }

        output_descriptors[ i ].addrlow    = ( uint32_t )aligned_buffer->start_aligned_buffer;
        output_descriptors[ i ].ctrl2      = aligned_buffer->start_copy_byte_count;
        output_descriptors[ i ].ctrl1      = HWCRYPTO_UNUSED;
        output_descriptors[ i ].addrhigh   = HWCRYPTO_UNUSED;
        i++;
    }

    /* Split the remaining payload into chunks of max MAX_DMA_BUFFER_SIZE */
    num_payload_descriptors             = hwcrypto_split_dma_data( ( output_size - aligned_buffer->start_copy_byte_count ),
                                ( uint8_t * ) ( cmd->output + aligned_buffer->start_copy_byte_count ), ( dma64dd_t * ) ( &output_descriptors[ i ] ) );

   i += num_payload_descriptors;

    /* if there is output data still remaining */
    if ( output_size  > ( aligned_buffer->start_copy_byte_count ) )
    {

        /* Find if size of the last output descriptor is Cache Line aligned */
        end_offset = PLATFORM_L1_CACHE_LINE_OFFSET( output_descriptors[ i-1 ].addrlow + output_descriptors[ i-1 ].ctrl2 );

        /* if cmd->output + output_size is not cache line aligned */
        if ( end_offset != 0 )
        {
            /* If last descriptor points to a buffer with size less than a cache line
             * use end aligned buffer instead of last descriptor
             */
            if ( output_descriptors[ i-1 ].ctrl2 <= PLATFORM_L1_CACHE_BYTES )
            {
                aligned_buffer->end_copy_byte_count  = output_descriptors[ i-1 ].ctrl2;
                output_descriptors[ i-1 ].addrlow  = ( uint32_t )aligned_buffer->end_aligned_buffer;
            }

            /* if Last descriptor points to a buffer size greater than a cache line
             * copy the unaligned bytes from last to end aligned buffer */
            else
            {
                uint32_t offset_from_start;
                aligned_buffer->end_copy_byte_count = end_offset;
                offset_from_start = output_descriptors[ i-1 ].ctrl2 - aligned_buffer->end_copy_byte_count;

                /* Reset the size of buffer pointed by last descriptor to make it cache line aligned */
                output_descriptors[ i-1 ].ctrl2      =  ( uint32_t )offset_from_start;

                /* Form a new descriptor for the end_aligned_buffer with end_copy_byte_count */
                output_descriptors[ i ].ctrl1        = HWCRYPTO_UNUSED;
                output_descriptors[ i ].ctrl2        = aligned_buffer->end_copy_byte_count;
                output_descriptors[ i ].addrlow      = ( uint32_t )aligned_buffer->end_aligned_buffer;
                output_descriptors[ i ].addrhigh     = HWCRYPTO_UNUSED;

                i++;
            }
        }
    }

    /* If hash output has to be stored in a separate address */
    if ( ( HWCRYPTO_UNUSED != cmd->msg.sctx2.bits.hash_algo ) && ( cmd->hash_output != NULL ) )
    {
        uint32_t hash_output_byte_count;
        uint8_t* hash_output_buffer;

        hash_output_byte_count        = ( uint32_t ) ( cmd->msg.sctx3.bits.icv_size * BYTES_IN_WORD );
        /* Additional descriptor for Hash output */
        if ( PLATFORM_L1_CACHE_LINE_OFFSET( cmd->hash_output) != 0)
        {
            hash_output_buffer            = aligned_buffer->aligned_hash_output_buffer;
            aligned_buffer->hash_output_byte_count = hash_output_byte_count;
        }
        else
        {
            hash_output_buffer            = cmd->hash_output;
        }

         output_descriptors[ i ].addrlow  = ( uint32_t )hash_output_buffer;
         output_descriptors[ i ].addrhigh = HWCRYPTO_UNUSED;
         output_descriptors[ i ].ctrl2    = hash_output_byte_count;
         output_descriptors[ i ].ctrl1    = HWCRYPTO_UNUSED;
         i++;
    }

    output_descriptors[ i ].ctrl1       = D64_CTRL1_EOF;
    output_descriptors[ i ].ctrl2       = SPUM_STATUS_SIZE;
    output_descriptors[ i ].addrlow     = ( uint32_t ) &status;
    output_descriptors[ i ].addrhigh    =  HWCRYPTO_UNUSED;

    *( uint32_t* )num_output_descriptors  = i + 1;

    /* Set the descriptor next to the Last descriptor to HWCRYPTO_UNUSED */
    output_descriptors[ i + 1 ].ctrl1       = HWCRYPTO_UNUSED;
    output_descriptors[ i + 1 ].ctrl2       = HWCRYPTO_UNUSED;
    output_descriptors[ i + 1 ].addrlow     = HWCRYPTO_UNUSED;
    output_descriptors[ i + 1 ].addrhigh    = HWCRYPTO_UNUSED;
}

/** Creates a SPU-M (HWCrypto) Message and Transfers it to SPU-M
 * using M2MDma.
 * Results of the HWCrypto Operation are stored in the buffers
 * specified in cmd.output
 * @param[in] cmd   :   HWCrypto command
 *
 * @return @ref platform_result_t
 *
 */
platform_result_t platform_hwcrypto_execute( crypto_cmd_t cmd )
{
    uint8       output_header[ OUTPUT_HDR_CACHE_ALIGNED_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);
    uint32_t    spum_header_size        = 0;
    uint32_t    total_size              = 0;
    uint32_t    output_size             = 0;
    uint32_t    num_input_descriptors   = 0;
    uint32_t    num_output_descriptors  = 0;
    aligned_buffers_t   aligned_buffers ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

    /* HWCrypto Engine needs the source and destination to be word aligned */
    if ( ( ( (unsigned int) cmd.source & WORD_ALIGNMENT_BYTES_MASK ) != 0 ) ||
        ( ( (unsigned int) cmd.output & WORD_ALIGNMENT_BYTES_MASK ) != 0 ) )
    {
        wiced_assert( "platform_hwcrypto_execute() : Source/Destination should be word aligned", WICED_FALSE );
        return PLATFORM_ERROR;
    }

    /* Either crypto payload competely overlaps the hash payload
     * or hash payload completely overlaps the crypto payload
     * So total input size is the MAX of length_crypto and length_mac
     */
    total_size  = MAX( cmd.msg.bdesc.crypto.length_crypto, cmd.msg.bdesc.mac.length_mac );
    output_size = 0;

    if ( ( HWCRYPTO_UNUSED != cmd.msg.sctx2.bits.hash_algo ) && ( cmd.hash_output == NULL ) )
    {
        /* User has not provided separate buffer for storing
         * hash output, Hash output is at the end of Crypto output payload
         */
        output_size += ( uint32_t ) ( cmd.msg.sctx3.bits.icv_size * BYTES_IN_WORD );
    }

    /*
     * A SPU-M Message Consists of
     *
     *
            INPUT HEADER                       OUTPUT HEADER
     +-------------------------+         +------------------------+
     | Message Header (MH )    |         | Message Header (MH)    |
     +-------------------------+         +------------------------+
     | Extended Header (EH)    |         | Extended Header (EH)   |
     +-------------------------+         +------------------------+
     | SCTX Header 0  (SCTX0)  |         | Output data : Payload  |
     +-------------------------+         +------------------------+
     | SCTX Header 1  (SCTX1)  |         | Status                 |
     +-------------------------+         +------------------------+
     | SCTX Header 2  (SCTX2)  |
     +-------------------------+
     | BufferDescriptor (BDESC)|
     +-------------------------+
     | Buffer Data (BD)        |
     +-------------------------+
     | Input data : Payload    |
     +-------------------------+
     | Status                  |
     +-------------------------+

     SCTX : Security Context

     */
    PLATFORM_CRYPTO_MUTEX_GET();

    memset( spum_header, 0, sizeof( spum_header ) );
    memset( status, 0, sizeof( status ) );

    populate_input_descriptors( &total_size, &cmd, &num_input_descriptors );

    output_size += total_size;

    /* Form the SCTX Header fields - Section 2.2.2.1 SPU-M data sheet Page 25 Rev 0.31 */
    populate_spum_header( &cmd, total_size, &spum_header_size );

    populate_output_descriptors( &cmd, output_size, &aligned_buffers, &num_output_descriptors, output_header );

    /*
    The DMA Descriptors to SPU-M look like :

        INPUT DMA Descriptors                           OUTPUT DMA Descriptors
    +--------------------------+                    +--------------------------+
    | Header                   |                    | Header                   |
    | (MH + EH + SCTX0 + SCTX1 |                    | (MH + EH + BDA )         |
    |  SCTX2 + SCTX3)          |                    +--------------------------+
    +--------------------------+                    | start_aligned_buffer@    |
    | Payload 1                |                    | (PLATFORM_L1_CACHE_BYTES |
    | (MAX_DMA_BUFFER_SIZE)    |                    +--------------------------+
    +--------------------------+                    | Payload 1                |
    | Payload 2*               |                    | (MAX_DMA_BUFFER_SIZE)    |
    | (MAX_DMA_BUFFER_SIZE)    |                    +--------------------------+
    +--------------------------+                    | Payload 2*               |
    | Payload 3*               |                    | (MAX_DMA_BUFFER_SIZE)    |
    | (MAX_DMA_BUFFER_SIZE)    |                    +--------------------------+
    +--------------------------+                    | Payload 3*               |
    | Payload 4*               |                    | (MAX_DMA_BUFFER_SIZE)    |
    | (MAX_DMA_BUFFER_SIZE)    |                    +--------------------------+
    +--------------------------+                    | Payload 4*               |
    | STATUS                   |                    | (BYTES_IN_WORD)          |
    | (BYTES_IN_WORD)          |                    +--------------------------+
    +--------------------------+                    | end_aligned_buffer@      |
                                                    | (MAX_DMA_BUFFER_SIZE)    |
                                                    +--------------------------+
                                                    | HASH_OUTPUT#             |
                                                    | (BYTES_IN_WORD)          |
                                                    +--------------------------+
                                                    | STATUS                   |
                                                    | (BYTES_IN_WORD)          |
                                                    +--------------------------+
    *
    * * : Payload2/3/4 only required if Payload1/2/3 > MAX_DMA_BUFFER_SIZE
    * # : Hash output present only if cmd.hash_output is Not NULL
    * @ : Start/end aligned buffer is needed to ensure that the Start address and size of DMA is
    *     PLATFORM_L1_CACHE_BYTES aligned
    */

    hwcrypto_unprotected_blocking_dma_transfer( ( int )num_input_descriptors, ( int )num_output_descriptors, input_descriptors, output_descriptors );

    /* If cmd.output is not cache line aligned, copy start_copy_byte_counts from
     * start_aligned buffer to cmd.output
     */
    if ( aligned_buffers.start_copy_byte_count != 0 )
    {
        memcpy( cmd.output, aligned_buffers.start_aligned_buffer, ( size_t )aligned_buffers.start_copy_byte_count );
    }

    /* If ( cmd.output + output_size ) is not cache line aligned, copy end_copy_byte_counts from
     * end_aligned buffer
     */
    if ( aligned_buffers.end_copy_byte_count != 0 )
    {
        memcpy( ( ( uint8_t* )cmd.output + ( output_size - aligned_buffers.end_copy_byte_count ) ), aligned_buffers.end_aligned_buffer,
                ( size_t )aligned_buffers.end_copy_byte_count );
    }

    if ( aligned_buffers.hash_output_byte_count != 0 )
    {
        memcpy( ( uint8_t* )cmd.hash_output, aligned_buffers.aligned_hash_output_buffer, ( size_t )aligned_buffers.hash_output_byte_count );
    }

    PLATFORM_CRYPTO_MUTEX_RELEASE();

    return PLATFORM_SUCCESS;
}

/*
 * Maximum size of a DMA descriptor Buffer is MAX_DMA_BUFFER_SIZE (~16K)
 * if DMA payload size is > MAX_DMA_BUFFER_SIZE,
 * split it to chunks of MAX_DMA_BUFFER_SIZE
 * @param[in]   size            size of payload
 * @param[in]   src             pointer to the payload
 * @param[out]  dma_descriptor  descriptor array
 */
static uint32_t hwcrypto_split_dma_data( uint32_t size, uint8_t* src, dma64dd_t* dma_descriptor )
{
    uint32_t dma_size;
    uint32_t i = 0;

    wiced_assert("DMA payload size Cannot be greater than HWCRYPTO_MAX_PAYLOAD_SIZE", ( size <= HWCRYPTO_MAX_PAYLOAD_SIZE ) );

    while ( size != 0 )
    {
        dma_size = MIN(size, MAX_DMA_BUFFER_SIZE);

        dma_descriptor[ i ].ctrl1    = HWCRYPTO_UNUSED;
        dma_descriptor[ i ].ctrl2    = dma_size;
        dma_descriptor[ i ].addrlow  = ( uint32_t ) src;
        dma_descriptor[ i ].addrhigh = HWCRYPTO_UNUSED;

        size    -= dma_size;
        src     += dma_size;
        i++ ;
    }

    return i;
}

/** Populate the hw_aes_context_t with key and keysize
 * @param[in]   ctx
 * @param[in]   key
 * @param[in]   keysize_bits
 */
void hw_aes_setkey_enc( hw_aes_context_t* ctx, unsigned char* key, uint32_t keysize_bits )
{
    memcpy( ctx->key, key, ( keysize_bits / 8 ) );
    ctx->keylen = keysize_bits;
}

/** Populate the hw_aes_context_t with key and keysize
 * @param[in]   ctx
 * @param[in]   key
 * @param[in]   keysize_bits
 */
void hw_aes_setkey_dec( hw_aes_context_t* ctx, unsigned char* key, uint32_t keysize_bits )
{
    memcpy( ctx->key, key, ( keysize_bits / 8 ) );
    ctx->keylen = keysize_bits;
}

/** AES CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_cbc(hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, unsigned char iv[ AES_IV_LEN ], const unsigned char* input, unsigned char* output)
{
    ctx->direction      =  ( mode == HW_AES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = CBC;

    hw_aes_crypt( ctx, length, iv, input, output );
}

/** AES ECB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data of size AES_BLOCK_SIZE
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_ecb(hw_aes_context_t* ctx, hw_aes_mode_type_t mode, const unsigned char input[ AES_BLOCK_SZ ], unsigned char output[ AES_BLOCK_SZ ])
{
    ctx->direction      =  ( mode == HW_AES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = ECB;

    hw_aes_crypt( ctx, AES_BLOCK_SZ, NULL, input, output );
}

/** AES CFB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_cfb(hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, uint32_t* iv_off, unsigned char iv[ AES_IV_LEN ], const unsigned char* input, unsigned char* output)
{
    UNUSED_PARAMETER( iv_off );

    ctx->direction      =  ( mode == HW_AES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = CFB;

    hw_aes_crypt( ctx, length, iv, input, output );
}

/** AES CTR Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_ctr(hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, unsigned char iv[ AES_IV_LEN ], const unsigned char* input, unsigned char* output)
{
    ctx->direction      =  ( mode == HW_AES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = CTR;

    hw_aes_crypt( ctx, length, iv, input, output );
}

/** AES CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
static void hw_aes_crypt(hw_aes_context_t* ctx, uint32_t length, unsigned char iv[ AES_IV_LEN ], const unsigned char* input, unsigned char* output)
{
    crypto_cmd_t cmd =
    { 0 };
    uint8_t temp_iv[ AES_IV_LEN ];

    unsigned    optype = 0;
    unsigned    order;

    cmd.crypt_key        = ctx->key;
    cmd.crypt_key_len    = ctx->keylen/BITS_IN_BYTE;

    if (iv != NULL)
    {
        cmd.crypt_iv     = iv;
        cmd.crypt_iv_len = AES_IV_LEN;
    }

    switch (cmd.crypt_key_len)
    {
        case 16 : /* AES128 */
            optype = 0;
            break;
        case 24 : /* AES192 */
            optype = 1;
            break;
        case 32 : /* AES256 */
            optype = 2;
            break;
        default:
            wiced_assert("Wrong Key length specified for AES", ( WICED_FALSE ) );
            break;
    }

    order = ( ctx->direction == INBOUND ) ? AUTH_ENCR : ENCR_AUTH;

    initialize_sctx2( cmd.msg.sctx2.raw, ctx->direction, order, AES, ctx->cipher_mode, optype, 0, 0, 0 );
    cmd.source          = ( unsigned char * ) input;
    cmd.output          = output;

    /* SPU-M can handle HWCRYPTO_MAX_PAYLOAD_SIZE at a time */
    while ( length != 0 )
    {
        uint32_t            crypt_size = MIN( length , HWCRYPTO_MAX_PAYLOAD_SIZE );

        cmd.msg.bdesc.crypto.length_crypto  = ( 0xffff & crypt_size );

        /* If source and output are same address, source will be modified after
         * platform_hwcrypto_execute, so back-up the iv.
         */
        if ( ( ( ctx->cipher_mode == CBC ) || ( ctx->cipher_mode == CFB ) ) )
        {
            if ( ctx->direction == INBOUND )
            {
                memcpy( temp_iv, ( ( uint8_t* )cmd.source + crypt_size - AES_IV_LEN ), AES_IV_LEN );
            }
        }

        platform_hwcrypto_execute( cmd );
        length  -= crypt_size;

        if ( ( ( ctx->cipher_mode == CBC ) || ( ctx->cipher_mode == CFB ) ) && ( iv != NULL ) )
        {
            if ( ctx->direction == INBOUND )
            {
                memcpy( iv, temp_iv, AES_IV_LEN );
            }

            if ( ctx->direction == OUTBOUND )
            {
                memcpy( iv, ( ( uint8_t* ) output + crypt_size - AES_IV_LEN ), AES_IV_LEN );
            }
        }

        cmd.source   += crypt_size;
        cmd.output   += crypt_size;
    }
}

/* DES/DES3 56 bit Key encryption/decryption
 * K1=K2=K3*/
void hw_des_setkey( hw_des_context_t* ctx, unsigned char* key )
{
    memcpy( ctx->key, key, ( 64 / 8 ) );
    ctx->keylen = 64;
}

/* DES3 112 bit Key encryption/decryption
 * K3 = K1 */
void hw_des3_set2key(hw_des_context_t* ctx, unsigned char* key)
{
    memcpy( ctx->key, key, ( 128 / 8 ) );
    memcpy((unsigned char*)ctx->key+16, (unsigned char*)key, 8);
    ctx->keylen = 192;
}

/* DES3 168 bit Key encryption/decryption */
void hw_des3_set3key(hw_des_context_t* ctx, unsigned char* key)
{
    memcpy( ctx->key, key, ( 192 / 8 ) );
    ctx->keylen = 192;
}

/* DES ECB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   input       input data of size DES_BLOCK_SIZE
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des_crypt_ecb( hw_des_context_t* ctx, hw_des_mode_type_t mode, const unsigned char input[ DES_BLOCK_SZ ], unsigned char output[ DES_BLOCK_SZ ] )
{
    ctx->direction      =  ( mode == HW_DES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = ECB;
    ctx->crypt_algo     = DES;
    hw_des_crypt( ctx, DES_BLOCK_SZ, NULL, input, output );
}

/* DES CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          iv used for DES
 * @param[in]   input       input data
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des_crypt_cbc( hw_des_context_t* ctx, hw_des_mode_type_t mode, uint32_t length, unsigned char iv[ DES_BLOCK_SZ ], const unsigned char* input, unsigned char* output )
{
    ctx->direction      =  ( mode == HW_DES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = CBC;
    ctx->crypt_algo     = DES;
    hw_des_crypt( ctx, length, iv, input, output );
}

void hw_des3_crypt_ecb( hw_des_context_t* ctx, hw_des_mode_type_t mode, const unsigned char input[ DES_BLOCK_SZ ], unsigned char output[ DES_BLOCK_SZ ] )
{
    ctx->direction      =  ( mode == HW_DES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = ECB;
    ctx->crypt_algo     = _3DES;
    hw_des_crypt( ctx, DES_BLOCK_SZ, NULL, input, output );
}

void hw_des3_crypt_cbc( hw_des_context_t* ctx, hw_des_mode_type_t mode, uint32_t length, unsigned char iv[ DES_BLOCK_SZ ], const unsigned char* input, unsigned char* output )
{
    ctx->direction      =  ( mode == HW_DES_ENCRYPT ) ? OUTBOUND : INBOUND;
    ctx->cipher_mode    = CBC;
    ctx->crypt_algo     = _3DES;
    hw_des_crypt( ctx, length, iv, input, output );
}

/* DES CBC Encryption/Decryption
 * @param[in]   ctx         hw_des_context
 * @param[in]   length      length of input data
 * @param[in]   iv          des_iv used for DES
 * @param[in]   input       input data
 * @param[out]  output      output of the DES encryption/decryption
 */
static void hw_des_crypt(hw_des_context_t* ctx, uint32_t length, unsigned char iv[ DES_IV_LEN ], const unsigned char* input, unsigned char* output)
{
    crypto_cmd_t cmd =
    { 0 };
    uint8_t temp_iv[ DES_IV_LEN ];

    unsigned    optype = 0;
    unsigned    order;

    cmd.crypt_key        = ctx->key;
    cmd.crypt_key_len    = ctx->keylen/BITS_IN_BYTE;

    if (iv != NULL)
    {
        cmd.crypt_iv     = iv;
        cmd.crypt_iv_len = DES_IV_LEN;
    }

    order = ( ctx->direction == INBOUND ) ? AUTH_ENCR : ENCR_AUTH;

    initialize_sctx2( cmd.msg.sctx2.raw, ctx->direction, order, ctx->crypt_algo, ctx->cipher_mode, optype, 0, 0, 0);
    cmd.source          = ( unsigned char* )input;
    cmd.output          = output;

    /* SPU-M can handle HWCRYPTO_MAX_PAYLOAD_SIZE at a time */
    while ( length != 0 )
    {
        uint32_t            crypt_size = MIN( length , HWCRYPTO_MAX_PAYLOAD_SIZE );

        cmd.msg.bdesc.crypto.length_crypto  = ( 0xffff & crypt_size );

        /* If source and output are same address, source will be modified after
         * platform_hwcrypto_execute, so back-up the iv.
         */
        if ( ( ( ctx->cipher_mode == CBC ) || ( ctx->cipher_mode == CFB ) ) )
        {
            if ( ctx->direction == INBOUND )
            {
                memcpy( temp_iv, ( ( uint8_t* )cmd.source + crypt_size - DES_IV_LEN ), DES_IV_LEN );
            }
        }

        platform_hwcrypto_execute( cmd );
        length  -= crypt_size;

        if ( ( ( ctx->cipher_mode == CBC ) || ( ctx->cipher_mode == CFB ) ) && ( iv != NULL ) )
        {
            if ( ctx->direction == INBOUND )
            {
                memcpy( iv, temp_iv, DES_IV_LEN );
            }

            if ( ctx->direction == OUTBOUND )
            {
                memcpy( iv, ( ( uint8_t* )output + crypt_size - DES_IV_LEN ), DES_IV_LEN );
            }
        }

        cmd.source   += crypt_size;
        cmd.output   += crypt_size;
    }
}

/* SHA256HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[in]   is224           0 : SHA256 1: SHA224
 *
 * SPU-M does not support SHA-256 HMAC of > 64K , but it supports SHA-256 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_sha2_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ SHA256_HASH_SIZE ], int32_t is224 )
{
    uint32_t residue;
    sha256_context_t sha256_ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &sha256_ctx, 0, sizeof( sha256_context_t ) );
    platform_hwcrypto_sha2_hmac_starts( &sha256_ctx, key, keylen, is224 );

    residue = ilen;

    while ( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_sha2_hmac_update( &sha256_ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue         -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input           += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_sha2_hmac_finish( &sha256_ctx, ( unsigned char* )input, ( int32_t )residue, output );

    return PLATFORM_SUCCESS;
}

/* SHA256 INIT
 * @param[in]   ctx             sha256_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[in]   is224           1: SHA224 0: SHA256
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_starts( sha256_context_t *ctx, const unsigned char *input, uint32_t ilen, int32_t is224 )
{
    platform_result_t   result;

    if ( ilen % SHA_BLOCK_SIZE != 0 )
    {
        wiced_assert("starts payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );
        return PLATFORM_ERROR;
    }

    ctx->is224          = is224;
    ctx->hash_optype    = HASH_INIT;
    ctx->prev_len       = 0;

    result = platform_hwcrypto_sha2_internal( ctx, ( unsigned char* )input, ilen);

    return result;
}

/* SHA256 UPDATE
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_update( sha256_context_t* ctx, const unsigned char* input, uint32_t ilen )
{
    platform_result_t   result;

    if ( ilen % SHA_BLOCK_SIZE != 0 )
    {
        wiced_assert("update payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );
        return PLATFORM_ERROR;
    }

    ctx->hash_optype    = HASH_UPDT;
    result = platform_hwcrypto_sha2_internal( ctx, ( unsigned char* )input, ilen );
    return result;
}

/* SHA256 FINISH
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha2_finish( sha256_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ] )
{

    platform_result_t   result;
    uint32_t sha2_output_size = ( ctx->is224 ) ? SHA224_HASH_SIZE : SHA256_HASH_SIZE;
    ctx->hash_optype    = HASH_FINAL;
    result = platform_hwcrypto_sha2_internal( ctx, ( unsigned char* )input, ilen );
    memcpy( output, ctx->state, sha2_output_size  );
    return result;
}

/* SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 * @param[in]   is224           1: SHA224 0: SHA256
 */

platform_result_t platform_hwcrypto_sha2_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], int32_t is224)
{
    uint32_t residue;
    sha256_context_t ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &ctx, 0, sizeof( sha256_context_t ) );

    platform_hwcrypto_sha2_starts( &ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE, is224 );
    residue = ilen - HWCRYPTO_MAX_PAYLOAD_SIZE;
    input   +=  HWCRYPTO_MAX_PAYLOAD_SIZE;

    while( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_sha2_update( &ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input   += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_sha2_finish( &ctx, input, residue, output );

    return PLATFORM_SUCCESS;
}

/*
 * SHA2: Perform HASH_INIT/ HASH_UPDT / HASH_FINAL Operation
 *
 * @param[in]   ctx             sha256_context
 * @param[in]   source          pointer to input data/payload
 * @param[in]   length          length of input data/payload
 *
 * HWcrypto Outputs the PAYLOAD + HASH,
 * ctx->payload_buffer has the output payload
 * ctx->state has the HASH result
 */
static platform_result_t platform_hwcrypto_sha2_internal( sha256_context_t* ctx, uint8_t* source, uint32_t length )
{
    crypto_cmd_t cmd =
    { 0 };

    cmd.source                      = source;
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx3.bits.icv_size     = SHA256_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_optype  = ( unsigned )( ctx->hash_optype & 0x3 );
    cmd.msg.bdesc.mac.length_mac    = ( 0xffff & length );
    cmd.msg.sctx2.bits.hash_algo    = ( ctx->is224 ) ? SHA224 : SHA256;

    if ( ctx->hash_optype == HASH_UPDT || ctx->hash_optype == HASH_FINAL )
    {
        cmd.msg.bd.prev_length      = ( unsigned )( ctx->prev_len & 0xffff );
        cmd.hash_key                = ctx->state;
        cmd.hash_key_len            = SHA256_HASH_SIZE;
    }

    /* For SHA-224 SPU-M only takes 7Bytes ICV size for HASH_FINAL
     * But HASH_INIT or HASH_UPDT require 8Bytes ICV size and
     * 8 bytes hash state (stored in cmd.hash_key)
     */
    if ( ( ctx->hash_optype == HASH_FINAL ) && ( ctx->is224 ) )
    {
        cmd.msg.sctx3.bits.icv_size = SHA224_HASH_SIZE / BYTES_IN_WORD;
    }

    platform_hwcrypto_execute( cmd );

    ctx->prev_len += length / BD_PREVLEN_BLOCKSZ;

    return PLATFORM_SUCCESS;
}

/* SHA256 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             sha256_context containing the previous sha result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   is224           1: SHA224 0: SHA256
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_hmac_starts( sha256_context_t* ctx, const unsigned char* key, uint32_t keylen, int32_t is224 )
{
    crypto_cmd_t cmd =
    { 0 };

    /* Form the ipad  */
    /* TODO append/truncate HMAC key to make it 256bits */

    memset( ctx->i_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );
    memset( ctx->o_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );
    ctx->is224 = is224;

    /*
     * InnexHashContext -> key XOR ipad
     * OuterHashContext -> key XOR opad
     * ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c
     */
    hwcrypto_compute_hmac_inner_outer_hashcontext( (unsigned char *)key, keylen, ctx->i_key_pad, ctx->o_key_pad );

    cmd.source                      = ctx->i_key_pad;
    /* TODO change output to source to eliminate need of payload buffer */
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx2.bits.hash_algo    = ( is224 == 1 ) ? SHA224 : SHA256;
    cmd.msg.sctx2.bits.hash_optype  = HASH_INIT;
    cmd.msg.sctx3.bits.icv_size     = SHA256_HASH_SIZE/BYTES_IN_WORD;
    cmd.msg.bdesc.mac.length_mac    = HMAC_INNERHASHCONTEXT_SIZE;
    cmd.msg.bd.prev_length          = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    /* Length (In BD_PREVLEN_BLOCKSZ) of data hashed till now */
    ctx->prev_len   = HMAC_INNERHASHCONTEXT_SIZE / BD_PREVLEN_BLOCKSZ;

    return PLATFORM_SUCCESS;
}

/* SHA256 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */

platform_result_t platform_hwcrypto_sha2_hmac_update( sha256_context_t* ctx, const unsigned char* input, int32_t ilen )
{
    crypto_cmd_t cmd =
    { 0 };

    /* SPU-M User manual Rev0.31 Page 41, Section 3.4.1
     * "Both INIT and UPDT requires data payload to be 64-byte multiples. Neither operation
     *  performs any padding to the payload data."
     */
    wiced_assert("HMAC UPDATE payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );

    cmd.source                          = ( unsigned char* )input;
    cmd.output                          = ctx->payload_buffer;
    cmd.hash_output                     = ctx->state;
    /* Use the Previous output , as key to the next iteration */
    cmd.hash_key                        = ctx->state;
    cmd.hash_key_len                    = SHA256_HASH_SIZE;
    cmd.msg.sctx3.bits.icv_size         = SHA256_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_algo        = ( ctx->is224 ) ? SHA224 : SHA256;

    cmd.msg.sctx2.bits.hash_optype      = HASH_UPDT;
    cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length              = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    ctx->prev_len   += ( unsigned )( ilen / BD_PREVLEN_BLOCKSZ );

    return PLATFORM_SUCCESS;
}

/* SHA256 HMAC FINISH
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha2_hmac_finish( sha256_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ SHA256_HASH_SIZE ] )
{
    crypto_cmd_t cmd =
    { 0 };
    uint8_t     store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + HMAC256_OUTPUT_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) = { 0 };
    uint32_t    sha2_output_size;
    uint8_t     state[ SHA256_HASH_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) = { 0 };
    uint8_t     payload_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + HMAC256_OUTPUT_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) = { 0 };

    memcpy ( state, ctx->state, SHA256_HASH_SIZE );
    if ( ctx->is224 ) /* SHA224 */
    {
        cmd.msg.sctx2.bits.hash_algo = SHA224;
        cmd.msg.sctx3.bits.icv_size  = ( SHA224_HASH_SIZE / BYTES_IN_WORD );
        sha2_output_size             = HMAC224_OUTPUT_SIZE;
    }
    else /* SHA256 */
    {

        cmd.msg.sctx2.bits.hash_algo = SHA256;
        cmd.msg.sctx3.bits.icv_size  = ( SHA256_HASH_SIZE / BYTES_IN_WORD );
        sha2_output_size             = HMAC256_OUTPUT_SIZE;
    }

    cmd.source                       = input;
    cmd.output                       = ctx->payload_buffer;
    cmd.hash_output                  = state;
    cmd.hash_key                     = state;
    cmd.msg.sctx2.bits.hash_optype   = HASH_FINAL;
    cmd.msg.bdesc.mac.length_mac     = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length           = ( unsigned )( ctx->prev_len & 0xffff );
    cmd.hash_key_len                 = SHA256_HASH_SIZE;

    platform_hwcrypto_execute( cmd );

    /* Compute HMAC = HASH(key XOR opad || innerHash) */
    memcpy( &store_buffer[ 0 ], ctx->o_key_pad, HMAC_OUTERHASHCONTEXT_SIZE );
    memcpy( &store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE ], cmd.hash_output, sha2_output_size );
    platform_hwcrypto_sha2( store_buffer, ( HMAC_OUTERHASHCONTEXT_SIZE + sha2_output_size ), output, payload_buffer, ctx->is224 );

    return PLATFORM_SUCCESS;
}

/* SHA256
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 * @param[in]   is224           0 : SHA256 1: SHA224
 */
platform_result_t platform_hwcrypto_sha2(const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], unsigned char* payload_buffer, int32_t is224)
{
    platform_result_t   result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_sha2_incremental( input, ilen, output, is224 );
    }
    else
    {
        crypto_cmd_t cmd =
        { 0 };

        cmd.source                          = ( unsigned char* )input;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.output                          = payload_buffer;
        cmd.hash_output                     = output;
        if ( is224 ) /* SHA224 */
        {
            cmd.msg.sctx2.bits.hash_algo    = SHA224;
            cmd.msg.sctx3.bits.icv_size     = ( SHA224_HASH_SIZE / BYTES_IN_WORD );

        }
        else /* SHA256 */
        {
            cmd.msg.sctx2.bits.hash_algo    = SHA256;
            cmd.msg.sctx3.bits.icv_size     = ( SHA256_HASH_SIZE / BYTES_IN_WORD );
        }

        cmd.msg.bdesc.mac.length_mac        = ( 0xffff & ilen );
        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}

/* SHA256HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 * @param[in]   is224           0 : SHA256 1: SHA224
 */
platform_result_t platform_hwcrypto_sha2_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ SHA256_HASH_SIZE ], unsigned char* payload_buffer, int32_t is224)
{

    platform_result_t result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_sha2_hmac_incremental( key, keylen, input, ilen, output, is224 );
    }
    else
    {
        uint32_t        hmac_key_copy_size;
        crypto_cmd_t    cmd = { 0 };
        uint8           hmac_key_32[ HMAC256_KEY_LEN ] = {'\0'};

        if ( is224 ) /* SHA224 HMAC */
        {
            /* Append / Truncate key to make it 224 bits */
            hmac_key_copy_size              = ( keylen > HMAC224_KEY_LEN ) ? HMAC224_KEY_LEN : keylen;
            cmd.hash_key_len                = HMAC224_KEY_LEN;
            cmd.msg.sctx2.bits.hash_algo    = SHA224;
            cmd.msg.sctx3.bits.icv_size     = SHA224_HASH_SIZE/BYTES_IN_WORD;
        }
        else
        {
            /* Append / Truncate key to make it 256 bits */
            hmac_key_copy_size              = ( keylen > HMAC256_KEY_LEN ) ? HMAC256_KEY_LEN : keylen;
            cmd.hash_key_len                = HMAC256_KEY_LEN;
            cmd.msg.sctx2.bits.hash_algo    = SHA256;
            cmd.msg.sctx3.bits.icv_size     = SHA256_HASH_SIZE/BYTES_IN_WORD;
        }

        memcpy( hmac_key_32, key, hmac_key_copy_size );
        cmd.source                          = ( unsigned char* )input;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.output                          = payload_buffer;
        cmd.hash_output                     = output;
        cmd.hash_key                        = hmac_key_32;
        cmd.msg.sctx2.bits.hash_mode        = HMAC;
        cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );

        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}

/* SHA1HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 *
 * SPU-M does not support SHA1 HMAC of > 64K , but it supports SHA1 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_sha1_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ SHA1_HASH_SIZE ] )
{
    uint32_t residue;
    sha1_context_t sha1_ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &sha1_ctx, 0, sizeof( sha1_context_t ) );
    platform_hwcrypto_sha1_hmac_starts( &sha1_ctx, key, keylen );

    residue = ilen;

    while ( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_sha1_hmac_update( &sha1_ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue         -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input           += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_sha1_hmac_finish( &sha1_ctx, ( unsigned char* )input, ( int32_t )residue, output );

    return PLATFORM_SUCCESS;
}

/* SHA1 INIT
 * @param[in]   ctx             sha1_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_starts( sha1_context_t *ctx, const unsigned char* input, uint32_t ilen )
{
    platform_result_t result;

    if ( ilen % SHA_BLOCK_SIZE != 0 )
    {
        wiced_assert("starts payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );
        return PLATFORM_ERROR;
    }

    /* just do the initialization */
    ctx->hash_optype    = HASH_INIT;
    ctx->prev_len       = 0;


    result = platform_hwcrypto_sha1_internal( ctx, ( unsigned char* )input, ilen );

    return result;
}

/* SHA1 UPDATE
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_update( sha1_context_t* ctx, const unsigned char* input, uint32_t ilen )
{
    platform_result_t   result = PLATFORM_SUCCESS;

    if ( ilen % SHA_BLOCK_SIZE != 0 )
    {
        wiced_assert("update payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );
        return PLATFORM_ERROR;
    }

    ctx->hash_optype    = HASH_UPDT;

    result = platform_hwcrypto_sha1_internal( ctx, ( unsigned char* )input, ilen );

    return result;
}

/* SHA1 FINISH
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1_finish( sha1_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] )
{

    const uint32_t sha1_output_size = SHA1_HASH_SIZE;
    platform_result_t result;

    ctx->hash_optype    = HASH_FINAL;
    result = platform_hwcrypto_sha1_internal( ctx, ( unsigned char* )input, ilen );

    memcpy( output, ctx->state, sha1_output_size  );

    return result;
}

/* SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 */

platform_result_t platform_hwcrypto_sha1_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] )
{
    uint32_t residue;
    sha1_context_t ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &ctx, 0, sizeof( sha1_context_t ) );

    platform_hwcrypto_sha1_starts( &ctx, input, ilen );
    residue = ilen - HWCRYPTO_MAX_PAYLOAD_SIZE;
    input   +=  HWCRYPTO_MAX_PAYLOAD_SIZE;

    while( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_sha1_update( &ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input   += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_sha1_finish( &ctx, input, ilen, output );

    return PLATFORM_SUCCESS;
}

/*
 * SHA1: Perform HASH_INIT/ HASH_UPDT / HASH_FINAL Operation
 *
 * @param[in]   ctx             sha1_context
 * @param[in]   source          pointer to input data/payload
 * @param[in]   length          length of input data/payload
 *
 * HWcrypto Outputs the PAYLOAD + HASH,
 * ctx->payload_buffer has the output payload
 * ctx->state has the HASH result
 */
static platform_result_t platform_hwcrypto_sha1_internal( sha1_context_t* ctx, uint8_t* source, uint32_t length )
{
    platform_result_t result;
    crypto_cmd_t cmd ALIGNED( HWCRYPTO_ALIGNMENT_BYTES ) =
    { 0 };

    uint8_t state_initial[20];

    memcpy(state_initial, ctx->state, sizeof(state_initial));

    cmd.source                      = source;
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx3.bits.icv_size     = SHA1_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_optype  = ( unsigned )( ctx->hash_optype & 0x3 );
    cmd.msg.bdesc.mac.length_mac    = ( 0xffff & length );
    cmd.msg.sctx2.bits.hash_algo    = SHA1;



    if ( ctx->hash_optype == HASH_UPDT || ctx->hash_optype == HASH_FINAL )
    {
        cmd.msg.bd.prev_length      = ( unsigned )( ctx->prev_len & 0xffff );
        cmd.hash_key                = ctx->state;
        cmd.hash_key_len            = SHA1_HASH_SIZE;
    }

    result = platform_hwcrypto_execute( cmd );



    ctx->prev_len += length / BD_PREVLEN_BLOCKSZ;



    return result;
}

/* SHA1 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             sha1_context containing the previous sha result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_hmac_starts( sha1_context_t* ctx, const unsigned char* key, uint32_t keylen )
{
    crypto_cmd_t cmd =
    { 0 };

    /* Form the ipad  */
    /* TODO append/truncate HMAC key to make it 256bits */

    memset( ctx->i_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );
    memset( ctx->o_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );

    /*
     * InnexHashContext -> key XOR ipad
     * OuterHashContext -> key XOR opad
     * ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c
     */
    hwcrypto_compute_hmac_inner_outer_hashcontext( (unsigned char *)key, keylen, ctx->i_key_pad, ctx->o_key_pad );

    cmd.source                      = ctx->i_key_pad;
    /* TODO change output to source to eliminate need of payload buffer */
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx2.bits.hash_algo    = SHA1;
    cmd.msg.sctx2.bits.hash_optype  = HASH_INIT;
    cmd.msg.sctx3.bits.icv_size     = SHA1_HASH_SIZE/BYTES_IN_WORD;
    cmd.msg.bdesc.mac.length_mac    = HMAC_INNERHASHCONTEXT_SIZE;
    cmd.msg.bd.prev_length          = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    /* Length (In BD_PREVLEN_BLOCKSZ) of data hashed till now */
    ctx->prev_len   = HMAC_INNERHASHCONTEXT_SIZE / BD_PREVLEN_BLOCKSZ;

    return PLATFORM_SUCCESS;
}

/* SHA1 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */

platform_result_t platform_hwcrypto_sha1_hmac_update( sha1_context_t* ctx, const unsigned char* input, int32_t ilen )
{
    crypto_cmd_t cmd =
    { 0 };

    /* SPU-M User manual Rev0.31 Page 41, Section 3.4.1
     * "Both INIT and UPDT requires data payload to be 64-byte multiples. Neither operation
     *  performs any padding to the payload data."
     */
    wiced_assert("HMAC UPDATE payload length should be multiple of 64 bytes", ( ( ilen & SHA_BLOCK_SIZE_MASK ) == 0 ) );

    cmd.source                          = ( unsigned char* )input;
    cmd.output                          = ctx->payload_buffer;
    cmd.hash_output                     = ctx->state;
    /* Use the Previous output , as key to the next iteration */
    cmd.hash_key                        = ctx->state;
    cmd.hash_key_len                    = SHA1_HASH_SIZE;
    cmd.msg.sctx3.bits.icv_size         = SHA1_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_algo        = SHA1;

    cmd.msg.sctx2.bits.hash_optype      = HASH_UPDT;
    cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length              = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    ctx->prev_len   += ( unsigned )( ilen / BD_PREVLEN_BLOCKSZ );

    return PLATFORM_SUCCESS;
}

/* SHA1 HMAC FINISH
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1_hmac_finish( sha1_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] )
{
    crypto_cmd_t cmd =
    { 0 };
    uint8_t store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + HMAC256_OUTPUT_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES ) =
    { 0 };
    uint8_t payload_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + HMAC256_OUTPUT_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES ) = { 0 };
    uint32_t    sha1_output_size;
    uint8_t     state[ SHA256_HASH_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) = { 0 };

    memcpy ( state, ctx->state, SHA256_HASH_SIZE );

    cmd.msg.sctx2.bits.hash_algo = SHA1;
    cmd.msg.sctx3.bits.icv_size  = ( SHA1_HASH_SIZE / BYTES_IN_WORD );
    sha1_output_size             = SHA1_HASH_SIZE;

    cmd.source                       = input;
    cmd.output                       = ctx->payload_buffer;
    cmd.hash_output                  = state;
    cmd.hash_key                     = state;
    cmd.msg.sctx2.bits.hash_optype   = HASH_FINAL;
    cmd.msg.bdesc.mac.length_mac     = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length           = ( unsigned )( ctx->prev_len & 0xffff );
    cmd.hash_key_len                 = SHA1_HASH_SIZE;

    platform_hwcrypto_execute( cmd );

    /* Compute HMAC = HASH(key XOR opad || innerHash) */
    memcpy( &store_buffer[ 0 ], ctx->o_key_pad, HMAC_OUTERHASHCONTEXT_SIZE );
    memcpy( &store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE ], cmd.hash_output, sha1_output_size );
    platform_hwcrypto_sha1( store_buffer, ( HMAC_OUTERHASHCONTEXT_SIZE + sha1_output_size ), output, payload_buffer );

    return PLATFORM_SUCCESS;
}

/* SHA1
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 */
platform_result_t platform_hwcrypto_sha1(const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ], unsigned char* payload_buffer )
{
    platform_result_t   result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_sha1_incremental( input, ilen, output );
    }
    else
    {
        crypto_cmd_t cmd =
        { 0 };

        cmd.source                          = ( unsigned char* )input;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.output                          = payload_buffer;
        cmd.hash_output                     = output;

        cmd.msg.sctx2.bits.hash_algo        = SHA1;
        cmd.msg.sctx3.bits.icv_size         = ( SHA1_HASH_SIZE / BYTES_IN_WORD );

        cmd.msg.bdesc.mac.length_mac        = ( 0xffff & ilen );
        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}

/* SHA1HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 *
 */
platform_result_t platform_hwcrypto_sha1_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ SHA1_HASH_SIZE ], unsigned char* payload_buffer )
{

    platform_result_t result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_sha1_hmac_incremental( key, keylen, input, ilen, output );
    }
    else
    {
        uint32_t        hmac_key_copy_size;
        crypto_cmd_t    cmd = { 0 };
        uint8           hmac_key_32[ HMAC160_KEY_LEN ] = {'\0'};

        /* Append / Truncate key to make it 256 bits */
        hmac_key_copy_size              = ( keylen > HMAC160_KEY_LEN ) ? HMAC160_KEY_LEN : keylen;
        cmd.hash_key_len                = HMAC160_KEY_LEN;
        cmd.msg.sctx2.bits.hash_algo    = SHA1;
        cmd.msg.sctx3.bits.icv_size     = SHA1_HASH_SIZE/BYTES_IN_WORD;

        memcpy( hmac_key_32, key, hmac_key_copy_size );
        cmd.source                          = ( unsigned char* )input;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.output                          = payload_buffer;
        cmd.hash_output                     = output;
        cmd.hash_key                        = hmac_key_32;
        cmd.msg.sctx2.bits.hash_mode        = HMAC;
        cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );

        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}

/* MD5HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 *
 * SPU-M does not support MD5 HMAC of > 64K , but it supports MD5 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_md5_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ MD5_HASH_SIZE ] )
{
    uint32_t residue;
    md5_context_t md5_ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &md5_ctx, 0, sizeof( md5_context_t ) );
    platform_hwcrypto_md5_hmac_starts( &md5_ctx, key, keylen );

    residue = ilen;

    while ( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_md5_hmac_update( &md5_ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue         -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input           += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_md5_hmac_finish( &md5_ctx, ( unsigned char* )input, ( int32_t )residue, output );

    return PLATFORM_SUCCESS;
}

/* MD5 INIT
 * @param[in]   ctx             md5_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_starts( md5_context_t *ctx, const unsigned char *input, uint32_t ilen )
{
    platform_result_t   result;

    ctx->hash_optype    = HASH_INIT;
    ctx->prev_len       = 0;

    result = platform_hwcrypto_md5_internal( ctx, ( unsigned char* )input, ilen);

    return result;
}

/* MD5 UPDATE
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             md5_context containing the previous md5 result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_update( md5_context_t* ctx, const unsigned char* input, uint32_t ilen )
{
    platform_result_t   result;

    ctx->hash_optype    = HASH_UPDT;
    result = platform_hwcrypto_md5_internal( ctx, ( unsigned char* )input, ilen );

    return result;
}

/* MD5 FINISH
 * @param[in]   ctx             md5_context containing the previous md5 result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5_finish( md5_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{

    platform_result_t   result;
    const uint32_t md5_output_size = MD5_HASH_SIZE;

    ctx->hash_optype          = HASH_FINAL;

    result = platform_hwcrypto_md5_internal( ctx, ( unsigned char* )input, ilen );

    memcpy( output, ctx->state, md5_output_size  );

    return result;
}

/* SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 */

platform_result_t platform_hwcrypto_md5_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    uint32_t residue;
    md5_context_t ctx ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    memset( &ctx, 0, sizeof( md5_context_t ) );

    platform_hwcrypto_md5_starts( &ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
    residue = ilen - HWCRYPTO_MAX_PAYLOAD_SIZE;
    input   +=  HWCRYPTO_MAX_PAYLOAD_SIZE;

    while( residue > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        platform_hwcrypto_md5_update( &ctx, input, HWCRYPTO_MAX_PAYLOAD_SIZE );
        residue -= HWCRYPTO_MAX_PAYLOAD_SIZE;
        input   += HWCRYPTO_MAX_PAYLOAD_SIZE;
    }

    platform_hwcrypto_md5_finish( &ctx, input, residue, output );

    return PLATFORM_SUCCESS;
}

/*
 * MD5: Perform HASH_INIT/ HASH_UPDT / HASH_FINAL Operation
 *
 * @param[in]   ctx             md5_context
 * @param[in]   source          pointer to input data/payload
 * @param[in]   length          length of input data/payload
 *
 * HWcrypto Outputs the PAYLOAD + HASH,
 * ctx->payload_buffer has the output payload
 * ctx->state has the HASH result
 */
static platform_result_t platform_hwcrypto_md5_internal( md5_context_t* ctx, uint8_t* source, uint32_t length )
{
    crypto_cmd_t cmd =
    { 0 };

    cmd.source                      = source;
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx3.bits.icv_size     = MD5_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_optype  = ( unsigned )( ctx->hash_optype & 0x3 );
    cmd.msg.bdesc.mac.length_mac    = ( 0xffff & length );
    cmd.msg.sctx2.bits.hash_algo    = MD5;

    if ( ctx->hash_optype == HASH_UPDT || ctx->hash_optype == HASH_FINAL )
    {
        cmd.msg.bd.prev_length      = ( unsigned )( ctx->prev_len & 0xffff );
        cmd.hash_key                = ctx->state;
        cmd.hash_key_len            = MD5_HASH_SIZE;
    }

    platform_hwcrypto_execute( cmd );

    ctx->prev_len += length / BD_PREVLEN_BLOCKSZ;

    return PLATFORM_SUCCESS;
}

/* MD5 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             md5_context containing the previous md5 result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_hmac_starts( md5_context_t* ctx, const unsigned char* key, uint32_t keylen )
{
    crypto_cmd_t cmd =
    { 0 };

    /* Form the ipad  */
    /* TODO append/truncate HMAC key to make it 256bits */

    memset( ctx->i_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );
    memset( ctx->o_key_pad, 0, HMAC_INNERHASHCONTEXT_SIZE );

    /*
     * InnexHashContext -> key XOR ipad
     * OuterHashContext -> key XOR opad
     * ipad -> 64 byte array of 0x36, opad -> 64 byte array of 0x5c
     */
    hwcrypto_compute_hmac_inner_outer_hashcontext( (unsigned char *)key, keylen, ctx->i_key_pad, ctx->o_key_pad );

    cmd.source                      = ctx->i_key_pad;
    /* TODO change output to source to eliminate need of payload buffer */
    cmd.output                      = ctx->payload_buffer;
    cmd.hash_output                 = ctx->state;
    cmd.msg.sctx2.bits.hash_algo    = MD5;
    cmd.msg.sctx2.bits.hash_optype  = HASH_INIT;
    cmd.msg.sctx3.bits.icv_size     = MD5_HASH_SIZE/BYTES_IN_WORD;
    cmd.msg.bdesc.mac.length_mac    = HMAC_INNERHASHCONTEXT_SIZE;
    cmd.msg.bd.prev_length          = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    /* Length (In BD_PREVLEN_BLOCKSZ) of data hashed till now */
    ctx->prev_len   = HMAC_INNERHASHCONTEXT_SIZE / BD_PREVLEN_BLOCKSZ;

    return PLATFORM_SUCCESS;
}

/* MD5 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             md5_context containing the previous md5 result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */

platform_result_t platform_hwcrypto_md5_hmac_update( md5_context_t* ctx, const unsigned char* input, int32_t ilen )
{
    crypto_cmd_t cmd =
    { 0 };

    /* SPU-M User manual Rev0.31 Page 41, Section 3.4.1
     * "Both INIT and UPDT requires data payload to be 64-byte multiples. Neither operation
     *  performs any padding to the payload data."
     */
    wiced_assert("HMAC UPDATE payload length should be multiple of 64 bytes", ( ( ilen & MD5_BLOCK_SIZE_MASK ) == 0 ) );

    cmd.source                          = ( unsigned char* )input;
    cmd.output                          = ctx->payload_buffer;
    cmd.hash_output                     = ctx->state;
    /* Use the Previous output , as key to the next iteration */
    cmd.hash_key                        = ctx->state;
    cmd.hash_key_len                    = MD5_HASH_SIZE;
    cmd.msg.sctx3.bits.icv_size         = MD5_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.sctx2.bits.hash_algo        = MD5;

    cmd.msg.sctx2.bits.hash_optype      = HASH_UPDT;
    cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length              = ( unsigned )( ctx->prev_len & 0xffff );

    platform_hwcrypto_execute( cmd );
    ctx->prev_len   += ( unsigned )( ilen / BD_PREVLEN_BLOCKSZ );

    return PLATFORM_SUCCESS;
}

/* MD5 HMAC FINISH
 * @param[in]   ctx             md5_context containing the previous md5 result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5_hmac_finish( md5_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ MD5_HASH_SIZE ] )
{
    crypto_cmd_t cmd =
    { 0 };
    uint8_t store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + MD5_HASH_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES ) =
    { 0 };
    uint8_t payload_buffer[ HMAC_OUTERHASHCONTEXT_SIZE + MD5_HASH_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES ) =
    { 0 };
    uint32_t    md5_output_size;
    uint8_t     state[ SHA256_HASH_SIZE ] ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) = { 0 };

    memcpy ( state, ctx->state, SHA256_HASH_SIZE );

    cmd.msg.sctx2.bits.hash_algo = MD5;
    cmd.msg.sctx3.bits.icv_size  = ( MD5_HASH_SIZE / BYTES_IN_WORD );
    md5_output_size             = MD5_HASH_SIZE;

    cmd.source                       = input;
    cmd.output                       = ctx->payload_buffer;
    cmd.hash_output                  = state;
    cmd.hash_key                     = state;
    cmd.msg.sctx2.bits.hash_optype   = HASH_FINAL;
    cmd.msg.bdesc.mac.length_mac     = ( unsigned )( 0xffff & ilen );
    cmd.msg.bd.prev_length           = ( unsigned )( ctx->prev_len & 0xffff );
    cmd.hash_key_len                 = MD5_HASH_SIZE;

    platform_hwcrypto_execute( cmd );

    /* Compute HMAC = HASH(key XOR opad || innerHash) */
    memcpy( &store_buffer[ 0 ], ctx->o_key_pad, HMAC_OUTERHASHCONTEXT_SIZE );
    memcpy( &store_buffer[ HMAC_OUTERHASHCONTEXT_SIZE ], cmd.hash_output, md5_output_size );
    platform_hwcrypto_md5( store_buffer, ( HMAC_OUTERHASHCONTEXT_SIZE + md5_output_size ), output, payload_buffer );

    return PLATFORM_SUCCESS;
}

/* MD5
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 */
platform_result_t platform_hwcrypto_md5( const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ], unsigned char* payload_buffer )
{
    platform_result_t   result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_md5_incremental( input, ilen, output );
    }
    else
    {
        crypto_cmd_t cmd ALIGNED(HWCRYPTO_ALIGNMENT_BYTES) =
        { 0 };


        cmd.source                          = ( unsigned char* )input;
        cmd.output                          = payload_buffer;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.hash_output                     = output;

        cmd.msg.sctx2.bits.hash_algo        = MD5;
        cmd.msg.sctx3.bits.icv_size         = ( MD5_HASH_SIZE / BYTES_IN_WORD );

        cmd.msg.bdesc.mac.length_mac        = ( 0xffff & ilen );
        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}

/* MD5HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[out]  payload_buffer  buffer to store output payload.
 *                              HWCrypto engine outputs the input payload and appends padding data(if needed).
 *                              Size of this buffer should be nearest 64 multiple from ilen.
 *                              For Ex, if ilen is 100 bytes, payload_buffer size should be 128.
 */
platform_result_t platform_hwcrypto_md5_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen,
        unsigned char output[ MD5_HASH_SIZE ], unsigned char* payload_buffer )
{

    platform_result_t result;

    /* HWCrypto can process only HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time */
    if ( ilen > HWCRYPTO_MAX_PAYLOAD_SIZE )
    {
        result = platform_hwcrypto_md5_hmac_incremental( key, keylen, input, ilen, output );
    }
    else
    {
        uint32_t        hmac_key_copy_size;
        crypto_cmd_t    cmd = { 0 };
        uint8           hmac_key_32[ HMAC128_KEY_LEN ] = {'\0'};

        /* Append / Truncate key to make it 256 bits */
        hmac_key_copy_size              = ( keylen > HMAC128_KEY_LEN ) ? HMAC128_KEY_LEN : keylen;
        cmd.hash_key_len                = HMAC128_KEY_LEN;
        cmd.msg.sctx2.bits.hash_algo    = MD5;
        cmd.msg.sctx3.bits.icv_size     = MD5_HASH_SIZE/BYTES_IN_WORD;

        memcpy( hmac_key_32, key, hmac_key_copy_size );
        cmd.source                          = ( unsigned char* )input;
        /* need to provide a separate payload buffer to h/w through cmd.output,
         * specifically when ilen is not multiple of 64. If input buffer only
         * is used for cmd.output, and if ilen is not multiple of 64, H/W
         * copies input data and padding data to cmd.output buffer. This causes
         * accessing input buffer beyond it's size, leads to memory corruption.
         */
        cmd.output                          = payload_buffer;
        cmd.hash_output                     = output;
        cmd.hash_key                        = hmac_key_32;
        cmd.msg.sctx2.bits.hash_mode        = HMAC;
        cmd.msg.bdesc.mac.length_mac        = ( unsigned )( 0xffff & ilen );

        result = platform_hwcrypto_execute( cmd );

    }

    return result;
}


/* Combo AES128 CBC decryption + SHA256HMAC Authentication
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be decrypted
 * @param[in]   auth_size       Size of data to be authenticated
 * @param[in]   hmac_key        HMAC key
 * @param[in]   hmac_key_len    HMAC key length
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of decryption
 * @param[out]  hash_dest       Result of authentication
 */
platform_result_t platform_hwcrypto_aescbc_decrypt_sha256_hmac(uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size,
        uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* src, uint8_t* crypt_dest, uint8_t* hash_dest)
{
    uint32_t        hmac_key_copy_size;
    uint8           hmac_key_32[ HMAC256_KEY_LEN ] = {'\0'};
    crypto_cmd_t  cmd = { 0 };
    platform_result_t result;

    /* Append / Truncate key to make it 256 bits */
    hmac_key_copy_size = ( hmac_key_len > HMAC256_KEY_LEN ) ? HMAC256_KEY_LEN : hmac_key_len;
    memcpy( hmac_key_32, hmac_key, hmac_key_copy_size );

    cmd.crypt_key       = crypt_key;
    cmd.crypt_key_len   = AES128_KEY_LEN;
    cmd.crypt_iv        = crypt_iv;
    cmd.crypt_iv_len    = AES_IV_LEN;
    cmd.hash_key        = hmac_key_32;
    cmd.hash_key_len    = HMAC256_KEY_LEN;
    cmd.source          = src;
    cmd.output          = crypt_dest;
    cmd.hash_output     = hash_dest;

    initialize_sctx2( cmd.msg.sctx2.raw, INBOUND, AUTH_ENCR, AES, CBC, 0, SHA256, HMAC, 0 );

    cmd.msg.sctx3.bits.icv_size         = SHA256_HASH_SIZE / BYTES_IN_WORD;
    cmd.msg.bdesc.crypto.length_crypto  = ( 0xffff & crypt_size );
    cmd.msg.bdesc.mac.length_mac        = ( 0xffff & auth_size );

    result = platform_hwcrypto_execute( cmd );

    return result;
}

/* Combo SHA256HMAC Authentication + AES128 CBC encryption
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be encrypted
 * @param[in]   auth_size       Size of data to be authenticated
 * @param[in]   hmac_key        HMAC key
 * @param[in]   hmac_key_len    HMAC key length
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of encryption
 * @param[out]  hash_dest       Result of authentication
 */
platform_result_t platform_hwcrypto_sha256_hmac_aescbc_encrypt(uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size,
        uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* src, uint8_t* crypt_dest, uint8_t* hash_dest)
{
    uint32_t        hmac_key_copy_size;
    uint8           hmac_key_32[ HMAC256_KEY_LEN ] = { '\0' };
    crypto_cmd_t  cmd = { 0 };
    platform_result_t result;

    /* Append / Truncate key to make it 256 bits */
    hmac_key_copy_size = MIN( hmac_key_len, HMAC256_KEY_LEN );
    memcpy( hmac_key_32, hmac_key, hmac_key_copy_size );

    cmd.crypt_key       = crypt_key;
    cmd.crypt_key_len   = AES128_KEY_LEN;
    cmd.crypt_iv        = crypt_iv;
    cmd.crypt_iv_len    = AES_IV_LEN;
    cmd.hash_key        = hmac_key_32;
    cmd.hash_key_len    = HMAC256_KEY_LEN;
    cmd.source          = src;
    cmd.output          = crypt_dest;
    cmd.hash_output     = hash_dest;

    initialize_sctx2( cmd.msg.sctx2.raw, OUTBOUND, AUTH_ENCR, AES, CBC, 0, SHA256, HMAC, 0);

    cmd.msg.sctx3.bits.icv_size         = SHA256_HASH_SIZE/BYTES_IN_WORD;
    cmd.msg.bdesc.crypto.length_crypto  = ( 0xffff & crypt_size );
    cmd.msg.bdesc.mac.length_mac        = ( 0xffff & auth_size );

    result = platform_hwcrypto_execute( cmd );

    return result;
}

/* AES128 CBC encryption
 * @Deprecated, use hw_aes_crypt() instead
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be encrypted
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of encryption
 */
platform_result_t platform_hwcrypto_aescbc_encrypt(uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size,
        uint8_t* src, uint8_t* crypt_dest)
{
    rommed_crypto_cmd_t  cmd = { 0 };

    cmd.crypt_key       = crypt_key;
    cmd.crypt_key_len   = AES128_KEY_LEN;
    cmd.crypt_iv        = crypt_iv;
    cmd.crypt_iv_len    = AES_IV_LEN;
    cmd.source          = src;
    cmd.output          = crypt_dest;
    cmd.crypt_size      = crypt_size;
    cmd.inbound         = OUTBOUND;
    cmd.order           = ENCR_AUTH;
    cmd.crypt_algo      = AES;
    cmd.crypt_mode      = CBC;

    platform_hwcrypto_execute( *((crypto_cmd_t *) &cmd) );
    memcpy( crypt_iv, (uint8_t*) ( src + crypt_size - AES_IV_LEN ), AES_IV_LEN );

    return PLATFORM_SUCCESS;
}

/*
 * SHA256 HASH on data size < HWCRYPTO_MAX_PAYLOAD_SIZE
 * @Deprecated, use platform_hwcrypto_sha2() instead
 * @Assumption
 * Size is < HWCRYPTO_MAX_PAYLOAD_SIZE
 * output is aligned to CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT
 */
platform_result_t platform_hwcrypto_sha256_hash(uint8_t *source, uint32_t size, uint8_t *output_payload_buffer, uint8_t *hash_output)
{
    //UNUSED_PARAMETER(output_payload_buffer);
    platform_hwcrypto_sha2(source, size, hash_output, output_payload_buffer, 0);
    return PLATFORM_SUCCESS;
}

/*
 * SHA256 HMAC on data size < HWCRYPTO_MAX_PAYLOAD_SIZE
 * @Deprecated, use platform_hwcrypto_sha2_hmac() instead
 * @Assumption
 * Size is < HWCRYPTO_MAX_PAYLOAD_SIZE
 * output is aligned to CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT
 */

platform_result_t platform_hwcrypto_sha256_hmac( uint8_t *hmac_key, uint32_t keysize, uint8_t *source, uint32_t size,
        uint8_t *payload_buffer, uint8_t *hash_output )
{
    //UNUSED_PARAMETER(payload_buffer);
    platform_hwcrypto_sha2_hmac(hmac_key, keysize, source, size, hash_output, payload_buffer, 0);
    return PLATFORM_SUCCESS;
}

/*
 * Invalidate/Flush DMA descriptors and the data they are pointing to
 * @param[in]   txendindex      number of TX descriptors
 * @param[in]   rxendindex      number of RX descriptors
 * @param[in]   input_desc      array of TX descriptors
 * @param[in]   output_desc     array of RX descriptors
 */
static void hwcrypto_dcache_clean_dma_input( int txendindex, int rxendindex, dma64dd_t input_desc[ ], dma64dd_t output_desc[ ] )
{
    int i;

    platform_dcache_clean_range( ( void* )&input_desc[ 0 ], ( unsigned )( sizeof( input_desc[ 0 ] )*( unsigned )( txendindex ) ) );
    platform_dcache_clean_range( ( void* )&output_desc[ 0 ], ( unsigned )( sizeof( output_desc[ 0 ] )*( unsigned )( rxendindex) ) );
    /* Flush the DMA data to main memory to ensure DMA picks them up
     * Flush the descriptor after the last descriptor too since cryptoreg->dmaregs.tx.ptr
     * points to it
     */
    for ( i = 0; i < txendindex + 1; i++ )
    {
        platform_dcache_clean_range( ( void* )input_desc[ i ].addrlow, input_desc[ i ].ctrl2);
    }

    for ( i = 0; i < rxendindex + 1; i++ )
    {
        platform_dcache_clean_range( ( uint32_t* )output_desc[ i ].addrlow, output_desc[ i ].ctrl2 );
    }
}

/*
 * Cache Invalidate the DMA output data
 * @param[in]   output_desc    Array of DMA output descriptors
 * @param[in]   rxendindex     Number of DMA output descriptors
 */
static void hwcrypto_dcache_invalidate_dma_output( int rxendindex, dma64dd_t output_desc[ ] )
{
    int i;
    /* Prepares the cache for reading the finished DMA data. *
     * Flush the descriptor after the last descriptor too since cryptoreg->dmaregs.rx.ptr
     * points to it
     */
    for ( i = 0; i < rxendindex + 1; i++ )
    {
        /* Make sure the output buffers are PLATFORM_L1_CACHE_LINE ALIGNED */
        wiced_assert( "check alignment of hwcrypto destination address", ( ( output_desc[ i ].addrlow & PLATFORM_L1_CACHE_LINE_MASK ) == 0 ) );

        platform_dcache_inv_range( ( uint32_t* )output_desc[ i ].addrlow, output_desc[ i ].ctrl2 );
    }
}

/*
 * @param[in]   txendindex      number of tx (input) descriptors
 * @param[in]   rxendindex      number of rx (output) descriptors
 * @param[in]   input_desc      input (TX) descriptor table
 * @param[in]   output_desc     output (RX) descriptor table
 */
static void hwcrypto_unprotected_blocking_dma_transfer( int txendindex, int rxendindex, dma64dd_t input_desc[ ], dma64dd_t output_desc[ ] )
{

    uint32_t indmasize = 0;
    uint32_t outdmasize = 0;
    int i;

    wiced_assert("Number of DMA descriptors greater than allocated", txendindex <= MAX_TX_DMADESCRIPTOS);
    wiced_assert("Number of DMA descriptors greater than allocated", rxendindex <= MAX_RX_DMADESCRIPTOS);

    /* Enable HWCrypto engine (SPU-M)
     * SPU-M has to be enabled and disabled for each transaction
     * to avoid random errors on output.
     */
    cryptoreg->cryptocontrol.raw = 0x0;

    /* Setup DMA channel receiver */

    /* bits 63:32 of the memory space address of the first byte of the receive descriptor table */
    cryptoreg->dmaregs.rx.addrhigh  = HWCRYPTO_UNUSED;

    /* low 32-bits of the memory space address of the first byte of the receive descriptor table */
    cryptoreg->dmaregs.rx.addrlow   = ( (uint32_t) &output_desc[ 0 ] );

    /* PD   1   Parity Check Disable
     * BL   2   Burstlen power(2, n+4)  // n = 2 below
     * PC   1   Prefetch upto 4 descriptors
     * RO   8   Specifies the size, in bytes, of the status region at the beginning of the receive frame buffer
     * SH   1   The receive descriptor processor will place the start of new frames only in descriptors that have the StartOfFrame field.
     * OC   1   receive FIFO overflow causes the processor to clear the receive FIFO, and discard all remaining incoming bytes in the current frame,
     *          and report a FIFO overflow error
     */
    cryptoreg->dmaregs.rx.control   = D64_RC_PD | ( ( 2 << D64_RC_BL_SHIFT ) & D64_RC_BL_MASK ) | ( ( 1 << D64_RC_PC_SHIFT ) & D64_RC_PC_MASK ) |
                                        (( 8 << D64_RC_RO_SHIFT) & D64_RC_RO_MASK) | D64_RC_SH | D64_RC_OC  ;

    /* byte offset of the descriptor following the last valid descriptor */
    cryptoreg->dmaregs.rx.ptr       = ( (uint32_t) &output_desc[ rxendindex - 1 ] + sizeof( dma64dd_t ) ) & 0xffff;

    /* Setup DMA channel transmitter */
    cryptoreg->dmaregs.tx.addrhigh  = HWCRYPTO_UNUSED;

    /* low 32-bits of the memory space address of the first byte of the transmit descriptor table */
    cryptoreg->dmaregs.tx.addrlow   = ( (uint32_t) &input_desc[ 0 ] );

    /* byte offset of the descriptor following the last valid descriptor */
    cryptoreg->dmaregs.tx.ptr       = ( (uint32_t) &input_desc[ txendindex - 1 ] + sizeof( dma64dd_t ) ) & 0xffff; // needs to be lower 16 bits of descriptor address

    /* PD   1   Parity Check Disable
     * BL   2   Burstlen power(2, n+4)  // n = 2 below
     * PC   1   Prefetch upto 4 descriptors
     */
    cryptoreg->dmaregs.tx.control   = D64_XC_PD | ( ( 2 << D64_XC_BL_SHIFT ) & D64_XC_BL_MASK ) | ( ( 1 << D64_XC_PC_SHIFT ) & D64_XC_PC_MASK );

    hwcrypto_dcache_clean_dma_input( txendindex, rxendindex, input_desc, output_desc );

    hwcrypto_dcache_invalidate_dma_output( rxendindex, output_desc );

    /* Fire off the DMA */
    cryptoreg->dmaregs.tx.control |= D64_XC_XE;
    cryptoreg->dmaregs.rx.control |= D64_RC_RE;

    for ( i = 0; i < txendindex; i++ )
    {
        indmasize   += input_desc[ i ].ctrl2;
    }

    for ( i = 0; i < rxendindex; i++ )
    {
        outdmasize  += output_desc[ i ].ctrl2;
    }

    /* Exclude 8 bytes of Crypto Status from outdmasize */
    outdmasize -= CRYPTO_STATUS_SIZE;

    cryptoreg->indmasize.raw        = ( indmasize ) / BYTES_IN_WORD;
    cryptoreg->outdmasize.raw       = ( outdmasize ) / BYTES_IN_WORD;
    /* Begin Crypto Operation - Begins fetching data if there is a valid descriptor and the infifo is empty */
    cryptoreg->cryptocontrol.raw    = 0x01;

    /* Wait till DMA has finished */
    while ( ( ( cryptoreg->dmaregs.tx.status0 & D64_XS0_XS_MASK ) == D64_XS0_XS_ACTIVE ) || ( ( cryptoreg->dmaregs.rx.status0 & D64_XS0_XS_MASK ) == D64_XS0_XS_ACTIVE ) )
    {
    }

    /* Switch off the DMA */
    cryptoreg->dmaregs.tx.control &= ( unsigned )( ~D64_XC_XE );
    cryptoreg->dmaregs.rx.control &= ( unsigned )( ~D64_XC_XE );

    /* Indicate empty table. Otherwise core is not freeing PMU resources. */
    cryptoreg->dmaregs.tx.ptr = cryptoreg->dmaregs.tx.addrlow & 0xFFFF;

    /* Disable HWCrypto engine (SPU-M) */
    cryptoreg->cryptocontrol.bits.softreset   = 0x1;
    cryptoreg->cryptocontrol.bits.disablespum = 0x1;
}

/* For incremental HMAC operations calculate the InnerHashContext (key XOR ipad)
 * and OuterHashContext (key XOR opad)  Note: thhis is also used by MD5
 *
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * @param[in]   key         key for hmac operation
 * @param[in]   keylen      length (in bytes) of hmac key
 * @param[out]  i_key_pad   InnexHashContext
 * @param[out]  o_key_pad   OuterHashContext
 */
static void hwcrypto_compute_hmac_inner_outer_hashcontext( uint8_t* key, uint32_t keylen, uint8_t i_key_pad[ ], uint8_t o_key_pad[ ] )
{
    int i = 0;

    memcpy( i_key_pad, key, keylen );
    memcpy( o_key_pad, key, keylen );

    for ( i = 0; i < HMAC_BLOCK_SIZE; i++ )
    {
        i_key_pad[ i ] ^= HMAC_IPAD;
        o_key_pad[ i ] ^= HMAC_OPAD;
    }
}
