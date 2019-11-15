/*******************************************************************************
  Ethernet Driver Library Source Code

Summary:
This file contains the source code for the Ethernet Driver library.

Description:
This library provides a low-level abstraction of the Ethernet module
on Microchip PIC32MX/PIC32MZ family microcontrollers with a convenient C language
interface.  It can be used to simplify low-level access to the module
without the necessity of interacting directly with the module's registers,
thus hiding differences from one microcontroller variant to another.
 *******************************************************************************/
//DOM-IGNORE-BEGIN
/*****************************************************************************
  Copyright (C) 2008-2018 Microchip Technology Inc. and its subsidiaries.

  Microchip Technology Inc. and its subsidiaries.

  Subject to your compliance with these terms, you may use Microchip software 
  and any derivatives exclusively with Microchip products. It is your 
  responsibility to comply with third party license terms applicable to your 
  use of third party software (including open source software) that may 
  accompany Microchip software.

  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR 
  PURPOSE.

  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
  BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE 
  FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN 
  ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
  THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *****************************************************************************/

//DOM-IGNORE-END


#include "./drv_eth_pic32_lib.h"

void DRV_ETH_RxBufferCountDecrement(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1SET =_ETHCON1_BUFCDEC_MASK;
}

void DRV_ETH_MIIResetEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxCFG1SET=_EMACxCFG1_SOFTRESET_MASK;
}

void DRV_ETH_MIIResetDisable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxCFG1CLR =_EMACxCFG1_SOFTRESET_MASK;
}

void DRV_ETH_MaxFrameLengthSet(DRV_ETHERNET_REGISTERS* eth, uint16_t MaxFrameLength)
{
    eth->EMACxMAXF.MACMAXF = (uint16_t) (_EMACxMAXF_MACMAXF_MASK & (MaxFrameLength << _EMACxMAXF_MACMAXF_POSITION));
}

void DRV_ETH_Disable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1CLR = _ETHCON1_ON_MASK;	    
}

void DRV_ETH_Enable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1SET = _ETHCON1_ON_MASK;	    
}

void DRV_ETH_TxRTSDisable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1CLR =_ETHCON1_TXRTS_MASK;    
}

void DRV_ETH_TxRTSEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1SET =_ETHCON1_TXRTS_MASK;
}

void DRV_ETH_RxDisable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1CLR =_ETHCON1_RXEN_MASK;
}

void DRV_ETH_RxEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1SET =_ETHCON1_RXEN_MASK;
}

bool DRV_ETH_EthernetIsBusy(DRV_ETHERNET_REGISTERS* eth)
{
    return (bool)(eth->ETHSTAT.BUSY); 						       
}

uint8_t DRV_ETH_RxPacketCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint8_t)(eth->ETHSTAT.BUFCNT); 
}

bool DRV_ETH_TransmitIsBusy(DRV_ETHERNET_REGISTERS* eth)
{
    return (bool)(eth->ETHSTAT.TXBUSY); 
}

bool DRV_ETH_ReceiveIsBusy(DRV_ETHERNET_REGISTERS* eth)
{
    return (bool)(eth->ETHSTAT.RXBUSY); 
}

void DRV_ETH_TxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* eth, uint8_t *txPacketDescStartAddr)
{
    eth->ETHTXST.TXSTADDR = ((uint32_t)txPacketDescStartAddr>>2);
}

uint8_t* DRV_ETH_TxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint8_t *)eth->ETHTXST.w;
}

void DRV_ETH_RxPacketDescAddrSet(DRV_ETHERNET_REGISTERS* eth, uint8_t *rxPacketDescStartAddr)
{
    eth->ETHRXST.RXSTADDR = ((uint32_t)rxPacketDescStartAddr>>2);
}

uint8_t* DRV_ETH_RxPacketDescAddrGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint8_t *)eth->ETHRXST.w;
}

