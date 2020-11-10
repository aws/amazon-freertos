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
#ifndef COAP_CF_OPTION
#define COAP_CF_OPTION

CoAP_Result_t CoAP_AddCfOptionToMsg(CoAP_Message_t* msg, uint16_t cf);
CoAP_Result_t CoAP_AddAcceptOptionToMsg(CoAP_Message_t* msg, uint16_t contentFormat);

uint16_t CoAP_GetAcceptOptionVal(CoAP_option_t* pAcceptOpt);
uint16_t CoAP_GetAcceptOptionValFromMsg(CoAP_Message_t* pMsg);
typedef enum
{
	COAP_CF_TEXT_PLAIN = 0,
	COAP_CF_LINK_FORMAT = 40,
	COAP_CF_APP_XML = 41,
	COAP_CF_OCTET_STREAM = 42,
	COAP_CF_EXI = 47,
	COAP_CF_JSON = 50,
	COAP_CF_CBOR = 60
}CoAP_ContentFormat_t;

#endif
