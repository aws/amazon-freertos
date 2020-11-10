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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_s12ad_rx.c
* Description  : Primary source code for 12-bit A/D Converter driver.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.07.2013 1.00    Initial Release.
*           21.04.2014 1.20    Updated for RX210 advanced features; RX110/63x support.
*           06.11.2014 1.40    Added RX113 support.
*           01.03.2016 2.11    Added RX130 and RX230 support.
*           01.10.2016 2.20    Added RX65x support.
*                              Corresponding to the notice for A/D convert stop.(RX110/RX111/RX113/RX210/RX63x)
*                              Corresponding to the notice for shift to Low power mode.(RX110/RX111/RX113/RX210/RX63x)
*                              Delete parameter check by the enum value.
*                              R_ADC_ReadAll() parameter change.(adjust the parameter structure to RX130/RX230/RX231)
*                              Bug fix for callback function execute when pointer is NULL.(RX130/RX230/RX231)
*                              TEMPS register can change only during temperature sensor mode.(RX63x/RX210)
*           03.09.2018 3.00    Added RX66T support.
*                              Added the comment to while statement.
*           28.12.2018 3.10    Added RX72T support.
*           05.04.2019 4.00    Added support for GNUC and ICCRX.
*                              Deleted the process of RX210, RX631, and RX63N.
*                              Deleted the function of adc_gbadi_isr.
*           28.06.2019 4.10    Added RX23W support.
*           31.07.2019 4.20    Added RX72M support.
*           30.08.2019 4.30    Added RX13T support.
*           22.11.2019 4.40    Added RX66N and RX72N support.
*                              Added support for atomic control.
*           28.02.2020 4.50    Added RX23E-A support.
*                              Added the following processing to fix the bug.
*                              - Added the process of read the A/D Data Duplication Register (ADDBLDR) to the 
*                                R_ADC_ReadAll function.
*                              - Added the interrupt function of A/D scan end interrupt for Group B.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "platform.h"
/* Configuration for this package. */
#include "r_s12ad_rx_config.h"
/* Public interface header file for this package. */
#include "r_s12ad_rx_if.h"
/* Private header file for this package. */
#include "r_s12ad_rx_private.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T)    || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))

/* In ROM */
extern R_BSP_VOLATILE_EVENACCESS uint16_t * const  gp_dreg0_ptrs[];
extern R_BSP_VOLATILE_EVENACCESS uint16_t * const  gp_dreg1_ptrs[];
#if (defined(BSP_MCU_RX66T) || defined(BSP_MCU_RX72T))
extern R_BSP_VOLATILE_EVENACCESS uint16_t * const  gp_dreg2_ptrs[];
#endif

#else  /* rx110/rx111/rx113/rx130/rx13t/rx230/rx231/rx23w/rx23e-a */

extern R_BSP_VOLATILE_EVENACCESS uint16_t * const  gp_dreg_ptrs[]; // In ROM
adc_ctrl_t g_dcb = { ADC_MODE_MAX, false, NULL};  // In RAM

#endif /* #if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T)    || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N)) */

#if (!defined(BSP_MCU_RX64M) && !defined(BSP_MCU_RX65_ALL) && !defined(BSP_MCU_RX66T) \
    && !defined(BSP_MCU_RX71M) && !defined(BSP_MCU_RX72T) && !defined(BSP_MCU_RX72M) \
    && !defined(BSP_MCU_RX13T) && !defined(BSP_MCU_RX66N) && !defined(BSP_MCU_RX72N))
R_BSP_PRAGMA_STATIC_INTERRUPT(adc_s12adi0_isr, VECT(S12AD,S12ADI0))
R_BSP_PRAGMA_STATIC_INTERRUPT(adc_gbadi_isr, VECT(S12AD,GBADI))

#endif

