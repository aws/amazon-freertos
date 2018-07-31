/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __USB_HOST_OHCI_H__
#define __USB_HOST_OHCI_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! HcRevision Register */
#define USB_HOST_OHCI_REVISION_MASK (0xFFU)

/*! HcControl Register */
#define USB_HOST_OHCI_CONTROL_CBSR_MASK (0x03U)
#define USB_HOST_OHCI_CONTROL_CBSR_C1_B1 (0U)
#define USB_HOST_OHCI_CONTROL_CBSR_C2_B1 (1U)
#define USB_HOST_OHCI_CONTROL_CBSR_C3_B1 (2U)
#define USB_HOST_OHCI_CONTROL_CBSR_C4_B1 (3U)
#define USB_HOST_OHCI_CONTROL_CBSR(n) ((n)&USB_HOST_OHCI_CONTROL_CBSR_MASK)
#define USB_HOST_OHCI_CONTROL_PLE_MASK (0x04U)
#define USB_HOST_OHCI_CONTROL_IE_MASK (0x08U)
#define USB_HOST_OHCI_CONTROL_CLE_MASK (0x10U)
#define USB_HOST_OHCI_CONTROL_BLE_MASK (0x20U)
#define USB_HOST_OHCI_CONTROL_HCFS_MASK (0xC0U)
#define USB_HOST_OHCI_CONTROL_HCFS_SHIFT (0x06U)
#define USB_HOST_OHCI_CONTROL_HCFS_RESET (0x00U)
#define USB_HOST_OHCI_CONTROL_HCFS_RESUME (0x01U)
#define USB_HOST_OHCI_CONTROL_HCFS_OPERATIONAL (0x02U)
#define USB_HOST_OHCI_CONTROL_HCFS_SUSPEND (0x03U)
#define USB_HOST_OHCI_CONTROL_HCFS(n) (((n) << USB_HOST_OHCI_CONTROL_HCFS_SHIFT) & USB_HOST_OHCI_CONTROL_HCFS_MASK)
#define USB_HOST_OHCI_CONTROL_IR_MASK (0x100U)
#define USB_HOST_OHCI_CONTROL_RWC_MASK (0x200U)
#define USB_HOST_OHCI_CONTROL_RWE_MASK (0x400U)

/*! HcCommandStatus Register */
#define USB_HOST_OHCI_COMMAND_STATUS_HCR_MASK (0x01U)
#define USB_HOST_OHCI_COMMAND_STATUS_CLF_MASK (0x02U)
#define USB_HOST_OHCI_COMMAND_STATUS_BLF_MASK (0x04U)
#define USB_HOST_OHCI_COMMAND_STATUS_OCR_MASK (0x08U)
#define USB_HOST_OHCI_COMMAND_STATUS_SOC_MASK (0x30000U)

/*! HcInterruptStatus Register */
#define USB_HOST_OHCI_INTERRUPT_STATUS_SO_MASK (0x01U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_WDH_MASK (0x02U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_SF_MASK (0x04U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_RD_MASK (0x08U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_UE_MASK (0x10U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_FNO_MASK (0x20U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_RHSC_MASK (0x40U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_OC_MASK (0x40000000U)
#define USB_HOST_OHCI_INTERRUPT_STATUS_MASK (0x4000007FU)

/*! HcInterruptEnable Register */
#define USB_HOST_OHCI_INTERRUPT_ENABLE_SO_MASK (0x01U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_WDH_MASK (0x02U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_SF_MASK (0x04U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_RD_MASK (0x08U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_UE_MASK (0x10U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_FNO_MASK (0x20U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_RHSC_MASK (0x40U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_OC_MASK (0x40000000U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_MIE_MASK (0x80000000U)
#define USB_HOST_OHCI_INTERRUPT_ENABLE_MASK (0x8000007FU)

