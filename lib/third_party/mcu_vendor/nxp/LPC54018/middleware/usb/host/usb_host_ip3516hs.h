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

#ifndef __USB_HOST_IP3516HS_H__
#define __USB_HOST_IP3516HS_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! CAPLENGTH/CHIPID register */
#define USB_HOST_IP3516HS_CAPLENGTH_CHIPID_CAPLENGTH_MASK (0xFFU)
#define USB_HOST_IP3516HS_HCSPARAMS_CHIPID_MASK (0xFFFF0000U)
#define USB_HOST_IP3516HS_HCSPARAMS_CHIPID_SHIFT (16U)

/*! HCSPARAMS register */
#define USB_HOST_IP3516HS_HCSPARAMS_N_PORTS_MASK (0x0FU)
#define USB_HOST_IP3516HS_HCSPARAMS_PPC_MASK (0x10U)
#define USB_HOST_IP3516HS_HCSPARAMS_P_INDICATOR_MASK (0x10000U)

/*! HCCPARAMS register */
#define USB_HOST_IP3516HS_HCCPARAMS_LPMC_MASK (0x20000U)

/*! FLADJ Register */
#define USB_HOST_IP3516HS_FLADJ_FLADJ_MASK (0x3FU)
#define USB_HOST_IP3516HS_FLADJ_FRINDEX_MASK (0x3FFF0000U)
#define USB_HOST_IP3516HS_FLADJ_FRINDEX_SHIFT (16U)

/*! ATL PTD BaseAddress register */
#define USB_HOST_IP3516HS_ATL_PTD_ATL_CUR_MASK (0x1F0U)
#define USB_HOST_IP3516HS_ATL_PTD_ATL_CUR_SHIFT (4U)
#define USB_HOST_IP3516HS_ATL_PTD_ATL_BASE_MASK (0xFFFFFE00U)
#define USB_HOST_IP3516HS_ATL_PTD_ATL_BASE_SHIFT (9U)

/*! ISO PTD BaseAddress register */
#define USB_HOST_IP3516HS_ISO_PTD_ISO_FIRST_MASK (0x3E0U)
#define USB_HOST_IP3516HS_ISO_PTD_ISO_FIRST_SHIFT (5U)
#define USB_HOST_IP3516HS_ISO_PTD_ISO_BASE_MASK (0xFFFFFC00U)
#define USB_HOST_IP3516HS_ISO_PTD_ISO_BASE_SHIFT (10U)

/*! INT PTD BaseAddress register */
#define USB_HOST_IP3516HS_INT_PTD_INT_FIRST_MASK (0x3E0U)
#define USB_HOST_IP3516HS_INT_PTD_INT_FIRST_SHIFT (5U)
#define USB_HOST_IP3516HS_INT_PTD_INT_BASE_MASK (0xFFFFFC00U)
#define USB_HOST_IP3516HS_INT_PTD_INT_BASE_SHIFT (10U)

/*! Data Payload BaseAddress register */
#define USB_HOST_IP3516HS_DAT_BASE_MASK (0xFFFF0000U)
#define USB_HOST_IP3516HS_DAT_BASE_SHIFT (16U)

/*! USBCMD Register */
#define USB_HOST_IP3516HS_USBCMD_RS_MASK (0x01U)
#define USB_HOST_IP3516HS_USBCMD_HCRESET_MASK (0x02U)
#define USB_HOST_IP3516HS_USBCMD_FLS_MASK (0x0CU)
#define USB_HOST_IP3516HS_USBCMD_FLS_SHIFT (2U)
#define USB_HOST_IP3516HS_USBCMD_FLS(x) \
    (((x) << USB_HOST_IP3516HS_USBCMD_FLS_SHIFT) & USB_HOST_IP3516HS_USBCMD_FLS_MASK)
#define USB_HOST_IP3516HS_USBCMD_LHCR_MASK (0x80U)
#define USB_HOST_IP3516HS_USBCMD_ATL_EN_MASK (0x100U)
#define USB_HOST_IP3516HS_USBCMD_ISO_EN_MASK (0x200U)
#define USB_HOST_IP3516HS_USBCMD_INT_EN_MASK (0x400U)
#define USB_HOST_IP3516HS_USBCMD_HIRD_MASK (0xF000000U)
#define USB_HOST_IP3516HS_USBCMD_HIRD_SHIFT (24U)
#define USB_HOST_IP3516HS_USBCMD_LPM_RWU_MASK (0x10000000U)
#define USB_HOST_IP3516HS_USBCMD_LPM_RWU_SHIFT (28U)