void DRV_ETH_BackToBackIPGSet(DRV_ETHERNET_REGISTERS* eth, uint8_t backToBackIPGValue)
{
    eth->EMACxIPGTCLR = _EMAC1IPGT_B2BIPKTGP_MASK;
    eth->EMACxIPGTSET = _EMAC1IPGT_B2BIPKTGP_MASK & ((uint32_t)backToBackIPGValue << _EMAC1IPGT_B2BIPKTGP_POSITION);	 
}

void DRV_ETH_NonBackToBackIPG1Set(DRV_ETHERNET_REGISTERS* eth, uint8_t nonBackToBackIPGValue)
{
    eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP1_MASK;
    eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP1_MASK & ((uint32_t)nonBackToBackIPGValue << _EMACxIPGR_NB2BIPKTGP1_POSITION);	
}

void DRV_ETH_NonBackToBackIPG2Set(DRV_ETHERNET_REGISTERS* eth, uint8_t nonBackToBackIPGValue)
{
    eth->EMACxIPGRCLR = _EMACxIPGR_NB2BIPKTGP2_MASK;
    eth->EMACxIPGRSET = _EMACxIPGR_NB2BIPKTGP2_MASK & ((uint32_t)nonBackToBackIPGValue <<_EMACxIPGR_NB2BIPKTGP2_POSITION);	
}

void DRV_ETH_CollisionWindowSet(DRV_ETHERNET_REGISTERS* eth, uint8_t collisionWindowValue)
{
    eth->EMACxCLRT.CWINDOW = collisionWindowValue;
}

void DRV_ETH_ReTxMaxSet(DRV_ETHERNET_REGISTERS* eth, uint16_t retransmitMax)
{
    eth->EMACxCLRT.RETX = retransmitMax;												     
}

void DRV_ETH_RMIIResetEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxSUPPSET = _EMACxSUPP_RESETRMII_MASK; 
}

void DRV_ETH_RMIIResetDisable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxSUPP.RESETRMII = 0ul;
}

void DRV_ETH_RMIISpeedSet(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_RMII_SPEED RMIISpeed)
{
    eth->EMACxSUPP.SPEEDRMII = RMIISpeed ;	
}

void DRV_ETH_RxFiltersClr(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_RX_FILTERS rxFilters)
{
    eth->ETHRXFCCLR = (uint32_t)rxFilters & DRV_ETH_FILT_ALL_FILTERS;	
}

void DRV_ETH_RxFiltersSet(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_RX_FILTERS rxFilters)
{
    eth->ETHRXFCSET = ((uint32_t)rxFilters  & DRV_ETH_FILT_ALL_FILTERS);	
}

void DRV_ETH_PauseTimerSet(DRV_ETHERNET_REGISTERS* eth, uint16_t PauseTimerValue)
{
    eth->ETHCON1SET = _ETHCON1_PTV_MASK & ((uint32_t)PauseTimerValue << _ETHCON1_PTV_POSITION);				 
}

void DRV_ETH_RxFullWmarkSet(DRV_ETHERNET_REGISTERS* eth, uint8_t watermarkValue)
{
    eth->ETHRXWM.RXFWM = watermarkValue;			 
}

void DRV_ETH_RxEmptyWmarkSet(DRV_ETHERNET_REGISTERS* eth, uint8_t watermarkValue)
{
    eth->ETHRXWM.RXEWM = watermarkValue;		
}

void DRV_ETH_TxPauseEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxCFG1SET = _EMACxCFG1_TXPAUSE_MASK;
}

void DRV_ETH_RxPauseEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxCFG1SET = _EMACxCFG1_RXPAUSE_MASK;
}

void DRV_ETH_AutoFlowControlEnable(DRV_ETHERNET_REGISTERS* eth)
{
    eth->ETHCON1SET = _ETHCON1_AUTOFC_MASK;
}

void DRV_ETH_RxFiltersHTSet(DRV_ETHERNET_REGISTERS* eth, uint64_t htable)
{
    eth->ETHHT0SET = _ETHHT0_HTLOWER_MASK & ((uint32_t)htable);
    eth->ETHHT1SET = _ETHHT1_HTUPPER_MASK &((uint32_t)(htable>>32));
}

