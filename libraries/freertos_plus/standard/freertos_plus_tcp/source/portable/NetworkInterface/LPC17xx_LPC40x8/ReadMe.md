# About

This is a driver published on [https://sourceforge.net/p/freertos/discussion/382005/thread/0d4d999eab/#2b40](https://sourceforge.net/p/freertos/discussion/382005/thread/0d4d999eab/#2b40) by Peter Wilks, and 
modified for use on LPC17xx, working with the last version of LPCOPen.

The tests of this driver are made using LPC1788 and PHY SMSC87x0 with the last version of LPCOpen of LPC17xx/LPC40xx.

I only changed for my own need and publish for other developers.

Recommened settings for LPC17xx/LPC40xx Network Interface:

#define iplpcNUM_RX_DESCRIPTORS				4
#define iplpcNUM_TX_DESCRIPTORS				4
#define iplpcLINK_RECEIVE_PERIOD			5
#define iplpcLINK_POLL_PERIOD				5
#define iplpcETH_RX_TASK_PRIORITY			5