/*! USBSTS Register */
#define USB_HOST_IP3516HS_USBSTS_PCD_MASK (0x04U)
#define USB_HOST_IP3516HS_USBSTS_FLR_MASK (0x08U)
#define USB_HOST_IP3516HS_USBSTS_ATL_IRQ_MASK (0x10000U)
#define USB_HOST_IP3516HS_USBSTS_ISO_IRQ_MASK (0x20000U)
#define USB_HOST_IP3516HS_USBSTS_INT_IRQ_MASK (0x40000U)
#define USB_HOST_IP3516HS_USBSTS_SOF_IRQ_MASK (0x80000U)

/*! USBINTR Register */
#define USB_HOST_IP3516HS_USBINTR_PCDE_MASK (0x04U)
#define USB_HOST_IP3516HS_USBINTR_FLRE_MASK (0x08U)
#define USB_HOST_IP3516HS_USBINTR_ATL_IRQ_E_MASK (0x10000U)
#define USB_HOST_IP3516HS_USBINTR_ISO_IRQ_E_MASK (0x20000U)
#define USB_HOST_IP3516HS_USBINTR_INT_IRQ_E_MASK (0x40000U)
#define USB_HOST_IP3516HS_USBINTR_SOF_E_MASK (0x80000U)

/*! PORTSC1 Register */
#define USB_HOST_IP3516HS_PORTSC1_CCS_MASK (0x01U)
#define USB_HOST_IP3516HS_PORTSC1_CSC_MASK (0x02U)
#define USB_HOST_IP3516HS_PORTSC1_PED_MASK (0x04U)
#define USB_HOST_IP3516HS_PORTSC1_PEDC_MASK (0x08U)
#define USB_HOST_IP3516HS_PORTSC1_OCA_MASK (0x10U)
#define USB_HOST_IP3516HS_PORTSC1_OCC_MASK (0x20U)
#define USB_HOST_IP3516HS_PORTSC1_FPR_MASK (0x40U)
#define USB_HOST_IP3516HS_PORTSC1_SUSP_MASK (0x80U)
#define USB_HOST_IP3516HS_PORTSC1_PR_MASK (0x100U)
#define USB_HOST_IP3516HS_PORTSC1_SUS_L1_MASK (0x200U)
#define USB_HOST_IP3516HS_PORTSC1_LS_MASK (0xC00U)
#define USB_HOST_IP3516HS_PORTSC1_LS_SHIFT (10U)
#define USB_HOST_IP3516HS_PORTSC1_PP_MASK (0x1000U)
#define USB_HOST_IP3516HS_PORTSC1_PIC_MASK (0xC000U)
#define USB_HOST_IP3516HS_PORTSC1_PIC_SHIFT (14U)
#define USB_HOST_IP3516HS_PORTSC1_PTC_MASK (0xF0000U)
#define USB_HOST_IP3516HS_PORTSC1_PTC_SHIFT (16U)
#define USB_HOST_IP3516HS_PORTSC1_PSPD_MASK (0x300000U)
#define USB_HOST_IP3516HS_PORTSC1_PSPD_SHIFT (20U)
#define USB_HOST_IP3516HS_PORTSC1_WOO_MASK (0x400000U)
#define USB_HOST_IP3516HS_PORTSC1_SUS_STAT_MASK (0x1800000U)
#define USB_HOST_IP3516HS_PORTSC1_SUS_STAT_SHIFT (23U)
#define USB_HOST_IP3516HS_PORTSC1_DEV_ADD_MASK (0xFE000000U)
#define USB_HOST_IP3516HS_PORTSC1_DEV_ADD_SHIFT (25U)
#define USB_HOST_IP3516HS_PORTSC1_WIC \
    (USB_HOST_IP3516HS_PORTSC1_CSC_MASK | USB_HOST_IP3516HS_PORTSC1_PEDC_MASK | USB_HOST_IP3516HS_PORTSC1_OCC_MASK)

/*! Last PTD in use Register */
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_ATL_LAST_MASK (0x1FU)
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_ATL_LAST_SHIFT (0U)
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_ISO_LAST_MASK (0x1F00U)
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_ISO_LAST_SHIFT (8U)
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_INT_LAST_MASK (0x1F0000U)
#define USB_HOST_IP3516HS_LAST_PTD_IN_USE_INT_LAST_SHIFT (16U)

