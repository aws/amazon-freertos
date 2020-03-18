/*******************************************************************************
* Sample code used for reference only
* TBD : Public URL for CY HAL repo
*******************************************************************************/

/** Commands that can be issued */
typedef enum
{
    CYHAL_SDIO_CMD_GO_IDLE_STATE  =  0, //!> Go to idle state
    CYHAL_SDIO_CMD_SEND_RELATIVE_ADDR  =  3, //!> Send a relative address
    CYHAL_SDIO_CMD_IO_SEND_OP_COND  =  5, //!> Send an OP IO
    CYHAL_SDIO_CMD_SELECT_CARD  =  7, //!> Send a card select
    CYHAL_SDIO_CMD_GO_INACTIVE_STATE = 15, //!> Go to inactive state
    CYHAL_SDIO_CMD_IO_RW_DIRECT = 52, //!> Perform a direct read/write
    CYHAL_SDIO_CMD_IO_RW_EXTENDED = 53, //!> Perform an extended read/write
} cyhal_sdio_command_t;

/** Types of transfer that can be performed */
typedef enum
{
    CYHAL_READ, //!> Read from the card
    CYHAL_WRITE //!> Write to the card
} cyhal_transfer_t;

/** Events that can cause an SDIO interrupt */
typedef enum
{
    CYHAL_SDIO_CMD_COMPLETE   = 0x0001, //!> Command Complete
    CYHAL_SDIO_XFER_COMPLETE  = 0x0002, //!> Host read/write transfer is complete
    CYHAL_SDIO_BGAP_EVENT     = 0x0004, //!> This bit is set when both read/write transaction is stopped
    CYHAL_SDIO_DMA_INTERRUPT  = 0x0008, //!> Host controller detects an SDMA Buffer Boundary during transfer
    CYHAL_SDIO_BUF_WR_READY   = 0x0010, //!> This bit is set if the Buffer Write Enable changes from 0 to 1
    CYHAL_SDIO_BUF_RD_READY   = 0x0020, //!> This bit is set if the Buffer Read Enable changes from 0 to 1
    CYHAL_SDIO_CARD_INSERTION = 0x0040, //!> This bit is set if the Card Inserted in the Present State
    CYHAL_SDIO_CARD_REMOVAL   = 0x0080, //!> This bit is set if the Card Inserted in the Present State
    CYHAL_SDIO_CARD_INTERRUPT = 0x0100, //!> The synchronized value of the DAT[1] interrupt input for SD mode
    CYHAL_SDIO_INT_A          = 0x0200, //!> Reserved: set to 0
    CYHAL_SDIO_INT_B          = 0x0400, //!> Reserved: set to 0
    CYHAL_SDIO_INT_C          = 0x0800, //!> Reserved: set to 0,
    CYHAL_SDIO_RE_TUNE_EVENT  = 0x1000, //!> Reserved: set to 0,
    CYHAL_SDIO_FX_EVENT       = 0x2000, //!> This status is set when R[14] of response register is set to 1
    CYHAL_SDIO_CQE_EVENT      = 0x4000, //!> This status is set if Command Queuing/Crypto event has occurred
    CYHAL_SDIO_ERR_INTERRUPT  = 0x8000, //!> If any of the bits in the Error Interrupt Status register are set
    CYHAL_SDIO_ALL_INTERRUPTS = 0xE1FF, //!> Is used to enable/disable all interrupts
} cyhal_sdio_irq_event_t;

/** Handler for SDIO interrupts */
typedef void (*cyhal_sdio_irq_handler_t)(void *handler_arg, cyhal_sdio_irq_event_t event);

/** Sends a command to the SDIO block.
 *
 * @param[in,out] obj       The SDIO object
 * @param[in]     direction The direction of transfer (read/write)
 * @param[in]     command   The SDIO command to send
 * @param[in]     argument  The argument to the command
 * @param[out]    response  The response from the SDIO device
 * @return The status of the configure request
 */

