<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>backoff_algorithm.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/</path>
    <filename>backoff__algorithm_8c.html</filename>
    <includes id="backoff__algorithm_8h" name="backoff_algorithm.h" local="yes" imported="no">backoff_algorithm.h</includes>
    <member kind="function">
      <type>BackoffAlgorithmStatus_t</type>
      <name>BackoffAlgorithm_GetNextBackoff</name>
      <anchorfile>backoff__algorithm_8c.html</anchorfile>
      <anchor>ae9dc843e520435569f40e8832791ecbe</anchor>
      <arglist>(BackoffAlgorithmContext_t *pRetryContext, uint32_t randomValue, uint16_t *pNextBackOff)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>BackoffAlgorithm_InitializeParams</name>
      <anchorfile>backoff__algorithm_8c.html</anchorfile>
      <anchor>ac37ecab230cf5da8f70547ec63776a48</anchor>
      <arglist>(BackoffAlgorithmContext_t *pContext, uint16_t backOffBase, uint16_t maxBackOff, uint32_t maxAttempts)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>backoff_algorithm.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/backoffAlgorithm/source/include/</path>
    <filename>backoff__algorithm_8h.html</filename>
    <class kind="struct">BackoffAlgorithmContext_t</class>
    <member kind="define">
      <type>#define</type>
      <name>BACKOFF_ALGORITHM_RETRY_FOREVER</name>
      <anchorfile>group__backoff__algorithm__constants.html</anchorfile>
      <anchor>ga89e511b4edee8a92050802a329753842</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>BackoffAlgorithmStatus_t</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>ga7b7f4051eb253599ffedd69285f5a89b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BackoffAlgorithmSuccess</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>gga7b7f4051eb253599ffedd69285f5a89baecf422ba6b1201d8ffbfd61e5712d695</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BackoffAlgorithmRetriesExhausted</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>gga7b7f4051eb253599ffedd69285f5a89ba4dd72a4822e0f93b5b98309453b774df</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>BackoffAlgorithm_InitializeParams</name>
      <anchorfile>backoff__algorithm_8h.html</anchorfile>
      <anchor>ac37ecab230cf5da8f70547ec63776a48</anchor>
      <arglist>(BackoffAlgorithmContext_t *pContext, uint16_t backOffBase, uint16_t maxBackOff, uint32_t maxAttempts)</arglist>
    </member>
    <member kind="function">
      <type>BackoffAlgorithmStatus_t</type>
      <name>BackoffAlgorithm_GetNextBackoff</name>
      <anchorfile>backoff__algorithm_8h.html</anchorfile>
      <anchor>ae9dc843e520435569f40e8832791ecbe</anchor>
      <arglist>(BackoffAlgorithmContext_t *pRetryContext, uint32_t randomValue, uint16_t *pNextBackOff)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BackoffAlgorithmContext_t</name>
    <filename>struct_backoff_algorithm_context__t.html</filename>
    <member kind="variable">
      <type>uint16_t</type>
      <name>maxBackoffDelay</name>
      <anchorfile>struct_backoff_algorithm_context__t.html</anchorfile>
      <anchor>aa8a4f1532288640dbdc45719bd758e21</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>attemptsDone</name>
      <anchorfile>struct_backoff_algorithm_context__t.html</anchorfile>
      <anchor>af205fc8af2271d4e709499c22d2f9b3f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>nextJitterMax</name>
      <anchorfile>struct_backoff_algorithm_context__t.html</anchorfile>
      <anchor>ac84bf83b50d395dae1e389e0fccf1094</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>maxRetryAttempts</name>
      <anchorfile>struct_backoff_algorithm_context__t.html</anchorfile>
      <anchor>a842d21d7cb802443c0cc18dfe12490f9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>backoff_algorithm_struct_types</name>
    <title>Parameter Structure</title>
    <filename>group__backoff__algorithm__struct__types.html</filename>
    <class kind="struct">BackoffAlgorithmContext_t</class>
  </compound>
  <compound kind="group">
    <name>backoff_algorithm_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__backoff__algorithm__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>BackoffAlgorithmStatus_t</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>ga7b7f4051eb253599ffedd69285f5a89b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BackoffAlgorithmSuccess</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>gga7b7f4051eb253599ffedd69285f5a89baecf422ba6b1201d8ffbfd61e5712d695</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BackoffAlgorithmRetriesExhausted</name>
      <anchorfile>group__backoff__algorithm__enum__types.html</anchorfile>
      <anchor>gga7b7f4051eb253599ffedd69285f5a89ba4dd72a4822e0f93b5b98309453b774df</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>backoff_algorithm_constants</name>
    <title>Constants</title>
    <filename>group__backoff__algorithm__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>BACKOFF_ALGORITHM_RETRY_FOREVER</name>
      <anchorfile>group__backoff__algorithm__constants.html</anchorfile>
      <anchor>ga89e511b4edee8a92050802a329753842</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>backoff_algorithm_functions</name>
    <title>Functions</title>
    <filename>backoff_algorithm_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>define_backoffalgorithm_initializeparams</name>
    <title>BackoffAlgorithm_InitializeParams</title>
    <filename>define_backoffalgorithm_initializeparams.html</filename>
  </compound>
  <compound kind="page">
    <name>define_backoffalgorithm_getnextbackoff</name>
    <title>BackoffAlgorithm_GetNextBackoff</title>
    <filename>define_backoffalgorithm_getnextbackoff.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">BackOff</docanchor>
    <docanchor file="index.html" title="Memory Requirements">backoff_algorithm_memory_requirements</docanchor>
    <docanchor file="index.html" title="Design">backoff_algorithm_design</docanchor>
  </compound>
</tagfile>