/*! PORT Mode Register */
#define USB_HOST_IP3516HS_PORT_MODE_ID0_MASK (0x01U)
#define USB_HOST_IP3516HS_PORT_MODE_ID0_EN_MASK (0x100U)
#define USB_HOST_IP3516HS_PORT_MODE_DEV_ENABLE_MASK (0x1000U)
#define USB_HOST_IP3516HS_PORT_MODE_SW_CTRL_PDCOM_MASK (0x4000U)
#define USB_HOST_IP3516HS_PORT_MODE_SW_PDCOM_MASK (0x8000U)
/*!
 * @addtogroup usb_host_controller_ip3516hs
 * @{
 */
/*! @brief IP3516HS Transaction Descriptor */
typedef struct _usb_host_ip3516hs_atl_struct
{
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t V : 1U;              /*!< Valid */
            volatile uint32_t NextPTDPointer : 5U; /*!< NextPTDPointer */
            uint32_t R1 : 1U;                      /*!< Reserved */
            volatile uint32_t J : 1U; /*!< Jump:  0: increment the PTD pointer. 1: enable the next PTD branching. */
            volatile uint32_t
                uFrame : 8U; /*!< This field is only applicable for interrupt and isochronous endpoints. */
            volatile uint32_t MaxPacketLength : 11U; /*!< Maximum Packet Length */
            uint32_t R2 : 1U;                        /*!< Reserved */
            volatile uint32_t Mult : 2U;             /*!< EndpointNumber */
            uint32_t R3 : 2U;                        /*!< Reserved */
        } stateBitField;
    } control1Union;
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t EP : 4U;            /*!< Endpoint number */
            volatile uint32_t DeviceAddress : 7U; /*!< Device address */
            volatile uint32_t S : 1U;  /*!< This bit indicates whether a split transaction has to be executed. */
            volatile uint32_t RL : 4U; /*!< Reload: If RL is set to 0h, hardware ignores the NakCnt value. RL and
                                       NakCnt are set to the same value before a transaction. */
            volatile uint32_t SE : 2U; /*!< This specifies the speed for a Control or Interrupt transaction to a device
                                       that is not high-speed: 00-Full-speed, 10-Low-speed */
            volatile uint32_t PortNumber : 7U; /*!< Port number */
            volatile uint32_t HubAddress : 7U; /*!< Hub Address */
        } stateBitField;
    } control2Union;
    union
    {
        uint32_t data;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes to Transfer. */
            volatile uint32_t I : 1U;                  /*!< Interrupt on Complete */
            volatile uint32_t DataStartAddress : 16U;  /*!< Data buffer address */
        } dataBitField;
    } dataUnion;
    union
    {
        uint32_t state;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes Transferred. */
            volatile uint32_t
                Token : 2U; /*!< Token: Identifies the token Packet Identifier (PID) for this transaction. */
            volatile uint32_t EpType : 2U; /*!< Endpoint type */
            volatile uint32_t NakCnt : 4U; /*!< Nak count */
            volatile uint32_t Cerr : 2U;   /*!< Error count */
            volatile uint32_t DT : 1U;     /*!< Data Toggle */
            volatile uint32_t P : 1U;      /*!< Ping */
            volatile uint32_t SC : 1U;     /*!< Start/Complete */
            volatile uint32_t X : 1U;      /*!< Error */
            volatile uint32_t B : 1U;      /*!< Babble */
            volatile uint32_t H : 1U;      /*!< Halt */
            volatile uint32_t A : 1U;      /*!< Active */
        } stateBitField;
    } stateUnion;
} usb_host_ip3516hs_atl_struct_t;

