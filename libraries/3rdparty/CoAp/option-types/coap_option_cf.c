#line __LINE__ "coap_option_cf.c"
/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/
#include "../coap.h"

CoAP_Result_t _rom CoAP_AddCfOptionToMsg(CoAP_Message_t* msg, uint16_t contentFormat)
{
	return CoAP_AppendUintOptionToList(&(msg->pOptionsList), OPT_NUM_CONTENT_FORMAT, contentFormat);
}

CoAP_Result_t _rom CoAP_AddAcceptOptionToMsg(CoAP_Message_t* msg, uint16_t contentFormat)
{
	return CoAP_AppendUintOptionToList(&(msg->pOptionsList), OPT_NUM_ACCEPT, contentFormat);
}

uint16_t _rom CoAP_GetAcceptOptionVal(CoAP_option_t* pAcceptOpt)
{
	if(pAcceptOpt == NULL) return 0;
	if(pAcceptOpt->Number != OPT_NUM_ACCEPT) return 0;
	if(pAcceptOpt->Length == 0 || pAcceptOpt->Length > 2) return 0;

	uint16_t retVal = 0;
	if(pAcceptOpt->Length == 1)
	{
		retVal = pAcceptOpt->Value[0];
	}
	else if(pAcceptOpt->Length == 2)
	{
		retVal = pAcceptOpt->Value[0];
		retVal |= pAcceptOpt->Value[1] << 8;
	}

	return retVal;
}

uint16_t _rom CoAP_GetAcceptOptionValFromMsg(CoAP_Message_t* pMsg)
{
	uint16_t retVal = 0;
	for(CoAP_option_t* pOpt =pMsg->pOptionsList ; pOpt != NULL; pOpt = pOpt->next) {
		if(pOpt->Number != OPT_NUM_ACCEPT)
			continue;
		retVal = CoAP_GetAcceptOptionVal(pOpt);
	}
	return retVal;
}