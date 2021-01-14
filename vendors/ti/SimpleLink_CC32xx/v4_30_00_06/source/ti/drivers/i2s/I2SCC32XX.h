/*
 * Copyright (c) 2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       I2SCC32XX.h
 *
 *  @brief      I2S driver implementation for a CC32XX I2S controller
 *
 *  ============================================================================
 */
#ifndef ti_drivers_i2s_I2SCC32XX__include
#define ti_drivers_i2s_I2SCC32XX__include

#include <ti/drivers/I2S.h>
#include <ti/drivers/dma/UDMACC32XX.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/utils/List.h>
#include <ti/drivers/Power.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 *  Macros defining possible I2S signal pin mux options
 *
 *  The bits in the pin mode macros are as follows:
 *  The lower 8 bits of the macro refer to the pin, offset by 1, to match
 *  driverlib pin defines.  For example, I2SCC32XX_PIN_02_WS & 0xff = 1,
 *  which equals PIN_02 in driverlib pin.h.  By matching the PIN_xx defines in
 *  driverlib pin.h, we can pass the pin directly to the driverlib functions.
 *  The upper 8 bits of the macro correspond to the pin mux confg mode
 *  value for the pin to operate in the I2S mode.  For example, pin 2 is
 *  configured with mode 13 to operate as WS.
 *
 *  The macro I2SCC32XX_PIN_UNUSED allows the user to not activate one
 *  of the signal.
 */
/*! @cond HIDDEN_DEFINES */
#define I2SCC32XX_PIN_02_McAFSX  0x0d01 /*!< PIN 2 is used for McAFSX  */
#define I2SCC32XX_PIN_03_McACLK  0x0302 /*!< PIN 3 is used for McCLK   */
#define I2SCC32XX_PIN_15_McAFSX  0x070e /*!< PIN 15 is used for McAFSX */
#define I2SCC32XX_PIN_17_McAFSX  0x0610 /*!< PIN 17 is used for McAFSX */
#define I2SCC32XX_PIN_21_McAFSX  0x0214 /*!< PIN 21 is used for McAFSX */
#define I2SCC32XX_PIN_45_McAXR0  0x062c /*!< PIN 45 is used for McXR0  */
#define I2SCC32XX_PIN_45_McAFSX  0x0c2c /*!< PIN 45 is used for McAFSX */
#define I2SCC32XX_PIN_50_McAXR0  0x0431 /*!< PIN 50 is used for McXR0  */
#define I2SCC32XX_PIN_50_McAXR1  0x0631 /*!< PIN 50 is used for McXR1  */
#define I2SCC32XX_PIN_52_McACLK  0x0233 /*!< PIN 52 is used for McCLK  */
#define I2SCC32XX_PIN_52_McAXR0  0x0433 /*!< PIN 52 is used for McXR0  */
#define I2SCC32XX_PIN_53_McACLK  0x0234 /*!< PIN 53 is used for McCLK  */
#define I2SCC32XX_PIN_53_McAFSX  0x0334 /*!< PIN 53 is used for McAFSX */
#define I2SCC32XX_PIN_60_McAXR1  0x063b /*!< PIN 60 is used for McXR1  */
#define I2SCC32XX_PIN_62_McACLKX 0x0d3d /*!< PIN 62 is used for McCLKX */
#define I2SCC32XX_PIN_63_McAFSX  0x073e /*!< PIN 53 is used for McAFSX */
#define I2SCC32XX_PIN_64_McAXR0  0x073f /*!< PIN 64 is used for McXR0  */
#define I2SCC32XX_PIN_UNUSED     0xffff /*!< PIN non activated  */

