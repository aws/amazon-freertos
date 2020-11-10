#line __LINE__ "diagnostic.c"
#include "coap.h"
#include "FreeRTOSConfig.h"
char* InteractionRoleToString(CoAP_InteractionRole_t role) {
	switch (role) {
	case COAP_ROLE_NOT_SET:
		return "NOT_SET";
	case COAP_ROLE_SERVER:
		return "SERVER";
	case COAP_ROLE_NOTIFICATION:
		return "NOTIFICATION";
	case COAP_ROLE_CLIENT:
		return "CLIENT";
	default:
		return "UNKNOWN_ROLE";
	}
}

char* InteractionStateToString(CoAP_InteractionState_t state) {
	switch (state) {
	case COAP_STATE_NOT_SET:
		return "NOT_SET";
	case COAP_STATE_HANDLE_REQUEST:
		return "HANDLE_REQUEST";
	case COAP_STATE_RESOURCE_POSTPONE_EMPTY_ACK_SENT:
		return "RESOURCE_POSTPONE_EMPTY_ACK_SENT";
	case COAP_STATE_RESPONSE_SENT:
		return "RESPONSE_SENT";
	case COAP_STATE_RESPONSE_WAITING_LEISURE:
		return "RESPONSE_WAITING_LEISURE";
	case COAP_STATE_READY_TO_NOTIFY:
		return "READY_TO_NOTIFY";
	case COAP_STATE_NOTIFICATION_SENT:
		return "NOTIFICATION_SENT";
	case COAP_STATE_READY_TO_REQUEST:
		return "READY_TO_REQUEST";
	case COAP_STATE_WAITING_RESPONSE:
		return "WAITING_RESPONSE";
	case COAP_STATE_HANDLE_RESPONSE:
		return "HANDLE_RESPONSE";
	case COAP_STATE_FINISHED:
		return "FINISHED";
	default:
		return "UNKNOWN_STATE";
	}
}

char* ResultToString(CoAP_Result_t res) {
	switch (res) {
	case COAP_OK:
		return "COAP_OK";
	case COAP_NOT_FOUND:
		return "COAP_NOT_FOUND";
	case COAP_PARSE_DATAGRAM_TOO_SHORT:
		return "COAP_PARSE_DATAGRAM_TOO_SHORT";
	case COAP_PARSE_UNKOWN_COAP_VERSION:
		return "COAP_PARSE_UNKOWN_COAP_VERSION";
	case COAP_PARSE_MESSAGE_FORMAT_ERROR:
		return "COAP_PARSE_MESSAGE_FORMAT_ERROR";
	case COAP_PARSE_TOO_MANY_OPTIONS:
		return "COAP_PARSE_TOO_MANY_OPTIONS";
	case COAP_PARSE_TOO_LONG_OPTION:
		return "COAP_PARSE_TOO_LONG_OPTION";
	case COAP_PARSE_TOO_MUCH_PAYLOAD:
		return "COAP_PARSE_TOO_MUCH_PAYLOAD";
	case COAP_PACK_TOO_MANY_OPTIONS:
		return "COAP_PACK_TOO_MANY_OPTIONS";
	case COAP_PACK_TOO_LONG_OPTION:
		return "COAP_PACK_TOO_LONG_OPTION";
	case COAP_ERR_ARGUMENT:
		return "COAP_ERR_ARGUMENT";
	case COAP_ERR_SOCKET:
		return "COAP_ERR_SOCKET";
	case COAP_ERR_NETWORK:
		return "COAP_ERR_NETWORK";
	case COAP_ERR_OUT_OF_MEMORY:
		return "COAP_ERR_OUT_OF_MEMORY";
	case COAP_ERR_TOO_LONG_URI_PATH:
		return "COAP_ERR_TOO_LONG_URI_PATH";
	case COAP_ERR_NOT_FOUND:
		return "COAP_ERR_NOT_FOUND";
	case COAP_ERR_WRONG_OPTION:
		return "COAP_ERR_WRONG_OPTION";
	case COAP_ERR_EXISTING:
		return "COAP_ERR_EXISTING";
	case COAP_TRUE:
		return "COAP_TRUE";
	case COAP_FALSE:
		return "COAP_FALSE";
	case COAP_ERR_WRONG_REQUEST:
		return "COAP_ERR_WRONG_REQUEST";
	case COAP_BAD_OPTION_VAL:
		return "COAP_BAD_OPTION_VAL";
	case COAP_BAD_OPTION_LEN:
		return "COAP_BAD_OPTION_LEN";
	case COAP_REMOVED:
		return "COAP_REMOVED";
	case COAP_ERR_UNKNOWN:
		return "COAP_ERR_UNKNOWN";
	case COAP_ERR_REMOTE_RST:
		return "COAP_ERR_REMOTE_RST";
	case COAP_ERR_OUT_OF_ATTEMPTS:
		return "COAP_ERR_OUT_OF_ATTEMPTS";
	case COAP_ERR_TIMEOUT:
		return "COAP_ERR_TIMEOUT";
	case COAP_WAITING:
		return "COAP_WAITING";
	case COAP_HOLDING_BACK:
		return "COAP_HOLDING_BACK";
	case COAP_RETRY:
		return "COAP_RETRY";
	default:
		return "UNKNOWN_RESULT";
	}
}

