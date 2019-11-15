/*******************************************************************************
  System Configuration Header

  File Name:
    configuration.h

  Summary:
    Build-time configuration header for the system defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

#include "user.h"
#include "toolchain_specifics.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************

#define SYS_CMD_ENABLE
#define SYS_CMD_DEVICE_MAX_INSTANCES       SYS_CONSOLE_DEVICE_MAX_INSTANCES
#define SYS_CMD_PRINT_BUFFER_SIZE          1024
#define SYS_CMD_BUFFER_DMA_READY
#define SYS_CMD_REMAP_SYS_CONSOLE_MESSAGE
/* Command System Service RTOS Configurations*/
#define SYS_CMD_RTOS_STACK_SIZE                256
#define SYS_CMD_RTOS_TASK_PRIORITY             1


#define SYS_DEBUG_ENABLE
#define SYS_DEBUG_GLOBAL_ERROR_LEVEL       SYS_ERROR_DEBUG
#define SYS_DEBUG_PRINT_BUFFER_SIZE        200
#define SYS_DEBUG_BUFFER_DMA_READY
#define SYS_DEBUG_USE_CONSOLE


/* TIME System Service Configuration Options */
#define SYS_TIME_INDEX_0                     0
#define SYS_TIME_MAX_TIMERS                  5
#define SYS_TIME_HW_COUNTER_WIDTH            16
#define SYS_TIME_HW_COUNTER_PERIOD           65535U
#define SYS_TIME_HW_COUNTER_HALF_PERIOD		 (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY         300000000
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES      (900)

/* Console System Service Configuration Options */
#define SYS_CONSOLE_DEVICE_MAX_INSTANCES   1
#define SYS_CONSOLE_UART_MAX_INSTANCES     1


/* RX queue size has one additional element for the empty spot needed in circular queue */
#define SYS_CONSOLE_UART_RD_QUEUE_DEPTH_IDX0    11

/* TX queue size has one additional element for the empty spot needed in circular queue */
#define SYS_CONSOLE_UART_WR_QUEUE_DEPTH_IDX0    65
#define SYS_CONSOLE_BUFFER_DMA_READY



// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************


/*** MIIM Driver Configuration ***/
#define DRV_MIIM_ETH_MODULE_ID              GMAC_BASE_ADDRESS
#define DRV_MIIM_INSTANCES_NUMBER           1
#define DRV_MIIM_INSTANCE_OPERATIONS        4
#define DRV_MIIM_INSTANCE_CLIENTS           2
#define DRV_MIIM_CLIENT_OP_PROTECTION   false
#define DRV_MIIM_COMMANDS   false
#define DRV_MIIM_DRIVER_OBJECT              DRV_MIIM_OBJECT_BASE_Default
#define DRV_MIIM_DRIVER_INDEX               DRV_MIIM_INDEX_0              


/* MIIM RTOS Configurations*/
#define DRV_MIIM_RTOS_STACK_SIZE           1024
#define DRV_MIIM_RTOS_TASK_PRIORITY             1




// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
/*** TCPIP MAC Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY				1
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY				1
#define TCPIP_GMAC_RX_BUFF_SIZE_DUMMY				    	64
#define TCPIP_GMAC_TX_BUFF_SIZE_DUMMY				    	64

/*** QUEUE 0 Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE0				10
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE0				10
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE0				    	1536
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE0				    	1536
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE0				   		12
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE0				1
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE0					1

/*** QUEUE 1 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE1				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE1				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE1				   		1
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE1				0
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE1					0

/*** QUEUE 2 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE2				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE2				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE2				   		1
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE2				0
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE2					0

/*** QUEUE 3 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE3				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE3				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE3				   		1
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE3				0
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE3					0

/*** QUEUE 4 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE4				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE4				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE4				   		1
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE4				0
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE4					0

/*** QUEUE 5 Disabled; Dummy Configuration ***/
#define TCPIP_GMAC_TX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_TX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_DESCRIPTORS_COUNT_QUE5				TCPIP_GMAC_RX_DESCRIPTORS_COUNT_DUMMY
#define TCPIP_GMAC_RX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_RX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_TX_BUFF_SIZE_QUE5				    	TCPIP_GMAC_TX_BUFF_SIZE_DUMMY
#define TCPIP_GMAC_RX_BUFF_COUNT_QUE5				   		1
#define TCPIP_GMAC_RX_BUFF_COUNT_THRESHOLD_QUE5				0
#define TCPIP_GMAC_RX_BUFF_ALLOC_COUNT_QUE5					0

#define TCPIP_GMAC_RX_MAX_FRAME		    			1536
#define TCPIP_GMAC_RX_FILTERS                       \
                                                    TCPIP_MAC_RX_FILTER_TYPE_BCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_MCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_UCAST_ACCEPT |\
                                                    TCPIP_MAC_RX_FILTER_TYPE_CRC_ERROR_REJECT |\
                                                    0
#define TCPIP_GMAC_ETH_OPEN_FLAGS       			\
                                                    TCPIP_ETH_OPEN_AUTO |\
                                                    TCPIP_ETH_OPEN_FDUPLEX |\
                                                    TCPIP_ETH_OPEN_HDUPLEX |\
                                                    TCPIP_ETH_OPEN_100 |\
                                                    TCPIP_ETH_OPEN_10 |\
                                                    TCPIP_ETH_OPEN_MDIX_AUTO |\
                                                    TCPIP_ETH_OPEN_RMII |\
                                                    0

#define TCPIP_INTMAC_MODULE_ID		    			GMAC_BASE_ADDRESS
#define TCPIP_INTMAC_PERIPHERAL_CLK  				150000000

#define DRV_GMAC_INSTANCES_NUMBER				1
#define DRV_GMAC_NUMBER_OF_QUEUES				6
#define DRV_GMAC_CLIENTS_NUMBER					1
#define DRV_GMAC_INDEX	    	    				1
#define DRV_GMAC_PERIPHERAL_ID					1
#define DRV_GMAC_INTERRUPT_SOURCE				GMAC_IRQn

#define DRV_GMAC_INTERRUPT_MODE        				true
#define DRV_GMAC_RMII_MODE					0




#define TCPIP_INTMAC_PHY_CONFIG_FLAGS     			\
                                                    DRV_ETHPHY_CFG_RMII | \
													0                                                    

#define TCPIP_INTMAC_PHY_LINK_INIT_DELAY  			500
#define TCPIP_INTMAC_PHY_ADDRESS		    			0
#define DRV_ETHPHY_INSTANCES_NUMBER					1
#define DRV_ETHPHY_CLIENTS_NUMBER					1
#define DRV_ETHPHY_INDEX		        			1
#define DRV_ETHPHY_PERIPHERAL_ID					1
#define DRV_ETHPHY_NEG_INIT_TMO		    			1
#define DRV_ETHPHY_NEG_DONE_TMO		    			2000
#define DRV_ETHPHY_RESET_CLR_TMO					500
#define DRV_ETHPHY_USE_DRV_MIIM                     true




// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // CONFIGURATION_H
/*******************************************************************************
 End of File
*/
