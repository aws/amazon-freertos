/*******************************************************************************
  KSZ8081 definitions

  Company:
    Microchip Technology Inc.
    
  File Name:
    drv_extphy_ksz8091.h

  Summary:
    KSZ8091 definitions

  Description:
    This file provides the KSZ8091 definitions.

*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2017-2018 Microchip Technology Inc. and its subsidiaries.

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

// DOM-IGNORE-END

#ifndef _KSZ8091_H_

#define _KSZ8091_H_

// define the type of PHY: KSZ8091RNB (RMII) if 1, else KSZ8091MNX (MII)
#define KSZ8091RNB      1

typedef enum
{
	/*
	// basic registers, across all registers: 0-8
	PHY_REG_BMCON		= 0,
	PHY_REG_BMSTAT		= 1,
	// extended registers: 2-15
	PHY_REG_PHYID1		= 2,
	PHY_REG_PHYID2		= 3,
	PHY_REG_ANAD		= 4,
	PHY_REG_ANLPAD		= 5,
	PHY_REG_ANEXP		= 6,
	PHY_REG_ANNPTR		= 7,
	PHY_REG_ANLPRNP		= 8,
	*/
	// specific vendor registers: 16-31
	PHY_REG_DIGITAL_CONTROL     = 0x10,
	PHY_REG_AFE_CONTROL         = 0x11,
	PHY_REG_RXER_COUNTER        = 0x15,
	PHY_REG_STRAP_OVERRIDE      = 0x16,
	PHY_REG_STRAP_STATUS        = 0x17,
	PHY_REG_EXPANDED_CONTROL    = 0x18,
	PHY_REG_INTERUPT            = 0x1b,
	PHY_REG_LINKMD_CONTROL      = 0x1d,
	PHY_REG_PHY_CONTROL_1       = 0x1e,
	PHY_REG_PHY_CONTROL_2       = 0x1f,
	//
	//PHY_REGISTERS		= 32	// total number of registers
}ePHY_VENDOR_REG_KSZ8091;
// updated version of ePHY_REG


// vendor registers
//
typedef union {
  struct {    
    unsigned            :4;  
    unsigned PLL_OFF    :1;  
    unsigned            :11;
    };
  unsigned short w:16;
} __KSZ8091_DigitalControlBits_t;	

typedef union {
  struct {    
    unsigned            :5;
    unsigned SLOW_OSC   :1;
    unsigned            :10;
  };
  unsigned short w:16;
} __KSZ8091_AFEControlBits_t;	


typedef union {
  struct {
    unsigned short rxerCount:16;
  };
  unsigned short w:16;
} __KSZ8091_RXERCounterBits_t;	


typedef union {
  struct {
    unsigned                :1;
    unsigned RMII_OVR       :1;
    unsigned                :3;
    unsigned NAND_TREE_OVR  :1;
    unsigned RMII_B2B_OVR   :1;
    unsigned                :1;
    unsigned                :1;
    unsigned BCAST_OFF_OVR  :1;
    unsigned                :1;
    unsigned                :4;
    unsigned                :1;
  };
  unsigned short w:16;
} __KSZ8091_StrapOverrideBits_t;	


typedef union {
  struct {
    unsigned                :1;
    unsigned RMII_STAT      :1;
    unsigned                :3;
    unsigned NAND_TREE_STAT :1;
    unsigned RMII_B2B_STAT  :1;
    unsigned                :1;
    unsigned                :1;
    unsigned BCAST_OFF_STAT :1;
    unsigned                :3;
    unsigned PHY_ADD        :3;
  };
  unsigned short w:16;
} __KSZ8091_StrapStatusBits_t;	

typedef union {
  struct {
    unsigned                :11;
    unsigned EDPD_DISABLE   :1;
    unsigned                :4;
  };
  unsigned short w:16;
} __KSZ8091_ExpandedControlBits_t;	


typedef union {
  struct {    
    unsigned LINK_UP_INTERRUPT                  :1;
    unsigned REMOTE_FAULT_INTERRUPT             :1;
    unsigned LINK_DOWN_INTERRUPT                :1;
    unsigned LINK_PARTNER_ACK_INTERRUPT         :1;
    unsigned PARALLEL_DETECT_FAULT_INTERRUPT    :1;
    unsigned PAGE_RECEIVE_INTERRUPT             :1;
    unsigned RECEIVE_ERROR_INTERRUPT            :1;
    unsigned JABBER_INTERRUPT                   :1;
    unsigned LINK_UP_ENABLE                     :1;
    unsigned REMOTE_FAULT_ENABLE                :1;
    unsigned LINK_DOWN_ENABLE                   :1;
    unsigned LINK_PARTNER_ACK_ENABLE            :1;
    unsigned PARALLEL_DETECT_FAULT_ENABLE       :1;
    unsigned PAGE_RECEIVE_ENABLE                :1;
    unsigned RECEIVE_ERROR_ENABLE               :1;
    unsigned JABBER_ENABLE                      :1;
  };
  unsigned short w:16;
} __KSZ8091_InterruptBits_t;	

typedef union {
	struct {
        unsigned CABLE_FAULT_COUNT  :9;
        unsigned                    :3;
		unsigned SHORT_CABLE        :1;
		unsigned CABLE_DIAG_RESULT  :2;
		unsigned CABLE_DIAG_ENABLE  :1;
	};
    unsigned short w:16;
} __KSZ8091_LinkMDControlBits_t;

typedef union {
	struct {
		unsigned OPERATION_MODE: 3;
		unsigned PHY_ISOLATE    :1;
		unsigned ENERGY_DETECT  :1;
		unsigned MDI_STATE      :1;
		unsigned                :1;
		unsigned POLARITY_STAT  :1;
		unsigned LINK_STAT      :1;
		unsigned ENABLE_PAUSE   :1;
        unsigned                :6;
	};
    unsigned short w:16;
} __KSZ8091_PhyControl1Bits_t;

typedef union {
	struct {
		unsigned DATA_SCRAMBLER     :1;
		unsigned SQE_TEST           :1;
		unsigned REMOTE_LOOPBACK    :1;
		unsigned DISABLE_TRANSMIT   :1;
		unsigned LED_MODE           :2;
		unsigned                    :1;
        unsigned RMII_CLOCK_SELECT  :1;
		unsigned JABBER_ENABLE      :1;
		unsigned INTERUPT_LEVEL     :1;
		unsigned POWER_SAVING       :1;
		unsigned FORCE_LINK         :1;
		unsigned                    :1;
		unsigned PAIR_SWAP_DISABLE  :1;
		unsigned MDI_X_SELECT       :1;
		unsigned HP_MDIX            :1;
	};
    unsigned short w: 16;
} __KSZ8091_PhyControl2Bits_t;



#endif  // _KSZ8091_H_