char* ReliabilityStateToString(CoAP_ConfirmationState_t state) {
	switch (state) {
	case NOT_SET:
		return "NOT_SET";
	case ACK_SEND:
		return "ACK_SET";
	case RST_SEND:
		return "RST_SET";
	default:
		return "UNKNOWN_STATE";
	}
}

void _rom PrintEndpoint(const NetEp_t* ep) {
	switch (ep->NetType) {
	case EP_NONE:
		configPRINTF(("NONE"));
		break;
	case IPV6:
//		configPRINTF(("IPv6, ["));
//		PRINT_IPV6(ep->NetAddr.IPv6);
//		configPRINTF(("]: %u", ep->NetPort));

		configPRINTF(("IPv6,[ %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]:%u \r\n",
						ep->NetAddr.IPv6.u8[0], ep->NetAddr.IPv6.u8[1], ep->NetAddr.IPv6.u8[2],
						ep->NetAddr.IPv6.u8[3],ep->NetAddr.IPv6.u8[4], ep->NetAddr.IPv6.u8[5],
						ep->NetAddr.IPv6.u8[6],ep->NetAddr.IPv6.u8[7], ep->NetAddr.IPv6.u8[8],
						ep->NetAddr.IPv6.u8[9],ep->NetAddr.IPv6.u8[10], ep->NetAddr.IPv6.u8[11],
						ep->NetAddr.IPv6.u8[12],ep->NetAddr.IPv6.u8[13], ep->NetAddr.IPv6.u8[14],
						ep->NetAddr.IPv6.u8[15],
						ep->NetPort));
		break;
	case IPV4:
		configPRINTF(("IPv4, %d.%d.%d.%d:%d \r\n",
				ep->NetAddr.IPv4.u8[0], ep->NetAddr.IPv4.u8[1], ep->NetAddr.IPv4.u8[2], ep->NetAddr.IPv4.u8[3],
				ep->NetPort));
		break;
	case BTLE:
		configPRINTF(("BTLE"));
		break;
	case UART:
		configPRINTF(("UART, COM%d", ep->NetAddr.Uart.ComPortID));
		break;
	default:
		configPRINTF(("UNKNOWN_EP (%d)", ep->NetType));
	}
}

void PrintToken(CoAP_Token_t* token) {
	uint8_t tokenBytes = token->Length;
	if (tokenBytes > 0) {
		configPRINTF(("%u Byte -> 0x", tokenBytes));
		int i;
		for (i = 0; i < tokenBytes; i++) {
			configPRINTF(("%02x", token->Token[i]));
		}
	} else {
		configPRINTF(("%u Byte -> 0x0", tokenBytes));
	}
}

void _rom PrintInteractions(CoAP_Interaction_t *pInteractions) {
	int cnt = 0;
	for (CoAP_Interaction_t* pIA = CoAP.pInteractions; pIA != NULL; pIA = pIA->next) {
		cnt++;
	}

	configPRINTF(("Interactions: %d\n", cnt));
	configPRINTF(("-------------\n"));
	for (CoAP_Interaction_t* pIA = CoAP.pInteractions; pIA != NULL; pIA = pIA->next) {
		configPRINTF(("- Role: %s, State: %s\n",
				InteractionRoleToString(pIA->Role), InteractionStateToString(pIA->State)));
		configPRINTF(("RetransCnt: %u, SleepUntil %lu, AckTimeout: %lu\n", pIA->RetransCounter, pIA->SleepUntil, pIA->AckTimeout));
		configPRINTF(("Socket: %04lx, RemoteEp: ", (uint32_t )pIA->socketHandle));
		PrintEndpoint(&pIA->RemoteEp);
		configPRINTF(("\n"));
		configPRINTF(("ReqReliabilityState: %s\n", ReliabilityStateToString(pIA->ReqConfirmState)));
		configPRINTF(("RespReliabilityState: %s\n", ReliabilityStateToString(pIA->ResConfirmState)));

		configPRINTF(("Observer: "));
		if (pIA->pObserver != NULL) {
			PrintEndpoint(&pIA->pObserver->Ep);
			configPRINTF((", Socket: %04lx, FailCnt: %d, Token: ", (uint32_t )pIA->pObserver->socketHandle, pIA->pObserver->FailCount));
			PrintToken(&pIA->pObserver->Token);
			configPRINTF(("\n"));
		} else {
			configPRINTF(("NONE\n"));
		}

		if (pIA->UpdatePendingNotification) {
			configPRINTF(("Update Pending Notifications\n"));
		}

		// TODO: Consider: Resource description and observers
	}
	configPRINTF(("-------------\n\n"));
}