/**********************************************************************************************************************
 * Function Name: R_ADC_Open
 ******************************************************************************************************************//**
 * @brief This function initializes the 12-bit A/D converter. This function must be run before calling any other API
 * function.
 * @param[in] unit Unit number. Set this to 0 if your MCU supports only one unit.
 * @param[in] mode Operating mode.
 * @param[in] p_cfg Pointer to the configuration structure of the 12-bit A/D converter function.
 * @param[in] p_callback Optional pointer to function called from interrupt when a scan completes or a comparator
 * condition is met. When not using this parameter, set FIT_NO_PTR.
 * @retval ADC_SUCCESS Successful
 * @retval ADC_ERR_AD_LOCKED Open() call is in progress elsewhere
 * @retval ADC_ERR_AD_NOT_CLOSED Peripheral is still running in another mode; Perform R_ADC_Close() first
 * @retval ADC_ERR_INVALID_ARG Element of p_cfg structure has invalid value
 * @retval ADC_ERR_ILLEGAL_ARG an argument is illegal based upon mode
 * @retval ADC_ERR_MISSING_PTR p_cfg pointer is FIT_NO_PTR/NULL
 * @retval ADC_ERR_CONFIGURABLE_INT vector number of software configurable interrupt B is out of range
 * @details Applies power to the A/D peripheral, sets the operational mode, trigger sources, interrupt priority, and
 * configurations common to all channels and sensors. With a non-zero interrupt priority (interrupt usage), a callback
 * function is called by the interrupts whenever a scan has completed or a comparator condition is met. When setting
 * the interrupt priority to 0, a callback function is not called. In this case, poll for scan completion with the
 * R_ADC_Control() function when necessary. To set values of parameters used in this function, first clear all members
 * of parameters to 0, and then set values.
 * @note (RX Family Common):\n
 * Before calling the R_ADC_Open() function, set the vector number for the software configurable interrupt B
 * assigned to the unit to be used.
 * Refer to the User's Manual: Hardware about limitations of using output pins on the same port as analog pins.\n
 * The application must set the A/D conversion clock prior to calling R_ADC_Open() function.\n
 * To stop A/D conversion which is started in continuous scan mode, call the R_ADC_Close function.\n
 * If continuous scan mode is selected, it is recommended not to use the S12ADI interrupt since scan completion occurs
 * continuously. That causes the majority of the processing time to be spent at the interrupt level.\n
 * If interrupts are in use, a callback function is required which takes a single argument. This is a pointer to a
 * which is cast to a void pointer (provides consistency with other FIT module callback functions). Cast to the
 * structure adc_cb_args_t pointer in the interrupt handling.\n\n
 * (S12ADb, A12ADC, S12ADE, S12ADF, S12ADFa, S12ADH):\n
 *    After the R_ADC_Open() function is executed, wait at least 1 us before executing A/D conversion.
 */
adc_err_t R_ADC_Open( uint8_t const          unit,
                    adc_mode_t const        mode,
                    adc_cfg_t * const       p_cfg,
                    void         (* const  p_callback)(void *p_args))
{
    return adc_open(unit, mode, p_cfg, p_callback);
} /* End of function R_ADC_Open() */


