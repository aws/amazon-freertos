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
#include "coap_main.h"
#include "liblobaro_coap.h"
#include "coap_socket.h"

CoAP_Result_t CoAP_HandleObservationInReq(CoAP_Interaction_t* pIA);

static CoAP_Interaction_t* _rom CoAP_AllocNewInteraction() {
	CoAP_Interaction_t* newInteraction = (CoAP_Interaction_t*) (pvPortMalloc(sizeof(CoAP_Interaction_t)));
	if (newInteraction == NULL) {
		//coap_mem_stats();
		configPRINTF(("- (!!!) CoAP_AllocNewInteraction() Out of Memory (Needed %d bytes) !!!\r\n", sizeof(CoAP_Interaction_t)));
		return NULL;
	}

	memset(newInteraction, 0, sizeof(CoAP_Interaction_t));
//	assert_coap(newInteraction->pObserver == NULL);
	return newInteraction;
}

CoAP_Result_t _rom CoAP_FreeInteraction(CoAP_Interaction_t** pInteraction) {
	configPRINTF(("Releasing Interaction...\r\n"));
	//coap_mem_stats();
	CoAP_FreeMessage((*pInteraction)->pReqMsg);
	(*pInteraction)->pReqMsg = NULL;
	CoAP_FreeMessage((*pInteraction)->pRespMsg);
	(*pInteraction)->pRespMsg = NULL;
	vPortFree((void*) (*pInteraction));
	//coap_mem_stats();

	*pInteraction = NULL;
	return COAP_OK;
}

static CoAP_Result_t _rom CoAP_UnlinkInteractionFromList(CoAP_Interaction_t** pListStart, CoAP_Interaction_t* pInteractionToRemove, bool FreeUnlinked) {
	CoAP_Interaction_t* currP;
	CoAP_Interaction_t* prevP;

	// For 1st node, indicate there is no previous.
	prevP = NULL;

	//Visit each node, maintaining a pointer to
	//the previous node we just visited.
	for (currP = *pListStart; currP != NULL;
			prevP = currP, currP = currP->next) {

		if (currP == pInteractionToRemove) {  // Found it.
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
				CoAP_FreeInteraction(&currP);
			}
			//Done searching.
			return COAP_OK;
		}
	}
	return COAP_OK;
}

static CoAP_Result_t _rom CoAP_UnlinkInteractionFromListByHandle(CoAP_Interaction_t** pListStart, uint16_t MsgID, Socket_t socketHandle, NetEp_t* RstEp, bool FreeUnlinked) {
	CoAP_Interaction_t* currP;
	CoAP_Interaction_t* prevP;

	// For 1st node, indicate there is no previous.
	prevP = NULL;

	//Visit each node, maintaining a pointer to
	//the previous node we just visited.
	for (currP = *pListStart; currP != NULL;
			prevP = currP, currP = currP->next) {

		if (currP->socketHandle == socketHandle
				&& ((currP->pReqMsg && currP->pReqMsg->MessageID == MsgID) || (currP->pRespMsg && currP->pRespMsg->MessageID == MsgID))
				&& EpAreEqual(&(currP->RemoteEp), RstEp)) {  // Found it.

			if (prevP == NULL) {
				//Fix beginning pointer.
				*pListStart = currP->next;
			} else {
				//Fix previous node's next to
				//skip over the removed node.
				prevP->next = currP->next;
			}

			// Deallocate the node.
			if (FreeUnlinked)
				CoAP_FreeInteraction(&currP);
			//Done searching.
			return COAP_OK;
		}
	}
	return COAP_NOT_FOUND;
}

static CoAP_Result_t _rom CoAP_AppendInteractionToList(CoAP_Interaction_t** pListStart, CoAP_Interaction_t* pInteractionToAdd) {
	if (pInteractionToAdd == NULL)
		return COAP_ERR_ARGUMENT;

	if (*pListStart == NULL) //List empty? create new first element
	{
		*pListStart = pInteractionToAdd;
		(*pListStart)->next = NULL;
	} else //append new element at end
	{
		CoAP_Interaction_t* pTrans = *pListStart;
		while (pTrans->next != NULL)
			pTrans = pTrans->next;

		pTrans->next = pInteractionToAdd;
		pTrans = pTrans->next;
		pTrans->next = NULL;
	}
	return COAP_OK;
}

