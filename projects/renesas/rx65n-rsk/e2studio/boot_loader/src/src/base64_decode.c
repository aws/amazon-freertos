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
* Copyright (C) 2015-2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : base64_decode.c
* Description  : base64 decode function.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 27.06.2015 1.00    First Release
*         : 15.05.2017 1.01    Clean up source file
***********************************************************************************************************************/
#include <string.h>
#include "base64_decode.h"

#if SECURE_BOOT
#pragma section SECURE_BOOT
#endif

/* base64 table */
const static char base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/=";

/***********************************************************************************************************************
* Function Name: base64_decode
* Description  : http://apis.jpn.ph/fswiki/wiki.cgi?page=ScrapCode%2FC%2FBASE64
* Arguments    : *source
*                *result
*                size
* Return Value : j
***********************************************************************************************************************/
uint32_t base64_decode(uint8_t *source, uint8_t *result, uint32_t size)
{
    uint8_t ind1;
    uint8_t ind2;
    uint8_t ind3;
    uint8_t ind4;
    uint32_t i;
    uint32_t j;
    uint8_t tmp[3];
    uint8_t *ptr;

    for( i = 0, j = 0 ; i < size; i += 4 )
    {
        /* get the character code */
        ind1 = ('=' == (*((source + i) + 0))) ? 0 : (strchr(base64, *((source + i) + 0)) - base64);
        ind2 = ('=' == (*((source + i) + 1))) ? 0 : (strchr(base64, *((source + i) + 1)) - base64);
        ind3 = ('=' == (*((source + i) + 2))) ? 0 : (strchr(base64, *((source + i) + 2)) - base64);
        ind4 = ('=' == (*((source + i) + 3))) ? 0 : (strchr(base64, *((source + i) + 3)) - base64);

		/* decode */
		tmp[0] = (uint8_t)( ((ind1 & 0x3f) << 2) | ((ind2 & 0x30) >> 4) );
		tmp[1] = (uint8_t)( ((ind2 & 0x0f) << 4) | ((ind3 & 0x3c) >> 2) );
		tmp[2] = (uint8_t)( ((ind3 & 0x03) << 6) | ((ind4 & 0x3f) >> 0) );

		ptr = strstr(source, "==");
		if(ptr == (source + i))
		{
			/* nothing to do */
		}
		else if(ptr == (source + i) + 2)
		{
			result[j+0] = tmp[0];
			j += 1;
		}
		else if(ptr == (source + i) + 3)
		{
			result[j+0] = tmp[0];
			result[j+1] = tmp[1];
			j += 2;
		}
		else
		{
			result[j+0] = tmp[0];
			result[j+1] = tmp[1];
			result[j+2] = tmp[2];
			j += 3;
		}
    }

    return j;
}

#if SECURE_BOOT
#pragma section
#endif
