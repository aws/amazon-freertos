<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>ota.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota_8c.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__config__defaults_8h" name="ota_config_defaults.h" local="yes" imported="no">ota_config_defaults.h</includes>
    <includes id="ota__base64__private_8h" name="ota_base64_private.h" local="yes" imported="no">ota_base64_private.h</includes>
    <includes id="ota__platform__interface_8h" name="ota_platform_interface.h" local="yes" imported="no">ota_platform_interface.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <includes id="ota__interface__private_8h" name="ota_interface_private.h" local="yes" imported="no">ota_interface_private.h</includes>
    <includes id="ota__os__interface_8h" name="ota_os_interface.h" local="yes" imported="no">ota_os_interface.h</includes>
    <includes id="ota__appversion32_8h" name="ota_appversion32.h" local="yes" imported="no">ota_appversion32.h</includes>
    <class kind="struct">OtaStateTableEntry_t</class>
    <member kind="define">
      <type>#define</type>
      <name>U16_OFFSET</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4342d17740cc60072d01f1bc9790bffa</anchor>
      <arglist>(type, member)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaErr_t(*</type>
      <name>OtaEventHandler_t</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ac4e61e364f1ac1e16bf9014a3de0d09c</anchor>
      <arglist>)(const OtaEventData_t *pEventMsg)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IngestResult_t</type>
      <name>ingestDataBlock</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a9419f57f0068e6a538664cfbd382832b</anchor>
      <arglist>(OtaFileContext_t *pFileContext, const uint8_t *pRawMsg, uint32_t messageSize, OtaPalStatus_t *pCloseResult)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IngestResult_t</type>
      <name>processDataBlock</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>acae48f24cd686ca6d3aeaccc27f48082</anchor>
      <arglist>(OtaFileContext_t *pFileContext, uint32_t uBlockIndex, uint32_t uBlockSize, OtaPalStatus_t *pCloseResult, uint8_t *pPayload)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IngestResult_t</type>
      <name>ingestDataBlockCleanup</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a8e1e2160e0b2595a1617448d68e827d0</anchor>
      <arglist>(OtaFileContext_t *pFileContext, OtaPalStatus_t *pCloseResult)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaFileContext_t *</type>
      <name>getFileContextFromJob</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4e44b0d336bfbe22e7bc6e3f86bbf008</anchor>
      <arglist>(const char *pRawMsg, uint32_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>validateJSON</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a5432c30dee29ec4906c2fba81da15a84</anchor>
      <arglist>(const char *pJson, uint32_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>extractParameter</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a26879a79af8d848224076acaaf31e588</anchor>
      <arglist>(JsonDocParam_t docParam, void *pContextBase, const char *pValueInJson, size_t valueLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>parseJSONbyModel</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>aa1e3a5b54edf70565e8b91bdd884e1eb</anchor>
      <arglist>(const char *pJson, uint32_t messageLength, JsonDocModel_t *pDocModel)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>decodeAndStoreKey</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a06fb03ec9d3fc5c9e4ce2826e533bf6d</anchor>
      <arglist>(const char *pValueInJson, size_t valueLength, void *pParamAdd)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>extractAndStoreArray</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a3b7846c523b892176de2118e822157a8</anchor>
      <arglist>(const char *pKey, const char *pValueInJson, size_t valueLength, void *pParamAdd, uint32_t *pParamSizeAdd)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>verifyRequiredParamsExtracted</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>aae8f6a684adf58acb5433aa77dca15db</anchor>
      <arglist>(const JsonDocParam_t *pModelParam, const JsonDocModel_t *pDocModel)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>validateUpdateVersion</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a38362bf3cd22684d5ed83748ab2f3adb</anchor>
      <arglist>(const OtaFileContext_t *pFileContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaJobParseErr_t</type>
      <name>handleCustomJob</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a932eec63e62b0c77076244ef6a3fb930</anchor>
      <arglist>(const char *pJson, uint32_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaJobParseErr_t</type>
      <name>verifyActiveJobStatus</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a9f24548cdb62b5aa567934a6b41701d8</anchor>
      <arglist>(OtaFileContext_t *pFileContext, OtaFileContext_t **pFinalFile, bool *pUpdateJob)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaJobParseErr_t</type>
      <name>validateAndStartJob</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>aee2078b96cf49831462a032bdd53e100</anchor>
      <arglist>(OtaFileContext_t *pFileContext, OtaFileContext_t **pFinalFile, bool *pUpdateJob)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaFileContext_t *</type>
      <name>parseJobDoc</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a86d467b6717607e7582dd916eb9fbbd4</anchor>
      <arglist>(const JsonDocParam_t *pJsonExpectedParams, uint16_t numJobParams, const char *pJson, uint32_t messageLength, bool *pUpdateJob)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>validateDataBlock</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a9e51070d847cd2ecf96fc8df58e0ad21</anchor>
      <arglist>(const OtaFileContext_t *pFileContext, uint32_t blockIndex, uint32_t blockSize)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IngestResult_t</type>
      <name>decodeAndStoreDataBlock</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a38c37b062ef279c31dba69c7ad8340ce</anchor>
      <arglist>(OtaFileContext_t *pFileContext, const uint8_t *pRawMsg, uint32_t messageSize, uint8_t **pPayload, uint32_t *pBlockSize, uint32_t *pBlockIndex)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>otaClose</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a87d0a2e81013e841561399a06c913eba</anchor>
      <arglist>(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>otaTimerCallback</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a8b8a4a176b27481784eefe1580a6624d</anchor>
      <arglist>(OtaTimerId_t otaTimerId)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>setImageStateWithReason</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a1bafe150ea1ae18a214ff1387906c03b</anchor>
      <arglist>(OtaImageState_t stateToSet, uint32_t reasonToSet)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>updateJobStatusFromImageState</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a596cf4427b32f3265556facc0f718815</anchor>
      <arglist>(OtaImageState_t state, int32_t subReason)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>agentShutdownCleanup</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>af50d949e3ccee6e464f3e062b1e83be7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>dataHandlerCleanup</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a1b5fcf884b2857014de968008924beb2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static DocParseErr_t</type>
      <name>initDocModel</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a2e0f5237102f7fd81bb2e162ca4cd0f0</anchor>
      <arglist>(JsonDocModel_t *pDocModel, const JsonDocParam_t *pBodyDef, void *contextBaseAddr, uint32_t contextSize, uint16_t numJobParams)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>initializeAppBuffers</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a0bafd2f7eb047f40f4ea7ac0e77f8af1</anchor>
      <arglist>(OtaAppBuffer_t *pOtaBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>initializeLocalBuffers</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a238a1d0b19c5ff32e5a9d1f4126dded5</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>searchTransition</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>af9ce02cafbc0554480802b965bc3ee0c</anchor>
      <arglist>(const OtaEventMsg_t *pEventMsg)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>processValidFileContext</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4334797dac98d572ae6ea0bdcf812209</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>handleSelfTestJobDoc</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a04a9f7e46088412b1186014bf4040813</anchor>
      <arglist>(OtaFileContext_t *pFileContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>processNullFileContext</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>aa0526ddce991d5566b6f209c60cafdef</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>platformInSelftest</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a7171071c7af3c2b241a76e1148188864</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>handleUnexpectedEvents</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ab10de29376ce3cb56f8a2e9ac38baa89</anchor>
      <arglist>(const OtaEventMsg_t *pEventMsg)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>freeFileContextMem</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ae466af59782893adbc54667423fe266f</anchor>
      <arglist>(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>handleJobParsingError</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a9d27d20bbbd359e673cff72b0565b8b0</anchor>
      <arglist>(const OtaFileContext_t *pFileContext, OtaJobParseErr_t err)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>receiveAndProcessOtaEvent</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ae751fbf1175cc0532e2c3c968854bfd2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>startHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>adc05572b247887937398695a7ae22428</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>requestJobHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>acfd737a3f8be41849474d97ddbe695a6</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>processJobHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a32593607464529ebfbf759706f26738e</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>inSelfTestHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>acad9250775186fe3986c5ccd72c8942d</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>initFileHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4d439e624b33eae921f10e515d029987</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>processDataHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a16eafd6185711b5016056603e0a42501</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>requestDataHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4c051ef9cabbefca475b1c63e31a2c2c</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>shutdownHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>adc27b49696f1562fd90a031d9d59c129</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>closeFileHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a858c721e7cd210b915a41d74c7e47d6f</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>userAbortHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a8cf09d8207b0c6a8f8aa118483a4aef0</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>suspendHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>abbda282e0291623cf680da11e58f0808</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>resumeHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>af0188eca0c85dcac5fed956f21d9207c</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaErr_t</type>
      <name>jobNotificationHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a59ee2cfa7669b761f5dfd4dbee7545f8</anchor>
      <arglist>(const OtaEventData_t *pEventData)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>executeHandler</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a497bd4e67afe70d3aafb00abc55979ae</anchor>
      <arglist>(uint32_t index, const OtaEventMsg_t *const pEventMsg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OTA_EventProcessingTask</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ab3a0cfdc8694a606a1d736b2f54fb113</anchor>
      <arglist>(void *pUnused)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_SignalEvent</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a2564144f284db077b8947ba58a6a72bb</anchor>
      <arglist>(const OtaEventMsg_t *const pEventMsg)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Init</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ae5cc1dcc13fbcb32230f552f48b24f03</anchor>
      <arglist>(OtaAppBuffer_t *pOtaBuffer, OtaInterfaces_t *pOtaInterfaces, const uint8_t *pThingName, OtaAppCallback_t OtaAppCallback)</arglist>
    </member>
    <member kind="function">
      <type>OtaState_t</type>
      <name>OTA_Shutdown</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ac779291eb93f4e0e6459816e60e13b09</anchor>
      <arglist>(uint32_t ticksToWait, uint8_t unsubscribeFlag)</arglist>
    </member>
    <member kind="function">
      <type>OtaState_t</type>
      <name>OTA_GetState</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a6db3f9cb417cb135cb0e68f5b5f2b11f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_GetStatistics</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a63182243ef3c18d5f36cd427b83a1a22</anchor>
      <arglist>(OtaAgentStatistics_t *pStatistics)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_CheckForUpdate</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a1178e8009eb05e6f55f6506b625c9fc2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_ActivateNewImage</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a5169ba09148e7f5668a90e776e712f8b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_SetImageState</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ab68cdf65934474e1f3d2cd1046314bea</anchor>
      <arglist>(OtaImageState_t state)</arglist>
    </member>
    <member kind="function">
      <type>OtaImageState_t</type>
      <name>OTA_GetImageState</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a9c5b25f9a7eff3ded8cdf088c2011742</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Suspend</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a65b61ae5dd477e8b2e6c88ea0473c62b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Resume</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ae9d40388ac87e4ac93288de37c98a138</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_Err_strerror</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a6f72911b8fe80f27bce42c3a36dca4b3</anchor>
      <arglist>(OtaErr_t err)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_JobParse_strerror</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a1d42efa1af7fa0ed92060a3b7e869648</anchor>
      <arglist>(OtaJobParseErr_t err)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_OsStatus_strerror</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ab5997df93a33e5f1260e7a16655a5cca</anchor>
      <arglist>(OtaOsStatus_t status)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_PalStatus_strerror</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ad9d9a19df9fdb8977762a45fe2848ba7</anchor>
      <arglist>(OtaPalMainStatus_t status)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static OtaControlInterface_t</type>
      <name>otaControlInterface</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a161d09693f72ddeeb6a9b407012f7d8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static OtaDataInterface_t</type>
      <name>otaDataInterface</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a4b6a0572316b231f66d2d2855701b0ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static OtaAgentContext_t</type>
      <name>otaAgent</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>af42d1da71846150bb2cdb2238fe861c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static OtaStateTableEntry_t</type>
      <name>otaTransitionTable</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a115d71f205e185e9b7031ba49de411da</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pOtaAgentStateStrings</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a35bd11f5108d7e9e4f97f147b12a8c92</anchor>
      <arglist>[OtaAgentStateAll+1]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pOtaEventStrings</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a427f6b2cde0e2fdd9136c273d0c8fae8</anchor>
      <arglist>[OtaAgentEventMax]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const JsonDocParam_t</type>
      <name>otaJobDocModelParamStructure</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>ad483f03f8d49725d5d5cfe9133e86854</anchor>
      <arglist>[OTA_NUM_JOB_PARAMS]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>pJobNameBuffer</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>aaf6b80a942717438ad5ee23466e62d01</anchor>
      <arglist>[OTA_JOB_ID_MAX_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>pProtocolBuffer</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a17a536dc3f752d78111477613ab0dca9</anchor>
      <arglist>[OTA_PROTOCOL_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static Sig256_t</type>
      <name>sig256Buffer</name>
      <anchorfile>ota_8c.html</anchorfile>
      <anchor>a6ae882f31f2728422eb3d256fbecd384</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota_8h.html</filename>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <includes id="ota__os__interface_8h" name="ota_os_interface.h" local="yes" imported="no">ota_os_interface.h</includes>
    <includes id="ota__mqtt__interface_8h" name="ota_mqtt_interface.h" local="yes" imported="no">ota_mqtt_interface.h</includes>
    <includes id="ota__http__interface_8h" name="ota_http_interface.h" local="yes" imported="no">ota_http_interface.h</includes>
    <includes id="ota__platform__interface_8h" name="ota_platform_interface.h" local="yes" imported="no">ota_platform_interface.h</includes>
    <class kind="struct">OtaJobDocument_t</class>
    <class kind="struct">OtaInterfaces_t</class>
    <class kind="struct">OtaAppBuffer_t</class>
    <class kind="struct">OtaAgentContext_t</class>
    <member kind="define">
      <type>#define</type>
      <name>CONST_STRLEN</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a2714f6afa56c8777b5e570a390a69a58</anchor>
      <arglist>(s)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_FILE_SIG_KEY_STR_MAX_LENGTH</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a8baee291088322fecd736ff0b03fb730</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>OtaAppCallback_t</name>
      <anchorfile>group__ota__callback__types.html</anchorfile>
      <anchor>ga3bedd7e278f89d555a32a7efa8023a28</anchor>
      <arglist>)(OtaJobEvent_t eEvent, const void *pData)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaErr_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga7ab3c74dc057383c56c6cb9aa6bf0b2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrNone</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0c644255eee375cb8e6bd9def0772e02</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUninitialized</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0c1dd94b8875896b0a9dd383ba239774</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrPanic</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da096a4af841716cf1ee881826d30b5406</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInvalidArg</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dae8b91d2bcdd38b2c79bc67122681d5b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrAgentStopped</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da2e43587b6c8d6abbdeed09232b8b9696</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrSignalEventFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da8de27454ca774732ca584803d1ab5888</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrRequestJobFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da494a5dcbd62a41249e5ae1c177b442cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInitFileTransferFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da750fde72ae523e47fa838cb781dba75f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrRequestFileBlockFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da6d94255e998ae49d916648bb94b7da84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrCleanupControlFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2daa7c1391bb361ec9857470b984cd31346</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrCleanupDataFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dae40f5144d7881fb9d8a7f403c44a3133</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUpdateJobStatusFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad4008e18cf533002f2f194a491f859e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrJobParserError</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da525ecca44a574d8917760f869853a6ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInvalidDataProtocol</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da06fb88bd9ddb49038227c63e15a83523</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrMomentumAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da48fcd14a5554b8c0e447403c059bc786</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrDowngradeNotAllowed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0e99db7052f9c7007083ecc3fc08604c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrSameFirmwareVersion</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad8a8f3c4637db28866f53fbc91fe6427</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrImageStateMismatch</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da38b24044cf02652dbb60efe38864e527</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrNoActiveJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da7dc1d1638189d537ce31ee3ce2d6d55e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUserAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dafbd1a370f69e3eb88eff25d4fe9facbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrFailedToEncodeCbor</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad57de41d65cd3bd5de28228883e83084</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrFailedToDecodeCbor</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da7fa31caf9e7a68df47ddbdb657f7f35d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrActivateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2daf5d3845d8b68e0c91cff60d2a38a2678</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga1cb476a5e0ee81fa486f605e64419dcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobParseErr_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gab8e370b46d0ae5d51879710d533a7314</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobEvent_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga96a2e1f5cfad783897e805196eee39a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventActivate</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7aaf02fd22119771d2980c7af439fa3280</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventFail</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a549e2b4c48aadc3c2f354647b8fd5728</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventStartTest</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a04e2c9b7143d34f63905cd052bf9763b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventProcessed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7abd5b42406265a448d9250890c0360c10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventSelfTestFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a26b36eee239cace8df37d0a9e5874212</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventParseCustomJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a8e314e0d83b5dcd572b08d9f205ab658</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventReceivedJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a5cf84ad545b4e47c7bc2fe36b4b83fdd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventUpdateComplete</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a2ea5dedb25b574c14f379ae6b530dfff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gabf6d6b1d2bca4c54f58bd622ca6242d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Init</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ae5cc1dcc13fbcb32230f552f48b24f03</anchor>
      <arglist>(OtaAppBuffer_t *pOtaBuffer, OtaInterfaces_t *pOtaInterfaces, const uint8_t *pThingName, OtaAppCallback_t OtaAppCallback)</arglist>
    </member>
    <member kind="function">
      <type>OtaState_t</type>
      <name>OTA_Shutdown</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ac779291eb93f4e0e6459816e60e13b09</anchor>
      <arglist>(uint32_t ticksToWait, uint8_t unsubscribeFlag)</arglist>
    </member>
    <member kind="function">
      <type>OtaState_t</type>
      <name>OTA_GetState</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a6db3f9cb417cb135cb0e68f5b5f2b11f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_ActivateNewImage</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a5169ba09148e7f5668a90e776e712f8b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_SetImageState</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ab68cdf65934474e1f3d2cd1046314bea</anchor>
      <arglist>(OtaImageState_t state)</arglist>
    </member>
    <member kind="function">
      <type>OtaImageState_t</type>
      <name>OTA_GetImageState</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a9c5b25f9a7eff3ded8cdf088c2011742</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_CheckForUpdate</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a1178e8009eb05e6f55f6506b625c9fc2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Suspend</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a65b61ae5dd477e8b2e6c88ea0473c62b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_Resume</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ae9d40388ac87e4ac93288de37c98a138</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>OTA_EventProcessingTask</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ab3a0cfdc8694a606a1d736b2f54fb113</anchor>
      <arglist>(void *pUnused)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_SignalEvent</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a2564144f284db077b8947ba58a6a72bb</anchor>
      <arglist>(const OtaEventMsg_t *const pEventMsg)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>OTA_GetStatistics</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a63182243ef3c18d5f36cd427b83a1a22</anchor>
      <arglist>(OtaAgentStatistics_t *pStatistics)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_Err_strerror</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a6f72911b8fe80f27bce42c3a36dca4b3</anchor>
      <arglist>(OtaErr_t err)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_JobParse_strerror</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a1d42efa1af7fa0ed92060a3b7e869648</anchor>
      <arglist>(OtaJobParseErr_t err)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_PalStatus_strerror</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ad9d9a19df9fdb8977762a45fe2848ba7</anchor>
      <arglist>(OtaPalMainStatus_t status)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_OsStatus_strerror</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>ab5997df93a33e5f1260e7a16655a5cca</anchor>
      <arglist>(OtaOsStatus_t status)</arglist>
    </member>
    <member kind="variable">
      <type>const char</type>
      <name>OTA_JsonFileSignatureKey</name>
      <anchorfile>ota_8h.html</anchorfile>
      <anchor>a3b34dcda67d1fd023ced9257eb00cdcb</anchor>
      <arglist>[OTA_FILE_SIG_KEY_STR_MAX_LENGTH]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_appversion32.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__appversion32_8h.html</filename>
    <class kind="struct">AppVersion32_t</class>
    <member kind="variable">
      <type>const AppVersion32_t</type>
      <name>appFirmwareVersion</name>
      <anchorfile>ota__appversion32_8h.html</anchorfile>
      <anchor>a368e45ac75fd6d315c0eb0ff6cb77a97</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_base64.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota__base64_8c.html</filename>
    <includes id="ota__base64__private_8h" name="ota_base64_private.h" local="yes" imported="no">ota_base64_private.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>NEWLINE</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a806511f4930171733227c99101dc0606</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>WHITESPACE</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>abfa0d183e035eb9705b5faec98294d0f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PADDING_SYMBOL</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a15b11bd3f54ab28f27732abb49687365</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NON_BASE64_INDEX</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a2e8507cabd05d65ba199e92b16772aa2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>VALID_BASE64_SYMBOL_INDEX_RANGE_MAX</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a6d12699999e54d62368510009f299a06</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SEXTET_SIZE</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a97b85b7f38b6169f648cb4d2b39130a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_NUM_BASE64_DATA</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a3c26bd7f35be32726494bb69ed4e0d60</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_EXPECTED_NUM_PADDING</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a7449bd7415c97034a57e913e36be5bed</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MIN_VALID_ENCODED_DATA_SIZE</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>ae381c52f0a96916029fdbf5faf5717d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_ONE_OCTET</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a7f8ffc8eb41b7717bf6baa7b52d492f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_TWO_OCTETS</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a19163fa405ab66be467d27160041e984</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_PADDING_WITH_TWO_SEXTETS</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a2e092a16d98b7fb75342caaa7e776296</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_PADDING_WITH_THREE_SEXTETS</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a10476733de6e465f23bd34c15a6e204f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SYMBOL_TO_INDEX_MAP_VALUE_UPPER_BOUND</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a0efa6e1881928e9ae47e84afb426d1d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BASE64_INDEX_VALUE_UPPER_BOUND</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>aeaa249d3e7a39f2bf639d5a3a676f27b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Base64Status_t</type>
      <name>preprocessBase64Index</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>af41f10337c1f0b9b696dbbe8b64f4546</anchor>
      <arglist>(uint8_t base64Index, int64_t *pNumPadding, int64_t *pNumWhitespace)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>updateBase64DecodingBuffer</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>ab884b58a1f532cc3944eae011bad5b76</anchor>
      <arglist>(const uint8_t base64Index, uint32_t *pBase64IndexBuffer, uint32_t *pNumDataInBuffer)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Base64Status_t</type>
      <name>decodeBase64IndexBuffer</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>aed7d641181bf7fa31fef12ca85fb5c33</anchor>
      <arglist>(uint32_t *pBase64IndexBuffer, uint32_t *pNumDataInBuffer, uint8_t *pDest, const size_t destLen, size_t *pOutputLen)</arglist>
    </member>
    <member kind="function">
      <type>Base64Status_t</type>
      <name>base64Decode</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>a94a9217b253f00b4c3e22132ef564393</anchor>
      <arglist>(uint8_t *pDest, const size_t destLen, size_t *pResultLen, const uint8_t *pEncodedData, const size_t encodedLen)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const uint8_t</type>
      <name>pBase64SymbolToIndexMap</name>
      <anchorfile>ota__base64_8c.html</anchorfile>
      <anchor>acd877cf168910f36a44a3040ff485ecd</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_base64_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__base64__private_8h.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Base64Status_t</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64Success</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3aa64f53586a7254b2b83fa1f1ff384600</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64InvalidSymbol</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3a0dd64af5b53fa164f7cd1d090eb8bbe4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64InvalidSymbolOrdering</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3ac817da9e553c5cc3d93ffe41ff7eb079</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64InvalidInputSize</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3a41dd573653f68e8a80cd68926f5e2fb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64NullPointerInput</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3a280706453d8b443e5726b69919028cc4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64InvalidBufferSize</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3af0372a809f58979a3cd2481b3b346ae5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64NonZeroPadding</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3ae81fab72d798446c4fa13cdce4f22437</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Base64InvalidPaddingSymbol</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>ad654d9148176cc1d7b054228c54bfca3a72a37beaf5de6649ed412996f9912722</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>Base64Status_t</type>
      <name>base64Decode</name>
      <anchorfile>ota__base64__private_8h.html</anchorfile>
      <anchor>a94a9217b253f00b4c3e22132ef564393</anchor>
      <arglist>(uint8_t *pDest, const size_t destLen, size_t *pResultLen, const uint8_t *pEncodedData, const size_t encodedLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_cbor.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota__cbor_8c.html</filename>
    <includes id="ota__cbor__private_8h" name="ota_cbor_private.h" local="yes" imported="no">ota_cbor_private.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_GETSTREAMREQUEST_ITEM_COUNT</name>
      <anchorfile>ota__cbor_8c.html</anchorfile>
      <anchor>af6ddd33ad69268d0b2fd8285af1a4e88</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CborError</type>
      <name>checkDataType</name>
      <anchorfile>ota__cbor_8c.html</anchorfile>
      <anchor>ade7c5559b60e2e7472336f29c44f7e2b</anchor>
      <arglist>(CborType expectedType, CborValue *cborValue)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_CBOR_Decode_GetStreamResponseMessage</name>
      <anchorfile>ota__cbor_8c.html</anchorfile>
      <anchor>a479c08472cc49b7b813d7a1b91d744a5</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_CBOR_Encode_GetStreamRequestMessage</name>
      <anchorfile>ota__cbor_8c.html</anchorfile>
      <anchor>aa163d0fa47b92895b9292a400bd89448</anchor>
      <arglist>(uint8_t *pMessageBuffer, size_t messageBufferSize, size_t *pEncodedMessageSize, const char *pClientToken, int32_t fileId, int32_t blockSize, int32_t blockOffset, uint8_t *pBlockBitmap, size_t blockBitmapSize, int32_t numOfBlocksRequested)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_cbor_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__cbor__private_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_CLIENTTOKEN_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a822dc9ac642a2a54e4298371da35de2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_FILEID_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a88bd07947e9e1d3682dda80b3f189b26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_BLOCKSIZE_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a420c092d59682f4d1dbbb0791b151301</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_BLOCKOFFSET_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a31a4d111612cf98bbcc5fd064b057a57</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_BLOCKBITMAP_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a00bd9b05e580f660424d99239cb65ac1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_STREAMDESCRIPTION_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a57ce6fdfab3ae3568ade8de6e41b59cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_STREAMFILES_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>ae1a72f8890e00accb25abe1ab59f17a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_FILESIZE_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>addd7143e9a429b6e688a19188da27d17</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_BLOCKID_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>aa1cc92139de0ccb03c3009102f7d6b1d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_BLOCKPAYLOAD_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a61fe6e62a38c30cc370db183aaa27106</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CBOR_NUMBEROFBLOCKS_KEY</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a7f2cb71d485996fa170b69efb31a0266</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_CBOR_Decode_GetStreamResponseMessage</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>a479c08472cc49b7b813d7a1b91d744a5</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>OTA_CBOR_Encode_GetStreamRequestMessage</name>
      <anchorfile>ota__cbor__private_8h.html</anchorfile>
      <anchor>aa163d0fa47b92895b9292a400bd89448</anchor>
      <arglist>(uint8_t *pMessageBuffer, size_t messageBufferSize, size_t *pEncodedMessageSize, const char *pClientToken, int32_t fileId, int32_t blockSize, int32_t blockOffset, uint8_t *pBlockBitmap, size_t blockBitmapSize, int32_t numOfBlocksRequested)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_config_defaults.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DO_NOT_USE_CUSTOM_CONFIG</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a946fe151bf744627fdafcff15b7bb318</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigSTACK_SIZE</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a1c40af496bc98a8dd73eac8366e13317</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigAGENT_PRIORITY</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a766d34dee20cbc36e1b440838f3adecb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigLOG2_FILE_BLOCK_SIZE</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a65facf4428a072324825abebb2b0e550</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigSELF_TEST_RESPONSE_WAIT_MS</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a68e7932e806aed9025cb815b1cbf2104</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigFILE_REQUEST_WAIT_MS</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a19a42bab9dafd45b4c85709e55137534</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigMAX_THINGNAME_LEN</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a5f9d4bbcc883e6ecff39f478a929df35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigMAX_NUM_BLOCKS_REQUEST</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>aac052a7234601140e882f0075f196554</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigMAX_NUM_REQUEST_MOMENTUM</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>ae3ba450609211d1e00f1c540ca889fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigOTA_UPDATE_STATUS_FREQUENCY</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a5b7b57d36faeae2c3ceacb16f497a108</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigMAX_NUM_OTA_DATA_BUFFERS</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a169ad4ac7245555d858f10d4df42be25</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>otaconfigAllowDowngrade</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a95dac9f2da0f3bd64c2183caa6292efe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>configOTA_FIRMWARE_UPDATE_FILE_TYPE_ID</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a6bb41cb18b72bfc7ef2046c25af3ab0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>configENABLED_CONTROL_PROTOCOL</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a20fed391548ba26a1bfd7fbfd362e6f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>configENABLED_DATA_PROTOCOLS</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>afc4d1763ec6f9abaacdace650252e7cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>configOTA_PRIMARY_DATA_PROTOCOL</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a96092c27429db9fcabd02c0c6a15756a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogError</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a8d9dbaaa88129137a4c68ba0456a18b1</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogWarn</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a7da92048aaf0cbfcacde9539c98a0e05</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogInfo</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>a00810b1cb9d2f25d25ce2d4d93815fba</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogDebug</name>
      <anchorfile>ota__config__defaults_8h.html</anchorfile>
      <anchor>af60e8ffc327d136e5d0d8441ed98c98d</anchor>
      <arglist>(message)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_http.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota__http_8c.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <includes id="ota__http__private_8h" name="ota_http_private.h" local="yes" imported="no">ota_http_private.h</includes>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>initFileTransfer_Http</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>ad8086a6e3c6ccf6b543fe7a375f824ba</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestDataBlock_Http</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>ad501dd9d4704c39272a1e7ae3ec1f217</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>decodeFileBlock_Http</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>aef5ca8e71ac452d251db5fef2e8dab84</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupData_Http</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>a11b07a9ef9722b5a062da46301e5e80c</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_HTTP_strerror</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>ae2e32e7b73de637586f718b66adac6d7</anchor>
      <arglist>(OtaHttpStatus_t status)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint32_t</type>
      <name>currBlock</name>
      <anchorfile>ota__http_8c.html</anchorfile>
      <anchor>a8c8624373c6a37dddbdd1ccf473084fb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_http_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__http__interface_8h.html</filename>
    <class kind="struct">OtaHttpInterface_t</class>
    <member kind="typedef">
      <type>OtaHttpStatus_t(*</type>
      <name>OtaHttpInit_t</name>
      <anchorfile>ota__http__interface_8h.html</anchorfile>
      <anchor>a16941a151ce9cdeab9138ac4cd9ea61c</anchor>
      <arglist>)(char *pUrl)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaHttpStatus_t(*</type>
      <name>OtaHttpRequest_t</name>
      <anchorfile>ota__http__interface_8h.html</anchorfile>
      <anchor>a0973339ed000f27f66b74c9261fce608</anchor>
      <arglist>)(uint32_t rangeStart, uint32_t rangeEnd)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaHttpStatus_t(*</type>
      <name>OtaHttpDeinit</name>
      <anchorfile>ota__http__interface_8h.html</anchorfile>
      <anchor>a99b8a3a434250dd127013ea48ececbd1</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaHttpStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga5d1a57074f3242737a756c3feb64d4d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4ac8837a3a448f8dc701d838974a2a6535</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpInitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a0d123a670a1f800a8f94b6e79d66ec49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpDeinitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a5e02f99f4a8e6020017668ebb6743ef9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpRequestFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a49f043cf87a8fefc6d7f383acc730dd5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_http_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__http__private_8h.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>initFileTransfer_Http</name>
      <anchorfile>ota__http__private_8h.html</anchorfile>
      <anchor>ad8086a6e3c6ccf6b543fe7a375f824ba</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestDataBlock_Http</name>
      <anchorfile>ota__http__private_8h.html</anchorfile>
      <anchor>ad501dd9d4704c39272a1e7ae3ec1f217</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>decodeFileBlock_Http</name>
      <anchorfile>ota__http__private_8h.html</anchorfile>
      <anchor>aef5ca8e71ac452d251db5fef2e8dab84</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupData_Http</name>
      <anchorfile>ota__http__private_8h.html</anchorfile>
      <anchor>a11b07a9ef9722b5a062da46301e5e80c</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_HTTP_strerror</name>
      <anchorfile>ota__http__private_8h.html</anchorfile>
      <anchor>ae2e32e7b73de637586f718b66adac6d7</anchor>
      <arglist>(OtaHttpStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_interface.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota__interface_8c.html</filename>
    <includes id="ota__interface__private_8h" name="ota_interface_private.h" local="yes" imported="no">ota_interface_private.h</includes>
    <member kind="function">
      <type>void</type>
      <name>setControlInterface</name>
      <anchorfile>ota__interface_8c.html</anchorfile>
      <anchor>a9466ce63c950d5f52cfdc22e3d5da719</anchor>
      <arglist>(OtaControlInterface_t *pControlInterface)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>setDataInterface</name>
      <anchorfile>ota__interface_8c.html</anchorfile>
      <anchor>a4a9279d8f03ad25704095d86160b6fd2</anchor>
      <arglist>(OtaDataInterface_t *pDataInterface, const uint8_t *pProtocol)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_interface_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__interface__private_8h.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <class kind="struct">OtaControlInterface_t</class>
    <class kind="struct">OtaDataInterface_t</class>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CONTROL_OVER_MQTT</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>a0f42a3e269464777303e990eaab32c65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DATA_OVER_MQTT</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>a21af8723c90fd4c4b899f76b51d23048</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DATA_OVER_HTTP</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>a2c0435fbfdf4e7e7c53ce8415db21ba2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DATA_NUM_PROTOCOLS</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>ae0261d8110a2b8853f25f0fa6dcc91bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setControlInterface</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>a9466ce63c950d5f52cfdc22e3d5da719</anchor>
      <arglist>(OtaControlInterface_t *pControlInterface)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>setDataInterface</name>
      <anchorfile>ota__interface__private_8h.html</anchorfile>
      <anchor>a4a9279d8f03ad25704095d86160b6fd2</anchor>
      <arglist>(OtaDataInterface_t *pDataInterface, const uint8_t *pProtocol)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_mqtt.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/</path>
    <filename>ota__mqtt_8c.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <includes id="ota__cbor__private_8h" name="ota_cbor_private.h" local="yes" imported="no">ota_cbor_private.h</includes>
    <includes id="ota__mqtt__private_8h" name="ota_mqtt_private.h" local="yes" imported="no">ota_mqtt_private.h</includes>
    <includes id="ota__appversion32_8h" name="ota_appversion32.h" local="yes" imported="no">ota_appversion32.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>OTA_CLIENT_TOKEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a43ee155a18e8bdb84648246627a52b95</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_STATUS_MSG_MAX_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a202644e333fe75c092efb34e46041024</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_THINGS</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aa098f692be289771870d2a7e87f08ade</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_JOBS_NEXT_GET</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ad4304c68ff182ae305bd1aef729cc649</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_JOBS_NOTIFY_NEXT</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a882ab8f412b7eafd62a4d30fa490215b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_JOBS</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a4ed0bc9a47fd867889c03aba6f173e76</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_UPDATE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a732c7c7d5bd3f591be7ed3ccaffc2ab6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_STREAMS</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a1bad29e0fa86933b6b5b7d1ae3e08d87</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_DATA_CBOR</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>afc69d7ad9b9f430a5415cc2c1e78f52c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MQTT_API_GET_CBOR</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a0af69ba9cd753388d7375ab5819af40d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_STATUS_IN_PROGRESS</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a33538872ddc4e588809f5e3589b1ca80</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_STATUS_FAILED</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a593d7e0360443075b28b79abdf8365f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_STATUS_SUCCEEDED</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a4e66ea77d83acb7c0de30cb628bafdb8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_STATUS_REJECTED</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aa856801466e59ece1094805a5664d80d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>U32_MAX_LEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a25ffab2259b4cc083e3c63f26d4f1f3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOB_NAME_MAX_LEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a06950b4fe27f046b19b36a94b6c8b759</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>STREAM_NAME_MAX_LEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>af85a24df201760600c421734be5b6b43</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NULL_CHAR_LEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a45f0275084cbf3026c6f0e8ca78d2011</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_PLUS_THINGNAME_LEN</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a31795da29bdaa9216955b56fa6d695aa</anchor>
      <arglist>(topic)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_GET_NEXT_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ae3a2a0e59418ef4abe47138b620aeb95</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_NOTIFY_NEXT_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a60067dc77fe7c068f74448564406eabc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_JOB_STATUS_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a258e2253dfcaaeb11ae338990b9f8f9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_STREAM_DATA_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a98df5e30bd64d854c055c4777681d7a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TOPIC_GET_STREAM_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>abff52aba6234bddab939c8fd6bd23d7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MSG_GET_NEXT_BUFFER_SIZE</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a46d3e2c694c79c58da14d865547545e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaMqttStatus_t</type>
      <name>subscribeToJobNotificationTopics</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a0e9fe160d3b5c2fa82f64dec9e7e2223</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaMqttStatus_t</type>
      <name>unsubscribeFromDataStream</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>af9ca52029b721890b54a073e37a05f55</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaMqttStatus_t</type>
      <name>unsubscribeFromJobNotificationTopic</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a3298a475bc64c592f658dedc132b475a</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OtaMqttStatus_t</type>
      <name>publishStatusMessage</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aa4f7d02282e60e0700df63cc4028ccab</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx, const char *pMsg, uint32_t msgSize, uint8_t qos)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>buildStatusMessageReceiving</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a0b21d0737bee094525b5a274a663d4c8</anchor>
      <arglist>(char *pMsgBuffer, size_t msgBufferSize, OtaJobStatus_t status, const OtaFileContext_t *pOTAFileCtx)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvBuildStatusMessageSelfTest</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a15053eb434a9a80cfe501ff2332a3811</anchor>
      <arglist>(char *pMsgBuffer, size_t msgBufferSize, OtaJobStatus_t status, int32_t reason)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>prvBuildStatusMessageFinish</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ab3ceb06564cb82c84fa06c060507add7</anchor>
      <arglist>(char *pMsgBuffer, size_t msgBufferSize, OtaJobStatus_t status, int32_t reason, int32_t subReason)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>stringBuilder</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aa2617f45133753d96b1f17f1f0dc8959</anchor>
      <arglist>(char *pBuffer, size_t bufferSizeBytes, const char *strings[])</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>stringBuilderUInt32Decimal</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a971b8c6e86d4bb4eab60cff88ae36c4e</anchor>
      <arglist>(char *pBuffer, size_t bufferSizeBytes, uint32_t value)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>stringBuilderUInt32Hex</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aff80c9f62bb98981e03bf88954324114</anchor>
      <arglist>(char *pBuffer, size_t bufferSizeBytes, uint32_t value)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestJob_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ae5d088697ab942fa1d7a88d83959bb75</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>updateJobStatus_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a16e4bdb664fe1ee97bf8ac6995b13dbb</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx, OtaJobStatus_t status, int32_t reason, int32_t subReason)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>initFileTransfer_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a8a60c3bf3dc91de2a9c34264ecfd4333</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestFileBlock_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ad9130a843fa7901fc0d59bd630b678bb</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>decodeFileBlock_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>af41c26c85d8a3b123dda8db825bba299</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupControl_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ac8a6105f2306eac0fc6e935fa8a4f895</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupData_Mqtt</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a410d9ba569c75d746e3009cac78df37a</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_MQTT_strerror</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ab62e959f2a60e315dd9baede311f521a</anchor>
      <arglist>(OtaMqttStatus_t status)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaJobsGetNextTopicTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>aeb5d3cc4e2941cb9196f217e73ceb89c</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaJobsNotifyNextTopicTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a8cb462e0a75ba04dfff8e0fd7523b472</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaJobStatusTopicTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a8c4451f9d873373c70853a05815860f1</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaStreamDataTopicTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a09aad5d53684a352368091451a4428fe</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaGetStreamTopicTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ac02c2b09d852a17138aace423ee73ac5</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaGetNextJobMsgTemplate</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ae97dec2af5c1d024f58c0e7248412217</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>pOtaStringReceive</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a9efcf18557f2542e2a47c0ffb8f02977</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pOtaJobStatusStrings</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>ad255a94505b8ab394079face8f89c390</anchor>
      <arglist>[NumJobStatusMappings]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char *</type>
      <name>pOtaJobReasonStrings</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>a43d49ac4984c6f0b73ffec22e20bfc0c</anchor>
      <arglist>[NumJobReasons]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const char</type>
      <name>asciiDigits</name>
      <anchorfile>ota__mqtt_8c.html</anchorfile>
      <anchor>abd5cfe0c9088a5193b93ffb1196fac3d</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_mqtt_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__mqtt__interface_8h.html</filename>
    <class kind="struct">OtaMqttInterface_t</class>
    <member kind="typedef">
      <type>OtaMqttStatus_t(*</type>
      <name>OtaMqttSubscribe_t</name>
      <anchorfile>ota__mqtt__interface_8h.html</anchorfile>
      <anchor>a9bf7833baeda3004bda9dadbd724422d</anchor>
      <arglist>)(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaMqttStatus_t(*</type>
      <name>OtaMqttUnsubscribe_t</name>
      <anchorfile>ota__mqtt__interface_8h.html</anchorfile>
      <anchor>a15308f09763f630bb0d0c1ab085908a3</anchor>
      <arglist>)(const char *pTopicFilter, uint16_t topicFilterLength, uint8_t ucQoS)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaMqttStatus_t(*</type>
      <name>OtaMqttPublish_t</name>
      <anchorfile>ota__mqtt__interface_8h.html</anchorfile>
      <anchor>a72aee623a6c21a7dc5b65dcf2cd73e23</anchor>
      <arglist>)(const char *const pacTopic, uint16_t usTopicLen, const char *pcMsg, uint32_t ulMsgSize, uint8_t ucQoS)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaMqttStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gaf32d40000cc4b46c0144fd673d6f24fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faab9ea57c4e9581fe81f32bb922e285ecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttPublishFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaf3c73c49216574f658702ea7c6fc6817</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttSubscribeFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaf675c3f005a4b554e8ac534981b66621</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttUnsubscribeFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaccb3e6812907c643ca68360ee6221bec</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_mqtt_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__mqtt__private_8h.html</filename>
    <includes id="ota_8h" name="ota.h" local="yes" imported="no">ota.h</includes>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestJob_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>ae5d088697ab942fa1d7a88d83959bb75</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>initFileTransfer_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>a8a60c3bf3dc91de2a9c34264ecfd4333</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>requestFileBlock_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>ad9130a843fa7901fc0d59bd630b678bb</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>decodeFileBlock_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>af41c26c85d8a3b123dda8db825bba299</anchor>
      <arglist>(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupControl_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>ac8a6105f2306eac0fc6e935fa8a4f895</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>cleanupData_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>a410d9ba569c75d746e3009cac78df37a</anchor>
      <arglist>(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="function">
      <type>OtaErr_t</type>
      <name>updateJobStatus_Mqtt</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>a16e4bdb664fe1ee97bf8ac6995b13dbb</anchor>
      <arglist>(OtaAgentContext_t *pAgentCtx, OtaJobStatus_t status, int32_t reason, int32_t subReason)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>OTA_MQTT_strerror</name>
      <anchorfile>ota__mqtt__private_8h.html</anchorfile>
      <anchor>ab62e959f2a60e315dd9baede311f521a</anchor>
      <arglist>(OtaMqttStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_os_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__os__interface_8h.html</filename>
    <class kind="struct">OtaEventInterface_t</class>
    <class kind="struct">OtaTimerInterface_t</class>
    <class kind="struct">OtaMallocInterface_t</class>
    <class kind="struct">OtaOSInterface_t</class>
    <member kind="typedef">
      <type>struct OtaEventContext</type>
      <name>OtaEventContext_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>a6821b4bd16ffc4216eca86683292ced0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaInitEvent_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>afa5686e7e0e73e5a3624d40eb0f22eac</anchor>
      <arglist>)(OtaEventContext_t *pEventCtx)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaSendEvent_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>ace6738f520b1d4fa9d519c87f309b3cb</anchor>
      <arglist>)(OtaEventContext_t *pEventCtx, const void *pEventMsg, unsigned int timeout)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaReceiveEvent_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>a2da38685efe0f2f791869b85cd6f65d0</anchor>
      <arglist>)(OtaEventContext_t *pEventCtx, void *pEventMsg, uint32_t timeout)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaDeinitEvent_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>ae4adb797e9209683a9ad221d3b4b1b73</anchor>
      <arglist>)(OtaEventContext_t *pEventCtx)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>OtaTimerCallback_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>acad5454ec7c7c67361d3a1578a07739c</anchor>
      <arglist>)(OtaTimerId_t otaTimerId)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaStartTimer_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>a96169f3b2158c7abc0d9ad2fa6a1ecbd</anchor>
      <arglist>)(OtaTimerId_t otaTimerId, const char *const pTimerName, const uint32_t timeout, OtaTimerCallback_t callback)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaStopTimer_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>aa56b962c5ff5ca9d5d09a41c8648eb9e</anchor>
      <arglist>)(OtaTimerId_t otaTimerId)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaOsStatus_t(*</type>
      <name>OtaDeleteTimer_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>acd5f9df58723bb05d34449962cb5ee48</anchor>
      <arglist>)(OtaTimerId_t otaTimerId)</arglist>
    </member>
    <member kind="typedef">
      <type>void *(*</type>
      <name>OtaMalloc_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>abb0c2255832a56e2427a54c150f3fb66</anchor>
      <arglist>)(size_t size)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>OtaFree_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>a3fa30179be8407cd1d198eae7e589f6e</anchor>
      <arglist>)(void *ptr)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaTimerId_t</name>
      <anchorfile>ota__os__interface_8h.html</anchorfile>
      <anchor>afb4112b0dbe23a668f5b69e56ffea085</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaOsStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gaececff6c0dd8ba4d884544571bd368e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7ad153172731315817688544ceff3eac50</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a93602f665d93e66ca2208d45219d975e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueSendFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a0d04ffe18a839e1d2b4ab049b8993462</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueReceiveFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a61507cc0e92c4dba4ce6b1e70f9b24fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueDeleteFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7add0eff4efdf80fa2796713bd7ffab78b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a5ed5da8e8321e3b89c11a279c884dc39</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerStartFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7ad9178092f091e650fdeab75ac7fa16f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerRestartFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7aade705ebfa991b59d6bd66523a9324d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerStopFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a760aadce2bd9363c4973720eec02b63f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerDeleteFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a22a0a901794cea938b8402a3d5c6499c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_platform_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__platform__interface_8h.html</filename>
    <includes id="ota__private_8h" name="ota_private.h" local="yes" imported="no">ota_private.h</includes>
    <class kind="struct">OtaPalInterface_t</class>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PAL_ERR_MASK</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>af86d69b12a8373d4843e693187c66c02</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PAL_SUB_BITS</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>adca6a50131b3abc3d910f4c7f39ce332</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PAL_MAIN_ERR</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>ab25e30871d1c2fc609552591d883c87e</anchor>
      <arglist>(err)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PAL_SUB_ERR</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a93e53a52e858cad1adc7231d8d6ff769</anchor>
      <arglist>(err)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PAL_COMBINE_ERR</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a0684a42fae0de7395399d8b4d248fb35</anchor>
      <arglist>(main, sub)</arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>OtaPalStatus_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a503f74c089364b482321578b541d7640</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t</type>
      <name>OtaPalSubStatus_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a4910712f01795de72f368d4614dd8561</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalAbort_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>aadc190c689aeeb5a611abc7297de5745</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalCreateFileForRx_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a5808e620bd4831bc60017b292dc92d05</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalCloseFile_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>ac71b2c3dd59681d851aa597be054060f</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="typedef">
      <type>int16_t(*</type>
      <name>OtaPalWriteBlock_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a481db6e2616fad63ca98d35d1eb29a91</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext, uint32_t offset, uint8_t *const pData, uint32_t blockSize)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalActivateNewImage_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a0a57a461aaba81de1345fb3412d5293c</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalResetDevice_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a1493f1fe9489073e7d615746d24fa2f0</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalStatus_t(*</type>
      <name>OtaPalSetPlatformImageState_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>a424ab827af6f0a9a1173f1b91af29917</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext, OtaImageState_t eState)</arglist>
    </member>
    <member kind="typedef">
      <type>OtaPalImageState_t(*</type>
      <name>OtaPalGetPlatformImageState_t</name>
      <anchorfile>ota__platform__interface_8h.html</anchorfile>
      <anchor>ab74a45be05c7a6fae3dc057394d07e3a</anchor>
      <arglist>)(OtaFileContext_t *const pFileContext)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaPalMainStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga8e503bbbffc277f6fd5b8645767a685b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba8dbb21a8a52503c7e8f33835dc6687c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalUninitialized</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba386aef27e1d9c7a937d3698e559641e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalOutOfMemory</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba0ec8f96ac056d352c45db250b4f6e05d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalNullFileContext</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba088a029115f9b8053b9c23eb50438a1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalSignatureCheckFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba3a92c1315048b471740472eaa91cd39b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRxFileCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba15af1448f649ae32235d23aaab93b90d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRxFileTooLarge</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf945ac941bd969b1833f80847d286805</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBootInfoCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba38e979079368805430af70c0daa35005</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBadSignerCert</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba7335f166465b3c6537093140445fdfef</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBadImageState</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba8022263fa243f4fedb05692f5816b709</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalAbortFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf4431964d1c516dde35c2b37c829b195</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRejectFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba0beb58eb2e069e80e51e3c4857549d14</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalCommitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf5d88629edcda34a8883985972e52126</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalActivateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba6e8ffdf4071a5ef58fdd3e3bdc1253de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalFileAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba233a2055c5de2e07759f3e39fa88f8cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalFileClose</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685bab9b6262d3d153e77d186f6df07eb3e6f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ota_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/ota-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>ota__private_8h.html</filename>
    <includes id="ota__config__defaults_8h" name="ota_config_defaults.h" local="yes" imported="no">ota_config_defaults.h</includes>
    <class kind="struct">JsonDocParam_t</class>
    <class kind="struct">JsonDocModel_t</class>
    <class kind="struct">OtaAgentStatistics_t</class>
    <class kind="struct">Sig256_t</class>
    <class kind="struct">OtaFileContext_t</class>
    <class kind="struct">OtaEventData_t</class>
    <class kind="struct">OtaEventMsg_t</class>
    <member kind="define">
      <type>#define</type>
      <name>LOG2_BITS_PER_BYTE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga9794a22e433da1780d89dc408f225192</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BITS_PER_BYTE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga369ecd38b3ab077fc235f892354bb46f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_FILE_BLOCK_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac70b23d17d31a065c30283c4fcc5ab8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_FILES</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga4cd0bbaca7882b52913ca82504dfaf86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_BLOCK_BITMAP_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga538d92a689f2b60c60bcef41e2739a1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_REQUEST_MSG_MAX_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga9b81d4cb7dbd60b668da6dc2dea072c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_REQUEST_URL_MAX_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gab4104ae99ac82ab47978b046e6e26e6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_ERASED_BLOCKS_VAL</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga759a079ce6746a1d1f7e6391565bbe4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_NUM_MSG_Q_ENTRIES</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga76a5e62449824989e82600736cc65c45</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_JSON_TOKENS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga059be218ff71c8df0eccda0a7115bfad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_JSON_STR_LEN</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gafaa370948b99101757c117fd5f85abc1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DOC_MODEL_MAX_PARAMS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gabe907910e02fd0d9ac522cc48c39e54d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JOB_PARAM_REQUIRED</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga716173ece0b83fdc783798bdf2cf5715</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JOB_PARAM_OPTIONAL</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gab4f3b8ca5434b9b02ae4244da5cd2078</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DONT_STORE_PARAM</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac462962d86d18bfe1fab0ca0527ea9ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_STORE_NESTED_JSON</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga62f786dfa9c243d52d490aedf55b74a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DATA_BLOCK_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga2333ad9c3ebbf1c5956e163cf1ff7998</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_JOB_MSG_READY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaa9b83cfe52a6ccb7df183fe92b73b7de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_DATA_MSG_READY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga77def055cb745579b01ef6372f848a84</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_SHUTDOWN</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga3bf06ac14931655f3f7956a519472b64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_REQ_TIMEOUT</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga808ed33f5edc75b564ae8ea8827bafa3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_USER_ABORT</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaffd25dc92e8b9ddbfab667dfcc79cbe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_ALL_EVENTS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga030724739b0df6d0b355893d4fd732e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_NUM_JOB_PARAMS</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a77c1dadee30b8e0beaea17fb8563e218</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JOB_ID_MAX_SIZE</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ab6942359a343a9a65279eec716cc4468</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_PROTOCOL_BUFFER_SIZE</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a2b311ebd30ad900edc6cfc2219366a22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_MaxSignatureSize</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga374cf454c7a4f641bb31433c410c49b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SEPARATOR</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gae53b17849f410c6ccf5b8862293fc302</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_CLIENT_TOKEN_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga3d69df69fbdff6811da80020c8afc61a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_TIMESTAMP_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga625051ce4de218406aad6dd0955f1c73</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_EXECUTION_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga4d831064bb7ea8bae1df86e1b77eaab6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_JOB_ID_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga44138af02a7ed130ff630788cbaf1c27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_STATUS_DETAILS_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga6a6b266874ad73e248d840643616325c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SELF_TEST_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaf7c64cc7a2f1a8141d00f0e33f83c1d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATED_BY_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaffd03defb729ae990baede2a0c704076</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATED_BY_KEY_ONLY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaed6d19287cf525cb7c5dcf50798c6cd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SELF_TEST_KEY_ONLY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga71be1d5c1e4a59d0627b7c83e64a5124</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_JOB_DOC_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac0132213a7beb808d8cf4b86a3ab6b86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_OTA_UNIT_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga1c56c6acb4d197a362143b670aeaac75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_PROTOCOLS_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaacb7d18662a1212605eede7c875016ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_GROUP_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga85efaf15e103da089f3de398948f46d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_STREAM_NAME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga1801081d3c11b83ce214949c54156587</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_PATH_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga209cd55d10743b96b2792ed8ae12fa51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_SIZE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gadbef300d5ee8db5704a53dd50f9babc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_ID_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac0ddd548347a6aa9e001eb3383ddeafe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_ATTRIBUTE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga098cfb263c324b93dee1e98917312ffd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_CERT_NAME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga6e5f882dc07e8a5a8b2f079cc4444a50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATE_DATA_URL_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gad3c584828dcc7d99d65931d0c1ded6b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_AUTH_SCHEME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaafc3e865308a16ec82181d6aea75d6bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILETYPE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaab7b7332590b19bcc767a290953c58a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IngestResult_t</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultFileComplete</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91dafd2b3919afdda669abc466f3e21a2f8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultSigCheckFail</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91da6851ed011462089d1e34c1849362d16d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultFileCloseFail</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91da149b4eb79d99914e4813ffeb6fd55d3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultNullInput</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91dae9e3e3c82c09a3aea117b897f91f40b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultBadFileHandle</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91daa513cc8bd2bf828f62b4431d6ff39f49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultUnexpectedBlock</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91da2e87c516779990608259dbc5b9143512</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultBlockOutOfRange</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91dafbb2d9ebcabfdb39b1dbe1d2976d82f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultBadData</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91dab25b021cbb3b2c1187f08863e1783313</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultWriteBlockFailed</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91daaf2b849dfea35e098aa297d1049bee43</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultNoDecodeMemory</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91da0a3bc7065269e3b2e14c4e1977662883</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultUninitialized</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91dadf717aab5a220c795405558a53dc6a96</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultAccepted_Continue</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91daa862035c4739541925592cdc04e8017d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IngestResultDuplicate_Continue</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a9d3eb1ee8e8a129c990fe9effd93f91da44dc594af083c46a71cf769cce92d61e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>DocParseErr_t</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrUnknown</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecac7cc7639b100a09d35f11c8165a8131e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrNone</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecaf42569b8440b06a29f3b7600e3d4eee8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrOutOfMemory</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca70e9f66cea472df88fc9e2ed8a4312a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrUserBufferInsuffcient</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca94a365911296b23442e5a2be1cb7c530</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrFieldTypeMismatch</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecacbe73852077d22f1555df2d5404ad965</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrBase64Decode</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecab6e0ae8a51c1f7bd5d37b977b7bc57d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrInvalidNumChar</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca2e625dd9fd24409d2bc5741e896974a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrDuplicatesNotAllowed</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca3a91f45846a2f6a0f8d552a2e9acc383</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrMalformedDoc</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca065d9f1ce07a8e9c2ff0eac78c9af578</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErr_InvalidJSONBuffer</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecaedf443469e154903ae6f5c5ad55ca3b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrNullModelPointer</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca9cc8b4d64f2350f985f476f54c3e305a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrNullBodyPointer</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca109b6bed28ce1428d8e02996da3049d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrNullDocPointer</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca5944dfc63ba11a347091780e57cdfa86</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrTooManyParams</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecaf655a0d5a94d161ff50886d442b7f035</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrParamKeyNotInModel</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca0c472f25591b09af6db35b5bca2237c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrInvalidModelParamType</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679eca13fadbeea324b6a609309a851bf4aa26</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>DocParseErrInvalidToken</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>ae7f5ab8221031b60eb2704cf840679ecaebec61efddc009993f2cc2d1b4bfea37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ModelParamType_t</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a15d02bfa1c580245061d4b7b5f30168a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobReason_t</name>
      <anchorfile>ota__private_8h.html</anchorfile>
      <anchor>a69404606813f1e6d6c7ba5abd674f5e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaImageState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gad4909faa8b9c8672e7f7bef1bc6b5b84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateUnknown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84afb051298f888e18cfeb7fd1b1986cb9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateTesting</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a7193a6a56fbd5578d87dfe46c785d420</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateAccepted</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84ad9ee33052c9c36593243b1bd0329a396</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateRejected</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a667999c3c223ac934ec3393520abd94a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateAborted</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a9d52d6fd99c42230d77ef9e40ac57b03</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaPalImageState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gab1f386c3bcc707ff32033659a0a4ddca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateUnknown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaad4cb7e3e6e8445f5f38860339439f31c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStatePendingCommit</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaafcc2daaa65a18953e4e51ebda28b7c59</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateValid</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaaf3d8a6cc3a9b662e480be1908caa0e1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateInvalid</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaa0074b4f18792780ed448586aaf1c2b7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaEvent_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga2c42c25235304ca5c3e3a6abef78be4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventStart</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eac965db79301fabd43971f2ad050d782c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventStartSelfTest</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eaeab0f855ccec790d94ac1e295256526e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestJobDocument</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea50831130508ed232d604aee5cf8999e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventReceivedJobDocument</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea593ee44d9ab623f9c9010603fcb7bca3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventCreateFile</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea9ff4b58dab8e17eca36163959ac02395</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestFileBlock</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead88894cbefd4b61cc2049e495e52cad1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventReceivedFileBlock</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea79ef0fd4aaffd57dd5af6ef646a70f11</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestTimer</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead424034a3d0a53c719c3dfcf110aedbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventCloseFile</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eacb6862078f37cb276c7c4429a16593fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventSuspend</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead2c2503cc33bf8309c28618897f8fcdd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventResume</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eaf1e3cb59f2e70908e121c218c27c1fe9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventUserAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea92146829f7a9aaccefc0d0d96daef50b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventShutdown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead02ce67dfb7d99810979ec659813f89b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventMax</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea668f452f3c81c0b76d09921893c7027c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>AppVersion32_t</name>
    <filename>struct_app_version32__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>build</name>
      <anchorfile>struct_app_version32__t.html</anchorfile>
      <anchor>a9e4251725bdc4abe2904e72bd10dbdc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>minor</name>
      <anchorfile>struct_app_version32__t.html</anchorfile>
      <anchor>a37fbe1709903b16f29aa79c0cbaa3380</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>major</name>
      <anchorfile>struct_app_version32__t.html</anchorfile>
      <anchor>abc623f9deacb56ff0a7dc1bbfa2d300a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct AppVersion32_t::@0::version</type>
      <name>x</name>
      <anchorfile>struct_app_version32__t.html</anchorfile>
      <anchor>ad916509bde38999fa0589b4460d66849</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union AppVersion32_t::@0</type>
      <name>u</name>
      <anchorfile>struct_app_version32__t.html</anchorfile>
      <anchor>a6bfe56ccca1a15c43140129761a4e22d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>JsonDocModel_t</name>
    <filename>struct_json_doc_model__t.html</filename>
    <member kind="variable">
      <type>void *</type>
      <name>contextBase</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a4b0418206c1d1aef360e5808cef93238</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>contextSize</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a7b8f3e89336902ebb234e0c65232001f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const JsonDocParam_t *</type>
      <name>pBodyDef</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a721647583d181988fdc5a40e95e87258</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>numModelParams</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a20a5e41a28a003345863cd6009830663</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>paramsReceivedBitmap</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a2244cbc61a874d88eb3e1a0dfae7ad01</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>paramsRequiredBitmap</name>
      <anchorfile>struct_json_doc_model__t.html</anchorfile>
      <anchor>a81c1b25cd117cea8ca780351d0fcb037</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>JsonDocParam_t</name>
    <filename>struct_json_doc_param__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pSrcKey</name>
      <anchorfile>struct_json_doc_param__t.html</anchorfile>
      <anchor>adf590486686c77a328e34ed2d6cd0e24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const bool</type>
      <name>required</name>
      <anchorfile>struct_json_doc_param__t.html</anchorfile>
      <anchor>a96709fba52845d5f5ad914c89db05989</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>pDestOffset</name>
      <anchorfile>struct_json_doc_param__t.html</anchorfile>
      <anchor>a82ab56ea82dd5d99bfb1224dcd70e898</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>pDestSizeOffset</name>
      <anchorfile>struct_json_doc_param__t.html</anchorfile>
      <anchor>aa6a954422b324818b6cad8ac866e40aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const ModelParamType_t</type>
      <name>modelParamType</name>
      <anchorfile>struct_json_doc_param__t.html</anchorfile>
      <anchor>aec93e5b397121785b9ccffe9d5a47daf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaAgentContext_t</name>
    <filename>struct_ota_agent_context__t.html</filename>
    <member kind="variable">
      <type>OtaState_t</type>
      <name>state</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a61a065c01ec6c93e1f957b6ee0b8465b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>pThingName</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>ab65c1d54c43a832265e1c106e091f384</anchor>
      <arglist>[otaconfigMAX_THINGNAME_LEN+1U]</arglist>
    </member>
    <member kind="variable">
      <type>OtaFileContext_t</type>
      <name>fileContext</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>adc641377ae4e07a5c056a851c0c1f113</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>fileIndex</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>aaedb31086304fd6cc93bcd03de9dbf24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>serverFileID</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>ad80a0e3d45f727bbc3dfd41e512a7622</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>pActiveJobName</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>aaab4a5711c7857fe9558ff052861e00c</anchor>
      <arglist>[OTA_JOB_ID_MAX_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pClientTokenFromJob</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a34d18fd7e4557aa45fe67d867b07e995</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>timestampFromJob</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a51deb609fd3c159213398d17cd677b72</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaImageState_t</type>
      <name>imageState</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a802de005e5206788c454854204768b5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>numOfBlocksToReceive</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a11f9ac6a4d87c3c05135f238f589797a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaAgentStatistics_t</type>
      <name>statistics</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a694d9d5b1f6d1073d81d143ff758ae09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>requestMomentum</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>abcb47ce949d9c9874ba53c08df390665</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaInterfaces_t *</type>
      <name>pOtaInterface</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>a06a0aaedbf13f965619a9e2208663011</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaAppCallback_t</type>
      <name>OtaAppCallback</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>ac8aad7846a2fc1a15fa35e8f9a2053ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>unsubscribeOnShutdown</name>
      <anchorfile>struct_ota_agent_context__t.html</anchorfile>
      <anchor>ab86e0efb9552ea4d9962e78d1feeb0f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaAgentStatistics_t</name>
    <filename>struct_ota_agent_statistics__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>otaPacketsReceived</name>
      <anchorfile>struct_ota_agent_statistics__t.html</anchorfile>
      <anchor>a43f7d9b2c6ead8213c6de51d6e65e980</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>otaPacketsQueued</name>
      <anchorfile>struct_ota_agent_statistics__t.html</anchorfile>
      <anchor>ab693dddf7707daf3315bc4c23bb4e29a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>otaPacketsProcessed</name>
      <anchorfile>struct_ota_agent_statistics__t.html</anchorfile>
      <anchor>adbcf157f4d998bea3bf3c508e37c8437</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>otaPacketsDropped</name>
      <anchorfile>struct_ota_agent_statistics__t.html</anchorfile>
      <anchor>ae644353e00d4a3770bd6400181c5785e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaAppBuffer_t</name>
    <filename>struct_ota_app_buffer__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pUpdateFilePath</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a1e8e0cf1e6b1f699409f22a4c03cefd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>updateFilePathsize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a4260f4d420eeb703cf3af5dc85dcd70d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pCertFilePath</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a64d0ac6d3c7d66193582640fd2d023d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>certFilePathSize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a5f08814fbd5e55f88e23677a4da8b732</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pStreamName</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a7bc6e9af2e5935ddf7168173bba51805</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>streamNameSize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>ad27ecad9649243f52d2067ec4c9f9849</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pDecodeMemory</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>aa9456f212031228ff3a0a04307430802</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>decodeMemorySize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a4cf8c691d69b5f951bb902d780fca1a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pFileBitmap</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a6d2a5ecba3800a44bf58223ea6785ddd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>fileBitmapSize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>ac9d07e3e4d6978bf9dee34eb3a330cbc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pUrl</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a493c9ece89e377df53c2a7e2089704ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>urlSize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a2b6bd46660f5f45859d36cfa330c8c90</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pAuthScheme</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a45d7da0e73376d5b2bf019f9c3eb3196</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>authSchemeSize</name>
      <anchorfile>struct_ota_app_buffer__t.html</anchorfile>
      <anchor>a0c474b181da044b9a06ee5da9d1b714d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaControlInterface_t</name>
    <filename>struct_ota_control_interface__t.html</filename>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>requestJob</name>
      <anchorfile>struct_ota_control_interface__t.html</anchorfile>
      <anchor>a277b7f1a53afc5e22b592b15aead09a0</anchor>
      <arglist>)(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>updateJobStatus</name>
      <anchorfile>struct_ota_control_interface__t.html</anchorfile>
      <anchor>aeb0edc908788d34c69f8bfc44d3a6545</anchor>
      <arglist>)(OtaAgentContext_t *pAgentCtx, OtaJobStatus_t status, int32_t reason, int32_t subReason)</arglist>
    </member>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>cleanup</name>
      <anchorfile>struct_ota_control_interface__t.html</anchorfile>
      <anchor>a8c05533f9db8e6dee6f58670545a0a3b</anchor>
      <arglist>)(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaDataInterface_t</name>
    <filename>struct_ota_data_interface__t.html</filename>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>initFileTransfer</name>
      <anchorfile>struct_ota_data_interface__t.html</anchorfile>
      <anchor>a7ba5722fab17bf6132e4f8fcfa2ebf5d</anchor>
      <arglist>)(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>requestFileBlock</name>
      <anchorfile>struct_ota_data_interface__t.html</anchorfile>
      <anchor>a8decde920215b9fbe9be7d405be10b68</anchor>
      <arglist>)(OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>decodeFileBlock</name>
      <anchorfile>struct_ota_data_interface__t.html</anchorfile>
      <anchor>aff7932cf2c665dc3f9b47356501c79f5</anchor>
      <arglist>)(const uint8_t *pMessageBuffer, size_t messageSize, int32_t *pFileId, int32_t *pBlockId, int32_t *pBlockSize, uint8_t **pPayload, size_t *pPayloadSize)</arglist>
    </member>
    <member kind="variable">
      <type>OtaErr_t(*</type>
      <name>cleanup</name>
      <anchorfile>struct_ota_data_interface__t.html</anchorfile>
      <anchor>a76c6df1455e8f58d4116c584a3388213</anchor>
      <arglist>)(const OtaAgentContext_t *pAgentCtx)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaEventData_t</name>
    <filename>struct_ota_event_data__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>struct_ota_event_data__t.html</anchorfile>
      <anchor>a2d6745b10e3e5ad0dac2ebb89a56f252</anchor>
      <arglist>[OTA_DATA_BLOCK_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>dataLength</name>
      <anchorfile>struct_ota_event_data__t.html</anchorfile>
      <anchor>a4f1b1b9d316c41d97fd9169dc6ff076d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>bufferUsed</name>
      <anchorfile>struct_ota_event_data__t.html</anchorfile>
      <anchor>ac9ddb6e90ae5fc1cc7cb804195535621</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaEventInterface_t</name>
    <filename>struct_ota_event_interface__t.html</filename>
    <member kind="variable">
      <type>OtaInitEvent_t</type>
      <name>init</name>
      <anchorfile>struct_ota_event_interface__t.html</anchorfile>
      <anchor>a70a33a62828b128cd625c1b13ff8550e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaSendEvent_t</type>
      <name>send</name>
      <anchorfile>struct_ota_event_interface__t.html</anchorfile>
      <anchor>a02285842a33d7f21990422fdeabc39eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaReceiveEvent_t</type>
      <name>recv</name>
      <anchorfile>struct_ota_event_interface__t.html</anchorfile>
      <anchor>acd46139762310587bb44d8b23b58c8fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaDeinitEvent_t</type>
      <name>deinit</name>
      <anchorfile>struct_ota_event_interface__t.html</anchorfile>
      <anchor>a0bc56e4a2b5c991272239045e75363d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaEventContext_t *</type>
      <name>pEventContext</name>
      <anchorfile>struct_ota_event_interface__t.html</anchorfile>
      <anchor>a6f6184ea615ef2e26093adcb136f8176</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaEventMsg_t</name>
    <filename>struct_ota_event_msg__t.html</filename>
    <member kind="variable">
      <type>OtaEventData_t *</type>
      <name>pEventData</name>
      <anchorfile>struct_ota_event_msg__t.html</anchorfile>
      <anchor>a3244289f279458a9bf6d8a1cedd0f27a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaEvent_t</type>
      <name>eventId</name>
      <anchorfile>struct_ota_event_msg__t.html</anchorfile>
      <anchor>a065656c53757f9f78f5ddfc5b25e77f8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaFileContext_t</name>
    <filename>struct_ota_file_context__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pFilePath</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a7281da77a81626910373a56efcf90262</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>filePathMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a011bd760168a0265d0912bfb9ae5b35a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pFile</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a731eefc516423588396a17c63cc159b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>fileSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a26d56a7c06c041a73e569ca53e71ac67</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>blocksRemaining</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a23ad6a417000fca03982f5dd67bae94f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>fileAttributes</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a1eb0744188e34521d45faf9f740043e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>serverFileID</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>ab972bcc57370ba912390035067c39a7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pJobName</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>af9fcbcb6f6948b6d72498457fd08c88d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>jobNameMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a14886bb26167aad01e7496fdb9473bde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pStreamName</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a5bfc26add016d1883c2373a63c2bf5b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>streamNameMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>acafd3a6210232929ca52beec8cef5e08</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pRxBlockBitmap</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a1876125c2893ff2639fdcebfe47aa073</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>blockBitmapMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a49d365eb241b8f22e341f9cf9c88f121</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pCertFilepath</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a7dafcbda219cc62527b3f697192f1d0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>certFilePathMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>ad973ad2e69778a929d962c7b773006a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pUpdateUrlPath</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a66b88f4b4d9ce8f7d40c5d306bc588c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>updateUrlMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a7dfc9c9989cbe538b78bc007748ecccb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pAuthScheme</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>ab314724b6cfbda0dd95899803ee0d514</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>authSchemeMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a67c5ddb9341a368e755b6ca386ea5d6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>updaterVersion</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a732b6021dc1856d54fea38588f65957d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isInSelfTest</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a4bed61a2b0553f841bbaca9abd0a1560</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pProtocols</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>af1c7d4797e2ae02f8f022bcaad229cb3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>protocolMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>ae0306a16a35e855c2cd4d60fcf940813</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pDecodeMem</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>acd7020697cf5a208cac8da603af33d1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>decodeMemMaxSize</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a14058c044993f0fe95a406b072204495</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>fileType</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a3b880b7e8fcb4fee11607ac8bd454915</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Sig256_t *</type>
      <name>pSignature</name>
      <anchorfile>struct_ota_file_context__t.html</anchorfile>
      <anchor>a7b19507fb15f4924d985ad0a48c671f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaHttpInterface_t</name>
    <filename>struct_ota_http_interface__t.html</filename>
    <member kind="variable">
      <type>OtaHttpInit_t</type>
      <name>init</name>
      <anchorfile>struct_ota_http_interface__t.html</anchorfile>
      <anchor>a9e6e01808eb7dcd83ea3562aeda6ce67</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaHttpRequest_t</type>
      <name>request</name>
      <anchorfile>struct_ota_http_interface__t.html</anchorfile>
      <anchor>a87e4e6ebc94538da49d7cfe24cefab1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaHttpDeinit</type>
      <name>deinit</name>
      <anchorfile>struct_ota_http_interface__t.html</anchorfile>
      <anchor>a71b991f269a830332759d07341dc540f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaInterfaces_t</name>
    <filename>struct_ota_interfaces__t.html</filename>
    <member kind="variable">
      <type>OtaOSInterface_t</type>
      <name>os</name>
      <anchorfile>struct_ota_interfaces__t.html</anchorfile>
      <anchor>a4309938e333d120eb646b1d24b5f413a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaMqttInterface_t</type>
      <name>mqtt</name>
      <anchorfile>struct_ota_interfaces__t.html</anchorfile>
      <anchor>a231d847d025ae94578f68d5ae8ff3e66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaHttpInterface_t</type>
      <name>http</name>
      <anchorfile>struct_ota_interfaces__t.html</anchorfile>
      <anchor>a76df425905b6e53f43148e03a4c1daee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalInterface_t</type>
      <name>pal</name>
      <anchorfile>struct_ota_interfaces__t.html</anchorfile>
      <anchor>a0c434ef971121cc7c64970cdb8606e2f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaJobDocument_t</name>
    <filename>struct_ota_job_document__t.html</filename>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pJobDocJson</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>a76f27be43afe47387d01b8a25fdb2555</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>jobDocLength</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>afdd32b88f641c9d5eac48115ecdade2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pJobId</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>ad5531b7d3f6c2e7f3560c7db13ad2951</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>jobIdLength</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>a3c4394a5170ade0003b2abcdfef78ac3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>fileTypeId</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>a684a4b443b542fc72c65318a2ed86b92</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaJobParseErr_t</type>
      <name>parseErr</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>afbc69ad19342769a7a24c4d8d03a935a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaJobStatus_t</type>
      <name>status</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>a82ca1e15150a3c9a8472845b48c7b525</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>reason</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>a5d8e1a7e8f8c32efebeaba6a119c98bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>subReason</name>
      <anchorfile>struct_ota_job_document__t.html</anchorfile>
      <anchor>ad149a314fb8e362f97d6d7f9e876cbcb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaMallocInterface_t</name>
    <filename>struct_ota_malloc_interface__t.html</filename>
    <member kind="variable">
      <type>OtaMalloc_t</type>
      <name>malloc</name>
      <anchorfile>struct_ota_malloc_interface__t.html</anchorfile>
      <anchor>a80caab4ab7b1486faaec6dc30b50a31f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaFree_t</type>
      <name>free</name>
      <anchorfile>struct_ota_malloc_interface__t.html</anchorfile>
      <anchor>a3de64b7780a2c70f182d4a64911dc476</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaMqttInterface_t</name>
    <filename>struct_ota_mqtt_interface__t.html</filename>
    <member kind="variable">
      <type>OtaMqttSubscribe_t</type>
      <name>subscribe</name>
      <anchorfile>struct_ota_mqtt_interface__t.html</anchorfile>
      <anchor>adf1c4fbd39dc489e5f446a163ed57d65</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaMqttUnsubscribe_t</type>
      <name>unsubscribe</name>
      <anchorfile>struct_ota_mqtt_interface__t.html</anchorfile>
      <anchor>afe7b58ce23961bd3023266e9abb26a85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaMqttPublish_t</type>
      <name>publish</name>
      <anchorfile>struct_ota_mqtt_interface__t.html</anchorfile>
      <anchor>aab31f752ef9b005cb5f06226fdce7540</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaOSInterface_t</name>
    <filename>struct_ota_o_s_interface__t.html</filename>
    <member kind="variable">
      <type>OtaEventInterface_t</type>
      <name>event</name>
      <anchorfile>struct_ota_o_s_interface__t.html</anchorfile>
      <anchor>a56b4e42ad807bb4f407f61cca2964016</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaTimerInterface_t</type>
      <name>timer</name>
      <anchorfile>struct_ota_o_s_interface__t.html</anchorfile>
      <anchor>a7ed5a92311ab933578d208b808780024</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaMallocInterface_t</type>
      <name>mem</name>
      <anchorfile>struct_ota_o_s_interface__t.html</anchorfile>
      <anchor>a383da1c5ad16c5d6668b4feeb9fc347f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaPalInterface_t</name>
    <filename>struct_ota_pal_interface__t.html</filename>
    <member kind="variable">
      <type>OtaPalAbort_t</type>
      <name>abort</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>abf9e21e27883ba652e88551cac464f49</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalCreateFileForRx_t</type>
      <name>createFile</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>ad46b44c6ea0e60206d7d2766bf340f91</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalCloseFile_t</type>
      <name>closeFile</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>aed51ccb56c34bcb8d7981b50e1230eef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalWriteBlock_t</type>
      <name>writeBlock</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>a6661bd9229a8191a3f758b77544f6d24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalActivateNewImage_t</type>
      <name>activate</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>ae13ed8c9101ce7a69210b1cc0d32befe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalResetDevice_t</type>
      <name>reset</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>a744b9602243baf92cbe2d1e8aef3b7ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalSetPlatformImageState_t</type>
      <name>setPlatformImageState</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>a4042675d1df2bcc0fca26a9121fe5668</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaPalGetPlatformImageState_t</type>
      <name>getPlatformImageState</name>
      <anchorfile>struct_ota_pal_interface__t.html</anchorfile>
      <anchor>ace5e6d57a59896ca51b41c685989dd13</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaStateTableEntry_t</name>
    <filename>struct_ota_state_table_entry__t.html</filename>
    <member kind="variable">
      <type>OtaState_t</type>
      <name>currentState</name>
      <anchorfile>struct_ota_state_table_entry__t.html</anchorfile>
      <anchor>a91baf25b705476a91a9d664b6d94465a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaEvent_t</type>
      <name>eventId</name>
      <anchorfile>struct_ota_state_table_entry__t.html</anchorfile>
      <anchor>a56b0815573f92a4d59328e382cadfcb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaEventHandler_t</type>
      <name>handler</name>
      <anchorfile>struct_ota_state_table_entry__t.html</anchorfile>
      <anchor>aa78d6811ea53b957f71fb034260acb7a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaState_t</type>
      <name>nextState</name>
      <anchorfile>struct_ota_state_table_entry__t.html</anchorfile>
      <anchor>a3e2c917274bd35b1fb7cab0f6d9427e9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OtaTimerInterface_t</name>
    <filename>struct_ota_timer_interface__t.html</filename>
    <member kind="variable">
      <type>OtaStartTimer_t</type>
      <name>startTimer</name>
      <anchorfile>struct_ota_timer_interface__t.html</anchorfile>
      <anchor>af07286f84fd61f5199380cba20b6d017</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaStopTimer_t</type>
      <name>stopTimer</name>
      <anchorfile>struct_ota_timer_interface__t.html</anchorfile>
      <anchor>ae4df58287c2c4320cd82054cfd060a73</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>OtaDeleteTimer_t</type>
      <name>deleteTimer</name>
      <anchorfile>struct_ota_timer_interface__t.html</anchorfile>
      <anchor>a687a7dd5b26160cff192e09e247405b2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Sig256_t</name>
    <filename>struct_sig256__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>size</name>
      <anchorfile>struct_sig256__t.html</anchorfile>
      <anchor>a03d5d9cc35524d46b65eeb6287e75ea3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>struct_sig256__t.html</anchorfile>
      <anchor>aae366e61733f67f99979cdd998fc8e01</anchor>
      <arglist>[kOTA_MaxSignatureSize]</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ota_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__ota__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>OtaErr_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga7ab3c74dc057383c56c6cb9aa6bf0b2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrNone</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0c644255eee375cb8e6bd9def0772e02</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUninitialized</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0c1dd94b8875896b0a9dd383ba239774</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrPanic</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da096a4af841716cf1ee881826d30b5406</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInvalidArg</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dae8b91d2bcdd38b2c79bc67122681d5b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrAgentStopped</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da2e43587b6c8d6abbdeed09232b8b9696</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrSignalEventFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da8de27454ca774732ca584803d1ab5888</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrRequestJobFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da494a5dcbd62a41249e5ae1c177b442cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInitFileTransferFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da750fde72ae523e47fa838cb781dba75f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrRequestFileBlockFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da6d94255e998ae49d916648bb94b7da84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrCleanupControlFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2daa7c1391bb361ec9857470b984cd31346</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrCleanupDataFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dae40f5144d7881fb9d8a7f403c44a3133</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUpdateJobStatusFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad4008e18cf533002f2f194a491f859e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrJobParserError</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da525ecca44a574d8917760f869853a6ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrInvalidDataProtocol</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da06fb88bd9ddb49038227c63e15a83523</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrMomentumAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da48fcd14a5554b8c0e447403c059bc786</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrDowngradeNotAllowed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da0e99db7052f9c7007083ecc3fc08604c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrSameFirmwareVersion</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad8a8f3c4637db28866f53fbc91fe6427</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrImageStateMismatch</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da38b24044cf02652dbb60efe38864e527</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrNoActiveJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da7dc1d1638189d537ce31ee3ce2d6d55e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrUserAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dafbd1a370f69e3eb88eff25d4fe9facbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrFailedToEncodeCbor</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2dad57de41d65cd3bd5de28228883e83084</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrFailedToDecodeCbor</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2da7fa31caf9e7a68df47ddbdb657f7f35d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaErrActivateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga7ab3c74dc057383c56c6cb9aa6bf0b2daf5d3845d8b68e0c91cff60d2a38a2678</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga1cb476a5e0ee81fa486f605e64419dcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobParseErr_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gab8e370b46d0ae5d51879710d533a7314</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobEvent_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga96a2e1f5cfad783897e805196eee39a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventActivate</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7aaf02fd22119771d2980c7af439fa3280</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventFail</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a549e2b4c48aadc3c2f354647b8fd5728</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventStartTest</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a04e2c9b7143d34f63905cd052bf9763b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventProcessed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7abd5b42406265a448d9250890c0360c10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventSelfTestFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a26b36eee239cace8df37d0a9e5874212</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventParseCustomJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a8e314e0d83b5dcd572b08d9f205ab658</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventReceivedJob</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a5cf84ad545b4e47c7bc2fe36b4b83fdd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaJobEventUpdateComplete</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga96a2e1f5cfad783897e805196eee39a7a2ea5dedb25b574c14f379ae6b530dfff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaJobStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gabf6d6b1d2bca4c54f58bd622ca6242d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaHttpStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga5d1a57074f3242737a756c3feb64d4d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4ac8837a3a448f8dc701d838974a2a6535</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpInitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a0d123a670a1f800a8f94b6e79d66ec49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpDeinitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a5e02f99f4a8e6020017668ebb6743ef9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaHttpRequestFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga5d1a57074f3242737a756c3feb64d4d4a49f043cf87a8fefc6d7f383acc730dd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaMqttStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gaf32d40000cc4b46c0144fd673d6f24fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faab9ea57c4e9581fe81f32bb922e285ecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttPublishFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaf3c73c49216574f658702ea7c6fc6817</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttSubscribeFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaf675c3f005a4b554e8ac534981b66621</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaMqttUnsubscribeFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaf32d40000cc4b46c0144fd673d6f24faaccb3e6812907c643ca68360ee6221bec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaOsStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gaececff6c0dd8ba4d884544571bd368e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7ad153172731315817688544ceff3eac50</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a93602f665d93e66ca2208d45219d975e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueSendFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a0d04ffe18a839e1d2b4ab049b8993462</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueReceiveFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a61507cc0e92c4dba4ce6b1e70f9b24fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsEventQueueDeleteFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7add0eff4efdf80fa2796713bd7ffab78b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a5ed5da8e8321e3b89c11a279c884dc39</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerStartFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7ad9178092f091e650fdeab75ac7fa16f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerRestartFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7aade705ebfa991b59d6bd66523a9324d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerStopFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a760aadce2bd9363c4973720eec02b63f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaOsTimerDeleteFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggaececff6c0dd8ba4d884544571bd368e7a22a0a901794cea938b8402a3d5c6499c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaPalMainStatus_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga8e503bbbffc277f6fd5b8645767a685b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalSuccess</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba8dbb21a8a52503c7e8f33835dc6687c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalUninitialized</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba386aef27e1d9c7a937d3698e559641e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalOutOfMemory</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba0ec8f96ac056d352c45db250b4f6e05d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalNullFileContext</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba088a029115f9b8053b9c23eb50438a1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalSignatureCheckFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba3a92c1315048b471740472eaa91cd39b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRxFileCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba15af1448f649ae32235d23aaab93b90d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRxFileTooLarge</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf945ac941bd969b1833f80847d286805</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBootInfoCreateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba38e979079368805430af70c0daa35005</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBadSignerCert</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba7335f166465b3c6537093140445fdfef</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalBadImageState</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba8022263fa243f4fedb05692f5816b709</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalAbortFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf4431964d1c516dde35c2b37c829b195</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalRejectFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba0beb58eb2e069e80e51e3c4857549d14</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalCommitFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685baf5d88629edcda34a8883985972e52126</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalActivateFailed</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba6e8ffdf4071a5ef58fdd3e3bdc1253de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalFileAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685ba233a2055c5de2e07759f3e39fa88f8cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalFileClose</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga8e503bbbffc277f6fd5b8645767a685bab9b6262d3d153e77d186f6df07eb3e6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaImageState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gad4909faa8b9c8672e7f7bef1bc6b5b84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateUnknown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84afb051298f888e18cfeb7fd1b1986cb9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateTesting</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a7193a6a56fbd5578d87dfe46c785d420</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateAccepted</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84ad9ee33052c9c36593243b1bd0329a396</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateRejected</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a667999c3c223ac934ec3393520abd94a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaImageStateAborted</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggad4909faa8b9c8672e7f7bef1bc6b5b84a9d52d6fd99c42230d77ef9e40ac57b03</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaPalImageState_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gab1f386c3bcc707ff32033659a0a4ddca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateUnknown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaad4cb7e3e6e8445f5f38860339439f31c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStatePendingCommit</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaafcc2daaa65a18953e4e51ebda28b7c59</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateValid</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaaf3d8a6cc3a9b662e480be1908caa0e1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaPalImageStateInvalid</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ggab1f386c3bcc707ff32033659a0a4ddcaa0074b4f18792780ed448586aaf1c2b7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>OtaEvent_t</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>ga2c42c25235304ca5c3e3a6abef78be4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventStart</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eac965db79301fabd43971f2ad050d782c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventStartSelfTest</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eaeab0f855ccec790d94ac1e295256526e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestJobDocument</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea50831130508ed232d604aee5cf8999e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventReceivedJobDocument</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea593ee44d9ab623f9c9010603fcb7bca3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventCreateFile</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea9ff4b58dab8e17eca36163959ac02395</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestFileBlock</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead88894cbefd4b61cc2049e495e52cad1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventReceivedFileBlock</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea79ef0fd4aaffd57dd5af6ef646a70f11</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventRequestTimer</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead424034a3d0a53c719c3dfcf110aedbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventCloseFile</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eacb6862078f37cb276c7c4429a16593fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventSuspend</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead2c2503cc33bf8309c28618897f8fcdd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventResume</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4eaf1e3cb59f2e70908e121c218c27c1fe9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventUserAbort</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea92146829f7a9aaccefc0d0d96daef50b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventShutdown</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ead02ce67dfb7d99810979ec659813f89b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OtaAgentEventMax</name>
      <anchorfile>group__ota__enum__types.html</anchorfile>
      <anchor>gga2c42c25235304ca5c3e3a6abef78be4ea668f452f3c81c0b76d09921893c7027c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ota_callback_types</name>
    <title>Callback Types</title>
    <filename>group__ota__callback__types.html</filename>
    <member kind="typedef">
      <type>void(*</type>
      <name>OtaAppCallback_t</name>
      <anchorfile>group__ota__callback__types.html</anchorfile>
      <anchor>ga3bedd7e278f89d555a32a7efa8023a28</anchor>
      <arglist>)(OtaJobEvent_t eEvent, const void *pData)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ota_struct_types</name>
    <title>Parameter Structures</title>
    <filename>group__ota__struct__types.html</filename>
    <class kind="struct">OtaJobDocument_t</class>
    <class kind="struct">OtaInterfaces_t</class>
    <class kind="struct">OtaAppBuffer_t</class>
    <class kind="struct">AppVersion32_t</class>
    <class kind="struct">OtaHttpInterface_t</class>
    <class kind="struct">OtaMqttInterface_t</class>
    <class kind="struct">OtaEventInterface_t</class>
    <class kind="struct">OtaTimerInterface_t</class>
    <class kind="struct">OtaMallocInterface_t</class>
    <class kind="struct">OtaOSInterface_t</class>
    <class kind="struct">OtaPalInterface_t</class>
    <class kind="struct">Sig256_t</class>
    <class kind="struct">OtaFileContext_t</class>
  </compound>
  <compound kind="group">
    <name>ota_basic_types</name>
    <title>Basic Types</title>
    <filename>group__ota__basic__types.html</filename>
  </compound>
  <compound kind="group">
    <name>ota_constants</name>
    <title>Constants</title>
    <filename>group__ota__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>LOG2_BITS_PER_BYTE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga9794a22e433da1780d89dc408f225192</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BITS_PER_BYTE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga369ecd38b3ab077fc235f892354bb46f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_FILE_BLOCK_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac70b23d17d31a065c30283c4fcc5ab8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_FILES</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga4cd0bbaca7882b52913ca82504dfaf86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_BLOCK_BITMAP_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga538d92a689f2b60c60bcef41e2739a1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_REQUEST_MSG_MAX_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga9b81d4cb7dbd60b668da6dc2dea072c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_REQUEST_URL_MAX_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gab4104ae99ac82ab47978b046e6e26e6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_ERASED_BLOCKS_VAL</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga759a079ce6746a1d1f7e6391565bbe4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_NUM_MSG_Q_ENTRIES</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga76a5e62449824989e82600736cc65c45</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_JSON_TOKENS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga059be218ff71c8df0eccda0a7115bfad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_MAX_JSON_STR_LEN</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gafaa370948b99101757c117fd5f85abc1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DOC_MODEL_MAX_PARAMS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gabe907910e02fd0d9ac522cc48c39e54d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JOB_PARAM_REQUIRED</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga716173ece0b83fdc783798bdf2cf5715</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JOB_PARAM_OPTIONAL</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gab4f3b8ca5434b9b02ae4244da5cd2078</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DONT_STORE_PARAM</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac462962d86d18bfe1fab0ca0527ea9ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_STORE_NESTED_JSON</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga62f786dfa9c243d52d490aedf55b74a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_DATA_BLOCK_SIZE</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga2333ad9c3ebbf1c5956e163cf1ff7998</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_JOB_MSG_READY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaa9b83cfe52a6ccb7df183fe92b73b7de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_DATA_MSG_READY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga77def055cb745579b01ef6372f848a84</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_SHUTDOWN</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga3bf06ac14931655f3f7956a519472b64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_REQ_TIMEOUT</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga808ed33f5edc75b564ae8ea8827bafa3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_USER_ABORT</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaffd25dc92e8b9ddbfab667dfcc79cbe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_EVT_MASK_ALL_EVENTS</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga030724739b0df6d0b355893d4fd732e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>kOTA_MaxSignatureSize</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga374cf454c7a4f641bb31433c410c49b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SEPARATOR</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gae53b17849f410c6ccf5b8862293fc302</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_CLIENT_TOKEN_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga3d69df69fbdff6811da80020c8afc61a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_TIMESTAMP_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga625051ce4de218406aad6dd0955f1c73</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_EXECUTION_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga4d831064bb7ea8bae1df86e1b77eaab6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_JOB_ID_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga44138af02a7ed130ff630788cbaf1c27</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_STATUS_DETAILS_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga6a6b266874ad73e248d840643616325c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SELF_TEST_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaf7c64cc7a2f1a8141d00f0e33f83c1d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATED_BY_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaffd03defb729ae990baede2a0c704076</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATED_BY_KEY_ONLY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaed6d19287cf525cb7c5dcf50798c6cd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_SELF_TEST_KEY_ONLY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga71be1d5c1e4a59d0627b7c83e64a5124</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_JOB_DOC_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac0132213a7beb808d8cf4b86a3ab6b86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_OTA_UNIT_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga1c56c6acb4d197a362143b670aeaac75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_PROTOCOLS_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaacb7d18662a1212605eede7c875016ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_GROUP_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga85efaf15e103da089f3de398948f46d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_STREAM_NAME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga1801081d3c11b83ce214949c54156587</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_PATH_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga209cd55d10743b96b2792ed8ae12fa51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_SIZE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gadbef300d5ee8db5704a53dd50f9babc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_ID_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gac0ddd548347a6aa9e001eb3383ddeafe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_ATTRIBUTE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga098cfb263c324b93dee1e98917312ffd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILE_CERT_NAME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>ga6e5f882dc07e8a5a8b2f079cc4444a50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_UPDATE_DATA_URL_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gad3c584828dcc7d99d65931d0c1ded6b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_AUTH_SCHEME_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaafc3e865308a16ec82181d6aea75d6bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OTA_JSON_FILETYPE_KEY</name>
      <anchorfile>group__ota__constants.html</anchorfile>
      <anchor>gaab7b7332590b19bcc767a290953c58a8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>ota_design</name>
    <title>Design</title>
    <filename>ota_design.html</filename>
    <docanchor file="ota_design.html" title="OTA Library Configurations">ota_design_library_configs</docanchor>
    <docanchor file="ota_design.html" title="OTA Library">ota_design_library</docanchor>
    <docanchor file="ota_design.html" title="OTA Agent API’s">ota_design_agent_api</docanchor>
    <docanchor file="ota_design.html" title="OTA Platform Abstraction Layer">ota_design_pal</docanchor>
    <docanchor file="ota_design.html" title="OTA OS Functional Abstraction">ota_design_os</docanchor>
    <docanchor file="ota_design.html" title="OTA MQTT interface">ota_design_mqtt</docanchor>
    <docanchor file="ota_design.html" title="OTA HTTP interface">ota_design_http</docanchor>
    <docanchor file="ota_design.html" title="OTA Memory Allocation">ota_design_memory</docanchor>
    <docanchor file="ota_design.html" title="OTA Agent Task">ota_design_agent_task</docanchor>
    <docanchor file="ota_design.html" title="Job Parser">ota_design_job_parser</docanchor>
    <docanchor file="ota_design.html" title="OTA Control and Data Protocols">ota_design_control_data_protocols</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_functions</name>
    <title>Functions</title>
    <filename>ota_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_init_function</name>
    <title>OTA_Init</title>
    <filename>ota_init_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_shutdown_function</name>
    <title>OTA_Shutdown</title>
    <filename>ota_shutdown_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_getstate_function</name>
    <title>OTA_GetState</title>
    <filename>ota_getstate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_activatenewimage_function</name>
    <title>OTA_ActivateNewImage</title>
    <filename>ota_activatenewimage_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_setimagestate_function</name>
    <title>OTA_SetImageState</title>
    <filename>ota_setimagestate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_getimagestate_function</name>
    <title>OTA_GetImageState</title>
    <filename>ota_getimagestate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_checkforupdate_function</name>
    <title>OTA_CheckForUpdate</title>
    <filename>ota_checkforupdate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_suspend_function</name>
    <title>OTA_Suspend</title>
    <filename>ota_suspend_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_resume_function</name>
    <title>OTA_Resume</title>
    <filename>ota_resume_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_signalevent_function</name>
    <title>OTA_SignalEvent</title>
    <filename>ota_signalevent_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_eventprocessingtask_function</name>
    <title>OTA_EventProcessingTask</title>
    <filename>ota_eventprocessingtask_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_getstatistics_function</name>
    <title>OTA_GetStatistics</title>
    <filename>ota_getstatistics_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_err_strerror_function</name>
    <title>OTA_Err_strerror</title>
    <filename>ota_err_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_jobparse_strerror_function</name>
    <title>OTA_JobParse_strerror</title>
    <filename>ota_jobparse_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_palstatus_strerror_function</name>
    <title>OTA_PalStatus_strerror</title>
    <filename>ota_palstatus_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_osstatus_strerror_function</name>
    <title>OTA_OsStatus_strerror</title>
    <filename>ota_osstatus_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>ota_config</name>
    <title>Configurations</title>
    <filename>ota_config.html</filename>
    <docanchor file="ota_config.html">OTA_DO_NOT_USE_CUSTOM_CONFIG</docanchor>
    <docanchor file="ota_config.html">otaconfigSTACK_SIZE</docanchor>
    <docanchor file="ota_config.html">otaconfigAGENT_PRIORITY</docanchor>
    <docanchor file="ota_config.html">otaconfigLOG2_FILE_BLOCK_SIZE</docanchor>
    <docanchor file="ota_config.html">otaconfigMAX_NUM_BLOCKS_REQUEST</docanchor>
    <docanchor file="ota_config.html">otaconfigSELF_TEST_RESPONSE_WAIT_MS</docanchor>
    <docanchor file="ota_config.html">otaconfigFILE_REQUEST_WAIT_MS</docanchor>
    <docanchor file="ota_config.html">otaconfigMAX_THINGNAME_LEN</docanchor>
    <docanchor file="ota_config.html">otaconfigMAX_NUM_REQUEST_MOMENTUM</docanchor>
    <docanchor file="ota_config.html">otaconfigMAX_NUM_OTA_DATA_BUFFERS</docanchor>
    <docanchor file="ota_config.html">otaconfigOTA_UPDATE_STATUS_FREQUENCY</docanchor>
    <docanchor file="ota_config.html">otaconfigAllowDowngrade</docanchor>
    <docanchor file="ota_config.html">configENABLED_CONTROL_PROTOCOL</docanchor>
    <docanchor file="ota_config.html">configENABLED_DATA_PROTOCOLS</docanchor>
    <docanchor file="ota_config.html">configOTA_PRIMARY_DATA_PROTOCOL</docanchor>
    <docanchor file="ota_config.html" title="LogError">ota_logerror</docanchor>
    <docanchor file="ota_config.html" title="LogWarn">ota_logwarn</docanchor>
    <docanchor file="ota_config.html" title="LogInfo">ota_loginfo</docanchor>
    <docanchor file="ota_config.html" title="LogDebug">ota_logdebug</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_porting</name>
    <title>Porting Guide</title>
    <filename>ota_porting.html</filename>
    <docanchor file="ota_porting.html" title="Configuration Macros">ota_porting_config</docanchor>
    <docanchor file="ota_porting.html" title="OTA PAL requirements:">ota_porting_pal</docanchor>
    <docanchor file="ota_porting.html" title="OTA OS Functional Interface requirements:">ota_porting_os</docanchor>
    <docanchor file="ota_porting.html" title="OTA MQTT requirements:">ota_porting_mqtt</docanchor>
    <docanchor file="ota_porting.html" title="OTA HTTP requirements:">ota_porting_http</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_http_interface</name>
    <title>OTA HTTP Interface</title>
    <filename>ota_http_interface.html</filename>
    <docanchor file="ota_http_interface.html" title="OTA HTTP Overview">ota_http_interface_overview</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_mqtt_interface</name>
    <title>OTA MQTT Interface</title>
    <filename>ota_mqtt_interface.html</filename>
    <docanchor file="ota_mqtt_interface.html" title="OTA MQTT Overview">ota_mqtt_interface_overview</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_os_functional_interface</name>
    <title>OTA OS Functional Interface</title>
    <filename>ota_os_functional_interface.html</filename>
    <docanchor file="ota_os_functional_interface.html" title="OTA OS Functional Interface">ota_os_functional_interface_overview</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_pal_interface</name>
    <title>OTA PAL Interface</title>
    <filename>ota_pal_interface.html</filename>
    <docanchor file="ota_pal_interface.html" title="OTA PAL Overview">ota_pal_interface_overview</docanchor>
  </compound>
  <compound kind="page">
    <name>ota_constants</name>
    <title>Constants</title>
    <filename>ota_constants.html</filename>
    <docanchor file="ota_constants.html" title="OTA Error Code Helper constants">ota_constants_err_code_helpers</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
    <docanchor file="index.html">ota</docanchor>
    <docanchor file="index.html" title="OTA Dependencies">ota_dependencies</docanchor>
    <docanchor file="index.html" title="Memory Requirements">ota_memory_requirements</docanchor>
  </compound>
</tagfile>