/*! HcInterruptDisable Register */
#define USB_HOST_OHCI_INTERRUPT_DISABLE_SO_MASK (0x01U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_WDH_MASK (0x02U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_SF_MASK (0x04U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_RD_MASK (0x08U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_UE_MASK (0x10U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_FNO_MASK (0x20U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_RHSC_MASK (0x40U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_OC_MASK (0x40000000U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_MIE_MASK (0x80000000U)
#define USB_HOST_OHCI_INTERRUPT_DISABLE_MASK (0xC000007FU)

/*! HcHCCA Register */
#define USB_HOST_OHCI_HCCA_MASK (0xFFFFFF00U)
#define USB_HOST_OHCI_HCCA_SIZE (32U)

/*! HcPeriodCurrentED Register */
#define USB_HOST_OHCI_PCED_MASK (0xFFFFFFF0U)

/*! HcControlHeadED Register */
#define USB_HOST_OHCI_CHED_MASK (0xFFFFFFF0U)

/*! HcControlCurrentED Register */
#define USB_HOST_OHCI_CCED_MASK (0xFFFFFFF0U)

/*! HcBulkHeadED Register */
#define USB_HOST_OHCI_BHED_MASK (0xFFFFFFF0U)

/*! HcBulkCurrentED Register */
#define USB_HOST_OHCI_BCED_MASK (0xFFFFFFF0U)

/*! HcDoneHead Register */
#define USB_HOST_OHCI_HD_MASK (0xFFFFFFF0U)

/*! HcFmInterval Register */
#define USB_HOST_OHCI_SOF_INTERVAL (11999U)
#define USB_HOST_OHCI_FMINTERVAL_FI_MASK (0x3FFFU)
#define USB_HOST_OHCI_FMINTERVAL_FI(n) ((n)&USB_HOST_OHCI_FMINTERVAL_FI_MASK)
#define USB_HOST_OHCI_FMINTERVAL_FSMPS_MASK (0x7FFF0000U)
#define USB_HOST_OHCI_FMINTERVAL_FSMPS_SHIFT (16U)
#define USB_HOST_OHCI_FMINTERVAL_FSMPS(n) \
    (((n) << USB_HOST_OHCI_FMINTERVAL_FSMPS_SHIFT) & USB_HOST_OHCI_FMINTERVAL_FSMPS_MASK)
#define USB_HOST_OHCI_FMINTERVAL_FIT_MASK (0x80000000U)

/*! HcFmRemaining Register */
#define USB_HOST_OHCI_FMREMAINING_FR_MASK (0x3FFFU)
#define USB_HOST_OHCI_FMREMAINING_FR(n) ((n)&USB_HOST_OHCI_FMINTERVAL_FR_MASK)
#define USB_HOST_OHCI_FMREMAINING_FRT_MASK (0x80000000U)

/*! HcFmNumber Register */
#define USB_HOST_OHCI_FMNUMBER_FN_MASK (0xFFFFU)

/*! HcPeriodicStart Register */
#define USB_HOST_OHCI_PERIODIC_START_PS_MASK (0x3FFFU)
#define USB_HOST_OHCI_PERIODIC_START_PS(n) ((n)&USB_HOST_OHCI_PERIODIC_START_PS_MASK)

/*! HcLSThreshold Register */
#define USB_HOST_OHCI_LSTHRESHOLD_LST_MASK (0xFFFU)
#define USB_HOST_OHCI_LSTHRESHOLD_LST(n) ((n)&USB_HOST_OHCI_LSTHRESHOLD_LST_MASK)

