var group__ota__struct__types =
[
    [ "OtaJobDocument_t", "struct_ota_job_document__t.html", [
      [ "pJobDocJson", "struct_ota_job_document__t.html#a76f27be43afe47387d01b8a25fdb2555", null ],
      [ "jobDocLength", "struct_ota_job_document__t.html#afdd32b88f641c9d5eac48115ecdade2b", null ],
      [ "pJobId", "struct_ota_job_document__t.html#ad5531b7d3f6c2e7f3560c7db13ad2951", null ],
      [ "jobIdLength", "struct_ota_job_document__t.html#a3c4394a5170ade0003b2abcdfef78ac3", null ],
      [ "fileTypeId", "struct_ota_job_document__t.html#a684a4b443b542fc72c65318a2ed86b92", null ],
      [ "parseErr", "struct_ota_job_document__t.html#afbc69ad19342769a7a24c4d8d03a935a", null ],
      [ "status", "struct_ota_job_document__t.html#a82ca1e15150a3c9a8472845b48c7b525", null ],
      [ "reason", "struct_ota_job_document__t.html#a5d8e1a7e8f8c32efebeaba6a119c98bc", null ],
      [ "subReason", "struct_ota_job_document__t.html#ad149a314fb8e362f97d6d7f9e876cbcb", null ]
    ] ],
    [ "OtaInterfaces_t", "struct_ota_interfaces__t.html", [
      [ "os", "struct_ota_interfaces__t.html#a4309938e333d120eb646b1d24b5f413a", null ],
      [ "mqtt", "struct_ota_interfaces__t.html#a231d847d025ae94578f68d5ae8ff3e66", null ],
      [ "http", "struct_ota_interfaces__t.html#a76df425905b6e53f43148e03a4c1daee", null ],
      [ "pal", "struct_ota_interfaces__t.html#a0c434ef971121cc7c64970cdb8606e2f", null ]
    ] ],
    [ "OtaAppBuffer_t", "struct_ota_app_buffer__t.html", [
      [ "pUpdateFilePath", "struct_ota_app_buffer__t.html#a1e8e0cf1e6b1f699409f22a4c03cefd7", null ],
      [ "updateFilePathsize", "struct_ota_app_buffer__t.html#a4260f4d420eeb703cf3af5dc85dcd70d", null ],
      [ "pCertFilePath", "struct_ota_app_buffer__t.html#a64d0ac6d3c7d66193582640fd2d023d7", null ],
      [ "certFilePathSize", "struct_ota_app_buffer__t.html#a5f08814fbd5e55f88e23677a4da8b732", null ],
      [ "pStreamName", "struct_ota_app_buffer__t.html#a7bc6e9af2e5935ddf7168173bba51805", null ],
      [ "streamNameSize", "struct_ota_app_buffer__t.html#ad27ecad9649243f52d2067ec4c9f9849", null ],
      [ "pDecodeMemory", "struct_ota_app_buffer__t.html#aa9456f212031228ff3a0a04307430802", null ],
      [ "decodeMemorySize", "struct_ota_app_buffer__t.html#a4cf8c691d69b5f951bb902d780fca1a8", null ],
      [ "pFileBitmap", "struct_ota_app_buffer__t.html#a6d2a5ecba3800a44bf58223ea6785ddd", null ],
      [ "fileBitmapSize", "struct_ota_app_buffer__t.html#ac9d07e3e4d6978bf9dee34eb3a330cbc", null ],
      [ "pUrl", "struct_ota_app_buffer__t.html#a493c9ece89e377df53c2a7e2089704ec", null ],
      [ "urlSize", "struct_ota_app_buffer__t.html#a2b6bd46660f5f45859d36cfa330c8c90", null ],
      [ "pAuthScheme", "struct_ota_app_buffer__t.html#a45d7da0e73376d5b2bf019f9c3eb3196", null ],
      [ "authSchemeSize", "struct_ota_app_buffer__t.html#a0c474b181da044b9a06ee5da9d1b714d", null ]
    ] ],
    [ "AppVersion32_t", "struct_app_version32__t.html", [
      [ "build", "struct_app_version32__t.html#a9e4251725bdc4abe2904e72bd10dbdc7", null ],
      [ "minor", "struct_app_version32__t.html#a37fbe1709903b16f29aa79c0cbaa3380", null ],
      [ "major", "struct_app_version32__t.html#abc623f9deacb56ff0a7dc1bbfa2d300a", null ],
      [ "x", "struct_app_version32__t.html#ad916509bde38999fa0589b4460d66849", null ],
      [ "u", "struct_app_version32__t.html#a6bfe56ccca1a15c43140129761a4e22d", null ]
    ] ],
    [ "OtaHttpInterface_t", "struct_ota_http_interface__t.html", [
      [ "init", "struct_ota_http_interface__t.html#a9e6e01808eb7dcd83ea3562aeda6ce67", null ],
      [ "request", "struct_ota_http_interface__t.html#a87e4e6ebc94538da49d7cfe24cefab1c", null ],
      [ "deinit", "struct_ota_http_interface__t.html#a71b991f269a830332759d07341dc540f", null ]
    ] ],
    [ "OtaMqttInterface_t", "struct_ota_mqtt_interface__t.html", [
      [ "subscribe", "struct_ota_mqtt_interface__t.html#adf1c4fbd39dc489e5f446a163ed57d65", null ],
      [ "unsubscribe", "struct_ota_mqtt_interface__t.html#afe7b58ce23961bd3023266e9abb26a85", null ],
      [ "publish", "struct_ota_mqtt_interface__t.html#aab31f752ef9b005cb5f06226fdce7540", null ]
    ] ],
    [ "OtaEventInterface_t", "struct_ota_event_interface__t.html", [
      [ "init", "struct_ota_event_interface__t.html#a70a33a62828b128cd625c1b13ff8550e", null ],
      [ "send", "struct_ota_event_interface__t.html#a02285842a33d7f21990422fdeabc39eb", null ],
      [ "recv", "struct_ota_event_interface__t.html#acd46139762310587bb44d8b23b58c8fe", null ],
      [ "deinit", "struct_ota_event_interface__t.html#a0bc56e4a2b5c991272239045e75363d7", null ],
      [ "pEventContext", "struct_ota_event_interface__t.html#a6f6184ea615ef2e26093adcb136f8176", null ]
    ] ],
    [ "OtaTimerInterface_t", "struct_ota_timer_interface__t.html", [
      [ "start", "struct_ota_timer_interface__t.html#a6667d075f8cc8314a21ba87733d52d0e", null ],
      [ "stop", "struct_ota_timer_interface__t.html#a6c58cf74616058c88030f06499913833", null ],
      [ "delete", "struct_ota_timer_interface__t.html#a2963e14c0ed499078b71b8b3b211a58d", null ]
    ] ],
    [ "OtaMallocInterface_t", "struct_ota_malloc_interface__t.html", [
      [ "malloc", "struct_ota_malloc_interface__t.html#a80caab4ab7b1486faaec6dc30b50a31f", null ],
      [ "free", "struct_ota_malloc_interface__t.html#a3de64b7780a2c70f182d4a64911dc476", null ]
    ] ],
    [ "OtaOSInterface_t", "struct_ota_o_s_interface__t.html", [
      [ "event", "struct_ota_o_s_interface__t.html#a56b4e42ad807bb4f407f61cca2964016", null ],
      [ "timer", "struct_ota_o_s_interface__t.html#a7ed5a92311ab933578d208b808780024", null ],
      [ "mem", "struct_ota_o_s_interface__t.html#a383da1c5ad16c5d6668b4feeb9fc347f", null ]
    ] ],
    [ "OtaPalInterface_t", "struct_ota_pal_interface__t.html", [
      [ "abort", "struct_ota_pal_interface__t.html#abf9e21e27883ba652e88551cac464f49", null ],
      [ "createFile", "struct_ota_pal_interface__t.html#ad46b44c6ea0e60206d7d2766bf340f91", null ],
      [ "closeFile", "struct_ota_pal_interface__t.html#aed51ccb56c34bcb8d7981b50e1230eef", null ],
      [ "writeBlock", "struct_ota_pal_interface__t.html#a6661bd9229a8191a3f758b77544f6d24", null ],
      [ "activate", "struct_ota_pal_interface__t.html#ae13ed8c9101ce7a69210b1cc0d32befe", null ],
      [ "reset", "struct_ota_pal_interface__t.html#a744b9602243baf92cbe2d1e8aef3b7ec", null ],
      [ "setPlatformImageState", "struct_ota_pal_interface__t.html#a4042675d1df2bcc0fca26a9121fe5668", null ],
      [ "getPlatformImageState", "struct_ota_pal_interface__t.html#ace5e6d57a59896ca51b41c685989dd13", null ]
    ] ],
    [ "Sig256_t", "struct_sig256__t.html", [
      [ "size", "struct_sig256__t.html#a03d5d9cc35524d46b65eeb6287e75ea3", null ],
      [ "data", "struct_sig256__t.html#aae366e61733f67f99979cdd998fc8e01", null ]
    ] ],
    [ "OtaFileContext_t", "struct_ota_file_context__t.html", [
      [ "pFilePath", "struct_ota_file_context__t.html#a7281da77a81626910373a56efcf90262", null ],
      [ "filePathMaxSize", "struct_ota_file_context__t.html#a011bd760168a0265d0912bfb9ae5b35a", null ],
      [ "pFile", "struct_ota_file_context__t.html#a731eefc516423588396a17c63cc159b9", null ],
      [ "fileSize", "struct_ota_file_context__t.html#a26d56a7c06c041a73e569ca53e71ac67", null ],
      [ "blocksRemaining", "struct_ota_file_context__t.html#a23ad6a417000fca03982f5dd67bae94f", null ],
      [ "fileAttributes", "struct_ota_file_context__t.html#a1eb0744188e34521d45faf9f740043e1", null ],
      [ "serverFileID", "struct_ota_file_context__t.html#ab972bcc57370ba912390035067c39a7c", null ],
      [ "pJobName", "struct_ota_file_context__t.html#af9fcbcb6f6948b6d72498457fd08c88d", null ],
      [ "jobNameMaxSize", "struct_ota_file_context__t.html#a14886bb26167aad01e7496fdb9473bde", null ],
      [ "pStreamName", "struct_ota_file_context__t.html#a5bfc26add016d1883c2373a63c2bf5b0", null ],
      [ "streamNameMaxSize", "struct_ota_file_context__t.html#acafd3a6210232929ca52beec8cef5e08", null ],
      [ "pRxBlockBitmap", "struct_ota_file_context__t.html#a1876125c2893ff2639fdcebfe47aa073", null ],
      [ "blockBitmapMaxSize", "struct_ota_file_context__t.html#a49d365eb241b8f22e341f9cf9c88f121", null ],
      [ "pCertFilepath", "struct_ota_file_context__t.html#a7dafcbda219cc62527b3f697192f1d0c", null ],
      [ "certFilePathMaxSize", "struct_ota_file_context__t.html#ad973ad2e69778a929d962c7b773006a3", null ],
      [ "pUpdateUrlPath", "struct_ota_file_context__t.html#a66b88f4b4d9ce8f7d40c5d306bc588c8", null ],
      [ "updateUrlMaxSize", "struct_ota_file_context__t.html#a7dfc9c9989cbe538b78bc007748ecccb", null ],
      [ "pAuthScheme", "struct_ota_file_context__t.html#ab314724b6cfbda0dd95899803ee0d514", null ],
      [ "authSchemeMaxSize", "struct_ota_file_context__t.html#a67c5ddb9341a368e755b6ca386ea5d6f", null ],
      [ "updaterVersion", "struct_ota_file_context__t.html#a732b6021dc1856d54fea38588f65957d", null ],
      [ "isInSelfTest", "struct_ota_file_context__t.html#a4bed61a2b0553f841bbaca9abd0a1560", null ],
      [ "pProtocols", "struct_ota_file_context__t.html#af1c7d4797e2ae02f8f022bcaad229cb3", null ],
      [ "protocolMaxSize", "struct_ota_file_context__t.html#ae0306a16a35e855c2cd4d60fcf940813", null ],
      [ "pDecodeMem", "struct_ota_file_context__t.html#acd7020697cf5a208cac8da603af33d1f", null ],
      [ "decodeMemMaxSize", "struct_ota_file_context__t.html#a14058c044993f0fe95a406b072204495", null ],
      [ "fileType", "struct_ota_file_context__t.html#a3b880b7e8fcb4fee11607ac8bd454915", null ],
      [ "pSignature", "struct_ota_file_context__t.html#a7b19507fb15f4924d985ad0a48c671f0", null ]
    ] ]
];