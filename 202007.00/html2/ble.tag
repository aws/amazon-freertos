<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20">
  <compound kind="file">
    <name>bt_hal_gatt_client.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__gatt__client_8h.html</filename>
    <includes id="bt__hal__gatt__types_8h" name="bt_hal_gatt_types.h" local="yes" imported="no">bt_hal_gatt_types.h</includes>
    <includes id="bt__hal__manager__types_8h" name="bt_hal_manager_types.h" local="yes" imported="no">bt_hal_manager_types.h</includes>
    <class kind="struct">BTGattUnformattedValue_t</class>
    <class kind="struct">BTGattReadParams_t</class>
    <class kind="struct">BTGattWriteParams_t</class>
    <class kind="struct">BTGattNotifyParams_t</class>
    <class kind="struct">BTGattTestParams_t</class>
    <class kind="struct">BTGattClientCallbacks_t</class>
    <class kind="struct">BTGattClientInterface_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterClientCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga81a74dd70142306c78bc18bb82cec62f</anchor>
      <arglist>)(BTGattStatus_t xStatus, uint8_t ucClientIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSearchCompleteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga82ef416e4893fa894f37f1f95e45e9e6</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterForNotificationCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga70c3a9bc07602d7ee4d244a1eec9cda1</anchor>
      <arglist>)(uint16_t usConnId, bool bRegistered, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTNotifyCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5e2f5a4784d42781adc2934ae1a0b098</anchor>
      <arglist>)(uint16_t usConnId, BTGattNotifyParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadCharacteristicCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga43cd7e112d436d7c107efb8b748f3247</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, BTGattReadParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWriteCharacteristicCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gadcf7430976f57d009fe1040f7d30b7fe</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTExecuteWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8d5f9c5e50bcdccb64af5771ade5410a</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadDescriptorCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gabaa06ef5d631a4bd5cab4dd4ded777fa</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, BTGattReadParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWriteDescriptorCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2510813148ed3b069040f410d7c55a2</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTListenCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa1892bb34b892b19aa61d0a748ea9924</anchor>
      <arglist>)(BTGattStatus_t xStatus, uint32_t ulServerIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConfigureMtuCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1e184e6dfa402cc6ddc16845030c0a31</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint32_t ulMtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTGetGattDbCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac6a0335b83f3b3d885dad0236a300eac</anchor>
      <arglist>)(uint16_t usConnId, BTGattDbElement_t *pxDb, uint32_t ulCount)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServicesRemovedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2601674bd7f321e3eaac2b8788cd3e87</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usStartHandle, uint16_t usEndHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServicesAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae993f95bcc54d151712184451658cfde</anchor>
      <arglist>)(uint16_t usConnId, BTGattDbElement_t *pxAdded, uint32_t ulAddedCount)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattcError_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gadf029413bc01e41d5a1d15395c1ac054</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7898a8e84242bf38a2bbe3b130bbb3f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandStarted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a57c962c53c0c5cbf9907c20fd1df862b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandBusy</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7b1d8881d312a2f1a53dc0c4ef0335d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandStored</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7ca21f4df5dc579b34a9976caf8008c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcNoResources</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a9536585b4e6c1f8c45c755cdf91c64de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcModeUnsupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054aeb21644a26b4b5c44b1ce5af84631fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcIllegalValue</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a71b6c46284120af40042f0ca0f6e074c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcIncorrectState</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a210c6a2336af185c12c18ca5602c1318</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcUnknownAddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a435254b539f6c4f30f7242790ee32019</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcDeviceTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054af541b51dd6bc051ff31a60416dc85af9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcInvalidControllerOutput</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a758ba640f9ed1334f92654ea79e88899</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcSecurityError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054ad7daa7f776ff3defe712c1c05578b75c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcDelayedEncryptionCheck</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a1b9312d7e04f80d4e8dd20f4da0a0543</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcErrProcessing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054ad3428549722d884bf2e602cbdbf62f6d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_gatt_server.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__gatt__server_8h.html</filename>
    <includes id="bt__hal__gatt__types_8h" name="bt_hal_gatt_types.h" local="yes" imported="no">bt_hal_gatt_types.h</includes>
    <class kind="struct">BTGattValue_t</class>
    <class kind="union">BTGattResponse_t</class>
    <class kind="struct">BTGattServerCallbacks_t</class>
    <class kind="struct">BTGattServerInterface_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterServerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4eba705215e4c27ee5bccdd37dcdfcde</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTUnregisterServerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf1281a92d067697bf867ec9a7be3c13e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnectionCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6851953b2552ca6e4fb80d3bf8613fcf</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucServerIf, bool bConnected, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga32f2846853cb51c0a1884c53fe2dd77e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTGattSrvcId_t *pxSrvcId, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTIncludedServiceAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3aa10d2a84b34b5865d3b248574bd6b9</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle, uint16_t usInclSrvcHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTCharacteristicAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7ec3da0a162e827f21e3d3f72f895383</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxUuid, uint16_t usServiceHandle, uint16_t usCharHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSetValCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac832c40fca8f7ea8844f92ff9227ab57</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usAttrHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDescriptorAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga97f57c2ca72c95126f52ee08a7cb6a82</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxUuid, uint16_t usServiceHandle, uint16_t usDescrHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceStartedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga52c077e02fb724a4a2fac950f1d94fae</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceStoppedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2f7a2c53b4130bafac3024bd2b2f59f2</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceDeletedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0feae4ce7639693710c5f87925732c1d</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestReadCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac3ced021a1484077860213dc30c51798</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, uint16_t usAttrHandle, uint16_t usOffset)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae5abff05ecc3368be82af799342dfa65</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, uint16_t usAttrHandle, uint16_t usOffset, size_t xLength, bool bNeedRsp, bool bIsPrep, uint8_t *pucValue)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestExecWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6da40d1176611ab97302e23e7812eed4</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, bool bExecWrite)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTResponseConfirmationCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga276589c8697edd6f6e2ff6692726422b</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTIndicationSentCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa165494a87ba7d3f6abf2edb77cd367b</anchor>
      <arglist>)(uint16_t usConnId, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMtuChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga24d664c493a06728e34423a7a8644cc8</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usMtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWhiteListChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7885d17d4d1d01a8e86b67647d44a39e</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, bool bIsAdded)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTRspErrorStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga968ba337d61fb6be716b8473ec97b319</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorNone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319ae1af56c9562f6efa35b7b2bfe875079f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorNo_mitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a59265ed653454848870015af2a7a2b8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a63fb666404e177358249052873b0a71e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorSignedNoMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319aed4b2446ae5950a0cd05132929518b32</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a555f5ae20e2c184614251bc3535a79bd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_gatt_types.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__gatt__types_8h.html</filename>
    <includes id="bt__hal__manager_8h" name="bt_hal_manager.h" local="yes" imported="no">bt_hal_manager.h</includes>
    <includes id="bt__hal__manager__types_8h" name="bt_hal_manager_types.h" local="yes" imported="no">bt_hal_manager_types.h</includes>
    <class kind="struct">BTGattInstanceId_t</class>
    <class kind="struct">BTGattSrvcId_t</class>
    <class kind="struct">BTGattTrackAdvInfo_t</class>
    <class kind="struct">BTCharacteristic_t</class>
    <class kind="struct">BTCharacteristicDescr_t</class>
    <class kind="struct">BTIncludedService_t</class>
    <class kind="struct">BTAttribute_t</class>
    <class kind="struct">BTService</class>
    <class kind="struct">BTGattDbElement_t</class>
    <member kind="define">
      <type>#define</type>
      <name>btGATT_MAX_ATTR_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1a7d653463b2a9fcf562fa114adaa5b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>BTUuid_t</type>
      <name>BTServiceUUID_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8b891d4e44836fd951ad1f13c42a0b3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnectCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5ca839bac0a52a1025843dd7c680f543</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint8_t ucClientIf, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDisconnectCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae09852d593ec0cf60ffe44519a9681cf</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint8_t ucClientIf, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadRemoteRssiCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1da38dd690cb247fab63a93f0af571f4</anchor>
      <arglist>)(uint8_t ucClientIf, BTBdaddr_t *pxBda, uint32_t ulRssi, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTCongestionCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga978849d2a447b29637b65bae86ce1973</anchor>
      <arglist>)(uint16_t usConnId, bool bCongested)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae3f7d1f4e7d4188a250e314475352374</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a51060d4d256106d510420d10634566fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTInvalidHandle</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a921c38dfe11e27b0e2e5899349741261</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusReadNotPermitted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a83c88bde61b69ec7e03d64a122ed7b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusWriteNotPermitted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a53fd128e06da4e14c525b9283a9f615c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidPDU</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374adcd0c29a4f7322ec61aca670dafdffbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientAuthentication</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a78782eeb9d080bab86409bdac2a108bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusRequestNotSupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a69162ea5a53e48acc2b785fad12c37da</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidOffset</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a05db7b9ff1ca372c0d97361e10c6cab1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientAuthorization</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a5bbd004757d6efa11f3dfcf144d97dd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusPrepareQueueFull</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a39a3aceaa8def06ba89d2d605128ad75</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusAttributeNotFound</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a4e0f2f9562afaa512853aa3a2eff9fa8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusAttributeNotLong</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a577cffc6cc5f43e5ee6849fa80dedf01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientKeySize</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a7091c30daa7c60ce08c7423d9af14880</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidAttributeLength</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a14358310b49ef4c2a0488b79e0106109</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusUnlikelyError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a7fe6ea959fbed7534518eb20799bd5b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientEncryption</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a21487939009d47907b56580df40c96c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusUnsupportedGroupType</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374afc0cd7f169a499a61f979cc2674dcce5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientResources</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a0c4bb7143ce3bb311b768e8b86dd8c95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInternalError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a1109b99f8c87820bf0026805505d723f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374ac795b7646783fb11796a24bc25669995</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusConnectionCongested</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a068e4573dd3f1a26be06e5c4cb7318c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusErrorConnEstFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374af3a559d6b08c51b05e248f9c3e7a7eb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusErrorConnTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a3a828672a83ab5c19d25779136dd33a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusLocalHostTerminatedConnection</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a9d51a7c30364f875aa103dab65e492a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelNoSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55bada228a8bef37fc65d0f3cca6873dbb87</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelUnauthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba67f3762fb7926f3777afd3b5cf4bea9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelAuthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba620198c17d4d990f84c07db5bc728f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55badcf89fab404e21871d93d872b7f98028</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermRead</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8325a48a4a20814e53afb40db5f4ffea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a5785a0ecec6d54319e016f5f0bea0f6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a4a2db8c46dcf6ce637a104ed992d73bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWrite</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a3cfa3940a8668127035c18c4ba7c613d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8c0ffeb6004b07ba2a51be5196286fe1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a2ea723131d60886fe6fe7d3d1a3b2ed9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7aa970663c467e491b324a4c7cb3079c64</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7acdf62b170fa40bf1bcfc6053d1fa7369</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqBonding</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a16aeff1e8090908e7a0c9134b23dd6bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7afb35866e76777ffa8692277904a8bb3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a7946f00729be41f9eae6f9fd8adae9c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAttrWriteRequests_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga55c1e7ffff1cf5ea0164fa8e6795a0ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeNoResponse</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0cea9681e4b581a7f800d3146cb7de0ea12a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeDefault</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0cea1540792ce245577295d72ca319baf92a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypePrepare</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0ceacb8341dddbbf007ac7221d10dd112596</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0ceaf4a0ec416dcea8d1591b85ee8df9e5b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattServiceTypes_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga42d9ff4543b70f554131656403ce0dde</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTServiceTypePrimary</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga42d9ff4543b70f554131656403ce0ddeac4af7bd10fe3fe69564551de8bbe2804</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTServiceTypeSecondary</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga42d9ff4543b70f554131656403ce0ddea2887a50ea7fdde925d35c3041f148fe3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDbAttributeType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae3c29649b63768bc288904e3f4202744</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbPrimaryService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a4d3ebaa6953a2f4b22abfb3b4688a525</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbSecondaryService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744aa6d9b379f74e9703140e3d9ffe4bfedf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbIncludedService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744aa0126b319c857ea5f0f5811cb6787f49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbCharacteristicDecl</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a6cb593924840487c6e1e0304adb2c07c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbCharacteristic</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a13d4e7209ea43c7ad5d50410b393404d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbDescriptor</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744ace647fcd790e4d26626f62e4baf310ae</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_manager.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__manager_8h.html</filename>
    <includes id="bt__hal__manager__types_8h" name="bt_hal_manager_types.h" local="yes" imported="no">bt_hal_manager_types.h</includes>
    <class kind="struct">BTPinCode_t</class>
    <class kind="struct">BTProperty_t</class>
    <class kind="struct">BTOutOfBandData_t</class>
    <class kind="struct">BTActivityEnergyInfo</class>
    <class kind="struct">BTUidTraffic_t</class>
    <class kind="struct">BTCallbacks_t</class>
    <class kind="struct">BTInterface_t</class>
    <member kind="define">
      <type>#define</type>
      <name>btMAJOR_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gacdb3e342f24ac5e39c745efb823bc2a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btMINOR_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gafe2f4e8cb2ad8cb0d3d47e51ad6084e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btPATCH_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6ceca2c5690fbbbcc9f3936d4b5fb5bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btSTR_HELPER</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga087df9185bb5c85141ae6b3a77e50be5</anchor>
      <arglist>(x, y, z)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btSTRING_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga244cc1213b691aa80da1bc75a0c53eff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btBLE_ADD_BLOB_SERVICE_SUPPORTED_BIT</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga11774cfd2d39d0b4fc3747b07049892d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDeviceStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2dedbe2af2576e4c167113bdf271de5</anchor>
      <arglist>)(BTState_t xState)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga99537ec9b0475d5f162d9cac65f22084</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6170bf7b014afbc2d3764ab5ceeb2fde</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPinRequestCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga178d889935bd336575e91a1e2cfbc91c</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteBdAddr, BTBdname_t *pxBdName, uint32_t ulCod, uint8_t ucMin16Digit)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSspRequestCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae01c7c1ee76ba40d7e74ec54efc39083</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteBdAddr, BTBdname_t *pxRemoteBdName, uint32_t ulCod, BTSspVariant_t xPairingVariant, uint32_t ulPassKey)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPairingStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga804f88c3d32375bd496e2a2c0a1503d8</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, BTBondState_t xState, BTSecurityLevel_t xSecurityLevel, BTAuthFailureReason_t xReason)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTEnergyInfoCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4277f9d5316419686a24a6c2e77dc58c</anchor>
      <arglist>)(BTActivityEnergyInfo *pxEnergyInfo, BTUidTraffic_t *pxUidData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDutModeRecvCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga711f1ac6ff9558bd079ad8cbb9997b7d</anchor>
      <arglist>)(uint16_t usOpcode, uint8_t *pucBuf, size_t xLen)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTLeTestModeCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga55f2ee38d318c5ef03ee4ba763f9b975</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usNumPackets)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadRssiCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7b931a36a7988ae30a2145dd2809de4a</anchor>
      <arglist>)(BTBdaddr_t *pxBda, uint32_t ulRssi, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSlaveSecurityRequestCallBack_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8c0d83c281853e8dcbb660c5b02d926c</anchor>
      <arglist>)(BTBdaddr_t *pxBda, BTSecurityLevel_t xSecurityLevel, bool bBonding)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTTxPowerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5a76aad0e81c46c1504990170149f994</anchor>
      <arglist>)(BTBdaddr_t *pxBda, BTTransport_t xTransport, uint8_t ucTxPowerValue, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBondedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaff4165bf99dfd12faacd7dccaeabcb28</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, bool bIsBonded)</arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelNoSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55bada228a8bef37fc65d0f3cca6873dbb87</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelUnauthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba67f3762fb7926f3777afd3b5cf4bea9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelAuthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba620198c17d4d990f84c07db5bc728f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55badcf89fab404e21871d93d872b7f98028</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermRead</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8325a48a4a20814e53afb40db5f4ffea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a5785a0ecec6d54319e016f5f0bea0f6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a4a2db8c46dcf6ce637a104ed992d73bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWrite</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a3cfa3940a8668127035c18c4ba7c613d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8c0ffeb6004b07ba2a51be5196286fe1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a2ea723131d60886fe6fe7d3d1a3b2ed9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7aa970663c467e491b324a4c7cb3079c64</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7acdf62b170fa40bf1bcfc6053d1fa7369</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqBonding</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a16aeff1e8090908e7a0c9134b23dd6bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7afb35866e76777ffa8692277904a8bb3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a7946f00729be41f9eae6f9fd8adae9c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTTransport_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga456876e81e2dc3f1b233196a688b34e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportAuto</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2a7b8f8330380d42dbd4d2a2fab024c6ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportBredr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2ab1d2a19fe52fcf4f898fcf95d6fcd215</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportLe</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2a99b4c8bf4860ffa4b48c993e4f25c464</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga76ea5dede6ba720344e1faddbd2bac67</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTstateOff</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga76ea5dede6ba720344e1faddbd2bac67a19766ff9a46c2d818190c14ba6edb9b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTstateOn</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga76ea5dede6ba720344e1faddbd2bac67a0ba12f7393a0e1697ae0114b1769eaf8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAuthFailureReason_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga346e38e1a77ce4a4a5d9e3dcc794d09b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf0d115435eb821c9aa427fbe0962dac0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailLmpRespTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba72725624b14f91d9172938b06c1631c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailKeyMissing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf99274687940754269bb2638146c6d9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailEncrypMode</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba7ac2e29664b766239530230ed5fd0af1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailUnitKey</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba6fc54b30804a3c51348875ab90229fff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpCfrmValue</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bae31afd13b950243f4da008b418ff0a58</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpEnc</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf0a2bfa37c783235dbaebb1f1b3de478</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba19fb4ad75ab958136b07ea2a4e19f31a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailPageTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bacb0c32b82023ce6525d5e03cb47d93a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpPasskeyFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba96449eb5b38b18a41c8337f8f463a80f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpOobFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba0e41631aaca1b9b3581efdee3e477130</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba4c97b248d7e4854f308780c001750990</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailConnTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba08851c2f4a81eac93e4a9c86530bf9fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmp</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba9d5013c4442967b635e89561326fd7f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpPairNotSupport</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba174b800bf85d35d613537d4fcca244af</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpUnknownIo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bad7cf34c1f83808f3a57d714cbd2fc1c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpUnknown</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba3b274104c333ad2e6bde5379994268f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailHostRejectSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09babb1fc776951bc2ea0fa1ca72786ed450</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailInsuffSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba72f12f5da7ef0ea9c7f4fe83c74bd3a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailPeerUser</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba230a7995b2ebc3dc9f93e52412e54848</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailUnspecified</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba12cf40f57b7329e6ddb1e43fc7b7753b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTPropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga530175d220bd1e204bbefdb3b966f5d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBdname</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab02f34113978d7142e6779b86a2b67ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBdaddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a9121617b9e0155d5f2d7a7cc9bc71280</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyTypeOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ad3c1685b0709e1a97f92d84c700a2940</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyAdapterBondedDevices</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a58de461279def747be7b5ea9c8e56018</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteFriendlyName</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a6658547f4158588d0486b906e04b7300</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteRssi</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7adff655a4f0b9ff1d80be8124fdf602c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteVersionInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab3b46be8a73df86cf5f1c1fd88c77a63</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyLocalMTUSize</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a6318bb5b7791d773d9ed6a6b187ded9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBondable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a26dba10da958c2132aadcffe738ec424</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyIO</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a91abfcdd260b1052ace98dcc1dbaad6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyConnectable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab39eecef92a7a995cad8948d2a668b6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertySecureConnectionOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab9b7832fd8b5f9b7b8e9a995741df673</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDeviceType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga846c67f839515745caaedb24eea443a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeBredr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6abb58d5d10fb99056d5f0466fe7db9346</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeBle</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6a5b777ca5358bb53fa7598e706dd77342</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeDual</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6ab5154bcba15fc7c333e2a8b263095dfc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTBondState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3b038e6390669d05aaa1b0d7ab8b4cee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTSspVariant_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2add06926fac1c572de9f754eab88568</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyConfirmation</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a670fda7532def327e8c7da1af9283a8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyEntry</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a144c3f0f72dbc83b4af7271896a71dea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantConsent</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568ae380862deefa0e42596372a419679615</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyNotification</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a3e3d66782089ad2828db348b8ebc0ae2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_manager_adapter_ble.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__manager__adapter__ble_8h.html</filename>
    <includes id="bt__hal__gatt__types_8h" name="bt_hal_gatt_types.h" local="yes" imported="no">bt_hal_gatt_types.h</includes>
    <includes id="bt__hal__manager__types_8h" name="bt_hal_manager_types.h" local="yes" imported="no">bt_hal_manager_types.h</includes>
    <includes id="bt__hal__manager_8h" name="bt_hal_manager.h" local="yes" imported="no">bt_hal_manager.h</includes>
    <class kind="struct">BTGattFiltParamSetup_t</class>
    <class kind="struct">BTGattAdvName_t</class>
    <class kind="struct">BTGattAdvertismentParams_t</class>
    <class kind="struct">BTLocalLeFeatures_t</class>
    <class kind="struct">BTBleProperty_t</class>
    <class kind="struct">BTBleAdapterCallbacks_t</class>
    <class kind="struct">BTBleAdapter_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterBleAdapterCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga678c3554952d1f6dc6723dce0141645e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucAdapterIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBleDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4b074dc84738af51b05d821c5ed7a32d</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTBleProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBleRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga68d00df313388dec35f7a6ddacfa4e77</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTBleProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanResultCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0e12fa85d0ed31d450dfb34fde0c2663</anchor>
      <arglist>)(BTBdaddr_t *pxBda, uint32_t ulRssi, uint8_t *pucAdvData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTAdvStatusCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa40080a2e276e49b1f967a52bfeada12</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucAdapterIf, bool bStart)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSetAdvDataCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab46a3f882124f4a007da2074b17b1541</anchor>
      <arglist>)(BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnParameterUpdateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga51661f41d0a1d333075917765db76a40</anchor>
      <arglist>)(BTStatus_t xStatus, const BTBdaddr_t *pxBdAddr, uint32_t ulMinInterval, uint32_t ulMaxInterval, uint32_t ulLatency, uint32_t usConnInterval, uint32_t ulTimeout)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterCfgCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga81db6bdf19000ea714c0ca35211f210a</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulFiltType, uint32_t ulAvblSpace)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterParamCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga813ce88b8d81384a6e8adca274b22790</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulAvblSpace)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterStatusCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga71024bd3944f1c3961c00916e3f3430c</anchor>
      <arglist>)(bool bEnable, uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvEnableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaaf18a6ad02100c7763856504c8b424d6</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvUpdateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gabe50a40fdcc3280ee24c41ba988b53f2</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvDataCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaea07ee7ba0648d3a1342bf5295e2d4d8</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvDisableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0b9aae1d97f57fa7c604bffb560874eb</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanCfgStorageCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga9ae4a3617b0e50b20d5b9e14001a8198</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanEnableDisableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga06cf4afc1f73f0fd6af57c191634ecc8</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanReportsCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6224a3f06cbd5a6efe22beb05fc9779d</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulReportFormat, uint32_t ulNumRecords, size_t XDataLen, uint8_t *pucRepData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanThresholdCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2f89dc7613730e834780827d1395feb</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTTrackAdvEventCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaed8a224017ec478959379b9955904a81</anchor>
      <arglist>)(BTGattTrackAdvInfo_t *pxTrackAdvInfo)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanParameterSetupCompletedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab3124b3e8483a807a432781fc1415832</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPhyUpdatedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga604ebe340a3340ed69cc1a81d45f9e56</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadClientPhyCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga83fc4219b95ba8ef0999819d5b1cbee6</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAddrType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa308d92c785fe8922cce3e1ef91f5e68</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAdvProperties_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae8f708573467e5503ca86021ab1b438e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTBlePropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gacf9b60f2e7d64cf18d46ae4a66d324d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyLocalLeFeatures</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggacf9b60f2e7d64cf18d46ae4a66d324d6a3d63788381d4042df232d7811a0368d3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_manager_adapter_classic.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__manager__adapter__classic_8h.html</filename>
    <includes id="bt__hal__manager_8h" name="bt_hal_manager.h" local="yes" imported="no">bt_hal_manager.h</includes>
    <class kind="struct">BTServiceRecord_t</class>
    <class kind="struct">BTEirManfInfo_t</class>
    <class kind="struct">BTGadgetEirManfInfo_t</class>
    <class kind="struct">BTClassicProperty_t</class>
    <class kind="struct">BTClassicCallbacks_t</class>
    <class kind="struct">BTClassicInterface_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTClassicDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga562f663c025ee918c4c0705ba6239f7e</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTClassicRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga62b3f905fe07987bb706bac2d6e6c847</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDeviceFoundCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa02b8d9903f1bb665f42220dffd23e74</anchor>
      <arglist>)(uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDiscoveryStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab2ae428a5a108343805f15bedddb6bd4</anchor>
      <arglist>)(BTDiscoveryState_t xState)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTaclStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaabe3141baf8b9bda500f5cfa402c060f</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, BTaclState_t xState)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanMode_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac777e1a3d6db541521c011a9c3a66378</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeNone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a164280095452f56cdf34be47afd87bdc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeConnectable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a82c11d80a0707e023e2092056c098dde</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeConnectableDiscoverable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a5d06debc4f829d352134e09f73ba5060</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanParam_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa55a14fdd3e156cf0a0e2f4fada3a726</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPageScanInterval220ms</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaa55a14fdd3e156cf0a0e2f4fada3a726a600d9b8bfe2920c0e246fbddeaf5c85b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPageScanInterval1280ms</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaa55a14fdd3e156cf0a0e2f4fada3a726a65cf5716b31ae7d7ad5a740f2ce69fb8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDiscoveryState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga36585806be9c4f9b584cbac871ddae96</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDiscoveryStopped</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga36585806be9c4f9b584cbac871ddae96a0b297bc4581e6c358cb26492cd5b0de2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDiscoveryStarted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga36585806be9c4f9b584cbac871ddae96ade632c087293593ffbf552a6432f32a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTaclState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0767b73311c42402d8ad53ff8f1360eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTaclStateConnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga0767b73311c42402d8ad53ff8f1360ebaece256ebffdcd98a04eeb6c390ac872a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTaclStateDisconnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga0767b73311c42402d8ad53ff8f1360ebac9c49a5282c66cdfb353b4c227621ca5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga29d4f9d0def7da97adf02176be01828e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanTypeStandard</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga29d4f9d0def7da97adf02176be01828ea6f8c11d8c4eca0b70ab2ab214b0698ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanTypeInterlaced</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga29d4f9d0def7da97adf02176be01828eacc3a9f1da5a1fb27aad9d261e56a182e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTClassicPropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf3da26eb473acab9d2041dd49381a795</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyBdname</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a7bccfc4c7c498564d0bb9dedc233dd88</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyBdaddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a946d96da947f66dce8f99c16e9cced54</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyTypeOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a90096e17efba46b63d172f8c153f9cba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyUUIDS</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a35a44b95c1e2d61321336bf7a39556fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyClassOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a334358aa71b5d53e2194afa3abd2a7c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyServiceRecord</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795aa027603425a430ad4db78727b21a27a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterDiscoveryTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a42ea706107b99c997f8866390fd9a06f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterScanMode</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a233ba6b293063c998896c16e4e674471</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyEIRManfInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a4c1efec4ff78d702bee3700cf5956b48</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterScanParam</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a29b04d57ede6b23c9033c8fa0a6d9b33</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyConnectability</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795ab6e9683e71c18abab5d710d500ae69bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyScanType</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a58cadd3758d894fde512d86095271ec3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyGadgetEIRManfInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a63002d576e812cf65bdcbdf7634d9a46</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>bt_hal_manager_types.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/abstractions/ble_hal/include/</path>
    <filename>bt__hal__manager__types_8h.html</filename>
    <class kind="struct">BTBdaddr_t</class>
    <class kind="struct">BTBdname_t</class>
    <class kind="struct">BTUuid_t</class>
    <member kind="define">
      <type>#define</type>
      <name>bt128BIT_UUID_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gad96edc6c251bcd5ce11768e6ef35460f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btADDRESS_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga04d807f006570ebb70f2bf530110b83e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTuuidType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga993ffac0caf6422a8419fe29dc02cf95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType16</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95af00ee220bbb21a208dea0db4c193ff38</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType32</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95a2e16322049fce6a01e12fe59b10b1985</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType128</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95a5e14440c8f890ba846e4b85bf299741e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga291acc452b2f23af88200b307f42fe7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da49b793cac6089c931ffd4b14d5c113d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da9cf206edeab8259ddaf8e49634cdc42d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusNotReady</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7dae48901aeb6bcad08bd5efb1e075eee55</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusNoMem</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daf5c8e6b4a3d1bde5773063f22a964fa0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusBusy</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da1622349c03760d0560dbf1b5776c8c72</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusDone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da8f728a7ac3567c2ecb28e84fe5870455</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusUnsupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daa81031081ee2b35ab432f73a00525c6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusParamInvalid</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da9e7af3fc8465c3d71ccf0f017ff260d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusUnHandled</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daa7c9eb7b4a4cd7e17c73c4654a79864c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusAuthFailure</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da46f85b08263c5464678f1e7f5d638ec5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusRMTDevDown</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7dae24c6829afe84e49ff66f912e57577a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusAuthRejected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da23cde6d940b0c57cc1c8c8bac319c148</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusWakeLockError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da5d149dd53538aba2b3aa577c7d008849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusLinkLoss</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da13e343a720acc82bb0fc79cf9ee3baeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTProfile_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3d56f24cb4a721748184d2f361bd3d74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eProfileHeadset</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aa038e4964a2593a2c8b891544c9dd5e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileA2DP</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a5cf777e07744fe8513cbea2965f53b62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileAVRCPTarget</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aaaeb18992fda3dd49d54243cd80023e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileA2DPSink</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a08572d36f2b3acaf243c750c3574bce9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileAVRCPController</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aebb301a785f08ad218eacd030c92421b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileInputDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a02d3336f3afe1ec8c1dff71ea3259658</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileHeadsetClient</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a1aaae56b98af7d53229474d777ad3777</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileGATT</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74af9d48553da005c4a98865659df3feb05</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileSock</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a2c1b4122c7f7c93211dc8c7fc12b0dd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileMaxID</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74ac596b44fff1c19e6246ef8c0c8feda42</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTIOtypes_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga646cea95f6a31da98ac491d843b23bbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIONone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaf435a06183502b7b1461949f4d482481</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIODisplayOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaae38272bc4f06ca91fbc96e6ce246537</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIODisplayYesNo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbba0cee846f770b51a844df73cdd698f278</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIOKeyboardOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbba45fff80dceef959356013afc0b1a1ac3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIOKeyboardDisplay</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaea5c462144661f39066c827678353413</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_ble.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/ble/include/</path>
    <filename>iot__ble_8h.html</filename>
    <includes id="bt__hal__manager__adapter__ble_8h" name="bt_hal_manager_adapter_ble.h" local="yes" imported="no">bt_hal_manager_adapter_ble.h</includes>
    <includes id="bt__hal__manager_8h" name="bt_hal_manager.h" local="yes" imported="no">bt_hal_manager.h</includes>
    <includes id="bt__hal__gatt__server_8h" name="bt_hal_gatt_server.h" local="yes" imported="no">bt_hal_gatt_server.h</includes>
    <includes id="bt__hal__gatt__types_8h" name="bt_hal_gatt_types.h" local="yes" imported="no">bt_hal_gatt_types.h</includes>
    <class kind="struct">IotBleAdvertisementParams_t</class>
    <class kind="struct">IotBleConnectionParam_t</class>
    <class kind="struct">IotBleConnectionInfoListElement_t</class>
    <class kind="struct">IotBleReadEventParams_t</class>
    <class kind="struct">IotBleWriteEventParams_t</class>
    <class kind="struct">IotBleExecWriteEventParams_t</class>
    <class kind="struct">IotBleRespConfirmEventParams_t</class>
    <class kind="struct">IotBleIndicationSentEventParams_t</class>
    <class kind="struct">IotBleAttributeEvent</class>
    <class kind="struct">IotBleAttributeData_t</class>
    <class kind="struct">IotBleEventResponse_t</class>
    <class kind="union">IotBleEventsCallbacks_t</class>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBleAttributeEventCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga7c0570c3f4d70b37ce4f5e29e313790c</anchor>
      <arglist>)(IotBleAttributeEvent_t *pEventParam)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_MtuChangedCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>gacad43808d7fce001db06e2563c3bacdd</anchor>
      <arglist>)(uint16_t connId, uint16_t mtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_ConnectionCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga525923992426a85caaf9d94206be197b</anchor>
      <arglist>)(BTStatus_t status, uint16_t connId, bool connected, BTBdaddr_t *pRemoteBdAddr)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_StartAdvCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0d75a95f024bb24f0ca438e3be0bd598</anchor>
      <arglist>)(BTStatus_t status)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_StopAdvCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0a1efc291ddcc545882a811b4ed59949</anchor>
      <arglist>)(BTStatus_t status)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_ConnParameterUpdateRequestCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga44c0d8c0a31c05266fdadb9d337e8443</anchor>
      <arglist>)(BTStatus_t status, const BTBdaddr_t *pRemoteBdAddr, IotBleConnectionParam_t *pConnectionParam, uint32_t connInterval)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_PairingStateChanged_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0a4531445768f3e3ecbad1700b2621a6</anchor>
      <arglist>)(BTStatus_t status, BTBdaddr_t *pRemoteBdAddr, BTBondState_t bondstate, BTSecurityLevel_t securityLevel, BTAuthFailureReason_t reason)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_NumericComparisonCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga1348d7caea520883b602a7aa404701d8</anchor>
      <arglist>)(BTBdaddr_t *pRemoteBdAddr, uint32_t passKey)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotBleAttributeEventType_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga5f7906be0497dbfff2d6f0232692f455</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLERead</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a74d3d77d4cc00629948926b2a63f66cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWrite</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a8e4a5a1ee31e67089f996a6f8206da6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWriteNoResponse</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ac4d7e6abb98e9967558b6000fadb64b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWritePrepare</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ab984f01b9221efa802906ff0b466d09c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEExecWrite</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a2f28c750c441ee258613a73df7e5ec7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEResponseConfirmation</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a8dac371f5c542c3f06fc7d8e56be9f6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEIndicationConfirmReceived</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ae2da4744c8347eb1ff2a9619b85ccfd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotBleEvents_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga3d9455e1f3d2f24b51ad7056465a1604</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEMtuChanged</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604a388d9ff53be0dda10b21a9f52803b36c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEConnection</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604ac9ccc26c3baccd6fbe384523fbb7b490</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEPairingStateChanged</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604ae1ebeb0e49c74e87b55b9cc38469db7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEConnParameterUpdateRequestCallback</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604a743eeb6541e030b17a3871474ba8582d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLENumericComparisonCallback</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604adc1aeff6aeeef08965e0f65ddb587b15</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eNbEvents</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604af6b68dafa3f892847b977b990ca8db4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_Init</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>ade83d07296b5cdad211ae9cd96cfe064</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_On</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>ac97ca82425289e3840ad4ccb83864a15</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_Off</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>aca0019d8f476b1c1e8c6044400a3ad7c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_StartAdv</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a628611b3d51dc809e6167a99a872847b</anchor>
      <arglist>(IotBle_StartAdvCallback_t pStartAdvCb)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_StopAdv</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>ac22c547802476ba6f119557fc56bf5be</anchor>
      <arglist>(IotBle_StopAdvCallback_t pStopAdvCb)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_ConnParameterUpdateRequest</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a73abc07c725aa0ddafa9ba462460bc0b</anchor>
      <arglist>(const BTBdaddr_t *pRemoteBdAddr, IotBleConnectionParam_t *pConnectionParam)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_RegisterEventCb</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a27edf4d60d70acb16fcb6708d7e7b2a8</anchor>
      <arglist>(IotBleEvents_t event, IotBleEventsCallbacks_t bleEventsCallbacks)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_UnRegisterEventCb</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>add73ca3ce3e2aa52e229a2a1b6642af9</anchor>
      <arglist>(IotBleEvents_t event, IotBleEventsCallbacks_t bleEventsCallbacks)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_RemoveBond</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a0df1acf75c563b14db74ba5f8eadeca7</anchor>
      <arglist>(const BTBdaddr_t *pRemoteBdAddr)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_CreateService</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>aede9ad9426e3cc67ff2a0ed68cd34263</anchor>
      <arglist>(BTService_t *pService, IotBleAttributeEventCallback_t pEventsCallbacks[])</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_DeleteService</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>aad04211ea49c493c139f7feb9d71321f</anchor>
      <arglist>(BTService_t *pService)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_SendIndication</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>aff4ba69e2aaece0ead3d92fd178d03b3</anchor>
      <arglist>(IotBleEventResponse_t *pResp, uint16_t connId, bool confirm)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_SendResponse</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>add2372e66891feafc6d2dc08c0e61b26</anchor>
      <arglist>(IotBleEventResponse_t *pResp, uint16_t connId, uint32_t transId)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_GetConnectionInfoList</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a0785831b636702a465824b5fe8e03b72</anchor>
      <arglist>(IotLink_t **pConnectionInfoList)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_GetConnectionInfo</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a5a0382ccd2d97c4602d248d2ee7491c4</anchor>
      <arglist>(uint16_t connId, IotBleConnectionInfoListElement_t **pConnectionInfo)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_ConfirmNumericComparisonKeys</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a05814e489f94bd5498f606c25bed2ea4</anchor>
      <arglist>(BTBdaddr_t *pBdAddr, bool keyAccepted)</arglist>
    </member>
    <member kind="function">
      <type>BTStatus_t</type>
      <name>IotBle_SetDeviceName</name>
      <anchorfile>iot__ble_8h.html</anchorfile>
      <anchor>a45508b90c9b63624b84021f4a3d69227</anchor>
      <arglist>(const char *pName, size_t length)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_ble_config_defaults.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/ble/include/</path>
    <filename>iot__ble__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_SERVER_UUID</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a401b940ab17fff11bdf065d60678d300</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DEVICE_INFO_SERVICE_UUID_MASK</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a891f7f5d7db8c07c161b5973c2748c75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_SET_CUSTOM_ADVERTISEMENT_MSG</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>adb51d17729237841daa174b4a6efe83e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADVERTISING_UUID</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>aabc99be879a7da8072fde93bee405850</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADVERTISING_CONN_INTERVAL_MIN</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ae4a9fcd6a9feb3a0f9552f338f00cd26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADVERTISING_CONN_INTERVAL_MAX</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>aa44f35108266f3bb39f620d2d09cf8dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADVERTISING_INTERVAL</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a7517767cc75bc22ac5ab5792e8897f4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADVERTISING_APPEARANCE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a63023edaa5a458b1d57df75c9d665cf1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_INPUT_OUTPUT</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>acd37fe616cc836749a86e2113ba09728</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DEVICE_SHORT_LOCAL_NAME_SIZE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a6fc0952bd37f9c6bff3389eb2cf70e2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DEVICE_COMPLETE_LOCAL_NAME</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a8b84a64d78dd060f47709417fd140656</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DEVICE_LOCAL_NAME_MAX_LENGTH</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ac6afc28664e165e56d0d5bd172b8cafa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_PREFERRED_MTU_SIZE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a929d1c89833a6b89cd9ef5380306a4b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_BONDING</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ac3eab9fc3fbbd7d5034c99d6bc5161f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_SECURE_CONNECTION</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>af85d3ce2ba6814500a68299680b72bd2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENCRYPTION_REQUIRED</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ac744224d194e48d3c6d98b9abb93e8ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_NUMERIC_COMPARISON</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a33a6a960bb5fcbdfa365cd930ecbfde8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a321f4222ada6d6c8d622d580a0acda94</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_MAX_BONDED_DEVICES</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ae1cb76aa4b4225cfc5a6c7e26d1cf897</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_FREERTOS_GATT_SERVICES</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a2824d63231db67a0e51aafc54307fd60</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ADD_CUSTOM_SERVICES</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a1f5d868ae768c0d9c96f2d048e37bb2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_DEVICE_INFO_SERVICE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a234a3364c19fde59ed32c73131f22911</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_WIFI_PROVISIONING</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>afa9651a036a42568a49c75e720013b26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_MQTT</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a333e88ee64b16ec5d526f73c5cd4c737</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a9346542811e2f6ad44ed8aa17501c4bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_WIFI_PROVISIONIG_MAX_SCAN_NETWORKS</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>aa6fa91ab7b7728e0777a35eac86b9567</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_MQTT_CREATE_CONNECTION_WAIT_MS</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ad7330b8dc3d2b9dc8485682e77b7d593</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_MQTT_CREATE_CONNECTION_RETRY</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a01acff64f11a04887791d01f348714de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>aee310e8db7e41f59e2e64af4b629edb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>afb359c15bda91cf2eaecba511bf6f0a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DATA_TRANSFER_TX_BUFFER_SIZE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a1a12d02fd17596f054e78cab43f30cd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a3f5247c4529428914b75dc5d2546cb75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_BLE_DATA_TRANSFER_TIMEOUT_MS</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>af2eff410b9cce27e587cda10d2a8dad3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IotBle_MallocDataBuffer</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>ab7632daf3c8c81912204e3842105f03f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IotBle_Assert</name>
      <anchorfile>iot__ble__config__defaults_8h.html</anchorfile>
      <anchor>a4c9a3feef5ae461a687d28a1f2e3c80a</anchor>
      <arglist>(expression)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_ble_device_information.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/ble/include/</path>
    <filename>iot__ble__device__information_8h.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>IotBleDeviceInfo_Init</name>
      <anchorfile>iot__ble__device__information_8h.html</anchorfile>
      <anchor>aa76a2ae4d4a5d394cb9257f93fe0f323</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotBleDeviceInfo_Cleanup</name>
      <anchorfile>iot__ble__device__information_8h.html</anchorfile>
      <anchor>af7d7ad7506c4d3e36a121009557e4dff</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_ble_mqtt_serialize.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/ble/include/</path>
    <filename>iot__ble__mqtt__serialize_8h.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>MQTTBLEConnectStatus_t</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68be</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_UNKNOWN</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68bea6e4e94c4e0edd18e45a196d1a7ac95ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_CONNECTING</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68beab482bf4e98f28528f847606469d6ac7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_CONNECTED</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68beaf4f207b83375d4d4e4cb37c5b5b63f01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_DISCONNECTED</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68bea3eae52801ced531b77ae700ce6f75d65</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_CONNECTION_REFUSED</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68beac923aab3a8d2ea94aa8c75a60f051d18</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_CONNECTION_ERROR</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68bea3983e484f8bb471c6de724c4e6538f1d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_MQTT_STATUS_PROTOCOL_ERROR</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63bd7f18059168d2bc02c2acb31f68bea1b0172303bbfc6816d3277d80fc327ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotBleMqtt_InitSerialize</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ae1ce7acb754ae523a3db1ce720adfbd3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotBleMqtt_CleanupSerialize</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a70917c7c670029109b17c322de91ad93</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializeConnect</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a4a5d60c7ddc7182a69d8fe27c8c5f58c</anchor>
      <arglist>(const IotMqttConnectInfo_t *const pConnectInfo, uint8_t **const pConnectPacket, size_t *const pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializeConnack</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ad79230f17ef1e69f88952bde0fd9153d</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializePublish</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a63babe4a0315fdcc8483558134d3f88d</anchor>
      <arglist>(const IotMqttPublishInfo_t *const pPublishInfo, uint8_t **const pPublishPacket, size_t *const pPacketSize, uint16_t *const pPacketIdentifier, uint8_t **pPacketIdentifierHigh)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotBleMqtt_PublishSetDup</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ad9fad59c7928363a8b07bd64312d77f0</anchor>
      <arglist>(uint8_t *const pPublishPacket, uint8_t *pPacketIdentifierHigh, uint16_t *const pNewPacketIdentifier)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializePublish</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>adc14fee1d10dd3dbfb5a9543bd07d5e8</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializePuback</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a2ba8fa75cf00ec770289e4d6b4eb6bd3</anchor>
      <arglist>(uint16_t packetIdentifier, uint8_t **const pPubackPacket, size_t *const pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializePuback</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>aa0e22c223cbfb1994c40ecf15191ac8f</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializeSubscribe</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ab7177df39a9c58157befd44a1b689cac</anchor>
      <arglist>(const IotMqttSubscription_t *const pSubscriptionList, size_t subscriptionCount, uint8_t **const pSubscribePacket, size_t *const pPacketSize, uint16_t *const pPacketIdentifier)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializeSuback</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ad553892050cce93ba93ae2c6f9b34c9f</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializeUnsubscribe</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a1d7d7fcfb8023b64f0d9ddaece490c2f</anchor>
      <arglist>(const IotMqttSubscription_t *const pSubscriptionList, size_t subscriptionCount, uint8_t **const pUnsubscribePacket, size_t *const pPacketSize, uint16_t *const pPacketIdentifier)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializeUnsuback</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a70460d75d63e94bd1b06b09d69ef78f0</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializeDisconnect</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>ad13a863acc5039393a15a1241d9d79cd</anchor>
      <arglist>(uint8_t **const pDisconnectPacket, size_t *const pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_SerializePingreq</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a5fbc2a01fd6cf474ac2f83b2e1fa5487</anchor>
      <arglist>(uint8_t **const pPingreqPacket, size_t *const pPacketSize)</arglist>
    </member>
    <member kind="function">
      <type>IotMqttError_t</type>
      <name>IotBleMqtt_DeserializePingresp</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a0cfabf38528c3911eaae9f1a95a86c4d</anchor>
      <arglist>(struct _mqttPacket *pConnack)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>IotBleMqtt_GetPacketType</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a34b0ce0cd5c5a453c390eb3abec96769</anchor>
      <arglist>(void *pNetworkConnection, const IotNetworkInterface_t *pNetworkInterface)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>IotBleMqtt_GetRemainingLength</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a1b828c7d5a8ab3d4e63ef232e8bc02de</anchor>
      <arglist>(void *pNetworkConnection, const IotNetworkInterface_t *pNetworkInterface)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotBleMqtt_FreePacket</name>
      <anchorfile>iot__ble__mqtt__serialize_8h.html</anchorfile>
      <anchor>a78d4ac62e8c0f3dac2524722be91eb3c</anchor>
      <arglist>(uint8_t *pPacket)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_ble_wifi_provisioning.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/ble/include/</path>
    <filename>iot__ble__wifi__provisioning_8h.html</filename>
    <includes id="iot__ble_8h" name="iot_ble.h" local="yes" imported="no">iot_ble.h</includes>
    <class kind="struct">IotBleListNetworkRequest_t</class>
    <class kind="struct">IotBleAddNetworkRequest_t</class>
    <class kind="struct">IotBleEditNetworkRequest_t</class>
    <class kind="struct">IotBleDeleteNetworkRequest_t</class>
    <class kind="struct">IotBleWifiNetworkInfo_t</class>
    <class kind="struct">IotBleWifiProvService_t</class>
    <member kind="enumeration">
      <type></type>
      <name>IotBleWifiProvEvent_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga442627b957005b6175227e879739eb8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_CONNECT</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8ea9c46bd8733d5e34c039bbe4cfac2d496</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_CONNECTED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eaaece6cd81e0648ce449212d3330942d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_DELETED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eac95c9f9dc426a86fe45a98a73fd5dc3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_FAILED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eaf82bcfc63005b4d6abbf0ee9b4cd73fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotBleWifiProv_Init</name>
      <anchorfile>iot__ble__wifi__provisioning_8h.html</anchorfile>
      <anchor>a49d56c5fe85285190b3dc9d5f230f1d6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint32_t</type>
      <name>IotBleWifiProv_GetNumNetworks</name>
      <anchorfile>iot__ble__wifi__provisioning_8h.html</anchorfile>
      <anchor>a92c504b45072ebd7346e55af4a87d1f2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotBleWifiProv_Connect</name>
      <anchorfile>iot__ble__wifi__provisioning_8h.html</anchorfile>
      <anchor>af75cc1194ba3f6e2b64c0757b9b2c149</anchor>
      <arglist>(uint32_t networkIndex)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>IotBleWifiProv_EraseAllNetworks</name>
      <anchorfile>iot__ble__wifi__provisioning_8h.html</anchorfile>
      <anchor>a21522bf09f97005f7faef993769ae3a2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotBleWifiProv_Deinit</name>
      <anchorfile>iot__ble__wifi__provisioning_8h.html</anchorfile>
      <anchor>adefbe341c865785f5d5b8fb65a80fd3b</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTActivityEnergyInfo</name>
    <filename>struct_b_t_activity_energy_info.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucCtrlState</name>
      <anchorfile>struct_b_t_activity_energy_info.html</anchorfile>
      <anchor>ac2f82f65206972c98209029f25cadf70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullTxTime</name>
      <anchorfile>struct_b_t_activity_energy_info.html</anchorfile>
      <anchor>a24452b384c1db85686281a7551c5077f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullRxTime</name>
      <anchorfile>struct_b_t_activity_energy_info.html</anchorfile>
      <anchor>a01ae3cc97d86afa472925cf89f940675</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullIdleTime</name>
      <anchorfile>struct_b_t_activity_energy_info.html</anchorfile>
      <anchor>ae39c0734fd25e9b0fc72389d0cc47799</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>ullEnergyUsed</name>
      <anchorfile>struct_b_t_activity_energy_info.html</anchorfile>
      <anchor>afc6af5ddecc2b72118e2bf6b8ae00672</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTAttribute_t</name>
    <filename>struct_b_t_attribute__t.html</filename>
    <member kind="variable">
      <type>BTDbAttributeType_t</type>
      <name>xAttributeType</name>
      <anchorfile>struct_b_t_attribute__t.html</anchorfile>
      <anchor>a0490d026eccb9eb26efff5ad18aadbf2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTServiceUUID_t</type>
      <name>xServiceUUID</name>
      <anchorfile>struct_b_t_attribute__t.html</anchorfile>
      <anchor>ac72404ae71f0eead3501f7b23ffd99df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTCharacteristic_t</type>
      <name>xCharacteristic</name>
      <anchorfile>struct_b_t_attribute__t.html</anchorfile>
      <anchor>a7c4b50f5ef7d7fc2afa8693e620477e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTCharacteristicDescr_t</type>
      <name>xCharacteristicDescr</name>
      <anchorfile>struct_b_t_attribute__t.html</anchorfile>
      <anchor>a25e48bff16215bc3dbaaeb949487641a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTIncludedService_t</type>
      <name>xIncludedService</name>
      <anchorfile>struct_b_t_attribute__t.html</anchorfile>
      <anchor>a5684b929ebe610fa4d425c26931abbed</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTAvsrcCallbacks_t</name>
    <filename>struct_b_t_avsrc_callbacks__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTAvsrcInterface_t</name>
    <filename>struct_b_t_avsrc_interface__t.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>xSize</name>
      <anchorfile>struct_b_t_avsrc_interface__t.html</anchorfile>
      <anchor>adf99d6299c1a7a7c0b1c86804b025127</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAvsrcInit</name>
      <anchorfile>struct_b_t_avsrc_interface__t.html</anchorfile>
      <anchor>afab188e3545baa93177bad7d362b0ad5</anchor>
      <arglist>)(BTAvsrcCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAvsrcCleanup</name>
      <anchorfile>struct_b_t_avsrc_interface__t.html</anchorfile>
      <anchor>a5b7fc14acca33ab23c74adf7ff36b687</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAvsrcConnect</name>
      <anchorfile>struct_b_t_avsrc_interface__t.html</anchorfile>
      <anchor>a504b5523d62dd77f63dc559403258225</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAvsrcDisconnect</name>
      <anchorfile>struct_b_t_avsrc_interface__t.html</anchorfile>
      <anchor>a872ea381234d943c1de222d0c4a3171c</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTBdaddr_t</name>
    <filename>struct_b_t_bdaddr__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAddress</name>
      <anchorfile>struct_b_t_bdaddr__t.html</anchorfile>
      <anchor>af6565cbb89d5532f75373f3998caa7f9</anchor>
      <arglist>[btADDRESS_LEN]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTBdname_t</name>
    <filename>struct_b_t_bdname__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>ucName</name>
      <anchorfile>struct_b_t_bdname__t.html</anchorfile>
      <anchor>ac469e66c279b8c9805488d8ed82bcb49</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTBleAdapter_t</name>
    <filename>struct_b_t_ble_adapter__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRegisterBleApp</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>addef45cdd3d43935077a54b6b76fbdba</anchor>
      <arglist>)(BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxUnregisterBleApp</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>aa21208374d468d11386a595d9928e747</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBleAdapterInit</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a6bd60d7a43e923a2f7ef5e20bb4d0235</anchor>
      <arglist>)(const BTBleAdapterCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetBleAdapterProperty</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a1eb6db2ff6a04d9d8be91d4a2f2fdbef</anchor>
      <arglist>)(BTBlePropertyType_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetBleAdapterProperty</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a13a2f178b09f6a985c047254e680faf4</anchor>
      <arglist>)(const BTBleProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetallBleRemoteDeviceProperties</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a2b86e511ba34faf8db6da628946a4ff8</anchor>
      <arglist>)(BTBdaddr_t *pxRremoteAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetBleRemoteDeviceProperty</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a3e2d34ddf2467ff49167d4a94e927ebd</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteAddr, BTBleProperty_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetBleRemoteDeviceProperty</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a3fd6065a3bf8cc3d81cba2a00e4ba6d5</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteAddr, const BTBleProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxScan</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a068332a79972ba59d0d95bae35b1dab6</anchor>
      <arglist>)(bool bStart)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConnect</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>ad5a663a44d54bcc1ae8df59ab3f353e3</anchor>
      <arglist>)(uint8_t ucAdapterIf, const BTBdaddr_t *pxBdAddr, bool bIsDirect, BTTransport_t ulTransport)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDisconnect</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a6df02479e589504b4f193e08417af911</anchor>
      <arglist>)(uint8_t ucAdapterIf, const BTBdaddr_t *pxBdAddr, uint16_t usConnId)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStartAdv</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a505539d0548a450f7cc884b08163910a</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStopAdv</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a2af8a8de88c839fe106ad5cc83c3dce9</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadRemoteRssi</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>ab40478ce0d174fcb617946be361de468</anchor>
      <arglist>)(uint8_t ucAdapterIf, const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxScanFilterParamSetup</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a0c730d8dfab39f9adb89b1a6e753f8c2</anchor>
      <arglist>)(BTGattFiltParamSetup_t xFiltParam)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxScanFilterAddRemove</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>ae9576dd67f911623cbfc27b3e5dd5837</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulAction, uint32_t ulFiltType, uint32_t ulFiltIndex, uint32_t ulCompanyId, uint32_t ulCompanyIdMask, const BTUuid_t *pxUuid, const BTUuid_t *pxUuidMask, const BTBdaddr_t *pxBdAddr, char cAddrType, size_t xDataLen, char *pcData, size_t xMaskLen, char *pcMask)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxScanFilterClear</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a9888999ee6b5e256b5c3964af9e6d415</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulFiltIndex)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxScanFilterEnable</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a741983736e6473936fe3e7a58ed05cb7</anchor>
      <arglist>)(uint8_t ucAdapterIf, bool bEnable)</arglist>
    </member>
    <member kind="variable">
      <type>BTTransport_t(*</type>
      <name>pxGetDeviceType</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a7caf16bb2f17673838ba6eb6ed531125</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetAdvData</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>aa0cc5805905b745c23f02b453c36acd9</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTGattAdvertismentParams_t *pxParams, uint16_t usManufacturerLen, char *pcManufacturerData, uint16_t usServiceDataLen, char *pcServiceData, BTUuid_t *pxServiceUuid, size_t xNbServices)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetAdvRawData</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a3e89c90b6ab2e54fe6c623ab0f5461dd</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint8_t *pucData, uint8_t ucLen)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConnParameterUpdateRequest</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a5376321c31e5f4bede276c0c61ae7d93</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, uint32_t ulMinInterval, uint32_t ulMaxInterval, uint32_t ulLatency, uint32_t ulTimeout)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetScanParameters</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a6ceb408f13a53056290ae6894e76b063</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulScanInterval, uint32_t ulScanWindow)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxMultiAdvEnable</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a8c49cc1a271c06816434bc6950e1af50</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTGattAdvertismentParams_t *xAdvParams)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxMultiAdvUpdate</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a17b460080ec124eb28f32851287775b9</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTGattAdvertismentParams_t *advParams)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxMultiAdvSetInstData</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a57990461e0f288d89cfff49689e197ab</anchor>
      <arglist>)(uint8_t ucAdapterIf, bool bSetScanRsp, bool bIncludeName, bool bInclTxpower, uint32_t ulAppearance, size_t xManufacturerLen, char *pcManufacturerData, size_t xServiceDataLen, char *pcServiceData, BTUuid_t *pxServiceUuid, size_t xNbServices)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxMultiAdvDisable</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>af582e28c5d0e35aa449f1354c0e2d54e</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBatchscanCfgStorage</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a21ec23a55f752e7336136ff5f607ab70</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulBatchScanFullMax, uint32_t ulBatchScanTruncMax, uint32_t ulBatchScanNotifyThreshold)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBatchscanEndBatchScan</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a6e08a2e7717c948afdfdcb658b220728</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulScanMode, uint32_t ulScanInterval, uint32_t ulScanWindow, uint32_t ulAddrType, uint32_t ulDiscardRule)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBatchscanDisBatchScan</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a84ceb88c8abc5b9dd53c0137f979df7b</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBatchscanReadReports</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>aabb17363807ee84b1475a4db53191323</anchor>
      <arglist>)(uint8_t ucAdapterIf, uint32_t ulScanMode)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetPreferredPhy</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a528a6ee5ec873487bf1433e884e8218f</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, uint16_t usPhyOptions)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadPhy</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>ad777cd2d6d58ab4afa20948679796a67</anchor>
      <arglist>)(uint16_t usConnId, BTReadClientPhyCallback_t xCb)</arglist>
    </member>
    <member kind="variable">
      <type>const void *(*</type>
      <name>ppvGetGattClientInterface</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>ab40b4a0138039e7bded7785891d4fbf7</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>const void *(*</type>
      <name>ppvGetGattServerInterface</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a8db4b104bf77aa46447f89758c531693</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxMultiAdvSetInstRawData</name>
      <anchorfile>struct_b_t_ble_adapter__t.html</anchorfile>
      <anchor>a67c05fbf2b9154592ade62bc0a974bee</anchor>
      <arglist>)(uint8_t ucAdapterIf, bool bSetScanRsp, uint8_t *pucData, size_t xDataLen)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTBleAdapterCallbacks_t</name>
    <filename>struct_b_t_ble_adapter_callbacks__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTBleProperty_t</name>
    <filename>struct_b_t_ble_property__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTCallbacks_t</name>
    <filename>struct_b_t_callbacks__t.html</filename>
    <member kind="variable">
      <type>BTDeviceStateChangedCallback_t</type>
      <name>pxDeviceStateChangedCb</name>
      <anchorfile>struct_b_t_callbacks__t.html</anchorfile>
      <anchor>a871f4bd7c75e5b5202631d665d25f57b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTDutModeRecvCallback_t</type>
      <name>pxDutModeRecvCb</name>
      <anchorfile>struct_b_t_callbacks__t.html</anchorfile>
      <anchor>a72592ac46fdae13e07d12d21a3240ed7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTCharacteristic_t</name>
    <filename>struct_b_t_characteristic__t.html</filename>
    <member kind="variable">
      <type>BTUuid_t</type>
      <name>xUuid</name>
      <anchorfile>struct_b_t_characteristic__t.html</anchorfile>
      <anchor>ac14f5c53b54a96e534d6afe49ccf6e09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTCharProperties_t</type>
      <name>xProperties</name>
      <anchorfile>struct_b_t_characteristic__t.html</anchorfile>
      <anchor>a37b8821900e810fb2e85d5f53b96d905</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTCharPermissions_t</type>
      <name>xPermissions</name>
      <anchorfile>struct_b_t_characteristic__t.html</anchorfile>
      <anchor>ade872337b5de7404c5bf49fc512fb095</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTCharacteristicDescr_t</name>
    <filename>struct_b_t_characteristic_descr__t.html</filename>
    <member kind="variable">
      <type>BTUuid_t</type>
      <name>xUuid</name>
      <anchorfile>struct_b_t_characteristic_descr__t.html</anchorfile>
      <anchor>a34dc2401254420655cce112977399426</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTCharPermissions_t</type>
      <name>xPermissions</name>
      <anchorfile>struct_b_t_characteristic_descr__t.html</anchorfile>
      <anchor>a7722f6379b92d5e33dc6fe7348872ef7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTClassicCallbacks_t</name>
    <filename>struct_b_t_classic_callbacks__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTClassicInterface_t</name>
    <filename>struct_b_t_classic_interface__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxInit</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a88053950a6f981cb715389ad3a4c0119</anchor>
      <arglist>)(BTClassicCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxCleanup</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>abe2556a4787c6d8b0d33fb1dfb4e6a97</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetClassicDeviceProperty</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a477374e69eb64ec00ee66741e268ff86</anchor>
      <arglist>)(BTClassicPropertyType_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetClassicDeviceProperty</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a0fcf10976da44c78fb1bf9deecb01660</anchor>
      <arglist>)(const BTClassicProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetAllRemoteClassicDeviceProperties</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>ac6ea765d6002a43a411e73d1117b25bc</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetRemoteClassicDeviceProperty</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>abfa7ff3d3adf9d13641e99e481172f3d</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr, BTClassicPropertyType_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetRemoteClassicDeviceProperty</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a9e2dbe703680e95e7a4fe76f89961e59</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr, const BTClassicProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetRemoteServiceRecord</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a16c45c9b3b629dd85085cc4123db4213</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr, BTUuid_t *pxUuid)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetRemoteService</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a7a78014ef47b28958e071036434f093f</anchor>
      <arglist>)(BTBdaddr_t *pxRemote_addr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStartDiscovery</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>ac4163ca0b2bc566dbef7668740eb186d</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStopDiscovery</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>ad4d1d76849e4eda7cca3a1b1605d8c8f</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>const void *(*</type>
      <name>ppvGetProfileInterface</name>
      <anchorfile>struct_b_t_classic_interface__t.html</anchorfile>
      <anchor>a05783af9fc4f32efbc09e7eb1783d518</anchor>
      <arglist>)(BTProfile_t xProfile)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTClassicProperty_t</name>
    <filename>struct_b_t_classic_property__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTEirManfInfo_t</name>
    <filename>struct_b_t_eir_manf_info__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGadgetEirManfInfo_t</name>
    <filename>struct_b_t_gadget_eir_manf_info__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattAdvertismentParams_t</name>
    <filename>struct_b_t_gatt_advertisment_params__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulMinInterval</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>a7d2702c29ab78d5a30b4496ad28e7585</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>ulMaxInterval</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>aaa1e132471e56cdd9f11fc5ea1f88802</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usMinAdvInterval</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>a2e0cd8e37076b5c95c699f1925f3c0d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usMaxAdvInterval</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>a9006a6b1fbfc95955bccbf124647ea12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTimeout</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>a79c89246bcae244c95ad3f3c6fd39a97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usTimeout</name>
      <anchorfile>struct_b_t_gatt_advertisment_params__t.html</anchorfile>
      <anchor>add2f8b2ff2f32c63e4743fa2dd9b7f19</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattAdvName_t</name>
    <filename>struct_b_t_gatt_adv_name__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattClientCallbacks_t</name>
    <filename>struct_b_t_gatt_client_callbacks__t.html</filename>
    <member kind="variable">
      <type>BTConfigureMtuCallback_t</type>
      <name>pxConfigureMtuCb</name>
      <anchorfile>struct_b_t_gatt_client_callbacks__t.html</anchorfile>
      <anchor>a22f23b5264bb270cdeed02a24c62d9c5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattClientInterface_t</name>
    <filename>struct_b_t_gatt_client_interface__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRegisterClient</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>ace140516115fcaeb609ed5bba59e421a</anchor>
      <arglist>)(BTUuid_t *pxUuid)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxUnregisterClient</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a4f307a60d9e27e05ee0856305f21558b</anchor>
      <arglist>)(uint8_t ucClientIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGattClientInit</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a157a0a97992ab6d65ffc16262df01e2c</anchor>
      <arglist>)(const BTGattClientCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConnect</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a93f0d55a911b639861b2fda1931f7fed</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr, bool bIsDirect, BTTransport_t xTransport)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDisconnect</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a941bccba2346b9564987288d6d8fe5d1</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr, uint16_t usConnId)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRefresh</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a4f44a4fb88c91649c904668519670933</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSearchService</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a3ddc673a573c0efcc3c7cb189d76df9f</anchor>
      <arglist>)(uint16_t usConnId, BTUuid_t *pxFilterUuid)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadCharacteristic</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a0bff1dbb5b51c9a857be04dfa37b8a2f</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usHandle, uint32_t ulAuthReq)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxWriteCharacteristic</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>aee7b57117100fda44d8e9962f104e27a</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usHandle, BTAttrWriteRequests_t xWriteType, size_t xLen, uint32_t ulAuthReq, char *pcValue)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadDescriptor</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a1267cb11db736bc0e10cc9ee73c24c5e</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usHandle, uint32_t ulAuthReq)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxWriteDescriptor</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>ad028a330802dc560e85cfee6c7cf9171</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usHandle, BTAttrWriteRequests_t xWriteType, size_t xLen, uint32_t ulAuthReq, char *pcValue)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxExecuteWrite</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>aaff1de355496bec6ee845aea6e82fda7</anchor>
      <arglist>)(uint16_t usConnId, bool bExecute)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRegisterForNotification</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a1ed681ec1589fe609490c2cf5ccb960e</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr, uint16_t usHandle)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxUnregisterForNotification</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a555f84ec182cc84ace7f2f45642947c4</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr, uint16_t usHandle)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadRemoteRssi</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>aa45e8614b41a39b7e587f47e3df34496</anchor>
      <arglist>)(uint8_t ucClientIf, const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTTransport_t(*</type>
      <name>pxGetDeviceType</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a6ed3d96c34ae904ac3acb2274095c35b</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConfigureMtu</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a9b8d6ff5f620ebc69c92107449196c8b</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usMtu)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxTestCommand</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a48fbcf8e8542b2519635ff89014d8592</anchor>
      <arglist>)(uint32_t ulCommand, BTGattTestParams_t *pxParams)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetGattDb</name>
      <anchorfile>struct_b_t_gatt_client_interface__t.html</anchorfile>
      <anchor>a4bc4a2276df913d96a89040e32b256f0</anchor>
      <arglist>)(uint16_t usConnId)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattDbElement_t</name>
    <filename>struct_b_t_gatt_db_element__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattFiltParamSetup_t</name>
    <filename>struct_b_t_gatt_filt_param_setup__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattInstanceId_t</name>
    <filename>struct_b_t_gatt_instance_id__t.html</filename>
    <member kind="variable">
      <type>BTUuid_t</type>
      <name>xUuid</name>
      <anchorfile>struct_b_t_gatt_instance_id__t.html</anchorfile>
      <anchor>a1cf0f7bf0056a488cb7d72b718d2ed27</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucInstId</name>
      <anchorfile>struct_b_t_gatt_instance_id__t.html</anchorfile>
      <anchor>a1f7290d77f969852a225403a70b2d438</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattNotifyParams_t</name>
    <filename>struct_b_t_gatt_notify_params__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattReadParams_t</name>
    <filename>struct_b_t_gatt_read_params__t.html</filename>
  </compound>
  <compound kind="union">
    <name>BTGattResponse_t</name>
    <filename>union_b_t_gatt_response__t.html</filename>
    <member kind="variable">
      <type>BTGattValue_t</type>
      <name>xAttrValue</name>
      <anchorfile>union_b_t_gatt_response__t.html</anchorfile>
      <anchor>ac6f3c90bbebe3eacb6c274a50f2f6958</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usHandle</name>
      <anchorfile>union_b_t_gatt_response__t.html</anchorfile>
      <anchor>a270240a577ab064b43664cede235f0ae</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattServerCallbacks_t</name>
    <filename>struct_b_t_gatt_server_callbacks__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattServerInterface_t</name>
    <filename>struct_b_t_gatt_server_interface__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRegisterServer</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>ace79c7eb11ecc50a2e28158ccf3d1d35</anchor>
      <arglist>)(BTUuid_t *pxUuid)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxUnregisterServer</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>aea0013422ff85dd35ac1d713596aa533</anchor>
      <arglist>)(uint8_t ucServerIf)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGattServerInit</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a8657c24e51266f2caf4227c0bce6a165</anchor>
      <arglist>)(const BTGattServerCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConnect</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a604badb727f28d4d375e396e27158c32</anchor>
      <arglist>)(uint8_t ucServerIf, const BTBdaddr_t *pxBdAddr, bool bIsDirect, BTTransport_t xTransport)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDisconnect</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>aa807b4c91beb84cd81a81842e250d245</anchor>
      <arglist>)(uint8_t ucServerIf, const BTBdaddr_t *pxBdAddr, uint16_t usConnId)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddServiceBlob</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a36cc3b00dca7446585c92e01f6ebc746</anchor>
      <arglist>)(uint8_t ucServerIf, BTService_t *pxService)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddService</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a6d227be94bd110228783a454f8d12387</anchor>
      <arglist>)(uint8_t ucServerIf, BTGattSrvcId_t *pxSrvcId, uint16_t usNumHandles)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddIncludedService</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>adc4dfffbd6e6459336a5969197b06e08</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle, uint16_t usIncludedHandle)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddCharacteristic</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>aa4051ed565cf82620c21189acc4fe12a</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t *pxUuid, BTCharProperties_t xProperties, BTCharPermissions_t xPermissions)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetVal</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a303aa146409e1755505887e71d28d2f4</anchor>
      <arglist>)(BTGattResponse_t *pxValue)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddDescriptor</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a21b8ee0642f5c2100d8fbf73eade737b</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle, BTUuid_t *pxUuid, BTCharPermissions_t ulPermissions)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStartService</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a6219d89024fa2601440cb3c84cf4a51a</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle, BTTransport_t xTransport)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxStopService</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>abf16e0b188e9b83b2c4e5cd925a60290</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDeleteService</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a916db733d6f94f8b28d778d979437a38</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSendIndication</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a2336460f9241c6fd5dcff7907f954756</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usAttributeHandle, uint16_t usConnId, size_t xLen, uint8_t *pucValue, bool bConfirm)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSendResponse</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a5092ffae6051aea65219cab7621ced9f</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTStatus_t xStatus, BTGattResponse_t *pxResponse)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReconnect</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>ade20662770f416f36c34f5e704e4556d</anchor>
      <arglist>)(uint8_t ucServerIf, const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxAddDevicesToWhiteList</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>ad62813d1f349bd458720403b21e2522d</anchor>
      <arglist>)(uint8_t ucServerIf, const BTBdaddr_t *pxBdAddr, uint32_t ulNumberOfDevices)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRemoveDevicesFromWhiteList</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a952dc138c428fb2ca070a32f69d9c5fc</anchor>
      <arglist>)(uint8_t ucServerIf, const BTBdaddr_t *pxBdAddr, uint32_t ulNumberOfDevices)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConfigureMtu</name>
      <anchorfile>struct_b_t_gatt_server_interface__t.html</anchorfile>
      <anchor>a77aab29dc44a5cbeacbfb78c2dd5b5fb</anchor>
      <arglist>)(uint8_t ucServerIf, uint16_t usMtu)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattSrvcId_t</name>
    <filename>struct_b_t_gatt_srvc_id__t.html</filename>
    <member kind="variable">
      <type>BTGattInstanceId_t</type>
      <name>xId</name>
      <anchorfile>struct_b_t_gatt_srvc_id__t.html</anchorfile>
      <anchor>ad440134db0ac0bfda80f6dc22c0519e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTGattServiceTypes_t</type>
      <name>xServiceType</name>
      <anchorfile>struct_b_t_gatt_srvc_id__t.html</anchorfile>
      <anchor>a511dbdc4ab4aa323d7ffe76509fc09ac</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattTestParams_t</name>
    <filename>struct_b_t_gatt_test_params__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattTrackAdvInfo_t</name>
    <filename>struct_b_t_gatt_track_adv_info__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucClientIf</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>ab6a37f12669573a7ad24b571807d5c50</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucFiltIndex</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>aeabdc217799b08220f95344e4fa9943f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAdvertiserState</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a4526e36767287b8f813cd175ea2f22d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAdvertiserInfoPresent</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a048510ef73d45f36542a80d11f072613</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAddrType</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>af3227f1664bfe68c72c2ad68f76796ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucTxPower</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a8da4134591434c7681dbf06967a4beaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int8_t</type>
      <name>cRssiValue</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a742b748d00199e76e6ea80cd67b2e747</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usTimeStamp</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a33ba1f1caa3e8751c221d9132f169d43</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTBdaddr_t</type>
      <name>xBdAddr</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a0311adbf055db532796c4e2d69f984c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucAdvPktLen</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>ae8a0c18ffb675101c46b6abfd526af33</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucAdvPktData</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a16e2947f31480ca6087ec5598b18981e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>xScanRspLen</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>a3f8a0506a7a25215736d9557a0520cff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucScanRspData</name>
      <anchorfile>struct_b_t_gatt_track_adv_info__t.html</anchorfile>
      <anchor>ab461bfa69c2f430c9bb8af7fc735b79e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattUnformattedValue_t</name>
    <filename>struct_b_t_gatt_unformatted_value__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTGattValue_t</name>
    <filename>struct_b_t_gatt_value__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usHandle</name>
      <anchorfile>struct_b_t_gatt_value__t.html</anchorfile>
      <anchor>a605dc7334f2197b0adbaf5ac03ac8f35</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pucValue</name>
      <anchorfile>struct_b_t_gatt_value__t.html</anchorfile>
      <anchor>acf8a8836892a1d9ea675095b93d60da5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>usOffset</name>
      <anchorfile>struct_b_t_gatt_value__t.html</anchorfile>
      <anchor>a73e46ec3c59812457268214f5fb757a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>xLen</name>
      <anchorfile>struct_b_t_gatt_value__t.html</anchorfile>
      <anchor>acd1b33c092cc58701390f5bbd1caf194</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTRspErrorStatus_t</type>
      <name>xRspErrorStatus</name>
      <anchorfile>struct_b_t_gatt_value__t.html</anchorfile>
      <anchor>a70c5add3ce41d62c1fa3301c6f31919c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTGattWriteParams_t</name>
    <filename>struct_b_t_gatt_write_params__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTIncludedService_t</name>
    <filename>struct_b_t_included_service__t.html</filename>
    <member kind="variable">
      <type>BTUuid_t</type>
      <name>xUuid</name>
      <anchorfile>struct_b_t_included_service__t.html</anchorfile>
      <anchor>a9dc372c249bbccb1294fda2f67cf64bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTService_t *</type>
      <name>pxPtrToService</name>
      <anchorfile>struct_b_t_included_service__t.html</anchorfile>
      <anchor>a23f71d33c39b2c21c3fe85ebe6577bfd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTInterface_t</name>
    <filename>struct_b_t_interface__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBtManagerInit</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a35411ff61ae85c3bf59e396e0a7a147e</anchor>
      <arglist>)(const BTCallbacks_t *pxCallbacks)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxBtManagerCleanup</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a84ace87abc6f01558c116c9ab269c399</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxEnable</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a637cfdb4ccda095fdbe899d969a86a9e</anchor>
      <arglist>)(uint8_t ucGuestMode)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDisable</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a09695eb134ba21021c7a3f873c2e6f84</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetAllDeviceProperties</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a202221f3a1481e0ec2b764143d42b5ca</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetDeviceProperty</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a8e12cdf8eac1486b68ea953ea41817be</anchor>
      <arglist>)(BTPropertyType_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetDeviceProperty</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a98ede5412caa0dc0ad858eb4fd4969ff</anchor>
      <arglist>)(const BTProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetAllRemoteDeviceProperties</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>ae14ee7deff2e5bd619afa4a20befd8f5</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetRemoteDeviceProperty</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a6b59f55623933f6bee44e25ccc89dacc</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteAddr, BTPropertyType_t xType)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSetRemoteDeviceProperty</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a6a33eaa2c3b0517705eb6c49a6153057</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteAddr, const BTProperty_t *pxProperty)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxPair</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a51bef55bdb320f5b67e22b3ad31c0a23</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, BTTransport_t xTransport, bool bCreateBond)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxCreateBondOutOfBand</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>ac8120e9e4377f0c9932e7566cef823f6</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, BTTransport_t xTransport, const BTOutOfBandData_t *pxOobData)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSendSlaveSecurityRequest</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>ab7c9b3f5a3bec5686b35d3c77db71963</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, BTSecurityLevel_t xSecurityLevel, bool bBonding)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxCancelBond</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a677a7b3c94a4ddacca151ac7f1b647df</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxRemoveBond</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a2bfb2d9115422e3db20dc0d20de76473</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetConnectionState</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>ab5f04a5d5a4c2a68b91999fd88e46d3f</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, bool *bConnectionState)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxPinReply</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a87187fb606042ed3d9ae9bbc847f15f8</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, uint8_t ucAccept, uint8_t ucPinLen, BTPinCode_t *pxPinCode)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxSspReply</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a911ffeb5d84a65be504ae9f23e445d9a</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, BTSspVariant_t xVariant, uint8_t ucAccept, uint32_t ulPasskey)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadEnergyInfo</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a3226a472cf3a9390d0466595d977b3dd</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDutModeConfigure</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a4181762f9c20106bd62efd43b090e994</anchor>
      <arglist>)(bool bEnable)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxDutModeSend</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a69be211fa1d75e96515ce2ad6d944a1a</anchor>
      <arglist>)(uint16_t usOpcode, uint8_t *pucBuf, size_t xLen)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxLeTestMode</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>affd2a6c85d4a5a7fbcbd59b4d91e6608</anchor>
      <arglist>)(uint16_t usOpcode, uint8_t *pucBuf, size_t xLen)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConfigHCISnoopLog</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a290f7a115a3cbd9a7f8028d7350463c0</anchor>
      <arglist>)(bool bEnable)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxConfigClear</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a6a68a5177853639d1f2019be846528f5</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxReadRssi</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>abe012dd31ac5c277d1248bbdd612b26e</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetTxpower</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>aab86de7399867c4ff23a02e03afc59ae</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, BTTransport_t xTransport)</arglist>
    </member>
    <member kind="variable">
      <type>const void *(*</type>
      <name>pxGetClassicAdapter</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a8a8cf08e5033fd66c934333a8f638d29</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>const void *(*</type>
      <name>pxGetLeAdapter</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a34f6d12565c1576ac410813ee769d571</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>uint32_t(*</type>
      <name>pxGetLastError</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a269328cc1c8cdfbe2a1409a38f6db470</anchor>
      <arglist>)()</arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t(*</type>
      <name>pxGetStackFeaturesSupport</name>
      <anchorfile>struct_b_t_interface__t.html</anchorfile>
      <anchor>a6e246ec7fc18b908cee540f1268f8112</anchor>
      <arglist>)(uint32_t *pulFeatureMask)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTLocalLeFeatures_t</name>
    <filename>struct_b_t_local_le_features__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTOutOfBandData_t</name>
    <filename>struct_b_t_out_of_band_data__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucBleDevAddr</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>a5106639d438d240281b6f7167911f5d4</anchor>
      <arglist>[btADDRESS_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucC192</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>a274638935d3db6d377eab69911472aff</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucR192</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>ae51ab18aac78376c9f087af57063e725</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucC256</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>a33b8a7f10b021b0929a18112421dac60</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucR256</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>aee1827a07471c9a06cc5368fdb788333</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucSm_tk</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>a79dc1d7fd716f58742784f4ce7cc478f</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucLe_sc_c</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>a3f6644a5e2cd0b553cb4dd87764c2e2e</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucLe_sc_r</name>
      <anchorfile>struct_b_t_out_of_band_data__t.html</anchorfile>
      <anchor>aa9bb44d71b062261d17b0b63d8aefd94</anchor>
      <arglist>[btKEY_MAX_LEN]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTPinCode_t</name>
    <filename>struct_b_t_pin_code__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucPin</name>
      <anchorfile>struct_b_t_pin_code__t.html</anchorfile>
      <anchor>ae940888bb9e44b8ce3bce2fff2b342b1</anchor>
      <arglist>[16]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTProperty_t</name>
    <filename>struct_b_t_property__t.html</filename>
    <member kind="variable">
      <type>BTPropertyType_t</type>
      <name>xType</name>
      <anchorfile>struct_b_t_property__t.html</anchorfile>
      <anchor>aeb3789c0849e271fbee573a42e7e6007</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>xLen</name>
      <anchorfile>struct_b_t_property__t.html</anchorfile>
      <anchor>aaf22e408c8ac6747581b8a0297a61a60</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pvVal</name>
      <anchorfile>struct_b_t_property__t.html</anchorfile>
      <anchor>a4c14b7786b15ec6bbd9015e10de5ba4e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTService</name>
    <filename>struct_b_t_service.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>ucInstId</name>
      <anchorfile>struct_b_t_service.html</anchorfile>
      <anchor>a6a4496be8f70678b0f2f647a1bba742e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTGattServiceTypes_t</type>
      <name>xType</name>
      <anchorfile>struct_b_t_service.html</anchorfile>
      <anchor>ad8f5dbcc3eb38ff5e444b68e34956343</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>xNumberOfAttributes</name>
      <anchorfile>struct_b_t_service.html</anchorfile>
      <anchor>a51ff8c8becf8ca050c58ab949ef43207</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t *</type>
      <name>pusHandlesBuffer</name>
      <anchorfile>struct_b_t_service.html</anchorfile>
      <anchor>a1baac82500b45647340d16caeacc4d16</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTAttribute_t *</type>
      <name>pxBLEAttributes</name>
      <anchorfile>struct_b_t_service.html</anchorfile>
      <anchor>a9fa4489965ba5cfbe15e751630104605</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BTServiceRecord_t</name>
    <filename>struct_b_t_service_record__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTUidTraffic_t</name>
    <filename>struct_b_t_uid_traffic__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>BTUuid_t</name>
    <filename>struct_b_t_uuid__t.html</filename>
    <member kind="variable">
      <type>BTuuidType_t</type>
      <name>ucType</name>
      <anchorfile>struct_b_t_uuid__t.html</anchorfile>
      <anchor>a3a987f03bcb7075dc1110a518592bce5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>uu16</name>
      <anchorfile>struct_b_t_uuid__t.html</anchorfile>
      <anchor>a13ce962b3891daea10fbb28aea3aa807</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>uu32</name>
      <anchorfile>struct_b_t_uuid__t.html</anchorfile>
      <anchor>a609e6f17891fc59a44150fe1745edcb2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>uu128</name>
      <anchorfile>struct_b_t_uuid__t.html</anchorfile>
      <anchor>a38989165938b99e4da5d817659fe7a4b</anchor>
      <arglist>[bt128BIT_UUID_LEN]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleAddNetworkRequest_t</name>
    <filename>struct_iot_ble_add_network_request__t.html</filename>
    <member kind="variable">
      <type>WIFINetworkProfile_t</type>
      <name>network</name>
      <anchorfile>struct_iot_ble_add_network_request__t.html</anchorfile>
      <anchor>a1120eef64bd422cd92ab1bbad30caeea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int16_t</type>
      <name>savedIdx</name>
      <anchorfile>struct_iot_ble_add_network_request__t.html</anchorfile>
      <anchor>af01e4dc626debcea29b9ffec606bc999</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>struct_iot_ble_add_network_request__t.html</anchorfile>
      <anchor>ad10840286b0f3256886e98fae2f0e241</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleAdvertisementParams_t</name>
    <filename>struct_iot_ble_advertisement_params__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>appearance</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>af2c2b304dfaa85ee4178a6d9f296b98d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>minInterval</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a52c31a5243a0d16599e1972ea0bd9bfe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>maxInterval</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a418538a4d0f4716b887e2e4bc4949b0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>pManufacturerData</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a9f43ca0255fd1e0ca92c91b3e7cd18db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>pServiceData</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a2e207496dc0ed5fd3fc10012db17b84c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTUuid_t *</type>
      <name>pUUID1</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a576f267fcacad38de0ca49d2efb88e12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTUuid_t *</type>
      <name>pUUID2</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>ad8026cb77eceb8696900e94068b46930</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>manufacturerLen</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a63700e2aba9ad874f66141920e638695</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>serviceDataLen</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>aee966567b6e769c7c5227644d8c0c161</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>includeTxPower</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>aa002e1df4d792962c497f51b8ba09fcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTGattAdvName_t</type>
      <name>name</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>a61420f4960f7f95ca5583242b19fcb40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>setScanRsp</name>
      <anchorfile>struct_iot_ble_advertisement_params__t.html</anchorfile>
      <anchor>ac87e0e1172432dae910bba35a971cc86</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleAttributeData_t</name>
    <filename>struct_iot_ble_attribute_data__t.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>struct_iot_ble_attribute_data__t.html</anchorfile>
      <anchor>afd86c0eed1e62534c14bc76c5ead332e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pData</name>
      <anchorfile>struct_iot_ble_attribute_data__t.html</anchorfile>
      <anchor>aedfed227d6f8416925e5164e27ff4e1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTUuid_t</type>
      <name>uuid</name>
      <anchorfile>struct_iot_ble_attribute_data__t.html</anchorfile>
      <anchor>a66926c57bfc3ff0fe9ee8d25d0671eaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>handle</name>
      <anchorfile>struct_iot_ble_attribute_data__t.html</anchorfile>
      <anchor>ac0823b7a61d73b561d8369654eb9b149</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleAttributeEvent</name>
    <filename>struct_iot_ble_attribute_event.html</filename>
    <member kind="variable">
      <type>IotBleReadEventParams_t *</type>
      <name>pParamRead</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>af000cffda3324dff9fb003e3ea5069e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleWriteEventParams_t *</type>
      <name>pParamWrite</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>a85a0549443858500f6ee232817499b76</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleExecWriteEventParams_t *</type>
      <name>pParamExecWrite</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>a89a133e4dd68ab388b7047813950f50d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleRespConfirmEventParams_t *</type>
      <name>pParamRespConfirm</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>a51476d61347df02cf125b0a8271e6632</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleIndicationSentEventParams_t *</type>
      <name>pParamIndicationSent</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>a8a3277d571f30f3ab4005c0239ae33b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleAttributeEventType_t</type>
      <name>xEventType</name>
      <anchorfile>struct_iot_ble_attribute_event.html</anchorfile>
      <anchor>a6a6ba8826b0a11003de39d74fdceb124</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleConnectionInfoListElement_t</name>
    <filename>struct_iot_ble_connection_info_list_element__t.html</filename>
    <member kind="variable">
      <type>IotLink_t</type>
      <name>connectionList</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a2aa514fc93a437b163eb3de3aed51f7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBleConnectionParam_t</type>
      <name>connectionParams</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a1696e126dfd03261814bac9da937597b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTBdaddr_t</type>
      <name>remoteBdAddr</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a84d19e76d732e3ab3bcdba21d9dbd34e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTSecurityLevel_t</type>
      <name>securityLevel</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a9bc19107a7927f8d3c5b0d2ff9bca083</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotLink_t</type>
      <name>clientCharDescrListHead</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a06544c5d16270eb20edaa6c82df56ec6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>connId</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>aa5f7b70790a024a5fe50986dd4da5167</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isBonded</name>
      <anchorfile>struct_iot_ble_connection_info_list_element__t.html</anchorfile>
      <anchor>a83efe4b777b0f0335cea8d3260a4d48b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleConnectionParam_t</name>
    <filename>struct_iot_ble_connection_param__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>minInterval</name>
      <anchorfile>struct_iot_ble_connection_param__t.html</anchorfile>
      <anchor>acfaf698d7274f5a9868a859cc85c339a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>maxInterval</name>
      <anchorfile>struct_iot_ble_connection_param__t.html</anchorfile>
      <anchor>a34aa00be9f23aeddaee2df4452cbf47f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>latency</name>
      <anchorfile>struct_iot_ble_connection_param__t.html</anchorfile>
      <anchor>a65fdeb5d8f8936254af7de21270d0da2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>timeout</name>
      <anchorfile>struct_iot_ble_connection_param__t.html</anchorfile>
      <anchor>aa7fc274041d12a8da764f31df5fe2312</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleDeleteNetworkRequest_t</name>
    <filename>struct_iot_ble_delete_network_request__t.html</filename>
    <member kind="variable">
      <type>int16_t</type>
      <name>idx</name>
      <anchorfile>struct_iot_ble_delete_network_request__t.html</anchorfile>
      <anchor>ab4d23a6ccd1b72d318730c8bc128b05d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleEditNetworkRequest_t</name>
    <filename>struct_iot_ble_edit_network_request__t.html</filename>
    <member kind="variable">
      <type>int16_t</type>
      <name>curIdx</name>
      <anchorfile>struct_iot_ble_edit_network_request__t.html</anchorfile>
      <anchor>af1de7515c63bd561974227b58f05c059</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int16_t</type>
      <name>newIdx</name>
      <anchorfile>struct_iot_ble_edit_network_request__t.html</anchorfile>
      <anchor>a4f67708ea6c3a968c07f7d6d28af994d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleEventResponse_t</name>
    <filename>struct_iot_ble_event_response__t.html</filename>
    <member kind="variable">
      <type>IotBleAttributeData_t *</type>
      <name>pAttrData</name>
      <anchorfile>struct_iot_ble_event_response__t.html</anchorfile>
      <anchor>a9e8dd37339ec95aa8958a01a251e55b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>attrDataOffset</name>
      <anchorfile>struct_iot_ble_event_response__t.html</anchorfile>
      <anchor>a64326656938aa31287fc2533d6aee973</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t</type>
      <name>eventStatus</name>
      <anchorfile>struct_iot_ble_event_response__t.html</anchorfile>
      <anchor>a97a079474634f6fc46eb6e954eb3ef00</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTRspErrorStatus_t</type>
      <name>rspErrorStatus</name>
      <anchorfile>struct_iot_ble_event_response__t.html</anchorfile>
      <anchor>a59be899db102159383553990f0b9b847</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>IotBleEventsCallbacks_t</name>
    <filename>union_iot_ble_events_callbacks__t.html</filename>
    <member kind="variable">
      <type>IotBle_MtuChangedCallback_t</type>
      <name>pMtuChangedCb</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>a082a8b2b7b7ba0de0ae6970bcdee76fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBle_ConnectionCallback_t</type>
      <name>pConnectionCb</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>a4d7eb5b979658273711128fb9dd01f8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBle_PairingStateChanged_t</type>
      <name>pGAPPairingStateChangedCb</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>a1408546ac060ac04be4bc57e2e767fc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBle_ConnParameterUpdateRequestCallback_t</type>
      <name>pConnParameterUpdateRequestCb</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>aeee7e91eaf75ab2954920e393d1d6357</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotBle_NumericComparisonCallback_t</type>
      <name>pNumericComparisonCb</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>a69ae6c4b48f24cec4407f8a96f3c9a6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pvPtr</name>
      <anchorfile>union_iot_ble_events_callbacks__t.html</anchorfile>
      <anchor>ac3481c0bb7d4a59eab511618323aa638</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleExecWriteEventParams_t</name>
    <filename>struct_iot_ble_exec_write_event_params__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>transId</name>
      <anchorfile>struct_iot_ble_exec_write_event_params__t.html</anchorfile>
      <anchor>a4a5481463cd72677a29bdb10257dbbf8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTBdaddr_t *</type>
      <name>pRemoteBdAddr</name>
      <anchorfile>struct_iot_ble_exec_write_event_params__t.html</anchorfile>
      <anchor>a16d9e31a657d8af854a17e30ca0eab94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>connId</name>
      <anchorfile>struct_iot_ble_exec_write_event_params__t.html</anchorfile>
      <anchor>a02216a1366225590c5d053c313cc0887</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>execWrite</name>
      <anchorfile>struct_iot_ble_exec_write_event_params__t.html</anchorfile>
      <anchor>aa827e03b3ba7ed318fbe4efc56f4c46c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleIndicationSentEventParams_t</name>
    <filename>struct_iot_ble_indication_sent_event_params__t.html</filename>
    <member kind="variable">
      <type>BTAttribute_t *</type>
      <name>pAttribute</name>
      <anchorfile>struct_iot_ble_indication_sent_event_params__t.html</anchorfile>
      <anchor>ac812d1e73ea99c389a8b1138f3b626b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>connId</name>
      <anchorfile>struct_iot_ble_indication_sent_event_params__t.html</anchorfile>
      <anchor>a90b85ff050b2d272c5ee00d353bd4f68</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTStatus_t</type>
      <name>status</name>
      <anchorfile>struct_iot_ble_indication_sent_event_params__t.html</anchorfile>
      <anchor>aa8c003ab7617649167f9d510bf00f055</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleListNetworkRequest_t</name>
    <filename>struct_iot_ble_list_network_request__t.html</filename>
    <member kind="variable">
      <type>int16_t</type>
      <name>maxNetworks</name>
      <anchorfile>struct_iot_ble_list_network_request__t.html</anchorfile>
      <anchor>ab569bac0526f308805f5063d8f056b40</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int16_t</type>
      <name>timeoutMs</name>
      <anchorfile>struct_iot_ble_list_network_request__t.html</anchorfile>
      <anchor>acc14419712e27e459d3a7a265fed4935</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleReadEventParams_t</name>
    <filename>struct_iot_ble_read_event_params__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>connId</name>
      <anchorfile>struct_iot_ble_read_event_params__t.html</anchorfile>
      <anchor>a676480b3d0714a6523ef2f6b462ad100</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>transId</name>
      <anchorfile>struct_iot_ble_read_event_params__t.html</anchorfile>
      <anchor>a757b1a25aaac8450b18d8a38025f5c55</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTBdaddr_t *</type>
      <name>pRemoteBdAddr</name>
      <anchorfile>struct_iot_ble_read_event_params__t.html</anchorfile>
      <anchor>aa43998fb9ad5b52de6999b81566c1c1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>attrHandle</name>
      <anchorfile>struct_iot_ble_read_event_params__t.html</anchorfile>
      <anchor>a0ad867195c80665f6044f6ccbd97f3b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>offset</name>
      <anchorfile>struct_iot_ble_read_event_params__t.html</anchorfile>
      <anchor>a5ec6dc40b7f36b1157bc4495f621c836</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleRespConfirmEventParams_t</name>
    <filename>struct_iot_ble_resp_confirm_event_params__t.html</filename>
    <member kind="variable">
      <type>BTStatus_t</type>
      <name>status</name>
      <anchorfile>struct_iot_ble_resp_confirm_event_params__t.html</anchorfile>
      <anchor>a851536e1dacc9fb72bdee6b44b7279a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>handle</name>
      <anchorfile>struct_iot_ble_resp_confirm_event_params__t.html</anchorfile>
      <anchor>a549e3a9ae98e7c2d3ca3b703b4bcf77f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleWifiNetworkInfo_t</name>
    <filename>struct_iot_ble_wifi_network_info__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pSSID</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a8c986732bac3a1498f17fad334b3c47d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>SSIDLength</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a62e4ee1bb5eea98c91c45a00aae8e349</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pBSSID</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a5f9b008c8a5e3fe9a43d0fd06579c2eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>BSSIDLength</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>acfc3878ef57374c79149436676a7b5cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int32_t</type>
      <name>savedIdx</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a472c86dbfc4d0b404a015dd1a5f0e0df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int8_t</type>
      <name>RSSI</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a60e21aa748fdf01533b2987f25cfa134</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>WIFISecurity_t</type>
      <name>security</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a09b57be99fb597c5bc8e038abc5b43d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>WIFIReturnCode_t</type>
      <name>status</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a6a754cf698e4880ef9205fe2a698d6b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>hidden</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a2f75cfe908003e18af17f7d4c03cf179</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>connected</name>
      <anchorfile>struct_iot_ble_wifi_network_info__t.html</anchorfile>
      <anchor>a1d8ddab827463f6fa3046b8da898ae0e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleWifiProvService_t</name>
    <filename>struct_iot_ble_wifi_prov_service__t.html</filename>
    <member kind="variable">
      <type>IotBleDataTransferChannel_t *</type>
      <name>pChannel</name>
      <anchorfile>struct_iot_ble_wifi_prov_service__t.html</anchorfile>
      <anchor>a7976c6b9ee6cd99914c61ccee51400bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotSemaphore_t</type>
      <name>lock</name>
      <anchorfile>struct_iot_ble_wifi_prov_service__t.html</anchorfile>
      <anchor>aa54ad1944f413e1d10659e01bfe2d9fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>numNetworks</name>
      <anchorfile>struct_iot_ble_wifi_prov_service__t.html</anchorfile>
      <anchor>a85db601e762484e693bdccfd8b956a26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int16_t</type>
      <name>connectedIdx</name>
      <anchorfile>struct_iot_ble_wifi_prov_service__t.html</anchorfile>
      <anchor>a6e6d30e48724227212c7bf0bd0f71b30</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotBleWriteEventParams_t</name>
    <filename>struct_iot_ble_write_event_params__t.html</filename>
    <member kind="variable">
      <type>uint32_t</type>
      <name>transId</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a85af199e536d8d3d146646b34a0b44d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>BTBdaddr_t *</type>
      <name>pRemoteBdAddr</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>ae87b754704ca4d8d3ed36754c59e6ccd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pValue</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a10a31f9bce025cafb8757e37bbe3ee18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>length</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>ab6aea1925055f9ec6851823481839ebd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>connId</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a24b14e2adf64d5b19a1c673d16133071</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>attrHandle</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a21464a99a4e888983a53ccd70ef92ae1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>offset</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a8cd2a17851b785272f0a4f79ade75c4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>needRsp</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a1956f3f8301cb64a0b3dc07102a194b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isPrep</name>
      <anchorfile>struct_iot_ble_write_event_params__t.html</anchorfile>
      <anchor>a9564b04190b11a43c0e3e6e4762312dc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ble_datatypes_enums</name>
    <title>Enumerated types</title>
    <filename>group__ble__datatypes__enums.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>IotBleAttributeEventType_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga5f7906be0497dbfff2d6f0232692f455</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLERead</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a74d3d77d4cc00629948926b2a63f66cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWrite</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a8e4a5a1ee31e67089f996a6f8206da6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWriteNoResponse</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ac4d7e6abb98e9967558b6000fadb64b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEWritePrepare</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ab984f01b9221efa802906ff0b466d09c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEExecWrite</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a2f28c750c441ee258613a73df7e5ec7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEResponseConfirmation</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455a8dac371f5c542c3f06fc7d8e56be9f6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEIndicationConfirmReceived</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga5f7906be0497dbfff2d6f0232692f455ae2da4744c8347eb1ff2a9619b85ccfd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotBleEvents_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga3d9455e1f3d2f24b51ad7056465a1604</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEMtuChanged</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604a388d9ff53be0dda10b21a9f52803b36c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEConnection</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604ac9ccc26c3baccd6fbe384523fbb7b490</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEPairingStateChanged</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604ae1ebeb0e49c74e87b55b9cc38469db7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLEConnParameterUpdateRequestCallback</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604a743eeb6541e030b17a3871474ba8582d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBLENumericComparisonCallback</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604adc1aeff6aeeef08965e0f65ddb587b15</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eNbEvents</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga3d9455e1f3d2f24b51ad7056465a1604af6b68dafa3f892847b977b990ca8db4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotBleWifiProvEvent_t</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>ga442627b957005b6175227e879739eb8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_CONNECT</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8ea9c46bd8733d5e34c039bbe4cfac2d496</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_CONNECTED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eaaece6cd81e0648ce449212d3330942d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_DELETED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eac95c9f9dc426a86fe45a98a73fd5dc3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_BLE_WIFI_PROV_FAILED</name>
      <anchorfile>group__ble__datatypes__enums.html</anchorfile>
      <anchor>gga442627b957005b6175227e879739eb8eaf82bcfc63005b4d6abbf0ee9b4cd73fa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ble_datatypes_paramstructs</name>
    <title>Parameter structures</title>
    <filename>group__ble__datatypes__paramstructs.html</filename>
  </compound>
  <compound kind="group">
    <name>ble_datatypes_handles</name>
    <title>Handles</title>
    <filename>group__ble__datatypes__handles.html</filename>
  </compound>
  <compound kind="group">
    <name>ble_datatypes_functionpointers</name>
    <title>Function pointer types</title>
    <filename>group__ble__datatypes__functionpointers.html</filename>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBleAttributeEventCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga7c0570c3f4d70b37ce4f5e29e313790c</anchor>
      <arglist>)(IotBleAttributeEvent_t *pEventParam)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_MtuChangedCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>gacad43808d7fce001db06e2563c3bacdd</anchor>
      <arglist>)(uint16_t connId, uint16_t mtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_ConnectionCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga525923992426a85caaf9d94206be197b</anchor>
      <arglist>)(BTStatus_t status, uint16_t connId, bool connected, BTBdaddr_t *pRemoteBdAddr)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_StartAdvCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0d75a95f024bb24f0ca438e3be0bd598</anchor>
      <arglist>)(BTStatus_t status)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_StopAdvCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0a1efc291ddcc545882a811b4ed59949</anchor>
      <arglist>)(BTStatus_t status)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_ConnParameterUpdateRequestCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga44c0d8c0a31c05266fdadb9d337e8443</anchor>
      <arglist>)(BTStatus_t status, const BTBdaddr_t *pRemoteBdAddr, IotBleConnectionParam_t *pConnectionParam, uint32_t connInterval)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_PairingStateChanged_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga0a4531445768f3e3ecbad1700b2621a6</anchor>
      <arglist>)(BTStatus_t status, BTBdaddr_t *pRemoteBdAddr, BTBondState_t bondstate, BTSecurityLevel_t securityLevel, BTAuthFailureReason_t reason)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IotBle_NumericComparisonCallback_t</name>
      <anchorfile>group__ble__datatypes__functionpointers.html</anchorfile>
      <anchor>ga1348d7caea520883b602a7aa404701d8</anchor>
      <arglist>)(BTBdaddr_t *pRemoteBdAddr, uint32_t passKey)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ble_datatypes_structs</name>
    <title>Structured types</title>
    <filename>group__ble__datatypes__structs.html</filename>
    <class kind="struct">IotBleAdvertisementParams_t</class>
    <class kind="struct">IotBleConnectionParam_t</class>
    <class kind="struct">IotBleConnectionInfoListElement_t</class>
    <class kind="struct">IotBleReadEventParams_t</class>
    <class kind="struct">IotBleWriteEventParams_t</class>
    <class kind="struct">IotBleExecWriteEventParams_t</class>
    <class kind="struct">IotBleRespConfirmEventParams_t</class>
    <class kind="struct">IotBleIndicationSentEventParams_t</class>
    <class kind="struct">IotBleAttributeEvent</class>
    <class kind="struct">IotBleAttributeData_t</class>
    <class kind="struct">IotBleEventResponse_t</class>
    <class kind="union">IotBleEventsCallbacks_t</class>
    <class kind="struct">IotBleListNetworkRequest_t</class>
    <class kind="struct">IotBleAddNetworkRequest_t</class>
    <class kind="struct">IotBleEditNetworkRequest_t</class>
    <class kind="struct">IotBleDeleteNetworkRequest_t</class>
    <class kind="struct">IotBleWifiNetworkInfo_t</class>
    <class kind="struct">IotBleWifiProvService_t</class>
  </compound>
  <compound kind="group">
    <name>HAL_BLUETOOTH</name>
    <title>HAL_BLUETOOTH</title>
    <filename>group___h_a_l___b_l_u_e_t_o_o_t_h.html</filename>
    <class kind="struct">BTAvsrcCallbacks_t</class>
    <class kind="struct">BTAvsrcInterface_t</class>
    <class kind="struct">BTGattUnformattedValue_t</class>
    <class kind="struct">BTGattReadParams_t</class>
    <class kind="struct">BTGattWriteParams_t</class>
    <class kind="struct">BTGattNotifyParams_t</class>
    <class kind="struct">BTGattTestParams_t</class>
    <class kind="struct">BTGattClientCallbacks_t</class>
    <class kind="struct">BTGattClientInterface_t</class>
    <class kind="struct">BTGattValue_t</class>
    <class kind="union">BTGattResponse_t</class>
    <class kind="struct">BTGattServerCallbacks_t</class>
    <class kind="struct">BTGattServerInterface_t</class>
    <class kind="struct">BTGattInstanceId_t</class>
    <class kind="struct">BTGattSrvcId_t</class>
    <class kind="struct">BTGattTrackAdvInfo_t</class>
    <class kind="struct">BTCharacteristic_t</class>
    <class kind="struct">BTCharacteristicDescr_t</class>
    <class kind="struct">BTIncludedService_t</class>
    <class kind="struct">BTAttribute_t</class>
    <class kind="struct">BTService</class>
    <class kind="struct">BTGattDbElement_t</class>
    <class kind="struct">BTPinCode_t</class>
    <class kind="struct">BTProperty_t</class>
    <class kind="struct">BTOutOfBandData_t</class>
    <class kind="struct">BTActivityEnergyInfo</class>
    <class kind="struct">BTUidTraffic_t</class>
    <class kind="struct">BTCallbacks_t</class>
    <class kind="struct">BTInterface_t</class>
    <class kind="struct">BTGattFiltParamSetup_t</class>
    <class kind="struct">BTGattAdvName_t</class>
    <class kind="struct">BTGattAdvertismentParams_t</class>
    <class kind="struct">BTLocalLeFeatures_t</class>
    <class kind="struct">BTBleProperty_t</class>
    <class kind="struct">BTBleAdapterCallbacks_t</class>
    <class kind="struct">BTBleAdapter_t</class>
    <class kind="struct">BTServiceRecord_t</class>
    <class kind="struct">BTEirManfInfo_t</class>
    <class kind="struct">BTGadgetEirManfInfo_t</class>
    <class kind="struct">BTClassicProperty_t</class>
    <class kind="struct">BTClassicCallbacks_t</class>
    <class kind="struct">BTClassicInterface_t</class>
    <class kind="struct">BTBdaddr_t</class>
    <class kind="struct">BTBdname_t</class>
    <class kind="struct">BTUuid_t</class>
    <member kind="define">
      <type>#define</type>
      <name>btGATT_MAX_ATTR_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1a7d653463b2a9fcf562fa114adaa5b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btMAJOR_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gacdb3e342f24ac5e39c745efb823bc2a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btMINOR_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gafe2f4e8cb2ad8cb0d3d47e51ad6084e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btPATCH_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6ceca2c5690fbbbcc9f3936d4b5fb5bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btSTR_HELPER</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga087df9185bb5c85141ae6b3a77e50be5</anchor>
      <arglist>(x, y, z)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btSTRING_VERSION</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga244cc1213b691aa80da1bc75a0c53eff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btBLE_ADD_BLOB_SERVICE_SUPPORTED_BIT</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga11774cfd2d39d0b4fc3747b07049892d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>bt128BIT_UUID_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gad96edc6c251bcd5ce11768e6ef35460f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>btADDRESS_LEN</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga04d807f006570ebb70f2bf530110b83e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTAvsrcConnectionStateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf9b5a60b3f022d57d8fa72c24a4d0537</anchor>
      <arglist>)(BTAvConnectionState_t xState, BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTAvsrcAudioStateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga21b3639cb6b125ed3ad0ad64e7974792</anchor>
      <arglist>)(BTAvAudioState_t xState, BTBdaddr_t *pxBdAddr)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterClientCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga81a74dd70142306c78bc18bb82cec62f</anchor>
      <arglist>)(BTGattStatus_t xStatus, uint8_t ucClientIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSearchCompleteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga82ef416e4893fa894f37f1f95e45e9e6</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterForNotificationCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga70c3a9bc07602d7ee4d244a1eec9cda1</anchor>
      <arglist>)(uint16_t usConnId, bool bRegistered, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTNotifyCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5e2f5a4784d42781adc2934ae1a0b098</anchor>
      <arglist>)(uint16_t usConnId, BTGattNotifyParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadCharacteristicCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga43cd7e112d436d7c107efb8b748f3247</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, BTGattReadParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWriteCharacteristicCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gadcf7430976f57d009fe1040f7d30b7fe</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTExecuteWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8d5f9c5e50bcdccb64af5771ade5410a</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadDescriptorCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gabaa06ef5d631a4bd5cab4dd4ded777fa</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, BTGattReadParams_t *pxData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWriteDescriptorCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2510813148ed3b069040f410d7c55a2</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTListenCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa1892bb34b892b19aa61d0a748ea9924</anchor>
      <arglist>)(BTGattStatus_t xStatus, uint32_t ulServerIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConfigureMtuCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1e184e6dfa402cc6ddc16845030c0a31</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint32_t ulMtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTGetGattDbCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac6a0335b83f3b3d885dad0236a300eac</anchor>
      <arglist>)(uint16_t usConnId, BTGattDbElement_t *pxDb, uint32_t ulCount)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServicesRemovedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2601674bd7f321e3eaac2b8788cd3e87</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usStartHandle, uint16_t usEndHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServicesAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae993f95bcc54d151712184451658cfde</anchor>
      <arglist>)(uint16_t usConnId, BTGattDbElement_t *pxAdded, uint32_t ulAddedCount)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterServerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4eba705215e4c27ee5bccdd37dcdfcde</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTUnregisterServerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf1281a92d067697bf867ec9a7be3c13e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnectionCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6851953b2552ca6e4fb80d3bf8613fcf</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucServerIf, bool bConnected, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga32f2846853cb51c0a1884c53fe2dd77e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTGattSrvcId_t *pxSrvcId, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTIncludedServiceAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3aa10d2a84b34b5865d3b248574bd6b9</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle, uint16_t usInclSrvcHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTCharacteristicAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7ec3da0a162e827f21e3d3f72f895383</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxUuid, uint16_t usServiceHandle, uint16_t usCharHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSetValCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac832c40fca8f7ea8844f92ff9227ab57</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usAttrHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDescriptorAddedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga97f57c2ca72c95126f52ee08a7cb6a82</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, BTUuid_t *pxUuid, uint16_t usServiceHandle, uint16_t usDescrHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceStartedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga52c077e02fb724a4a2fac950f1d94fae</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceStoppedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2f7a2c53b4130bafac3024bd2b2f59f2</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTServiceDeletedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0feae4ce7639693710c5f87925732c1d</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucServerIf, uint16_t usServiceHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestReadCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac3ced021a1484077860213dc30c51798</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, uint16_t usAttrHandle, uint16_t usOffset)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae5abff05ecc3368be82af799342dfa65</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, uint16_t usAttrHandle, uint16_t usOffset, size_t xLength, bool bNeedRsp, bool bIsPrep, uint8_t *pucValue)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRequestExecWriteCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6da40d1176611ab97302e23e7812eed4</anchor>
      <arglist>)(uint16_t usConnId, uint32_t ulTransId, BTBdaddr_t *pxBda, bool bExecWrite)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTResponseConfirmationCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga276589c8697edd6f6e2ff6692726422b</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usHandle)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTIndicationSentCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa165494a87ba7d3f6abf2edb77cd367b</anchor>
      <arglist>)(uint16_t usConnId, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMtuChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga24d664c493a06728e34423a7a8644cc8</anchor>
      <arglist>)(uint16_t usConnId, uint16_t usMtu)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTWhiteListChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7885d17d4d1d01a8e86b67647d44a39e</anchor>
      <arglist>)(const BTBdaddr_t *pxBdAddr, bool bIsAdded)</arglist>
    </member>
    <member kind="typedef">
      <type>BTUuid_t</type>
      <name>BTServiceUUID_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8b891d4e44836fd951ad1f13c42a0b3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnectCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5ca839bac0a52a1025843dd7c680f543</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint8_t ucClientIf, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDisconnectCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae09852d593ec0cf60ffe44519a9681cf</anchor>
      <arglist>)(uint16_t usConnId, BTGattStatus_t xStatus, uint8_t ucClientIf, BTBdaddr_t *pxBda)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadRemoteRssiCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga1da38dd690cb247fab63a93f0af571f4</anchor>
      <arglist>)(uint8_t ucClientIf, BTBdaddr_t *pxBda, uint32_t ulRssi, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTCongestionCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga978849d2a447b29637b65bae86ce1973</anchor>
      <arglist>)(uint16_t usConnId, bool bCongested)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDeviceStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2dedbe2af2576e4c167113bdf271de5</anchor>
      <arglist>)(BTState_t xState)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga99537ec9b0475d5f162d9cac65f22084</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6170bf7b014afbc2d3764ab5ceeb2fde</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPinRequestCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga178d889935bd336575e91a1e2cfbc91c</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteBdAddr, BTBdname_t *pxBdName, uint32_t ulCod, uint8_t ucMin16Digit)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSspRequestCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae01c7c1ee76ba40d7e74ec54efc39083</anchor>
      <arglist>)(BTBdaddr_t *pxRemoteBdAddr, BTBdname_t *pxRemoteBdName, uint32_t ulCod, BTSspVariant_t xPairingVariant, uint32_t ulPassKey)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPairingStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga804f88c3d32375bd496e2a2c0a1503d8</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, BTBondState_t xState, BTSecurityLevel_t xSecurityLevel, BTAuthFailureReason_t xReason)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTEnergyInfoCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4277f9d5316419686a24a6c2e77dc58c</anchor>
      <arglist>)(BTActivityEnergyInfo *pxEnergyInfo, BTUidTraffic_t *pxUidData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDutModeRecvCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga711f1ac6ff9558bd079ad8cbb9997b7d</anchor>
      <arglist>)(uint16_t usOpcode, uint8_t *pucBuf, size_t xLen)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTLeTestModeCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga55f2ee38d318c5ef03ee4ba763f9b975</anchor>
      <arglist>)(BTStatus_t xStatus, uint16_t usNumPackets)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadRssiCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga7b931a36a7988ae30a2145dd2809de4a</anchor>
      <arglist>)(BTBdaddr_t *pxBda, uint32_t ulRssi, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSlaveSecurityRequestCallBack_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga8c0d83c281853e8dcbb660c5b02d926c</anchor>
      <arglist>)(BTBdaddr_t *pxBda, BTSecurityLevel_t xSecurityLevel, bool bBonding)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTTxPowerCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5a76aad0e81c46c1504990170149f994</anchor>
      <arglist>)(BTBdaddr_t *pxBda, BTTransport_t xTransport, uint8_t ucTxPowerValue, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBondedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaff4165bf99dfd12faacd7dccaeabcb28</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, bool bIsBonded)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTRegisterBleAdapterCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga678c3554952d1f6dc6723dce0141645e</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucAdapterIf, BTUuid_t *pxAppUuid)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBleDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga4b074dc84738af51b05d821c5ed7a32d</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTBleProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBleRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga68d00df313388dec35f7a6ddacfa4e77</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTBleProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanResultCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0e12fa85d0ed31d450dfb34fde0c2663</anchor>
      <arglist>)(BTBdaddr_t *pxBda, uint32_t ulRssi, uint8_t *pucAdvData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTAdvStatusCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa40080a2e276e49b1f967a52bfeada12</anchor>
      <arglist>)(BTStatus_t xStatus, uint8_t ucAdapterIf, bool bStart)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTSetAdvDataCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab46a3f882124f4a007da2074b17b1541</anchor>
      <arglist>)(BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTConnParameterUpdateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga51661f41d0a1d333075917765db76a40</anchor>
      <arglist>)(BTStatus_t xStatus, const BTBdaddr_t *pxBdAddr, uint32_t ulMinInterval, uint32_t ulMaxInterval, uint32_t ulLatency, uint32_t usConnInterval, uint32_t ulTimeout)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterCfgCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga81db6bdf19000ea714c0ca35211f210a</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulFiltType, uint32_t ulAvblSpace)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterParamCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga813ce88b8d81384a6e8adca274b22790</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulAvblSpace)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanFilterStatusCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga71024bd3944f1c3961c00916e3f3430c</anchor>
      <arglist>)(bool bEnable, uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvEnableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaaf18a6ad02100c7763856504c8b424d6</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvUpdateCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gabe50a40fdcc3280ee24c41ba988b53f2</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvDataCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaea07ee7ba0648d3a1342bf5295e2d4d8</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTMultiAdvDisableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0b9aae1d97f57fa7c604bffb560874eb</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanCfgStorageCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga9ae4a3617b0e50b20d5b9e14001a8198</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanEnableDisableCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga06cf4afc1f73f0fd6af57c191634ecc8</anchor>
      <arglist>)(uint32_t ulAction, uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanReportsCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga6224a3f06cbd5a6efe22beb05fc9779d</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus, uint32_t ulReportFormat, uint32_t ulNumRecords, size_t XDataLen, uint8_t *pucRepData)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTBatchscanThresholdCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf2f89dc7613730e834780827d1395feb</anchor>
      <arglist>)(uint8_t ucAdapterIf)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTTrackAdvEventCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaed8a224017ec478959379b9955904a81</anchor>
      <arglist>)(BTGattTrackAdvInfo_t *pxTrackAdvInfo)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTScanParameterSetupCompletedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab3124b3e8483a807a432781fc1415832</anchor>
      <arglist>)(uint8_t ucAdapterIf, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTPhyUpdatedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga604ebe340a3340ed69cc1a81d45f9e56</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTReadClientPhyCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga83fc4219b95ba8ef0999819d5b1cbee6</anchor>
      <arglist>)(uint16_t usConnId, uint8_t ucTxPhy, uint8_t ucRxPhy, BTStatus_t xStatus)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTClassicDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga562f663c025ee918c4c0705ba6239f7e</anchor>
      <arglist>)(BTStatus_t xStatus, uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTClassicRemoteDevicePropertiesCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga62b3f905fe07987bb706bac2d6e6c847</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxBdAddr, uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDeviceFoundCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa02b8d9903f1bb665f42220dffd23e74</anchor>
      <arglist>)(uint32_t ulNumProperties, BTClassicProperty_t *pxProperties)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTDiscoveryStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gab2ae428a5a108343805f15bedddb6bd4</anchor>
      <arglist>)(BTDiscoveryState_t xState)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>BTaclStateChangedCallback_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaabe3141baf8b9bda500f5cfa402c060f</anchor>
      <arglist>)(BTStatus_t xStatus, BTBdaddr_t *pxRemoteBdAddr, BTaclState_t xState)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAvConnectionState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2b792ffc38fe9fa955165dbc0429c03a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcConnectionStateDisconnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2b792ffc38fe9fa955165dbc0429c03aa61005cc759cf5654399ed8209cfe6cc8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcConnectionStateConnecting</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2b792ffc38fe9fa955165dbc0429c03aace816925482be5aec95626a841db31de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcConnectionStateConnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2b792ffc38fe9fa955165dbc0429c03aa9bff2fb95894ee395035cbf92f576bf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcConnectionStateDisconnecting</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2b792ffc38fe9fa955165dbc0429c03aad90b2388757a3535f4148c334784502e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAvAudioState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga5255dbdf8f3a717f628b251c1d807669</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcAudioStateRemoteSuspend</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga5255dbdf8f3a717f628b251c1d807669ad60d1c6c1d597a775a94127661d33bd7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcAudioStateStopped</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga5255dbdf8f3a717f628b251c1d807669a907d1237483173c1bc731f02905a9000</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAvsrcAudioStateStarted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga5255dbdf8f3a717f628b251c1d807669ad186292a1af21be576a4ec50436635f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattcError_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gadf029413bc01e41d5a1d15395c1ac054</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7898a8e84242bf38a2bbe3b130bbb3f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandStarted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a57c962c53c0c5cbf9907c20fd1df862b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandBusy</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7b1d8881d312a2f1a53dc0c4ef0335d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcCommandStored</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a7ca21f4df5dc579b34a9976caf8008c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcNoResources</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a9536585b4e6c1f8c45c755cdf91c64de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcModeUnsupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054aeb21644a26b4b5c44b1ce5af84631fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcIllegalValue</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a71b6c46284120af40042f0ca0f6e074c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcIncorrectState</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a210c6a2336af185c12c18ca5602c1318</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcUnknownAddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a435254b539f6c4f30f7242790ee32019</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcDeviceTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054af541b51dd6bc051ff31a60416dc85af9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcInvalidControllerOutput</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a758ba640f9ed1334f92654ea79e88899</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcSecurityError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054ad7daa7f776ff3defe712c1c05578b75c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcDelayedEncryptionCheck</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054a1b9312d7e04f80d4e8dd20f4da0a0543</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattcErrProcessing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf029413bc01e41d5a1d15395c1ac054ad3428549722d884bf2e602cbdbf62f6d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTRspErrorStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga968ba337d61fb6be716b8473ec97b319</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorNone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319ae1af56c9562f6efa35b7b2bfe875079f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorNo_mitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a59265ed653454848870015af2a7a2b8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a63fb666404e177358249052873b0a71e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorSignedNoMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319aed4b2446ae5950a0cd05132929518b32</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTRspErrorSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga968ba337d61fb6be716b8473ec97b319a555f5ae20e2c184614251bc3535a79bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae3f7d1f4e7d4188a250e314475352374</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a51060d4d256106d510420d10634566fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTInvalidHandle</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a921c38dfe11e27b0e2e5899349741261</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusReadNotPermitted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a83c88bde61b69ec7e03d64a122ed7b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusWriteNotPermitted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a53fd128e06da4e14c525b9283a9f615c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidPDU</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374adcd0c29a4f7322ec61aca670dafdffbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientAuthentication</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a78782eeb9d080bab86409bdac2a108bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusRequestNotSupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a69162ea5a53e48acc2b785fad12c37da</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidOffset</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a05db7b9ff1ca372c0d97361e10c6cab1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientAuthorization</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a5bbd004757d6efa11f3dfcf144d97dd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusPrepareQueueFull</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a39a3aceaa8def06ba89d2d605128ad75</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusAttributeNotFound</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a4e0f2f9562afaa512853aa3a2eff9fa8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusAttributeNotLong</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a577cffc6cc5f43e5ee6849fa80dedf01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientKeySize</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a7091c30daa7c60ce08c7423d9af14880</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInvalidAttributeLength</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a14358310b49ef4c2a0488b79e0106109</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusUnlikelyError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a7fe6ea959fbed7534518eb20799bd5b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientEncryption</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a21487939009d47907b56580df40c96c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusUnsupportedGroupType</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374afc0cd7f169a499a61f979cc2674dcce5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInsufficientResources</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a0c4bb7143ce3bb311b768e8b86dd8c95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusInternalError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a1109b99f8c87820bf0026805505d723f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374ac795b7646783fb11796a24bc25669995</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusConnectionCongested</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a068e4573dd3f1a26be06e5c4cb7318c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusErrorConnEstFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374af3a559d6b08c51b05e248f9c3e7a7eb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusErrorConnTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a3a828672a83ab5c19d25779136dd33a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTGattStatusLocalHostTerminatedConnection</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3f7d1f4e7d4188a250e314475352374a9d51a7c30364f875aa103dab65e492a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelNoSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55bada228a8bef37fc65d0f3cca6873dbb87</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelUnauthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba67f3762fb7926f3777afd3b5cf4bea9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelAuthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba620198c17d4d990f84c07db5bc728f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55badcf89fab404e21871d93d872b7f98028</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermRead</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8325a48a4a20814e53afb40db5f4ffea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a5785a0ecec6d54319e016f5f0bea0f6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a4a2db8c46dcf6ce637a104ed992d73bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWrite</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a3cfa3940a8668127035c18c4ba7c613d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8c0ffeb6004b07ba2a51be5196286fe1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a2ea723131d60886fe6fe7d3d1a3b2ed9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7aa970663c467e491b324a4c7cb3079c64</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7acdf62b170fa40bf1bcfc6053d1fa7369</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqBonding</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a16aeff1e8090908e7a0c9134b23dd6bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7afb35866e76777ffa8692277904a8bb3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a7946f00729be41f9eae6f9fd8adae9c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAttrWriteRequests_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga55c1e7ffff1cf5ea0164fa8e6795a0ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeNoResponse</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0cea9681e4b581a7f800d3146cb7de0ea12a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeDefault</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0cea1540792ce245577295d72ca319baf92a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypePrepare</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0ceacb8341dddbbf007ac7221d10dd112596</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTWriteTypeSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga55c1e7ffff1cf5ea0164fa8e6795a0ceaf4a0ec416dcea8d1591b85ee8df9e5b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTGattServiceTypes_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga42d9ff4543b70f554131656403ce0dde</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTServiceTypePrimary</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga42d9ff4543b70f554131656403ce0ddeac4af7bd10fe3fe69564551de8bbe2804</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTServiceTypeSecondary</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga42d9ff4543b70f554131656403ce0ddea2887a50ea7fdde925d35c3041f148fe3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDbAttributeType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae3c29649b63768bc288904e3f4202744</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbPrimaryService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a4d3ebaa6953a2f4b22abfb3b4688a525</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbSecondaryService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744aa6d9b379f74e9703140e3d9ffe4bfedf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbIncludedService</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744aa0126b319c857ea5f0f5811cb6787f49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbCharacteristicDecl</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a6cb593924840487c6e1e0304adb2c07c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbCharacteristic</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744a13d4e7209ea43c7ad5d50410b393404d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDbDescriptor</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggae3c29649b63768bc288904e3f4202744ace647fcd790e4d26626f62e4baf310ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelNoSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55bada228a8bef37fc65d0f3cca6873dbb87</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelUnauthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba67f3762fb7926f3777afd3b5cf4bea9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelAuthenticatedPairing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55ba620198c17d4d990f84c07db5bc728f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTSecLevelSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga06fc87d81c62e9abb8790b6e5713c55badcf89fab404e21871d93d872b7f98028</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermRead</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8325a48a4a20814e53afb40db5f4ffea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a5785a0ecec6d54319e016f5f0bea0f6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermReadEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a4a2db8c46dcf6ce637a104ed992d73bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWrite</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a3cfa3940a8668127035c18c4ba7c613d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncrypted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a8c0ffeb6004b07ba2a51be5196286fe1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteEncryptedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a2ea723131d60886fe6fe7d3d1a3b2ed9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSigned</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7aa970663c467e491b324a4c7cb3079c64</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPermWriteSignedMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7acdf62b170fa40bf1bcfc6053d1fa7369</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqBonding</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a16aeff1e8090908e7a0c9134b23dd6bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqMitm</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7afb35866e76777ffa8692277904a8bb3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTAuthReqSecureConnect</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggadf764cbdea00d65edcd07bb9953ad2b7a7946f00729be41f9eae6f9fd8adae9c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTTransport_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga456876e81e2dc3f1b233196a688b34e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportAuto</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2a7b8f8330380d42dbd4d2a2fab024c6ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportBredr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2ab1d2a19fe52fcf4f898fcf95d6fcd215</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BTTransportLe</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga456876e81e2dc3f1b233196a688b34e2a99b4c8bf4860ffa4b48c993e4f25c464</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga76ea5dede6ba720344e1faddbd2bac67</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTstateOff</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga76ea5dede6ba720344e1faddbd2bac67a19766ff9a46c2d818190c14ba6edb9b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTstateOn</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga76ea5dede6ba720344e1faddbd2bac67a0ba12f7393a0e1697ae0114b1769eaf8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAuthFailureReason_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga346e38e1a77ce4a4a5d9e3dcc794d09b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf0d115435eb821c9aa427fbe0962dac0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailLmpRespTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba72725624b14f91d9172938b06c1631c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailKeyMissing</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf99274687940754269bb2638146c6d9b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailEncrypMode</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba7ac2e29664b766239530230ed5fd0af1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailUnitKey</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba6fc54b30804a3c51348875ab90229fff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpCfrmValue</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bae31afd13b950243f4da008b418ff0a58</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpEnc</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09baf0a2bfa37c783235dbaebb1f1b3de478</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba19fb4ad75ab958136b07ea2a4e19f31a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailPageTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bacb0c32b82023ce6525d5e03cb47d93a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpPasskeyFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba96449eb5b38b18a41c8337f8f463a80f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpOobFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba0e41631aaca1b9b3581efdee3e477130</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba4c97b248d7e4854f308780c001750990</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailConnTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba08851c2f4a81eac93e4a9c86530bf9fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmp</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba9d5013c4442967b635e89561326fd7f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpPairNotSupport</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba174b800bf85d35d613537d4fcca244af</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpUnknownIo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09bad7cf34c1f83808f3a57d714cbd2fc1c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailSmpUnknown</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba3b274104c333ad2e6bde5379994268f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailHostRejectSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09babb1fc776951bc2ea0fa1ca72786ed450</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailInsuffSecurity</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba72f12f5da7ef0ea9c7f4fe83c74bd3a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailPeerUser</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba230a7995b2ebc3dc9f93e52412e54848</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTauthFailUnspecified</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga346e38e1a77ce4a4a5d9e3dcc794d09ba12cf40f57b7329e6ddb1e43fc7b7753b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTPropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga530175d220bd1e204bbefdb3b966f5d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBdname</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab02f34113978d7142e6779b86a2b67ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBdaddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a9121617b9e0155d5f2d7a7cc9bc71280</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyTypeOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ad3c1685b0709e1a97f92d84c700a2940</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyAdapterBondedDevices</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a58de461279def747be7b5ea9c8e56018</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteFriendlyName</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a6658547f4158588d0486b906e04b7300</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteRssi</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7adff655a4f0b9ff1d80be8124fdf602c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyRemoteVersionInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab3b46be8a73df86cf5f1c1fd88c77a63</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyLocalMTUSize</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a6318bb5b7791d773d9ed6a6b187ded9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyBondable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a26dba10da958c2132aadcffe738ec424</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyIO</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7a91abfcdd260b1052ace98dcc1dbaad6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertyConnectable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab39eecef92a7a995cad8948d2a668b6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTpropertySecureConnectionOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga530175d220bd1e204bbefdb3b966f5d7ab9b7832fd8b5f9b7b8e9a995741df673</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDeviceType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga846c67f839515745caaedb24eea443a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeBredr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6abb58d5d10fb99056d5f0466fe7db9346</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeBle</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6a5b777ca5358bb53fa7598e706dd77342</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTdeviceDevtypeDual</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga846c67f839515745caaedb24eea443a6ab5154bcba15fc7c333e2a8b263095dfc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTBondState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3b038e6390669d05aaa1b0d7ab8b4cee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTSspVariant_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga2add06926fac1c572de9f754eab88568</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyConfirmation</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a670fda7532def327e8c7da1af9283a8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyEntry</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a144c3f0f72dbc83b4af7271896a71dea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantConsent</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568ae380862deefa0e42596372a419679615</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTsspVariantPasskeyNotification</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga2add06926fac1c572de9f754eab88568a3e3d66782089ad2828db348b8ebc0ae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAddrType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa308d92c785fe8922cce3e1ef91f5e68</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTAdvProperties_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gae8f708573467e5503ca86021ab1b438e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTBlePropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gacf9b60f2e7d64cf18d46ae4a66d324d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyLocalLeFeatures</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggacf9b60f2e7d64cf18d46ae4a66d324d6a3d63788381d4042df232d7811a0368d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanMode_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gac777e1a3d6db541521c011a9c3a66378</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeNone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a164280095452f56cdf34be47afd87bdc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeConnectable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a82c11d80a0707e023e2092056c098dde</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanModeConnectableDiscoverable</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggac777e1a3d6db541521c011a9c3a66378a5d06debc4f829d352134e09f73ba5060</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanParam_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaa55a14fdd3e156cf0a0e2f4fada3a726</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPageScanInterval220ms</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaa55a14fdd3e156cf0a0e2f4fada3a726a600d9b8bfe2920c0e246fbddeaf5c85b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPageScanInterval1280ms</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaa55a14fdd3e156cf0a0e2f4fada3a726a65cf5716b31ae7d7ad5a740f2ce69fb8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTDiscoveryState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga36585806be9c4f9b584cbac871ddae96</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDiscoveryStopped</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga36585806be9c4f9b584cbac871ddae96a0b297bc4581e6c358cb26492cd5b0de2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTDiscoveryStarted</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga36585806be9c4f9b584cbac871ddae96ade632c087293593ffbf552a6432f32a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTaclState_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga0767b73311c42402d8ad53ff8f1360eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTaclStateConnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga0767b73311c42402d8ad53ff8f1360ebaece256ebffdcd98a04eeb6c390ac872a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTaclStateDisconnected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga0767b73311c42402d8ad53ff8f1360ebac9c49a5282c66cdfb353b4c227621ca5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTScanType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga29d4f9d0def7da97adf02176be01828e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanTypeStandard</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga29d4f9d0def7da97adf02176be01828ea6f8c11d8c4eca0b70ab2ab214b0698ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTScanTypeInterlaced</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga29d4f9d0def7da97adf02176be01828eacc3a9f1da5a1fb27aad9d261e56a182e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTClassicPropertyType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gaf3da26eb473acab9d2041dd49381a795</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyBdname</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a7bccfc4c7c498564d0bb9dedc233dd88</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyBdaddr</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a946d96da947f66dce8f99c16e9cced54</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyTypeOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a90096e17efba46b63d172f8c153f9cba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyUUIDS</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a35a44b95c1e2d61321336bf7a39556fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyClassOfDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a334358aa71b5d53e2194afa3abd2a7c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyServiceRecord</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795aa027603425a430ad4db78727b21a27a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterDiscoveryTimeout</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a42ea706107b99c997f8866390fd9a06f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterScanMode</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a233ba6b293063c998896c16e4e674471</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyEIRManfInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a4c1efec4ff78d702bee3700cf5956b48</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyAdapterScanParam</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a29b04d57ede6b23c9033c8fa0a6d9b33</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyConnectability</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795ab6e9683e71c18abab5d710d500ae69bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyScanType</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a58cadd3758d894fde512d86095271ec3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTPropertyGadgetEIRManfInfo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ggaf3da26eb473acab9d2041dd49381a795a63002d576e812cf65bdcbdf7634d9a46</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTuuidType_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga993ffac0caf6422a8419fe29dc02cf95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType16</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95af00ee220bbb21a208dea0db4c193ff38</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType32</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95a2e16322049fce6a01e12fe59b10b1985</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTuuidType128</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga993ffac0caf6422a8419fe29dc02cf95a5e14440c8f890ba846e4b85bf299741e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTStatus_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga291acc452b2f23af88200b307f42fe7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusSuccess</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da49b793cac6089c931ffd4b14d5c113d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusFail</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da9cf206edeab8259ddaf8e49634cdc42d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusNotReady</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7dae48901aeb6bcad08bd5efb1e075eee55</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusNoMem</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daf5c8e6b4a3d1bde5773063f22a964fa0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusBusy</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da1622349c03760d0560dbf1b5776c8c72</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusDone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da8f728a7ac3567c2ecb28e84fe5870455</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusUnsupported</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daa81031081ee2b35ab432f73a00525c6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusParamInvalid</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da9e7af3fc8465c3d71ccf0f017ff260d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusUnHandled</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7daa7c9eb7b4a4cd7e17c73c4654a79864c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusAuthFailure</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da46f85b08263c5464678f1e7f5d638ec5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusRMTDevDown</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7dae24c6829afe84e49ff66f912e57577a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusAuthRejected</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da23cde6d940b0c57cc1c8c8bac319c148</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusWakeLockError</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da5d149dd53538aba2b3aa577c7d008849</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTStatusLinkLoss</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga291acc452b2f23af88200b307f42fe7da13e343a720acc82bb0fc79cf9ee3baeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTProfile_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga3d56f24cb4a721748184d2f361bd3d74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eProfileHeadset</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aa038e4964a2593a2c8b891544c9dd5e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileA2DP</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a5cf777e07744fe8513cbea2965f53b62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileAVRCPTarget</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aaaeb18992fda3dd49d54243cd80023e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileA2DPSink</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a08572d36f2b3acaf243c750c3574bce9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileAVRCPController</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74aebb301a785f08ad218eacd030c92421b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileInputDevice</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a02d3336f3afe1ec8c1dff71ea3259658</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileHeadsetClient</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a1aaae56b98af7d53229474d777ad3777</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileGATT</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74af9d48553da005c4a98865659df3feb05</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileSock</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74a2c1b4122c7f7c93211dc8c7fc12b0dd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTProfileMaxID</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga3d56f24cb4a721748184d2f361bd3d74ac596b44fff1c19e6246ef8c0c8feda42</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BTIOtypes_t</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>ga646cea95f6a31da98ac491d843b23bbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIONone</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaf435a06183502b7b1461949f4d482481</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIODisplayOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaae38272bc4f06ca91fbc96e6ce246537</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIODisplayYesNo</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbba0cee846f770b51a844df73cdd698f278</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIOKeyboardOnly</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbba45fff80dceef959356013afc0b1a1ac3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eBTIOKeyboardDisplay</name>
      <anchorfile>group___h_a_l___b_l_u_e_t_o_o_t_h.html</anchorfile>
      <anchor>gga646cea95f6a31da98ac491d843b23bbbaea5c462144661f39066c827678353413</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>ble_tests</name>
    <title>Tests</title>
    <filename>ble_tests.html</filename>
  </compound>
  <compound kind="page">
    <name>ble_config</name>
    <title>Configuration</title>
    <filename>ble_config.html</filename>
  </compound>
  <compound kind="page">
    <name>BLE_functions</name>
    <title>Functions</title>
    <filename>_b_l_e_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>ble_constants</name>
    <title>Constants</title>
    <filename>ble_constants.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_functions</name>
    <title>Functions</title>
    <filename>iotble_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_init</name>
    <title>IotBle_Init</title>
    <filename>iotble_function_init.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_on</name>
    <title>IotBle_On</title>
    <filename>iotble_function_on.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_off</name>
    <title>IotBle_Off</title>
    <filename>iotble_function_off.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_startadv</name>
    <title>IotBle_StartAdv</title>
    <filename>iotble_function_startadv.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_stopadv</name>
    <title>IotBle_StopAdv</title>
    <filename>iotble_function_stopadv.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_connparameterupdaterequest</name>
    <title>IotBle_ConnParameterUpdateRequest</title>
    <filename>iotble_function_connparameterupdaterequest.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_registereventcb</name>
    <title>IotBle_RegisterEventCb</title>
    <filename>iotble_function_registereventcb.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_unregistereventcb</name>
    <title>IotBle_UnRegisterEventCb</title>
    <filename>iotble_function_unregistereventcb.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_removebond</name>
    <title>IotBle_RemoveBond</title>
    <filename>iotble_function_removebond.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_createservice</name>
    <title>IotBle_CreateService</title>
    <filename>iotble_function_createservice.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_deleteservice</name>
    <title>IotBle_DeleteService</title>
    <filename>iotble_function_deleteservice.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_sendindication</name>
    <title>IotBle_SendIndication</title>
    <filename>iotble_function_sendindication.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_sendresponse</name>
    <title>IotBle_SendResponse</title>
    <filename>iotble_function_sendresponse.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_getconnectioninfolist</name>
    <title>IotBle_GetConnectionInfoList</title>
    <filename>iotble_function_getconnectioninfolist.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_getconnectioninfo</name>
    <title>IotBle_GetConnectionInfo</title>
    <filename>iotble_function_getconnectioninfo.html</filename>
  </compound>
  <compound kind="page">
    <name>iotble_function_confirmnumericcomparisonkeys</name>
    <title>IotBle_ConfirmNumericComparisonKeys</title>
    <filename>iotble_function_confirmnumericcomparisonkeys.html</filename>
  </compound>
  <compound kind="page">
    <name>iotbledeviceinfo_functions</name>
    <title>Functions</title>
    <filename>iotbledeviceinfo_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>iotbledeviceinfo_function_init</name>
    <title>IotBleDeviceInfo_Init</title>
    <filename>iotbledeviceinfo_function_init.html</filename>
  </compound>
  <compound kind="page">
    <name>iotbledeviceinfo_function_cleanup</name>
    <title>IotBleDeviceInfo_Cleanup</title>
    <filename>iotbledeviceinfo_function_cleanup.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_functions</name>
    <title>Functions</title>
    <filename>iotblewifiprov_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_init</name>
    <title>IotBleWifiProv_Init</title>
    <filename>iotblewifiprov_function_init.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_start</name>
    <title>IotBleWifiProv_Start</title>
    <filename>iotblewifiprov_function_start.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_stop</name>
    <title>IotBleWifiProv_Stop</title>
    <filename>iotblewifiprov_function_stop.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_getnumnetworks</name>
    <title>IotBleWifiProv_GetNumNetworks</title>
    <filename>iotblewifiprov_function_getnumnetworks.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_connect</name>
    <title>IotBleWifiProv_Connect</title>
    <filename>iotblewifiprov_function_connect.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_eraseallnetworks</name>
    <title>IotBleWifiProv_EraseAllNetworks</title>
    <filename>iotblewifiprov_function_eraseallnetworks.html</filename>
  </compound>
  <compound kind="page">
    <name>iotblewifiprov_function_delete</name>
    <title>IotBleWifiProv_Delete</title>
    <filename>iotblewifiprov_function_delete.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
    <docanchor file="index.html">iotble</docanchor>
    <docanchor file="index.html" title="Dependencies">ble_dependencies</docanchor>
  </compound>
</tagfile>
