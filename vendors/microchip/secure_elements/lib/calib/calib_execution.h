/**
 * \file
 * \brief Defines an execution handler that executes a given command on a
 *        device and returns the results.
 *
 * The basic flow is to wake the device, send the command, wait/poll for
 * completion, and finally receives the response from the device and does
 * basic checks before returning to caller.
 *
 * This handler supports the ATSHA and ATECC device family.
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


#ifndef CALIB_EXECUTION_H
#define CALIB_EXECUTION_H

#include "atca_status.h"
#include "calib_command.h"
#include "atca_device.h"
#include "atca_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATCA_UNSUPPORTED_CMD ((uint16_t)0xFFFF)

#define CALIB_SWI_FLAG_WAKE     0x00    //!< flag preceding a command
#define CALIB_SWI_FLAG_CMD      0x77    //!< flag preceding a command
#define CALIB_SWI_FLAG_TX       0x88    //!< flag requesting a response
#define CALIB_SWI_FLAG_IDLE     0xBB    //!< flag requesting to go into Idle mode
#define CALIB_SWI_FLAG_SLEEP    0xCC    //!< flag requesting to go into Sleep mode

#ifdef ATCA_NO_POLL
/** \brief Structure to hold the device execution time and the opcode for the
 *         corresponding command
 */
typedef struct
{
    uint8_t  opcode;
    uint16_t execution_time_msec;
}device_execution_time_t;

ATCA_STATUS calib_get_execution_time(uint8_t opcode, ATCACommand ca_cmd);
#endif

#ifndef ATCA_HAL_LEGACY_API
ATCA_STATUS calib_execute_receive(ATCADevice device, uint8_t device_address, uint8_t* rxdata, uint16_t* rxlength);
#endif

ATCA_STATUS calib_execute_command(ATCAPacket* packet, ATCADevice device);

#ifdef __cplusplus
}
#endif
#endif

