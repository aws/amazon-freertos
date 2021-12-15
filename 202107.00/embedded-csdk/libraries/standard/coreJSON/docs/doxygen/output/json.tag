<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>core_json.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreJSON/source/</path>
    <filename>core__json_8c.html</filename>
    <includes id="core__json_8h" name="core_json.h" local="yes" imported="no">core_json.h</includes>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_Validate</name>
      <anchorfile>core__json_8c.html</anchorfile>
      <anchor>af4eb1eb1e2a2563abd0d5404b04aa9ff</anchor>
      <arglist>(const char *buf, size_t max)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_SearchConst</name>
      <anchorfile>core__json_8c.html</anchorfile>
      <anchor>a25a2e8856cd0ca13501af4c12ad25096</anchor>
      <arglist>(const char *buf, size_t max, const char *query, size_t queryLength, const char **outValue, size_t *outValueLength, JSONTypes_t *outType)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_SearchT</name>
      <anchorfile>core__json_8c.html</anchorfile>
      <anchor>a5d11f980c095f94b2fe6cb8833933663</anchor>
      <arglist>(char *buf, size_t max, const char *query, size_t queryLength, char **outValue, size_t *outValueLength, JSONTypes_t *outType)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_Iterate</name>
      <anchorfile>core__json_8c.html</anchorfile>
      <anchor>aabbdd89124648df7b21e4f7215cd23dd</anchor>
      <arglist>(const char *buf, size_t max, size_t *start, size_t *next, JSONPair_t *outPair)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_json.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreJSON/source/include/</path>
    <filename>core__json_8h.html</filename>
    <class kind="struct">JSONPair_t</class>
    <member kind="define">
      <type>#define</type>
      <name>JSON_Search</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>a1955fd7e92837fa6f234bdfb511cc59d</anchor>
      <arglist>(buf, max, query, queryLength, outValue, outValueLength)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_INDEX_VALUE</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>aef79ff8de4cb794d9b5415a403119607</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JSONStatus_t</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ga2b720193877345b237518677216f00a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONPartial</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0ae06fe683a38c3fbd8cf55f0324c4a1ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONSuccess</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a6b579c45491843ff8dbc535fb17a0402</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONIllegalDocument</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a5a11baf84e1d3a36dd78c3e87b420d4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONMaxDepthExceeded</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a95c8555b742422f8a4a3d47f586082f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNotFound</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a2f53f1dedfe8a356102000360cab5c4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNullParameter</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0ac600e51779a633bc071b876e1b8144a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONBadParameter</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a8bdd764c656184f942d493fecd12afb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JSONTypes_t</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gae84f98faf50c290deab5278913de93d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONInvalid</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9ae087f2ebf4a5890911eca3aa79de46c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONString</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a8564601104ed8e533c081049adbe27af</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNumber</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a5ca5f2586a794838072e4b047dc922f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONTrue</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a815a9b46b68290aed0646e2cfb02da6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONFalse</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9ab415d1e19653af0904b277155578911f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNull</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a2be3130ff4fda556d345e4ac90267bed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONObject</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a1d957a852e0969b2839082414860b85c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONArray</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a9e2a41c9b40c722fb4e6106289a2e870</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_Validate</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>af4eb1eb1e2a2563abd0d5404b04aa9ff</anchor>
      <arglist>(const char *buf, size_t max)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_SearchT</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>a5d11f980c095f94b2fe6cb8833933663</anchor>
      <arglist>(char *buf, size_t max, const char *query, size_t queryLength, char **outValue, size_t *outValueLength, JSONTypes_t *outType)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_SearchConst</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>a25a2e8856cd0ca13501af4c12ad25096</anchor>
      <arglist>(const char *buf, size_t max, const char *query, size_t queryLength, const char **outValue, size_t *outValueLength, JSONTypes_t *outType)</arglist>
    </member>
    <member kind="function">
      <type>JSONStatus_t</type>
      <name>JSON_Iterate</name>
      <anchorfile>core__json_8h.html</anchorfile>
      <anchor>aabbdd89124648df7b21e4f7215cd23dd</anchor>
      <arglist>(const char *buf, size_t max, size_t *start, size_t *next, JSONPair_t *outPair)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>JSONPair_t</name>
    <filename>struct_j_s_o_n_pair__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>struct_j_s_o_n_pair__t.html</anchorfile>
      <anchor>ae1b11b39a8e57ab6880bc69346bebb37</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>keyLength</name>
      <anchorfile>struct_j_s_o_n_pair__t.html</anchorfile>
      <anchor>a86253f1a1ac1478a39cf60063b189d5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>value</name>
      <anchorfile>struct_j_s_o_n_pair__t.html</anchorfile>
      <anchor>a29f8e22baab1b7ae15cc649437fb0fb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>valueLength</name>
      <anchorfile>struct_j_s_o_n_pair__t.html</anchorfile>
      <anchor>abb0c5909612fb388fbdd662d8b9b486a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>JSONTypes_t</type>
      <name>jsonType</name>
      <anchorfile>struct_j_s_o_n_pair__t.html</anchorfile>
      <anchor>a9573fc5ea1fa08c78490b28e4bbef588</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>json_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__json__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>JSONStatus_t</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ga2b720193877345b237518677216f00a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONPartial</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0ae06fe683a38c3fbd8cf55f0324c4a1ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONSuccess</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a6b579c45491843ff8dbc535fb17a0402</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONIllegalDocument</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a5a11baf84e1d3a36dd78c3e87b420d4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONMaxDepthExceeded</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a95c8555b742422f8a4a3d47f586082f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNotFound</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a2f53f1dedfe8a356102000360cab5c4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNullParameter</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0ac600e51779a633bc071b876e1b8144a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONBadParameter</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gga2b720193877345b237518677216f00a0a8bdd764c656184f942d493fecd12afb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JSONTypes_t</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>gae84f98faf50c290deab5278913de93d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONInvalid</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9ae087f2ebf4a5890911eca3aa79de46c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONString</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a8564601104ed8e533c081049adbe27af</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNumber</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a5ca5f2586a794838072e4b047dc922f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONTrue</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a815a9b46b68290aed0646e2cfb02da6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONFalse</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9ab415d1e19653af0904b277155578911f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONNull</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a2be3130ff4fda556d345e4ac90267bed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONObject</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a1d957a852e0969b2839082414860b85c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JSONArray</name>
      <anchorfile>group__json__enum__types.html</anchorfile>
      <anchor>ggae84f98faf50c290deab5278913de93d9a9e2a41c9b40c722fb4e6106289a2e870</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>json_struct_types</name>
    <title>Struct Types</title>
    <filename>group__json__struct__types.html</filename>
    <class kind="struct">JSONPair_t</class>
  </compound>
  <compound kind="page">
    <name>json_functions</name>
    <title>Functions</title>
    <filename>json_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>json_validate_function</name>
    <title>JSON_Validate</title>
    <filename>json_validate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>json_search_function</name>
    <title>JSON_Search</title>
    <filename>json_search_function.html</filename>
  </compound>
  <compound kind="page">
    <name>json_searcht_function</name>
    <title>JSON_SearchT</title>
    <filename>json_searcht_function.html</filename>
  </compound>
  <compound kind="page">
    <name>json_searchconst_function</name>
    <title>JSON_SearchConst</title>
    <filename>json_searchconst_function.html</filename>
  </compound>
  <compound kind="page">
    <name>json_iterate_function</name>
    <title>JSON_Iterate</title>
    <filename>json_iterate_function.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">json</docanchor>
    <docanchor file="index.html" title="Memory Requirements">json_memory_requirements</docanchor>
    <docanchor file="index.html" title="Design">json_design</docanchor>
  </compound>
</tagfile>
