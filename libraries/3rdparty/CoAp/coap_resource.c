#line __LINE__ "coap_resource.c"
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
#include "coap.h"
#include "liblobaro_coap.h"

static CoAP_Res_t* pResList = NULL;
static uint32_t ResListMembers = 0;

uint8_t TempPage[2048];


/**
 * Save resource and observe information as options to non volatile storage
 *
 * @param writeBufFn a function that can write bytes to non volatile memory
 * @return
 */
CoAP_Result_t _rom CoAP_NVsaveObservers(WriteBuf_fn writeBufFn) {
	CoAP_Res_t* pList = pResList; //List of internal resources
	CoAP_option_t* pOptList = NULL;
	uint8_t* pTempPage = TempPage;
	uint32_t TotalPageBytes = 0;
	bool UriPathHasBeenEncoded = false; //uri path options are only encoded one time for each resource and not for every observer

	while (pList != NULL) { //iterate over all resources
		UriPathHasBeenEncoded = false; //reset for next resource

		if (pList->pListObservers != NULL) { //Resource has active observers

			CoAP_Observer_t* pObserverList = pList->pListObservers;

			while (pObserverList != NULL) { //iterate over all observers of this resource

				//Store Uri of external Oberserver as option
				CoAP_AppendOptionToList(&pOptList, OPT_NUM_URI_HOST, (uint8_t*) &(pObserverList->Ep), sizeof(NetEp_t)); //IP+Port of external Observer
				CoAP_AppendOptionToList(&pOptList, OPT_NUM_URI_PORT, (uint8_t*) &(pObserverList->socketHandle), sizeof(Socket_t)); //socketHandle as pseudo "Port"

				CoAP_option_t* pOptionsTemp = pList->pUri;

				//Copy uri-path option of resource
				if (UriPathHasBeenEncoded == false) {
					while (pOptionsTemp != NULL) {
						CoAP_CopyOptionToList(&pOptList, pOptionsTemp); //todo check for sufficent memory or implement cross linking to save memory
						pOptionsTemp = pOptionsTemp->next;
					}
					UriPathHasBeenEncoded = true; //store uri path of resource only once
				}

				//Also copy any other options of observe structure (e.g. uri-query)
				pOptionsTemp = pObserverList->pOptList;
				while (pOptionsTemp != NULL) {
					CoAP_CopyOptionToList(&pOptList, pOptionsTemp); //todo check for sufficent memory or implement cross linking to save memory
					pOptionsTemp = pOptionsTemp->next;
				}

				CoAP_AppendOptionToList(&pOptList, OPT_NUM_LOBARO_TOKEN_SAVE, (uint8_t*) &(pObserverList->Token), 8);

				uint16_t BytesWritten = 0;
				pack_OptionsFromList(pTempPage, &BytesWritten, pOptList);

				pTempPage[BytesWritten] = 0xff; //add pseudo "payload"-marker to make reparsing easier
				TotalPageBytes += BytesWritten + 1;
				pTempPage = pTempPage + TotalPageBytes;

				configPRINTF(("Wrote Observer Option List:\r\n"));
				CoAP_printOptionsList(pOptList);

				CoAP_FreeOptionList(pOptList); //free options
				pObserverList = pObserverList->next;
			}
		}

		pList = pList->next;
	}

	pTempPage = TempPage;
	configPRINTF(("writing: %d bytes to flash\r\n", TotalPageBytes));
	writeBufFn(pTempPage, TotalPageBytes);
	return COAP_OK;
}

/**
 * Load and attach observers
 * @param pRawPage pointer to the non volatile memory to load the observer from
 * @return
 */
CoAP_Result_t _rom CoAP_NVloadObservers(uint8_t* pRawPage) {
	CoAP_option_t* pOptList = NULL;
	CoAP_Res_t* pRes = NULL;
	CoAP_Res_t* pResTemp = NULL;

	while (parse_OptionsFromRaw(pRawPage, 2048, &pRawPage, &pOptList) == COAP_OK) { //finds "payload-marker" and sets pointer to its beginning. in this context this is the next stored observe dataset
		configPRINTF(("found flash stored options:\r\n"));
		CoAP_printOptionsList(pOptList);

		//Map Stored option to resource
		pResTemp = CoAP_FindResourceByUri(pResList, pOptList);
		if (pResTemp != NULL) pRes = pResTemp;
		else {
			configPRINTF(("- Observed Resource not found!\r\n"));
			//todo: del observe res?
			continue;
		}


		//(re)create observer for resource
		CoAP_Observer_t* pNewObserver = CoAP_AllocNewObserver();

		if (pNewObserver == NULL) {
			configPRINTF(("pNewObserver out of Mem!\r\n"));
			//todo: do anything different to simple continue
			continue;
		}

		CoAP_option_t* pOpt = pOptList;
		while (pOpt != NULL) {
			configPRINTF(("."));

			switch (pOpt->Number) {
				case OPT_NUM_URI_HOST:
					coap_memcpy((void*) &(pNewObserver->Ep), pOpt->Value, sizeof(NetEp_t));
					break;
				case OPT_NUM_URI_PORT: //"hack" netID, remove if netID removal cleanup done!
					coap_memcpy((void*) &(pNewObserver->socketHandle), pOpt->Value, sizeof(Socket_t));
					break;
				case OPT_NUM_URI_PATH:
					break; //dont copy path to observe struct (it's connected to its resource anyway!)
				case OPT_NUM_LOBARO_TOKEN_SAVE:
					coap_memcpy((void*) &(pNewObserver->Token), pOpt->Value, 8);
					break;
				default:
					CoAP_CopyOptionToList(&(pNewObserver->pOptList), pOpt);
					break;
			}

			pOpt = pOpt->next;
		}

		//attach observer to resource
		CoAP_AppendObserverToList(&(pRes->pListObservers), pNewObserver);

		CoAP_FreeOptionList(pOptList); //free temp options
	}

	CoAP_PrintAllResources();
	//CoAP_FindResourceByUri
	return COAP_OK;
}


