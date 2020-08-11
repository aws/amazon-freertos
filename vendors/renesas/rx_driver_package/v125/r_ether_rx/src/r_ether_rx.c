/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
 * applicable laws, including copyright laws. 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 * following link:
 * http://www.renesas.com/disclaimer 
 *
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_ether_rx.c
 * Version      : 1.20
 * Description  : Ethernet module device driver
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.07.2014 1.00     First Release
 *         : 16.12.2014 1.01     Made changes related to header file include.
 *         : 27.03.2015 1.02     Changed the R_ETHER_LinkProcess() Function
 *         : 31.03.2016 1.10     Added changes for MCU which have single channel Ethernet controller.
 *         : 01.10.2016 1.11     Added changes for RX65N.
 *         : 11.11.2016 1.12     The module is updated to fix the software issue.
 *                               When R_ETHER_LinkProcess function is called, 
 *                               there are cases when link up/link down are not processed successfully.
 *                                 The issue occurs when ETHER_CFG_USE_LINKSTA is set to a value of 0.
 *                               Corrected source code of the R_ETHER_Close_ZC2 function.
 *                               Corrected source code of the R_ETHER_LinkProcess function.
 *         : 01.10.2017 1.13     Removed ether_clear_icu_source function in R_ETHER_Close_ZC2 function.
 *         : 07.05.2018 1.14     The module is updated to fix the software issues.
 *                               (1) When R_ETHER_Read_ZC2 function or R_ETHER_Read function is called,
 *                               there is case when the Ethernet frame cannot be received normally.
 *                                 The issue occurs when R_ETHER_Read_ZC2 function or R_ETHER_Read function
 *                                 is called in the interrupt function.
 *                               Corrected source code of the R_ETHER_LinkProcess function.
 *                               (2) When R_ETHER_LinkProcess function is called,
 *                               there is case when link up processing is not completed normally.
 *                                 The issue occurs When R_ETHER_LinkProcess function is called,
 *                                 PHY auto-negotiation is not completed.
 *                               Corrected source code of the R_ETHER_LinkProcess function.
 *                               (3) When R_ETHER_Read_ZC2 function or R_ETHER_Read function is called,
 *                               there is case when execution of function is not completed.
 *                                 The issue occurs when R_ETHER_LinkProcess function is called
 *                                 in the interrupt function.
 *                               Corrected source code of the R_ETHER_Read_ZC2 function.
 *         : 20.05.2019 1.16     Added support for GNUC and ICCRX.
 *                               Fixed coding style.
 *         : 30.07.2019 1.17     Added WAIT LOOP.
 *                               Added changes for RX72M.
 *                               The module is updated to fix the software issue.
 *                               When R_ETHER_Read_ZC2_BufRelease function and R_ETHER_Read function are executed,
 *                               data is received when the reception descriptor (RAM) operated by EDMAC and the 
 *                               reception descriptor (RAM) operated by Ether FIT module are the same. There may occur  
 *                               two phenomena:
 *                               (1) Data loss for one frame of received data may occur.
 *                               (2) Also, even if no data loss occurs, if an error frame is received, it may be 
 *                                   erroneously recognized as a normal frame.
 *         : 22.11.2019 1.20     The module is updated to solve the following problem.
 *                               (1) Added changes for RX72N.
 *                               (2) Added changes for RX66N.
 *                               (3) Changed R_ETHER_Initial, R_ETHER_Open_ZC2, R_ETHER_Close_ZC2, R_ETHER_CheckLink_ZC,
 *                                   R_ETHER_LinkProcess, R_ETHER_WakeOnLAN, R_ETHER_Control for NON-BLOCKING.
 *                               (4) Added ether_set_pmgi_callback, get_pmgi_channel, pmgi_open_zc2_step0,  
 *                                   pmgi_open_zc2_step1, pmgi_open_zc2_step2, pmgi_open_zc2_step3, pmgi_open_zc2_step4, 
 *                                   pmgi_open_zc2_step5, pmgi_open_zc2_step6, pmgi_checklink_zc_step0,
 *                                   pmgi_checklink_zc_step1, pmgi_linkprocess_step0,pmgi_linkprocess_step1, 
 *                                   pmgi_linkprocess_step2, pmgi_wakeonlan_step0, pmgi_wakeonlan_step1,
 *                                   pmgi_wakeonlan_step2, pmgi_writephy_step0, pmgi_readphy_step0, pmgi_modestep_invalid, 
 *                                   R_ETHER_WritePHY, R_ETHER_ReadPHY, ether_pmgi0i_isr, ether_pmgi1i_isr, set_ether_channel
 *                                   for NON-BLOCKING.
 *                               (5) Added PmgiAccessFun_tbl for NON-BLOCKING.
 *                               (6) Added local_advertise, g_local_pause_bits, g_phy_current_param for NON-BLOCKING.
 *                               (7) Changed R_ETHER_Initial, R_ETHER_Control, ether_config_ethernet for insert padding 
 *                                   into received data.
 *                               (8) Added ether_receive_data_padding for insert padding into received data.
 *                               (9) Added padding_insert_position and padding_insert_size for insert padding into 
 *                                   received data.
 *                               (10) Added support for atomic control.  
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
#include <string.h>

/* Access to peripherals and board defines. */
#include "platform.h"

#include "r_ether_rx_if.h"
#include "src/r_ether_rx_private.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 ***********************************************************************************************************************/
#if (ETHER_CFG_NON_BLOCKING == 1)
static uint16_t local_advertise[ETHER_CHANNEL_MAX]; /* the capabilities of the local link as PHY data */
static uint16_t g_local_pause_bits[ETHER_CHANNEL_MAX];
static pmgi_param_t g_phy_current_param[PMGI_CHANNEL_MAX];
#endif

static uint8_t padding_insert_position [ETHER_CHANNEL_MAX];
static uint8_t padding_insert_size [ETHER_CHANNEL_MAX];

/*
 * Private global function prototypes
 */
static void ether_reset_mac (uint32_t channel);
static void ether_init_descriptors (uint32_t channel);
static void ether_config_ethernet (uint32_t channel, const uint8_t mode);
static void ether_pause_resolution (uint16_t local_ability, uint16_t partner_ability, uint16_t *ptx_pause,
        uint16_t *prx_pause);
static void ether_configure_mac (uint32_t channel, const uint8_t mac_addr[], const uint8_t mode);
#if (ETHER_CFG_NON_BLOCKING == 0)
static ether_return_t ether_do_link (uint32_t channel, const uint8_t mode);
#endif
static ether_return_t ether_set_callback (ether_param_t const control);
static ether_return_t ether_set_promiscuous_mode (ether_param_t const control);
static ether_return_t ether_set_int_handler (ether_param_t const control);
static ether_return_t ether_power_on (ether_param_t const control);
static ether_return_t ether_power_off (ether_param_t const control);
static ether_return_t power_on (uint32_t channel);
static void power_off (uint32_t channel);
static ether_return_t ether_set_multicastframe_filter (ether_param_t const control);
static ether_return_t ether_set_broadcastframe_filter (ether_param_t const control);
static ether_return_t ether_receive_data_padding (ether_param_t const control);

#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX72M) || \
     defined(BSP_MCU_RX72N) || defined(BSP_MCU_RX66N))
static void ether_eint0 (void * pparam);
#endif
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N))
static void ether_eint1(void * pparam);
#endif

static void ether_int_common (uint32_t channel);
static ether_return_t power_on_control (uint32_t channel);
static void power_off_control (uint32_t channel);
static uint8_t check_mpde_bit (void);

#if (ETHER_CFG_NON_BLOCKING == 1)
static ether_return_t ether_set_pmgi_callback(ether_param_t const control);
static uint16_t get_pmgi_channel(uint32_t channel);
static void set_ether_channel(uint32_t channel);
static ether_return_t pmgi_open_zc2_step0(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step1(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step2(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step3(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step4(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step5(uint32_t ether_channel);
static ether_return_t pmgi_open_zc2_step6(uint32_t ether_channel);
static ether_return_t pmgi_checklink_zc_step0(uint32_t ether_channel);
static ether_return_t pmgi_checklink_zc_step1(uint32_t ether_channel);
static ether_return_t pmgi_linkprocess_step0(uint32_t ether_channel);
static ether_return_t pmgi_linkprocess_step1(uint32_t ether_channel);
static ether_return_t pmgi_linkprocess_step2(uint32_t ether_channel);
static ether_return_t pmgi_wakeonlan_step0(uint32_t ether_channel);
static ether_return_t pmgi_wakeonlan_step1(uint32_t ether_channel);
static ether_return_t pmgi_wakeonlan_step2(uint32_t ether_channel);
static ether_return_t pmgi_writephy_step0(uint32_t ether_channel);
static ether_return_t pmgi_readphy_step0(uint32_t ether_channel);
static ether_return_t pmgi_modestep_invalid(uint32_t ether_channel);
static const st_pmgi_interrupt_func_t PmgiAccessFun_tbl[PMGI_MODE_NUM][PMGI_STEP_NUM] =
{
    {
        { pmgi_open_zc2_step0 },
        { pmgi_open_zc2_step1 },
        { pmgi_open_zc2_step2 },
        { pmgi_open_zc2_step3 },
        { pmgi_open_zc2_step4 },
        { pmgi_open_zc2_step5 },
        { pmgi_open_zc2_step6 },
    },
    {
        { pmgi_checklink_zc_step0 },
        { pmgi_checklink_zc_step1 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_linkprocess_step0 },
        { pmgi_linkprocess_step1 },
        { pmgi_linkprocess_step2 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_wakeonlan_step0 },
        { pmgi_wakeonlan_step1 },
        { pmgi_wakeonlan_step2 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_open_zc2_step0 },
        { pmgi_open_zc2_step1 },
        { pmgi_open_zc2_step2 },
        { pmgi_open_zc2_step3 },
        { pmgi_open_zc2_step4 },
        { pmgi_open_zc2_step5 },
        { pmgi_open_zc2_step6 },
    },
    {
        { pmgi_checklink_zc_step0 },
        { pmgi_checklink_zc_step1 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_checklink_zc_step0 },
        { pmgi_checklink_zc_step1 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_checklink_zc_step0 },
        { pmgi_checklink_zc_step1 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_checklink_zc_step0 },
        { pmgi_checklink_zc_step1 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_writephy_step0 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
    {
        { pmgi_readphy_step0 },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
        { pmgi_modestep_invalid },
    },
};
#endif /* (ETHER_CFG_NON_BLOCKING == 1) */
/*
 * Private global variables
 */

/* Pointer to the receive descriptors */
#if (ETHER_CHANNEL_MAX == 1)
static descriptor_t * papp_rx_desc[ETHER_CHANNEL_MAX] =
{ NULL };
#elif (ETHER_CHANNEL_MAX == 2)
static descriptor_t * papp_rx_desc[ETHER_CHANNEL_MAX] =
{   NULL, NULL};
#endif

/* Pointer to the transmit descriptors */
#if (ETHER_CHANNEL_MAX == 1)
static descriptor_t * papp_tx_desc[ETHER_CHANNEL_MAX] =
{ NULL };
#elif (ETHER_CHANNEL_MAX == 2)
static descriptor_t * papp_tx_desc[ETHER_CHANNEL_MAX] =
{   NULL, NULL};
#endif

/* Pointer to the callback function */
static ether_cb_t cb_func;

static bool is_entry = false;

/* 
 * The flag which control the pause frame.
 *
 * The value of flag and the situation which is indicatived of by the value.
 * ETHER_FLAG_OFF (0): Don't use the pause frame (default). 
 * ETHER_FLAG_ON  (1): Use the pause frame. 
 */
#if (ETHER_CHANNEL_MAX == 1)
static uint8_t pause_frame_enable[ETHER_CHANNEL_MAX] =
{ ETHER_FLAG_OFF };
#elif (ETHER_CHANNEL_MAX == 2)
static uint8_t pause_frame_enable[ETHER_CHANNEL_MAX] =
{   ETHER_FLAG_OFF, ETHER_FLAG_OFF};
#endif

/*
 * The flag indicatives of the state that the interrupt of Link Up/Down occur.
 * 
 * Value and state of flag
 * ETHER_FLAG_OFF (0): It is not possible to communicate. 
 * ETHER_FLAG_ON  (1): It is possible to communicate. 
 */
static uint8_t transfer_enable_flag[ETHER_CHANNEL_MAX];

/*
 * The flag indicatives of the state that the interrupt of magic packet detection occur.
 *
 * Value and state of flag
 * ETHER_FLAG_OFF (0): The interrupt of the magic packet detection has not been generated. 
 * ETHER_FLAG_ON  (1): The interrupt of the magic packet detection was generated. 
 *
 * If the R_ETHER_LinkProcess function is called, and the interrupt processing of the magic packet detection is done,
 * this flag becomes ETHER_FLAG_OFF(0). 
 */
static uint8_t mpd_flag[ETHER_CHANNEL_MAX];

static uint8_t mac_addr_buf[ETHER_CHANNEL_MAX][6];

/*
 * The flag indicatives of the state that the interrupt of Link Up/Down occur.
 * 
 * Value and state of flag
 * ETHER_FLAG_OFF         (0) : The Link up/down interrupt has not been generated. 
 * ETHER_FLAG_ON_LINK_OFF (2) : The Link down interrupt was generated. 
 * ETHER_FLAG_ON_LINK_ON  (3) : The Link up interrupt was generated. 
 * 
 * If the R_ETHER_LinkProcess function is called, and the interrupt processing of Link Up/Down is done,
 * this flag becomes ETHER_FLAG_OFF(0). 
 */
static uint8_t lchng_flag[ETHER_CHANNEL_MAX];

/*
 * The flag indicatives of the state that enable/disable multicast frame filtering.
 *
 * Value and state of flag
 * ETHER_MC_FILTER_OFF          (0) : Disable multicast frame filtering.
 * ETHER_MC_FILTER_ON           (1) : Enable multicast frame filtering.
 *
 * The frame multicast filtering is software filter. If you want to use Hardware filter,
 * please use it EPTPC in RX64M/RX71M/RX72M/RX72N.
 */
static uint8_t mc_filter_flag[ETHER_CHANNEL_MAX];

/*
 * The value indicatives of receive count for continuous broadcast frame.
 */
static uint32_t bc_filter_count[ETHER_CHANNEL_MAX];

/*
 * PAUSE Resolution as documented in IEEE 802.3-2008_section2 Annex
 * 28B, Table 28B-3. The following table codify logic that
 * determines how the PAUSE is configured for local transmitter
 * and receiver and partner transmitter and receiver.
 */
static const pauseresolution_t pause_resolution[PAUSE_TABLE_ENTRIES] =
{
{ PAUSE_MASKC, PAUSE_VAL0, XMIT_PAUSE_OFF, RECV_PAUSE_OFF },
{ PAUSE_MASKE, PAUSE_VAL4, XMIT_PAUSE_OFF, RECV_PAUSE_OFF },
{ PAUSE_MASKF, PAUSE_VAL6, XMIT_PAUSE_OFF, RECV_PAUSE_OFF },
{ PAUSE_MASKF, PAUSE_VAL7, XMIT_PAUSE_ON, RECV_PAUSE_OFF },
{ PAUSE_MASKE, PAUSE_VAL8, XMIT_PAUSE_OFF, RECV_PAUSE_OFF },
{ PAUSE_MASKA, PAUSE_VALA, XMIT_PAUSE_ON, RECV_PAUSE_ON },
{ PAUSE_MASKF, PAUSE_VALC, XMIT_PAUSE_OFF, RECV_PAUSE_OFF },
{ PAUSE_MASKF, PAUSE_VALD, XMIT_PAUSE_OFF, RECV_PAUSE_ON } };

/*
 * Receive, transmit descriptors and their buffer.  They are
 * defined with section pragma directives to easily locate them
 * on the memory map.
 */
R_BSP_ATTRIB_SECTION_CHANGE(B, _RX_DESC, 1)
static R_BSP_VOLATILE_EVENACCESS descriptor_t rx_descriptors[ETHER_CHANNEL_MAX][ETHER_CFG_EMAC_RX_DESCRIPTORS];
R_BSP_ATTRIB_SECTION_CHANGE(B, _TX_DESC, 1)
static R_BSP_VOLATILE_EVENACCESS descriptor_t tx_descriptors[ETHER_CHANNEL_MAX][ETHER_CFG_EMAC_TX_DESCRIPTORS];

/* 
 * As for Ethernet buffer, the size of total buffer which are use for transmission and the reception is secured.
 * The total buffer's size which the value is integrated from  EMAC_NUM_BUFFERS (buffer number) and 
 * ETHER_CFG_BUFSIZE (the size of one buffer).
 * The ETHER_CFG_BUFSIZE and EMAC_NUM_BUFFERS are defined by macro in the file "r_ether_private.h".
 * It is sequentially used from the head of the buffer as a receive buffer or a transmission buffer.
 */
R_BSP_ATTRIB_SECTION_CHANGE(B, _ETHERNET_BUFFERS, 1)
static etherbuffer_t ether_buffers[ETHER_CHANNEL_MAX];

R_BSP_ATTRIB_SECTION_CHANGE_END

static uint8_t promiscuous_mode[ETHER_CHANNEL_MAX];

#if (ETHER_CHANNEL_MAX == 1)
static const ether_control_t ether_ch_0[] =
{
/* Ether = ch0, Phy access = ch0 */
    {   &ETHERC0, &EDMAC0, (volatile uint32_t R_BSP_EVENACCESS_SFR *)&ETHERC0.PIR.LONG, ETHER_CFG_CH0_PHY_ADDRESS, PORT_CONNECT_ET0 } };
#elif (ETHER_CHANNEL_MAX == 2)
static const ether_control_t ether_ch_0[]=
{
    /* Ether = ch0, Phy access = ch0 */
    {   &ETHERC0, &EDMAC0, (volatile uint32_t R_BSP_EVENACCESS_SFR *)&ETHERC0.PIR.LONG, ETHER_CFG_CH0_PHY_ADDRESS, PORT_CONNECT_ET0},

    /* Ether = ch0, Phy access = ch1 */
    {   &ETHERC0, &EDMAC0, (volatile uint32_t R_BSP_EVENACCESS_SFR *)&ETHERC1.PIR.LONG, ETHER_CFG_CH0_PHY_ADDRESS, PORT_CONNECT_ET0_ET1}
};

static const ether_control_t ether_ch_1[]=
{
    /* Ether = ch1, Phy access = ch0 */
    {   &ETHERC1, &EDMAC1, (volatile uint32_t R_BSP_EVENACCESS_SFR *)&ETHERC0.PIR.LONG, ETHER_CFG_CH1_PHY_ADDRESS, PORT_CONNECT_ET0_ET1},

    /* Ether = ch1, Phy access = ch1 */
    {   &ETHERC1, &EDMAC1, (volatile uint32_t R_BSP_EVENACCESS_SFR *)&ETHERC1.PIR.LONG, ETHER_CFG_CH1_PHY_ADDRESS, PORT_CONNECT_ET1}
};
#endif

/* This table is used to convert it from the channel number for the communication
 into the channel number for the PHY register access. */

#if (ETHER_CHANNEL_MAX == 1)
static const uint32_t ether_phy_access[] =
{
ETHER_CFG_CH0_PHY_ACCESS };
#elif (ETHER_CHANNEL_MAX == 2)
static const uint32_t ether_phy_access[] =
{
    ETHER_CFG_CH0_PHY_ACCESS,
    ETHER_CFG_CH1_PHY_ACCESS
};
#endif

static uint8_t etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_MAX][ETHER_CHANNEL_MAX];

#if (ETHER_CHANNEL_MAX == 1)
const ether_ch_control_t g_eth_control_ch[] =
{
{ ether_ch_0, ETHER_CFG_CH0_PHY_ACCESS } };
#elif (ETHER_CHANNEL_MAX == 2)
const ether_ch_control_t g_eth_control_ch[] =
{
    {   ether_ch_0, ETHER_CFG_CH0_PHY_ACCESS},
    {   ether_ch_1, ETHER_CFG_CH1_PHY_ACCESS}
};
#endif

#if (ETHER_CFG_USE_LINKSTA == 0)
    #if (ETHER_CHANNEL_MAX == 1)
    /* Previous link status */
    static int16_t g_pre_link_stat[ETHER_CHANNEL_MAX] = {ETHER_ERR_OTHER};
    #elif (ETHER_CHANNEL_MAX == 2)
    /* Previous link status */
    static int16_t g_pre_link_stat[ETHER_CHANNEL_MAX] = {ETHER_ERR_OTHER, ETHER_ERR_OTHER};
    #endif
#endif

/*
 * Renesas Ethernet API functions
 */
/**********************************************************************************************************************
 * Function Name: R_ETHER_Initial
 *****************************************************************************************************************/ /**
 * @brief     This function makes initial settings to the Ethernet FIT module.
 * @retval    None.
 * @details   Initializes the memory to be used in order to start Ethernet communication.
 * @note      This function must be called before calling the R_ETHER_Open_ZC2() function.
 */
void R_ETHER_Initial (void)
{
    /* Initialize the transmit and receive descriptor */
    memset((void *)&rx_descriptors, 0x00, sizeof(rx_descriptors));
    memset((void *)&tx_descriptors, 0x00, sizeof(tx_descriptors));

    /* Initialize the Ether buffer */
    memset(&ether_buffers, 0x00, sizeof(ether_buffers));

    memset(etherc_edmac_power_cont, 0x00, sizeof(etherc_edmac_power_cont));

    /* Initialize the callback function pointer */
    cb_func.pcb_func = NULL;

    /* Initialize the interrupt handler pointer */
    cb_func.pcb_int_hnd = NULL;

    /* Initialize the PMGI callback function pointer */
    cb_func.pcb_pmgi_hnd = NULL;

    /* Initialize */
#if (ETHER_CHANNEL_MAX == 1)
    promiscuous_mode[ETHER_CHANNEL_0] = ETHER_PROMISCUOUS_OFF;
    mc_filter_flag[ETHER_CHANNEL_0] = ETHER_MC_FILTER_OFF;
    bc_filter_count[ETHER_CHANNEL_0] = 0;
    padding_insert_position [ETHER_CHANNEL_0] = 0;
    padding_insert_size [ETHER_CHANNEL_0] = 0;
#elif (ETHER_CHANNEL_MAX == 2)
    promiscuous_mode[ETHER_CHANNEL_0] = ETHER_PROMISCUOUS_OFF;
    promiscuous_mode[ETHER_CHANNEL_1] = ETHER_PROMISCUOUS_OFF;
    mc_filter_flag[ETHER_CHANNEL_0] = ETHER_MC_FILTER_OFF;
    mc_filter_flag[ETHER_CHANNEL_1] = ETHER_MC_FILTER_OFF;
    bc_filter_count[ETHER_CHANNEL_0] = 0;
    bc_filter_count[ETHER_CHANNEL_1] = 0;
    padding_insert_position [ETHER_CHANNEL_0] = 0;
    padding_insert_position [ETHER_CHANNEL_1] = 0;
    padding_insert_size [ETHER_CHANNEL_0] = 0;
    padding_insert_size [ETHER_CHANNEL_1] = 0;
#endif
#if (ETHER_CFG_NON_BLOCKING == 1)
#if (PMGI_CHANNEL_MAX == 1)
    g_phy_current_param[PMGI_CHANNEL_0].locked.lock = false;
    g_phy_current_param[PMGI_CHANNEL_0].event = PMGI_IDLE;
    g_phy_current_param[PMGI_CHANNEL_0].mode = OPEN_ZC2;
    g_phy_current_param[PMGI_CHANNEL_0].step = STEP0;
    g_phy_current_param[PMGI_CHANNEL_0].read_data = 0;
    g_phy_current_param[PMGI_CHANNEL_0].reset_counter = 0;
#elif (ETHER_CHANNEL_MAX == 2)
    g_phy_current_param[PMGI_CHANNEL_0].locked.lock = false;
    g_phy_current_param[PMGI_CHANNEL_0].event = PMGI_IDLE;
    g_phy_current_param[PMGI_CHANNEL_0].mode = OPEN_ZC2;
    g_phy_current_param[PMGI_CHANNEL_0].step = STEP0;
    g_phy_current_param[PMGI_CHANNEL_0].read_data = 0;
    g_phy_current_param[PMGI_CHANNEL_0].reset_counter = 0;
    g_phy_current_param[PMGI_CHANNEL_1].locked.lock = false;
    g_phy_current_param[PMGI_CHANNEL_1].event = PMGI_IDLE;
    g_phy_current_param[PMGI_CHANNEL_1].mode = OPEN_ZC2;
    g_phy_current_param[PMGI_CHANNEL_1].step = STEP0;
    g_phy_current_param[PMGI_CHANNEL_1].read_data = 0;
    g_phy_current_param[PMGI_CHANNEL_1].reset_counter = 0;
#endif
#endif

} /* End of function R_ETHER_Initial() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Open_ZC2
 *****************************************************************************************************************/ /**
 * @brief     When using the ETHER API, this function is used first.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[in] mac_addr
 *             Specifies the MAC address of ETHERC.
 * @param[in] pause
 *             Specifies the value set in bit 10 (Pause) in register 4 (auto-negotiation advertisement)
 *             of the PHY-LSI. The setting ETHER_FLAG_ON is possible only when the user's PHY-LSI supports
 *             the pause function. This value is passed to the other PHY-LSI during auto-negotiation.
 *             Flow control is enabled if the auto-negotiation result indicates that both the local PHY-LSI
 *             and the other PHY-LSI support the pause function.\n
 *             Specify ETHER_FLAG_ON to convey that the pause function is supported to the other PHY-LSI
 *             during auto-negotiation, and specify ETHER_FLAG_OFF if the pause function is not supported
 *             or will not be used even though it is supported.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully or the PMGI operation start normally when the non-blocking mode
 *              is enable.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_INVALID_DATA
 *            - Value of the argument is out of range.
 * @retval    ETHER_ERR_OTHER
 *            - PHY-LSI initialization failed when the non-blocking mode is enabled and PMGI callback function is
 *              not registered .
 * @retval    ETHER_ERR_LOCKED
 *            - When PHY access is in progress when non-blocking mode is enabled .
 * @details   The R_ETHER_Open_ZC2() function resets the ETHERC, EDMAC and PHY-LSI by software, and starts PHY-LSI
 *            auto-negotiation to enable the link signal change interrupt.\n
 *            The MAC address is used to initialize the ETHERC MAC address register.\n
 *            When non-blocking mode is enabled, the processing result of the function is passed as an argument of
 *            the PMGI callback function.
 * @note      Either after the R_ETHER_initial() function is called immediately following a power-on reset,
 *            or after the R_ETHER_Close_ZC2() function was called, applications should only use the other
 *            API functions after first calling this function and verifying that the return value is ETHER_SUCCESS.
 */
ether_return_t R_ETHER_Open_ZC2 (uint32_t channel, const uint8_t mac_addr[], uint8_t pause)
{
    volatile bsp_int_err_t bsp_int_err;
#if (ETHER_CFG_NON_BLOCKING == 0)
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    ether_return_t ret;
    int16_t phy_ret;
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
#else
    uint16_t pmgi_channel;
    pmgi_cb_arg_t pmgi_cb_arg;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((NULL == mac_addr) || (FIT_NO_PTR == mac_addr))
    {
        return ETHER_ERR_INVALID_PTR;
    }
    if ((ETHER_FLAG_OFF != pause) && (ETHER_FLAG_ON != pause))
    {
        return ETHER_ERR_INVALID_DATA;
    }

#if (ETHER_CFG_NON_BLOCKING == 1)
    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return ETHER_ERR_OTHER;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE *)&g_phy_current_param[pmgi_channel].locked))
    {
        return ETHER_ERR_LOCKED;
    }
    set_ether_channel(channel);
#endif
#if (ETHER_CFG_NON_BLOCKING == 0)
    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;
#endif

    /* Initialize the flags */
    transfer_enable_flag[channel] = ETHER_FLAG_OFF;
    mpd_flag[channel] = ETHER_FLAG_OFF;
    lchng_flag[channel] = ETHER_FLAG_OFF;

    pause_frame_enable[channel] = pause;

    mac_addr_buf[channel][0] = mac_addr[0];
    mac_addr_buf[channel][1] = mac_addr[1];
    mac_addr_buf[channel][2] = mac_addr[2];
    mac_addr_buf[channel][3] = mac_addr[3];
    mac_addr_buf[channel][4] = mac_addr[4];
    mac_addr_buf[channel][5] = mac_addr[5];

#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N))
    /* Subscribe to r_bsp an interrupt function */
    if (false == is_entry)
    {
        bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC0_EINT0, ether_eint0); /* EINT0 */
        bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC1_EINT1, ether_eint1); /* EINT1 */
        is_entry = true;
    }
#elif (defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX66N))
    /* Subscribe to r_bsp an interrupt function */
    if (false == is_entry)
    {
        bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC0_EINT0, ether_eint0); /* EINT0 */
        is_entry = true;
    }
#endif

    /* Software reset */
    ether_reset_mac(channel);

#if (ETHER_CFG_NON_BLOCKING == 0)
    /* Software reset the PHY */
    phy_ret = phy_init(channel);
    if (R_PHY_OK == phy_ret)
    {
        phy_start_autonegotiate(channel, pause_frame_enable[channel]);

        /* Clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
        petherc_adr->ECSR.LONG = 0x00000037;

        /* Clear all EDMAC status bits */
        pedmac_adr->EESR.LONG = 0x47FF0F9F;

#if (ETHER_CFG_USE_LINKSTA == 1)
        /* Enable interrupts of interest only. */
        petherc_adr->ECSIPR.BIT.LCHNGIP = 1;
#endif

        pedmac_adr->EESIPR.BIT.ECIIP = 1;

        /* Set Ethernet interrupt level and enable */
        ether_enable_icu(channel);

        ret = ETHER_SUCCESS;
    }
    else
    {
        ret = ETHER_ERR_OTHER;
    }

    return ret;
#elif (ETHER_CFG_NON_BLOCKING == 1)

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_ERROR;
            pmgi_cb_arg.mode = OPEN_ZC2;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
        return ETHER_ERR_OTHER;
    }

    g_phy_current_param[pmgi_channel].mode = OPEN_ZC2;
    g_phy_current_param[pmgi_channel].step = STEP0;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_CONTROL, PHY_CONTROL_RESET, PMGI_WRITE);

    return ETHER_SUCCESS;