#define I2SCC32XX_PIN_50_SD1     I2SCC32XX_PIN_50_McAXR1   /*!< PIN 50 is used for SD1  */
#define I2SCC32XX_PIN_60_SD1     I2SCC32XX_PIN_60_McAXR1   /*!< PIN 60 is used for SD1  */
#define I2SCC32XX_PIN_52_SD0     I2SCC32XX_PIN_52_McAXR0   /*!< PIN 52 is used for SD0  */
#define I2SCC32XX_PIN_64_SD0     I2SCC32XX_PIN_64_McAXR0   /*!< PIN 64 is used for SD0  */
#define I2SCC32XX_PIN_45_SD0     I2SCC32XX_PIN_45_McAXR0   /*!< PIN 45 is used for SD0  */
#define I2SCC32XX_PIN_50_SD0     I2SCC32XX_PIN_50_McAXR0   /*!< PIN 50 is used for SD0  */
#define I2SCC32XX_PIN_03_SCK     I2SCC32XX_PIN_03_McACLK   /*!< PIN 03 is used for SCK  */
#define I2SCC32XX_PIN_52_SCK     I2SCC32XX_PIN_52_McACLK   /*!< PIN 52 is used for SCK  */
#define I2SCC32XX_PIN_53_SCK     I2SCC32XX_PIN_53_McACLK   /*!< PIN 53 is used for SCK  */
#define I2SCC32XX_PIN_62_SCKX    I2SCC32XX_PIN_62_McACLKX  /*!< PIN 62 is used for SCKX */
#define I2SCC32XX_PIN_02_WS      I2SCC32XX_PIN_02_McAFSX   /*!< PIN 02 is used for WS   */
#define I2SCC32XX_PIN_15_WS      I2SCC32XX_PIN_15_McAFSX   /*!< PIN 15 is used for WS   */
#define I2SCC32XX_PIN_17_WS      I2SCC32XX_PIN_17_McAFSX   /*!< PIN 17 is used for WS   */
#define I2SCC32XX_PIN_21_WS      I2SCC32XX_PIN_21_McAFSX   /*!< PIN 21 is used for WS   */
#define I2SCC32XX_PIN_45_WS      I2SCC32XX_PIN_45_McAFSX   /*!< PIN 45 is used for WS   */
#define I2SCC32XX_PIN_63_WS      I2SCC32XX_PIN_63_McAFSX   /*!< PIN 63 is used for WS   */
#define I2SCC32XX_PIN_53_WS      I2SCC32XX_PIN_53_McAFSX   /*!< PIN 53 is used for WS   */

/*! @endcond*/

/*!
 *  @brief  I2S Hardware attributes
 *
 *  intPriority is the I2S peripheral's interrupt priority, as defined by the
 *  TI-RTOS kernel. This value is passed unmodified to Hwi_create().
 *
 *  pinSD1 and pinSD0 define the SD0 and SD1 data pin mapping, respectively.
 *  pinSCK, pinMCLK and pinWS define the SCK, MCLK and WS clock pin mapping, respectively.
 *  All these pins are typically defined with a macro in a header file, which maps to an IOID.
 *
 *  rxChannelIndex and txChannelIndex are uDMA channels respectively used for the read and
 *  the write interface.
 *
 *  A sample structure is shown below:
 *  @code
 *  const I2SCC32XX_HWAttrs i2sCC3220SHWAttrs[CC3220S_LAUNCHXL_I2SCOUNT] = {
 *      {
 *        .pinSD1           =  I2SCC32XX_PIN_50_SD1,
 *        .pinSD0           =  I2SCC32XX_PIN_64_SD0,
 *        .pinSCK           =  I2SCC32XX_PIN_53_SCK,
 *        .pinSCKX          =  I2SCC32XX_PIN_UNUSED,
 *        .pinWS            =  I2SCC32XX_PIN_63_WS,
 *        .rxChannelIndex   =  UDMA_CH4_I2S_RX,
 *        .txChannelIndex   =  UDMA_CH5_I2S_TX,
 *        .intPriority      =  0x40,
 *      }
 *  };
 *  @endcode
 */
