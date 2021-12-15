<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>core_mqtt.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/</path>
    <filename>core__mqtt_8c.html</filename>
    <includes id="core__mqtt_8h" name="core_mqtt.h" local="yes" imported="no">core_mqtt.h</includes>
    <includes id="core__mqtt__state_8h" name="core_mqtt_state.h" local="yes" imported="no">core_mqtt_state.h</includes>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>sendPacket</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aa90246e1432265a720bed6a4661d636b</anchor>
      <arglist>(MQTTContext_t *pContext, const uint8_t *pBufferToSend, size_t bytesToSend)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>calculateElapsedTime</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a04f9f5742bc28fe29e61f3f46d20d3d6</anchor>
      <arglist>(uint32_t later, uint32_t start)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTPubAckType_t</type>
      <name>getAckFromPacketType</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aa1f35063dbe3b2c35f278ea6aa347a0e</anchor>
      <arglist>(uint8_t packetType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>recvExact</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a509d9dd1ede2bc000035d8f9926a473c</anchor>
      <arglist>(const MQTTContext_t *pContext, size_t bytesToRecv)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>discardPacket</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>abb02f1853a4805205636f2c11a435216</anchor>
      <arglist>(const MQTTContext_t *pContext, size_t remainingLength, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>receivePacket</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aa674664c166b58a5b6630961d8760d3a</anchor>
      <arglist>(const MQTTContext_t *pContext, MQTTPacketInfo_t incomingPacket, uint32_t remainingTimeMs)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t</type>
      <name>getAckTypeToSend</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a02f112e21a6d0b87a3c69ef300d264de</anchor>
      <arglist>(MQTTPublishState_t state)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>sendPublishAcks</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ab4b719d2f726b049c279dcb37fcba840</anchor>
      <arglist>(MQTTContext_t *pContext, uint16_t packetId, MQTTPublishState_t publishState)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>handleKeepAlive</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ae0e50de302a1aa66e3c5b2cdcacc4f3f</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>handleIncomingPublish</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a4ac6e6829500c4f522eae413c9470e4d</anchor>
      <arglist>(MQTTContext_t *pContext, MQTTPacketInfo_t *pIncomingPacket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>handlePublishAcks</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a2363868c0417261c27c750251aad13e5</anchor>
      <arglist>(MQTTContext_t *pContext, MQTTPacketInfo_t *pIncomingPacket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>handleIncomingAck</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a264afa489cbfbd96086614d335969115</anchor>
      <arglist>(MQTTContext_t *pContext, MQTTPacketInfo_t *pIncomingPacket, bool manageKeepAlive)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>receiveSingleIteration</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>acd60a3ca6607331b61c34b7f8204a010</anchor>
      <arglist>(MQTTContext_t *pContext, uint32_t remainingTimeMs, bool manageKeepAlive)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>validateSubscribeUnsubscribeParams</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a37c146709806e0974638784edeb587f8</anchor>
      <arglist>(const MQTTContext_t *pContext, const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>sendPublish</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ad2a473b263a9c86a69bcd0b69e19970b</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTPublishInfo_t *pPublishInfo, size_t headerSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>receiveConnack</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aa3c5d4f6154122cedcce9508ea7d1dce</anchor>
      <arglist>(const MQTTContext_t *pContext, uint32_t timeoutMs, bool cleanSession, MQTTPacketInfo_t *pIncomingPacket, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>handleSessionResumption</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aae9ba11e41bc1dfef340208bc49c836c</anchor>
      <arglist>(MQTTContext_t *pContext, bool sessionPresent)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>serializePublish</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a7d697c65b96ef13fc57e1133d8427ebb</anchor>
      <arglist>(const MQTTContext_t *pContext, const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId, size_t *const pHeaderSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>validatePublishParams</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ad7eda8c9d4a5afa7b3f830dbd8cf4de4</anchor>
      <arglist>(const MQTTContext_t *pContext, const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>matchEndWildcardsSpecialCases</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ab29bb66fe7385c52452a3087bcfbc98e</anchor>
      <arglist>(const char *pTopicFilter, uint16_t topicFilterLength, uint16_t filterIndex)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>matchWildcards</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ab1f061741c445d07454cfa03786a5eea</anchor>
      <arglist>(const char *pTopicName, uint16_t topicNameLength, const char *pTopicFilter, uint16_t topicFilterLength, uint16_t *pNameIndex, uint16_t *pFilterIndex, bool *pMatch)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>matchTopicFilter</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a4c052d9dd6a81e866121c24a2ee2aa0b</anchor>
      <arglist>(const char *pTopicName, uint16_t topicNameLength, const char *pTopicFilter, uint16_t topicFilterLength)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Init</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ae8444f3a85535e62cdb1ae9c192677d6</anchor>
      <arglist>(MQTTContext_t *pContext, const TransportInterface_t *pTransportInterface, MQTTGetCurrentTimeFunc_t getTimeFunction, MQTTEventCallback_t userCallback, const MQTTFixedBuffer_t *pNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Connect</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>aed1e4dc123a8ba79ac569cb17c69bfa0</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, uint32_t timeoutMs, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Subscribe</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a567aa9c38726a7879f9cbf943e813e8f</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Publish</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a1d8217e9d30fb2aed002060a8c97c63e</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Ping</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a66eced0c62ace790354ae3de40fc0959</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Unsubscribe</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a77c911dbe24c5a51aaea88250895dba4</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Disconnect</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ac79c366acbc3dddd88072d99ccb9140c</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_ProcessLoop</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>adce9111350db412c1256689ef9a7b9f4</anchor>
      <arglist>(MQTTContext_t *pContext, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_ReceiveLoop</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a274d7a62c9c011a063031c2e678fb40a</anchor>
      <arglist>(MQTTContext_t *pContext, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>MQTT_GetPacketId</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a00e1a3eba2c21899a6b4312c7d65d090</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_MatchTopic</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a633409812b18547365ec9b853069021b</anchor>
      <arglist>(const char *pTopicName, const uint16_t topicNameLength, const char *pTopicFilter, const uint16_t topicFilterLength, bool *pIsMatch)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetSubAckStatusCodes</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>ac43449e06856c6703cda73359c222bd2</anchor>
      <arglist>(const MQTTPacketInfo_t *pSubackPacket, uint8_t **pPayloadStart, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>MQTT_Status_strerror</name>
      <anchorfile>core__mqtt_8c.html</anchorfile>
      <anchor>a5aa4e3926dc8edf42eb7230f27b7de13</anchor>
      <arglist>(MQTTStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/</path>
    <filename>core__mqtt_8h.html</filename>
    <includes id="core__mqtt__config__defaults_8h" name="core_mqtt_config_defaults.h" local="yes" imported="no">core_mqtt_config_defaults.h</includes>
    <includes id="core__mqtt__serializer_8h" name="core_mqtt_serializer.h" local="yes" imported="no">core_mqtt_serializer.h</includes>
    <includes id="transport__interface_8h" name="transport_interface.h" local="yes" imported="no">transport_interface.h</includes>
    <class kind="struct">MQTTPubAckInfo_t</class>
    <class kind="struct">MQTTContext_t</class>
    <class kind="struct">MQTTDeserializedInfo_t</class>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_ID_INVALID</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga9fde6503edb9eaad50ecd3392ab9992a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t(*</type>
      <name>MQTTGetCurrentTimeFunc_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>gae3bea55b0e49e5208b8c5709a5ea23aa</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>MQTTEventCallback_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>ga00d348277ed4fde23c95bfc749ae954a</anchor>
      <arglist>)(struct MQTTContext *pContext, struct MQTTPacketInfo *pPacketInfo, struct MQTTDeserializedInfo *pDeserializedInfo)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTConnectionStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga9f84d003695205cf10a7bd0bafb3dbf6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNotConnected</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga9f84d003695205cf10a7bd0bafb3dbf6a0320177ebf1f1b2e24646b44702cec69</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTConnected</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga9f84d003695205cf10a7bd0bafb3dbf6a82c8f64d976734e5632e5257bc429ef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTPublishState_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga0480de7552eedd739a26a23fa8e6fd94</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTStateNull</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a8349567b7a9efb3913a64a8f4f6fe5c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPublishSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a896b1507647b504c9208580e4cde26ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubAckSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a65f6f7b343a30fc0558e3aeeb8c97f35</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRecSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a11e2319a2b25b82121471743d39761e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRelSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a5d2ee2709c6dc7a1eb8b9c40f318909b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubCompSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a7d88904d550b502b4424a41aa4205e56</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubAckPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94ab086c55acf106cdc8d420f90899b6803</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRecPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a1bea59454700be9b683b7eb8aaf6bb4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRelPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a695431cde1dc9dc5a2dcbd10eba49df2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubCompPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a3281a28d1829d954b596f091b547b627</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPublishDone</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94ad07733793a235ef9a6a04d16637cd7dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTPubAckType_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga8c1bee959b3ed5fab2a2688dd72bf237</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPuback</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a53d5939c680962f37c15ee87ffd63d0f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubrec</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a8c98d5d1a68dda33d9039009ab4ef053</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubrel</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237af2d737088a231c88e7603acfdbc4fc8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubcomp</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a910c34311ad6a2341afc04839e1c13bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTSubAckStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga48dabc1579e3c0ac6058ce9068054611</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos0</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611abcc3040d7d53025baee3542c40758abb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos1</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611ab180361a6da712c8144d8c499537787d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos2</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611a877b2afbc6ec7d9ab57d4862caadf4f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckFailure</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611aeb83b20da8eda934cde6b92db225a808</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Init</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>ae8444f3a85535e62cdb1ae9c192677d6</anchor>
      <arglist>(MQTTContext_t *pContext, const TransportInterface_t *pTransportInterface, MQTTGetCurrentTimeFunc_t getTimeFunction, MQTTEventCallback_t userCallback, const MQTTFixedBuffer_t *pNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Connect</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>aed1e4dc123a8ba79ac569cb17c69bfa0</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, uint32_t timeoutMs, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Subscribe</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a567aa9c38726a7879f9cbf943e813e8f</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Publish</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a1d8217e9d30fb2aed002060a8c97c63e</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Ping</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a66eced0c62ace790354ae3de40fc0959</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Unsubscribe</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a77c911dbe24c5a51aaea88250895dba4</anchor>
      <arglist>(MQTTContext_t *pContext, const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_Disconnect</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>ac79c366acbc3dddd88072d99ccb9140c</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_ProcessLoop</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>adce9111350db412c1256689ef9a7b9f4</anchor>
      <arglist>(MQTTContext_t *pContext, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_ReceiveLoop</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a274d7a62c9c011a063031c2e678fb40a</anchor>
      <arglist>(MQTTContext_t *pContext, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>MQTT_GetPacketId</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a00e1a3eba2c21899a6b4312c7d65d090</anchor>
      <arglist>(MQTTContext_t *pContext)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_MatchTopic</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a633409812b18547365ec9b853069021b</anchor>
      <arglist>(const char *pTopicName, const uint16_t topicNameLength, const char *pTopicFilter, const uint16_t topicFilterLength, bool *pIsMatch)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetSubAckStatusCodes</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>ac43449e06856c6703cda73359c222bd2</anchor>
      <arglist>(const MQTTPacketInfo_t *pSubackPacket, uint8_t **pPayloadStart, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>MQTT_Status_strerror</name>
      <anchorfile>core__mqtt_8h.html</anchorfile>
      <anchor>a5aa4e3926dc8edf42eb7230f27b7de13</anchor>
      <arglist>(MQTTStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt_config_defaults.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/</path>
    <filename>core__mqtt__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_DO_NOT_USE_CUSTOM_CONFIG</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>abd12bb401eecf3f6694447ea3b1c886d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_STATE_ARRAY_MAX_COUNT</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a50b149716607c0b7974c391830f1b007</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_MAX_CONNACK_RECEIVE_RETRY_COUNT</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a8ca6c96436d5e7c2c8a7933fb28a5c87</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PINGRESP_TIMEOUT_MS</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>afa825fddb52da7df88fb56d2befcd2fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_RECV_POLLING_TIMEOUT_MS</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a43dc9a67427d9e420a65955eea0e2671</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_SEND_RETRY_TIMEOUT_MS</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a73228316a06bbbb91c931dbade56de35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogError</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a8d9dbaaa88129137a4c68ba0456a18b1</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogWarn</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a7da92048aaf0cbfcacde9539c98a0e05</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogInfo</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>a00810b1cb9d2f25d25ce2d4d93815fba</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogDebug</name>
      <anchorfile>core__mqtt__config__defaults_8h.html</anchorfile>
      <anchor>af60e8ffc327d136e5d0d8441ed98c98d</anchor>
      <arglist>(message)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt_serializer.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/</path>
    <filename>core__mqtt__serializer_8c.html</filename>
    <includes id="core__mqtt__serializer_8h" name="core_mqtt_serializer.h" local="yes" imported="no">core_mqtt_serializer.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_VERSION_3_1_1</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a7c621dd360dd439f0d6d8dc5d951a619</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_CONNECT_HEADER_SIZE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aa7c310cb084af0025c356ed844ae443d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_CLEAN</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a1b131e766e003e36fe499d9f6a79fc03</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_WILL</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a04d8c55ea2b595a277cbcd4340e36d6c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_WILL_QOS1</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a2aee739b1fa7e61feb907bc92a73c3b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_WILL_QOS2</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ac750789b338a2b9be75725ab340dabce</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_WILL_RETAIN</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a8ae294d4ca7960920816339fedbdc4a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_PASSWORD</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ac5f0bb47789c1182392f5029e0238a81</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_CONNECT_FLAG_USERNAME</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a8d23d14a4cf296feffb9db79728dd1d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_FLAG_RETAIN</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a3d04b1e1ad7ec25d18fd13726e164f06</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_FLAG_QOS1</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ac23212835606fade167fb5ce25eaf103</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_FLAG_QOS2</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>afe1d2a0b7c0803f5a20ebb3c7a607d65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_FLAG_DUP</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a57c437ecc3720de76093b08eb0d4f813</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_DISCONNECT_PACKET_SIZE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>abdcffcd69d858203747236b5c4afa834</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_PINGREQ_SIZE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6e8a49d0d88f0b038a5379d533858103</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_DISCONNECT_REMAINING_LENGTH</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ac4f3ff016aa6011e3fc707b9f27f6b8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_CONNACK_REMAINING_LENGTH</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a999c4ebc0d629df9b08a3c88107b5b80</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_CONNACK_SESSION_PRESENT_MASK</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aab69dd14c12f8086245c2371288944f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_SIMPLE_ACK_REMAINING_LENGTH</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a4c576df64bca769a91cb64d5d5d86505</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_PINGRESP_REMAINING_LENGTH</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aeab5c92e86ed98750cbf6422b8b57c06</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_MAX_REMAINING_LENGTH</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a9d9ea40a1ff486557a553523a0743647</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT8_SET_BIT</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>af259c91b3075c24df53fa3ffe516b208</anchor>
      <arglist>(x, position)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT8_CHECK_BIT</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a07cc5f3f934e1ebf8011a6c15a667206</anchor>
      <arglist>(x, position)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_HIGH_BYTE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a24aab781ef139dd38be534ee137ea2f9</anchor>
      <arglist>(x)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_LOW_BYTE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>af2ae35b27e0140a77238cd175508cb4e</anchor>
      <arglist>(x)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_DECODE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>acc849aa739edff3ec532219a3860a3a0</anchor>
      <arglist>(ptr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_REMAINING_LENGTH_INVALID</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6f6b43661df6f9e9e9e7123ab01e9eb5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_MIN_PUBLISH_REMAINING_LENGTH_QOS0</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a3b8709529a24bc195c7310183ffbcc2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTSubscriptionType_t</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a92e39b92b76d439a183fc6f5e300195f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTT_SUBSCRIBE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a92e39b92b76d439a183fc6f5e300195fa7dd20d5d68728190c8c1050599b562f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTT_UNSUBSCRIBE</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a92e39b92b76d439a183fc6f5e300195fa94eb5b78f584ff379c799a142b03e7a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>serializePublishCommon</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6b4138d990e2c8fbedbe28683c0fa83a</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, size_t remainingLength, uint16_t packetIdentifier, const MQTTFixedBuffer_t *pFixedBuffer, bool serializePayload)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>calculatePublishPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a76d0156e521588fb3319043f9d35ea9a</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>calculateSubscriptionPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>abdbabda0aa9db25166963afa975adb10</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, size_t *pRemainingLength, size_t *pPacketSize, MQTTSubscriptionType_t subscriptionType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>validateSubscriptionSerializeParams</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a81262cb0b9d47dee979420f6fd8a7271</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>serializeConnectPacket</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a95bf697a3b1f86950e5c199d9cf3a185</anchor>
      <arglist>(const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>logConnackResponse</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a5451f2e3468faaf2bdf85220ebb95aaa</anchor>
      <arglist>(uint8_t responseCode)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t *</type>
      <name>encodeRemainingLength</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a3a3858fbb0cbd845f208b3fc60f36130</anchor>
      <arglist>(uint8_t *pDestination, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>remainingLengthEncodedSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aeead0813fa045d754e3d6ec964d0686e</anchor>
      <arglist>(size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t *</type>
      <name>encodeString</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a60e580c28431eb08f05a156949137f1f</anchor>
      <arglist>(uint8_t *pDestination, const char *pSource, uint16_t sourceLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>getRemainingLength</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a5685b753d1d42788a00bd59ffa4639e2</anchor>
      <arglist>(TransportRecv_t recvFunc, NetworkContext_t *pNetworkContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>incomingPacketValid</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a03dfebbfbc1635567839f7abb7c0f8db</anchor>
      <arglist>(uint8_t packetType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>checkPublishRemainingLength</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a33a2680aab1ce2186acd7c78aeb270f1</anchor>
      <arglist>(size_t remainingLength, MQTTQoS_t qos, size_t qos0Minimum)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>processPublishFlags</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a47a044115ee5df1ac7fe02d2ee37e1e0</anchor>
      <arglist>(uint8_t publishFlags, MQTTPublishInfo_t *pPublishInfo)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>deserializeConnack</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aa7b25e1e3114536e9b0526fc93a1f76c</anchor>
      <arglist>(const MQTTPacketInfo_t *pConnack, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>readSubackStatus</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a02cace9415c300f4dd3394bf1eee0787</anchor>
      <arglist>(size_t statusCount, const uint8_t *pStatusStart)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>deserializeSuback</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ae7b71036fc19c9a6da480dcfd3a2387b</anchor>
      <arglist>(const MQTTPacketInfo_t *pSuback, uint16_t *pPacketIdentifier)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>deserializePublish</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6e8bcde1280e14706e0cb9180358607c</anchor>
      <arglist>(const MQTTPacketInfo_t *pIncomingPacket, uint16_t *pPacketId, MQTTPublishInfo_t *pPublishInfo)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>deserializeSimpleAck</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a5d437c287290fa28a0ed65635fd6c9ae</anchor>
      <arglist>(const MQTTPacketInfo_t *pAck, uint16_t *pPacketIdentifier)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>deserializePingresp</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>afdd9b08562ccaa6cf8dd68baa6bc7060</anchor>
      <arglist>(const MQTTPacketInfo_t *pPingresp)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetConnectPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a4e57ccef527a25b572dc66eeb2e217c5</anchor>
      <arglist>(const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeConnect</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aa2e2300d6c43e61f8f2cf83f7149835c</anchor>
      <arglist>(const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetSubscribePacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>abb9a703cb23ab39fdd6fe282a5f3ddc5</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeSubscribe</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a21273b13070e8340cc33b0f86bf79571</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetUnsubscribePacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a2a18563d5f63c8975b57118a6836c932</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeUnsubscribe</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>aab58219c203077c07ffd7061405e1adb</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetPublishPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a9971fb98c6af22b1bfe697d44492a819</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePublish</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ac6c453f9c4b7f48aad511bc677ec7eb9</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePublishHeader</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a659cf2bcaf2c5131daa0acc1917401a2</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer, size_t *pHeaderSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeAck</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a11ea4ac5ea27e93121288e463ca34ee6</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer, uint8_t packetType, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetDisconnectPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6fdd8cbde6b7c4ff85c20aaca0fd8741</anchor>
      <arglist>(size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeDisconnect</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a6aae40d4656eb533a74b67bf9c827d3b</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetPingreqPacketSize</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a015562f30e220d2534f773bfa45a5cfe</anchor>
      <arglist>(size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePingreq</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>af3b3e40858fd984c871511e02a61e15d</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_DeserializePublish</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a4c2aec031f31d0fe55c1cda46544e95a</anchor>
      <arglist>(const MQTTPacketInfo_t *pIncomingPacket, uint16_t *pPacketId, MQTTPublishInfo_t *pPublishInfo)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_DeserializeAck</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>ae9971855df71edf94124116e0625bf18</anchor>
      <arglist>(const MQTTPacketInfo_t *pIncomingPacket, uint16_t *pPacketId, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetIncomingPacketTypeAndLength</name>
      <anchorfile>core__mqtt__serializer_8c.html</anchorfile>
      <anchor>a98cdda86f86a0a1888745a584199e930</anchor>
      <arglist>(TransportRecv_t readFunc, NetworkContext_t *pNetworkContext, MQTTPacketInfo_t *pIncomingPacket)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt_serializer.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/</path>
    <filename>core__mqtt__serializer_8h.html</filename>
    <includes id="core__mqtt__config__defaults_8h" name="core_mqtt_config_defaults.h" local="yes" imported="no">core_mqtt_config_defaults.h</includes>
    <includes id="transport__interface_8h" name="transport_interface.h" local="yes" imported="no">transport_interface.h</includes>
    <class kind="struct">MQTTFixedBuffer_t</class>
    <class kind="struct">MQTTConnectInfo_t</class>
    <class kind="struct">MQTTSubscribeInfo_t</class>
    <class kind="struct">MQTTPublishInfo_t</class>
    <class kind="struct">MQTTPacketInfo_t</class>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_CONNECT</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga64a0515bda2ecc89e97595535e1cf0ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_CONNACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gab14f6c39c303eac1a76816edfde7feab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBLISH</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga5b2d79c61f2591c8e5772f974826d4ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga5f279d63de47a973b41b95f74f47a4f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBREC</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gafa2d8f28da39560f152076b99610e6a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBREL</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gaeaa2ceecffda50e2d22ccecf046083c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBCOMP</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga478ecbc98d2ca83d4ce7db33622b5c3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_SUBSCRIBE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga80cfef333e60d967ca927b2e5e665f18</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_SUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga307e0186aa17fdd0d6d181d3d2715766</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_UNSUBSCRIBE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga4a94c954cfcea31c8fc3e2adf092b228</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_UNSUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga38bc8ed0b9a1581cf85cecdede7d1a64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PINGREQ</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gacbe28c7d081275d1805c2142ff792185</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PINGRESP</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga285fc02048e2482794042fa98639e514</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_DISCONNECT</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gaed07155a3d6fa4b7624b9f36ed33ec6d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_ACK_PACKET_SIZE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga26994fcfacb1cff892caa45ec31ca7c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gaba7ec045874a1c3432f99173367f735c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSuccess</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca484e062cb4f3fccc1858dd25cfeee056</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTBadParameter</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa39030c93b0263b2699502a074f003b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNoMemory</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735cab1be4db832a0468f024243bca151a8df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSendFailed</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735cafd06b63fe9677fa2af06b0f4c7d4ad55</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTRecvFailed</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa14bc8aa4ad218702d782366945d43ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTBadResponse</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa5d7507e7664a14d63a8bc44b280093e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTServerRefused</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca25a3d1747e308e99daa805fe576f84b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNoDataAvailable</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca676f21c0ddf297ae3ec874bc829aa957</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTIllegalState</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca67905d7a05f98faa557a73eb5092bd8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTStateCollision</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca8d05b92240dea6df08eab5a9e3799c11</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTKeepAliveTimeout</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca076ca8965e836a06e707a94adb26144f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTQoS_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gae308a5928d7f537379c29a894228093a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS0</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aaad51b23a1ae1417f96d8f343c788d1d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS1</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aa019d0b8a8cfadb6f98462b046bdacbb2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS2</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aa85e04ac0465cbdef6dd69ff71b2bbfbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetConnectPacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a4e57ccef527a25b572dc66eeb2e217c5</anchor>
      <arglist>(const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeConnect</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>aa2e2300d6c43e61f8f2cf83f7149835c</anchor>
      <arglist>(const MQTTConnectInfo_t *pConnectInfo, const MQTTPublishInfo_t *pWillInfo, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetSubscribePacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>abb9a703cb23ab39fdd6fe282a5f3ddc5</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeSubscribe</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a21273b13070e8340cc33b0f86bf79571</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetUnsubscribePacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a2a18563d5f63c8975b57118a6836c932</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeUnsubscribe</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>aab58219c203077c07ffd7061405e1adb</anchor>
      <arglist>(const MQTTSubscribeInfo_t *pSubscriptionList, size_t subscriptionCount, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetPublishPacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a9971fb98c6af22b1bfe697d44492a819</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, size_t *pRemainingLength, size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePublish</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>ac6c453f9c4b7f48aad511bc677ec7eb9</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePublishHeader</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a659cf2bcaf2c5131daa0acc1917401a2</anchor>
      <arglist>(const MQTTPublishInfo_t *pPublishInfo, uint16_t packetId, size_t remainingLength, const MQTTFixedBuffer_t *pFixedBuffer, size_t *pHeaderSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeAck</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a11ea4ac5ea27e93121288e463ca34ee6</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer, uint8_t packetType, uint16_t packetId)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetDisconnectPacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a6fdd8cbde6b7c4ff85c20aaca0fd8741</anchor>
      <arglist>(size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializeDisconnect</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a6aae40d4656eb533a74b67bf9c827d3b</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetPingreqPacketSize</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a015562f30e220d2534f773bfa45a5cfe</anchor>
      <arglist>(size_t *pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_SerializePingreq</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>af3b3e40858fd984c871511e02a61e15d</anchor>
      <arglist>(const MQTTFixedBuffer_t *pFixedBuffer)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_DeserializePublish</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a4c2aec031f31d0fe55c1cda46544e95a</anchor>
      <arglist>(const MQTTPacketInfo_t *pIncomingPacket, uint16_t *pPacketId, MQTTPublishInfo_t *pPublishInfo)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_DeserializeAck</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>ae9971855df71edf94124116e0625bf18</anchor>
      <arglist>(const MQTTPacketInfo_t *pIncomingPacket, uint16_t *pPacketId, bool *pSessionPresent)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_GetIncomingPacketTypeAndLength</name>
      <anchorfile>core__mqtt__serializer_8h.html</anchorfile>
      <anchor>a98cdda86f86a0a1888745a584199e930</anchor>
      <arglist>(TransportRecv_t readFunc, NetworkContext_t *pNetworkContext, MQTTPacketInfo_t *pIncomingPacket)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt_state.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/</path>
    <filename>core__mqtt__state_8c.html</filename>
    <includes id="core__mqtt__state_8h" name="core_mqtt_state.h" local="yes" imported="no">core_mqtt_state.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_BITMAP_BIT_SET_AT</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a50892214c51968df798f584272f16a17</anchor>
      <arglist>(position)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_SET_BIT</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>acd96521b31682b7d93de544704fd9594</anchor>
      <arglist>(x, position)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>UINT16_CHECK_BIT</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a085ab1307745f304ce2e6d24bdc3f6a7</anchor>
      <arglist>(x, position)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>validateTransitionPublish</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>aad1473b9a2d46be62c3e80dd3524af9d</anchor>
      <arglist>(MQTTPublishState_t currentState, MQTTPublishState_t newState, MQTTStateOperation_t opType, MQTTQoS_t qos)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>validateTransitionAck</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>ac85ca8874163b399b7f8e5e17d3c5872</anchor>
      <arglist>(MQTTPublishState_t currentState, MQTTPublishState_t newState)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>isPublishOutgoing</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>aaf9d4c6e766e40189ff7b68ffea40aa0</anchor>
      <arglist>(MQTTPubAckType_t packetType, MQTTStateOperation_t opType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>findInRecord</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>ac805558ac65e84ea9111ce70c873e59e</anchor>
      <arglist>(const MQTTPubAckInfo_t *records, size_t recordCount, uint16_t packetId, MQTTQoS_t *pQos, MQTTPublishState_t *pCurrentState)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>compactRecords</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a6cd7b86de2ddb125fee886d58d1a5fd4</anchor>
      <arglist>(MQTTPubAckInfo_t *records, size_t recordCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>addRecord</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a5d0ffdfde0c38a1cc1d4e3f4750a8cc4</anchor>
      <arglist>(MQTTPubAckInfo_t *records, size_t recordCount, uint16_t packetId, MQTTQoS_t qos, MQTTPublishState_t publishState)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>updateRecord</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a819c7c72087621fcf97a028bff02759e</anchor>
      <arglist>(MQTTPubAckInfo_t *records, size_t recordIndex, MQTTPublishState_t newState, bool shouldDelete)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint16_t</type>
      <name>stateSelect</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>adfc09b0c75d5de09cd73650f944699c0</anchor>
      <arglist>(const MQTTContext_t *pMqttContext, uint16_t searchStates, MQTTStateCursor_t *pCursor)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>updateStateAck</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a897d2385c4f4f49589e664e1df0afd3f</anchor>
      <arglist>(MQTTPubAckInfo_t *records, size_t recordIndex, uint16_t packetId, MQTTPublishState_t currentState, MQTTPublishState_t newState)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static MQTTStatus_t</type>
      <name>updateStatePublish</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a05a9eda7866e5c9fa95d4c447b1f4921</anchor>
      <arglist>(MQTTContext_t *pMqttContext, size_t recordIndex, uint16_t packetId, MQTTStateOperation_t opType, MQTTQoS_t qos, MQTTPublishState_t currentState, MQTTPublishState_t newState)</arglist>
    </member>
    <member kind="function">
      <type>MQTTPublishState_t</type>
      <name>MQTT_CalculateStateAck</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a0cad28e34f03b84aff43ee243ce8e2cf</anchor>
      <arglist>(MQTTPubAckType_t packetType, MQTTStateOperation_t opType, MQTTQoS_t qos)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_ReserveState</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>ae47d7b3a2ddcaeddfb0ed9063801ca7f</anchor>
      <arglist>(MQTTContext_t *pMqttContext, uint16_t packetId, MQTTQoS_t qos)</arglist>
    </member>
    <member kind="function">
      <type>MQTTPublishState_t</type>
      <name>MQTT_CalculateStatePublish</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>aadc4fdd8af74ac25b848a33e916bff50</anchor>
      <arglist>(MQTTStateOperation_t opType, MQTTQoS_t qos)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_UpdateStatePublish</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a0791ed4537962861ab5d15e0094b195f</anchor>
      <arglist>(MQTTContext_t *pMqttContext, uint16_t packetId, MQTTStateOperation_t opType, MQTTQoS_t qos, MQTTPublishState_t *pNewState)</arglist>
    </member>
    <member kind="function">
      <type>MQTTStatus_t</type>
      <name>MQTT_UpdateStateAck</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>aed480e79fa1300737c16964e7082afa8</anchor>
      <arglist>(MQTTContext_t *pMqttContext, uint16_t packetId, MQTTPubAckType_t packetType, MQTTStateOperation_t opType, MQTTPublishState_t *pNewState)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>MQTT_PubrelToResend</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>ae58ade262ec01262687554b349b2fdf5</anchor>
      <arglist>(const MQTTContext_t *pMqttContext, MQTTStateCursor_t *pCursor, MQTTPublishState_t *pState)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>MQTT_PublishToResend</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a44b3cf50dc477a9f97413a9238a961f6</anchor>
      <arglist>(const MQTTContext_t *pMqttContext, MQTTStateCursor_t *pCursor)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>MQTT_State_strerror</name>
      <anchorfile>core__mqtt__state_8c.html</anchorfile>
      <anchor>a53d786203ca4d5d5630a9eb3dd4cddae</anchor>
      <arglist>(MQTTPublishState_t state)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_mqtt_state.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/include/</path>
    <filename>core__mqtt__state_8h.html</filename>
    <includes id="core__mqtt_8h" name="core_mqtt.h" local="yes" imported="no">core_mqtt.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_STATE_CURSOR_INITIALIZER</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga666ad78e7eaaffa51f5cab96201a9476</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>size_t</type>
      <name>MQTTStateCursor_t</name>
      <anchorfile>group__mqtt__basic__types.html</anchorfile>
      <anchor>ga2ca7d486d83fe555953a8c7876ee0d6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>MQTT_PublishToResend</name>
      <anchorfile>core__mqtt__state_8h.html</anchorfile>
      <anchor>a44b3cf50dc477a9f97413a9238a961f6</anchor>
      <arglist>(const MQTTContext_t *pMqttContext, MQTTStateCursor_t *pCursor)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>transport_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreMQTT/source/interface/</path>
    <filename>transport__interface_8h.html</filename>
    <class kind="struct">TransportInterface_t</class>
    <member kind="typedef">
      <type>struct NetworkContext</type>
      <name>NetworkContext_t</name>
      <anchorfile>group__mqtt__struct__types.html</anchorfile>
      <anchor>ga7769e434e7811caed8cd6fd7f9ec26ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportRecv_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>gaff3e08cfa7368b526ec1f8d87d10d7c2</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportSend_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>ga90d540c6a334b9b73f94d83f08a0edd6</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTConnectInfo_t</name>
    <filename>struct_m_q_t_t_connect_info__t.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>cleanSession</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a606e7765c4f2215fb2bf630f6eb9ff6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>keepAliveSeconds</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a7d05d53261732ebdfbb9ee665a347591</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pClientIdentifier</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a010f8f6993cbf8899648d5c515ff7884</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>clientIdentifierLength</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a8077ef36ab318f3d35bee6f098fa54d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pUserName</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a1118d7d3251a11445318557280db53b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>userNameLength</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a7165be3bb06d4527ab4eb773b50e05e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pPassword</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>acec6c79a11d2f0f130802393f34d2b5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>passwordLength</name>
      <anchorfile>struct_m_q_t_t_connect_info__t.html</anchorfile>
      <anchor>a818c4e212a12020a4109eb890ec96383</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTContext_t</name>
    <filename>struct_m_q_t_t_context__t.html</filename>
    <member kind="variable">
      <type>MQTTPubAckInfo_t</type>
      <name>outgoingPublishRecords</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>ad2845a6789e2f49dae22d67c91e48d53</anchor>
      <arglist>[MQTT_STATE_ARRAY_MAX_COUNT]</arglist>
    </member>
    <member kind="variable">
      <type>MQTTPubAckInfo_t</type>
      <name>incomingPublishRecords</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>a2603a6d9ba3443a323f2262025b5c5ef</anchor>
      <arglist>[MQTT_STATE_ARRAY_MAX_COUNT]</arglist>
    </member>
    <member kind="variable">
      <type>TransportInterface_t</type>
      <name>transportInterface</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>a87ab9d61e7711325c2c85ce3ce63386a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTFixedBuffer_t</type>
      <name>networkBuffer</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>a231c5576a6ce389317a3f00f95628276</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>nextPacketId</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>af47ed55ad7e9bb112324f5f209b70534</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTConnectionStatus_t</type>
      <name>connectStatus</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>a4e38c4dc77e7751a0ad8730a41bee47f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTGetCurrentTimeFunc_t</type>
      <name>getTime</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>aabe1d302a16771292151013e8e30c582</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTEventCallback_t</type>
      <name>appCallback</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>a73bd9259db9c3a9b84518cbf928ed91f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>lastPacketTime</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>abd7afb708927ec69530408defa192d2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>controlPacketSent</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>af9724f2426132e3ce96a03892902ef89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>keepAliveIntervalSec</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>afd6071827ef48b230212a5725c2075be</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>pingReqSendTimeMs</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>acca3efa4146d85f7e874c7c326e23556</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>waitingForPingResp</name>
      <anchorfile>struct_m_q_t_t_context__t.html</anchorfile>
      <anchor>ac7073f43645f7b7c0c5b7763980004bb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTDeserializedInfo_t</name>
    <filename>struct_m_q_t_t_deserialized_info__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>packetIdentifier</name>
      <anchorfile>struct_m_q_t_t_deserialized_info__t.html</anchorfile>
      <anchor>af4df2a9926a4a68059195daa712d9b84</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTPublishInfo_t *</type>
      <name>pPublishInfo</name>
      <anchorfile>struct_m_q_t_t_deserialized_info__t.html</anchorfile>
      <anchor>ac347273fae1e92b9cbeda1714066c1de</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTStatus_t</type>
      <name>deserializationResult</name>
      <anchorfile>struct_m_q_t_t_deserialized_info__t.html</anchorfile>
      <anchor>a7df1b7b60404c9f1604fec0081d2625d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTFixedBuffer_t</name>
    <filename>struct_m_q_t_t_fixed_buffer__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBuffer</name>
      <anchorfile>struct_m_q_t_t_fixed_buffer__t.html</anchorfile>
      <anchor>acea147448e044870fb36b7fa2347dbd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>struct_m_q_t_t_fixed_buffer__t.html</anchorfile>
      <anchor>a0b0b6a93cc62751ebeb03095d5431636</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTPacketInfo_t</name>
    <filename>struct_m_q_t_t_packet_info__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>type</name>
      <anchorfile>struct_m_q_t_t_packet_info__t.html</anchorfile>
      <anchor>a7fef40548c1aa0f0e7f812a6a7243758</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pRemainingData</name>
      <anchorfile>struct_m_q_t_t_packet_info__t.html</anchorfile>
      <anchor>ac66cedff052bc844ec9b296387df60bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>remainingLength</name>
      <anchorfile>struct_m_q_t_t_packet_info__t.html</anchorfile>
      <anchor>a7c85becf08de0ec9776dd4be1fcc4bf8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTPubAckInfo_t</name>
    <filename>struct_m_q_t_t_pub_ack_info__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>packetId</name>
      <anchorfile>struct_m_q_t_t_pub_ack_info__t.html</anchorfile>
      <anchor>a66cef7b43af5d7fdd33b5d2dc766b2d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTQoS_t</type>
      <name>qos</name>
      <anchorfile>struct_m_q_t_t_pub_ack_info__t.html</anchorfile>
      <anchor>a086fcd48ef0b787697526a95c861e8a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MQTTPublishState_t</type>
      <name>publishState</name>
      <anchorfile>struct_m_q_t_t_pub_ack_info__t.html</anchorfile>
      <anchor>a61314203ef87a231c6489c68b579de34</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTPublishInfo_t</name>
    <filename>struct_m_q_t_t_publish_info__t.html</filename>
    <member kind="variable">
      <type>MQTTQoS_t</type>
      <name>qos</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>a178224d02b4acdec7e08e88de0e4b399</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>retain</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>a343b0af89c46a900db4aa5c775a0975a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>dup</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>aa1c8954e83bfa678d1ff5429679d4e89</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pTopicName</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>aa80e8ca282d01630f878ad0afe81d7a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>topicNameLength</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>a6161c792d20cc7cf8284c1b71ea1145f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const void *</type>
      <name>pPayload</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>afc28299f4f625f5e674bb61b42f03380</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>payloadLength</name>
      <anchorfile>struct_m_q_t_t_publish_info__t.html</anchorfile>
      <anchor>a7997964e11571f35f0c3b729db0f760f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>MQTTSubscribeInfo_t</name>
    <filename>struct_m_q_t_t_subscribe_info__t.html</filename>
    <member kind="variable">
      <type>MQTTQoS_t</type>
      <name>qos</name>
      <anchorfile>struct_m_q_t_t_subscribe_info__t.html</anchorfile>
      <anchor>a64cf2e423f60cfec122eeaef80c0fd86</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pTopicFilter</name>
      <anchorfile>struct_m_q_t_t_subscribe_info__t.html</anchorfile>
      <anchor>adb0b28240fdcd82a85f11cf2f8b5bbf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>topicFilterLength</name>
      <anchorfile>struct_m_q_t_t_subscribe_info__t.html</anchorfile>
      <anchor>a6972f8e036f8bde9b1f23a2aacb61382</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TransportInterface_t</name>
    <filename>struct_transport_interface__t.html</filename>
    <member kind="variable">
      <type>TransportRecv_t</type>
      <name>recv</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>a7c34e9b865e2a509306f09c7dfa3699e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TransportSend_t</type>
      <name>send</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>a01cd9935e9a5266ca196243a0054d489</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>NetworkContext_t *</type>
      <name>pNetworkContext</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>aaf4702050bef8d62714a4d3900e95087</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>mqtt_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__mqtt__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>MQTTConnectionStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga9f84d003695205cf10a7bd0bafb3dbf6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNotConnected</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga9f84d003695205cf10a7bd0bafb3dbf6a0320177ebf1f1b2e24646b44702cec69</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTConnected</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga9f84d003695205cf10a7bd0bafb3dbf6a82c8f64d976734e5632e5257bc429ef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTPublishState_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga0480de7552eedd739a26a23fa8e6fd94</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTStateNull</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a8349567b7a9efb3913a64a8f4f6fe5c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPublishSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a896b1507647b504c9208580e4cde26ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubAckSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a65f6f7b343a30fc0558e3aeeb8c97f35</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRecSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a11e2319a2b25b82121471743d39761e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRelSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a5d2ee2709c6dc7a1eb8b9c40f318909b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubCompSend</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a7d88904d550b502b4424a41aa4205e56</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubAckPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94ab086c55acf106cdc8d420f90899b6803</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRecPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a1bea59454700be9b683b7eb8aaf6bb4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubRelPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a695431cde1dc9dc5a2dcbd10eba49df2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubCompPending</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94a3281a28d1829d954b596f091b547b627</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPublishDone</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga0480de7552eedd739a26a23fa8e6fd94ad07733793a235ef9a6a04d16637cd7dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTPubAckType_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga8c1bee959b3ed5fab2a2688dd72bf237</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPuback</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a53d5939c680962f37c15ee87ffd63d0f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubrec</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a8c98d5d1a68dda33d9039009ab4ef053</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubrel</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237af2d737088a231c88e7603acfdbc4fc8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTPubcomp</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga8c1bee959b3ed5fab2a2688dd72bf237a910c34311ad6a2341afc04839e1c13bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTSubAckStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ga48dabc1579e3c0ac6058ce9068054611</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos0</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611abcc3040d7d53025baee3542c40758abb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos1</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611ab180361a6da712c8144d8c499537787d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckSuccessQos2</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611a877b2afbc6ec7d9ab57d4862caadf4f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSubAckFailure</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gga48dabc1579e3c0ac6058ce9068054611aeb83b20da8eda934cde6b92db225a808</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTStatus_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gaba7ec045874a1c3432f99173367f735c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSuccess</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca484e062cb4f3fccc1858dd25cfeee056</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTBadParameter</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa39030c93b0263b2699502a074f003b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNoMemory</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735cab1be4db832a0468f024243bca151a8df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTSendFailed</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735cafd06b63fe9677fa2af06b0f4c7d4ad55</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTRecvFailed</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa14bc8aa4ad218702d782366945d43ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTBadResponse</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735caa5d7507e7664a14d63a8bc44b280093e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTServerRefused</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca25a3d1747e308e99daa805fe576f84b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTNoDataAvailable</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca676f21c0ddf297ae3ec874bc829aa957</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTIllegalState</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca67905d7a05f98faa557a73eb5092bd8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTStateCollision</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca8d05b92240dea6df08eab5a9e3799c11</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTKeepAliveTimeout</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggaba7ec045874a1c3432f99173367f735ca076ca8965e836a06e707a94adb26144f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>MQTTQoS_t</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>gae308a5928d7f537379c29a894228093a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS0</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aaad51b23a1ae1417f96d8f343c788d1d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS1</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aa019d0b8a8cfadb6f98462b046bdacbb2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MQTTQoS2</name>
      <anchorfile>group__mqtt__enum__types.html</anchorfile>
      <anchor>ggae308a5928d7f537379c29a894228093aa85e04ac0465cbdef6dd69ff71b2bbfbb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>mqtt_callback_types</name>
    <title>Callback Types</title>
    <filename>group__mqtt__callback__types.html</filename>
    <member kind="typedef">
      <type>uint32_t(*</type>
      <name>MQTTGetCurrentTimeFunc_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>gae3bea55b0e49e5208b8c5709a5ea23aa</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>MQTTEventCallback_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>ga00d348277ed4fde23c95bfc749ae954a</anchor>
      <arglist>)(struct MQTTContext *pContext, struct MQTTPacketInfo *pPacketInfo, struct MQTTDeserializedInfo *pDeserializedInfo)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportRecv_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>gaff3e08cfa7368b526ec1f8d87d10d7c2</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportSend_t</name>
      <anchorfile>group__mqtt__callback__types.html</anchorfile>
      <anchor>ga90d540c6a334b9b73f94d83f08a0edd6</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>mqtt_struct_types</name>
    <title>Parameter Structures</title>
    <filename>group__mqtt__struct__types.html</filename>
    <class kind="struct">MQTTPubAckInfo_t</class>
    <class kind="struct">MQTTContext_t</class>
    <class kind="struct">MQTTDeserializedInfo_t</class>
    <class kind="struct">MQTTFixedBuffer_t</class>
    <class kind="struct">MQTTConnectInfo_t</class>
    <class kind="struct">MQTTSubscribeInfo_t</class>
    <class kind="struct">MQTTPublishInfo_t</class>
    <class kind="struct">MQTTPacketInfo_t</class>
    <class kind="struct">TransportInterface_t</class>
    <member kind="typedef">
      <type>struct NetworkContext</type>
      <name>NetworkContext_t</name>
      <anchorfile>group__mqtt__struct__types.html</anchorfile>
      <anchor>ga7769e434e7811caed8cd6fd7f9ec26ec</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>mqtt_basic_types</name>
    <title>Basic Types</title>
    <filename>group__mqtt__basic__types.html</filename>
    <member kind="typedef">
      <type>size_t</type>
      <name>MQTTStateCursor_t</name>
      <anchorfile>group__mqtt__basic__types.html</anchorfile>
      <anchor>ga2ca7d486d83fe555953a8c7876ee0d6e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>mqtt_constants</name>
    <title>Constants</title>
    <filename>group__mqtt__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_ID_INVALID</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga9fde6503edb9eaad50ecd3392ab9992a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_CONNECT</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga64a0515bda2ecc89e97595535e1cf0ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_CONNACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gab14f6c39c303eac1a76816edfde7feab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBLISH</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga5b2d79c61f2591c8e5772f974826d4ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga5f279d63de47a973b41b95f74f47a4f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBREC</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gafa2d8f28da39560f152076b99610e6a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBREL</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gaeaa2ceecffda50e2d22ccecf046083c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PUBCOMP</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga478ecbc98d2ca83d4ce7db33622b5c3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_SUBSCRIBE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga80cfef333e60d967ca927b2e5e665f18</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_SUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga307e0186aa17fdd0d6d181d3d2715766</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_UNSUBSCRIBE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga4a94c954cfcea31c8fc3e2adf092b228</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_UNSUBACK</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga38bc8ed0b9a1581cf85cecdede7d1a64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PINGREQ</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gacbe28c7d081275d1805c2142ff792185</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_PINGRESP</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga285fc02048e2482794042fa98639e514</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PACKET_TYPE_DISCONNECT</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>gaed07155a3d6fa4b7624b9f36ed33ec6d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_PUBLISH_ACK_PACKET_SIZE</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga26994fcfacb1cff892caa45ec31ca7c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_STATE_CURSOR_INITIALIZER</name>
      <anchorfile>group__mqtt__constants.html</anchorfile>
      <anchor>ga666ad78e7eaaffa51f5cab96201a9476</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>mqtt_design</name>
    <title>Design</title>
    <filename>mqtt_design.html</filename>
    <docanchor file="mqtt_design.html" title="Interfaces and Callbacks">mqtt_interfaces</docanchor>
    <docanchor file="mqtt_design.html" title="Serializers and Deserializers">mqtt_serializers</docanchor>
    <docanchor file="mqtt_design.html" title="Sessions and State">mqtt_sessions</docanchor>
    <docanchor file="mqtt_design.html" title="Packet Reception">mqtt_receivepackets</docanchor>
    <docanchor file="mqtt_design.html" title="Runtime Timeouts passed to MQTT library">mqtt_receivetimeout</docanchor>
    <docanchor file="mqtt_design.html" title="Keep-Alive">mqtt_keepalive</docanchor>
  </compound>
  <compound kind="page">
    <name>core_mqtt_config</name>
    <title>Configurations</title>
    <filename>core_mqtt_config.html</filename>
    <docanchor file="core_mqtt_config.html">MQTT_DO_NOT_USE_CUSTOM_CONFIG</docanchor>
    <docanchor file="core_mqtt_config.html">MQTT_STATE_ARRAY_MAX_COUNT</docanchor>
    <docanchor file="core_mqtt_config.html">MQTT_PINGRESP_TIMEOUT_MS</docanchor>
    <docanchor file="core_mqtt_config.html">MQTT_RECV_POLLING_TIMEOUT_MS</docanchor>
    <docanchor file="core_mqtt_config.html">MQTT_SEND_RETRY_TIMEOUT_MS</docanchor>
    <docanchor file="core_mqtt_config.html">MQTT_MAX_CONNACK_RECEIVE_RETRY_COUNT</docanchor>
    <docanchor file="core_mqtt_config.html" title="LogError">mqtt_logerror</docanchor>
    <docanchor file="core_mqtt_config.html" title="LogWarn">mqtt_logwarn</docanchor>
    <docanchor file="core_mqtt_config.html" title="LogInfo">mqtt_loginfo</docanchor>
    <docanchor file="core_mqtt_config.html" title="LogDebug">mqtt_logdebug</docanchor>
  </compound>
  <compound kind="page">
    <name>mqtt_functions</name>
    <title>Functions</title>
    <filename>mqtt_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_init_function</name>
    <title>MQTT_Init</title>
    <filename>mqtt_init_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_connect_function</name>
    <title>MQTT_Connect</title>
    <filename>mqtt_connect_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_subscribe_function</name>
    <title>MQTT_Subscribe</title>
    <filename>mqtt_subscribe_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_publish_function</name>
    <title>MQTT_Publish</title>
    <filename>mqtt_publish_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_ping_function</name>
    <title>MQTT_Ping</title>
    <filename>mqtt_ping_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_unsubscribe_function</name>
    <title>MQTT_Unsubscribe</title>
    <filename>mqtt_unsubscribe_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_disconnect_function</name>
    <title>MQTT_Disconnect</title>
    <filename>mqtt_disconnect_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_processloop_function</name>
    <title>MQTT_ProcessLoop</title>
    <filename>mqtt_processloop_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_receiveloop_function</name>
    <title>MQTT_ReceiveLoop</title>
    <filename>mqtt_receiveloop_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getpacketid_function</name>
    <title>MQTT_GetPacketId</title>
    <filename>mqtt_getpacketid_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getsubackstatuscodes_function</name>
    <title>MQTT_GetSubAckStatusCodes</title>
    <filename>mqtt_getsubackstatuscodes_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_status_strerror_function</name>
    <title>MQTT_Status_strerror</title>
    <filename>mqtt_status_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_publishtoresend_function</name>
    <title>MQTT_PublishToResend</title>
    <filename>mqtt_publishtoresend_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getconnectpacketsize_function</name>
    <title>MQTT_GetConnectPacketSize</title>
    <filename>mqtt_getconnectpacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializeconnect_function</name>
    <title>MQTT_SerializeConnect</title>
    <filename>mqtt_serializeconnect_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getsubscribepacketsize_function</name>
    <title>MQTT_GetSubscribePacketSize</title>
    <filename>mqtt_getsubscribepacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializesubscribe_function</name>
    <title>MQTT_SerializeSubscribe</title>
    <filename>mqtt_serializesubscribe_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getunsubscribepacketsize_function</name>
    <title>MQTT_GetUnsubscribePacketSize</title>
    <filename>mqtt_getunsubscribepacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializeunsubscribe_function</name>
    <title>MQTT_SerializeUnsubscribe</title>
    <filename>mqtt_serializeunsubscribe_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getpublishpacketsize_function</name>
    <title>MQTT_GetPublishPacketSize</title>
    <filename>mqtt_getpublishpacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializepublish_function</name>
    <title>MQTT_SerializePublish</title>
    <filename>mqtt_serializepublish_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializepublishheader_function</name>
    <title>MQTT_SerializePublishHeader</title>
    <filename>mqtt_serializepublishheader_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializeack_function</name>
    <title>MQTT_SerializeAck</title>
    <filename>mqtt_serializeack_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getdisconnectpacketsize_function</name>
    <title>MQTT_GetDisconnectPacketSize</title>
    <filename>mqtt_getdisconnectpacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializedisconnect_function</name>
    <title>MQTT_SerializeDisconnect</title>
    <filename>mqtt_serializedisconnect_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getpingreqpacketsize_function</name>
    <title>MQTT_GetPingreqPacketSize</title>
    <filename>mqtt_getpingreqpacketsize_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_serializepingreq_function</name>
    <title>MQTT_SerializePingreq</title>
    <filename>mqtt_serializepingreq_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_deserializepublish_function</name>
    <title>MQTT_DeserializePublish</title>
    <filename>mqtt_deserializepublish_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_deserializeack_function</name>
    <title>MQTT_DeserializeAck</title>
    <filename>mqtt_deserializeack_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_getincomingpackettypeandlength_function</name>
    <title>MQTT_GetIncomingPacketTypeAndLength</title>
    <filename>mqtt_getincomingpackettypeandlength_function.html</filename>
  </compound>
  <compound kind="page">
    <name>mqtt_porting</name>
    <title>Porting Guide</title>
    <filename>mqtt_porting.html</filename>
    <docanchor file="mqtt_porting.html" title="Configuration Macros">mqtt_porting_config</docanchor>
    <docanchor file="mqtt_porting.html" title="Transport Interface">mqtt_porting_transport</docanchor>
    <docanchor file="mqtt_porting.html" title="Time Function">mqtt_porting_time</docanchor>
  </compound>
  <compound kind="page">
    <name>mqtt_timeouts</name>
    <title>Timeouts in coreMQTT library</title>
    <filename>mqtt_timeouts.html</filename>
    <docanchor file="mqtt_timeouts.html" title="Transport Send and Receive timeouts">mqtt_timeouts_transport_send_receive</docanchor>
    <docanchor file="mqtt_timeouts.html" title="MQTT Keep Alive interval">mqtt_timeouts_keep_alive</docanchor>
    <docanchor file="mqtt_timeouts.html" title="MQTT Ping Response timeout">mqtt_timeouts_ping_response</docanchor>
    <docanchor file="mqtt_timeouts.html" title="MQTT Receive Polling timeout">mqtt_timeouts_receive_polling</docanchor>
    <docanchor file="mqtt_timeouts.html" title="MQTT Send Retry timeout">mqtt_timeouts_send_retry</docanchor>
    <docanchor file="mqtt_timeouts.html" title="Timeouts for MQTT_ProcessLoop and MQTT_ReceiveLoop APIs">mqtt_timeouts_process_receive_loop</docanchor>
    <docanchor file="mqtt_timeouts.html" title="Timeout for MQTT_Connect">mqtt_timeouts_connect</docanchor>
  </compound>
  <compound kind="page">
    <name>mqtt_transport_interface</name>
    <title>Transport Interface</title>
    <filename>mqtt_transport_interface.html</filename>
    <docanchor file="mqtt_transport_interface.html" title="Transport Interface Overview">mqtt_transport_interface_overview</docanchor>
    <docanchor file="mqtt_transport_interface.html" title="Implementing the Transport Interface">mqtt_transport_interface_implementation</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">mqtt</docanchor>
    <docanchor file="index.html" title="Memory Requirements">mqtt_memory_requirements</docanchor>
  </compound>
</tagfile>