#endif

} /* End of function R_ETHER_Open_ZC2() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Close_ZC2
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Close_ZC2() function disables transmit and receive functionality on the ETHERC.
 *            This function does not put the ETHERC and EDMAC into the module stop state..
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @details   The R_ETHER_Close_ZC2() function disables transmit and receive functionality on the ETHERC and
 *            disables Ethernet interrupts. It does not put the ETHERC and EDMAC into the module stop state.\n
 *            Execute this function to end the Ethernet communication.
 * @note      None.
 */
ether_return_t R_ETHER_Close_ZC2 (uint32_t channel)
{
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;

    /* Disable Ethernet interrupt. */
    ether_disable_icu(channel);

    petherc_adr->ECSIPR.BIT.LCHNGIP = 0;
    pedmac_adr->EESIPR.BIT.ECIIP = 0;

    /* Disable TE and RE  */
    petherc_adr->ECMR.LONG = 0x00000000;

    /* Initialize the flags */
    transfer_enable_flag[channel] = ETHER_FLAG_OFF;
    mpd_flag[channel] = ETHER_FLAG_OFF;
    lchng_flag[channel] = ETHER_FLAG_OFF;
#if (ETHER_CFG_USE_LINKSTA == 0)
    g_pre_link_stat[channel] = ETHER_ERR_OTHER;
#endif

    return ETHER_SUCCESS;
} /* End of function R_ETHER_Close_ZC2() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Read_ZC2
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Read_ZC2() function returns a pointer to the starting address of the buffer storing the
 *            receive data.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[out] **pbuf
 *             Returns a pointer to the starting address of the buffer storing the receive data.
 * @retval    A value of 1 or greater
 *            - Returns the number of bytes received.
 * @retval    ETHER_NO_DATA
 *            - A zero value indicates no data is received.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @retval    ETHER_ERR_MC_FRAME
 *            - Multicast frame detected when multicast frame filtering is enabled.
 * @details   The driver's buffer pointer to the starting address of the buffer storing the receive data
 *            is returned in the parameter pbuf. Returning the pointer allows the operation to be performed with
 *            zero-copy. Return value shows the number of received bytes. If there is no data available at the time
 *            of the call, ETHER_NO_DATA is returned. When auto-negotiation is not completed, and reception is not
 *            enabled, ETHER_ERR_LINK is returned. ETHER_ERR_MPDE is returned when a Magic Packet is being detected.\n
 *            The EDMAC hardware operates independent of the R_ETHER_Read_ZC2() function and reads data into a buffer
 *            pointed by the EDMAC receive descriptor. The buffer pointed by the EDMAC receive descriptor is
 *            statically allocated by the driver.\n
 *            When multicast frame filtering on the specified channel is enabled by the R_ETHER_Control function,
 *            the buffer is released immediately when a multicast frame is detected. Also, the value
 *            ETHER_ERR_MC_FRAME is returned. Note that when hardware-based multicast frame filtering is enabled
 *            on the RX64M, RX71M, RX72M, RX72N or RX66N, multicast frames are discarded by the hardware and
 *            detection is not possible.\n
 *            Frames that generate a receive FIFO overflow, residual-bit frame receive error, long frame receive
 *            error, short frame receive error, PHY-LSI receive error, or receive frame CRC error are treated
 *            as receive frame errors. When a receive frame error occurs, the descriptor data is discarded,
 *            the status is cleared, and reading of data continues.
 * @note      This function is used in combination with the R_ETHER_Read_ZC2_BufRelease function.
 *            Always call the R_ETHER_Read_ZC2 function and then the R_ETHER_Read_ZC2_BufRelease function
 *            in sequence. If the value ETHER_ERR_LINK is returned when this function is called,
 *            initialize the Ethernet FIT module.
 */
int32_t R_ETHER_Read_ZC2 (uint32_t channel, void **pbuf)
{
    int32_t num_recvd;
    int32_t ret;
    int32_t complete_flag;
    int32_t ret2;
#if (1 == ETHER_CFG_EMAC_RX_DESCRIPTORS)
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
#endif
    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((NULL == pbuf) || (FIT_NO_PTR == pbuf))
    {
        return ETHER_ERR_INVALID_PTR;
    }

    /* When the Link up processing is not completed, return error */
    if (ETHER_FLAG_OFF == transfer_enable_flag[channel])
    {
        ret = ETHER_ERR_LINK;
    }

    /* In case of detection mode of magic packet, return error. */
    else if (1 == check_mpde_bit())
    {
        ret = ETHER_ERR_MPDE;
    }

    /* When the Link up processing is completed */
    else
    {
        ret = ETHER_NO_DATA;
        complete_flag = ETHER_ERR_OTHER;
#if (1 == ETHER_CFG_EMAC_RX_DESCRIPTORS)
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;
#endif
        /* WAIT_LOOP */
        while (ETHER_SUCCESS != complete_flag)
        {
#if (1 == ETHER_CFG_EMAC_RX_DESCRIPTORS)
            /* When receive function enable  */
            if (0x00000000L == pedmac_adr->EDRRR.LONG)
#else
            /* When receive data exists. */
            if (RACT != (papp_rx_desc[channel]->status & RACT))
#endif
            {
                /* Check multicast is detected when multicast frame filter is enabled */
                if (ETHER_MC_FILTER_ON == mc_filter_flag[channel])
                {
                    if (RFS7_RMAF == (papp_rx_desc[channel]->status & RFS7_RMAF))
                    {
                        /* The buffer is released at the multicast frame detect.  */
                        ret2 = R_ETHER_Read_ZC2_BufRelease(channel);
                        if (ETHER_SUCCESS != ret2)
                        {
                            return ret2;
                        }

                        ret = ETHER_ERR_MC_FRAME;
                        complete_flag = ETHER_SUCCESS;
                    }
                }

                if (ETHER_ERR_MC_FRAME != ret)
                {
                    if (RFE == (papp_rx_desc[channel]->status & RFE))
                    {
                        /* The buffer is released at the error.  */
                        ret2 = R_ETHER_Read_ZC2_BufRelease(channel);
                        if (ETHER_SUCCESS != ret2)
                        {
                            return ret2;
                        }
                    }
                    else
                    {
                        /**
                         * Pass the pointer to received data to application.  This is
                         * zero-copy operation.
                         */
                        (*pbuf) = (void *) papp_rx_desc[channel]->buf_p;

                        /* Get bytes received */
                        num_recvd = papp_rx_desc[channel]->size;
                        ret = num_recvd;
                        complete_flag = ETHER_SUCCESS;
                    }
                }
            }
            else
            {
                ret = ETHER_NO_DATA;
                complete_flag = ETHER_SUCCESS;
            }
        }
    }
    return ret;
} /* End of function R_ETHER_Read_ZC2() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Read_ZC2_BufRelease
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Read_ZC2_BufRelease() function releases the buffer read by the R_ETHER_Read_ZC2() function.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @details   The R_ETHER_Read_ZC2_BufRelease() function releases the buffer read by the R_ETHER_Read_ZC2() function.
 * @note      Before calling this function, use the R_ETHER_Read_ZC2 function to read data. Call this function after
 *            a value of 1 or greater is returned.\n
 *            This function is used in combination with the R_ETHER_Read_ZC2_BufRelease function.
 *            Always call the R_ETHER_Read_ZC2 function and then the R_ETHER_Read_ZC2_BufRelease function in sequence.
 *            If the value ETHER_ERR_LINK is returned when this function is called, initialize the Ethernet FIT module.
 */