typedef struct {
    uint32_t                        pinSD1;                  /*!< Pin used for SD1 signal.
                                                                  Must be chosen between I2SCC32XX_PIN_50_SD1 and I2SCC32XX_PIN_60_SD1.
                                                                  Signal can be deactivated using I2SCC32XX_PIN_UNUSED. */
    uint32_t                        pinSD0;                  /*!< Pin used for SD0 signal.
                                                                  Must be chosen between I2SCC32XX_PIN_45_SD0, I2SCC32XX_PIN_50_SD0, I2SCC32XX_PIN_52_SD0, and I2SCC32XX_PIN_64_SD0.
                                                                  Signal can be deactivated using I2SCC32XX_PIN_UNUSED.*/
    uint32_t                        pinSCK;                  /*!< Pin used for SCK signal.
                                                                  Must be chosen between I2SCC32XX_PIN_03_SCK, I2SCC32XX_PIN_52_SCK, and I2SCC32XX_PIN_53_SCK.
                                                                  Signal can be deactivated using I2SCC32XX_PIN_UNUSED.*/
    uint32_t                        pinSCKX;                 /*!< Pin used for SCKX signal.
                                                                  Must be I2SCC32XX_PIN_62_SCKX. */
    uint32_t                        pinWS;                   /*!< Pin used for WS signal.
                                                                  Must be chosen between I2SCC32XX_PIN_02_WS, I2SCC32XX_PIN_15_WS, I2SCC32XX_PIN_17_WS, I2SCC32XX_PIN_21_WS, I2SCC32XX_PIN_45_WS, I2SCC32XX_PIN_53_WS, and I2SCC32XX_PIN_63_WS.
                                                                  This signal cannot be deactivated.*/
    uint32_t                        rxChannelIndex;          /*!< uDMA channel index used for Rx.
                                                                  Must be chosen between UDMA_CH4_I2S_RX and UDMA_CH18_I2S_RX. Does not need to be set if no using the DMA transfers. */
    uint32_t                        txChannelIndex;          /*!< uDMA channel index used for Tx.
                                                                  Must be chosen between UDMA_CH5_I2S_TX and UDMA_CH19_I2S_TX. Does not need to be set if no using the DMA transfers. */
    uint32_t                        intPriority;             /*!< I2S Peripheral's interrupt priority */
}I2SCC32XX_HWAttrs;

/*!
 *  @cond NODOC
 *  I2S data-interface
 *
 *  This struct defines how the physical I2S interface (SD0/SD1) behaves.
 *  Do not modify.
 */
typedef struct {
    I2S_DataInterfaceUse            interfaceConfig;          /*!< IN / OUT / UNUSED */
    I2S_ChannelConfig               channelsUsed;             /*!< List of the used channels. */
    uint8_t                         numberOfChannelsUsed;     /*!< Number of channels used on SDx. */
    uint8_t                         dataLine;                 /*!< DataLine used by SDx (I2S_DATA_LINE_0 or I2S_DATA_LINE_1) */
}I2SCC32XX_DataInterface;
/*! @endcond */

/*!
 *  @cond NODOC
 *  I2S interface
 *
 *  This enum defines one of the interfaces (READ or WRITE) of the I2S module.
 *  Do not modify.
 */
typedef struct {
    uint16_t                        delay;                    /*!< Number of WS cycles to wait before starting the first transfer. This value is mostly used when performing constant latency transfers. */
    I2S_Transaction                *activeTransfer;           /*!< Pointer on the ongoing transfer */
    I2S_Callback                    callback;                 /*!< Pointer to callback */
    uint32_t                        udmaConfig;               /*!< Bit field. It's a OR of  UDMA_SIZE_xx, UDMA_CHCTL_SRCINC_xx, UDMA_DSTINC_xx, UDMA_ARB_xx. */
    I2S_StopInterface               stopInterface;            /*!< Pointer on the function used to stop the interface */
}I2SCC32XX_Interface;
/*! @endcond */

/*!
 *  @brief      The definition of a function used by the I2S driver
 *              to refresh the FIFO
 *
 *  @param      I2S_Handle  I2S_Handle
 *
 */
typedef void (*I2SCC32XX_FifoUpdate)(uintptr_t arg);

/*!
 *  @cond NODOC
 *  I2S Object.  The application must not access any member variables
 *  of this structure!
 */