/*! HcRhDescriptorA Register */
#define USB_HOST_OHCI_RHDESCRIPTORA_NDP_MASK (0xFFU)
#define USB_HOST_OHCI_RHDESCRIPTORA_PSM_MASK (0x100U)
#define USB_HOST_OHCI_RHDESCRIPTORA_NPS_MASK (0x200U)
#define USB_HOST_OHCI_RHDESCRIPTORA_DT_MASK (0x400U)
#define USB_HOST_OHCI_RHDESCRIPTORA_OCPM_MASK (0x800U)
#define USB_HOST_OHCI_RHDESCRIPTORA_NOCP_MASK (0x1000U)
#define USB_HOST_OHCI_RHDESCRIPTORA_POTPGT_MASK (0xFF000000U)
#define USB_HOST_OHCI_RHDESCRIPTORA_POTPGT_SHIFT (24U)
#define USB_HOST_OHCI_RHDESCRIPTORA_POTPGT(n) \
    (((n) << USB_HOST_OHCI_RHDESCRIPTORA_POTPGT_SHIFT) & USB_HOST_OHCI_RHDESCRIPTORA_POTPGT_MASK)

/*! HcRhDescriptorB Register */
#define USB_HOST_OHCI_RHDESCRIPTORB_DR_MASK (0xFFFFU)
#define USB_HOST_OHCI_RHDESCRIPTORB_DR(n) ((n)&USB_HOST_OHCI_RHDESCRIPTORB_DR_MASK)
#define USB_HOST_OHCI_RHDESCRIPTORB_PPCM_MASK (0xFFFF0000U)
#define USB_HOST_OHCI_RHDESCRIPTORB_PPCM_SHIFT (16U)
#define USB_HOST_OHCI_RHDESCRIPTORB_PPCM(n) \
    (((n) << USB_HOST_OHCI_RHDESCRIPTORB_PPCM_SHIFT) & USB_HOST_OHCI_RHDESCRIPTORB_PPCM_MASK)

/*! HcRhStatus Register */
#define USB_HOST_OHCI_RHSTATUS_LPS_MASK (0x01U)
#define USB_HOST_OHCI_RHSTATUS_OCI_MASK (0x02U)
#define USB_HOST_OHCI_RHSTATUS_DRWE_MASK (0x8000U)
#define USB_HOST_OHCI_RHSTATUS_LPSC_MASK (0x10000U)
#define USB_HOST_OHCI_RHSTATUS_OCIC_MASK (0x20000U)
#define USB_HOST_OHCI_RHSTATUS_CRWE_MASK (0x80000000U)

/*! HcRhPortStatus[1:NDP] Register */
#define USB_HOST_OHCI_RHPORTSTATUS_CCS_MASK (0x01U)
#define USB_HOST_OHCI_RHPORTSTATUS_PES_MASK (0x02U)
#define USB_HOST_OHCI_RHPORTSTATUS_PSS_MASK (0x04U)
#define USB_HOST_OHCI_RHPORTSTATUS_POCI_MASK (0x08U)
#define USB_HOST_OHCI_RHPORTSTATUS_PRS_MASK (0x10U)
#define USB_HOST_OHCI_RHPORTSTATUS_PPS_MASK (0x100U)
#define USB_HOST_OHCI_RHPORTSTATUS_LSDA_MASK (0x200U)
#define USB_HOST_OHCI_RHPORTSTATUS_CSC_MASK (0x10000U)
#define USB_HOST_OHCI_RHPORTSTATUS_PESC_MASK (0x20000U)
#define USB_HOST_OHCI_RHPORTSTATUS_PSSC_MASK (0x40000U)
#define USB_HOST_OHCI_RHPORTSTATUS_OCIC_MASK (0x80000U)
#define USB_HOST_OHCI_RHPORTSTATUS_PRSC_MASK (0x100000U)
#define USB_HOST_OHCI_RHPORTSTATUS_WIC (0x1F0000U)

/*! PortMode Register */
#define USB_HOST_OHCI_PORTMODE_ID_MASK (0x01U)
#define USB_HOST_OHCI_PORTMODE_ID_EN_MASK (0x100U)
#define USB_HOST_OHCI_PORTMODE_DEV_ENABLE_MASK (0x10000U)