/*! @brief IP3516HS Transaction Descriptor */
typedef struct _usb_host_ip3516hs_ptl_struct
{
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t V : 1U; /*!< valid */
            volatile uint32_t
                NextPTDPointer : 5U;  /*!< Next PTD Counter: Next PTD branching assigned by the PTDpointer. */
            uint32_t R1 : 1U;         /*!< Reserved */
            volatile uint32_t J : 1U; /*!< Jump:  0: increment the PTD pointer. 1: enable the next PTD branching. */
            volatile uint32_t
                uFrame : 8U; /*!< This field is only applicable for interrupt and isochronous endpoints. */
            volatile uint32_t MaxPacketLength : 11U; /*!< Maximum Packet Length */
            uint32_t R2 : 1U;                        /*!< Reserved */
            volatile uint32_t Mult : 2U;             /*!< Multiplier */
            uint32_t R3 : 2U;                        /*!< Reserved */
        } stateBitField;
    } control1Union;
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t EP : 4U;            /*!< Endpoint number */
            volatile uint32_t DeviceAddress : 7U; /*!< Device address */
            volatile uint32_t S : 1U;  /*!< This bit indicates whether a split transaction has to be executed. */
            volatile uint32_t RL : 4U; /*!< Reload: If RL is set to 0h, hardware ignores the NakCnt value. RL and
                                       NakCnt are set to the same value before a transaction. */
            volatile uint32_t SE : 2U; /*!< This specifies the speed for a Control or Interrupt transaction to a device
                                       that is not high-speed: 00-Full-speed, 10-Low-speed */
            uint32_t R1 : 14U;         /*!< Reserved */
        } stateBitField;
    } control2Union;
    union
    {
        uint32_t data;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes to Transfer. */
            volatile uint32_t I : 1U;                  /*!< Interrupt on Complete. */
            volatile uint32_t DataStartAddress : 16U;  /*!< Data buffer address */
        } dataBitField;
    } dataUnion;
    union
    {
        uint32_t state;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes Transferred. */
            volatile uint32_t
                Token : 2U; /*!< Token: Identifies the token Packet Identifier (PID) for this transaction. */
            volatile uint32_t EpType : 2U; /*!< Endpoint type */
            volatile uint32_t NakCnt : 4U; /*!< Nak count */
            volatile uint32_t Cerr : 2U;   /*!< Error count */
            volatile uint32_t DT : 1U;     /*!< Data Toggle */
            volatile uint32_t P : 1U;      /*!< Ping */
            volatile uint32_t SC : 1U;     /*!< Start/Complete */
            volatile uint32_t X : 1U;      /*!< Error */
            volatile uint32_t B : 1U;      /*!< Babble */
            volatile uint32_t H : 1U;      /*!< Halt */
            volatile uint32_t A : 1U;      /*!< Active */
        } stateBitField;
    } stateUnion;
    union
    {
        uint32_t status;
        struct
        {
            volatile uint32_t
                uSA : 8U; /*!< This field is only used for periodic split transactions or if the port is enabled
                               in HS mode. */
            volatile uint32_t Status0 : 3U; /*!< Isochronous IN or OUT status at uSOF0 */
            volatile uint32_t Status1 : 3U; /*!< Isochronous IN or OUT status at uSOF1 */
            volatile uint32_t Status2 : 3U; /*!< Isochronous IN or OUT status at uSOF2 */
            volatile uint32_t Status3 : 3U; /*!< Isochronous IN or OUT status at uSOF3 */
            volatile uint32_t Status4 : 3U; /*!< Isochronous IN or OUT status at uSOF4 */
            volatile uint32_t Status5 : 3U; /*!< Isochronous IN or OUT status at uSOF5 */
            volatile uint32_t Status6 : 3U; /*!< Isochronous IN or OUT status at uSOF6 */
            volatile uint32_t Status7 : 3U; /*!< Isochronous IN or OUT status at uSOF7 */
        } statusBitField;
    } statusUnion;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t isoIn0 : 12U;   /*!< Data length */
            volatile uint32_t isoIn1 : 12U;   /*!< Data length */
            volatile uint32_t isoIn2low : 8U; /*!< Data length */
        } bitField;
    } isoInUnion1;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t isoIn2High : 4U; /*!< Data length */
            volatile uint32_t isoIn3 : 12U;    /*!< Data length */
            volatile uint32_t isoIn4 : 12U;    /*!< Data length */
            volatile uint32_t isoIn5Low : 4U;  /*!< Data length */
        } bitField;
    } isoInUnion2;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t isoIn5High : 8U; /*!< Data length */
            volatile uint32_t isoIn6 : 12U;    /*!< Data length */
            volatile uint32_t isoIn7 : 12U;    /*!< Data length */
        } bitField;
    } isoInUnion3;
} usb_host_ip3516hs_ptl_struct_t;

