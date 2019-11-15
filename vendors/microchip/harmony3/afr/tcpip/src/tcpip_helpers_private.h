/*******************************************************************************
  Header file for tcpip_helpers_private

  Company:
    Microchip Technology Inc.
    
  File Name:
    tcpip_helpers_private.h

  Summary:
    
  Description:
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*****************************************************************************
 Copyright (C) 2012-2018 Microchip Technology Inc. and its subsidiaries.

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

#ifndef __TCPIP_HELPERS_PRIVATE_H_
#define __TCPIP_HELPERS_PRIVATE_H_



unsigned char   TCPIP_Helper_FindCommonPrefix (const unsigned char * addr1, const unsigned char * addr2, unsigned char bytes);

TCPIP_MAC_POWER_MODE TCPIP_Helper_StringToPowerMode(const char* str);

const char*     TCPIP_Helper_PowerModeToString(TCPIP_MAC_POWER_MODE mode);

uint16_t        TCPIP_Helper_CalcIPChecksum(uint8_t* buffer, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_PacketChecksum(TCPIP_MAC_PACKET* pPkt, uint8_t* startAdd, uint16_t len, uint16_t seed);

uint16_t        TCPIP_Helper_ChecksumFold(uint32_t checksum);

uint16_t        TCPIP_Helper_PacketCopy(TCPIP_MAC_PACKET* pSrcPkt, uint8_t* pDest, uint8_t** pStartAdd, uint16_t len, bool srchTransport);

void            TCPIP_Helper_FormatNetBIOSName(uint8_t Name[16]);


// Protocols understood by the TCPIP_Helper_ExtractURLFields() function.  IMPORTANT: If you 
// need to reorder these (change their constant values), you must also reorder 
// the constant arrays in TCPIP_Helper_ExtractURLFields().
typedef enum
{
	PROTOCOL_HTTP = 0u,
	PROTOCOL_HTTPS,
	PROTOCOL_MMS,
	PROTOCOL_RTSP
} PROTOCOLS;

uint8_t         TCPIP_Helper_ExtractURLFields(uint8_t *vURL, PROTOCOLS *protocol,
                                 uint8_t *vUsername, uint16_t *wUsernameLen,
                                 uint8_t *vPassword, uint16_t *wPasswordLen,
                                 uint8_t *vHostname, uint16_t *wHostnameLen,
                                 uint16_t *wPort, uint8_t *vFilePath, uint16_t *wFilePathLen);


#endif  // __TCPIP_HELPERS_PRIVATE_H_
