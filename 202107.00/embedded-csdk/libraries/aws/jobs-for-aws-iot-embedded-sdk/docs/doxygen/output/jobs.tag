<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>jobs.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source/</path>
    <filename>jobs_8c.html</filename>
    <includes id="jobs_8h" name="jobs.h" local="yes" imported="no">jobs.h</includes>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_GetTopic</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>afe747bcbbd6eec4715b70df9f376de0a</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, JobsTopic_t api, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_MatchTopic</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>ae3e1205f419d335dcf9b338f0db19815</anchor>
      <arglist>(char *topic, size_t length, const char *thingName, uint16_t thingNameLength, JobsTopic_t *outApi, char **outJobId, uint16_t *outJobIdLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_GetPending</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>a5b4fd584f0400e51b1d29189b24c91a5</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_StartNext</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>ad69cddd288dd61b3716021478725c174</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_Describe</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>adfdc2db49fa5102fdb0e0b826e86b150</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, const char *jobId, uint16_t jobIdLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_Update</name>
      <anchorfile>jobs_8c.html</anchorfile>
      <anchor>a4ab3fd29173814ab73d764d123e9f400</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, const char *jobId, uint16_t jobIdLength, size_t *outLength)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>jobs.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/aws/jobs-for-aws-iot-embedded-sdk/source/include/</path>
    <filename>jobs_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_THINGNAME_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga2b2ec2a330942e5eb6fca9a597dcf8c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_JOBID_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga1725f6f0c1997682b3100e7d23d138a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>THINGNAME_MAX_LENGTH</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>ad71fccb339adc1986401cb60fec0bc2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBID_MAX_LENGTH</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>a30bd49398cb42116098ed8b2e4afec8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_SUBSCRIBE_NEXTJOBCHANGED</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga3837d5c45a0628b3d771b2d55bba416b</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_SUBSCRIBE_JOBSCHANGED</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga1fc910b7128664da161acbc886729d46</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_STARTNEXT</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>gaa3c483208b1e45fe6af4b4da1801ee13</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_GETPENDING</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga9b70e9e9b21ae85d329d9a2470739ec7</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_DESCRIBENEXTJOB</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga106b0c4b9a3a805c0b26834c3c0b8bfa</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>gabb375381728f738d2bf931b7dbd91103</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JobsStatus_t</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>gacc72f7076cee5bd66a46453420d97921</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsSuccess</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a23d9fa1f6d5d5e51f681e3682d9235f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsNoMatch</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a01346b489cb3839b59cd0e81d98f2851</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsBadParameter</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a9b6dea4829e16d4d5e43c99f00286e0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsBufferTooSmall</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a12ff29ab79c3596a4a54cfa1a541cc79</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JobsTopic_t</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ga315af209e86f50615c9e76948cf268ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_GetTopic</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>afe747bcbbd6eec4715b70df9f376de0a</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, JobsTopic_t api, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_MatchTopic</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>ae3e1205f419d335dcf9b338f0db19815</anchor>
      <arglist>(char *topic, size_t length, const char *thingName, uint16_t thingNameLength, JobsTopic_t *outApi, char **outJobId, uint16_t *outJobIdLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_GetPending</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>a5b4fd584f0400e51b1d29189b24c91a5</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_StartNext</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>ad69cddd288dd61b3716021478725c174</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_Describe</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>adfdc2db49fa5102fdb0e0b826e86b150</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, const char *jobId, uint16_t jobIdLength, size_t *outLength)</arglist>
    </member>
    <member kind="function">
      <type>JobsStatus_t</type>
      <name>Jobs_Update</name>
      <anchorfile>jobs_8h.html</anchorfile>
      <anchor>a4ab3fd29173814ab73d764d123e9f400</anchor>
      <arglist>(char *buffer, size_t length, const char *thingName, uint16_t thingNameLength, const char *jobId, uint16_t jobIdLength, size_t *outLength)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>jobs_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__jobs__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>JobsStatus_t</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>gacc72f7076cee5bd66a46453420d97921</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsSuccess</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a23d9fa1f6d5d5e51f681e3682d9235f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsNoMatch</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a01346b489cb3839b59cd0e81d98f2851</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsBadParameter</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a9b6dea4829e16d4d5e43c99f00286e0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>JobsBufferTooSmall</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ggacc72f7076cee5bd66a46453420d97921a12ff29ab79c3596a4a54cfa1a541cc79</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>JobsTopic_t</name>
      <anchorfile>group__jobs__enum__types.html</anchorfile>
      <anchor>ga315af209e86f50615c9e76948cf268ac</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>jobs_constants</name>
    <title>Constants</title>
    <filename>group__jobs__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_THINGNAME_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga2b2ec2a330942e5eb6fca9a597dcf8c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_JOBID_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga1725f6f0c1997682b3100e7d23d138a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_SUBSCRIBE_NEXTJOBCHANGED</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga3837d5c45a0628b3d771b2d55bba416b</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_SUBSCRIBE_JOBSCHANGED</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga1fc910b7128664da161acbc886729d46</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_STARTNEXT</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>gaa3c483208b1e45fe6af4b4da1801ee13</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_GETPENDING</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga9b70e9e9b21ae85d329d9a2470739ec7</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_PUBLISH_DESCRIBENEXTJOB</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>ga106b0c4b9a3a805c0b26834c3c0b8bfa</anchor>
      <arglist>(thingName)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>JOBS_API_MAX_LENGTH</name>
      <anchorfile>group__jobs__constants.html</anchorfile>
      <anchor>gabb375381728f738d2bf931b7dbd91103</anchor>
      <arglist>(thingNameLength)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>jobs_features</name>
    <title>Features</title>
    <filename>jobs_features.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_config</name>
    <title>Configurations</title>
    <filename>jobs_config.html</filename>
    <docanchor file="jobs_config.html">THINGNAME_MAX_LENGTH</docanchor>
    <docanchor file="jobs_config.html">JOBID_MAX_LENGTH</docanchor>
  </compound>
  <compound kind="page">
    <name>jobs_functions</name>
    <title>Functions</title>
    <filename>jobs_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_gettopic_function</name>
    <title>Jobs_GetTopic</title>
    <filename>jobs_gettopic_function.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_matchtopic_function</name>
    <title>Jobs_MatchTopic</title>
    <filename>jobs_matchtopic_function.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_getpending_function</name>
    <title>Jobs_GetPending</title>
    <filename>jobs_getpending_function.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_startnext_function</name>
    <title>Jobs_StartNext</title>
    <filename>jobs_startnext_function.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_describe_function</name>
    <title>Jobs_Describe</title>
    <filename>jobs_describe_function.html</filename>
  </compound>
  <compound kind="page">
    <name>jobs_update_function</name>
    <title>Jobs_Update</title>
    <filename>jobs_update_function.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">jobs</docanchor>
    <docanchor file="index.html" title="Memory Requirements">jobs_memory_requirements</docanchor>
  </compound>
</tagfile>