/*! @brief IP3516HS Transaction Descriptor */
typedef struct _usb_host_ip3516hs_sptl_struct
{
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t V : 1U;                /*!< Valid */
            volatile uint32_t NextPTDPointer : 5U;   /*!< NextPTDPointer */
            uint32_t R1 : 1U;                        /*!< Reserved */
            volatile uint32_t J : 1U;                /*!< Jump */
            volatile uint32_t uFrame : 8U;           /*!< Frame number at which this PTD will be sent. */
            volatile uint32_t MaxPacketLength : 11U; /*!< Maximum Packet Length. */
            uint32_t R2 : 1U;                        /*!< Reserved */
            volatile uint32_t Mult : 2U;             /*!< Multiplier */
            uint32_t R3 : 2U;                        /*!< Reserved */
        } stateBitField;
    } control1Union;
    union
    {
        uint32_t controlState;
        struct
        {
            volatile uint32_t EP : 4U;            /*!< Endpoint number */
            volatile uint32_t DeviceAddress : 7U; /*!< Device address */
            volatile uint32_t S : 1U;  /*!< This bit indicates whether a split transaction has to be executed. */
            volatile uint32_t RL : 4U; /*!< Reload: If RL is set to 0h, hardware ignores the NakCnt value. RL and
                                       NakCnt are set to the same value before a transaction. */
            volatile uint32_t SE : 2U; /*!< This specifies the speed for a Control or Interrupt transaction to a device
                                       that is not high-speed: 00-Full-speed, 10-Low-speed */
            volatile uint32_t PortNumber : 7U; /*!< Port number */
            volatile uint32_t HubAddress : 7U; /*!< Hub Address */
        } stateBitField;
    } control2Union;
    union
    {
        uint32_t data;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes to Transfer. */
            volatile uint32_t I : 1U;                  /*!< Interrupt on Complete. */
            volatile uint32_t DataStartAddress : 16U;  /*!< Data buffer address */
        } dataBitField;
    } dataUnion;
    union
    {
        uint32_t state;
        struct
        {
            volatile uint32_t NrBytesToTransfer : 15U; /*!< Number of Bytes Transferred. */
            volatile uint32_t
                Token : 2U; /*!< Token: Identifies the token Packet Identifier (PID) for this transaction. */
            volatile uint32_t EpType : 2U; /*!< Endpoint type */
            volatile uint32_t NakCnt : 4U; /*!< Nak count */
            volatile uint32_t Cerr : 2U;   /*!< Error count */
            volatile uint32_t DT : 1U;     /*!< Data Toggle */
            volatile uint32_t P : 1U;      /*!< Ping */
            volatile uint32_t SC : 1U;     /*!< Start/Complete */
            volatile uint32_t X : 1U;      /*!< Error */
            volatile uint32_t B : 1U;      /*!< Babble */
            volatile uint32_t H : 1U;      /*!< Halt */
            volatile uint32_t A : 1U;      /*!< Active */
        } dataBitField;
    } stateUnion;
    union
    {
        uint32_t status;
        struct
        {
            volatile uint32_t
                uSA : 8U; /*!< This field is only used for periodic split transactions or if the port is enabled
                               in HS mode. */
            volatile uint32_t Status0 : 3U; /*!< Isochronous IN or OUT status at uSOF0 */
            volatile uint32_t Status1 : 3U; /*!< Isochronous IN or OUT status at uSOF1 */
            volatile uint32_t Status2 : 3U; /*!< Isochronous IN or OUT status at uSOF2 */
            volatile uint32_t Status3 : 3U; /*!< Isochronous IN or OUT status at uSOF3 */
            volatile uint32_t Status4 : 3U; /*!< Isochronous IN or OUT status at uSOF4 */
            volatile uint32_t Status5 : 3U; /*!< Isochronous IN or OUT status at uSOF5 */
            volatile uint32_t Status6 : 3U; /*!< Isochronous IN or OUT status at uSOF6 */
            volatile uint32_t Status7 : 3U; /*!< Isochronous IN or OUT status at uSOF7 */
        } statusBitField;
    } statusUnion;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t uSCS : 8U; /*!< All bits can be set to one for every transfer. It specifies which uSOF the
                                              complete split needs to be sent. */
            volatile uint32_t isoIn0 : 8U; /*!< Data length */
            volatile uint32_t isoIn1 : 8U; /*!< Data length */
            volatile uint32_t isoIn2 : 8U; /*!< Data length */
        } bitField;
    } isoInUnion1;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t isoIn3 : 8U; /*!< Data length */
            volatile uint32_t isoIn4 : 8U; /*!< Data length */
            volatile uint32_t isoIn5 : 8U; /*!< Data length */
            volatile uint32_t isoIn6 : 8U; /*!< Data length */
        } bitField;
    } isoInUnion2;
    union
    {
        uint32_t isoIn;
        struct
        {
            volatile uint32_t isoIn7 : 8U; /*!< Data length */
            volatile uint32_t R : 24U;     /*!< Reserved */
        } bitField;
    } isoInUnion3;
} usb_host_ip3516hs_sptl_struct_t;