int32_t R_ETHER_Read_ZC2_BufRelease (uint32_t channel)
{
    int32_t ret;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    uint32_t status;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

    /* When the Link up processing is not completed, return error */
    if (ETHER_FLAG_OFF == transfer_enable_flag[channel])
    {
        ret = ETHER_ERR_LINK;
    }

    /* In case of detection mode of magic packet, return error. */
    else if (1 == check_mpde_bit())

    {
        ret = ETHER_ERR_MPDE;
    }

    /* When the Link up processing is completed */
    else
    {
#if (1 == ETHER_CFG_EMAC_RX_DESCRIPTORS)
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;
#endif
        /* When receive data exists */
#if (1 == ETHER_CFG_EMAC_RX_DESCRIPTORS)
        if (0x00000000L == pedmac_adr->EDRRR.LONG)
#else
        if (RACT != (papp_rx_desc[channel]->status & RACT))
#endif
        {
            /* Move to next descriptor */

            status = RFP1;
            status |= RFP0;
            status |= RFE;
            status |= RFS9_RFOVER;
            status |= RFS8_RAD;
            status |= RFS7_RMAF;
            status |= RFS4_RRF;
            status |= RFS3_RTLF;
            status |= RFS2_RTSF;
            status |= RFS1_PRE;
            status |= RFS0_CERF;

            papp_rx_desc[channel]->status &= (~status);
            papp_rx_desc[channel]->status |= RACT;
            papp_rx_desc[channel] = papp_rx_desc[channel]->next;
        }
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;

        if (0x00000000L == pedmac_adr->EDRRR.LONG)
        {
            /* Restart if stopped */
            pedmac_adr->EDRRR.LONG = 0x00000001L;
        }

        ret = ETHER_SUCCESS;
    }
    return ret;
} /* End of function R_ETHER_Read_ZC2_BufRelease() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Write_ZC2_GetBuf
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Write_ZC2_GetBuf() function returns a pointer to the starting address of
 *            the transmit data destination.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[out] **pbuf
 *             Returns a pointer to the starting address of the transmit data destination.
 * @param[out] *pbuf_size
 *             Returns the maximum size to write to the buffer.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @retval    ETHER_ERR_TACT
 *            - Transmit buffer is not empty.
 * @details   The R_ETHER_Write_ZC2_GetBuf() function returns the parameter pbuf containing a pointer to
 *            the starting address of the transmit data destination. The function also returns the maximum size to
 *            write to the buffer to the parameter pbuf_size. Returning the pointer allows the operation to
 *            be performed with zero-copy.\n
 *            Return values indicate if the transmit buffer (pbuf) is writable or not. ETHER_SUCCESS is returned
 *            when the buffer is writable at the time of the call. When auto-negotiation is not completed,
 *            and transmission is not enabled, ETHER_ERR_LINK is returned. ETHER_ERR_MPDE is returned when
 *            a Magic Packet is being detected. ETHER_ERR_TACT is returned when the transmit buffer is not empty.\n
 *            The EDMAC hardware operates independent of the R_ETHER_Write_ZC2_GetBuf() function and writes data
 *            stored in a buffer pointed by the EDMAC transmit descriptor. The buffer pointed by the EDMAC
 *            transmit descriptor is statically allocated by the driver.
 * @note      This function is used in combination with the R_ETHER_Write_ZC2_SetBuf function.
 *            Always call the R_ETHER_Write_ZC2_GetBuf function and then the R_ETHER_Write_ZC2_SetBuf function
 *            in sequence. If the value ETHER_ERR_LINK is returned when this function is called,
 *            initialize the Ethernet FIT module.
 */
ether_return_t R_ETHER_Write_ZC2_GetBuf (uint32_t channel, void **pbuf, uint16_t *pbuf_size)
{
    ether_return_t ret;
#if (1 == ETHER_CFG_EMAC_TX_DESCRIPTORS)
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((NULL == pbuf) || (FIT_NO_PTR == pbuf))
    {
        return ETHER_ERR_INVALID_PTR;
    }
    if ((NULL == pbuf_size) || (FIT_NO_PTR == pbuf_size))
    {
        return ETHER_ERR_INVALID_PTR;
    }

    /* When the Link up processing is not completed, return error */
    if (ETHER_FLAG_OFF == transfer_enable_flag[channel])
    {
        ret = ETHER_ERR_LINK;
    }

    /* In case of detection mode of magic packet, return error. */
    else if (1 == check_mpde_bit())
    {
        ret = ETHER_ERR_MPDE;
    }

    /* When the Link up processing is completed */
    else
    {
#if (1 == ETHER_CFG_EMAC_TX_DESCRIPTORS)
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;
#endif

        /* All transmit buffers are full */
#if (1 == ETHER_CFG_EMAC_TX_DESCRIPTORS)
        if (0x00000000L != pedmac_adr->EDTRR.LONG)
#else
        if (TACT == (papp_tx_desc[channel]->status & TACT))
#endif
        {
            ret = ETHER_ERR_TACT;
        }
        else
        {
            /* Give application another buffer to work with */
            (*pbuf) = papp_tx_desc[channel]->buf_p;
            (*pbuf_size) = ETHER_CFG_BUFSIZE;
            ret = ETHER_SUCCESS;
        }
    }
    return ret;
} /* End of function R_ETHER_Write_ZC2_GetBuf() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Write_ZC2_SetBuf
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Write_ZC2_SetBuf() function enables the EDMAC to transmit the data in the transmit buffer.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[in] len
 *             Specifies the size (60 to 1,514 bytes) which is the Ethernet frame length minus 4 bytes of CRC.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @details   Call this function after writing one frame of transmit data is completed.\n
 *            Set the buffer length to be not less than 60 bytes (64 bytes of the minimum Ethernet frame minus 4 bytes
 *            of CRC) and not more than 1,514 bytes (1,518 bytes of the maximum Ethernet frame minus 4 bytes of CRC).\n
 *            To transmit data less than 60 bytes, make sure to pad the data with zero to be 60 bytes.\n
 *            Return values indicate that the data written in the transmit buffer is enabled to be transmitted.\n
 *            ETHER_SUCCESS is returned when the data in the transmit buffer is enabled to be transmitted at the time
 *            of the call. When auto-negotiation is not completed, and transmission is not enabled,
 *            ETHER_ERR_LINK is returned. ETHER_ERR_MPDE is returned when a Magic Packet is being detected.
 * @note      1.Call this function after writing one frame of transmit data is completed.\n
 *            2.To transmit data less than 60 bytes, make sure to pad the data with zero to be 60 bytes.\n
 *            3.Before calling this function, use the R_ETHER_Write_ZC2_GetBuf function to read data.
 *              Call this function after ETHER_SUCCESS is returned.\n
 *            4.This function is used in combination with the R_ETHER_Write_ZC2_GetBuf function.
 *              Always call the R_ETHER_Write_ZC2_GetBuf function and then the R_ETHER_Write_ZC2_SetBuf
 *              function in sequence. If the value ETHER_ERR_LINK is returned when this function is called,
 *              initialize the Ethernet FIT module.
 */
ether_return_t R_ETHER_Write_ZC2_SetBuf (uint32_t channel, const uint32_t len)
{
    ether_return_t ret;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((ETHER_BUFSIZE_MIN > len) || (ETHER_BUFSIZE_MAX < len))
    {
        return ETHER_ERR_INVALID_DATA;
    }

    /* When the Link up processing is not completed, return error */
    if (ETHER_FLAG_OFF == transfer_enable_flag[channel])
    {
        ret = ETHER_ERR_LINK;
    }

    /* In case of detection mode of magic packet, return error. */
    else if (1 == check_mpde_bit())

    {
        ret = ETHER_ERR_MPDE;
    }

    /* When the Link up processing is completed */
    else
    {
        /* The data of the buffer is made active.  */
        papp_tx_desc[channel]->bufsize = len;
        papp_tx_desc[channel]->status &= (~(TFP1 | TFP0));
        papp_tx_desc[channel]->status |= ((TFP1 | TFP0) | TACT);
        papp_tx_desc[channel] = papp_tx_desc[channel]->next;

        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;

        if (0x00000000L == pedmac_adr->EDTRR.LONG)
        {
            /* Restart if stopped */
            pedmac_adr->EDTRR.LONG = 0x00000001L;
        }

        ret = ETHER_SUCCESS;
    }
    return ret;
} /* End of function R_ETHER_Write_ZC2_SetBuf() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_CheckLink_ZC
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_CheckLink_ZC() function checks the status of the physical Ethernet link
 *            using PHY management interface. Ethernet link is up when the cable is connected to a peer device
 *            whose PHY is properly initialized.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    ETHER_SUCCESS
 *            - the link status is link up or the operation starts normally when the non-blocking mode is enabled.
 * @retval    ETHER_ERR_OTHER
 *            - the link status is link-down or the non-blocking mode is enabled and the interrupt handler function
 *              is not registered
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_LOCKED
 *            - When PHY access is in progress when non-blocking mode is enabled .
 * @details   The R_ETHER_CheckLink_ZC() function checks the status of the physical Ethernet link
 *            using PHY management interface. This information (status of Ethernet link) is read from
 *            the basic status register (register 1) of the PHY-LSI device. If non-blocking mode is disabled,
 *            ETHER_SUCCESS is returned when the link is up, and ETHER_ERR_OTHER when the link is down.\n
 *            When non-blocking mode is enabled, the check result is passed as an argument of
 *            the interrupt handler function after the link status check is completed.
 * @note      None.
 */
ether_return_t R_ETHER_CheckLink_ZC (uint32_t channel)
{
#if (ETHER_CFG_NON_BLOCKING == 0)
    int16_t status;
#endif
#if (ETHER_CFG_NON_BLOCKING == 1)
    uint16_t pmgi_channel;
    pmgi_cb_arg_t pmgi_cb_arg;
    uint16_t reg = 0;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

#if (ETHER_CFG_NON_BLOCKING == 1)
    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return ETHER_ERR_OTHER;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE *)&g_phy_current_param[pmgi_channel].locked))
    {
         return ETHER_ERR_LOCKED;
    }
    set_ether_channel(channel);

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_ERROR;
            pmgi_cb_arg.mode = CHECKLINK_ZC;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
        return ETHER_ERR_OTHER;
    }

    g_phy_current_param[pmgi_channel].mode = CHECKLINK_ZC;

    g_phy_current_param[pmgi_channel].step = STEP0;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

    return ETHER_SUCCESS;
#elif ETHER_CFG_NON_BLOCKING == 0
    status = phy_get_link_status(channel);

    if (R_PHY_ERROR == status)
    {
        /* Link is down */
        return ETHER_ERR_OTHER;
    }
    else
    {
        /* Link is up */
        return ETHER_SUCCESS;
    }
#endif

} /* End of function R_ETHER_CheckLink_ZC() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_LinkProcess
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_LinkProcess() function performs link signal change interrupt processing and
 *            Magic Packet detection interrupt processing.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    None.
 * @details   Note that link status change detection processing takes place but link signal change interrupt
 *            processing does not occur when ETHER_CFG_USE_LINKSTA is set to a value of 0.
 *            When non-blocking mode is enabled, the processing result of the function is passed as an argument of
 *            the PMGI callback function.
 *            See Section 3.9 in the application note for more details.
 * @note      1.If ETHER_CFG_USE_LINKSTA is set to a value of 1, either call this function periodically within the
 *              normal processing routine. Note that Ethernet transmission and reception may not operate correctly,
 *              and the Ethernet driver may not enter Magic Packet detection mode correctly, if this function
 *              is not called.
 *            2.If ETHER_CFG_USE_LINKSTA is set to a value of 0, either call this function periodically within the
 *              normal processing routine, or call it from an interrupt function that is processed when a periodically
 *              occurring interrupt source occurs. Note that Ethernet transmission and reception may
 *              not operate correctly, and the Ethernet driver may not enter Magic Packet detection mode correctly,
 *              if this function is not called.
 *            3.If no callback function was registered with the function R_ETHER_Control(),
 *              there will be no notification by a callback function.
 */
void R_ETHER_LinkProcess (uint32_t channel)
{

#if (ETHER_CFG_NON_BLOCKING == 0)
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    int32_t ret;
#endif

#if (ETHER_CFG_NON_BLOCKING == 1)
    volatile bsp_int_err_t bsp_int_err;
    uint16_t pmgi_channel;
    pmgi_cb_arg_t pmgi_cb_arg;
    uint16_t reg_data = 0;
#endif
    ether_cb_arg_t cb_arg;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return;
    }
#if (ETHER_CFG_NON_BLOCKING == 1)

    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE *)&g_phy_current_param[pmgi_channel].locked))
    {
        R_BSP_NOP();
        return;
    }

    set_ether_channel(channel);

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_ERROR;
            pmgi_cb_arg.mode = LINKPROCESS;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
        return;
    }
#endif

    /* When the magic packet is detected. */
    if (ETHER_FLAG_ON == mpd_flag[channel])
    {
        mpd_flag[channel] = ETHER_FLAG_OFF;

        if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
        {
            cb_arg.channel = channel;
            cb_arg.event_id = ETHER_CB_EVENT_ID_WAKEON_LAN;
            (*cb_func.pcb_func)((void *) &cb_arg);
        }

        /*
         * After the close function is called, the open function is called 
         * to have to set ETHERC to a usual operational mode
         * to usually communicate after magic packet is detected. 
         */
        R_ETHER_Close_ZC2(channel);
#if (ETHER_CFG_NON_BLOCKING == 1)

        /* Initialize the flags */
        transfer_enable_flag[channel] = ETHER_FLAG_OFF;
        mpd_flag[channel] = ETHER_FLAG_OFF;
        lchng_flag[channel] = ETHER_FLAG_OFF;

    #if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N))
        /* Subscribe to r_bsp an interrupt function */
        if (false == is_entry)
        {
            bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC0_EINT0, ether_eint0); /* EINT0 */
            bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC1_EINT1, ether_eint1); /* EINT1 */
            is_entry = true;
        }
    #elif (defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX66N))
        /* Subscribe to r_bsp an interrupt function */
        if (false == is_entry)
        {
            bsp_int_err = R_BSP_InterruptWrite(BSP_INT_SRC_AL1_EDMAC0_EINT0, ether_eint0); /* EINT0 */
            is_entry = true;
        }
    #endif

        /* Software reset */
        ether_reset_mac(channel);

        g_phy_current_param[pmgi_channel].mode = LINKPROCESS_OPEN_ZC2;
        g_phy_current_param[pmgi_channel].step = STEP0;

        g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

        pmgi_access(channel, PHY_REG_CONTROL, PHY_CONTROL_RESET, PMGI_WRITE);
        return;

#elif (ETHER_CFG_NON_BLOCKING == 0)
        R_ETHER_Open_ZC2(channel, mac_addr_buf[channel], pause_frame_enable[channel]);
#endif
    }

#if (ETHER_CFG_USE_LINKSTA == 0)
    #if (ETHER_CFG_NON_BLOCKING == 1)
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC0;
        g_phy_current_param[pmgi_channel].step = STEP0;

        pmgi_access(channel, PHY_REG_STATUS, reg_data, PMGI_READ);
        return ;

    #elif (ETHER_CFG_NON_BLOCKING == 0)
        ret = R_ETHER_CheckLink_ZC(channel);
        if (g_pre_link_stat[channel] != ret)
        {
            if (ret == ETHER_SUCCESS)
            {
                /* The state of the link status in PHY-LSI is confirmed and Link Up/Down is judged. */
                /* When becoming Link up */
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;
            }
            else
            {
                /* When becoming Link down */
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_OFF;
            }
        }
        g_pre_link_stat[channel] = ret;
    #endif
#endif

    /* When the link is up */
    if (ETHER_FLAG_ON_LINK_ON == lchng_flag[channel])
    {
#if (ETHER_CFG_USE_LINKSTA == 1)
        /* 
         * The Link Up/Down is confirmed by the Link Status bit of PHY register1, 
         * because the LINK signal of PHY-LSI is used for LED indicator, and 
         * isn't used for notifing the Link Up/Down to external device.
         */
    #if (ETHER_CFG_NON_BLOCKING == 1)
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC1;
        g_phy_current_param[pmgi_channel].step = STEP0;

        pmgi_access(channel, PHY_REG_STATUS, reg_data, PMGI_READ);
        return ;
    #elif (ETHER_CFG_NON_BLOCKING == 0)
        ret = R_ETHER_CheckLink_ZC(channel);
        if (ETHER_SUCCESS == ret)
        {
            /*
             * The status of the LINK signal became "link-up" even if PHY-LSI did not detect "link-up"
             * after a reset. To avoid this wrong detection, processing in R_ETHER_LinkProcess has been modified to
             * clear the flag after link-up is confirmed in R_ETHER_CheckLink_ZC.
             */
            lchng_flag[channel] = ETHER_FLAG_OFF;

            /* Initialize the transmit and receive descriptor */
            memset((void *)&rx_descriptors[channel], 0x00, sizeof(rx_descriptors[channel]));
            memset((void *)&tx_descriptors[channel], 0x00, sizeof(tx_descriptors[channel]));

            /* Initialize the Ether buffer */
            memset(&ether_buffers[channel], 0x00, sizeof(ether_buffers[channel]));

            transfer_enable_flag[channel] = ETHER_FLAG_ON;
            
            /*
             * ETHERC and EDMAC are set after ETHERC and EDMAC are reset in software
             * and sending and receiving is permitted. 
             */
            ether_configure_mac(channel, mac_addr_buf[channel], NO_USE_MAGIC_PACKET_DETECT);
            ret = ether_do_link(channel, NO_USE_MAGIC_PACKET_DETECT);
            if (ETHER_SUCCESS == ret)
            {
                if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
                {
                    cb_arg.channel = channel;
                    cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_ON;
                    (*cb_func.pcb_func)((void *) &cb_arg);
                }
            }
            else
            {
                /* When PHY auto-negotiation is not completed */
                transfer_enable_flag[channel] = ETHER_FLAG_OFF;
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;
            }
        }
        else
        {
            /* no process */
        }
    #endif /* (ETHER_CFG_NON_BLOCKING == 1) */
#elif ((ETHER_CFG_USE_LINKSTA == 0) && (ETHER_CFG_NON_BLOCKING == 0))
       /*
        * The status of the LINK signal became "link-up" even if PHY-LSI did not detect "link-up"
        * after a reset. To avoid this wrong detection, processing in R_ETHER_LinkProcess has been modified to
        * clear the flag after link-up is confirmed in R_ETHER_CheckLink_ZC.
        */
        lchng_flag[channel] = ETHER_FLAG_OFF;

        /* Initialize the transmit and receive descriptor */
        memset((void *)&rx_descriptors[channel], 0x00, sizeof(rx_descriptors[channel]));
        memset((void *)&tx_descriptors[channel], 0x00, sizeof(tx_descriptors[channel]));

        /* Initialize the Ether buffer */
        memset(&ether_buffers[channel], 0x00, sizeof(ether_buffers[channel]));

        transfer_enable_flag[channel] = ETHER_FLAG_ON;
       
       /*
        * ETHERC and EDMAC are set after ETHERC and EDMAC are reset in software
        * and sending and receiving is permitted. 
        */
        ether_configure_mac(channel, mac_addr_buf[channel], NO_USE_MAGIC_PACKET_DETECT);
        ret = ether_do_link(channel, NO_USE_MAGIC_PACKET_DETECT);
        if (ETHER_SUCCESS == ret)
        {
            if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
            {
                cb_arg.channel = channel;
                cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_ON;
                (*cb_func.pcb_func)((void *) &cb_arg);
            }
        }
        else
        {
            /* When PHY auto-negotiation is not completed */
            transfer_enable_flag[channel] = ETHER_FLAG_OFF;
            lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;
        }
#endif /* (ETHER_CFG_USE_LINKSTA == 1) */
    }

    /* When the link is down */
    else if (ETHER_FLAG_ON_LINK_OFF == lchng_flag[channel])
    {
        lchng_flag[channel] = ETHER_FLAG_OFF;

#if (ETHER_CFG_USE_LINKSTA == 1)
        /* 
         * The Link Up/Down is confirmed by the Link Status bit of PHY register1, 
         * because the LINK signal of PHY-LSI is used for LED indicator, and 
         * isn't used for notifying the Link Up/Down to external device.
         */
    #if (ETHER_CFG_NON_BLOCKING == 1)
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC2;
        g_phy_current_param[pmgi_channel].step = STEP0;

        pmgi_access(channel, PHY_REG_STATUS, reg_data, PMGI_READ);
        return ;
    #elif (ETHER_CFG_NON_BLOCKING == 0)
        ret = R_ETHER_CheckLink_ZC(channel);
        if (ETHER_ERR_OTHER == ret)
        {
            pether_ch = g_eth_control_ch[channel].pether_control;
            phy_access = g_eth_control_ch[channel].phy_access;
            petherc_adr = pether_ch[phy_access].petherc;

            /* Disable receive and transmit. */
            petherc_adr->ECMR.BIT.RE = 0;
            petherc_adr->ECMR.BIT.TE = 0;

            transfer_enable_flag[channel] = ETHER_FLAG_OFF;

            if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
            {
                cb_arg.channel = channel;
                cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_OFF;
                (*cb_func.pcb_func)((void *) &cb_arg);
            }
        }
        else
        {
            ; /* no operation */
        }
    #endif
#elif ((ETHER_CFG_USE_LINKSTA == 0) && (ETHER_CFG_NON_BLOCKING == 0))
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        petherc_adr = pether_ch[phy_access].petherc;

        /* Disable receive and transmit. */
        petherc_adr->ECMR.BIT.RE = 0;
        petherc_adr->ECMR.BIT.TE = 0;

        transfer_enable_flag[channel] = ETHER_FLAG_OFF;

        if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
        {
            cb_arg.channel = channel;
            cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_OFF;
            (*cb_func.pcb_func)((void *) &cb_arg);
        }
#endif
    }
    else
    {
#if (ETHER_CFG_NON_BLOCKING == 1)
        pmgi_close(pmgi_channel);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_IDLE;
            pmgi_cb_arg.mode = LINKPROCESS;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
#endif
        /* no operation */
    }
} /* End of function R_ETHER_LinkProcess() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_WakeOnLAN
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_WakeOnLAN() function switches the ETHERC setting from normal transmission/reception
 *            to Magic Packet detection.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully or the PMGI operation start normally when the non-blocking mode
 *              is enable.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_OTHER
 *            - A switch to magic packet detection was performed when the link state was link is down.
 *              Or the non-blocking mode is enabled and the interrupt handler function is not registered.
 * @retval    ETHER_ERR_LOCKED
 *            - When PHY access is in progress when non-blocking mode is enabled.
 * @details   The R_ETHER_WakeOnLAN() function initializes the ETHERC and EDMAC,
 *            and then switches the ETHERC to Magic Packet detection.\n
 *            If non-blocking call is disabled, return values indicate whether the ETHERC has been switched
 *            to Magic Packet detection or not. When auto-negotiation is not completed,
 *            and transmission/reception is not enabled, ETHER_ERR_LINK is returned.
 *            ETHER_ERR_OTHER is returned if the link is down after ETHERC is set to Magic Packet detection.\n
 *            When non-blocking mode is enabled, the processing result of the function is passed as
 *            an argument of the PMGI callback function.
 * @note      None.
 */