static CoAP_Result_t _rom CoAP_MoveInteractionToListEnd(CoAP_Interaction_t** pListStart, CoAP_Interaction_t* pInteractionToMove) {
	CoAP_Interaction_t* currP;
	CoAP_Interaction_t* prevP;

	// For 1st node, indicate there is no previous.
	prevP = NULL;

	//is interaction in List? if so delete it temporarily and add it to the back then

	//Visit each node, maintaining a pointer to
	//the previous node we just visited.
	for (currP = *pListStart;
			currP != NULL;
			prevP = currP, currP = currP->next) {

		if (currP == pInteractionToMove) {  // Found it.
			if (prevP == NULL) {
				//Fix beginning pointer.
				*pListStart = currP->next;
			} else {
				//Fix previous node's next to
				//skip over the removed node.
				prevP->next = currP->next;
			}
		}
	}
	//node removed now put it at end of list
	CoAP_AppendInteractionToList(pListStart, pInteractionToMove);

	return COAP_OK;
}

CoAP_Result_t _rom CoAP_SetSleepInteraction(CoAP_Interaction_t* pIA, uint32_t seconds) {
	pIA->SleepUntil = IotClock_GetTimeMs()/1000 + seconds;
	return COAP_OK;
}

CoAP_Result_t _rom CoAP_EnableAckTimeout(CoAP_Interaction_t* pIA, uint8_t retryNum) {
	uint32_t waitTime = ACK_TIMEOUT;
	int i;
	for (i = 0; i < retryNum; i++) { //"exponential backoff"
		waitTime *= ACK_TIMEOUT;
	}

	pIA->AckTimeout = IotClock_GetTimeMs()/1000 + waitTime;
	return COAP_OK;
}

CoAP_Interaction_t* _rom CoAP_GetLongestPendingInteraction() {
	return CoAP.pInteractions;
}

// Each incoming message belongs to an interaction
// CON <-> ACK/RST are matched by id and endpoint
CoAP_Interaction_t* _rom CoAP_FindInteractionByMessageIdAndEp(CoAP_Interaction_t* pList, uint16_t mID, NetEp_t* fromEp) {

	// A received RST message rejects a former send CON message or (optional) NON message send by us
	// A received ACK message acknowledges a former send CON message or (optional) NON message send by us
	// servers and notificators use CON only in responses, clients in requests
	while (pList != NULL) {
		if ((pList->pRespMsg != NULL && pList->pRespMsg->MessageID == mID || pList->pReqMsg != NULL && pList->pReqMsg->MessageID == mID) && EpAreEqual(fromEp, &(pList->RemoteEp))) {
			return pList;
		}
		pList = pList->next;
	}

	return NULL;
}

CoAP_Result_t _rom CoAP_DeleteInteraction(CoAP_Interaction_t* pInteractionToDelete) {
	return CoAP_UnlinkInteractionFromList(&(CoAP.pInteractions), pInteractionToDelete, true);
}

CoAP_Result_t _rom CoAP_ResetInteractionByHandle(uint16_t MsgID, Socket_t socketHandle, NetEp_t* RstEp) {
	return CoAP_UnlinkInteractionFromListByHandle(&(CoAP.pInteractions), MsgID, socketHandle, RstEp, true);
}

CoAP_Result_t _rom CoAP_EnqueueLastInteraction(CoAP_Interaction_t* pInteractionToEnqueue) {
	return CoAP_MoveInteractionToListEnd(&(CoAP.pInteractions), pInteractionToEnqueue);
}

//we act as a CoAP Client (sending requests) in this interaction
CoAP_Result_t _rom CoAP_StartNewClientInteraction(CoAP_Message_t* pMsgReq, Socket_t socketHandle, NetEp_t* ServerEp, CoAP_RespHandler_fn_t cb) {
	if (pMsgReq == NULL || CoAP_MsgIsRequest(pMsgReq) == false)
		return COAP_ERR_ARGUMENT;

	CoAP_Interaction_t* newIA = CoAP_AllocNewInteraction();
	if (newIA == NULL)
		return COAP_ERR_OUT_OF_MEMORY;

	//attach request message
	newIA->pReqMsg = pMsgReq;

	newIA->socketHandle = socketHandle;
	CopyEndpoints(&(newIA->RemoteEp), ServerEp);
	newIA->RespCB = cb;

	newIA->Role = COAP_ROLE_CLIENT;
	newIA->State = COAP_STATE_READY_TO_REQUEST;

	CoAP_AppendInteractionToList(&(CoAP.pInteractions), newIA);

	return COAP_OK;
}

