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
 * File Name    : phy.c
 * Version      : 1.20
 * Description  : Ethernet PHY device driver
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.07.2014 1.00     First Release
 *         : 16.12.2014 1.01     Made changes related to header file include.
 *         : 29.01.2015 1.02     Correction of ETHER_CFG_USE_PHY_KSZ8041NL.
 *         : 31.03.2016 1.10     Added changes behavior of phy_get_link_status function depending on number of Ethernet channel.
 *         : 20.05.2019 1.16     Added support for GNUC and ICCRX.
 *                               Fixed coding style.
 *         : 30.07.2019 1.17     Added WAIT LOOP.
 *         : 22.11.2019 1.20     Added macro ETHER_CFG_NON_BLOCKING to choose whether to use PMGI.
 *                               Added pmgi_initial, pmgi_access, pmgi_read_reg, pmgi_close function for NON-BLOCKING.
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 ***********************************************************************************************************************/
/* Access to peripherals and board defines. */
#include "platform.h"

#include "r_ether_rx_config.h"
#include "src/r_ether_rx_private.h"
#include "src/phy/phy.h"

/***********************************************************************************************************************
 Macro definitions
 ***********************************************************************************************************************/
/* Media Independent Interface */
#define PHY_MII_ST                      (1)
#define PHY_MII_READ                    (2)
#define PHY_MII_WRITE                   (1)

/***********************************************************************************************************************
 Typedef definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 Private global variables and functions
 ***********************************************************************************************************************/
#if ETHER_CFG_NON_BLOCKING == 0
uint16_t phy_read (uint32_t ether_channel, uint16_t reg_addr);
void phy_write (uint32_t ether_channel, uint16_t reg_addr, uint16_t data);
static void phy_preamble (uint32_t ether_channel);
static void phy_reg_set (uint32_t ether_channel, uint16_t reg_addr, int32_t option);
static void phy_reg_read (uint32_t ether_channel, uint16_t *pdata);
static void phy_reg_write (uint32_t ether_channel, uint16_t data);
static void phy_trans_zto0 (uint32_t ether_channel);
static void phy_trans_1to0 (uint32_t ether_channel);
static void phy_mii_write1 (uint32_t ether_channel);
static void phy_mii_write0 (uint32_t ether_channel);
static int16_t phy_get_pir_address (uint32_t ether_channel, volatile uint32_t R_BSP_EVENACCESS_SFR ** pppir_addr);

static uint16_t local_advertise[ETHER_CHANNEL_MAX]; /* the capabilities of the local link as PHY data */
#endif

/*
 * Private global variables
 */
#if ETHER_CFG_NON_BLOCKING == 1

  /* Used to prevent having duplicate code for each channel. This only works if the channels are identical (just at
   different locations in memory). This is easy to tell by looking in iodefine.h and seeing if the same structure
   was used for all channels. */
volatile struct st_pmgi R_BSP_EVENACCESS_SFR * gp_pmgi_channels[PMGI_CHANNEL_MAX] =
{
/* Initialize the array for up to 3 channels. Add more as needed. */
#if   PMGI_CHANNEL_MAX == 1
    &PMGI0,
#elif PMGI_CHANNEL_MAX == 2
    &PMGI0, &PMGI1
#endif
};
#endif
/**
 * Public functions
 */
#if ETHER_CFG_NON_BLOCKING == 0
/***********************************************************************************************************************
 * Function Name: phy_init
 * Description  : Resets Ethernet PHY device
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : R_PHY_OK -
 *                    
 *                R_PHY_ERROR -
 *                    
 ***********************************************************************************************************************/
int16_t phy_init (uint32_t ether_channel)
{
    uint16_t reg;
    uint32_t count;

    /* Reset PHY */
    phy_write(ether_channel, PHY_REG_CONTROL, PHY_CONTROL_RESET);

    count = 0;

    /* Reset completion waiting */
    do
    {
        reg = phy_read(ether_channel, PHY_REG_CONTROL);
        count++;
    } while ((reg & PHY_CONTROL_RESET) && (count < ETHER_CFG_PHY_DELAY_RESET));     /* WAIT_LOOP */

    if (count < ETHER_CFG_PHY_DELAY_RESET)
    {
        /* 
         * When KSZ8041NL of the Micrel, Inc. is used, 
         * the pin that outputs the state of LINK is used combinedly with ACTIVITY in default. 
         * The setting of the pin is changed so that only the state of LINK is output. 
         */
#if ETHER_CFG_USE_PHY_KSZ8041NL != 0
        reg = phy_read(ether_channel, PHY_REG_PHY_CONTROL_1);
        reg &= ~0x8000;
        reg |= 0x4000;
        phy_write(ether_channel, PHY_REG_PHY_CONTROL_1, reg);
#endif /* ETHER_CFG_USE_PHY_KSZ8041NL != 0 */

        return R_PHY_OK;
    }

    return R_PHY_ERROR;
} /* End of function phy_init() */

