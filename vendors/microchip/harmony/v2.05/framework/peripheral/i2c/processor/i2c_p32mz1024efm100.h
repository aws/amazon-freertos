/* Created by plibgen $Revision: 1.31 $ */

#ifndef _I2C_P32MZ1024EFM100_H
#define _I2C_P32MZ1024EFM100_H

/* Section 1 - Enumerate instances, define constants, VREGs */

#include <xc.h>
#include <stdbool.h>

#include "peripheral/peripheral_common_32bit.h"

/* Default definition used for all API dispatch functions */
#ifndef PLIB_INLINE_API
    #define PLIB_INLINE_API extern inline
#endif

/* Default definition used for all other functions */
#ifndef PLIB_INLINE
    #define PLIB_INLINE extern inline
#endif

typedef enum {

    I2C_ID_1 = _I2C1_BASE_ADDRESS,
    I2C_ID_2 = _I2C2_BASE_ADDRESS,
    I2C_ID_3 = _I2C3_BASE_ADDRESS,
    I2C_ID_4 = _I2C4_BASE_ADDRESS,
    I2C_ID_5 = _I2C5_BASE_ADDRESS,
    I2C_NUMBER_OF_MODULES = 5

} I2C_MODULE_ID;

typedef enum {

    I2C_SDA_MIN_HOLD_TIME_100NS = 0x00,
    I2C_SDA_MIN_HOLD_TIME_300NS = 0x01

} I2C_SDA_MIN_HOLD_TIME;

/* Section 2 - Feature variant inclusion */

#define PLIB_TEMPLATE PLIB_INLINE
#include "../templates/i2c_ClockStretching_Default.h"
#include "../templates/i2c_GeneralCall_Default.h"
#include "../templates/i2c_SMBus_Default.h"
#include "../templates/i2c_HighFrequency_Default.h"
#include "../templates/i2c_ReservedAddressProtect_Default.h"
#include "../templates/i2c_StopInIdle_Default.h"
#include "../templates/i2c_IPMI_Unsupported.h"
#include "../templates/i2c_ModuleEnable_Default.h"
#include "../templates/i2c_StartDetect_Default.h"
#include "../templates/i2c_StopDetect_Default.h"
#include "../templates/i2c_ArbitrationLoss_Default.h"
#include "../templates/i2c_BusIdle_Default.h"
#include "../templates/i2c_BaudRate_Default.h"
#include "../templates/i2c_SlaveAddress7Bit_Default.h"
#include "../templates/i2c_SlaveAddress10Bit_Default.h"
#include "../templates/i2c_SlaveAddressDetect_Default.h"
#include "../templates/i2c_GeneralCallAddressDetect_Default.h"
#include "../templates/i2c_SlaveMask_Default.h"
#include "../templates/i2c_SlaveReadRequest_Default.h"
#include "../templates/i2c_SlaveDataDetect_Default.h"
#include "../templates/i2c_SlaveClockHold_Default.h"
#include "../templates/i2c_MasterStart_Default.h"
#include "../templates/i2c_MasterStartRepeat_Default.h"
#include "../templates/i2c_MasterReceiverClock1Byte_Default.h"
#include "../templates/i2c_MasterStop_Default.h"
#include "../templates/i2c_TransmitterBusy_Default.h"
#include "../templates/i2c_TransmitterByteSend_Default.h"
#include "../templates/i2c_TransmitterByteComplete_Default.h"
#include "../templates/i2c_TransmitterByteAcknowledge_Default.h"
#include "../templates/i2c_TransmitterOverflow_Default.h"
#include "../templates/i2c_ReceivedByteAvailable_Default.h"
#include "../templates/i2c_ReceivedByteGet_Default.h"
#include "../templates/i2c_ReceivedByteAcknowledge_Default.h"
#include "../templates/i2c_ReceiverOverflow_Default.h"
#include "../templates/i2c_SlaveInterruptOnStop_Default.h"
#include "../templates/i2c_SlaveInterruptOnStart_Default.h"
#include "../templates/i2c_SlaveBufferOverwrite_Default.h"
#include "../templates/i2c_DataLineHoldTime_Default.h"
#include "../templates/i2c_SlaveBusCollisionDetect_Default.h"
#include "../templates/i2c_SlaveAddressHoldEnable_Default.h"
#include "../templates/i2c_SlaveDataHoldEnable_Default.h"
#include "../templates/i2c_AcksequenceIsInProgress_Default.h"

/* Section 3 - PLIB dispatch function definitions */

