#line __LINE__ "coap_option_ETag.c"
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




CoAP_Result_t _rom AddETagOptionToMsg(CoAP_Message_t* msg, uint8_t* pData, uint32_t size)
{
	uint32_t hash=0;
	uint32_t i;

	//TODO use a hash function
	for(i=0; i < size; i++)
	{
		hash+= pData[i];
	}

	uint8_t wBuf[4];
	/*
	wBuf[0]=hash & 0xff;
	wBuf[1] = (hash >> 8) & 0xff;
	wBuf[2] = (hash >> 16) & 0xff;
	wBuf[3] = (hash >> 24) & 0xff;
	*/

	wBuf[0]=1;
	wBuf[1] = 2;
	wBuf[2] = 3;
	wBuf[3] = 4;

	return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_ETAG,wBuf, 4);
}

CoAP_Result_t _rom GetETagOptionFromMsg(CoAP_Message_t* msg, uint8_t* val, uint8_t* pLen) { //len  [1..8]


	CoAP_option_t* pOpts = msg->pOptionsList;
	*val = 0;

	while(pOpts != NULL) {
		if(pOpts->Number == OPT_NUM_ETAG) {

			if(pOpts->Length == 0 || pOpts->Length > 8) { //implicit val = 0 (register)
				return COAP_BAD_OPTION_LEN;
			} else {
				*pLen = (uint8_t)pOpts->Length;
				int i;
				for(i=0; i< pOpts->Length; i++) {
					val[i]=pOpts->Value[i];
				}
			}
			return COAP_OK;
		}

		pOpts = pOpts->next;
	}
	return COAP_NOT_FOUND;
}

CoAP_Result_t _rom Add64BitETagOptionToMsg(CoAP_Message_t* msg, uint64_t val) {
	uint8_t wBuf[8];
	uint8_t i = 0;

	for(i=0; i<8; i++){
		wBuf[i]= val & 0xff;
		val = val >> 8;
		if(!val) break;
	}

	return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_ETAG, wBuf, i+1);
}

CoAP_Result_t _rom Get64BitETagOptionFromMsg(CoAP_Message_t* msg, uint64_t* pVal) {
	CoAP_Result_t res;
	uint8_t wBuf[8];
	uint8_t pLen = 0;

	res = GetETagOptionFromMsg(msg, wBuf, &pLen);

	if(res != COAP_OK) return res;

	*pVal = 0;
	int i;
	for(i= 0; i<pLen; i++) {
		*pVal |= ((uint64_t)wBuf[i]) << (i*8);
	}

	return COAP_OK;
}




