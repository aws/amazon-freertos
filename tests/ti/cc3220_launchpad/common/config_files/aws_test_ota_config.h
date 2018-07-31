/*
 * Amazon FreeRTOS V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_test_ota_config.h
 * @brief Port-specific variables for firmware Over-the-Air Update tests. */

#ifndef _AWS_TEST_OTA_CONFIG_H_
#define _AWS_TEST_OTA_CONFIG_H_

/**
 * @brief Job document, with all required fields, that is expected to parse.
 */
#define otatestLASER_JSON "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"ts_ota\": {\"files\": [{\"streamname\": \"1\",\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"sig-sha1-rsa\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

/**
 * @brief Job document that is missing a required field.
 */
#define otatestBAD_LASER_JSON "{\"clientToken\":\"mytoken\",\"timestamp\":1508445004,\"execution\":{\"jobId\":\"15\",\"status\":\"QUEUED\",\"queuedAt\":1507697924,\"lastUpdatedAt\":1507697924,\"versionNumber\":1,\"executionNumber\":1,\"jobDocument\":{\"ts_ota\": {\"files\": [{\"streamname\": \"1\",\"filepath\": \"payload.bin\",\"version\":\"1.0.0.0\",\"filesize\": 90860,\"fileid\": 0,\"attr\": 3,\"certfile\":\"rsasigner.crt\", \"sig-sha256-rsa\":\"OHj5sNjxqMNK3WNEwbyfs/PeSSS1kzLkAQ4MSu0yKNFoGxJrUKuIWhjQbQiPlXcDtXlSXE8ydAwoxnnw5lcwpJsbXxD1K1PwZJoc/3mv5XHXbvvEoFr4yA0rhY4tyrMDBesEtOVrW0yI4mM4Lde5OtdIxo8sjTSPGXo2Ejuhn+LDRD3gKdb1gtPpoJ/YBQmYKXHFQ5QW58GOSlB9prq5v+MloVCATjmzb9tu4msScXYYy41ikEhK2eyfl7/vpc2vMNX6uhyyeZhku9namI4OZmsp72tLL4D4pFt4/nDWYSAo8sQAwns1RNY+j52KfvgvKKN3u6G3suFyVQoxWJu3aA==\"}]}}}}"

#endif 
