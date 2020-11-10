#line __LINE__ "coap_option_observe.c"
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

CoAP_Result_t _rom AddObserveOptionToMsg(CoAP_Message_t* msg, uint32_t val)
{
	uint8_t wBuf[3];
	wBuf[2] = val & 0xff;
	wBuf[1] = (val >> 8) & 0xff;
	wBuf[0] = (val >> 16) & 0xff;

	if (val > 0xffff) {
		wBuf[0] = (val >> 16) & 0xff;
		wBuf[1] = (val >> 8) & 0xff;
		wBuf[2] = val & 0xff;
		return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_OBSERVE, wBuf, 3);
	} else if (val > 0xff) {
		wBuf[0] = (val >> 8) & 0xff;
		wBuf[1] = val & 0xff;
		return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_OBSERVE, wBuf, 2);
	} else if (val > 0) {
		wBuf[0] = val & 0xff;
		return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_OBSERVE, wBuf, 1);
	} else { //val == 0
		return CoAP_AppendOptionToList(&(msg->pOptionsList), OPT_NUM_OBSERVE, NULL, 0);
	}
}

CoAP_Result_t _rom RemoveObserveOptionFromMsg(CoAP_Message_t* msg) {
	CoAP_option_t* pOpt;
	for (pOpt = msg->pOptionsList; pOpt != NULL; pOpt = pOpt->next) {
		if (pOpt->Number == OPT_NUM_OBSERVE) {
			CoAP_RemoveOptionFromList(&(msg->pOptionsList), pOpt);
			return COAP_OK;
		}
	}
	return COAP_NOT_FOUND;
}

CoAP_Result_t _rom UpdateObserveOptionInMsg(CoAP_Message_t* msg, uint32_t val)
{
	RemoveObserveOptionFromMsg(msg);
	return AddObserveOptionToMsg(msg, val);
}

CoAP_Result_t _rom GetObserveOptionFromMsg(CoAP_Message_t* msg, uint32_t* val) {

	CoAP_option_t* pOpts = msg->pOptionsList;
	*val = 0;

	while (pOpts != NULL) {
		if (pOpts->Number == OPT_NUM_OBSERVE) {

			if (pOpts->Length == 0) { //implicit val = 0 (register)
				return COAP_OK;
			} else {
				if (pOpts->Length == 1) {
					*val |= (uint32_t) (pOpts->Value[0]);
				} else if (pOpts->Length == 2) {
					*val |= (((uint32_t) pOpts->Value[0]) << 8) | ((uint32_t) pOpts->Value[1]);
				} else if (pOpts->Length == 3) {
					*val |= (((uint32_t) pOpts->Value[0]) << 16) | (((uint32_t) pOpts->Value[1]) << 8) | ((uint32_t) pOpts->Value[2]);
				} else {
					return COAP_BAD_OPTION_VAL;
				}
			}
			return COAP_OK;
		}

		pOpts = pOpts->next;
	}
	return COAP_NOT_FOUND;
}

CoAP_Observer_t* _rom CoAP_AllocNewObserver()
{
	CoAP_Observer_t* newObserver = (CoAP_Observer_t*) (pvPortMalloc(sizeof(CoAP_Observer_t)));
	if (newObserver == NULL) {
		return NULL;
	}

	memset(newObserver, 0, sizeof(CoAP_Observer_t));
	return newObserver;
}

CoAP_Result_t _rom CoAP_FreeObserver(CoAP_Observer_t** pObserver)
{
	configPRINTF(("Releasing pObserver\r\n"));
	//coap_mem_stats();

	CoAP_FreeOptionList((*pObserver)->pOptList);
	vPortFree((void*) (*pObserver));
	*pObserver = NULL;

	return COAP_OK;
}

//does not copy!
CoAP_Result_t _rom CoAP_AppendObserverToList(CoAP_Observer_t** pListStart, CoAP_Observer_t* pObserverToAdd)
{
	if (pObserverToAdd == NULL)
		return COAP_ERR_ARGUMENT;

	if (*pListStart == NULL) //List empty? create new first element
	{
		*pListStart = pObserverToAdd;
		(*pListStart)->next = NULL;
	}
	else //append new element at end
	{
		CoAP_Observer_t* pObs = *pListStart;
		while (pObs->next != NULL) {
			pObs = pObs->next;
		}

		pObs->next = pObserverToAdd;
		pObs = pObs->next;
		pObs->next = NULL;
	}
	return COAP_OK;
}

CoAP_Result_t _rom CoAP_UnlinkObserverFromList(CoAP_Observer_t** pListStart, CoAP_Observer_t* pObserverToRemove, bool FreeUnlinked)
{
	CoAP_Observer_t* currP;
	CoAP_Observer_t* prevP;

	// For 1st node, indicate there is no previous.
	prevP = NULL;

	//Visit each node, maintaining a pointer to
	//the previous node we just visited.
	for (currP = *pListStart; currP != NULL; prevP = currP, currP = currP->next) {

		if (currP == pObserverToRemove) {  // Found it.
			if (prevP == NULL) {
				//Fix beginning pointer.
				*pListStart = currP->next;
			} else {
				//Fix previous node's next to
				//skip over the removed node.
				prevP->next = currP->next;
			}

			// Deallocate the node.
			if (FreeUnlinked) {
				CoAP_FreeObserver(&currP);
			}
			//Done searching.
			return COAP_OK;
		}
	}
	return COAP_OK;
}