/*! Completion Codes */
#define USB_HOST_OHCI_CC_NO_ERROR (0U)
#define USB_HOST_OHCI_CC_CRC (1U)
#define USB_HOST_OHCI_CC_BIT_STUFFING (2U)
#define USB_HOST_OHCI_CC_DATA_TOGGLE_MISMATCH (3U)
#define USB_HOST_OHCI_CC_STALL (4U)
#define USB_HOST_OHCI_CC_DEVICE_NOT_RESPONDING (5U)
#define USB_HOST_OHCI_CC_PID_CHECK_FAILURE (6U)
#define USB_HOST_OHCI_CC_UNEXPECTED_PID (7U)
#define USB_HOST_OHCI_CC_DATA_OVERRUN (8U)
#define USB_HOST_OHCI_CC_DATA_UNDERRUN (9U)
#define USB_HOST_OHCI_CC_BUFFER_OVERRUN (12U)
#define USB_HOST_OHCI_CC_BUFFER_UNDERRUN (13U)
#define USB_HOST_OHCI_CC_NOT_ACCESSED (14U)

#define USB_HOST_OHCI_ED_HEADP_HALT_MASK (0x00000001U)  /*!< Stopped bit */
#define USB_HOST_OHCI_ED_HEADP_CARRY_MASK (0x00000002U) /*!< Toggle carry bit */
#define USB_HOST_OHCI_ED_HEADP_MASK (0xFFFFFFF0U)
#define USB_HOST_OHCI_ED_TAILP_MASK (0xFFFFFFF0U)

#define USB_HOST_OHCI_GTD_DP_SETUP (0U)
#define USB_HOST_OHCI_GTD_DP_OUT (1U)
#define USB_HOST_OHCI_GTD_DP_IN (2U)
#define USB_HOST_OHCI_GTD_DI_NO_INTERRUPT (7U)

#define USB_HOST_OHCI_ITD_BP0_MASK (0xFFFFF000U)
/*!
 * @addtogroup usb_host_controller_ohci
 * @{
 */
/*! @brief OHCI Endpoint Descriptor */
typedef struct _usb_host_ohci_endpoint_descritpor_struct
{
    union
    {
        uint32_t state;
        struct
        {
            uint32_t FA : 7U;            /*!< FunctionAddress */
            uint32_t EN : 4U;            /*!< EndpointNumber */
            uint32_t D : 2U;             /*!< Direction: 00,11 - Get dir from TD, 01 - OUT, 10 - IN. */
            uint32_t S : 1U;             /*!< Speed: 0 - full speed, 1 - low speed. */
            uint32_t K : 1U;             /*!< Skip */
            uint32_t F : 1U;             /*!< Format: 0 - Control, Bulk, or Interrupt Endpoint, 1 - ISO Endpoint. */
            volatile uint32_t MPS : 11U; /*!< MaximumPacketSize */
            uint32_t reserved1 : 5U;     /*!< Reserved */
        } stateBitField;
    } stateUnion;
    volatile uint32_t TailP;                 /*!< TDQueueTailPointer */
    volatile uint32_t HeadP;                 /*!< TDQueueHeadPointer */
    volatile uint32_t NextED;                /*!< NextED */
    struct _usb_host_ohci_pipe_struct *pipe; /*!< Pipe handle for the ED */
    usb_host_transfer_t *trListHead;
    usb_host_transfer_t *trListTail;
    usb_host_transfer_t *dealTr;
} usb_host_ohci_endpoint_descritpor_struct_t;

