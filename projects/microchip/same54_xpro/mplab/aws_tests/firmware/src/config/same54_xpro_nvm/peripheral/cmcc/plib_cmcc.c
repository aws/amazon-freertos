/*******************************************************************************
   CMCC(Cortex M Cache Controller) Peripheral Library

  Company:
    Microchip Technology Inc.

  File Name:
    plib_cmcc.c

  Summary:
    CMCC Source File

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "device.h"
#include "peripheral/cmcc/plib_cmcc.h"

void CMCC_Disable (void )
{
    CMCC_REGS->CMCC_CTRL &=(~CMCC_CTRL_CEN_Msk);

    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
}

void CMCC_EnableICache (void )
{
    CMCC_REGS->CMCC_CTRL &= (~CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_CFG &= (~CMCC_CFG_ICDIS_Msk);
    CMCC_REGS->CMCC_CTRL = (CMCC_CTRL_CEN_Msk);
}

void CMCC_DisableICache (void )
{
    CMCC_REGS->CMCC_CTRL &= (~CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_CFG |= (CMCC_CFG_ICDIS_Msk);
    CMCC_REGS->CMCC_CTRL = (CMCC_CTRL_CEN_Msk);
}

void CMCC_EnableDCache (void )
{
    CMCC_REGS->CMCC_CTRL &= (~CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_CFG &= (~CMCC_CFG_DCDIS_Msk);
    CMCC_REGS->CMCC_CTRL = (CMCC_CTRL_CEN_Msk);
}

void CMCC_DisableDCache (void )
{
    CMCC_REGS->CMCC_CTRL &= (~CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_CFG |= (CMCC_CFG_DCDIS_Msk);
    CMCC_REGS->CMCC_CTRL = (CMCC_CTRL_CEN_Msk);
}

void CMCC_InvalidateAll (void )
{
    CMCC_REGS->CMCC_CTRL &= (~CMCC_CTRL_CEN_Msk);
    while((CMCC_REGS->CMCC_SR & CMCC_SR_CSTS_Msk) == CMCC_SR_CSTS_Msk)
    {
        /*Wait for the operation to complete*/
    }
    CMCC_REGS->CMCC_MAINT0 = CMCC_MAINT0_INVALL_Msk;
}

