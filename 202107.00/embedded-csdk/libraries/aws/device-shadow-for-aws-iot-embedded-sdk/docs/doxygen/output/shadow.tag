<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>shadow.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/source/</path>
    <filename>shadow_8c.html</filename>
    <includes id="shadow_8h" name="shadow.h" local="yes" imported="no">shadow.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_MAX_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a5cbe106195747b791e600d4fb1f3fda3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_THINGNAME_MAX_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a590b20b05734d1d9e9bebc29fcc0e4c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_ACCEPTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a3314f371c8b2ef0884e6508e3afdaf41</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_REJECTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a2691fddfde4eabf6421c4a6eb97ff11f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_DELTA</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>aab93fbc2127186ea1859254e3587c716</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_DOCUMENTS</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a14949898540fe2c8637dde7990043a21</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_ACCEPTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>ab9e1c6c82ed719cc442bdd5e44de9c10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_REJECTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a6143bdaf412b551a03bf77d8d7a17dd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_ACCEPTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a2f5623d61e9678b3ebd6f2264cb477c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_REJECTED</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a0ab1b932d1f053f46a2eb9d761efcb80</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_ACCEPTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a8f3d68afa47e544c0853d00c20a4318d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_REJECTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>af0dd9ca4f193c487d2b073616af49da1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_DOCUMENTS_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a9b96e303bacfd5d61d1fb7ec7cb33573</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_DELTA_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>ae60e52c043a851b445e687aca9851fe9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_ACCEPTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a193688dfbfadcf4e62cedc1954e2f90a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_REJECTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a82f4f20e9a4637dcc8319cd58238c01f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_ACCEPTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a2bd6b1ba6013dbe3dce4dfafd416f6c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_REJECTED_LENGTH</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a47d5acf6c9fec029e83d321c26653c65</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>validateMatchTopicParameters</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>ab2cbf9f827397771bf969724c7d2b6a9</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, const ShadowMessageType_t *pMessageType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>validateAssembleTopicParameters</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a1445204e4fbb3ae02539f19973c9bfba</anchor>
      <arglist>(ShadowTopicStringType_t topicType, const char *pThingName, uint8_t thingNameLength, const char *pShadowName, uint8_t shadowNameLength, const char *pTopicBuffer, const uint16_t *pOutLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>containsSubString</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>aad2a9736dcde952f1fc81a2f2dd9ef9c</anchor>
      <arglist>(const char *pString, uint16_t stringLength, const char *pSubString, uint16_t subStringLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>validateName</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>aba1a36ac405c3b9267f3c0b72c1f8122</anchor>
      <arglist>(const char *pString, uint16_t stringLength, uint8_t maxAllowedLength, uint8_t *pNameLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>extractShadowMessageType</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a44ac471c87ff242631452113f64db82b</anchor>
      <arglist>(const char *pString, uint16_t stringLength, ShadowMessageType_t *pMessageType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>extractThingName</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>ab1c3a9bbd3ce20d89a7bb24f71e42eab</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, uint16_t *pConsumedTopicLength, uint8_t *pThingNameLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ShadowStatus_t</type>
      <name>extractShadowRootAndName</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>ae7ecd63bc468de04377902858a53a273</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, uint16_t *pConsumedTopicLength, uint8_t *pShadowNameLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static const char *</type>
      <name>getShadowOperationString</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a55cea962b2488282937660bf77e495be</anchor>
      <arglist>(ShadowTopicStringType_t topicType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint16_t</type>
      <name>getShadowOperationLength</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a055099a3dec4d191e4e681483d0096f8</anchor>
      <arglist>(ShadowTopicStringType_t topicType)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>createShadowTopicString</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>aa51803491b68e80589eef184df92dab2</anchor>
      <arglist>(ShadowTopicStringType_t topicType, const char *pThingName, uint8_t thingNameLength, const char *pShadowName, uint8_t shadowNameLength, char *pTopicBuffer)</arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_MatchTopicString</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a14cc590d8d8e6a0a50e482b0134a3e1d</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, ShadowMessageType_t *pMessageType, const char **pThingName, uint8_t *pThingNameLength, const char **pShadowName, uint8_t *pShadowNameLength)</arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_AssembleTopicString</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a62cae7ffa13611e1ee52128934c63c3b</anchor>
      <arglist>(ShadowTopicStringType_t topicType, const char *pThingName, uint8_t thingNameLength, const char *pShadowName, uint8_t shadowNameLength, char *pTopicBuffer, uint16_t bufferSize, uint16_t *pOutLength)</arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_MatchTopic</name>
      <anchorfile>shadow_8c.html</anchorfile>
      <anchor>a3ba8ea42875554cbec68d99178ef948c</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, ShadowMessageType_t *pMessageType, const char **pThingName, uint16_t *pThingNameLength)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>shadow.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>shadow_8h.html</filename>
    <includes id="shadow__config__defaults_8h" name="shadow_config_defaults.h" local="yes" imported="no">shadow_config_defaults.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_PREFIX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga247201767a9d4e96e68659b3d45b56fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_PREFIX_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga59c22b8ca0015dbfc31e791d25a99e1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_CLASSIC_ROOT</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga2df6e35154585f33c2d05e6a3704d94d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_CLASSIC_ROOT_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga065e8dae9780f7f4155bdd2d3009fb30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAMED_ROOT</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d823051abc0619a4923ec84f082bba5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAMED_ROOT_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3bfe09c0b85254cc8253d5499e38f776</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gae632f7b67feb5e0523a2d417bc269808</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga587bbe5229d85b6080f2b0cc7fda193c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3ad1b29d634829613e9a9c15b32cdfca</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga95123f616be6c6daea8b87922e37f748</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga9af4141be718b1c3722c4b6af2a9e8d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3aff5db93f1b232b414b01553dedbea4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_ACCEPTED</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga2555351e653e1db780c9868d1bb2b509</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_ACCEPTED_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3264059dc3013a79e18b67bc06d88f69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_REJECTED</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga1a0642131b9a46928e52e77edd12d5d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_REJECTED_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga77f4b26fbe2704fdd487176b2b264b9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga240b2250a6d4c2ff22efd09f44467302</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DELTA_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad7ff1f3d0c7c04350e0cab841583dd10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DOCUMENTS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gadbc41cdfbace3887ccae228abae2496a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DOCUMENTS_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga77996f84fbc11a25684f1efff4c5d39b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_NULL</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga8c4621347f122281206cc7e720e74862</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_NULL_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3ae17b88b5eaf29c2f7c351d64a43b5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_THINGNAME_LENGTH_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga869560502bc342389e18abb4a8dcc44c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_LENGTH_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaeec411b1db744d8a2b9c38df7addbb35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_CLASSIC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3e907070bdaef2ff4858185523a6bfa2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_CLASSIC_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga881aa54cba5b4d796d23327ed6b1fd41</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d3c83f41203b9ab208eb3ec064fdd40</anchor>
      <arglist>(operationLength, suffixLength, thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad34496cc83acd544c041db4668ed6588</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga67b99c5643c599287e16e7642829d960</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d0d33650171cf7bff585549a3a5fd59</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_DOCS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga581b64e6f2d870bdd7a28258255e284c</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad7a18c46a12b7b3a57b819c303bc8593</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga85280cc66085d714287ec4d52e6312d8</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaef17e7c96ea20b6bdd27696cbea684d5</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga25ce1e96eb56eab5073402e491d743e6</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga544b69ac04c6621357162b7d3947065f</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaae719bcfa4700bdf34194cbf4bf98716</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaafd489e72189e2211d4693b66754d1ba</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga10df53c8dbebf2be45828ef7e072d61a</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gadc8a22bebccd83e6a01e6391f9abed4c</anchor>
      <arglist>(thingName, shadowName, operation, suffix)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga7dced9a78b23e6f4b705d9106aab34f4</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga7938a0ec40256ff522989f6cebdd5615</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3797f09c9ad8a5eac90edd4c26ad64dd</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_DOCS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gafbf92f891c437a74a34d4008144ad77a</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gafc701e6f9462d1df14f95b78d861d9ef</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga86212bffcd99191c1388e0b36dc864e3</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaa98622d58233595101ec9a7a994878a3</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga81ed8941dc2f8cdfbab5ce54be145f9b</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad9b45f032470dccc367ad1d56f8c3f45</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaf7274c2851db832885ae185c229a8b86</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga9962475913e37e81e7b3528023f7b284</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>af220bc585744d3226c370bf7d108be43</anchor>
      <arglist>(operationLength, suffixLength, thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_UPDATE</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a2d88d5b629b2949e5e88a4bb99b9824c</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_UPDATE_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a0f9cfc57b778ea1800b67936de7889d8</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_UPDATE_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>adca90dd211f1814c64b0f8e0e10c7518</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_UPDATE_DOCUMENTS</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a1530af1581d500a8ad3a4a33e60db725</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_UPDATE_DELTA</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a2590f480f923d12cbf5d42d658f33907</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_GET</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>aaffe89fecaad4540e70d022474da112a</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_GET_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a82bd24b19dc44b91581925419e881b4c</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_GET_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a12a39264a971cb83266965abc7f4d2a1</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_DELETE</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a21c97aa81ad721f2b7296a7feca20c12</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_DELETE_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a9b090af699bc85da71bbe2c9a8983fb0</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_DELETE_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a42e06e23712168c9d78d24c32b439b13</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LENGTH_MAX</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a68b2bddcf124940a061481c32218a600</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>aad6bc554cb9be80281457bfcaf0e9eea</anchor>
      <arglist>(thingName, operation, suffix)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_UPDATE</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a3b22295c60ac864ce115f9d9217f2ebb</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_UPDATE_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>ab71da3b18dfe48daf56257114868bd25</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_UPDATE_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a84c91c5635c869c96295cc880842bd44</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_UPDATE_DOCUMENTS</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>ad61164381eb932aff862e9eb193d6b97</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_UPDATE_DELTA</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>af4c894d53a086e805c9202dc51393a5a</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_GET</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a2104c07f91b0ed6dbac9404c03ec2928</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_GET_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a002707cf823ffd3568138a1743956161</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_GET_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a042631b8ff0edcb69f9d7454f8b37ece</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_DELETE</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>aff9adba7f638da1983449b2f4106f584</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_DELETE_ACCEPTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>ab670e88ba87afb267d15dccce29602f6</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STRING_DELETE_REJECTED</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a9e0d8a4cef6d2795c3f96546180bb0a5</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>Shadow_GetTopicString</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a1141285b0f4bf6315e3beb9f6ce2978a</anchor>
      <arglist>(topicType, pThingName, thingNameLength, pTopicBuffer, bufferSize, pOutLength)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ShadowMessageType_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gac953015263099970421c6af6ce1ccba9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ShadowTopicStringType_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>ga22d6eeafd1388d2392ef488d4281b813</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ShadowStatus_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>ga41ed1ee9c48c83638bef476fd2773398</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_SUCCESS</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a568e9c9ebfb0ae4d11587ea70694f855</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_FAIL</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398ac012ee2b49d3335869e406c7fbaa81b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_BAD_PARAMETER</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a0de4231574a9757a55db7de7adf36dcb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_BUFFER_TOO_SMALL</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398ab3f0d6b2e5376c60bab1fa000c0f6b3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_THINGNAME_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398afa6fde5609e538c824bdec4c4f2b5267</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_MESSAGE_TYPE_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a7745c90794ce2c9250f27708364f3630</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_ROOT_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a15e1e9e30ba821e818646ec99328d933</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_SHADOWNAME_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398aaf659f822e7f1800c41b1e009bc724a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_AssembleTopicString</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a62cae7ffa13611e1ee52128934c63c3b</anchor>
      <arglist>(ShadowTopicStringType_t topicType, const char *pThingName, uint8_t thingNameLength, const char *pShadowName, uint8_t shadowNameLength, char *pTopicBuffer, uint16_t bufferSize, uint16_t *pOutLength)</arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_MatchTopicString</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a14cc590d8d8e6a0a50e482b0134a3e1d</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, ShadowMessageType_t *pMessageType, const char **pThingName, uint8_t *pThingNameLength, const char **pShadowName, uint8_t *pShadowNameLength)</arglist>
    </member>
    <member kind="function">
      <type>ShadowStatus_t</type>
      <name>Shadow_MatchTopic</name>
      <anchorfile>shadow_8h.html</anchorfile>
      <anchor>a3ba8ea42875554cbec68d99178ef948c</anchor>
      <arglist>(const char *pTopic, uint16_t topicLength, ShadowMessageType_t *pMessageType, const char **pThingName, uint16_t *pThingNameLength)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>shadow_config_defaults.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/device-shadow-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>shadow__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_DO_NOT_USE_CUSTOM_CONFIG</name>
      <anchorfile>shadow__config__defaults_8h.html</anchorfile>
      <anchor>a728b94a3302bb00789aba09241a0256f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogError</name>
      <anchorfile>shadow__config__defaults_8h.html</anchorfile>
      <anchor>a8d9dbaaa88129137a4c68ba0456a18b1</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogWarn</name>
      <anchorfile>shadow__config__defaults_8h.html</anchorfile>
      <anchor>a7da92048aaf0cbfcacde9539c98a0e05</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogInfo</name>
      <anchorfile>shadow__config__defaults_8h.html</anchorfile>
      <anchor>a00810b1cb9d2f25d25ce2d4d93815fba</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogDebug</name>
      <anchorfile>shadow__config__defaults_8h.html</anchorfile>
      <anchor>af60e8ffc327d136e5d0d8441ed98c98d</anchor>
      <arglist>(message)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>shadow_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__shadow__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ShadowMessageType_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gac953015263099970421c6af6ce1ccba9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ShadowTopicStringType_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>ga22d6eeafd1388d2392ef488d4281b813</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ShadowStatus_t</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>ga41ed1ee9c48c83638bef476fd2773398</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_SUCCESS</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a568e9c9ebfb0ae4d11587ea70694f855</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_FAIL</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398ac012ee2b49d3335869e406c7fbaa81b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_BAD_PARAMETER</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a0de4231574a9757a55db7de7adf36dcb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_BUFFER_TOO_SMALL</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398ab3f0d6b2e5376c60bab1fa000c0f6b3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_THINGNAME_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398afa6fde5609e538c824bdec4c4f2b5267</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_MESSAGE_TYPE_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a7745c90794ce2c9250f27708364f3630</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_ROOT_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398a15e1e9e30ba821e818646ec99328d933</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SHADOW_SHADOWNAME_PARSE_FAILED</name>
      <anchorfile>group__shadow__enum__types.html</anchorfile>
      <anchor>gga41ed1ee9c48c83638bef476fd2773398aaf659f822e7f1800c41b1e009bc724a5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>shadow_constants</name>
    <title>Constants</title>
    <filename>group__shadow__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_PREFIX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga247201767a9d4e96e68659b3d45b56fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_PREFIX_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga59c22b8ca0015dbfc31e791d25a99e1c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_CLASSIC_ROOT</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga2df6e35154585f33c2d05e6a3704d94d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_CLASSIC_ROOT_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga065e8dae9780f7f4155bdd2d3009fb30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAMED_ROOT</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d823051abc0619a4923ec84f082bba5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAMED_ROOT_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3bfe09c0b85254cc8253d5499e38f776</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gae632f7b67feb5e0523a2d417bc269808</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_DELETE_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga587bbe5229d85b6080f2b0cc7fda193c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3ad1b29d634829613e9a9c15b32cdfca</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_GET_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga95123f616be6c6daea8b87922e37f748</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga9af4141be718b1c3722c4b6af2a9e8d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_OP_UPDATE_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3aff5db93f1b232b414b01553dedbea4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_ACCEPTED</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga2555351e653e1db780c9868d1bb2b509</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_ACCEPTED_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3264059dc3013a79e18b67bc06d88f69</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_REJECTED</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga1a0642131b9a46928e52e77edd12d5d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_REJECTED_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga77f4b26fbe2704fdd487176b2b264b9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga240b2250a6d4c2ff22efd09f44467302</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DELTA_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad7ff1f3d0c7c04350e0cab841583dd10</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DOCUMENTS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gadbc41cdfbace3887ccae228abae2496a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_DOCUMENTS_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga77996f84fbc11a25684f1efff4c5d39b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_NULL</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga8c4621347f122281206cc7e720e74862</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_SUFFIX_NULL_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3ae17b88b5eaf29c2f7c351d64a43b5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_THINGNAME_LENGTH_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga869560502bc342389e18abb4a8dcc44c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_LENGTH_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaeec411b1db744d8a2b9c38df7addbb35</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_CLASSIC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3e907070bdaef2ff4858185523a6bfa2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_NAME_CLASSIC_LENGTH</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga881aa54cba5b4d796d23327ed6b1fd41</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d3c83f41203b9ab208eb3ec064fdd40</anchor>
      <arglist>(operationLength, suffixLength, thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad34496cc83acd544c041db4668ed6588</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga67b99c5643c599287e16e7642829d960</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga0d0d33650171cf7bff585549a3a5fd59</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_DOCS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga581b64e6f2d870bdd7a28258255e284c</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_UPDATE_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad7a18c46a12b7b3a57b819c303bc8593</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga85280cc66085d714287ec4d52e6312d8</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaef17e7c96ea20b6bdd27696cbea684d5</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_GET_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga25ce1e96eb56eab5073402e491d743e6</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga544b69ac04c6621357162b7d3947065f</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaae719bcfa4700bdf34194cbf4bf98716</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_DELETE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaafd489e72189e2211d4693b66754d1ba</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_LEN_MAX</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga10df53c8dbebf2be45828ef7e072d61a</anchor>
      <arglist>(thingNameLength, shadowNameLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gadc8a22bebccd83e6a01e6391f9abed4c</anchor>
      <arglist>(thingName, shadowName, operation, suffix)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga7dced9a78b23e6f4b705d9106aab34f4</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga7938a0ec40256ff522989f6cebdd5615</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga3797f09c9ad8a5eac90edd4c26ad64dd</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_DOCS</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gafbf92f891c437a74a34d4008144ad77a</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_UPDATE_DELTA</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gafc701e6f9462d1df14f95b78d861d9ef</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga86212bffcd99191c1388e0b36dc864e3</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaa98622d58233595101ec9a7a994878a3</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_GET_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga81ed8941dc2f8cdfbab5ce54be145f9b</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gad9b45f032470dccc367ad1d56f8c3f45</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE_ACC</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>gaf7274c2851db832885ae185c229a8b86</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SHADOW_TOPIC_STR_DELETE_REJ</name>
      <anchorfile>group__shadow__constants.html</anchorfile>
      <anchor>ga9962475913e37e81e7b3528023f7b284</anchor>
      <arglist>(thingName, shadowName)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>shadow_design</name>
    <title>Design</title>
    <filename>shadow_design.html</filename>
  </compound>
  <compound kind="page">
    <name>shadow_config</name>
    <title>Configurations</title>
    <filename>shadow_config.html</filename>
    <docanchor file="shadow_config.html">SHADOW_DO_NOT_USE_CUSTOM_CONFIG</docanchor>
    <docanchor file="shadow_config.html" title="LogError">shadow_logerror</docanchor>
    <docanchor file="shadow_config.html" title="LogWarn">shadow_logwarn</docanchor>
    <docanchor file="shadow_config.html" title="LogInfo">shadow_loginfo</docanchor>
    <docanchor file="shadow_config.html" title="LogDebug">shadow_logdebug</docanchor>
  </compound>
  <compound kind="page">
    <name>shadow_functions</name>
    <title>Functions</title>
    <filename>shadow_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>shadow_matchtopicstring_function</name>
    <title>Shadow_MatchTopicString</title>
    <filename>shadow_matchtopicstring_function.html</filename>
  </compound>
  <compound kind="page">
    <name>shadow_assembletopicstring_function</name>
    <title>Shadow_AssembleTopicString</title>
    <filename>shadow_assembletopicstring_function.html</filename>
  </compound>
  <compound kind="page">
    <name>shadow_porting</name>
    <title>Porting Guide</title>
    <filename>shadow_porting.html</filename>
    <docanchor file="shadow_porting.html" title="Configuration Macros">shadow_porting_config</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">shadow</docanchor>
    <docanchor file="index.html" title="Memory Requirements">shadow_memory_requirements</docanchor>
  </compound>
</tagfile>