/**********************************************************************************************************************
 * Function Name: R_ADC_Control
 ******************************************************************************************************************//**
 * @brief This function makes 12-bit A/D converter function settings and acquires the interrupt control and A/D
 * converter start/stop state.
 * @param[in] unit Unit number. Set this to 0 if your MCU supports only one unit.
 * @param[in] cmd Command to run.
 * @param[in] p_args Pointer to optional configuration structure. Clear all members of the argument to 0 before setting
 * values to them. If the command requires no argument, set FIT_NO_PTR.
 * @retval ADC_SUCCESS Successful
 * @retval ADC_ERR_MISSING_PTR p_args pointer is FIT_NO_PTR/NULL when required as an argument
 * @retval ADC_ERR_INVALID_ARG Invalid value is specified to p_args structure
 * @retval ADC_ERR_ILLEGAL_ARG cmd is illegal based upon mode
 * @retval ADC_ERR_SCAN_NOT_DONE The requested scan has not completed
 * @retval ADC_ERR_TRIG_ENABLED Cannot configure comparator because scan still running
 * @retval ADC_ERR_CONDITION_NOT_MET No channels/sensors met the comparison condition
 * @details Provides commands for enabling channels and sensors and for runtime operations. These include
 * enabling/disabling trigger sources and interrupts, initiating a software trigger, and checking for scan completion.
 * After the R_ADC_Open function is called, the commands can be issued by using the R_ADC_Control function.
 * See Section 3.2 in the application note for details.
 * @note (RX Family Common) \n
 * When the A/D conversion start (ADST) bit is 1, settings such as mode must not be changed
 * using this function. However, the conversion status or the comparison result can be obtained.\n
 * When switching channels used for A/D conversion or settings, call the R_ADC_Close() function once and then call the
 * R_ADC_Open() function again to start.\n
 * When waiting completion of A/D conversion using the R_ADC_Control function, use the commands.See Section
 * 3.2 in the application note for details.\n
 * When A/D conversion interrupts are enabled, the R_ADC_Control() function cannot be used to wait completion of
 * A/D conversion except when using single scan mode with software trigger. In this case, use the callback function for
 * the A/D conversion interrupt to wait completion of A/D conversion.\n\n
 * (S12ADF)\n
 * If Group A Priority is selected such that Group B operates in continuous scan mode, it is recommended not
 * to use the GBADI interrupt since the interrupt handling will be processed so often. That causes the majority of
 * the processing time to be spent at the interrupt level.\n\n
 * (S12ADC, S12ADFa)\n
 * Channels and sensors can be combined in the same unit.\n
 * ELC is only for S12ADI, not S12GBADI or S12CMPI. (S12ADC)\n
 * ELC is only for S12ADI, not GBADI, GCADI, S12CMPAI or S12CMPBI. (S12ADFa)\n
 * The application should wait 30 us after configuring the scan before enabling the trigger for Temperature Sensor
 * for best results.\n
 * If Group A Priority is selected such that Group B operates in continuous scan mode, it is recommended not to
 * use the S12GBADI interrupt (S12ADC) and GBADI interrupt (S12ADFa) since the interrupt handling will be processed
 * so often. That causes the majority of the processing time to be spent at the interrupt level.\n
 * Enabling the comparator should be done prior to enabling the triggers. Some features may not be used with others.
 * See Section 3.2 in the application note for details.\n\n
 * (S12ADE)\n
 * This function does not support following features.\n
 * Compare function window B\n
 * Compare function window A/B composite condition setting\n\n
 * (S12ADC, S12ADE, S12ADFa)\n
 * When using the comparison, configure the comparison after the channel configuration.\n\n
 * (S12ADb, S12ADFa, A12ADH)\n
 * For temperature sensor output and internal reference voltage, the number of
 * sampling states must be set as indicated below, at a minimum:\n
 * S12ADb, S12ADFa: 5 us\n
 * S12ADH: 4 us\n
 */
adc_err_t R_ADC_Control(uint8_t const       unit,
                        adc_cmd_t const     cmd,
                        void * const        p_args)
{
    return adc_control(unit, cmd, p_args);
} /* End of function R_ADC_Control() */


/**********************************************************************************************************************
 * Function Name: R_ADC_Read
 ******************************************************************************************************************//**
 * @brief This function reads conversion results from a single channel, sensor, double trigger, or self-diagnosis
 * register.
 * @param[in] unit Unit number. Set this to 0 if your MCU supports only one unit.
 * @param[in] reg_id ID of the register to read.
 * @param[in] p_data Pointer to variable to load value into.
 * @retval ADC_SUCCESS Success
 * @retval ADC_ERR_INVALID_ARG unit or reg_id contains an invalid value
 * @retval ADC_ERR_MISSING _PTR p_data is FIT_NO_PTR/NULL
 * @details Reads conversion results from a single channel, sensor, double trigger, or self-diagnosis register.
 * @note (S12ADb)\n
 * For temperature sensor output and internal reference voltage, discard the first A/D conversion
 * result after the open, and use the second and the subsequent A/D conversion results.
 */
