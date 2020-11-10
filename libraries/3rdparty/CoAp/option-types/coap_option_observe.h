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
#ifndef COAP_OBSERVE_OPTION
#define COAP_OBSERVE_OPTION

#define OBSERVE_OPT_REGISTER (0)
#define OBSERVE_OPT_DEREGISTER (1)

CoAP_Result_t AddObserveOptionToMsg(CoAP_Message_t *msg, uint32_t val);
CoAP_Result_t GetObserveOptionFromMsg(CoAP_Message_t *msg, uint32_t *val);
CoAP_Result_t RemoveObserveOptionFromMsg(CoAP_Message_t *msg);
CoAP_Result_t UpdateObserveOptionInMsg(CoAP_Message_t *msg, uint32_t val);
CoAP_Observer_t *CoAP_AllocNewObserver();
CoAP_Result_t CoAP_FreeObserver(CoAP_Observer_t **pObserver);
CoAP_Result_t CoAP_AppendObserverToList(CoAP_Observer_t **pListStart, CoAP_Observer_t *pObserverToAdd);
CoAP_Result_t CoAP_UnlinkObserverFromList(CoAP_Observer_t **pListStart, CoAP_Observer_t *pObserverToRemove, bool FreeUnlinked);

#endif