PLIB_INLINE_API void PLIB_I2C_SlaveClockStretchingEnable(I2C_MODULE_ID index)
{
     I2C_SlaveClockStretchingEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveClockStretchingDisable(I2C_MODULE_ID index)
{
     I2C_SlaveClockStretchingDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsClockStretching(I2C_MODULE_ID index)
{
     return I2C_ExistsClockStretching_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_GeneralCallEnable(I2C_MODULE_ID index)
{
     I2C_GeneralCallEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_GeneralCallDisable(I2C_MODULE_ID index)
{
     I2C_GeneralCallDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsGeneralCall(I2C_MODULE_ID index)
{
     return I2C_ExistsGeneralCall_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SMBEnable(I2C_MODULE_ID index)
{
     I2C_SMBEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SMBDisable(I2C_MODULE_ID index)
{
     I2C_SMBDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSMBus(I2C_MODULE_ID index)
{
     return I2C_ExistsSMBus_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_HighFrequencyEnable(I2C_MODULE_ID index)
{
     I2C_HighFrequencyEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_HighFrequencyDisable(I2C_MODULE_ID index)
{
     I2C_HighFrequencyDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsHighFrequency(I2C_MODULE_ID index)
{
     return I2C_ExistsHighFrequency_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_ReservedAddressProtectEnable(I2C_MODULE_ID index)
{
     I2C_ReservedAddressProtectEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_ReservedAddressProtectDisable(I2C_MODULE_ID index)
{
     I2C_ReservedAddressProtectDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsReservedAddressProtect(I2C_MODULE_ID index)
{
     return I2C_ExistsReservedAddressProtect_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_StopInIdleEnable(I2C_MODULE_ID index)
{
     I2C_StopInIdleEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_StopInIdleDisable(I2C_MODULE_ID index)
{
     I2C_StopInIdleDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsStopInIdle(I2C_MODULE_ID index)
{
     return I2C_ExistsStopInIdle_Default(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_I2C_IPMIEnable(I2C_MODULE_ID index)
{
     I2C_IPMIEnable_Unsupported(index);
}

PLIB_INLINE_API void _PLIB_UNSUPPORTED PLIB_I2C_IPMIDisable(I2C_MODULE_ID index)
{
     I2C_IPMIDisable_Unsupported(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsIPMI(I2C_MODULE_ID index)
{
     return I2C_ExistsIPMI_Unsupported(index);
}

PLIB_INLINE_API void PLIB_I2C_Enable(I2C_MODULE_ID index)
{
     I2C_Enable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_Disable(I2C_MODULE_ID index)
{
     I2C_Disable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsModuleEnable(I2C_MODULE_ID index)
{
     return I2C_ExistsModuleEnable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_StartWasDetected(I2C_MODULE_ID index)
{
     return I2C_StartWasDetected_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_StartClear(I2C_MODULE_ID index)
{
     I2C_StartClear_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsStartDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsStartDetect_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_StopWasDetected(I2C_MODULE_ID index)
{
     return I2C_StopWasDetected_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_StopClear(I2C_MODULE_ID index)
{
     I2C_StopClear_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsStopDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsStopDetect_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ArbitrationLossHasOccurred(I2C_MODULE_ID index)
{
     return I2C_ArbitrationLossHasOccurred_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_ArbitrationLossClear(I2C_MODULE_ID index)
{
     I2C_ArbitrationLossClear_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsArbitrationLoss(I2C_MODULE_ID index)
{
     return I2C_ExistsArbitrationLoss_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_BusIsIdle(I2C_MODULE_ID index)
{
     return I2C_BusIsIdle_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsBusIsIdle(I2C_MODULE_ID index)
{
     return I2C_ExistsBusIsIdle_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_BaudRateSet(I2C_MODULE_ID index, uint32_t clockFrequencyHz, I2C_BAUD_RATE baudRate)
{
     I2C_BaudRateSet_Default(index, clockFrequencyHz, baudRate);
}

PLIB_INLINE_API I2C_BAUD_RATE PLIB_I2C_BaudRateGet(I2C_MODULE_ID index, uint32_t clockFrequencyHz)
{
     return I2C_BaudRateGet_Default(index, clockFrequencyHz);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsBaudRate(I2C_MODULE_ID index)
{
     return I2C_ExistsBaudRate_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveAddress7BitSet(I2C_MODULE_ID index, uint8_t address)
{
     I2C_SlaveAddress7BitSet_Default(index, address);
}

PLIB_INLINE_API uint8_t PLIB_I2C_SlaveAddress7BitGet(I2C_MODULE_ID index)
{
     return I2C_SlaveAddress7BitGet_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveAddress7Bit(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveAddress7Bit_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveAddress10BitSet(I2C_MODULE_ID index, uint16_t address)
{
     I2C_SlaveAddress10BitSet_Default(index, address);
}

PLIB_INLINE_API uint16_t PLIB_I2C_SlaveAddress10BitGet(I2C_MODULE_ID index)
{
     return I2C_SlaveAddress10BitGet_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveAddressModeIs10Bits(I2C_MODULE_ID index)
{
     return I2C_SlaveAddressModeIs10Bits_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveAddress10Bit(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveAddress10Bit_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveAddress10BitWasDetected(I2C_MODULE_ID index)
{
     return I2C_SlaveAddress10BitWasDetected_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveAddressDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveAddressDetect_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveAddressIsGeneralCall(I2C_MODULE_ID index)
{
     return I2C_SlaveAddressIsGeneralCall_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsGeneralCallAddressDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsGeneralCallAddressDetect_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveMask7BitSet(I2C_MODULE_ID index, uint8_t mask)
{
     I2C_SlaveMask7BitSet_Default(index, mask);
}

PLIB_INLINE_API uint8_t PLIB_I2C_SlaveMask7BitGet(I2C_MODULE_ID index)
{
     return I2C_SlaveMask7BitGet_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveMask10BitSet(I2C_MODULE_ID index, uint16_t mask)
{
     I2C_SlaveMask10BitSet_Default(index, mask);
}

PLIB_INLINE_API uint16_t PLIB_I2C_SlaveMask10BitGet(I2C_MODULE_ID index)
{
     return I2C_SlaveMask10BitGet_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveMask(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveMask_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveReadIsRequested(I2C_MODULE_ID index)
{
     return I2C_SlaveReadIsRequested_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveReadRequest(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveReadRequest_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveDataIsDetected(I2C_MODULE_ID index)
{
     return I2C_SlaveDataIsDetected_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_SlaveAddressIsDetected(I2C_MODULE_ID index)
{
     return I2C_SlaveAddressIsDetected_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveDataDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveDataDetect_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveClockHold(I2C_MODULE_ID index)
{
     I2C_SlaveClockHold_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveClockRelease(I2C_MODULE_ID index)
{
     I2C_SlaveClockRelease_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveClockHold(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveClockHold_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_MasterStart(I2C_MODULE_ID index)
{
     I2C_MasterStart_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsMasterStart(I2C_MODULE_ID index)
{
     return I2C_ExistsMasterStart_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_MasterStartRepeat(I2C_MODULE_ID index)
{
     I2C_MasterStartRepeat_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsMasterStartRepeat(I2C_MODULE_ID index)
{
     return I2C_ExistsMasterStartRepeat_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_MasterReceiverClock1Byte(I2C_MODULE_ID index)
{
     I2C_MasterReceiverClock1Byte_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsMasterReceiverClock1Byte(I2C_MODULE_ID index)
{
     return I2C_ExistsMasterReceiverClock1Byte_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_MasterStop(I2C_MODULE_ID index)
{
     I2C_MasterStop_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsMasterStop(I2C_MODULE_ID index)
{
     return I2C_ExistsMasterStop_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_TransmitterIsBusy(I2C_MODULE_ID index)
{
     return I2C_TransmitterIsBusy_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_TransmitterIsReady(I2C_MODULE_ID index)
{
     return I2C_TransmitterIsReady_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsTransmitterIsBusy(I2C_MODULE_ID index)
{
     return I2C_ExistsTransmitterIsBusy_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_TransmitterByteSend(I2C_MODULE_ID index, uint8_t data)
{
     I2C_TransmitterByteSend_Default(index, data);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsTransmitterByteSend(I2C_MODULE_ID index)
{
     return I2C_ExistsTransmitterByteSend_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_TransmitterByteHasCompleted(I2C_MODULE_ID index)
{
     return I2C_TransmitterByteHasCompleted_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsTransmitterByteComplete(I2C_MODULE_ID index)
{
     return I2C_ExistsTransmitterByteComplete_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_TransmitterByteWasAcknowledged(I2C_MODULE_ID index)
{
     return I2C_TransmitterByteWasAcknowledged_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsTransmitterByteAcknowledge(I2C_MODULE_ID index)
{
     return I2C_ExistsTransmitterByteAcknowledge_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_TransmitterOverflowHasOccurred(I2C_MODULE_ID index)
{
     return I2C_TransmitterOverflowHasOccurred_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_TransmitterOverflowClear(I2C_MODULE_ID index)
{
     I2C_TransmitterOverflowClear_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsTransmitterOverflow(I2C_MODULE_ID index)
{
     return I2C_ExistsTransmitterOverflow_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ReceivedByteIsAvailable(I2C_MODULE_ID index)
{
     return I2C_ReceivedByteIsAvailable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsReceivedByteAvailable(I2C_MODULE_ID index)
{
     return I2C_ExistsReceivedByteAvailable_Default(index);
}

PLIB_INLINE_API uint8_t PLIB_I2C_ReceivedByteGet(I2C_MODULE_ID index)
{
     return I2C_ReceivedByteGet_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsReceivedByteGet(I2C_MODULE_ID index)
{
     return I2C_ExistsReceivedByteGet_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_ReceivedByteAcknowledge(I2C_MODULE_ID index, bool ack)
{
     I2C_ReceivedByteAcknowledge_Default(index, ack);
}

PLIB_INLINE_API bool PLIB_I2C_ReceiverByteAcknowledgeHasCompleted(I2C_MODULE_ID index)
{
     return I2C_ReceiverByteAcknowledgeHasCompleted_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_MasterReceiverReadyToAcknowledge(I2C_MODULE_ID index)
{
     return I2C_MasterReceiverReadyToAcknowledge_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsReceivedByteAcknowledge(I2C_MODULE_ID index)
{
     return I2C_ExistsReceivedByteAcknowledge_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ReceiverOverflowHasOccurred(I2C_MODULE_ID index)
{
     return I2C_ReceiverOverflowHasOccurred_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_ReceiverOverflowClear(I2C_MODULE_ID index)
{
     I2C_ReceiverOverflowClear_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsReceiverOverflow(I2C_MODULE_ID index)
{
     return I2C_ExistsReceiverOverflow_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveInterruptOnStopEnable(I2C_MODULE_ID index)
{
     I2C_SlaveInterruptOnStopEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveInterruptOnStopDisable(I2C_MODULE_ID index)
{
     I2C_SlaveInterruptOnStopDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveInterruptOnStop(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveInterruptOnStop_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveInterruptOnStartEnable(I2C_MODULE_ID index)
{
     I2C_SlaveInterruptOnStartEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveInterruptOnStartDisable(I2C_MODULE_ID index)
{
     I2C_SlaveInterruptOnStartDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveInterruptOnStart(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveInterruptOnStart_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveBufferOverwriteEnable(I2C_MODULE_ID index)
{
     I2C_SlaveBufferOverwriteEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveBufferOverwriteDisable(I2C_MODULE_ID index)
{
     I2C_SlaveBufferOverwriteDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveBufferOverwrite(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveBufferOverwrite_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_DataLineHoldTimeSet(I2C_MODULE_ID index, I2C_SDA_MIN_HOLD_TIME sdaHoldTimeNs)
{
     I2C_DataLineHoldTimeSet_Default(index, sdaHoldTimeNs);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsDataLineHoldTime(I2C_MODULE_ID index)
{
     return I2C_ExistsDataLineHoldTime_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveBusCollisionDetectEnable(I2C_MODULE_ID index)
{
     I2C_SlaveBusCollisionDetectEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveBusCollisionDetectDisable(I2C_MODULE_ID index)
{
     I2C_SlaveBusCollisionDetectDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveBusCollisionDetect(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveBusCollisionDetect_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveAddressHoldEnable(I2C_MODULE_ID index)
{
     I2C_SlaveAddressHoldEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveAddressHoldDisable(I2C_MODULE_ID index)
{
     I2C_SlaveAddressHoldDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveAddressHoldEnable(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveAddressHoldEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveDataHoldEnable(I2C_MODULE_ID index)
{
     I2C_SlaveDataHoldEnable_Default(index);
}

PLIB_INLINE_API void PLIB_I2C_SlaveDataHoldDisable(I2C_MODULE_ID index)
{
     I2C_SlaveDataHoldDisable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsSlaveDataHoldEnable(I2C_MODULE_ID index)
{
     return I2C_ExistsSlaveDataHoldEnable_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_AcksequenceIsInProgress(I2C_MODULE_ID index)
{
     return I2C_AcksequenceIsInProgress_Default(index);
}

PLIB_INLINE_API bool PLIB_I2C_ExistsAcksequenceProgress(I2C_MODULE_ID index)
{
     return I2C_ExistsAcksequenceProgress_Default(index);
}

#endif