adc_err_t R_ADC_Read(uint8_t            unit,
                    adc_reg_t const    reg_id,
                    uint16_t * const   p_data)
{
    dregs_t *p_dregs;

#if (defined(BSP_MCU_RX64M)   || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T)    || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    p_dregs = ADC_PRV_GET_DATA_ARR(unit);
#else
    p_dregs = gp_dreg_ptrs;
#endif /* #if (defined(BSP_MCU_RX64M)   || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72T)    || defined(BSP_MCU_RX72M) \
    || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N)) */

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    #if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) \
        || defined(BSP_MCU_RX66N) || defined(BSP_MCU_RX72N))
    if (unit > 1)
    {
        return ADC_ERR_INVALID_ARG;
    }
    #elif (defined(BSP_MCU_RX66T) || defined(BSP_MCU_RX72T))
    if (unit > 2)
    {
        return ADC_ERR_INVALID_ARG;
    }
    #endif

    /* Casting is valid because it matches the type to the right side or argument. */
    if ((NULL == p_data) || (FIT_NO_PTR == p_data))
    {
        return ADC_ERR_MISSING_PTR;
    }

    if (NULL == p_dregs[reg_id])
    {
        return ADC_ERR_INVALID_ARG;
    }

    /* Do not check if corresponding channel/sensor is enabled */
#endif

    *p_data = *p_dregs[reg_id];

    return ADC_SUCCESS;
} /* End of function R_ADC_Read() */

/**********************************************************************************************************************
 * Function Name: R_ADC_ReadAll
 ******************************************************************************************************************//**
 * @brief This function reads conversion results from all storage registers supported by the MCU.
 * @param[in] p_all_data Pointer to structure in which register values are loaded.
 * @retval ADC_SUCCESS Success
 * @retval ADC_ERR_MISSING_PTR p_data is FIT_NO_PTR/NULL
 * @details Reads conversion results from all potential sources, enabled or not.
 * @note None.
 */
adc_err_t R_ADC_ReadAll(adc_data_t * const  p_all_data)
{
#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    /* Casting is valid because it matches the type to the right side or argument. */
    if ((NULL == p_all_data) || (FIT_NO_PTR == p_all_data))
    {
        return ADC_ERR_MISSING_PTR;
    }
#endif

#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M)    || defined(BSP_MCU_RX72T) \
    || defined(BSP_MCU_RX231) || defined(BSP_MCU_RX230)    || defined(BSP_MCU_RX23W) \
    || defined(BSP_MCU_RX130) || defined(BSP_MCU_RX13T)    || defined(BSP_MCU_RX66N) \
    || defined(BSP_MCU_RX72N) || defined(BSP_MCU_RX23E_A))

    return adc_read_all(p_all_data);

#else /* rx110/rx111/rx113 */

    p_all_data->chan[ADC_REG_CH0] = S12AD.ADDR0;
    p_all_data->chan[ADC_REG_CH1] = S12AD.ADDR1;
    p_all_data->chan[ADC_REG_CH2] = S12AD.ADDR2;
    p_all_data->chan[ADC_REG_CH3] = S12AD.ADDR3;
    p_all_data->chan[ADC_REG_CH4] = S12AD.ADDR4;
    p_all_data->chan[ADC_REG_CH6] = S12AD.ADDR6;
    p_all_data->chan[ADC_REG_CH8] = S12AD.ADDR8;
    p_all_data->chan[ADC_REG_CH9] = S12AD.ADDR9;
    p_all_data->chan[ADC_REG_CH10] = S12AD.ADDR10;
    p_all_data->chan[ADC_REG_CH11] = S12AD.ADDR11;
    p_all_data->chan[ADC_REG_CH12] = S12AD.ADDR12;
    p_all_data->chan[ADC_REG_CH13] = S12AD.ADDR13;
    p_all_data->chan[ADC_REG_CH14] = S12AD.ADDR14;
    p_all_data->chan[ADC_REG_CH15] = S12AD.ADDR15;

#if (!defined(BSP_MCU_RX110) && !defined(BSP_MCU_RX111))
    p_all_data->chan[ADC_REG_CH5] = S12AD.ADDR5;
    p_all_data->chan[ADC_REG_CH7] = S12AD.ADDR7;
#endif
#ifdef BSP_MCU_RX113
    p_all_data->chan[ADC_REG_CH21] = S12AD.ADDR21;