CoAP_Result_t _rom CoAP_StartNewGetRequest(char* UriString, Socket_t socketHandle, NetEp_t* ServerEp, CoAP_RespHandler_fn_t cb) {

	CoAP_Message_t* pReqMsg = CoAP_CreateMessage(CON, REQ_GET, CoAP_GetNextMid(), CoAP_GenerateToken());

	if (pReqMsg != NULL) {
		CoAP_AppendUriOptionsFromString(&(pReqMsg->pOptionsList), UriString);
		return CoAP_StartNewClientInteraction(pReqMsg, socketHandle, ServerEp, cb);
	}

	configPRINTF(("- New GetRequest failed: Out of Memory\r\n"));

	return COAP_ERR_OUT_OF_MEMORY;
}

CoAP_Result_t _rom CoAP_StartNotifyInteractions(CoAP_Res_t* pRes) {
	// TODO: find all notification interactions that match this resource
	// CoAP.pInteractions->pRes == pRes && CoAP.pInteractions->Role == COAP_ROLE_NOTIFICATION
	// and call the pRes->Notifier(...) to get a new response
	// the new response is then send to the client and the interaction must wait for the ACK (or not for NON)
	// While the interaction is waiting for an ACK or doing retries the pending response might change but NOT the messageId
	// ------------

	CoAP_Observer_t* pObserver = pRes->pListObservers;
	CoAP_Interaction_t* newIA;
	bool sameNotificationOngoing = false;

	int cnt = 0;
	while (pObserver != NULL) {
		cnt++;
		pObserver = pObserver->next;
	}
	configPRINTF(("Notify %d observers for res %s\n", cnt, pRes->pDescription));



	//iterate over all observers of this resource and check if
	//a) a new notification interaction has to be created
	//or
	//b) wait for currently pending notification to end
	//or
	//c) a currently pending older notification has be updated to the new resource representation
	//   as stated in Observe RFC7641 ("4.5.2.  Advanced Transmission")
	pObserver = pRes->pListObservers;
	while (pObserver != NULL) {

		//search for pending notification of this resource to this observer and set update flag of representation
		sameNotificationOngoing = false;
		CoAP_Interaction_t* pIA;
		for (pIA = CoAP.pInteractions; pIA != NULL; pIA = pIA->next) {
			if (pIA->Role == COAP_ROLE_NOTIFICATION &&
					pIA->pRes == pRes &&
					pIA->socketHandle == pObserver->socketHandle &&
					EpAreEqual(&(pIA->RemoteEp), &(pObserver->Ep))) {
				sameNotificationOngoing = true;

#if USE_RFC7641_ADVANCED_TRANSMISSION == 1
				pIA->UpdatePendingNotification = true; //will try to update the ongoing resource representation on ongoing transfer
				pIA->SleepUntil = 0; // Wakeup interaction
#endif

				break;
			}
		}

#if USE_RFC7641_ADVANCED_TRANSMISSION == 1
		// If there is already a running interaction for this notification
		// it will be updated for next retry
		// or just send another message with fresh data after it is finished
		// Thus we do not need to create a new interaction
		if (sameNotificationOngoing) {
			pObserver = pObserver->next; //skip this observer, don't start a 2nd notification now
			continue;
		}
#endif

		//Start new IA for this observer
		newIA = CoAP_AllocNewInteraction();
		if (newIA == NULL) {
			return COAP_ERR_OUT_OF_MEMORY;
		}

		//Create fresh response message
		newIA->pRespMsg = CoAP_CreateMessage(CON, RESP_SUCCESS_CONTENT_2_05, CoAP_GetNextMid(), pObserver->Token);

		//Call Notify Handler of resource and add to interaction list
		if (newIA->pRespMsg != NULL && pRes->Notifier != NULL &&
				pRes->Notifier(pObserver, newIA->pRespMsg) == HANDLER_OK) { //<------ call notify handler of resource

			newIA->Role = COAP_ROLE_NOTIFICATION;
			newIA->State = COAP_STATE_READY_TO_NOTIFY;
			newIA->RemoteEp = pObserver->Ep;
			newIA->socketHandle = pObserver->socketHandle;
			newIA->pRes = pRes;
			newIA->pObserver = pObserver;

			if (newIA->pRespMsg->Code >= RESP_ERROR_BAD_REQUEST_4_00) { //remove this observer from resource in case of non OK Code (see RFC7641, 3.2., 3rd paragraph)
				pObserver = pObserver->next; //next statement will free current observer so save its ancestor node right now
				newIA->pObserver = NULL;
				CoAP_RemoveObserverFromResource(&(newIA->pRes->pListObservers), newIA->socketHandle, &(pIA->RemoteEp), newIA->pRespMsg->Token);
				continue;
			} else {
				AddObserveOptionToMsg(newIA->pRespMsg, pRes->UpdateCnt); // Only 2.xx responses do include an Observe Option.
			}

			if (newIA->pRespMsg->Type == NON && pRes->UpdateCnt % 20 == 0) { //send every 20th message as CON even if notify handler defines the send out as NON to support "lazy" cancelation
				newIA->pRespMsg->Type = CON;
			}

			CoAP_AppendInteractionToList(&(CoAP.pInteractions), newIA);

		} else {
			CoAP_FreeInteraction(&newIA); //revert IA creation above
		}

		pObserver = pObserver->next;
	}
	return COAP_OK;
}

