<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20">
  <compound kind="file">
    <name>aws_iot_ota_agent.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/freertos_plus/aws/ota/include/</path>
    <filename>aws__iot__ota__agent_8h.html</filename>
    <class kind="struct">Sig256_t</class>
    <class kind="struct">OTA_FileContext_t</class>
    <class kind="struct">OTA_ConnectionContext_t</class>
    <class kind="struct">OTA_PAL_Callbacks_t</class>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DEBUG_LOG_LEVEL</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ae42219e07ba0882145d2709c7f0cbaf1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_MaxSignatureSize</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a374cf454c7a4f641bb31433c410c49b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_Panic</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a0966831bd017278016028ee58fa6c219</anchor>
      <arglist></arglist>
      <docanchor file="aws__iot__ota__agent_8h.html" title="OTA Error Codes">ota_constants_err_codes</docanchor>
      <docanchor file="aws__iot__ota__agent_8h.html" title="OTA Error Code Helper constants">ota_constants_err_code_helpers</docanchor>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_Uninitialized</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a30d050d86291f0dbdf7855fd2ae49a28</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_SignatureCheckFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a70b11a20ee91509f88c82e14716eb5d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_BadSignerCert</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a9983ebbc9ca5a06ea4c598f66f4e9144</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_OutOfMemory</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a5c23aaa410cd7d49be3dc4e00e35d688</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_ActivateFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a5c8245184ee9c1f547a31972133cf1e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_CommitFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a52cbfbef51475f7ac0f716b9c0062181</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_RejectFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a4347fe919df55a2184ca54735db30c8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_AbortFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>aa5aca7732329641e06a01d20e6d9eabd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_PublishFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>abbdd433e0380e2f19a9d9e5c4fce7e23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_BadImageState</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a7a5272fab4b57164ace5a4a3e972395b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_NoActiveJob</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af507b51c9d8c824c4a7a4ef7e4b2654f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_NoFreeContext</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a03927af21a4e744a47dd48747bc35f99</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_HTTPInitFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a2fcec3ec208ed22c88a15c72316ebae5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_HTTPRequestFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a2bf9f2f9fbd1392d34c38d969c28c30b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_FileAbort</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a0cfd8cca1b4c00540d705103bca5a1d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_FileClose</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a30428c44b3cfac58e5aa11c3b2b0282a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_RxFileCreateFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a221253b172af008fed6eab65b9fdf141</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_BootInfoCreateFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ad02b06dd9ca1ecb8a1dc8a2f441179e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_RxFileTooLarge</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a052fd3ba800b3cd9996a19e65132761a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_NullFilePtr</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a3b7a812eb903f22143b2212a60ccaec2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_MomentumAbort</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a93bafa645c97dd78b197a7ab29990b9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_DowngradeNotAllowed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>abf50fb4d540294eb67fa9f3115917a6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_SameFirmwareVersion</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>affd1de3c6529654735232bc497302ff8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_JobParserError</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af0ac18b5cf7cce0201c5eb6cb01f693c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_FailedToEncodeCBOR</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a6fde83b932d97968ac4e538e5af8b395</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_ImageStateMismatch</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af01b5250d8dd341cd6264ec94d8f6b6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_GenericIngestError</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a381f92adfa7c4706d6ac32157c5d6461</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_UserAbort</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a38f36c42abe006b3e4116ed4c5891992</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_ResetNotSupported</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a879e2508a9fbf027d2b59bb8fb3c52a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_TopicTooLarge</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a0fa51d591efef9519b7ad47df6081c23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_SelfTestTimerFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a7ffb66de118db08153a372d6d5b6709a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_EventQueueSendFailed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a0efecd73b4c9c9b03be112592717daa7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_InvalidDataProtocol</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ada6a950df3b87cad318980db1e402605</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Err_OTAAgentStopped</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a9b710dccfccb9130846fa1997a39b21d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_PAL_ErrMask</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a8bb4eeee0b63d81ab1567ea0bca8e8f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_Main_ErrMask</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75b7d5cf4b9551dcd6d16ed5dd71d94f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_MainErrShiftDownBits</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>accc670320701bc64f260e73c323b4bbf</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct OTA_FileContext</type>
      <name>OTA_FileContext_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>aae3b526aacc1a1df0db9889f8549c766</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>OTA_Err_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ad65b7bf247fac4ec69a7dcff1bf7999c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>pxOTACompleteCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a8c3cb025cb25e7e1f67dc7f72264f936</anchor>
      <arglist>)(OTA_JobEvent_t eEvent)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALAbortCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a7eb647cb5204924b67cfae42eba2a573</anchor>
      <arglist>)(OTA_FileContext_t *const C)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALActivateNewImageCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>aa30d350feccfb1d7a6158b64a4d18a11</anchor>
      <arglist>)(uint32_t ulServerFileID)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALCloseFileCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>afb98b4d65ec3f55de61ce2acaa59ae5e</anchor>
      <arglist>)(OTA_FileContext_t *const C)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALCreateFileForRxCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a1d7f3cce6af0838935a00b3370b2a66f</anchor>
      <arglist>)(OTA_FileContext_t *const C)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_PAL_ImageState_t(*</type>
      <name>pxOTAPALGetPlatformImageStateCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a37c984c8806c01476a0e65bd6987cd4c</anchor>
      <arglist>)(uint32_t ulServerFileID)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALResetDeviceCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a16f628e3ca83bd07f0caddb7fbf572f0</anchor>
      <arglist>)(uint32_t ulServerFileID)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_Err_t(*</type>
      <name>pxOTAPALSetPlatformImageStateCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a2717e17a80417033ec64ed5ef9bba771</anchor>
      <arglist>)(uint32_t ulServerFileID, OTA_ImageState_t eState)</arglist>
    </member>
    <member kind="typedef">
      <type>int16_t(*</type>
      <name>pxOTAPALWriteBlockCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a29f2b070fc6075865f14118b62c84dcd</anchor>
      <arglist>)(OTA_FileContext_t *const C, uint32_t iOffset, uint8_t *const pacData, uint32_t iBlockSize)</arglist>
    </member>
    <member kind="typedef">
      <type>OTA_JobParseErr_t(*</type>
      <name>pxOTACustomJobCallback_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a3ef44fff7c30dc32f6eed489b8035663</anchor>
      <arglist>)(const char *pcJSON, uint32_t ulMsgLen)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_State_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a8eddffdd56ba01c6bd0a3275241af74c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_Event_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a86d0f677f09590baf1e74a8731877975</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_PAL_ImageState_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af1eafdaf1ba8ba1b332784151dc51100</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_JobParseErr_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a57f25917832ea39b522c7c062c16615d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_JobEvent_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ab85403792655b68f99981c9229dfa16e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_JobEvent_Activate</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ab85403792655b68f99981c9229dfa16ea114fb550ca7ce7ca983c8b8b56246e33</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_JobEvent_Fail</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ab85403792655b68f99981c9229dfa16eaffe7acba7849e0a844e4a664321e1d6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_JobEvent_StartTest</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ab85403792655b68f99981c9229dfa16ea298b43f57160345ef78ad80a8a3acead</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OTA_ImageState_t</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_ImageState_Unknown</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25a6c218365f1c071ae4f6f657a7bca2e0a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_ImageState_Testing</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25a3eaede105fc88165c501ca09b04d5289</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_ImageState_Accepted</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25a12975ca3b62a523383dd62ab2cafb53d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_ImageState_Rejected</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25acad30f8b6288aec4686215dbaa74c21a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eOTA_ImageState_Aborted</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a75288843c3e7c89eb390525f67a66b25ad12abedcd867e5394040c6e3271e7bae</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>OTA_State_t</type>
      <name>OTA_AgentInit</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a277a883031dfa97680cab57847de7594</anchor>
      <arglist>(void *pvConnectionContext, const uint8_t *pucThingName, pxOTACompleteCallback_t xFunc, TickType_t xTicksToWait)</arglist>
    </member>
    <member kind="function">
      <type>OTA_State_t</type>
      <name>OTA_AgentInit_internal</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a7cae4cd9f827e84988771a08ced87a10</anchor>
      <arglist>(void *pvConnectionContext, const uint8_t *pucThingName, const OTA_PAL_Callbacks_t *pxCallbacks, TickType_t xTicksToWait)</arglist>
    </member>
    <member kind="function">
      <type>OTA_State_t</type>
      <name>OTA_AgentShutdown</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a34c0b607cc1356d51e4e4c8c25f4c050</anchor>
      <arglist>(TickType_t xTicksToWait)</arglist>
    </member>
    <member kind="function">
      <type>OTA_State_t</type>
      <name>OTA_GetAgentState</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ae15e4aa0f46a43e9282e5bc6ee70c2b9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OTA_Err_t</type>
      <name>OTA_ActivateNewImage</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af97ccf7aec8a7a1ceae33acab26a7cf3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OTA_Err_t</type>
      <name>OTA_SetImageState</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a91c2b0a1bbb0e17baf9269df8bd31184</anchor>
      <arglist>(OTA_ImageState_t eState)</arglist>
    </member>
    <member kind="function">
      <type>OTA_ImageState_t</type>
      <name>OTA_GetImageState</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>af5d6f4963c036ef7552361316aed2250</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OTA_Err_t</type>
      <name>OTA_CheckForUpdate</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a70d6dfbaee5d9428fffc91b1830f1cfd</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OTA_Err_t</type>
      <name>OTA_Suspend</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>ab83a4488ec87b1c1dfed84ba361a2afc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OTA_Err_t</type>
      <name>OTA_Resume</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a665434a1f53b700fea2fa82a50f5e926</anchor>
      <arglist>(void *pxConnection)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>OTA_GetPacketsReceived</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a5853484481fcf8ad51d9d2d8d214aa5d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>OTA_GetPacketsQueued</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>acccd0652d654306770eca5aa9b680b8d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>OTA_GetPacketsProcessed</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>a0a183b18b12df36666a9924b0956a1fa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>OTA_GetPacketsDropped</name>
      <anchorfile>aws__iot__ota__agent_8h.html</anchorfile>
      <anchor>aea2a07c3d269402c2fcb90ae918b56c3</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OTA_ConnectionContext_t</name>
    <filename>struct_o_t_a___connection_context__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>OTA_FileContext_t</name>
    <filename>struct_o_t_a___file_context__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucFilePath</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ae11887a29e694bae3a9322efc03cf5a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>lFileHandle</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ae676803bb1279bb203402964c1c1a289</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucFile</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a3cb58c6dc66d9e4ed795ed633fa7a2ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulFileSize</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ae3a66375e96de66b607f0e2ab53cb647</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulBlocksRemaining</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a12cfc4fa8a8be01a2dfa7e939ebf5853</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulFileAttributes</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>aad5f391e71fc62b7971753597db9f475</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulServerFileID</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ad3f1095807ecdbc0d251d418171a8f4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucJobName</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a64d33df0b42ff581652082dfd9265794</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucStreamName</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a63ec42041720374709dad58dee82a61e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Sig256_t *</type>
      <name>pxSignature</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a199f0206612e36e660247b666da08286</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucRxBlockBitmap</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>aaee50bbd302801e6befe676a1dafb381</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucCertFilepath</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a32e0b6811b00f1b866ddc76d89c53a71</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucUpdateUrlPath</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>af8be1935ec31d717aa5ac8640e76bbd2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucAuthScheme</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ab8056510dd2953ee6c25088b8bb51c37</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulUpdaterVersion</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a7af091d145f3fbb6b53e9ee1dc6e9f6d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>bIsInSelfTest</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>ae7607366e477618468a1a237450be7fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucProtocols</name>
      <anchorfile>struct_o_t_a___file_context__t.html</anchorfile>
      <anchor>a6e5bbdc7cafbd338201b5d2d295734c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OTA_PAL_Callbacks_t</name>
    <filename>struct_o_t_a___p_a_l___callbacks__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>Sig256_t</name>
    <filename>struct_sig256__t.html</filename>
  </compound>
</tagfile>