#endif
    p_all_data->temp = S12AD.ADTSDR;
    p_all_data->volt = S12AD.ADOCDR;
    p_all_data->dbltrig = S12AD.ADDBLDR;

    return ADC_SUCCESS;

#endif /* #if definedBSP_MCU_RX64M || definedBSP_MCU_RX65_ALL || definedBSP_MCU_RX66T \
    || definedBSP_MCU_RX71M || definedBSP_MCU_RX72M    || definedBSP_MCU_RX72T \
    || definedBSP_MCU_RX231 || definedBSP_MCU_RX230    || definedBSP_MCU_RX23W \
    || definedBSP_MCU_RX130 || definedBSP_MCU_RX13T    || definedBSP_MCU_RX66N \
    || definedBSP_MCU_RX72N)|| defined(BSP_MCU_RX23E_A) */

} /* End of function R_ADC_ReadAll() */

/**********************************************************************************************************************
* Function Name: R_ADC_Close
 ******************************************************************************************************************//**
 * @brief This function ends any scan in progress, disables interrupts, and removes power to the A/D peripheral.
 * @param[in] unit Unit number. Set this to 0 if your MCU supports only one unit.
 * @retval ADC_SUCCESS Success
 * @retval ADC_ERR_INVALID_ARG unit contains an invalid value.
 * @details Ends the A/D conversion in progress, disables interrupts, and ends A/D converter operation. This function
 * can be called once per unit after the R_ADC_Open function is called.\n
 * When changing A/D conversion settings, call the R_ADC_Open() function again after this function is called.
 * @note This function will abort any scan that may be in progress.
 */
adc_err_t   R_ADC_Close(uint8_t const unit)
{
#if (defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65_ALL) || defined(BSP_MCU_RX66T) \
    || defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M)    || defined(BSP_MCU_RX72T) \
    || defined(BSP_MCU_RX231) || defined(BSP_MCU_RX230)    || defined(BSP_MCU_RX23W) \
    || defined(BSP_MCU_RX130) || defined(BSP_MCU_RX13T)    || defined(BSP_MCU_RX66N) \
    || defined(BSP_MCU_RX72N) || defined(BSP_MCU_RX23E_A))

    return adc_close(unit);

#else /* rx110/rx111/rx113 */
    volatile uint16_t i;

#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    bsp_int_ctrl_t int_ctrl;
#endif

#if ADC_CFG_PARAM_CHECKING_ENABLE == 1
    if (unit > 0)
    {
        return ADC_ERR_INVALID_ARG;
    }
#endif

    /* Stop triggers & conversions, and disable peripheral interrupts */
    R_BSP_InterruptRequestDisable(VECT(S12AD,S12ADI0));         // disable interrupts in ICU
    IR(S12AD,S12ADI0) = 0;          // clear interrupt flag
    R_BSP_InterruptRequestDisable(VECT(S12AD,GBADI));           // disable interrupts in ICU
    IR(S12AD,GBADI) = 0;            // clear interrupt flag
    S12AD.ADCSR.BIT.TRGE = 0;
    S12AD.ADCSR.WORD = 0;

    /* Wait for 2 ADCLK cycles (MAX: 128 ICLK cycles) */
    /* WAIT_LOOP */
    for (i = 0; i < 128; i++)
    {
        R_BSP_NOP();
    }

    /* Power down peripheral */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
#ifndef BSP_MCU_RX11_ALL             // RX63x
    if (g_dcb.mode == ADC_MODE_SS_TEMPERATURE)
    {
        TEMPS.TSCR.BYTE = 0;
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
#endif
        MSTP(TEMPS) = 1;
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
#endif
    }
#endif
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_DISABLE, &int_ctrl);
#endif
    MSTP(S12AD) = 1;
#if ((R_BSP_VERSION_MAJOR == 5) && (R_BSP_VERSION_MINOR >= 30)) || (R_BSP_VERSION_MAJOR >= 6)
    R_BSP_InterruptControl(BSP_INT_SRC_EMPTY, BSP_INT_CMD_FIT_INTERRUPT_ENABLE, &int_ctrl);