#define USB_HOST_IP3516HS_PTD_TOKEN_OUT 0x00U
#define USB_HOST_IP3516HS_PTD_TOKEN_IN 0x01U
#define USB_HOST_IP3516HS_PTD_TOKEN_SETUP 0x02U

#define USB_HOST_IP3516HS_PTD_EPTYPE_OUT 0x00U
#define USB_HOST_IP3516HS_PTD_EPTYPE_IN 0x01U
#define USB_HOST_IP3516HS_PTD_EPTYPE_SETUP 0x02U

#define USB_HOST_IP3516HS_PTD_MAX_TRANSFER_LENGTH 0x7FFFU

#define USB_HOST_IP3516HS_MAX_UFRAME (1U << 8)
#define USB_HOST_IP3516HS_MAX_FRAME (USB_HOST_IP3516HS_MAX_UFRAME)

#define USB_HOST_IP3516HS_PERIODIC_TRANSFER_GAP (3U)

/*! @brief Check the port connect state delay if the state is unstable */
#define USB_HOST_IP3516HS_PORT_CONNECT_DEBOUNCE_DELAY (101U)

#define USB_HOST_IP3516HS_TIME_DELAY (3500U)
#define USB_HOST_IP3516HS_HUB_LS_SETUP_TIME_DELAY (333U)

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
typedef enum _bus_ip3516hs_suspend_request_state
{
    kBus_Ip3516HsIdle = 0,
    kBus_Ip3516HsStartSuspend,
    kBus_Ip3516HsSuspended,
    kBus_Ip3516HsStartResume,
    kBus_Ip3516HsL1StartSleep,
    kBus_Ip3516HsL1Sleeped,
    kBus_Ip3516HsL1StartResume,
    kBus_Ip3516HsError,
} bus_ip3516hs_suspend_request_state_t;
#endif

typedef enum _usb_host_ip3516hs_transfer_status
{
    kStatus_UsbHostIp3516Hs_Idle = 0,
    kStatus_UsbHostIp3516Hs_Setup,
    kStatus_UsbHostIp3516Hs_Data,
    kStatus_UsbHostIp3516Hs_Data2,
    kStatus_UsbHostIp3516Hs_State,
} usb_host_ip3516hs_transfer_status_t;

typedef struct _usb_host_ip3516hs_td_struct
{
#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_ISO) && (USB_HOST_CONFIG_IP3516HS_MAX_ISO > 0U))
    usb_host_ip3516hs_ptl_struct_t iso[USB_HOST_CONFIG_IP3516HS_MAX_ISO];
#endif
#if (defined(USB_HOST_CONFIG_IP3516HS_MAX_INT) && (USB_HOST_CONFIG_IP3516HS_MAX_INT > 0U))
    usb_host_ip3516hs_ptl_struct_t interrupt[USB_HOST_CONFIG_IP3516HS_MAX_INT];
#endif
    usb_host_ip3516hs_atl_struct_t atl[(((USB_HOST_CONFIG_IP3516HS_MAX_ATL - 1U) >> 1U) + 1U) << 1U];
} usb_host_ip3516hs_ptd_struct_t;

/*! @brief task event */
#define USB_HOST_IP3516HS_EVENT_ATTACH (0x01U)
#define USB_HOST_IP3516HS_EVENT_DETACH (0x02U)
#define USB_HOST_IP3516HS_EVENT_PORT_CHANGE (0x04U)
#define USB_HOST_IP3516HS_EVENT_ISO_TOKEN_DONE (0x08U)
#define USB_HOST_IP3516HS_EVENT_INT_TOKEN_DONE (0x10U)
#define USB_HOST_IP3516HS_EVENT_ATL_TOKEN_DONE (0x20U)
#define USB_HOST_IP3516HS_EVENT_SOF (0x40U)

