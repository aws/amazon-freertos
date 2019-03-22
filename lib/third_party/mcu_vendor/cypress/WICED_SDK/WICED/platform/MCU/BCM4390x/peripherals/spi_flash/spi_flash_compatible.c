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

/*
 * WICED 4390x Sflash driver
 */
#include "spi_flash.h"
#include <string.h>
#include "crypto_api.h"
#include <limits.h>
#include "platform_spi_flash.h"
#include "wiced_power_logger.h"
#include "wiced_utilities.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define SFLASH_CHECK_RESULT( expr, retval )  { if (expr) { return retval; }}
#define BITS_PER_BYTE                        (8)
#define SWAP( T, x, y )                      do { T temp = x; x = y; y = temp; } while ( 0 )

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/
static int sflash_read_nonblocking( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size );
static int sflash_verify_data( uint8_t* aes128_key, uint8_t* aes_iv, uint32_t crypt_size, uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* input_buffer, uint8_t* output_buffer, uint8_t* hmac_output );
static void otp_read_keys( uint8_t* buffer, uint8_t* buffer_r, uint32_t word_num, uint32_t word_num_r, uint32_t size_in_bytes );
static void generate_aes_iv(const sflash_handle_t* const handle, unsigned long sector_addr, uint8_t* aes_iv);
static int init_securesflash( sflash_handle_t* const handle);

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
static securesflash_handle_t securesflash_handle ALIGNED(HWCRYPTO_ALIGNMENT_BYTES);

/******************************************************
 *               Variables Definitions
 ******************************************************/
static int sflash_read_nonblocking( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size )
{
    int result;
    sflash_handle_t* sflash_handle = ( sflash_handle_t* ) handle;

    sflash_handle->read_blocking = WICED_FALSE;
    result = bcm4390x_sflash_read_data( sflash_handle, device_address, data_addr, size, SFLASH_DATA_READ_MODE );
    return result;
}

int init_sflash ( const sflash_handle_t* handle, void* peripheral_id, sflash_write_allowed_t write_allowed_in )
{
    int result = 0;
    sflash_handle_t* sflash_handle = ( sflash_handle_t* ) handle;

    /* Initialize the serial flash driver */
    result = bcm4390x_sflash_init( sflash_handle, peripheral_id, write_allowed_in );
    if ( result != SFLASH_MSG_OK )
    {
        /* return from main in order to try rebooting... */
        WPRINT_SFLASH_ERROR( ( "SFlash init, fail!\n" ) );
        deinit_sflash( sflash_handle );
        goto fail;
    }
    WPRINT_SFLASH_DEBUG( ( "SFlash init, OK!\n" ) );

    init_securesflash( sflash_handle );
    sflash_handle->write_allowed = write_allowed_in;

fail :
    return result;
}

int deinit_sflash ( const sflash_handle_t* handle )
{
    int result = 0;

    /* De-initialize the serial flash driver */
    result = bcm4390x_sflash_deinit( ( sflash_handle_t* ) handle );
    if ( result != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "SFlash de-init, fail!\n" ) );
    }

    return result;
}

int sflash_get_size ( const sflash_handle_t* handle, unsigned long* size )
{
    *size = handle->capabilities->total_size;
    return SFLASH_MSG_OK;
}

int sflash_read ( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size )
{
    int result;
    sflash_handle_t* sflash_handle = ( sflash_handle_t* ) handle;
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_READ );


    sflash_handle->read_blocking = WICED_TRUE;

    result = bcm4390x_sflash_read_data( sflash_handle, device_address, data_addr, size, SFLASH_DATA_READ_MODE );


    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_IDLE );

    return result;
}

int sflash_chip_erase ( const sflash_handle_t* handle )
{
    int result;
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_ERASE );


    result = bcm4390x_sflash_erase_data( ( sflash_handle_t* ) handle, 0, ERASE_ALL);


    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_IDLE );

    return result;
}