CoAP_HandlerResult_t _rom WellKnown_GetHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
//	static uint8_t wellknownStr[500];
//	uint8_t* pWr = wellknownStr;

	if (pReq->Code != REQ_GET) {
		char errMsg[] = {"CoAP GET only!"};
		pResp->Code = RESP_ERROR_BAD_REQUEST_4_00;
		CoAP_SetPayload(pResp, errMsg, (uint16_t) coap_strlen(errMsg), true);
		return HANDLER_ERROR;
	}

	CoAP_Res_t* pList = pResList; //List of internal resources
	uint8_t* pStr = (uint8_t*) pvPortMalloc((ResListMembers + 1) * 64); //first estimation of needed memory
	uint8_t* pStrStart = pStr;

	if (pStr == NULL) {
		configPRINTF(("- WellKnown_GetHandler(): Ouf memory error!\r\n"));
		return HANDLER_ERROR;
	}
	memset(pStr, 0, (ResListMembers + 1) * 64);

	configPRINTF(("- WellKnown_GetHandler(): res cnt:%u temp alloc:%u\r\n", (unsigned int) ResListMembers, (unsigned int) (ResListMembers + 2) * 64));

	//TODO: Implement non ram version, e.g. write to memory to eeprom
	while (pList != NULL) {
		CoAP_option_t* pUriOpt = pList->pUri;

		*pStr++ = '<';
		while (pUriOpt != NULL) {
			*pStr++ = '/';
			coap_memcpy(pStr, pUriOpt->Value, pUriOpt->Length);
			pStr += pUriOpt->Length;
			pUriOpt = pUriOpt->next;
		}
		*pStr++ = '>';
		if (pList->Options.Cf != COAP_CF_LINK_FORMAT) {
			if( pList->pDescription != NULL ) {
				pStr += coap_sprintf((char *) pStr, ";title=\"%s\"", pList->pDescription);
			}
			
			pStr += coap_sprintf((char *) pStr, ";ct=%d", pList->Options.Cf);
			
			if (pList->Notifier != NULL) {
				pStr += coap_sprintf((char*) pStr, ";obs");
			}
		}
		*pStr++ = ',';

		pList = pList->next;

		//TODO: implement growing of buf/overwrite check
	}

	CoAP_SetPayload(pResp, pStrStart, (uint16_t) coap_strlen((char*) pStrStart), true);
	vPortFree(pStrStart);

	CoAP_AddCfOptionToMsg(pResp, COAP_CF_LINK_FORMAT);

	return HANDLER_OK;
}

void _rom CoAP_InitResources() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_LINK_FORMAT, .AllowedMethods = RES_OPT_GET};
//	CoAP_CreateResource("/.well-known/core", "\0", Options, WellKnown_GetHandler, NULL);
	CoAP_CreateResource(configCOAP_URI_PATH, "\0", Options, WellKnown_GetHandler, NULL);
}

static CoAP_Result_t _rom CoAP_AppendResourceToList(CoAP_Res_t** pListStart, CoAP_Res_t* pResToAdd) {
	if (pResToAdd == NULL) return COAP_ERR_ARGUMENT;

	if (*pListStart == NULL) //List empty? create new first element
	{
		*pListStart = pResToAdd;
		(*pListStart)->next = NULL;
	} else //append new element at end
	{
		CoAP_Res_t* pRes = *pListStart;
		while (pRes->next != NULL) pRes = pRes->next;

		pRes->next = pResToAdd;
		pRes = pRes->next;
		pRes->next = NULL;
	}
	return COAP_OK;
}

CoAP_Result_t _rom CoAP_FreeResource(CoAP_Res_t** pResource) {
	CoAP_FreeOptionList((*pResource)->pUri);
	(*pResource)->pUri = NULL;

	vPortFree((*pResource)->pDescription);
	vPortFree((void*) (*pResource));
	*pResource = NULL;
	return COAP_OK;
}