#define USB_HOST_IP3516HS_PERIODIC_BANDWIDTH_PERCENT ((float)90U / (float)100U)
#define USB_HOST_IP3516HS_PERIODIC_BANDWIDTH (125 * USB_HOST_IP3516HS_PERIODIC_BANDWIDTH_PERCENT)

/*! @brief Transfer scan interval (ms)*/
#define USB_HOST_IP3516HS_TRANSFER_SCAN_INTERVAL (200U)
/*! @brief Time out gap for each transfer (USB_HOST_OHCI_TRANSFER_SCAN_INTERVAL * 1ms) */
#define USB_HOST_IP3516HS_TRANSFER_TIMEOUT_GAP ((5000U * 8) / USB_HOST_IP3516HS_TRANSFER_SCAN_INTERVAL)

#define USB_HOST_IP3516HS_CONTROL_PIPE_MAX_TRANSFER_LENGTH 64U

/*! @brief USB host Ip3516Hs lock */
#define USB_HostIp3516HsLock() USB_OsaMutexLock(usbHostState->mutex)
/*! @brief USB host Ip3516Hs unlock */
#define USB_HostIp3516HsUnlock() USB_OsaMutexUnlock(usbHostState->mutex)

/*! @brief IP3516HS Host Controller Operational Registers */
typedef struct _usb_host_ip3516hs_hcor_struct
{
    __I uint32_t CAPLENGTH_CHIPID;
    __I uint32_t HCSPARAMS;
    __I uint32_t HCCPARAMS;
    __IO uint32_t FLADJ_FRINDEX;
    __IO uint32_t ATL_PTD_BASE_ADDRESS;
    __IO uint32_t ISO_PTD_BASE_ADDRESS;
    __IO uint32_t INT_PTD_BASE_ADDRESS;
    __IO uint32_t DATA_PAYLOAD_BASE_ADDRESS;
    __IO uint32_t USBCMD;
    __IO uint32_t USBSTS;
    __IO uint32_t USBINTR;
    __IO uint32_t PORTSC1;
    __IO uint32_t ATL_PTD_DONE_MAP;
    __IO uint32_t ATL_PTD_SKIP_MAP;
    __IO uint32_t ISO_PTD_DONE_MAP;
    __IO uint32_t ISO_PTD_SKIP_MAP;
    __IO uint32_t INT_PTD_DONE_MAP;
    __IO uint32_t INT_PTD_SKIP_MAP;
    __IO uint32_t LAST_PTD_IN_USE;
    __IO uint32_t UTMI_ULPI_DEBUG;
    __IO uint32_t PORT_MODE;
} usb_host_ip3516hs_register_struct_t;

/*! @brief Port state for device attachment/detachment. */
typedef enum _usb_host_ip3516hs_port_state
{
    kUSB_DeviceIp3516HsPortDetached = 0U, /*!< Port is detached and de-initialized */
    kUSB_DeviceIp3516HsPortPhyDetached,   /*!< Port is physically detached */
    kUSB_DeviceIp3516HsPortPhyAttached,   /*!< Port is physically attached */
    kUSB_DeviceIp3516HsPortAttached,      /*!< Port is attached and initialized */
} usb_host_ip3516hs_port_state_t;

typedef struct _usb_host_ip3516hs_port_state_struct
{
    volatile uint8_t portSpeed;  /*!< Port speed */
    volatile uint8_t portStatus; /*!< Port status */
} usb_host_ip3516hs_port_state_struct_t;

typedef struct _usb_host_ip3516hs_pipe_struct
{
    usb_host_pipe_t pipeCommon; /*!< Common pipe information */
    usb_host_transfer_t *trList;
    volatile usb_host_transfer_t *currentTr;
    uint32_t bufferIndex;
    uint32_t bufferLength;
    uint16_t cutOffTime;
    uint16_t busHsTime;
    uint16_t busNoneHsTime;
    uint16_t startUFrame;
    uint8_t ssSlot;
    uint8_t csSlot;
    uint8_t tdIndex;
    volatile uint8_t isBusy;
} usb_host_ip3516hs_pipe_struct_t;