int sflash_write ( const sflash_handle_t* handle, unsigned long device_address, const void* data_addr, unsigned int size )
{
    int result;
    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_WRITE );


    result = bcm4390x_sflash_write_data( ( sflash_handle_t* ) handle, device_address, ( void* )data_addr, size, SFLASH_DATA_WRITE_MODE );


    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_IDLE );

    return result;
}

int sflash_sector_erase ( const sflash_handle_t* handle, unsigned long device_address )
{
    int result;

    result = bcm4390x_sflash_erase_data( ( sflash_handle_t* ) handle, device_address, ERASE_SECTOR);

    return result;
}

int sflash_block_erase ( const sflash_handle_t* handle, unsigned long device_address )
{
    int result;

    result = bcm4390x_sflash_erase_data( ( sflash_handle_t* ) handle, device_address, ERASE_BLOCK );

    return result;
}

/********************************************************************************
 *                   Secure Sflash Functions
 *******************************************************************************/

/*The function init_securesflash should not be ROMmed
 *
 * Initialize Securesflash handle , Which includes pointers to
 * secure read and write functions and hwcrypto_buffer, used to store
 * data read from Sflash which is passed to HWCrypto engine */
static int init_securesflash( sflash_handle_t* const handle )
{

    if ( PLATFORM_SECURESFLASH_ENABLED )
    {
        platform_hwcrypto_init( );

        securesflash_handle.sflash_secure_read_function  = &sflash_read_secure;

#ifndef SFLASH_WRITE_DATA_DISABLE
        securesflash_handle.sflash_secure_write_function = &sflash_write_secure;
#else
        securesflash_handle.sflash_secure_write_function = NULL;
#endif /* SFLASH_WRITE_DATA_DISABLE */

        handle->securesflash_handle = &securesflash_handle;

        otp_read_keys( securesflash_handle.hmac_key, securesflash_handle.scratch_pad,
            OTP_WORD_NUM_SHA_KEY, OTP_WORD_NUM_SHA_KEY_R, SECUREBOOT_SHA_KEY_SIZE );
        otp_read_keys( securesflash_handle.aes128_key, securesflash_handle.scratch_pad,
            OTP_WORD_NUM_AES_KEY, OTP_WORD_NUM_AES_KEY_R, AES128_KEY_LEN );
    }
    else
    {
        handle->securesflash_handle = NULL;
    }

    return 0;
}

/* Read Cryptographic keys stored in OTP
 * buffer_out           : Output buffer, contains the ORred result of Keys read from OTP and redundant OTP bits
 * buff_out_redundant   : Contains the Keys read from redundant OTP bits
 * word_num             : OTP word number for the key
 * word_num_redundant   : OTP word number for the redundant key
 * size_in_bytes        : Size of Key in bytes
 * */
static void otp_read_keys( uint8_t* buffer_out, uint8_t* buffer_out_redundant, uint32_t word_num, uint32_t word_num_redundant, uint32_t size_in_bytes )
{
    uint32_t i;
    uint16_t* otp_ptr = NULL;
    uint16_t* otp_ptr_redundant = NULL;
    uint32_t size_in_word = size_in_bytes/2;

    otp_ptr = ( uint16_t* )buffer_out;
    otp_ptr_redundant = ( uint16_t* )buffer_out_redundant;

    for (i = 0; i < ( size_in_word ); i++)
    {
        platform_otp_read_word_unprotected( ( word_num + i ), otp_ptr );
        platform_otp_read_word_unprotected( ( word_num_redundant + i ), otp_ptr_redundant );
        *otp_ptr++ |= *otp_ptr_redundant++;
    }
}

/* For secure sflash, AES IV = SHA256_HASH( KEY XOR SECTOR_NUMBER ) */
static void generate_aes_iv(const sflash_handle_t* const handle, unsigned long sector_addr, uint8_t* aes_iv)
{
    int                    i;
    securesflash_handle_t* secure_sflash_handle = handle->securesflash_handle;
    uint8_t*               scratch_pad = secure_sflash_handle->scratch_pad;

    /* AES_IV = SHA256_HASH( KEY XOR SECTOR_NUM) */
    for ( i = 0; i < AES128_KEY_LEN; i++ )
    {
        scratch_pad[ i ] = (uint8_t) ( ( ( sector_addr ) / SECURE_SECTOR_SIZE ) % UCHAR_MAX ) ^ secure_sflash_handle->aes128_key[ i ];
    }

    platform_hwcrypto_sha256_hash( scratch_pad, AES128_KEY_LEN, scratch_pad, aes_iv );
}