ether_return_t R_ETHER_WakeOnLAN (uint32_t channel)
{
#if (ETHER_CFG_USE_LINKSTA == 1 && ETHER_CFG_NON_BLOCKING == 0)
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
#endif
    ether_return_t ret;

#if (ETHER_CFG_NON_BLOCKING == 1)
    uint16_t reg_data = 0;
    uint16_t pmgi_channel;
    pmgi_cb_arg_t pmgi_cb_arg;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

#if (ETHER_CFG_NON_BLOCKING == 1)
    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return ETHER_ERR_OTHER;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE *)&g_phy_current_param[pmgi_channel].locked))
    {
        return ETHER_ERR_LOCKED;
    }

    set_ether_channel(channel);

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_ERROR;
            pmgi_cb_arg.mode = WAKEONLAN;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
        return ETHER_ERR_OTHER;
    }
#endif

    /* When the Link up processing is not completed, return error */
    if (ETHER_FLAG_OFF == transfer_enable_flag[channel])
    {
        ret = ETHER_ERR_LINK;
#if (ETHER_CFG_NON_BLOCKING == 1)
        pmgi_close(pmgi_channel);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
            pmgi_cb_arg.channel = channel;
            pmgi_cb_arg.event = PMGI_IDLE;
            pmgi_cb_arg.mode = WAKEONLAN;
            (*cb_func.pcb_pmgi_hnd)((void *)&pmgi_cb_arg);
        }
#endif
    }
    /* When the Link up processing is completed */
    else
    {
        /* Change to the magic packet detection mode.  */
        ether_configure_mac(channel, mac_addr_buf[channel], USE_MAGIC_PACKET_DETECT);
#if (ETHER_CFG_NON_BLOCKING == 1)
        g_phy_current_param[pmgi_channel].mode = WAKEONLAN;
        g_phy_current_param[pmgi_channel].step = STEP0;

        pmgi_access(channel, PHY_REG_STATUS, reg_data, PMGI_READ);

        return ETHER_SUCCESS;
#elif (ETHER_CFG_NON_BLOCKING == 0)
        ret = ether_do_link(channel, USE_MAGIC_PACKET_DETECT);
        if (ETHER_SUCCESS == ret)
        {
    #if (ETHER_CFG_USE_LINKSTA == 1)
            pether_ch = g_eth_control_ch[channel].pether_control;
            phy_access = g_eth_control_ch[channel].phy_access;
            petherc_adr = pether_ch[phy_access].petherc;

            /* It is confirmed not to become Link down while changing the setting. */
            if (ETHER_CFG_LINK_PRESENT == petherc_adr->PSR.BIT.LMON)
            {
                ret = ETHER_SUCCESS;
            }
            else
            {
                ret = ETHER_ERR_OTHER;
            }
    #else
            /* It is confirmed not to become Link down while changing the setting. */
            ret = R_ETHER_CheckLink_ZC(channel);
    #endif
        }
        else
        {
            ret = ETHER_ERR_OTHER;
        }
#endif
    }
    return ret;
} /* End of function R_ETHER_WakeOnLAN() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Read
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Read() function receives data into the specified receive buffer.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[out] *pbuf
 *             Specifies the receive buffer (to store the receive data).\n
 *             The maximum write size is 1,514 bytes. When calling this function,
 *             specify the start address of a array with a capacity of 1,514 bytes.
 * @retval    A value of 1 or greater
 *            - Returns the number of bytes received.
 * @retval    ETHER_NO_DATA
 *            - A zero value indicates no data is received.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @retval    ETHER_ERR_MC_FRAME
 *            - Multicast frame detected when multicast frame filtering is enabled.
 * @details   This function stores the receive data in the specified receive buffer.\n
 *            Return values indicate the number of bytes received. If there is no data available at the time
 *            of the call, ETHER_NO_DATA is returned. When auto-negotiation is not completed,
 *            and reception is not enabled, ETHER_ERR_LINK is returned. ETHER_ERR_MPDE is returned
 *            when a Magic Packet is being detected.\n
 *            When multicast frame filtering on the specified channel is enabled by the R_ETHER_Control function,
 *            the buffer is released immediately when a multicast frame is detected.
 *            Also, the value ETHER_ERR_MC_FRAME is returned.
 *            Note that when hardware-based multicast frame filtering is enabled on the RX64M, RX71M, RX72M,
 *            RX72N or RX66N, multicast frames are discarded by the hardware and detection is not possible.\n
 *            Frames that generate a receive FIFO overflow, residual-bit frame receive error,
 *            long frame receive error, short frame receive error, PHY-LSI receive error, or receive frame
 *            CRC error are treated as receive frame errors. When a receive frame error occurs,
 *            the descriptor data is discarded, the status is cleared, and reading of data continues.
 * @note      1.As this function calls the R_ETHER_Read_ZC2() function and the R_ETHER_Read_ZC2_BufRelease()
 *              function internally, data is copied between the buffer pointed by the EDMAC receive descriptor
 *              and the receive buffer specified by the R_ETHER_Read() function. (The maximum write size is
 *              1,514 bytes, so set aside a space of 1,514 bytes for the specified receive buffer.)\n
 *            2.Make sure not to use the R_ETHER_Read_ZC2() function and R_ETHER_Read_ZC2_BufRelease()
 *              function when using the R_ETHER_Read() function.\n
 *            3.This function uses the standard function memcpy, so string.h is included.\n
 *            4.If the value ETHER_ERR_LINK is returned when this function is called,
 *              initialize the Ethernet FIT module.
 */
int32_t R_ETHER_Read (uint32_t channel, void *pbuf)
{
    int32_t ret;
    int32_t ret2;
    uint8_t * pread_buffer_address; /* Buffer location controlled by the Ethernet driver */

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((NULL == pbuf) || (FIT_NO_PTR == pbuf))
    {
        return ETHER_ERR_INVALID_PTR;
    }

    /* (1) Retrieve the receive buffer location controlled by the  descriptor. */
    ret = R_ETHER_Read_ZC2(channel, (void **) &pread_buffer_address);

    /* When there is data to receive */
    if (ret > ETHER_NO_DATA)
    {
        /* (2) Copy the data read from the receive buffer which is controlled by the descriptor to
         the buffer which is specified by the user (up to 1024 bytes). */
        memcpy(pbuf, pread_buffer_address, (uint32_t )ret);

        /* (3) Read the receive data from the receive buffer controlled by the descriptor,
         and then release the receive buffer. */
        ret2 = R_ETHER_Read_ZC2_BufRelease(channel);

        if (ETHER_SUCCESS == ret2) /* When this function is completed successfully */
        {
            /* Do Nothing */
        }

        /* Overwrite the error information */
        else
        {
            ret = ret2;
        }
    }

    /* When there is no data to receive */
    else
    {
        /* Do Nothing */
    }
    return ret;
} /* End of function R_ETHER_Read() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Write
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Write() function transmits the data from the specified transmit buffer.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[in] *pbuf
 *             Specifies the transmit data (the destination for the transmit data to be written).
 * @param[in] len
 *             Specifies the size (60 to 1,514 bytes) which is the Ethernet frame length minus 4 bytes of CRC.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_INVALID_DATA
 *            - Value of the argument is out of range.
 * @retval    ETHER_ERR_INVALID_PTR
 *            - Value of the pointer is NULL or FIT_NO_PTR.
 * @retval    ETHER_ERR_LINK
 *            - Auto-negotiation is not completed, and reception is not enabled.
 * @retval    ETHER_ERR_MPDE
 *            - As a Magic Packet is being detected, transmission and reception is not enabled.
 * @retval    ETHER_ERR_TACT
 *            - Transmit buffer is not empty.
 * @details   This function transmits data from the specified transmit buffer.\n
 *            Set the buffer length to be not less than 60 bytes (64 bytes of the minimum Ethernet frame
 *            minus 4 bytes of CRC) and not more than 1,514 bytes (1,518 bytes of the maximum Ethernet frame
 *            minus 4 bytes of CRC).\n
 *            To transmit data less than 60 bytes, make sure to pad the data with zero to be 60 bytes.\n
 *            Return values indicate that the data written in the transmit buffer is enabled to be transmitted.\n
 *            ETHER_SUCCESS is returned when the data in the transmit buffer is enabled to transmit at the time
 *            of the call. When auto-negotiation is not completed, and transmission is not enabled,
 *            ETHER_ERR_LINK is returned. ETHER_ERR_MPDE is returned when a Magic Packet is being detected.
 *            The value ETHER_ERR_TACT is returned if there is no free space in the transmit buffer.
 * @note      1.To transmit data less than 60 bytes, make sure to pad the data with zero to be 60 bytes.\n
 *            2.As this function calls the R_ETHER_Write_ZC2_GetBuf() function and
 *              the R_ETHER_Write_ZC2_SetBuf() function internally, data is copied between the buffer pointed
 *              by the EDMAC transmit descriptor and the transmit buffer specified by the R_ETHER_Write() function.\n
 *            3.Make sure not to use the R_ETHER_Write_ZC2_GetBuf() function and R_ETHER_Write_ZC2_SetBuf()
 *              function when using the R_ETHER_Write() function.\n
 *            4.This function uses the standard functions memset and memcpy, so string.h is included.\n
 *            5.If the value ETHER_ERR_LINK is returned when this function is called,
 *              initialize the Ethernet FIT module.
 */
ether_return_t R_ETHER_Write (uint32_t channel, void *pbuf, const uint32_t len)
{
    ether_return_t ret;
    uint8_t * pwrite_buffer_address;
    uint16_t write_buf_size;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    if ((NULL == pbuf) || (FIT_NO_PTR == pbuf))
    {
        return ETHER_ERR_INVALID_PTR;
    }
    if ((ETHER_BUFSIZE_MIN > len) || (ETHER_BUFSIZE_MAX < len))
    {
        return ETHER_ERR_INVALID_DATA;
    }

    /* (1) Retrieve the transmit buffer location controlled by the  descriptor. */
    ret = R_ETHER_Write_ZC2_GetBuf(channel, (void **) &pwrite_buffer_address, &write_buf_size);

    /* Writing to the transmit buffer (buf) is enabled. */
    if (ETHER_SUCCESS == ret)
    {
        if (write_buf_size < len)
        {
            ret = ETHER_ERR_TACT; /* Transmit buffer overflow */
        }
        else
        {
            /* Write the transmit data to the transmit buffer. */

            /* (2) Write the data to the transmit buffer controlled by the  descriptor. */
            memcpy(pwrite_buffer_address, pbuf, len);

            /* (3) Enable the EDMAC to transmit data in the transmit buffer. */
            ret = R_ETHER_Write_ZC2_SetBuf(channel, len);

            /* 
             * Confirm that the transmission is completed.
             * Data written in the transmit buffer is transmitted by the EDMAC. Make sure that the
             * transmission is completed after writing data to the transmit buffer.
             * If the R_ETHER_Close_ZC2 function is called to stop the Ethernet communication before
             * verifying that the transmission is completed, the written data written may not be transmitted.
             */
            ret = R_ETHER_CheckWrite(channel);
        }
    }
    return ret;
} /* End of function R_ETHER_Write() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_CheckWrite
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_CheckWrite() function verifies that data transmission has completed.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @details   The R_ETHER_CheckWrite() function verifies that data was transmitted.\n
 *            If the transmission completed, ETHER_SUCCESS is returned.
 * @note      1.This function should be called after transmit data has been written with
 *              the R_ETHER_Write_ZC2_Setbuf() function.\n
 *            2.Note that it takes several tens of microseconds for data transmission to actually complete after
 *              the R_ETHER_Write_ZC2_Setbuf() function is called. Therefore, when using the R_ETHER_Close_ZC2()
 *              function to shut down the Ethernet module following data transmission, call the R_ETHER_CheckWrite()
 *              function after calling the R_ETHER_Write_ZC2_Setbuf() function and, after waiting for
 *              data transmission to finish, call the R_ETHER_Close_ZC2() function. Calling the R_ETHER_Close_ZC2()
 *              function without calling the R_ETHER_CheckWrite() function can cause data transmission to be
 *              cut off before it completes.
 */
ether_return_t R_ETHER_CheckWrite (uint32_t channel)
{
    ether_return_t ret;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        ret = ETHER_ERR_INVALID_CHAN;
    }
    else
    {
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        pedmac_adr = pether_ch[phy_access].pedmac;
        /* WAIT_LOOP */
        while (0 != pedmac_adr->EDTRR.BIT.TR)
        {
            /* Do Nothing */
        }
        ret = ETHER_SUCCESS;
    }
    return ret;
} /* End of function R_ETHER_CheckWrite() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_Control
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_Control() function performs the processing that corresponds to the control code.
 * @param[in] cmd
 *             Specifies the control code.
 * @param[in] control
 *             Specify the parameters according to the control code.
 * @retval    ETHER_SUCCESS
 *            - Processing completed successfully or the PMGI operation start normally when the non-blocking mode
 *              is enable.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_CHAN_OPEN
 *            - Indicates the Ethernet cannot be opened because it is being used by another application.
 * @retval    ETHER_ERR_INVALID_ARG
 *            - Invalid argument.
 * @retval    ETHER_ERR_RECV_ENABLE
 *            - ETHERC receive function enabled.
 * @details   Performs the processing that corresponds to the control code.
 *            The value ETHER_ERR_INVALID_ARG is returned if the control code is not supported.\n
 *            See section 3.14 in application note for more details.
 * @note      Register callback functions and interrupt handlers before calling the R_ETHER_Open_ZC2() function.
 *            It may not be possible to detect the first interrupt if the preceding are registered after
 *            the R_ETHER_Open_ZC2() function is called.\n
 *            Specify promiscuous mode after setting the control code to CONTROL_POWER_ON and calling this function.
 *            The intended value will not be stored in the ETHERC mode register if the promiscuous mode setting
 *            is specified without first setting the control code to CONTROL_POWER_ON and calling this function.\n
 *            Multicast frame filtering and broadcast frame filtering settings cannot be made while the receive
 *            functionality of the ETHERC is enabled. Make these settings before calling the R_ETHER_LinkProcess
 *            function. After the R_ETHER_LinkProcess function is called, the receive functionality is enabled
 *            when the Ethernet FIT module enters link up status, so calling this function with
 *            CONTROL_MULTICASTFRAME_FILTER or CONTROL_BROADCASTFRAME_FILTER set as the control code causes
 *            ETHER_ERR_RECV_ENABLE to be returned, and the settings have no effect.
 */
ether_return_t R_ETHER_Control (ether_cmd_t const cmd, ether_param_t const control)
{
    ether_return_t ret;

    switch (cmd)
    {
        /* Set the callback function pointer */
        case CONTROL_SET_CALLBACK :
            ret = ether_set_callback(control);
        break;

        case CONTROL_SET_PROMISCUOUS_MODE :
            ret = ether_set_promiscuous_mode(control);
        break;

        case CONTROL_SET_INT_HANDLER :
            ret = ether_set_int_handler(control);
        break;

        case CONTROL_POWER_ON :
            ret = ether_power_on(control);
        break;

        case CONTROL_POWER_OFF :
            ret = ether_power_off(control);
        break;

        case CONTROL_MULTICASTFRAME_FILTER :
            ret = ether_set_multicastframe_filter(control);
        break;

        case CONTROL_BROADCASTFRAME_FILTER :
            ret = ether_set_broadcastframe_filter(control);
        break;

        case CONTROL_RECEIVE_DATA_PADDING :
            ret = ether_receive_data_padding(control);
        break;
#if (ETHER_CFG_NON_BLOCKING == 1)
        case CONTROL_SET_PMGI_CALLBACK :
            ret = ether_set_pmgi_callback(control);
        break;
#endif

        default :
            ret = ETHER_ERR_INVALID_ARG;
        break;
    }
    return ret;
} /* End of function R_ETHER_Control() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_WritePHY
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_WritePHY function uses the PHY management interface to write to registers in the PHY-LSI.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[in] address
 *             Specify the address of the PHY-LSI register to be accessed. For details,
 *             check the data sheet of the PHY-LSI to be used.
 * @param[in] data
 *             Specify the data to be written to the PHY-LSI register.
 *             For details, check the data sheet of the PHY-LSI to be used.
 * @retval    ETHER_SUCCESS
 *            - When access is completed normally or when the operation start normally when non-blocking mode is enabled.
 * @retval    ETHER_ERR_OTHER
 *            - When non-blocking mode is enabled and no interrupt handler function is registered.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_LOCKED
 *            - When non-blocking mode is enabled and PHY is being accessed.
 * @details   The R_ETHER_WritePHY function uses the PHY management interface to write access to registers in the PHY-LSI.
 *            If non-blocking mode is disabled, ETHER_SUCCESS is returned when write access is successfully completed.\n
 *            When non-blocking mode is enabled, the callback function is executed after the write access is completed.
 * @note      None
 */
ether_return_t R_ETHER_WritePHY(uint32_t channel, uint16_t address, uint16_t data)
{
#if (ETHER_CFG_NON_BLOCKING == 1)
    uint32_t pmgi_channel;
#endif
    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

 #if (ETHER_CFG_NON_BLOCKING == 1)
    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return ETHER_ERR_OTHER;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE *)&g_phy_current_param[pmgi_channel].locked))
    {
        return ETHER_ERR_LOCKED;
    }

    set_ether_channel(channel);

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        return ETHER_ERR_OTHER;
    }

    g_phy_current_param[pmgi_channel].mode = WRITEPHY;
    g_phy_current_param[pmgi_channel].step = STEP0;

    pmgi_access(channel, address, data, PMGI_WRITE);

#elif (ETHER_CFG_NON_BLOCKING == 0)
    phy_write(channel, address, data);