/***********************************************************************************************************************
 * Function Name: phy_start_autonegotiate
 * Description  : Starts auto-negotiate
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                pause -
 *                    Using state of pause frames
 * Return Value : none
 ***********************************************************************************************************************/
void phy_start_autonegotiate (uint32_t ether_channel, uint8_t pause)
{
    volatile uint16_t reg = 0;

    /* Set local ability */
    /* When pause frame is not used */
    if (ETHER_FLAG_OFF == pause)
    {
        local_advertise[ether_channel] = ((((PHY_AN_ADVERTISEMENT_100F |
        PHY_AN_ADVERTISEMENT_100H) |
        PHY_AN_ADVERTISEMENT_10F) |
        PHY_AN_ADVERTISEMENT_10H) |
        PHY_AN_ADVERTISEMENT_SELECTOR);

    }

    /* When pause frame is used */
    else
    {
        local_advertise[ether_channel] = ((((((PHY_AN_ADVERTISEMENT_ASM_DIR |
        PHY_AN_ADVERTISEMENT_PAUSE) |
        PHY_AN_ADVERTISEMENT_100F) |
        PHY_AN_ADVERTISEMENT_100H) |
        PHY_AN_ADVERTISEMENT_10F) |
        PHY_AN_ADVERTISEMENT_10H) |
        PHY_AN_ADVERTISEMENT_SELECTOR);
    }

    /* Configure what the PHY and the Ethernet controller on this board supports */
    phy_write(ether_channel, PHY_REG_AN_ADVERTISEMENT, local_advertise[ether_channel]);
    phy_write(ether_channel, PHY_REG_CONTROL, (PHY_CONTROL_AN_ENABLE |
    PHY_CONTROL_AN_RESTART));

    reg = phy_read(ether_channel, PHY_REG_AN_ADVERTISEMENT);

} /* End of function phy_start_autonegotiate() */

/***********************************************************************************************************************
 * Function Name: phy_set_autonegotiate
 * Description  : reports the other side's physical capability
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *              : *pline_speed_duplex - 
 *                    a pointer to the location of both the line speed and the duplex
 *                *plocal_pause - 
 *                    a pointer to the location to store the local pause bits.
 *                *ppartner_pause - 
 *                    a pointer to the location to store the partner pause bits.
 * Return Value : R_PHY_OK -
 *                    
 *                R_PHY_ERROR -
 *                    
 * Note         : The value returned to local_pause and patner_pause is used 
 *                as it is as an argument of ether_pause_resolution function. 
 ***********************************************************************************************************************/