int DRV_ETH_RxSetBufferSize(DRV_ETHERNET_REGISTERS* eth, int rxBuffSize)
{
    rxBuffSize >>= 4;     // truncate
    if(!rxBuffSize)
    {
        return -1;
    }

    eth->ETHCON2CLR = _ETHCON2_RXBUF_SZ_MASK;
    eth->ETHCON2SET = _ETHCON2_RXBUF_SZ_MASK & ((uint32_t)rxBuffSize << _ETHCON2_RXBUF_SZ_POSITION);

    return rxBuffSize << 4;
}

void DRV_ETH_MACSetAddress(DRV_ETHERNET_REGISTERS* eth, uint8_t *bAddress)
{
    eth->EMACxSA0.w = (uint16_t)*(bAddress + 0) + ((uint16_t)*(bAddress + 1) << 8);
    eth->EMACxSA1.w = (uint16_t)*(bAddress + 2) + ((uint16_t)*(bAddress + 3) << 8);
    eth->EMACxSA2.w = (uint16_t)*(bAddress + 4) + ((uint16_t)*(bAddress + 5) << 8);
}

void DRV_ETH_MACGetAddress(DRV_ETHERNET_REGISTERS* eth, uint8_t *bAddress )
{
    *(bAddress + 0) = (uint8_t)eth->EMACxSA2.w;
    *(bAddress + 1) = (uint8_t)(eth->EMACxSA2.w >> 8);
    *(bAddress + 2) = (uint8_t)eth->EMACxSA1.w;
    *(bAddress + 3) = (uint8_t)(eth->EMACxSA1.w >> 8);
    *(bAddress + 4) = (uint8_t)eth->EMACxSA0.w;
    *(bAddress + 5) = (uint8_t)(eth->EMACxSA0.w >> 8);
}

uint16_t DRV_ETH_FramesTxdOkCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)eth->ETHFRMTXOK.FRMTXOKCNT;
}

uint16_t DRV_ETH_FramesRxdOkCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)eth->ETHFRMRXOK.FRMRXOKCNT;
}

uint16_t DRV_ETH_RxOverflowCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)eth->ETHRXOVFLOW.RXOVFLWCNT;
}

uint16_t DRV_ETH_FCSErrorCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)eth->ETHFCSERR.FCSERRCNT;
}

uint16_t DRV_ETH_AlignErrorCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (eth->ETHALGNERR.ALGNERRCNT);
}

uint16_t DRV_ETH_SingleCollisionCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)(eth->ETHSCOLFRM.SCOLFRMCNT);
}

uint16_t DRV_ETH_MultipleCollisionCountGet(DRV_ETHERNET_REGISTERS* eth)
{
    return (uint16_t)(eth->ETHMCOLFRM.MCOLFRMCNT);
}


void DRV_ETH_EventsClr(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_EVENTS eEvents )
{
    eth->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)eEvents << _ETHIRQ_w_POSITION);
}

void DRV_ETH_EventsEnableSet(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_EVENTS eEvents )
{
    eth->ETHIENSET = _ETHIEN_w_MASK & ((uint32_t)eEvents <<_ETHIEN_w_POSITION);
}

void DRV_ETH_EventsEnableClr(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_EVENTS eEvents)
{
    eth->ETHIENCLR = _ETHIEN_w_MASK & ((uint32_t)eEvents <<_ETHIEN_w_POSITION);
}

DRV_ETH_EVENTS DRV_ETH_EventsGet(DRV_ETHERNET_REGISTERS* eth)
{
    return  (DRV_ETH_EVENTS) eth->ETHIRQ.w;
}


void DRV_ETH_EventsClear(DRV_ETHERNET_REGISTERS* eth, DRV_ETH_EVENTS evmask)
{
    eth->ETHIRQCLR = _ETHIRQ_w_MASK & ((uint32_t)evmask << _ETHIRQ_w_POSITION);
}


bool DRV_ETH_IsEnabled(DRV_ETHERNET_REGISTERS* eth)
{
    return (bool)(eth->ETHCON1.ON) ;	
}

