//
// Created by Tobias on 27.02.2018.
//

#ifndef APP_EON_WMBUS_REPEATER_COAP_SOCKET_H
#define APP_EON_WMBUS_REPEATER_COAP_SOCKET_H

CoAP_Result_t CoAP_SendMsg(CoAP_Message_t* Msg, Socket_t socketHandle, NetEp_t receiver);
CoAP_Result_t CoAP_SendEmptyAck(uint16_t MessageID, Socket_t socketHandle, NetEp_t receiver);
CoAP_Result_t CoAP_SendEmptyRST(uint16_t MessageID, Socket_t socketHandle, NetEp_t receiver);
CoAP_Result_t CoAP_SendShortResp(CoAP_MessageType_t Type, CoAP_MessageCode_t Code, uint16_t MessageID, CoAP_Token_t token, Socket_t socketHandle, NetEp_t receiver);

#endif //APP_EON_WMBUS_REPEATER_COAP_SOCKET_H
