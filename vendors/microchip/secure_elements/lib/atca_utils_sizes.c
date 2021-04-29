/**
 * \file
 * \brief API to Return structure sizes of cryptoauthlib structures
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"

#define SIZE_OF_API_T(x)  size_t x ## _size(void); size_t x ## _size(void) { return sizeof( x ); }
#define SIZE_OF_API_S(x)  size_t x ## _size(void); size_t x ## _size(void) { return sizeof(struct x ); }

#if ATCA_CA_SUPPORT
#include "atcacert/atcacert_date.h"
#include "atcacert/atcacert_def.h"
/* atcacert_date.h */
SIZE_OF_API_T(atcacert_tm_utc_t)
SIZE_OF_API_T(atcacert_date_format_t)

/* atcacert_def.h */
SIZE_OF_API_T(atcacert_cert_type_t)
SIZE_OF_API_T(atcacert_cert_sn_src_t)
SIZE_OF_API_T(atcacert_device_zone_t)
SIZE_OF_API_T(atcacert_std_cert_element_t)
SIZE_OF_API_T(atcacert_device_loc_t)
SIZE_OF_API_T(atcacert_cert_loc_t)
SIZE_OF_API_T(atcacert_cert_element_t)
SIZE_OF_API_T(atcacert_def_t)
SIZE_OF_API_T(atcacert_build_state_t)
#endif

/* atcab.h */
SIZE_OF_API_T(atca_aes_cbc_ctx_t)
SIZE_OF_API_T(atca_aes_cmac_ctx_t)
SIZE_OF_API_T(atca_aes_ctr_ctx_t)

#if ATCA_CA_SUPPORT
#include "host/atca_host.h"

/* atca_host.h */
SIZE_OF_API_T(atca_temp_key_t)
SIZE_OF_API_S(atca_include_data_in_out)
SIZE_OF_API_T(atca_nonce_in_out_t)
SIZE_OF_API_T(atca_io_decrypt_in_out_t)
SIZE_OF_API_T(atca_verify_mac_in_out_t)
SIZE_OF_API_T(atca_secureboot_enc_in_out_t)
SIZE_OF_API_T(atca_secureboot_mac_in_out_t)
SIZE_OF_API_T(atca_mac_in_out_t)
SIZE_OF_API_S(atca_hmac_in_out)
SIZE_OF_API_T(atca_gen_dig_in_out_t)
SIZE_OF_API_T(atca_write_mac_in_out_t)
SIZE_OF_API_S(atca_derive_key_in_out)
SIZE_OF_API_S(atca_derive_key_mac_in_out)
SIZE_OF_API_S(atca_decrypt_in_out)
SIZE_OF_API_T(atca_check_mac_in_out_t)
SIZE_OF_API_T(atca_verify_in_out_t)
SIZE_OF_API_T(atca_gen_key_in_out_t)
SIZE_OF_API_T(atca_sign_internal_in_out_t)
#endif

/* atca_bool.h */
SIZE_OF_API_T(bool)

/* atca_command.h */
#if ATCA_CA_SUPPORT
SIZE_OF_API_T(ATCAPacket)
#endif

/* atca_device.h */
SIZE_OF_API_S(atca_device)

/* atca_devtypes.h */
SIZE_OF_API_T(ATCADeviceType)

/* calib_execution.h */
#ifdef ATCA_NO_POLL
#include "calib/calib_execution.h"
SIZE_OF_API_T(device_execution_time_t)
#endif

/* atca_iface.h */
SIZE_OF_API_T(ATCAIfaceType)
SIZE_OF_API_T(ATCAIfaceCfg)
SIZE_OF_API_S(atca_iface)

/* atca_status.h */
SIZE_OF_API_T(ATCA_STATUS)
