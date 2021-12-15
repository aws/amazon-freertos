var group__mqtt__struct__types =
[
    [ "MQTTPubAckInfo_t", "struct_m_q_t_t_pub_ack_info__t.html", [
      [ "packetId", "struct_m_q_t_t_pub_ack_info__t.html#a66cef7b43af5d7fdd33b5d2dc766b2d0", null ],
      [ "qos", "struct_m_q_t_t_pub_ack_info__t.html#a086fcd48ef0b787697526a95c861e8a0", null ],
      [ "publishState", "struct_m_q_t_t_pub_ack_info__t.html#a61314203ef87a231c6489c68b579de34", null ]
    ] ],
    [ "MQTTContext_t", "struct_m_q_t_t_context__t.html", [
      [ "outgoingPublishRecords", "struct_m_q_t_t_context__t.html#ad2845a6789e2f49dae22d67c91e48d53", null ],
      [ "incomingPublishRecords", "struct_m_q_t_t_context__t.html#a2603a6d9ba3443a323f2262025b5c5ef", null ],
      [ "transportInterface", "struct_m_q_t_t_context__t.html#a87ab9d61e7711325c2c85ce3ce63386a", null ],
      [ "networkBuffer", "struct_m_q_t_t_context__t.html#a231c5576a6ce389317a3f00f95628276", null ],
      [ "nextPacketId", "struct_m_q_t_t_context__t.html#af47ed55ad7e9bb112324f5f209b70534", null ],
      [ "connectStatus", "struct_m_q_t_t_context__t.html#a4e38c4dc77e7751a0ad8730a41bee47f", null ],
      [ "getTime", "struct_m_q_t_t_context__t.html#aabe1d302a16771292151013e8e30c582", null ],
      [ "appCallback", "struct_m_q_t_t_context__t.html#a73bd9259db9c3a9b84518cbf928ed91f", null ],
      [ "lastPacketTime", "struct_m_q_t_t_context__t.html#abd7afb708927ec69530408defa192d2f", null ],
      [ "controlPacketSent", "struct_m_q_t_t_context__t.html#af9724f2426132e3ce96a03892902ef89", null ],
      [ "keepAliveIntervalSec", "struct_m_q_t_t_context__t.html#afd6071827ef48b230212a5725c2075be", null ],
      [ "pingReqSendTimeMs", "struct_m_q_t_t_context__t.html#acca3efa4146d85f7e874c7c326e23556", null ],
      [ "waitingForPingResp", "struct_m_q_t_t_context__t.html#ac7073f43645f7b7c0c5b7763980004bb", null ]
    ] ],
    [ "MQTTDeserializedInfo_t", "struct_m_q_t_t_deserialized_info__t.html", [
      [ "packetIdentifier", "struct_m_q_t_t_deserialized_info__t.html#af4df2a9926a4a68059195daa712d9b84", null ],
      [ "pPublishInfo", "struct_m_q_t_t_deserialized_info__t.html#ac347273fae1e92b9cbeda1714066c1de", null ],
      [ "deserializationResult", "struct_m_q_t_t_deserialized_info__t.html#a7df1b7b60404c9f1604fec0081d2625d", null ]
    ] ],
    [ "MQTTFixedBuffer_t", "struct_m_q_t_t_fixed_buffer__t.html", [
      [ "pBuffer", "struct_m_q_t_t_fixed_buffer__t.html#acea147448e044870fb36b7fa2347dbd6", null ],
      [ "size", "struct_m_q_t_t_fixed_buffer__t.html#a0b0b6a93cc62751ebeb03095d5431636", null ]
    ] ],
    [ "MQTTConnectInfo_t", "struct_m_q_t_t_connect_info__t.html", [
      [ "cleanSession", "struct_m_q_t_t_connect_info__t.html#a606e7765c4f2215fb2bf630f6eb9ff6b", null ],
      [ "keepAliveSeconds", "struct_m_q_t_t_connect_info__t.html#a7d05d53261732ebdfbb9ee665a347591", null ],
      [ "pClientIdentifier", "struct_m_q_t_t_connect_info__t.html#a010f8f6993cbf8899648d5c515ff7884", null ],
      [ "clientIdentifierLength", "struct_m_q_t_t_connect_info__t.html#a8077ef36ab318f3d35bee6f098fa54d4", null ],
      [ "pUserName", "struct_m_q_t_t_connect_info__t.html#a1118d7d3251a11445318557280db53b4", null ],
      [ "userNameLength", "struct_m_q_t_t_connect_info__t.html#a7165be3bb06d4527ab4eb773b50e05e1", null ],
      [ "pPassword", "struct_m_q_t_t_connect_info__t.html#acec6c79a11d2f0f130802393f34d2b5e", null ],
      [ "passwordLength", "struct_m_q_t_t_connect_info__t.html#a818c4e212a12020a4109eb890ec96383", null ]
    ] ],
    [ "MQTTSubscribeInfo_t", "struct_m_q_t_t_subscribe_info__t.html", [
      [ "qos", "struct_m_q_t_t_subscribe_info__t.html#a64cf2e423f60cfec122eeaef80c0fd86", null ],
      [ "pTopicFilter", "struct_m_q_t_t_subscribe_info__t.html#adb0b28240fdcd82a85f11cf2f8b5bbf0", null ],
      [ "topicFilterLength", "struct_m_q_t_t_subscribe_info__t.html#a6972f8e036f8bde9b1f23a2aacb61382", null ]
    ] ],
    [ "MQTTPublishInfo_t", "struct_m_q_t_t_publish_info__t.html", [
      [ "qos", "struct_m_q_t_t_publish_info__t.html#a178224d02b4acdec7e08e88de0e4b399", null ],
      [ "retain", "struct_m_q_t_t_publish_info__t.html#a343b0af89c46a900db4aa5c775a0975a", null ],
      [ "dup", "struct_m_q_t_t_publish_info__t.html#aa1c8954e83bfa678d1ff5429679d4e89", null ],
      [ "pTopicName", "struct_m_q_t_t_publish_info__t.html#aa80e8ca282d01630f878ad0afe81d7a4", null ],
      [ "topicNameLength", "struct_m_q_t_t_publish_info__t.html#a6161c792d20cc7cf8284c1b71ea1145f", null ],
      [ "pPayload", "struct_m_q_t_t_publish_info__t.html#afc28299f4f625f5e674bb61b42f03380", null ],
      [ "payloadLength", "struct_m_q_t_t_publish_info__t.html#a7997964e11571f35f0c3b729db0f760f", null ]
    ] ],
    [ "MQTTPacketInfo_t", "struct_m_q_t_t_packet_info__t.html", [
      [ "type", "struct_m_q_t_t_packet_info__t.html#a7fef40548c1aa0f0e7f812a6a7243758", null ],
      [ "pRemainingData", "struct_m_q_t_t_packet_info__t.html#ac66cedff052bc844ec9b296387df60bc", null ],
      [ "remainingLength", "struct_m_q_t_t_packet_info__t.html#a7c85becf08de0ec9776dd4be1fcc4bf8", null ]
    ] ],
    [ "TransportInterface_t", "struct_transport_interface__t.html", [
      [ "recv", "struct_transport_interface__t.html#a7c34e9b865e2a509306f09c7dfa3699e", null ],
      [ "send", "struct_transport_interface__t.html#a01cd9935e9a5266ca196243a0054d489", null ],
      [ "pNetworkContext", "struct_transport_interface__t.html#aaf4702050bef8d62714a4d3900e95087", null ]
    ] ],
    [ "NetworkContext_t", "group__mqtt__struct__types.html#ga7769e434e7811caed8cd6fd7f9ec26ec", null ]
];