#endif

    return ETHER_SUCCESS;
} /* End of function R_ETHER_WritePHY() */
/**********************************************************************************************************************
 * Function Name: R_ETHER_ReadPHY
 *****************************************************************************************************************/ /**
 * @brief     The R_ETHER_ReadPHY function uses the PHY management interface to access to the registers in the PHY-LSI.
 * @param[in] channel
 *             Specifies the ETHERC and EDMAC channel number (0 or 1).
 *             This value must be specified as 0 on products with only one ETHERC and EDMAC channel.
 * @param[in] address
 *             Specify the address of the PHY-LSI register to be accessed. For details,
 *             check the data sheet of the PHY-LSI to be used.
 * @param[out] *p_data
 *             Specify the pointer of the variable to store the register value read from PHY-LSI.
 *             For details, check the data sheet of the PHY-LSI to be used.
 * @retval    ETHER_SUCCESS
 *            - When access is completed normally or when the operation start normally when non-blocking mode is enabled.
 * @retval    ETHER_ERR_OTHER
 *            - When non-blocking mode is enabled and no interrupt handler function is registered.
 * @retval    ETHER_ERR_INVALID_CHAN
 *            - Nonexistent channel number.
 * @retval    ETHER_ERR_LOCKED
 *            - When non-blocking mode is enabled and PHY is being accessed.
 * @details   When non-blocking mode is disabled, the register value read from the PHY-LSI is stored in
 *            the argument p_data. Also, ETHER_SUCCESS is returned when the read access is successfully completed.\n
 *            When non-blocking mode is enabled, the read value is transferred as an argument of the callback function.\n
 * @note      None
 */
ether_return_t R_ETHER_ReadPHY(uint32_t channel, uint16_t address, uint16_t *p_data)
{
#if (ETHER_CFG_NON_BLOCKING == 1)
    uint32_t pmgi_channel;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

#if (ETHER_CFG_NON_BLOCKING == 1)
    if (NULL == cb_func.pcb_pmgi_hnd)
    {
        return ETHER_ERR_OTHER;
    }

    pmgi_channel = get_pmgi_channel(channel);

    if (false == R_BSP_SoftwareLock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked))
    {
        return ETHER_ERR_LOCKED;
    }

    set_ether_channel(channel);

    if (R_PHY_ERROR == pmgi_initial(pmgi_channel))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[pmgi_channel].locked);
        return ETHER_ERR_OTHER;
    }

    g_phy_current_param[pmgi_channel].mode = READPHY;
    g_phy_current_param[pmgi_channel].step = STEP0;

    pmgi_access(channel, address, *p_data, PMGI_READ);

#elif (ETHER_CFG_NON_BLOCKING == 0)
    *p_data = phy_read(channel, address);
#endif

    return ETHER_SUCCESS;
} /* End of function R_ETHER_ReadPHY() */

/**********************************************************************************************************************
 * Function Name: R_ETHER_GetVersion
 *****************************************************************************************************************/ /**
 * @brief     This function returns the API version.
 * @return    Version number.
 * @details   Returns the API version number.
 * @note      None
 */
uint32_t R_ETHER_GetVersion (void)
{
    return ((((uint32_t) ETHER_RX_VERSION_MAJOR) << 16) | ((uint32_t) ETHER_RX_VERSION_MINOR));
} /* End of function R_ETHER_GetVersion() */

/*
 * Private functions
 */

/***********************************************************************************************************************
 * Function Name: ether_reset_mac
 * Description  : The EDMAC and EtherC are reset through the software reset.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_reset_mac (uint32_t channel)
{
    volatile uint32_t i;

    /* Software reset */
#if (ETHER_CHANNEL_MAX == 1)
    if (ETHER_CHANNEL_0 == channel)
    {
        EDMAC0.EDMR.BIT.SWR = 1;
    }
#elif (ETHER_CHANNEL_MAX == 2)
    if (ETHER_CHANNEL_0 == channel)
    {
        EDMAC0.EDMR.BIT.SWR = 1;
    }
    else
    {
        EDMAC1.EDMR.BIT.SWR = 1;
    }
#endif

    /*
     * Waiting time until the initialization of ETHERC and EDMAC is completed is 64 cycles
     * in the clock conversion of an internal bus of EDMAC. 
     */
    /* WAIT_LOOP */
    for (i = 0; i < 0x00000180; i++)
    {
        ;
    }

} /* End of function ether_reset_mac() */

/***********************************************************************************************************************
 * Function Name: ether_init_descriptors
 * Description  : The EDMAC descriptors and the driver buffers are initialized.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_init_descriptors (uint32_t channel)
{
    descriptor_t * pdescriptor;
    uint32_t i;

    /* Initialize the receive descriptors */
    /* WAIT_LOOP */
    for (i = 0; i < ETHER_CFG_EMAC_RX_DESCRIPTORS; i++)
    {
        pdescriptor = (descriptor_t *) &rx_descriptors[channel][i];
        pdescriptor->buf_p = (uint8_t *) &(ether_buffers[channel].buffer[i][0]);
        pdescriptor->bufsize = ETHER_CFG_BUFSIZE;
        pdescriptor->size = 0;
        pdescriptor->status = RACT;
        pdescriptor->next = (descriptor_t *) &rx_descriptors[channel][(i + 1)];
    }

    /* The last descriptor points back to the start */
    pdescriptor->status |= RDLE;
    pdescriptor->next = (descriptor_t *) &rx_descriptors[channel][0];

    /* Initialize application receive descriptor pointer */
    papp_rx_desc[channel] = (descriptor_t *) &rx_descriptors[channel][0];

    /* Initialize the transmit descriptors */
    /* WAIT_LOOP */
    for (i = 0; i < ETHER_CFG_EMAC_TX_DESCRIPTORS; i++)
    {
        pdescriptor = (descriptor_t *) &tx_descriptors[channel][i];
        pdescriptor->buf_p = (uint8_t *) &(ether_buffers[channel].buffer[(ETHER_CFG_EMAC_RX_DESCRIPTORS + i)][0]);
        pdescriptor->bufsize = 1; /* Set a value equal to or greater than 1. (reference to UMH)
         When transmitting data, the value of size is set to the function argument
         R_ETHER_Write_ZC2_SetBuf. */
        pdescriptor->size = 0; /* Reserved : The write value should be 0. (reference to UMH) */
        pdescriptor->status = 0;
        pdescriptor->next = (descriptor_t *) &(tx_descriptors[channel][(i + 1)]);
    }

    /* The last descriptor points back to the start */
    pdescriptor->status |= TDLE;
    pdescriptor->next = (descriptor_t *) &tx_descriptors[channel][0];

    /* Initialize application transmit descriptor pointer */
    papp_tx_desc[channel] = (descriptor_t *) &tx_descriptors[channel][0];
} /* End of function ether_init_descriptors() */

/***********************************************************************************************************************
 * Function Name: ether_config_ethernet
 * Description  : Configure the Ethernet Controller (EtherC) and the Ethernet
 *                Direct Memory Access controller (EDMAC).
 * Arguments    : channel -
 *                    ETHERC channel number
 *                mode - 
 *                   The operational mode is specified. 
 *                   NO_USE_MAGIC_PACKET_DETECT (0) - Communicate mode usually
 *                   USE_MAGIC_PACKET_DETECT    (1) - Magic packet detection mode
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_config_ethernet (uint32_t channel, const uint8_t mode)
{
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;

    /* Magic packet detection mode */
    if (USE_MAGIC_PACKET_DETECT == mode)
    {
#if (ETHER_CFG_USE_LINKSTA == 1)
        petherc_adr->ECSIPR.LONG = 0x00000006;
#elif (ETHER_CFG_USE_LINKSTA == 0)
        petherc_adr->ECSIPR.LONG = 0x00000002;
#endif
        pedmac_adr->EESIPR.LONG = 0x00400000;
    }

    /* Normal mode */
    else
    {
#if (ETHER_CFG_USE_LINKSTA == 1)
        /* LINK Signal Change Interrupt Enable */
        petherc_adr->ECSR.BIT.LCHNG = 1;
        petherc_adr->ECSIPR.BIT.LCHNGIP = 1;
#endif
        pedmac_adr->EESIPR.BIT.ECIIP = 1;

        /* Frame receive interrupt and frame transmit end interrupt */
        pedmac_adr->EESIPR.BIT.FRIP = 1; /* Enable the frame receive interrupt. */
        pedmac_adr->EESIPR.BIT.TCIP = 1; /* Enable the frame transmit end interrupt. */
    }

    /* Ethernet length 1514bytes + CRC and intergap is 96-bit time */
    petherc_adr->RFLR.LONG = 1518;
    petherc_adr->IPGR.LONG = 0x00000014;

    /* Continuous reception number of Broadcast frame */
    petherc_adr->BCFRR.LONG = bc_filter_count[channel];

#ifdef __LIT
    /* Set little endian mode */
    /* Ethernet length 1514bytes + CRC and intergap is 96-bit time */
    pedmac_adr->EDMR.BIT.DE = 1;
#endif
    /* __LIT */

    /* Initialize Rx descriptor list address */
    /* Casting the pointer to a uint32_t type is valid because the Renesas Compiler uses 4 bytes per pointer. */
    pedmac_adr->RDLAR = (void*) papp_rx_desc[channel];

    /* Initialize Tx descriptor list address */
    /* Casting the pointer to a uint32_t type is valid because the Renesas Compiler uses 4 bytes per pointer. */
    pedmac_adr->TDLAR = (void*) papp_tx_desc[channel];

    if (ETHER_MC_FILTER_ON == mc_filter_flag[channel])
    {
        /* Reflect the EESR.RMAF bit status in the RD0.RFS bit in the receive descriptor */
        pedmac_adr->TRSCER.LONG = 0x00000000;
    }
    else
    {
        /* Don't reflect the EESR.RMAF bit status in the RD0.RFS bit in the receive descriptor */
        pedmac_adr->TRSCER.LONG = 0x00000080;
    }

    /* Threshold of Tx_FIFO */
    /* To prevent a transmit underflow, setting the initial value (store and forward modes) is recommended. */
    pedmac_adr->TFTR.LONG = 0x00000000;

#if (defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX66N))
    /* transmit fifo & receive fifo is 2048 bytes */
    pedmac_adr->FDR.LONG = 0x00000707;
#elif (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N))
    /* transmit fifo is 2048 bytes, receive fifo is 4096 bytes */
    pedmac_adr->FDR.LONG = 0x0000070F;
#endif

    /*  Configure receiving method
     b0      RNR - Receive Request Bit Reset - Continuous reception of multiple frames is possible.
     b31:b1  Reserved set to 0
     */
    pedmac_adr->RMCR.LONG = 0x00000001;

    /* Insert receive data padding  */
    pedmac_adr->RPADIR.BIT.PADR = padding_insert_position[channel];
    pedmac_adr->RPADIR.BIT.PADS = padding_insert_size[channel];

} /* End of function ether_config_ethernet() */

/***********************************************************************************************************************
 * Function Name: ether_pause_resolution
 * Description  : Determines PAUSE frame generation and handling. Uses
 *                the resolution Table 28B-3 of IEEE 802.3-2008.
 * Arguments    : local_ability -
 *                    local PAUSE capability (2 least significant bits)
 *                partner_ability -
 *                    link partner PAUSE capability (2 least significant bits)
 *                *ptx_pause -
 *                    pointer to location to store the result of the table lookup for transmit
 *                    PAUSE. 1 is enable, 0 is disable.
 *                *prx_pause -
 *                    pointer to location to store the result of the table lookup for receive
 *                    PAUSE. 1 is enable, 0 is disable.
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_pause_resolution (uint16_t local_ability, uint16_t partner_ability, uint16_t *ptx_pause,
        uint16_t *prx_pause)
{
    uint32_t i;
    uint32_t ability_compare;

    /*
     * Arrange the bits so that they correspond to the Table 28B-3
     * of the IEEE 802.3 values.
     */
    ability_compare = (uint32_t) (((local_ability & LINK_RES_ABILITY_MASK) << LINK_RES_LOCAL_ABILITY_BITSHIFT)
            | (partner_ability & LINK_RES_ABILITY_MASK));

    /* Walk through the look up table */
    /* WAIT_LOOP */
    for (i = 0; i < PAUSE_TABLE_ENTRIES; i++)
    {
        if ((ability_compare & pause_resolution[i].mask) == pause_resolution[i].value)
        {
            (*ptx_pause) = pause_resolution[i].transmit;
            (*prx_pause) = pause_resolution[i].receive;
            return;
        }
    }
} /* End of function ether_pause_resolution() */

/***********************************************************************************************************************
 * Function Name: ether_configure_mac
 * Description  : Software reset is executed, and ETHERC and EDMAC are configured. 
 * Arguments    : channel -
 *                    ETHERC channel number
 *                mac_addr -
 *                    The MAC address of ETHERC
 *                mode -
 *                    The operational mode is specified. 
 *                    NO_USE_MAGIC_PACKET_DETECT (0) - Communicate mode usually
 *                    USE_MAGIC_PACKET_DETECT    (1) - Magic packet detection mode
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_configure_mac (uint32_t channel, const uint8_t mac_addr[], const uint8_t mode)
{
    uint32_t mac_h;
    uint32_t mac_l;
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;

    /* Software reset */
    ether_reset_mac(channel);

    /* Set MAC address */
    mac_h = (((((uint32_t) mac_addr[0] << 24) | ((uint32_t) mac_addr[1] << 16)) | ((uint32_t) mac_addr[2] << 8))
            | (uint32_t) mac_addr[3]);

    mac_l = (((uint32_t) mac_addr[4] << 8) | (uint32_t) mac_addr[5]);

    petherc_adr->MAHR = mac_h;
    petherc_adr->MALR.LONG = mac_l;

    /* Initialize receive and transmit descriptors */
    ether_init_descriptors(channel);

    /* Perform reset of hardware interface configuration */
    ether_config_ethernet(channel, mode);

} /* End of function ether_configure_mac() */

#if (ETHER_CFG_NON_BLOCKING == 0)
/***********************************************************************************************************************
 * Function Name: ether_do_link
 * Description  : Determines the partner PHY capability through
 *                auto-negotiation process. The link abilities
 *                are handled to determine duplex, speed and flow
 *                control (PAUSE frames).
 * Arguments    : channel -
 *                    ETHERC channel number
 *                mode - 
 *                    The operational mode is specified. 
 *                    NO_USE_MAGIC_PACKET_DETECT (0) - Communicate mode usually
 *                    USE_MAGIC_PACKET_DETECT    (1) - Magic packet detection mode
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_CHAN -
 *                    Nonexistent channel number
 *                ETHER_ERR_OTHER -
 *                    Auto-negotiation of PHY-LSI is not completed or result of Auto-negotiation is abnormal.
 ***********************************************************************************************************************/
static ether_return_t ether_do_link (uint32_t channel, const uint8_t mode)
{
    ether_return_t ret;
    uint16_t link_speed_duplex = 0;
    uint16_t local_pause_bits = 0;
    uint16_t partner_pause_bits = 0;
    uint16_t transmit_pause_set = 0;
    uint16_t receive_pause_set = 0;
    uint16_t full_duplex = 0;
    uint16_t link_result = 0;
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;

    /* Set the link status */
    link_result = phy_set_autonegotiate(channel, &link_speed_duplex, &local_pause_bits, &partner_pause_bits);

    if (R_PHY_OK == link_result)
    {
        switch (link_speed_duplex)
        {
            /* Half duplex link */
            case PHY_LINK_100H :
                petherc_adr->ECMR.BIT.DM = 0;
                petherc_adr->ECMR.BIT.RTM = 1;
                ret = ETHER_SUCCESS;
            break;

            case PHY_LINK_10H :
                petherc_adr->ECMR.BIT.DM = 0;
                petherc_adr->ECMR.BIT.RTM = 0;
                ret = ETHER_SUCCESS;
            break;

                /* Full duplex link */
            case PHY_LINK_100F :
                petherc_adr->ECMR.BIT.DM = 1;
                petherc_adr->ECMR.BIT.RTM = 1;
                full_duplex = 1;
                ret = ETHER_SUCCESS;
            break;

            case PHY_LINK_10F :
                petherc_adr->ECMR.BIT.DM = 1;
                petherc_adr->ECMR.BIT.RTM = 0;
                full_duplex = 1;
                ret = ETHER_SUCCESS;
            break;

            default :
                ret = ETHER_ERR_OTHER;
            break;
        }

        /* At the communicate mode usually */
        if (ETHER_SUCCESS == ret)
        {
            if (NO_USE_MAGIC_PACKET_DETECT == mode)
            {

                /* When pause frame is used */
                if ((full_duplex) && (ETHER_FLAG_ON == pause_frame_enable[channel]))
                {
                    /* Set automatic PAUSE for 512 bit-time */
                    petherc_adr->APR.LONG = 0x0000FFFF;

                    /* Set unlimited retransmit of PAUSE frames */
                    petherc_adr->TPAUSER.LONG = 0;

                    /* PAUSE flow control FIFO settings. */
                    pedmac_adr->FCFTR.LONG = 0x00000000;

                    /* Control of a PAUSE frame whose TIME parameter value is 0 is enabled. */
                    petherc_adr->ECMR.BIT.ZPF = 1;

                    /**
                     * Enable PAUSE for full duplex link depending on
                     * the pause resolution results
                     */
                    ether_pause_resolution(local_pause_bits, partner_pause_bits, &transmit_pause_set,
                            &receive_pause_set);

                    if (XMIT_PAUSE_ON == transmit_pause_set)
                    {
                        /* Enable automatic PAUSE frame transmission */
                        petherc_adr->ECMR.BIT.TXF = 1;
                    }
                    else
                    {
                        /* Disable automatic PAUSE frame transmission */
                        petherc_adr->ECMR.BIT.TXF = 0;
                    }

                    if (RECV_PAUSE_ON == receive_pause_set)
                    {
                        /* Enable reception of PAUSE frames */
                        petherc_adr->ECMR.BIT.RXF = 1;
                    }
                    else
                    {
                        /* Disable reception of PAUSE frames */
                        petherc_adr->ECMR.BIT.RXF = 0;
                    }
                }

                /* When pause frame is not used */
                else
                {
                    /* Disable PAUSE for half duplex link */
                    petherc_adr->ECMR.BIT.TXF = 0;
                    petherc_adr->ECMR.BIT.RXF = 0;
                }

                /* Set the promiscuous mode bit */
                petherc_adr->ECMR.BIT.PRM = promiscuous_mode[channel];

                /* Enable receive and transmit. */
                petherc_adr->ECMR.BIT.RE = 1;
                petherc_adr->ECMR.BIT.TE = 1;

                /* Enable EDMAC receive */
                pedmac_adr->EDRRR.LONG = 0x1;
            }

            /* At the magic packet detection mode */
            else
            {
                /* The magic packet detection is permitted. */
                petherc_adr->ECMR.BIT.MPDE = 1;

                /* Because data is not transmitted for the magic packet detection waiting, 
                 only the reception is permitted. */
                petherc_adr->ECMR.BIT.RE = 1;

                /*
                 * The reception function of EDMAC keep invalidity 
                 * because the receive data don't need to be read when the magic packet detection mode.
                 */
            }
        }
    }
    else
    {
        ret = ETHER_ERR_OTHER;
    }

    return ret;
} /* End of function ether_do_link() */
#endif

/***********************************************************************************************************************
 * Function Name: ether_set_callback
 * Description  : Set the callback function
 * Arguments    : control -
 *                    Callback function pointer. 
 *                    If you would rather poll for finish then please input FIT_NO_FUNC for this argument.
 * Return Value : ETHER_SUCCESS
 *                    Processing completed successfully
 ***********************************************************************************************************************/
static ether_return_t ether_set_callback (ether_param_t const control)
{
    void (*pcb_func) (void *);

    pcb_func = control.ether_callback.pcb_func;

    /* Check callback function pointer, of whether or NULL FIT_NO_FUNC */
    if ((NULL != pcb_func) && (FIT_NO_FUNC != pcb_func))
    {
        cb_func.pcb_func = pcb_func; /* Set the callback function */
    }

    return ETHER_SUCCESS;
} /* End of function ether_set_callback() */

