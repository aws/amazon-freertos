/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *
 * SD-SPI Protocol Standard
 *
 * $Id: sdspi.h 382882 2013-02-04 23:24:31Z xwei $
 */
#ifndef	_SD_SPI_H
#define	_SD_SPI_H

#define SPI_START_M		BITFIELD_MASK(1)	/* Bit [31] 	- Start Bit */
#define SPI_START_S		31
#define SPI_DIR_M		BITFIELD_MASK(1)	/* Bit [30] 	- Direction */
#define SPI_DIR_S		30
#define SPI_CMD_INDEX_M		BITFIELD_MASK(6)	/* Bits [29:24] - Command number */
#define SPI_CMD_INDEX_S		24
#define SPI_RW_M		BITFIELD_MASK(1)	/* Bit [23] 	- Read=0, Write=1 */
#define SPI_RW_S		23
#define SPI_FUNC_M		BITFIELD_MASK(3)	/* Bits [22:20]	- Function Number */
#define SPI_FUNC_S		20
#define SPI_RAW_M		BITFIELD_MASK(1)	/* Bit [19] 	- Read After Wr */
#define SPI_RAW_S		19
#define SPI_STUFF_M		BITFIELD_MASK(1)	/* Bit [18] 	- Stuff bit */
#define SPI_STUFF_S		18
#define SPI_BLKMODE_M		BITFIELD_MASK(1)	/* Bit [19] 	- Blockmode 1=blk */
#define SPI_BLKMODE_S		19
#define SPI_OPCODE_M		BITFIELD_MASK(1)	/* Bit [18] 	- OP Code */
#define SPI_OPCODE_S		18
#define SPI_ADDR_M		BITFIELD_MASK(17)	/* Bits [17:1] 	- Address */
#define SPI_ADDR_S		1
#define SPI_STUFF0_M		BITFIELD_MASK(1)	/* Bit [0] 	- Stuff bit */
#define SPI_STUFF0_S		0

#define SPI_RSP_START_M		BITFIELD_MASK(1)	/* Bit [7] 	- Start Bit (always 0) */
#define SPI_RSP_START_S		7
#define SPI_RSP_PARAM_ERR_M	BITFIELD_MASK(1)	/* Bit [6] 	- Parameter Error */
#define SPI_RSP_PARAM_ERR_S	6
#define SPI_RSP_RFU5_M		BITFIELD_MASK(1)	/* Bit [5] 	- RFU (Always 0) */
#define SPI_RSP_RFU5_S		5
#define SPI_RSP_FUNC_ERR_M	BITFIELD_MASK(1)	/* Bit [4] 	- Function number error */
#define SPI_RSP_FUNC_ERR_S	4
#define SPI_RSP_CRC_ERR_M	BITFIELD_MASK(1)	/* Bit [3] 	- COM CRC Error */
#define SPI_RSP_CRC_ERR_S	3
#define SPI_RSP_ILL_CMD_M	BITFIELD_MASK(1)	/* Bit [2] 	- Illegal Command error */
#define SPI_RSP_ILL_CMD_S	2
#define SPI_RSP_RFU1_M		BITFIELD_MASK(1)	/* Bit [1] 	- RFU (Always 0) */
#define SPI_RSP_RFU1_S		1
#define SPI_RSP_IDLE_M		BITFIELD_MASK(1)	/* Bit [0] 	- In idle state */
#define SPI_RSP_IDLE_S		0

/* SD-SPI Protocol Definitions */
#define SDSPI_COMMAND_LEN	6	/* Number of bytes in an SD command */
#define SDSPI_START_BLOCK	0xFE	/* SD Start Block Token */
#define SDSPI_IDLE_PAD		0xFF	/* SD-SPI idle value for MOSI */
#define SDSPI_START_BIT_MASK	0x80

#endif /* _SD_SPI_H */