int16_t phy_set_autonegotiate (uint32_t ether_channel, uint16_t *pline_speed_duplex, uint16_t *plocal_pause,
        uint16_t *ppartner_pause)
{
    uint16_t reg;

    /* Because reading the first time shows the previous state, the Link status bit is read twice. */
    reg = phy_read(ether_channel, PHY_REG_STATUS);
    reg = phy_read(ether_channel, PHY_REG_STATUS);

    /* When the link isn't up, return error */
    if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
    {
        R_BSP_NOP();
        return R_PHY_ERROR;
    }

    /* Establish local pause capability */
    if (PHY_AN_ADVERTISEMENT_PAUSE == (local_advertise[ether_channel] & PHY_AN_ADVERTISEMENT_PAUSE))
    {
        (*plocal_pause) |= (1 << 1);
    }

    if (PHY_AN_ADVERTISEMENT_ASM_DIR == (local_advertise[ether_channel] & PHY_AN_ADVERTISEMENT_ASM_DIR))
    {
        (*plocal_pause) |= 1;
    }

    /* When the auto-negotiation isn't completed, return error */
    if (PHY_STATUS_AN_COMPLETE != (reg & PHY_STATUS_AN_COMPLETE))
    {
        return R_PHY_ERROR;
    }
    else
    {
        /* Get the link partner response */
        reg = phy_read(ether_channel, PHY_REG_AN_LINK_PARTNER);

        /* Establish partner pause capability */
        if (PHY_AN_LINK_PARTNER_PAUSE == (reg & PHY_AN_LINK_PARTNER_PAUSE))
        {
            (*ppartner_pause) = (1 << 1);
        }

        if (PHY_AN_LINK_PARTNER_ASM_DIR == (reg & PHY_AN_LINK_PARTNER_ASM_DIR))
        {
            (*ppartner_pause) |= 1;
        }

        /* Establish the line speed and the duplex */
        if (PHY_AN_LINK_PARTNER_10H == (reg & PHY_AN_LINK_PARTNER_10H))
        {
            (*pline_speed_duplex) = PHY_LINK_10H;
        }

        if (PHY_AN_LINK_PARTNER_10F == (reg & PHY_AN_LINK_PARTNER_10F))
        {
            (*pline_speed_duplex) = PHY_LINK_10F;
        }

        if (PHY_AN_LINK_PARTNER_100H == (reg & PHY_AN_LINK_PARTNER_100H))
        {
            (*pline_speed_duplex) = PHY_LINK_100H;
        }

        if (PHY_AN_LINK_PARTNER_100F == (reg & PHY_AN_LINK_PARTNER_100F))
        {
            (*pline_speed_duplex) = PHY_LINK_100F;
        }

        return R_PHY_OK;
    }
} /* End of function phy_set_autonegotiate() */

/***********************************************************************************************************************
 * Function Name: phy_get_link_status 
 * Description  : Returns the status of the physical link 
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : -1 if links is down, 0 otherwise 
 ***********************************************************************************************************************/
int16_t phy_get_link_status (uint32_t ether_channel)
{
    uint16_t reg;

    /* Because reading the first time shows the previous state, the Link status bit is read twice. */
    reg = phy_read(ether_channel, PHY_REG_STATUS);
    reg = phy_read(ether_channel, PHY_REG_STATUS);

    /* When the link isn't up, return error */
    if (PHY_STATUS_LINK_UP != (reg & PHY_STATUS_LINK_UP))
    {
        R_BSP_NOP();

        /* Link is down */
        return R_PHY_ERROR;
    }
    else
    {
        /* Link is up */
        return R_PHY_OK;
    }
} /* End of function phy_get_link_status() */

/**
 * Private functions
 */

/***********************************************************************************************************************
 * Function Name: phy_read
 * Description  : Reads a PHY register
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                reg_addr -
 *                    address of the PHY register
 * Return Value : read value
 ***********************************************************************************************************************/
uint16_t phy_read (uint32_t ether_channel, uint16_t reg_addr)
{
    uint16_t data;

    /*
     * The value is read from the PHY register by the frame format of MII Management Interface provided 
     * for by Table 22-12 of 22.2.4.5 of IEEE 802.3-2008_section2. 
     */
    phy_preamble(ether_channel);
    phy_reg_set(ether_channel, reg_addr, PHY_MII_READ);
    phy_trans_zto0(ether_channel);
    phy_reg_read(ether_channel, &data);
    phy_trans_zto0(ether_channel);

    return (data);
} /* End of function phy_read() */

/***********************************************************************************************************************
 * Function Name: phy_write
 * Description  : Writes to a PHY register
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                reg_addr -
 *                    address of the PHY register
 *                data -
 *                    value
 * Return Value : none
 ***********************************************************************************************************************/
void phy_write (uint32_t ether_channel, uint16_t reg_addr, uint16_t data)
{
    /*
     * The value is read from the PHY register by the frame format of MII Management Interface provided
     * for by Table 22-12 of 22.2.4.5 of IEEE 802.3-2008_section2. 
     */
    phy_preamble(ether_channel);
    phy_reg_set(ether_channel, reg_addr, PHY_MII_WRITE);
    phy_trans_1to0(ether_channel);
    phy_reg_write(ether_channel, data);
    phy_trans_zto0(ether_channel);
} /* End of function phy_write() */

/***********************************************************************************************************************
 * Function Name: phy_preamble
 * Description  : As preliminary preparation for access to the PHY module register,
 *                "1" is output via the MII management interface.                  
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_preamble (uint32_t ether_channel)
{
    int16_t i;

    /*
     * The processing of PRE (preamble) about the frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    i = 32;
    /* WAIT_LOOP */
    while (i > 0)
    {
        phy_mii_write1(ether_channel);
        i--;
    }
} /* End of function phy_preamble() */