//static CoAp_Result_t CoAP_UnlinkResourceFromList(CoAP_Res_t** pListStart, CoAP_Res_t* pResToRemove, bool FreeUnlinked)
//{
//	CoAP_Res_t* currP;
//	CoAP_Res_t* prevP;
//
//  // For 1st node, indicate there is no previous.
//  prevP = NULL;
//
//   //Visit each node, maintaining a pointer to
//   //the previous node we just visited.
//  for (currP = *pListStart; currP != NULL;
//		  prevP = currP, currP = currP->next) {
//
//    if (currP == pResToRemove) {  // Found it.
//      if (prevP == NULL) {
//        //Fix beginning pointer.
//        *pListStart = currP->next;
//      } else {
//        //Fix previous node's next to
//        //skip over the removed node.
//        prevP->next = currP->next;
//      }
//
//      // Deallocate the node.
//      if(FreeUnlinked) CoAP_FreeResource(&currP);
//      //Done searching.
//      ResListMembers--;
//      return COAP_OK;
//    }
//  }
//  return COAP_OK;
//}

CoAP_Res_t* _rom CoAP_FindResourceByUri(CoAP_Res_t* pResListToSearchIn, CoAP_option_t* pOptionsToMatch) {
	CoAP_Res_t* pList = pResList;
	if (pResListToSearchIn != NULL) {
		pList = pResListToSearchIn;
	}

	for (; pList != NULL; pList = pList->next) {
		if (CoAP_UriOptionsAreEqual(pList->pUri, pOptionsToMatch)) {
			return pList;
		}
	}

	return NULL;
}

CoAP_Res_t* _rom CoAP_CreateResource(char* Uri, char* Descr, CoAP_ResOpts_t Options, CoAP_ResourceHandler_fPtr_t pHandlerFkt, CoAP_ResourceNotifier_fPtr_t pNotifierFkt) {
	configPRINTF(("Creating resource %s (%s) AllowedMethods: %x%x%x%x\r\n", Uri, Descr == NULL ? "" : Descr,
		 !!(Options.AllowedMethods & RES_OPT_GET),
		 !!(Options.AllowedMethods & RES_OPT_POST),
		 !!(Options.AllowedMethods & RES_OPT_PUT),
		 !!(Options.AllowedMethods & RES_OPT_DELETE)));

	if (Options.AllowedMethods == 0) {
		IotLogError("Can not create Resource that does not allow any method!");
		return NULL;
	}

	CoAP_Res_t* pRes = (CoAP_Res_t*) (pvPortMalloc(sizeof(CoAP_Res_t)));
	if (pRes == NULL) {
		return NULL;
	}
	memset(pRes, 0, sizeof(CoAP_Res_t));

	pRes->pListObservers = NULL;
	pRes->pUri = NULL;
	pRes->next = NULL;

	pRes->Options = Options;

	if (Descr != NULL && *Descr != '\0') {
		pRes->pDescription = (char*) (pvPortMalloc(sizeof(char) * (coap_strlen(Descr) + 1)));
		coap_strcpy(pRes->pDescription, Descr);
	} else {
		pRes->pDescription = NULL;
	}

	CoAP_AppendUriOptionsFromString(&(pRes->pUri), Uri);

	pRes->Handler = pHandlerFkt;
	pRes->Notifier = pNotifierFkt;

	CoAP_AppendResourceToList(&pResList, pRes);

	ResListMembers++;

	return pRes;
}


CoAP_Result_t _rom CoAP_NotifyResourceObservers(CoAP_Res_t* pRes) {
	pRes->UpdateCnt++;
	CoAP_StartNotifyInteractions(pRes); //async start of update interaction
	return COAP_OK;
}


void _rom CoAP_PrintResource(CoAP_Res_t* pRes) {
	CoAP_printUriOptionsList(pRes->pUri);
	configPRINTF(("Observers:\r\n"));
	CoAP_Observer_t* pOpserver = pRes->pListObservers; //point to ListStart
	while (pOpserver != NULL) {
		configPRINTF(("Token:%llx - ", (uint64_t)pOpserver->Token.Token[0]));
		PrintEndpoint(&(pOpserver->Ep));
		configPRINTF(("\n"));
		CoAP_printUriOptionsList(pOpserver->pOptList);
		CoAP_printOptionsList(pOpserver->pOptList);
		configPRINTF(("---\r\n"));
		pOpserver = pOpserver->next;

	}
	configPRINTF(("\r\n"));
}

void _rom CoAP_PrintAllResources() {
	CoAP_Res_t* pRes = pResList;
	while (pRes != NULL) {
		CoAP_PrintResource(pRes);
		pRes = pRes->next;
	}
}


CoAP_Result_t _rom CoAP_RemoveObserverFromResource(CoAP_Observer_t** pObserverList, Socket_t socketHandle, NetEp_t* pRemoteEP, CoAP_Token_t token) {
	CoAP_Observer_t* pObserver = *pObserverList;

	while (pObserver != NULL) { //found right existing observation -> delete it

		if (CoAP_TokenEqual(token, pObserver->Token) && socketHandle == pObserver->socketHandle && EpAreEqual(pRemoteEP, &(pObserver->Ep))) {

			configPRINTF(("- (!) Unlinking observer from resource\r\n"));
			CoAP_UnlinkObserverFromList(pObserverList, pObserver, true);
			return COAP_REMOVED;

		}
		pObserver = pObserver->next;
	}
	return COAP_ERR_NOT_FOUND;
}