/***********************************************************************************************************************
 * Function Name: ether_set_promiscuous_mode
 * Description  : Set promiscuous mode bit
 * Arguments    : control -
 *                    Promiscuous mode bit
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_CHAN - 
 *                    Nonexistent channel number
 ***********************************************************************************************************************/
static ether_return_t ether_set_promiscuous_mode (ether_param_t const control)
{
    ether_promiscuous_t * p_arg;
    uint32_t channel;

    p_arg = control.p_ether_promiscuous;
    channel = p_arg->channel;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_CHAN;
    }
    else
    {
        promiscuous_mode[channel] = p_arg->bit;
    }
    return ETHER_SUCCESS;
} /* End of function ether_set_promiscuous_mode() */

/***********************************************************************************************************************
 * Function Name: ether_set_int_handler
 * Description  : Set the interrupt handler
 * Arguments    : control -
 *                    Interrupt handler pointer.
 *                    If you would rather poll for finish then please input FIT_NO_FUNC for this argument.
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 ***********************************************************************************************************************/
static ether_return_t ether_set_int_handler (ether_param_t const control)
{
    void (*pcb_int_hnd) (void *);
    ether_return_t ret = ETHER_ERR_INVALID_ARG;

    pcb_int_hnd = control.ether_int_hnd.pcb_int_hnd;
    if (NULL != pcb_int_hnd)
    {
        cb_func.pcb_int_hnd = pcb_int_hnd;
        ret = ETHER_SUCCESS;
    }
    return ret;
} /* End of function ether_set_int_handler() */

/***********************************************************************************************************************
 * Function Name: ether_power_on
 * Description  : Turns on power to a ETHER channel.
 * Arguments    : control -
 *                    Ethernet channel number
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_CHAN -
 *                    Nonexistent channel number
 *                ETHER_ERR_CHAN_OPEN -
 *                    Indicates the Ethernet cannot be opened because it is being used by another application
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 ***********************************************************************************************************************/
static ether_return_t ether_power_on (ether_param_t const control)
{
    uint32_t channel;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    ether_return_t ret;

    ret = ETHER_ERR_INVALID_ARG;
    channel = (uint32_t) control.channel;

    if (ETHER_CHANNEL_MAX > channel)
    {
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;

        /* Set port connect */
        ether_set_phy_mode(pether_ch[phy_access].port_connect);

        /* ETHERC/EDMAC Power on */
        ret = power_on_control(channel);
    }
    return ret;
} /* End of function ether_power_on() */

/***********************************************************************************************************************
 * Function Name: ether_power_off
 * Description  : Turns on power to a ETHER channel.
 * Arguments    : control -
 *                    Ethernet channel number
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 ***********************************************************************************************************************/
static ether_return_t ether_power_off (ether_param_t const control)
{
    uint32_t channel;
    ether_return_t ret;

    ret = ETHER_ERR_INVALID_ARG;
    channel = (uint32_t) control.channel;

    if (ETHER_CHANNEL_MAX > channel)
    {
        /* ETHERC/EDMAC Power off */
        power_off_control(channel);
        ret = ETHER_SUCCESS;
    }
    return ret;
} /* End of function ether_power_off() */

/***********************************************************************************************************************
 * Function Name: power_on
 * Description  : Turns on power to a ETHER channel.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_CHAN -
 *                    Nonexistent channel number
 *                ETHER_ERR_CHAN_OPEN -
 *                    Indicates the Ethernet cannot be opened because it is being used by another application
 ***********************************************************************************************************************/
static ether_return_t power_on (uint32_t channel)
{
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    bsp_int_ctrl_t     int_ctrl;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        /* Should never get here. Valid channel number is checked above. */
        return ETHER_ERR_INVALID_CHAN;
    }

#if (ETHER_CHANNEL_MAX == 1)
    /* Attempt to obtain lock so we know we have exclusive access to ETHER channel. */
    if (false == R_BSP_HardwareLock((mcu_lock_t) (BSP_LOCK_ETHERC0)))
    {
        return ETHER_ERR_CHAN_OPEN;
    }

    /* Attempt to obtain lock so we know we have exclusive access to EDMAC channel. */
    if (false == R_BSP_HardwareLock((mcu_lock_t) (BSP_LOCK_EDMAC0)))
    {
        /* Release reservation on peripheral. */
        R_BSP_HardwareUnlock((mcu_lock_t) (BSP_LOCK_ETHERC0));
        return ETHER_ERR_CHAN_OPEN;
    }
#elif (ETHER_CHANNEL_MAX == 2)
    /* Attempt to obtain lock so we know we have exclusive access to ETHER channel. */
    if (false == R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_ETHERC0 + channel)))
    {
        return ETHER_ERR_CHAN_OPEN;
    }

    /* Attempt to obtain lock so we know we have exclusive access to EDMAC channel. */
    if (false == R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_EDMAC0 + channel)))
    {
        /* Release reservation on peripheral. */
        R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_ETHERC0 + channel));
        return ETHER_ERR_CHAN_OPEN;
    }
#endif

    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
#endif

    /* Enable selected ETHERC/EDMAC Channel. */
#if (ETHER_CHANNEL_MAX == 1)
    if (ETHER_CHANNEL_0 == channel)
    {
        MSTP(EDMAC0) = 0;
    }
#elif (ETHER_CHANNEL_MAX == 2)
    if (ETHER_CHANNEL_0 == channel)
    {
        MSTP(EDMAC0) = 0;
    }
    else
    {
        MSTP(EDMAC1) = 0;
    }
#endif

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
#endif

    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    return ETHER_SUCCESS;
} /* End of function power_on() */

/***********************************************************************************************************************
 * Function Name: power_off
 * Description  : Turns off power to a ETHER channel.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void power_off (uint32_t channel)
{
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    bsp_int_ctrl_t    int_ctrl;
#endif

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        /* Should never get here. Valid channel number is checked above. */
        return;
    }

    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
#endif

    /* Disable selected ETHERC/EDMAC Channel. */
#if (ETHER_CHANNEL_MAX == 1)
    if (ETHER_CHANNEL_0 == channel)
    {
        MSTP(EDMAC0) = 1;
    }
#elif (ETHER_CHANNEL_MAX == 2)
    if (ETHER_CHANNEL_0 == channel)
    {
        MSTP(EDMAC0) = 1;
    }
    else
    {
        MSTP(EDMAC1) = 1;
    }
#endif

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
#endif

    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* Release reservation on peripheral. */
#if (ETHER_CHANNEL_MAX == 1)
    R_BSP_HardwareUnlock((mcu_lock_t) (BSP_LOCK_ETHERC0));
    R_BSP_HardwareUnlock((mcu_lock_t) (BSP_LOCK_EDMAC0));
#elif (ETHER_CHANNEL_MAX == 2)
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_ETHERC0 + channel));
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_EDMAC0 + channel));
#endif

} /* End of function power_off() */

/***********************************************************************************************************************
 * Function Name: ether_set_multicastframe_filter
 * Description  : set multicast frame filtering function.
 * Arguments    : control -
 *                    Ethernet channel number and Multicast Frame filter enable switch
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 *                ETHER_ERR_RECV_ENABLE -
 *                    Receive function is enable.
 ***********************************************************************************************************************/
static ether_return_t ether_set_multicastframe_filter (ether_param_t const control)
{
    ether_multicast_t * p_arg;
    uint32_t channel;
    ether_mc_filter_t flag;
    ether_return_t ret;
    uint32_t phy_access;

    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    const ether_control_t * pether_ch;

    ret = ETHER_ERR_INVALID_ARG;
    p_arg = control.p_ether_multicast;
    channel = p_arg->channel;
    flag = p_arg->flag;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ret;
    }
    if ((ETHER_MC_FILTER_ON != flag) && (ETHER_MC_FILTER_OFF != flag))
    {
        return ret;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;

    /* Check receive function is enable in ETHERC */
    if (0 == petherc_adr->ECMR.BIT.RE)
    {
        if (ETHER_MC_FILTER_ON == flag)
        {
            mc_filter_flag[channel] = ETHER_MC_FILTER_ON;
        }
        else
        {
            mc_filter_flag[channel] = ETHER_MC_FILTER_OFF;
        }

        ret = ETHER_SUCCESS;
    }
    else
    {
        ret = ETHER_ERR_RECV_ENABLE;
    }

    return ret;
} /* End of function ether_set_multicastframe_filter() */

/***********************************************************************************************************************
 * Function Name: ether_set_broadcastframe_filter
 * Description  : set broadcast frame filtering function.
 * Arguments    : control -
 *                    ETHERC channel number and receive count for continuous Broadcast Frame
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 *                ETHER_ERR_RECV_ENABLE -
 *                    Receive function is enable.
 ***********************************************************************************************************************/
static ether_return_t ether_set_broadcastframe_filter (ether_param_t const control)
{
    ether_broadcast_t * p_arg;
    uint32_t channel;
    uint32_t counter;
    ether_return_t ret;
    uint32_t phy_access;

    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    const ether_control_t * pether_ch;

    ret = ETHER_ERR_INVALID_ARG;
    p_arg = control.p_ether_broadcast;
    channel = p_arg->channel;
    counter = p_arg->counter;

    /* Check argument */
    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ret;
    }
    if (0 != (counter & 0x0FFFF0000))
    {
        return ret;
    }

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;

    /* Check receive function is enable in ETHERC */
    if (0 == petherc_adr->ECMR.BIT.RE)
    {
        bc_filter_count[channel] = counter;
        ret = ETHER_SUCCESS;
    }
    else
    {
        ret = ETHER_ERR_RECV_ENABLE;
    }

    return ret;
} /* End of function ether_set_broadcastframe_filter() */

/***********************************************************************************************************************
 * Function Name: power_on_control
 * Description  : Powers on the channel if the ETHEC channel used and the PHY access channel are different, or if the 
 *                PHY access channel is powered off.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_CHAN -
 *                    Nonexistent channel number
 *                ETHER_ERR_CHAN_OPEN -
 *                    Indicates the Ethernet cannot be opened because it is being used by another application
 *              : ETHER_ERR_OTHER -
 *                    
 ***********************************************************************************************************************/
static ether_return_t power_on_control (uint32_t channel)
{
#if (ETHER_CHANNEL_MAX == 1)
    ether_return_t ret = ETHER_ERR_OTHER;

    etherc_edmac_power_cont[ether_phy_access[channel]][channel] = ETHER_MODULE_USE;

    ret = power_on(channel);

    return ret;

#elif ETHER_CHANNEL_MAX == 2
    ether_return_t ret = ETHER_ERR_OTHER;

    etherc_edmac_power_cont[ether_phy_access[channel]] [channel] = ETHER_MODULE_USE;

    if (channel != ether_phy_access[channel])
    {
        ret = power_on(channel);
        if (ETHER_SUCCESS != ret)
        {
            return ret;
        }
    }

    if (!(( ETHER_MODULE_USE == etherc_edmac_power_cont[ether_phy_access[channel]] [ETHER_CHANNEL_0] ) &&
                    ( ETHER_MODULE_USE == etherc_edmac_power_cont[ether_phy_access[channel]] [ETHER_CHANNEL_1] )))
    {
        ret = power_on(ether_phy_access[channel]);
    }

    return ret;

#endif
} /* End of function power_on_control() */

/***********************************************************************************************************************
 * Function Name: power_off_control
 * Description  : Powers off the channel if the ETHEC channel used and the PHY access channel are different, or if the 
 *                PHY access channel is powered off.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void power_off_control (uint32_t channel)
{
#if (ETHER_CHANNEL_MAX == 1)
    etherc_edmac_power_cont[ether_phy_access[channel]][channel] = ETEHR_MODULE_NOT_USE;

    power_off(channel);

#elif (ETHER_CHANNEL_MAX == 2)
    etherc_edmac_power_cont[ether_phy_access[channel]] [channel] = ETEHR_MODULE_NOT_USE;

    if (channel != ether_phy_access[channel])
    {
        power_off(channel);
    }

    if (( ETEHR_MODULE_NOT_USE == etherc_edmac_power_cont[ether_phy_access[channel]] [ETHER_CHANNEL_0] ) &&
            ( ETEHR_MODULE_NOT_USE == etherc_edmac_power_cont[ether_phy_access[channel]] [ETHER_CHANNEL_1] ))
    {
        power_off(ether_phy_access[channel]);
    }
#endif
} /* End of function power_off_control() */

/***********************************************************************************************************************
 * Function Name: check_mpde_bit
 * Description  : 
 * Arguments    : none
 * Return Value : 1: Magic Packet detection is enabled.
 *                0: Magic Packet detection is disabled.
 ***********************************************************************************************************************/
static uint8_t check_mpde_bit (void)
{
#if (ETHER_CHANNEL_MAX == 1)
    /* The MPDE bit can be referred to only when ETHERC operates. */
    if ((ETHER_MODULE_USE == etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_0][ETHER_CHANNEL_0])
            && (1 == ETHERC0.ECMR.BIT.MPDE))
    {
        return 1;
    }
    else
    {
        return 0;
    }
#elif (ETHER_CHANNEL_MAX == 2)
    /* The MPDE bit can be referred to only when ETHERC0 operates. */
    if ( ( ((ETHER_MODULE_USE == etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_0] [ETHER_CHANNEL_0])
                            || (ETHER_MODULE_USE == etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_1] [ETHER_CHANNEL_0]))
                    && (1 == ETHERC0.ECMR.BIT.MPDE)
            )

            /* The MPDE bit can be referred to only when ETHERC1 operates. */
            || ( ((ETHER_MODULE_USE == etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_0] [ETHER_CHANNEL_1])
                            || (ETHER_MODULE_USE == etherc_edmac_power_cont[ETHER_PHY_ACCESS_CHANNEL_1] [ETHER_CHANNEL_1]))
                    && (1 == ETHERC1.ECMR.BIT.MPDE)
            )
    )
    {
        return 1;
    }
    else
    {
        return 0;
    }
#endif
} /* End of function check_mpde_bit() */

#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX65N) || defined(BSP_MCU_RX72M) || \
       defined(BSP_MCU_RX72N) || defined(BSP_MCU_RX66N))
/***********************************************************************************************************************
 * Function Name: ether_eint0
 * Description  : EINT0 interrupt processing (A callback function to be called from r_bsp.)
 * Arguments    : pparam -
 *                    unused
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_eint0 (void * pparam)
{
    volatile uint32_t dummy;

    ether_int_common(ETHER_CHANNEL_0);

    dummy = (uint32_t) pparam;
} /* End of function ether_eint0() */
#endif

#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N))
/***********************************************************************************************************************
 * Function Name: ether_eint1
 * Description  : EINT1 interrupt processing (A callback function to be called from r_bsp.)
 * Arguments    : pparam -
 *                    unused
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_eint1(void * pparam)
{
    volatile uint32_t dummy;

    ether_int_common(ETHER_CHANNEL_1);

    dummy = (uint32_t)pparam;
} /* End of function ether_eint1() */
#endif

/***********************************************************************************************************************
 * Function Name: ether_int_common
 * Description  : Interrupt handler for Ethernet receive and transmit interrupts.
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void ether_int_common (uint32_t channel)
{
    uint32_t status_ecsr;
    uint32_t status_eesr;
    volatile struct st_etherc R_BSP_EVENACCESS_SFR * petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR * pedmac_adr;
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    ether_cb_arg_t cb_arg;

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;

    status_ecsr = petherc_adr->ECSR.LONG;
    status_eesr = pedmac_adr->EESR.LONG;

    /* Callback : Interrupt handler */
    if ((NULL != cb_func.pcb_int_hnd) && (FIT_NO_FUNC != cb_func.pcb_int_hnd))
    {
        cb_arg.channel = channel;
        cb_arg.status_ecsr = status_ecsr;
        cb_arg.status_eesr = status_eesr;
        (*cb_func.pcb_int_hnd)((void *) &cb_arg);
    }

    /* When the ETHERC status interrupt is generated */
    if (status_eesr & EMAC_ECI_INT)
    {
#if (ETHER_CFG_USE_LINKSTA == 1)
        /* When the link signal change interrupt is generated */
        if (EMAC_LCHNG_INT == (status_ecsr & EMAC_LCHNG_INT))
        {
            /* The state of the link signal is confirmed and Link Up/Down is judged. */
            /* When becoming Link up */
            if (ETHER_CFG_LINK_PRESENT == petherc_adr->PSR.BIT.LMON)
            {
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;
            }

            /* When Link becomes down */
            else
            {
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_OFF;
            }
        }
#endif

        /* When the Magic Packet detection interrupt is generated */
        if (EMAC_MPD_INT == (status_ecsr & EMAC_MPD_INT))
        {
            mpd_flag[channel] = ETHER_FLAG_ON;
        }

        /*
         * Because each bit of the ECSR register is cleared when one is written, 
         * the value read from the register is written and the bit is cleared. 
         */
        /* Clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
        petherc_adr->ECSR.LONG = status_ecsr;
    }

    /*
     * Because each bit of the EESR register is cleared when one is written, 
     * the value read from the register is written and the bit is cleared. 
     */
    pedmac_adr->EESR.LONG = status_eesr; /* Clear EDMAC status bits */

    /* Whether it is a necessary code is confirmed. */

} /* End of function ether_int_common() */

/***********************************************************************************************************************
 * Function Name: ether_receive_data_padding
 * Description  : Save the receive data padding information.
 * Arguments    : control -
 *                    Receive data padding information
 * Return Value : ETHER_SUCCESS -
 *                    Processing completed successfully
 *                ETHER_ERR_INVALID_ARG -
 *                    Invalid argument
 ***********************************************************************************************************************/
static ether_return_t ether_receive_data_padding (ether_param_t const control)
{
    ether_recv_padding_t *  p_arg;
    uint32_t                channel;
    uint8_t                 position;
    uint8_t                 size;

    p_arg      = control.padding_param;
    channel     = p_arg->channel;
    position    = p_arg->position;
    size        = p_arg->size;

    if (ETHER_CHANNEL_MAX <= channel)
    {
        return ETHER_ERR_INVALID_ARG;    // ETHERC channel invalid
    }

    if (INSERT_POSITION_MAX < position )
    {
        return ETHER_ERR_INVALID_ARG;    // Insert position out the range
    }

    if (INSERT_SIZE_MAX < size )
    {
        return ETHER_ERR_INVALID_ARG;    // Insert size beyond the MAX size
    }

    /* save insert information */
    padding_insert_position[channel] = position;
    padding_insert_size[channel] = size;

    return ETHER_SUCCESS;
} /* End of function ether_receive_data_padding() */

#if (ETHER_CFG_NON_BLOCKING == 1)
/***********************************************************************************************************************
 * Function Name: ether_set_pmgi_callback
 * Description  : Set the callback function
 * Arguments    : control -
 *                    Callback function pointer.
 *                    If you would rather poll for finish then please input FIT_NO_FUNC for this argument.
 * Return Value : ETHER_SUCCESS
 *                    Processing completed successfully
 ***********************************************************************************************************************/
static ether_return_t ether_set_pmgi_callback(ether_param_t const control)
{
    void (*pcb_func) (void *);

    pcb_func = control.pmgi_callback.pcb_pmgi_hnd;

    /* Check callback function pointer, of whether or NULL FIT_NO_FUNC */
    if ((NULL != pcb_func) && (FIT_NO_FUNC != pcb_func))
    {
        cb_func.pcb_pmgi_hnd = pcb_func; /* Set the callback function */
    }

    return ETHER_SUCCESS;
} /* End of function ether_set_pmgi_callback() */

/***********************************************************************************************************************
 * Function Name: get_pmgi_channel
 * Description  : Calculate the PMGI channel number from used ETHERC channel
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : Used PMGI channel number
 ***********************************************************************************************************************/