#endif

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

    /* Show driver as closed */
    g_dcb.opened = false;

    return ADC_SUCCESS;

#endif /* #if (definedBSP_MCU_RX64M || definedBSP_MCU_RX65_ALL || definedBSP_MCU_RX66T \
    || definedBSP_MCU_RX71M || definedBSP_MCU_RX72M    || definedBSP_MCU_RX72T \
    || definedBSP_MCU_RX231 || definedBSP_MCU_RX230    || definedBSP_MCU_RX23W \
    || definedBSP_MCU_RX130 || definedBSP_MCU_RX13T    || definedBSP_MCU_RX66N \
    || definedBSP_MCU_RX72N) || defined(BSP_MCU_RX23E_A) */
} /* End of function R_ADC_Close() */


/**********************************************************************************************************************
* Function Name: R_ADC_GetVersion
 ******************************************************************************************************************//**
 * @brief This function returns the driver version number at runtime.
 * @retval Version number.
 * @details Returns the version of this module. The version number is encoded such that the top 2 bytes are the major
 * version number and the bottom 2 bytes are the minor version number.
 * @note None.
 */
uint32_t  R_ADC_GetVersion(void)
{
    uint32_t const version = (ADC_VERSION_MAJOR << 16) | ADC_VERSION_MINOR;

    return version;
} /* End of function R_ADC_GetVersion() */


#if (!defined(BSP_MCU_RX64M) && !defined(BSP_MCU_RX65_ALL) && !defined(BSP_MCU_RX66T) \
    && !defined(BSP_MCU_RX71M) && !defined(BSP_MCU_RX72T) && !defined(BSP_MCU_RX72M) \
    && !defined(BSP_MCU_RX13T) && !defined(BSP_MCU_RX66N) && !defined(BSP_MCU_RX72N))

/******************************************************************************
* Function Name: adc_enable_s12adi0
* Description  : This function clears the S12ADI0 interrupt flag and enables
*                interrupts in the peripheral (for IR flag usage). If priority
*                is not 0, interrupts are enabled in the ICU.
*                NOTE: This has nothing to do with enabling triggers.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void adc_enable_s12adi0(void)
{

    IR(S12AD,S12ADI0) = 0;                  // clear flag
    S12AD.ADCSR.BIT.ADIE = 1;               // enable in peripheral
    if (0 != ICU.IPR[IPR_S12AD_S12ADI0].BYTE)
    {
        R_BSP_InterruptRequestEnable(VECT(S12AD,S12ADI0));             // enable in ICU
    }
} /* End of function adc_enable_s12adi0() */


/*****************************************************************************
* Function Name: adc_s12adi0_isr
* Description  : Interrupt handler for normal/Group A/double trigger
*                scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void adc_s12adi0_isr(void)
{
    adc_cb_evt_t    event = ADC_EVT_SCAN_COMPLETE;

    /* presence of callback function verified in Open() */
    if ((NULL != g_dcb.p_callback) && (FIT_NO_FUNC != g_dcb.p_callback))
    {
        g_dcb.p_callback(&event);
    }
} /* End of function adc_s12adi0_isr() */

/*****************************************************************************
* Function Name: adc_gbadi_isr
* Description  : Interrupt handler for Group B scan complete.
* Arguments    : none
* Return Value : none
******************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void adc_gbadi_isr(void)
{
    adc_cb_evt_t    event = ADC_EVT_SCAN_COMPLETE_GROUPB;

    /* presence of callback function verified in Open() */
    if ((NULL != g_dcb.p_callback) && (FIT_NO_FUNC != g_dcb.p_callback))
    {
        g_dcb.p_callback(&event);
    }
} /* End of function adc_gbadi_isr() */

#endif /* #if (!definedBSP_MCU_RX64M && !definedBSP_MCU_RX65_ALL && !definedBSP_MCU_RX66T \
    && !definedBSP_MCU_RX71M && !definedBSP_MCU_RX72T && !definedBSP_MCU_RX72M \
    && !definedBSP_MCU_RX13T && !definedBSP_MCU_RX66N && !definedBSP_MCU_RX72N) */