cy_rslt_t cyhal_sdio_send_cmd(const cyhal_sdio_t *obj, cyhal_transfer_t direction, \
                              cyhal_sdio_command_t command, uint32_t argument, uint32_t *response)
{
    if (NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_en_sd_host_status_t result = CY_SD_HOST_ERROR_TIMEOUT;
    cy_stc_sd_host_cmd_config_t cmd;
    uint32_t retry = 50UL;
    uint32_t regIntrSts = Cy_SD_Host_GetNormalInterruptMask(obj->base);

    /* Clear out the response */
    if (response != NULL)
    {
        *response = 0UL;
    }

    while ( (CY_SD_HOST_SUCCESS != result) && (retry-- > 0UL) )
    {
        /* First clear out the command complete and transfer complete statuses */
        Cy_SD_Host_ClearNormalInterruptStatus(obj->base, CY_SD_HOST_CMD_COMPLETE);

        /* Disable CMD Done interrupt, will be enabled after transition is complete */
        Cy_SD_Host_SetNormalInterruptMask(obj->base, (regIntrSts & (uint16_t) ~CY_SD_HOST_CMD_COMPLETE) );

        /* Check if an error occurred on any previous transactions */
        if (Cy_SD_Host_GetNormalInterruptStatus(obj->base) & CY_SD_HOST_ERR_INTERRUPT)
        {
            /* Reset the block if there was an error. Note a full reset usually
             * requires more time, but this short version is working quite well and
             * successfully clears out the error state.
             */
            Cy_SD_Host_ClearErrorInterruptStatus(obj->base, 0xffffU);
            Cy_SD_Host_SoftwareReset(obj->base, CY_SD_HOST_RESET_CMD_LINE);
            Cy_SD_Host_SoftwareReset(obj->base, CY_SD_HOST_RESET_DATALINE);
            Cy_SysLib_DelayUs(1);
            obj->base->CORE.SW_RST_R = 0x00;
        }

        cmd.commandIndex                 = (uint32_t)command;
        cmd.commandArgument              = argument;
        cmd.enableCrcCheck               = true;
        cmd.enableAutoResponseErrorCheck = false;
        cmd.respType                     = CY_SD_HOST_RESPONSE_LEN_48;
        cmd.enableIdxCheck               = true;
        cmd.dataPresent                  = false;
        cmd.cmdType                      = CY_SD_HOST_CMD_NORMAL;

        result = Cy_SD_Host_SendCommand(obj->base, &cmd);

        if (CY_SD_HOST_SUCCESS == result)
        {
            result = Cy_SD_Host_PollCmdComplete(obj->base);
        }
    }

    (void)Cy_SD_Host_GetResponse(obj->base, response, false);

    if (CY_SD_HOST_SUCCESS != result)
    {
        ret = CY_RSLT_TYPE_ERROR;
    }

    /* Restore interrupts after transition */
    Cy_SD_Host_SetNormalInterruptMask(obj->base, regIntrSts);

    return ret;
}

/** Performs a bulk data transfer (CMD=53) to the SDIO block.
 *
 * @param[in,out] obj       The SDIO object
 * @param[in]     direction The direction of transfer (read/write)
 * @param[in]     argument  The argument to the command
 * @param[in]     data      The data to send to the SDIO device. The data buffer
 *                          should be aligned to the block size (64 bytes) if data
 *                          size is greater that block size (64 bytes).
 * @param[in]     length    The number of bytes to send
 * @param[out]    response  The response from the SDIO device
 * @return The status of the configure request
 */
cy_rslt_t cyhal_sdio_bulk_transfer(cyhal_sdio_t *obj, cyhal_transfer_t direction,
                                   uint32_t argument, const uint32_t *data,
                                   uint16_t length, uint32_t *response)
{
    if (NULL == obj)
    {
        return CYHAL_SDIO_RSLT_ERR_BAD_PARAM;
    }

    cy_rslt_t ret = CY_RSLT_SUCCESS;
    uint32_t retry = CY_SDIO_BULK_TRANSF_TRIES;
    cy_stc_sd_host_cmd_config_t cmd;
    cy_stc_sd_host_data_config_t dat;
    cy_en_sd_host_status_t result = CY_SD_HOST_ERROR_TIMEOUT;
    uint32_t regIntrSts = Cy_SD_Host_GetNormalInterruptMask(obj->base);

    /* Initialize data constants*/
    dat.autoCommand         = CY_SD_HOST_AUTO_CMD_NONE;
    dat.dataTimeout         = 0x0dUL;
    dat.enableIntAtBlockGap = false;
    dat.enReliableWrite     = false;
    dat.enableDma           = true;

    dat.read = (direction == CYHAL_WRITE) ? false : true;

    /* Clear out the response */
    if (response != NULL)
    {
        *response = 0UL;
    }

    while ( (CY_SD_HOST_SUCCESS != result) && (retry-- > 0UL) )
    {
        /* Add SDIO Error Handling
         * SDIO write timeout is expected when doing first write to register
         * after KSO bit disable (as it goes to AOS core).
         * This timeout, however, triggers an error state in the hardware.
         * So, check for the error and then recover from it
         * as needed via reset issuance. This is the only time known that
         * a write timeout occurs.
         */

        /* First clear out the command complete and transfer complete statuses */
        Cy_SD_Host_ClearNormalInterruptStatus(obj->base, (CY_SD_HOST_XFER_COMPLETE | CY_SD_HOST_CMD_COMPLETE) );

        /* Check if an error occurred on any previous transactions or reset
         *  after the first unsuccessful bulk transfer try
         */
        if ( (Cy_SD_Host_GetNormalInterruptStatus(obj->base) & CY_SD_HOST_ERR_INTERRUPT) ||
             (retry < CY_SDIO_BULK_TRANSF_TRIES) )
        {
            /* Reset the block if there was an error. Note a full reset usually
             * requires more time, but this short version is working quite well
             * and successfully clears out the error state.
             */
            Cy_SD_Host_ClearErrorInterruptStatus(obj->base, 0xffffU);
            Cy_SD_Host_SoftwareReset(obj->base, CY_SD_HOST_RESET_CMD_LINE);
            Cy_SD_Host_SoftwareReset(obj->base, CY_SD_HOST_RESET_DATALINE);
            Cy_SysLib_DelayUs(1);
            obj->base->CORE.SW_RST_R = 0x00;
        }

        /* Prepare the data transfer register */
        cmd.commandIndex                 = (uint32_t)CYHAL_SDIO_CMD_IO_RW_EXTENDED;
        cmd.commandArgument              = argument;
        cmd.enableCrcCheck               = true;
        cmd.enableAutoResponseErrorCheck = false;
        cmd.respType                     = CY_SD_HOST_RESPONSE_LEN_48;
        cmd.enableIdxCheck               = true;
        cmd.dataPresent                  = true;
        cmd.cmdType                      = CY_SD_HOST_CMD_NORMAL;
        dat.data                         = (uint32_t *)data;

        /* TODO - BSP-542 */
        /* Block mode */
        if (length >= obj->block_size)
        {
            dat.blockSize     = obj->block_size;
            dat.numberOfBlock = (length + obj->block_size - 1) / obj->block_size;
        }
        /* Byte mode */
        else
        {
            dat.blockSize     = length;
            dat.numberOfBlock = 1UL;
        }

        /* Disable XFER Done interrupt */
        Cy_SD_Host_SetNormalInterruptMask(obj->base, (regIntrSts &
                                                      (uint16_t) ~(CY_SD_HOST_XFER_COMPLETE |
                                                                   CY_SD_HOST_CMD_COMPLETE) ) );

        (void)Cy_SD_Host_InitDataTransfer(obj->base, &dat);

        result = Cy_SD_Host_SendCommand(obj->base, &cmd);
        if (CY_SD_HOST_SUCCESS == result)
        {
            result = Cy_SD_Host_PollCmdComplete(obj->base);

            if (CY_SD_HOST_SUCCESS == result)
            {
                result = Cy_SD_Host_PollTransferComplete(obj->base);
            }
        }

        Cy_SysLib_DelayUs(CY_HAL_SDHC_OPER_CMPLT_US);
    }

    if (response != NULL)
    {
        (void)Cy_SD_Host_GetResponse(obj->base, response, false);
    }

    /* Restore interrupts after transition */
    Cy_SD_Host_SetNormalInterruptMask(obj->base, regIntrSts);

    if (CY_SD_HOST_SUCCESS != result)
    {
        ret = CY_RSLT_TYPE_ERROR;
    }

    if (0u != (CY_SD_HOST_CARD_INTERRUPT & obj->irq_cause) )
    {
        uint32_t intrStsEn = Cy_SD_Host_GetNormalInterruptEnable(obj->base);

        /* This interrupt is disabled in interrupt handler so need to enable it here */
        Cy_SD_Host_SetNormalInterruptEnable(obj->base, (intrStsEn | CY_SD_HOST_CARD_INTERRUPT) );
    }

    return ret;
}

/** The sdio interrupt handler registration
 *
 * @param[in] obj         The SDIO object
 * @param[in] handler     The callback handler which will be invoked when the interrupt fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 */
void cyhal_sdio_register_irq(cyhal_sdio_t *obj, cyhal_sdio_irq_handler_t handler, void *handler_arg)
{
    uint8_t index = obj->resource.block_num;

    IRQn_Type irqn = CY_SDHC_IRQ_N[index];
    cy_sdio_config_structs[index] = obj;
    callbacks[index] = handler;
    callback_args[index] = handler_arg;

    /* Only enable if it's not already enabled */
    if (NVIC_GetEnableIRQ(irqn) == 0)
    {
        cy_stc_sysint_t irqCfg = {irqn, SDIO_IRQ_PRIORITY};

        Cy_SysInt_Init(&irqCfg, interrupts_dispatcher_table[index]);
        NVIC_EnableIRQ(irqn);
    }
    else
    {
        NVIC_SetPriority(irqn, SDIO_IRQ_PRIORITY);
    }
}

/** Configure sdio interrupt.
 *
 * @param[in] obj      The SDIO object
 * @param[in] event    The sdio IRQ type
 * @param[in] enable   Set to non-zero to enable events, or zero to disable them
 */
void cyhal_sdio_irq_enable(cyhal_sdio_t *obj, cyhal_sdio_irq_event_t event, bool enable)
{
    uint32_t interruptEnable = Cy_SD_Host_GetNormalInterruptEnable(obj->base);
    uint32_t interruptMask = Cy_SD_Host_GetNormalInterruptMask(obj->base);

    if (enable)
    {
        interruptEnable |= event;
        interruptMask   |= event;

        obj->irq_cause |= event;
    }
    else
    {
        interruptEnable &= ~(event);
        interruptMask &= ~(event);

        obj->irq_cause &= ~event;
    }

    Cy_SD_Host_SetNormalInterruptMask(obj->base, interruptMask);
    Cy_SD_Host_SetNormalInterruptEnable(obj->base, interruptEnable);
}