static uint16_t get_pmgi_channel(uint32_t channel)
{
    uint16_t pmgi_channel;

    if (ETHER_CHANNEL_0 == channel)
    {
        pmgi_channel = ETHER_CFG_CH0_PHY_ACCESS;
    }
    else
    {
        pmgi_channel = ETHER_CFG_CH1_PHY_ACCESS;
    }

    return pmgi_channel;
} /* End of function get_pmgi_channel() */

/***********************************************************************************************************************
 * Function Name: set_ether_channel
 * Description  : Save the ETHERC channel number
 * Arguments    : channel -
 *                    ETHERC channel number
 * Return Value : None
 ***********************************************************************************************************************/
static void set_ether_channel(uint32_t channel)
{
    if (ETHER_CHANNEL_0 == channel)
    {
        g_phy_current_param[ETHER_CFG_CH0_PHY_ACCESS].ether_channel = channel;
    }
    else
    {
        g_phy_current_param[ETHER_CFG_CH1_PHY_ACCESS].ether_channel = channel;
    }
} /* End of function set_ether_channel() */

#if (ETHER_CFG_CH0_PHY_ACCESS == 0) || (ETHER_CFG_CH1_PHY_ACCESS == 0)
/******************************************************************************
* Function Name:    ether_pmgi0i_isr
* Description  :    PMGI0 complete ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_PRAGMA_STATIC_INTERRUPT(ether_pmgi0i_isr, VECT_PMGI0_PMGI0I)
R_BSP_ATTRIB_STATIC_INTERRUPT void ether_pmgi0i_isr(void)
{
    pmgi_cb_arg_t           cb_arg;
    pmgi_mode_t             mode;
    uint32_t                ether_channel;
    pmgi_step_t             step;
    pmgi_event_t            event;

    mode            = g_phy_current_param[PMGI_CHANNEL_0].mode;
    step            = g_phy_current_param[PMGI_CHANNEL_0].step;
    ether_channel   = g_phy_current_param[PMGI_CHANNEL_0].ether_channel;

    if (PmgiAccessFun_tbl[mode][step].p_func(ether_channel) != ETHER_SUCCESS)
    {
        g_phy_current_param[PMGI_CHANNEL_0].event = PMGI_ERROR;
    }

    event = g_phy_current_param[PMGI_CHANNEL_0].event;
    if ((PMGI_COMPLETE == event) || (PMGI_ERROR == event))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[PMGI_CHANNEL_0].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            if (PMGI_COMPLETE == event)
            {
                cb_arg.event = PMGI_COMPLETE;
                cb_arg.mode = g_phy_current_param[PMGI_CHANNEL_0].mode;
            }
            else
            {
                cb_arg.event = PMGI_ERROR;
                cb_arg.mode = g_phy_current_param[PMGI_CHANNEL_0].mode;
            }
            cb_arg.channel = ether_channel;
            cb_arg.reg_data = g_phy_current_param[PMGI_CHANNEL_0].read_data;
            /* set PMGI parameters to default */
            g_phy_current_param[PMGI_CHANNEL_0].event = PMGI_IDLE;
            g_phy_current_param[PMGI_CHANNEL_0].mode = OPEN_ZC2;
            g_phy_current_param[PMGI_CHANNEL_0].step = STEP0;
            pmgi_close(PMGI_CHANNEL_0);
            (*cb_func.pcb_pmgi_hnd)((void *)&cb_arg);
        }
    }
    else
    {

    }
} /* End of function ether_pmgi0i_isr() */
#endif /* (ETHER_CFG_CH0_PHY_ACCESS == 0) || (ETHER_CFG_CH1_PHY_ACCESS == 0) */

#if (PMGI_CHANNEL_MAX ==2)
#if (ETHER_CFG_CH0_PHY_ACCESS == 1) || (ETHER_CFG_CH1_PHY_ACCESS == 1)
/******************************************************************************
* Function Name:    ether_pmgi1i_isr
* Description  :    PMGI1 complete ISR.
*                   Each ISR calls a common function but passes its channel number.
* Arguments    :    N/A
* Return Value :    N/A
******************************************************************************/
R_BSP_PRAGMA_STATIC_INTERRUPT(ether_pmgi1i_isr, VECT_PMGI1_PMGI1I)
R_BSP_ATTRIB_STATIC_INTERRUPT void ether_pmgi1i_isr(void)
{
    pmgi_cb_arg_t           cb_arg;
    pmgi_mode_t             mode;
    uint32_t                ether_channel;
    pmgi_step_t             step;
    pmgi_event_t            event;

    mode            = g_phy_current_param[PMGI_CHANNEL_1].mode;
    step            = g_phy_current_param[PMGI_CHANNEL_1].step;
    ether_channel   = g_phy_current_param[PMGI_CHANNEL_1].ether_channel;

    if (PmgiAccessFun_tbl[mode][step].p_func(ether_channel) != ETHER_SUCCESS)
    {
        g_phy_current_param[PMGI_CHANNEL_1].event = PMGI_ERROR;
    }

    event = g_phy_current_param[PMGI_CHANNEL_1].event;
    if ((PMGI_COMPLETE == event) || (PMGI_ERROR == event))
    {
        R_BSP_SoftwareUnlock((BSP_CFG_USER_LOCKING_TYPE * )&g_phy_current_param[PMGI_CHANNEL_1].locked);
        if ((NULL != cb_func.pcb_pmgi_hnd) && (FIT_NO_FUNC != cb_func.pcb_pmgi_hnd))
        {
            if (PMGI_COMPLETE == event)
            {
                cb_arg.event = PMGI_COMPLETE;
                cb_arg.mode = g_phy_current_param[PMGI_CHANNEL_1].mode;
            }
            else
            {
                cb_arg.event = PMGI_ERROR;
                cb_arg.mode = g_phy_current_param[PMGI_CHANNEL_1].mode;
            }
            cb_arg.channel = ether_channel;
            cb_arg.reg_data = g_phy_current_param[PMGI_CHANNEL_1].read_data;
            /* set PMGI parameters to default */
            g_phy_current_param[PMGI_CHANNEL_1].event = PMGI_IDLE;
            g_phy_current_param[PMGI_CHANNEL_1].mode = OPEN_ZC2;
            g_phy_current_param[PMGI_CHANNEL_1].step = STEP0;
            pmgi_close(PMGI_CHANNEL_1);
            (*cb_func.pcb_pmgi_hnd)((void *)&cb_arg);
        }
    }
    else
    {

    }
} /* End of function ether_pmgi1i_isr() */
#endif /* (ETHER_CFG_CH0_PHY_ACCESS == 1) || (ETHER_CFG_CH1_PHY_ACCESS == 1) */
#endif

/******************************************************************************
* Function Name:    pmgi_open_zc2_step0
* Description  :    Processing after R_ETHER_Open_ZC2'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step0(uint32_t channel)
{
    ether_return_t  ret = ETHER_SUCCESS;
    uint16_t        pmgi_channel;
    uint16_t        reg = 0;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP1;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
    g_phy_current_param[pmgi_channel].reset_counter = 0;

    pmgi_access(channel, PHY_REG_CONTROL, reg, PMGI_READ);

    return ret;
} /* End of function pmgi_open_zc2_step0() */

/******************************************************************************
* Function Name:    pmgi_open_zc2_step1
* Description  :    Processing after R_ETHER_Open_ZC2'step1 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step1(uint32_t channel)
{
    ether_return_t          ret = ETHER_SUCCESS;
    uint16_t                pmgi_channel;
    uint16_t                reg;
    uint16_t                data = 0;

    pmgi_channel = get_pmgi_channel(channel);

    reg = pmgi_read_reg(pmgi_channel);
    if (reg & PHY_CONTROL_RESET)
    {
        pmgi_access(channel, PHY_REG_CONTROL, data, PMGI_READ);

        /* set the current parameter of phy */
        g_phy_current_param[pmgi_channel].reset_counter ++;
    }
    else
    {
#if ETHER_CFG_USE_PHY_KSZ8041NL == 1
        g_phy_current_param[pmgi_channel].step = STEP2;
        g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
        pmgi_access(channel, PHY_REG_PHY_CONTROL_1, data, PMGI_READ);

#elif ETHER_CFG_USE_PHY_KSZ8041NL == 0
        g_phy_current_param[pmgi_channel].step = STEP4;
        g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

        if (ETHER_FLAG_OFF == pause_frame_enable[channel])
        {
            local_advertise[channel] = ((((PHY_AN_ADVERTISEMENT_100F |
            PHY_AN_ADVERTISEMENT_100H) |
            PHY_AN_ADVERTISEMENT_10F) |
            PHY_AN_ADVERTISEMENT_10H) |
            PHY_AN_ADVERTISEMENT_SELECTOR);
        }

        /* When pause frame is used */
        else
        {
            local_advertise[channel] = ((((((PHY_AN_ADVERTISEMENT_ASM_DIR |
            PHY_AN_ADVERTISEMENT_PAUSE) |
            PHY_AN_ADVERTISEMENT_100F) |
            PHY_AN_ADVERTISEMENT_100H) |
            PHY_AN_ADVERTISEMENT_10F) |
            PHY_AN_ADVERTISEMENT_10H) |
            PHY_AN_ADVERTISEMENT_SELECTOR);
        }

        pmgi_access(channel, PHY_REG_AN_ADVERTISEMENT, local_advertise[channel], PMGI_WRITE);

#endif
    }

    if (g_phy_current_param[pmgi_channel].reset_counter >= ETHER_CFG_PHY_DELAY_RESET)
    {
        ret = ETHER_ERR_OTHER;
    }

    return ret;
} /* End of function pmgi_open_zc2_step1() */

#if ETHER_CFG_USE_PHY_KSZ8041NL == 1
/******************************************************************************
* Function Name:    pmgi_open_zc2_step2
* Description  :    Processing after R_ETHER_Open_ZC2'step2 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step2(uint32_t channel)
{
    ether_return_t          ret = ETHER_SUCCESS;
    uint16_t                pmgi_channel;
    uint16_t                reg;

    pmgi_channel = get_pmgi_channel(channel);

    reg = pmgi_read_reg(pmgi_channel);
    reg &= (~0x8000);
    reg |= 0x4000;

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP3;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_PHY_CONTROL_1, reg, PMGI_WRITE);

    return ret;
} /* End of function pmgi_open_zc2_step2() */

/******************************************************************************
* Function Name:    pmgi_open_zc2_step3
* Description  :    Processing after R_ETHER_Open_ZC2'step3 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step3(uint32_t channel)
{
    ether_return_t              ret = ETHER_SUCCESS;
    uint16_t                    pmgi_channel;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP4;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    if (ETHER_FLAG_OFF == pause_frame_enable[channel])
    {
        local_advertise[channel] = ((((PHY_AN_ADVERTISEMENT_100F |
        PHY_AN_ADVERTISEMENT_100H) |
        PHY_AN_ADVERTISEMENT_10F) |
        PHY_AN_ADVERTISEMENT_10H) |
        PHY_AN_ADVERTISEMENT_SELECTOR);
    }

    /* When pause frame is used */
    else
    {
        local_advertise[channel] = ((((((PHY_AN_ADVERTISEMENT_ASM_DIR |
        PHY_AN_ADVERTISEMENT_PAUSE) |
        PHY_AN_ADVERTISEMENT_100F) |
        PHY_AN_ADVERTISEMENT_100H) |
        PHY_AN_ADVERTISEMENT_10F) |
        PHY_AN_ADVERTISEMENT_10H) |
        PHY_AN_ADVERTISEMENT_SELECTOR);
    }

    pmgi_access(channel, PHY_REG_AN_ADVERTISEMENT, local_advertise[channel], PMGI_WRITE);

    return ret;
} /* End of function pmgi_open_zc2_step3() */

#elif ETHER_CFG_USE_PHY_KSZ8041NL == 0
static ether_return_t pmgi_open_zc2_step2(uint32_t channel)
{
    return ETHER_ERR_OTHER;
}

static ether_return_t pmgi_open_zc2_step3(uint32_t channel)
{
    return ETHER_ERR_OTHER;
}
#endif

/******************************************************************************
* Function Name:    pmgi_open_zc2_step4
* Description  :    Processing after R_ETHER_Open_ZC2'step4 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step4(uint32_t channel)
{
    ether_return_t          ret = ETHER_SUCCESS;
    uint16_t                pmgi_channel;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP5;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_CONTROL, (PHY_CONTROL_AN_ENABLE |
            PHY_CONTROL_AN_RESTART), PMGI_WRITE);

    return ret;
} /* End of function pmgi_open_zc2_step4() */

/******************************************************************************
* Function Name:    pmgi_open_zc2_step5
* Description  :    Processing after R_ETHER_Open_ZC2'step5 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step5(uint32_t channel)
{
    ether_return_t          ret = ETHER_SUCCESS;
    uint16_t                pmgi_channel;
    uint16_t                data = 0;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP6;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_AN_ADVERTISEMENT, data, PMGI_READ);

    return ret;
} /* End of function pmgi_open_zc2_step5() */

/******************************************************************************
* Function Name:    pmgi_open_zc2_step6
* Description  :    Processing after R_ETHER_Open_ZC2'step6 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_open_zc2_step6(uint32_t channel)
{
    ether_return_t              ret = ETHER_SUCCESS;
    uint16_t                    pmgi_channel;
    uint16_t                    reg;
    const ether_control_t *     pether_ch;
    uint32_t                    phy_access;

    volatile struct st_etherc R_BSP_EVENACCESS_SFR *    petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR *     pedmac_adr;

    pmgi_channel = get_pmgi_channel(channel);
    reg = pmgi_read_reg(pmgi_channel);

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;


    /* Clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
    petherc_adr->ECSR.LONG = 0x00000037;

    /* Clear all EDMAC status bits */
    pedmac_adr->EESR.LONG = 0x47FF0F9F;

#if (ETHER_CFG_USE_LINKSTA == 1)
    /* Enable interrupts of interest only. */
    petherc_adr->ECSIPR.BIT.LCHNGIP = 1;
#endif

    pedmac_adr->EESIPR.BIT.ECIIP = 1;

    /* Set Ethernet interrupt level and enable */
    ether_enable_icu(channel);

    if (LINKPROCESS_OPEN_ZC2 == g_phy_current_param[pmgi_channel].mode)
    {
#if (ETHER_CFG_USE_LINKSTA == 0)
        /* set the current parameter of phy */
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC0;
        g_phy_current_param[pmgi_channel].step = STEP0;
        g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

        pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

#elif (ETHER_CFG_USE_LINKSTA == 1)
        if (ETHER_FLAG_ON_LINK_ON == lchng_flag[channel])
        {
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC1;
            g_phy_current_param[pmgi_channel].step = STEP0;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
            pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);
        }
        else if (ETHER_FLAG_ON_LINK_OFF == lchng_flag[channel])
        {
            lchng_flag[channel] = ETHER_FLAG_OFF;
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS_CHECKLINK_ZC2;
            g_phy_current_param[pmgi_channel].step = STEP0;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
            pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);
        }
        else
        {
            g_phy_current_param[channel].mode = LINKPROCESS;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        }
#endif
    }
    else
    {

        /* PMGI access complete */
        g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        g_phy_current_param[pmgi_channel].read_data = reg;
    }

    return ret;
} /* End of function pmgi_open_zc2_step6() */

/******************************************************************************
* Function Name:    pmgi_checklink_zc_step0
* Description  :    Processing after R_ETHER_CHECKLINK_ZC'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_checklink_zc_step0(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;
    uint16_t            reg = 0;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP1;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
    pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

    return ret;
} /* End of function pmgi_checklink_zc_step0() */

/******************************************************************************
* Function Name:    pmgi_checklink_zc_step1
* Description  :    Processing after R_ETHER_CHECKLINK_ZC'step1 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_checklink_zc_step1(uint32_t channel)
{
    ether_return_t                                      ret = ETHER_SUCCESS;
    volatile struct st_etherc R_BSP_EVENACCESS_SFR *    petherc_adr;
    const ether_control_t *                             pether_ch;
    uint32_t                                            phy_access;
    ether_cb_arg_t                                      cb_arg;
    uint16_t                                            pmgi_channel;
    uint16_t                                            reg;
#if (ETHER_CFG_USE_LINKSTA == 0)
    int16_t                                             link_status;
#endif

    pmgi_channel = get_pmgi_channel(channel);

    if ((CHECKLINK_ZC == g_phy_current_param[pmgi_channel].mode))
    {
        reg = pmgi_read_reg(pmgi_channel);
        if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
        {
            g_phy_current_param[pmgi_channel].read_data = reg;
            g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
        }
        else
        {
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].read_data = reg;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        }
    }
    else if (LINKPROCESS_CHECKLINK_ZC0 == g_phy_current_param[pmgi_channel].mode)
    {
#if (ETHER_CFG_USE_LINKSTA == 0)
        reg = pmgi_read_reg(pmgi_channel);

        if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
        {
         link_status = ETHER_ERR_OTHER;
        }
        else
        {
         link_status = ETHER_SUCCESS;
        }

        if (g_pre_link_stat[channel] != link_status)
        {
            if (ETHER_SUCCESS == (ether_return_t)link_status)
            {
                /* The state of the link status in PHY-LSI is confirmed and Link Up/Down is judged. */
                /* When becoming Link up */
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;
            }
            else
            {
                /* When becoming Link down */
                lchng_flag[channel] = ETHER_FLAG_ON_LINK_OFF;
            }
        }
        g_pre_link_stat[channel] = link_status;

        if (ETHER_FLAG_ON_LINK_ON == lchng_flag[channel])
        {
            /*
            * The status of the LINK signal became "link-up" even if PHY-LSI did not detect "link-up"
            * after a reset. To avoid this wrong detection, processing in R_ETHER_LinkProcess has been modified to
            * clear the flag after link-up is confirmed in R_ETHER_CheckLink_ZC.
            */
            lchng_flag[channel] = ETHER_FLAG_OFF;

            /* Initialize the transmit and receive descriptor */
            memset((void *)&rx_descriptors[channel], 0x00, sizeof(rx_descriptors[channel]));
            memset((void *)&tx_descriptors[channel], 0x00, sizeof(tx_descriptors[channel]));

            /* Initialize the Ether buffer */
            memset(&ether_buffers[channel], 0x00, sizeof(ether_buffers[channel]));

            transfer_enable_flag[channel] = ETHER_FLAG_ON;

            /*
            * ETHERC and EDMAC are set after ETHERC and EDMAC are reset in software
            * and sending and receiving is permitted.
            */
            ether_configure_mac(channel, mac_addr_buf[channel], NO_USE_MAGIC_PACKET_DETECT);

            /* set PMGI current parameter */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            g_phy_current_param[pmgi_channel].step = STEP0;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
            /* PMGI access  */
            pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

            return ret;
        }
        else if (ETHER_FLAG_ON_LINK_OFF == lchng_flag[channel])
        {
            pether_ch = g_eth_control_ch[channel].pether_control;
            phy_access = g_eth_control_ch[channel].phy_access;
            petherc_adr = pether_ch[phy_access].petherc;

            /* Disable receive and transmit. */
            petherc_adr->ECMR.BIT.RE = 0;
            petherc_adr->ECMR.BIT.TE = 0;

            transfer_enable_flag[channel] = ETHER_FLAG_OFF;

            if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
            {
                cb_arg.channel = channel;
                cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_OFF;
                (*cb_func.pcb_func)((void *) &cb_arg);
            }

            /* PMGI access complete */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            ret = ETHER_ERR_OTHER;
            return ret;
        }
        else
        {
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        }
#elif (ETHER_CFG_USE_LINKSTA == 1)
        ret = ETHER_ERR_OTHER;
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
        return ret;
