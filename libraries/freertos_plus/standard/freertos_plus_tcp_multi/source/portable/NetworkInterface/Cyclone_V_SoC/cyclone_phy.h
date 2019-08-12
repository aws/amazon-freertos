/*
 * cyclone_phy.h
 */

#ifndef CYCLONE_PHY_INIT_H

#define CYCLONE_PHY_INIT_H

/* ------------------------------------------------------------------------------------------------ */
/* Rate values:																						*/
/*		  10 :   10 Mbps full duplex			 													*/
/*		  11 :   10 Mbps half duplex			 													*/
/*		 101 :  100 Mbps full duplex			 													*/
/*		 101 :  100 Mbps half duplex			 													*/
/*		1000 : 1000 Mbps full duplex			 													*/
/*		1001 : 1000 Mbps half duplex		 														*/
/*		 < 0 : auto-negotiation (only as the argument, not for the return value)					*/
/* ------------------------------------------------------------------------------------------------ */
int cyclone_phy_init( int iMacID, int Rate );

extern uint32_t ulUsePHYAddress;


#endif