/*! @brief OHCI General Transfer Descriptor */
typedef struct _usb_host_ohci_general_transfer_descritpor_struct
{
    union
    {
        uint32_t state;
        struct
        {
            uint32_t reserved1 : 18U;  /*!< Reserved */
            volatile uint32_t R : 1U;  /*!< bufferRounding */
            volatile uint32_t DP : 2U; /*!< Direction/PID */
            volatile uint32_t DI : 3U; /*!< DelayInterrupt */
            volatile uint32_t T : 2U;  /*!< DataToggle */
            volatile uint32_t EC : 2U; /*!< ErrorCount */
            volatile uint32_t CC : 4U; /*!< ConditionCode */
        } stateBitField;
    } stateUnion;
    uint32_t CBP;                            /*!< CurrentBufferPointer */
    volatile uint32_t NextTD;                /*!< NextTD */
    uint32_t BE;                             /*!< BufferEnd */
    struct _usb_host_ohci_pipe_struct *pipe; /*!< Pipe handle for the GTD */
    usb_host_transfer_t *tr;
    struct _usb_host_ohci_general_transfer_descritpor_struct *nextGtd;
    uint32_t length;
} usb_host_ohci_general_transfer_descritpor_struct_t;

/*! @brief OHCI Isochronous Transfer Descriptor */
typedef struct _usb_host_ohci_isochronous_transfer_descritpor_struct
{
    union
    {
        uint32_t state;
        struct
        {
            volatile uint32_t SF : 16U; /*!< StartingFrame */
            uint32_t reserved1 : 5U;    /*!< Reserved */
            volatile uint32_t DI : 3U;  /*!< DelayInterrupt */
            volatile uint32_t FC : 3U;  /*!< FrameCount */
            uint32_t reserved2 : 1U;    /*!< Reserved */
            volatile uint32_t CC : 4U;  /*!< ConditionCode */
        } stateBitField;
    } stateUnion;
    uint32_t BP0;                            /*!< BufferPage0 */
    volatile uint32_t NextTD;                /*!< NextTD */
    uint32_t BE;                             /*!< BufferEnd */
    volatile uint16_t OffsetPSW[8];          /*!< Offest or PacketStatusWord */
    struct _usb_host_ohci_pipe_struct *pipe; /*!< Pipe handle for the ITD */
    usb_host_transfer_t *tr;
    struct _usb_host_ohci_isochronous_transfer_descritpor_struct *nextItd;
    uint32_t length;
    uint32_t reserved[4];
} usb_host_ohci_isochronous_transfer_descritpor_struct_t;

/*! @brief OHCI Host Controller Communications Area */
typedef struct _usb_host_ohci_hcca_struct
{
    volatile uint32_t
        HccaInterrruptTable[USB_HOST_OHCI_HCCA_SIZE]; /*!< These 32 Dwords are pointers to interrupt EDs */
    volatile uint16_t HccaFrameNumber;                /*!< Contains the current frame number */
    uint16_t HccaPad1;              /*!< When the HC updates HccaFrameNumber, it sets this word to 0 */
    volatile uint32_t HccaDoneHead; /*!< Hcca done head */
    uint32_t reserved[30];          /*!< Reserved for use by HC */
} usb_host_ohci_hcca_struct_t;

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
typedef enum _bus_ohci_suspend_request_state
{
    kBus_OhciIdle = 0U,
    kBus_OhciStartSuspend,
    kBus_OhciSuspended,
    kBus_OhciStartResume,
} bus_ohci_suspend_request_state_t;
#endif

#define USB_HOST_OHCI_CONTROL_ED_DUMMY (0U)
#define USB_HOST_OHCI_BULK_ED_DUMMY (1U)
#define USB_HOST_OHCI_PERIODIC_ED_DUMMY (2U)

#define USB_HOST_OHCI_TIME_DELAY (3500U)
#define USB_HOST_OHCI_HUB_LS_SETUP_TIME_DELAY (333U)

#define USB_HOST_OHCI_PERIODIC_BANDWIDTH_PERCENT ((float)80U / (float)100U)
#define USB_HOST_OHCI_PERIODIC_BANDWIDTH (1000 * USB_HOST_OHCI_PERIODIC_BANDWIDTH_PERCENT)

#define USB_HOST_OHCI_GTD_MAX_LENGTH (0x2000U)
#define USB_HOST_OHCI_GTD_BUFFER_ALIGN_MASK (0x0FFFU)