#endif
    }
    else if (LINKPROCESS_CHECKLINK_ZC1 == g_phy_current_param[pmgi_channel].mode)
    {
#if (ETHER_CFG_USE_LINKSTA == 0)
        ret = ETHER_ERR_OTHER;
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
        return ret;
#elif (ETHER_CFG_USE_LINKSTA == 1)
        reg = pmgi_read_reg(pmgi_channel);

        if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
        {
            R_BSP_NOP();

            /* Link is down */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            ret = ETHER_ERR_OTHER;
            return ret;
        }
        else
        {
            /*
             * The status of the LINK signal became "link-up" even if PHY-LSI did not detect "link-up"
             * after a reset. To avoid this wrong detection, processing in R_ETHER_LinkProcess has been modified to
             * clear the flag after link-up is confirmed in R_ETHER_CheckLink_ZC.
             */
            lchng_flag[channel] = ETHER_FLAG_OFF;

            /* Initialize the transmit and receive descriptor */
            memset((void *)&rx_descriptors[channel], 0x00, sizeof(rx_descriptors[channel]));
            memset((void *)&tx_descriptors[channel], 0x00, sizeof(tx_descriptors[channel]));

            /* Initialize the Ether buffer */
            memset(&ether_buffers[channel], 0x00, sizeof(ether_buffers[channel]));

            transfer_enable_flag[channel] = ETHER_FLAG_ON;

            /*
             * ETHERC and EDMAC are set after ETHERC and EDMAC are reset in software
             * and sending and receiving is permitted.
             */
            ether_configure_mac(channel, mac_addr_buf[channel], NO_USE_MAGIC_PACKET_DETECT);

            /* set PMGI current parameters */
            g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
            g_phy_current_param[pmgi_channel].step = STEP0;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

            /* PMGI Read */

            pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

            return ret;
        }
#endif /* (ETHER_CFG_USE_LINKSTA == 0) */
    }
    else if (LINKPROCESS_CHECKLINK_ZC2 == g_phy_current_param[pmgi_channel].mode)
    {
#if (ETHER_CFG_USE_LINKSTA == 1)
        reg = pmgi_read_reg(pmgi_channel);

        if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
        {    /* Link is down */
            pether_ch = g_eth_control_ch[channel].pether_control;
            phy_access = g_eth_control_ch[channel].phy_access;
            petherc_adr = pether_ch[phy_access].petherc;

            /* Disable receive and transmit. */
            petherc_adr->ECMR.BIT.RE = 0;
            petherc_adr->ECMR.BIT.TE = 0;

            transfer_enable_flag[channel] = ETHER_FLAG_OFF;

            if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
            {
                cb_arg.channel = channel;
                cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_OFF;
                (*cb_func.pcb_func)((void *) &cb_arg);
            }
        }
        else
        {
            R_BSP_NOP();
        }

        g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
        g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        return ret;
#elif (ETHER_CFG_USE_LINKSTA == 0)
        g_phy_current_param[pmgi_channel].mode = LINKPROCESS;
        ret = ETHER_ERR_OTHER;
        return ret;
#endif
    }
    else if (WAKEONLAN_CHECKLINK_ZC == g_phy_current_param[pmgi_channel].mode)
    {
        reg = pmgi_read_reg(pmgi_channel);
        if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
        {
            g_phy_current_param[pmgi_channel].read_data = reg;
            g_phy_current_param[pmgi_channel].mode = WAKEONLAN;
            g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
        }
        else
        {
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].read_data = reg;
            g_phy_current_param[pmgi_channel].mode = WAKEONLAN;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        }
    }
    else
    {
        g_phy_current_param[pmgi_channel].mode = CHECKLINK_ZC;
        g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
    }

    return ret;
} /* End of function pmgi_checklink_zc_step1() */

/******************************************************************************
* Function Name:    pmgi_linkprocess_step0
* Description  :    Processing after R_ETHER_LinkProcess'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_linkprocess_step0(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;
    uint16_t            reg = 0;

    pmgi_channel = get_pmgi_channel(channel);

    /* set PMGI current parameter */
    g_phy_current_param[pmgi_channel].step = STEP1;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

    return ret;
} /* End of function pmgi_linkprocess_step0() */

/******************************************************************************
* Function Name:    pmgi_linkprocess_step1
* Description  :    Processing after R_ETHER_LinkProcess'step1 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_linkprocess_step1(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;
    uint16_t            reg;
    uint16_t            data = 0;

    g_local_pause_bits[channel] = 0;

    pmgi_channel = get_pmgi_channel(channel);

    reg = pmgi_read_reg(pmgi_channel);

    if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
    {
        R_BSP_NOP();
        ret = ETHER_ERR_OTHER;
        return ret;
    }
    else
    {
        /* Establish local pause capability */
        if (PHY_AN_ADVERTISEMENT_PAUSE == (local_advertise[channel] & PHY_AN_ADVERTISEMENT_PAUSE))
        {
            g_local_pause_bits[channel] |= (1 << 1);
        }

        if (PHY_AN_ADVERTISEMENT_ASM_DIR == (local_advertise[channel] & PHY_AN_ADVERTISEMENT_ASM_DIR))
        {
            g_local_pause_bits[channel] |= 1;
        }

        /* When the auto-negotiation isn't completed, return error */
        if (PHY_STATUS_AN_COMPLETE != (reg & PHY_STATUS_AN_COMPLETE))
        {
            /* When PHY auto-negotiation is not completed */
            transfer_enable_flag[channel] = ETHER_FLAG_OFF;
            lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;

            ret= ETHER_ERR_OTHER;
            return ret;
        }
        else
        {
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].step = STEP2;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
            pmgi_access(channel, PHY_REG_AN_LINK_PARTNER, data, PMGI_READ);
        }
    }
    return ret;
} /* End of function pmgi_linkprocess_step1() */

/******************************************************************************
* Function Name:    pmgi_linkprocess_step2
* Description  :    Processing after R_ETHER_LinkProcess'step2 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_linkprocess_step2(uint32_t channel)
{
    ether_return_t              ret = ETHER_SUCCESS;
    uint16_t                    link_speed_duplex = 0;
    uint16_t                    partner_pause_bits = 0;
    uint16_t                    transmit_pause_set = 0;
    uint16_t                    receive_pause_set = 0;
    uint16_t                    full_duplex = 0;
    uint16_t                    pmgi_channel;
    uint16_t                    reg;
    ether_cb_arg_t              cb_arg;

    volatile struct st_etherc R_BSP_EVENACCESS_SFR *    petherc_adr;
    volatile struct st_edmac R_BSP_EVENACCESS_SFR *     pedmac_adr;
    const ether_control_t *                             pether_ch;
    uint32_t                                            phy_access;

    pmgi_channel = get_pmgi_channel(channel);
    reg = pmgi_read_reg(pmgi_channel);

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;
    pedmac_adr = pether_ch[phy_access].pedmac;

    /* Establish partner pause capability */
    if (PHY_AN_LINK_PARTNER_PAUSE == (reg & PHY_AN_LINK_PARTNER_PAUSE))
    {
        partner_pause_bits = (1 << 1);
    }

    if (PHY_AN_LINK_PARTNER_ASM_DIR == (reg & PHY_AN_LINK_PARTNER_ASM_DIR))
    {
        partner_pause_bits |= 1;
    }

    /* Establish the line speed and the duplex */
    if (PHY_AN_LINK_PARTNER_10H == (reg & PHY_AN_LINK_PARTNER_10H))
    {
        link_speed_duplex = PHY_LINK_10H;
    }

    if (PHY_AN_LINK_PARTNER_10F == (reg & PHY_AN_LINK_PARTNER_10F))
    {
        link_speed_duplex = PHY_LINK_10F;
    }
    if (PHY_AN_LINK_PARTNER_100H == (reg & PHY_AN_LINK_PARTNER_100H))
    {
        link_speed_duplex = PHY_LINK_100H;
    }

    if (PHY_AN_LINK_PARTNER_100F == (reg & PHY_AN_LINK_PARTNER_100F))
    {
        link_speed_duplex = PHY_LINK_100F;
    }

    switch (link_speed_duplex)
    {
        /* Half duplex link */
        case PHY_LINK_100H :
            petherc_adr->ECMR.BIT.DM = 0;
            petherc_adr->ECMR.BIT.RTM = 1;
            ret = ETHER_SUCCESS;
        break;

        case PHY_LINK_10H :
            petherc_adr->ECMR.BIT.DM = 0;
            petherc_adr->ECMR.BIT.RTM = 0;
            ret = ETHER_SUCCESS;
        break;

            /* Full duplex link */
        case PHY_LINK_100F :
            petherc_adr->ECMR.BIT.DM = 1;
            petherc_adr->ECMR.BIT.RTM = 1;
            full_duplex = 1;
            ret = ETHER_SUCCESS;
        break;

        case PHY_LINK_10F :
            petherc_adr->ECMR.BIT.DM = 1;
            petherc_adr->ECMR.BIT.RTM = 0;
            full_duplex = 1;
            ret = ETHER_SUCCESS;
        break;

        default :
            ret = ETHER_ERR_OTHER;
        break;
    }

    if (ETHER_SUCCESS == ret)
    {

        /* When pause frame is used */
        if ((full_duplex) && (ETHER_FLAG_ON == pause_frame_enable[channel]))
        {
            /* Set automatic PAUSE for 512 bit-time */
            petherc_adr->APR.LONG = 0x0000FFFF;

            /* Set unlimited retransmit of PAUSE frames */
            petherc_adr->TPAUSER.LONG = 0;

            /* PAUSE flow control FIFO settings. */
            pedmac_adr->FCFTR.LONG = 0x00000000;

            /* Control of a PAUSE frame whose TIME parameter value is 0 is enabled. */
            petherc_adr->ECMR.BIT.ZPF = 1;

            /**
             * Enable PAUSE for full duplex link depending on
             * the pause resolution results
             */
            ether_pause_resolution(g_local_pause_bits[channel], partner_pause_bits, &transmit_pause_set,
                    &receive_pause_set);

            if (XMIT_PAUSE_ON == transmit_pause_set)
            {
                /* Enable automatic PAUSE frame transmission */
                petherc_adr->ECMR.BIT.TXF = 1;
            }
            else
            {
                /* Disable automatic PAUSE frame transmission */
                petherc_adr->ECMR.BIT.TXF = 0;
            }

            if (RECV_PAUSE_ON == receive_pause_set)
            {
                /* Enable reception of PAUSE frames */
                petherc_adr->ECMR.BIT.RXF = 1;
            }
            else
            {
                /* Disable reception of PAUSE frames */
                petherc_adr->ECMR.BIT.RXF = 0;
            }
        }

        /* When pause frame is not used */
        else
        {
            /* Disable PAUSE for half duplex link */
            petherc_adr->ECMR.BIT.TXF = 0;
            petherc_adr->ECMR.BIT.RXF = 0;
        }

        /* Set the promiscuous mode bit */
        petherc_adr->ECMR.BIT.PRM = promiscuous_mode[channel];

        /* Enable receive and transmit. */
        petherc_adr->ECMR.BIT.RE = 1;
        petherc_adr->ECMR.BIT.TE = 1;

        /* Enable EDMAC receive */
        pedmac_adr->EDRRR.LONG = 0x1;

        if ((NULL != cb_func.pcb_func) && (FIT_NO_FUNC != cb_func.pcb_func))
        {
            cb_arg.channel = channel;
            cb_arg.event_id = ETHER_CB_EVENT_ID_LINK_ON;
            (*cb_func.pcb_func)((void *) &cb_arg);
        }

        g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
    }
    else
    {
        /* When PHY auto-negotiation is not completed */
        transfer_enable_flag[channel] = ETHER_FLAG_OFF;
        lchng_flag[channel] = ETHER_FLAG_ON_LINK_ON;

        g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
    }

    /* set the current parameter of phy */

    return ret;
} /* End of function pmgi_linkprocess_step2() */

/******************************************************************************
* Function Name:    pmgi_wakeonlan_step0
* Description  :    Processing after R_ETHER_WakeOnLAN'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_wakeonlan_step0(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;
    uint16_t            reg = 0;

    pmgi_channel = get_pmgi_channel(channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].step = STEP1;
    g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

    pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);

    return ret;
} /* End of function pmgi_wakeonlan_step0() */

/******************************************************************************
* Function Name:    pmgi_wakeonlan_step1
* Description  :    Processing after R_ETHER_WakeOnLAN'step1 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_wakeonlan_step1(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;
    uint16_t            reg;
    uint16_t            data = 0;

    g_local_pause_bits[channel] = 0;

    pmgi_channel = get_pmgi_channel(channel);

    reg = pmgi_read_reg(pmgi_channel);

    if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
    {
        R_BSP_NOP();
        ret = ETHER_ERR_OTHER;
    }
    else
    {
        /* Establish local pause capability */
        if (PHY_AN_ADVERTISEMENT_PAUSE == (local_advertise[channel] & PHY_AN_ADVERTISEMENT_PAUSE))
        {
            g_local_pause_bits[channel] |= (1 << 1);
        }

        if (PHY_AN_ADVERTISEMENT_ASM_DIR == (local_advertise[channel] & PHY_AN_ADVERTISEMENT_ASM_DIR))
        {
            g_local_pause_bits[channel] |= 1;
        }

        /* When the auto-negotiation isn't completed, return error */
        if (PHY_STATUS_AN_COMPLETE != (reg & PHY_STATUS_AN_COMPLETE))
        {
            ret= ETHER_ERR_OTHER;
        }
        else
        {
            /* set the current parameter of phy */
            g_phy_current_param[pmgi_channel].step = STEP2;
            g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;
            pmgi_access(channel, PHY_REG_AN_LINK_PARTNER, data, PMGI_READ);
        }
    }
    return ret;
} /* End of function pmgi_wakeonlan_step1() */

/******************************************************************************
* Function Name:    pmgi_wakeonlan_step2
* Description  :    Processing after R_ETHER_WakeOnLAN'step2 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI one step processing successful
*                   ETHER_ERR_OTHER
*                       Other error
******************************************************************************/
static ether_return_t pmgi_wakeonlan_step2(uint32_t channel)
{
    ether_return_t              ret = ETHER_SUCCESS;
    uint16_t                    link_speed_duplex = 0;
    uint16_t                    pmgi_channel;
    uint16_t                    reg;

    volatile struct st_etherc R_BSP_EVENACCESS_SFR *    petherc_adr;
    const ether_control_t *                             pether_ch;
    uint32_t                                            phy_access;

    pmgi_channel = get_pmgi_channel(channel);
    reg = pmgi_read_reg(pmgi_channel);

    pether_ch = g_eth_control_ch[channel].pether_control;
    phy_access = g_eth_control_ch[channel].phy_access;
    petherc_adr = pether_ch[phy_access].petherc;

    /* Establish the line speed and the duplex */
    if (PHY_AN_LINK_PARTNER_10H == (reg & PHY_AN_LINK_PARTNER_10H))
    {
        link_speed_duplex = PHY_LINK_10H;
    }

    if (PHY_AN_LINK_PARTNER_10F == (reg & PHY_AN_LINK_PARTNER_10F))
    {
        link_speed_duplex = PHY_LINK_10F;
    }
    if (PHY_AN_LINK_PARTNER_100H == (reg & PHY_AN_LINK_PARTNER_100H))
    {
        link_speed_duplex = PHY_LINK_100H;
    }

    if (PHY_AN_LINK_PARTNER_100F == (reg & PHY_AN_LINK_PARTNER_100F))
    {
        link_speed_duplex = PHY_LINK_100F;
    }

    switch (link_speed_duplex)
    {
        /* Half duplex link */
        case PHY_LINK_100H :
            petherc_adr->ECMR.BIT.DM = 0;
            petherc_adr->ECMR.BIT.RTM = 1;
            ret = ETHER_SUCCESS;
        break;

        case PHY_LINK_10H :
            petherc_adr->ECMR.BIT.DM = 0;
            petherc_adr->ECMR.BIT.RTM = 0;
            ret = ETHER_SUCCESS;
        break;

            /* Full duplex link */
        case PHY_LINK_100F :
            petherc_adr->ECMR.BIT.DM = 1;
            petherc_adr->ECMR.BIT.RTM = 1;
            ret = ETHER_SUCCESS;
        break;

        case PHY_LINK_10F :
            petherc_adr->ECMR.BIT.DM = 1;
            petherc_adr->ECMR.BIT.RTM = 0;
            ret = ETHER_SUCCESS;
        break;

        default :
            ret = ETHER_ERR_OTHER;
        break;
    }

    if (ETHER_SUCCESS == ret)
    {

        /* The magic packet detection is permitted. */
        petherc_adr->ECMR.BIT.MPDE = 1;

        /* Because data is not transmitted for the magic packet detection waiting,
         only the reception is permitted. */
        petherc_adr->ECMR.BIT.RE = 1;

        /*
         * The reception function of EDMAC keep invalidity
         * because the receive data don't need to be read when the magic packet detection mode.
         */
#if (ETHER_CFG_USE_LINKSTA == 1)
        pether_ch = g_eth_control_ch[channel].pether_control;
        phy_access = g_eth_control_ch[channel].phy_access;
        petherc_adr = pether_ch[phy_access].petherc;

        /* It is confirmed not to become Link down while changing the setting. */
        if (ETHER_CFG_LINK_PRESENT == petherc_adr->PSR.BIT.LMON)
        {
            ret = ETHER_SUCCESS;
            g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;
        }
        else
        {
            ret = ETHER_ERR_OTHER;
            g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
        }
#elif (ETHER_CFG_USE_LINKSTA == 0)
        /* set the current parameter of phy */
        g_phy_current_param[pmgi_channel].mode = WAKEONLAN_CHECKLINK_ZC;
        g_phy_current_param[pmgi_channel].step = STEP0;
        g_phy_current_param[pmgi_channel].event = PMGI_RUNNING;

        pmgi_access(channel, PHY_REG_STATUS, reg, PMGI_READ);
#endif
    }
    else
    {
        g_phy_current_param[pmgi_channel].event = PMGI_ERROR;
    }

    return ret;
} /* End of function pmgi_wakeonlan_step2() */

/******************************************************************************
* Function Name:    pmgi_writephy_step0
* Description  :    Processing after R_ETHER_WritePHY'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI operate successfully
******************************************************************************/
static ether_return_t pmgi_writephy_step0(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t            pmgi_channel;

    /* set the current parameter of phy */
    pmgi_channel = get_pmgi_channel(channel);
    g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;

    return ret;
} /* End of function pmgi_writephy_step0() */

/******************************************************************************
* Function Name:    pmgi_readphy_step0
* Description  :    Processing after R_ETHER_ReadPHY'step0 is completed.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_SUCCESS
*                       PMGI operate successfully
******************************************************************************/
static ether_return_t pmgi_readphy_step0(uint32_t channel)
{
    ether_return_t      ret = ETHER_SUCCESS;
    uint16_t pmgi_channel;
    uint16_t reg_data;

    /* get the PMGI channel */
    pmgi_channel = get_pmgi_channel(channel);

    /* read the PMGI register */
    reg_data = pmgi_read_reg(pmgi_channel);

    /* set the current parameter of phy */
    g_phy_current_param[pmgi_channel].read_data = reg_data;
    g_phy_current_param[pmgi_channel].event = PMGI_COMPLETE;

    return ret;
} /* End of function pmgi_readphy_step0() */

/******************************************************************************
* Function Name:    pmgi_modestep_invalid
* Description  :    When the parameters of PMGI interrupt process function pointer array
*                   is error, return ETHER_ERR_INVALID_ARG.
* Arguments    :    channel -
*                       ETHERC channel number
* Return Value :    ETHER_ERR_INVALID_ARG
*                       The paramters of PMGI interrupt processing function pointer array
*                       is error.
******************************************************************************/
static ether_return_t pmgi_modestep_invalid(uint32_t channel)
{
    return ETHER_ERR_INVALID_ARG;
} /* End of function pmgi_modestep_invalid() */
#endif /* (ETHER_CFG_NON_BLOCKING == 1) */