//we act as a CoAP Server (receiving requests) in this interaction
CoAP_Result_t _rom CoAP_StartNewServerInteraction(CoAP_Message_t* pMsgReq, CoAP_Res_t* pRes, Socket_t socketHandle, NetPacket_t* pPacket) {
	if (!CoAP_MsgIsRequest(pMsgReq))
		return COAP_ERR_ARGUMENT;

	//NON or CON request
	NetEp_t* pReqEp = &(pPacket->remoteEp);
	CoAP_Interaction_t* pIA;

	//Set the CoAP resource to the requst message
//	pMsgReq->pResource = pRes;

	//duplicate detection:
	//same request already received before?
	//iterate over all interactions
	for (pIA = CoAP.pInteractions; pIA != NULL; pIA = pIA->next) {
		if (pIA->Role == COAP_ROLE_SERVER && pIA->socketHandle == socketHandle && pIA->pReqMsg->MessageID == pMsgReq->MessageID && EpAreEqual(&(pIA->RemoteEp), pReqEp)) {
			//implements 4.5. "SHOULD"s
			if (pIA->pReqMsg->Type == CON && pIA->State == COAP_STATE_RESOURCE_POSTPONE_EMPTY_ACK_SENT) { //=> must be postponed resource with empty ack already sent, send it again
				CoAP_SendEmptyAck(pIA->pReqMsg->MessageID, pIA->socketHandle, pPacket->remoteEp); //send another empty ack
			}

			//pIA->ReqReliabilityState

			return COAP_ERR_EXISTING;
		}
	}

	//no duplicate request found-> create a new interaction for this new request
	CoAP_Interaction_t* newIA = CoAP_AllocNewInteraction();

	if (newIA == NULL) {
//		ERROR("(!) can't create new interaction - out of memory!\r\n");
		return COAP_ERR_OUT_OF_MEMORY;
	}

	newIA->socketHandle = socketHandle;
	newIA->pRes = pRes;
	newIA->Role = COAP_ROLE_SERVER;
	newIA->State = COAP_STATE_HANDLE_REQUEST;
	newIA->ReqMetaInfo = pPacket->metaInfo;
	newIA->pReqMsg = pMsgReq;

	CopyEndpoints(&(newIA->RemoteEp), pReqEp);
	CoAP_AppendInteractionToList(&(CoAP.pInteractions), newIA);
	return COAP_OK;
}

CoAP_Result_t _rom CoAP_RemoveInteractionsObserver(CoAP_Interaction_t* pIA, CoAP_Token_t token) {

	return CoAP_RemoveObserverFromResource(&((pIA->pRes)->pListObservers), pIA->socketHandle, &(pIA->RemoteEp), token);
}

