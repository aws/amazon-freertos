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
#ifndef SRC_COAP_COAP_RES_H_
#define SRC_COAP_COAP_RES_H_

typedef uint32_t (* CoAP_ResourceGetETag_fPtr_t)();

typedef bool (* WriteBuf_fn)(uint8_t* data, uint32_t len);


CoAP_Res_t* CoAP_FindResourceByUri(CoAP_Res_t* pResListToSearchIn, CoAP_option_t* pOptionsToMatch);
CoAP_Result_t CoAP_NotifyResourceObservers(CoAP_Res_t* pRes);
CoAP_Result_t CoAP_FreeResource(CoAP_Res_t** pResource);

void CoAP_PrintResource(CoAP_Res_t* pRes);
void CoAP_PrintAllResources();

void CoAP_InitResources();

CoAP_Result_t CoAP_NVsaveObservers(WriteBuf_fn writeBufFn);
CoAP_Result_t CoAP_NVloadObservers(uint8_t* pRawPage);

CoAP_Result_t CoAP_RemoveObserverFromResource(CoAP_Observer_t** pObserverList, Socket_t socketHandle, NetEp_t* pRemoteEP, CoAP_Token_t token);

#endif /* SRC_COAP_COAP_RESOURCES_H_ */
