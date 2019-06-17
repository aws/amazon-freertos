/**
 * Copyright (c) 2012 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup ble_sdk_srv_gls_db Glucose Database Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Glucose Service module.
 *
 * @details This module implements at database of stored glucose measurement values.
 *
 * @note Attention!
 *  To maintain compliance with Nordic Semiconductor ASA Bluetooth profile
 *  qualification listings, These APIs must not be modified. However, the corresponding
 *  functions' implementations can be modified.
 */

#ifndef BLE_GLS_DB_H__
#define BLE_GLS_DB_H__

#include <stdint.h>
#include "ble_gls.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_GLS_DB_MAX_RECORDS      20

/**@brief Function for initializing the glucose record database.
 *
 * @details This call initializes the database holding glucose records.
 *
 * @return      NRF_SUCCESS on success.
 */
uint32_t ble_gls_db_init(void);

/**@brief Function for getting the number of records in the database.
 *
 * @details This call returns the number of records in the database.
 *
 * @return      Number of records in the database.
 */
uint16_t ble_gls_db_num_records_get(void);

/**@brief Function for getting a record from the database.
 *
 * @details This call returns a specified record from the database.
 *
 * @param[in]   record_num    Index of the record to retrieve.
 * @param[out]  p_rec         Pointer to record structure where retrieved record is copied to.
 *
 * @return      NRF_SUCCESS on success.
 */
uint32_t ble_gls_db_record_get(uint8_t record_num, ble_gls_rec_t * p_rec);

/**@brief Function for adding a record at the end of the database.
 *
 * @details This call adds a record as the last record in the database.
 *
 * @param[in]   p_rec   Pointer to record to add to database.
 *
 * @return      NRF_SUCCESS on success.
 */
uint32_t ble_gls_db_record_add(ble_gls_rec_t * p_rec);

/**@brief Function for deleting a database entry.
 *
 * @details This call deletes an record from the database.
 *
 * @param[in]   record_num   Index of record to delete.
 *
 * @return      NRF_SUCCESS on success.
 */
uint32_t ble_gls_db_record_delete(uint8_t record_num);


#ifdef __cplusplus
}
#endif

#endif // BLE_GLS_DB_H__

/** @} */