CoAP_Result_t _rom CoAP_HandleObservationInReq(CoAP_Interaction_t* pIA) {
	if (pIA == NULL || pIA->pReqMsg == NULL) {
		return COAP_ERR_ARGUMENT;    //safety checks
	}

	CoAP_Result_t res;
	uint32_t obsVal = 0;
	CoAP_Observer_t* pObserver = NULL;
	CoAP_Observer_t* pExistingObserver = (pIA->pRes)->pListObservers;

	if (pIA->pRes->Notifier == NULL) {
		return COAP_ERR_NOT_FOUND;            //resource does not support observe
	}
	if ((res = GetObserveOptionFromMsg(pIA->pReqMsg, &obsVal)) != COAP_OK) {
		return res; //if no observe option in req function can't do anything
	}

	//Client registers
	if (obsVal == OBSERVE_OPT_REGISTER) { // val == 0

		//Alloc memory for new Observer
		pObserver = CoAP_AllocNewObserver();
		if (pObserver == NULL) {
			return COAP_ERR_OUT_OF_MEMORY;
		}

		//Copy relevant information for observation from current interaction
		pObserver->Ep = pIA->RemoteEp;
		pObserver->socketHandle = pIA->socketHandle;
		pObserver->Token = pIA->pReqMsg->Token;
		pObserver->next = NULL;

		//Copy relevant Options from Request (uri-query, observe)
		//Note: uri-path is not relevant since observers are fixed to its resource
		CoAP_option_t* pOption = pIA->pReqMsg->pOptionsList;
		while (pOption != NULL) {
			if (pOption->Number == OPT_NUM_URI_QUERY || pOption->Number == OPT_NUM_OBSERVE) {
				//create copy from volatile Iinteraction msg options
				if (CoAP_AppendOptionToList(&(pObserver->pOptList), pOption->Number, pOption->Value, pOption->Length) != COAP_OK) {
					CoAP_FreeObserver(&pObserver);
					return COAP_ERR_OUT_OF_MEMORY;
				}
			}
			pOption = pOption->next;
		}

		//delete eventually existing same observer
		while (pExistingObserver != NULL) { //found right existing observation -> delete it
			// Changed: Do NOT check the token. if socket and EP are the same, it's the same observer
			// We do NOT allow to observe the same resource twice from the same observer
			// CoAP_TokenEqual(pIA->pReqMsg->Token, pExistingObserver->Token) &&
			if (pIA->socketHandle == pExistingObserver->socketHandle && EpAreEqual(&(pIA->RemoteEp), &(pExistingObserver->Ep))) {
				CoAP_UnlinkObserverFromList(&((pIA->pRes)->pListObservers), pExistingObserver, true);
				break;
			}
			pExistingObserver = pExistingObserver->next;
		}

		//attach/update observer to resource
		return CoAP_AppendObserverToList(&((pIA->pRes)->pListObservers), pObserver);

		//Client cancels observation actively (this is an alternative to simply forget the req token and send rst on next notification)
	} else if (obsVal == OBSERVE_OPT_DEREGISTER) { // val == 1
		//find matching observer in resource observers-list
		CoAP_RemoveInteractionsObserver(pIA, pIA->pReqMsg->Token); //remove observer identified by token, socketHandle and remote EP from resource

		//delete/abort any pending notification interaction
		CoAP_Interaction_t* pIApending;
		for (pIApending = CoAP.pInteractions; pIApending != NULL; pIApending = pIApending->next) {
			if (pIApending->Role == COAP_ROLE_NOTIFICATION) {
				if (CoAP_TokenEqual(pIApending->pRespMsg->Token, pIA->pReqMsg->Token)
						&& pIApending->socketHandle == pIA->socketHandle
						&& EpAreEqual(&(pIApending->RemoteEp), &(pIA->RemoteEp))) {
					configPRINTF(("Abort of pending notificaton interaction\r\n"));
					CoAP_DeleteInteraction(pIApending);
					break;
				}
			}
		}

	} else {
		return COAP_BAD_OPTION_VAL;
	}

	return COAP_ERR_NOT_FOUND;
}