/* Decrypt and Verify the signature of data stored in input_buffer
 * aes128_key   : Key used for AES128-CBC encryption/decryption
 * aes_iv    : iv used for AES128-CBC encryption/decryption
 * crypt_size   : Size of data stored in input_buffer
 * auth_size    : Size of data that needs to be Sign-Verified
 * hmac_key     : Key used for sha256_hmac signature verification
 * hmac_key_len : Length of the hmac_key in bytes
 * input_buffer : Data that needs to be Decrypted/Signature Verified
 * output_buffer: Encryption/Decryption Output from HWCrypto engine
 * hmac_output  : Authentication Output from HWCrypto Engine
 */
static int sflash_verify_data( uint8_t* aes128_key, uint8_t* aes_iv, uint32_t crypt_size, uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* input_buffer, uint8_t* output_buffer, uint8_t* hmac_output )
{
    int result = 0;
    uint8_t *expected_digest;

    platform_hwcrypto_aescbc_decrypt_sha256_hmac( aes128_key, aes_iv, crypt_size, auth_size, hmac_key,
            hmac_key_len, input_buffer, output_buffer, hmac_output );

    /* Compare Computed Signature with the Digest */
    /* Digest is stored at SECURE_SECTOR_DATA_SIZE offset from start */
    expected_digest = ( uint8_t* ) output_buffer + SECURE_SECTOR_DATA_SIZE;
    for ( int i = 0; i < HMAC256_OUTPUT_SIZE; i++ )
    {
        result |= ( expected_digest[ i ] ^ hmac_output[ i ] );
    }

    return result;
}