typedef struct _usb_host_ohci_td_struct
{
#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
    usb_host_ohci_isochronous_transfer_descritpor_struct_t itd[USB_HOST_CONFIG_OHCI_MAX_ITD];
#endif
#if (defined(USB_HOST_CONFIG_OHCI_MAX_GTD) && (USB_HOST_CONFIG_OHCI_MAX_GTD > 0U))
    usb_host_ohci_general_transfer_descritpor_struct_t gtd[USB_HOST_CONFIG_OHCI_MAX_GTD];
#endif
    usb_host_ohci_endpoint_descritpor_struct_t ed[USB_HOST_CONFIG_OHCI_MAX_ED + 3U];
} usb_host_ohci_td_struct_t;

/*! @brief task event */
#define USB_HOST_OHCI_EVENT_ATTACH (0x01U)
#define USB_HOST_OHCI_EVENT_DETACH (0x02U)
#define USB_HOST_OHCI_EVENT_PORT_CHANGE (0x04U)
#define USB_HOST_OHCI_EVENT_TOKEN_DONE (0x08U)
#define USB_HOST_OHCI_EVENT_SOF (0x10U)

/*! @brief Check the port connect state delay if the state is unstable */
#define USB_HOST_OHCI_PORT_CONNECT_DEBOUNCE_DELAY (101U)

#define USB_HOST_OHCI_PIPE_QUEUE_TYPE_IDLE (0U)
#define USB_HOST_OHCI_PIPE_QUEUE_TYPE_INUSING (1U)

#define USB_HOST_OHCI_ISO_TRANSFER_GAP (3U)

/*! @brief Transfer scan interval (ms)*/
#define USB_HOST_OHCI_TRANSFER_SCAN_INTERVAL (10U)
/*! @brief Time out gap for each transfer (USB_HOST_OHCI_TRANSFER_SCAN_INTERVAL * 1ms) */
#define USB_HOST_OHCI_TRANSFER_TIMEOUT_GAP (5000U / USB_HOST_OHCI_TRANSFER_SCAN_INTERVAL)

/*! @brief USB host OHCI lock */
#define USB_HostOhciLock() USB_OsaMutexLock(usbHostState->mutex)
/*! @brief USB host OHCI unlock */
#define USB_HostOhciUnlock() USB_OsaMutexUnlock(usbHostState->mutex)

/*! @brief OHCI Host Controller Operational Registers */
typedef struct _usb_host_ohci_hcor_struct
{
    __I uint32_t HcRevision;          /* Offset 0x00 */
    __IO uint32_t HcControl;          /* Offset 0x04 */
    __IO uint32_t HcCommandStatus;    /* Offset 0x08 */
    __IO uint32_t HcInterruptStatus;  /* Offset 0x0C */
    __IO uint32_t HcInterruptEnable;  /* Offset 0x10 */
    __IO uint32_t HcInterruptDisable; /* Offset 0x14 */
    __IO uint32_t HcHCCA;             /* Offset 0x18 */
    __I uint32_t HcPeriodCurrentED;   /* Offset 0x1C */
    __IO uint32_t HcControlHeadED;    /* Offset 0x20 */
    __IO uint32_t HcControlCurrentED; /* Offset 0x24 */
    __IO uint32_t HcBulkHeadED;       /* Offset 0x28 */
    __IO uint32_t HcBulkCurrentED;    /* Offset 0x2C */
    __I uint32_t HcDoneHead;          /* Offset 0x30 */
    __IO uint32_t HcFmInterval;       /* Offset 0x34 */
    __I uint32_t HcFmRemaining;       /* Offset 0x38 */
    __I uint32_t HcFmNumber;          /* Offset 0x3C */
    __IO uint32_t HcPeriodicStart;    /* Offset 0x40 */
    __IO uint32_t HcLSThreshold;      /* Offset 0x44 */
    __IO uint32_t HcRhDescriptorA;    /* Offset 0x48 */
    __IO uint32_t HcRhDescriptorB;    /* Offset 0x4C */
    __IO uint32_t HcRhStatus;         /* Offset 0x50 */
    __IO uint32_t HcRhPortStatus[2];  /* Offset 0x54 - 0x58 */
    __IO uint32_t PortMode;           /* Offset 0x5C */
} usb_host_ohci_hcor_struct_t;