typedef struct {

    bool                            isOpen;                  /*!< To avoid multiple openings of the I2S. */
    bool                            invertWS;                /*!< WS inversion.
                                                                          false: The WS signal is not internally inverted.
                                                                          true:  The WS signal is internally inverted. */
    bool                            isMSBFirst;              /*!< Endianness of the samples transmitted.
                                                                          false: Transmission are performed LSB first.
                                                                          true:  Transmission are performed MSB first. */
    bool                            isDMAUnused;             /*!< Selection between DMA transmissions and CPU transmissions.
                                                                          false: Transmission are performed by DMA.
                                                                          true:  Transmission are performed by CPU. */
    volatile bool                   isLastReadTransfer;      /*!< This boolean indicates that no new DMA transfer must be started for the read interface.
                                                                      The read interface will be stopped as soon as the current DMA transfer is finished.
                                                                      This parameter is valid only in DMA mode. */
    volatile bool                   isLastWriteTransfer;     /*!< This boolean indicates that no new DMA transfer must be started for the write interface.
                                                                      The write interface will be stopped as soon as the current DMA transfer is finished.
                                                                      This parameter is valid only in DMA mode. */
    uint8_t                         dataShift;               /*!< When dataShift is set to 0, data are read/write on the data lines from the first SCK edge of the WS half-period.
                                                                  By setting dataShift to a value different from zero, you can postpone the moment when data are read/write during the WS half-period.
                                                                  For example, by setting dataShift to 1, data are read/write on the data lines from the second SCK period of the WS half-period. If no padding is activated, this corresponds to the I2S standard.
                                                                  The only values possible for dataShift are DATA_SHIFT_NONE, DATA_SHIFT_1 and DATA_SHIFT_2 */
    uint8_t                         memorySlotLength;        /*!< Select the size of the memory used.
                                                                          I2S_MEMORY_LENGTH_8BITS  : Memory length is 8  bits.
                                                                          I2S_MEMORY_LENGTH_16BITS : Memory length is 16 bits.
                                                                          I2S_MEMORY_LENGTH_24BITS : Memory length is 24 bits.
                                                                          I2S_MEMORY_LENGTH_32BITS : Memory length is 32 bits. */
    uint8_t                         sampleRotation;          /*!< Right rotation of the samples: used to configure the padding before and after the word. */
    uint8_t                         noOfInputs;              /*!< Number of I2S inputs (possible values are 0, 1 and 2) */
    uint8_t                         noOfOutputs;             /*!< Number of I2S outputs (possible values are 0, 1 and 2) */
    uint8_t                         udmaArbLength;           /*!< Number of uDMA transfers between two bus arbitration. By increasing this value, you increase the risk of incomplete I2S transfers. Possible values are 1, 2, 4(default), 8, 16, 32, 64 and 128. */
    uint8_t                         dataLength;              /*!< Length (in bits) of the data transmitted. Must be between 8 and 32. */
    I2S_Role                        moduleRole;              /*!< Select if the current device is a Slave or a Master.
                                                                    I2S_SLAVE:  The device is a slave (clocks are generated externally).
                                                                    I2S_MASTER: The device is a master (clocks are generated internally). */
    I2S_SamplingEdge                samplingEdge;            /*!< Select edge sampling type.
                                                                     I2S_SAMPLING_EDGE_FALLING: Sampling on falling edges.
                                                                     I2S_SAMPLING_EDGE_RISING:  Sampling on raising edges. */
    uint32_t                        samplingFrequency;       /*!< I2S sampling frequency configuration in samples/second. */
    uint32_t                        sampleMask;              /*!< Mask applied to the samples: used to perform sample truncation and avoid transmission of unexpected data. */
    uint32_t                        activatedFlag;           /*!< List of the activated flags. This field is required for CC32XX as we do not have a mechanism to only read the activated flags.  */
    I2SCC32XX_DataInterface         dataInterfaceSD0;        /*!< Structure to describe the SD0 interface */
    I2SCC32XX_DataInterface         dataInterfaceSD1;        /*!< Structure to describe the SD1 interface */
    I2SCC32XX_Interface             read;                    /*!< Structure to describe the read (in) interface */
    I2SCC32XX_Interface             write;                   /*!< Structure to describe the write (out) interface */
    I2S_Callback                    errorCallback;           /*!< Pointer to error callback */
    I2SCC32XX_FifoUpdate            updateDataReadFxn;       /*!< Pointer on the function used to update the FIFO */
    I2SCC32XX_FifoUpdate            updateDataWriteFxn;      /*!< Pointer on the function used to update the FIFO */
    HwiP_Handle                     hwi;                     /*!< Hwi object for interrupts */
    UDMACC32XX_Handle               dmaHandle;               /*!< DMA object */
    Power_NotifyObj                 notifyObj;               /*!< For notification of wake from LPDS */
} I2SCC32XX_Object;
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2s_I2SCC32XX__include */
