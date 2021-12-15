<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.2" doxygen_gitid="60732c55c4bd92cb7ac11f173183b93ff63509f7">
  <compound kind="file">
    <name>FreeRTOS_ARP.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___a_r_p_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>arpMAX_ARP_AGE_BEFORE_NEW_ARP_REQUEST</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a22f53fd4d5218d841f6176b29603a955</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>arpGRATUITOUS_ARP_PERIOD</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a7b4eb297ab0d35cfe95b1879495a3d7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static eARPLookupResult_t</type>
      <name>prvCacheLookup</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a78b76b83a58bdb609db07f2949b39dcb</anchor>
      <arglist>(uint32_t ulAddressToLookup, MACAddress_t *const pxMACAddress)</arglist>
    </member>
    <member kind="function">
      <type>eFrameProcessingResult_t</type>
      <name>eARPProcessPacket</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>aa06d3a8a2d409547e12b4825ebc885b5</anchor>
      <arglist>(ARPPacket_t *const pxARPFrame)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vARPRefreshCacheEntry</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>aa9740bba27acb79ac417f9b187630e3b</anchor>
      <arglist>(const MACAddress_t *pxMACAddress, const uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>eARPLookupResult_t</type>
      <name>eARPGetCacheEntry</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a1ee3377f9c286506135c17be1962d8d6</anchor>
      <arglist>(uint32_t *pulIPAddress, MACAddress_t *const pxMACAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vARPAgeCache</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a3d3934dd27e4d024ab7a14ed0c80b3cc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vARPSendGratuitous</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>aa96d87a3e4505229094bf3d81330c2f4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_OutputARPRequest</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a87ebc2f32c8cda6cdc24a265df2388c0</anchor>
      <arglist>(uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vARPGenerateRequestPacket</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a27861e019fb0f3c1d5579f6806f75b4c</anchor>
      <arglist>(NetworkBufferDescriptor_t *const pxNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_ClearARP</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a7ad41b60042629fc57086d4990fc6347</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xCheckLoopback</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>a0927214ffe1d8b142d6e49fc26430339</anchor>
      <arglist>(NetworkBufferDescriptor_t *const pxDescriptor, BaseType_t bReleaseAfterSend)</arglist>
    </member>
    <member kind="variable">
      <type>_static ARPCacheRow_t</type>
      <name>xARPCache</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>afdc43c741d35db756e9a4ca5cd064bf1</anchor>
      <arglist>[ipconfigARP_CACHE_ENTRIES]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static TickType_t</type>
      <name>xLastGratuitousARPTime</name>
      <anchorfile>_free_r_t_o_s___a_r_p_8c.html</anchorfile>
      <anchor>aa9c3721b546b6d8021730dbc1d49c263</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_DHCP.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___d_h_c_p_8c.html</filename>
    <class kind="struct">struct</class>
    <member kind="define">
      <type>#define</type>
      <name>dhcpCLIENT_HARDWARE_ADDRESS_LENGTH</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a9dfdebcc673e74887f8512868fa361ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpSERVER_HOST_NAME_LENGTH</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a3460ffc5d7a95ff76742f7dc24a60fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpBOOT_FILE_NAME_LENGTH</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a525076903bc0e6179e9f18f11067018a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpINITIAL_TIMER_PERIOD</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a7e77d690aa573e19c016d4bab59a51b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpINITIAL_DHCP_TX_PERIOD</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>ae21108aea9e8f67970c05f03ae882832</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_ZERO_PAD_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a8c7929e27cfef12a81737179f4844eec</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_SUBNET_MASK_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a9b5e6f35b4d3c21e7339fd864b2eb083</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_GATEWAY_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a4c597e1ce55a624d96d679c7905ce68a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_DNS_SERVER_OPTIONS_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a152cd2137919e452b5329615b580f263</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_DNS_HOSTNAME_OPTIONS_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a0e26813b27eb5216e4b4a69190f0e2a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_REQUEST_IP_ADDRESS_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a199afb602e62b7b4a8264ca6b4ce3d39</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_LEASE_TIME_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aee62afab740f4b806fcb62947799bf22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_MESSAGE_TYPE_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a2dfbd401c9846f500668864b7fb2710c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_SERVER_IP_ADDRESS_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a1ae323b48e61a9cd0f8ea6d6f96bd9bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_PARAMETER_REQUEST_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>ab0415fe448ea9cf9bd3978ab4a5a2db4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpIPv4_CLIENT_IDENTIFIER_OPTION_CODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>ae5814df2d0f5ab149f9528a8cec3ff89</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMESSAGE_TYPE_DISCOVER</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a2e24721bd39e17d9b183e2125687d62d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMESSAGE_TYPE_OFFER</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a62c031313a09d86b89496ba6a9cae006</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMESSAGE_TYPE_REQUEST</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a869e3c5f0c92896e6cb35e3e655b46b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMESSAGE_TYPE_ACK</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a775354bab8931849febdbe263c58d1d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMESSAGE_TYPE_NACK</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a6b165aa67cc11f56afbf388e7ac97264</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpCLIENT_IDENTIFIER_OFFSET</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a4ceff5f0da2ce1cb99569c34fef4d681</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpREQUESTED_IP_ADDRESS_OFFSET</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a25caf50e4ba7269beaf6c6075fd46464</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpDHCP_SERVER_IP_ADDRESS_OFFSET</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aeb87759824da5d6e8f7f89ed9319258c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpREQUEST_OPCODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a73c6f002510653b02300a1c673f72c9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpREPLY_OPCODE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a3024e6e8ac856f362aa981ee5a8b9135</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpADDRESS_TYPE_ETHERNET</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aa0d773d9ec08477bcad8103f5a38c3a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpETHERNET_ADDRESS_LENGTH</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a2767c2133228aee9dfb336269332dabc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>EP_DHCPData</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>adfb2f90932de3ba1166a8ae0e22cfa16</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>EP_IPv4_SETTINGS</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a7855dad00c2e1354eaafb18a0ec8d999</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpDEFAULT_LEASE_TIME</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a10e722aeaae4c7083c3e373361e4289c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpMINIMUM_LEASE_TIME</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>ab94b8bccbab25d280da8ccdaf4455d57</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpOPTION_END_BYTE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>af99f3d8cdc1153ec6629288600a967f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpFIRST_OPTION_BYTE_OFFSET</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a91ced274d25f700a12f7c520388e2fdf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpCLIENT_PORT_IPv4</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a6314ef4310bb7e7b007fc8f8bd7caa64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpSERVER_PORT_IPv4</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a8312231f5161d99c7b8b752fde8cb030</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpCOOKIE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a9c0d01ea72440e996e7cf6599efa518e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dhcpBROADCAST</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a399a2cda3f448f8b5c56fa4708ee4793</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a828bde991d7f730cc44a775b504d85f8</anchor>
      <arglist>(DHCPMessage_IPv4_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_CONST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a83261d43472efe09f8f0d99c8a4c14d4</anchor>
      <arglist>(DHCPMessage_IPv4_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvSendDHCPDiscover</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a505ce80137ded6e3390b00cb0341d3a5</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>_static BaseType_t</type>
      <name>prvProcessDHCPReplies</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a03acd9164ad584eb639fd70356794366</anchor>
      <arglist>(BaseType_t xExpectedMessageType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvSendDHCPRequest</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a5b4f4e1210c8272bd82c36477a1778f6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvInitialiseDHCP</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a0b627c523231206cd31f2bb46372e44e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t *</type>
      <name>prvCreatePartDHCPMessage</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a8d2f4ae9d52b68d32dae4991f3cb3229</anchor>
      <arglist>(struct freertos_sockaddr *pxAddress, BaseType_t xOpcode, const uint8_t *const pucOptionsArray, size_t *pxOptionsArraySize)</arglist>
    </member>
    <member kind="function">
      <type>_static void</type>
      <name>prvCreateDHCPSocket</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aaff6679eef687e2c208ac49d368eabff</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvCloseDHCPSocket</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a9d09a6b3e592978816bf2d7fdd98b699</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xIsDHCPSocket</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aed005283591e5af559f10afb9090eae0</anchor>
      <arglist>(Socket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>eDHCPState_t</type>
      <name>eGetDHCPState</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a9d1cad5d747fbc585080929f3169cf9f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vDHCPProcess</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a6a554f1a835cbef4ee620f3839841777</anchor>
      <arglist>(BaseType_t xReset, eDHCPState_t eExpectedState)</arglist>
    </member>
    <member kind="variable">
      <type>_static Socket_t</type>
      <name>xDHCPSocket</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>aea41a17995bc1183eb373bf376e90ae5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>_static DHCPData_t</type>
      <name>xDHCPData</name>
      <anchorfile>_free_r_t_o_s___d_h_c_p_8c.html</anchorfile>
      <anchor>a501856edb97370508c912a8b49dbf3cb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_DNS.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___d_n_s_8c.html</filename>
    <class kind="struct">struct</class>
    <class kind="struct">DNSCallback_t</class>
    <member kind="define">
      <type>#define</type>
      <name>dnsDNS_PORT</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8cca321343618c8fa0a260fd8a819473</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsONE_QUESTION</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8b428fe7de1758f2b1b8e657a1ef47b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsOUTGOING_FLAGS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a3d0da0f6f7b0dedccdad53936eb78ae3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsRX_FLAGS_MASK</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a2f2d9e5e60a455c209b38793a2a67914</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsEXPECTED_RX_FLAGS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8ef84cca921f793513d8559c542bd831</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipconfigDNS_REQUEST_ATTEMPTS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a7bf53c7d169359031def8b0e480a2a07</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNAME_IS_OFFSET</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ab4a05dba8a79bc6a0726ea852247186a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_FLAGS_RESPONSE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a81dadf70a217c0616c8d44ec042cbd48</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_FLAGS_OPCODE_MASK</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a09346ef677131535076a2bad887c440f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_FLAGS_OPCODE_QUERY</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8a10899c4a10c6c8b4c481c7dadfa3b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsTYPE_A_HOST</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a9d6929d8badbc074a31f8e7d82a6db08</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsCLASS_IN</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ac9b1a453ec5f5fa4ac573e4e26f07efc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsLLMNR_TTL_VALUE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a6d6507fae08709343eecb47140c20339</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsLLMNR_FLAGS_IS_REPONSE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ac8a96b45af40fbae735b159cf60024e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_TTL_VALUE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8be33ea2d0671ca94acc1f6f3f743631</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_TYPE_NET_BIOS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a7a7208ad3a0d52636d21af3692832ec9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_CLASS_IN</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a2312cbbbf89cc258051715b0baee1204</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_NAME_FLAGS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a898ba9455e4b297a5f82ef9a7498ca78</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_ENCODED_NAME_LENGTH</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a9332d7b813faaec069dfcb7f16db3454</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsNBNS_QUERY_RESPONSE_FLAGS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a7ede7682a08c4a122d4d4191a54724e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>dnsPARSE_ERROR</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a3c0262be7e4c69b9a7e6763cc3bacc04</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASCII_BASELINE_DOT</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ac30e7dd81c154bb807ffcfb12a5c3db7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Socket_t</type>
      <name>prvCreateDNSSocket</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a063465bac49177bfa4b33b0dfa51d0b8</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>_static size_t</type>
      <name>prvCreateDNSMessage</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a46f16170c8337a04643aa443d1b2c28c</anchor>
      <arglist>(uint8_t *pucUDPPayloadBuffer, const char *pcHostName, TickType_t uxIdentifier)</arglist>
    </member>
    <member kind="function">
      <type>_static size_t</type>
      <name>prvSkipNameField</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a0a5a65eaad2e66f4776e42e5657d9828</anchor>
      <arglist>(const uint8_t *pucByte, size_t uxLength)</arglist>
    </member>
    <member kind="function">
      <type>_static uint32_t</type>
      <name>prvParseDNSReply</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a8fc27b08cd9f232f256f0b31a8451bd8</anchor>
      <arglist>(uint8_t *pucUDPPayloadBuffer, size_t uxBufferLength, BaseType_t xExpected)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvPrepareLookup</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a614dd060ab2026ff3b1ce9f107d4ea1d</anchor>
      <arglist>(const char *pcHostName, FOnDNSEvent pCallback, void *pvSearchID, TickType_t uxTimeout)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvGetHostByName</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a51955624ad265fc7e0a54f2fcba5a526</anchor>
      <arglist>(const char *pcHostName, TickType_t uxIdentifier, TickType_t uxReadTimeOut_ticks)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>vDNSSetCallBack</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>aec205a2c074cd4053f12742127206470</anchor>
      <arglist>(const char *pcHostName, void *pvSearchID, FOnDNSEvent pCallbackFunction, TickType_t uxTimeout, TickType_t uxIdentifier)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>xDNSDoCallback</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ab1c8c7ff4cc58998ddebd08e3493d3b1</anchor>
      <arglist>(TickType_t uxIdentifier, const char *pcName, uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvReplyDNSMessage</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a5346e54c0e7d9500ff620d4093a92079</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer, BaseType_t lNetLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE void</type>
      <name>prvTreatNBNS</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a2c0bc0834d9400fa4f163e87cf88a4e5</anchor>
      <arglist>(uint8_t *pucPayload, size_t uxBufferLength, uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>_static size_t</type>
      <name>prvReadNameField</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a9be17b9ef1b9acb444489556f65e15c4</anchor>
      <arglist>(const uint8_t *pucByte, size_t uxRemainingBytes, char *pcName, size_t uxDestLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a50f1fe3a9127acf4661c35b9dad5ba7d</anchor>
      <arglist>(DNSMessage_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_CONST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a06c387b3a061ae83b2ba510fb414daa6</anchor>
      <arglist>(DNSMessage_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>adc9fb4649bb1dc3f0444924c30018aad</anchor>
      <arglist>(DNSTail_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a11317ec8a3156abc9d6d9b7b7b606a91</anchor>
      <arglist>(DNSAnswerRecord_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>acf9b85bceb8f9d6a00bf66f6407a8fe7</anchor>
      <arglist>(LLMNRAnswer_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>aaeed97a20d7783f062334ea3aa787ac9</anchor>
      <arglist>(NBNSAnswer_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a437c735abe2d2bb1925fb5905706e3be</anchor>
      <arglist>(DNSCallback_t)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_gethostbyname</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a71b77cb5028f171304f1535f7406766f</anchor>
      <arglist>(const char *pcHostName)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vDNSInitialise</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ad8cd6ab9c87ad7e087c0ec09b118c9b7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vDNSCheckCallBack</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a492ba3e0cfc6bb68ca22ec7015909cdc</anchor>
      <arglist>(void *pvSearchID)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_gethostbyname_cancel</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ad6746bbebd1cca52f8a6d4cafcc5bbcf</anchor>
      <arglist>(void *pvSearchID)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_gethostbyname_a</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a585a2b637e9683132af556a2b3147ad0</anchor>
      <arglist>(const char *pcHostName, FOnDNSEvent pCallback, void *pvSearchID, TickType_t uxTimeout)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>ulDNSHandlePacket</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>ae560e9af4e296059183eb6ddbe9458a1</anchor>
      <arglist>(const NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>ulNBNSHandlePacket</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a9da21a35903e781448c9d2bc89075beb</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="variable">
      <type>const MACAddress_t</type>
      <name>xLLMNR_MacAdress</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a446a9cc680b225064e8acf91d92fa7d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static List_t</type>
      <name>xCallbackList</name>
      <anchorfile>_free_r_t_o_s___d_n_s_8c.html</anchorfile>
      <anchor>a407c6353786ffeed66a0b5ffef3f46e3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_IP.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___i_p_8c.html</filename>
    <class kind="union">xUnion32</class>
    <class kind="union">xUnionPtr</class>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_EthernetHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a8ef9c6533cd9f34111f6ff1e5843507d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_ARPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac13c9f74e9fe374fa459f8df1864dc72</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_IPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a43457a3ba4fd6f16a4d33394af459536</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_IGMPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a527470135bea5ef3d75f3c1206431584</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_ICMPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a0dc89907d7a25a7f795f2abcd9eb6aa6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_UDPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a2cc551f421556711f0924201c04c9f82</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipEXPECTED_TCPHeader_t_SIZE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ada34cd2dcc842a6c43cd9c85acd0ebc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipICMP_ECHO_REQUEST</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a576713b2b5c693d521be50e120d1d106</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipICMP_ECHO_REPLY</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a20f4a6a59729b84b4f858c9e2532fa9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipFIRST_MULTI_CAST_IPv4</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a8d61855955d5d08f6933f84bc668d80f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipLAST_MULTI_CAST_IPv4</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac31c27794ca5338cda64cc9282dadfed</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipIPV4_VERSION_HEADER_LENGTH_MIN</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a599ef40bb9c6a811c74e16ae1a00fdf2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipIPV4_VERSION_HEADER_LENGTH_MAX</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af3e37d63f99e31d8af56e446b8ddd837</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipINITIALISATION_RETRY_DELAY</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af982816b81c2c41e4e50643993a23085</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipARP_TIMER_PERIOD_MS</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a7e4e7fc3c3cb2528f45fbff05a95c278</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>iptraceIP_TASK_STARTING</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac51b5f3c553cd23a5fd940017637737f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipTCP_TIMER_PERIOD_MS</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aef1630538f6d0701a36f878b67bbff8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipCONSIDER_FRAME_FOR_PROCESSING</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a615aa656020ebf93f31e4ccbd47a880d</anchor>
      <arglist>(pucEthernetBuffer)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipFRAGMENT_OFFSET_BIT_MASK</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac47f6670e30fff23d3f497bf36987eca</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipconfigMAX_IP_TASK_SLEEP_TIME</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aba7b582c7eacfa1a6448433f019cad23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipUNHANDLED_PROTOCOL</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aa61c957eee4cfdb4853c29cfef224e33</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipCORRECT_CRC</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>afde58aa20087aac4f57fbe82401f39b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipWRONG_CRC</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ad2652c0fbddea6f0fe507d8d28e8816f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipINVALID_LENGTH</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a21e1d2184955efc68fe033a6ffdc44f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEBUG_DECLARE_TRACE_VARIABLE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a167d6d53f49e298310e581dbdf4128de</anchor>
      <arglist>(type, var, init)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEBUG_SET_TRACE_VARIABLE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a9e73738efbee764aa5e89b9d6445410e</anchor>
      <arglist>(var, value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a52fc9e0117c61a8d8e0895e01552250e</anchor>
      <arglist>(NetworkBufferDescriptor_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvIPTask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a4a02bc45b29f74e5f48bd71bca5c1b61</anchor>
      <arglist>(void *pvParameters)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvProcessEthernetPacket</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>abbd5bae218c03d5d539d8f2fa8148a87</anchor>
      <arglist>(NetworkBufferDescriptor_t *const pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static eFrameProcessingResult_t</type>
      <name>prvProcessIPPacket</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a0ff2a58fc92561f47bdd9c45f20cf4f9</anchor>
      <arglist>(IPPacket_t *pxIPPacket, NetworkBufferDescriptor_t *const pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvProcessNetworkDownEvent</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac20b2833c8912039201c1baf2f25a8f6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvCheckNetworkTimers</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af5ce8d5c2384dfac1c01fd0a63d87581</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TickType_t</type>
      <name>prvCalculateSleepTime</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aeb2c5b32a896922e43d1183953922815</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvHandleEthernetPacket</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a9d5b9dd29ac8f3830bc4b1531b10228a</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvIPTimerStart</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a6bbcca14e07b67e72962278309dd8d07</anchor>
      <arglist>(IPTimer_t *pxTimer, TickType_t xTime)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvIPTimerCheck</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ad2a2e3f4cbb0aee222276cfbae2b1590</anchor>
      <arglist>(IPTimer_t *pxTimer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvIPTimerReload</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ae25d502fab7e4af29bc9b30dffc75ccf</anchor>
      <arglist>(IPTimer_t *pxTimer, TickType_t xTime)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static eFrameProcessingResult_t</type>
      <name>prvAllowIPPacket</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a0a84cb5865863076c9fd79c75c442bcb</anchor>
      <arglist>(const IPPacket_t *const pxIPPacket, const NetworkBufferDescriptor_t *const pxNetworkBuffer, UBaseType_t uxHeaderLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static NetworkBufferDescriptor_t *</type>
      <name>prvPacketBuffer_to_NetworkBuffer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>afb009e3856db87301868748c1732f74d</anchor>
      <arglist>(const void *pvBuffer, size_t uxOffset)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xIsCallingFromIPTask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a9844166e6474d5f406df1c57aa4da98f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_NetworkDown</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aa2c7afd343400f1ec8b01cde2f1a8f2e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_NetworkDownFromISR</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>adb922d3358a892833f3a4383f7c9c552</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>FreeRTOS_GetUDPPayloadBuffer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aca554a4fa9d8738e5423e3a2912bf340</anchor>
      <arglist>(size_t uxRequestedSizeBytes, TickType_t uxBlockTimeTicks)</arglist>
    </member>
    <member kind="function">
      <type>NetworkBufferDescriptor_t *</type>
      <name>pxDuplicateNetworkBufferWithDescriptor</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ad104b025d007fc96fbc8d7192c019c2b</anchor>
      <arglist>(const NetworkBufferDescriptor_t *const pxNetworkBuffer, size_t uxNewLength)</arglist>
    </member>
    <member kind="function">
      <type>NetworkBufferDescriptor_t *</type>
      <name>pxUDPPayloadBuffer_to_NetworkBuffer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a75b905aeed098b75f34aa8bf80ea22d5</anchor>
      <arglist>(const void *pvBuffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_ReleaseUDPPayloadBuffer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ab9acf473a4cda3f58dde4180091464ed</anchor>
      <arglist>(void const *pvBuffer)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_IPInit</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ae5a03616eca530b4ac3ab7d05ff4cc51</anchor>
      <arglist>(const uint8_t ucIPAddress[ipIP_ADDRESS_LENGTH_BYTES], const uint8_t ucNetMask[ipIP_ADDRESS_LENGTH_BYTES], const uint8_t ucGatewayAddress[ipIP_ADDRESS_LENGTH_BYTES], const uint8_t ucDNSServerAddress[ipIP_ADDRESS_LENGTH_BYTES], const uint8_t ucMACAddress[ipMAC_ADDRESS_LENGTH_BYTES])</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_GetAddressConfiguration</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ad802acf10146e09bb75437c6fef04632</anchor>
      <arglist>(uint32_t *pulIPAddress, uint32_t *pulNetMask, uint32_t *pulGatewayAddress, uint32_t *pulDNSServerAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_SetAddressConfiguration</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a17cb4b8b79208cca6a95244620ec45c5</anchor>
      <arglist>(const uint32_t *pulIPAddress, const uint32_t *pulNetMask, const uint32_t *pulGatewayAddress, const uint32_t *pulDNSServerAddress)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xSendEventToIPTask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a64324b01a1fc85efdb7129316e8f7737</anchor>
      <arglist>(eIPEvent_t eEvent)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xSendEventStructToIPTask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a7190bc18fa344fe1fd79021a6db05f84</anchor>
      <arglist>(const IPStackEvent_t *pxEvent, TickType_t uxTimeout)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xSendDHCPEvent</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a9edea1d81a89bc1838f51f6699982740</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>eFrameProcessingResult_t</type>
      <name>eConsiderFrameForProcessing</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ab15bee3df12f185ea7a1ba671121d19f</anchor>
      <arglist>(const uint8_t *const pucEthernetBuffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vIPNetworkUpCalls</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>abd469315acbf173d5cc16168721a17fb</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xIsIPv4Multicast</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a3c787bfa6502ecfd173ee8f6974e8628</anchor>
      <arglist>(uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vSetMultiCastIPv4MacAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af110bcf8c7910fc9149a4a09895efcf4</anchor>
      <arglist>(uint32_t ulIPAddress, MACAddress_t *pxMACAddress)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>usGenerateProtocolChecksum</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aad4d89d0e834cef68e0746e0404f319d</anchor>
      <arglist>(const uint8_t *const pucEthernetBuffer, size_t uxBufferLength, BaseType_t xOutgoingPacket)</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>usGenerateChecksum</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a97162cfd5b071ccee8fec503879a3e43</anchor>
      <arglist>(uint16_t usSum, const uint8_t *pucNextData, size_t uxByteCount)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vReturnEthernetFrame</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a5fb218e96c829c263a677428fe4d5d24</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer, BaseType_t xReleaseAfterSend)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_GetIPAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>aeaf07062b415dab09f3e333134b97103</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_SetIPAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a99f06ef42d1ac00f4ede5a68962a1502</anchor>
      <arglist>(uint32_t ulIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_GetGatewayAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a6a9033775f90dafde04eaa8236ca36c7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_GetDNSServerAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a0445e00d7886a94e4b39ec47a7e96964</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_GetNetmask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac94f1e03ad9facdc02d68063f7ba3101</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_UpdateMACAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af4d5550342028837be656fbf35cae815</anchor>
      <arglist>(const uint8_t ucMACAddress[ipMAC_ADDRESS_LENGTH_BYTES])</arglist>
    </member>
    <member kind="function">
      <type>const uint8_t *</type>
      <name>FreeRTOS_GetMACAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a5245023fe8eda88775545c90d39c7763</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_SetNetmask</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a69b7f10a7f753fc50ea2bf7712dbcd5b</anchor>
      <arglist>(uint32_t ulNetmask)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_SetGatewayAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a3329850735ce894baeef8fbac70be579</anchor>
      <arglist>(uint32_t ulGatewayAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vIPSetDHCPTimerEnableState</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ab4f26bbec48d40bb077621b734adddc8</anchor>
      <arglist>(BaseType_t xEnableState)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vIPReloadDHCPTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a880467f5ff22e0154bf8124fa437ea8e</anchor>
      <arglist>(uint32_t ulLeaseTime)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vIPSetDnsTimerEnableState</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a575576b2fe366f64e201c7cef093f454</anchor>
      <arglist>(BaseType_t xEnableState)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vIPReloadDNSTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ab6d1e820182a6f16577d5f36cd2e0819</anchor>
      <arglist>(uint32_t ulCheckTime)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xIPIsNetworkTaskReady</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a52b2222c2b48904fb90674927a2c489d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_IsNetworkUp</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac1f02ee6cc4e8f633ed3ddbc3d01e943</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>FreeRTOS_strerror_r</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a738b12572023fadb0e5ed3c3204a2ccb</anchor>
      <arglist>(BaseType_t xErrnum, char *pcBuffer, size_t uxLength)</arglist>
    </member>
    <member kind="variable">
      <type>QueueHandle_t</type>
      <name>xNetworkEventQueue</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a7447ea310c26bfce949913de72f3d6f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usPacketIdentifier</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a292a9f76f680f80f60d785e0c7bb20f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const MACAddress_t</type>
      <name>xBroadcastMACAddress</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a4eda95e8d4e0340cd07f7fd0e9637ec4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>NetworkAddressingParameters_t</type>
      <name>xNetworkAddressing</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af505b2f99bf8fc1c3b4d61cd5ecd21e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>NetworkAddressingParameters_t</type>
      <name>xDefaultAddressing</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ab27de5a3f6e8a7b47b696604cb837c18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static volatile BaseType_t</type>
      <name>xNetworkDownEventPending</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a0113c3af0709b245d34e7dec5df16119</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static TaskHandle_t</type>
      <name>xIPTaskHandle</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>ac45764b03f1eb0385c0cd08bc62394d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static BaseType_t</type>
      <name>xProcessedTCPMessage</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af2ee87b297b5e8d2b71edb828b44c834</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static BaseType_t</type>
      <name>xNetworkUp</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a6831050baaf1f467735e027fb77ba98e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IPTimer_t</type>
      <name>xARPTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a847894eed20607f2865876d8d034d445</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IPTimer_t</type>
      <name>xDHCPTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>af1400bc732b7f47d9e33033fcb35f907</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IPTimer_t</type>
      <name>xTCPTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a173c03d7a816518ad4d362a7c61088f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IPTimer_t</type>
      <name>xDNSTimer</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a457d35af454ec380c8e37f2c00ea8c49</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static BaseType_t</type>
      <name>xIPTaskInitialised</name>
      <anchorfile>_free_r_t_o_s___i_p_8c.html</anchorfile>
      <anchor>a9fdfb0a695267908cfd8d1b792efaf13</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_Sockets.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___sockets_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>socketSET_SOCKET_PORT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a8fc4e3506e42e9eef4b51ffd91e665b3</anchor>
      <arglist>(pxSocket, usPort)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketGET_SOCKET_PORT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ab3a0b0023a907a0cff7dc53d226f80cd</anchor>
      <arglist>(pxSocket)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketSOCKET_IS_BOUND</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a2e01dee7309f496afde943fed947729c</anchor>
      <arglist>(pxSocket)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketAUTO_PORT_ALLOCATION_START_NUMBER</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a350b6e0f49c270cb5c13c6aeeae116af</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketAUTO_PORT_ALLOCATION_MAX_NUMBER</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a3319ecf6f85a2b64a8d48396e0279e24</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketMAX_IP_ADDRESS_OCTETS</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>afc265c349a43891cb7ab972fd58630e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>socketDONT_BLOCK</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a42f8714f024edfbb0a683d31d2ee7e60</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipTCP_TIMER_PERIOD_MS</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>aef1630538f6d0701a36f878b67bbff8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>sock20_PERCENT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a68795ded8df36253a9ae4a48f728f863</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>sock80_PERCENT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a63599b39bbf9eea1e37fcbb06035027d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>sock100_PERCENT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a67b2a67aa4db57cbda7a7150eb4f58f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>sockDIGIT_COUNT</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad93067b1b641f4859cd5501786573802</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a52fc9e0117c61a8d8e0895e01552250e</anchor>
      <arglist>(NetworkBufferDescriptor_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>aa30c98bf08053c764516fab0d9e76673</anchor>
      <arglist>(StreamBuffer_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint16_t</type>
      <name>prvGetPrivatePortNumber</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a83a99d42729cdba57e78dd6c3c9e66e1</anchor>
      <arglist>(BaseType_t xProtocol)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static const ListItem_t *</type>
      <name>pxListFindListItemWithValue</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0788754fab0379fd522b0ff849692fc4</anchor>
      <arglist>(const List_t *pxList, TickType_t xWantedItemValue)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvValidSocket</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a4328ec58511a541031cc6a60da027ab4</anchor>
      <arglist>(const FreeRTOS_Socket_t *pxSocket, BaseType_t xProtocol, BaseType_t xIsBound)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvSockopt_so_buffer</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a43126375c017bf52614bff3f1702d1b1</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, int32_t lOptionName, const void *pvOptionValue)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvDetermineSocketSize</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a4ed71e4108c462911d2fa3229eec6df6</anchor>
      <arglist>(BaseType_t xDomain, BaseType_t xType, BaseType_t xProtocol, size_t *pxSocketSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static StreamBuffer_t *</type>
      <name>prvTCPCreateStream</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a6fbe49f69aeaa10ff6d5d7d2cc82c71d</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, BaseType_t xIsInputStream)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>prvTCPSendCheck</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad719999c8bcbddf392affdeb6be2f554</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, size_t uxDataLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvTCPSetSocketCount</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a91064db1bf7a4b1f49922c8f9927c701</anchor>
      <arglist>(FreeRTOS_Socket_t const *pxSocketToDelete)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPConnectStart</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a5d910cb9c6b4f63d7f8a75cd1ed7fa62</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr const *pxAddress)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>bMayConnect</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ae8a5f1c37d0260304e2acb616ca18247</anchor>
      <arglist>(FreeRTOS_Socket_t const *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvFindSelectedSocket</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a69915c323634b32c5534bd1a17b79673</anchor>
      <arglist>(SocketSelect_t *pxSocketSet)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vNetworkSocketsInit</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a4143378ec838cd340efd7f95b30444a5</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Socket_t</type>
      <name>FreeRTOS_socket</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0707f288bef6f9a6cafd3dc3c4cfab99</anchor>
      <arglist>(BaseType_t xDomain, BaseType_t xType, BaseType_t xProtocol)</arglist>
    </member>
    <member kind="function">
      <type>SocketSet_t</type>
      <name>FreeRTOS_CreateSocketSet</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a6ffb7662be38da18db6ec8e87da4735e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_DeleteSocketSet</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a427e342cb8d9c513d117ba772f427cca</anchor>
      <arglist>(SocketSet_t xSocketSet)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_FD_SET</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>accf33877d948d53b4b189ddc41718ccd</anchor>
      <arglist>(Socket_t xSocket, SocketSet_t xSocketSet, EventBits_t xBitsToSet)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_FD_CLR</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a7ecd213d4d45a6dbc0e900f52f2b4541</anchor>
      <arglist>(Socket_t xSocket, SocketSet_t xSocketSet, EventBits_t xBitsToClear)</arglist>
    </member>
    <member kind="function">
      <type>EventBits_t</type>
      <name>FreeRTOS_FD_ISSET</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a038d57f22343be8a68a4d3431b94a306</anchor>
      <arglist>(Socket_t xSocket, SocketSet_t xSocketSet)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_select</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a659a570c1f1b5a11c17d816cb5541291</anchor>
      <arglist>(SocketSet_t xSocketSet, TickType_t xBlockTimeTicks)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>FreeRTOS_recvfrom</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a72b60b05c7a0fded34d9823536bb013a</anchor>
      <arglist>(Socket_t xSocket, void *pvBuffer, size_t uxBufferLength, BaseType_t xFlags, struct freertos_sockaddr *pxSourceAddress, socklen_t *pxSourceAddressLength)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>FreeRTOS_sendto</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a1493cacf34d993b8b87c41242d7fc6d1</anchor>
      <arglist>(Socket_t xSocket, const void *pvBuffer, size_t uxTotalDataLength, BaseType_t xFlags, const struct freertos_sockaddr *pxDestinationAddress, socklen_t xDestinationAddressLength)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_bind</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a6fe933805d4f44b09b3b04a13ed28e72</anchor>
      <arglist>(Socket_t xSocket, struct freertos_sockaddr const *pxAddress, socklen_t xAddressLength)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>vSocketBind</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>add35977107add8357366416d55c64e69</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, struct freertos_sockaddr *pxBindAddress, size_t uxAddressLength, BaseType_t xInternal)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_closesocket</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>abefb0186bf6e06816a6acd12432dae9b</anchor>
      <arglist>(Socket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vSocketClose</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a6f2765a0d6dddbae7dee11b966813905</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_setsockopt</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a7faeae52e5489bc7b5ee8ab9392630d5</anchor>
      <arglist>(Socket_t xSocket, int32_t lLevel, int32_t lOptionName, const void *pvOptionValue, size_t uxOptionLength)</arglist>
    </member>
    <member kind="function">
      <type>FreeRTOS_Socket_t *</type>
      <name>pxUDPSocketLookup</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad924ce49378843372b331ddd89a6d389</anchor>
      <arglist>(UBaseType_t uxLocalPort)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>FreeRTOS_inet_ntoa</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad284624af0262fa60b7ebe59711dfd47</anchor>
      <arglist>(uint32_t ulIPAddress, char *pcBuffer)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_inet_pton</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a47ae572a3caf62d61b5aaeb462914516</anchor>
      <arglist>(BaseType_t xAddressFamily, const char *pcSource, void *pvDestination)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>FreeRTOS_inet_ntop</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a252d2f28fb55a14a47aab86579e5c8cc</anchor>
      <arglist>(BaseType_t xAddressFamily, const void *pvSource, char *pcDestination, socklen_t uxSize)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>FreeRTOS_inet_ntop4</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>aab5898b7c8a6d8f924d0cf144dec218f</anchor>
      <arglist>(const void *pvSource, char *pcDestination, socklen_t uxSize)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_inet_pton4</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ae3306e04e9b856682503e39f856ec7ff</anchor>
      <arglist>(const char *pcSource, void *pvDestination)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>FreeRTOS_inet_addr</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a17f785c94c2b8d0c80cf615a898c70b8</anchor>
      <arglist>(const char *pcIPAddress)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>FreeRTOS_GetLocalAddress</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a5cfd261a29fa206a8664ab5171ad9570</anchor>
      <arglist>(ConstSocket_t xSocket, struct freertos_sockaddr *pxAddress)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vSocketWakeUpUser</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0387edebc3e7bcf7fcf038a01a098213</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_connect</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>aa17e87db63f5dd047cc4fe5c2368555e</anchor>
      <arglist>(Socket_t xClientSocket, struct freertos_sockaddr *pxAddress, socklen_t xAddressLength)</arglist>
    </member>
    <member kind="function">
      <type>Socket_t</type>
      <name>FreeRTOS_accept</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a1b0aabc4d95b9a61e531429839f50852</anchor>
      <arglist>(Socket_t xServerSocket, struct freertos_sockaddr *pxAddress, socklen_t *pxAddressLength)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_recv</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a44ab7ba5fddce925519c7ccc7b887033</anchor>
      <arglist>(Socket_t xSocket, void *pvBuffer, size_t uxBufferLength, BaseType_t xFlags)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t *</type>
      <name>FreeRTOS_get_tx_head</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>acf2b08087032671c04f786b3992fdc58</anchor>
      <arglist>(ConstSocket_t xSocket, BaseType_t *pxLength)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_send</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>abfb04a35d27c55380be674ebd8204351</anchor>
      <arglist>(Socket_t xSocket, const void *pvBuffer, size_t uxDataLength, BaseType_t xFlags)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_listen</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a1718a2539e0de40d95472ea59859be5c</anchor>
      <arglist>(Socket_t xSocket, BaseType_t xBacklog)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_shutdown</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>af3c0198b40cbd5c5e4bb4f173df4fff1</anchor>
      <arglist>(Socket_t xSocket, BaseType_t xHow)</arglist>
    </member>
    <member kind="function">
      <type>TickType_t</type>
      <name>xTCPTimerCheck</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad3802e1a3603f571bcb185f2afbd48de</anchor>
      <arglist>(BaseType_t xWillSleep)</arglist>
    </member>
    <member kind="function">
      <type>FreeRTOS_Socket_t *</type>
      <name>pxTCPSocketLookup</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ae8099cad140d7cf14f3d25392eb74bea</anchor>
      <arglist>(uint32_t ulLocalIP, UBaseType_t uxLocalPort, uint32_t ulRemoteIP, UBaseType_t uxRemotePort)</arglist>
    </member>
    <member kind="function">
      <type>const struct xSTREAM_BUFFER *</type>
      <name>FreeRTOS_get_rx_buf</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0c996d1928c4d004672d4d996f1d12c1</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>lTCPAddRxdata</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a712ce92b8518f62444aad31e0fb1d424</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, size_t uxOffset, const uint8_t *pcData, uint32_t ulByteCount)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_GetRemoteAddress</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a9e0d7303574cd99128cbdb79e54adc89</anchor>
      <arglist>(ConstSocket_t xSocket, struct freertos_sockaddr *pxAddress)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_maywrite</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a50d7c4b5b3eeea34905a4f16b6f13965</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_tx_space</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ab49ef118747de118579ce9ba6e2d3d10</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_tx_size</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>af92a73b86b115ac2caeaf6d98d696723</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_issocketconnected</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ad8dffb3417a60b9c98108408052579ad</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_mss</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a10a19083f6bdd12aa4e0a8e508454385</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_connstatus</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0a6df495a5df34259dd6ef400992a46f</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>FreeRTOS_rx_size</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a6780a9d34f029526febb9b8a26f1b0c5</anchor>
      <arglist>(ConstSocket_t xSocket)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>FreeRTOS_netstat</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a48a287752bb66a04cb00ab0e427f4808</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vSocketSelect</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>abc66394397377cd08c86af20547f9302</anchor>
      <arglist>(SocketSelect_t *pxSocketSet)</arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xBoundUDPSocketsList</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>a0bb39b9379d9271063dad16351b7d3ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xBoundTCPSocketsList</name>
      <anchorfile>_free_r_t_o_s___sockets_8c.html</anchorfile>
      <anchor>ae02571e6608569169207562135f16785</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_Stream_Buffer.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___stream___buffer_8c.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>uxStreamBufferAdd</name>
      <anchorfile>_free_r_t_o_s___stream___buffer_8c.html</anchorfile>
      <anchor>a1616165d71579114071ce6c9fcec9d74</anchor>
      <arglist>(StreamBuffer_t *pxBuffer, size_t uxOffset, const uint8_t *pucData, size_t uxByteCount)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>uxStreamBufferGet</name>
      <anchorfile>_free_r_t_o_s___stream___buffer_8c.html</anchorfile>
      <anchor>ab2e63d3708d238cb5b6dd9f8372f8a62</anchor>
      <arglist>(StreamBuffer_t *pxBuffer, size_t uxOffset, uint8_t *pucData, size_t uxMaxCount, BaseType_t xPeek)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_TCP_IP.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___t_c_p___i_p_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_FIN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aa23ea9f1090719d5fe63b05b916f8475</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_SYN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aab413c82652ca09af97361cf9477a47c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_RST</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aeaca8c0103423aaab32d151387adee18</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_PSH</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a71daac922d12e3e3ff760fbfc7db8875</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_ACK</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a96dee2ad910b9b2121318ab0a22291dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_URG</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a0cecc71bb9663461bbcdd14e1cca10b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_ECN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a9bd4fe51b7e508a123941990ff10ffee</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_CWR</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aa88c66473fde8218a9e885b645e41a13</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_FLAG_CTRL</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ac11f82b0e649e5d88a6a1196b08b401d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_END</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a0bba2554431f5b7fda5d97d222ef9786</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_NOOP</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a0fdf7f0c704090fc5e0439f993a40f8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_MSS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a1ca14d2b8c5971642e2d2645c492cc2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_WSOPT</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a7fd42af6084ca092b20953b996f39719</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_SACK_P</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a7cecfff8a8e16e2059268c16af8f5615</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_SACK_A</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>adc66468782cc29eb6ce182a07b2d11d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_TIMESTAMP</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a7fedb3a976274f11461a300b2d9568e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_MSS_LEN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a0d7cfb33a05d29167808c7c7a093d5bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_WSOPT_LEN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a3fe51ca2c0ca39f68c207456152fad2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OPT_TIMESTAMP_LEN</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ad565b934d8982b674a926eefa85481fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ipconfigTCP_ACK_EARLIER_PACKET</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ab2443d4b4e820eba87abb0cc920cccff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpNOW_CONNECTED</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aa179a7d374ac0a969dba186a61621db1</anchor>
      <arglist>(status)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpVALID_BITS_IN_TCP_OFFSET_BYTE</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a70d3b988aa560daa7b2675ff9986c01b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpDELAYED_ACK_SHORT_DELAY_MS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ae3395a76923220eca4221c12dff07d3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpDELAYED_ACK_LONGER_DELAY_MS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a2813bcb2476a861459453422d1960008</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpREDUCED_MSS_THROUGH_INTERNET</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>abc72505b99e0b00f63279c6c4b6ab24c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OFFSET_LENGTH_BITS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a61dd1d433f631a2c079b0ef7703cf764</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpTCP_OFFSET_STANDARD_LENGTH</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ae9e28e136632b3bc7881c2ef87e966c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SEND_REPEATED_COUNT</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a45c75cc05cf4bbf1d8160191dff0c303</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>tcpMAXIMUM_TCP_WAKEUP_TIME_MS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aa70b9afd422a764386be33ca7847d5bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>xIPHeaderSize</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a688a156ac82e335f6acb73564af691e7</anchor>
      <arglist>(pxNetworkBuffer)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>uxIPHeaderSizeSocket</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a8136ac000e6a04d5f098cd691ac1a14b</anchor>
      <arglist>(pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPSocketIsActive</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a074e94106db91d94f21c0580affde782</anchor>
      <arglist>(eIPTCPState_t xStatus)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>prvTCPSendPacket</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>abfb9df3f6539966ec17c4752cd99e035</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>prvTCPSendRepeated</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ae55092c8fe479837a69c29167b5e16ae</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvTCPReturnPacket</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a90c3299ce1b59128557449658e170da3</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxDescriptor, uint32_t ulLen, BaseType_t xReleaseAfterSend)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvTCPCreateWindow</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a486e0a50c3dc44834ee585ed9c62766b</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPPrepareConnect</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a259d393932637c52fd7e1eb22c10b3c6</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function">
      <type>_static void</type>
      <name>prvCheckOptions</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aaa4372b4f0f440715d288b7fd5681be3</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, const NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>_static size_t</type>
      <name>prvSingleStepTCPHeaderOptions</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a8f5ccd90ad6e056252cbde7d34c9bda0</anchor>
      <arglist>(const uint8_t *const pucPtr, size_t uxTotalLength, FreeRTOS_Socket_t *const pxSocket, BaseType_t xHasSYNFlag)</arglist>
    </member>
    <member kind="function">
      <type>_static void</type>
      <name>prvReadSackOption</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a5bd3ea49e8ee6f2c7a12bb2708575d88</anchor>
      <arglist>(const uint8_t *const pucPtr, size_t uxIndex, FreeRTOS_Socket_t *const pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static UBaseType_t</type>
      <name>prvSetSynAckOptions</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a0ffa471bf4c769535d3f7b9d5a968cdb</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, TCPHeader_t *pxTCPHeader)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvTCPTouchSocket</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>af1e47864a5591d814aa9baf281c20cd5</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>prvTCPPrepareSend</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a03f032b5eef69f4f6a2ba16a2c516954</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, UBaseType_t uxOptionsLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TickType_t</type>
      <name>prvTCPNextTimeout</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ae80be66ff5dea32208cc3303072fbfa3</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvTCPAddTxData</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a5c3b5073967e63a162c6fd13dc9b2a45</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPHandleFin</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aed19306eb74e28863615dd7be8e2e1a7</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, const NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvCheckRxData</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a5273259449f802266c3c5a8d0ef03389</anchor>
      <arglist>(const NetworkBufferDescriptor_t *pxNetworkBuffer, uint8_t **ppucRecvData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvStoreRxData</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a918dc39521690f49ba16c2ed9deacf4e</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, const uint8_t *pucRecvData, NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulReceiveLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static UBaseType_t</type>
      <name>prvSetOptions</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a2f106d9f19f8c57b3b851fb77eab9445</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, const NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvHandleSynReceived</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a38bfd1b31c5ec1532040b5c728b52222</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, const NetworkBufferDescriptor_t *pxNetworkBuffer, uint32_t ulReceiveLength, UBaseType_t uxOptionsLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvHandleEstablished</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a321329a25c8f6de7856bec44678d6326</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, uint32_t ulReceiveLength, UBaseType_t uxOptionsLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvSendData</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ad48f7b6332e810a14c8269f17775ea10</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer, uint32_t ulReceiveLength, BaseType_t xByteCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPHandleState</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a396f3542839fd1e377e333918afd81a4</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t **ppxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPSendSpecialPacketHelper</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a1f06e746cb1cfac5d3e8c5a9465ba950</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer, uint8_t ucTCPFlags)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPSendChallengeAck</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>abd027c2104ce7522015fe84d02681516</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPSendReset</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ac7a1dc6f7c485315189b8b0f3094d839</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>prvSocketSetMSS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>adfc845a88dfffbbcd658bb30f10508ad</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static FreeRTOS_Socket_t *</type>
      <name>prvHandleListen</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ac2785c9db86dfb1f5f6b01155ddd54e0</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPSocketCopy</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>af01e793e6003047552ed3f542f0a66ff</anchor>
      <arglist>(FreeRTOS_Socket_t *pxNewSocket, FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static NetworkBufferDescriptor_t *</type>
      <name>prvTCPBufferResize</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>ad42a8368c193b45516ebc2082c8c6ea3</anchor>
      <arglist>(const FreeRTOS_Socket_t *pxSocket, NetworkBufferDescriptor_t *pxNetworkBuffer, int32_t lDataLen, UBaseType_t uxOptionsLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t</type>
      <name>prvWinScaleFactor</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>aed3d321465a45d65a96c7762cd5a8612</anchor>
      <arglist>(const FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xTCPSocketCheck</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a6f0ad2de00346e1017a692e4da7c3f0c</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vTCPStateChange</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a97d34454b4f1d4b0a4f5e5b998fdf578</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket, enum eTCP_STATE eTCPState)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xProcessReceivedTCPPacket</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>a3afd3312f86e44f4b4d95ab1e7bd34be</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxDescriptor)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xTCPCheckNewClient</name>
      <anchorfile>_free_r_t_o_s___t_c_p___i_p_8c.html</anchorfile>
      <anchor>abd9d9857511e8c986d7eb52849cfe73d</anchor>
      <arglist>(FreeRTOS_Socket_t *pxSocket)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_TCP_WIN.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___t_c_p___w_i_n_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>winSRTT_INCREMENT_NEW</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ad55bd7a27c05e46e1f269973ed270c2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>winSRTT_INCREMENT_CURRENT</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>acc3b8daf4436eb0a75561cd808256b35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>winSRTT_DECREMENT_NEW</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>af1109c15055d77738365721c9181600e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>winSRTT_DECREMENT_CURRENT</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>aeb55e5f7a5fbf975104e669323d115a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>winSRTT_CAP_mS</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a88e05f703fda730dec8cde14f9a91ef4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>xTCPWindowRxNew</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a6d6c19030ec6a51641664d45735a889d</anchor>
      <arglist>(pxWindow, ulSequenceNumber, lCount)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>xTCPWindowTxNew</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a6ebac9742c5a0d2013d77ef6084ea5ef</anchor>
      <arglist>(pxWindow, ulSequenceNumber, lCount)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OPTION_CODE_SINGLE_SACK</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>adec136da43a296db8af25299ca738963</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DUPLICATE_ACKS_BEFORE_FAST_RETRANSMIT</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a374732a2bcde8457d90a1e2ee2c665c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_TRANSMIT_COUNT_USING_LARGE_WINDOW</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>aa04acc1274bb1e10366880ee23dcfb82</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE</type>
      <name>ipDECL_CAST_PTR_FUNC_FOR_TYPE</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a43d5cbca96f53ed71a177b6e6d691d1a</anchor>
      <arglist>(TCPSegment_t)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>vListInsertGeneric</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a5674002cd6d72c9e1a6265b975546a87</anchor>
      <arglist>(List_t *const pxList, ListItem_t *const pxNewListItem, MiniListItem_t *const pxWhere)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvCreateSectors</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a4ce7e1d620e6b6384ef695bf959281d7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPWindowRxFind</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>af119bc1139e90f53f68a83870740e4e8</anchor>
      <arglist>(const TCPWindow_t *pxWindow, uint32_t ulSequenceNumber)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPWindowNew</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a386665c510184cd30ae32626ac59725e</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulSequenceNumber, int32_t lCount, BaseType_t xIsForRx)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPWindowGetHead</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a64cc22b4a6a21004ee0c952e51ccf41b</anchor>
      <arglist>(const List_t *pxList)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPWindowPeekHead</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ac065c8219a962706da4f2cd464f91215</anchor>
      <arglist>(const List_t *pxList)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>vTCPWindowFree</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a2eaaaa2cefae9b44094d0f069e2e486b</anchor>
      <arglist>(TCPSegment_t *pxSegment)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPWindowRxConfirm</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a9a4e36ef6bea7d2a2671e0a0d15ae85d</anchor>
      <arglist>(const TCPWindow_t *pxWindow, uint32_t ulSequenceNumber, uint32_t ulLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int32_t</type>
      <name>lTCPIncrementTxPosition</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ade7b7293181555c6a62ea9bac03becde</anchor>
      <arglist>(int32_t lPosition, int32_t lMax, int32_t lCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static BaseType_t</type>
      <name>prvTCPWindowTxHasSpace</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a633c1b1230a3c8c269c1af1a0fda8eb8</anchor>
      <arglist>(TCPWindow_t const *pxWindow, uint32_t ulWindowSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvTCPWindowTxCheckAck</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ac97053985dc382025116f6a528ace15e</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulFirst, uint32_t ulLast)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvTCPWindowFastRetransmit</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ac472739d521ae909bade3efb8c1678d1</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulFirst)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE BaseType_t</type>
      <name>xSequenceLessThanOrEqual</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>afb170f245b4ad4c30e552ac81e9c2519</anchor>
      <arglist>(uint32_t a, uint32_t b)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE BaseType_t</type>
      <name>xSequenceLessThan</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ab7a0e2f66ef1fe43dba4b3dccde4e322</anchor>
      <arglist>(uint32_t a, uint32_t b)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE BaseType_t</type>
      <name>xSequenceGreaterThan</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a1f8de3f936778f5e20f940dd26486ac0</anchor>
      <arglist>(uint32_t a, uint32_t b)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE BaseType_t</type>
      <name>xSequenceGreaterThanOrEqual</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>aff8c4167d89211fb1574ca667803c74c</anchor>
      <arglist>(uint32_t a, uint32_t b)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE void</type>
      <name>vListInsertFifo</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a868e38d1df123d9c14f49634cfe6a90a</anchor>
      <arglist>(List_t *const pxList, ListItem_t *const pxNewListItem)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE void</type>
      <name>vTCPTimerSet</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a583207662849bc5f15a8c7953eb9352b</anchor>
      <arglist>(TCPTimer_t *pxTimer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portINLINE uint32_t</type>
      <name>ulTimerGetAge</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a183f7f0caa41207e80e13fc41c4fdec3</anchor>
      <arglist>(const TCPTimer_t *pxTimer)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xTCPWindowRxEmpty</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a15f755746f17a8d2651bf3ba8002b76b</anchor>
      <arglist>(const TCPWindow_t *pxWindow)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vTCPWindowDestroy</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a0e8c6e2df2e93c6925a5db3268e54360</anchor>
      <arglist>(TCPWindow_t const *pxWindow)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vTCPWindowCreate</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a5168ceefeecf8e442b072b14d1516875</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulRxWindowLength, uint32_t ulTxWindowLength, uint32_t ulAckNumber, uint32_t ulSequenceNumber, uint32_t ulMSS)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vTCPWindowInit</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>acf48d309eda32d86dce923674e49b753</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulAckNumber, uint32_t ulSequenceNumber, uint32_t ulMSS)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vTCPSegmentCleanup</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a7dfa9050ace0b44f42ff29badf9a328b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>lTCPWindowRxCheck</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ac6932320f82e054f4dd20f12c4465472</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulSequenceNumber, uint32_t ulLength, uint32_t ulSpace)</arglist>
    </member>
    <member kind="function">
      <type>int32_t</type>
      <name>lTCPWindowTxAdd</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a26ee21f292a5c75d9700f154fbec8ea8</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulLength, int32_t lPosition, int32_t lMax)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xTCPWindowTxDone</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>ab1bde32e6d419a46c0db6a3270dcc73d</anchor>
      <arglist>(const TCPWindow_t *pxWindow)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xTCPWindowTxHasData</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>adb593eadf315d3ac0972308edb628f47</anchor>
      <arglist>(TCPWindow_t const *pxWindow, uint32_t ulWindowSize, TickType_t *pulDelay)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>ulTCPWindowTxGet</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a243b172dce331d908977afee9fceb09e</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulWindowSize, int32_t *plPosition)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>ulTCPWindowTxAck</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a4a87a9463c8daa90d88ebf25427f65af</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulSequenceNumber)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>ulTCPWindowTxSack</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a6b8d5c58d868eb5cb47ac185897bc6ab</anchor>
      <arglist>(TCPWindow_t *pxWindow, uint32_t ulFirst, uint32_t ulLast)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static TCPSegment_t *</type>
      <name>xTCPSegments</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>acf6414e1dbb7e9b0124eb236d573fde8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>_static List_t</type>
      <name>xSegmentList</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>aa333a2014b23e19157a9a8ab2f6b7bc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BaseType_t</type>
      <name>xTCPWindowLoggingLevel</name>
      <anchorfile>_free_r_t_o_s___t_c_p___w_i_n_8c.html</anchorfile>
      <anchor>a560d80e721483cab1b44f93d193091f7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FreeRTOS_UDP_IP.c</name>
    <path>/home/czjaso/amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/</path>
    <filename>_free_r_t_o_s___u_d_p___i_p_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ipIP_VERSION_AND_HEADER_LENGTH_BYTE</name>
      <anchorfile>_free_r_t_o_s___u_d_p___i_p_8c.html</anchorfile>
      <anchor>a8d163a6ae62224008f3c9fecf8fb7e8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vProcessGeneratedUDPPacket</name>
      <anchorfile>_free_r_t_o_s___u_d_p___i_p_8c.html</anchorfile>
      <anchor>a033394b01fa7d80e6393ea272a31814b</anchor>
      <arglist>(NetworkBufferDescriptor_t *const pxNetworkBuffer)</arglist>
    </member>
    <member kind="function">
      <type>BaseType_t</type>
      <name>xProcessReceivedUDPPacket</name>
      <anchorfile>_free_r_t_o_s___u_d_p___i_p_8c.html</anchorfile>
      <anchor>a49d02db9d4860930e55d072983162463</anchor>
      <arglist>(NetworkBufferDescriptor_t *pxNetworkBuffer, uint16_t usPort)</arglist>
    </member>
    <member kind="variable">
      <type>UDPPacketHeader_t</type>
      <name>xDefaultPartUDPPacketHeader</name>
      <anchorfile>_free_r_t_o_s___u_d_p___i_p_8c.html</anchorfile>
      <anchor>af9083d86a4c7d83308fc4d642dc07683</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ARPCacheRow_t</name>
    <filename>struct_a_r_p_cache_row__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulIPAddress</name>
      <anchorfile>struct_a_r_p_cache_row__t.html</anchorfile>
      <anchor>ad7e6f78fbdbf7496f71897b831ee9e19</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MACAddress_t</type>
      <name>xMACAddress</name>
      <anchorfile>struct_a_r_p_cache_row__t.html</anchorfile>
      <anchor>acb75c73fe317ae0ad35275551f4c4041</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAge</name>
      <anchorfile>struct_a_r_p_cache_row__t.html</anchorfile>
      <anchor>a62d48e7f8d847def3e252b48abc201e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucValid</name>
      <anchorfile>struct_a_r_p_cache_row__t.html</anchorfile>
      <anchor>a3097622f9ccba0e9b82e1d4b5e317606</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>xDHCP_DATA</name>
    <filename>structx_d_h_c_p___d_a_t_a.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulTransactionId</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>adbc2b6c978a64237b21501d47b4a68a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulOfferedIPAddress</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a7744ff1fe929ff7419929e3482daa9ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulDHCPServerAddress</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a04c9aadfb84a9f0c128ccd3cace59d26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulLeaseTime</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a7b77339ff6e3d172bb8541ae4af31002</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>xDHCPTxTime</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a62eb4df9377176103366608f89cde773</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>xDHCPTxPeriod</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a67d277497631a2be9c5d2b1cd5458cb0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BaseType_t</type>
      <name>xUseBroadcast</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a57b735613f051927ed9e82f07256d3cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>eDHCPState_t</type>
      <name>eDHCPState</name>
      <anchorfile>structx_d_h_c_p___d_a_t_a.html</anchorfile>
      <anchor>a5dd7a3139a5d159a65c6604a44afe5fc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>DNSCallback_t</name>
    <filename>struct_d_n_s_callback__t.html</filename>
    <member kind="variable">
      <type>TickType_t</type>
      <name>uxRemaningTime</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>add4c492671d7a66d13dbb9f766a66399</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>FOnDNSEvent</type>
      <name>pCallbackFunction</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>ac09c7cd402a8a9fcd004a7732dc283f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TimeOut_t</type>
      <name>uxTimeoutState</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>ab9f9a9a1e2fa1680adf0bed9596b3977</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pvSearchID</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>a0645204b5bcb9458b18dd206f5e96782</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct xLIST_ITEM</type>
      <name>xListItem</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>a229e8071c1bf899f6dbb900a9a546b76</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>pcName</name>
      <anchorfile>struct_d_n_s_callback__t.html</anchorfile>
      <anchor>ae287a16502741b744be1b4b5a286d67c</anchor>
      <arglist>[1]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>freertos_sockaddr</name>
    <filename>structfreertos__sockaddr.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>sin_len</name>
      <anchorfile>structfreertos__sockaddr.html</anchorfile>
      <anchor>ae3dd93b37bd98e67a7eee205880fab42</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>sin_family</name>
      <anchorfile>structfreertos__sockaddr.html</anchorfile>
      <anchor>a825766c52957d63bcf215bbf042e7edd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>sin_port</name>
      <anchorfile>structfreertos__sockaddr.html</anchorfile>
      <anchor>a7bdc446dabdc9f6a4ddf3f75fa80bcb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>sin_addr</name>
      <anchorfile>structfreertos__sockaddr.html</anchorfile>
      <anchor>a71e5f78f1f8f5260b0b21f82b182f0f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>FreeRTOS_Socket_t</name>
    <filename>struct_free_r_t_o_s___socket__t.html</filename>
    <member kind="variable">
      <type>EventBits_t</type>
      <name>xEventBits</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>ac537b7cd7c67cb72ee80539a3ec86615</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EventGroupHandle_t</type>
      <name>xEventGroup</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a1087ecfb6310a094c37199bb74ee552d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ListItem_t</type>
      <name>xBoundSocketListItem</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a81f7fb13eb2eb7e9d3dd6d53d1b62fc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>xReceiveBlockTime</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a11b6b2c460bed74b44982db42571f18c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>xSendBlockTime</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a62980b673e34f3d860d28291e8368dc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usLocalPort</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a0823b422673d39a0777f782e5b228924</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucSocketOptions</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a5c58826d6de231ea85fbd8232d483e73</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucProtocol</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a4957f47b13a4cab656e3394af700dea8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct xSOCKET_SET *</type>
      <name>pxSocketSet</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>aa9d04341596f66a37c9d07a74ed0072a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EventBits_t</type>
      <name>xSelectBits</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a6c47e1e3ea0d3f73530f37086e5fd1c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>EventBits_t</type>
      <name>xSocketBits</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>af47947f9665f96f2f2c5746f6ea1b1c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IPUDPSocket_t</type>
      <name>xUDP</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>af7d20b59a58392c60e90742d7e05f9eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IPTCPSocket_t</type>
      <name>xTCP</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>ad7b0c1a2abd13c300e256223cfdb8861</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullTCPAlignment</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>a218ef7bb881b6f6cca87d328758d7eee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union FreeRTOS_Socket_t::@3</type>
      <name>u</name>
      <anchorfile>struct_free_r_t_o_s___socket__t.html</anchorfile>
      <anchor>af45d92a8fd3ac7bbe14eb37eb769079b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IPStackEvent_t</name>
    <filename>struct_i_p_stack_event__t.html</filename>
    <member kind="variable">
      <type>eIPEvent_t</type>
      <name>eEventType</name>
      <anchorfile>struct_i_p_stack_event__t.html</anchorfile>
      <anchor>a3c79cf0368a1c276db07326c04d2f48c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pvData</name>
      <anchorfile>struct_i_p_stack_event__t.html</anchorfile>
      <anchor>a74a2b43a698e7b1f80af6cbe7ba766f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IPTCPSocket_t</name>
    <filename>struct_i_p_t_c_p_socket__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulRemoteIP</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a32ed7eb620f15975b202bafaae98033b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usRemotePort</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a242d2f450863eb04051d067b1f9a65e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bMssChange</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ae2e1b69ff7626e60490dfce5c4e4b49e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bPassAccept</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a4fbfbc25d2999a94283498c287e948c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bPassQueued</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>aefabd205c12a912148f520bc7331bda3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bReuseSocket</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a67e3b0df6d2efac1d3c7479160b03a09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bCloseAfterSend</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a7955b5826284647786ec3d76738b5c60</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bUserShutdown</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>aed89f4457684a9741063ebdd0055615c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bCloseRequested</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ad3f84b87d8c3ad0d534ccf7636759aea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bLowWater</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a1b1283ba1e84b4194f16e27884ab6e56</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bWinChange</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a3266106030dfdc10c6cce5cd729e8753</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bSendKeepAlive</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a0797356381128d67f9446c98b178a3fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bWaitKeepAlive</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a950472303383c1403beb0c4f8730c13c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bConnPrepared</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>af610864692c1bb9612186b6124399381</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bConnPassed</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a8f6097b20b01bde570d44fde1b991838</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bFinAccepted</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ab54b8e386f194f485fb248924d8fbfbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bFinSent</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a6e595043eea244df0d3276d709347aaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bFinRecv</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a7a3a0e7d85826d93bca3ecbedc8b790b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bFinAcked</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>af48a9f4d51b0eb0d9050ddb9f29af026</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bFinLast</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a71a1bd7622b0355b0da95a015c199f9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bRxStopped</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a684da3bb496f67b256d4d5d3653e1b3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bMallocError</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a084d5462a8ac914e1295bb9841a9d9b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bWinScaling</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a108ad4e95847749be6d17617368fa88a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct IPTCPSocket_t::@2</type>
      <name>bits</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a65e5019837336ac80d144aa1c91028e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulHighestRxAllowed</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a8558cd1bc60e6ec1a490fb0627a8a90e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usTimeout</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a6af07b9c7a0e7a73bf52c11ea4657b64</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usCurMSS</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a327fd7be334ed970068b0ba825453376</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usInitMSS</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a12363ab6c5cdac8495864a358c4a0d5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usChildCount</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ab9c9a21ca5f0a92fe267bf054d905bbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usBacklog</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a3faa3e47f0f9024e6544317387f5257d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucRepCount</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a114f05ceb8efdc0552608a6adb6d46db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTCPState</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ad8bcdb186864a071efa65791fbea737c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct xSOCKET *</type>
      <name>pxPeerSocket</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>afa792ed5525eb9c9f2190ed3b6319ba1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>xLastActTime</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ad0f826d4ed758787daef5d0c938bcdf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxLittleSpace</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a2790d23e492ae2e72115f524518eae79</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxEnoughSpace</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a08e862a831d0c8ccce04281b2a4f99dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxRxStreamSize</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>adae0a2e945eb93b7197fe4d1db9ff502</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxTxStreamSize</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ab8fb5fcab4fb96ef8d5074624d886926</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StreamBuffer_t *</type>
      <name>rxStream</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>acc0e922869614b29dfacbaaca87a8fd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>StreamBuffer_t *</type>
      <name>txStream</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a0f8ca18cd9438cbb3ff4b81df888c2f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>NetworkBufferDescriptor_t *</type>
      <name>pxAckMessage</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a2684031d3344d337d00783af8c7834d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>LastTCPPacket_t</type>
      <name>xPacket</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a825906211789ffb7e900020e3a700d2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>tcpflags</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a88f867efcc4b3e6de5994f2ec8a43489</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucMyWinScaleFactor</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>ab8f19ca09df246571c4503d26184882a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucPeerWinScaleFactor</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a9af40a7697b7ea5a2fbd9e34a108f9de</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulWindowSize</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a397e9d9a6b7fe660b0603a0933c82652</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxRxWinSize</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a3a237318fc4107062470fe9523f67bc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxTxWinSize</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a7d77b6dadf986bbb813da8b9fca7fa15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPWindow_t</type>
      <name>xTCPWindow</name>
      <anchorfile>struct_i_p_t_c_p_socket__t.html</anchorfile>
      <anchor>a50eccf22fc5189e7710be4271d4fb740</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IPTimer_t</name>
    <filename>struct_i_p_timer__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bActive</name>
      <anchorfile>struct_i_p_timer__t.html</anchorfile>
      <anchor>a5d4b2f7d9479bad52891214655c91d05</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bExpired</name>
      <anchorfile>struct_i_p_timer__t.html</anchorfile>
      <anchor>a7244df48107f28b502e0482d80d5ed79</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TimeOut_t</type>
      <name>xTimeOut</name>
      <anchorfile>struct_i_p_timer__t.html</anchorfile>
      <anchor>a1fbb967ea7d847861fcdc91fac46f664</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>ulRemainingTime</name>
      <anchorfile>struct_i_p_timer__t.html</anchorfile>
      <anchor>a12d360ad42cfca0fb6e9e1fe02c89130</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TickType_t</type>
      <name>ulReloadTime</name>
      <anchorfile>struct_i_p_timer__t.html</anchorfile>
      <anchor>aec37c3212367cf4478cd02b26cb9fdb9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IPUDPSocket_t</name>
    <filename>struct_i_p_u_d_p_socket__t.html</filename>
    <member kind="variable">
      <type>List_t</type>
      <name>xWaitingPacketsList</name>
      <anchorfile>struct_i_p_u_d_p_socket__t.html</anchorfile>
      <anchor>a75d281c062cef5f3ac4ba623f57cff9d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>LastTCPPacket_t</name>
    <filename>union_last_t_c_p_packet__t.html</filename>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullAlignmentWord</name>
      <anchorfile>union_last_t_c_p_packet__t.html</anchorfile>
      <anchor>acac0b536fd23d0549090a14fee75c2f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct LastTCPPacket_t::@0</type>
      <name>a</name>
      <anchorfile>union_last_t_c_p_packet__t.html</anchorfile>
      <anchor>a0c7565d1dcdd4b49f66993a07aed2b49</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct LastTCPPacket_t::@1</type>
      <name>u</name>
      <anchorfile>union_last_t_c_p_packet__t.html</anchorfile>
      <anchor>a52e1ac85c8c531d827d8919dc5a10d6d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LowHighWater_t</name>
    <filename>struct_low_high_water__t.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>uxLittleSpace</name>
      <anchorfile>struct_low_high_water__t.html</anchorfile>
      <anchor>ad06f5b9357359dd6d4fe360b27d58576</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>uxEnoughSpace</name>
      <anchorfile>struct_low_high_water__t.html</anchorfile>
      <anchor>ab46bf12f20f5b1b8b4f20b56be2799e8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>NetworkAddressingParameters_t</name>
    <filename>struct_network_addressing_parameters__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulDefaultIPAddress</name>
      <anchorfile>struct_network_addressing_parameters__t.html</anchorfile>
      <anchor>ae045d16fa68bfe73642742a5f31e2c37</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulNetMask</name>
      <anchorfile>struct_network_addressing_parameters__t.html</anchorfile>
      <anchor>a22fbe93b74fb20da191c622797d3df81</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulGatewayAddress</name>
      <anchorfile>struct_network_addressing_parameters__t.html</anchorfile>
      <anchor>a9d71a84b99c03d5b7ba7a8bd4a83eaed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulDNSServerAddress</name>
      <anchorfile>struct_network_addressing_parameters__t.html</anchorfile>
      <anchor>aa67e87c2631d5ec4b4521ce60fce0c13</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulBroadcastAddress</name>
      <anchorfile>struct_network_addressing_parameters__t.html</anchorfile>
      <anchor>ad73e26c68c4bb9f768ddee1c509097ba</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>NetworkBufferDescriptor_t</name>
    <filename>struct_network_buffer_descriptor__t.html</filename>
    <member kind="variable">
      <type>ListItem_t</type>
      <name>xBufferListItem</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>a2686bee348dfc8652b3d533d8f3fc834</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulIPAddress</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>ae5ab9a79f989beec2c8a5f20b6507f1b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucEthernetBuffer</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>abf478d3a5f40ee8fcff38b60d40d3ad9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>xDataLength</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>a8f99bd5f37c30f0479c46ef33edbbbef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usPort</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>a7c33c718263189dfb7e5d69faed2acd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usBoundPort</name>
      <anchorfile>struct_network_buffer_descriptor__t.html</anchorfile>
      <anchor>a03acb46a1d606b0a1fa383fd29e3ba2e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>ProtocolHeaders_t</name>
    <filename>union_protocol_headers__t.html</filename>
    <member kind="variable">
      <type>ICMPHeader_t</type>
      <name>xICMPHeader</name>
      <anchorfile>union_protocol_headers__t.html</anchorfile>
      <anchor>a34fa07a31297737bff19d733c6b89437</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>UDPHeader_t</type>
      <name>xUDPHeader</name>
      <anchorfile>union_protocol_headers__t.html</anchorfile>
      <anchor>a340de9e523d1acc9d2f74bc2aa68c623</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPHeader_t</type>
      <name>xTCPHeader</name>
      <anchorfile>union_protocol_headers__t.html</anchorfile>
      <anchor>a4ea5af3581d9234f7eebf3f3191cc04a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>ProtocolPacket_t</name>
    <filename>union_protocol_packet__t.html</filename>
    <member kind="variable">
      <type>ARPPacket_t</type>
      <name>xARPPacket</name>
      <anchorfile>union_protocol_packet__t.html</anchorfile>
      <anchor>aad27f9018b835568291e438814538985</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPPacket_t</type>
      <name>xTCPPacket</name>
      <anchorfile>union_protocol_packet__t.html</anchorfile>
      <anchor>a06b806c78329b262b110ae28acf4b248</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>UDPPacket_t</type>
      <name>xUDPPacket</name>
      <anchorfile>union_protocol_packet__t.html</anchorfile>
      <anchor>a361b2c5467713271312045f01a9b4b0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ICMPPacket_t</type>
      <name>xICMPPacket</name>
      <anchorfile>union_protocol_packet__t.html</anchorfile>
      <anchor>abf91797faa0b5541232bbe007fd5c358</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>SocketSelect_t</name>
    <filename>struct_socket_select__t.html</filename>
    <member kind="variable">
      <type>EventGroupHandle_t</type>
      <name>xSelectGroup</name>
      <anchorfile>struct_socket_select__t.html</anchorfile>
      <anchor>ad8778bf9441050e999d8c1a887e56ddf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>SocketSelectMessage_t</name>
    <filename>struct_socket_select_message__t.html</filename>
    <member kind="variable">
      <type>TaskHandle_t</type>
      <name>xTaskhandle</name>
      <anchorfile>struct_socket_select_message__t.html</anchorfile>
      <anchor>aeb82944475acabab3da61b8a1eb6fbf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>SocketSelect_t *</type>
      <name>pxSocketSet</name>
      <anchorfile>struct_socket_select_message__t.html</anchorfile>
      <anchor>a743e5052f1c84acf389d5b58f874711a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>StreamBuffer_t</name>
    <filename>struct_stream_buffer__t.html</filename>
    <member kind="variable">
      <type>volatile size_t</type>
      <name>uxTail</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>ab4b51182188e1d42c49918d10d939785</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile size_t</type>
      <name>uxMid</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>ab3f03b76564f83ef2e040aaf14ca1330</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile size_t</type>
      <name>uxHead</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>ae29b53069e0afd6e117f77d43e4c68a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile size_t</type>
      <name>uxFront</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>a4f2a7a3c3a1098a77e74df26d78b92de</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>LENGTH</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>a68ae1bd7578a1851412320d4e234c00f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucArray</name>
      <anchorfile>struct_stream_buffer__t.html</anchorfile>
      <anchor>a1db2d6ef9f4266a540d86097a4051cdf</anchor>
      <arglist>[sizeof(size_t)]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>struct</name>
    <filename>structstruct.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucOpcode</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ad884a11402ccadd1b88ca8f9394ce0fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAddressType</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>abfa2426341ed766152ea7edfcd30ad7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAddressLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a14e877b0915020479dfd71609dd6f60b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucHops</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a0c8d8fde35ef807181a3af28302931e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulTransactionID</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ade137bb75fbc6cf8b3076fd707de4807</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usElapsedTime</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ab65dc4a8cd915d8a3123f4452679930a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usFlags</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a9ca7c93121ebde17c241c6090c313f93</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulClientIPAddress_ciaddr</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a4eecb7fd87bcbac3b01d61207829c585</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulYourIPAddress_yiaddr</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ae058b7eaa364f8fcf38ee855b8fb648c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulServerIPAddress_siaddr</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a0a54655dc8374e96f97de34aafa9d409</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulRelayAgentIPAddress_giaddr</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a1da36384572e810902bc854cd28f00b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucClientHardwareAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a13ee27d7a1edfa18cf81cc9b91272487</anchor>
      <arglist>[dhcpCLIENT_HARDWARE_ADDRESS_LENGTH]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucServerHostName</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aee417fad4b56e93f970d8dc2c258452a</anchor>
      <arglist>[dhcpSERVER_HOST_NAME_LENGTH]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucBootFileName</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ab24e7f933bf7a8f0282e440be4e5ff78</anchor>
      <arglist>[dhcpBOOT_FILE_NAME_LENGTH]</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulDHCPCookie</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a1d3d088b7f2d720efef9773d8ba4c3bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usIdentifier</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af96f1fbbc84c6ac1c3719857392f8597</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usQuestions</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a7752f9ed2e25dd035b4f31b368de8fe0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAnswers</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a483bc958fc50d3b5e47ddf9a539d4344</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAuthorityRRs</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ae2aa9304babbd2541f1bf4dbb8c309e0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAdditionalRRs</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a4bb8658bff14067061b80cacf6761e11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usType</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>afa179e4aeb32555362ee66606d2324bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usClass</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>adf285d61ed77b0940467158cdfaf8aa3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulTTL</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a34ae47269739c65e8469f7fd09b96964</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usDataLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a9ffbda20e045457c9bbe403a24759404</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucNameCode</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a4b50a198a9df148cbde7b2c95f81843b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucNameOffset</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a257301885c032e92e9542267e86469cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulIPAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a7e9ec58f99b2f9e37f65003da0eda465</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usRequestId</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a07f39d1e5a960e4f38338e95fab5a090</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>ulRequestCount</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aa3543798540fd8600ebeefed7bce7e39</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAnswerRSS</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a643bbda4fc302ec6b950f43a9cb2aa39</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAuthRSS</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a8583be7db5798133650c8c427c593d62</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usAdditionalRSS</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a5932152b28319245098e34ba6dbe9435</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucNameSpace</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a788d06e257ae1419413056158496e5b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucName</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a291119af58dc7278af21aba45c6d7c98</anchor>
      <arglist>[dnsNBNS_ENCODED_NAME_LENGTH]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucNameZero</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a1cc2f4af20d58eac892fa1d77c91e958</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usNbFlags</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af9009784cc534974da828691cd9a7f6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucBytes</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a42f7aefbd34c2b9c513a9d7cb8d289aa</anchor>
      <arglist>[ipMAC_ADDRESS_LENGTH_BYTES]</arglist>
    </member>
    <member kind="variable">
      <type>MACAddress_t</type>
      <name>xDestinationAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ade7fc0f7222ea6079955b2cdcb6408a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MACAddress_t</type>
      <name>xSourceAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af203141196c89d5f1a90af29dd077212</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usFrameType</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ac2294956ef25abf79b292eae53ca0d97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usHardwareType</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a1f9dc6989d16e650f0b15b646fc502b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usProtocolType</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a499848689c664a20ef209c0d22b42223</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucHardwareAddressLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aaff67273a3b66f2d23759c1479025bae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucProtocolAddressLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a654eb61393780f5a5d5cfb93a31579f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usOperation</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a76d52ece3a066bebc7f18ac933f9a956</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>MACAddress_t</type>
      <name>xSenderHardwareAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af5cfb48d31b8d85652a9da0821582606</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucSenderProtocolAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a9318bcd50bdd8448215145252f182139</anchor>
      <arglist>[4]</arglist>
    </member>
    <member kind="variable">
      <type>MACAddress_t</type>
      <name>xTargetHardwareAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a37eed545b17af44cc19f5ef63335aa4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulTargetProtocolAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a3fcf3a4c59e190915b0d09718269c5c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucVersionHeaderLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a54ef7e602f84f2b3476cd10efef96720</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucDifferentiatedServicesCode</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a0ddd590409edd968cefb18ededaefc4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usLength</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a5e26363013a5761eb3d0796d35b7f71d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usIdentification</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ad4acebb8f4e30bae0506558e18be1538</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usFragmentOffset</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aa3955126880a21beccb06bfa72f05b60</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTimeToLive</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a5bfe2de234be9e3342e60e8fa9c09e11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucProtocol</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>acd2508a55e01fa8fa55a65b9febc7e2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usHeaderChecksum</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a78614faa161325bbc6e5f1edd43d1f32</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulSourceIPAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a70f5e64b15e32ce8d87ec4c2afe7fafc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulDestinationIPAddress</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a12f4a589751b2c6ac6181f585cceb3d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTypeOfMessage</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>adf99ac0dc4ef8a20834574a8d06dbe48</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTypeOfService</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a8dc4f9048189a6571fbb64cae047e3fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usChecksum</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af1e30c3a15d68c6692928407fc26d225</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usSequenceNumber</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a4188a123470f3f59e17107bfb3a533e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usSourcePort</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ade05608e9ba9d8391b1ed07dbdd260fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usDestinationPort</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a8eec861cc2d2845acdb21d45f244169b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulSequenceNumber</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aa407a89e2e8b38f3ce62e8da06b4dfbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulAckNr</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a3c191068c88c577058c4ef882321a1b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTCPOffset</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a083a955bb7ee558067570a394179e5f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTCPFlags</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a915becd2b5be617372864ba45febfaec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usWindow</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a8b27bc8a1b97fddab32d51ad373c018d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usUrgent</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a2d74d4b3d045fe40aa1a26f345e2473c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucOptdata</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>af10dade2e3ca23ad86b3f24fec92e00c</anchor>
      <arglist>[ipSIZE_TCP_OPTIONS]</arglist>
    </member>
    <member kind="variable">
      <type>EthernetHeader_t</type>
      <name>xEthernetHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ade289b8c53e649fa2559f1cee4766a40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ARPHeader_t</type>
      <name>xARPHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a27b6bfc4e4db6aa1ff10e08bdd13dcda</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IPHeader_t</type>
      <name>xIPHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>ad73143081d8c065e2fed17fbff3d7fbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>ICMPHeader_t</type>
      <name>xICMPHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>aff9b7d7a7806207eb441d9d0ed36a808</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>UDPHeader_t</type>
      <name>xUDPHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a2943ee8413c4b5e44adad8a970db6977</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPHeader_t</type>
      <name>xTCPHeader</name>
      <anchorfile>structstruct.html</anchorfile>
      <anchor>a3c5bfce97918a23030d84a5ea120a988</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TCPSegment_t</name>
    <filename>struct_t_c_p_segment__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulSequenceNumber</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a8b4011e5bbe6f229a6f47c072a3d8297</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lMaxLength</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>acfdf465e8083ecc8f0370b7bc9328b11</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lDataLength</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>af4be27b695914f3035e0320c19346d05</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lStreamPos</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>ad3c4fa986c17a6076142594ab3c4f67f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPTimer_t</type>
      <name>xTransmitTimer</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a57ed94ff6a304cd5e90ae0a96c253f56</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ucTransmitCount</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a15cd5a913a70812749c6ca24f9e34cd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ucDupAckCount</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>aad2e3391e82c9783740bd991cf1359d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bOutstanding</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a56f492387b0815a3cb07720268e5e043</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bAcked</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a37fbd53c8c14464983fdb9e4bc32ba98</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bIsForRx</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>ab8ea887b001f6304c98e402b32574b13</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union TCPSegment_t::@4</type>
      <name>u</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a3d63c42a342e707b55841aecc8138a62</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct xLIST_ITEM</type>
      <name>xQueueItem</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a7bde38f7cc5b76a79c350141167992e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct xLIST_ITEM</type>
      <name>xSegmentItem</name>
      <anchorfile>struct_t_c_p_segment__t.html</anchorfile>
      <anchor>a59a037cac9030851305587a913cab86b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TCPTimer_t</name>
    <filename>struct_t_c_p_timer__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulBorn</name>
      <anchorfile>struct_t_c_p_timer__t.html</anchorfile>
      <anchor>a9fe34b13c92967053a2f5d89a3d71ac5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TCPWindow_t</name>
    <filename>struct_t_c_p_window__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bHasInit</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>acd7249d799b345cd92c97e3bfb12933b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bSendFullSize</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a97000733d08ecc2dce73e544ad7b9213</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bTimeStamps</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>aee1b76d9af1c92e55dc5137e547bc2fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct TCPWindow_t::@6::@8</type>
      <name>bits</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>ad2c811975f783cff7f2b1d717f2057ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union TCPWindow_t::@6</type>
      <name>u</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a84ca8818abbb0d2481624fcf31a772ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPWinSize_t</type>
      <name>xSize</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a0e1bb052b5e4f97b77761c917ced73aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulFirstSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a1e468229cce59ab0dc1599f648b2c667</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulCurrentSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a4d9da94474f534f21e773aaf056ae9b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulFINSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a8be59622f0a92e22d48b9cb3886109a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulHighestSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a8e161ce8c71ddf5509b057d81451a03e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct TCPWindow_t::@7</type>
      <name>rx</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a7bc4bf0b6fe9219457b690b4b32ebf71</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct TCPWindow_t::@7</type>
      <name>tx</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>ae34e3cbd54fcb8892c86d0f54099a48c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulOurSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a5bfdac42880e5ae6ef7ac77c1374f4e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulUserDataLength</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>ae6f1dd8e9d4a66a7797bcec120f059d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulNextTxSequenceNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a070811b69e87c61e47c59ff50e29dadc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lSRTT</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>acdf5efc1b38f7bc4fdd3cfbe874af180</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucOptionLength</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a6bde9c3ebe7f805aa99cccc49e0527b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xPriorityQueue</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a11a5714b6370953331491c9c5fa31355</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xTxQueue</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a8a28a2fa04e71e2cb8c9c84a918be55a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xWaitQueue</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a2e14cd25378090ca2fa7119a7bfa6ca9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TCPSegment_t *</type>
      <name>pxHeadSegment</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a308519ac46924e91342c1432c8edcd75</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulOptionsData</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>aeee5f06c6ecdcd5f093379c6e764af92</anchor>
      <arglist>[ipSIZE_TCP_OPTIONS/sizeof(uint32_t)]</arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xTxSegments</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a79746d8f377d703da7efeee43ea3ad91</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>List_t</type>
      <name>xRxSegments</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a67b0d188b8800f0984e2561661dca5b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usOurPortNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>af9d582e60dff97a5248009d5a5fa2a23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usPeerPortNumber</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>a1472b1532d03ab2057f67081e116457a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usMSS</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>abd2c39637cacf9b3b4bb934d1c43be64</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usMSSInit</name>
      <anchorfile>struct_t_c_p_window__t.html</anchorfile>
      <anchor>ab46f5a3d7e7e9452372e8cd5ff625c78</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TCPWinSize_t</name>
    <filename>struct_t_c_p_win_size__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulRxWindowLength</name>
      <anchorfile>struct_t_c_p_win_size__t.html</anchorfile>
      <anchor>a31a006b4cdadeb02ba0631ffdad7e9f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulTxWindowLength</name>
      <anchorfile>struct_t_c_p_win_size__t.html</anchorfile>
      <anchor>a6bfa12d5c5a63af08bcaca8b81bdc4e2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>UDPPacketHeader_t</name>
    <filename>union_u_d_p_packet_header__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucBytes</name>
      <anchorfile>union_u_d_p_packet_header__t.html</anchorfile>
      <anchor>a3be686b900eda092711056d7b6de68ef</anchor>
      <arglist>[24]</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulWords</name>
      <anchorfile>union_u_d_p_packet_header__t.html</anchorfile>
      <anchor>ac218ebdd30f53cc5dea89918865e8077</anchor>
      <arglist>[6]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>WinProperties_t</name>
    <filename>struct_win_properties__t.html</filename>
    <member kind="variable">
      <type>int32_t</type>
      <name>lTxBufSize</name>
      <anchorfile>struct_win_properties__t.html</anchorfile>
      <anchor>a999e0b398e52ffa79d5c8bfcff7a9323</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lTxWinSize</name>
      <anchorfile>struct_win_properties__t.html</anchorfile>
      <anchor>a50c170f82dc44a406463dfeb72277b67</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lRxBufSize</name>
      <anchorfile>struct_win_properties__t.html</anchorfile>
      <anchor>aafb5181e10dcf313d3d5d5d94a1f530d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lRxWinSize</name>
      <anchorfile>struct_win_properties__t.html</anchorfile>
      <anchor>a7a5729f01fbbf3e06ab911572cd2cecd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>xUnion32</name>
    <filename>unionx_union32.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>u32</name>
      <anchorfile>unionx_union32.html</anchorfile>
      <anchor>a0b4c94abdafb022ad16715a8bde5ee58</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>u16</name>
      <anchorfile>unionx_union32.html</anchorfile>
      <anchor>a362fc666e3b98db61fb823c8a079d991</anchor>
      <arglist>[2]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>u8</name>
      <anchorfile>unionx_union32.html</anchorfile>
      <anchor>a57bc88ea027b2f345d590d1ef1e10883</anchor>
      <arglist>[4]</arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>xUnionPtr</name>
    <filename>unionx_union_ptr.html</filename>
    <member kind="variable">
      <type>uint32_t *</type>
      <name>u32ptr</name>
      <anchorfile>unionx_union_ptr.html</anchorfile>
      <anchor>a50b27c7a9a159a9f99504c9da1ad462b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t *</type>
      <name>u16ptr</name>
      <anchorfile>unionx_union_ptr.html</anchorfile>
      <anchor>a64cd6ee0fbab334c3ba5f2b8c9818a0a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>u8ptr</name>
      <anchorfile>unionx_union_ptr.html</anchorfile>
      <anchor>a07a49178e15b944dc96ed0b4e6d29e81</anchor>
      <arglist></arglist>
    </member>
  </compound>
</tagfile>