/*! @brief Port state for device attachment/detachment. */
typedef enum _usb_host_ohci_port_state
{
    kUSB_DeviceOhciPortDetached = 0U, /*!< Port is detached and de-initialized */
    kUSB_DeviceOhciPortPhyDetached,   /*!< Port is physically detached */
    kUSB_DeviceOhciPortPhyAttached,   /*!< Port is physically attached */
    kUSB_DeviceOhciPortAttached,      /*!< Port is attached and initialized */
} usb_host_ohci_port_state_t;

typedef struct _usb_host_ohci_port_state_struct
{
    uint8_t portSpeed;  /*!< Port speed */
    uint8_t portStatus; /*!< Port status */
} usb_host_ohci_port_state_struct_t;

typedef struct _usb_host_ohci_pipe_struct
{
    usb_host_pipe_t pipeCommon; /*!< Common pipe information */
    usb_host_ohci_endpoint_descritpor_struct_t *ed;
    usb_host_transfer_t *currentTr;
    uint32_t cutOffTime;
    volatile uint32_t startWriteBackCount;
    uint16_t busTime;
    uint16_t startFrame;
    volatile uint8_t isBusy;
    volatile uint8_t isDone;
    volatile uint8_t isCanceling;
} usb_host_ohci_pipe_struct_t;

typedef enum _usb_host_ohci_transfer_status
{
    kStatus_UsbHostOhci_Idle = 0,
    kStatus_UsbHostOhci_Setup,
    kStatus_UsbHostOhci_Data,
    kStatus_UsbHostOhci_State,
} usb_host_ohci_transfer_status_t;