/*! @brief IP3516HS controller driver instance structure */
typedef struct _usb_host_ip3516hs_state_struct
{
    volatile usb_host_ip3516hs_register_struct_t *usbRegBase; /*!< The base address of the register */
    void *hostHandle;                                         /*!< Related host handle*/
    usb_host_ip3516hs_port_state_struct_t *portState;
    usb_host_ip3516hs_pipe_struct_t *pipeList;
    usb_host_ip3516hs_pipe_struct_t *pipeListInUsing;
    volatile uint32_t bufferArrayBitMap[4]; /*!< Bit map for USB dedicated RAM (Uint is 64bytes) */
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    uint64_t matchTick;
#endif
    usb_osa_event_handle ip3516HsEvent; /*!< IP3516HS event*/
    usb_osa_mutex_handle mutex;         /*!< Ip3516Hs layer mutex*/
    usb_host_ip3516hs_pipe_struct_t pipePool[USB_HOST_CONFIG_IP3516HS_MAX_PIPE];
    uint8_t controllerId;      /*!< Controller id */
    uint8_t portNumber;        /*!< Port count */
    uint8_t isrNumber;         /*!< ISR Number */
    volatile uint8_t isrLevel; /*!< The disable deep of the ISR */
#if ((defined USB_HOST_CONFIG_COMPLIANCE_TEST) && (USB_HOST_CONFIG_COMPLIANCE_TEST))
    uint8_t complianceTestStart;
    uint8_t complianceTest;
#endif
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
#if ((defined(USB_HOST_CONFIG_LPM_L1)) && (USB_HOST_CONFIG_LPM_L1 > 0U))
    uint8_t hirdValue;
    uint8_t L1remoteWakeupEnable;
#endif
    bus_ip3516hs_suspend_request_state_t busSuspendStatus; /*!< Bus Suspend Status*/
#endif
} usb_host_ip3516hs_state_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * @name USB host IP3516HS APIs
 * @{
 */

/*!
 * @brief Creates the USB host IP3516HS instance.
 *
 * This function initializes the USB host IP3516HS controller driver.
 *
 * @param controllerId      The controller ID of the USB IP. See the enumeration usb_controller_index_t.
 * @param hostHandle The host level handle.
 * @param controllerHandle  Returns the controller instance handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 * @retval kStatus_USB_AllocFail            Allocates memory failed.
 * @retval kStatus_USB_Error                Host mutex create failed, IP3516HS mutex or IP3516HS event create failed.
 *                                          Or, IP3516HS IP initialize failed.
 */
extern usb_status_t USB_HostIp3516HsCreate(uint8_t controllerId,
                                           usb_host_handle hostHandle,
                                           usb_host_controller_handle *controllerHandle);

/*!
 * @brief Destroys the USB host IP3516HS instance.
 *
 * This function deinitializes the USB host IP3516HS controller driver.
 *
 * @param controllerHandle  The controller handle.
 *
 * @retval kStatus_USB_Success              The host is initialized successfully.
 */
extern usb_status_t USB_HostIp3516HsDestory(usb_host_controller_handle controllerHandle);

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
extern usb_status_t USB_HostIp3516HsOpenPipe(usb_host_controller_handle controllerHandle,
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
extern usb_status_t USB_HostIp3516HsClosePipe(usb_host_controller_handle controllerHandle,
                                              usb_host_pipe_handle pipeHandle);

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
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for IP3516HS.
 */
extern usb_status_t USB_HostIp3516HsWritePipe(usb_host_controller_handle controllerHandle,
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
 * @retval kStatus_USB_LackSwapBuffer       There is no swap buffer for IP3516HS.
 */
extern usb_status_t USB_HostIp3516HsReadPipe(usb_host_controller_handle controllerHandle,
                                             usb_host_pipe_handle pipeHandle,
                                             usb_host_transfer_t *transfer);

/*!
 * @brief Controls the IP3516HS.
 *
 * This function controls the IP3516HS.
 *
 * @param controllerHandle The controller handle.
 * @param ioctlEvent       See the enumeration host_bus_control_t.
 * @param ioctlParam       The control parameter.
 *
 * @retval kStatus_USB_Success                Cancel successful.
 * @retval kStatus_USB_InvalidHandle        The controllerHandle is a NULL pointer.
 */
extern usb_status_t USB_HostIp3516HsIoctl(usb_host_controller_handle controllerHandle,
                                          uint32_t ioctlEvent,
                                          void *ioctlParam);

/*! @}*/
#ifdef __cplusplus
}
#endif
/*! @}*/
#endif /* __USB_HOST_IP3516HS_H__ */
