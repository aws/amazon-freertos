var group__mqtt__agent__struct__types =
[
    [ "MQTTAgentReturnInfo_t", "struct_m_q_t_t_agent_return_info__t.html", [
      [ "returnCode", "struct_m_q_t_t_agent_return_info__t.html#ab04f05e53b8e9039f8983f68b032ccc8", null ],
      [ "pSubackCodes", "struct_m_q_t_t_agent_return_info__t.html#ad68d82134f1f72460f82b83256409542", null ]
    ] ],
    [ "MQTTAgentCommand_t", "struct_m_q_t_t_agent_command.html", [
      [ "commandType", "struct_m_q_t_t_agent_command.html#ae810f32d3650badbe3f8a86d5754adf1", null ],
      [ "pArgs", "struct_m_q_t_t_agent_command.html#a0d41721e83bc91b39b7f54b39c97fafa", null ],
      [ "pCommandCompleteCallback", "struct_m_q_t_t_agent_command.html#a4221813f0ee8d6be1e2b1f60b31fb2a3", null ],
      [ "pCmdContext", "struct_m_q_t_t_agent_command.html#ae7f9dad4bc0c849dae9dd2aa8c20eeef", null ]
    ] ],
    [ "MQTTAgentAckInfo_t", "struct_m_q_t_t_agent_ack_info__t.html", [
      [ "packetId", "struct_m_q_t_t_agent_ack_info__t.html#ac3db584579455d37c7ad8656aa4b03f2", null ],
      [ "pOriginalCommand", "struct_m_q_t_t_agent_ack_info__t.html#a4cddb1600b6ef0d3a34af262d919b2be", null ]
    ] ],
    [ "MQTTAgentContext_t", "struct_m_q_t_t_agent_context__t.html", [
      [ "mqttContext", "struct_m_q_t_t_agent_context__t.html#aec01b2b61213956dc2219f620964a055", null ],
      [ "agentInterface", "struct_m_q_t_t_agent_context__t.html#ab89557b0a015d1848b8d6a9adbff8a4d", null ],
      [ "pPendingAcks", "struct_m_q_t_t_agent_context__t.html#a324ccd898ba0769142b29b4cab38c5be", null ],
      [ "pIncomingCallback", "struct_m_q_t_t_agent_context__t.html#a1e20b7a77aab94f5c775ca3c534cb006", null ],
      [ "pIncomingCallbackContext", "struct_m_q_t_t_agent_context__t.html#a00a73b9d9cfda64aeb9e7b796222dde4", null ],
      [ "packetReceivedInLoop", "struct_m_q_t_t_agent_context__t.html#af284de4e09e0b18411969aad986c8789", null ]
    ] ],
    [ "MQTTAgentSubscribeArgs_t", "struct_m_q_t_t_agent_subscribe_args__t.html", [
      [ "pSubscribeInfo", "struct_m_q_t_t_agent_subscribe_args__t.html#a27e21bfcf0184a4a168a6170a60fc026", null ],
      [ "numSubscriptions", "struct_m_q_t_t_agent_subscribe_args__t.html#ae30aaa33e7a0e67a5989c2d15170de95", null ]
    ] ],
    [ "MQTTAgentConnectArgs_t", "struct_m_q_t_t_agent_connect_args__t.html", [
      [ "pConnectInfo", "struct_m_q_t_t_agent_connect_args__t.html#a9b71b1787c8cfe03654b1ca06ac594fa", null ],
      [ "pWillInfo", "struct_m_q_t_t_agent_connect_args__t.html#a1ab24eb17eebc51bdad53ee2b36c8cdb", null ],
      [ "timeoutMs", "struct_m_q_t_t_agent_connect_args__t.html#a631a199abde2ad2b3e4c69cf211e4d54", null ],
      [ "sessionPresent", "struct_m_q_t_t_agent_connect_args__t.html#a01e3a80d5db4f5f89df44697cca1513f", null ]
    ] ],
    [ "MQTTAgentCommandInfo_t", "struct_m_q_t_t_agent_command_info__t.html", [
      [ "cmdCompleteCallback", "struct_m_q_t_t_agent_command_info__t.html#a4c9e9b55c7b576a390f734238b60f3aa", null ],
      [ "pCmdCompleteCallbackContext", "struct_m_q_t_t_agent_command_info__t.html#a22fc349b76808064646fe71a43d37b96", null ],
      [ "blockTimeMs", "struct_m_q_t_t_agent_command_info__t.html#aa0f490187c1199c2d31d4c04a01d4637", null ]
    ] ],
    [ "MQTTAgentCommandFuncReturns_t", "struct_m_q_t_t_agent_command_func_returns__t.html", [
      [ "packetId", "struct_m_q_t_t_agent_command_func_returns__t.html#ae37354b1e32541cb6b014f270815a28d", null ],
      [ "endLoop", "struct_m_q_t_t_agent_command_func_returns__t.html#aea0742c902ee70380f308b7f957dc9a2", null ],
      [ "addAcknowledgment", "struct_m_q_t_t_agent_command_func_returns__t.html#a474988426514661ae649a613b1dee051", null ],
      [ "runProcessLoop", "struct_m_q_t_t_agent_command_func_returns__t.html#aae5a1d50a22df21950d586f5584e8994", null ]
    ] ],
    [ "MQTTAgentMessageInterface_t", "struct_m_q_t_t_agent_message_interface__t.html", [
      [ "pMsgCtx", "struct_m_q_t_t_agent_message_interface__t.html#a00cad3c3514a03d5deed20609ffdc94b", null ],
      [ "send", "struct_m_q_t_t_agent_message_interface__t.html#abdc8e1c30aa27bf633f4f6e7da9a6465", null ],
      [ "recv", "struct_m_q_t_t_agent_message_interface__t.html#a19e00e6431f53e595837d8ac7e4f744b", null ],
      [ "getCommand", "struct_m_q_t_t_agent_message_interface__t.html#aaa4d0614e8289d7ea12422e66c1e8689", null ],
      [ "releaseCommand", "struct_m_q_t_t_agent_message_interface__t.html#ac809ba46da91df0fc4750ce515bf8f41", null ]
    ] ],
    [ "MQTTAgentCommandContext_t", "group__mqtt__agent__struct__types.html#ga953da1618097a29381f3fc38f576055c", null ],
    [ "MQTTAgentMessageContext_t", "group__mqtt__agent__struct__types.html#ga3c47f2ebcec2e1150e40d039639e5a3b", null ]
];