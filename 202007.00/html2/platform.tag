<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20">
  <compound kind="file">
    <name>atomic.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/freertos_kernel/include/</path>
    <filename>atomic_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ATOMIC_COMPARE_AND_SWAP_SUCCESS</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a2d30f083e50b866caf8822ec31a75969</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ATOMIC_COMPARE_AND_SWAP_FAILURE</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>ab374919fbdbc585c3436a94f5bad31b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_CompareAndSwap_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>ad0c956772f1d09cea1120c0ed6c4a0df</anchor>
      <arglist>(uint32_t volatile *pulDestination, uint32_t ulExchange, uint32_t ulComparand)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE void *</type>
      <name>Atomic_SwapPointers_p32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a64d255792849f5443c4209d746eb60c3</anchor>
      <arglist>(void *volatile *ppvDestination, void *pvExchange)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_CompareAndSwapPointers_p32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a0dfb0b26706475d490792af561a143d5</anchor>
      <arglist>(void *volatile *ppvDestination, void *pvExchange, void *pvComparand)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_Add_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a14d4af36077403e01685fae0cc93ed3d</anchor>
      <arglist>(uint32_t volatile *pulAddend, uint32_t ulCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_Subtract_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>ac5d1592ac108f14a62aa88be210a205e</anchor>
      <arglist>(uint32_t volatile *pulAddend, uint32_t ulCount)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_Increment_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a7ce395cb8553d0fb973f1db2dbac294f</anchor>
      <arglist>(uint32_t volatile *pulAddend)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_Decrement_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a6bb0ca2c06e0627945b1351ba36d55a7</anchor>
      <arglist>(uint32_t volatile *pulAddend)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_OR_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>af1dcc9bcd06047887bc57acb2133b622</anchor>
      <arglist>(uint32_t volatile *pulDestination, uint32_t ulValue)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_AND_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a9eeea6a0a5bec7c5ad9d3f073d396338</anchor>
      <arglist>(uint32_t volatile *pulDestination, uint32_t ulValue)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_NAND_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a9c13a49fba8a8f1eeca954d6b9dbfac4</anchor>
      <arglist>(uint32_t volatile *pulDestination, uint32_t ulValue)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static portFORCE_INLINE uint32_t</type>
      <name>Atomic_XOR_u32</name>
      <anchorfile>atomic_8h.html</anchorfile>
      <anchor>a070776e59f4253dda4ec58b5e2710e81</anchor>
      <arglist>(uint32_t volatile *pulDestination, uint32_t ulValue)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_atomic.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/common/include/</path>
    <filename>iot__atomic_8h.html</filename>
    <includes id="atomic_8h" name="atomic.h" local="yes" imported="no">atomic.h</includes>
  </compound>
  <compound kind="file">
    <name>iot_clock.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/platform/include/platform/</path>
    <filename>iot__clock_8h.html</filename>
    <includes id="iot__platform__types_8h" name="iot_platform_types.h" local="yes" imported="no">types/iot_platform_types.h</includes>
    <member kind="function">
      <type>bool</type>
      <name>IotClock_GetTimestring</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>a0553965ba7229559867bbc40fa66b5f2</anchor>
      <arglist>(char *pBuffer, size_t bufferSize, size_t *pTimestringLength)</arglist>
    </member>
    <member kind="function">
      <type>uint64_t</type>
      <name>IotClock_GetTimeMs</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>a4250d025e520722e00ec8962373aeffe</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotClock_SleepMs</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>aa417f6325c96e106be7f9475b4518fdd</anchor>
      <arglist>(uint32_t sleepTimeMs)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotClock_TimerCreate</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>ac0a636145d4c404321468a1717b77327</anchor>
      <arglist>(IotTimer_t *pNewTimer, IotThreadRoutine_t expirationRoutine, void *pArgument)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotClock_TimerDestroy</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>aed96751196e917c47c863ebe0d57c165</anchor>
      <arglist>(IotTimer_t *pTimer)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotClock_TimerArm</name>
      <anchorfile>iot__clock_8h.html</anchorfile>
      <anchor>a8a1408ae0ea0491a590b78f5ecbba369</anchor>
      <arglist>(IotTimer_t *pTimer, uint32_t relativeTimeoutMs, uint32_t periodMs)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_metrics.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/platform/include/platform/</path>
    <filename>iot__metrics_8h.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>IotMetrics_Init</name>
      <anchorfile>iot__metrics_8h.html</anchorfile>
      <anchor>ac4462b6a8ae2a9fe5b6d4e81151aeefa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotMetrics_Cleanup</name>
      <anchorfile>iot__metrics_8h.html</anchorfile>
      <anchor>a4889dd6622861a215584aa68601b0785</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotMetrics_GetTcpConnections</name>
      <anchorfile>iot__metrics_8h.html</anchorfile>
      <anchor>a3791b42a0495ec6c770235127eba5b19</anchor>
      <arglist>(void *pContext, void(*metricsCallback)(void *, const IotListDouble_t *))</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_network.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/platform/include/platform/</path>
    <filename>iot__network_8h.html</filename>
    <class kind="struct">IotNetworkInterface_t</class>
    <class kind="struct">IotNetworkServerInfo_t</class>
    <class kind="struct">IotNetworkCredentials_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotNetworkReceiveCallback_t</name>
      <anchorfile>iot__network_8h.html</anchorfile>
      <anchor>a544d243b70fe5ecd409ec79528bc1a65</anchor>
      <arglist>)(void *pConnection, void *pContext)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotNetworkError_t</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>gad892c8e3c5c42f699131c286d327bc7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_SUCCESS</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7faa97d8896628e062567f10ed81ab19e6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_FAILURE</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa6a0dcbd4fe4d46901b578e090220b7f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_BAD_PARAMETER</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fab92144374e38d6877f54e7124f4f21aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_NO_MEMORY</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa784a9c3f2e41c6c632e5212b88ec9037</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_SYSTEM_ERROR</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa4251f8441883776dce2fb01c54847ec7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_platform_types.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/platform/include/types/</path>
    <filename>iot__platform__types_8h.html</filename>
    <class kind="struct">IotMetricsTcpConnection_t</class>
    <member kind="define">
      <type>#define</type>
      <name>IOT_THREAD_DEFAULT_PRIORITY</name>
      <anchorfile>iot__platform__types_8h.html</anchorfile>
      <anchor>a41f0c9d181387db9557b17c9f6cd21cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_THREAD_DEFAULT_STACK_SIZE</name>
      <anchorfile>iot__platform__types_8h.html</anchorfile>
      <anchor>aee57e19310ccb730070bc3f10c4ea070</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_METRICS_IP_ADDRESS_LENGTH</name>
      <anchorfile>iot__platform__types_8h.html</anchorfile>
      <anchor>a20a16e3fbb2a0bfa5c7cec84e5466dae</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>_IotSystemMutex_t</type>
      <name>IotMutex_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>ga5c08296e536e5cc7e08f9ee8e1103d46</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>_IotSystemSemaphore_t</type>
      <name>IotSemaphore_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>ga8ab95c5b6c27799f3bf68c999a125bc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotThreadRoutine_t</name>
      <anchorfile>iot__platform__types_8h.html</anchorfile>
      <anchor>ad12e30cfb51d68c848befdcf92d791b4</anchor>
      <arglist>)(void *)</arglist>
    </member>
    <member kind="typedef">
      <type>_IotSystemTimer_t</type>
      <name>IotTimer_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>gaa6c5411f9ddc9ca8b0cbe180ebdc0b9b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_threads.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/platform/include/platform/</path>
    <filename>iot__threads_8h.html</filename>
    <includes id="iot__platform__types_8h" name="iot_platform_types.h" local="yes" imported="no">types/iot_platform_types.h</includes>
    <member kind="function">
      <type>bool</type>
      <name>Iot_CreateDetachedThread</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a0080f6e6731bce968cc061bd85c5bf68</anchor>
      <arglist>(IotThreadRoutine_t threadRoutine, void *pArgument, int32_t priority, size_t stackSize)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotMutex_Create</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a73ca5cbeca854379969a4a76d8bdd583</anchor>
      <arglist>(IotMutex_t *pNewMutex, bool recursive)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotMutex_Destroy</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a6ed0668270fbbc6b72d485857fca82bb</anchor>
      <arglist>(IotMutex_t *pMutex)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotMutex_Lock</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>ae1964d4d67f85635a15b119ab4c5822a</anchor>
      <arglist>(IotMutex_t *pMutex)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotMutex_TryLock</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a0160def05dc99fac5cc163fd8fc185bd</anchor>
      <arglist>(IotMutex_t *pMutex)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotMutex_Unlock</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a34318dcc0db7a255fafe3e19fbf09f93</anchor>
      <arglist>(IotMutex_t *pMutex)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotSemaphore_Create</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>aa982d3a141991065ad90da71c5947d88</anchor>
      <arglist>(IotSemaphore_t *pNewSemaphore, uint32_t initialValue, uint32_t maxValue)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotSemaphore_Destroy</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a6ff7563b8cba347415564d4616d24419</anchor>
      <arglist>(IotSemaphore_t *pSemaphore)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>IotSemaphore_GetCount</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>aa4002142e42c3ddf613cad03afc10c86</anchor>
      <arglist>(IotSemaphore_t *pSemaphore)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotSemaphore_Wait</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>af7621c64266d4f98e119f32851921437</anchor>
      <arglist>(IotSemaphore_t *pSemaphore)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotSemaphore_TryWait</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>a0feb6d747bcfa4404c3137839c6d45a5</anchor>
      <arglist>(IotSemaphore_t *pSemaphore)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotSemaphore_TimedWait</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>adcc0527e59df4d2b8cbdf1f8c505d35c</anchor>
      <arglist>(IotSemaphore_t *pSemaphore, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotSemaphore_Post</name>
      <anchorfile>iot__threads_8h.html</anchorfile>
      <anchor>abf975c1f553369610c54ddb50e1ec7e7</anchor>
      <arglist>(IotSemaphore_t *pSemaphore)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotMetricsTcpConnection_t</name>
    <filename>struct_iot_metrics_tcp_connection__t.html</filename>
    <member kind="variable">
      <type>IotLink_t</type>
      <name>link</name>
      <anchorfile>struct_iot_metrics_tcp_connection__t.html</anchorfile>
      <anchor>a730f62e3ed36ddff9cff9f6ae860ac39</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pNetworkContext</name>
      <anchorfile>struct_iot_metrics_tcp_connection__t.html</anchorfile>
      <anchor>a990a5a35e8236992d3ebfaaba1536a72</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>addressLength</name>
      <anchorfile>struct_iot_metrics_tcp_connection__t.html</anchorfile>
      <anchor>a51b8ac936afc4290243b36443fdff037</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>pRemoteAddress</name>
      <anchorfile>struct_iot_metrics_tcp_connection__t.html</anchorfile>
      <anchor>aec4c68cb131e7d32c87b3a1e6c4acf91</anchor>
      <arglist>[IOT_METRICS_IP_ADDRESS_LENGTH]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotNetworkCredentials_t</name>
    <filename>struct_iot_network_credentials__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pAlpnProtos</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a6043f7040d62c03ee9b9e64622ad713f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>maxFragmentLength</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>aa3d53a9b5a1a27a173b575972bdfe0a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>disableSni</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a2a1c333015b688f188f2db55768fb5e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pRootCa</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a6b8141736ae9ea36e164419f438c65c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>rootCaSize</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>ab93607920319b58a0c9c812acca45d24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pClientCert</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a38e2b899efa59e07bf0217806e88aa34</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>clientCertSize</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a81d5144471b3a79edebd67c2845d7fde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pPrivateKey</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>a10211ef6d727e7009844606811453968</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>privateKeySize</name>
      <anchorfile>struct_iot_network_credentials__t.html</anchorfile>
      <anchor>af9d4c4d40a9f6fe87e391209e041e3ab</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotNetworkInterface_t</name>
    <filename>struct_iot_network_interface__t.html</filename>
    <member kind="variable">
      <type>IotNetworkError_t(*</type>
      <name>create</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>a086d26a000f1409bd5a590287b508577</anchor>
      <arglist>)(void *pConnectionInfo, void *pCredentialInfo, void **pConnection)</arglist>
    </member>
    <member kind="variable">
      <type>IotNetworkError_t(*</type>
      <name>setReceiveCallback</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>a9958fc96d1ba3394de6182f3b0e65494</anchor>
      <arglist>)(void *pConnection, IotNetworkReceiveCallback_t receiveCallback, void *pContext)</arglist>
    </member>
    <member kind="variable">
      <type>size_t(*</type>
      <name>send</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>ac90a431f397d0c7283282d4da22061a0</anchor>
      <arglist>)(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="variable">
      <type>size_t(*</type>
      <name>receive</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>a8bd9f4c986b7ebac168372324d8334e8</anchor>
      <arglist>)(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="variable">
      <type>size_t(*</type>
      <name>receiveUpto</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>a1a3df78cb09a2a38815619e60087f97e</anchor>
      <arglist>)(void *pConnection, uint8_t *pBuffer, size_t bufferSize)</arglist>
    </member>
    <member kind="variable">
      <type>IotNetworkError_t(*</type>
      <name>close</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>aa4a1a5d4c82f167857f1bcefe28d87d2</anchor>
      <arglist>)(void *pConnection)</arglist>
    </member>
    <member kind="variable">
      <type>IotNetworkError_t(*</type>
      <name>destroy</name>
      <anchorfile>struct_iot_network_interface__t.html</anchorfile>
      <anchor>a747367333eba3766711b5d4f3ab0904c</anchor>
      <arglist>)(void *pConnection)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotNetworkServerInfo_t</name>
    <filename>struct_iot_network_server_info__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pHostName</name>
      <anchorfile>struct_iot_network_server_info__t.html</anchorfile>
      <anchor>a67b771966f8220edb22b16950bf6c1af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>port</name>
      <anchorfile>struct_iot_network_server_info__t.html</anchorfile>
      <anchor>a3d585a43fec6a74752023c5bbb7f20ab</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>platform_datatypes_handles</name>
    <title>Handles</title>
    <filename>group__platform__datatypes__handles.html</filename>
    <member kind="typedef">
      <type>_IotSystemMutex_t</type>
      <name>IotMutex_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>ga5c08296e536e5cc7e08f9ee8e1103d46</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>_IotSystemSemaphore_t</type>
      <name>IotSemaphore_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>ga8ab95c5b6c27799f3bf68c999a125bc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>_IotSystemTimer_t</type>
      <name>IotTimer_t</name>
      <anchorfile>group__platform__datatypes__handles.html</anchorfile>
      <anchor>gaa6c5411f9ddc9ca8b0cbe180ebdc0b9b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>platform_datatypes_enums</name>
    <title>Enumerated types</title>
    <filename>group__platform__datatypes__enums.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>IotNetworkError_t</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>gad892c8e3c5c42f699131c286d327bc7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_SUCCESS</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7faa97d8896628e062567f10ed81ab19e6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_FAILURE</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa6a0dcbd4fe4d46901b578e090220b7f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_BAD_PARAMETER</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fab92144374e38d6877f54e7124f4f21aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_NO_MEMORY</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa784a9c3f2e41c6c632e5212b88ec9037</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_NETWORK_SYSTEM_ERROR</name>
      <anchorfile>group__platform__datatypes__enums.html</anchorfile>
      <anchor>ggad892c8e3c5c42f699131c286d327bc7fa4251f8441883776dce2fb01c54847ec7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>platform_datatypes_paramstructs</name>
    <title>Parameter structures</title>
    <filename>group__platform__datatypes__paramstructs.html</filename>
    <class kind="struct">IotNetworkInterface_t</class>
    <class kind="struct">IotNetworkServerInfo_t</class>
    <class kind="struct">IotNetworkCredentials_t</class>
  </compound>
  <compound kind="page">
    <name>platform_config</name>
    <title>Configuration</title>
    <filename>platform_config.html</filename>
    <docanchor file="platform_config.html">IOT_LOG_LEVEL_PLATFORM</docanchor>
    <docanchor file="platform_config.html">IOT_LOG_LEVEL_NETWORK</docanchor>
    <docanchor file="platform_config.html" title="Memory allocation">platform_config_memory</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_clock</name>
    <title>Clock</title>
    <filename>platform_clock.html</filename>
    <docanchor file="platform_clock.html" title="Dependencies">platform_clock_dependencies</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_threads</name>
    <title>Thread Management</title>
    <filename>platform_threads.html</filename>
    <docanchor file="platform_threads.html" title="Dependencies">platform_threads_dependencies</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_network</name>
    <title>Networking</title>
    <filename>platform_network.html</filename>
    <docanchor file="platform_network.html" title="Dependencies">platform_network_dependencies</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_metrics</name>
    <title>Metrics</title>
    <filename>platform_metrics.html</filename>
    <docanchor file="platform_metrics.html" title="Dependencies">platform_metrics_dependencies</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_atomic</name>
    <title>Atomics</title>
    <filename>platform_atomic.html</filename>
    <docanchor file="platform_atomic.html" title="Dependencies">platform_atomic_dependencies</docanchor>
  </compound>
  <compound kind="page">
    <name>platform_functions</name>
    <title>Functions</title>
    <filename>platform_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_functions</name>
    <title>Clock</title>
    <filename>platform_clock_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_gettimestring</name>
    <title>IotClock_GetTimestring</title>
    <filename>platform_clock_function_gettimestring.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_gettimems</name>
    <title>IotClock_GetTimeMs</title>
    <filename>platform_clock_function_gettimems.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_sleepms</name>
    <title>IotClock_SleepMs</title>
    <filename>platform_clock_function_sleepms.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_timercreate</name>
    <title>IotClock_TimerCreate</title>
    <filename>platform_clock_function_timercreate.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_timerdestroy</name>
    <title>IotClock_TimerDestroy</title>
    <filename>platform_clock_function_timerdestroy.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_clock_function_timerarm</name>
    <title>IotClock_TimerArm</title>
    <filename>platform_clock_function_timerarm.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_metrics_functions</name>
    <title>Metrics</title>
    <filename>platform_metrics_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_metrics_function_init</name>
    <title>IotMetrics_Init</title>
    <filename>platform_metrics_function_init.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_metrics_function_cleanup</name>
    <title>IotMetrics_Cleanup</title>
    <filename>platform_metrics_function_cleanup.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_metrics_function_gettcpconnections</name>
    <title>IotMetrics_GetTcpConnections</title>
    <filename>platform_metrics_function_gettcpconnections.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_functions</name>
    <title>Networking</title>
    <filename>platform_network_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_create</name>
    <title>IotNetworkInterface_t::create</title>
    <filename>platform_network_function_create.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_setreceivecallback</name>
    <title>IotNetworkInterface_t::setReceiveCallback</title>
    <filename>platform_network_function_setreceivecallback.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_send</name>
    <title>IotNetworkInterface_t::send</title>
    <filename>platform_network_function_send.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_receive</name>
    <title>IotNetworkInterface_t::receive</title>
    <filename>platform_network_function_receive.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_receiveupto</name>
    <title>IotNetworkInterface_t::receiveUpto</title>
    <filename>platform_network_function_receiveupto.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_close</name>
    <title>IotNetworkInterface_t::close</title>
    <filename>platform_network_function_close.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_destroy</name>
    <title>IotNetworkInterface_t::destroy</title>
    <filename>platform_network_function_destroy.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_network_function_receivecallback</name>
    <title>IotNetworkReceiveCallback_t</title>
    <filename>platform_network_function_receivecallback.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_functions</name>
    <title>Thread Management</title>
    <filename>platform_threads_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_createdetachedthread</name>
    <title>Iot_CreateDetachedThread</title>
    <filename>platform_threads_function_createdetachedthread.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_mutexcreate</name>
    <title>IotMutex_Create</title>
    <filename>platform_threads_function_mutexcreate.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_mutexdestroy</name>
    <title>IotMutex_Destroy</title>
    <filename>platform_threads_function_mutexdestroy.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_mutexlock</name>
    <title>IotMutex_Lock</title>
    <filename>platform_threads_function_mutexlock.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_mutextrylock</name>
    <title>IotMutex_TryLock</title>
    <filename>platform_threads_function_mutextrylock.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_mutexunlock</name>
    <title>IotMutex_Unlock</title>
    <filename>platform_threads_function_mutexunlock.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphorecreate</name>
    <title>IotSemaphore_Create</title>
    <filename>platform_threads_function_semaphorecreate.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphoredestroy</name>
    <title>IotSemaphore_Destroy</title>
    <filename>platform_threads_function_semaphoredestroy.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphoregetcount</name>
    <title>IotSemaphore_GetCount</title>
    <filename>platform_threads_function_semaphoregetcount.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphorewait</name>
    <title>IotSemaphore_Wait</title>
    <filename>platform_threads_function_semaphorewait.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphoretrywait</name>
    <title>IotSemaphore_TryWait</title>
    <filename>platform_threads_function_semaphoretrywait.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphoretimedwait</name>
    <title>IotSemaphore_TimedWait</title>
    <filename>platform_threads_function_semaphoretimedwait.html</filename>
  </compound>
  <compound kind="page">
    <name>platform_threads_function_semaphorepost</name>
    <title>IotSemaphore_Post</title>
    <filename>platform_threads_function_semaphorepost.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
    <docanchor file="index.html">platform</docanchor>
  </compound>
</tagfile>