int sflash_read_secure( const sflash_handle_t* handle, unsigned long device_address, void* data_addr, unsigned int size )
{
    uint8_t*        data_addr_ptr;
    uint8_t*        crypto_buffer = NULL;
    uint8_t*        sflash_buffer = NULL;
    uint32_t        remaining_size;
    uint32_t        read_size;
    uint32_t        prev_sector_read_size = 0;
    uint32_t        prev_sector_read_offset = 0;
    int             result = 0;
    bool            sflash_read_finished = FALSE;
    unsigned long   sector_aligned_sflash_addr = 0;
    unsigned long   read_offset;
    uint8_t         aes_iv[ SHA256_HASH_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );

    securesflash_handle_t *secure_sflash_handle = handle->securesflash_handle;
    wiced_assert( "sflash_read_secure : securesflash handle is null", ( secure_sflash_handle != NULL ) );

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_READ );

    data_addr_ptr  = ( uint8_t* ) data_addr;
    sflash_buffer  = ( uint8_t* ) secure_sflash_handle->hwcrypto_buffer;
    crypto_buffer  = ( uint8_t* ) secure_sflash_handle->hwcrypto_buffer + SECURE_SECTOR_SIZE;
    remaining_size = size;

    /* To speed up the process of decrypting and verifying multiple sectors, two
     * buffers are used, the idea is that when one sector of data is being read from
     * sflash, the previously read sector can be parallely decrypted/verified.
     *
     * - crypto_buffer - Contents of this buffer are currently being decrypted/verified
     * - sflash_buffer - Contents of this buffer are currently being read from sflash
     *
     * First iteration of the while loop
     * sflash_read_finished = FALSE
     * sflash_read_nonblocking() reads the first sector of data
     * sflash_read_finished is false, so code inside if (sflash_read_finished == TRUE) block is not executed
     * SWAP( crypto_buffer, sflash_buffer)
     * sflash_read_finished = TRUE
     *
     * All other iterations
     * sflash_read_finished = TRUE -> Set by the first iteration
     * sflash_read_nonblocking() - read the next block from sflash
     * The previously read block is decrypted/sign verified
     *
     * At the end of the while loop, All Sectors have been read from sflash, but only n-1
     * sectors have been verified, so the veficiation for last block is handled after the while loop
     */

    /* Get semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( handle->hw_interface, SEMAPHORE_GET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Get Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

    /* read in chunks of SECURE_SECTOR_SIZE */
    while ( remaining_size != 0 )
    {
        /* Offset from start of sector*/
        read_offset = device_address % SECURE_SECTOR_SIZE;
        /* How much data can be read from the current sector */
        read_size = MIN( remaining_size, ( SECURE_SECTOR_DATA_SIZE - read_offset ) );

        sector_aligned_sflash_addr = ALIGN_TO_SECTOR_ADDRESS( device_address );

        /* Read FULL Sector, for verification */
        WICED_VERIFY( sflash_read_nonblocking( handle, ( unsigned long ) sector_aligned_sflash_addr, sflash_buffer, SECURE_SECTOR_SIZE ) );

        if ( sflash_read_finished == TRUE )
        {
            generate_aes_iv(handle, ( sector_aligned_sflash_addr - SECURE_SECTOR_SIZE ), aes_iv);
            result = sflash_verify_data( secure_sflash_handle->aes128_key, aes_iv, SECURE_SECTOR_SIZE, SECURE_SECTOR_DATA_SIZE,
                    secure_sflash_handle->hmac_key, SECUREBOOT_SHA_KEY_SIZE, crypto_buffer, crypto_buffer, secure_sflash_handle->scratch_pad );
            SFLASH_CHECK_RESULT( result != 0, result );

            /* Copy back requested data to output buffer */
            memcpy( data_addr_ptr, ( ( uint8_t* ) crypto_buffer + prev_sector_read_offset ), prev_sector_read_size );
            data_addr_ptr += prev_sector_read_size;
        }

        /* Wait for sflash_read_nonblocking() to complete */
        m2m_switch_off_dma_post_completion( );
        m2m_post_dma_completion_operations( ( void* )sflash_buffer, SECURE_SECTOR_SIZE );
        sflash_read_finished = TRUE;

        /* SWAP crypto_buffer and sflash_buffer */
        SWAP( uint8_t * ,crypto_buffer, sflash_buffer );

        /* Update counters */
        prev_sector_read_size   = read_size;
        prev_sector_read_offset = read_offset;
        device_address          = sector_aligned_sflash_addr + SECURE_SECTOR_SIZE;
        remaining_size          -= read_size;
    }

    /* Set semaphore */
    if ( ( result = PLATFORM_INTERFACE_SEMAPHORE( handle->hw_interface, SEMAPHORE_SET ) ) != SFLASH_MSG_OK )
    {
        WPRINT_SFLASH_ERROR( ( "!!! [%s] - Set Semaphore fail!!!\n", __FUNCTION__ ) );
        return result;
    }

    /* Last Block */
    generate_aes_iv(handle, sector_aligned_sflash_addr, aes_iv);
    result = sflash_verify_data( secure_sflash_handle->aes128_key, aes_iv, SECURE_SECTOR_SIZE, SECURE_SECTOR_DATA_SIZE,
            secure_sflash_handle->hmac_key, SECUREBOOT_SHA_KEY_SIZE, crypto_buffer, crypto_buffer, secure_sflash_handle->scratch_pad );

    if ( result == 0 )
    {
        memcpy( data_addr_ptr, ( crypto_buffer + prev_sector_read_offset ), prev_sector_read_size );
    }

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_IDLE );

    return result;
}