/***********************************************************************************************************************
 * Function Name: phy_reg_set
 * Description  : Sets a PHY device to read or write mode
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                reg_addr -
 *                    address of the PHY register
 *                option -
 *                    mode
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_reg_set (uint32_t ether_channel, uint16_t reg_addr, int32_t option)
{
    int32_t i;
    uint16_t data;
    int8_t phy_acc_channel;
    const ether_control_t * pether_control;

    /*
     * The processing of ST (start of frame),OP (operation code), PHYAD (PHY Address), and 
     * REGAD (Register Address)  about the frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    data = 0;
    data = (PHY_MII_ST << 14); /* ST code    */

    if (PHY_MII_READ == option)
    {
        data |= (PHY_MII_READ << 12); /* OP code(RD)  */
    }
    else
    {
        data |= (PHY_MII_WRITE << 12); /* OP code(WT)  */
    }

    phy_acc_channel = g_eth_control_ch[ether_channel].phy_access;

    pether_control = g_eth_control_ch[ether_channel].pether_control;

    data |= (uint16_t) (pether_control[phy_acc_channel].phy_address << 7); /* PHY Address  */

    data |= (reg_addr << 2); /* Reg Address  */

    i = 14;
    /* WAIT_LOOP */
    while (i > 0)
    {
        if (0 == (data & 0x8000))
        {
            phy_mii_write0(ether_channel);
        }
        else
        {
            phy_mii_write1(ether_channel);
        }
        data <<= 1;
        i--;
    }
} /* End of function phy_reg_set() */

/***********************************************************************************************************************
 * Function Name: phy_reg_read
 * Description  : Reads PHY register through MII interface
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                pdata -
 *                    pointer to store the data read
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_reg_read (uint32_t ether_channel, uint16_t *pdata)
{
    int32_t i;
    int32_t j;
    uint16_t reg_data;
    int16_t ret;
    volatile uint32_t R_BSP_EVENACCESS_SFR * petherc_pir;

    ret = phy_get_pir_address(ether_channel, &petherc_pir);
    if ( R_PHY_ERROR == ret)
    {
        return;
    }

    /*
     * The processing of DATA (data) about reading of the frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    reg_data = 0;
    i = 16;
    /* WAIT_LOOP */
    while (i > 0)
    {
        /* WAIT_LOOP */
        for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
        {
            (*petherc_pir) = 0x00000000;
        }

        /* WAIT_LOOP */
        for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
        {
            (*petherc_pir) = 0x00000001;
        }
        reg_data <<= 1;

        reg_data |= (uint16_t) (((*petherc_pir) & 0x00000008) >> 3); /* MDI read  */

        /* WAIT_LOOP */
        for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
        {
            (*petherc_pir) = 0x00000001;
        }

        /* WAIT_LOOP */
        for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
        {
            (*petherc_pir) = 0x00000000;
        }

        i--;
    }
    (*pdata) = reg_data;
} /* End of function phy_reg_read() */

/***********************************************************************************************************************
 * Function Name: phy_reg_write
 * Description  : Writes to PHY register through MII interface
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                data -
 *                    value to write
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_reg_write (uint32_t ether_channel, uint16_t data)
{
    int32_t i;

    /*
     * The processing of DATA (data) about writing of the frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    i = 16;
    /* WAIT_LOOP */
    while (i > 0)
    {
        if (0 == (data & 0x8000))
        {
            phy_mii_write0(ether_channel);
        }
        else
        {
            phy_mii_write1(ether_channel);
        }
        i--;
        data <<= 1;
    }
} /* End of function phy_reg_write() */

/***********************************************************************************************************************
 * Function Name: phy_trans_zto0
 * Description  : Performs bus release so that PHY can drive data
 *              : for read operation 
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_trans_zto0 (uint32_t ether_channel)
{
    int32_t j;
    int16_t ret;
    volatile uint32_t R_BSP_EVENACCESS_SFR * petherc_pir;

    ret = phy_get_pir_address(ether_channel, &petherc_pir);
    if ( R_PHY_ERROR == ret)
    {
        return;
    }

    /*
     * The processing of TA (turnaround) about reading of the frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000000;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000001;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000001;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000000;
    }

} /* End of function phy_trans_zto0() */