void DRV_ETH_MIIMNoPreEnable(DRV_ETHERNET_REGISTERS* eth)
{    	
	eth->EMACxMCFGSET = _EMACxMCFG_NOPRE_MASK;	
}

void DRV_ETH_MIIMNoPreDisable(DRV_ETHERNET_REGISTERS* eth)
{	
	eth->EMACxMCFGCLR = _EMACxMCFG_NOPRE_MASK;	
}

void DRV_ETH_MIIMScanIncrEnable(DRV_ETHERNET_REGISTERS* eth)
{    	
	eth->EMACxMCFGSET = _EMACxMCFG_SCANINC_MASK;		
}


void DRV_ETH_MIIMScanIncrDisable(DRV_ETHERNET_REGISTERS* eth)
{		
	eth->EMACxMCFGCLR = _EMACxMCFG_SCANINC_MASK;		
}

bool DRV_ETH_MIIMIsBusy(DRV_ETHERNET_REGISTERS* eth)
{		
	return (bool) (eth->EMACxMIND.MIIMBUSY);
}

void DRV_ETH_PHYAddressSet(DRV_ETHERNET_REGISTERS* eth, uint8_t phyAddr )
{	
	eth->EMACxMADR.PHYADDR = phyAddr;
}

void DRV_ETH_RegisterAddressSet(DRV_ETHERNET_REGISTERS* eth, uint8_t regAddr)
{	
	eth->EMACxMADR.REGADDR = regAddr;
}

void DRV_ETH_ClearDataValid(DRV_ETHERNET_REGISTERS* eth)
{
    eth->EMACxMINDSET = _EMACxMIND_NOTVALID_MASK;
}

void DRV_ETH_MIIMScanModeEnable(DRV_ETHERNET_REGISTERS* eth)
{		
	eth->EMACxMCMDSET = _EMACxMCMD_SCAN_MASK;
}

void DRV_ETH_MIIMScanModeDisable(DRV_ETHERNET_REGISTERS* eth)
{	
	eth->EMACxMCMDCLR = _EMACxMCMD_SCAN_MASK;
}

void DRV_ETH_MIIMWriteDataSet(DRV_ETHERNET_REGISTERS* eth, uint16_t writeData )
{
	eth->EMACxMWTD.MWTD =writeData ;  				 
                     
    // there's 2 clock cycles till busy is set for a write op
#if defined(__PIC32MZ__)
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
#else                     
    __asm__ __volatile__ ("nop; nop;"); 
#endif
}

void DRV_ETH_MIIMWriteStart(DRV_ETHERNET_REGISTERS* eth)
{	
	eth->EMACxMCMDCLR=_EMACxMCMD_READ_MASK;
}

void DRV_ETH_MIIMReadStart(DRV_ETHERNET_REGISTERS* eth)
{		
	eth->EMACxMCMDSET=_EMACxMCMD_READ_MASK;

    // there's 4 clock cycles till busy is set for a read op
#if defined(__PIC32MZ__)
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); 
#else
    __asm__ __volatile__ ("nop; nop; nop; nop;");
#endif
    
}

uint16_t DRV_ETH_MIIMReadDataGet(DRV_ETHERNET_REGISTERS* eth)
{    	
	return (uint16_t)(eth->EMACxMRDD.MRDD);								 
}

bool DRV_ETH_DataNotValid(DRV_ETHERNET_REGISTERS* eth)
{		
	return (bool) (eth->EMACxMIND.NOTVALID);
}

void DRV_ETH_MIIMResetEnable(DRV_ETHERNET_REGISTERS* eth)
{		
	eth->EMACxMCFGSET = _EMACxMCFG_RESETMGMT_MASK; 
}

void DRV_ETH_MIIMResetDisable(DRV_ETHERNET_REGISTERS* eth)
{	
	eth->EMACxMCFGCLR = _EMACxMCFG_RESETMGMT_MASK; 
}

void DRV_ETH_MIIMClockSet(DRV_ETHERNET_REGISTERS* eth , ETH_MIIM_CLK MIIMClock )
{
	eth->EMACxMCFG.CLKSEL = MIIMClock ;
}

