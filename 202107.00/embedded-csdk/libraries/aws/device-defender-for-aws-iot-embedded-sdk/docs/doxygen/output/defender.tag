<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>defender.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-defender-for-aws-iot-embedded-sdk/source/</path>
    <filename>defender_8c.html</filename>
    <includes id="defender_8h" name="defender.h" local="yes" imported="no">defender.h</includes>
    <member kind="function" static="yes">
      <type>static uint16_t</type>
      <name>getTopicLength</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a9dce6bc2fb629daa4c97e13ef840b097</anchor>
      <arglist>(uint16_t thingNameLength, DefenderTopic_t api)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>writeFormatAndSuffix</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a60fe95b13a67b283dc799b3afa44edd2</anchor>
      <arglist>(char *pBuffer, DefenderTopic_t api)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DefenderStatus_t</type>
      <name>matchPrefix</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a8257a83f71389efe1b2dcbad82ebe691</anchor>
      <arglist>(const char *pRemainingTopic, uint16_t remainingTopicLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DefenderStatus_t</type>
      <name>extractThingNameLength</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a14aba93ece124f7abe14920bbf98df6f</anchor>
      <arglist>(const char *pRemainingTopic, uint16_t remainingTopicLength, uint16_t *pOutThingNameLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DefenderStatus_t</type>
      <name>matchBridge</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a260f31fcb65124373dd355d1a83c1606</anchor>
      <arglist>(const char *pRemainingTopic, uint16_t remainingTopicLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DefenderStatus_t</type>
      <name>matchApi</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a3191da717da1a673c2e765ea766e35bc</anchor>
      <arglist>(const char *pRemainingTopic, uint16_t remainingTopicLength, DefenderTopic_t *pOutApi)</arglist>
    </member>
    <member kind="function">
      <type>DefenderStatus_t</type>
      <name>Defender_GetTopic</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>a22eb9dc43200e71931a69ae60ffce08b</anchor>
      <arglist>(char *pBuffer, uint16_t bufferLength, const char *pThingName, uint16_t thingNameLength, DefenderTopic_t api, uint16_t *pOutLength)</arglist>
    </member>
    <member kind="function">
      <type>DefenderStatus_t</type>
      <name>Defender_MatchTopic</name>
      <anchorfile>defender_8c.html</anchorfile>
      <anchor>acf6f6908afeef3897ef57f445fa021aa</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, DefenderTopic_t *pOutApi, const char **ppOutThingName, uint16_t *pOutThingNameLength)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>defender.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-defender-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>defender_8h.html</filename>
    <includes id="defender__config__defaults_8h" name="defender_config_defaults.h" local="yes" imported="no">defender_config_defaults.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>STRING_LITERAL_LENGTH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>ac84fe737d133a156ec692e5b1a98b897</anchor>
      <arglist>(literal)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_THINGNAME_MAX_LENGTH</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga795de261864ac9564c333c0860dffbd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_MIN_PERIOD_SECONDS</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gace94b49f1b57860ed78987312ced610d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_JSON_PUBLISH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a25387664c608208744427e75310f5e24</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_JSON_ACCEPTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a07edd4a6293af1e4aa77e2551c28f828</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_JSON_REJECTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>ae612c28d502db53cbefe02c1c9e981e2</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_CBOR_PUBLISH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a7d21c226df7bcd3aa58140ebe8a60d7a</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_CBOR_ACCEPTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>ad1208c9414a9e9e6c6bca886217524e6</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_LENGTH_CBOR_REJECTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a107daa5f5f767f80238570dabda7e981</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_MAX_LENGTH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>ad6bd5352014a36989f7d485ea0bf007d</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_JSON_PUBLISH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>af87b3231b89b4c15f1916630dd2e4ca3</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_JSON_ACCEPTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a1d48f5b24f469f1153cb9b20d428c3d1</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_JSON_REJECTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a9592f534f04322d87da70b80791bc521</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_CBOR_PUBLISH</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a32203daad8332c03edcf8a2cc760e63b</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_CBOR_ACCEPTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a6f52446cd136f4f88fa67688820ad3a2</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_API_CBOR_REJECTED</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a5889911de48a087c00e4fd0089637922</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_HEADER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa42ea1a95ac0fb10b65fa88392ac0010</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_HEADER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga010b4c8d694ac1b990f9376d61da24ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga5556373ee6ead186526bf4b2eea4aa5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gae3052b282d85f714d0dd84431ad88d27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_ID_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6c4636676ef4541af24e99b9bdd0044d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_ID_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga8668999a7f8d89da32c445e3fcb95505</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_VERSION_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gabea127bceab156aff78167f77ecea3c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_VERSION_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga742c9b0eba86a32c3dee77f4fe79cb05</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TCP_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga9af9e172657279f709a5d2a10a12a511</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TCP_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gae8921479144d4924cd0b07b261601dff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_ESTABLISHED_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa907afcf5688799aea1b2c2c0ab67d71</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_ESTABLISHED_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaccaeb81a213d2d061514d962255b7e6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga86d4dba319631b359d21d086e08aa73a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gafe687e4d39ecb5251a286d96d94a719b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_REMOTE_ADDR_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga458102d85551d76a8954f5cdcef3dfe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_REMOTE_ADDR_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gacfdec34aa746810f48949a150e463aff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LOCAL_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga186e4f7320304e36d37f2be1b0a7e9b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_LOCAL_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaaa7a0368e8eeeb2668cf8caec8939fe9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LOCAL_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa2fc355a3d40bdd43b8ebe8a900a48c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_LOCAL_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga4675f838e360c0ed5e611a8f6c6198b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TOTAL_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gab58827829b675507e6091587dddd7098</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TOTAL_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga61797c15cfaab8aa4c1eda90b1f5d571</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TCP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga00f1ac09a6f2e6f135b19212755ccd63</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TCP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaeccbfc9253c5fff23a1e54f9e4f9715e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga44cacd91eddac3bff4cec64fcb282b69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga07e2a0a6a8de3fbb4a72e39399cf8456</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6944e61d618453eaa4fbf5f84dd9c77f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga315ae4fd3a4d753940f62dae7172113e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6290b754dda0e417e584d79970c80abc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaffe43ced95f3f203d85d3dc0afef8ebe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_UDP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga0019479e1808c562fa2706b25ac3bb25</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_UDP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga806709d65c6bdc673809645724a6003c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NETWORK_STATS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gad323155f3508ba8120efe4a562c413ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NETWORK_STATS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gad95d5f888abcf8f8e7a5ccf1013a7f52</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_BYTES_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga00d33f391c55527fe2fae505f15866f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_BYTES_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gacd1abfece5a9883250e6c5422359cdaf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_BYTES_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga5a93447c7820fdbce4b423d2c3d70d50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_BYTES_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga1b3a674d1f57f6e49d1b872e5a9caf72</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PKTS_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga55d897c5c217741078d3d4f592f380b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PKTS_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga881bb06751727953c7bbd8305d443a5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PKTS_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaf8d50f39bc7268d4f5176929dc191287</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PKS_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga862ed43e832ced6fb17eb890477f5521</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_CUSTOM_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa807bd6a618151d597e43015bbd26532</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_CUSTOM_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga57a455fd9d1953cd492fc94bc8aba476</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NUMBER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gadca733260eaf286111d1219d393ab6ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NUMBER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga99926efe3830340754b32573e196cfeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NUMBER_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaef9637e45907884914f822137d03aecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NUMBER_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga2c8b5529194d142f0dbaca680ed84d37</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_STRING_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga2566f90f865db560c1b9565fbfc583f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_STRING_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga0deb02295553ba409341e19e7e46093d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_IP_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gac7db1f31660c66d01cabf312e470723b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_IP_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga10b6687120aee26ead870ddda6087d83</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>DefenderStatus_t</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>ga60365ff6836fab9ad427f4edbe83d020</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderError</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020ae486edc8367abd34d5fb2157efd01a07</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderSuccess</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020af2b4bec80da4dc078b9fa393c5b450e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderNoMatch</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a637a7aa6491cbd53c0c41fe212d3bac9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderBadParameter</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a0b02aad65c5224b7e785a101bcd49e29</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderBufferTooSmall</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a5fe84be8f6490a5a855583896e60b59c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>DefenderTopic_t</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>ga75b56c3b6f174d7671aae7afb998221f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderInvalidTopic</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa3f4c04ad3e932a071ccfd445731611be</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportPublish</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa6477d3b592e8669fa82c5ea3f453cb7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportAccepted</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fab84742e9598458a7b05e17c4f0d0248c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportRejected</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fac1fc990713e97be6f506d7df0c3ff013</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportPublish</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa2461b9af3a3f7614ca3127428ef7a763</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportAccepted</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221faf0241ab4d26daad847142f78fc024653</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportRejected</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221faeef60aa0f955ddd3bdfbfcafa7ba69f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>DefenderStatus_t</type>
      <name>Defender_GetTopic</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>a22eb9dc43200e71931a69ae60ffce08b</anchor>
      <arglist>(char *pBuffer, uint16_t bufferLength, const char *pThingName, uint16_t thingNameLength, DefenderTopic_t api, uint16_t *pOutLength)</arglist>
    </member>
    <member kind="function">
      <type>DefenderStatus_t</type>
      <name>Defender_MatchTopic</name>
      <anchorfile>defender_8h.html</anchorfile>
      <anchor>acf6f6908afeef3897ef57f445fa021aa</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, DefenderTopic_t *pOutApi, const char **ppOutThingName, uint16_t *pOutThingNameLength)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>defender_config_defaults.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-defender-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>defender__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_DO_NOT_USE_CUSTOM_CONFIG</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>a542996a8d7671eeaf1e6912163b08c38</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_USE_LONG_KEYS</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>a080d61fff8c7941f112c582698b4d124</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogError</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>a8d9dbaaa88129137a4c68ba0456a18b1</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogWarn</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>a7da92048aaf0cbfcacde9539c98a0e05</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogInfo</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>a00810b1cb9d2f25d25ce2d4d93815fba</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogDebug</name>
      <anchorfile>defender__config__defaults_8h.html</anchorfile>
      <anchor>af60e8ffc327d136e5d0d8441ed98c98d</anchor>
      <arglist>(message)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>defender_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__defender__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>DefenderStatus_t</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>ga60365ff6836fab9ad427f4edbe83d020</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderError</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020ae486edc8367abd34d5fb2157efd01a07</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderSuccess</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020af2b4bec80da4dc078b9fa393c5b450e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderNoMatch</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a637a7aa6491cbd53c0c41fe212d3bac9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderBadParameter</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a0b02aad65c5224b7e785a101bcd49e29</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderBufferTooSmall</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga60365ff6836fab9ad427f4edbe83d020a5fe84be8f6490a5a855583896e60b59c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>DefenderTopic_t</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>ga75b56c3b6f174d7671aae7afb998221f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderInvalidTopic</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa3f4c04ad3e932a071ccfd445731611be</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportPublish</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa6477d3b592e8669fa82c5ea3f453cb7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportAccepted</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fab84742e9598458a7b05e17c4f0d0248c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderJsonReportRejected</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fac1fc990713e97be6f506d7df0c3ff013</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportPublish</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221fa2461b9af3a3f7614ca3127428ef7a763</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportAccepted</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221faf0241ab4d26daad847142f78fc024653</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DefenderCborReportRejected</name>
      <anchorfile>group__defender__enum__types.html</anchorfile>
      <anchor>gga75b56c3b6f174d7671aae7afb998221faeef60aa0f955ddd3bdfbfcafa7ba69f9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>defender_constants</name>
    <title>Constants</title>
    <filename>group__defender__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_THINGNAME_MAX_LENGTH</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga795de261864ac9564c333c0860dffbd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_MIN_PERIOD_SECONDS</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gace94b49f1b57860ed78987312ced610d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_HEADER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa42ea1a95ac0fb10b65fa88392ac0010</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_HEADER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga010b4c8d694ac1b990f9376d61da24ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga5556373ee6ead186526bf4b2eea4aa5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gae3052b282d85f714d0dd84431ad88d27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_ID_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6c4636676ef4541af24e99b9bdd0044d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_ID_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga8668999a7f8d89da32c445e3fcb95505</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_VERSION_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gabea127bceab156aff78167f77ecea3c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_VERSION_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga742c9b0eba86a32c3dee77f4fe79cb05</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TCP_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga9af9e172657279f709a5d2a10a12a511</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TCP_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gae8921479144d4924cd0b07b261601dff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_ESTABLISHED_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa907afcf5688799aea1b2c2c0ab67d71</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_ESTABLISHED_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaccaeb81a213d2d061514d962255b7e6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga86d4dba319631b359d21d086e08aa73a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_CONNECTIONS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gafe687e4d39ecb5251a286d96d94a719b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_REMOTE_ADDR_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga458102d85551d76a8954f5cdcef3dfe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_REMOTE_ADDR_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gacfdec34aa746810f48949a150e463aff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LOCAL_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga186e4f7320304e36d37f2be1b0a7e9b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_LOCAL_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaaa7a0368e8eeeb2668cf8caec8939fe9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LOCAL_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa2fc355a3d40bdd43b8ebe8a900a48c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_LOCAL_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga4675f838e360c0ed5e611a8f6c6198b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TOTAL_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gab58827829b675507e6091587dddd7098</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TOTAL_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga61797c15cfaab8aa4c1eda90b1f5d571</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_TCP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga00f1ac09a6f2e6f135b19212755ccd63</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_TCP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaeccbfc9253c5fff23a1e54f9e4f9715e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga44cacd91eddac3bff4cec64fcb282b69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga07e2a0a6a8de3fbb4a72e39399cf8456</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6944e61d618453eaa4fbf5f84dd9c77f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PORT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga315ae4fd3a4d753940f62dae7172113e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga6290b754dda0e417e584d79970c80abc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_INTERFACE_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaffe43ced95f3f203d85d3dc0afef8ebe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_UDP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga0019479e1808c562fa2706b25ac3bb25</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_UDP_LISTENING_PORTS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga806709d65c6bdc673809645724a6003c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NETWORK_STATS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gad323155f3508ba8120efe4a562c413ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NETWORK_STATS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gad95d5f888abcf8f8e7a5ccf1013a7f52</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_BYTES_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga00d33f391c55527fe2fae505f15866f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_BYTES_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gacd1abfece5a9883250e6c5422359cdaf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_BYTES_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga5a93447c7820fdbce4b423d2c3d70d50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_BYTES_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga1b3a674d1f57f6e49d1b872e5a9caf72</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PKTS_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga55d897c5c217741078d3d4f592f380b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PKTS_IN_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga881bb06751727953c7bbd8305d443a5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_PKTS_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaf8d50f39bc7268d4f5176929dc191287</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_PKS_OUT_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga862ed43e832ced6fb17eb890477f5521</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_CUSTOM_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaa807bd6a618151d597e43015bbd26532</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_CUSTOM_METRICS_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga57a455fd9d1953cd492fc94bc8aba476</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NUMBER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gadca733260eaf286111d1219d393ab6ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NUMBER_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga99926efe3830340754b32573e196cfeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_NUMBER_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gaef9637e45907884914f822137d03aecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_NUMBER_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga2c8b5529194d142f0dbaca680ed84d37</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_STRING_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga2566f90f865db560c1b9565fbfc583f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_STRING_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga0deb02295553ba409341e19e7e46093d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_IP_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>gac7db1f31660c66d01cabf312e470723b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFENDER_REPORT_LENGTH_IP_LIST_KEY</name>
      <anchorfile>group__defender__constants.html</anchorfile>
      <anchor>ga10b6687120aee26ead870ddda6087d83</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>defender_design</name>
    <title>Design</title>
    <filename>defender_design.html</filename>
  </compound>
  <compound kind="page">
    <name>defender_config</name>
    <title>Configurations</title>
    <filename>defender_config.html</filename>
    <docanchor file="defender_config.html">DEFENDER_DO_NOT_USE_CUSTOM_CONFIG</docanchor>
    <docanchor file="defender_config.html">DEFENDER_USE_LONG_KEYS</docanchor>
    <docanchor file="defender_config.html" title="LogError">defender_logerror</docanchor>
    <docanchor file="defender_config.html" title="LogWarn">defender_logwarn</docanchor>
    <docanchor file="defender_config.html" title="LogInfo">defender_loginfo</docanchor>
    <docanchor file="defender_config.html" title="LogDebug">defender_logdebug</docanchor>
  </compound>
  <compound kind="page">
    <name>defender_functions</name>
    <title>Functions</title>
    <filename>defender_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>defender_gettopic_function</name>
    <title>Defender_GetTopic</title>
    <filename>defender_gettopic_function.html</filename>
  </compound>
  <compound kind="page">
    <name>defender_matchtopic_function</name>
    <title>Defender_MatchTopic</title>
    <filename>defender_matchtopic_function.html</filename>
  </compound>
  <compound kind="page">
    <name>defender_porting</name>
    <title>Porting Guide</title>
    <filename>defender_porting.html</filename>
    <docanchor file="defender_porting.html" title="Configuration Macros">defender_porting_config</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">defender</docanchor>
    <docanchor file="index.html" title="Memory Requirements">defender_memory_requirements</docanchor>
  </compound>
</tagfile>