/***********************************************************************************************************************
 * Function Name: phy_trans_1to0
 * Description  : Switches data bus so MII interface can drive data
 *              : for write operation 
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_trans_1to0 (uint32_t ether_channel)
{
    /*
     * The processing of TA (turnaround) about writing of the frame format of MII Management Interface which is
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2".
     */
    phy_mii_write1(ether_channel);
    phy_mii_write0(ether_channel);
} /* End of function phy_trans_1to0() */

/***********************************************************************************************************************
 * Function Name: phy_mii_write1
 * Description  : Outputs 1 to the MII interface 
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_mii_write1 (uint32_t ether_channel)
{
    int32_t j;
    int16_t ret;
    volatile uint32_t R_BSP_EVENACCESS_SFR * petherc_pir;

    ret = phy_get_pir_address(ether_channel, &petherc_pir);
    if ( R_PHY_ERROR == ret)
    {
        return;
    }

    /*
     * The processing of one bit about frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2". 
     * The data that 1 is output. 
     */
    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000006;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000007;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000007;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000006;
    }

} /* End of function phy_mii_write1() */

/***********************************************************************************************************************
 * Function Name: phy_mii_write0
 * Description  : Outputs 0 to the MII interface 
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 * Return Value : none
 ***********************************************************************************************************************/
static void phy_mii_write0 (uint32_t ether_channel)
{
    int32_t j;
    int16_t ret;
    volatile uint32_t R_BSP_EVENACCESS_SFR * petherc_pir;

    ret = phy_get_pir_address(ether_channel, &petherc_pir);
    if ( R_PHY_ERROR == ret)
    {
        return;
    }

    /*
     * The processing of one bit about frame format of MII Management Interface which is 
     * provided by "Table 22-12" of "22.2.4.5" of "IEEE 802.3-2008_section2". 
     * The data that 0 is output. 
     */
    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000002;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000003;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000003;
    }

    /* WAIT_LOOP */
    for (j = ETHER_CFG_PHY_MII_WAIT; j > 0; j--)
    {
        (*petherc_pir) = 0x00000002;
    }

} /* End of function phy_mii_write0() */

/***********************************************************************************************************************
 * Function Name: phy_get_pir_address
 * Description  : Get the address of the PHY interface register.
 * Arguments    : ether_channel -
 *                    Ethernet channel number
 *                pppir_addr -
 *                    Pointer of the PHY interface register
 * Return Value : none
 ***********************************************************************************************************************/
static int16_t phy_get_pir_address (uint32_t ether_channel, volatile uint32_t R_BSP_EVENACCESS_SFR ** pppir_addr)
{
    const ether_control_t * pether_ch;
    uint32_t phy_access;
    volatile uint32_t R_BSP_EVENACCESS_SFR * petherc_pir;

    if (ETHER_CHANNEL_MAX <= ether_channel)
    {
        return R_PHY_ERROR;
    }

    pether_ch = g_eth_control_ch[ether_channel].pether_control;
    phy_access = g_eth_control_ch[ether_channel].phy_access;
    petherc_pir = pether_ch[phy_access].preg_pir;

    (*pppir_addr) = petherc_pir;

    return R_PHY_OK;
} /* End of function phy_get_pir_address() */

#endif /* End of ETHER_CFG_NON_BLOCKING == 0 */

#if ETHER_CFG_NON_BLOCKING == 1
/***********************************************************************************************************************
 * Function Name: pmgi_initial
 * Description  : PMGI initialze.
 * Arguments    : pmgi_channel -
 *                    PMGI channel number
 * Return Value : R_PHY_OK      success
                : R_PHY_ERROR   error
 ***********************************************************************************************************************/