int sflash_write_secure( const sflash_handle_t* handle, unsigned long device_address, const void* data_addr, unsigned int size )
{
    uint8_t*         data_ptr;
    uint32_t         write_size;
    uint32_t         write_offset;
    uint32_t         remaining_size;
    int              result = 0;
    unsigned long    sector_aligned_device_address;
    uint8_t*         read_buffer;
    uint8_t          aes_iv[ SHA256_HASH_SIZE ] ALIGNED( HWCRYPTO_ALIGNMENT_BYTES );
#ifndef BOOTLOADER
    hw_aes_context_t aes_ctx;
#endif
    securesflash_handle_t *secure_sflash_handle = handle->securesflash_handle;

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_WRITE );

    wiced_assert( "sflash_write_secure : securesflash handle is null", ( secure_sflash_handle != NULL ) );
    remaining_size  = size;
    data_ptr        = ( uint8_t* ) data_addr;
    read_buffer     = secure_sflash_handle->hwcrypto_buffer;

    while ( remaining_size != 0 )
    {
        /* aligned to SECURE_SECTOR_SIZE */
        sector_aligned_device_address = ALIGN_TO_SECTOR_ADDRESS( device_address );
        write_offset = device_address % SECURE_SECTOR_SIZE;
        write_size = MIN( remaining_size, ( SECURE_SECTOR_DATA_SIZE - write_offset ) );

        /* Read the Virtual sector */
        WICED_VERIFY( sflash_read( handle, sector_aligned_device_address, read_buffer, SECURE_SECTOR_SIZE ) );
        generate_aes_iv(handle, sector_aligned_device_address, aes_iv);

#ifdef BOOTLOADER
        platform_hwcrypto_aes128cbc_decrypt( secure_sflash_handle->aes128_key, 16, aes_iv, SECURE_SECTOR_SIZE, read_buffer, read_buffer );

#else
        /* Decrypt */
        hw_aes_setkey_dec(&aes_ctx, secure_sflash_handle->aes128_key, ( AES128_KEY_LEN * BITS_PER_BYTE ) );
        hw_aes_crypt_cbc(&aes_ctx, HW_AES_DECRYPT, SECURE_SECTOR_SIZE, aes_iv, read_buffer, read_buffer);
#endif
        /* Modify Sector contents */
        memcpy( (uint8_t*) read_buffer + write_offset, data_ptr, write_size );
        platform_hwcrypto_sha256_hmac( secure_sflash_handle->hmac_key, SHA256_HASH_SIZE, read_buffer, SECURE_SECTOR_DATA_SIZE, read_buffer, (read_buffer + SECURE_SECTOR_DATA_SIZE) );

        generate_aes_iv(handle, sector_aligned_device_address, aes_iv);

#ifdef BOOTLOADER
        platform_hwcrypto_aescbc_encrypt(secure_sflash_handle->aes128_key, aes_iv, SECURE_SECTOR_SIZE, read_buffer, read_buffer);
#else
        /* Encrypt */
        hw_aes_crypt_cbc(&aes_ctx, HW_AES_ENCRYPT, SECURE_SECTOR_SIZE, aes_iv, read_buffer, read_buffer);
#endif
        /* Write the Modified sector back using sflash_write */
        WICED_VERIFY( sflash_sector_erase( handle, sector_aligned_device_address ) );
        result = sflash_write( handle, sector_aligned_device_address, read_buffer, SECURE_SECTOR_SIZE );
        SFLASH_CHECK_RESULT( result != 0, -1 );

#ifdef VERIFY_SECURE_WRITE
    sflash_read_secure( handle, sector_aligned_device_address, read_buffer + SECURE_SECTOR_SIZE, SECURE_SECTOR_DATA_SIZE );
    if( memcmp( read_buffer, read_buffer + SECURE_SECTOR_SIZE, SECURE_SECTOR_DATA_SIZE) != 0 )
    {
        wiced_assert( "sflash_write_secure() : error in verification", WICED_FALSE );
    }
#endif

        data_ptr        += write_size;
        remaining_size  = remaining_size - write_size;
        device_address  = sector_aligned_device_address + SECURE_SECTOR_SIZE;
    }

    WICED_POWER_LOGGER( EVENT_PROC_ID_MCU, EVENT_ID_SPI_SFLASH, EVENT_DESC_SPI_SFLASH_IDLE );

    return result;
}