/*! @brief OHCI controller driver instance structure */
typedef struct _usb_host_ohci_state_struct
{
    volatile usb_host_ohci_hcor_struct_t *usbRegBase; /*!< The base address of the register */
    void *hostHandle;                                 /*!< Related host handle*/
    usb_host_ohci_port_state_struct_t *portState;
    usb_host_ohci_pipe_struct_t *pipeList;
    usb_host_ohci_pipe_struct_t *pipeListInUsing;
    volatile uint32_t writeBackCount;
    volatile usb_host_ohci_general_transfer_descritpor_struct_t *tdDoneListHead;
    volatile usb_host_ohci_general_transfer_descritpor_struct_t *tdDoneListTail;
#if (defined(USB_HOST_CONFIG_OHCI_MAX_GTD) && (USB_HOST_CONFIG_OHCI_MAX_GTD > 0U))
    usb_host_ohci_general_transfer_descritpor_struct_t *gtdList;
#endif
#if (defined(USB_HOST_CONFIG_OHCI_MAX_ITD) && (USB_HOST_CONFIG_OHCI_MAX_ITD > 0U))
    usb_host_ohci_isochronous_transfer_descritpor_struct_t *itdList;
#endif

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    uint64_t matchTick;
#endif
    usb_osa_event_handle ohciEvent; /*!< OHCI event*/
    usb_osa_mutex_handle mutex;     /*!< OHCI layer mutex*/
    usb_host_ohci_pipe_struct_t pipePool[USB_HOST_CONFIG_OHCI_MAX_ED];
    uint8_t controllerId;           /*!< Controller id */
    uint8_t portNumber;             /*!< Port count */
    uint8_t isrNumber;              /*!< ISR Number */
    volatile uint8_t gtdCount;      /*!< Gtd count */
    volatile uint8_t itdCount;      /*!< Itd count */
    volatile uint8_t controlIsBusy; /*!< Is handling control tansfer */
    volatile uint8_t isrLevel;      /*!< The disable deep of the ISR */

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    volatile bus_ohci_suspend_request_state_t busSuspendStatus; /*!< Bus Suspend Status*/
#endif
} usb_host_ohci_state_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * @name USB host OHCI APIs
 * @{
 */

/*!
 * @brief Creates the USB host OHCI instance.
 *
 * This function initializes the USB host OHCI controller driver.
 *
 * @param controllerId      The controller ID of the USB IP. See the enumeration usb_controller_index_t.
 * @param hostHandle The host level handle.
 * @param controllerHandle  Returns the controller instance handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_AllocFail            Allocates memory failed.
 * @retval kStatus_USB_Error                Host mutex create failed, OHCI mutex or OHCI event create failed.
 *                                          Or, OHCI IP initialize failed.
 */
extern usb_status_t USB_HostOhciCreate(uint8_t controllerId,
                                       usb_host_handle hostHandle,
                                       usb_host_controller_handle *controllerHandle);

/*!
 * @brief Destroys the USB host OHCI instance.
 *
 * This function deinitializes the USB host OHCI controller driver.
 *
 * @param controllerHandle  The controller handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
extern usb_status_t USB_HostOhciDestory(usb_host_controller_handle controllerHandle);

/*!
 * @brief Opens the USB host pipe.
 *
 * This function opens a pipe according to the pipe_init_ptr parameter.
 *
 * @param controllerHandle    The controller handle.
 * @param pipeHandle          The pipe handle pointer used to return the pipe handle.
 * @param pipeInit            It is used to initialize the pipe.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_Error                There is no idle pipe.
 *
 */
extern usb_status_t USB_HostOhciOpenPipe(usb_host_controller_handle controllerHandle,
                                         usb_host_pipe_handle *pipeHandle,
                                         usb_host_pipe_init_t *pipeInit);

/*!
 * @brief Closes the USB host pipe.
 *
 * This function closes a pipe and frees the related resources.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The closing pipe handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
extern usb_status_t USB_HostOhciClosePipe(usb_host_controller_handle controllerHandle, usb_host_pipe_handle pipeHandle);

/*!
 * @brief Sends data to the pipe.
 *
 * This function requests to send the transfer to the specified pipe.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The sending pipe handle.
 * @param transfer          The transfer information.
 *
 * @retval kStatus_USB_Success                  Send successful.
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for OHCI.
 */
extern usb_status_t USB_HostOhciWritePipe(usb_host_controller_handle controllerHandle,
                                          usb_host_pipe_handle pipeHandle,
                                          usb_host_transfer_t *transfer);

/*!
 * @brief Receives data from the pipe.
 *
 * This function requests to receive the transfer from the specified pipe.
 *
 * @param controllerHandle The controller handle.
 * @param pipeHandle       The receiving pipe handle.
 * @param transfer          The transfer information.
 *
 * @retval kStatus_USB_Success                  Receive successful.
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for OHCI.
 */
extern usb_status_t USB_HostOhciReadPipe(usb_host_controller_handle controllerHandle,
                                         usb_host_pipe_handle pipeHandle,
                                         usb_host_transfer_t *transfer);

/*!
 * @brief Controls the OHCI.
 *
 * This function controls the OHCI.
 *
 * @param controllerHandle The controller handle.
 * @param ioctlEvent       See the enumeration host_bus_control_t.
 * @param ioctlParam       The control parameter.
 *
 * @retval kStatus_USB_Success                Cancel successful.
 * @retval kStatus_USB_InvalidHandle        The controllerHandle is a NULL pointer.
 */
extern usb_status_t USB_HostOhciIoctl(usb_host_controller_handle controllerHandle,
                                      uint32_t ioctlEvent,
                                      void *ioctlParam);

/*! @}*/
#ifdef __cplusplus
}
#endif
/*! @}*/
#endif /* __USB_HOST_OHCI_H__ */