int16_t pmgi_initial(uint16_t pmgi_channel)
{
    int32_t     f;  // Frequency
    int32_t     n;  // n term in equation

    /* set the frequency of MDC */
    f = BSP_PCLKA_HZ;
    n = (f/(ETHER_CFG_PMGI_CLOCK * 2)) - 1;

    if ((n <= 0) || (n > 63))
    {
        return R_PHY_ERROR;
    }
    else
    {
        (*gp_pmgi_channels[pmgi_channel]).PMGCR.BIT.PSMCS = n;
    }

    /* set the preamble control bit */
    (*gp_pmgi_channels[pmgi_channel]).PMGCR.BIT.PSMDP = ETHER_CFG_PMGI_ENABLE_PREAMBLE;

    /* set the hold time adjustment */
    (*gp_pmgi_channels[pmgi_channel]).PMGCR.BIT.PSMHT = ETHER_CFG_PMGI_HOLD_TIME;

    /* set the capture time adjustment */
    (*gp_pmgi_channels[pmgi_channel]).PMGCR.BIT.PSMCT = ETHER_CFG_PMGI_CAPTURE_TIME;

#if ((ETHER_CFG_CH0_PHY_ACCESS == 0) || (ETHER_CFG_CH1_PHY_ACCESS == 0))
    if (PMGI_CHANNEL_0 == pmgi_channel)
    {
        R_BSP_InterruptRequestEnable(VECT(PMGI0, PMGI0I));
        IPR(PMGI0, PMGI0I) = ETHER_CFG_PMGI_INT_PRIORTY;
    }
#endif
#if (PMGI_CHANNEL_MAX == 2)
#if ((ETHER_CFG_CH0_PHY_ACCESS == 1) || (ETHER_CFG_CH1_PHY_ACCESS == 1))
    if (PMGI_CHANNEL_1 == pmgi_channel)
    {
        R_BSP_InterruptRequestEnable(VECT(PMGI1, PMGI1I));
        IPR(PMGI1, PMGI1I) = ETHER_CFG_PMGI_INT_PRIORTY;
    }
#endif
#endif

    return R_PHY_OK;
} /* End of function pmgi_initial */

/***********************************************************************************************************************
 * Function Name: pmgi_read_reg
 * Description  : read PMGI register.
 * Arguments    : pmgi_channel -
 *                    PMGI channel number
 * Return Value : reg      register value
 ***********************************************************************************************************************/
uint16_t pmgi_read_reg(uint16_t pmgi_channel)
{
    uint16_t reg;

    reg = (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PRD;

    return reg;
} /*End of function pmgi_read_reg */

/***********************************************************************************************************************
 * Function Name: pmgi_close
 * Description  : PMGI close.
 * Arguments    : pmgi_channel -
 *                    PMGI channel number
 * Return Value : R_PHY_OK      success
 ***********************************************************************************************************************/
int16_t pmgi_close(uint16_t pmgi_channel)
{
#if ((ETHER_CFG_CH0_PHY_ACCESS == 0) || (ETHER_CFG_CH1_PHY_ACCESS == 0))
    if (PMGI_CHANNEL_0 == pmgi_channel )
    {
        R_BSP_InterruptRequestDisable(VECT(PMGI0, PMGI0I));
    }
#endif
#if (PMGI_CHANNEL_MAX == 2)
#if ((ETHER_CFG_CH0_PHY_ACCESS == 1) || (ETHER_CFG_CH1_PHY_ACCESS == 1))
    if (PMGI_CHANNEL_1 == pmgi_channel )
    {
        R_BSP_InterruptRequestDisable(VECT(PMGI1, PMGI1I));
    }
#endif
#endif
    return R_PHY_OK;
} /* End of function pmgi_close */

/***********************************************************************************************************************
 * Function Name: pmgi_access
 * Description  : PMGI access.
 * Arguments    : channel -
 *                    ETHER     channel number
 *                    address   phy register address
 *                    data      access data
 *                    dir       access direction
 * Return Value : R_PHY_OK      success
                : R_PHY_ERROR   error
 ***********************************************************************************************************************/
int16_t pmgi_access(uint32_t channel, uint16_t address, uint16_t data, uint16_t dir)
{
    uint16_t pmgi_channel;

    if (ETHER_CHANNEL_0 == channel)
    {
        pmgi_channel = ETHER_CFG_CH0_PHY_ACCESS;
        (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PDA = ETHER_CFG_CH0_PHY_ADDRESS;
    }
    else
    {
        pmgi_channel = ETHER_CFG_CH1_PHY_ACCESS;
        (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PDA = ETHER_CFG_CH1_PHY_ADDRESS;
    }
    (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PSMAD = dir;
    (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PRA = address;

    if (PMGI_WRITE == dir)
    {
        (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PRD = data;
    }

    (*gp_pmgi_channels[pmgi_channel]).PSMR.BIT.PSME = PMGI_START;

    return R_PHY_OK;
} /*End of function pmgi_access */
#endif /* End of ETHER_CFG_NON_BLOCKING == 1 */

